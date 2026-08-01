#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define WIDTH 4
#define HEIGHT 4

void initBoard(int board[WIDTH][HEIGHT]);
void printBoard(int board[WIDTH][HEIGHT]);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int board[WIDTH][HEIGHT] = {0};

    initBoard(board);

    printBoard(board);

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
    printf("Board:\n");
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}
