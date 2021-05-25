cmake_minimum_required(VERSION 2.8...3.13)
project(OthelloGame)
set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0015 NEW)
cmake_policy(SET CMP0023 NEW)

SET(AppName OthelloGame)
SET(ClientName OthelloClient)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)

include_directories("include")

link_directories("lib")

add_executable (OthelloGame source/OthelloGame.cpp source/OthelloClass.cpp include/OthelloClass.h)
add_executable (OthelloClient source/OthelloClient.cpp source/OthelloClass.cpp include/OthelloClass.h include/sio_client.h include/sio_message.h)

target_link_libraries (OthelloClient Threads::Threads)
target_link_libraries (OthelloClient ${CMAKE_THREAD_LIBS_INIT} libsioclient.a)

if(APPLE OR WIN32)
    #target_link_libraries(OthelloClient boost_thread-mt boost_system-mt)
else()
    target_link_libraries(OthelloClient boost_thread boost_system)
endif()
