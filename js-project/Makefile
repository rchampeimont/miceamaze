EMSCRIPTEN_ROOT=$(shell em-config EMSCRIPTEN_ROOT)
DATAPATH=data
CXXFLAGS+=-Wall -DDATAPATH=\"${DATAPATH}\" -I. -s LEGACY_GL_EMULATION=1
CFLAGS=$(CXXFLAGS)
LDLIBS=-Wall -lGL -lm
CXX=emcc
CC=emcc

OBJS := $(patsubst src/%.cpp,src/%.o,$(wildcard src/*.cpp)) SOIL/image_helper.o SOIL/stb_image_aug.o SOIL/image_DXT.o SOIL/SOIL.o

all: miceamaze.html

miceamaze.js: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@ --embed-file data

miceamaze.html: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@ --embed-file data

clean:
	rm -f miceamaze.js miceamaze.js.map miceamaze.html miceamaze.html.map src/*.o SOIL/*.o

test:
	echo $(EMSCRIPTEN_ROOT)
