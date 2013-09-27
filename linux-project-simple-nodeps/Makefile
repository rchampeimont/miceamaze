CXXFLAGS+=-Wall -DDATAPATH=\"${DATAPATH}\" $(shell sdl-config --cflags)
LDLIBS=-Wall -lGLC $(shell sdl-config --libs) -lGL -lm

OBJS := $(patsubst src/%.cpp,src/%.o,$(wildcard src/*.cpp)) SOIL/lib/libSOIL.a

all: miceamaze 

miceamaze: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

SOIL/lib/libSOIL.a:
	cd SOIL/projects/makefile && make

clean:
	rm -f miceamaze src/*.o
	cd SOIL/projects/makefile && make clean

