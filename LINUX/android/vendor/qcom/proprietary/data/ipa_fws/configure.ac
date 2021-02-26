AC_PREREQ(2.61)
AC_INIT(ipa-fws, 1.0.0)
AM_INIT_AUTOMAKE([foreign])

AC_ARG_ENABLE([target-mdmcalifornium],
	AC_HELP_STRING([--enable-target-mdmcalifornium],
		[Enable conditional compile for target mdmcalifornium]),
	[target_mdmcalifornium="${enableval}"],
	target_mdmcalifornium=no)

AC_ARG_ENABLE([target-sdxhedgehog],
	AC_HELP_STRING([--enable-target-sdxhedgehog],
		[Enable conditional compile for target sdxhedgehog]),
	[target_sdxhedgehog="${enableval}"],
	target_sdxhedgehog=no)

AM_CONDITIONAL([IS_MDMCALIFORNIUM], [test "x$target_mdmcalifornium" = xyes])
AM_CONDITIONAL([IS_SDXHEDGEHOG], [test "x$target_sdxhedgehog" = xyes])

AC_OUTPUT(Makefile)
AC_OUTPUT
