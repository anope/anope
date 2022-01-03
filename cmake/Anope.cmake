###############################################################################
# extract_include_filename(<line> <output variable> [<optional output variable of quote type>])
#
# This macro will take a #include line and extract the filename.
###############################################################################
macro(extract_include_filename INCLUDE FILENAME)
  # Strip the leading and trailing spaces from the include line
  string(STRIP ${INCLUDE} INCLUDE_STRIPPED)
  # Make sure to only do the following if there is a string
  if(INCLUDE_STRIPPED STREQUAL "")
    set(FILE "")
  else()
    # Extract the filename including the quotes or angle brackets
    string(REGEX REPLACE "^.*([\"<].*[\">]).*$" "\\1" FILE "${INCLUDE_STRIPPED}")
    # If an optional 3rd argument is given, we'll store if the quote style was quoted or angle bracketed
    if(${ARGC} GREATER 2)
      string(SUBSTRING ${FILE} 0 1 QUOTE)
      if(QUOTE STREQUAL "<")
        set(${ARGV2} "angle brackets")
      else()
        set(${ARGV2} "quotes")
      endif()
    endif()
    # Now remove the quotes or angle brackets
    string(REGEX REPLACE "^[\"<](.*)[\">]$" "\\1" FILE "${FILE}")
  endif()
  # Set the filename to the the given variable
  set(${FILENAME} "${FILE}")
endmacro()

###############################################################################
# find_includes(<source filename> <output variable>)
#
# This macro will search through a file for #include lines, regardless of
#   whitespace, but only returns the lines that are valid for the current
#   platform CMake is running on.
###############################################################################
macro(find_includes SRC INCLUDES)
  # Read all lines from the file that start with #, regardless of whitespace before the #
  file(STRINGS ${SRC} LINES REGEX "^[ \t]*#.*$")
  # Set that any #include lines found are valid, and create temporary variables for the last found #ifdef/#ifndef
  set(VALID_LINE TRUE)
  set(LAST_DEF)
  set(LAST_CHECK)
  # Create an empty include list
  set(INCLUDES_LIST)
  # Iterate through all the # lines
  foreach(LINE ${LINES})
    # Search for #ifdef, #ifndef, #else, #endif, and #include
    string(REGEX MATCH "^[ \t]*#[ \t]*ifdef[ \t]*.*$" FOUND_IFDEF ${LINE})
    string(REGEX MATCH "^[ \t]*#[ \t]*ifndef[ \t]*.*$" FOUND_IFNDEF ${LINE})
    string(REGEX MATCH "^[ \t]*#[ \t]*else.*$" FOUND_ELSE ${LINE})
    string(REGEX MATCH "^[ \t]*#[ \t]*endif.*$" FOUND_ENDIF ${LINE})
    string(REGEX MATCH "^[ \t]*#[ \t]*include[ \t]*[\"<].*[\">][\ t]*.*$" FOUND_INCLUDE ${LINE})
    # If we found a #ifdef on the line, extract the data after the #ifdef and set if the lines after it are valid based on the variables in CMake
    if(FOUND_IFDEF)
      # Extract the define
      string(REGEX REPLACE "^[ \t]*#[ \t]*ifdef[ \t]*(.*)$" "\\1" DEFINE ${LINE})
      # Replace _WIN32 with WIN32, so we can check if the WIN32 variable of CMake is set instead of _WIN32
      if(DEFINE STREQUAL "_WIN32")
        set(DEFINE WIN32)
      endif()
      # Set the last define to this one, and set the last check to true, so when #else is encountered, we can do an opposing check
      set(LAST_DEF ${DEFINE})
      set(LAST_CHECK TRUE)
      # If the define is true (it either exists or is a non-false result), the lines following will be checked, otherwise they will be skipped
      if(${DEFINE})
        set(VALID_LINE TRUE)
      else()
        set(VALID_LINE FALSE)
      endif()
    else()
      # If we found a #ifndef on the line, the same thing as #ifdef is done, except with the checks in the opposite direction
      if(FOUND_IFNDEF)
        # Extract the define
        string(REGEX REPLACE "^[ \t]*#[ \t]*ifndef[ \t]*(.*)$" "\\1" DEFINE ${LINE})
        # Replace _WIN32 with WIN32, so we can check if the WIN32 variable of CMake is set instead of _WIN32
        if(DEFINE STREQUAL "_WIN32")
          set(DEFINE WIN32)
        endif()
        # Set the last define to this one, and set the last check to false, so when #else is encountered, we can do an opposing check
        set(LAST_DEF ${DEFINE})
        set(LAST_CHECK FALSE)
        # If the define is not true (it either doesn't exists or is a false result), the lines following will be checked, otherwise they will be skipped
        if(${DEFINE})
          set(VALID_LINE FALSE)
        else()
          set(VALUE_LINE TRUE)
        endif()
      else()
        # If we found a #else on the line, we check the last define in the opposite direction
        if(FOUND_ELSE)
          # When LAST_CHECK is true, we were inside a #ifdef, now act as if we are entering a #ifndef section by doing an opposing check
          if(LAST_CHECK)
            if(${LAST_DEF})
              set(VALID_LINE FALSE)
            else()
              set(VALID_LINE TRUE)
            endif()
          # When LAST_CHECK is false, we were inside a #ifndef, now act as if we are entering a #ifdef section by doing an opposing check
          else()
            if(${LAST_DEF})
              set(VALID_LINE TRUE)
            else()
              set(VALID_LINE FALSE)
            endif()
          endif()
        else()
          # If we found a #endif on the line, we'll assume everything following the line is valid until we meet another one of the above lines
          if(FOUND_ENDIF)
            set(VALID_LINE TRUE)
          else()
            # If we found a #include on the line, add the entire line to the list of includes unless the line isn't valid
            if(FOUND_INCLUDE)
              if(VALID_LINE)
                list(APPEND INCLUDES_LIST "${LINE}")
              endif()
            endif()
          endif()
        endif()
      endif()
    endif()
  endforeach()
  set(${INCLUDES} ${INCLUDES_LIST})
endmacro()

###############################################################################
# calculate_depends(<source filename> [<optional output variable for includes>])
#
# This macro is used in most of the src (sub)directories to calculate the
#   header file dependencies for the given source file.
###############################################################################
macro(calculate_depends SRC)
  # Temporarily set that we didn't get a 3rd argument before we actually check if we did get one or not
  set(CHECK_ANGLE_INCLUDES FALSE)
  # Check for a third argument
  if(${ARGC} GREATER 1)
    set(CHECK_ANGLE_INCLUDES TRUE)
  endif()
  # Find all the lines in the given source file that have any form of #include on them, regardless of whitespace, but only if they are valid for the platform we are on
  find_includes(${SRC} INCLUDES)
  # Reset the list of headers to empty
  set(HEADERS)
  # Iterate through the strings containing #include (if any)
  foreach(INCLUDE ${INCLUDES})
    # Extract the filename from the #include line
    extract_include_filename(${INCLUDE} FILENAME QUOTE_TYPE)
    if(QUOTE_TYPE STREQUAL "angle brackets")
      # The following checks will only be done if there was a request for angle includes to be checked
      if(CHECK_ANGLE_INCLUDES)
        # Find the path of the include file
        if(DEFAULT_INCLUDE_DIRS OR WSDK_PATH OR DEFINED $ENV{VCINSTALLDIR})
          find_path(FOUND_${FILENAME}_INCLUDE NAMES ${FILENAME} PATHS ${DEFAULT_INCLUDE_DIRS} ${WSDK_PATH}/include $ENV{VCINSTALLDIR}/include ${EXTRA_INCLUDE})
        else()
          find_path(FOUND_${FILENAME}_INCLUDE NAMES ${FILENAME} ${EXTRA_INCLUDE})
        endif()
        # If the include file was found, add it's path to the list of include paths, but only if it doesn't already exist and isn't in the defaults for the compiler
        if(FOUND_${FILENAME}_INCLUDE)
          # This used to be list(FIND), but it was changed to this loop to do a find on each default include directory, this fixes Mac OS X trying to get it's framework directories in here
          set(FOUND_IN_DEFAULTS -1)
          foreach(DEFAULT_INCLUDE_DIR ${DEFAULT_INCLUDE_DIRS})
            string(REGEX REPLACE "\\+" "\\\\+" DEFAULT_INCLUDE_DIR ${DEFAULT_INCLUDE_DIR})
            string(REGEX MATCH ${DEFAULT_INCLUDE_DIR} FOUND_DEFAULT ${FOUND_${FILENAME}_INCLUDE})
            if(FOUND_DEFAULT)
              set(FOUND_IN_DEFAULTS 0)
            endif()
          endforeach()
          if(FOUND_IN_DEFAULTS EQUAL -1)
            if("${FOUND_${FILENAME}_INCLUDE}" IN_LIST ARGV1)
              list(APPEND ${ARGV1} "${FOUND_${FILENAME}_INCLUDE}")
            endif()
          endif()
        else()
          # XXX
          if(NOT ${FILENAME} STREQUAL "libintl.h")
            message(FATAL_ERROR "${SRC} needs header file ${FILENAME} but we were unable to locate that header file! Check that the header file is within the search path of your OS.")
          endif()
        endif()
      endif()
    endif()
  endforeach()
endmacro()

###############################################################################
# calculate_libraries(<source filename> <output variable for linker flags> <output variable for extra depends>)
#
# This macro is used in most of the module (sub)directories to calculate the
#   library dependencies for the given source file.
###############################################################################
macro(calculate_libraries SRC SRC_LDFLAGS EXTRA_DEPENDS)
  # Set up a temporary LDFLAGS for this file
  set(THIS_LDFLAGS "${LDFLAGS}")
  # Reset extra dependencies
  set(EXTRA_DEPENDENCIES)
  # Reset library paths
  set(LIBRARY_PATHS)
  # Reset libraries
  set(LIBRARIES)
  # Check to see if there are any lines matching: /* RequiredLibraries: [something] */
  if(WIN32)
    file(STRINGS ${SRC} REQUIRED_LIBRARIES REGEX "/\\*[ \t]*RequiredWindowsLibraries:[ \t]*.*[ \t]*\\*/")
  else()
    file(STRINGS ${SRC} REQUIRED_LIBRARIES REGEX "/\\*[ \t]*RequiredLibraries:[ \t]*.*[ \t]*\\*/")
  endif()
  # Iterate through those lines
  foreach(REQUIRED_LIBRARY ${REQUIRED_LIBRARIES})
    # Strip off the /* RequiredLibraries: and */ from the line
    string(REGEX REPLACE "/\\*[ \t]*Required.*Libraries:[ \t]*([^ \t]*)[ \t]*\\*/" "\\1" REQUIRED_LIBRARY ${REQUIRED_LIBRARY})
    # Replace all commas with semicolons
    string(REGEX REPLACE "," ";" REQUIRED_LIBRARY ${REQUIRED_LIBRARY})
    # Iterate through the libraries given
    foreach(LIBRARY ${REQUIRED_LIBRARY})
      # If the library has multiple names extract the alternate.
      unset(LIBRARY_ALT)
      if (${LIBRARY} MATCHES "^.+\\|.+$")
        string(REGEX REPLACE ".+\\|(.*)" "\\1" LIBRARY_ALT ${LIBRARY})
        string(REGEX REPLACE "(.+)\\|.*" "\\1" LIBRARY ${LIBRARY})
      endif()
      # Locate the library to see if it exists
      if(DEFAULT_LIBRARY_DIRS OR WSDK_PATH OR DEFINED $ENV{VCINSTALLDIR})
        find_library(FOUND_${LIBRARY}_LIBRARY NAMES ${LIBRARY} ${LIBRARY_ALT} PATHS ${DEFAULT_LIBRARY_DIRS} ${WSDK_PATH}/lib $ENV{VCINSTALLDIR}/lib ${EXTRA_INCLUDE} ${EXTRA_LIBS})
      else()
        find_library(FOUND_${LIBRARY}_LIBRARY NAMES ${LIBRARY} ${LIBRARY_ALT} PATHS ${EXTRA_INCLUDE} ${EXTRA_LIBS} NO_DEFAULT_PATH)
        find_library(FOUND_${LIBRARY}_LIBRARY NAMES ${LIBRARY} ${LIBRARY_ALT} PATHS ${EXTRA_INCLUDE} ${EXTRA_LIBS})
      endif()
      # If the library was found, we will add it to the linker flags
      if(FOUND_${LIBRARY}_LIBRARY)
        if(MSVC)
          # For Visual Studio, instead of editing the linker flags, we'll add the library to a separate list of extra dependencies
          list(APPEND EXTRA_DEPENDENCIES "${FOUND_${LIBRARY}_LIBRARY}")
        else()
          # Get the path only of the library, to add it to library paths.
          get_filename_component(LIBRARY_PATH ${FOUND_${LIBRARY}_LIBRARY} PATH)
          list(APPEND LIBRARY_PATHS "${LIBRARY_PATH}")
          # Extract the library short name, add it to the library path
          get_filename_component(LIBRARY_NAME ${FOUND_${LIBRARY}_LIBRARY} NAME_WE)
          string(REGEX REPLACE "^lib" "" LIBRARY_NAME ${LIBRARY_NAME})
          list(APPEND LIBRARIES ${LIBRARY_NAME})
        endif()
      else()
        # In the case of the library not being found, we fatally error so CMake stops trying to generate
        message(FATAL_ERROR "${SRC} needs library ${LIBRARY} but we were unable to locate that library! Check that the library is within the search path of your OS.")
      endif()
    endforeach()
  endforeach()
  # Remove duplicates from the library paths
  if(LIBRARY_PATHS)
    list(REMOVE_DUPLICATES LIBRARY_PATHS)
  endif()
  # Remove diplicates from the libraries
  if(LIBRARIES)
    list(REMOVE_DUPLICATES LIBRARIES)
  endif()
  # Iterate through library paths and add them to the linker flags
  foreach(LIBRARY_PATH ${LIBRARY_PATHS})
    if(NOT "${LIBRARY_PATH}" IN_LIST DEFAULT_LIBRARY_DIRS)
      set(THIS_LDFLAGS "${THIS_LDFLAGS} -L${LIBRARY_PATH}")
    endif()
  endforeach()
  # Iterate through libraries and add them to the linker flags
  foreach(LIBRARY ${LIBRARIES})
    list(APPEND EXTRA_DEPENDENCIES "${LIBRARY}")
  endforeach()
  set(${SRC_LDFLAGS} "${THIS_LDFLAGS}")
  set(${EXTRA_DEPENDS} "${EXTRA_DEPENDENCIES}")
endmacro()

###############################################################################
# check_functions(<source filename> <output variable set to TRUE on success>)
#
# This macro is used in most of the module (sub)directories to calculate the
#   function dependencies for the given source file.
###############################################################################
macro(check_functions SRC SUCCESS)
  # Default to true
  set(${SUCCESS} TRUE)
  # Check to see if there are any lines matching: /* RequiredFunctions: [something] */
  file(STRINGS ${SRC} REQUIRED_FUNCTIONS REGEX "/\\*[ \t]*RequiredFunctions:[ \t]*.*[ \t]*\\*/")
  # Iterate through those lines
  foreach(REQUIRED_FUNCTION ${REQUIRED_FUNCTIONS})
    # Strip off the /* RequiredFunctions: and */ from the line
    string(REGEX REPLACE "/\\*[ \t]*RequiredFunctions:[ \t]*([^ \t]*)[ \t]*\\*/" "\\1" REQUIRED_FUNCTION ${REQUIRED_FUNCTION})
    # Replace all commas with semicolons
    string(REGEX REPLACE "," ";" REQUIRED_FUNCTION ${REQUIRED_FUNCTION})
    # Iterate through the functions given
    foreach(FUNCTION ${REQUIRED_FUNCTION})
      # Check if the function exists
      check_function_exists(${REQUIRED_FUNCTION} HAVE_${REQUIRED_FUNCTION})
      # If we don't have the function warn the user and set SUCCESS to FALSE
      if(NOT HAVE_${REQUIRED_FUNCTION})
        message("${SRC} needs function ${REQUIRED_FUNCTION} but we were unable to locate that function!")
        set(${SUCCESS} FALSE)
      endif()
    endforeach()
  endforeach()
endmacro()

###############################################################################
# add_to_cpack_ignored_files(<item> [TRUE])
#
# A macro to update the environment variable CPACK_IGNORED_FILES which
#   contains a list of files for CPack to ignore. If the optional 2nd argument
#   of TRUE is given, periods will be converted to \\. for CPack.
###############################################################################
macro(add_to_cpack_ignored_files ITEM)
  # Temporary copy of the orignal item
  set(REAL_ITEM "${ITEM}")
  # If we have 2+ arguments, assume that the second one was something like TRUE (doesn't matter really) and convert periods so they will be \\. for CPack
  if(${ARGC} GREATER 1)
    string(REPLACE "." "\\\\." REAL_ITEM ${REAL_ITEM})
  endif()
  # If the environment variable is already defined, just tack the item to the end
  if(DEFINED ENV{CPACK_IGNORED_FILES})
    set(ENV{CPACK_IGNORED_FILES} "$ENV{CPACK_IGNORED_FILES};${REAL_ITEM}")
  # Otherwise set the environment variable to the item
  else()
    set(ENV{CPACK_IGNORED_FILES} "${REAL_ITEM}")
  endif()
endmacro()
