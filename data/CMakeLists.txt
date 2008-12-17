file(GLOB DATA RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
list(REMOVE_ITEM DATA CMakeLists.txt CMakeFiles Makefile cmake_install.cmake)
install(FILES ${DATA}
  DESTINATION "${DATADIR}"
)

