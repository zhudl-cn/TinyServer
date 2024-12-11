cmake_minimum_required(VERSION 3.00)

project (TinyServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -g -Wall -O0 -Wno-unused-variable -pthread")

include_directories(
	utils/
	net/
)

link_directories(
	/lib64
)

set(net_srcs
net/Acceptor.cpp
net/Buffer.cpp
net/Epoll.cpp
net/EventLoop.cpp
net/Poll_.cpp
net/Select.cpp
net/TCPConnection.cpp
net/TCPServer.cpp
net/ThreadPool.cpp
)

set(util_srcs
utils/utils.cpp
)

set(app_srcs
appsrc/TinyServer.cpp
appsrc/Session.cpp
appsrc/main.cpp
)

add_executable(tinyServer ${net_srcs} ${util_srcs} ${app_srcs})
TARGET_LINK_LIBRARIES(tinyServer)