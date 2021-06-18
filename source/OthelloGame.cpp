#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>
#include "../include/OthelloClass.h"

using namespace std;

int main() {

    // construct the default board
    Board gameObject;

    int gameType = 3;
    int totalMoveCount = 0; 
    int noMoveCount    = 0; 
    int moveSelection  = 0;

    spaceState *** availableMoves;
    spaceState ** objectBoard;

    // randomizing rand() function based on time
    srand(time(NULL));

    // Game type to indicate if players are human or computer. Our game type will always be 3
    gameType = gameObject.init();

    // take in non-Default board or stick with constructor board
    objectBoard = gameObject.initBoard();

    // Set board to new board
    gameObject.setBoard(objectBoard);

    // Display the initial board and set turn to black
    gameObject.display(gameObject.gameBoard,(gameObject.turn));

    // While the game is in play (one player has available moves)
    while(gameObject.state != 1) 
    {
        // return a full set of possible moves (max sized 64 moves)
        availableMoves = gameObject.legalMoves((gameObject.gameBoard),gameObject.turn);
        totalMoveCount = gameObject.moveCount(availableMoves);

        // if the current player has at least 1 move
        if (totalMoveCount != 0) 
        {
            // this function should be cut to request from the client VM
            moveSelection = gameObject.moveSelect(gameType, totalMoveCount); 

            // the new board is the old board with the selected move applied
            // updated the board, set the object value and display it
            objectBoard = availableMoves[moveSelection-1];
            gameObject.setBoard(objectBoard);
            gameObject.display(gameObject.gameBoard,gameObject.changePiece(gameObject.turn));

            // reset the no move count
            noMoveCount = 0;
            cout << endl;
        }

        // the current count of moves is 0 for the current player
        else 
        {
            noMoveCount++;
            if (noMoveCount >= 2)
            {
                // Set State to game over since no one can play
                gameObject.setState(1);
            }
            else if (noMoveCount == 1)
            {
                // Skip the current player's turn as there are no moves to play
                cout << "Player " << gameObject.turn << " has no moves." << endl;
                cout << "Skipping Turn: it is now Player " << gameObject.changePiece(gameObject.turn) << "'s turn" <<endl;
                gameObject.display(gameObject.gameBoard,gameObject.changePiece(gameObject.turn));
            }
            else
            {
                // placed here for completeness this sound never happen
                ;
            }
        }

        // the current player has played and we switch the player's current turn
        gameObject.switchTurn();

        // delete the availableMoves for the current player which are dynamically allocated arrays
        for(int k = 0; k < totalMoveCount; ++k) 
        {
            for (int l = 0;l<8;++l) 
            {
                delete [] availableMoves[k][l];
            }
            delete [] availableMoves[k];
        }
        delete [] availableMoves;
    }

    // check who won and report it to the user
    gameObject.winCheck(gameObject.gameBoard);
    return 0;
}
