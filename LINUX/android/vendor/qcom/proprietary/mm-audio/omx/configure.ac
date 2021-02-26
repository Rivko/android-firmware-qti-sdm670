#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for omx
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the omx package version 1.0.0
AC_INIT([omx],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# Verifies the --srcdir is correct by checking for the path
AC_CONFIG_SRCDIR([alsa-utils/qdsp6/src/omx_alsa_utils.c])
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
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

AC_ARG_ENABLE([target-apq8016-le20],
        AC_HELP_STRING([--enable-target-apq8016-le20],
                [Enable conditional compile for target apq8016 [default=no]]),
        [target_apq8016_le20="${enableval}"],
        target_apq8016_le20=no)
AM_CONDITIONAL(TARGET_APQ8016_LE_2_0, test "x$target_apq8016_le20" = "xyes")

if (test "x$target_apq8016_le20" = "xyes"); then
AC_SUBST([MM_CORE_CFLAGS],[-I ${WORKSPACE}/mm-video-oss/mm-core/inc])
AC_SUBST([MM_CORE_LIBS],[-lOmxCore])
else
AC_SUBST([MM_CORE_CFLAGS],[-I ${WORKSPACE}/hardware/qcom/media/mm-core/inc])
AC_SUBST([MM_CORE_LIBS],[-lmm-omxcore])
fi

AC_ARG_WITH([feature-omx-adec-g711],
      AC_HELP_STRING([--with-feature-omx-adec-g711],
         [Enable feature omx adec g711 [default=no]]),
      [feature_omx_adec_g711=yes],
      feature_omx_adec_g711=no)

AM_CONDITIONAL(FEATURE_OMX_ADEC_G711, test "x$feature_omx_adec_g711" = "xyes")

PKG_CHECK_MODULES([AUDIOALSA],[audioalsa])
AC_SUBST([AUDIOALSA_CFLAGS])
AC_SUBST([AUDIOALSA_LIBS])
AC_SUBST([SYSTEM_MEDIA_CFLAGS],[-I$(includedir)/])
PKG_CHECK_MODULES([OMX_NOSHIP],[omx-noship])
AC_SUBST([OMX_NOSHIP_LIBS])

AC_ARG_WITH(sanitized-headers,
        AS_HELP_STRING([--with-sanitized-headers=DIR],
                [Specify the location of the sanitized Linux headers]),
        [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

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

AC_CONFIG_FILES([ \
        Makefile \
        ./adec-aac/qdsp6/Makefile \
        ./adec-alac/qdsp6/Makefile \
        ./adec-amrwbplus/qdsp6/Makefile \
        ./adec-ape/qdsp6/Makefile \
        ./adec-wma/qdsp6/Makefile \
        ./adec-amr/qdsp6/Makefile \
        ./adec-evrc/sw/Makefile \
        ./adec-qcelp13/sw/Makefile \
        omx.pc
        ])

if (test "x$feature_omx_adec_g711" = "xyes"); then
AC_CONFIG_FILES([ \
        ./adec-g711/qdsp6/Makefile
        ])
fi

AC_OUTPUT
