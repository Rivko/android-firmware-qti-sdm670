# Makefile.am - Automake script for audio ftm

ACLOCAL_AMFLAGS = -I m4


if USE_LEGACY
SUBDIRS = legacy_intf/src/
else
SUBDIRS = family-b/src/
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audio_ftm.pc
EXTRA_DIST = $(pkgconfig_DATA)
