set(LIB_NAME ${CMAKE_PROJECT_NAME}_common)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/common/misc.cpp
)

add_library(${LIB_NAME} ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})

unset(${LIB_NAME})
