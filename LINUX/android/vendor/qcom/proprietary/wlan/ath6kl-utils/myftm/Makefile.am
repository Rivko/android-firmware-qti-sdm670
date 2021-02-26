AM_CFLAGS = -Wall \
             -g -O0 \
             -DLIBNL_2 \
             $(LIBNL_CFLAGS) \
             $(ATH6KL_UTILS_CFLAGS) \
             -DCONFIG_FTM_WLAN \
             -DCONFIG_FTM_WLAN_AUTOLOAD \
             -I $(top_srcdir)/libathtestcmd \
             -I $(top_srcdir)/libtcmd \
	     -I $(top_srcdir)/libathtestcmd/include

requiredlibs := -lrt $(ATH6KL_UTILS_LIBS) \
               $(LIBNL_LIBS)

if USE_GLIB
AM_CFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
requiredlibs += $(GLIB_LIBS)
endif

if MDM
AM_CFLAGS += -DMDM
endif

c_sources = myftm_main.c \
            myftm_wlan.c \
            myftm_qtip.c

requiredlibs += -lz ../libathtestcmd/libtestcmd6174.a \
               ../libathtestcmd/libtlvutil/libtlvutil.a \
               ../libtcmd/libtcmd.a

myftm_LDADD = $(requiredlibs)
myftm_SOURCES = $(c_sources)

bin_PROGRAMS = myftm
