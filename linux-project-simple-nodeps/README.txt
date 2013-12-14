Like linux-project, but with as few dependencies as possible.
Removed dependencies:
- rsvg, convert (images are pre-rendered)
- gzip (man pages removed)
- SOIL (compiled and linked statically)
- QuesoGLC (compiled and linked statically)
Dependencies that still apply:
- OpenGL
- SDL, SDL_mixer
- fontconfig (added for QuesoGLC)
