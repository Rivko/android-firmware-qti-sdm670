
ACLOCAL_AMFLAGS = -I m4

brary_includedir = $(pkgincludedir)
lib_LTLIBRARIES = libmmi.la
libmmi_la_CC = @CC@
libmmi_la_CPPFLAGS = -O2 -g -Wno-all -Wno-error -Wno-unused-parameter -I. -I$(WORKSPACE)/system/core/include $(XML2_CFLAGS) -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS)
libmmi_la_LDFLAGS = libcutils libdiag libft2 libutils libc libc++ $(DIAG_LIBS) $(XML2_LIBS) -avoid-version
# source to compile
libmmi_la_SOURCES = util_comm.cpp \
              util_string.cpp \
              util_system.cpp \
              textview.cpp \
              button.cpp \
              listview.cpp \
              layout.cpp \
              nv.cpp \
              events.cpp \
              lang.cpp \
              xmlparse.cpp \
              util_ui.cpp

if USE_GLIB
libmmi_la_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
libmmi_la_LDFLAGS += $(GLIB_LIBS)
endif

if NO_UI
libmmi_la_CPPFLAGS +=  -DNO_UI
endif

if ANDROID
libmmi_la_CPPFLAGS +=  -DANDROID
endif

if USE_LIBMINIUI
libmmi_la_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
#library_include_HEADERS =
