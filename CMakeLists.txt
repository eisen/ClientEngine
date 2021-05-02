cmake_minimum_required(VERSION 2.8...3.13)
project(OthelloGame)
set(CMAKE_CXX_STANDARD 17)

SET(AppName OthelloGame)
SET(ClientName OthelloClient)

include_directories("./include")

link_directories("./lib")

add_executable (OthelloGame ../src/OthelloGame.cpp ../src/OthelloClass.cpp ../include/OthelloClass.h)
add_executable (OthelloClient ../src/OthelloClient.cpp ../src/OthelloClass.cpp ../include/OthelloClass.h ../include/sio_client.h ../include/sio_message.h)

target_link_libraries (OthelloClient sioclient.lib)