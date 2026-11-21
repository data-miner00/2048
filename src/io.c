#include "io.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

static struct termios originalSettings;
static bool rawModeEnabled = false;

void enableRawMode(void) {
    atexit(disableRawMode);

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
    int input;

    while (true) {
        printf("Play again? (y/n): ");
        fflush(stdout); // Prompt has no '\n', so push it out before waiting for input
        input = getRawInput();

        switch (input) {
        case 'y':
        case 'Y':
            return true;
        case 'n':
        case 'N':
        case EOF:
            return false;
        case '\n':
        default:
            printf("Invalid input\n");
            break;
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
        case KEY_UP:
            return UP;
        case 's':
        case 'S':
        case 'j':
        case 'J':
        case KEY_DOWN:
            return DOWN;
        case 'a':
        case 'A':
        case 'h':
        case 'H':
        case KEY_LEFT:
            return LEFT;
        case 'd':
        case 'D':
        case 'l':
        case 'L':
        case KEY_RIGHT:
            return RIGHT;
        case 'q':
        case 'Q':
        case EOF:
            return QUIT;
        case '\n':
            continue;
        default:
            printf("Invalid input\n");
            break;
        }
    }
}

int getRawInput(void) {
    int input = getchar();

    if (input == 27) { // Escape character
        // Check for arrow keys
        if (getchar() == '[') {
            input = getchar();
            switch (input) {
            case 'A':
                return KEY_UP;
            case 'B':
                return KEY_DOWN;
            case 'C':
                return KEY_RIGHT;
            case 'D':
                return KEY_LEFT;
            case EOF:
                return EOF;
            default:
                return KEY_UNKNOWN;
            }
        }
    }

    return input;
}
