set(LIB_NAME ${CMAKE_PROJECT_NAME}_meters4)

set(CMAKE_CXX_STANDARD 11)

set(
    ${LIB_NAME}_SOURCES_LIST
	${SRC_DIR}/meters/logika4/archive_field_def4.cpp
	${SRC_DIR}/meters/logika4/tag_def4.cpp
	${SRC_DIR}/meters/logika4/logika4.cpp
)

add_compile_definitions(${PROJECT_NAME_UPPER}_USE_METERS4=)
add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_meters_common)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
