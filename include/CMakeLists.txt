# NOTE: Need a better dependency system here!

set(HEADERS_REL commands.h extern.h messages.h module.h modules.h pseudo.h services.h)

set(commands.h_HEADERS modules.h)
set(extern.h_HEADERS slist.h)
set(messages.h_HEADERS modules.h)
set(module.h_HEADERS services.h commands.h ${Anope_BINARY_DIR}/lang/language.h modules.h ${CMAKE_CURRENT_BINARY_DIR}/version.h)
set(modules.h_HEADERS services.h)
set(pseudo.h_HEADERS commands.h ${Anope_BINARY_DIR}/lang/language.h timeout.h encrypt.h datafiles.h slist.h)
set(services.h_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/sysconf.h config.h sockets.h defs.h slist.h events.h bots.h account.h regchannel.h users.h extern.h configreader.h)

if(WIN32)
  set_source_files_properties(version.sh.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  add_executable(version.sh version.sh.c)
  set_target_properties(version.sh PROPERTIES LINK_FLAGS "${LDFLAGS}")
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
    COMMAND version.sh ${Anope_SOURCE_DIR}/version.log ${CMAKE_CURRENT_SOURCE_DIR}/version.sh ${CMAKE_CURRENT_BINARY_DIR}/version.h
    MAIN_DEPENDENCY version.sh DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/services.h ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h ${CMAKE_CURRENT_SOURCE_DIR}/messages.h ${SRC_SRCS}
  )
else(WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.h
    #COMMAND ${CMAKE_COMMAND} -E copy ${Anope_SOURCE_DIR}/version.log ../
    COMMAND ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/version.sh ${Anope_SOURCE_DIR}/version.log ${CMAKE_CURRENT_BINARY_DIR}/version.h
    #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_CURRENT_SOURCE_DIR}/
    #COMMAND ${CMAKE_COMMAND} -E remove ../version.log
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/version.sh DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/services.h ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h ${CMAKE_CURRENT_SOURCE_DIR}/messages.h ${SRC_SRCS}
  #  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/version.sh DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/services.h ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h ${CMAKE_CURRENT_SOURCE_DIR}/messages.h
  )
endif(WIN32)

#foreach(HEADER ${HEADERS_REL})
#  if(NOT WIN32)
#    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
#      COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}
#    )
#  endif(NOT WIN32)
#  set(HEADERS_FULL ${HEADERS_FULL} ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER})
#  set(HEADERS)
#  if(${HEADER}_HEADERS)
#    foreach(HEADER_REL ${${HEADER}_HEADERS})
#      string(SUBSTRING ${HEADER_REL} 0 1 FIRST_CHAR)
#      string(SUBSTRING ${HEADER_REL} 1 1 SECOND_CHAR)
#      if(FIRST_CHAR STREQUAL "/" OR SECOND_CHAR STREQUAL ":")
#        set(HEADERS ${HEADERS} ${HEADER_REL})
#      else(FIRST_CHAR STREQUAL "/" OR SECOND_CHAR STREQUAL ":")
#        set(HEADERS ${HEADERS} ${Anope_SOURCE_DIR}/include/${HEADER_REL})
#      endif(FIRST_CHAR STREQUAL "/" OR SECOND_CHAR STREQUAL ":")
#    endforeach(HEADER_REL)
#  endif(${HEADER}_HEADERS)
#  if(HEADERS)
#    set_source_files_properties(${HEADER} PROPERTIES OBJECT_DEPENDS "${HEADERS}")
#  endif(HEADERS)
#endforeach(HEADER)

#set(HEADERS_FULL ${HEADERS_FULL} ${CMAKE_CURRENT_BINARY_DIR}/version.h)
#message(STATUS "HEADERS_FULL: ${HEADERS_FULL}")

#add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/services.h
#  COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/services.h
#  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/extern.h DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sysconf.h ${CMAKE_CURRENT_SOURCE_DIR}/config.h
#)
#add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/extern.h
#  COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/extern.h
#  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/slist.h
#)
#add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h
#  COMMAND ${CMAKE_COMMAND} -E touch_nocreate ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h
#  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/timeout.h DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/commands.h ${CMAKE_CURRENT_SOURCE_DIR}/encrypt.h ${CMAKE_CURRENT_SOURCE_DIR}/datafiles.h ${CMAKE_CURRENT_SOURCE_DIR}/slist.h
#)
#add_dependencies(${CMAKE_CURRENT_SOURCE_DIR}/version.h src_srcs)
#add_custom_target(include_version_h ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/version.h)
#add_custom_target(include_version_h DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/version.h)
#add_dependencies(include_version_h src_srcs)

#add_custom_target(include ALL)
#add_custom_target(include)
#add_custom_target(headers DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_CURRENT_SOURCE_DIR}/services.h ${CMAKE_CURRENT_SOURCE_DIR}/extern.h ${CMAKE_CURRENT_SOURCE_DIR}/pseudo.h)
#add_custom_target(headers DEPENDS ${HEADERS_FULL})
if(ALL_HEADERS)
  set_source_files_properties(${ALL_HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
  add_custom_target(headers DEPENDS ${ALL_HEADERS})
else(ALL_HEADERS)
  add_custom_target(headers)
endif(ALL_HEADERS)
#add_dependencies(include include_version_h)

set(HEADERS_CLEAN ${CMAKE_CURRENT_BINARY_DIR}/version.h)
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM yes ADDITIONAL_MAKE_CLEAN_FILES "${HEADERS_CLEAN}")
