#include "gameio.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

static struct termios originalSettings;
static bool rawModeEnabled = false;

void enableRawMode(void) {
    // when pipe mode
    if (isatty(STDIN_FILENO) == 0) {
        return;
    }

    struct termios newSettings;

    if (tcgetattr(STDIN_FILENO, &originalSettings) == -1) {
        perror("tcgetattr");
        return;
    }
    newSettings = originalSettings;

    // Disable canonical mode and echo
    newSettings.c_lflag &= ~(ICANON | ECHO);

    // Set minimum number of characters for non-canonical read to 1
    newSettings.c_cc[VMIN] = 1;
    // Set timeout for non-canonical read to 0 (no timeout)
    newSettings.c_cc[VTIME] = 0;

    // Don't use TCSANOW
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &newSettings);
    rawModeEnabled = true;
}

void disableRawMode(void) {
    if (!rawModeEnabled) {
        return;
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalSettings);
}

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
