cmake_minimum_required(VERSION 3.14)

project(logika LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES_LIST
	${SRC_DIR}/connections/connection.cpp
	${SRC_DIR}/connections/serial/serial_connection.cpp
	${SRC_DIR}/connections/serial/serial_port_connection.cpp
	${SRC_DIR}/connections/network/net_connection.cpp
	${SRC_DIR}/connections/network/udp_connection.cpp
	${SRC_DIR}/connections/network/tcp_connection.cpp
	${SRC_DIR}/connections/utils/types_converter.cpp
	${SRC_DIR}/log/logger.cpp
	${SRC_DIR}/common/misc.cpp
	${SRC_DIR}/main.cpp
)

if (WIN32)
	list(APPEND SOURCES_LIST
		${SRC_DIR}/connections/serial/serial_port_connection_windows.cpp
		${SRC_DIR}/connections/network/udp_connection_windows.cpp
		${SRC_DIR}/connections/network/tcp_connection_windows.cpp
		${SRC_DIR}/connections/common/windows_socket_io.cpp
	)
elseif(UNIX)
	list(APPEND SOURCES_LIST
		${SRC_DIR}/connections/serial/serial_port_connection_linux.cpp
		${SRC_DIR}/connections/network/udp_connection_linux.cpp
		${SRC_DIR}/connections/network/tcp_connection_linux.cpp
		${SRC_DIR}/connections/common/linux_io.cpp
	)
endif()

add_executable(
	test_main
	${SOURCES_LIST}
)

target_include_directories(test_main PRIVATE ${INCLUDE_DIR})
target_link_libraries(test_main PRIVATE ws2_32)
