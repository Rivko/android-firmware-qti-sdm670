
AC_PREREQ(2.61)

AC_INIT([mpdecision],1.0.0)

AM_INIT_AUTOMAKE([-Wall -Wno-error foreign])

AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_ARG_WITH([sanitized-headers],
        [AS_HELP_STRING([--with-sanitized-headers=DIR],[location of the sanitized Linux kernel headers])],
        [CFLAGS="${CFLAGS} -I$withval"])

AC_ARG_WITH([additional-includes],
	[AS_HELP_STRING([--with-additional-includes=DIR],[location of additional headers])],
	[CFLAGS="${CFLAGS} -I$withval"])



AC_SUBST(CFLAGS)

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

AC_ARG_ENABLE([target-msm8960],
	AC_HELP_STRING([--enable-target-msm8960],
		[Enable conditional compile for target msm8960 [default=no]]),
	[target_msm8960="${enableval}"],
	target_msm8960=no)

AC_ARG_ENABLE([target-msm8610],
	AC_HELP_STRING([--enable-target-msm8610],
		[Enable conditional compile for target msm8610 [default=no]]),
	[target_msm8610="${enableval}"],
	target_msm8600=no)

AC_ARG_ENABLE([target-msm8226],
	AC_HELP_STRING([--enable-target-msm8226],
		[Enable conditional compile for target msm8226 [default=no]]),
	[target_msm8226="${enableval}"],
	target_msm8226=no)

AC_ARG_ENABLE([target-msm8974],
	AC_HELP_STRING([--enable-target-msm8974],
		[Enable conditional compile for target msm8974 [default=no]]),
	[target_msm8974="${enableval}"],
	target_msm8974=no)

AC_ARG_ENABLE([target-msm8916],
	AC_HELP_STRING([--enable-target-msm8916],
		[Enable conditional compile for target msm8916 [default=no]]),
	[target_msm8916="${enableval}"],
	target_msm8916=no)

# Comment out MPCTL. Enable it only if Perflock is needed which is part of android-perf repo
#AM_CONDITIONAL(USE_MPCTL_SOCKET, test "x${target_msm8974}" = "xyes")
#AM_CONDITIONAL(USE_MPCTL_SOCKET, test "x${target_msm8610}" = "xyes")
#AM_CONDITIONAL(USE_MPCTL_SOCKET, test "x${target_msm8226}" = "xyes")
#AM_CONDITIONAL(USE_MPCTL_SOCKET, test "x${target_msm8916}" = "xyes")
AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_CONFIG_FILES([ \
	Makefile
	])
AC_OUTPUT
