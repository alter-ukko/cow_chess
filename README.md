## cow_chess

`cow_chess` is a work-in-progress cross-platform [UCI chess client](https://wbec-ridderkerk.nl/html/UCIProtocol.html) written in C.

In order to use `cow_chess`, you need a UCI chess engine installed, like [leela chess zero](https://lczero.org/) or [stockfish](https://stockfishchess.org/). At the moment, you'll set the command you want to run in the source code (just search for `lc0` in `game.c`).


### get/build/run

```
$ git clone https://github.com/alter-ukko/cow_chess.git
$ cd cow_chess
$ git submodule update --init --recursive
$ mkdir build && cd build
$ cmake ..
$ make
$ ./cow_chess
```

At the moment, I don't think `cow_chess` works on Windows. To make that work, I'll need to write code that forks processes using the Windows API, which I imagine I'll get to. There are already a lot of chess GUIs for Windows though.

### dependencies

`cow_chess` uses three libraries, included as git submodules and compiled into the final binary:

* [stb](https://github.com/nothings/stb) (used to load png spritesheet)
* [cimgui](https://github.com/cimgui/cimgui) (used for ui)
* [sokol] (https://github.com/floooh/sokol) (used for cross-platform graphics, sound, windowing, input, etc.)

All of these libraries are incredible.

I got a lot of information about how to do things from [c-chess-cli](https://github.com/lucasart/c-chess-cli), and the string (`str.c` and `str.h`) and utility (`util.c` and `util.h`) sources are used directly from that project.

Other utility code I'm using from source:

* (AHEasing)[https://github.com/warrenm/AHEasing/] (easing functions for sprite movement)
* (uthash)[https://github.com/troydhanson/uthash] (dynamic arrays)

Both of these are awesome and part of my standard toolbox when I make things with C.

The chess board and pieces come from Brysia's very cool [Pixel Chess Assets](https://brysiaa.itch.io/pixel-chess-assets-pack), which I purchased and modified slightly

### current status

You can play a game of chess as white against the engine. It's not properly detecting the end of the game, although the code is there to detect a checkmate.

I'm going to see if it's possible to move my kanban board to github. If not, I'll create issues for the things I'm going to do.
