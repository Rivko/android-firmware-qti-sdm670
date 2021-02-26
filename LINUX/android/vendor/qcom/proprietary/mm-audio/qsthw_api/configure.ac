#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for sound_trigger
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the sound_trigger package version 1.0.0
AC_INIT([qsthw_api],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([qsthw_api.c])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_PROG_AS
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

AM_CONDITIONAL([QSTHW_API],[test x$BOARD_SUPPORTS_QSTHW_API = xtrue])

AC_CONFIG_FILES([ \
        Makefile \
        qsthw_api.pc
        ])

AC_OUTPUT
