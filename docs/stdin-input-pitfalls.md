# Reading keyboard input with `getchar()`: three pitfalls

`getInput()` in `main.c` reads moves from stdin one character at a time
with `getchar()`. This is the simplest way to read input in C, but it has
three sharp edges that are easy to miss as a beginner. All three were hit
(and fixed) while building this project — this doc explains what they were,
why they happened, and how to reason about them if similar bugs show up
elsewhere.

## Background: `getchar()` returns an `int`, not a `char`

`getchar()`'s return type is `int`, even though it conceptually reads a
single character. That's not an accident — it needs a way to signal
"there is no character," which it does by returning the special value
`EOF` (defined in `stdio.h`, typically `-1`). No real character value
collides with `EOF`, which is exactly why the return type has to be wider
than `char`.

`EOF` means **the input source is closed — no more data will ever
arrive**. This happens when:
- input is piped from a file or another program that has run out of data
  (e.g. `./2048 < /dev/null`, or `some_program | ./2048` after
  `some_program` exits),
- the user presses **Ctrl+D** on an empty line in an interactive
  terminal — the standard "end of input" keystroke on Linux/macOS, and
  an easy one to hit by accident.

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

## Takeaway

Any `while (true)` loop built around `getchar()` (or similar raw stdin
reads) needs to answer three questions explicitly, not by accident:
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
