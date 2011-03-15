AC_PREREQ([2.62])
AC_INIT([yane], [0.2.0], [tosi.kizluk@gmail.com])
AC_CONFIG_AUX_DIR([buildaux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall])
AC_CONFIG_SRCDIR([src/yane.c])

AC_PROG_CC
AC_PROG_CC_C_O
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
dnl AC_DISABLE_SHARED

AC_SUBST(LIBTOOL_DEPS)

AC_CHECK_HEADERS([getopt.h stdarg.h stdio.h stdlib.h unistd.h])

SDL_CFLAGS=`sdl-config --cflags`
SDL_LDADD=`sdl-config --libs`
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LDADD)

AC_CONFIG_FILES([Makefile img/Makefile src/Makefile src/common/Makefile src/nes/Makefile])
AC_CONFIG_HEADERS([src/config.h])
AC_OUTPUT

