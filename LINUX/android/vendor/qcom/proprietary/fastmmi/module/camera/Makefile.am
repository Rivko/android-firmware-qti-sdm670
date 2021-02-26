
ACLOCAL_AMFLAGS = -I m4

brary_includedir = $(pkgincludedir)
lib_LTLIBRARIES = libmmi_camera.la
libmmi_camera_la_CC = @CC@
libmmi_camera_la_CPPFLAGS = -O2 -g -Wno-all -Wno-error -Wno-unused-parameter -I. -I$(WORKSPACE)/system/core/include $(XML2_CFLAGS) -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS) -I../../libmmi \
			    -I$(WORKSPACE)/hardware/qcom/camera/libcamera/inc
libmmi_camera_la_LDFLAGS = -lcutils ../../libmmi/libmmi.la -avoid-version -lcamera -lcamparams
# source to compile
libmmi_camera_la_SOURCES = camera_uav.cpp

if USE_GLIB
libmmi_camera_la_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
libmmi_camera_la_LDFLAGS += $(GLIB_LIBS)
endif

if NO_UI
libmmi_camera_la_CPPFLAGS +=  -DNO_UI
endif

if ANDROID
libmmi_camera_la_CPPFLAGS +=  -DANDROID
endif

if USE_LIBMINIUI
libmmi_camera_la_CPPFLAGS +=  -DUSE_LIBMINIUI
endif

