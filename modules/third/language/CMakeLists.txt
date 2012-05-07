# Only do this if gettext is installed
if(GETTEXT_FOUND)
  # Get all of the .po files
  file(GLOB LANG_SRCS_PO RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.*.po")
  sort_list(LANG_SRCS_PO)

  foreach(LANG_PO ${LANG_SRCS_PO})
    # Get the domain for this language file
    string(LENGTH ${LANG_PO} LANG_PO_LENGTH)
    math(EXPR DOMAIN_LENGTH "${LANG_PO_LENGTH} - 9")
    string(SUBSTRING ${LANG_PO} 0 ${DOMAIN_LENGTH} LANG_DOMAIN)

    # Get the language for this language file
    math(EXPR DOMAIN_LENGTH "${LANG_PO_LENGTH} - 8")
    string(SUBSTRING ${LANG_PO} ${DOMAIN_LENGTH} 5 LANG_LANG)

    # Get the .mo file name
    string(REGEX REPLACE "\\.po$" ".mo" LANG_MO ${LANG_PO})
    # Add the .mo file to a list for use later with add_custom_target
    set(LANG_SRCS_MO ${LANG_SRCS_MO} ${CMAKE_CURRENT_BINARY_DIR}/${LANG_MO})
    # Run msgfmt on the language file, depends on the .po file
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${LANG_MO}
      COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -c ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_PO} -o ${CMAKE_CURRENT_BINARY_DIR}/${LANG_MO}
      MAIN_DEPENDENCY ${LANG_PO}
    )
    # Add to cpack ignored files if not on Windows.
    if(NOT WIN32)
      add_to_cpack_ignored_files("${LANG_MO}")
    endif(NOT WIN32)

    # Install the new language file
    install(CODE "FILE(MAKE_DIRECTORY ${LOCALE_DIR}/${LANG_LANG}/LC_MESSAGES/)")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LANG_MO} DESTINATION ${LOCALE_DIR}/${LANG_LANG}/LC_MESSAGES RENAME ${LANG_DOMAIN}.mo PERMISSIONS ${PERMS})
  endforeach(LANG_PO)

  # Generate languages, depends on the mo files
  add_custom_target(module_language DEPENDS ${LANG_SRCS_MO})
endif(GETTEXT_FOUND)
