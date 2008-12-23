# Find all the *.c and *.cpp files within the current source directory, and sort the list
file(GLOB TOOLS_SRCS_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB TOOLS_SRCS_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(TOOLS_SRCS ${TOOLS_SRCS_C} ${TOOLS_SRCS_CPP})
if(CMAKE244_OR_BETTER)
  list(SORT TOOLS_SRCS)
endif(CMAKE244_OR_BETTER)

# Set all the files to use C++ as well as set their compile flags
set_source_files_properties(${TOOLS_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Iterate through all the source files
foreach(SRC ${TOOLS_SRCS})
  # Convert the source file extension to have no extension
  string(REGEX REPLACE "\\.(c|cpp)$" "" EXE ${SRC})
  # Calculate the header file dependencies for the given source file
  calculate_depends(${SRC})
  # Generate the executable and set it's linker flags, also set it to depend on the main Anope executable to be built beforehand
  add_executable(${EXE} ${SRC})
  set_target_properties(${EXE} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
  add_dependencies(${EXE} ${PROGRAM_NAME})
  # Set the executable to be installed to the tools directory under the bin directory
  install(TARGETS ${EXE}
    DESTINATION tools
  )
  if(IN_SOURCE)
    # Add the executable to the list of files for CPack to ignore
    get_target_property(EXE_BINARY ${EXE} LOCATION)
    get_filename_component(EXE_BINARY ${EXE_BINARY} NAME)
    add_to_cpack_ignored_files("${EXE_BINARY}$" TRUE)
  endif(IN_SOURCE)
endforeach(SRC)

# Only for Windows, set anopesmtp to require the wsock32 library
if(WIN32)
  target_link_libraries(anopesmtp wsock32)
endif(WIN32)

# On non-Windows platforms, if RUNGROUP is set, change the permissions of the tools directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2770 \"\${CMAKE_INSTALL_PREFIX}/tools\")")
endif(NOT WIN32 AND RUNGROUP)
