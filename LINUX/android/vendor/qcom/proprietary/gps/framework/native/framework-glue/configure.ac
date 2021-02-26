# configure.ac -- Autoconf script for gps location-service-glue
#
# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the gps location-service-glue package version 1.0.0
AC_INIT([loc-glue],1.0.0)
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
PKG_CHECK_MODULES([GPSUTILS], [gps-utils])
AC_SUBST([GPSUTILS_CFLAGS])
AC_SUBST([GPSUTILS_LIBS])

PKG_CHECK_MODULES([LOCCORE], [loc-core])
AC_SUBST([LOCCORE_CFLAGS])
AC_SUBST([LOCCORE_LIBS])

PKG_CHECK_MODULES([LBSCORE], [lbs-core])
AC_SUBST([LBSCORE_CFLAGS])
AC_SUBST([LBSCORE_LIBS])

PKG_CHECK_MODULES([ULP2], [ulp2])
AC_SUBST([ULP2_CFLAGS])
AC_SUBST([ULP2_LIBS])

PKG_CHECK_MODULES([LOCFLP], [location-flp])
AC_SUBST([LOCFLP_CFLAGS])
AC_SUBST([LOCFLP_LIBS])

PKG_CHECK_MODULES([LOCGF], [location-geofence])
AC_SUBST([LOCGF_CFLAGS])
AC_SUBST([LOCGF_LIBS])

PKG_CHECK_MODULES([LOCSERVICE], [location-service])
AC_SUBST([LOCSERVICE_CFLAGS])
AC_SUBST([LOCSERVICE_LIBS])

PKG_CHECK_MODULES([LOCBASEUTIL], [loc-base-util])
AC_SUBST([LOCBASEUTIL_CFLAGS])
AC_SUBST([LOCBASEUTIL_LIBS])

PKG_CHECK_MODULES([QMIF], [qmi-framework])
AC_SUBST([QMIF_CFLAGS])
AC_SUBST([QMIF_LIBS])

PKG_CHECK_MODULES([DATA], [data])
AC_SUBST([DATA_CFLAGS])
AC_SUBST([DATA_LIBS])

PKG_CHECK_MODULES([QMI], [qmi])
AC_SUBST([QMI_CFLAGS])
AC_SUBST([QMI_LIBS])

PKG_CHECK_MODULES([DSUTILS], [dsutils])
AC_SUBST([DSUTILS_CFLAGS])
AC_SUBST([DSUTILS_LIBS])

PKG_CHECK_MODULES([DATAITEMS], [data-items])
AC_SUBST([DATAITEMS_CFLAGS])
AC_SUBST([DATAITEMS_LIBS])

AC_ARG_ENABLE(target,
    [AS_HELP_STRING([--enable-target=TARGET], [Specify the target product to build])],
    [TARGET=$enableval],
    [TARGET=none]
)

# qcmap disabled targets
# eg: test "x$TARGET" = "xsdxpoorwills" || test "x$TARGET" = "x8x96autogvmquintcu"
if (test "x$TARGET" = "x8x96autogvmquintcu"); then
    TARGET_QCMAP_DISABLED=yes
else
    TARGET_QCMAP_DISABLED=no
fi

if (test "x$TARGET_QCMAP_DISABLED" != "xyes"); then
    ## include LocNetiface
    AC_MSG_NOTICE([Target supports QCMAP.])
    PKG_CHECK_MODULES([LOCNETIFACE], [loc-net-iface])
    AC_SUBST([LOCNETIFACE_CFLAGS])
    AC_SUBST([LOCNETIFACE_LIBS])
else
    AC_MSG_NOTICE([Target does not supports QCMAP.])
fi

AM_CONDITIONAL(USE_QCMAP, test "x$TARGET_QCMAP_DISABLED" != "xyes")

AC_ARG_WITH([core_includes],
      AC_HELP_STRING([--with-core-includes=@<:@dir@:>@],
         [Specify the location of the core headers]),
      [core_incdir=$withval],
      with_core_includes=no)

if test "x$with_core_includes" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -I${core_incdir}"
fi

AC_SUBST([CPPFLAGS])

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

AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_CONFIG_FILES([ \
        Makefile \
        loc-glue.pc \
        ])

AC_OUTPUT
