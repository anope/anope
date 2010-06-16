# This file is external to the read_from_file macro in Anope.cmake in order to
# get around a possible memory leak in older versions of CMake.

file(READ "${FILE}" RESULT)
message("${RESULT}")
