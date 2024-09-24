# Find all the *.cpp files within the current source directory, and sort the list
file(GLOB TOOLS_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
list(SORT TOOLS_SRCS)

# Set all the files to use C++ as well as set their compile flags
set_source_files_properties(${TOOLS_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

# Iterate through all the source files
foreach(SRC ${TOOLS_SRCS})
  # Convert the source file extension to have no extension
  string(REGEX REPLACE "\\.cpp$" "" EXE ${SRC})
  # Only continue if this file isn't skipped
  if(NOT SKIP)
    # Generate the executable and set its linker flags, also set it to depend on the main Anope executable to be built beforehand
    add_executable(${EXE} ${SRC})
    set_target_properties(${EXE} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
    add_dependencies(${EXE} ${PROGRAM_NAME})
    # Set the executable to be installed to the bin directory under the main directory
    install(TARGETS ${EXE}
      DESTINATION ${BIN_DIR}
      RUNTIME
    )
    # Add the executable to the list of files for CPack to ignore
    set(EXE_BINARY "$<TARGET_FILE:${EXE}>")
    get_filename_component(EXE_BINARY ${EXE_BINARY} NAME)
    add_to_cpack_ignored_files("${EXE_BINARY}$" TRUE)
  endif()
endforeach()

# If not on Windows, generate anoperc and install it along with mydbgen
if(NOT WIN32)
  if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    configure_file(${Anope_SOURCE_DIR}/src/tools/anope.service.in ${Anope_BINARY_DIR}/src/tools/anope.service)
    install(
      PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/anope.service
      DESTINATION ${BIN_DIR}
    )
  endif()

  configure_file(${Anope_SOURCE_DIR}/src/tools/anoperc.in ${Anope_BINARY_DIR}/src/tools/anoperc)
  install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/anoperc
    DESTINATION ${BIN_DIR}
  )
  install (PROGRAMS geoipupdate.sh
    DESTINATION ${BIN_DIR}
  )
endif()

# On non-Windows platforms, if RUNGROUP is set, change the permissions of the tools directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2770 \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin\")")
endif()
