# Set the source file for langcomp to use C++ as well as set it's compile flags
set_source_files_properties(langcomp.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
# Generate langcomp and set it's linker flags
add_executable(langcomp langcomp.c)
set_target_properties(langcomp PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
# Get the location of the binary to use later
get_target_property(langcomp_BINARY langcomp LOCATION)
if(IN_SOURCE)
  # Add the executable to the list of files for CPack to ignore
  file(RELATIVE_PATH langcomp_BINARY_RELATIVE ${Anope_BINARY_DIR} ${langcomp_BINARY})
  add_to_cpack_ignored_files("${langcomp_BINARY_RELATIVE}$" TRUE)
endif(IN_SOURCE)

# Determine if langtool should be built
if(MSVC)
  set(BUILD_LANGTOOL TRUE)
else(MSVC)
  if(NOT GREP OR NOT PERL)
    set(BUILD_LANGTOOL TRUE)
  else(NOT GREP OR NOT PERL)
    set(BUILD_LANGTOOL FALSE)
  endif(NOT GREP OR NOT PERL)
endif(MSVC)

# If grep or perl don't exist on the system, build langtool to generate index and language.h
if(BUILD_LANGTOOL)
  # Set the source file for langtool to use C++ as well as set it's compile flags
  set_source_files_properties(langtool.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  # Generate langtool and set it's linker flags
  add_executable(langtool langtool.c)
  set_target_properties(langtool PROPERTIES LINKER_LANGUAGE CXX LINK_FLAGS "${LDFLAGS}")
  # Get the location of the binary to use later)
  get_target_property(langtool_BINARY langtool LOCATION)
  if(IN_SOURCE)
    # Add the executable to the list of files for CPack to ignore
    file(RELATIVE_PATH langtool_BINARY_RELATIVE ${Anope_BINARY_DIR} ${langtool_BINARY})
    add_to_cpack_ignored_files("${langtool_BINARY_RELATIVE}$" TRUE)
  endif(IN_SOURCE)
endif(BUILD_LANGTOOL)

# If grep exists (and we aren't using Visual Studio, it hates this), use it to generate the index file
if(NOT MSVC AND GREP)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index
    COMMAND ${GREP} '^[A-Z]' ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l > ${CMAKE_CURRENT_BINARY_DIR}/index
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l
  )
# Otherwise, use langtool to generate the index file
else(NOT MSVC AND GREP)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index
    COMMAND ${langtool_BINARY} index ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l ${CMAKE_CURRENT_BINARY_DIR}/index
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l DEPENDS langtool
  )
endif(NOT MSVC AND GREP)
if(IN_SOURCE)
  # Add the index file to the list of files for CPack to ignore
  file(RELATIVE_PATH index_RELATIVE ${Anope_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/index)
  add_to_cpack_ignored_files("${index_RELATIVE}$")
endif(IN_SOURCE)

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
    COMMAND ${langcomp_BINARY} ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_L} ${CMAKE_CURRENT_BINARY_DIR}/${LANG}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_L} DEPENDS langcomp ${CMAKE_CURRENT_BINARY_DIR}/index
  )
  if(IN_SOURCE)
    # Add the language file to the list of files for CPack to ignore
    file(RELATIVE_PATH LANG_RELATIVE ${Anope_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${LANG})
    add_to_cpack_ignored_files("${LANG_RELATIVE}$")
  endif(IN_SOURCE)
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
    COMMAND ${langtool_BINARY} language.h ${CMAKE_CURRENT_BINARY_DIR}/index ${CMAKE_CURRENT_BINARY_DIR}/language.h
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/index DEPENDS langtool
  )
endif(NOT MSVC AND PERL)
if(IN_SOURCE)
  # Add language.h to the list of files for CPack to ignore
  add_to_cpack_ignored_files("language.h$" TRUE)
endif(IN_SOURCE)

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
  DESTINATION data/languages
  PERMISSIONS ${PERMS}
)

# On non-Windows platforms, if RUNGROUP is set, change the permissions of the languages directory
if(NOT WIN32)
  if(RUNGROUP)
    install(CODE "execute_process(COMMAND ${CHMOD} 2770 \"\${CMAKE_INSTALL_PREFIX}/data/languages\")")
  else(RUNGROUP)
    install(CODE "execute_process(COMMAND ${CHMOD} 0700 \"\${CMAKE_INSTALL_PREFIX}/data/languages\")")
  endif(RUNGROUP)
endif(NOT WIN32)
