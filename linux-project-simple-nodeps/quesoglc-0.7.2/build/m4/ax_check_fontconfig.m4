AC_DEFUN([AX_CHECK_FONTCONFIG], [
  PKG_CHECK_MODULES([FONTCONFIG], [fontconfig >= 2.2] , ,[

    AC_CHECK_HEADER(fontconfig/fontconfig.h,
      AC_CHECK_LIB(fontconfig, FcInit,
                   [LIBS="-lfontconfig $LIBS"],
                   no_fc="yes"
                  )
    )
  ])
])
