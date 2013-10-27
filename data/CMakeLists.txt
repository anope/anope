# Only install example.chk and example.conf from this directory
# NOTE: I would've had this just find all files in the directory, but that would include files not needed (like this file)
set(DATA example.chk botserv.example.conf example.conf hostserv.example.conf modules.example.conf operserv.example.conf chanserv.example.conf global.example.conf memoserv.example.conf nickserv.example.conf chanstats.example.conf irc2sql.example.conf stats.standalone.example.conf)
install(FILES ${DATA}
  DESTINATION ${CONF_DIR}
)
