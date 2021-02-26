
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = mmi
mmi_LDADD := -lpthread -lcutils $(XML2_LIBS) ../libmmi/libmmi.la $(DIAG_LIBS)
mmi_CPPFLAGS = -O2
mmi_CPPFLAGS += -g
mmi_CPPFLAGS += -Wno-all
mmi_CPPFLAGS += -Wno-error
mmi_CPPFLAGS += -Wno-unused-parameter
mmi_CPPFLAGS += -I .
mmi_CPPFLAGS += -I../libmmi
mmi_CPPFLAGS += -I$(WORKSPACE)/system/core/include
mmi_CPPFLAGS += $(XML2_CFLAGS)
mmi_CPPFLAGS +=  -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS)
if USE_GLIB
mmi_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
mmi_LDADD += $(GLIB_LIBS)
endif

if NO_UI
mmi_CPPFLAGS +=  -DNO_UI
endif
if ANDROID
mmi_CPPFLAGS +=  -DANDROID
endif
if USE_LIBMINIUI
mmi_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
# source to compile
mmi_SOURCES = mmi.cpp \
              mmi_handle.cpp \
              module.cpp \
              config.cpp \
              func_map.cpp \
              controller.cpp \
              draw.cpp \
              mmi_cfg.cpp

bin_PROGRAMS += mmi_agent
mmi_agent_LDADD := -ldl -lcutils ../libmmi/libmmi.la
mmi_agent_CPPFLAGS = -O2
mmi_agent_CPPFLAGS += -g
mmi_agent_CPPFLAGS += -Wno-all
mmi_agent_CPPFLAGS += -Wno-error
mmi_agent_CPPFLAGS += -Wno-unused-parameter
mmi_agent_CPPFLAGS += -I .
mmi_agent_CPPFLAGS += -I../libmmi
mmi_agent_CPPFLAGS += -I$(WORKSPACE)/system/core/include
mmi_agent_CPPFLAGS +=  -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS)
if USE_GLIB
mmi_agent_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
mmi_agent_LDADD += $(GLIB_LIBS)
endif

if NO_UI
mmi_agent_CPPFLAGS +=  -DNO_UI
endif
if ANDROID
mmi_agent_CPPFLAGS +=  -DANDROID
endif
if USE_LIBMINIUI
mmi_agent_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
# source to compile
mmi_agent_SOURCES = agent_main.cpp \
              agent_handle.cpp

bin_PROGRAMS += mmi_diag
mmi_diag_LDADD := -lcutils ../libmmi/libmmi.la $(DIAG_LIBS)
mmi_diag_CPPFLAGS = -O2
mmi_diag_CPPFLAGS += -g
mmi_diag_CPPFLAGS += -Wno-all
mmi_diag_CPPFLAGS += -Wno-error
mmi_diag_CPPFLAGS += -Wno-unused-parameter
mmi_diag_CPPFLAGS += -I .
mmi_diag_CPPFLAGS += -I../libmmi
mmi_diag_CPPFLAGS += -I$(WORKSPACE)/system/core/include
mmi_diag_CPPFLAGS +=  -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS)
if USE_GLIB
mmi_diag_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
mmi_diag_LDADD += $(GLIB_LIBS)
endif

if NO_UI
mmi_diag_CPPFLAGS +=  -DNO_UI
endif
if ANDROID
mmi_diag_CPPFLAGS +=  -DANDROID
endif
if USE_LIBMINIUI
mmi_diag_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
# source to compile
mmi_diag_SOURCES = diag_main.cpp \
              diag_handle.cpp

bin_PROGRAMS += mmi_debug
mmi_debug_LDADD := -ldl -lcutils ../libmmi/libmmi.la
mmi_debug_CPPFLAGS = -O2
mmi_debug_CPPFLAGS += -g
mmi_debug_CPPFLAGS += -Wno-all
mmi_debug_CPPFLAGS += -Wno-error
mmi_debug_CPPFLAGS += -Wno-unused-parameter
mmi_debug_CPPFLAGS += -I .
mmi_debug_CPPFLAGS += -I../libmmi
mmi_debug_CPPFLAGS += -I$(WORKSPACE)/system/core/include
mmi_debug_CPPFLAGS +=  -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS)
if USE_GLIB
mmi_debug_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
mmi_debug_LDADD += $(GLIB_LIBS)
endif

if NO_UI
mmi_debug_CPPFLAGS +=  -DNO_UI
endif
if ANDROID
mmi_debug_CPPFLAGS +=  -DANDROID
endif
if USE_LIBMINIUI
mmi_debug_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
# source to compile
mmi_debug_SOURCES = debug_main.cpp

