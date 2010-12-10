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

# Add a custom target to the above file
add_custom_target(headers DEPENDS version ${CMAKE_CURRENT_BINARY_DIR}/version.h)
