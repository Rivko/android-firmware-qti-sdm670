AC_PREREQ(2.61)
AC_INIT([data],
        1.0.0)
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
AC_CONFIG_SRCDIR([netmgr/src/netmgr_main.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([DIAG], [diag])
AC_SUBST([DIAG_CFLAGS])
AC_SUBST([DIAG_LIBS])

PKG_CHECK_MODULES([XMLLIB], [xmllib])
AC_SUBST([XMLLIB_CFLAGS])
AC_SUBST([XMLLIB_LIBS])

PKG_CHECK_MODULES([DSUTILS], [dsutils])
AC_SUBST([DSUTILS_CFLAGS])
AC_SUBST([DSUTILS_LIBS])

PKG_CHECK_MODULES([CONFIGDB], [configdb])
AC_SUBST([CONFIGDB_CFLAGS])
AC_SUBST([CONFIGDB_LIBS])

PKG_CHECK_MODULES([QMI], [qmi])
AC_SUBST([QMI_CFLAGS])
AC_SUBST([QMI_LIBS])

PKG_CHECK_MODULES([QMIFRAMEWORK], [qmi-framework])
AC_SUBST([QMIFRAMEWORK_CFLAGS])
AC_SUBST([QMIFRAMEWORK_LIBS])

PKG_CHECK_MODULES([QMICLIENTHELPER], [qmi-client-helper])
AC_SUBST([QMICLIENTHELPER_CFLAGS])
AC_SUBST([QMICLIENTHELPER_LIBS])

PKG_CHECK_MODULES([DATAOSS], [data-oss])
AC_SUBST([DATAOSS_CFLAGS])
AC_SUBST([DATAOSS_LIBS])

# Library configs
AC_ARG_WITH([lib_path],
      AC_HELP_STRING([--with-lib-path=@<:@dir@:>@],
         [Specify the location of the libraries]),
      [std_libdir=$withval],
      with_lib_path=no)

if test "x$with_lib_path" != "xno"; then
   LDFLAGS="${LDFLAGS} -L${std_libdir}"
fi

AC_ARG_WITH([stderr],
      AC_HELP_STRING([--with-stderr=@<:@dir@:>@],
         [Enable stderr logging]),
      [],
      with_stderr=no)

if test "x$with_stderr" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_LOG_STDERR"
fi

AC_ARG_WITH([qxdm],
      AC_HELP_STRING([--with-qxdm=@<:@dir@:>@],
         [Enable qxdm logging]),
      [],
      with_qxdm=no)

if test "x$with_qxdm" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_LOG_QXDM"
fi

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

AC_ARG_WITH([common_includes],
      AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
         [Specify the location of the common headers]),
      [common_incdir=$withval],
      with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -I${common_incdir}"
fi

AC_ARG_WITH(sanitized-headers,
      AS_HELP_STRING([--with-sanitized-headers=DIR],
         [Specify the location of the sanitized Linux headers]),
      [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

AC_ARG_WITH([updated_linux_libc_headers],
      AC_HELP_STRING([--with-updated-linux-libc-headers=@<:@dir@:>@],
         [With updated linux-libc-headers]),
      [],
      with_updated_linux_libc_headers=no)

if test "x$with_updated_linux_libc_headers" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_WITH_UPDATED_HEADERS"
fi


AC_ARG_ENABLE(target,
  [AS_HELP_STRING([--enable-target=TARGET], [Specify the target product to build])],
  [TARGET=$enableval],
  [TARGET=none]
)

if test "x$TARGET" = "x9615-cdp"; then
  TARGET_IS_9615=yes
  AC_MSG_NOTICE([Target is 9615.])
  CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_TARGET_MDM9X15"
  CFLAGS="${CFLAGS} -DFEATURE_DATA_TARGET_MDM9X15"

elif test "x$TARGET" = "xmdm9625"; then
  TARGET_IS_9625=yes
  AC_MSG_NOTICE([Target is 9625.])
  CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_TARGET_MDM9X25"
  CFLAGS="${CFLAGS} -DFEATURE_DATA_TARGET_MDM9X25"

elif test "x$TARGET" = "xmdm9635"; then
  CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_MDM_SINGLE_QMUX_CHANNEL -DNETMGR_USE_RMNET_DATA -DFEATURE_DATA_TARGET_MDM9X35"
  CFLAGS="${CFLAGS} -DFEATURE_DATA_MDM_SINGLE_QMUX_CHANNEL -DNETMGR_USE_RMNET_DATA -DFEATURE_DATA_TARGET_MDM9X35"

elif test "x$TARGET" = "xmdm9607"; then
  CPPFLAGS="${CPPFLAGS} -DFEATURE_DATA_TARGET_MDM9607"
  CFLAGS="${CFLAGS} -DFEATURE_DATA_TARGET_MDM9607"
fi

AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])

AC_CONFIG_FILES([ \
        Makefile \
        netmgr/src/Makefile \
        radish/Makefile \
        qdi/src/Makefile \
        dsi_netctrl/src/Makefile \
        tests/dsi_netctrl/src/Makefile \
        qdp/src/Makefile \
        ipacm-diag/src/Makefile \
        ipacm-perf/src/Makefile \
        data.pc \
        mobileap/ipc/Makefile qti/src/Makefile \
        mobileap/server/src/Makefile \
        mobileap/client/src/Makefile \
        mobileap/webserver/qcmap_web_client/src/Makefile \
        mobileap/webserver/cgi/Makefile \
        mobileap/eMBMS_multiclient/tunneling/src/Makefile \
        mobileap/eMBMS_multiclient/qmi_ip_multiclient/src/Makefile
        ])
AC_OUTPUT
