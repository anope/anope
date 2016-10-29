if(PREFER_EMBEDDED_SQLITE OR NOT SQLITE_FOUND)
	add_library(sqlite3 SHARED sqlite3.c)
	install(TARGETS sqlite3 DESTINATION ${LIB_DIR})

	add_executable(shell shell.c)
	target_link_libraries(shell sqlite3 dl pthread)
	set_target_properties(shell PROPERTIES OUTPUT_NAME sqlite3 INSTALL_RPATH_USE_LINK_PATH ON BUILD_WITH_INSTALL_RPATH ON INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${LIB_DIR}")
	install(TARGETS shell DESTINATION ${BIN_DIR})
endif()
