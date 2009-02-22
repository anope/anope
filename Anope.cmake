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
    # replace the old list with the new list
    set(${LIST} ${NEW_LIST})
  endif(CMAKE26_OR_BETTER)
endmacro(remove_list_duplicates)

###############################################################################
# read_from_file(<filename> <regex> <output variable>)
#
# A macro to handle reading specific lines from a file, uses file(STRINGS) if
#   using CMake 2.6.x or better, otherwise we read in the entire file and
#   perform a string(REGEX MATCH) on each line of the file instead.
###############################################################################
macro(read_from_file FILE REGEX STRINGS)
  if(CMAKE26_OR_BETTER)
    # For CMake 2.6.x or better, we can just use this function to get the lines that match the given regular expression
    file(STRINGS ${FILE} RESULT REGEX ${REGEX})
  else(CMAKE26_OR_BETTER)
    # For CMake 2.4.x, we need to do this manually, firstly we read the file in
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

###############################################################################
# extract_include_filename(<line> <output variable> [<optional output variable of quote type>])
#
# This function will take a #include line and extract the filename.
###############################################################################
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
  # Find the filename including the quotes or angle brackets, it should be at the end of the line after whitespace was stripped
  string(REGEX MATCH "[\"<].*[\">]$" FILE ${INCLUDE_STRIPPED})
  # If an optional 3rd argument is given, we'll store if the quote style was quoted or angle bracketed
  if(${ARGC} GREATER 2)
    string(SUBSTRING ${FILE} 0 1 QUOTE)
    if(QUOTE STREQUAL "<")
      set(${ARGV2} "angle brackets")
    else(QUOTE STREQUAL "<")
      set(${ARGV2} "quotes")
    endif(QUOTE STREQUAL "<")
  endif(${ARGC} GREATER 2)
  # Get the length of the filename with quotes or angle brackets
  string(LENGTH ${FILE} FILENAME_LEN)
  # Subtract 2 from this length, for the quotes or angle brackets
  math(EXPR FILENAME_LEN "${FILENAME_LEN} - 2")
  # Overwrite the filename with a version sans quotes or angle brackets
  string(SUBSTRING ${FILE} 1 ${FILENAME_LEN} FILE)
  # Set the filename to the the given variable
  set(${FILENAME} "${FILE}")
endmacro(extract_include_filename)

###############################################################################
# calculate_depends(<source filename>)
#
# This function is used in most of the src (sub)directories to calculate the
#   header file dependencies for the given source file.
###############################################################################
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
