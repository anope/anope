# Set version.cpp to use C++ as well as set its compile flags
set_source_files_properties(version.cpp PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
# Generate version-bin executable to modify version.h, setting it's linker flags as well
add_executable(version-bin version.cpp)
set_target_properties(version-bin PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
set(version_BINARY "$<TARGET_FILE:version-bin>")
# Modify version.h from the above executable, with dependencies to version.cpp
# and all of the source files in the main build
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version_build
  COMMAND ${version_BINARY} ${Anope_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_CURRENT_BINARY_DIR}/build.h
  DEPENDS version-bin ${SRC_SRCS}
)
# Add version-bin to list of files for CPack to ignore
get_filename_component(version_BINARY ${version_BINARY} NAME)
add_to_cpack_ignored_files("${version_BINARY}$" TRUE)
if(NOT WIN32)
  add_to_cpack_ignored_files("version.h$" TRUE)
  add_to_cpack_ignored_files("build.h$" TRUE)
endif()

# Add a custom target to the above file
add_custom_target(headers DEPENDS version-bin ${CMAKE_CURRENT_BINARY_DIR}/version_build)
