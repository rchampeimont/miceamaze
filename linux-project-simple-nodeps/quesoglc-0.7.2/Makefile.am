SUBDIRS = include build @EXECUTABLES@

ACLOCAL_AMFLAGS = -I build/m4

EXTRA_DIST = ChangeLog \
             INSTALL \
             INSTALL.win \
             COPYING \
             makefile.mgw \
             database/buildDB.py \
             $(DOCS) \
             $(VCPROJS)

VCPROJS = QuesoGLC.sln \
          build/QuesoGLC.vcproj \
          tests/test1.vcproj \
          tests/test4.vcproj \
          tests/test5.vcproj \
          tests/test6.vcproj \
          tests/test7.vcproj \
          tests/test8.vcproj \
          tests/test9.1.vcproj \
          tests/test9.2.vcproj \
          tests/test9.3.vcproj \
          tests/test9.4.vcproj \
          tests/test9.5.vcproj \
          tests/test9.6.vcproj \
          tests/test9.7.vcproj \
          tests/test9.8.vcproj \
          tests/test10.vcproj \
          tests/test11.1.vcproj \
          tests/test11.2.vcproj \
          tests/test11.3.vcproj \
          tests/test11.4.vcproj \
          tests/test11.5.vcproj \
          tests/test11.6.vcproj \
          tests/test11.7.vcproj \
          tests/test12.vcproj \
          tests/testcontex.vcproj \
          tests/testfont.vcproj \
          tests/testmaster.vcproj \
          tests/testrender.vcproj \
          examples/demo.vcproj \
          examples/glcdemo.vcproj \
          examples/glclogo.vcproj \
          examples/tutorial.vcproj \
          examples/tutorial2.vcproj \
          examples/unicode.vcproj

DOCS = docs/body_comparison.eps \
       docs/body_comparison.png \
       docs/Doxyfile \
       docs/glclogo.png \
       docs/Image1.eps \
       docs/Image1.png \
       docs/Image2.eps \
       docs/Image2.png \
       docs/Image3.eps \
       docs/Image3.png \
       docs/Image4.eps \
       docs/Image4.png \
       docs/mainpage.dox \
       docs/measure.eps \
       docs/measure.png \
       docs/qfooter.html \
       docs/qheader.html \
       docs/quesoglc.css \
       docs/screenshot.eps \
       docs/screenshot.png \
       docs/title.png \
       docs/tutorial2.eps \
       docs/tutorial2.png \
       docs/tutorial2_wrong.eps \
       docs/tutorial2_wrong.png \
       docs/tutorial.eps \
       docs/tutorial.png

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = quesoglc.pc

dist-hook:
	mkdir -p -- $(distdir)/build/m4

.PHONY: doc

doc: $(DOCS)
	cd docs && doxygen && cd ..
