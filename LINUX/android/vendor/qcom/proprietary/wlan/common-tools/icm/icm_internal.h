/*
 * Copyright (c) 2012,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2012 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * =====================================================================================
 *
 *       Filename:  icm_internal.h
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


#ifndef __ICM_INTERNEL_H__
#define __ICM_INTERNEL_H__

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

#include "wireless_copy.h"

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

/*
 * Scan state and meta-information, used to decode events...
 */
typedef struct iwscan_state {
    /* State */
    int ap_num;                 /* Access Point number 1->N */
    int val_index;              /* Value in table 0->(N-1) */
} iwscan_state;
/*
 * This value does not appear to be defined anywhere that would make sense
 * to include here. Even wlan_api.h defines it like this, so for now, that
 * is done here too.
 */
enum {
    SPECIAL_PARAM_COUNTRY_ID
};

/*
 * Meta-data about standard Wireless Extension ioctl results.
 * This is based upon standard Wireless Extensions definitions.
 */
static const ICM_EVENT_DESCRIPTOR_T we_ioc_descriptors[] = {
    [SIOCSIWCOMMIT - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_NULL,
        .ev_len         = IW_EV_LCP_PK_LEN,
    },

    [SIOCGIWNAME - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_CHAR,
        .ev_len         = IW_EV_CHAR_PK_LEN,
    },

    [SIOCSIWNWID - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWNWID - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWFREQ - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_FREQ,
        .ev_len         = IW_EV_FREQ_PK_LEN,
    },

    [SIOCGIWFREQ - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_FREQ,
        .ev_len         = IW_EV_FREQ_PK_LEN,
    },

    [SIOCSIWMODE - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_UINT,
        .ev_len         = IW_EV_UINT_PK_LEN,
    },

    [SIOCGIWMODE - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_UINT,
        .ev_len         = IW_EV_UINT_PK_LEN,
    },

    [SIOCSIWSENS - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWSENS - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWRANGE - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_NULL,
        .ev_len         = IW_EV_LCP_PK_LEN,
    },

    [SIOCGIWRANGE - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_POINT,
        .ev_len         = IW_EV_POINT_PK_LEN,
        .ev_unit_size   = 1,
        .ev_max_units   = sizeof(struct iw_range),
    },

    [SIOCSIWPRIV - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_NULL,
        .ev_len         = IW_EV_LCP_PK_LEN,
    },

    [SIOCGIWPRIV - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_NULL,
        .ev_len         = IW_EV_LCP_PK_LEN,
    },

    [SIOCSIWSTATS - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_NULL,
        .ev_len         = IW_EV_LCP_PK_LEN,
    },

    [SIOCGIWSTATS - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_NULL,
        .ev_len         = IW_EV_LCP_PK_LEN,
    },

    [SIOCSIWSPY - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = sizeof(struct sockaddr),
        .ev_max_units    = IW_MAX_SPY,
    },

    [SIOCGIWSPY - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = sizeof(struct sockaddr) +
            sizeof(struct iw_quality),
        .ev_max_units    = IW_MAX_SPY,
    },

    [SIOCSIWTHRSPY - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = sizeof(struct iw_thrspy),
        .ev_min_units    = 1,
        .ev_max_units    = 1,
    },

    [SIOCGIWTHRSPY - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = sizeof(struct iw_thrspy),
        .ev_min_units    = 1,
        .ev_max_units    = 1,
    },

    [SIOCSIWAP - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_ADDR,
        .ev_len         = IW_EV_ADDR_PK_LEN,
    },

    [SIOCGIWAP - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_ADDR,
        .ev_len         = IW_EV_ADDR_PK_LEN,
    },

    [SIOCSIWMLME - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_min_units    = sizeof(struct iw_mlme),
        .ev_max_units    = sizeof(struct iw_mlme),
    },

    [SIOCGIWAPLIST - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = sizeof(struct sockaddr) +
            sizeof(struct iw_quality),
        .ev_max_units    = IW_MAX_AP,
        .ev_is_nolimit   = 1,
    },

    [SIOCSIWSCAN - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_min_units    = 0,
        .ev_max_units    = sizeof(struct iw_scan_req),
    },

    [SIOCGIWSCAN - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_SCAN_MAX_DATA,
        .ev_is_nolimit   = 1,
    },

    [SIOCSIWESSID - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_ESSID_MAX_SIZE + 1,
    },

    [SIOCGIWESSID - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_ESSID_MAX_SIZE + 1,
    },

    [SIOCSIWNICKN - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_ESSID_MAX_SIZE + 1,
    },

    [SIOCGIWNICKN - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_ESSID_MAX_SIZE + 1,
    },

    [SIOCSIWRATE - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWRATE - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWRTS - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWRTS - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWFRAG - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWFRAG - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWTXPOW - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWTXPOW - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWRETRY - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWRETRY - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWENCODE - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_ENCODING_TOKEN_MAX,
    },

    [SIOCGIWENCODE - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_ENCODING_TOKEN_MAX,
    },

    [SIOCSIWPOWER - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWPOWER - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWMODUL - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWMODUL - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWGENIE - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_GENERIC_IE_MAX,
    },

    [SIOCGIWGENIE - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_GENERIC_IE_MAX,
    },

    [SIOCSIWAUTH - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCGIWAUTH - SIOCIWFIRST] = {
        .ev_type        = ICM_EV_TYPE_PARAM,
        .ev_len         = IW_EV_PARAM_PK_LEN,
    },

    [SIOCSIWENCODEEXT - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_min_units    = sizeof(struct iw_encode_ext),
        .ev_max_units    = sizeof(struct iw_encode_ext) +
            IW_ENCODING_TOKEN_MAX,
    },

    [SIOCGIWENCODEEXT - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_min_units    = sizeof(struct iw_encode_ext),
        .ev_max_units    = sizeof(struct iw_encode_ext) +
            IW_ENCODING_TOKEN_MAX,
    },

    [SIOCSIWPMKSA - SIOCIWFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_min_units    = sizeof(struct iw_pmksa),
        .ev_max_units    = sizeof(struct iw_pmksa),
    },
};

/*
 * Meta-data about standard Wireless Extension events.
 * This is based upon standard Wireless Extensions definitions.
 */
static const ICM_EVENT_DESCRIPTOR_T we_ev_descriptors[] = {
    [IWEVTXDROP - IWEVFIRST] = {
        .ev_type        = ICM_EV_TYPE_ADDR,
        .ev_len         = IW_EV_ADDR_PK_LEN,
    },

    [IWEVQUAL - IWEVFIRST] = {
        .ev_type        = ICM_EV_TYPE_QUALITY,
        .ev_len         = IW_EV_QUAL_PK_LEN,
    },

    [IWEVCUSTOM - IWEVFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_CUSTOM_MAX,
    },

    [IWEVREGISTERED - IWEVFIRST] = {
        .ev_type        = ICM_EV_TYPE_ADDR,
        .ev_len         = IW_EV_ADDR_PK_LEN,
    },

    [IWEVEXPIRED - IWEVFIRST] = {
        .ev_type        = ICM_EV_TYPE_ADDR,
        .ev_len         = IW_EV_ADDR_PK_LEN,
    },

    [IWEVGENIE - IWEVFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_GENERIC_IE_MAX,
    },

    [IWEVMICHAELMICFAILURE - IWEVFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = sizeof(struct iw_michaelmicfailure),
    },

    [IWEVASSOCREQIE - IWEVFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_GENERIC_IE_MAX,
    },

    [IWEVASSOCRESPIE - IWEVFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = IW_GENERIC_IE_MAX,
    },

    [IWEVPMKIDCAND - IWEVFIRST] = {
        .ev_type         = ICM_EV_TYPE_POINT,
        .ev_len          = IW_EV_POINT_PK_LEN,
        .ev_unit_size    = 1,
        .ev_max_units    = sizeof(struct iw_pmkid_cand),
    },
};

/* Function definitions */

extern void icm_update_scan_results(ICM_INFO_T* picm,
        struct iw_event *event,
        struct iwscan_state *state,
        struct iw_range *iw_range,
        int has_range);

extern ICM_EVENT_DESCRIPTOR_T const*
icm_get_event_descriptor(int cmd,
        int we_ver,
        ICM_EVENT_DESCRIPTOR_ADJ_T *desc_adj);

extern int
icm_get_currdomain(ICM_INFO_T* picm);

extern int
icm_is_non_ap_channel(ICM_INFO_T* picm, int channel);

extern int
icm_get_80211_priv(ICM_INFO_T* picm, const char *ifname, int op, void *data, size_t len);

extern int icm_enable_11ac_scanchevents(ICM_DEV_INFO_T* pdev,
        ICM_INFO_T * picm,
        int is_11ac_offload);
extern int icm_disable_11ac_scanchevents(ICM_DEV_INFO_T* pdev,
        ICM_INFO_T * picm,
        int is_11ac_offload);
extern int icm_get_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm);
extern int icm_set_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm,
        int scanband);
extern int icm_configure_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm);
extern int icm_restore_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm);

extern int icm_enable_sending_additional_ies(ICM_DEV_INFO_T *pdev, ICM_INFO_T *picm);
extern int icm_disable_sending_additional_ies(ICM_DEV_INFO_T *pdev, ICM_INFO_T *picm);

extern int icm_init_seldbg_decinfo(ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize,
        ICM_BAND_T band,
        ICM_PHY_SPEC_T physpec,
        ICM_CH_BW_T width);

extern int icm_seldbg_process(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize);

extern int icm_seldbg_filedump(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize);

extern int icm_seldbg_get_rel_maxstrlen(ICM_DEV_INFO_T* pdev);

extern int icm_seldbg_get_fstatus_maxstrlen(ICM_DEV_INFO_T* pdev);

extern void icm_seldbg_printlegend(ICM_DEV_INFO_T* pdev);

extern int icm_seldbg_consoledump(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize);

extern int icm_best_cfreq2_selection_helper(ICM_INFO_T* picm,
        int pri20_chan_80_80);

extern int icm_init_cfreq2_seldbg_decinfo(ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize, ICM_PHY_SPEC_T physpec);

extern int icm_cfreq2_seldbg_process(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize,
        ICM_PHY_SPEC_T physpec);

extern int icm_cfreq2_seldbg_consoledump(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize,
        ICM_PHY_SPEC_T physpec);
void icm_parse_gen_ie(ICM_INFO_T *picm, struct iwscan_state *state, unsigned char* buffer, int buflen);
#endif  /* __ICM_INTERNEL_H__*/
