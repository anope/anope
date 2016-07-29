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
      # Locate the library to see if it exists
      find_library(FOUND_${LIBRARY}_LIBRARY NAMES ${LIBRARY} PATHS $ENV{VCINSTALLDIR}/lib ${EXTRA_INCLUDE} ${EXTRA_LIBS})
      # If the library was found, we will add it to the linker flags
      if(FOUND_${LIBRARY}_LIBRARY)
        # Get the path only of the library, to add it to linker flags
        get_filename_component(LIBRARY_PATH ${FOUND_${LIBRARY}_LIBRARY} PATH)
        if(MSVC)
          # For Visual Studio, instead of editing the linker flags, we'll add the library to a separate list of extra dependencies
          list(APPEND EXTRA_DEPENDENCIES "${FOUND_${LIBRARY}_LIBRARY}")
        else()
          # For all others, add the library paths and libraries
          list(APPEND LIBRARY_PATHS "${LIBRARY_PATH}")
          list(APPEND LIBRARIES "${LIBRARY}")
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
    set(THIS_LDFLAGS "${THIS_LDFLAGS} -L${LIBRARY_PATH}")
  endforeach()
  # Iterate through libraries and add them to the linker flags
  foreach(LIBRARY ${LIBRARIES})
    list(APPEND EXTRA_DEPENDENCIES "${LIBRARY}")
  endforeach()
  set(${SRC_LDFLAGS} "${THIS_LDFLAGS}")
  set(${EXTRA_DEPENDS} "${EXTRA_DEPENDENCIES}")
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

macro(calculate_dependencies SRC DEPENDENCIES)
  file(STRINGS ${SRC} REQUIRED_LIBRARIES REGEX "/\\*[ \t]*Dependencies:[ \t]*.*[ \t]*\\*/")
  # Iterate through those lines
  foreach(REQUIRED_LIBRARY ${REQUIRED_LIBRARIES})
    string(REGEX REPLACE "/\\*[ \t]*Dependencies:[ \t]*([^ \t]*)[ \t]*\\*/" "\\1" REQUIRED_LIBRARY ${REQUIRED_LIBRARY})
    # Replace all commas with semicolons
    string(REGEX REPLACE "," ";" REQUIRED_LIBRARY ${REQUIRED_LIBRARY})
    # Iterate through the libraries given
    foreach(LIBRARY ${REQUIRED_LIBRARY})
      list(APPEND ${DEPENDENCIES} "${LIBRARY}")
    endforeach()
  endforeach()
endmacro()

