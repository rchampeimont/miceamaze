UNAME := $(shell uname)
INSTALL_ICONS=1
PREFIX=/usr/local
BINDIR=${PREFIX}/bin
DATAPATH=${PREFIX}/share/miceamaze
MANPATH=${PREFIX}/share/man/man6
PIXPATH=${PREFIX}/share/pixmaps
MENUPATH=${PREFIX}/share/applications
ICONPATH=${PREFIX}/share/icons/hicolor/64x64/apps
CXXFLAGS+=-Wall -DDATAPATH=\"${DATAPATH}\" $(shell sdl-config --cflags)
LDLIBS=-Wall -lGLC -lSOIL $(shell sdl-config --libs) -lSDL_mixer
ifeq ($(UNAME), Darwin)
  LDLIBS+=-framework OpenGL -lm
else
  LDLIBS+=-lGL -lm
endif
SVGCOMMAND=rsvg-convert
SVGOPTIONS=-w 500

OBJS := $(patsubst src/%.cpp,src/%.o,$(wildcard src/*.cpp))
BUILTDATA := data/images/eagle.png data/images/eagle2.png

all: miceamaze miceamaze.xpm miceamaze.png miceamaze.6.gz $(BUILTDATA)

miceamaze: $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

miceamaze.xpm: data/images/mouse.png
	convert -scale 32x32 $^ $@

miceamaze.png: data/images/mouse.png
	convert -scale 64x64 $^ +set date:create +set date:modify -define png:exclude-chunk=time $@

miceamaze.6.gz: man/miceamaze.6
	gzip -nc $^ > $@

data/images/eagle2.png: src/data-sources/eagle2.svg
	$(SVGCOMMAND) $(SVGOPTIONS) $^ >$@

data/images/eagle.png: src/data-sources/eagle.svg
	$(SVGCOMMAND) $(SVGOPTIONS) $^ >$@

clean:
	rm -f miceamaze miceamaze.xpm miceamaze.png miceamaze.6.gz src/*.o $(BUILTDATA)

install: all
	install -d -m 755 ${DESTDIR}${DATAPATH}
	cp -R -L data/* ${DESTDIR}${DATAPATH}/
	install -D -m 755 miceamaze ${DESTDIR}${BINDIR}/miceamaze
	install -D -m 644 miceamaze.6.gz ${DESTDIR}${MANPATH}/miceamaze.6.gz
ifeq ($(INSTALL_ICONS), 1)
	install -D -m 644 miceamaze.desktop ${DESTDIR}${MENUPATH}/miceamaze.desktop
	install -D -m 644 miceamaze.png ${DESTDIR}${ICONPATH}/miceamaze.png
	install -D -m 644 miceamaze.xpm ${DESTDIR}${PIXPATH}/miceamaze.xpm
endif

uninstall:
	rm -rf ${DESTDIR}${DATAPATH}
	rm -f ${DESTDIR}${BINDIR}/miceamaze
	rm -f ${DESTDIR}${MANPATH}/miceamaze.6.gz
ifeq ($(INSTALL_ICONS), 1)
	rm -f ${DESTDIR}${MENUPATH}/miceamaze.desktop
	rm -f ${DESTDIR}${ICONPATH}/miceamaze.png
	rm -f ${DESTDIR}${PIXPATH}/miceamaze.xpm
endif

re: clean all
