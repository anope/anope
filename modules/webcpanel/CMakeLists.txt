build_subdir(${CMAKE_CURRENT_SOURCE_DIR})

install(DIRECTORY templates
  DESTINATION "${DB_DIR}/modules/webcpanel"
)
