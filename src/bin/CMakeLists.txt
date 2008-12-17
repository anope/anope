# If not on Windows, generate anoperc and install it along with mydbgen
if(NOT WIN32)
  configure_file(${Anope_SOURCE_DIR}/src/bin/anoperc.cmake ${Anope_BINARY_DIR}/src/bin/anoperc)
  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/anoperc
    DESTINATION "${INSTDIR}"
  )
  install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/mydbgen
    DESTINATION "${DATADIR}"
  )
endif(NOT WIN32)
