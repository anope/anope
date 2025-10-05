###############################################################################
# add_to_cpack_ignored_files(<item> [TRUE])
#
# A macro to update the environment variable CPACK_IGNORED_FILES which
#   contains a list of files for CPack to ignore. If the optional 2nd argument
#   of TRUE is given, periods will be converted to \\. for CPack.
###############################################################################
macro(add_to_cpack_ignored_files ITEM)
  # Temporary copy of the original item
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

###############################################################################
# inline_cmake(TARGET FILE)
#
# A macro to execute inline CMake instructions from within a module source.
###############################################################################
macro(inline_cmake TARGET FILE)
  file(STRINGS ${FILE} SRC)
  set(CODE "")
  set(IN_CODE OFF)
  foreach(LINE IN LISTS SRC)
    if(IN_CODE)
      string(REGEX REPLACE "/// " "" CLEAN_LINE ${LINE})
      if(CLEAN_LINE MATCHES "^END CMAKE$")
        cmake_language(EVAL CODE "${CODE}")
        set(CODE "")
        set(IN_CODE OFF)
      else()
        set(CODE "${CODE}\n${CLEAN_LINE}")
      endif()
    elseif(LINE MATCHES "^/// BEGIN CMAKE$")
        message(STATUS "Executing inline CMake code for ${TARGET}")
        set(IN_CODE ON)
    endif()
  endforeach()
endmacro()
