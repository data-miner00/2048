#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define WIDTH 4
#define HEIGHT 4
#define WIN_SCORE 2048

void spawnTile(int board[WIDTH][HEIGHT]);
void printBoard(int board[WIDTH][HEIGHT]);
void gameLoop(int board[WIDTH][HEIGHT]);
int getInput();
int getNonZeroIndex(int currentIndex, int row[WIDTH]);
void compactLeft(int row[WIDTH]);
void mergeLeft(int row[WIDTH]);
void moveLeft(int row[WIDTH]);
void reverseRow(int row[WIDTH]);
void moveRight(int row[WIDTH]);
void getColumn(int board[WIDTH][HEIGHT], int column, int buffer[WIDTH]);
void setColumn(int board[WIDTH][HEIGHT], int column, int buffer[WIDTH]);
void moveUp(int board[WIDTH][HEIGHT]);
void moveDown(int board[WIDTH][HEIGHT]);
bool hasChanged(int oldBoard[WIDTH][HEIGHT], int newBoard[WIDTH][HEIGHT]);
bool hasWon(int board[WIDTH][HEIGHT]);
bool canMove(int board[WIDTH][HEIGHT]);

enum Direction { UP, DOWN, LEFT, RIGHT };

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int board[WIDTH][HEIGHT] = {0};

    spawnTile(board);

    gameLoop(board);

    return 0;
}

bool hasChanged(int oldBoard[WIDTH][HEIGHT], int newBoard[WIDTH][HEIGHT]) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (oldBoard[i][j] != newBoard[i][j]) {
                return true;
            }
        }
    }
    return false;
}

void spawnTile(int board[WIDTH][HEIGHT]) {
    while (true) {
        int randomX = rand() % WIDTH;
        int randomY = rand() % HEIGHT;

        if (board[randomX][randomY] != 0) {
            continue;
        }

        board[randomX][randomY] = rand() % 100 < 90 ? 2 : 4;
        break;
    }
}

void printBoard(int board[WIDTH][HEIGHT]) {
    printf("Board:\n\n");
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (board[i][j] == 0) {
                printf("   . ");
            } else {
                printf("%4d ", board[i][j]);
            }
        }
        printf("\n");
    }
}

void gameLoop(int board[WIDTH][HEIGHT]) {
    int input;
    int snapshot[WIDTH][HEIGHT];

    while (true) {
        memcpy(snapshot, board, sizeof(int) * WIDTH * HEIGHT); // Can also use sizeof(snapshot)

        printBoard(board);

        input = getInput();

        if (input == LEFT) {
            for (int i = 0; i < HEIGHT; i++) {
                moveLeft(board[i]);
            }
        } else if (input == RIGHT) {
            for (int i = 0; i < HEIGHT; i++) {
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

        if (hasWon(board)) {
            printBoard(board);
            printf("You win!\n");
            break;
        }

        if (!canMove(board)) {
            printBoard(board);
            printf("You lose!\n");
            break;
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

int getNonZeroIndex(int currentIndex, int row[WIDTH]) {
    // Find first non-zero index
    for (int i = currentIndex + 1; i < WIDTH; i++) {
        if (row[i] != 0) {
            return i;
        }
    }
    return -1;
}

void compactLeft(int row[WIDTH]) {
    // Compact to left
    for (int i = 0; i < WIDTH - 1; i++) {
        int nonZeroIndex = getNonZeroIndex(i, row);
        if (row[i] == 0 && nonZeroIndex != -1) {
            row[i] = row[nonZeroIndex];
            row[nonZeroIndex] = 0;
        }
    }
}

// Adding same tiles in a row to left
void mergeLeft(int row[WIDTH]) {
    for (int i = 0; i < WIDTH - 1; i++) {
        if (row[i] != 0 && row[i] == row[i + 1]) {
            row[i] *= 2;
            row[i + 1] = 0;
        }
    }
}

void moveLeft(int row[WIDTH]) {
    compactLeft(row);
    mergeLeft(row);
    compactLeft(row);
}

void reverseRow(int row[WIDTH]) {
    for (int i = 0; i < WIDTH / 2; i++) {
        int temp = row[i];
        row[i] = row[WIDTH - i - 1];
        row[WIDTH - i - 1] = temp;
    }
}

void moveRight(int row[WIDTH]) {
    reverseRow(row);
    moveLeft(row);
    reverseRow(row);
}

void getColumn(int board[WIDTH][HEIGHT], int column, int buffer[WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        buffer[i] = board[i][column];
    }
}

void setColumn(int board[WIDTH][HEIGHT], int column, int buffer[WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        board[i][column] = buffer[i];
    }
}

void moveUp(int board[WIDTH][HEIGHT]) {
    int buffer[WIDTH];
    for (int i = 0; i < WIDTH; i++) {
        getColumn(board, i, buffer);
        moveLeft(buffer);
        setColumn(board, i, buffer);
    }
}

void moveDown(int board[WIDTH][HEIGHT]) {
    int buffer[WIDTH];
    for (int i = 0; i < WIDTH; i++) {
        getColumn(board, i, buffer);
        moveRight(buffer);
        setColumn(board, i, buffer);
    }
}

bool hasWon(int board[WIDTH][HEIGHT]) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (board[i][j] == WIN_SCORE) {
                return true;
            }
        }
    }
    return false;
}

bool canMove(int board[WIDTH][HEIGHT]) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (board[i][j] == 0) {
                return true;
            }
            // Check left
            if (i > 0 && board[i - 1][j] == board[i][j]) {
                return true;
            }
            // Check right
            if (i < WIDTH - 1 && board[i + 1][j] == board[i][j]) {
                return true;
            }
            // Check top
            if (j > 0 && board[i][j - 1] == board[i][j]) {
                return true;
            }
            // Check bottom
            if (j < HEIGHT - 1 && board[i][j + 1] == board[i][j]) {
                return true;
            }
        }
    }
    return false;
}

