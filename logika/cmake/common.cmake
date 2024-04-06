set(LIB_NAME ${CMAKE_PROJECT_NAME}_common)

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/common/misc.cpp
    ${SRC_DIR}/common/any.cpp
)

add_library(${LIB_NAME} SHARED ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${LIB_NAME} PRIVATE ${LIB_NAME}_EXPORT)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_SHARED_LIBS)
endif()

unset(${LIB_NAME})
