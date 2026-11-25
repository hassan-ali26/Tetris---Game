#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Values and data used by everybody

const int ROWS = 20;
const int COLS = 10;
const float CELL = 30.f;                 // one cell is 30 x 30 pixels
const float BOARD_X = 20.f;              // top-left corner of the board
const float BOARD_Y = 20.f;

const int MENU = 0, PLAYING = 1, GAME_OVER = 2;
const int NEW_GAME = 0, CONTINUE = 1, EXIT = 2;

int  board[ROWS][COLS];      // 0 = empty, 1-7 = locked piece (piece number + 1)
char pieces[7][4][4];        // the 7 pieces which read from pieces.txt
char shape[4][4];            // rotating copy of the current piece

int  currentPiece = 0;       // 0-6
int  nextPiece = 0;       // 0-6
int  pieceRow = 0;       // top-left of the 4x4 box on the board
int  pieceCol = 3;

int  frameCounter = 0;
int  fallSpeed = 30;      // frames per row, level 1
bool gameOver = false;

int  score = 0, linesCleared = 0, level = 1;
int  gameState = MENU;