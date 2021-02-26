AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(SECUREMSM-NOSHIP_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = ml_app
ml_app_SOURCES = app_main.c seccamlib.c sock_comm.c
ml_app_CFLAGS = $(AM_CFLAGS)

ml_app_LDFLAGS = -lpthread 
