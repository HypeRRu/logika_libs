set(LIB_NAME ${CMAKE_PROJECT_NAME}_meters4m)

set(CMAKE_CXX_STANDARD 11)

set(
    ${LIB_NAME}_SOURCES_LIST
	${SRC_DIR}/meters/logika4/4m/ads_tag_block.cpp
	${SRC_DIR}/meters/logika4/4m/tag_def4m.cpp
	${SRC_DIR}/meters/logika4/4m/archive_def4m.cpp
	${SRC_DIR}/meters/logika4/4m/archive_field_def4m.cpp
	${SRC_DIR}/meters/logika4/4m/logika4m.cpp
	${SRC_DIR}/meters/logika4/4m/lgk410.cpp
	${SRC_DIR}/meters/logika4/4m/spg740.cpp
	${SRC_DIR}/meters/logika4/4m/spg742.cpp
	${SRC_DIR}/meters/logika4/4m/spt940.cpp
	${SRC_DIR}/meters/logika4/4m/spt941_20.cpp
	${SRC_DIR}/meters/logika4/4m/spt943rev3.cpp
	${SRC_DIR}/meters/logika4/4m/spt944.cpp
)

add_compile_definitions(${PROJECT_NAME_UPPER}_USE_METERS4M=)
add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_PROJECT_NAME}_meters4)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
