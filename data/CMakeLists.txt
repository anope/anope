# Only install example.chk and example.conf from this directory
# NOTE: I would've had this just find all files in the directory, but that would include files not needed (like this file)
set(DATA example.chk example.conf mysql/tables.sql)
install(FILES ${DATA}
  DESTINATION data
)
