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

bin_PROGRAMS = qseecom_sample_client
h_sources = SecureUILib.h
sampleclient_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
qseecom_sample_client_SOURCES = qseecom_sample_client.c $(h_sources)
qseecom_sample_client_CPPFLAGS = $(AM_CPPFLAGS)
qseecom_sample_client_CFLAGS = $(AM_CFLAGS)
qseecom_sample_client_LDFLAGS = -lpthread -ldl $(SECUREMSMNOSHIP_LIBS)
qseecom_sample_client_LDADD = $(SECUREMSMNOSHIP_LIBS)
