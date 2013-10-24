CXXFLAGS+=-Wall -DDATAPATH=\"${DATAPATH}\" $(shell sdl-config --cflags) -I.
LDLIBS=-Wall $(shell sdl-config --libs) -lGL -lm -lfontconfig -lGLU
QUESODIR=quesoglc-0.7.2
QUESOLIB=$(QUESODIR)/build/.libs/libGLC.a

OBJS := $(patsubst src/%.cpp,src/%.o,$(wildcard src/*.cpp)) SOIL/lib/libSOIL.a $(QUESOLIB)

all: miceamaze 

miceamaze: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

SOIL/lib/libSOIL.a:
	cd SOIL/projects/makefile && make

$(QUESOLIB):
	tar zxvf $(QUESODIR).tar.gz && cd $(QUESODIR) && ./configure --enable-static=yes --enable-shared=no --with-fribidi=no && make

clean:
	rm -f miceamaze src/*.o
	cd SOIL/projects/makefile && make clean
	rm -rf $(QUESODIR)

