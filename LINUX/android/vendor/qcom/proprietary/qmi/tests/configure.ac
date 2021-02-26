AC_PREREQ(2.61)
AC_INIT([qmi-simple-ril], 1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AM_MAINTAINER_MODE
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])


AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AS
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([QMI], [qmi])
AC_SUBST([QMI_CFLAGS])
AC_SUBST([QMI_LIBS])

PKG_CHECK_MODULES([QMIFRAMEWORK], [qmi-framework])
AC_SUBST([QMIFRAMEWORK_CFLAGS])
AC_SUBST([QMIFRAMEWORK_LIBS])

AS_CASE([$host],
    [arm*], [ARM=yes],
    [ARM=no]
)

AC_ARG_WITH([common_includes],
      AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
         [Specify the location of the common headers]),
      [common_incdir=$withval],
      with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -I${common_incdir}"
fi

AM_CONDITIONAL(ARM, [test "x$ARM" = "xyes"])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])
AC_CONFIG_FILES([ \
        Makefile
        ])
AC_OUTPUT
