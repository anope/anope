# This usage of CMake requires at least version 2.4 (checks are made to determine what to use when certain versions lack functions)
cmake_minimum_required(VERSION 2.4 FATAL_ERROR)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# If the Source dir and the Binary dir are the same, we are building in-source, which we will disallow due to Autotools being there (but only on non-Windows)
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR} AND NOT WIN32)
  message(FATAL_ERROR "You can not use CMake to build Anope from the root of it's source tree! Remove the CMakeCache.txt file from this directory, then create a separate directory (either below this directory or elsewhere), and then re-run CMake from there.")
endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR} AND NOT WIN32)

# Detect is we are using CMake 2.6 or better, these versions include functions that require less work than CMake 2.4 does
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
  set(CMAKE26_OR_BETTER TRUE)
  set(CMAKE244_OR_BETTER TRUE)
  set(CMAKE242_OR_BETTER TRUE)
else(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
  set(CMAKE26_OR_BETTER FALSE)
  # Also detect if we are using CMake 2.4.4 or better, the CheckCXXCompilerFlag module is non-existant in earlier versions
  if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.3)
    set(CMAKE244_OR_BETTER TRUE)
    set(CMAKE242_OR_BETTER TRUE)
  else(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.3)
    set(CMAKE244_OR_BETTER FALSE)
    # ALSO detect if we are using CMake 2.4.2 or better, the APPEND sub-command of list() is non-existant in earlier versions
    if(CMAKE_PATCH_VERSION GREATER 1)
      set(CMAKE242_OR_BETTER TRUE)
    else(CMAKE_PATCH_VERSION GREATER 1)
      set(CMAKE242_OR_BETTER FALSE)
    endif(CMAKE_PATCH_VERSION GREATER 1)
  endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.3)
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
# Only do this if not using Visual Studio
if(NOT MSVC)
  if(CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
  else(CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
  endif(CMAKE_BUILD_TYPE)
endif(NOT MSVC)

# Set the project as C++ primarily, but have C enabled for the checks required later
project(Anope CXX)
enable_language(C)

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

# Add an optional variable for using run-cc.pl for building, Perl will be checked later regardless of this setting
option(USE_RUN_CC_PL "Use run-cc.pl for building" OFF)

# Use the following directories as includes
include_directories(${Anope_SOURCE_DIR}/include ${Anope_BINARY_DIR}/include ${Anope_BINARY_DIR}/lang)

# If using Windows, always add the _WIN32 define
if(WIN32)
  add_definitions(-D_WIN32)
endif(WIN32)

# If using Visual Studio, set the C++ flags accordingly
if(MSVC)
  # Remove the default exception handling flags, also remove default warning level flag
  string(REPLACE "/EHsc " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/GX " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/W3 " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # Set the compile flags to have warnings on the max setting (but disable a few annoying ones), exception handling turned on, the proper defines
  set(CXXFLAGS "${CXXFLAGS} /W4 /wd4251 /wd4706 /wd4800 /wd4996 /EHs")
  add_definitions(-DMSVCPP -D_CRT_SECURE_NO_WARNINGS)
# Otherwise, we're not using Visual Studio
else(MSVC)
  # Set the compile flags to have all warnings on (including shadowed variables)
  set(CXXFLAGS "${CXXFLAGS} -Wall -Wshadow")
  # If on a *nix system, also set the compile flags to remove GNU extensions (favor ISO C++) as well as reject non-ISO C++ code, also remove all leading underscores in exported symbols
  if(UNIX)
    set(CXXFLAGS "${CXXFLAGS} -ansi -pedantic -fno-leading-underscore")
    # Set the module-specific compile flags to the same setting as the compile flags
    set(MODULE_CXXFLAGS "${CXXFLAGS}")
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
  set(LDFLAGS "${LDFLAGS} -l${CMAKE_DL_LIBS}")
endif(CMAKE_DL_LIBS)

# Under MinGW, the -shared flag isn't properly set in the module-specific linker flags, add it from the C flags for shared libraries
if(MINGW)
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS}")
endif(MINGW)

# Under Windows, we set the executable name for Anope to be anope
if(WIN32)
  set(PROGRAM_NAME anope)
# Under *nix, we set the executable name for Anope to be services
else(WIN32)
  set(PROGRAM_NAME services)
endif(WIN32)

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
      set(LDFLAGS "${LDFLAGS} -lsocket")
    endif(HAVE_SOCKET_LIB)
  endif(NOT WIN32)

  # Check if va_list can be copied as an array, and if it can, set the flag for it
  try_run(RUN_VA_LIST_AS_ARRAY COMPILE_VA_LIST_AS_ARRAY
    ${Anope_SOURCE_DIR} ${Anope_SOURCE_DIR}/va_list_check.c
  )
  if(COMPILE_VA_LIST_AS_ARRAY AND NOT RUN_VA_LIST_AS_ARRAY)
    set(HAVE_VA_LIST_AS_ARRAY 1)
  endif(COMPILE_VA_LIST_AS_ARRAY AND NOT RUN_VA_LIST_AS_ARRAY)
endif(NOT MSVC)

# If DEFUMASK wasn't passed to CMake, set a default depending on if RUNGROUP was passed in or not
if(NOT DEFUMASK)
  if(RUNGROUP)
    set(DEFUMASK "007")
  else(RUNGROUP)
    set(DEFUMASK "077")
  endif(RUNGROUP)
endif(NOT DEFUMASK)

# Check for the existance of the following include files
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(sys/select.h HAVE_SYS_SELECT_H)

# Check for the existance of the following functions
check_function_exists(gethostbyname HAVE_GETHOSTBYNAME)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(setgrent HAVE_SETGRENT)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(stricmp HAVE_STRICMP)
check_function_exists(strlcat HAVE_STRLCAT)
check_function_exists(strlcpy HAVE_STRLCPY)
check_function_exists(umask HAVE_UMASK)
check_function_exists(backtrace HAVE_BACKTRACE)

# Check for the existance of the following types
check_type_size(uint8_t UINT8_T)
check_type_size(u_int8_t U_INT8_T)
check_type_size(int16_t INT16_T)
check_type_size(uint16_t UINT16_T)
check_type_size(u_int16_t U_INT16_T)
check_type_size(int32_t INT32_T)
check_type_size(uint32_t UINT32_T)
check_type_size(u_int32_t U_INT32_T)

# Only CMake 2.6.x and later contain the STRIP sub-command for string()
if(CMAKE26_OR_BETTER)
  # Strip the leading and trailing spaces from the compile flags
  if(CXXFLAGS)
    string(STRIP ${CXXFLAGS} CXXFLAGS)
  endif(CXXFLAGS)
  # Strip the leading and trailing spaces from the linker flags
  if(LDFLAGS)
    string(STRIP ${LDFLAGS} LDFLAGS)
  endif(LDFLAGS)
endif(CMAKE26_OR_BETTER)

# A macro to handle appending to lists
macro(append_to_list LIST)
  if(CMAKE242_OR_BETTER)
    # For CMake 2.4.2 or better, we can just use the APPEND sub-command of list()
    list(APPEND ${LIST} ${ARGN})
  else(CMAKE242_OR_BETTER)
    # For CMake 2.4.x before 2.4.2, we have to do this manually use set() instead
    set(${LIST} ${${LIST}} ${ARGN})
  endif(CMAKE242_OR_BETTER)
endmacro(append_to_list)

# A macro to handle reading specific lines from a file
macro(read_from_file FILE REGEX STRINGS)
  if(CMAKE26_OR_BETTER)
    # For CMake 2.6.x or better, we can just use this function to get the lines that match the given regular expression
    file(STRINGS ${FILE} RESULT REGEX ${REGEX})
  else(CMAKE26_OR_BETTER)
    # For CMake 2.4.x, we need to do this manually, firsly we read the file in
    file(READ ${FILE} ALL_STRINGS)
    # Next we replace all newlines with semicolons
    string(REGEX REPLACE "\n" ";" ALL_STRINGS ${ALL_STRINGS})
    # Clear the result list
    set(RESULT)
    # Iterate through all the lines of the file
    foreach(STRING ${ALL_STRINGS})
      # Check for a match against the given regular expression
      string(REGEX MATCH ${REGEX} STRING_MATCH ${STRING})
      # If we had a match, append the match to the list
      if(STRING_MATCH)
        append_to_list(RESULT ${STRING})
      endif(STRING_MATCH)
    endforeach(STRING)
  endif(CMAKE26_OR_BETTER)
  # Set the given STRINGS variable to the result
  set(${STRINGS} ${RESULT})
endmacro(read_from_file)

# A macro to handle searching within a list
macro(find_in_list LIST ITEM_TO_FIND FOUND)
  if(CMAKE26_OR_BETTER OR ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.7)
    # For CMake 2.6.x or better (as well as CMake 2.4.8 or better), we can use the FIND sub-command of list()
    list(FIND ${LIST} ${ITEM_TO_FIND} ITEM_FOUND)
  else(CMAKE26_OR_BETTER OR ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.7)
    # For CMake 2.4.x before 2.4.8, we have to do this ourselves (NOTE: This is very slow due to a lack of break() as well), firstly we set that we a temporary boolean
    set(ITEM_FOUND -1)
    set(POS 0)
    # Iterate through the list
    foreach(ITEM ${${LIST}})
      # If the item we are looking at is the item we are trying to find, set that we've found the item
      if(${ITEM} STREQUAL ${ITEM_TO_FIND})
        set(ITEM_FOUND ${POS})
      endif(${ITEM} STREQUAL ${ITEM_TO_FIND})
      math(EXPR POS "${POS} + 1")
    endforeach(ITEM)
  endif(CMAKE26_OR_BETTER OR ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.7)
  # Set the given FOUND variable to the result
  set(${FOUND} ${ITEM_FOUND})
endmacro(find_in_list)

# A macro to handle removing duplicates from a list
macro(remove_list_duplicates LIST)
  if(CMAKE26_OR_BETTER)
    # For CMake 2.6.x or better, this can be done automatically
    list(REMOVE_DUPLICATES ${LIST})
  else(CMAKE26_OR_BETTER)
    # For CMake 2.4.x, we have to do this ourselves, firstly we'll clear a temporary list
    set(NEW_LIST)
    # Iterate through the old list
    foreach(ITEM ${${LIST}})
      # Check if the item is in the new list
      find_in_list(NEW_LIST ${ITEM} FOUND_ITEM)
      if(FOUND_ITEM EQUAL -1)
        # If the item was not found, append it to the list
        append_to_list(NEW_LIST ${ITEM})
      endif(FOUND_ITEM EQUAL -1)
    endforeach(ITEM)
    # replace the old list with the new list
    set(${LIST} ${NEW_LIST})
  endif(CMAKE26_OR_BETTER)
endmacro(remove_list_duplicates)

# Search for the following programs
find_program(GREP grep)
find_program(SH sh)
find_program(CHGRP chgrp)
find_program(CHMOD chmod)
find_program(PERL perl)

# If perl is included on the system and the user wants to use run-cc.pl, change the commands for compiling and linking
if(PERL AND USE_RUN_CC_PL)
  set(CMAKE_CXX_COMPILE_OBJECT "${PERL} ${Anope_SOURCE_DIR}/run-cc.pl -q ${CMAKE_CXX_COMPILE_OBJECT}")
  set(CMAKE_CXX_LINK_EXECUTABLE "${PERL} ${Anope_SOURCE_DIR}/run-cc.pl -q ${CMAKE_CXX_LINK_EXECUTABLE}")
  set(CMAKE_CXX_CREATE_SHARED_MODULE "${PERL} ${Anope_SOURCE_DIR}/run-cc.pl -q ${CMAKE_CXX_CREATE_SHARED_MODULE}")
endif(PERL AND USE_RUN_CC_PL)

# If a INSTDIR was passed in to CMake, use it as the install prefix, otherwise set the default install prefix to the services directory under the user's home directory
if(INSTDIR)
  set(CMAKE_INSTALL_PREFIX "${INSTDIR}")
else(INSTDIR)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/services")
endif(INSTDIR)

# Version number processing
# Find all lines in version.log that start with VERSION_
read_from_file(${Anope_SOURCE_DIR}/version.log "^VERSION_" VERSIONS)
# Iterate through the strings found
foreach(VERSION_STR ${VERSIONS})
  # Get the length of the string
  string(LENGTH ${VERSION_STR} VERSION_LEN)
  # Subtract 16 from the string's length (8 for VERSION_, 5 more for the type, 2 for the space and leading quote, 1 for the trailing quote)
  math(EXPR VERSION_NUM_LEN "${VERSION_LEN} - 16")
  # Extract the type from the string
  string(SUBSTRING ${VERSION_STR} 8 5 VERSION_TYPE)
  # Extract the actual value from the string
  string(SUBSTRING ${VERSION_STR} 15 ${VERSION_NUM_LEN} VERSION)
  # Set the version type to the value extract from above
  set(VERSION_${VERSION_TYPE} ${VERSION})
endforeach(VERSION_STR ${VERSIONS})

# Set the version variables based on what was found above
set(VERSION_COMMA "${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH},${VERSION_BUILD}")
set(VERSION_DOTTED_NOBUILD "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(VERSION_DOTTED "${VERSION_DOTTED_NOBUILD}.${VERSION_BUILD}")
set(VERSION_FULL "${VERSION_DOTTED}${VERSION_EXTRA}")
set(VERSION_FULL_NOBUILD "${VERSION_DOTTED_NOBUILD}${VERSION_EXTRA}")

# Only do the following for Windows
if(WIN32)
  # Generate the win32.rc file using the above variables
  configure_file(${Anope_SOURCE_DIR}/src/win32.rc.cmake ${Anope_BINARY_DIR}/src/win32.rc)
endif(WIN32)

# Calculate dependencies for each header
# I would've done this inside the CMakeLists.txt for the include directory, but since it's added AFTER everything else, it won't help...

# Firstly, find all the header files
file(GLOB_RECURSE ALL_HEADERS "*.h")
# Iterate through the headers
foreach(HEADER ${ALL_HEADERS})
  # Don't process the file if it's in an obsolete directory
  if(NOT HEADER MATCHES ".*obsolete.*")
    append_to_list(TMP_HEADERS ${HEADER})
    # In addition, also set up a variable to store the fullpath of the header, in a variable prefixed with just the header's filename for easy access later
    get_filename_component(HEADER_FILENAME ${HEADER} NAME)
    set(${HEADER_FILENAME}_FULLPATH ${HEADER})
  endif(NOT HEADER MATCHES ".*obsolete.*")
endforeach(HEADER)
# Set the list of headers to be all the non-obsolete ones, then sort the list
set(ALL_HEADERS ${TMP_HEADERS})
if(CMAKE244_OR_BETTER)
  list(SORT ALL_HEADERS)
endif(CMAKE244_OR_BETTER)

# This function will take a #include line and extract the filename minus the quotes
macro(extract_include_filename INCLUDE FILENAME)
  # Detect if there is any trailing whitespace (basically see if the last character is a space or a tab)
  string(LENGTH ${INCLUDE} INCLUDE_LEN)
  math(EXPR LAST_CHAR_POS "${INCLUDE_LEN} - 1")
  string(SUBSTRING ${INCLUDE} ${LAST_CHAR_POS} 1 LAST_CHAR)
  # Only strip if the last character was a space or a tab
  if(LAST_CHAR STREQUAL " " OR LAST_CHAR STREQUAL "\t")
    # Strip away trailing whitespace from the line
    string(REGEX REPLACE "[ \t]*$" "" INCLUDE_STRIPPED ${INCLUDE})
  else(LAST_CHAR STREQUAL " " OR LAST_CHAR STREQUAL "\t")
    # Just copy INCLUDE to INCLUDE_STRIPPED so the below code doesn't complain about a lack of INCLUDE_STRIPPED
    set(INCLUDE_STRIPPED ${INCLUDE})
  endif(LAST_CHAR STREQUAL " " OR LAST_CHAR STREQUAL "\t")
  # Find the filename including the quotes, it should be at the end of the line after whitespace was stripped
  string(REGEX MATCH "\".*\"$" FILE ${INCLUDE_STRIPPED})
  # Get the length of the filename with quotes
  string(LENGTH ${FILE} FILENAME_LEN)
  # Subtract 2 from this length, for the quotes
  math(EXPR FILENAME_LEN "${FILENAME_LEN} - 2")
  # Overwrite the filename with a version sans quotes
  string(SUBSTRING ${FILE} 1 ${FILENAME_LEN} FILE)
  # Set the filename to the the given variable
  set(${FILENAME} "${FILE}")
endmacro(extract_include_filename)

# Preparse step 1: get filenames sans paths
# Iterate through the headers
foreach(HEADER ${ALL_HEADERS})
  # Find all the lines in the current header that have any form of #include on them, regardless of whitespace
  read_from_file(${HEADER} "^[ \t]*#[ \t]*include[ \t]*\".*\"[ \t]*$" INCLUDES)
  # Get the filename only of the header we just checked
  get_filename_component(HEADER_FILENAME ${HEADER} NAME)
  # Iterate through the strings containing #include (if any)
  foreach(INCLUDE ${INCLUDES})
    # Extract the filename from the #include line
    extract_include_filename(${INCLUDE} FILENAME)
    # Append this filename to the list of headers for the header we are checking
    append_to_list(${HEADER_FILENAME}_HEADERS ${FILENAME})
  endforeach(INCLUDE)
endforeach(HEADER)

# Preparse step 2: for every header from above that had includes, recursively find the headers each header relies on
# Iterate through the headers (again)
foreach(HEADER ${ALL_HEADERS})
  # Get the filename only of the current header
  get_filename_component(HEADER_FILENAME ${HEADER} NAME)
  # If there were any include, we'll be checking them
  if(${HEADER_FILENAME}_HEADERS)
    # Set the variables, old for all previously found headers, new for all newly found headers
    set(OLD_HEADERS)
    set(HEADERS ${${HEADER_FILENAME}_HEADERS})
    set(NEW_HEADERS)
    # Loop as long as there are still headers to be parsed
    while(HEADERS)
      # Iterate through the list of the current headers
      foreach(CURR_HEADER ${HEADERS})
        # If that header has headers it relies on, we'll add them to the list of new headers
        if(${CURR_HEADER}_HEADERS)
          foreach(CURR_HEADERS_HEADER ${${CURR_HEADER}_HEADERS})
            append_to_list(NEW_HEADERS ${CURR_HEADERS_HEADER})
          endforeach(CURR_HEADERS_HEADER)
        endif(${CURR_HEADER}_HEADERS)
      endforeach(CURR_HEADER)
      # Append the headers we checked to the old headers
      append_to_list(OLD_HEADERS ${HEADERS})
      # Set the headers to check to the new headers (it may be empty and that'll exit the loop)
      set(HEADERS ${NEW_HEADERS})
      # Erase the new headers
      set(NEW_HEADERS)
    endwhile(HEADERS)
    # OLD_HEADERS will now contain all headers that the current header relies on, remove duplicate headers from the list and sort the list
    remove_list_duplicates(OLD_HEADERS)
    if(CMAKE244_OR_BETTER)
      list(SORT OLD_HEADERS)
    endif(CMAKE244_OR_BETTER)
    # Set the current header's list of headers to the cleaned up list from above
    set(${HEADER_FILENAME}_HEADERS ${OLD_HEADERS})
  endif(${HEADER_FILENAME}_HEADERS)
endforeach(HEADER)

# The following headers are generated from CMake rules and won't be found with the above
append_to_list(ALL_HEADERS ${Anope_BINARY_DIR}/lang/language.h ${Anope_BINARY_DIR}/include/sysconf.h ${Anope_BINARY_DIR}/include/version.h)
set(language.h_FULLPATH ${Anope_BINARY_DIR}/lang/language.h)
set(sysconf.h_FULLPATH ${Anope_BINARY_DIR}/include/sysconf.h)
set(version.h_FULLPATH ${Anope_BINARY_DIR}/include/version.h)

# This function is used in most of the src (sub)directories to calculate the header file dependencies for the given source file
macro(calculate_depends SRC)
  # Find all the lines in the given source file that have any form of #include on them, regardless of whitespace
  read_from_file(${SRC} "^[ \t]*#[ \t]*include[ \t]*\".*\"[ \t]*$" INCLUDES)
  # Reset the list of headers to empty
  set(HEADERS)
  # Iterate through the strings containing #include (if any)
  foreach(INCLUDE ${INCLUDES})
    # Extract the filename from the #include line
    extract_include_filename(${INCLUDE} FILENAME)
    # Append the filename to the list of headers
    append_to_list(HEADERS ${FILENAME})
  endforeach(INCLUDE)
  # Set the list of new headers to empty (this will store all the headers that the above list depends on)
  set(NEW_HEADERS)
  # Iterate through the list of headers
  foreach(HEADER ${HEADERS})
    # If the current header has it's own headers to depend on, append those to the list of new headers
    if(${HEADER}_HEADERS)
      append_to_list(NEW_HEADERS ${${HEADER}_HEADERS})
    endif(${HEADER}_HEADERS)
  endforeach(HEADER)
  # If there were new headers, append them to the list of headers
  if(NEW_HEADERS)
    append_to_list(HEADERS ${NEW_HEADERS})
  endif(NEW_HEADERS)
  # If after all the above there is a list of header, we'll process them, converting them to full paths
  if(HEADERS)
    # Remove duplicate headers from the list and sort the list
    remove_list_duplicates(HEADERS)
    if(CMAKE244_OR_BETTER)
      list(SORT HEADERS)
    endif(CMAKE244_OR_BETTER)
    # Set the list of full path headers to empty
    set(HEADERS_FULL)
    # Iterate through the list of headers
    foreach(HEADER ${HEADERS})
      # Append the full path of the header to the full path headers list
      append_to_list(HEADERS_FULL ${${HEADER}_FULLPATH})
    endforeach(HEADER)
    # Set the given source file to depend on the headers given
    set_source_files_properties(${SRC} PROPERTIES OBJECT_DEPENDS "${HEADERS_FULL}")
  endif(HEADERS)
endmacro(calculate_depends)

# A macro to update the environment variable CPACK_IGNORED_FILES which contains a list of files for CPack to ignore
macro(add_to_cpack_ignored_files ITEM)
  # Temporary copy of the orignal item
  set(REAL_ITEM "${ITEM}")
  # If we have 2+ arguments, assume that the second one was something like TRUE (doesn't matter really) and convert periods so they will be \\. for CPack
  if(${ARGC} GREATER 1)
    string(REPLACE "." "\\\\." REAL_ITEM ${REAL_ITEM})
  endif(${ARGC} GREATER 1)
  # If the environment variable is already defined, just tack the item to the end
  if(DEFINED ENV{CPACK_IGNORED_FILES})
    set(ENV{CPACK_IGNORED_FILES} "$ENV{CPACK_IGNORED_FILES};${REAL_ITEM}")
  # Otherwise set the environment variable to the item
  else(DEFINED ENV{CPACK_IGNORED_FILES})
    set(ENV{CPACK_IGNORED_FILES} "${REAL_ITEM}")
  endif(DEFINED ENV{CPACK_IGNORED_FILES})
endmacro(add_to_cpack_ignored_files)

# Add the initial files to ignore which will be ignored regardless of if you are building in-source or out-of-source
add_to_cpack_ignored_files(".git\;config.cache\;.svn\;CMakeFiles" TRUE)
# If the two directories are the same, we are building in-source, thus we need to ignore more files from the build
if(${Anope_SOURCE_DIR} STREQUAL ${Anope_BINARY_DIR})
  # This variable is used in the sub-directories to determine if we are building in-source
  set(IN_SOURCE TRUE)
  # Add the files that need their periods converted
  add_to_cpack_ignored_files("Makefile\;cmake_install.cmake\;sysconf.h$\;CMakeCache.txt\;install_manifest.txt" TRUE)
  # Add the files we don't want the periods converted for
  add_to_cpack_ignored_files(".\\\\\\\\.so$;CPack.;anope-${VERSION_FULL_NOBUILD}-source\\\\\\\\..")
  # If using Visual Studio, add these files as well
  if(MSVC)
    add_to_cpack_ignored_files(".vcproj$\;.sln$\;.ncb$\;.suo$\;.dir$\;.ilk$\;.exp$\;.pdb$\;.lib$\;/debug$;/release$;/relwithdebinfo$;/minsizerel$" TRUE)
  endif(MSVC)
# Otherwise, the two directories are different and we are building out-of-source
else(${Anope_SOURCE_DIR} STREQUAL ${Anope_BINARY_DIR})
  set(IN_SOURCE FALSE)
endif(${Anope_SOURCE_DIR} STREQUAL ${Anope_BINARY_DIR})

# Go into the following directories and run their CMakeLists.txt as well
add_subdirectory(data)
add_subdirectory(docs)
add_subdirectory(lang)
add_subdirectory(src)
add_subdirectory(include)

# Get the filename of the Anope binary, to use later
get_target_property(SERVICES_BINARY ${PROGRAM_NAME} LOCATION)
get_filename_component(SERVICES_BINARY ${SERVICES_BINARY} NAME)

# At install time, create the following additional directories
install(CODE "file(MAKE_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/data/backups\")")
install(CODE "file(MAKE_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/data/logs\")")
install(CODE "file(MAKE_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/data/modules/runtime\")")
# On non-Windows platforms, if RUNGROUP is set, change the permissions of the below directories, as well as the group of the data directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\${CMAKE_INSTALL_PREFIX}/data/backups\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\${CMAKE_INSTALL_PREFIX}/data/logs\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"\${CMAKE_INSTALL_PREFIX}/data/modules/runtime\")")
  install(CODE "execute_process(COMMAND ${CHGRP} -R ${RUNGROUP} \"\${CMAKE_INSTALL_PREFIX}\")")
endif(NOT WIN32 AND RUNGROUP)
# On Windows platforms, install extra files
if(WIN32)
  install(FILES ${Anope_SOURCE_DIR}/anope.bat
    ${Anope_SOURCE_DIR}/Changes
    ${Anope_SOURCE_DIR}/Changes.conf
    ${Anope_SOURCE_DIR}/Changes.lang
    DESTINATION .
  )
endif(WIN32)

# Only process the CPack section if we have CPack
if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  # Override the module include path to include our directory, as we are using our own version of the NSIS template
  set(CMAKE_MODULE_PATH ${Anope_SOURCE_DIR})
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
    # Also for Windows, include installing the MSVCRT library
    include(InstallRequiredSystemLibraries)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "Anope")
    set(CPACK_PACKAGE_EXECUTABLES "")
    set(CPACK_NSIS_MENU_LINKS
      "${SERVICES_BINARY}" "Anope IRC Services"
      "anope.bat\\\" \\\"-debug -nofork" "Anope IRC Services (Debug and Window Logging)"
      "anope.bat\\\" \\\"-nofork" "Anope IRC Services (Window Logging)"
      "anope.bat\\\" \\\"-nothird" "Anope IRC Services (No Third Party Modules)"
      "http://www.anope.org" "Anope Web Site"
    )
    # The following doesn't work, but a bug report has been filed about it
    #set(CPACK_CREATE_DESKTOP_LINK_${SERVICES_BINARY} TRUE)
    set(CPACK_NSIS_MUI_ICON "${Anope_SOURCE_DIR}/src\\\\anope-icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${Anope_SOURCE_DIR}/src\\\\anope-icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${SERVICES_BINARY}")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://www.anope.org")
    set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  endif(WIN32)
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "anope-${VERSION_FULL_NOBUILD}-source")
  set(CPACK_SOURCE_IGNORE_FILES "$ENV{CPACK_IGNORED_FILES}")
  set(CPACK_MONOLITHIC_INSTALL TRUE)
  include(CPack)
endif(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
