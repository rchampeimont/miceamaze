AC_DEFUN([AX_CHECK_FRIBIDI], [
  PKG_CHECK_MODULES([FRIBIDI], [fribidi >= 0.10.4], ,[
    AC_CHECK_LIB(fribidi, fribidi_log2vis,
                 [LIBS="-lfribidi $LIBS"],
                 no_fribidi="yes"
                )
  ])
])
