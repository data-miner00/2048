#ifndef IO_H
#define IO_H
#include <stdbool.h>

enum Command { UP, DOWN, LEFT, RIGHT, QUIT };

void enableRawMode(void);
void disableRawMode(void);
int getInput(void);
void clearScreen(void);
bool askPlayAgain(void);
int getRawInput(void);

#endif // IO_H
