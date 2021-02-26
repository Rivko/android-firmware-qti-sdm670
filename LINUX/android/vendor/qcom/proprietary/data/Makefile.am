# Makefile.am - Automake script for data
#
ACLOCAL_AMFLAGS = -I m4

dirs :

dirs = netmgr/src
dirs += qdi/src
dirs += dsi_netctrl/src
dirs += tests/dsi_netctrl/src
dirs += qdp/src
dirs += radish
dirs += ipacm-diag/src
dirs += mobileap/ipc
dirs += mobileap/server/src mobileap/client/src qti/src
dirs += mobileap/webserver/qcmap_web_client/src mobileap/webserver/cgi
dirs += mobileap/eMBMS_multiclient/qmi_ip_multiclient/src
dirs += mobileap/eMBMS_multiclient/tunneling/src
dirs += ipacm-perf/src

SUBDIRS = $(dirs)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = data.pc
EXTRA_DIST = $(pkgconfig_DATA)
