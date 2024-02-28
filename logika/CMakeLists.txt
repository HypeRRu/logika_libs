cmake_minimum_required(VERSION 3.14)

project(logika LANGUAGES CXX VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# @todo Разбить на модули

set(SOURCES_LIST
	${SRC_DIR}/connections/connection.cpp
	${SRC_DIR}/connections/serial/serial_connection.cpp
	${SRC_DIR}/connections/serial/serial_port_connection.cpp
	${SRC_DIR}/connections/network/net_connection.cpp
	${SRC_DIR}/connections/network/udp_connection.cpp
	${SRC_DIR}/connections/network/tcp_connection.cpp
	${SRC_DIR}/connections/utils/types_converter.cpp
	${SRC_DIR}/protocols/protocol.cpp
	${SRC_DIR}/protocols/m4/archive_record.cpp
	${SRC_DIR}/protocols/m4/packet.cpp
	${SRC_DIR}/protocols/m4/m4protocol.cpp
	${SRC_DIR}/meters/types.cpp
	${SRC_DIR}/meters/archive_types.cpp
	${SRC_DIR}/meters/archive_def.cpp
	${SRC_DIR}/meters/archive_field_def.cpp
	${SRC_DIR}/meters/channel.cpp
	${SRC_DIR}/meters/item_def_base.cpp
	${SRC_DIR}/meters/meter.cpp
	${SRC_DIR}/meters/tag_def.cpp
	${SRC_DIR}/meters/tag.cpp
	${SRC_DIR}/meters/logika4/tag_def4.cpp
	${SRC_DIR}/meters/logika4/4l/tag_def4l.cpp
	${SRC_DIR}/meters/logika4/4l/archive_def4l.cpp
	${SRC_DIR}/meters/logika4/4m/tag_def4m.cpp
	${SRC_DIR}/meters/logika4/4m/archive_def4m.cpp
	${SRC_DIR}/meters/logika6/tag_def6.cpp
	${SRC_DIR}/meters/logika6/archive_def6.cpp
	${SRC_DIR}/meters/utils/types_converter.cpp
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
	list(APPEND CMAKE_CXX_FLAGS -g3)
endif()

add_executable(
	test_main
	${SOURCES_LIST}
)

target_include_directories(test_main PRIVATE ${INCLUDE_DIR})

if (WIN32)
target_link_libraries(test_main PRIVATE ws2_32)
endif()

target_link_libraries(test_main logika_resources)
target_include_directories(test_main PRIVATE ${logika_resources_INCLUDE_DIR})
