brr
===

A simple force-feedback tool for Linux.

Tested with: Microsoft X-Box 360 pad [vendor 045e, product 028e, version 0114].

Reads operations from standard input (to execute until directed otherwise), one per line.

Usage example:

``` text
❯ make
cc -g -O2 -Wall -Wextra -fPIE -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fsanitize=address -Wformat -Wformat-security -Werror=format-security -Wl,-z,relro -Wl,-z,now -Werror -Wshadow -Wpointer-arith -Wcast-align -Wformat=2 -Wstrict-overflow=5 -Wstrict-aliasing=2 brr.c -o brr

❯ ./brr
Usage: ./brr <device>

Input: <mode> <power>
Modes: {hard, soft, wave, none, quit}
Power: [0, 100], for: {hard, soft, wave}

❯ ./brr /dev/input/event23
hard 40
hard 500
error: invalid input
soft 20
none 40
error: invalid input
soft 20
none
wave 60
quit
```
