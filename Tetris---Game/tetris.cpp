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