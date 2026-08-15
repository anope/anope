# Only install cron.example.sh and anope.example.conf from this directory
# NOTE: I would've had this just find all files in the directory, but that would include files not needed (like this file)
set(DATA cron.example.sh anope.example.conf botserv.example.conf hostserv.example.conf modules.example.conf operserv.example.conf chanserv.example.conf global.example.conf memoserv.example.conf nickserv.example.conf chanstats.example.conf)
install(FILES ${DATA}
  DESTINATION ${CONF_DIR}
)

set(EMAILS "email_change" "memo" "password_reset" "registration")
foreach(EMAIL IN LISTS EMAILS)
  install(
    FILES "email/${EMAIL}.example.html" "email/${EMAIL}.example.txt"
    DESTINATION "${CONF_DIR}/email"
  )

  set(EMAIL_LANGS "pt_PT")
  foreach(EMAIL_LANG IN LISTS EMAIL_LANGS)
    set(EMAIL_DIR "email/${EMAIL_LANG}")
    install(
      FILES "${EMAIL_DIR}/${EMAIL}.example.html" "${EMAIL_DIR}/${EMAIL}.example.txt"
      DESTINATION "${CONF_DIR}/${EMAIL_DIR}"
    )
  endforeach()
endforeach()
