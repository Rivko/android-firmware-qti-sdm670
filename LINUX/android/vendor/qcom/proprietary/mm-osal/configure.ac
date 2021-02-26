#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for mm-osal
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the mm-osal package version 1.0.0
AC_INIT([mm-osal],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([-Wall -Werror gnu foreign subdir-objects])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE

# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_ENABLE_SHARED

#release versioning
MM_OSAL_MAJOR_VERSION=1
MM_OSAL_MINOR_VERSION=0
MM_OSAL_MICRO_VERSION=0

MM_OSAL_LIBRARY_VERSION=$MM_OSAL_MAJOR_VERSION:$MM_OSAL_MINOR_VERSION:$MM_OSAL_MICRO_VERSION
AC_SUBST(MM_OSAL_LIBRARY_VERSION)

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AM_PROG_AR
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

AC_ARG_WITH([utils-headers],
            [AS_HELP_STRING([--with-utils-inc=DIR],[location of common headers])],
            [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([cutils-headers],
            [AS_HELP_STRING([--with-cutils-inc=DIR],[location of common headers])],
            [CPPFLAGS="$CPPFLAGS -I$withval"])


AC_ARG_WITH(sanitized-headers,
        AS_HELP_STRING([--with-sanitized-headers=DIR],
                [Specify the location of the sanitized Linux headers]),
        [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([glib],
      AC_HELP_STRING([--with-glib],
         [enable glib, Build against glib. Use this when building for HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
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

AC_ARG_ENABLE(machine_bits,
        [AS_HELP_STRING([--enable-machine_bits=MACHINE_BITS], [Specify the compiler bit])],
        [MACHINE_BITS=$enableval],
        [MACHINE_BITS=none]
)
AM_CONDITIONAL(MACHINE_64BIT, test "x${MACHINE_BITS}" = "x64bit")
AM_CONDITIONAL(MACHINE_32BIT, test "x${MACHINE_BITS}" = "x32bit")

AS_CASE([$host],
        [arm*], [ARM=yes],
        [ARM=no]
)
AM_CONDITIONAL(ARM, [test "x$ARM" = "xyes"])

AS_CASE([$host],
        [aarch64*], [ARM64=yes],
        [ARM64=no]
)
AM_CONDITIONAL(ARM64, [test "x$ARM64" = "xyes"])

AM_CONDITIONAL(AARCH64_32COMPILER, [test "x${MACHINE_BITS}" = "x64bit" && test "x$ARM" = "xyes"])


AC_SUBST([CFLAGS])
AC_SUBST([CC])
AC_CONFIG_FILES([
        Makefile \
        src/Makefile
        ])
AC_OUTPUT

