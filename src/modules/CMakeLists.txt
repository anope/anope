# Find all the *.c and *.cpp files within the current source directory, and sort the list
file(GLOB MODULES_SRCS_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB MODULES_SRCS_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(MODULES_SRCS ${MODULES_SRCS_C} ${MODULES_SRCS_CPP})
sort_list(MODULES_SRCS)

# If using Windows, add the MODULE_COMPILE define
if(WIN32)
  add_definitions(-DMODULE_COMPILE)
endif(WIN32)

# Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
set_source_files_properties(${MODULES_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Create an empty list to store extra include directories
set(EXTRA_INCLUDES)

# Iterate through all the source files
foreach(SRC ${MODULES_SRCS})
  # Convert the source file extension to have a .so extension
  string(REGEX REPLACE "\\.(c|cpp)$" ".so" SO ${SRC})
  # Temporary variable for the current source's include directories
  set(TEMP_INCLUDES)
  # Calculate the header file dependencies for the given source file
  calculate_depends(${SRC} TEMP_INCLUDES)
  # If there were some extra include directories, add them to the list
  if(TEMP_INCLUDES)
    append_to_list(EXTRA_INCLUDES ${TEMP_INCLUDES})
  endif(TEMP_INCLUDES)
  # Set up a temporary LDFLAGS for this file
  set(THIS_LDFLAGS "${LDFLAGS}")
  # Reset extra dependencies
  set(EXTRA_DEPENDENCIES)
  # Reset library paths
  set(LIBRARY_PATHS)
  # Reset libraries
  set(LIBRARIES)
  # Check to see if there are any lines matching: /* RequiredLibraries: [something] */
  read_from_file(${SRC} "/\\\\*[ \t]*RequiredLibraries:[ \t]*.*[ \t]*\\\\*/" REQUIRED_LIBRARIES)
  # Iterate through those lines
  foreach(REQUIRED_LIBRARY ${REQUIRED_LIBRARIES})
    # Strip off the /* RequiredLibraries: and */ from the line
    string(REGEX REPLACE "/\\*[ \t]*RequiredLibraries:[ \t]*([^ \t]*)[ \t]*\\*/" "\\1" REQUIRED_LIBRARY ${REQUIRED_LIBRARY})
    # Replace all commas with semicolons
    string(REGEX REPLACE "," ";" REQUIRED_LIBRARY ${REQUIRED_LIBRARY})
    # Iterate through the libraries given
    foreach(LIBRARY ${REQUIRED_LIBRARY})
      # Locate the library to see if it exists
      if(DEFAULT_LIBRARY_DIRS OR WSDK_PATH OR DEFINED $ENV{VCINSTALLDIR})
        find_library(FOUND_${LIBRARY}_LIBRARY NAMES ${LIBRARY} PATHS ${DEFAULT_LIBRARY_DIRS} ${WSDK_PATH}/lib $ENV{VCINSTALLDIR}/lib)
      else(DEFAULT_LIBRARY_DIRS OR WSDK_PATH OR DEFINED $ENV{VCINSTALLDIR})
        find_library(FOUND_${LIBRARY}_LIBRARY NAMES ${LIBRARY})
      endif(DEFAULT_LIBRARY_DIRS OR WSDK_PATH OR DEFINED $ENV{VCINSTALLDIR})
      # If the library was found, we will add it to the linker flags
      if(FOUND_${LIBRARY}_LIBRARY)
        # Get the path only of the library, to add it to linker flags
        get_filename_component(LIBRARY_PATH ${FOUND_${LIBRARY}_LIBRARY} PATH)
        if(MSVC)
          # For Visual Studio, instead of editing the linker flags, we'll add the library to a separate list of extra dependencies
          append_to_list(EXTRA_DEPENDENCIES "${FOUND_${LIBRARY}_LIBRARY}")
        else(MSVC)
          # For all others, add the library paths and libraries
          append_to_list(LIBRARY_PATHS "${LIBRARY_PATH}")
          append_to_list(LIBRARIES "${LIBRARY}")
        endif(MSVC)
      else(FOUND_${LIBRARY}_LIBRARY)
        # In the case of the library not being found, we fatally error so CMake stops trying to generate
        message(FATAL_ERROR "${SRC} needs library ${LIBRARY} but we were unable to locate that library! Check that the library is within the search path of your OS.")
      endif(FOUND_${LIBRARY}_LIBRARY)
    endforeach(LIBRARY)
  endforeach(REQUIRED_LIBRARY)
  # For Visual Studio only, include win32_memory.cpp to the list of sources, required to override Visual Studio's overrides of the new/delete operators
  if(MSVC)
    append_to_list(SRC ${Anope_SOURCE_DIR}/src/win32_memory.cpp)
    set_source_files_properties(${Anope_SOURCE_DIR}/src/win32_memory.cpp LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  endif(MSVC)
  # Remove duplicates from the library paths
  if(LIBRARY_PATHS)
    remove_list_duplicates(LIBRARY_PATHS)
  endif(LIBRARY_PATHS)
  # Remove diplicates from the libraries
  if(LIBRARIES)
    remove_list_duplicates(LIBRARIES)
  endif(LIBRARIES)
  # Iterate through library paths and add them to the linker flags
  foreach(LIBRARY_PATH ${LIBRARY_PATHS})
    find_in_list(DEFAULT_LIBRARY_DIRS "${LIBRARY_PATH}" FOUND_IN_DEFAULTS)
    if(FOUND_IN_DEFAULTS EQUAL -1)
      set(THIS_LDFLAGS "${THIS_LDFLAGS} -L${LIBRARY_PATH}")
    endif(FOUND_IN_DEFAULTS EQUAL -1)
  endforeach(LIBRARY_PATH)
  # Iterate through libraries and add them to the linker flags
  foreach(LIBRARY ${LIBRARIES})
    set(THIS_LDFLAGS "${THIS_LDFLAGS} -l${LIBRARY}")
  endforeach(LIBRARY)
  # Generate the module and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
  add_library(${SO} MODULE ${SRC})
  set_target_properties(${SO} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" SUFFIX "" LINK_FLAGS "${THIS_LDFLAGS}")
  add_dependencies(${SO} ${PROGRAM_NAME})
  # For Windows only, have the module link to the export library of Anope as well as the wsock32 library (most of the modules probably don't need this, but this is to be on the safe side), also set it's version
  if(WIN32)
    target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 ${EXTRA_DEPENDENCIES})
    set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
  endif(WIN32)
  # Set the module to be installed to the module directory under the data directory
  install(TARGETS ${SO}
    DESTINATION data/modules
  )
endforeach(SRC)

# If there were extra include directories, remove the duplicates and add the directories to the include path
if(EXTRA_INCLUDES)
  remove_list_duplicates(EXTRA_INCLUDES)
  include_directories(${EXTRA_INCLUDES})
endif(EXTRA_INCLUDES)
