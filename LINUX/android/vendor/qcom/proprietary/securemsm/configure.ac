AC_PREREQ(2.61)
AC_INIT([securemsm], 1.0.0)
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
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_CXX

#LT_INIT

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([SECUREMSMNOSHIP], [securemsm-noship])
AC_SUBST([SECUREMSMNOSHIP_LIBS])
AC_SUBST([SECUREMSMNOSHIP_CFLAGS])

AC_ARG_WITH(sanitized-headers,
AS_HELP_STRING([--with-sanitized-headers=DIR],
[Specify the location of the sanitized Linux headers]),
[CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH([kernel],
AC_HELP_STRING([--with-kernel=@<:@dir@:>@],
[Specify the location of the Linux kernel headers]),
[kerneldir=$withval],
with_kernel=no)

if test "x$with_kernel" != "xno"; then
   CFLAGS="${CFLAGS} -I${kerneldir}/include -I${kerneldir}/arch/arm/include"
fi

AS_CASE([$host],
    [arm*], [ARM=yes],
    [ARM=no]
)

AM_CONDITIONAL(ARM, [test "x$ARM" = "xyes"])
AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([CC])
#AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([ \
    Makefile \
    sampleclient/Makefile \
    daemon/Makefile \
    securemsm.pc
    ])
AC_OUTPUT
