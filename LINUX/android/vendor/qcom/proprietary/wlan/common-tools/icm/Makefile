#
# Makes icm apps
#

ALL+= icm
all: ${ALL}

icm: icm.c icm_socket.c icm_spectral.c icm_test.c icm_scan.c icm_utils.c icm_selector.c icm_cmds.c icm_wal.c icm_wal_ioctl.c icm_wal_rtrcfg.c icm_wal_mblcfg.c driver/events.c driver/driver_nl80211.c
	${CC} -o icm ${CFLAGS} ${LDFLAGS} -lpthread icm.c icm_test.c icm_scan.c icm_socket.c icm_spectral.c icm_utils.c icm_selector.c  icm_cmds.c icm_wal.c icm_wal_ioctl.c icm_wal_rtrcfg.c icm_wal_mblcfg.c driver/events.c driver/driver_nl80211.c $(STAGING_DIR)/usr/lib/libath_classifier.a -lm

clean:
	@rm -rf icm
