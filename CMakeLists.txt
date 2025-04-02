# This usage of CMake requires at least version 3.20
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Set the project as C++ primarily, but have C enabled for the checks required later
project(Anope CXX)
enable_language(C)

# Override the module include path to include our directory, for our Anope.cmake, as well as we are using our own version of the NSIS template
set(CMAKE_MODULE_PATH ${Anope_SOURCE_DIR}/cmake)

include(Anope)

# Force the locale to C for later uses of things like gcc so the messages come up in English, not the user's default language
set(ENV{LC_ALL} C)

# Start with empty defaults for library and include directories, to be used by GNU compilers only
set(DEFAULT_LIBRARY_DIRS)
set(DEFAULT_INCLUDE_DIRS)

# We require C++17 to build
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put modules in their own folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# If we are using a GNU compiler (have to use CXX because it seems to fail on C), we will be able to determine it's default paths for libraries and includes
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang$")
  # First look for the compiler's default library directories
  execute_process(COMMAND ${CMAKE_C_COMPILER} -print-search-dirs OUTPUT_VARIABLE LINES OUTPUT_STRIP_TRAILING_WHITESPACE)
  # Find only the part after "libraries: "
  string(REGEX REPLACE ".*\nlibraries: (.*)$" "\\1" LINE "${LINES}")
  # Replace the colons in the list with semicolons
  string(REGEX REPLACE ":" ";" LIBRARIES ${LINE})
  # Iterate through the libraries
  foreach(LIBRARY ${LIBRARIES})
    # Check if the first character is an equal sign, and skip that library directory as it is (I believe) the primary default and shows up later in the list anyways
    string(SUBSTRING ${LIBRARY} 0 1 FIRST_CHAR)
    if(NOT FIRST_CHAR STREQUAL "=")
      # If the directory had no = in front of it, make sure it's absolute and add it to the list of default library directories
      get_filename_component(LIBRARY ${LIBRARY} ABSOLUTE)
      list(APPEND DEFAULT_LIBRARY_DIRS ${LIBRARY})
    endif()
  endforeach()
  # Remove duplicate entries from the list
  if(DEFAULT_LIBRARY_DIRS)
    list(REMOVE_DUPLICATES DEFAULT_LIBRARY_DIRS)
  endif()
  # Create a temporary file to test for the default include directories
  FILE(WRITE empty.cpp "")
  # Next, we look for the compiler's default include directories
  # Run the command to find the default include directories
  execute_process(COMMAND ${CMAKE_C_COMPILER} -v -x c++ -E ${CMAKE_CURRENT_SOURCE_DIR}/empty.cpp ERROR_VARIABLE LINES OUTPUT_QUIET ERROR_STRIP_TRAILING_WHITESPACE)
  # Remove the empty file, it is no longer needed
  FILE(REMOVE empty.cpp)
  # Convert the new lines to semicolons
  string(REGEX REPLACE "\n" ";" LINES ${LINES})
  # Temporary variable saying if we are in the search list or not
  set(IN_SEARCH_LIST FALSE)
  # Iterate through the lines
  foreach(LINE ${LINES})
    # If the line has the following on it, the next lines will contain directory names
    if(LINE STREQUAL "#include <...> search starts here:")
      set(IN_SEARCH TRUE)
    else()
      # If the line has the following on it, we hit the end of the list
      if(LINE STREQUAL "End of search list.")
        set(IN_SEARCH FALSE)
      else()
        # If we are within the block between the above two lines...
        if(IN_SEARCH)
          # Get everything but the first character of the line
          string(LENGTH ${LINE} LINE_LENGTH)
          math(EXPR LINE_LENGTH "${LINE_LENGTH} - 1")
          string(SUBSTRING ${LINE} 1 ${LINE_LENGTH} INCLUDE)
          # For systems like Mac OS X, look for include paths that say " (framework directory)" at the end of them and strip that off
          string(REGEX REPLACE " \\(framework directory\\)$" "" INCLUDE ${INCLUDE})
          # Convert the path to an absolute one, just in case it wasn't
          get_filename_component(INCLUDE ${INCLUDE} ABSOLUTE)
          # Add that directory to the list of default include directories
          list(APPEND DEFAULT_INCLUDE_DIRS ${INCLUDE})
        endif()
      endif()
    endif()
  endforeach()
  # Remove duplicate entries from the list
  if(DEFAULT_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES DEFAULT_INCLUDE_DIRS)
  endif()
endif()

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
endif()

# Include the checking functions used later in this CMakeLists.txt
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckLibraryExists)
include(CheckCXXCompilerFlag)

# If extra include directories were specified, tell cmake about them.
if(EXTRA_INCLUDE)
  include_directories(${EXTRA_INCLUDE})
endif()

# If extra library directories were specified, tell cmake about them.
if(EXTRA_LIBS)
  link_directories(${EXTRA_LIBS})
endif()

# setup conan
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake")
  conan_basic_setup()
endif()

# Find gettext
find_package(Gettext)
find_package(Intl)
if(GETTEXT_FOUND AND Intl_FOUND)
  set(HAVE_LOCALIZATION ON)
  include_directories(${Intl_INCLUDE_DIRS})
else()
  message("Unable to find gettext and libintl; disabling localization")
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
  # Remove the default exception handling flags, also remove default warning level flag
  string(REPLACE "/EHsc " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/GX " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/W3 " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # Set the compile flags to have warnings on the max setting (but disable a few annoying ones), exception handling turned on, the proper defines
  set(CXXFLAGS "${CXXFLAGS} /W4 /wd4100 /wd4127 /wd4250 /wd4251 /wd4267 /wd4275 /wd4355 /wd4706 /wd4800 /wd4996 /EHs")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
# Otherwise, we're not using Visual Studio
else()
  # Set the compile flags to have all warnings on (including shadowed variables)
  set(CXXFLAGS "${CXXFLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Wextra -Wformat=2 -Wmissing-format-attribute -Wpedantic -Wno-format-nonliteral -Wno-format-zero-length -Wno-unused-parameter ${CMAKE_CXX_FLAGS}")
endif()

# If CMake has found that the given system requires a special library for dl* calls, include it with the linker flags
if(CMAKE_DL_LIBS)
  list(APPEND LINK_LIBS ${CMAKE_DL_LIBS})
endif()

# Find the linker flags required for using threads.
find_package("Threads" REQUIRED)
if(CMAKE_THREAD_LIBS_INIT)
  list(APPEND LINK_LIBS ${CMAKE_THREAD_LIBS_INIT})
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
    set(CXXFLAGS "${CXXFLAGS} -pipe")
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
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(DEBUG_BUILD TRUE)
endif()

# Check for the existence of the following functions
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
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
if(NOT WIN32 AND RUNGROUP)
  find_program(CHGRP "chgrp" REQUIRED)
  find_program(CHMOD "chmod" REQUIRED)
endif()

# If a INSTDIR was passed in to CMake, use it as the install prefix, otherwise set the default install prefix to the anope directory under the user's home directory
if(INSTDIR)
  set(CMAKE_INSTALL_PREFIX "${INSTDIR}")
elseif(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/anope")
endif()

# Set default paths for various directories if not already defined
if(NOT BIN_DIR)
  set(BIN_DIR "bin")
endif()
if(NOT DATA_DIR)
  set(DATA_DIR "data")
endif()
if(NOT DOC_DIR)
  set(DOC_DIR "doc")
endif()
if(NOT CONF_DIR)
  set(CONF_DIR "conf")
endif()
if(NOT MODULE_DIR)
  set(MODULE_DIR "modules")
endif()
if(NOT LOCALE_DIR)
  set(LOCALE_DIR "locale")
endif()
if(NOT LOG_DIR)
  set(LOG_DIR "logs")
endif()

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

# Get the filename of the Anope binary, to use later
set(SERVICES_BINARY "$<TARGET_FILE:${PROGRAM_NAME}>")
get_filename_component(SERVICES_BINARY ${SERVICES_BINARY} NAME)

# At install time, create the following additional directories
get_filename_component(ABSOLUTE_DATA_DIR ${DATA_DIR} REALPATH BASE_DIR ${CMAKE_INSTALL_PREFIX})
get_filename_component(ABSOLUTE_LOG_DIR ${LOG_DIR} REALPATH BASE_DIR ${CMAKE_INSTALL_PREFIX})
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}${ABSOLUTE_DATA_DIR}/backups\")")
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}${ABSOLUTE_LOG_DIR}\")")
if(WIN32)
  install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}${ABSOLUTE_DATA_DIR}/runtime\")")
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

get_filename_component(ABSOLUTE_MODULE_DIR ${MODULE_DIR} REALPATH BASE_DIR ${CMAKE_INSTALL_PREFIX})
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
  set(CPACK_RESOURCE_FILE_LICENSE "${Anope_SOURCE_DIR}/docs/COPYING")
  # The following doesn't actually do anything. :(
  #set(CPACK_RESOURCE_FILE_README "${Anope_SOURCE_DIR}/docs/README")
  # The following is primarily for NSIS
  if(WIN32)
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
  set(CPACK_SOURCE_IGNORE_FILES "$ENV{CPACK_IGNORED_FILES}")
  set(CPACK_MONOLITHIC_INSTALL TRUE)
  include(CPack)
endif()
