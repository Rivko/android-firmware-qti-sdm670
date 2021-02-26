#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Requires autoconf tool later than 2.61
AC_PREREQ([2.61])
# Initialize the omxvideo package version 1.0.0
AC_INIT([omxvideo], 1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([gnu foreign subdir-objects])
AM_MAINTAINER_MODE
# defines some macros variable to be included by source
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_PROG_AR
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

PKG_PROG_PKG_CONFIG

AC_ARG_WITH([glib],
    AC_HELP_STRING([--with-glib],
    [enable glib, building HLOS systems which use glib]))

AC_ARG_ENABLE([use-glib],
    AC_HELP_STRING([--enable-use-glib],
    [Enable conditional compile for use glib [default=no]]),
    [use_glib="${enableval}"])

AC_ARG_WITH([glib-headers],
    [AS_HELP_STRING([--with-glib-headers=DIR],[location of common headers])],
    [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([sanitized-headers],
    [AS_HELP_STRING([--with-sanitized-headers=DIR],[location of the sanitized Linux kernel headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([sanitized-headers],
    [AS_HELP_STRING([--with-sanitized-headers=DIR],[location of the sanitized Linux kernel headers])],
    [CFLAGS="$CFLAGS -I $withval"])

AC_ARG_WITH([omx-headers],
    [AS_HELP_STRING([--with-omx-headers=DIR],[location of omx headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([graphics-headers],
    [AS_HELP_STRING([--with-graphics-headers=DIR],[location of graphics headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([stagefright-headers],
    [AS_HELP_STRING([--with-stagefright-headers=DIR],[location of stagefright headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([streamparser-headers],
    [AS_HELP_STRING([--with-streamparser-headers=DIR],[location of streamparser headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([fastcrc-headers],
    [AS_HELP_STRING([--with-fastcrc-headers=DIR],[location of fastcrc headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([utils-headers],
    [AS_HELP_STRING([--with-utils-headers=DIR],[location of utils headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([kernel-media-headers],
    [AS_HELP_STRING([--with-kernel-media-headers=DIR],[location of kernel-media-headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([xml2-headers],
    [AS_HELP_STRING([--with-xml2-headers=DIR],[location of xml2-headers])],
    [CPPFLAGS="$CPPFLAGS -I $withval"])

AC_ARG_WITH([glib-lib-dir],
    [AS_HELP_STRING([--with-glib-lib-dir=DIR],[location of glib lib directory])],
    [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([common-headers],
    [AS_HELP_STRING([--with-common-headers=DIR],[location of common headers])],
    [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([android-headers],
    [AS_HELP_STRING([--with-android-headers=DIR],[location of android headers])],
    [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([time-headers],
    [AS_HELP_STRING([--with-time-headers=DIR],[location of time-headers])],
    [CFLAGS="$CPPFLAGS $CFLAGS -I $withval"])

AM_CONDITIONAL(USE_GLIB, [test "x$use_glib" = "xyes"])

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
         [enable glib, building HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
fi

AC_SUBST([CPPFLAGS])
AC_SUBST([CFLAGS])

AC_CONFIG_FILES([ \
        Makefile \
        vtest-omx/Makefile \
        ])
AC_OUTPUT