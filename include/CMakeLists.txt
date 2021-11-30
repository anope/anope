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

set(PCH_SOURCES_GCH "")
if(USE_PCH AND CMAKE_COMPILER_IS_GNUCXX)
  string(REPLACE " " ";" PCH_CXXFLAGS "${CXXFLAGS} ${CMAKE_CXX_FLAGS}")

  file(GLOB PCH_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h")
  list(SORT PCH_SOURCES)

  foreach(PCH_SOURCE ${PCH_SOURCES})
    find_includes(${PCH_SOURCE} INCLUDES)
    set(INCLUDES_LIST)
    list(APPEND INCLUDES_LIST ${PCH_SOURCE})
    foreach(INCLUDE ${INCLUDES})
      # Extract the filename from the #include line
      extract_include_filename(${INCLUDE} FILENAME QUOTE_TYPE)
      if(QUOTE_TYPE STREQUAL "quotes")
        if(NOT ${FILENAME} IN_LIST PCH_SOURCES)
          list(APPEND INCLUDES_LIST ${FILENAME})
        endif()
      endif()
    endforeach()

    set(PCH_EXTRAFLAGS "")
    if(DEBUG_BUILD)
      set(PCH_EXTRAFLAGS "-g")
    endif()
    if(PCH_SOURCE STREQUAL "module.h")
      set(PCH_EXTRAFLAGS ${PCH_EXTRAFLAGS} -fPIC)
    endif()
    if(GETTEXT_INCLUDE)
      set(PCH_GETTEXT_INCLUDE "-I${GETTEXT_INCLUDE}")
    endif()

    set(PCH_SOURCES_GCH "${PCH_SOURCES_GCH};${CMAKE_CURRENT_BINARY_DIR}/${PCH_SOURCE}.gch")
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PCH_SOURCE}.gch
      COMMAND ${CMAKE_CXX_COMPILER} ARGS ${PCH_CXXFLAGS} ${PCH_EXTRAFLAGS}
      ${PCH_GETTEXT_INCLUDE} -I${CMAKE_CURRENT_BINARY_DIR} -I${Anope_SOURCE_DIR}/modules/pseudoclients ${CMAKE_CURRENT_SOURCE_DIR}/${PCH_SOURCE} -o ${CMAKE_CURRENT_BINARY_DIR}/${PCH_SOURCE}.gch
      DEPENDS ${INCLUDES_LIST} VERBATIM
    )
  endforeach()
endif()

# Add a custom target to the above file
add_custom_target(headers DEPENDS version-bin ${CMAKE_CURRENT_BINARY_DIR}/version_build ${PCH_SOURCES_GCH})
