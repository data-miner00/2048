#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
  srand(time(NULL));

  int width = 4;
  int height = 4;

  int board[width][height];

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      board[i][j] = rand() % 2;
    }
  }

  printf("Board:\n");
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      printf("%d ", board[i][j]);
    }
    printf("\n");
  }

  return 0;
}
