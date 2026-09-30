# 2D arrays: which index is the row?

In `board[i][j]`, which index is the row and which is the column is a
convention, and conventions differ between projects and libraries. A
stable mental model starts from what C guarantees, then adds the
convention on top.

## What C guarantees

`int a[2][3]` is 2 arrays of 3 ints, stored back to back:

```c
int a[2][3] = {
    {1, 2, 3},   // a[0]
    {4, 5, 6},   // a[1]
};
// memory: 1 2 3 4 5 6
```

- The first index selects an inner array; the second selects a cell
  within it.
- Inner arrays are adjacent, so `a[0][3]` in practice reads `a[1][0]` —
  undefined behavior that raises no error.
- An inner array is a real array and can be passed to a function
  (`moveLeft(board[i])`). The other direction is not stored together and
  no `a[?]` yields it, which is why `getColumn` copies a column cell by
  cell.

## The convention: inner arrays are rows

The common convention is `a[row][col]`: a nested initializer and a
nested print loop both lay out one inner array per line. It matches
"row F, seat 12" and compiler diagnostics like `main.c:84:35` (line 84,
column 35).

**Model: first index = which line; second index = position along it.**

## The trap: (x, y) is reversed

x is horizontal, so it selects the column; y selects the row. A point
(x, y) lives at `grid[y][x]`. Writing `grid[x][y]` because "x comes
first" is the most common mix-up. Some libraries use both orders, e.g.
OpenCV's `mat.at<int>(row, col)` vs `cv::Point(x, y)`.

## Example: `main.c` (as of 2026-10-01)

`main.c` mixes both conventions:

- `int board[WIDTH][HEIGHT]` and `board[randomX][randomY]` in
  `spawnTile` put x first.
- `printBoard`, `moveLeft(board[i])`, and the Left/Right loops
  (`i < HEIGHT`) treat the first index as the row.

A 4×4 board hides this. With `#define WIDTH 5`, the board prints 4 wide
and 5 tall, and `moveLeft` loops over 5 cells of each 4-cell row,
reading past its end.

Proposed fix (not yet applied): one `SIZE` constant replacing `WIDTH`
and `HEIGHT` — 2048 is square, and `moveUp` already assumes it by
running the row logic on columns — plus `row`/`col` names in
`spawnTile`.

## Staying consistent across projects

1. **Indices named by meaning.** `row`/`col` for grid positions; `x`/`y`
   only for geometric coordinates. Wrong code then looks wrong:
   `board[col][row]` stands out, `board[j][i]` does not.
2. **Sizes named by what they count.** `grid[ROWS][COLS]`. With width
   and height, the row count is the height: `grid[HEIGHT][WIDTH]`.
3. **Two facts pinned per project:** which index is the row, and where
   row 0 is (top of the screen here; bottom in y-up coordinates). The
   rendering function is the ground truth. Both belong in a comment at
   the declaration: `// board[row][col], row 0 = top`.
4. **A one-minute test when unsure.** Set `[0][1]` to 9 and print; top
   line, second cell means `[row][col]`.
5. **Column-major languages.** Fortran, MATLAB, Julia, and R store grids
   column by column. Notation is still `A(row, col)`, but the column is
   the stored-together unit — relevant when passing arrays between
   languages or ordering loops for speed.
