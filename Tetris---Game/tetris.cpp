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

// For drawing and input (owner A)
sf::Color colors[8] = { sf::Color(25, 25, 45),   // 0: empty cell
                        sf::Color::Cyan,         // 1: I
                        sf::Color::Yellow,       // 2: O
                        sf::Color(170, 90, 255), // 3: T (purple)
                        sf::Color::Green,        // 4: S
                        sf::Color::Red,          // 5: Z
                        sf::Color::Blue,         // 6: J
                        sf::Color(255, 150, 0) };// 7: L (orange)
sf::Font font;
bool   piecesOk = false;     // false if pieces.txt could not be read
char   pressedKey = 0;       // 'L' 'R' 'D' 'U' ' ' (drop), 'E' = Enter, 'X' = Esc
int    menuChoice = NEW_GAME;
string menuMessage = "";     // for example "No saved game"

// clearLines: after every lock, remove the full rows (all 10 cells not 0).
// Every row above moves down by one and the top row becomes empty.

int clearLines() {
    int removed = 0;
    int r = ROWS - 1;                          // start at the bottom row

    while (r >= 0) {
        bool full = true;                      // is every cell of this row used?
        for (int c = 0; c < COLS; c++)
            if (board[r][c] == 0) {
                full = false;
                break;
            }

        if (full) {
            for (int rr = r; rr > 0; rr--)     // every row above moves down by one
                for (int c = 0; c < COLS; c++)
                    board[rr][c] = board[rr - 1][c];
            for (int c = 0; c < COLS; c++)     // the top row becomes empty
                board[0][c] = 0;
            removed++;                         // do not change r: check this row again
        }
        else
            r--;                               // not full: go one row up
    }

    return removed;
}

// clearSaveFile: when a game ends, empty savegame.txt

void clearSaveFile() {
    ofstream fout("savegame.txt");             // opening for writing empties the file
}


// loadPieces: read pieces.txt into pieces[7][4][4]
// First the number of pieces, then every character with fin >> ...

bool loadPieces() {
    ifstream fin("pieces.txt");
    if (!fin) return false;                        // file is missing

    int howMany;
    fin >> howMany;
    if (howMany != 7) return false;

    for (int p = 0; p < 7; p++)
        for (int r = 0; r < 4; r++)
            for (int c = 0; c < 4; c++)
                fin >> pieces[p][r][c];

    return !fin.fail();                            // false if the file was short
}


// readEvents: read the window events and keep the pressed key in pressedKey

void readEvents(sf::RenderWindow& window) {
    pressedKey = 0;
    while (const optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();                        // X button ends the program
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            sf::Keyboard::Key k = key->code;
            if (k == sf::Keyboard::Key::Left || k == sf::Keyboard::Key::A)
                pressedKey = 'L';
            else if (k == sf::Keyboard::Key::Right || k == sf::Keyboard::Key::D)
                pressedKey = 'R';
            else if (k == sf::Keyboard::Key::Down || k == sf::Keyboard::Key::S)
                pressedKey = 'D';
            else if (k == sf::Keyboard::Key::Up || k == sf::Keyboard::Key::W)
                pressedKey = 'U';
            else if (k == sf::Keyboard::Key::Space)
                pressedKey = ' ';
            else if (k == sf::Keyboard::Key::Enter)
                pressedKey = 'E';
            else if (k == sf::Keyboard::Key::Escape)
                pressedKey = 'X';
        }
    }
}

// drawText / drawBlock: small helpers

void drawText(sf::RenderWindow& window, string str, float x, float y, int size, sf::Color color) {
    sf::Text text(font, str, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}

// one square, 2 pixels smaller than the cell so the grid shows
void drawBlock(sf::RenderWindow& window, float x, float y, float size, sf::Color color) {
    sf::RectangleShape block({ size - 2, size - 2 });
    block.setFillColor(color);
    block.setPosition({ x + 1, y + 1 });
    window.draw(block);
}

// drawBoard: border, then every cell with two nested loops

void drawBoard(sf::RenderWindow& window) {
    sf::RectangleShape border({ COLS * CELL, ROWS * CELL });
    border.setPosition({ BOARD_X, BOARD_Y });
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(2);
    border.setOutlineColor(sf::Color(120, 120, 180));
    window.draw(border);

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            drawBlock(window, BOARD_X + c * CELL, BOARD_Y + r * CELL, CELL, colors[board[r][c]]);
}

// drawPiece: the falling piece on top of the board

void drawPiece(sf::RenderWindow& window) {
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            if (shape[r][c] == '#')
                drawBlock(window, BOARD_X + (pieceCol + c) * CELL,
                    BOARD_Y + (pieceRow + r) * CELL, CELL, colors[currentPiece + 1]);
}
