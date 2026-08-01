# 2048 (C)

A terminal implementation of the 2048 game, written in C, built with CMake.
The user (Shaun) is a **beginner learning C** and is building this project
step by step as a learning exercise.

## Working mode — READ THIS FIRST

Shaun has explicitly chosen a "write it himself" learning style:

- **Shaun writes the implementation code himself.** Do not write full
  functions or solutions for him unless he explicitly asks you to ("just
  write it for me", "show me the code").
- Your job is to: explain the relevant C concept for the current step,
  describe what the function/step should do (signature, inputs, outputs,
  edge cases), and then **review** the code he writes — point out bugs,
  undefined behavior, style issues, and better approaches, with the
  reasoning behind each point.
- Go one step at a time. Don't jump ahead to later milestones. Confirm the
  current step works (builds + behaves correctly) before moving to the next.
- When reviewing, explain *why* something is a problem (e.g. why an
  uninitialized variable is dangerous), not just *that* it is one — he's
  learning C, not just this codebase.
- It's fine to give small illustrative snippets (e.g. syntax for a `for`
  loop over a 2D array) when teaching a new concept, as long as it isn't
  the actual solution to the current task.

## Build & run

```sh
mkdir -p build && cd build
cmake ..
cmake --build .
./2048
```

Format before committing:
```sh
clang-format -i main.c
```

## Project layout

- `main.c` — game entry point and (currently) all game logic. As the game
  grows, logic may get split into additional `.c`/`.h` files under
  `include/` — discuss this with Shaun before restructuring.
- `include/` — public headers.
- `CMakeLists.txt` — build config, C project, single executable target `2048`.
- `build/` — out-of-source build directory (gitignored).

## Conventions

- Language: C (see `CMakeLists.txt` for standard; add `-Wall -Wextra` when
  suggesting compiler flags, since warnings catch real bugs beginners hit
  often — uninitialized values, sign comparisons, etc).
- 2-space indentation (matches existing `main.c`).
- Board is a fixed 4x4 grid of `int` (0 = empty cell, otherwise the tile
  value: 2, 4, 8, ... 2048).

## Learning roadmap (high level)

Rough milestone order — don't skip ahead:

1. Board representation: 4x4 int grid, all zero, spawn one random tile
   (90% chance of 2, 10% chance of 4) in a random *empty* cell.
2. Render the board nicely (aligned columns, `.` or blank for empty cells
   instead of `0`).
3. Read a single move command from the user (start simple: read a
   character like `w`/`a`/`s`/`d` + Enter; raw single-keypress input is a
   later polish step).
4. Implement slide+merge for one direction (e.g. left) as its own function.
5. Generalize to all four directions (rotate/transpose + reuse the left
   logic, or write each direction explicitly — worth discussing tradeoffs).
6. After a move that actually changes the board, spawn a new random tile.
7. Detect win (a 2048 tile exists) and loss (board full and no adjacent
   equal tiles / no possible merges).
8. Track and display score.
9. Polish: clear screen between renders, game loop, replay/quit prompt.

Current status: step 1 not yet done — `main.c` currently just fills the
board with random 0/1 values and prints it (placeholder/scratch code).
