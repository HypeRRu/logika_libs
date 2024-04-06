set(LIB_NAME ${CMAKE_PROJECT_NAME}_connections)

# @todo Еще более мелкое разбиение на модули
# @todo Выделить tcp/udp и com

set(
    ${LIB_NAME}_SOURCES_LIST
    ${SRC_DIR}/connections/connection.cpp
	${SRC_DIR}/connections/serial/serial_connection.cpp
	${SRC_DIR}/connections/serial/serial_port_connection.cpp
	${SRC_DIR}/connections/network/net_connection.cpp
	${SRC_DIR}/connections/network/udp_connection.cpp
	${SRC_DIR}/connections/network/tcp_connection.cpp
	${SRC_DIR}/connections/utils/types_converter.cpp
)

if (WIN32)
	list(APPEND ${LIB_NAME}_SOURCES_LIST
		${SRC_DIR}/connections/serial/serial_port_connection_windows.cpp
		${SRC_DIR}/connections/network/udp_connection_windows.cpp
		${SRC_DIR}/connections/network/tcp_connection_windows.cpp
		${SRC_DIR}/connections/common/windows_socket_io.cpp
	)
elseif(UNIX)
	list(APPEND ${LIB_NAME}_SOURCES_LIST
		${SRC_DIR}/connections/serial/serial_port_connection_linux.cpp
		${SRC_DIR}/connections/network/udp_connection_linux.cpp
		${SRC_DIR}/connections/network/tcp_connection_linux.cpp
		${SRC_DIR}/connections/common/linux_io.cpp
	)
endif()

add_library(${LIB_NAME} ${${LIB_NAME}_SOURCES_LIST})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LIB_NAME} INTERFACE ${CMAKE_PROJECT_NAME}_logger)
target_link_libraries(${LIB_NAME} INTERFACE ${CMAKE_PROJECT_NAME}_common)

if (WIN32)
    target_link_libraries(${LIB_NAME} INTERFACE ws2_32)
endif()

unset(${LIB_NAME})
