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
  # Reset linker flags
  set(TEMP_LDFLAGS)
  # Reset extra dependencies
  set(TEMP_DEPENDENCIES)
  # Calculate the library dependencies for the given source file
  calculate_libraries(${SRC} TEMP_LDFLAGS TEMP_DEPENDENCIES)
  # For Visual Studio only, include win32_memory.cpp to the list of sources, required to override Visual Studio's overrides of the new/delete operators
  if(MSVC)
    append_to_list(SRC ${Anope_SOURCE_DIR}/src/win32_memory.cpp)
    set_source_files_properties(${Anope_SOURCE_DIR}/src/win32_memory.cpp LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  endif(MSVC)
  # Generate the module and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
  add_library(${SO} MODULE ${SRC})
  set_target_properties(${SO} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" SUFFIX "" LINK_FLAGS "${TEMP_LDFLAGS}")
  add_dependencies(${SO} ${PROGRAM_NAME})
  # For Windows only, have the module link to the export library of Anope as well as the wsock32 library (most of the modules probably don't need this, but this is to be on the safe side), also set it's version
  if(WIN32)
    target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 ${TEMP_DEPENDENCIES})
    set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
  endif(WIN32)
  # Set the module to be installed to the module directory under the data directory
  install(TARGETS ${SO}
    DESTINATION data/modules
  )
endforeach(SRC)

# Get a list of ALL files and directories within the current directory
file(GLOB MODULES_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
remove_item_from_list(MODULES_FILES ".svn")

# Iterate through this directory searching for subdirectories, and creating modules for those subdirectories
foreach(FILE ${MODULES_FILES})
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
    file(GLOB MODULES_SUBDIR_SRCS_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${FILE}/*.c")
    file(GLOB MODULES_SUBDIR_SRCS_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${FILE}/*.cpp")
    set(MODULES_SUBDIR_SRCS ${MODULES_SUBDIR_SRCS_C} ${MODULES_SUBDIR_SRCS_CPP})
    sort_list(MODULES_SUBDIR_SRCS)

    # Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
    set_source_files_properties(${MODULES_SUBDIR_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

    # Add .so to the end of the directory name, this will be the module's name
    set(SO "${FILE}.so")

    # Temporary linker flags for this subdirectory
    set(SUBDIR_LDFLAGS "${LDFLAGS}")
    # Temporary extra dependencies for this subdirectory
    set(SUBDIR_EXTRA_DEPENDS)

    # Iterate through the source files in the subdirectory
    foreach(SRC ${MODULES_SUBDIR_SRCS})
      # Temporary variable for the current source's include directories
      set(TEMP_INCLUDES)
      # Calculate the header file dependencies for the given source file
      calculate_depends(${SRC} TEMP_INCLUDES)
      # If there were some extra include directories, add them to the list
      if(TEMP_INCLUDES)
        append_to_list(EXTRA_INCLUDES ${TEMP_INCLUDES})
      endif(TEMP_INCLUDES)
      # Reset linker flags
      set(TEMP_LDFLAGS)
      # Reset extra dependencies
      set(TEMP_DEPENDENCIES)
      # Calculate the library dependencies for the given source file
      calculate_libraries(${SRC} TEMP_LDFLAGS TEMP_DEPENDENCIES)

      # Append this source file's linker flags to the subdirectoy's linker flags, if there are any to append
      if(TEMP_LDFLAGS)
        set(SUBDIR_LDFLAGS "${SUBDIR_LDFLAGS} ${TEMP_LDFLAGS}")
      endif(TEMP_LDFLAGS)
      # Append this source file's extra dependencies to the subdirector's extra dependencies, if there are any to append
      if(TEMP_DEPENDENCIES)
        append_to_list(SUBDIR_EXTRA_DEPENDS ${TEMP_DEPENDENCIES})
      endif(TEMP_DEPENDENCIES)
    endforeach(SRC)

    # Remove duplicates from the linker flags
    if(SUBDIR_LDFLAGS)
      remove_list_duplicates(SUBDIR_LDFLAGS)
    endif(SUBDIR_LDFLAGS)
    # Remove duplicates from the extra dependencies
    if(SUBDIR_EXTRA_DEPENDS)
      remove_list_duplicates(SUBDIR_EXTRA_DEPENDS)
    endif(SUBDIR_EXTRA_DEPENDS)

    # For Visual Studio only, include win32_memory.cpp to the list of sources, required to override Visual Studio's overrides of the new/delete operators
    if(MSVC)
      append_to_list(MODULE_SUBDIR_SRCS ${Anope_SOURCE_DIR}/src/win32_memory.cpp)
      set_source_files_properties(${Anope_SOURCE_DIR}/src/win32_memory.cpp LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
    endif(MSVC)

    # Generate the module and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
    add_library(${SO} MODULE ${MODULES_SUBDIR_SRCS})
    set_target_properties(${SO} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" SUFFIX "" LINK_FLAGS "${SUBDIR_LDFLAGS}")
    add_dependencies(${SO} ${PROGRAM_NAME})
    # For Windows only, have the module link to the export library of Anope as well as the wsock32 library (most of the modules probably don't need this, but this is to be on the safe side), also set it's version
    if(WIN32)
      target_link_libraries(${SO} ${PROGRAM_NAME} wsock32 ${SUBDIR_EXTRA_DEPENDS})
      set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
    endif(WIN32)
    # Set the module to be installed to the module directory under the data directory
    install(TARGETS ${SO}
      DESTINATION data/modules
    )
  endif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
endforeach(FILE)

# If there were extra include directories, remove the duplicates and add the directories to the include path
if(EXTRA_INCLUDES)
  remove_list_duplicates(EXTRA_INCLUDES)
  include_directories(${EXTRA_INCLUDES})
endif(EXTRA_INCLUDES)
