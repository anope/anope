file(GLOB LIBRARIES CONFIGURE_DEPENDS "*")
foreach(LIBRARY IN LISTS LIBRARIES)
	if(IS_DIRECTORY ${LIBRARY})
		file(GLOB_RECURSE LIBRARY_SOURCES CONFIGURE_DEPENDS
			"${LIBRARY}/*.c"
			"${LIBRARY}/*.cc"
			"${LIBRARY}/*.cpp"
		)
		if(LIBRARY_SOURCES)
			cmake_path(GET LIBRARY FILENAME LIBRARY_NAME)
			set(LIBRARY_TARGET "vendored_${LIBRARY_NAME}")
			add_library(${LIBRARY_TARGET} STATIC ${LIBRARY_SOURCES})
			target_compile_options(${LIBRARY_TARGET} PRIVATE "$<IF:$<BOOL:${MSVC}>,/W0,-w>")
			set_target_properties(${LIBRARY_TARGET} PROPERTIES
				FOLDER "VendoredLibraries"
				PREFIX ""
			)
		endif()
	endif()
endforeach()
