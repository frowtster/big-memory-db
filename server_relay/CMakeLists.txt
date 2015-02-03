cmake_minimum_required(VERSION 2.6)

PROJECT(bmrelay)
ADD_EXECUTABLE(bmrelay BmrelayMain.cpp UserClient.cpp ServerClient.cpp)

FIND_LIBRARY(EVENT_LIB NAMES event PATHS /opt/local/lib)
FIND_LIBRARY(BMDB_LIB NAMES Bmdb PATHS ../lib)
TARGET_LINK_LIBRARIES(bmrelay ${EVENT_LIB} ${BMDB_LIB} pthread )
INCLUDE_DIRECTORIES(/usr/include /opt/local/include ../lib ../server_db)
set(CMAKE_CXX_FLAGS "-g -ggdb")
