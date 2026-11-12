#include <stdlib.h>
#include <time.h>
#include "gameio.h"
#include "board.h"

int main(void) {
    srand(time(NULL));

    do {
        int board[SIZE][SIZE] = {0};

        spawnTile(board);
        enum GameState state = gameLoop(board);
        if (state == EXIT) {
            break;
        }
    } while (askPlayAgain());

    return 0;
}
