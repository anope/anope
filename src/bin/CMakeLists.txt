# If not on Windows, generate anoperc and install it along with mydbgen
if(NOT WIN32)
  configure_file(${Anope_SOURCE_DIR}/src/bin/anoperc.in ${Anope_BINARY_DIR}/src/bin/anoperc)
  install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/anoperc
    DESTINATION bin
  )

endif(NOT WIN32)
