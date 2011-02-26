# Set version.cpp to use C++ as well as set its compile flags
set_source_files_properties(version.cpp PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
# Generate version executable to modify version.h, setting it's linker flags as well
add_executable(version version.cpp)
set_target_properties(version PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
get_target_property(version_BINARY version LOCATION)
# Modify version.h from the above executable, with dependencies to the given headers, version.cpp, and all source files in the main Anope build
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
  COMMAND ${version_BINARY} ${Anope_SOURCE_DIR}/src/version.sh ${CMAKE_CURRENT_SOURCE_DIR}/version.h
  DEPENDS version ${SRC_SRCS}
)
# Add version to list of files for CPack to ignore
get_filename_component(version_BINARY ${version_BINARY} NAME)
add_to_cpack_ignored_files("${version_BINARY}$" TRUE)
if(NOT WIN32)
  add_to_cpack_ignored_files("version.h$" TRUE)
endif(NOT WIN32)

set(PCH_SOURCES_GCH "")
if(CMAKE_COMPILER_IS_GNUCXX)
  string(REPLACE " " ";" PCH_CXXFLAGS ${CXXFLAGS})
  file(GLOB INCLUDE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
  remove_item_from_list(INCLUDE_SRCS "version.h")

  set(PCH_SOURCES "module.h;modules.h;services.h")
  foreach(PCH_SOURCE ${PCH_SOURCES})
    set(PCH_EXTRAFLAGS "")
    if(DEBUG_BUILD)
      set(PCH_EXTRAFLAGS "-g")
    endif(DEBUG_BUILD)
    if(PCH_SOURCE STREQUAL "module.h")
      set(PCH_EXTRAFLAGS ${PCH_EXTRAFLAGS} -fPIC)
    endif(PCH_SOURCE STREQUAL "module.h")
    if(GETTEXT_INCLUDE)
	set(PCH_GETTEXT_INCLUDE "-I${GETTEXT_INCLUDE}")
    endif(GETTEXT_INCLUDE)

    set(PCH_SOURCES_GCH "${PCH_SOURCES_GCH};${CMAKE_CURRENT_BINARY_DIR}/${PCH_SOURCE}.gch")
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PCH_SOURCE}.gch
      COMMAND ${CMAKE_CXX_COMPILER} ARGS ${PCH_CXXFLAGS} ${PCH_EXTRAFLAGS} ${PCH_GETTEXT_INCLUDE} -I${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE} -o ${CMAKE_CURRENT_BINARY_DIR}/${PCH_SOURCE}.gch
      DEPENDS ${INCLUDE_SRCS} VERBATIM
    )
  endforeach(PCH_SOURCE ${PCH_SOURCES})
endif(CMAKE_COMPILER_IS_GNUCXX)

# Add a custom target to the above file
add_custom_target(headers DEPENDS version ${CMAKE_CURRENT_BINARY_DIR}/version.h ${PCH_SOURCES_GCH})
