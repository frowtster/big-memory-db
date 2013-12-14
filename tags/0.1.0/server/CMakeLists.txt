cmake_minimum_required(VERSION 2.6)

PROJECT(bmdb)
ADD_EXECUTABLE(bmdb BmdMain.cpp UserClient.cpp Table.cpp)

FIND_LIBRARY(EVENT_LIB NAMES event PATHS /opt/local/lib)
FIND_LIBRARY(BMDB_LIB NAMES Bmdb PATHS ../lib)
TARGET_LINK_LIBRARIES(bmdb ${EVENT_LIB} ${BMDB_LIB})
INCLUDE_DIRECTORIES(/usr/include /opt/local/include ../lib)
set(CMAKE_CXX_FLAGS "-g -ggdb")
