#ifndef BOARD_H
#define BOARD_H
#include <stdbool.h>

// make these to get from optional params
#define SIZE 4
#define WIN_SCORE 2048

enum GameState { WIN, LOSE, EXIT };

// board
bool spawnTile(int board[SIZE][SIZE]);
void printBoard(int board[SIZE][SIZE], int score);
enum GameState gameLoop(int board[SIZE][SIZE]);

void getColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);
void setColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);

void compactLeft(int row[SIZE]);
int mergeLeft(int row[SIZE]);
void reverseRow(int row[SIZE]);

int moveUp(int board[SIZE][SIZE]);
int moveDown(int board[SIZE][SIZE]);
int moveLeft(int row[SIZE]);
int moveRight(int row[SIZE]);
int getNonZeroIndex(int currentIndex, int row[SIZE]);

bool hasChanged(int oldBoard[SIZE][SIZE], int newBoard[SIZE][SIZE]);
bool hasWon(int board[SIZE][SIZE]);
bool canMove(int board[SIZE][SIZE]);

#endif // BOARD_H
