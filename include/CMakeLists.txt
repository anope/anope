# Generate sysconf.h from the earlier configuration
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/sysconf.h.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/sysconf.h"
)

# Generate version-bin executable to modify version.h, setting it's linker flags as well
add_executable(version-bin version.cpp)
set(version_BINARY "$<TARGET_FILE:version-bin>")
# Modify version.h from the above executable, with dependencies to version.cpp
# and all of the source files in the main build
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_CURRENT_BINARY_DIR}/build.h
  COMMAND version-bin ${Anope_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_CURRENT_BINARY_DIR}/build.h
  DEPENDS version-bin ${SRC_SRCS}
)

# Add a custom target to the above file
add_custom_target(headers DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_CURRENT_BINARY_DIR}/build.h)
