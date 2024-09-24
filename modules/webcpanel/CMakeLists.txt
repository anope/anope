build_subdir(${CMAKE_CURRENT_SOURCE_DIR})

install(DIRECTORY "templates/"
  DESTINATION "${DATA_DIR}/webcpanel/templates/default"
)
