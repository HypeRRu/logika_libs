set(LIB_NAME ${CMAKE_PROJECT_NAME}_resource_converters)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/meters/converters/meter_converters.cpp
)

add_library(${LIB_NAME} ${${LIB_NAME}_SOURCES_LIST})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_resources)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_logger)
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_meters)
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_include_directories(${LIB_NAME} PUBLIC ${logika_resources_INCLUDE_DIR})

unset(${LIB_NAME})