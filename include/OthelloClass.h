#include <string>
#include <cstring>

enum spaceState{
    EMPTY,
    BLACK,
    WHITE
};

class Board
{
public:
    spaceState ** gameBoard;
    spaceState turn;
    int state;

    void setState(int);
    spaceState ** getBoard(char*);
    spaceState *** legalMoves(spaceState **,spaceState);
    spaceState changePiece(spaceState);

    
    int movesLeft(void);
    void switchTurn(void);
    
    int moveCount(spaceState ***);
    int winCheck(spaceState**);
    int init(void);
    spaceState** initBoard(void);
    void reset(int);
    void display(spaceState**,spaceState);
    int isLegalMove(spaceState **,spaceState,int,int);
    void setBoard(spaceState **);
    Board(void);    //constructor function
    ~Board(void); // destructor function

    // Select functions for AI computation and operation
    // overall input is all the inputs to alphabeta
    int moveSelect(int);
    int alphabeta(spaceState**, int, int, int, spaceState, spaceState, int*, timeval, timeval);
    spaceState *** AIMoves(spaceState **,spaceState);
    spaceState ** pseudoplay(spaceState**, int,int, spaceState);
    int heuristicFunction(spaceState **,spaceState);
};
