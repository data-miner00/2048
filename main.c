#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define SIZE 4
#define WIN_SCORE 2048

void spawnTile(int board[SIZE][SIZE]);
void printBoard(int board[SIZE][SIZE]);
void gameLoop(int board[SIZE][SIZE]);
int getInput();
int getNonZeroIndex(int currentIndex, int row[SIZE]);
void compactLeft(int row[SIZE]);
void mergeLeft(int row[SIZE]);
void moveLeft(int row[SIZE]);
void reverseRow(int row[SIZE]);
void moveRight(int row[SIZE]);
void getColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);
void setColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);
void moveUp(int board[SIZE][SIZE]);
void moveDown(int board[SIZE][SIZE]);
bool hasChanged(int oldBoard[SIZE][SIZE], int newBoard[SIZE][SIZE]);
bool hasWon(int board[SIZE][SIZE]);
bool canMove(int board[SIZE][SIZE]);

enum Direction { UP, DOWN, LEFT, RIGHT };

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int board[SIZE][SIZE] = {0};

    spawnTile(board);

    gameLoop(board);

    return 0;
}

bool hasChanged(int oldBoard[SIZE][SIZE], int newBoard[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (oldBoard[i][j] != newBoard[i][j]) {
                return true;
            }
        }
    }
    return false;
}

void spawnTile(int board[SIZE][SIZE]) {
    while (true) {
        int randomX = rand() % SIZE;
        int randomY = rand() % SIZE;

        if (board[randomY][randomX] != 0) {
            continue;
        }

        board[randomY][randomX] = rand() % 100 < 90 ? 2 : 4;
        break;
    }
}

void printBoard(int board[SIZE][SIZE]) {
    printf("Board:\n\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                printf("   . ");
            } else {
                printf("%4d ", board[i][j]);
            }
        }
        printf("\n");
    }
}

void gameLoop(int board[SIZE][SIZE]) {
    int input;
    int snapshot[SIZE][SIZE];

    while (true) {
        memcpy(snapshot, board, sizeof(int) * SIZE * SIZE); // Can also use sizeof(snapshot)

        printBoard(board);

        if (hasWon(board)) {
            printf("You win!\n");
            break;
        }

        if (!canMove(board)) {
            printf("You lose!\n");
            break;
        }

        input = getInput();

        if (input == LEFT) {
            for (int i = 0; i < SIZE; i++) {
                moveLeft(board[i]);
            }
        } else if (input == RIGHT) {
            for (int i = 0; i < SIZE; i++) {
                moveRight(board[i]);
            }
        } else if (input == UP) {
            moveUp(board);
        } else if (input == DOWN) {
            moveDown(board);
        }

        if (hasChanged(snapshot, board)) {
            spawnTile(board);
        }
    }
}

int getInput() {
    int input, discard;

    while (true) {
        input = getchar();
        while ((discard = getchar()) != '\n' && discard != EOF)
            ;

        if (input == EOF) {
            exit(0);
        }

        if (input == 'w' || input == 'W') {
            return UP;
        } else if (input == 's' || input == 'S') {
            return DOWN;
        } else if (input == 'a' || input == 'A') {
            return LEFT;
        } else if (input == 'd' || input == 'D') {
            return RIGHT;
        } else {
            printf("Invalid input\n");
        }
    }
}

int getNonZeroIndex(int currentIndex, int row[SIZE]) {
    // Find first non-zero index
    for (int i = currentIndex + 1; i < SIZE; i++) {
        if (row[i] != 0) {
            return i;
        }
    }
    return -1;
}

void compactLeft(int row[SIZE]) {
    // Compact to left
    for (int i = 0; i < SIZE - 1; i++) {
        int nonZeroIndex = getNonZeroIndex(i, row);
        if (row[i] == 0 && nonZeroIndex != -1) {
            row[i] = row[nonZeroIndex];
            row[nonZeroIndex] = 0;
        }
    }
}

// Adding same tiles in a row to left
void mergeLeft(int row[SIZE]) {
    for (int i = 0; i < SIZE - 1; i++) {
        if (row[i] != 0 && row[i] == row[i + 1]) {
            row[i] *= 2;
            row[i + 1] = 0;
        }
    }
}

void moveLeft(int row[SIZE]) {
    compactLeft(row);
    mergeLeft(row);
    compactLeft(row);
}

void reverseRow(int row[SIZE]) {
    for (int i = 0; i < SIZE / 2; i++) {
        int temp = row[i];
        row[i] = row[SIZE - i - 1];
        row[SIZE - i - 1] = temp;
    }
}

void moveRight(int row[SIZE]) {
    reverseRow(row);
    moveLeft(row);
    reverseRow(row);
}

void getColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        buffer[i] = board[i][column];
    }
}

void setColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        board[i][column] = buffer[i];
    }
}

void moveUp(int board[SIZE][SIZE]) {
    int buffer[SIZE];
    for (int i = 0; i < SIZE; i++) {
        getColumn(board, i, buffer);
        moveLeft(buffer);
        setColumn(board, i, buffer);
    }
}

void moveDown(int board[SIZE][SIZE]) {
    int buffer[SIZE];
    for (int i = 0; i < SIZE; i++) {
        getColumn(board, i, buffer);
        moveRight(buffer);
        setColumn(board, i, buffer);
    }
}

bool hasWon(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == WIN_SCORE) {
                return true;
            }
        }
    }
    return false;
}

// For optimization, can just compare one horizontal and one vertical neighbor, as eventually all
// will be compared Commented out 2 perpendicular checks for now
bool canMove(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                return true;
            }
            // Check top
            if (i > 0 && board[i - 1][j] == board[i][j]) {
                return true;
            }
            // Check bottom
            // if (i < SIZE - 1 && board[i + 1][j] == board[i][j]) {
            //     return true;
            // }
            // Check left
            if (j > 0 && board[i][j - 1] == board[i][j]) {
                return true;
            }
            // Check right
            // if (j < SIZE - 1 && board[i][j + 1] == board[i][j]) {
            //     return true;
            // }
        }
    }
    return false;
}
