#include "gameio.h"
#include <stdio.h>


void clearScreen(void) {
    // ESC[2J = clear screen
    // ESC[H = move cursor to top left
    printf("\033[2J\033[H");
}

bool askPlayAgain(void) {
    while (true) {
        printf("Play again? (y/n): ");
        fflush(stdout); // Prompt has no '\n', so push it out before waiting for input

        int answer = getRawInput();

        if (answer == EOF) {
            return false;
        }

        if (answer == 'y' || answer == 'Y') {
            return true;
        } else if (answer == 'n' || answer == 'N') {
            return false;
        } else {
            printf("Invalid input\n");
        }
    }
}


int getInput(void) {
    int input;

    while (true) {
        input = getRawInput();

        switch (input) {
            case 'w':
            case 'W':
            case 'k':
            case 'K':
                return UP;
            case 's':
            case 'S':
            case 'j':
            case 'J':
                return DOWN;
            case 'a':
            case 'A':
            case 'h':
            case 'H':
                return LEFT;
            case 'd':
            case 'D':
            case 'l':
            case 'L':
                return RIGHT;
            case 'q':
            case 'Q':
            case EOF:
                return QUIT;
            default:
                printf("Invalid input\n");
        }
    }
}


int getRawInput(void) {
    int input = getchar(), discard;

    if (input == EOF) {
        return input;
    }

    if (input != '\n') {
        while ((discard = getchar()) != '\n' && discard != EOF)
            ;
    }

    return input;
}
