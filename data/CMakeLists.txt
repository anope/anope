# Only install cron.example.sh and anope.example.conf from this directory
# NOTE: I would've had this just find all files in the directory, but that would include files not needed (like this file)
set(DATA cron.example.sh anope.example.conf botserv.example.conf hostserv.example.conf modules.example.conf operserv.example.conf chanserv.example.conf global.example.conf memoserv.example.conf nickserv.example.conf chanstats.example.conf)
install(FILES ${DATA}
  DESTINATION ${CONF_DIR}
)
