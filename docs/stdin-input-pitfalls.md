# Reading keyboard input: pitfalls

All keyboard input goes through `src/io.c`. `getRawInput()` is the only
function that calls `getchar()`. `getInput()` (moves) and `askPlayAgain()`
(the y/n prompt) both get their keys from it.

`getchar()` is the simplest way to read input in C, but it has sharp edges
that are easy to miss as a beginner. Every pitfall below was hit (and
fixed) while building this project. This doc explains what each one was,
why it happened, and how to reason about similar bugs elsewhere.

The game first read whole lines in the terminal's normal line mode, and
Pitfalls 1–3 come from that version. It now reads one key per press in
raw mode, which [Raw mode](#raw-mode-one-key-per-press) describes along
with Pitfalls 4 and 5. Raw mode removed the drain loop that Pitfalls 1–3
built, but their lessons still apply, Pitfall 2's above all.

## Background: `getchar()` returns an `int`, not a `char`

`getchar()`'s return type is `int`, even though it conceptually reads a
single character. That's not an accident — it needs a way to signal
"there is no character," which it does by returning the special value
`EOF` (defined in `stdio.h`, typically `-1`). No real character value
collides with `EOF`, which is exactly why the return type has to be wider
than `char`. Real bytes come back as 0–255, so any value of your own that
must never be mistaken for a byte has to sit outside that range. That's
why the arrow-key values start at 256 (Pitfall 5).

`EOF` means **the input source is closed — no more data will ever
arrive**. This happens when:
- input is piped from a file or another program that has run out of data
  (e.g. `./2048 < /dev/null`, or `some_program | ./2048` after
  `some_program` exits),
- the user presses **Ctrl+D** on an empty line in an interactive
  terminal in line mode, the standard "end of input" keystroke on
  Linux/macOS. In raw mode, Ctrl+D is an ordinary key (byte 4), so `EOF`
  now only comes from piped or redirected input.

## Pitfall 1: assuming a fixed number of characters per line

### The bug

The original code read one character, then unconditionally read a second
one and assumed it was the trailing `'\n'`:

```c
input = getchar();
getchar(); // Consume newline
```

This only works if the user types *exactly one character* before
pressing Enter. If they type anything else — a typo, multiple characters,
whitespace — the second `getchar()` doesn't consume the newline, it
consumes whatever the next real character happens to be. From that point
on, every subsequent read is off by however many extra characters were
left unconsumed.

### Demonstrating it

```sh
printf 'xy\nw\n' | ./2048
```

Feeding it the line `xy` followed by a real move `w` caused the `w` to be
silently swallowed as part of "resyncing" from the bad line — it was
never recognized as a move at all. The parser had desynchronized itself
from the actual line boundaries in the input stream.

### The fix

Don't assume where the newline is — actively discard everything up to and
including it, however many characters that takes:

```c
input = getchar();

int discard;
while ((discard = getchar()) != '\n' && discard != EOF);

/* ...now check what `input` was... */
```

This drains the rest of the line, whether it held one character or ten.
Re-running the same repro afterwards, the `w` on the second line is
correctly detected as a move. Draining *unconditionally* turned out to be
a bug of its own, though — see Pitfall 3.

## Pitfall 2: not checking for `EOF`, causing a busy-loop

### The bug

`getInput()`'s outer loop looks like this:

```c
while (true) {
    input = getchar();
    /* ...flush rest of line... */

    if (input == 'w' || ...) return UP;
    else if (...) return ...;
    else printf("Invalid input\n");
}
```

Every branch treats an unrecognized `input` as "the user typed junk,
print a message and wait for the next line." That assumption breaks for
`EOF`: unlike a bad keystroke, once stdin is closed, `getchar()` **stops
blocking** — it returns `EOF` immediately, forever, on every subsequent
call. A loop that treats `EOF` as just another invalid character never
waits for new input again; it spins as fast as the CPU allows.

### Demonstrating it

```sh
timeout 1 ./2048 < /dev/null > out.txt
wc -l out.txt
```

Before the fix, one second of runtime produced **over 23 million lines**
of `"Invalid input"` — a full-speed busy loop with no way to stop it
short of killing the process.

### The fix

Check for `EOF` explicitly, right after the read that could produce it,
and handle it as its own case rather than falling through to "invalid
input":

```c
if (input == EOF) {
    exit(0);
}
```

After this, `./2048 < /dev/null` exits cleanly (status `0`) instead of
running forever.

When the `q` key was added, `exit(0)` here became `return QUIT;`, so EOF
ends the game the same way `q` does: `gameLoop` sees `QUIT` and `break`s
out of its loop.

## Pitfall 3: a blank line swallows the next line

### The bug

Pitfall 1's drain loop reads "up to and including the `'\n'`", which
assumes the `'\n'` hasn't been read yet. On a blank line (the user just
presses Enter) that's false: the first `getchar()` already returned the
`'\n'` itself. The drain loop then keeps reading until the *next* `'\n'`,
throwing away the whole next line.

### Demonstrating it

```sh
printf '\nw\n' | ./2048 | grep -c 'Board:'
```

The board is printed once at the start and again after every move, so a
working `w` gives `2`. Before the fix this printed `1`: the blank line was
reported as invalid, and the `w` was drained along with it. Interactively,
pressing Enter and then typing `q` printed "Invalid input" and kept
waiting — the `q` line was the one being drained.

### The fix

Check for `EOF` first, right after the read that can produce it (a closed
stream has no line left to drain), then drain only if `input` wasn't the
`'\n'`:

```c
input = getchar();

if (input == EOF) {
    return QUIT;
}

if (input != '\n') {
    while ((discard = getchar()) != '\n' && discard != EOF)
        ;
}
```

A blank line still prints "Invalid input", but the line after it is now
read normally.

## Raw mode: one key per press

### What it is

Normally the terminal collects a whole line (**canonical mode**, which is
what lets Backspace work) and hands it over only when Enter is pressed. It
also echoes each key. `enableRawMode()` clears `ICANON` and `ECHO` in
`c_lflag` and sets `c_cc[VMIN] = 1` and `c_cc[VTIME] = 0`, so each key
reaches the program the moment it's pressed, and nothing is echoed.

Every other setting is left alone, on purpose:
- `ICRNL` stays on. It turns Enter's carriage return (`'\r'`) into
  `'\n'`, so Enter still arrives as `'\n'`.
- Output processing stays on. It's what makes `'\n'` also return the
  cursor to column 0.

`cfmakeraw()` would switch both off: Enter would arrive as `'\r'`, and the
board would print as a staircase. That's why the code clears only two
flags.

### Switching it off again

- **`isatty(STDIN_FILENO)` comes first.** Piped or redirected input
  (which is how the tests run) has no terminal settings, so nothing is
  changed and nothing needs restoring.
- **`rawModeEnabled` records whether raw mode was actually switched on,**
  and `disableRawMode()` does nothing unless it was. Here's what the flag
  prevents:
  - If `tcgetattr` fails, `originalSettings` keeps its starting value of
    all zeros (file-scope variables start zeroed).
  - "Restoring" that would apply an all-zero `termios`: no line mode, no
    echo, no Ctrl+C, 5-bit characters, and line speed `B0` (hang up).
  - Before the flag existed, a traced run on a pipe showed exactly this
    call being made. It did no harm only because a pipe rejects terminal
    settings.
- **`atexit(disableRawMode)`** restores the terminal however the program
  ends normally: returning from `main`, or calling `exit()` from anywhere.
- **Both `tcsetattr` calls use `TCSAFLUSH`,** which also throws away keys
  typed but not yet read. Keys pressed while the game exits, arrow escape
  codes above all, never reach the shell.

**Ctrl+C and crashes skip all of this.** A signal kills the program
without running `atexit` functions, so the shell stops showing what you
type. To fix it, type `reset` and press Enter, even though you can't see
the typing.

### Enter and Ctrl+D are just keys

- **Enter arrives as `'\n'`.** `getInput()` skips it silently, because
  the piped tests send a `'\n'` after every key and a player may press
  Enter out of habit. `askPlayAgain()` counts it as an invalid answer.
- **Ctrl+D arrives as byte 4** and gets "Invalid input". Use `q` to quit.

## Pitfall 4: treating `EOF` as a key to skip

### The bug

`askPlayAgain()` briefly grouped `EOF` with `'\n'` as "ignore it and ask
again":

```c
case EOF:
case '\n':
    continue;
```

This is Pitfall 2 again. Once stdin is closed, `getchar()` returns `EOF`
instantly on every call, so `continue` reprinted the prompt as fast as the
CPU allowed. In 3 seconds it printed 2.4 million prompts (44 MB of
output). It only hits piped or redirected input that ends at the prompt,
but that includes every test run.

### The fix

`EOF` has to end the loop. `askPlayAgain()` returns `false` ("don't play
again") and `getInput()` returns `QUIT`.

To check: 1000 moves of `wasd` always end in a loss, and then the input
runs out at the prompt:

```sh
printf 'w\na\ns\nd\n%.0s' {1..1000} | timeout 3 ./build/2048 > /dev/null; echo $?
```

`0` means the game exited properly. `124` means `timeout` had to kill it.

## Pitfall 5: arrow-key decoding that turned keys into the wrong move

### Background: escape sequences

An arrow key sends three bytes: ESC (27), then `[`, then a letter (`A`
up, `B` down, `C` right, `D` left). Some terminals, in "application
cursor" mode, send `O` instead of `[`. To see the raw bytes, run `cat -v`
and press a key: `^[` is how it shows ESC.

`getRawInput()` decodes these, so each caller gets **one value per key
press**. The values are `enum ArrowKeys` in `src/io.h`: `KEY_UP` to
`KEY_RIGHT`, plus `KEY_UNKNOWN` for any other sequence. That way an arrow
at the play-again prompt is one invalid key, not three.

### The bugs

The first version of the decoder had three bugs, and each one made some
key do the wrong move:

1. **Arrow values inside 0–255.** `KEY_UP` was 165. `getchar()` returns
   any byte from 0 to 255, and bytes 128–255 make up every non-ASCII
   UTF-8 character. `好` is the bytes 229, 165, 189, so typing it moved
   the tiles up. Only values from 256 up, like `EOF` at -1, can never
   match a real byte.
2. **Returning the last byte of an unknown sequence.** Home sends
   `ESC [ H`, and returning the `'H'` made it a left move (`h`/`H`). An
   unknown sequence now returns `KEY_UNKNOWN`. `EOF` in that position
   must still return `EOF`, so it has its own `case`.
3. **Reading the second byte inside the comparison.** `getchar() == '['`
   reads a byte and discards it after comparing, so `ESC O A` left its
   `'A'` behind to be read as the next key (left). Store the byte in a
   variable first, then compare the variable as often as needed. Writing
   `getchar() == '[' || getchar() == 'O'` would read two *different*
   bytes.

### Known limitations

These were accepted on purpose:

- **A lone Esc waits for the next key** and swallows it as part of a
  sequence. Telling a lone Esc from an arrow needs a timeout.
- **Longer sequences** such as Page Up (`ESC [ 5 ~`) give two invalid
  keys: `KEY_UNKNOWN` for `ESC [ 5`, then the `'~'`.
- **ESC followed by end of input** gives one "Invalid input" before
  quitting. `getRawInput()` returns the ESC, and the next read returns
  `EOF`.

## Takeaway

Any `while (true)` loop built around `getchar()` (or similar raw stdin
reads) needs to answer these questions explicitly, not by accident:
1. **How much input does one "turn" actually consume?** Don't hardcode an
   assumption about line length — drain up to the delimiter instead.
2. **What happens when the input source is gone, not just wrong?** `EOF`
   is a permanent, non-blocking condition, not a transient bad keystroke
   — it needs its own branch, checked before you decide the input was
   merely "invalid."
3. **Has the delimiter already been read?** "Drain up to the next `'\n'`"
   only works while the `'\n'` is still ahead. When the character you
   just read *is* the `'\n'` (a blank line), there's nothing left to
   drain — draining anyway eats the next line.
4. **Is the input actually a terminal?** Terminal settings only exist for
   a terminal. Check `isatty` first, and undo only what you actually
   changed.
5. **Can my special values collide with real input?** `getchar()` already
   uses 0–255 and `EOF`. Any signal value of your own has to sit outside
   that range.
6. **Does this call consume input?** Each `getchar()` call consumes a
   byte. Call it once for each byte you mean to read, store the result,
   and compare the variable.
