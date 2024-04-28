set(LIB_NAME ${CMAKE_PROJECT_NAME}_meters6)

set(CMAKE_CXX_STANDARD 11)

set(
    ${LIB_NAME}_SOURCES_LIST
	${SRC_DIR}/meters/logika6/data_tag_def6.cpp
	${SRC_DIR}/meters/logika6/data_tag6.cpp
	${SRC_DIR}/meters/logika6/archive_def6.cpp
	${SRC_DIR}/meters/logika6/archive_field_def6.cpp
)

add_compile_definitions(${PROJECT_NAME_UPPER}_USE_METERS6=)
add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_meters_common)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
