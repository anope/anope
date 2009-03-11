# Find all the *.c and *.cpp files within the current source directory, and sort the list
file(GLOB SRC_SRCS_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB SRC_SRCS_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(SRC_SRCS ${SRC_SRCS_C} ${SRC_SRCS_CPP})
# If not using Visual Studio, don't include win32_memory.cpp, it's only required by Visual Studio to override it's override of the new/delete operators
if(NOT MSVC)
  remove_item_from_list(SRC_SRCS win32_memory.cpp)
endif(NOT MSVC)
# If not using Windows, don't include windows.cpp, as it's Windows-specific
if(NOT WIN32)
  remove_item_from_list(SRC_SRCS windows.cpp)
endif(NOT WIN32)
sort_list(SRC_SRCS)

# Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
set_source_files_properties(${SRC_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Create an empty list to store extra include directories
set(EXTRA_INCLUDES)
# Iterate through all the source files
foreach(SRC ${SRC_SRCS})
  # Temporary variable for the current source's include directories
  set(TEMP_INCLUDES)
  # Calculate the header file dependencies for the given source file
  calculate_depends(${SRC} TEMP_INCLUDES)
  # If there were some extra include directories, add them to the list
  if(TEMP_INCLUDES)
    append_to_list(EXTRA_INCLUDES ${TEMP_INCLUDES})
  endif(TEMP_INCLUDES)
endforeach(SRC)
# If there were extra include directories, remove the duplicates and add the directories to the include path
if(EXTRA_INCLUDES)
  remove_list_duplicates(EXTRA_INCLUDES)
  include_directories(${EXTRA_INCLUDES})
endif(EXTRA_INCLUDES)

# Under Windows, we also include a resource file to the build
if(WIN32)
  # Make sure that the resource file is seen as an RC file to be compiled with a resource compiler, not a C++ compiler
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32.rc LANGUAGE RC)
  # Add the resource file to the list of sources
  append_to_list(SRC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/win32.rc)
  # For MinGW, we have to change the compile flags
  if(MINGW)
    set(RC_CFLAGS "-DMINGW -Ocoff -I${Anope_SOURCE_DIR}/include")
    # If any sort of debugging is being enabled, add a _DEBUG define to the flags for the resource compiler
    if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
      set(RC_CFLAGS "${RC_CFLAGS} -D_DEBUG")
    endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32.rc COMPILE_FLAGS "${RC_CFLAGS}")
  # For anything else, assumingly Visual Studio at this point, use a different set of compile flags
  else(MINGW)
    set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/win32.rc COMPILE_FLAGS "/i\"${Anope_SOURCE_DIR}/include\"")
  endif(MINGW)
endif(WIN32)

# Generate the Anope executable and set it's linker flags, also set it to export it's symbols even though it's not a module
add_executable(${PROGRAM_NAME} ${SRC_SRCS})
set_target_properties(${PROGRAM_NAME} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}" ENABLE_EXPORTS ON)
# On Windows, also link Anope to the wsock32 library, as well as set the version
if(WIN32)
  target_link_libraries(${PROGRAM_NAME} wsock32)
  set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
endif(WIN32)
# Building the Anope executable requires the language files to be compiled first as well as the version.h header to be generated
add_dependencies(${PROGRAM_NAME} language headers)

# Get the filename of the Anope executable as it is in on this system
get_target_property(SERVICES_BINARY ${PROGRAM_NAME} LOCATION)
get_filename_component(SERVICES_BINARY ${SERVICES_BINARY} NAME)
set(SERVICES_BIN "${SERVICES_BINARY}")
# Add the Anope executable to the list of files for CPack to ignore
add_to_cpack_ignored_files("${SERVICES_BINARY}$" TRUE)

# Generate sysconf.h from the earlier configuration
configure_file(${Anope_SOURCE_DIR}/include/sysconf.h.cmake ${Anope_BINARY_DIR}/include/sysconf.h)

# Go into the following directories and run their CMakeLists.txt as well
add_subdirectory(bin)
add_subdirectory(core)
add_subdirectory(modules)
add_subdirectory(protocol)
add_subdirectory(tools)

# Set Anope to be installed to the bin directory
install(TARGETS ${PROGRAM_NAME}
  DESTINATION bin
)
