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
#define NOHEURVAL 2147483645 // 2^31 -1
#define POSINF 2147483646 // 2^31 - 2
#define RETOVERHEAD 50 // overhead timing of return function in milliseconds
#define DEFAULT_TIME_PER_MOVE 1

using namespace std;
int Board::moveTime_ = DEFAULT_TIME_PER_MOVE;

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
    char ans1 = '\0', ans2 = '\0';
    float inputTime = DEFAULT_TIME_PER_MOVE;
    moveTime_ = (int)(1000*inputTime);
    while (ans1 != 'y' && ans1 != 'Y' && ans1 != 'n' && ans1 != 'N')
    {
        cout << "is Black Human? (y/n): " << flush;
        cin >> ans1;
    }

    while (ans2 != 'y' && ans2 != 'Y' && ans2 != 'n' && ans2 != 'N')
    {
        cout << "is White Human? (y/n): " << flush;
        cin >> ans2;
    }

    if ((ans1 == 'y' || ans1 == 'Y') && (ans2 == 'y' || ans2 == 'Y')) {
        cout << "Player 1 \033[47;30;7mBlack\033[0m is Human" << endl;
        cout << "Player 2 \033[40;37;7mWhite\033[0m is Human" << endl<<endl;
        return 0;
    }
    else if ((ans1 == 'y' || ans1 == 'Y') && (ans2 == 'n' || ans2 == 'N')) {
        cout << "Player 1 \033[47;30;7mBlack\033[0m is Human" << endl;
        cout << "Player 2 \033[40;37;7mWhite\033[0m is Computer" << endl <<endl;
        cout << "How much time should the AI take on its turn (in seconds): " << flush;
        cin >> inputTime;
        if(!inputTime || inputTime <= 0)
        {
            cout << "NOT A VALID ENTRY, Selecting AI move time to: " << DEFAULT_TIME_PER_MOVE << " Seconds" << endl;
        }
        else {
            moveTime_ = (int)(1000*inputTime);
        }
        cout << endl;
        return 1;
    }
    else if ((ans1 == 'n' || ans1 == 'N') && (ans2 == 'y' || ans2 == 'Y')) {
        cout << "Player 1 \033[47;30;7mBlack\033[0m is Computer" << endl;
        cout << "Player 2 \033[40;37;7mWhite\033[0m is Human" << endl<<endl;
        cout << "How much time should the AI take on its turn (in seconds): " << flush;
        cin >> inputTime;
        if(!inputTime || inputTime <= 0)
        {
            cout << "NOT A VALID ENTRY, Selecting AI move time to: 5 Seconds" << endl;
        }
        else {
            moveTime_ = (int)(1000*inputTime);
        }
        cout << endl;
        return 2;
    }
    else if ((ans1 == 'n' || ans1 == 'N') && (ans2 == 'n' || ans2 == 'N')) {
        cout << "Player 1 \033[47;30;7mBlack\033[0m is Computer" << endl;
        cout << "Player 2 \033[40;37;7mWhite\033[0m is Computer" << endl<<endl;
        cout << "How much time should the AI take on its turn (in seconds): " << flush;
        cin >> inputTime;
        if(!inputTime || inputTime <= 0)
        {
            cout << "NOT A VALID ENTRY, Selecting AI move time to: 5 Seconds" << endl;
        }
        else {
            moveTime_ = (int)(1000*inputTime);
        }
        cout << endl;
        return 3;
    }
    else {
        cout << "Unknown gameType" <<endl;
        return 5;
    }
}

// initializing the board if it is requested from the user
spaceState ** Board::initBoard()
{
    char filename[100];
    cout << "Do you want to input a non-default inital board state? (N/y): " << flush;
    cin.clear();
    cin.ignore(10000, '\n');
    if (tolower(cin.get()) == 'y')
    {
        cout << "Input the name of the file containing the board state: " << flush;
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> filename;
        gameBoard = getBoard(filename);
        //get file name and fill in board to gameBoard
    }
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
                    moveTime_ = (int)(1000*AI_Time);
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
int Board::moveSelect(int gameType, int movemax)
{
    struct timeval start_t, end_t;
    long mtime, seconds, useconds;
    int moveSelection = 0; // pass in &moveSelection for alphabet
    int tempmoveSelection = 0;
    int totalMovesLeft;
    string input;

    if(gameType == 0 || (gameType == 1 && turn == BLACK) || (gameType == 2 && turn == WHITE))
    {
        // request input from the VM Client for human player move
        cout << endl << "Player " << turn <<" Enter move: " << flush;
        NewIn:
        cin >> moveSelection;
        if(!moveSelection || moveSelection < 1 || moveSelection > movemax)
        {
            cout << "Not a Valid Entry, Select another move: " << flush;
            cin.clear();
            cin.ignore(10000, '\n');
            goto NewIn;
        }
    }
    else if (gameType == 3 || (gameType == 2 && turn == BLACK) || (gameType == 1 && turn == WHITE))
    {
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
    }
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

    if ((mtime + RETOVERHEAD > moveTime_) && v != POSINF && v != -1*POSINF) {
        return NOHEURVAL;
    }

    //delete AI moves somewhere where its not needed: after depth is searched?
    //modify AIMoves to only return a specific move number and if its NULL then
    //exit the loop  and return v
    nextMoves = AIMoves(brd,pt);
    if (d == 0 || nextMoves[0] == NULL) {
        if (pieceColor == BLACK) {
            v = heuristicFunction(brd,pieceColor);
        }
        if (pieceColor == WHITE) {
            v = heuristicFunction(brd,pieceColor);
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
                val++;
            }
            else if (inputBoard[i][j] != EMPTY)
            {
                val--;
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

int Board::heuristicFunction(spaceState** inputBoard, spaceState pieceColor)
{
    spaceState *** myMoves;
    spaceState *** oppMoves;

    //mp is my pieces
    //op is opp pieces

    //mc is my corner
    //oc is opp corner

    //ml is my corner closeness
    //ol is opp corner closeness

    //mm is my mobility
    //om is opp mobility

    //ms is my stable
    //os is opp stable
    //d is board benefit

    int p, m, d, l, s, c;
    int mp, op, mc, oc, ml, ol, mm, om, ms, os;
    mp = op = mc = oc = ml = ol = mm = om = ms = os = d = 0;
    spaceState opponent;
    spaceState stable[8][8];
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            stable[i][j] = EMPTY;
        }
    }
    opponent = changePiece(pieceColor);

    int V[8][8];
    //assigning board values
    V[0][0] = V[0][7] = V[7][0] = V[7][7] = 20;
    V[0][1] = V[1][0] = V[0][6] = V[6][0] = V[1][7] = V[7][1] = V[6][7] = V[7][6] = -3;
    V[0][2] = V[2][0] = V[0][5] = V[5][0] = V[2][7] = V[7][2] = V[5][7] = V[7][5] = 11;
    V[0][3] = V[3][0] = V[0][4] = V[4][0] = V[3][7] = V[7][3] = V[4][7] = V[7][4] = 8;
    V[1][1] = V[6][6] = V[6][1] = V[1][6] = -7;
    V[1][2] = V[2][1] = V[1][5] = V[5][1] = V[2][6] = V[6][2] = V[5][6] = V[6][5] = -4;
    V[3][1] = V[1][3] = V[4][1] = V[1][4] = V[3][6] = V[6][3] = V[6][4] = V[4][6] = 1;
    V[2][2] = V[2][3] = V[2][4] = V[2][5] = V[3][2] = V[4][2] = V[5][2] = 2;
    V[5][5] = V[5][4] = V[5][3] = V[4][5] = V[3][5] = 2;
    V[3][3] = V[4][4] = V[3][4] = V[4][3] = -3;

    //V[0] = {20, -3, 11,  8,  8, 11, -3, 20};
    //V[1] = {-3, -7, -4,  1,  1, -4, -7, -3};
    //V[2] = {11, -4,  2,  2,  2,  2, -4, 11};
    //V[3] = {8,   1,  2, -3, -3,  2,  1,  8};
    //V[4] = {8,   1,  2, -3, -3,  2,  1,  8};
    //V[5] = {11, -4,  2,  2,  2,  2, -4, 11};
    //V[6] = {-3, -7, -4,  1,  1, -4, -7, -3};
    //V[7] = {20, -3, 11,  8,  8, 11, -3, 20};

    // Piece difference, frontier disks and disk squares
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++)  {
            if (inputBoard[i][j] == pieceColor)  {
                d = d + V[i][j];
                mp++;
            }
            else if (inputBoard[i][j] == opponent)  {
                d = d - V[i][j];
                op++;
            }
        }
    }

    // Corner occupancy
    int maxcol;
    if(inputBoard[0][0] == pieceColor) {
        maxcol = 7;
        mc++;
        for(int i = 0; i < 8; i++) {
            if (inputBoard[i][0] != pieceColor) {
                break;
            }
            for (int j = 0; j < maxcol + 1;j++) {
                if (inputBoard[i][j] == pieceColor) {
                    stable[i][j] = pieceColor;
                }
                else {
                    maxcol = min(maxcol,j - 1);
                    break;
                }
            }
        }
    }
    else if(inputBoard[0][0] == opponent) {
        maxcol = 7;
        oc++;
        for(int i = 0; i < 8; i++) {
            if (inputBoard[i][0] != opponent) {
                break;
            }
            for (int j = 0; j < maxcol + 1;j++) {
                if (inputBoard[i][j] == opponent) {
                    stable[i][j] = opponent;
                }
                else {
                    maxcol = min(maxcol,j - 1);
                    break;
                }
            }
        }
    }
    if(inputBoard[0][7] == pieceColor) {
        maxcol = 0;
        mc++;
        for(int i = 0; i < 8; i++) {
            if (inputBoard[i][7] != pieceColor) {
                break;
            }
            for (int j = 7; j > maxcol-1;j--) {
                if (inputBoard[i][j] == pieceColor) {
                    stable[i][j] = pieceColor;
                }
                else {
                    maxcol = max(maxcol,j + 1);
                    break;
                }
            }
        }
    }
    else if(inputBoard[0][7] == opponent) {
        maxcol = 0;
        oc++;
        for(int i = 0; i < 8; i++) {
            if (inputBoard[i][7] != opponent) {
                break;
            }
            for (int j = 7; j > maxcol-1; j--) {
                if (inputBoard[i][j] == opponent) {
                    stable[i][j] = opponent;
                }
                else {
                    maxcol = max(maxcol,j + 1);
                    break;
                }
            }
        }

    }
    if(inputBoard[7][0] == pieceColor) {
        maxcol = 7;
        mc++;
        for(int i = 7; i > -1; i--) {
            if (inputBoard[i][0] != pieceColor) {
                break;
            }
            for (int j = 0; j < maxcol+1;j++) {
                if (inputBoard[i][j] == pieceColor) {
                    stable[i][j] = pieceColor;
                }
                else {
                    maxcol = min(maxcol,j - 1);
                    break;
                }
            }
        }
    }
    else if(inputBoard[7][0] == opponent) {
        maxcol = 7;
        oc++;
        for(int i = 7; i > -1; i--) {
            if (inputBoard[i][0] != opponent) {
                break;
            }
            for (int j = 0; j < maxcol+1;j++) {
                if (inputBoard[i][j] == opponent) {
                    stable[i][j] = opponent;
                }
                else {
                    maxcol = min(maxcol, j- 1);
                    break;
                }
            }
        }
    }
    if(inputBoard[7][7] == pieceColor) {
        maxcol = 0;
        mc++;
        for(int i = 7; i > -1; i--) {
            if (inputBoard[i][7] != pieceColor) {
                break;
            }
            for (int j = 7; j > maxcol-1;j--) {
                if (inputBoard[i][j] == pieceColor) {
                    stable[i][j] = pieceColor;
                }
                else {
                    maxcol= max(maxcol,j+1);
                    break;
                }
            }
        }
    }
    else if(inputBoard[7][7] == opponent) {
        maxcol = 0;
        oc++;
        for(int i = 7; i > -1; i--) {
            if (inputBoard[i][7] != opponent) {
                break;
            }
            for (int j = 7; j > maxcol-1;j--) {
                if (inputBoard[i][j] == opponent) {
                    stable[i][j] = opponent;
                }
                else {
                    maxcol = max(maxcol,j+1);
                    break;
                }
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (stable[i][j] == pieceColor) {
                ms++;
            }
            else if (stable[i][j] == opponent) {
                os++;
            }
        }
    }

    // Corner closeness
    if(inputBoard[0][0] == EMPTY)   {
        if(inputBoard[0][1] == pieceColor) {
            ml++;
        }
        else if(inputBoard[0][1] == opponent) {
            ol++;
        }
        if(inputBoard[1][1] == pieceColor) {
            ml++;
        }
        else if(inputBoard[1][1] == opponent) {
            ol++;
        }
        if(inputBoard[1][0] == pieceColor) {
            ml++;
        }
        else if(inputBoard[1][0] == opponent) {
            ol++;
        }
    }
    if(inputBoard[0][7] == EMPTY)   {
        if(inputBoard[0][6] == pieceColor) {
            ml++;
        }
        else if(inputBoard[0][6] == opponent) {
            ol++;
        }
        if(inputBoard[1][6] == pieceColor) {
            ml++;
        }
        else if(inputBoard[1][6] == opponent) {
            ol++;
        }
        if(inputBoard[1][7] == pieceColor) {
            ml++;
        }
        else if(inputBoard[1][7] == opponent) {
            ol++;
        }
    }
    if(inputBoard[7][0] == EMPTY)   {
        if(inputBoard[7][1] == pieceColor) {
            ml++;
        }
        else if(inputBoard[7][1] == opponent){
            ol++;
        }
        if(inputBoard[6][1] == pieceColor) {
            ml++;
        }
        else if(inputBoard[6][1] == opponent) {
            ol++;
        }
        if(inputBoard[6][0] == pieceColor) {
            ml++;
        }
        else if(inputBoard[6][0] == opponent) {
            ol++;
        }
    }
    if(inputBoard[7][7] == EMPTY)  {
        if(inputBoard[6][7] == pieceColor) {
            ml++;
        }
        else if(inputBoard[6][7] == opponent) {
            ol++;
        }
        if(inputBoard[6][6] == pieceColor) {
            ml++;
        }
        else if(inputBoard[6][6] == opponent) {
            ol++;
        }
        if(inputBoard[7][6] == pieceColor) {
            ml++;
        }
        else if(inputBoard[7][6] == opponent) {
            ol++;
        }
    }

    // Mobility check whose moves array we are looking at
    myMoves = AIMoves(inputBoard,pieceColor);
    oppMoves = AIMoves(inputBoard,opponent);
    mm = moveCount(myMoves);
    om = moveCount(oppMoves);

    for(int k = 0; k < mm; ++k) {
        for (int l = 0;l<8;++l) {
            delete [] myMoves[k][l];
        }
        delete [] myMoves[k];
    }
    delete [] myMoves;

    for(int k = 0; k < om; ++k) {
        for (int l = 0;l<8;++l) {
            delete [] oppMoves[k][l];
        }
        delete [] oppMoves[k];
    }
    delete [] oppMoves;

    int randm = 0;
    randm = (rand() % 10) - 4 - (rand() % 2); // add randomness to change the game up

    // final weighted score 633600 is the score for a perfect game
    /*
    mp = mp*900;
    op = op*900;
    ms = ms*900;
    os = os*900;
    mc = mc*14400;
    oc = oc*14400;
    mm = mm*1800;
    om = om*1800;
    ml = ml*4800;
    ol = ol*4800;
    d = d*160;
    */
    int score = 0;
    if ((mp - op) > 0) {
        p = 10000*(mp - op)/(mp+op);
        if (p == 10000)
        {
            score = score + 600000000;
        }
    }
    else if ((mp - op) < 0) {
        p = -10000*(op - mp)/(mp+op);
        if (p == -10000)
        {
            score = score - 600000000;
        }
    }
    else {
        p = 0;
    }

    if ((ms - os) > 0) {
        s = 10000*(ms - os)/(ms+os);
        if (ms > 32)
        {
            score = score + 600000000;
        }
    }
    else if ((ms - os) < 0) {
        s = -10000*(os - ms)/(ms+os);
        if (os > 32)
        {
            score = score - 600000000;
        }
    }
    else {
        s = 0;
    }

    if ((mc - oc) > 0) {
        c = 160000*(mc - oc)/(mc+oc);
    }
    else if ((mc - oc) < 0) {
        c = -160000*(oc - mc)/(mc+oc);
    }
    else {
        c = 0;
    }

    //reveresed for closeness
    if ((ml - ol) > 0) {
        l = -50000*(ml - ol)/(ml+ol);
    }
    else if ((ml - ol) < 0) {
        l = 50000*(ol - ml)/(ml+ol);
    }
    else {
        l = 0;
    }

    if ((mm - om) > 0) {
        m = 20000*(mm - om)/(mm+om);
    }
    else if ((mm - om) < 0) {
        m = -20000*(om - mm)/(mm+om);
    }
    else {
        m = 0;
    }
    d = d*1700;

    score = 10*p + 2000*s + 2000*c + 200*m + 400*l + 10*d + randm;

    // add no move bonus
    if (om == 0) {
        score = score + 2000*160000;
    }
  if (mm == 0) {
        score = score - 2000*160000;
    }
    //if both have no moves then they cancel


    //cout heurstic block
    /*
    cout <<endl;
    cout << "Player " << player << " has " << mp << " pieces taken" <<endl;
    cout << "Player " << opponent << " has " << op << " pieces taken" <<endl;
    cout << "Player " << player << " has " << mc << " corners" <<endl;
    cout << "Player " << opponent << " has " << oc << " corners" <<endl;
    cout << "Player " << player << " has " << ms << " stable pieces"<<endl;
    cout << "Player " << opponent << " has " << os << " stable pieces"<<endl;
    cout << "Player " << player << " has " << mm << " moves to play" <<endl;
    cout << "Player " << opponent << " has " << om << " moves to play"<<endl;
    cout << "Player " << player << " has " << ml << " corner closeness pieces"<<endl;
    cout << "Player " << opponent << " has " << ol << " corner closness pieces"<<endl;
    cout << "Random Number Generated: " << randm <<endl;
    cout << "Board Value Sum for player " << player << " is: " << d <<endl;
    cout << "Final Score: " << score << endl;
    */

    return score;
}