EMSCRIPTEN_ROOT=$(shell em-config EMSCRIPTEN_ROOT)
DATAPATH=data
CXXFLAGS+=-Wall -DDATAPATH=\"${DATAPATH}\" -I. -s LEGACY_GL_EMULATION=1
CFLAGS=$(CXXFLAGS)
LDLIBS=-Wall -lGL -lm
CXX=emcc
CC=emcc

OBJS := $(patsubst src/%.cpp,build/%.o,$(wildcard src/*.cpp)) SOIL/image_helper.o SOIL/stb_image_aug.o SOIL/image_DXT.o SOIL/SOIL.o

all: miceamaze.html
	echo $(OBJS)

build/%.o: src/%.cpp src/*.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

SOIL/%.o: SOIL/%.c SOIL/*.h
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

miceamaze.js: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@ --embed-file data

miceamaze.html: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@ --embed-file data

clean:
	rm -f miceamaze.js miceamaze.js.map miceamaze.html miceamaze.html.map build/*.o SOIL/*.o

test:
	echo $(EMSCRIPTEN_ROOT)
