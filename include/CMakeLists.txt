# If we are building for Visual Studio OR if the system we are on doesn't have sh (which would be odd on a *nix system...), we'll build a C++ program to create version.h
if(MSVC OR NOT SH)
  # Set version.sh.c to use C++ as well as set it's compile flags
  set_source_files_properties(version.sh.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  # Generate version_sh executable to create version.h from the contents of version.sh, setting it's linker flags as well
  add_executable(version_sh version.sh.c)
  set_target_properties(version_sh PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
  # Generate version.h from the above executable and the version.log file from the main source directory, with dependencies to the given headers and all source files in the main Anope build
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
    COMMAND version_sh ${Anope_SOURCE_DIR}/version.log ${CMAKE_CURRENT_SOURCE_DIR}/version.sh ${CMAKE_CURRENT_BINARY_DIR}/version.h
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/version.sh DEPENDS version_sh ${CMAKE_CURRENT_SOURCE_DIR}/services.h ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h ${CMAKE_CURRENT_SOURCE_DIR}/messages.h ${SRC_SRCS}
  )
  # Add version_sh to list of files for CPack to ignore
  get_target_property(version_sh_BINARY version_sh LOCATION)
  get_filename_component(version_sh_BINARY ${version_sh_BINARY} NAME)
  add_to_cpack_ignored_files("${version_sh_BINARY}$" TRUE)
# For any non-Visual Studio platforms that do have sh, we will run version.h through the version.h shell script
else(MSVC OR NOT SH)
  # Generate version.h from version.sh and the version.log file from the main source directory, with dependencies to the given headers and all source files in the main Anope build
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
    COMMAND ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/version.sh ${Anope_SOURCE_DIR}/version.log ${CMAKE_CURRENT_BINARY_DIR}/version.h
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/version.sh DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/services.h ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h ${CMAKE_CURRENT_SOURCE_DIR}/messages.h ${SRC_SRCS}
  )
endif(MSVC OR NOT SH)

# Add version.h to the list of files for CPack to ignore
add_to_cpack_ignored_files("version.h$" TRUE)

# Add a custom target to the above file
add_custom_target(headers DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version.h)
