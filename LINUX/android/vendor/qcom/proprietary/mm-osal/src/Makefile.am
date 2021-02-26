ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -Wundef             \
            -Wall               \
            -Wstrict-prototypes \
            -Wno-trigraphs      \
            -g -O0              \
            -fno-short-enums

AM_CPPFLAGS  = -include sys/stat.h
AM_CPPFLAGS += -include sys/time.h
AM_CPPFLAGS += -D_GNU_SOURCE
AM_CPPFLAGS += -D_ANDROID_

AM_CPPFLAGS += -I../inc -I.

AM_CPPFLAGS += -D__align=__alignx
AM_CPPFLAGS += -D__alignx\(x\)=__attribute__\(\(__aligned__\(x\)\)\)
AM_CPPFLAGS += -Dinline=__inline
AM_CPPFLAGS += -include stddef.h

AM_CPPFLAGS += -D__USE_FILE_OFFSET64
AM_CPPFLAGS += -D__USE_LARGEFILE64
AM_CPPFLAGS += -D_LARGEFILE_SOURCE
AM_CPPFLAGS += -D_LARGEFILE64_SOURCE
AM_CPPFLAGS += -D_FILE_OFFSET_BITS=64

# source to compile
libmmosal_la_SOURCES = MMMalloc.c \
              MMCriticalSection.c \
              MMDebug.c           \
              MMTimer.c           \
              MMThread.c          \
              MMSignal.c          \
              MMFile.c            \
              AEEstd.c


lib_LTLIBRARIES = libmmosal.la

libmmosal_la_CC = @CC@
libmmosal_la_CFLAGS = $(AM_CFLAGS) -fPIC
libmmosal_la_CPPFLAGS = $(AM_CPPFLAGS)
if USE_GLIB
libmmosal_la_CPPFLAGS += @GLIB_CFLAGS@
endif
libmmosal_la_LDFLAGS = -shared @GLIB_LIBS@ -llog -lcutils -lutils -lpthread -avoid-version -lrt
