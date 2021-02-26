# configure.ac -- Autoconf script for gps lbs-core
#
# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the gps lbs-core package version 1.0.0
AC_INIT([lbs-core],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([Makefile.am])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([QMIF], [qmi-framework])
AC_SUBST([QMIF_CFLAGS])
AC_SUBST([QMIF_LIBS])

PKG_CHECK_MODULES([GPSUTILS], [gps-utils])
AC_SUBST([GPSUTILS_CFLAGS])
AC_SUBST([GPSUTILS_LIBS])

PKG_CHECK_MODULES([LOCCORE], [loc-core])
AC_SUBST([LOCCORE_CFLAGS])
AC_SUBST([LOCCORE_LIBS])

PKG_CHECK_MODULES([LOCAPIV02], [loc-api-v02])
AC_SUBST([LOCAPIV02_CFLAGS])
AC_SUBST([LOCAPIV02_LIBS])

PKG_CHECK_MODULES([LOCDS], [loc-ds])
AC_SUBST([LOCDS_CFLAGS])
AC_SUBST([LOCDS_LIBS])

PKG_CHECK_MODULES([IZATCORE], [izat-core])
AC_SUBST([IZATCORE_CFLAGS])
AC_SUBST([IZATCORE_LIBS])

PKG_CHECK_MODULES([LOCBASEUTIL], [loc-base-util])
AC_SUBST([LOCBASEUTIL_CFLAGS])
AC_SUBST([LOCBASEUTIL_LIBS])

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
         [enable glib, building HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        AC_DEFINE(ENABLE_USEGLIB, 1, [Define if HLOS systems uses glib])
        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GThread >= 2.16 is required))
        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GLib >= 2.16 is required))
        GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
fi

AC_ARG_WITH([locflp_includes],
      AC_HELP_STRING([--with-locflp-includes=@<:@dir@:>@],
         [specify the path to locflp-includes in izat-core_git.bb]),
      [locflp_incdir=$withval],
      with_locflp_includes=no)

if test "x$with_locflp_includes" != "xno"; then
   AC_SUBST(LOCFLP_CFLAGS, "-I${locflp_incdir}")
fi

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_CONFIG_FILES([ \
        Makefile \
        lbs-core.pc
        ])

AC_OUTPUT
