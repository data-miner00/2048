#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define SIZE 4
#define WIN_SCORE 2048

enum GameState { WIN, LOSE, EXIT };

void spawnTile(int board[SIZE][SIZE]);
void printBoard(int board[SIZE][SIZE], int score);
enum GameState gameLoop(int board[SIZE][SIZE]);
int getInput();
int getNonZeroIndex(int currentIndex, int row[SIZE]);
void compactLeft(int row[SIZE]);
int mergeLeft(int row[SIZE]);
int moveLeft(int row[SIZE]);
void reverseRow(int row[SIZE]);
int moveRight(int row[SIZE]);
void getColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);
void setColumn(int board[SIZE][SIZE], int column, int buffer[SIZE]);
int moveUp(int board[SIZE][SIZE]);
int moveDown(int board[SIZE][SIZE]);
bool hasChanged(int oldBoard[SIZE][SIZE], int newBoard[SIZE][SIZE]);
bool hasWon(int board[SIZE][SIZE]);
bool canMove(int board[SIZE][SIZE]);
void clearScreen(void);
bool askPlayAgain(void);

enum Command { UP, DOWN, LEFT, RIGHT, QUIT };

int main(int argc, char *argv[]) {
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

void spawnTile(int board[SIZE][SIZE]) {
    while (true) {
        int randomX = rand() % SIZE;
        int randomY = rand() % SIZE;

        if (board[randomY][randomX] != 0) {
            continue;
        }

        board[randomY][randomX] = rand() % 100 < 90 ? 2 : 4;
        break;
    }
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

int getInput() {
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

        if (input == 'w' || input == 'W') {
            return UP;
        } else if (input == 's' || input == 'S') {
            return DOWN;
        } else if (input == 'a' || input == 'A') {
            return LEFT;
        } else if (input == 'd' || input == 'D') {
            return RIGHT;
        } else if (input == 'q' || input == 'Q') {
            return QUIT;
        } else {
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
    printf("Play again? (y/n): ");
    char answer = getchar();
    
    while (answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N') {
        printf("Invalid input\n");
        printf("Play again? (y/n): ");
        answer = getchar();
    }

    return answer == 'y' || answer == 'Y';
}
