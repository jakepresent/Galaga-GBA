# Galaga-GBA 
Here is the source code for the Game Boy game I made for my Computer Organization and Programming (CS 2110) class at Georgia Tech.

To compile this for yourself, run the following commands:

```bash
$ sudo add-apt-repository ppa:tricksterguy87/ppa-gt-cs2110
$ sudo apt install gcc-arm-none-eabi cs2110-vbam-sdl mednafen cs2110-gba-linker-script nin10kit
```

To build the code and run the VBA emulator, run:
```bash
$ make emu
```

`make clean` will remove any compiled files.

Enjoy!
