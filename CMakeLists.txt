# This usage of CMake requires at least version 2.4 (checks are made to determine what to use when certain versions lack functions)
cmake_minimum_required(VERSION 2.4 FATAL_ERROR)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  if(POLICY CMP0026)
    cmake_policy(SET CMP0026 OLD)
  endif(POLICY CMP0026)
  if(POLICY CMP0007)
    cmake_policy(SET CMP0007 OLD)
  endif(POLICY CMP0007)
endif(COMMAND cmake_policy)

# Set the project as C++ primarily, but have C enabled for the checks required later
project(Anope CXX)
enable_language(C)

# Detect the version of CMake for the later conditional checks
execute_process(COMMAND ${CMAKE_COMMAND} --version OUTPUT_VARIABLE VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "cmake version 2\\.(.*)" "\\1" ONLY_VERSION "${VERSION}")
string(REGEX MATCH "-patch .*$" HAS_PATCH "${ONLY_VERSION}")
if(HAS_PATCH)
  string(REGEX REPLACE "(.*)-patch .*" "\\1" MINOR_VERSION "${ONLY_VERSION}")
  string(REGEX REPLACE ".*-patch (.*)" "\\1" PATCH_VERSION "${ONLY_VERSION}")
else(HAS_PATCH)
  string(REGEX MATCH "\\." HAS_DOT "${ONLY_VERSION}")
  if(HAS_DOT)
    string(REGEX REPLACE "(.*)\\..*" "\\1" MINOR_VERSION "${ONLY_VERSION}")
    string(REGEX REPLACE ".*\\.(.*)" "\\1" PATCH_VERSION "${ONLY_VERSION}")
  else(HAS_DOT)
    string(REGEX REPLACE "(.*)-beta" "\\1" MINOR_VERSION "${ONLY_VERSION}")
    if(MINOR_VERSION STREQUAL "4-1\n")
      set(PATCH_VERSION 1)
    else(MINOR_VERSION STREQUAL "4-1\n")
      set(PATCH_VERSION 0)
    endif(MINOR_VERSION STREQUAL "4-1\n")
    set(MINOR_VERSION 4)
  endif(HAS_DOT)
endif(HAS_PATCH)

# Detect is we are using CMake 2.6 or better, these versions include functions that require less work than CMake 2.4 does
if(MINOR_VERSION GREATER 5)
  set(CMAKE26_OR_BETTER TRUE)
  set(CMAKE248_OR_BETTER TRUE)
  set(CMAKE244_OR_BETTER TRUE)
  set(CMAKE242_OR_BETTER TRUE)
else(MINOR_VERSION GREATER 5)
  set(CMAKE26_OR_BETTER FALSE)
  # Also detect if we are using CMake 2.4.8 or better, the FIND sub-command of list() is non-existent in earlier versions
  if(PATCH_VERSION GREATER 7)
    set(CMAKE248_OR_BETTER TRUE)
    set(CMAKE244_OR_BETTER TRUE)
    set(CMAKE242_OR_BETTER TRUE)
  else(PATCH_VERSION GREATER 7)
    set(CMAKE248_OR_BETTER FALSE)
    # Also detect if we are using CMake 2.4.4 or better, the CheckCXXCompilerFlag module and SORT sub-command of list() are non-existent in earlier versions
    if(PATCH_VERSION GREATER 3)
      set(CMAKE244_OR_BETTER TRUE)
      set(CMAKE242_OR_BETTER TRUE)
    else(PATCH_VERSION GREATER 3)
      set(CMAKE244_OR_BETTER FALSE)
      # ALSO detect if we are using CMake 2.4.2 or better, the APPEND sub-command of list() is non-existent in earlier versions
      if(PATCH_VERSION GREATER 1)
        set(CMAKE242_OR_BETTER TRUE)
      else(PATCH_VERSION GREATER 1)
        set(CMAKE242_OR_BETTER FALSE)
      endif(PATCH_VERSION GREATER 1)
    endif(PATCH_VERSION GREATER 3)
  endif(PATCH_VERSION GREATER 7)
endif(MINOR_VERSION GREATER 5)

# Override the module include path to include our directory, for our Anope.cmake, as well as we are using our own version of the NSIS template
set(CMAKE_MODULE_PATH ${Anope_SOURCE_DIR}/cmake)

include(Anope)

# Force the locale to C for later uses of things like gcc so the messages come up in English, not the user's default language
set(ENV{LC_ALL} C)

# Start with empty defaults for library and include directories, to be used by GNU compilers only
set(DEFAULT_LIBRARY_DIRS)
set(DEFAULT_INCLUDE_DIRS)

# Check that we aren't running on an ancient broken GCC
if(CMAKE_COMPILER_IS_GNUCXX)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_FULL_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX REPLACE "^(\\d+\\.\\d+)" "\\1" GCC_VERSION ${GCC_FULL_VERSION})
  if(GCC_VERSION LESS 4.2)
    message(FATAL_ERROR "Your compiler is too old to build Anope. Upgrade to GCC 4.2 or newer!")
  endif(GCC_VERSION LESS 4.2)
endif(CMAKE_COMPILER_IS_GNUCXX)

# If we are using a GNU compiler (have to use CXX because it seems to fail on C), we will be able to determine it's default paths for libraries and includes
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang$")
  # First look for the compiler's default library directories
  execute_process(COMMAND ${CMAKE_C_COMPILER} -print-search-dirs OUTPUT_VARIABLE LINES OUTPUT_STRIP_TRAILING_WHITESPACE)
  # Find only the part after "libraries: "
  string(REGEX REPLACE ".*\nlibraries: (.*)$" "\\1" LINE "${LINES}")
  # Replace the colons in the list with semicolons (only when not on MinGW, which uses semicolons already), and if on MinGW, just copy the line
  if(NOT MINGW)
    string(REGEX REPLACE ":" ";" LIBRARIES ${LINE})
  else(NOT MINGW)
    set(LIBRARIES "${LINE}")
  endif(NOT MINGW)
  # Iterate through the libraries
  foreach(LIBRARY ${LIBRARIES})
    # Check if the first character is an equal sign, and skip that library directory as it is (I believe) the primary default and shows up later in the list anyways
    string(SUBSTRING ${LIBRARY} 0 1 FIRST_CHAR)
    if(NOT FIRST_CHAR STREQUAL "=")
      # If the directory had no = in front of it, make sure it's absolute and add it to the list of default library directories
      get_filename_component(LIBRARY ${LIBRARY} ABSOLUTE)
      append_to_list(DEFAULT_LIBRARY_DIRS ${LIBRARY})
    endif(NOT FIRST_CHAR STREQUAL "=")
  endforeach(LIBRARY)
  # Remove duplicate entries from the list
  if(DEFAULT_LIBRARY_DIRS)
    remove_list_duplicates(DEFAULT_LIBRARY_DIRS)
  endif(DEFAULT_LIBRARY_DIRS)
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
    else(LINE STREQUAL "#include <...> search starts here:")
      # If the line has the following on it, we hit the end of the list
      if(LINE STREQUAL "End of search list.")
        set(IN_SEARCH FALSE)
      else(LINE STREQUAL "End of search list.")
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
          append_to_list(DEFAULT_INCLUDE_DIRS ${INCLUDE})
        endif(IN_SEARCH)
      endif(LINE STREQUAL "End of search list.")
    endif(LINE STREQUAL "#include <...> search starts here:")
  endforeach(LINE)
  # Remove duplicate entries from the list
  if(DEFAULT_INCLUDE_DIRS)
    remove_list_duplicates(DEFAULT_INCLUDE_DIRS)
  endif(DEFAULT_INCLUDE_DIRS)
endif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang$")

# If we are using Visual Studio, locate the path of the Windows Server 2008 SDK or Windows Server 2003 Platform SDK, depending on which is installed
if(MSVC)
  # If the path comes up as "/registry" from any of these, the path wasn't found, otherwise, we'll set WSDK_PATH to the corresponding path
  # Look for the 2008 SDK under HKLM first
  get_filename_component(WSDK2008_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" ABSOLUTE CACHE)
  if(WSDK2008_PATH STREQUAL "/registry")
    # If not found, look for the 2003 SDK under HKLM
    get_filename_component(WSDK2003_PATH "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\MicrosoftSDK\\InstalledSDKs\\D2FF9F89-8AA2-4373-8A31-C838BF4DBBE1;Install Dir]" ABSOLUTE CACHE)
    if(WSDK2003_PATH STREQUAL "/registry")
      # If not found, look for the 2008 SDK under HKCU
      get_filename_component(WSDK2008_PATH "[HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows;CurrentInstallFolder]" ABSOLUTE CACHE)
      if(WSDK2008_PATH STREQUAL "/registry")
        # If not found, look for the 2003 SDK under HKCU
        get_filename_component(WSDK2003_PATH "[HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\MicrosoftSDK\\InstalledSDKs\\D2FF9F89-8AA2-4373-8A31-C838BF4DBBE1;Install Dir]" ABSOLUTE CACHE)
        if(WSDK2003_PATH STREQUAL "/registry")
          # The SDK was never found, set the path to nothing
          set(WSDK_PATH "")
        else(WSDK2003_PATH STREQUAL "/registry")
          set(WSDK_PATH "${WSDK2003_PATH}")
        endif(WSDK2003_PATH STREQUAL "/registry")
      else(WSDK2008_PATH STREQUAL "/registry")
        set(WSDK_PATH "${WSDK2008_PATH}")
      endif(WSDK2008_PATH STREQUAL "/registry")
    else(WSDK2003_PATH STREQUAL "/registry")
      set(WSDK_PATH "${WSDK2003_PATH}")
    endif(WSDK2003_PATH STREQUAL "/registry")
  else(WSDK2008_PATH STREQUAL "/registry")
    set(WSDK_PATH "${WSDK2008_PATH}")
  endif(WSDK2008_PATH STREQUAL "/registry")
endif(MSVC)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug
# Only do this if not using Visual Studio
if(NOT MSVC)
  if(CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
  else(CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
  endif(CMAKE_BUILD_TYPE)
endif(NOT MSVC)

# If running under MinGW, we have to force the resource compiler settings (hopefully this will be fixed in a later version of CMake)
if(MINGW)
  set(CMAKE_RC_COMPILER_INIT windres)
  enable_language(RC)
  set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
endif(MINGW)

# Include the checking functions used later in this CMakeLists.txt
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckTypeSize)
include(CheckLibraryExists)
if(CMAKE244_OR_BETTER)
  include(CheckCXXCompilerFlag)
else(CMAKE244_OR_BETTER)
  include(TestCXXAcceptsFlag)
endif(CMAKE244_OR_BETTER)

# If extra include directories were specified, tell cmake about them.
if(EXTRA_INCLUDE)
  include_directories(${EXTRA_INCLUDE})
endif(EXTRA_INCLUDE)

# If extra library directories were specified, tell cmake about them.
if(EXTRA_LIBS)
  link_directories(${EXTRA_LIBS})
endif(EXTRA_LIBS)

# Find gettext
find_package(Gettext)

option(USE_PCH "Use precompiled headers" OFF)

# Use the following directories as includes
# Note that it is important the binary include directory comes before the
# source include directory so the precompiled headers work correctly.
include_directories(${Anope_BINARY_DIR}/include ${Anope_SOURCE_DIR}/include ${Anope_BINARY_DIR}/language ${Anope_SOURCE_DIR}/modules/pseudoclients)

# If using Windows, always add the _WIN32 define
if(WIN32)
  add_definitions(-D_WIN32)
  # And include the windows specific folder for our anope_windows.h
  include_directories(${Anope_SOURCE_DIR}/src/win32)
endif(WIN32)

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
else(MSVC)
  # Set the compile flags to have all warnings on (including shadowed variables)
  set(CXXFLAGS "${CXXFLAGS} -Wall -Wshadow")
  # If on a *nix system, also set the compile flags to remove GNU extensions (favor ISO C++) as well as reject non-ISO C++ code, also remove all leading underscores in exported symbols (only on GNU compiler)
  if(UNIX)
    set(CXXFLAGS "${CXXFLAGS} -ansi -pedantic ${CMAKE_CXX_FLAGS}")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set(CXXFLAGS "${CXXFLAGS} -Wno-long-long -fno-leading-underscore")
    endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # If we aren't on a *nix system, we are using MinGW
  else(UNIX)
    # Also, if we are building under MinGW, add another define for MinGW
    if(MINGW)
      add_definitions(-DMINGW)
    endif(MINGW)
  endif(UNIX)
endif(MSVC)

# If CMake has found that the given system requires a special library for dl* calls, include it with the linker flags
if(CMAKE_DL_LIBS)
  append_to_list(LINK_LIBS ${CMAKE_DL_LIBS})
endif(CMAKE_DL_LIBS)

# Under MinGW, the -shared flag isn't properly set in the module-specific linker flags, add it from the C flags for shared libraries
if(MINGW)
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS}")
endif(MINGW)

if(NOT PROGRAM_NAME)
  # Under Windows, we set the executable name for Anope to be anope
  if(WIN32)
    set(PROGRAM_NAME anope)
  # Under *nix, we set the executable name for Anope to be services
  else(WIN32)
    set(PROGRAM_NAME services)
  endif(WIN32)
endif(NOT PROGRAM_NAME)

# If we are not using Visual Studio, we'll run the following checks
if(NOT MSVC)
  # Check if the C++ compiler can accept the -pipe flag, and add it to the compile flags if it works
  if(CMAKE244_OR_BETTER)
    # If using CMake 2.4.4 or better, we can use check_cxx_compiler_flag
    check_cxx_compiler_flag(-pipe HAVE_PIPE_FLAG)
  else(CMAKE244_OR_BETTER)
    # If using CMake 2.4.3 or older, we will use check_cxx_accepts_flags instead
    check_cxx_accepts_flag(-pipe HAVE_PIPE_FLAG)
  endif(CMAKE244_OR_BETTER)
  # If the flag was accepted, add it to the list of flags
  if(HAVE_PIPE_FLAG)
    set(CXXFLAGS "${CXXFLAGS} -pipe")
  endif(HAVE_PIPE_FLAG)

  # The following are additional library checks, they are not required for Windows
  if(NOT WIN32)
    # Check if socket is within the socket library (if the library exists), and add it to the linker flags if needed
    check_library_exists(socket socket "" HAVE_SOCKET_LIB)
    if(HAVE_SOCKET_LIB)
      append_to_list(LINK_LIBS socket)
    endif(HAVE_SOCKET_LIB)
    # Check if inet_addr is within the nsl library (if the library exists), and add it to the linker flags if needed
    check_library_exists(nsl inet_addr "" HAVE_NSL_LIB)
    if(HAVE_NSL_LIB)
      append_to_list(LINK_LIBS nsl)
    endif(HAVE_NSL_LIB)
    # Check if pthread_create is within the pthread library (if the library exists), and add it to the linker flags if needed
    check_library_exists(pthread pthread_create "" HAVE_PTHREAD)
    if(HAVE_PTHREAD)
      if(NOT APPLE)
        set(LDFLAGS "${LDFLAGS} -pthread")
      endif(NOT APPLE)
    else(HAVE_PTHREAD)
      message(FATAL_ERROR "The pthread library is required to build Anope")
    endif(HAVE_PTHREAD)
  endif(NOT WIN32)
endif(NOT MSVC)

# If DEFUMASK wasn't passed to CMake, set a default depending on if RUNGROUP was passed in or not
if(NOT DEFUMASK)
  if(RUNGROUP)
    set(DEFUMASK "007")
  else(RUNGROUP)
    set(DEFUMASK "077")
  endif(RUNGROUP)
endif(NOT DEFUMASK)

# Set the DEBUG_BUILD for sysconf.h
if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
  set(DEBUG_BUILD TRUE)
endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")

# Check for the existence of the following include files
check_include_file(cstdint HAVE_CSTDINT)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(strings.h HAVE_STRINGS_H)

# Check for the existence of the following functions
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(stricmp HAVE_STRICMP)
check_function_exists(umask HAVE_UMASK)
check_function_exists(epoll_wait HAVE_EPOLL)
check_function_exists(poll HAVE_POLL)
check_function_exists(kqueue HAVE_KQUEUE)

# Strip the leading and trailing spaces from the compile flags
if(CXXFLAGS)
  strip_string(${CXXFLAGS} CXXFLAGS)
endif(CXXFLAGS)
# Strip the leading and trailing spaces from the linker flags
if(LDFLAGS)
  strip_string(${LDFLAGS} LDFLAGS)
endif(LDFLAGS)

# Search for the following programs
find_program(GREP grep)
find_program(SH sh)
find_program(CHGRP chgrp)
find_program(CHMOD chmod)

# If a INSTDIR was passed in to CMake, use it as the install prefix, otherwise set the default install prefix to the services directory under the user's home directory
if(INSTDIR)
  set(CMAKE_INSTALL_PREFIX "${INSTDIR}")
elseif(NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/services")
endif(INSTDIR)

# Set default paths for various directories if not already defined
if(NOT BIN_DIR)
  set(BIN_DIR "bin")
endif(NOT BIN_DIR)
if(NOT DB_DIR)
  set(DB_DIR "data")
endif(NOT DB_DIR)
if(NOT DOC_DIR)
  set(DOC_DIR "doc")
endif(NOT DOC_DIR)
if(NOT CONF_DIR)
  set(CONF_DIR "conf")
endif(NOT CONF_DIR)
if(NOT LIB_DIR)
  set(LIB_DIR "lib")
endif(NOT LIB_DIR)
if(NOT LOCALE_DIR)
  set(LOCALE_DIR "locale")
endif(NOT LOCALE_DIR)
if(NOT LOGS_DIR)
  set(LOGS_DIR "logs")
endif(NOT LOGS_DIR)

# Version number processing
# Find all lines in src/version.sh that start with VERSION_
read_from_file(${Anope_SOURCE_DIR}/src/version.sh "^VERSION_" VERSIONS)
# Iterate through the strings found
foreach(VERSION_STR ${VERSIONS})
  string(REGEX REPLACE "^VERSION_([A-Z]+)=\"?([^\"]*)\"?$" "\\1;\\2" VERSION_OUT ${VERSION_STR})
  # Depends on CMP0007 OLD
  list(LENGTH VERSION_OUT VERSION_LEN)
  list(GET VERSION_OUT 0 VERSION_TYPE)
  if(${VERSION_LEN} GREATER 1)
    list(GET VERSION_OUT 1 VERSION_DATA)
    set(VERSION_${VERSION_TYPE} ${VERSION_DATA})
  endif(${VERSION_LEN} GREATER 1)
endforeach(VERSION_STR ${VERSIONS})

# Default build version to 0
set(VERSION_BUILD 0)

# Only change the build number if version.h exists
if(EXISTS "${Anope_SOURCE_DIR}/include/version.h")
  # Attempt to read the build number from include/version.h
  read_from_file(${Anope_SOURCE_DIR}/include/version.h "^#define VERSION_BUILD" VERSIONS)
  foreach(VERSION_STR ${VERSIONS})
    # Get the length of the string
    string(LENGTH ${VERSION_STR} VERSION_LEN)
    # Subtract 22 from the string's length
    math(EXPR VERSION_NUM_LEN "${VERSION_LEN} - 22")
    # Extract the value from the string
    string(SUBSTRING ${VERSION_STR} 22 ${VERSION_NUM_LEN} VERSION)
    # Set VERSION_BUILD correctly
    set(VERSION_BUILD ${VERSION})
  endforeach(VERSION_STR ${VERSIONS})
endif(EXISTS "${Anope_SOURCE_DIR}/include/version.h")

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
endif(WIN32)

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
  endif(MSVC)
endif(${Anope_SOURCE_DIR} STREQUAL ${Anope_BINARY_DIR})

# Go into the following directories and run their CMakeLists.txt as well
add_subdirectory(data)
add_subdirectory(docs)
add_subdirectory(language)
add_subdirectory(src)
add_subdirectory(modules)
add_subdirectory(include)

# Get the filename of the Anope binary, to use later
get_target_property(SERVICES_BINARY ${PROGRAM_NAME} LOCATION)
get_filename_component(SERVICES_BINARY ${SERVICES_BINARY} NAME)

# At install time, create the following additional directories
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${DB_DIR}/backups\")")
install(CODE "file(MAKE_DIRECTORY \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${LOGS_DIR}\")")
if(WIN32)
  install(CODE "file(MAKE_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/${DB_DIR}/runtime\")")
endif(WIN32)
# On non-Windows platforms, if RUNGROUP is set, change the permissions of the below directories, as well as the group of the data directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/\${DB_DIR}/backups\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/\${LOGS_DIR}\")")
  install(CODE "execute_process(COMMAND ${CHGRP} -R ${RUNGROUP} \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}\")")
endif(NOT WIN32 AND RUNGROUP)
# On Windows platforms, install extra files
if(WIN32)
  install(FILES ${Anope_SOURCE_DIR}/src/win32/anope.bat
    DESTINATION ${BIN_DIR}
  )

  # Package any DLLs in src/win/
  file(GLOB EXTRA_DLLS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${Anope_SOURCE_DIR}/src/win32/*.dll")
  install(FILES ${EXTRA_DLLS} DESTINATION ${BIN_DIR})
endif(WIN32)

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
      "http://www.anope.org/" "Anope Web Site"
    )
    # The following doesn't work, but a bug report has been filed about it
    #set(CPACK_CREATE_DESKTOP_LINK_${SERVICES_BINARY} TRUE)
    set(CPACK_NSIS_MUI_ICON "${Anope_SOURCE_DIR}/src\\\\win32\\\\anope-icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${Anope_SOURCE_DIR}/src\\\\win32\\\\anope-icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${SERVICES_BINARY}")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.anope.org/")
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  endif(WIN32)
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "anope-${VERSION_FULL_NOBUILD}-source")
  set(CPACK_SOURCE_GENERATOR "TGZ")
  set(CPACK_SOURCE_IGNORE_FILES "$ENV{CPACK_IGNORED_FILES}")
  set(CPACK_MONOLITHIC_INSTALL TRUE)
  include(CPack)
endif(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
