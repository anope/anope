#if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
#  set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
#else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
#  file(RELATIVE_PATH DIR ${Anope_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
#  set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
#endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})

#add_custom_command(OUTPUT ${BUILD_DIR}/langcomp
#  COMMAND ${MY_COMPILER} ${MY_COMP_ARG} ${CMAKE_CXX_CFLAGS} langcomp.c -o ${BUILD_DIR}/langcomp
#  MAIN_DEPENDENCY langcomp.c
#)
#add_custom_target(lang_langcomp DEPENDS ${BUILD_DIR}/langcomp)
set_source_files_properties(langcomp.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
#add_executable(${BUILD_DIR}/langcomp langcomp.c)
add_executable(langcomp langcomp.c)
set_target_properties(langcomp PROPERTIES LINK_FLAGS "${LDFLAGS}")
#set_target_properties(langcomp PROPERTIES COMPILE_FLAGS ${CXXFLAGS})
#set_target_properties(${BUILD_DIR}/langcomp PROPERTIES LINKER_LANGUAGE CXX)

#add_custom_command(OUTPUT ${BUILD_DIR}/index
#  COMMAND ${GREP} '^[A-Z]' en_us.l > ${BUILD_DIR}/index
#  MAIN_DEPENDENCY en_us.l
#)
if(WIN32)
  set_source_files_properties(langtool.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")
  add_executable(langtool langtool.c)
  set_target_properties(langtool PROPERTIES LINK_FLAGS "${LDFLAGS}")
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index
    COMMAND langtool index ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l ${CMAKE_CURRENT_BINARY_DIR}/index
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l DEPENDS langtool
  )
else(WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index
    COMMAND ${GREP} '^[A-Z]' ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l > ${CMAKE_CURRENT_BINARY_DIR}/index
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/en_us.l
  )
endif(WIN32)
#add_custom_target(lang_index DEPENDS ${BUILD_DIR}/index)

#add_custom_target(language)

set(LANG_SRCS cat.l de.l en_us.l es.l fr.l gr.l hun.l it.l nl.l pl.l pt.l ru.l tr.l)
list(SORT LANG_SRCS)

foreach(LANG_L ${LANG_SRCS})
  STRING(REGEX REPLACE "\\.l$" "" LANG ${LANG_L})
  #set(LANGS ${LANGS} ${BUILD_DIR}/${LANG})
  set(LANGS ${LANGS} ${CMAKE_CURRENT_BINARY_DIR}/${LANG})
  #add_custom_command(OUTPUT ${BUILD_DIR}/${LANG}
  #  COMMAND langcomp ${LANG_L}
  #  MAIN_DEPENDENCY ${LANG_L} DEPENDS langcomp ${BUILD_DIR}/index
  #)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${LANG}
    #CMMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_L} ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND langcomp ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_L} ${CMAKE_CURRENT_BINARY_DIR}/${LANG}
    #COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/${LANG_L}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${LANG_L} DEPENDS langcomp ${CMAKE_CURRENT_BINARY_DIR}/index
  )
  #add_custom_target(lang_${LANG} ALL DEPENDS ${BUILD_DIR}/${LANG})
  #add_custom_target(lang_${LANG} DEPENDS ${BUILD_DIR}/${LANG})
  #add_dependencies(language lang_${LANG})
  #add_dependencies(language ${BUILD_DIR}/${LANG})
endforeach(LANG_L)

#add_custom_command(OUTPUT ${BUILD_DIR}/language.h
#  COMMAND ${PERL} -e < ${BUILD_DIR}/index > ${BUILD_DIR}/language.h 'print STDERR \"Generating language.h... \"\; $$i=0\; while \(<>\) { chop\; printf \"\#define %-32s %d\\n\", $$_, $$i++\; } print \"\\n\#define NUM_STRINGS $$i\\n\"\; print STDERR \"$$i strings\\n\"\;'
#  COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_DIR}/language.h ${Anope_SOURCE_DIR}/include/
#  MAIN_DEPENDENCY ${BUILD_DIR}/index
#)
if(WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/language.h
    COMMAND langtool language.h ${CMAKE_CURRENT_BINARY_DIR}/index ${CMAKE_CURRENT_BINARY_DIR}/language.h
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/index DEPENDS langtool
  )
else(WIN32)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/language.h
    COMMAND ${PERL} -e < ${CMAKE_CURRENT_BINARY_DIR}/index > ${CMAKE_CURRENT_BINARY_DIR}/language.h 'print STDERR \"Generating language.h... \"\; $$i=0\; while \(<>\) { chop\; printf \"\#define %-32s %d\\n\", $$_, $$i++\; } print \"\\n\#define NUM_STRINGS $$i\\n\"\; print STDERR \"$$i strings\\n\"\;'
    #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/language.h ${Anope_SOURCE_DIR}/include/
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/index
  )
endif(WIN32)
#add_custom_target(lang_language_h DEPENDS ${BUILD_DIR}/language.h)
#add_dependencies(language lang_language_h)
#add_dependencies(language ${BUILD_DIR}/language.h)
#add_custom_target(language DEPENDS ${LANGS} ${BUILD_DIR}/language.h)
add_custom_target(language DEPENDS ${LANGS} ${CMAKE_CURRENT_BINARY_DIR}/language.h)

if(RUNGROUP)
  set(PERMS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE)
else(RUNGROUP)
  set(PERMS OWNER_READ OWNER_WRITE)
endif(RUNGROUP)
install(FILES ${LANGS}
  DESTINATION "${DATADIR}/languages"
  PERMISSIONS ${PERMS}
)
if(NOT WIN32)
  if(RUNGROUP)
    #install(CODE "execute_process(COMMAND ${CHGRP} -R ${RUNGROUP} \"${DATADIR}/languages\")")
    install(CODE "execute_process(COMMAND ${CHMOD} 2775 \"${DATADIR}/languages\")")
  else(RUNGROUP)
    install(CODE "execute_process(COMMAND ${CHMOD} 0700 \"${DATADIR}/languages\")")
  endif(RUNGROUP)
endif(NOT WIN32)
