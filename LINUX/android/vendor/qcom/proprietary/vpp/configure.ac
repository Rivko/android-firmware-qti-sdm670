#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the vpp package version 1.0.0
AC_INIT([vpp],1.0.0)
#AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_INIT_AUTOMAKE([-Werror -Wall gnu foreign subdir-objects])
AM_MAINTAINER_MODE

# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

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

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_ARG_WITH([sanitized-headers],
        [AS_HELP_STRING([--with-sanitized-headers=DIR],
                        [specify the location of the sanitized Linux headers])],
        [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([glib],
        AC_HELP_STRING([--with-glib],[enable glib, building HLOS systems which use glib]))

AC_ARG_WITH([vppnoship-headers],
        [AS_HELP_STRING([--with-vppnoship-headers=DIR],
                        [location of vpp-noship headers])],
        [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_ENABLE([vpp-lib],
        [AS_HELP_STRING([--enable-vpp-lib], [enable make vpp lib])],
        [vpp_lib=$enableval],
        [vpp_lib=no])

AC_ARG_ENABLE([vpp-test],
        [AS_HELP_STRING([--enable-vpp-test], [enable make vpp test app])],
        [vpp_test=$enableval],
        [vpp_test=no])

AC_ARG_ENABLE([target],
        [AS_HELP_STRING([--enable-target=TARGET], [specify the target product])],
        [target=$enableval],
        [target=none])

AM_CONDITIONAL(TARGET_MSM8998, [test "x$target" = "xmsm8998" -o "x$target" = "xapq8098"])
AM_CONDITIONAL(TARGET_MSM8996, [test "x$target" = "xmsm8996" -o "x$target" = "xapq8096"])
AM_CONDITIONAL(TARGET_MSM8953, [test "x$target" = "xmsm8953"])
AM_CONDITIONAL(TARGET_MSM8952, [test "x$target" = "xmsm8952"])
AM_CONDITIONAL(TARGET_MSM8937, [test "x$target" = "xmsm8937"])

DSP_RPC_LIB=cdsprpc
case "$target" in
    (msm8998|apq8098|msm8996|apq8096)
        VPP_TARGET_USES_HVX=yes;
        VPP_TARGET_USES_FRC=yes;
        VPP_TARGET_USES_HVX_CORE=yes;
        DSP_RPC_LIB=adsprpc;
        CPPFLAGS="$CPPFLAGS -DVPP_TARGET_USES_HVX=1 \
                -DVPP_TARGET_USES_FRC=1 \
                -DVPP_EXTERNAL_EXTRADATA_TYPE=EXTRADATA_BUFFER_FORMAT_OMX \
                -DVPP_RES_DI_IS_DISABLED_FOR_AUTO=VPP_RESOLUTION_UHD \
                -DVPP_GRALLOC_DOES_NOT_EXIST";;
    (msm8953|msm8952|msm8937)
        VPP_TARGET_USES_GPU=yes;
        CPPFLAGS="$CPPFLAGS -DVPP_TARGET_USES_GPU=1 \
                -DVPP_EXTERNAL_EXTRADATA_TYPE=EXTRADATA_BUFFER_FORMAT_OMX \
                -DVPP_GRALLOC_DOES_NOT_EXIST";;
    (*) AC_MSG_ERROR([unknown target: $target])
esac

AM_CONDITIONAL(VPP_TARGET_USES_GPU, [test "x$VPP_TARGET_USES_GPU" = "xyes"])
AM_CONDITIONAL(VPP_TARGET_USES_HVX, [test "x$VPP_TARGET_USES_HVX" = "xyes"])
AM_CONDITIONAL(VPP_TARGET_USES_FRC, [test "x$VPP_TARGET_USES_FRC" = "xyes"])
AM_CONDITIONAL(VPP_TARGET_USES_HVX_CORE, [test "x$VPP_TARGET_USES_HVX_CORE" = "xyes"])
AC_SUBST([DSP_RPC_LIB])
AM_CONDITIONAL(MAKE_VPP_LIB, [test "x$vpp_lib" = "xyes"])
AM_CONDITIONAL(MAKE_VPP_TEST, [test "x$vpp_test" = "xyes"])


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

AC_CONFIG_FILES([Makefile])
AM_COND_IF([MAKE_VPP_LIB], [AC_CONFIG_FILES([lib/Makefile])])
AM_COND_IF([MAKE_VPP_TEST], [AC_CONFIG_FILES([test/Makefile])])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])
AC_OUTPUT
