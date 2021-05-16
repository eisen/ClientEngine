//
//  sio_test_sample.cpp
//
//  Created by Melo Yao on 3/24/15.
//

#include "sio_client.h"
#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <utility>
#include <unordered_map>
#include "../include/OthelloClass.h"

#ifdef WIN32
#define HIGHLIGHT(__O__) std::cout<<__O__<<std::endl
#define EM(__O__) std::cout<<__O__<<std::endl

#include <stdio.h>
#include <tchar.h>
#define MAIN_FUNC int _tmain(int argc, _TCHAR* argv[])
#else
#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl

#define MAIN_FUNC int main(int argc ,const char* args[])
#endif

using namespace sio;
using namespace std;
std::mutex _lock;

std::condition_variable_any _cond;
bool connect_finish = false;

class connection_listener
{
    sio::client &handler;
    socket::ptr socket_;

public:
    
    connection_listener(sio::client& h):
    handler(h)
    {
        socket_ = h.socket();
    }
    

    void on_connected()
    {
        _lock.lock();
        _cond.notify_all();
        socket_->emit("set team", handler.get_sessionid());
        connect_finish = true;
        _lock.unlock();
    }
    void on_close(client::close_reason const& reason)
    {
        std::cout<<"sio closed "<<std::endl;
        exit(0);
    }
    
    void on_fail()
    {
        std::cout<<"sio failed "<<std::endl;
        exit(0);
    }
};

int timeout = 1;

socket::ptr current_socket;

string ToString(spaceState ** inputBoard)
{
    string strBoard = "";
    for(int i; i < 8; i++)
    {
        for (int j; j < 8; j++)
        {
            strBoard = strBoard + to_string(inputBoard[i][j]);
        }
    }
    return strBoard;
}

spaceState ** ToArray(string strBoard)
{
    spaceState ** gameBoard;
    gameBoard = new spaceState *[8];

    int charCount = 0;
    
    for (int i = 0; i < 8; i++) 
    {
        gameBoard[i] = new spaceState [8];
    }
    for(int i = 0;i<8;i++) 
    {
        for(int j=0;j<8;j++) 
        {
            gameBoard[i][j] = EMPTY;
        }
    }


    for (string::size_type i = 0; i < strBoard.size(); i++) 
    {
        gameBoard[i / 8][i % 8] = static_cast<spaceState>(strBoard[i]-'0');
    }

    return gameBoard;

}

void bind_events()
{
    current_socket->on("set timeout", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string moveTimeout = data->get_map()["timeout"]->get_string();
                           EM("moveTimeout set to " << moveTimeout << "Seconds");
                           _lock.unlock();
                       }));
    
    current_socket->on("set opponent",sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string gameID = data->get_map()["game_id"]->get_string();
                           string opponentName = data->get_map()["name"]->get_string();
                           EM("Game ID set to " << gameID << " with opponent " <<opponentName);
                           _lock.unlock();
                       }));
    current_socket->on("make move", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                            _lock.lock();
                            string gameID = data->get_map()["game_id"]->get_string();
                            spaceState ** in_board = ToArray(data->get_map()["board"]->get_string());
                            string turnStr = data->get_map()["turn"]->get_string();
                            spaceState turn = static_cast<spaceState>(stoi(turnStr));
                            spaceState ** out_board = in_board;

                            string out_boardStr = ToString(out_board);
                            turnStr = to_string(static_cast<int>(turn) % 2 + 1);
                            string data_out = gameID + turnStr + out_boardStr;

                            // hook this up to the Othello Class to be able to calculate moves properly
                            current_socket->emit("move", data_out);

                           _lock.unlock();
                       }));

    current_socket->on("game ended", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string user = data->get_map()["username"]->get_string();
                           _lock.unlock();
                       }));

    current_socket->on("tournament ended", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string user = data->get_map()["username"]->get_string();
                           _lock.unlock();
                       }));

    current_socket->on("connect", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string user = data->get_map()["username"]->get_string();
                           _lock.unlock();
                       }));

    current_socket->on("disconnect", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string user = data->get_map()["username"]->get_string();
                           _lock.unlock();
                       }));
}

MAIN_FUNC
{

    sio::client h;
    connection_listener l(h);
    
    std::cout << "" <<std::endl;
    h.set_open_listener(std::bind(&connection_listener::on_connected, &l));
    
    std::cout << "" <<std::endl;
    h.set_close_listener(std::bind(&connection_listener::on_close, &l,std::placeholders::_1));
    
    std::cout << "" <<std::endl;
    h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));
    
    std::cout << "" <<std::endl;
    h.connect("http://127.0.0.1:8080");
    
    std::cout << "" <<std::endl;
    _lock.lock();
    
    
    if(!connect_finish)
    {
        _cond.wait(_lock);
    }
    _lock.unlock();
	current_socket = h.socket();
Login:

    HIGHLIGHT("bind on login");

    HIGHLIGHT("emit add user");
    

    HIGHLIGHT("Locking");
    
    _lock.lock();
    //current_socket->emit("start", "round robin");
    // if (timeou) 
    // {
    //     _cond.wait(_lock);
    // }

    HIGHLIGHT("Unlocking");
    _lock.unlock();
    
    HIGHLIGHT("Binding Events");
    bind_events();
    
    HIGHLIGHT("Start to chat,commands:\n'$exit' : exit chat\n'$nsp <namespace>' : change namespace");
    for (std::string line; std::getline(std::cin, line);) {
        if(line.length()>0)
        {
            if(line == "$exit")
            {
                break;
            }
            else if(line.length() > 5&&line.substr(0,5) == "$nsp ")
            {
                string new_nsp = line.substr(5);
                if(new_nsp == current_socket->get_namespace())
                {
                    continue;
                }
                current_socket->off_all();
                current_socket->off_error();
                //per socket.io, default nsp should never been closed.
                if(current_socket->get_namespace() != "/")
                {
                    current_socket->close();
                }
                current_socket = h.socket(new_nsp);
                bind_events();
                //if change to default nsp, we do not need to login again (since it is not closed).
                if(current_socket->get_namespace() == "/")
                {
                    continue;
                }
                goto Login;
            }
            current_socket->emit("new message", line);
            _lock.lock();
            EM("\t\t\t"<<line<<":"<<"You");
            _lock.unlock();
        }
    }

    HIGHLIGHT("Closing...");
    h.sync_close();
    h.clear_con_listeners();
	return 0;
}

