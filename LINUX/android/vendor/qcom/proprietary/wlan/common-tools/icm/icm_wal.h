/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2016 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * =====================================================================================
 *
 *       Filename:  icm_wal.h
 *
 *    Description:  Intelligent Channel Manager wireless abstraction layer
 *
 *        Version:  1.0
 *        Created:  04/19/2012 01:17:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */


#ifndef __ICM_WAL_H__
#define __ICM_WAL_H__

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include "wireless_copy.h"

#ifdef ICM_RTR_DRIVER
#include "spectral_data.h"
#include "classifier.h"
#include "spec_msg_proto.h"
#include "if_athioctl.h"
#include "a_types.h"
#include "cdp_txrx_stats_struct.h"
#include "spectral.h"
#include "ieee80211_external.h"
#endif /* ICM_RTR_DRIVER */

/*
 * Linux uses __BIG_ENDIAN and __LITTLE_ENDIAN while BSD uses _foo and an
 * explicit _BYTE_ORDER. We define things the BSD way since BSD got to this
 * first, and for compliance with rest of the QCA framework.
 */
#ifndef _LITTLE_ENDIAN
#define _LITTLE_ENDIAN 1234 /* LSB first */
#endif
#ifndef _BIG_ENDIAN
#define _BIG_ENDIAN 4321 /* MSB first */
#endif
#include <asm/byteorder.h>
#if defined(__LITTLE_ENDIAN)
#define _BYTE_ORDER _LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN)
#define _BYTE_ORDER _BIG_ENDIAN
#else
#error "Please fix asm/byteorder.h"
#endif

#include "icm.h"

/*WIN IOCTL wireless abstraction APIs */

#ifdef ICM_RTR_DRIVER
int icm_wal_ioctl_is_spectral_enab(ICM_INFO_T* picm);
int icm_wal_ioctl_get_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp);
int icm_wal_ioctl_set_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp);
int icm_wal_ioctl_start_spectral_scan(ICM_INFO_T* picm);
int icm_wal_ioctl_stop_spectral_scan(ICM_INFO_T* picm);
int icm_wal_ioctl_set_spectral_debug(ICM_INFO_T* picm, int dbglevel);
int icm_wal_ioctl_clear_spectral_chan_properties(ICM_INFO_T* picm);
#endif /* ICM_RTR_DRIVER */

int icm_wal_ioctl_get_channel_loading(ICM_INFO_T* picm);
int icm_wal_ioctl_set_icm_active(ICM_INFO_T *picm, u_int32_t val);
int icm_wal_ioctl_get_nominal_noisefloor(ICM_INFO_T *picm);

int icm_wal_ioctl_do_80211_scan(ICM_INFO_T* picm);
int icm_wal_ioctl_cancel_80211_scan(ICM_INFO_T * picm);
int
icm_wal_ioctl_do_80211_priv(ICM_INFO_T *picm, struct iwreq *iwr, const char *ifname, int op, void *data, size_t len);
int icm_wal_ioctl_get_currdomain(ICM_INFO_T* picm);
int icm_wal_ioctl_get_reg_domain(ICM_INFO_T* picm);
int icm_wal_ioctl_get_currchan(ICM_INFO_T* picm);
int icm_wal_ioctl_get_ieee_chaninfo(ICM_INFO_T* picm);
void icm_wal_ioctl_init_channel_params(ICM_INFO_T *picm);
int icm_wal_ioctl_get_paramrange(ICM_INFO_T *picm, struct iw_range *range);

int icm_wal_ioctl_is_dev_up(ICM_DEV_INFO_T* pdev, char *dev_ifname, bool *isup);
int icm_wal_ioctl_is_dev_ap(ICM_DEV_INFO_T* pdev, char *dev_ifname, bool *isap);

int icm_wal_ioctl_get_iface_addr(ICM_DEV_INFO_T* pdev, char* ifname, u_int8_t *ifaddr);
int icm_wal_ioctl_get_radio_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname,
        int param);
int icm_wal_ioctl_get_vap_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname, int param);
int icm_wal_ioctl_set_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int32_t val);
enum ieee80211_cwm_width icm_wal_ioctl_get_channel_width(ICM_INFO_T* picm);
int icm_wal_ioctl_set_width_and_channel(ICM_INFO_T *picm, char *dev_ifname);

/* WIN cfg80211 wireless abstraction APIs*/
#ifdef ICM_RTR_DRIVER
int icm_wal_rtrcfg_is_spectral_enab(ICM_INFO_T* picm);
int icm_wal_rtrcfg_get_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp);
int icm_wal_rtrcfg_set_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp);
int icm_wal_rtrcfg_start_spectral_scan(ICM_INFO_T* picm);
int icm_wal_rtrcfg_stop_spectral_scan(ICM_INFO_T* picm);
int icm_wal_rtrcfg_set_spectral_debug(ICM_INFO_T* picm, int dbglevel);
int icm_wal_rtrcfg_clear_spectral_chan_properties(ICM_INFO_T* picm);
#endif /* ICM_RTR_DRIVER */

int icm_wal_rtrcfg_get_channel_loading(ICM_INFO_T* picm);
int icm_wal_rtrcfg_set_icm_active(ICM_INFO_T *picm, u_int32_t val);
int icm_wal_rtrcfg_get_nominal_noisefloor(ICM_INFO_T *picm);

int icm_wal_rtrcfg_do_80211_scan(ICM_INFO_T* picm);
int icm_wal_rtrcfg_cancel_80211_scan(ICM_INFO_T * picm);
int
icm_wal_rtrcfg_do_80211_priv(ICM_INFO_T *picm, struct iwreq *iwr, const char *ifname, int op, void *data, size_t len);
int icm_wal_rtrcfg_get_currdomain(ICM_INFO_T* picm);
int icm_wal_rtrcfg_get_ieee_chaninfo(ICM_INFO_T* picm);
int icm_wal_rtrcfg_get_paramrange(ICM_INFO_T *picm, struct iw_range *range);

int icm_wal_rtrcfg_is_dev_up(ICM_DEV_INFO_T* pdev, char *dev_ifname, bool *isup);
int icm_wal_rtrcfg_is_dev_ap(ICM_DEV_INFO_T* pdev, char *dev_ifname, bool *isap);

int icm_wal_rtrcfg_get_iface_addr(ICM_DEV_INFO_T* pdev, char* ifname, u_int8_t *ifaddr);
int icm_wal_rtrcfg_get_radio_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname,
        int param);
int icm_wal_rtrcfg_get_vap_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname, int param);
int icm_wal_rtrcfg_set_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int32_t val);
enum ieee80211_cwm_width icm_wal_rtrcfg_get_channel_width(ICM_INFO_T* picm);

/* MBL cfg80211 Wireless abstraction functions */
int icm_wal_mblcfg_do_80211_scan(ICM_INFO_T* picm);
int icm_wal_mblcfg_cancel_80211_scan(ICM_INFO_T * picm);
int icm_wal_mblcfg_get_currdomain(ICM_INFO_T* picm);
int icm_wal_mblcfg_get_reg_domain(ICM_INFO_T* picm);
int icm_wal_mblcfg_set_channel(ICM_INFO_T * picm, struct nl80211_channel_config *chan_config);

int icm_wal_mblcfg_set_width_and_channel(ICM_INFO_T *picm, char *dev_ifname);
void icm_wal_cfg_init_channel_params(ICM_INFO_T *picm);
#endif  /* __ICM_WAL_H__ */
