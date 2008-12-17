#if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
#  set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
#else(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
#  file(RELATIVE_PATH DIR ${Anope_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
#  set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/${DIR})
#endif(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB CORE_SRCS_C RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB CORE_SRCS_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
set(CORE_SRCS ${CORE_SRCS_C} ${CORE_SRCS_CPP})
list(SORT CORE_SRCS)

#add_custom_target(core ALL)
#add_custom_target(core)
#add_dependencies(core build)

set_source_files_properties(${CORE_SRCS} PROPERTIES LANGUAGE CXX COMPILE_FLAGS "${CXXFLAGS}")

set(cs_set.c_HEADERS encrypt.h)
set(ns_register.c_HEADERS encrypt.h)
set(ns_saset.c_HEADERS encrypt.h)
set(ns_set.c_HEADERS encrypt.h)

foreach(SRC ${CORE_SRCS})
  string(REGEX REPLACE "\\.cpp$" ".x" SRC_X ${SRC})
  string(REGEX REPLACE "\\.c$" ".o" SRC_O ${SRC_X})
  string(REGEX REPLACE "\\.x$" ".o" OBJ ${SRC_O})
  #string(REGEX REPLACE "\\." "_" OBJ_TARGET ${OBJ})
  string(REGEX REPLACE "\\.o" ".so" SO ${OBJ})
  #string(REGEX REPLACE "\\.o" "" SO ${OBJ})
  #string(REGEX REPLACE "\\." "_" SO_TARGET ${SO})
  #add_custom_command(OUTPUT ${BUILD_DIR}/${OBJ}
  #  COMMAND ${MY_COMPILER} ${MY_COMP_ARG} ${CMAKE_CXX_CFLAGS} -I${Anope_SOURCE_DIR}/include -c ${SRC} -o ${BUILD_DIR}/${OBJ}
  #  MAIN_DEPENDENCY ${SRC}
  #)
  #add_custom_target(core_${OBJ_TARGET} DEPENDS ${BUILD_DIR}/${OBJ})
  #add_custom_command(OUTPUT ${BUILD_DIR}/${SO}
  #  COMMAND ${MY_COMPILER} ${MY_COMP_ARG} ${CMAKE_CXX_CFLAGS} ${CMAKE_SHARED_LIBRARY_CXX_FLAGS} ${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} ${BUILD_DIR}/${OBJ} -o ${BUILD_DIR}/${SO}
  #  MAIN_DEPENDENCY ${BUILD_DIR}/${OBJ}
  #)
  #add_custom_target(core_${SO_TARGET} DEPENDS ${BUILD_DIR}/${SO})
  #add_dependencies(core core_${SO_TARGET})
  set(HEADERS ${Anope_SOURCE_DIR}/include/module.h)
  if(${SRC}_HEADERS)
    foreach(HEADER ${${SRC}_HEADERS})
      string(SUBSTRING ${HEADER} 0 1 FIRST_CHAR)
      if(FIRST_CHAR STREQUAL "/")
        set(HEADERS ${HEADERS} ${HEADER})
      else(FIRST_CHAR STREQUAL "/")
        set(HEADERS ${HEADERS} ${Anope_SOURCE_DIR}/include/${HEADER})
      endif(FIRST_CHAR STREQUAL "/")
    endforeach(HEADER)
  endif(${SRC}_HEADERS)
  if(HEADERS)
    set_source_files_properties(${SRC} PROPERTIES OBJECT_DEPENDS "${HEADERS}")
  endif(HEADERS)
  add_library(${SO} MODULE ${SRC})
  add_dependencies(${SO} services)
  #set_target_properties(${SO} PROPERTIES PREFIX "" SUFFIX "" COMPILE_FLAGS ${CXXFLAGS})
  set_target_properties(${SO} PROPERTIES PREFIX "" SUFFIX "")
  install(TARGETS ${SO}
    DESTINATION "${DATADIR}/modules"
  )
endforeach(SRC)
