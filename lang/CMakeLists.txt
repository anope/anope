# Set the source file for langcomp to use C++ as well as set it's compile flags
set_source_files_properties(langcomp.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
# Generate langcomp and set it's linker flags
add_executable(langcomp langcomp.c)
set_target_properties(langcomp PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")

# If grep or perl don't exist on the system, build langtool to generate index and language.h
if(NOT GREP OR NOT PERL)
  # Set the source file for langtool to use C++ as well as set it's compile flags
  set_source_files_properties(langtool.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  # Generate langtool and set it's linker flags
  add_executable(langtool langtool.c)
  set_target_properties(langtool PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
endif(NOT GREP OR NOT PERL)

# If grep exists (and we aren't using Visual Studio, it hates this), use it to generate the index file
if(NOT MSVC AND GREP)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index
    COMMAND ${GREP} '^[A-Z]' ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l > ${CMAKE_CURRENT_BINARY_DIR}/index
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l
  )
# Otherwise, use langtool to generate the index file
else(NOT MSVC AND GREP)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/langtool index ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l ${CMAKE_CURRENT_BINARY_DIR}/index
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l DEPENDS langtool
  )
endif(NOT MSVC AND GREP)

# Find all the *.l files within the current source directory, and sort the list
file(GLOB LANG_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.l")
if(CMAKE244_OR_BETTER)
  list(SORT LANG_SRCS)
endif(CMAKE244_OR_BETTER)

# Iterate through the language files
foreach(LANG_L ${LANG_SRCS})
  # Convert the language file's extension to have no extension
  STRING(REGEX REPLACE "\\.l$" "" LANG ${LANG_L})
  # Add the language file to the list of compiled language files
  append_to_list(LANGS ${CMAKE_CURRENT_BINARY_DIR}/${LANG})
  # Generate a compiled language file using langcomp, as well as having a dependency on the index file being generated
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${LANG}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/langcomp ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_L} ${CMAKE_CURRENT_BINARY_DIR}/${LANG}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_L} DEPENDS langcomp ${CMAKE_CURRENT_BINARY_DIR}/index
  )
endforeach(LANG_L)

# If perl exists (and we aren't using Visual Studio, it hates this), use it to generate language.h
if(NOT MSVC AND PERL)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/language.h
    COMMAND ${PERL} -e < ${CMAKE_CURRENT_BINARY_DIR}/index > ${CMAKE_CURRENT_BINARY_DIR}/language.h 'print STDERR \"Generating language.h... \"\; $$i=0\; while \(<>\) { chop\; printf \"\#define %-32s %d\\n\", $$_, $$i++\; } print \"\\n\#define NUM_STRINGS $$i\\n\"\; print STDERR \"$$i strings\\n\"\;'
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/index
  )
# Otherwise, use langtool to generate language.h
else(NOT MSVC AND PERL)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/language.h
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/langtool language.h ${CMAKE_CURRENT_BINARY_DIR}/index ${CMAKE_CURRENT_BINARY_DIR}/language.h
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/index DEPENDS langtool
  )
endif(NOT MSVC AND PERL)

# Add a custom target to depend on the language files and language.h
add_custom_target(language DEPENDS ${LANGS} ${CMAKE_CURRENT_BINARY_DIR}/language.h)

# If RUNGROUP was set, make the permissions be to have owner read/write as well as group read/write
if(RUNGROUP)
  set(PERMS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE)
# Otherwise, only make the permissions be owner read/write
else(RUNGROUP)
  set(PERMS OWNER_READ OWNER_WRITE)
endif(RUNGROUP)
# Set the language files to be installed to the languages directory under the data directory
install(FILES ${LANGS}
  DESTINATION "${DATADIR}/languages"
  PERMISSIONS ${PERMS}
)

# On non-Windows platforms, if RUNGROUP is set, change the permissions of the languages directory
if(NOT WIN32)
  if(RUNGROUP)
    install(CODE "execute_process(COMMAND ${CHMOD} 2770 \"${DATADIR}/languages\")")
  else(RUNGROUP)
    install(CODE "execute_process(COMMAND ${CHMOD} 0700 \"${DATADIR}/languages\")")
  endif(RUNGROUP)
endif(NOT WIN32)
