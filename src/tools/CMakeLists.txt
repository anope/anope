if(PREFER_EMBEDDED_SQLITE OR NOT SQLITE_FOUND)
  include_directories(${Anope_SOURCE_DIR}/lib/sqlite)
endif()

function(build_sqlite LIBNAME)
  add_library(tools_anope_sqlite_${LIBNAME} SHARED anope_sqlite.cpp)
  if(PREFER_EMBEDDED_SQLITE OR NOT SQLITE_FOUND)
    target_link_libraries(tools_anope_sqlite_${LIBNAME} sqlite3)
  else()
    target_link_libraries(tools_anope_sqlite_${LIBNAME} ${SQLITE_LIBRARIES})
  endif()
  set_source_files_properties(anope_sqlite.cpp PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  set_target_properties(tools_anope_sqlite_${LIBNAME} PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}" OUTPUT_NAME "anope_sqlite_${LIBNAME}")
  install(TARGETS tools_anope_sqlite_${LIBNAME} DESTINATION ${LIB_DIR})
endfunction()

build_sqlite(ascii)
build_sqlite(rfc1459)
build_sqlite(rfc1459_inspircd)

# If not on Windows, generate anoperc and install it along with mydbgen
if(NOT WIN32)
  configure_file(${Anope_SOURCE_DIR}/src/tools/anoperc.in ${Anope_BINARY_DIR}/src/tools/anoperc)
  install (PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/anoperc
    DESTINATION ${BIN_DIR}
  )
  install (PROGRAMS geoipupdate.sh
    DESTINATION ${BIN_DIR}
  )
endif()

# On non-Windows platforms, if RUNGROUP is set, change the permissions of the tools directory
if(NOT WIN32 AND RUNGROUP)
  install(CODE "execute_process(COMMAND ${CHMOD} 2770 \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin\")")
endif()
