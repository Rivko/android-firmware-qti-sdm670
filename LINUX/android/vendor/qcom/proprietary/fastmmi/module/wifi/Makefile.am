
ACLOCAL_AMFLAGS = -I m4

brary_includedir = $(pkgincludedir)
lib_LTLIBRARIES = libmmi_wifi.la
libmmi_wifi_la_CC = @CC@
libmmi_wifi_la_CPPFLAGS = -O2 -g -Wno-all -Wno-error -Wno-unused-parameter -I. -I$(WORKSPACE)/system/core/include $(XML2_CFLAGS) -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS) -I../../libmmi
libmmi_wifi_la_LDFLAGS = libcutils libutils libc libc++ ../../libmmi/libmmi.la -avoid-version
# source to compile
libmmi_wifi_la_SOURCES = wifi_uav.cpp

if USE_GLIB
libmmi_wifi_la_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
libmmi_wifi_la_LDFLAGS += $(GLIB_LIBS)
endif

if NO_UI
libmmi_wifi_la_CPPFLAGS +=  -DNO_UI
endif

if ANDROID
libmmi_wifi_la_CPPFLAGS +=  -DANDROID
endif

if USE_LIBMINIUI
libmmi_wifi_la_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
