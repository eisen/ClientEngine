#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include "../include/OthelloClass.h"

using namespace std;

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

string colorInt2Str(int colorInt)
{
    if (colorInt == 0)
    {
        return "Empty";
    }
    else if (colorInt == 1)
    {
        return "Black";
    }
    else if (colorInt == 2)
    {
        return "White";
    }
    return "Error";
}

int main(int argc ,const char* args[]) 
{
    // construct the default board
    Board gameObject;
    string currentLine = "";
    string delimiter = ":";
    string turnStr = "";
    string colorStr = "";
    string boardStr = "";
    string token;
    int parseCount = 0;
    size_t pos = 0;

    if (argc < 2)
    {
        cout << "missing input file name" << endl;
        return 1;
    }
    string filename = args[1];
    //GameLogs/eli_6db9d4ae-1a14-4850-a5f8-2282c32eed59.txt

    if (filename != "")
    {
        ifstream inputFile(filename);
        //cout << filename << endl;
        if(!inputFile)
        {
            cout<<"Error opening output file"<<endl;
            system("pause");
            return -1;
        }
        getline(inputFile,currentLine);
        while(!inputFile.eof())
        {
            getline(inputFile,currentLine);
            //cout << currentLine << endl;
            parseCount = 0;

            while((pos = currentLine.find(delimiter)) != string::npos)
            {
                token = currentLine.substr(0,pos);
                if (parseCount == 0)
                {
                    colorStr = token;
                }
                else if (parseCount == 1)
                {
                    boardStr = token;
                }
                parseCount++;
                currentLine.erase(0,pos + delimiter.length());
            }
            turnStr = currentLine;

            cout << "Currently " << turnStr << " with the color " << flush;
            cout << colorInt2Str(stoi(colorStr)) << endl;
            
            gameObject.gameBoard = ToArray(boardStr);

            gameObject.display(gameObject.gameBoard,(gameObject.turn));
            cin.get();
        }

    }
    else
    {
        return 2;
    }

    return 0;
}
