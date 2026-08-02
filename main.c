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

