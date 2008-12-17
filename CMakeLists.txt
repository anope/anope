# This usage of CMake requires at least version 2.6 (I don't know if it'll work with earlier versions)
cmake_minimum_required(VERSION 2.6)

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
include(CheckCXXCompilerFlag)
include(CheckLibraryExists)

# Add an optional variable for using run-cc.pl for building, Perl will be checked later regardless of this setting
option(USE_RUN_CC_PL "Use run-cc.pl for building" OFF)

# If using Visual Studio, set the C++ flags accordingly
if(MSVC)
  # Remove the default exception handling flags, also remove default warning level flag
  string(REPLACE "/EHsc " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/GX " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "/W3 " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # Set the compile flags to have warnings on the max setting (but disable a few annoying ones), exception handling turned on, the proper defines, and the include directories
  set(CXXFLAGS "${CXXFLAGS} /W4 /wd4251 /wd4706 /wd4800 /EHs /D_WIN32 /DMSVCPP /I\"${Anope_SOURCE_DIR}/include\" /I\"${Anope_BINARY_DIR}/include\" /I\"${Anope_BINARY_DIR}/lang\"")
  # Set the module-specific compile flags to include a define for module compiling
  set(MODULE_CXXFLAGS "${CXXFLAGS} /DMODULE_COMPILE")
# Otherwise, we're not using Visual Studio
else(MSVC)
  # Set the compile flags to have all warnings on (including shadowed variables) and the include directories
  set(CXXFLAGS "${CXXFLAGS} -Wall -Wshadow -I${Anope_SOURCE_DIR}/include -I${Anope_BINARY_DIR}/include -I${Anope_BINARY_DIR}/lang")
  # If on a *nix system, also set the compile flags to remove GNU extensions (favor ISO C++) as well as reject non-ISO C++ code, also remove all leading underscores in exported symbols
  if(UNIX)
    set(CXXFLAGS "${CXXFLAGS} -ansi -pedantic -fno-leading-underscore")
    # Set the module-specific compile flags to the same setting as the compile flags
    set(MODULE_CXXFLAGS "${CXXFLAGS}")
  # If we aren't on a *nix system, set the compile flags to include a define for Windows
  else(UNIX)
    set(CXXFLAGS "${CXXFLAGS} -D_WIN32")
    # Also, if we are building under MinGW, add another define for MinGW
    if(MINGW)
      set(CXXFLAGS "${CXXFLAGS} -DMINGW")
    endif(MINGW)
    # Set the module-specific compile flags to include a define for module compiling
    set(MODULE_CXXFLAGS "${CXXFLAGS} -DMODULE_COMPILE")
  endif(UNIX)
endif(MSVC)

# If CMake has found that the given system requires a special library for dl* calls, include it with the linker flags
if(CMAKE_DL_LIBS)
  set(LDFLAGS "${LDFLAGS} ${CMAKE_DL_LIBS}")
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
  check_cxx_compiler_flag(-pipe HAVE_PIPE_FLAG)
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
check_include_file(execinfo.h HAVE_BACKTRACE)
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

# Check for the existance of the following types
check_type_size(uint8_t UINT8_T)
check_type_size(u_int8_t U_INT8_T)
check_type_size(int16_t INT16_T)
check_type_size(uint16_t UINT16_T)
check_type_size(u_int16_t U_INT16_T)
check_type_size(int32_t INT32_T)
check_type_size(uint32_t UINT32_T)
check_type_size(u_int32_t U_INT32_T)

# Strip the leading and trailing spaces from the compile flags
if(CXXFLAGS)
  string(STRIP ${CXXFLAGS} CXXFLAGS)
endif(CXXFLAGS)
# Strip the leading and trailing spaces from the linker flags
if(LDFLAGS)
  string(STRIP ${LDFLAGS} LDFLAGS)
endif(LDFLAGS)

# Search for the following programs
find_program(GREP grep)
find_program(SH sh)
find_program(CHGRP chgrp)
find_program(CHMOD chmod)
find_program(PERL perl)

# If perl is included on the system and the user wants to use run-cc.pl, change the commands for compiling and linking
if(PERL AND USE_RUN_CC_PL)
  set(CMAKE_CXX_COMPILE_OBJECT "${PERL} ${Anope_SOURCE_DIR}/run-cc.pl ${CMAKE_CXX_COMPILE_OBJECT}")
  set(CMAKE_CXX_LINK_EXECUTABLE "${PERL} ${Anope_SOURCE_DIR}/run-cc.pl ${CMAKE_CXX_LINK_EXECUTABLE}")
  set(CMAKE_CXX_CREATE_SHARED_MODULE "${PERL} ${Anope_SOURCE_DIR}/run-cc.pl ${CMAKE_CXX_CREATE_SHARED_MODULE}")
endif(PERL AND USE_RUN_CC_PL)

# If a INSTDIR wasn't passed in to CMake, set the default to the services directory under the user's home directory
if(NOT INSTDIR)
  set(INSTDIR "$ENV{HOME}/services")
endif(NOT INSTDIR)

# Set Anope's data install location to be the data directory under the install directory
set(DATADIR "${INSTDIR}/data")

# Only process Anope's version on Windows, it's needed for win32.rc as well as version branding at link time
if(WIN32)
  # Find all lines in version.log that start with VERSION_
  file(STRINGS ${Anope_SOURCE_DIR}/version.log VERSIONS REGEX "^VERSION_")
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
  set(VERSION_DOTTED "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_BUILD}")
  set(VERSION_FULL "${VERSION_DOTTED}${VERSION_EXTRA}")

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
    list(APPEND TMP_HEADERS ${HEADER})
    # In addition, also set up a variable to store the fullpath of the header, in a variable prefixed with just the header's filename for easy access later
    get_filename_component(HEADER_FILENAME ${HEADER} NAME)
    set(${HEADER_FILENAME}_FULLPATH ${HEADER})
  endif(NOT HEADER MATCHES ".*obsolete.*")
endforeach(HEADER)
# Set the list of headers to be all the non-obsolete ones, then sort the list
set(ALL_HEADERS ${TMP_HEADERS})
list(SORT ALL_HEADERS)

# This function will take a #include line and extract the filename minus the quotes
function(extract_include_filename INCLUDE FILENAME)
  # Strip away leading and trailing whitespace from the line
  string(STRIP ${INCLUDE} INCLUDE_STRIPPED)
  # Find the filename including the quotes, it should be at the end of the line after whitespace was stripped
  string(REGEX MATCH "\".*\"$" FILE ${INCLUDE_STRIPPED})
  # Get the length of the filename with quotes
  string(LENGTH ${FILE} FILENAME_LEN)
  # Subtract 2 from this length, for the quotes
  math(EXPR FILENAME_LEN "${FILENAME_LEN} - 2")
  # Overwrite the filename with a version sans quotes
  string(SUBSTRING ${FILE} 1 ${FILENAME_LEN} FILE)
  # Set the filename to the the given variable
  set(${FILENAME} "${FILE}" PARENT_SCOPE)
endfunction(extract_include_filename)

# Preparse step 1: get filenames sans paths
# Iterate through the headers
foreach(HEADER ${ALL_HEADERS})
  # Find all the lines in the current header that have any form of #include on them, regardless of whitespace
  file(STRINGS ${HEADER} INCLUDES REGEX "^[ \t]*#[ \t]*include[ \t]*\".*\"[ \t]*$")
  # Get the filename only of the header we just checked
  get_filename_component(HEADER_FILENAME ${HEADER} NAME)
  # Iterate through the strings containing #include (if any)
  foreach(INCLUDE ${INCLUDES})
    # Extract the filename from the #include line
    extract_include_filename(${INCLUDE} FILENAME)
    # Append this filename to the list of headers for the header we are checking
    list(APPEND ${HEADER_FILENAME}_HEADERS ${FILENAME})
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
            list(APPEND NEW_HEADERS ${CURR_HEADERS_HEADER})
          endforeach(CURR_HEADERS_HEADER)
        endif(${CURR_HEADER}_HEADERS)
      endforeach(CURR_HEADER)
      # Append the headers we checked to the old headers
      list(APPEND OLD_HEADERS ${HEADERS})
      # Set the headers to check to the new headers (it may be empty and that'll exit the loop)
      set(HEADERS ${NEW_HEADERS})
      # Erase the new headers
      set(NEW_HEADERS)
    endwhile(HEADERS)
    # OLD_HEADERS will now contain all headers that the current header relies on, remove duplicate headers from the list and sort the list
    list(REMOVE_DUPLICATES OLD_HEADERS)
    list(SORT OLD_HEADERS)
    # Set the current header's list of headers to the cleaned up list from above
    set(${HEADER_FILENAME}_HEADERS ${OLD_HEADERS})
  endif(${HEADER_FILENAME}_HEADERS)
endforeach(HEADER)

# The following headers are generated from CMake rules and won't be found with the above
list(APPEND ALL_HEADERS ${Anope_BINARY_DIR}/lang/language.h ${Anope_BINARY_DIR}/include/sysconf.h ${Anope_BINARY_DIR}/include/version.h)
set(language.h_FULLPATH ${Anope_BINARY_DIR}/lang/language.h)
set(sysconf.h_FULLPATH ${Anope_BINARY_DIR}/include/sysconf.h)
set(version.h_FULLPATH ${Anope_BINARY_DIR}/include/version.h)

# This function is used in most of the src (sub)directories to calculate the header file dependencies for the given source file
function(calculate_depends SRC)
  # Find all the lines in the given source file that have any form of #include on them, regardless of whitespace
  file(STRINGS ${SRC} INCLUDES REGEX "^[ \t]*#[ \t]*include[ \t]*\".*\"[ \t]*$")
  # Reset the list of headers to empty
  set(HEADERS)
  # Iterate through the strings containing #include (if any)
  foreach(INCLUDE ${INCLUDES})
    # Extract the filename from the #include line
    extract_include_filename(${INCLUDE} FILENAME)
    # Append the filename to the list of headers
    list(APPEND HEADERS ${FILENAME})
  endforeach(INCLUDE)
  # Set the list of new headers to empty (this will store all the headers that the above list depends on)
  set(NEW_HEADERS)
  # Iterate through the list of headers
  foreach(HEADER ${HEADERS})
    # If the current header has it's own headers to depend on, append those to the list of new headers
    if(${HEADER}_HEADERS)
      list(APPEND NEW_HEADERS ${${HEADER}_HEADERS})
    endif(${HEADER}_HEADERS)
  endforeach(HEADER)
  # If there were new headers, append them to the list of headers
  if(NEW_HEADERS)
    list(APPEND HEADERS ${NEW_HEADERS})
  endif(NEW_HEADERS)
  # If after all the above there is a list of header, we'll process them, converting them to full paths
  if(HEADERS)
    # Remove duplicate headers from the list and sort the list
    list(REMOVE_DUPLICATES HEADERS)
    list(SORT HEADERS)
    # Set the list of full path headers to empty
    set(HEADERS_FULL)
    # Iterate through the list of headers
    foreach(HEADER ${HEADERS})
      # Append the full path of the header to the full path headers list
      list(APPEND HEADERS_FULL ${${HEADER}_FULLPATH})
    endforeach(HEADER)
    # Set the given source file to depend on the headers given
    set_source_files_properties(${SRC} PROPERTIES OBJECT_DEPENDS "${HEADERS_FULL}")
  endif(HEADERS)
endfunction(calculate_depends)

# Go into the following directories and run their CMakeLists.txt as well
add_subdirectory(data)
add_subdirectory(lang)
add_subdirectory(src)
add_subdirectory(include)

# At install time, create the following additional directories
install(CODE "file(MAKE_DIRECTORY \"${DATADIR}/backups\")")
install(CODE "file(MAKE_DIRECTORY \"${DATADIR}/logs\")")
install(CODE "file(MAKE_DIRECTORY \"${DATADIR}/modules/runtime\")")
# On non-Windows platforms, if RUNGROUP is set, change the permissions of the below directories, as well as the group of the data directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"${DATADIR}/backups\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"${DATADIR}/logs\")")
  install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"${DATADIR}/modules/runtime\")")
  install(CODE "execute_process(COMMAND ${CHGRP} -R ${RUNGROUP} \"${DATADIR}\")")
endif(NOT WIN32 AND RUNGROUP)
