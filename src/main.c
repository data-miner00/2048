#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// make these to get from optional params
#define SIZE 4
#define WIN_SCORE 2048

enum GameState { WIN, LOSE, EXIT };
enum Command { UP, DOWN, LEFT, RIGHT, QUIT };

int getInput(void);
void clearScreen(void);
bool askPlayAgain(void);
int getRawInput(void);

// board
bool spawnTile(int board[SIZE][SIZE]);
void printBoard(int board[SIZE][SIZE], int score);
enum GameState gameLoop(int board[SIZE][SIZE]);

void getColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);
void setColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);

void compactLeft(int row[SIZE]);
int mergeLeft(int row[SIZE]);
void reverseRow(int row[SIZE]);

int moveUp(int board[SIZE][SIZE]);
int moveDown(int board[SIZE][SIZE]);
int moveLeft(int row[SIZE]);
int moveRight(int row[SIZE]);
int getNonZeroIndex(int currentIndex, int row[SIZE]);

bool hasChanged(int oldBoard[SIZE][SIZE], int newBoard[SIZE][SIZE]);
bool hasWon(int board[SIZE][SIZE]);
bool canMove(int board[SIZE][SIZE]);


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

bool spawnTile(int board[SIZE][SIZE]) {
    int emptyCount = 0;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == 0) {
                emptyCount++;
            }
        }
    }

    if (emptyCount == 0) {
        return false;
    }

    int emptySeen = 0;

    int randomSelect = rand() % emptyCount;

    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == 0) {
                if (emptySeen == randomSelect) {
                    board[row][col] = rand() % 100 < 90 ? 2 : 4;
                    return true;
                }
                emptySeen++;
            }
        }
    }

    return false;
}

void printBoard(int board[SIZE][SIZE], int score) {
    printf("Score: %d\n\n", score);
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

enum GameState gameLoop(int board[SIZE][SIZE]) {
    int input;
    int snapshot[SIZE][SIZE];
    int score = 0;

    while (true) {
        memcpy(snapshot, board, sizeof(int) * SIZE * SIZE); // Can also use sizeof(snapshot)

        clearScreen();
        printBoard(board, score);

        if (hasWon(board)) {
            printf("You win!\n");
            return WIN;
        }

        if (!canMove(board)) {
            printf("You lose!\n");
            return LOSE;
        }

        input = getInput();

        if (input == QUIT) {
            return EXIT;
        }

        if (input == LEFT) {
            for (int i = 0; i < SIZE; i++) {
                score += moveLeft(board[i]);
            }
        } else if (input == RIGHT) {
            for (int i = 0; i < SIZE; i++) {
                score += moveRight(board[i]);
            }
        } else if (input == UP) {
            score += moveUp(board);
        } else if (input == DOWN) {
            score += moveDown(board);
        }

        if (hasChanged(snapshot, board)) {
            spawnTile(board);
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

int getInput(void) {
    int input, discard;

    while (true) {
        input = getchar();

        if (input == EOF) {
            return QUIT;
        }

        if (input != '\n') {
            while ((discard = getchar()) != '\n' && discard != EOF)
                ;
        }

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
                return QUIT;
            default:
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
int mergeLeft(int row[SIZE]) {
    int currentScore = 0;
    for (int i = 0; i < SIZE - 1; i++) {
        if (row[i] != 0 && row[i] == row[i + 1]) {
            row[i] *= 2;
            currentScore += row[i];
            row[i + 1] = 0;
        }
    }
    return currentScore;
}

int moveLeft(int row[SIZE]) {
    compactLeft(row);
    int score = mergeLeft(row);
    compactLeft(row);
    return score;
}

void reverseRow(int row[SIZE]) {
    for (int i = 0; i < SIZE / 2; i++) {
        int temp = row[i];
        row[i] = row[SIZE - i - 1];
        row[SIZE - i - 1] = temp;
    }
}

int moveRight(int row[SIZE]) {
    reverseRow(row);
    int score = moveLeft(row);
    reverseRow(row);
    return score;
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

int moveUp(int board[SIZE][SIZE]) {
    int currentScore = 0;
    int buffer[SIZE];
    for (int i = 0; i < SIZE; i++) {
        getColumn(board, i, buffer);
        currentScore += moveLeft(buffer);
        setColumn(board, i, buffer);
    }
    return currentScore;
}

int moveDown(int board[SIZE][SIZE]) {
    int currentScore = 0;
    int buffer[SIZE];
    for (int i = 0; i < SIZE; i++) {
        getColumn(board, i, buffer);
        currentScore += moveRight(buffer);
        setColumn(board, i, buffer);
    }
    return currentScore;
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
