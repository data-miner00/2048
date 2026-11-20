#include <stdlib.h>
#include <time.h>
#include "io.h"
#include "board.h"

int main(void) {
    srand(time(NULL));
    enableRawMode();

    do {
        int board[SIZE][SIZE] = {0};

        spawnTile(board);
        enum GameState state = gameLoop(board);
        if (state == EXIT) {
            break;
        }
    } while (askPlayAgain());

    disableRawMode();
    return 0;
}
