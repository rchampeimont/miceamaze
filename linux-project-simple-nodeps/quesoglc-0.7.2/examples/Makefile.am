
## Process this file with automake to produce Makefile.in.

noinst_PROGRAMS = glclogo \
                  tutorial \
                  tutorial2 \
                  unicode \
                  glcdemo \
                  demo

AM_CFLAGS = @PTHREAD_CFLAGS@ \
            @FREETYPE2_CFLAGS@ \
            @FONTCONFIG_CFLAGS@ \
            @GLUT_CFLAGS@ \
            @FRIBIDI_CFLAGS@

LDADD = $(top_builddir)/build/libGLC.la \
        @GLUT_LIBS@ \
        @FONTCONFIG_LIBS@ \
        @FREETYPE2_LIBS@ \
        @PTHREAD_LIBS@ \
        @FRIBIDI_LIBS@

glclogo_SOURCES = glclogo.c

tutorial_SOURCES = tutorial.c

tutorial2_SOURCES = tutorial2.c

unicode_SOURCES = unicode.c

glcdemo_SOURCES = glcdemo.c

demo_SOURCES = demo.c trackball.c trackball.h

clean-generic:
	rm -f *.gcno *.gcda *.gcov
