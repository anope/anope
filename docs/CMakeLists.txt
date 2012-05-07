# We only need to install these docs on Windows as most Windows installations will be binary, not source
if(WIN32)
  # Only install given files from this directory
  # NOTE: I would've had this just find all files in the directory, but that would include files not needed (like this file)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_CURRENT_BINARY_DIR}/README.txt)
  if(IN_SOURCE)
    # Add README.txt to list of files for CPack to ignore
    add_to_cpack_ignored_files("README.txt$" TRUE)
  endif(IN_SOURCE)
  set(DOCS Changes Changes.conf DEFCON FAQ INSTALL LANGUAGE MODULES NEWS ${CMAKE_CURRENT_BINARY_DIR}/README.txt WIN32.txt)
  install(FILES ${DOCS}
    DESTINATION ${DOC_DIR}
  )
  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/README.txt")
endif(WIN32)
