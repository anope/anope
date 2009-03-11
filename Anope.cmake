###############################################################################
# strip_string(<input string> <output string>)
#
# A macro to handle stripping the leading and trailing spaces from a string,
#   uses string(STRIP) if using CMake 2.6.x or better, otherwise uses
#   string(REGEX REPLACE).
###############################################################################
macro(strip_string INPUT_STRING OUTPUT_STRING)
  if(CMAKE26_OR_BETTER)
    # For CMake 2.6.x or better, we can just use the STRIP sub-command of string()
    string(STRIP ${INPUT_STRING} ${OUTPUT_STRING})
  else(CMAKE26_OR_BETTER)
    # For CMake 2.4.x, we will have to use the REGEX REPLACE sub-command of string() instead
    # First check if the input string is empty or not
    if (${INPUT_STRING} STREQUAL "")
      set(${OUTPUT_STRING} "")
    else(${INPUT_STRING} STREQUAL "")
      # Determine if the string is entirely empty or not
      string(REGEX MATCH "^[ \t]*$" EMPTY_STRING "${INPUT_STRING}")
      if(EMPTY_STRING)
        set(${OUTPUT_STRING} "")
      else(EMPTY_STRING)
        # We detect if there is any leading whitespace and remove any if there is
        string(SUBSTRING "${INPUT_STRING}" 0 1 FIRST_CHAR)
        if(FIRST_CHAR STREQUAL " " OR FIRST_CHAR STREQUAL "\t")
          string(REGEX REPLACE "^[ \t]+" "" TEMP_STRING "${INPUT_STRING}")
        else(FIRST_CHAR STREQUAL " " OR FIRST_CHAR STREQUAL "\t")
          set(TEMP_STRING "${INPUT_STRING}")
        endif(FIRST_CHAR STREQUAL " " OR FIRST_CHAR STREQUAL "\t")
        # Next we detect if there is any trailing whitespace and remove any if there is
        string(LENGTH "${TEMP_STRING}" STRING_LEN)
        math(EXPR STRING_LEN "${STRING_LEN} - 1")
        string(SUBSTRING "${TEMP_STRING}" ${STRING_LEN} 1 LAST_CHAR)
        if(LAST_CHAR STREQUAL " " OR LAST_CHAR STREQUAL "\t")
          string(REGEX REPLACE "[ \t]+$" "" ${OUTPUT_STRING} "${TEMP_STRING}")
        else(LAST_CHAR STREQUAL " " OR LAST_CHAR STREQUAL "\t")
          set(${OUTPUT_STRING} "${TEMP_STRING}")
        endif(LAST_CHAR STREQUAL " " OR LAST_CHAR STREQUAL "\t")
      endif(EMPTY_STRING)
    endif(${INPUT_STRING} STREQUAL "")
  endif(CMAKE26_OR_BETTER)
endmacro(strip_string)

###############################################################################
# append_to_list(<list> <args>...)
#
# A macro to handle appending to lists, uses list(APPEND) if using CMake 2.4.2
#   or better, otherwise uses set() instead.
###############################################################################
macro(append_to_list LIST)
  if(CMAKE242_OR_BETTER)
    # For CMake 2.4.2 or better, we can just use the APPEND sub-command of list()
    list(APPEND ${LIST} ${ARGN})
  else(CMAKE242_OR_BETTER)
    # For CMake 2.4.x before 2.4.2, we have to do this manually use set() instead
    set(${LIST} ${${LIST}} ${ARGN})
  endif(CMAKE242_OR_BETTER)
endmacro(append_to_list)

###############################################################################
# find_in_list(<list> <value> <output variable>)
#
# A macro to handle searching within a list, will store the result in the
#   given <output variable>, uses list(FIND) if using CMake 2.6.x or better
#   (or CMake 2.4.8 or better), otherwise it iterates through the list to find
#   the item.
###############################################################################
macro(find_in_list LIST ITEM_TO_FIND FOUND)
  if(CMAKE26_OR_BETTER OR ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.7)
    # For CMake 2.6.x or better (as well as CMake 2.4.8 or better), we can use the FIND sub-command of list()
    list(FIND ${LIST} ${ITEM_TO_FIND} ITEM_FOUND)
  else(CMAKE26_OR_BETTER OR ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.7)
    # For CMake 2.4.x before 2.4.8, we have to do this ourselves (NOTE: This is very slow due to a lack of break() as well)
    # Firstly we set the position to -1 indicating nothing found, we also use a temporary position
    set(ITEM_FOUND -1)
    set(POS 0)
    # Iterate through the list
    foreach(ITEM ${${LIST}})
      # If the item we are looking at is the item we are trying to find, set that we've found the item
      if(${ITEM} STREQUAL ${ITEM_TO_FIND})
        set(ITEM_FOUND ${POS})
      endif(${ITEM} STREQUAL ${ITEM_TO_FIND})
      # Increase the position value by 1
      math(EXPR POS "${POS} + 1")
    endforeach(ITEM)
  endif(CMAKE26_OR_BETTER OR ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} GREATER 2.4.7)
  # Set the given FOUND variable to the result
  set(${FOUND} ${ITEM_FOUND})
endmacro(find_in_list)

###############################################################################
# remove_list_duplicates(<list>)
#
# A macro to handle removing duplicates from a list, uses
#   list(REMOVE_DUPLICATES) if using CMake 2.6.x or better, otherwise it uses
#   a slower method of creating a temporary list and only adding to it when
#   a duplicate item hasn't been found.
###############################################################################
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
    # Replace the old list with the new list
    set(${LIST} ${NEW_LIST})
  endif(CMAKE26_OR_BETTER)
endmacro(remove_list_duplicates)

###############################################################################
# remove_item_from_list(<list> <value>)
#
# A macro to handle removing a value from a list, uses list(REMOVE_ITEM) if
#   using CMake 2.4.2 or better, otherwise it uses a slower method of creating
#   a temporary list and adding every item except the one given.
###############################################################################
macro(remove_item_from_list LIST VALUE)
  if(CMAKE242_OR_BETTER)
    # For CMake 2.4.2 or better, this can be done automatically
    list(REMOVE_ITEM ${LIST} ${VALUE})
  else(CMAKE242_OR_BETTER)
    # For CMake 2.4.x before 2.4.2, we have to do this ourselves, firstly we'll create a temporary list
    set(NEW_LIST)
    # Iterate through the old list
    foreach(ITEM ${${LIST}})
      # Check if the current item is the same as the item we are removing, and if it isn't, append it to the list
      if(NOT ITEM STREQUAL ${VALUE})
        append_to_list(NEW_LIST ${ITEM})
      endif(NOT ITEM STREQUAL ${VALUE})
    endforeach(ITEM)
    # Replace the old list with the new list
    set(${LIST} ${NEW_LIST})
  endif(CMAKE242_OR_BETTER)
endmacro(remove_item_from_list)

###############################################################################
# sort_list(<list>)
#
# A macro to handle sorting a list, uses list(SORT) if using CMake 2.4.4 or
#   better, otherwise it uses a slower method of creating a temporary list and
#   adding elements in alphabetical order.
###############################################################################
macro(sort_list LIST)
  if(CMAKE244_OR_BETTER)
    # For CMake 2.4.4 or better, this can be done automatically
    list(SORT ${LIST})
  else(CMAKE244_OR_BETTER)
    # For CMake 2.4.x before 2.4.4, we have to do this ourselves, firstly we'll create a teporary list
    set(NEW_LIST)
    # Iterate through the old list
    foreach(ITEM ${${LIST}})
      # Temporary index position for the new list, as well as temporary value to store if the item was ever found
      set(INDEX 0)
      set(FOUND FALSE)
      # Iterate through the new list
      foreach(NEW_ITEM ${NEW_LIST})
        # Compare the items, only if nothing was found before
        if(NOT FOUND)
          if(NEW_ITEM STRGREATER ${ITEM})
            set(FOUND TRUE)
            list(INSERT NEW_LIST ${INDEX} ${ITEM})
          endif(NEW_ITEM STRGREATER ${ITEM})
        endif(NOT FOUND)
        math(EXPR INDEX "${INDEX} + 1")
      endforeach(NEW_ITEM)
      # If the item was never found, just append it to the end
      if(NOT FOUND)
        append_to_list(NEW_LIST ${ITEM})
      endif(NOT FOUND)
    endforeach(ITEM)
    # Replace the old list with the new list
    set(${LIST} ${NEW_LIST})
  endif(CMAKE244_OR_BETTER)
endmacro(sort_list)

###############################################################################
# read_from_file(<filename> <regex> <output variable>)
#
# A macro to handle reading specific lines from a file, uses file(STRINGS) if
#   using CMake 2.6.x or better, otherwise we read in the entire file and
#   perform a string(REGEX MATCH) on each line of the file instead.  This
#   macro can also be used to read in all the lines of a file if REGEX is set
#   to "".
###############################################################################
macro(read_from_file FILE REGEX STRINGS)
  if(CMAKE26_OR_BETTER)
    # For CMake 2.6.x or better, we can just use the STRINGS sub-command to get the lines that match the given regular expression (if one is given, otherwise get all lines)
    if(REGEX STREQUAL "")
      file(STRINGS ${FILE} RESULT)
    else(REGEX STREQUAL "")
      file(STRINGS ${FILE} RESULT REGEX ${REGEX})
    endif(REGEX STREQUAL "")
  else(CMAKE26_OR_BETTER)
    # For CMake 2.4.x, we need to do this manually, firstly we read the file in
    execute_process(COMMAND ${CMAKE_COMMAND} -DFILE:STRING=${FILE} -P ${Anope_SOURCE_DIR}/ReadFile.cmake ERROR_VARIABLE ALL_STRINGS)
    # Next we replace all newlines with semicolons
    string(REGEX REPLACE "\n" ";" ALL_STRINGS ${ALL_STRINGS})
    if(REGEX STREQUAL "")
      # For no regular expression, just set the result to all the lines
      set(RESULT ${ALL_STRINGS})
    else(REGEX STREQUAL "")
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
    endif(REGEX STREQUAL "")
  endif(CMAKE26_OR_BETTER)
  # Set the given STRINGS variable to the result
  set(${STRINGS} ${RESULT})
endmacro(read_from_file)

###############################################################################
# extract_include_filename(<line> <output variable> [<optional output variable of quote type>])
#
# This macro will take a #include line and extract the filename.
###############################################################################
macro(extract_include_filename INCLUDE FILENAME)
  # Strip the leading and trailing spaces from the include line
  strip_string(${INCLUDE} INCLUDE_STRIPPED)
  # Make sure to only do the following if there is a string
  if(INCLUDE_STRIPPED STREQUAL "")
    set(FILE "")
  else(INCLUDE_STRIPPED STREQUAL "")
    # Extract the filename including the quotes or angle brackets
    string(REGEX REPLACE "^.*([\"<].*[\">]).*$" "\\1" FILE "${INCLUDE_STRIPPED}")
    # If an optional 3rd argument is given, we'll store if the quote style was quoted or angle bracketed
    if(${ARGC} GREATER 2)
      string(SUBSTRING ${FILE} 0 1 QUOTE)
      if(QUOTE STREQUAL "<")
        set(${ARGV2} "angle brackets")
      else(QUOTE STREQUAL "<")
        set(${ARGV2} "quotes")
      endif(QUOTE STREQUAL "<")
    endif(${ARGC} GREATER 2)
    # Now remove the quotes or angle brackets
    string(REGEX REPLACE "^[\"<](.*)[\">]$" "\\1" FILE "${FILE}")
  endif(INCLUDE_STRIPPED STREQUAL "")
  # Set the filename to the the given variable
  set(${FILENAME} "${FILE}")
endmacro(extract_include_filename)

###############################################################################
# find_includes(<source filename> <output variable>)
#
# This macro will search through a file for #include lines, regardless of
#   whitespace, but only returns the lines that are valid for the current
#   platform CMake is running on.
###############################################################################
macro(find_includes SRC INCLUDES)
  # Read all lines from the file that start with #, regardless of whitespace before the #
  read_from_file(${SRC} "^[ \t]*#.*$" LINES)
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
      endif(DEFINE STREQUAL "_WIN32")
      # Set the last define to this one, and set the last check to true, so when #else is encountered, we can do an opposing check
      set(LAST_DEF ${DEFINE})
      set(LAST_CHECK TRUE)
      # If the define is true (it either exists or is a non-false result), the lines following will be checked, otherwise they will be skipped
      if(${DEFINE})
        set(VALID_LINE TRUE)
      else(${DEFINE})
        set(VALID_LINE FALSE)
      endif(${DEFINE})
    else(FOUND_IFDEF)
      # If we found a #ifndef on the line, the same thing as #ifdef is done, except with the checks in the opposite direction
      if(FOUND_IFNDEF)
        # Extract the define
        string(REGEX REPLACE "^[ \t]*#[ \t]*ifndef[ \t]*(.*)$" "\\1" DEFINE ${LINE})
        # Replace _WIN32 with WIN32, so we can check if the WIN32 variable of CMake is set instead of _WIN32
        if(DEFINE STREQUAL "_WIN32")
          set(DEFINE WIN32)
        endif(DEFINE STREQUAL "_WIN32")
        # Set the last define to this one, and set the last check to false, so when #else is encountered, we can do an opposing check
        set(LAST_DEF ${DEFINE})
        set(LAST_CHECK FALSE)
        # If the define is not true (it either doesn't exists or is a false result), the lines following will be checked, otherwise they will be skipped
        if(NOT ${DEFINE})
          set(VALID_LINE TRUE)
        else(NOT ${DEFINE})
          set(VALUE_LINE FALSE)
        endif(NOT ${DEFINE})
      else(FOUND_IFNDEF)
        # If we found a #else on the line, we check the last define in the opposite direction
        if(FOUND_ELSE)
          # When LAST_CHECK is true, we were inside a #ifdef, now act as if we are entering a #ifndef section by doing an opposing check
          if(LAST_CHECK)
            if(NOT ${LAST_DEF})
              set(VALID_LINE TRUE)
            else(NOT ${LAST_DEF})
              set(VALID_LINE FALSE)
            endif(NOT ${LAST_DEF})
          # When LAST_CHECK is false, we were inside a #ifndef, now act as if we are entering a #ifdef section by doing an opposing check
          else(LAST_CHECK)
            if(${LAST_DEF})
              set(VALID_LINE TRUE)
            else(${LAST_DEF})
              set(VALID_LINE FALSE)
            endif(${LAST_DEF})
          endif(LAST_CHECK)
        else(FOUND_ELSE)
          # If we found a #endif on the line, we'll assume everything following the line is valid until we meet another one of the above lines
          if(FOUND_ENDIF)
            set(VALID_LINE TRUE)
          else(FOUND_ENDIF)
            # If we found a #include on the line, add the entire line to the list of includes unless the line isn't valid
            if(FOUND_INCLUDE)
              if(VALID_LINE)
                append_to_list(INCLUDES_LIST "${LINE}")
              endif(VALID_LINE)
            endif(FOUND_INCLUDE)
          endif(FOUND_ENDIF)
        endif(FOUND_ELSE)
      endif(FOUND_IFNDEF)
    endif(FOUND_IFDEF)
  endforeach(LINE)
  set(${INCLUDES} ${INCLUDES_LIST})
endmacro(find_includes)

###############################################################################
# calculate_depends(<source filename> [<optional output variable for includes>])
#
# This macro is used in most of the src (sub)directories to calculate the
#   header file dependencies for the given source file.
###############################################################################
macro(calculate_depends SRC)
  # Temporarily set that we didn't get a 2nd argument before we actually check if we did get one or not
  set(CHECK_ANGLE_INCLUDES FALSE)
  # Check for a second argument
  if(${ARGC} GREATER 1)
    set(CHECK_ANGLE_INCLUDES TRUE)
  endif(${ARGC} GREATER 1)
  # Find all the lines in the given source file that have any form of #include on them, regardless of whitespace, but only if they are valid for the platform we are on
  find_includes(${SRC} INCLUDES)
  # Reset the list of headers to empty
  set(HEADERS)
  # Iterate through the strings containing #include (if any)
  foreach(INCLUDE ${INCLUDES})
    # Extract the filename from the #include line
    extract_include_filename(${INCLUDE} FILENAME QUOTE_TYPE)
    if(QUOTE_TYPE STREQUAL "quotes")
      # Append the filename to the list of headers
      append_to_list(HEADERS ${FILENAME})
    else(QUOTE_TYPE STREQUAL "quotes")
      # The following checks will only be done if there was a request for angle includes to be checked
      if(CHECK_ANGLE_INCLUDES)
        # Find the path of the include file
        if(DEFAULT_INCLUDE_DIRS OR WSDK_PATH)
          find_path(FOUND_${FILENAME}_INCLUDE NAMES ${FILENAME} PATHS ${DEFAULT_INCLUDE_DIRS} ${WSDK_PATH}/include)
        else(DEFAULT_INCLUDE_DIRS OR WSDK_PATH)
          find_path(FOUND_${FILENAME}_INCLUDE NAMES ${FILENAME})
        endif(DEFAULT_INCLUDE_DIRS OR WSDK_PATH)
        # If the include file was found, add it's path to the list of include paths, but only if it doesn't already exist and isn't in the defaults for the compiler
        if(FOUND_${FILENAME}_INCLUDE)
          find_in_list(DEFAULT_INCLUDE_DIRS "${FOUND_${FILENAME}_INCLUDE}" FOUND_IN_DEFAULTS)
          if(FOUND_IN_DEFAULTS EQUAL -1)
            find_in_list(${ARGV1} "${FOUND_${FILENAME}_INCLUDE}" FOUND_IN_INCLUDES)
            if(FOUND_IN_INCLUDES EQUAL -1)
              append_to_list(${ARGV1} "${FOUND_${FILENAME}_INCLUDE}")
            endif(FOUND_IN_INCLUDES EQUAL -1)
          endif(FOUND_IN_DEFAULTS EQUAL -1)
        else(FOUND_${FILENAME}_INCLUDE)
          message(FATAL_ERROR "${SRC} needs header file ${FILENAME} but we were unable to locate that header file! Check that the header file is within the search path of your OS.")
        endif(FOUND_${FILENAME}_INCLUDE)
      endif(CHECK_ANGLE_INCLUDES)
    endif(QUOTE_TYPE STREQUAL "quotes")
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
    sort_list(HEADERS)
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
  endif(${ARGC} GREATER 1)
  # If the environment variable is already defined, just tack the item to the end
  if(DEFINED ENV{CPACK_IGNORED_FILES})
    set(ENV{CPACK_IGNORED_FILES} "$ENV{CPACK_IGNORED_FILES};${REAL_ITEM}")
  # Otherwise set the environment variable to the item
  else(DEFINED ENV{CPACK_IGNORED_FILES})
    set(ENV{CPACK_IGNORED_FILES} "${REAL_ITEM}")
  endif(DEFINED ENV{CPACK_IGNORED_FILES})
endmacro(add_to_cpack_ignored_files)
