cmake_minimum_required(VERSION 2.6)

PROJECT(libBmdb)
ADD_LIBRARY(Bmdb STATIC EventMain.cpp EventChannel.cpp PlainChannel.cpp HttpChannel.cpp Packet.cpp PlainPacket.cpp 
		HttpReqPacket.cpp TimerThread.cpp TimeoutThread.cpp OmniPacket.cpp OmniClient.cpp OmniConnect.cpp Log.cpp 
		util.cpp FileIni.cpp )

TARGET_LINK_LIBRARIES(Bmdb event)
INCLUDE_DIRECTORIES(/usr/include /usr/local/include /opt/local/include )
set(CMAKE_CXX_FLAGS "-g -ggdb")
