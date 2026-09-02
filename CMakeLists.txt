# This usage of CMake requires at least version 3.20
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Set the project as C++ primarily with C for the vendored libraries
project(Anope CXX C)

# Force the locale to C for later uses of things like gcc so the messages come up in English, not the user's default language
set(ENV{LC_ALL} C)

# We require C++17 to build
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put modules in their own folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_BUILD_TYPE STREQUAL "")
  message(STATUS "CMAKE_BUILD_TYPE has not been specified; defaulting to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Include the checking functions used later in this CMakeLists.txt
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckLibraryExists)
include(CheckCXXCompilerFlag)
include(CheckPIESupported)

if(NOT WIN32)
  include(FindPkgConfig)
endif()

# If extra include directories were specified, tell cmake about them.
if(EXTRA_INCLUDE)
  include_directories(${EXTRA_INCLUDE})
endif()

# If extra library directories were specified, tell cmake about them.
if(EXTRA_LIBS)
  link_directories(${EXTRA_LIBS})
endif()

# Enable -fPIC for all targets.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
check_pie_supported()

# Find gettext
find_package(Gettext)
find_package(Intl)
if(Gettext_FOUND AND Intl_FOUND)
  set(HAVE_LOCALIZATION ON)
  include_directories(${Intl_INCLUDE_DIRS})
  link_libraries(${Intl_LIBRARIES})
else()
  message(STATUS "Unable to find gettext and/or libintl -- read docs/LANGUAGE for how to enable localization")
  set(HAVE_LOCALIZATION OFF)
endif()

# Use the following directories as includes
include_directories(
  ${Anope_BINARY_DIR}/include
  ${Anope_SOURCE_DIR}/include
  ${Anope_SOURCE_DIR}/vendor
)

if(WIN32)
  # If using Windows, include the windows specific folder for anope_windows.h
  include_directories(${Anope_SOURCE_DIR}/src/win32)
endif()

# If using Visual Studio, set the C++ flags accordingly
if(MSVC)
  # Set the compile flags to have warnings on the max setting (but disable a few annoying ones), exception handling turned on
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4100 /wd4127 /wd4250 /wd4251 /wd4267 /wd4275 /wd4355 /wd4706 /wd4800 /wd4996 /EHs")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
# Otherwise, we're not using Visual Studio
else()
  # Set the compile flags to have all warnings on (including shadowed variables)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Wextra -Wformat=2 -Wmissing-format-attribute -Wpedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-nonliteral -Wno-format-y2k -Wno-format-zero-length -Wno-date-time -Wno-unused-parameter")
endif()

# If CMake has found that the given system requires a special library for dl* calls, include it with the linker flags
if(CMAKE_DL_LIBS)
  link_libraries(${CMAKE_DL_LIBS})
endif()

# Find the linker flags required for using threads.
find_package("Threads" REQUIRED)
if(CMAKE_THREAD_LIBS_INIT)
  link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

# On Windows, also link Anope to the memory library and Winsock.
if(WIN32)
  link_libraries("Ws2_32")
  if(MSVC)
    link_libraries("win32_memory")
  endif()
endif()

if(NOT PROGRAM_NAME)
  set(PROGRAM_NAME anope)
endif()

# If we are not using Visual Studio, we'll run the following checks
if(NOT MSVC)
  # Check if the C++ compiler can accept the -pipe flag, and add it to the compile flags if it works
  check_cxx_compiler_flag(-pipe HAVE_PIPE_FLAG)
  # If the flag was accepted, add it to the list of flags
  if(HAVE_PIPE_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
  endif()
endif()

# If DEFUMASK wasn't passed to CMake, set a default depending on if RUNGROUP was passed in or not
if(NOT DEFUMASK)
  if(RUNGROUP)
    set(DEFUMASK "007")
  else()
    set(DEFUMASK "077")
  endif()
endif()

# Set the DEBUG_BUILD for sysconf.h
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(DEBUG_BUILD TRUE)
endif()

# Check for the existence of the following functions
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
check_function_exists(umask HAVE_UMASK)
check_function_exists(epoll_wait HAVE_EPOLL)
check_function_exists(poll HAVE_POLL)
check_function_exists(kqueue HAVE_KQUEUE)

# Search for the following programs
if(NOT WIN32 AND RUNGROUP)
  find_program(CHGRP "chgrp" REQUIRED)
  find_program(CHMOD "chmod" REQUIRED)
endif()

# If a INSTDIR was passed in to CMake, use it as the install prefix, otherwise set the default install prefix to the anope directory under the user's home directory
if(INSTDIR)
  set(CMAKE_INSTALL_PREFIX "${INSTDIR}")
elseif(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/anope-${VERSION_MAJOR}-${VERSION_MINOR}")
endif()

function(configure_path VARIABLE DEFAULT)
  if(NOT DEFINED ${VARIABLE})
      set(${VARIABLE} ${DEFAULT} CACHE PATH "Location of the ${DEFAULT} directory")
  endif()

  if(WIN32)
    set(ABSOLUTE_${VARIABLE} ${${VARIABLE}})
  else()
    cmake_path(
      APPEND CMAKE_INSTALL_PREFIX "${${VARIABLE}}"
      OUTPUT_VARIABLE ABSOLUTE_${VARIABLE}
    )
    cmake_path(NORMAL_PATH ABSOLUTE_${VARIABLE})
  endif()
  set(ABSOLUTE_${VARIABLE} ${ABSOLUTE_${VARIABLE}} CACHE INTERNAL "" FORCE)

  install(
    DIRECTORY
    DESTINATION ${${VARIABLE}}
  )
endfunction()


# Set default paths for various directories if not already defined
configure_path(BIN_DIR "bin")
configure_path(DATA_DIR "data")
configure_path(DOC_DIR "doc")
configure_path(CONF_DIR "conf")
configure_path(MODULE_DIR "modules")
configure_path(LOCALE_DIR "locale")
configure_path(LOG_DIR "logs")

# Version number processing
# Find all lines in src/version.sh that start with VERSION_
file(STRINGS ${Anope_SOURCE_DIR}/src/version.sh VERSIONS REGEX "^VERSION_")
# Iterate through the strings found
foreach(VERSION_STR ${VERSIONS})
  string(REGEX REPLACE "^VERSION_([A-Z]+)=\"?([^\"]*)\"?$" "\\1;\\2" VERSION_OUT ${VERSION_STR})
  list(LENGTH VERSION_OUT VERSION_LEN)
  list(GET VERSION_OUT 0 VERSION_TYPE)
  if(${VERSION_LEN} GREATER 1)
    list(GET VERSION_OUT 1 VERSION_DATA)
    set(VERSION_${VERSION_TYPE} ${VERSION_DATA})
  endif()
endforeach()

# Default build version to 0
set(VERSION_BUILD 0)

# Only change the build number if version.h exists
if(EXISTS "${Anope_SOURCE_DIR}/include/version.h")
  # Attempt to read the build number from include/version.h
  file(STRINGS ${Anope_SOURCE_DIR}/src/version.sh VERSIONS REGEX "^#define VERSION_BUILD")
  foreach(VERSION_STR ${VERSIONS})
    # Get the length of the string
    string(LENGTH ${VERSION_STR} VERSION_LEN)
    # Subtract 22 from the string's length
    math(EXPR VERSION_NUM_LEN "${VERSION_LEN} - 22")
    # Extract the value from the string
    string(SUBSTRING ${VERSION_STR} 22 ${VERSION_NUM_LEN} VERSION)
    # Set VERSION_BUILD correctly
    set(VERSION_BUILD ${VERSION})
  endforeach()
endif()

# Set the version variables based on what was found above
set(VERSION_COMMA "${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH},${VERSION_BUILD}")
set(VERSION_DOTTED_NOBUILD "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(VERSION_DOTTED "${VERSION_DOTTED_NOBUILD}.${VERSION_BUILD}")
set(VERSION_FULL "${VERSION_DOTTED}${VERSION_EXTRA}")
set(VERSION_FULL_NOBUILD "${VERSION_DOTTED_NOBUILD}${VERSION_EXTRA}")

# Only do the following for Windows
if(WIN32)
  # Generate the win32.rc file using the above variables
  configure_file(${Anope_SOURCE_DIR}/src/win32/win32.rc.cmake ${Anope_BINARY_DIR}/src/win32/win32.rc)
endif()

# At install time, create the following additional directories
install(DIRECTORY DESTINATION "${DATA_DIR}/backups")
if(WIN32)
  install(DIRECTORY DESTINATION "${DATA_DIR}/runtime")
endif()
# On non-Windows platforms, if RUNGROUP is set, change the permissions of the below directories, as well as the group of the data directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\$ENV{DESTDIR}${ABSOLUTE_DATA_DIR}/backups\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\$ENV{DESTDIR}${ABSOLUTE_LOG_DIR}\")")
  install(CODE "execute_process(COMMAND ${CHGRP} -R ${RUNGROUP} \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\")")
endif()
# On Windows platforms, install extra files
if(WIN32)
  install(FILES ${Anope_SOURCE_DIR}/src/win32/anope.bat
    DESTINATION ${BIN_DIR}
  )
endif()

install(CODE "file(REMOVE_RECURSE \"$ENV{DESTDIR}${ABSOLUTE_MODULE_DIR}\")")

# Only process the CPack section if we have CPack
if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  # Various options for CPack
  set(CPACK_PACKAGE_NAME "Anope IRC Services")
  set(CPACK_PACKAGE_VENDOR "Anope Team")
  set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}${VERSION_EXTRA}")
  set(CPACK_PACKAGE_FILE_NAME "anope-${VERSION_FULL_NOBUILD}")
  set(CPACK_RESOURCE_FILE_LICENSE "${Anope_SOURCE_DIR}/docs/LICENSE.txt")
  # The following doesn't actually do anything. :(
  #set(CPACK_RESOURCE_FILE_README "${Anope_SOURCE_DIR}/docs/README")
  # The following is primarily for NSIS
  if(WIN32)
    # Get the filename of the Anope binary, to use later
    set(SERVICES_BINARY "${PROGRAM_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
    # By default, do not warn when built on machines using only VS Express:
    IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
      SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
    ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    # Also for Windows, include installing the MSVCRT library
    include(InstallRequiredSystemLibraries)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Anope")
    set(CPACK_PACKAGE_EXECUTABLES "")
    set(CPACK_NSIS_MENU_LINKS
      "bin\\\\${SERVICES_BINARY}" "Anope IRC Services"
      "bin\\\\anope.bat\\\" \\\"-debug -nofork" "Anope IRC Services (Debug and Window Logging)"
      "bin\\\\anope.bat\\\" \\\"-nofork" "Anope IRC Services (Window Logging)"
      "bin\\\\anope.bat\\\" \\\"-nothird" "Anope IRC Services (No Third Party Modules)"
      "https://www.anope.org/" "Anope Web Site"
    )
    # The following doesn't work, but a bug report has been filed about it
    #set(CPACK_CREATE_DESKTOP_LINK_${SERVICES_BINARY} TRUE)
    set(CPACK_NSIS_MUI_ICON "${Anope_SOURCE_DIR}/src\\\\win32\\\\anope-icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${Anope_SOURCE_DIR}/src\\\\win32\\\\anope-icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${SERVICES_BINARY}")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://www.anope.org/")
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  endif()
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "anope-${VERSION_FULL_NOBUILD}-source")
  set(CPACK_SOURCE_GENERATOR "TGZ")
  set(CPACK_MONOLITHIC_INSTALL TRUE)
  include(CPack)
endif()

# Go into the following directories and run their CMakeLists.txt as well
add_subdirectory(data)
add_subdirectory(docs)
add_subdirectory(language)
add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(modules)
add_subdirectory(include)
