set(STREAMING_PROTOCOL_SRC
angularsegmentkinematicsdatagram.cpp
centerofmassdatagram.cpp
datagram.cpp
eulerdatagram.cpp
jointanglesdatagram.cpp
linearsegmentkinematicsdatagram.cpp
metadatagram.cpp
parsermanager.cpp
positiondatagram.cpp
quaterniondatagram.cpp
scaledatagram.cpp
streamer.cpp
timecodedatagram.cpp
trackerkinematicsdatagram.cpp
udpserver.cpp
)

set(STREAMING_PROTOCOL_HEADERS
angularsegmentkinematicsdatagram.h
centerofmassdatagram.h
datagram.h
eulerdatagram.h
jointanglesdatagram.h
linearsegmentkinematicsdatagram.h
metadatagram.h
parsermanager.h
positiondatagram.h
quaterniondatagram.h
scaledatagram.h
streamer.h
timecodedatagram.h
trackerkinematicsdatagram.h
udpserver.h
)

add_library(streaming_protocol SHARED ${STREAMING_PROTOCOL_SRC} ${STREAMING_PROTOCOL_HEADERS})
target_link_libraries(streaming_protocol PUBLIC
    mtsdk::mtsdk # only xstypes needed
)
target_link_libraries(streaming_protocol PRIVATE Threads::Threads)


install(TARGETS streaming_protocol
    EXPORT ${TARGETS_EXPORT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

add_executable(xsens_streaming_sample main.cpp)
target_link_libraries(xsens_streaming_sample streaming_protocol)
target_link_libraries(xsens_streaming_sample Boost::program_options)
set_target_properties(xsens_streaming_sample PROPERTIES INSTALL_RPATH "${MTSDK_LIBRARY_DIR}") # "/usr/local/xsens/lib64")

install(TARGETS xsens_streaming_sample)