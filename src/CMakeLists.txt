
set(WSS_LINUX64 1)

if(WSS_LINUX64)
	set(SRC
		${SRC}
		${LIBWSSDIR}/src/inet/linux_socket_traits.cpp
		${LIBWSSDIR}/src/platform/linux/linux_file.cpp
		${LIBWSSDIR}/src/platform/linux/linux_time.cpp
		${LIBWSSDIR}/src/platform/linux/linux_string.cpp
	)
elseif(WSS_WIN64)
	include(platform/CMakeLists.win64)
elseif(WSS_MAC64)
	include(platform/CMakeLists.mac64)
endif()

include_directories(
	${LIBWSSDIR}/src
)

set(LIBWSSSRC
	${SRC}
	${LIBWSSDIR}/src/wsinit.cpp
	${LIBWSSDIR}/src/error_type.cpp
	${LIBWSSDIR}/src/buffer/segmented_buffer.cpp
	${LIBWSSDIR}/src/buffer/segmented_reader_writer_impl.cpp
	${LIBWSSDIR}/src/inet/common_socket.cpp
	${LIBWSSDIR}/src/inet/inetaddress_v4.cpp
	${LIBWSSDIR}/src/inet/tcp.cpp
	${LIBWSSDIR}/src/inet/channel.cpp
	${LIBWSSDIR}/src/observer/signal_base.cpp
	${LIBWSSDIR}/src/observer/signal_list.cpp
	${LIBWSSDIR}/src/observer/signal_map.cpp
	${LIBWSSDIR}/src/io/iotraits_linux.cpp
	${LIBWSSDIR}/src/io/properties.cpp
	${LIBWSSDIR}/src/io/readerwriter.cpp
)

