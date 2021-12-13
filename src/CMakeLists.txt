
set(WSS_LINUX64 1)

if(WSS_LINUX64)
	set(SRC
		${SRC}
		inet/linux_socket_traits.cpp
		platform/linux/linux_file.cpp
		platform/linux/linux_time.cpp
		platform/linux/linux_string.cpp
	)
elseif(WSS_WIN64)
	include(platform/CMakeLists.win64)
elseif(WSS_MAC64)
	include(platform/CMakeLists.mac64)
endif()

include_directories(
	./src
)

set(SRC
	${SRC}
	error_type.cpp
	buffer/segmented_buffer.cpp
	buffer/segmented_reader_writer_impl.cpp
	inet/common_socket.cpp
	inet/inetaddress_v4.cpp
	inet/tcp.cpp
	inet/channel.cpp
	observer/signal_base.cpp
	observer/signal_list.cpp
	observer/signal_map.cpp
)

add_library(wss STATIC ${SRC})

