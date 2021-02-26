# Makefile.am - Automake script for ath6kl-utils

ACLOCAL_AMFLAGS = -I m4

dirs :

dirs = libtcmd
dirs += libathtestcmd
dirs += myftm

SUBDIRS = $(dirs)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ath6kl-utils.pc
EXTRA_DIST = $(pkgconfig_DATA)
