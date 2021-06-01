#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <unistd.h>
#include "../include/OthelloClass.h"
#define NOHEURVAL 2147483646 // 2^31 -2
#define POSINF 2147483645 // 2^31 - 3
#define RETOVERHEAD 50 // overhead timing of return function in milliseconds
#define DEFAULT_TIME_PER_MOVE 1

using namespace std;
static int moveTime = 3000;

// Default initializer for the board Class
Board::Board(void)
{
    gameBoard = new spaceState *[8];
    for (int i = 0; i < 8; i++) {
        gameBoard[i] = new spaceState [8];
    }
    for(int i = 0;i<8;i++) {
        for(int j=0;j<8;j++) {
            gameBoard[i][j] = EMPTY;
        }
    }
    gameBoard[4][3] = BLACK;
    gameBoard[3][4] = BLACK;
    gameBoard[3][3] = WHITE;
    gameBoard[4][4] = WHITE;
    state = 0;
    turn = BLACK;
}

// Default destructor for the board class
Board::~Board(void)
{
    ;
}

// is the input move one of the legal moves?
// This function is used to determine the number to display on the board
int Board::isLegalMove(spaceState ** inputBoard, spaceState pieceColor, int row, int column)
{
    //pointer to a pointer to a pointer of an 64x8x8 spaceStates
    int legalMoveSelection = 0;
    int rowCounter         = 0;
    int columnCounter      = 0;
    int xchange            = 0;
    int ychange            = 0;
    int rowIterator        = 0;
    int moveCount          = 0;

    for (rowCounter = 0;rowCounter<8;rowCounter++)
    {
        for (columnCounter = 0;columnCounter<8;columnCounter++)
        {
            if (inputBoard[rowCounter][columnCounter] == EMPTY)
            {
                for(xchange = -1;xchange < 2;xchange++)
                {
                    for (ychange = -1;ychange < 2;ychange++)
                    {
                        //if tile is a valid board space
                        if (rowCounter+xchange >= 0 && rowCounter+xchange <=7 && columnCounter+ychange >= 0 && columnCounter+ychange <=7 )
                        {
                            //if the next tile is empty or the same as the players piece
                            if(pieceColor == inputBoard[rowCounter+xchange][columnCounter+ychange] ||
                               inputBoard[rowCounter+xchange][columnCounter+ychange] == EMPTY)
                            {
                                ;//do nothing since cant move do to this direction
                            }
                            else //the next tile is the opponents piece
                            {

                                //search for a piece of the same type in a direction
                                for (rowIterator=2;rowIterator<8;rowIterator++)
                                {
                                    //if tile is a valid board space
                                    if(rowCounter+rowIterator*xchange >= 0 && rowCounter+rowIterator*xchange <=7 &&
                                       columnCounter+rowIterator*ychange >= 0 && columnCounter+rowIterator*ychange <=7 )
                                    {
                                        // if a same piece is found in direction
                                        if(inputBoard[rowCounter+rowIterator*xchange][columnCounter+rowIterator*ychange] == pieceColor)
                                        {
                                            moveCount++;
                                            if (rowCounter == row && columnCounter == column)
                                            {
                                                legalMoveSelection = moveCount;
                                            }

                                            // This move is determined to be legal and we can move on to the next boardSpace to see if it is legal
                                            goto NextBoardSpace;
                                        }
                                        //if empty spot found in direction
                                        else if (inputBoard[rowCounter+rowIterator*xchange][columnCounter+rowIterator*ychange] == EMPTY)
                                        {
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
NextBoardSpace:
            ;
        }
    }
    return legalMoveSelection;
}

// display the board using command shell coloring
void Board::display(spaceState ** inputBoard,spaceState pieceColor)
{
    int moveNumber;
    ostringstream displayStream;
    string numStr1;
    string numStr2;

    // top of the row with all empty spaces
    static char topRow[] =  "   \033[40;32;2;7m|    ||    |"
                               "\033[40;32;2;7m|    ||    |"
                               "\033[40;32;2;7m|    ||    |"
                               "\033[40;32;2;7m|    ||    |\033[0m\n";

    // Bottom of the row with all empty spaces
    static char bottomRow[] = "   \033[40;32;2;7m|____||____|"
                            "\033[40;32;2;7m|____||____|"
                            "\033[40;32;2;7m|____||____|"
                            "\033[40;32;2;7m|____||____|\033[0m\n";

    // Top row border
    static char topBorder[] = "   \033[40;32;2;7m____________"
                            "\033[40;32;2;7m____________"
                            "\033[40;32;2;7m____________"
                            "\033[40;32;2;7m____________\033[0m\n";

    // An individual spot display
    static char spot[] = "\033[40;32;2;7m ";

    // Black Piece Spot display
    static char blackPiece[] = "\033[47;30;7m  ";

    // White Piece Spot display
    static char whitePiece[] = "\033[40;37;7m  ";

    // Black player move location display
    static char blackMoveOption[] = "\033[47;32;2;7m";

    // White player move location display
    static char whiteMoveOption[] = "\033[40;32;2;7m";

    // Empty board space display
    static char emptySpace[] = "\033[0m";

    // showing which player is what color
    // cout << endl << "Player 1 -> \033[47;30;7mBlack" << normal << endl;
    // cout << "Player 2 -> \033[40;37;7mWhite" << normal << endl << endl;

    // display top boarder with numbers
    cout << "     00    01    02    03    04    05    06    07"<<endl;
    cout << topBorder;

    // for each row diplay the relevant row
    for (int rowCounter = 0; rowCounter < 8; rowCounter++)
    {
        // print the top row
        cout << topRow;

        // print the row counter on the left
        cout << " " << rowCounter << " ";

        // display each column in the row accordingly
        for (int columnCounter = 0; columnCounter < 8; columnCounter++)
        {
            // column empty space
            cout << "\033[40;32;2;7m|" << spot << emptySpace;

            // if the game board shows there is a black piece in the current row / column board space
            if (gameBoard[rowCounter][columnCounter] == BLACK)
            {
                cout << blackPiece;
            }

            // else if the game board shows there is a white piece in the current row / column board space
            else if (gameBoard[rowCounter][columnCounter] == WHITE)
            {
                cout << whitePiece;
            }

            // else if the game board shows this is a valid move for the current player's turn
            else if ((moveNumber = isLegalMove(inputBoard,pieceColor,rowCounter,columnCounter)) != 0) {
                displayStream.str("");
                displayStream.clear();
                if (floor(moveNumber/10) != 0) {
                    displayStream << floor(moveNumber/10);
                    numStr1 = displayStream.str();
                }
                else {
                    displayStream << " ";
                    numStr1 = displayStream.str();
                }
                displayStream.str("");
                displayStream.clear();
                displayStream << (moveNumber % 10);
                numStr2 = displayStream.str();

                if (pieceColor == WHITE)
                {
                    cout << blackMoveOption << numStr1 << numStr2;
                }
                else
                {
                    cout << whiteMoveOption << numStr1 << numStr2;
                }
            }
            else {
                cout << spot << spot << emptySpace;
            }
            cout << spot << "|";
    }
    cout << emptySpace << endl;
    cout << bottomRow;
  }
}

// set the game state
void Board::setState(int s)
{
    // state 0 is game is active
    // state 1 is game is over
    state = s;
}

// initialize the game by requesting information from the user and starting the game
int Board::init(void)
{
    float inputTime = 0;
    cout << "How much time should the AI take on its turn (in seconds): " << flush;
    inputTime = DEFAULT_TIME_PER_MOVE;
    cout << inputTime << endl;
    moveTime = (int)(1000*inputTime);
    cout << endl;
    return 3;
}

// initializing the board if it is requested from the user
spaceState ** Board::initBoard()
{
    char filename[100];
    cout << "Do you want to input a non-default inital board state? (N/y): " << flush;
    cout << "N" << endl;
    // cin.clear();
    // cin.ignore(10000, '\n');
    // if (tolower(cin.get()) == 'y')
    // {
    //     cout << "Input the name of the file containing the board state: " << flush;
    //     cin.clear();
    //     cin.ignore(10000, '\n');
    //     cin >> filename;
    //     gameBoard = getBoard(filename);
    //     //get file name and fill in board to gameBoard
    // }
    return gameBoard;
}

// get an input board from the input filename
spaceState ** Board::getBoard(char * filename)
{

    int lineCount = 0;
    int columnLineCount = 0;
    int pieceCount = 0;
    float AI_Time = 2;
    spaceState ** outBoard;
    outBoard = new spaceState *[8];
    for (int i = 0; i < 8; i++) {
        outBoard[i] = new spaceState [8];
    }

    cout << endl << "Opening and initializing based on file " << filename << endl;
    ifstream inFile(filename);

    string line;
    while (getline(inFile, line))
    {
            lineCount++;
            istringstream iss(line);
            if (lineCount > 0 && lineCount < 9)
            {
                columnLineCount = 0;
                pieceCount = 0;
                while (pieceCount < 9) {
                    if(line[columnLineCount] == '0' )
                    {
                        outBoard[lineCount-1][pieceCount] = EMPTY;
                        pieceCount++;
                    }
                    else if ( line[columnLineCount] == '1') {
                        outBoard[lineCount-1][pieceCount] = BLACK;
                        pieceCount++;
                    }
                    else if (line[columnLineCount] == '2') {
                        outBoard[lineCount-1][pieceCount] = WHITE;
                        pieceCount++;
                    }
                    columnLineCount++;
                }
            }
            else if (lineCount == 9)
            {
                columnLineCount = 0;
                pieceCount = 0;
                while (pieceCount < 1)
                {
                    if(line[columnLineCount] == '1')
                    {
                        turn = BLACK;
                        pieceCount++;
                    }
                    else if (line[columnLineCount] == '2')
                    {
                        turn = WHITE;
                        pieceCount++;
                    }
                    columnLineCount++;
                }
            }
            else if (lineCount == 10)
            {
                //need to convert string to float
                AI_Time = atof(line.c_str());
                if( !AI_Time || AI_Time <= 0)
                {
                    cout << "NOT A VALID ENTRY, Selecting AI move time to: 2 Seconds" << endl;
                }
                else {
                    moveTime = (int)(1000*AI_Time);
                    cout << "Computer Time: " << AI_Time << " Seconds" << endl;
                }
            }
    }
    cout << endl;
    return outBoard;
}

// set the object's board to the input board
void Board::setBoard(spaceState ** inputBoard)
{
    for(int row = 0; row < 8; row++)
    {
        for(int column = 0; column < 8; column++)
        {
            gameBoard[row][column] = inputBoard[row][column];
        }
    }
}

// input the current player and the current board
// output the list of legal moves for the current player
spaceState *** Board::legalMoves(spaceState** inputBoard, spaceState pieceColor)
{
    //pointer to a pointer to a pointer of an 64x8x8 spaceStates
    spaceState*** moves = new spaceState**[64];

    // initialize the legal moves array and set the pointers to NULL
    for(int k = 0; k < 64; k++) {
        moves[k] = NULL;
    }

    // initialize counter variables
    int rowCounter    = 0;
    int columnCounter = 0;
    int xchange       = 0;
    int ychange       = 0;
    int rowIterator   = 0;
    int moveCounter   = 0;

    // go through the board and assemble the legal moves
    for (rowCounter = 0;rowCounter<8;rowCounter++)
    {
        for (columnCounter = 0;columnCounter<8;columnCounter++)
        {
            // if the current space is not empty it is not a legal move
            if (inputBoard[rowCounter][columnCounter] == EMPTY)
            {
                for(xchange = -1;xchange < 2;xchange++)
                {
                    for (ychange = -1;ychange < 2;ychange++)
                    {
                        //if the current space we are checking is a valid board space
                        if (rowCounter+xchange >= 0 && rowCounter+xchange <=7 && columnCounter+ychange >= 0 && columnCounter+ychange <=7 )
                        {
                            //if the next space is empty or is the same as the current player's piece
                            if(pieceColor == inputBoard[rowCounter+xchange][columnCounter+ychange] || inputBoard[rowCounter+xchange][columnCounter+ychange] == EMPTY)
                            {
                                ;//do nothing since cant move do to this direction
                            }

                            //the next tile is the opponents piece
                            else
                            {
                                //search for a piece of the same type in a direction
                                for (rowIterator=2;rowIterator<8;rowIterator++)
                                {
                                    //if the current space we are checking is a valid board space
                                    if(rowCounter+rowIterator*xchange >= 0 && rowCounter+rowIterator*xchange <=7 && columnCounter+rowIterator*ychange >= 0 && columnCounter+rowIterator*ychange <=7 )
                                    {
                                        //if a same piece is found in a certain direction
                                        if(inputBoard[rowCounter+rowIterator*xchange][columnCounter+rowIterator*ychange] == pieceColor)
                                        {
                                            // we found a legal move
                                            // cout << (moveCounter+1) << ". move at (" << rowCounter << "," << columnCounter << ")" <<  endl;
                                            moves[moveCounter] = pseudoplay(inputBoard,rowCounter,columnCounter,pieceColor);
                                            moveCounter++;
                                            moves[moveCounter] = NULL;
                                            goto NextBoardSpace;
                                        }
                                        //if empty spot found in direction then this is not a valid move
                                        else if (inputBoard[rowCounter+rowIterator*xchange][columnCounter+rowIterator*ychange] == EMPTY)
                                        {
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
NextBoardSpace:
            ;
        }
    }
    return moves;
}

// return a count of moves given the array of available moves
int Board::moveCount(spaceState *** moves)
{
    int count;
    for(count = 0; moves[count] != NULL; count++)
    {
        ;
    }
    return count;
}

// rotate a piece from black to white or white to black
spaceState Board::changePiece(spaceState pieceColor)
{
    spaceState newPiece;
    if (pieceColor == WHITE)
    {
        newPiece = BLACK;
    }
    else
    {
        newPiece = WHITE;
    }

    return newPiece;
}

// switch the current player's turn
void Board::switchTurn(void)
{
    if (turn == BLACK)
    {
        turn = WHITE;
    }
    else if (turn == WHITE)
    {
        turn = BLACK;
    }
}

// count the amount of white pieces and count the amount of black pieces
// compare which one is larger and report that they won
int Board::winCheck(spaceState** inputBoard)
{
    // initial counts
    int whiteCount = 0;
    int blackCount = 0;

    for(int row = 0; row < 8; row++)
    {
        for(int column = 0; column < 8; column++)
        {
            if (inputBoard[row][column] == WHITE)
            {
                whiteCount++;
            }
            else if (inputBoard[row][column] == BLACK)
            {
                blackCount++;
            }
        }
    }
    if (blackCount > whiteCount)
    {
        cout << "black wins: " << blackCount << " - " << whiteCount <<  endl;
        return 1;
    }
    else if (whiteCount > blackCount)
    {
        cout << "white wins: " << whiteCount << " - " << blackCount << endl;
        return 2;
    }
    else if (whiteCount == blackCount)
    {
        cout << "tie: " << whiteCount << " - " << blackCount << endl;
        return 3;
    }
    else
    {
        return 0;
    }
}

// return the number of empty spaces left on the board
int Board::movesLeft(void)
{
    int numMovesLeft = 0;
    for(int row = 0; row < 8; row++)
    {
        for(int column = 0; column < 8; column++)
        {
            if(gameBoard[row][column] == EMPTY)
            {
                numMovesLeft++;
            }
        }
    }
    return numMovesLeft;
}

// ----- EVERYTHING ABOVE THIS DOES NOT NEED TO BE MODIFIED -----

// This is the function that needs to be requested from the client to the server
int Board::moveSelect(int movemax)
{
    struct timeval start_t, end_t;
    long mtime, seconds, useconds;
    int moveSelection = 0; // pass in &moveSelection for alphabet
    int tempmoveSelection = 0;
    int totalMovesLeft;
    string input;

    //cout << endl << turn <<"'s turn: " << endl;

    //prune goes in here and check time
    int depth = 1;
    int a = -1*POSINF;
    int b = POSINF;
    int hval;

    spaceState cplayer = turn;
    totalMovesLeft = movesLeft();
    gettimeofday(&start_t,NULL);
    gettimeofday(&end_t,NULL);
    seconds = end_t.tv_sec - start_t.tv_sec;
    useconds = end_t.tv_usec - start_t.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0);

    while (1) {
        //figure out what moveSelection we should chose
        if (movemax == 1) {
            moveSelection = 1;
            break;
        }
        hval = alphabeta(gameBoard, depth, a, b, turn, cplayer, &tempmoveSelection, start_t, end_t);
        if (hval == NOHEURVAL || depth > totalMovesLeft) {
            break;
        }
        else {
            moveSelection = tempmoveSelection + 1;
            if (depth > 5 && depth < 8)
            {
                cout << "At depth: " << depth << " move number: " << moveSelection << " hval: " << hval << endl;
            }
            depth++;
        }

    }

    gettimeofday(&end_t,NULL);
    seconds = end_t.tv_sec - start_t.tv_sec;
    useconds = end_t.tv_usec - start_t.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0);

    //cout << "At depth: " << depth-1 << ", Selecting Move: " << moveSelection << endl;
    //cout << "Elapsed time: " << mtime << " milliseconds" <<endl;

    return moveSelection;
}

// This is the alphabeta algorithm called by moveSelect
// Move select will be ported to a separate piece of software which will be developed by the VM users
int Board::alphabeta(spaceState ** brd, int d, int a, int b, spaceState pieceColor, spaceState pt, int* ind, timeval start_t, timeval end_t)
{
    //ind is the pointer to moveSelection only passed in when first called
    int tempv;
    int v = 0;
    int i = 0;
    long mtime, seconds, useconds;
    spaceState *** nextMoves;

    gettimeofday(&end_t,NULL);
    seconds = end_t.tv_sec - start_t.tv_sec;
    useconds = end_t.tv_usec - start_t.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0);

    if (mtime + RETOVERHEAD > moveTime) {
        return NOHEURVAL;
    }

    //delete AI moves somewhere where its not needed: after depth is searched?
    //modify AIMoves to only return a specific move number and if its NULL then
    //exit the loop  and return v
    nextMoves = AIMoves(brd,pt);
    if (d == 0 || nextMoves[0] == NULL) {
        if (pieceColor == BLACK) {
            v = heuristicFunction0(brd,pieceColor);
        }
        if (pieceColor == WHITE) {
            v = heuristicFunction1(brd,pieceColor);
        }
        while (nextMoves[i] != NULL)
        {
            i++;
        }
        for(int k = 0; k < i; ++k) {
            for (int l = 0;l<8;++l) {
                delete [] nextMoves[k][l];
            }
            delete [] nextMoves[k];
        }
        delete [] nextMoves;
    }
    else if (pt == pieceColor) {
        v = -1*POSINF;
        pt = changePiece(pt);
        while (nextMoves[i] != NULL)
        {
            tempv = alphabeta(nextMoves[i], d - 1, a, b, pieceColor, pt, NULL,start_t,end_t);
            if (v < tempv) {
                v = tempv;
                if (ind != NULL) {
                    *ind = i;
                }
            }
            a = (a < v) ? v : a;
            if (b <= a)
                break;
            i++;
        }
        while (nextMoves[i] != NULL)
        {
            i++;
        }
        for(int k = 0; k < i; ++k) {
            for (int l = 0;l<8;++l) {
                delete [] nextMoves[k][l];
            }
            delete [] nextMoves[k];
        }
        delete [] nextMoves;
    }
    else {
        v = POSINF;
        pt = changePiece(pt);
        while (nextMoves[i] != NULL)
        {
            tempv = alphabeta(nextMoves[i], d - 1, a, b, pieceColor, pt, NULL, start_t, end_t);
            if (v >= tempv) {
                v = tempv;
            }
            b = (b < v) ? b : v;
            if (b <= a)
                break;
            i++;
        }
        while (nextMoves[i] != NULL)
        {
            i++;
        }
        for(int k = 0; k < i; ++k) {
            for (int l = 0;l<8;++l) {
                delete [] nextMoves[k][l];
            }
            delete [] nextMoves[k];
        }
        delete [] nextMoves;
    }
    return v;
}

// pass in the current board and current player's turn
// output the list of moves for the AI
spaceState *** Board::AIMoves(spaceState** inputBoard, spaceState pieceColor)
{
    //pointer to a pointer to a pointer of an 64x8x8 spaceStates
    spaceState*** moves = new spaceState**[64];
    for(int k = 0; k < 64; ++k)
    {
        moves[k] = NULL;
    }

    moves[0] = NULL;

    int rowCounter    = 0;
    int columnCounter = 0;
    int xchange       = 0;
    int ychange       = 0;
    int rowIterator   = 0;
    int moveCounter   = 0;

    for (rowCounter = 0;rowCounter<8;rowCounter++)
    {
        for (columnCounter = 0;columnCounter<8;columnCounter++)
        {
            if (inputBoard[rowCounter][columnCounter] == EMPTY)
            {
                for(xchange = -1;xchange < 2;xchange++)
                {
                    for (ychange = -1;ychange < 2;ychange++)
                    {

                        //if tile is a valid board space
                        if (rowCounter+xchange >= 0 &&
                            rowCounter+xchange <=7 &&
                            columnCounter+ychange >= 0 &&
                            columnCounter+ychange <=7 )
                        {
                            //if the next tile is empty or the same as the players piece
                            if(pieceColor == inputBoard[rowCounter+xchange][columnCounter+ychange] ||
                               inputBoard[rowCounter+xchange][columnCounter+ychange] == EMPTY)
                            {
                                ; //do nothing since cant move do to this direction
                            }
                            //the next tile is the opponents piece
                            else
                            {
                                //search for a piece of the same type in a direction
                                for (rowIterator = 2;rowIterator < 8; rowIterator++)
                                {
                                    //if tile is a valid board space
                                    if(rowCounter+rowIterator*xchange >= 0 && rowCounter+rowIterator*xchange <=7 && columnCounter+rowIterator*ychange >= 0 && columnCounter+rowIterator*ychange <=7 )
                                    {
                                        //if a same piece is found in direction
                                        if(inputBoard[rowCounter+rowIterator*xchange][columnCounter+rowIterator*ychange] == pieceColor)
                                        {
                                            moves[moveCounter] = pseudoplay(inputBoard,rowCounter,columnCounter,pieceColor);
                                            moveCounter++;
                                            moves[moveCounter] = NULL;
                                            goto NextAIBoardSpace;
                                        }

                                        //if empty spot found in direction
                                        else if (inputBoard[rowCounter+rowIterator*xchange][columnCounter+rowIterator*ychange] == EMPTY)
                                        {
                                            break;
                                        }

                                    }
                                    else {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            NextAIBoardSpace:
            ;
        }
    }
    return moves;
}

// input the current board, current player's turn, and selected move on the board
// output the resulting board IF we are playing in the selected spot
spaceState ** Board::pseudoplay(spaceState** inputBoard, int rowSelect,int columnSelect, spaceState pieceColor)
{
    int xchange       = 0;
    int ychange       = 0;
    int rowCounter    = 0;
    int columnCounter = 0;

    spaceState** pseudoboard = new spaceState*[8];
    for(int k = 0; k < 8; k++)
    {
        pseudoboard[k] = new spaceState[8]();
    }

    for (int k = 0; k < 8; k++)
    {
        for (int l=0; l < 8; l++)
        {
            pseudoboard[k][l] = inputBoard[k][l];
        }
    }

    for(xchange = -1;xchange < 2;xchange++)
    {
        for (ychange = -1;ychange < 2;ychange++)
        {
            //if tile is a valid board space
            if (rowSelect+xchange >= 0 &&
                rowSelect+xchange <= 7 &&
                columnSelect+ychange >= 0 &&
                columnSelect+ychange <= 7 )
            {
                //if the next tile is empty or the same as the players piece
                if(pieceColor == inputBoard[rowSelect+xchange][columnSelect+ychange] ||
                   inputBoard[rowSelect+xchange][columnSelect+ychange] == EMPTY)
                {
                    ; //do nothing since cant move do to this direction
                }
                //the next tile is the opponents piece
                else
                {
                    //search for a piece of the same type in a direction
                    for (rowCounter=2; rowCounter < 8; rowCounter++)
                    {
                        //if tile is a valid board space
                        if(rowSelect+rowCounter*xchange >= 0 &&
                           rowSelect+rowCounter*xchange <=7 &&
                           columnSelect+rowCounter*ychange >= 0 &&
                           columnSelect+rowCounter*ychange <=7 )
                        {
                            //if a same piece is found in direction
                            if(inputBoard[rowSelect+rowCounter*xchange][columnSelect+rowCounter*ychange] == pieceColor)
                            {
                                for(columnCounter = 0; columnCounter < rowCounter;columnCounter++)
                                {
                                    pseudoboard[rowSelect+columnCounter*xchange][columnSelect+columnCounter*ychange] = pieceColor;
                                }
                                break;
                            }
                            //if empty spot found in direction
                            else if (inputBoard[rowSelect+rowCounter*xchange][columnSelect+rowCounter*ychange] == EMPTY)
                            {
                                break;
                            }
                        }
                        else {
                            break;
                        }
                    }
                }
            }
        }
    }
    return pseudoboard;
}


// input takes in the current player's turn and the current game board
// output a zero sum value representative of the who is winning at that moment
int Board::heuristicFunction0(spaceState** inputBoard, spaceState pieceColor)
{
    //ensure the heurstic is zero sum!
    int val = 0;
    for(int i = 0;i<8;i++) {
        for(int j=0;j<8;j++) {
            if (inputBoard[i][j] == pieceColor)
            {
                val--;
            }
            else if (inputBoard[i][j] != EMPTY)
            {
                val++;
            }
        }
    }

    val = val*10 + (rand() % 10);
    return val;
}

int Board::heuristicFunction1(spaceState** inputBoard, spaceState pieceColor)
{
    //ensure the heurstic is zero sum!
    int val = 0;
    int corner = 0;
    for(int i = 0;i<8;i++) {
        for(int j=0;j<8;j++) {
            if (inputBoard[i][j] == pieceColor)
            {
                val++;
            }
            else if (inputBoard[i][j] != EMPTY)
            {
                val--;
            }
        }
    }
    if (inputBoard[0][0] == pieceColor)
    {
        corner++;
    }
    else if (inputBoard[0][0] != EMPTY)
    {
        corner--;
    }
        if (inputBoard[0][0] == pieceColor)
    {
        corner++;
    }
    else if (inputBoard[0][0] != EMPTY)
    {
        corner--;
    }
        if (inputBoard[0][0] == pieceColor)
    {
        corner++;
    }
    else if (inputBoard[0][7] != EMPTY)
    {
        corner--;
    }
        if (inputBoard[7][0] == pieceColor)
    {
        corner++;
    }
    else if (inputBoard[7][7] != EMPTY)
    {
        corner--;
    }

    val = val*10 + (rand() % 10) + corner*300;
    return val;
}