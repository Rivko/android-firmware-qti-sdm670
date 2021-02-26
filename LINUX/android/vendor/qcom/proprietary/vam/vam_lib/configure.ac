
AC_PREREQ(2.61)

AC_INIT([VAM_LIB], 1.0.0)

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



# Check for libraries

#PKG_CHECK_MODULES([QMIF], [qmi-framework])

#AC_SUBST([QMIF_CFLAGS])

#AC_SUBST([QMIF_LIBS])



#PKG_CHECK_MODULES([LOCPLA], [loc-pla])

#AC_SUBST([LOCPLA_CFLAGS])

#AC_SUBST([LOCPLA_LIBS])



#PKG_CHECK_MODULES([LOC], [loc-hal])

#AC_SUBST([LOC_CFLAGS])

#AC_SUBST([LOC_LIBS])



#PKG_CHECK_MODULES([LOCATIONCORE], [lbs-core])

#AC_SUBST([LOCATIONCORE_CFLAGS])

#AC_SUBST([LOCATIONCORE_LIBS])



#PKG_CHECK_MODULES([IZATCORE], [izat-core])

#AC_SUBST([IZATCORE_CFLAGS])

#AC_SUBST([IZATCORE_LIBS])



#PKG_CHECK_MODULES([LBSCORE], [lbs-core])

#AC_SUBST([LBSCORE_CFLAGS])

#AC_SUBST([LBSCORE_LIBS])





AS_CASE([$host],

        [arm*], [ARM=yes],

        [ARM=no]

)



ARM=yes

#AC_ARG_WITH([hardware_include],

#                  AC_HELP_STRING([--with-hardware-include=@<:@dir@:>@],

#                                 [Specify the location of the hardware headers]),

#                                 [hardware_incdir=$withval],

#                                 with_hardware_include=no)



#if test "x$with_hardware_include" != "xno"; then

#       CPPFLAGS="${CPPFLAGS} -I${hardware_incdir}"

#fi



#AC_ARG_WITH([hardware_libraries],

#                 AC_HELP_STRING([--with-hardware-libraries=@<:@dir@:>@],

#                                     [Specify the location of the hardware libraries]),

#                                     [hardware_libdir=$withval],

#                                          with_hardware_libraries=no)



#if test "x$with_hardware_libraries" != "xno"; then

#    LDFLAGS="${LDFLAGS} -L${hardware_libdir}"

#fi



#AC_ARG_WITH([glib],

#      AC_HELP_STRING([--with-glib],

#         [enable glib, building HLOS systems which use glib]))



#if (test "x${with_glib}" = "xyes"); then

#        AC_DEFINE(ENABLE_USEGLIB, 1, [Define if HLOS systems uses glib])

#        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,

#                                AC_MSG_ERROR(GThread >= 2.16 is required))

#        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,

#                                AC_MSG_ERROR(GLib >= 2.16 is required))

#        GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"

#        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

#        AC_SUBST(GLIB_CFLAGS)

#        AC_SUBST(GLIB_LIBS)

#fi



#CPPFLAGS="${CPPFLAGS} --host=arm"



#AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

LT_INIT

AC_SUBST([CFLAGS])

AC_SUBST([CPPFLAGS])

AC_SUBST([LIBS])



AC_CONFIG_FILES([ \
        Makefile  \
	src/Makefile \
        ])

AC_OUTPUT
