# This usage of CMake requires at least version 3.0
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

cmake_policy(SET CMP0026 OLD)

# Set the project as C++ primarily, but have C enabled for the checks required later
project(Anope CXX)
enable_language(C)

option(PREFER_EMBEDDED_SQLITE OFF)

# Override the module include path to include our directory, for our Anope.cmake, as well as we are using our own version of the NSIS template
list(APPEND CMAKE_MODULE_PATH "${Anope_SOURCE_DIR}/cmake")

include("Anope")
include("CheckFunctionExists")
include("CheckLibraryExists")
include("CheckCXXCompilerFlag")

# Force the locale to C for later uses of things like gcc so the messages come up in English, not the user's default language
set(ENV{LC_ALL} C)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug
# Only do this if not using Visual Studio
if(NOT MSVC)
  if(CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
  else()
    set(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
  endif()
 
  check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a newer compiler.")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# If extra include directories were specified, tell cmake about them.
if(EXTRA_INCLUDE)
  include_directories(${EXTRA_INCLUDE})
endif()

# If extra library directories were specified, tell cmake about them.
if(EXTRA_LIBS)
  link_directories(${EXTRA_LIBS})
endif()

# Find gettext
find_package(Gettext)
# FindBoost.cmake warns even if you specify QUIET, so
find_path(BOOST_LOCALE_HEADER_FILE NAMES boost/locale.hpp)
if(BOOST_LOCALE_HEADER_FILE)
  find_package(Boost COMPONENTS locale)
endif()
find_package(Sqlite)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# Use the following directories as includes
include_directories(${Anope_BINARY_DIR}/include ${Anope_SOURCE_DIR}/include)

# Pass on REPRODUCIBLE_BUILD
if(REPRODUCIBLE_BUILD)
  add_definitions(-DREPRODUCIBLE_BUILD)
endif()

# If using Windows, always add the _WIN32 define
if(WIN32)
  add_definitions(-D_WIN32)
  # And include the windows specific folder for our anope_windows.h
  include_directories(${Anope_SOURCE_DIR}/src/win32)
endif()

# If using Visual Studio, set the C++ flags accordingly
if(MSVC)
  # Remove the default exception handling flags, also remove default warning level flag
  string(REPLACE "/EHsc " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/GX " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/W3 " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # Set the compile flags to have warnings on the max setting (but disable a few annoying ones), exception handling turned on, the proper defines
  set(CXXFLAGS "${CXXFLAGS} /W4 /wd4100 /wd4127 /wd4250 /wd4251 /wd4355 /wd4706 /wd4800 /wd4996 /EHs")
  add_definitions(-DMSVCPP -D_CRT_SECURE_NO_WARNINGS)
# Otherwise, we're not using Visual Studio
else()
  # Set the compile flags to have all warnings on (including shadowed variables)
  set(CXXFLAGS "${CXXFLAGS} -Wall -Wshadow")
  # If on a *nix system, also set the compile flags to remove GNU extensions (favor ISO C++) as well as reject non-ISO C++ code, also remove all leading underscores in exported symbols (only on GNU compiler)
  if(UNIX)
    set(CXXFLAGS "${CXXFLAGS} -pedantic ${CMAKE_CXX_FLAGS}")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
      set(CXXFLAGS "${CXXFLAGS} -stdlib=libc++")
      set(LDFLAGS "${LDFLAGS} -stdlib=libc++")
    endif()
  endif()
endif()

# If CMake has found that the given system requires a special library for dl* calls, include it with the linker flags
if(CMAKE_DL_LIBS)
  list(APPEND LINK_LIBS ${CMAKE_DL_LIBS})
endif()

if(NOT PROGRAM_NAME)
  set(PROGRAM_NAME anope)
endif()

# If we are not using Visual Studio, we'll run the following checks
if(NOT MSVC)
  # The following are additional library checks, they are not required for Windows
  if(NOT WIN32)
    # Check if socket is within the socket library (if the library exists), and add it to the linker flags if needed
    check_library_exists(socket socket "" HAVE_SOCKET_LIB)
    if(HAVE_SOCKET_LIB)
      list(APPEND LINK_LIBS socket)
    endif()
    # Check if inet_addr is within the nsl library (if the library exists), and add it to the linker flags if needed
    check_library_exists(nsl inet_addr "" HAVE_NSL_LIB)
    if(HAVE_NSL_LIB)
      list(APPEND LINK_LIBS nsl)
    endif()
    # Check if pthread_create is within the pthread library (if the library exists), and add it to the linker flags if needed
    check_library_exists(pthread pthread_create "" HAVE_PTHREAD)
    if(HAVE_PTHREAD)
      if(NOT APPLE)
        set(LDFLAGS "${LDFLAGS} -pthread")
      endif()
    else()
      message(FATAL_ERROR "The pthread library is required to build Anope")
    endif()
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
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
  set(DEBUG_BUILD TRUE)
endif()

# Check for the existance of the following functions
check_function_exists(umask HAVE_UMASK)
check_function_exists(epoll_wait HAVE_EPOLL)
check_function_exists(poll HAVE_POLL)
check_function_exists(kqueue HAVE_KQUEUE)

# Strip the leading and trailing spaces from the compile flags
if(CXXFLAGS)
  string(STRIP ${CXXFLAGS} CXXFLAGS)
endif()
# Strip the leading and trailing spaces from the linker flags
if(LDFLAGS)
  string(STRIP ${LDFLAGS} LDFLAGS)
endif()

# Search for the following programs
find_program(CHGRP chgrp)
find_program(CHMOD chmod)

# If CMAKE_INSTALL_PREFIX was not passed in to CMake set the default install prefix to the "anope" directory under the user's home directory
if(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/anope")
endif()

# Set default paths for various directories if not already defined
if(NOT BIN_DIR)
  set(BIN_DIR "bin")
endif()
if(NOT DB_DIR)
  set(DB_DIR "data")
endif()
if(NOT DOC_DIR)
  set(DOC_DIR "doc")
endif()
if(NOT CONF_DIR)
  set(CONF_DIR "conf")
endif()
if(NOT LIB_DIR)
  set(LIB_DIR "lib")
endif()
if(NOT LOCALE_DIR)
  set(LOCALE_DIR "locale")
endif()
if(NOT LOGS_DIR)
  set(LOGS_DIR "logs")
endif()

# Version number processing
# Find all lines in src/version.sh that start with VERSION_
file(STRINGS ${Anope_SOURCE_DIR}/src/version.sh VERSIONS REGEX "^VERSION_")
# Iterate through the strings found
foreach(VERSION_STR ${VERSIONS})
  string(REGEX REPLACE "^VERSION_([A-Z]+)=\"?([^\"]*)\"?$" "\\1;\\2" VERSION_OUT ${VERSION_STR})
  # Depends on CMP0007 OLD
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
  file(STRINGS ${Anope_SOURCE_DIR}/include/version.h VERSIONS REGEX "^#defineVERSION_BUILD")
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

# Add the initial files to ignore which will be ignored regardless of if you are building in-source or out-of-source
add_to_cpack_ignored_files(".git\;config.cache\;CMakeFiles\;sysconf.h$\;build" TRUE)
# Add the files we don't want the periods converted for
add_to_cpack_ignored_files(".\\\\\\\\.so$;.\\\\\\\\.o$;.\\\\\\\\.s$;${Anope_SOURCE_DIR}/Makefile$")
# If the two directories are the same, we are building in-source, thus we need to ignore more files from the build
if(${Anope_SOURCE_DIR} STREQUAL ${Anope_BINARY_DIR})
  # Add the files that need their periods converted
  add_to_cpack_ignored_files("Makefile\;cmake_install.cmake\;sysconf.h$\;CMakeCache.txt\;install_manifest.txt" TRUE)
  # Add the files we don't want the periods converted for
  add_to_cpack_ignored_files(".\\\\\\\\.so$;CPack.;anope-${VERSION_FULL_NOBUILD}-source\\\\\\\\..")
  # If using Visual Studio, add these files as well
  if(MSVC)
    add_to_cpack_ignored_files(".vcproj$\;.sln$\;.ncb$\;.suo$\;.dir$\;.ilk$\;.exp$\;.pdb$\;.lib$\;/debug$;/release$;/relwithdebinfo$;/minsizerel$" TRUE)
  endif()
endif()

# Go into the following directories and run their CMakeLists.txt as well
add_subdirectory(data)
add_subdirectory(docs)
add_subdirectory(language)
add_subdirectory(src)
add_subdirectory(modules)
add_subdirectory(include)
add_subdirectory(lib)

# Get the filename of the Anope binary, to use later
get_target_property(SERVICES_BINARY ${PROGRAM_NAME} LOCATION)
get_filename_component(SERVICES_BINARY ${SERVICES_BINARY} NAME)

# At install time, create the following additional directories
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DB_DIR}/backups\")")
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${LOGS_DIR}\")")
if(WIN32)
  install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DB_DIR}/runtime\")")
endif()
# On non-Windows platforms, if RUNGROUP is set, change the permissions of the below directories, as well as the group of the data directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/\${DB_DIR}/backups\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/\${LOGS_DIR}\")")
  install(CODE "execute_process(COMMAND ${CHGRP} -R ${RUNGROUP} \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\")")
endif()
# On Windows platforms, install extra files
if(WIN32)
  install(FILES ${Anope_SOURCE_DIR}/src/win32/anope.bat
    DESTINATION ${BIN_DIR}
  )

  # Package any DLLs in src/win/
  file(GLOB EXTRA_DLLS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${Anope_SOURCE_DIR}/src/win32/*.dll")
  install(FILES ${EXTRA_DLLS} DESTINATION ${BIN_DIR})
endif()

install(CODE "file(REMOVE_RECURSE \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${LIB_DIR}/modules\")")

# Only process the CPack section if we have CPack
if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  # Various options for CPack
  set(CPACK_PACKAGE_NAME "Anope IRC Services")
  set(CPACK_PACKAGE_VENDOR "Anope Team")
  set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}${VERSION_EXTRA}")
  set(CPACK_PACKAGE_FILE_NAME "anope-${VERSION_FULL_NOBUILD}")
  set(CPACK_RESOURCE_FILE_LICENSE "${Anope_SOURCE_DIR}/docs/COPYING")
  # The following doesn't actually do anything. :(
  #set(CPACK_RESOURCE_FILE_README "${Anope_SOURCE_DIR}/docs/README")
  # The following is primarily for NSIS
  if(WIN32)
    # By default, do not warn when built on machines using only VS Express:
    if(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
      set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
    endif()
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
      "http://www.anope.org/" "Anope Web Site"
    )
    # The following doesn't work, but a bug report has been filed about it
    #set(CPACK_CREATE_DESKTOP_LINK_${SERVICES_BINARY} TRUE)
    set(CPACK_NSIS_MUI_ICON "${Anope_SOURCE_DIR}/src\\\\win32\\\\anope-icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${Anope_SOURCE_DIR}/src\\\\win32\\\\anope-icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${SERVICES_BINARY}")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.anope.org/")
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  endif()
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "anope-${VERSION_FULL_NOBUILD}-source")
  set(CPACK_SOURCE_GENERATOR "TGZ")
  set(CPACK_SOURCE_IGNORE_FILES "$ENV{CPACK_IGNORED_FILES}")
  set(CPACK_MONOLITHIC_INSTALL TRUE)
  include(CPack)
endif()
