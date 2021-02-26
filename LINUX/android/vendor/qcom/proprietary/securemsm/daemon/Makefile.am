AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(SECUREMSM-NOSHIP_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG \
    -DOE \
    $(SECUREMSMNOSHIP_CFLAGS)

#init_ddir = ${sysconfdir}/init.d
#init_d_SCRIPTS = start_qseecomd

bin_PROGRAMS = qseecomd
qseecomd_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
qseecomd_SOURCES = qseecomd.c
qseecomd_CPPFLAGS = $(AM_CPPFLAGS)
qseecomd_CFLAGS = $(AM_CFLAGS)
qseecomd_LDFLAGS = -lpthread -ldl $(SECUREMSMNOSHIP_LIBS)
qseecomd_LDADD = $(SECUREMSMNOSHIP_LIBS)
