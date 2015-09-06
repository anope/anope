# Find all the *.cpp files within the current source directory, and sort the list
file(GLOB TOOLS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
sort_list(TOOLS_SRCS)

# Set all the files to use C++ as well as set their compile flags
set_source_files_properties(${TOOLS_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Iterate through all the source files
foreach(SRC ${TOOLS_SRCS})
  # Convert the source file extension to have no extension
  string(REGEX REPLACE "\\.cpp$" "" EXE ${SRC})
  # Calculate the header file dependencies for the given source file
  calculate_depends(${SRC})
  # Only continue if this file isn't skipped
  if(NOT SKIP)
    # Generate the executable and set its linker flags, also set it to depend on the main Anope executable to be built beforehand
    add_executable(${EXE} ${SRC})
    set_target_properties(${EXE} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
    add_dependencies(${EXE} ${PROGRAM_NAME})
    # Only for Windows, set anopesmtp to require the wsock32 library
    if(WIN32 AND ${EXE} STREQUAL anopesmtp)
      target_link_libraries(${EXE} wsock32)
    endif(WIN32 AND ${EXE} STREQUAL anopesmtp)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "SunOS" AND ${EXE} STREQUAL anopesmtp)
      target_link_libraries(${EXE} socket nsl)
    endif(${CMAKE_SYSTEM_NAME} STREQUAL "SunOS" AND ${EXE} STREQUAL anopesmtp)
    # Set the executable to be installed to the bin directory under the main directory
    install(TARGETS ${EXE}
      DESTINATION ${BIN_DIR}
    )
    # Add the executable to the list of files for CPack to ignore
    get_target_property(EXE_BINARY ${EXE} LOCATION)
    get_filename_component(EXE_BINARY ${EXE_BINARY} NAME)
    add_to_cpack_ignored_files("${EXE_BINARY}$" TRUE)
  endif(NOT SKIP)
endforeach(SRC)

# If not on Windows, generate anoperc and install it along with mydbgen
if(NOT WIN32)
  configure_file(${Anope_SOURCE_DIR}/src/tools/anoperc.in ${Anope_BINARY_DIR}/src/tools/anoperc)
  install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/anoperc
    DESTINATION ${BIN_DIR}
  )
  install (PROGRAMS geoipupdate.sh
    DESTINATION ${BIN_DIR}
  )
endif(NOT WIN32)

# On non-Windows platforms, if RUNGROUP is set, change the permissions of the tools directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2770 \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin\")")
endif(NOT WIN32 AND RUNGROUP)
