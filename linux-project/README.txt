=============================================
HOW TO COMPILE AND INSTALL MICEAMAZE ON LINUX
=============================================

You need:
- GCC C++ compiler and GNU Make
- OpenGL (with headers)
- SDL (with headers)
- SDL_mixer (with headers)
- SOIL (with headers)
- QuesoGLC (with headers)
- "DejaVu" font installed
- rsvg (only the "rsvg-convert" command is used)
- ImageMagick (only the "convert" command is used)

When "with headers" is specified, you need the package with the
"-dev" suffix (called like this in most distributions).

To compile and run without installing:
  make
  ./miceamaze

To compile and install (requires administrator privileges):
  make
  sudo make install
Then simply enter "miceamaze" in a terminal or search for MiceAmaze
in your applications menu to play the game.

