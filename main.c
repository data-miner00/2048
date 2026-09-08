#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define WIDTH 4
#define HEIGHT 4

void initBoard(int board[WIDTH][HEIGHT]);
void printBoard(int board[WIDTH][HEIGHT]);
void gameLoop(int board[WIDTH][HEIGHT]);
int getInput();
int getNonZeroIndex(int currentIndex, int row[WIDTH]);
void compactLeft(int row[WIDTH]);
void mergeLeft(int row[WIDTH]);
void moveLeft(int row[WIDTH]);

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int board[WIDTH][HEIGHT] = {0};

    initBoard(board);

    gameLoop(board);

    return 0;
}

void initBoard(int board[WIDTH][HEIGHT]) {
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

    while (true) {
        printBoard(board);

        input = getInput();

        printf("Input: %d\n", input);

        if (input == LEFT) {
            for (int i = 0; i < HEIGHT; i++) {
                moveLeft(board[i]);
            }
        }
    }
}

int getInput() {
    int input, discard;

    while (true) {
        input = getchar();
        while ((discard = getchar()) != '\n' && discard != EOF);

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
