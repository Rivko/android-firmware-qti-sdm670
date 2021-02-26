
AC_PREREQ(2.61)

AC_INIT([VAM], 1.0.0)

AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])



# Check for programs

LT_INIT

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


AS_CASE([$host],
        [arm*], [ARM=yes],
        [ARM=no]
)

ARM=yes

LT_INIT

AC_SUBST([CFLAGS])

AC_SUBST([CPPFLAGS])

AC_SUBST([LIBS])

AC_ARG_WITH(sanitized-headers,
      AS_HELP_STRING([--with-sanitized-headers=DIR],
         [Specify the location of the sanitized Linux headers]),
      [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

AC_ARG_WITH([snpe-enable],
        AC_HELP_STRING([--with-snpe],
        [Enable SNPE]))
AM_CONDITIONAL(SNPE_ENABLE, [test "x${with_snpe}" = "xyes"])

AC_CONFIG_FILES([ \
        Makefile  \
    test_engines/TestEngine1/Makefile \
    test_engines/TestEngine2/Makefile \
    test_engines/GroundTruthEngine/Makefile \
    deeplearning_engine/Makefile \
    vam_utils/JSON/Makefile \
    vam_utils/VAMReport/Makefile \
    engine_generator/Makefile \
])

AC_OUTPUT
