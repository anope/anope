# If not on Windows, generate anoperc and install it along with mydbgen
if(NOT WIN32)
  configure_file(${Anope_SOURCE_DIR}/src/bin/anoperc.in ${Anope_BINARY_DIR}/src/bin/anoperc)
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/anoperc
    DESTINATION .
  )
  # Add anoperc to list of files for CPack to ignore
  add_to_cpack_ignored_files("anoperc$")
  install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/mydbgen
    DESTINATION data
  )
endif(NOT WIN32)
