AC_PREREQ([2.62])
AC_INIT([yane], [0.2.0], [tosi.kizluk@gmail.com])
AC_CONFIG_AUX_DIR([buildaux])
AM_INIT_AUTOMAKE([foreign -Wall])
AC_CONFIG_SRCDIR([src/yane.c])

AC_PROG_CC
AC_PROG_CC_C_O
AM_PROG_CC_C_O
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CHECK_HEADERS([getopt.h stdarg.h stdio.h stdlib.h unistd.h])

AC_CONFIG_FILES([Makefile img/Makefile src/Makefile src/common/Makefile src/cpu/Makefile])
AC_CONFIG_HEADERS([src/config.h])
AC_OUTPUT

