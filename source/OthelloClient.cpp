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
#include <map>
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
    string name_;

public:

    connection_listener(sio::client& h, string n):
    handler(h),
    name_(n)
    {
        socket_ = h.socket();
    }


    void on_connected()
    {
        _lock.lock();
        _cond.notify_all();
        message::ptr player_name = string_message::create(name_);
        message::ptr data_out  = object_message::create();

        data_out->get_map().insert(pair<string,message::ptr>("name", player_name));

        socket_->emit("set team", data_out);
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
    for(int i=0; i < 8; i++)
    {
        for (int j=0; j < 8; j++)
        {
            //cout << inputBoard[i][j];
            strBoard = strBoard + to_string(inputBoard[i][j]);
            //cout << " ";
        }
        //cout << endl;
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
                           int moveTimeout = data->get_map()["timeout"]->get_int();
                           EM("moveTimeout set to " << moveTimeout << " Seconds");
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
                            spaceState turn = static_cast<spaceState>(data->get_map()["turn"]->get_int());
                            string turnStr = to_string(turn);

                            // hook this up to the Othello Class to be able to calculate moves properly
                            Board othelloBoard;
                            othelloBoard.setBoard(in_board);
                            if (turn != othelloBoard.turn)
                            {
                                othelloBoard.switchTurn();
                            }

                            int totalMoveCount = 0;
                            int moveSelection  = 0;
                            spaceState *** availableMoves;
                            spaceState ** objectBoard;

                            // return a full set of possible moves (max sized 64 moves)
                            availableMoves = othelloBoard.legalMoves(othelloBoard.gameBoard,othelloBoard.turn);
                            totalMoveCount = othelloBoard.moveCount(availableMoves);

                            // if the current player has at least 1 move
                            if (totalMoveCount != 0)
                            {
                                // this function should be cut to request from the client VM
                                moveSelection = othelloBoard.moveSelect(totalMoveCount);

                                // the new board is the old board with the selected move applied
                                // updated the board, set the object value and display it
                                objectBoard = availableMoves[moveSelection-1];
                                othelloBoard.setBoard(objectBoard);

                                // reset the no move count
                                //cout << endl;

                                spaceState ** out_board = othelloBoard.gameBoard; // change the in_board to Board.gameBoard

                                string out_boardStr = ToString(out_board);
                                //data_out->get_map()["game_id"]->set_string(gameID);

                                message::ptr game_id_out = string_message::create(gameID);
                                message::ptr board_out  = string_message::create(out_boardStr);
                                message::ptr data_out  = object_message::create();

                                data_out->get_map().insert(pair<string,message::ptr>("game_id",game_id_out));
                                data_out->get_map().insert(pair<string,message::ptr>("board",board_out));

                                current_socket->emit("move", data_out);
                            }
                            // the current count of moves is 0 for the current player
                            else
                            {
                                message::ptr game_id_out = string_message::create(gameID);
                                message::ptr data_out  = object_message::create();

                                data_out->get_map().insert(pair<string,message::ptr>("game_id",game_id_out));

                                current_socket->emit("pass", data_out);
                            }

                           _lock.unlock();
                       }));

    current_socket->on("game ended", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string game_id = data->get_map()["game_id"]->get_string();
                           int black_count = data->get_map()["black_count"]->get_int();
                           int white_count = data->get_map()["white_count"]->get_int();
                           cout << "Game ended: B-" << black_count << " W-" << white_count << endl;
                           _lock.unlock();
                       }));

    current_socket->on("tournament ended", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp)
                       {
                           _lock.lock();
                           string pname = data->get_map()["name"]->get_string();
                           int game_count = data->get_map()["game_count"]->get_int();
                           int win_count = data->get_map()["win_count"]->get_int();
                           int tie_count = data->get_map()["tie_count"]->get_int();
                           cout << "Tournament results for " << pname << " Games: " << game_count;
                           cout << " Wins: " << win_count << " Ties: " << tie_count << endl;
                           _lock.unlock();
                       }));
}

MAIN_FUNC
{

    sio::client h;
    connection_listener l(h, args[1]);

    std::cout << "" <<std::endl;
    h.set_open_listener(std::bind(&connection_listener::on_connected, &l));

    std::cout << "" <<std::endl;
    h.set_close_listener(std::bind(&connection_listener::on_close, &l,std::placeholders::_1));

    std::cout << "" <<std::endl;
    h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));

    std::cout << "" <<std::endl;
    string server_IP = args[2];
    std::cout << "connecting to: " << server_IP << std::endl;
    h.connect("http://13.57.33.218:8080");

    std::cout << "" <<std::endl;

    _lock.lock();

    if(!connect_finish)
    {
        _cond.wait(_lock);
    }
    _lock.unlock();

	current_socket = h.socket();
    bind_events();

    while(h.opened()){}

    h.sync_close();
    h.clear_con_listeners();
	return 0;
}

