cmake_minimum_required(VERSION 2.6)

PROJECT(client)

ADD_LIBRARY(BmdbClient STATIC ConnectServer.cpp ServerManager.cpp)
#TARGET_LINK_LIBRARIES(BmdbClient event)

ADD_EXECUTABLE(client MyClient.cpp)

#FIND_LIBRARY(BMDBCLIENT_LIB NAMES BmdbClient PATHS ./)
FIND_LIBRARY(BMDB_LIB NAMES Bmdb PATHS ../lib)
FIND_LIBRARY(EVENT_LIB NAMES event PATHS /opt/local/lib)
TARGET_LINK_LIBRARIES(client ${EVENT_LIB} ${BMDB_LIB} BmdbClient)
INCLUDE_DIRECTORIES(/usr/include /opt/local/include ../lib)
set(CMAKE_CXX_FLAGS "-g -ggdb")
