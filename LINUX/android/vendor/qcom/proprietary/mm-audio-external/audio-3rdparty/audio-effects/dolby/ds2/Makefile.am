ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -g  -O3 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        -I ./inc/ \
        -I $(WORKSPACE)/system/core/include/ \
        $(TINYALSA_CFLAGS) \
        $(SYSTEM_MEDIA_CFLAGS)

library_includedir = $(includedir)/audio-effects/ds2
library_include_HEADERS = ./inc/dap_hal_api.h

c_sources = src/dap_hal_api.c \
            src/dap_hal_util.c

lib_LTLIBRARIES = libhwdaphal.la
libhwdaphal_la_CC = @CC@
libhwdaphal_la_SOURCES = $(c_sources)
libhwdaphal_la_LIBADD = -lcutils
libhwdaphal_la_CFLAGS = $(AM_CFLAGS)
libhwdaphal_la_LDFLAGS = -shared -avoid-version
