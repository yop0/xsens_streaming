set(STREAMING_PROTOCOL_SRC
angularsegmentkinematicsdatagram.cpp
centerofmassdatagram.cpp
datagram.cpp
eulerdatagram.cpp
jointanglesdatagram.cpp
linearsegmentkinematicsdatagram.cpp
main.cpp
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

add_executable(streaming_protocol ${STREAMING_PROTOCOL_SRC} ${STREAMING_PROTOCOL_HEADERS})
target_include_directories(streaming_protocol PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_directories(streaming_protocol PUBLIC ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(streaming_protocol PUBLIC
    xstypes
    pthread
)