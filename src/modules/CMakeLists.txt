# Find all the *.c and *.cpp files within the current source directory, and sort the list
file(GLOB MODULES_SRCS_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB MODULES_SRCS_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(MODULES_SRCS ${MODULES_SRCS_C} ${MODULES_SRCS_CPP})
list(SORT MODULES_SRCS)

# Set all the files to use C++ as well as set their compile flags (use the module-specific compile flags, though)
set_source_files_properties(${MODULES_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${MODULE_CXXFLAGS}")

# Iterate through all the source files
foreach(SRC ${MODULES_SRCS})
  # Convert the source file extension to have a .so extension
  string(REGEX REPLACE "\\.(c|cpp)$" ".so" SO ${SRC})
  # Calculate the header file dependencies for the given source file
  calculate_depends(${SRC})
  # For Visual Studio only, include win32_memory.cpp to the list of sources, required to override Visual Studio's overrides of the new/delete operators
  if(MSVC)
    list(APPEND SRC ${Anope_SOURCE_DIR}/src/win32_memory.cpp)
  endif(MSVC)
  # Generate the module and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
  add_library(${SO} MODULE ${SRC})
  set_target_properties(${SO} PROPERTIES LINKER_LANGUAGE CXX PREFIX "" SUFFIX "" LINK_FLAGS "${LDFLAGS}")
  add_dependencies(${SO} ${PROGRAM_NAME})
  # For Windows only, have the module link to the export library of Anope as well as the wsock32 library (most of the modules probably don't need this, but this is to be on the safe side), also set it's version
  if(WIN32)
    target_link_libraries(${SO} ${PROGRAM_NAME} wsock32)
    set_target_properties(${PROGRAM_NAME} PROPERTIES VERSION "${VERSION_DOTTED}")
  endif(WIN32)
  # Set the module to be installed to the module directory under the data directory
  install(TARGETS ${SO}
    DESTINATION "${DATADIR}/modules"
  )
endforeach(SRC)
