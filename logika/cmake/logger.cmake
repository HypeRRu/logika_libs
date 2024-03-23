set(LIB_NAME ${CMAKE_PROJECT_NAME}_logger)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/log/logger.cpp
)

add_library(${LIB_NAME} ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})

unset(${LIB_NAME})
