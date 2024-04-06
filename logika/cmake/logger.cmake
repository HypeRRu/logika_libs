set(LIB_NAME ${CMAKE_PROJECT_NAME}_logger)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/log/logger.cpp
)

add_library(${LIB_NAME} ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} INTERFACE ${CMAKE_PROJECT_NAME}_common)

unset(${LIB_NAME})
