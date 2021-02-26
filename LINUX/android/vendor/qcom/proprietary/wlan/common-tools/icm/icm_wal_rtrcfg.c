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
 *       Filename:  icm_wal_rtrcfg.c
 *
 *    Description:  ICM WAL IOCTL related changes
 *
 *        Version:  1.0
 *        Created:  04/19/2012 01:18:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */

#ifdef ICM_RTR_DRIVER_CFG_SUPPORT

#include <stdio.h>
#include <icm.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include "if_athioctl.h"
#define _LINUX_TYPES_H

#include "icm.h"
#include "icm_wal.h"
#include "spectral_ioctl.h"
#include "spectral_data.h"
#include "spec_msg_proto.h"
#include "ath_classifier.h"
#include "spectral_ioctl.h"
#include "spectral.h"
#include "ieee80211_external.h"
#include "icm_api.h"
#include "icm_wal.h"
#include "spectral_data.h"

#ifndef ATH_DEFAULT
#define ATH_DEFAULT "wifi0"
#endif

/*
 * Function     : icm_wal_rtrcfg_is_spectral_enab
 * Description  : check is spectral is enabled
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_rtrcfg_is_spectral_enab(ICM_INFO_T* picm)
{
    u_int32_t status =  SUCCESS;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;

    return status;
}

/*
 * Function     : icm_wal_rtrcfg_get_spectral_params
 * Description  : Get values of Spectral parameters
 * Input params : pointer to icm info structure
 * Output params: pointer to Spectral params structure, to be populated
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_rtrcfg_get_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp)
{
    ICM_DEV_INFO_T *pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);
    ICM_SPECTRAL_INFO_T *psinfo = NULL;

    if (picm == NULL) {
        err("icm: ICM Information structure is invalid");
        return FAILURE;
    }

    if (sp == NULL) {
        err("icm: Spectral Parameters structure is invalid");
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : icm_wal_rtrcfg_set_spectral_params
 * Description  : Set values of Spectral parameters
 * Input params : pointer to icm info structure, pointer to Spectral params
 *                structure containing values to be set
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_rtrcfg_set_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);
    ICM_SPECTRAL_INFO_T *psinfo = NULL;

    if (picm == NULL) {
        err("icm: ICM Information structure is invalid");
        return FAILURE;
    }

    if (sp == NULL) {
        err("icm: Spectral Parameters structure is invalid");
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : icm_wal_start_spectral_scan
 * Description  : start the spectrla scan on current channel
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_rtrcfg_start_spectral_scan(ICM_INFO_T* picm)
{
    u_int32_t status = SUCCESS;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;

    picm->substate = ICM_STATE_SPECTRAL_SCAN;

    return status;
}

/*
 * Function     : icm_wal_rtrcfg_set_spectral_debug
 * Description  : set the spectral module debug level
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_rtrcfg_set_spectral_debug(ICM_INFO_T* picm, int dbglevel)
{
    u_int32_t status = SUCCESS;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */

    return status;
}


/*
 * Function     : icm_wal_rtrcfg_stop_spectral_scan
 * Description  : stop the spectrla scan on current channel
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_rtrcfg_stop_spectral_scan(ICM_INFO_T* picm)
{
    u_int32_t status = SUCCESS;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */

    return status;
}

/*
 * Function     : icm_wal_get_channel_loading
 * Description  : get the channel loading information
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_rtrcfg_get_channel_loading(ICM_INFO_T* picm)
{

    u_int32_t status =  SUCCESS;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    return status;
}
/*
 * Function     : icm_wal_rtrcfg_clear_spectral_chan_properties
 * Description  : clean the spectral channel properties
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */

int icm_wal_rtrcfg_clear_spectral_chan_properties(ICM_INFO_T* picm)
{
    u_int32_t status = SUCCESS;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */

    return status;


}

/*
 * Function     : icm_wal_rtrcfg_set_icm_active
 * Description  : set the spectral module debug level
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_rtrcfg_set_icm_active(ICM_INFO_T *picm, u_int32_t val)
{
    u_int32_t status = SUCCESS;

    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */


    return status;

}

/*
 * Function     : icm_wal_rtrcfg_get_nominal_noisefloor
 * Description  : get nominal noisefloor
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_rtrcfg_get_nominal_noisefloor(ICM_INFO_T *picm)
{

    int nominal_nf =  ATH_DEFAULT_NOMINAL_NF;
    struct ifreq ifr;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);


    return nominal_nf;

}
/*
 * Function     : icm_wal_rtrcfg_do_80211_scan
 * Description  : do an 802.11 scan and print scan results
 * Input params : pointer to icm
 * Return       : 0 on success, -1 on general errors, -2 on scan cancellation
 */

int icm_wal_rtrcfg_do_80211_scan(ICM_INFO_T * picm)
{
    struct iw_scan_req scanopt; /* options for set */
    int scanflags = 0;              /* options for scan */
    unsigned char *buffer = NULL;   /* results */
    int buflen = IW_SCAN_MAX_DATA;
    struct iw_range range;      /* range */
    int has_range = 0;
    struct timeval tv;          /* range */
    int timeout = ICM_DEFAULT_SCAN_TIMEOUT;     /* 30 seconds */
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);
    int is_11ac = 0;

    /* Get range info if we can */
    if (icm_get_paramrange(picm, &range) == SUCCESS) {
        has_range = 1;
    }

    if ((is_11ac = is_11ac_offload(picm)) < 0) {
        fprintf(stderr, "%-8.16s Unable to determine if radio is 11ac "
                "offload based.\n\n", picm->radio_ifname);
        return (-1);
    }

    /* XXX: Migrate prints to new debug framework. Give lowest priority to
       below. */
    if (is_11ac) {
        icm_printf("icm : Scan interface %s is 11ac offload based.\n",
                picm->dev_ifname);
    } else {
        icm_printf("icm : Scan interface %s is not 11ac offload based.\n",
                picm->dev_ifname);
    }

    if ((!has_range) || (range.we_version_compiled <= 15)) {
        fprintf(stderr, "%-8.16s Cannot scan with the interface.\n\n", picm->dev_ifname);
        return (-1);
    }

    memset(picm->slist.elem, 0, sizeof(picm->slist.elem));

    picm->substate = ICM_STATE_CHANNEL_SCAN;

    return 0;
}
/*
 * Function     : icm_wal_rtrcfg_cancel_80211_scan
 * Description  : Cancel all 802.11 scans for the given icm
 * Input params : pointer to icm
 * Return       : success/failure
 */
int icm_wal_rtrcfg_cancel_80211_scan(ICM_INFO_T * picm)
{
    struct iwreq iwr;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    memset(&iwr, 0, sizeof(iwr));

    return SUCCESS;
}
/*
 * Function     : icm_wal_rtrcfg_do_80211_priv
 * Description  : interface to 80211 priv ioctl
 * Input params : pointer to icm, channel number, pointer to
 *                iwreq, interface name
 * Return       : success/failure
 */

int icm_wal_rtrcfg_do_80211_priv(ICM_INFO_T *picm, struct iwreq *iwr, const char *ifname, int op, void *data, size_t len)
{
#define N(a)    (sizeof(a)/sizeof(a[0]))

    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

}
/*
 * Function     : icm_wal_rtrcfg_get_currdomain
 * Description  : Obtain the current RF regulatory domain being used by
 *                the radio and place it in the rfreg_domain of the
 *                ICM_INFO_T object.
 * Input params : pointer to icm
 * Return       : 0 on success, -1 on failure
 */
    static int
icm_wal_rtrcfg_get_currdomain(ICM_INFO_T* picm)
{
    struct iwreq wrq;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    if (picm == NULL) {
        return -1;
    }

    return 0;
}
/*
 * Function     : icm_wal_rtrcfg_get_ieee_chaninfo
 * Description  : prints ieee related channel information
 * Input params : pointer to ieee80211_ath_channel
 * Return       : success/failure
 */
int icm_wal_rtrcfg_get_ieee_chaninfo(ICM_INFO_T* picm)
{
    struct ieee80211req_chaninfo chans, chans_160;
    struct ieee80211req_chaninfo achans, achans_160;
    const struct ieee80211_ath_channel *c;
    const struct ieee80211_ath_channel *c_160;
    struct ieee80211_wlanconfig *config;
    struct iwreq iwr;
    const char* ifname = picm->dev_ifname;
    int allchans = 1;
    int i;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);


    return SUCCESS;
}
/*
 * Function     : icm_wal_rtrcfg_get_paramrange
 * Description  : gets parameter range information
 * Input params : pointer to icm
 * Output params: pointer to iw_range structure
 * Return       : success/failure
 */
int icm_wal_rtrcfg_get_paramrange(ICM_INFO_T *picm, struct iw_range *range)
{
    /* Note: We are sure that drivers intended to work with ICM are
       compiled with WIRELESS_EXT > 15
       This code is NOT meant to work with obsolete driver versions.

       XXX - If support for drivers with WIRELESS_EXT <= 15 is ever
       required for some reason, make required modifications below.
     */

    ICM_DEV_INFO_T  *pdev = get_pdev();
    ICM_NLSOCK_T    *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);
    struct iwreq    wrq;
    char            buf[sizeof(struct iw_range)];
    struct iw_range *temp_range;


    if (picm == NULL || range == NULL) {
        return FAILURE;
    }


    return SUCCESS;
}
/*
 * Function     : icm_wal_rtrcfg_is_dev_up
 * Description  : find whether a device is up
 * Input params : pointer to ICM_DEV_INFO_T, device name
 * Out params   : pointer to bool indicating whether device is
 up (valid only on SUCCESS)
 * Return       : SUCCESS/FAILURE
 */
static int icm_wal_rtrcfg_is_dev_up(ICM_DEV_INFO_T* pdev,
        char *dev_ifname,
        bool *isup)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);
    struct ifreq ifr;

    *isup = FALSE;


    return SUCCESS;
}
/*
 * Function     : icm_wal_rtrcfg_is_dev_ap
 * Description  : find whether a device is in AP mode
 * Input params : pointer to ICM_DEV_INFO_T, device name
 * Out params   : pointer to bool indicating whether device is
 in AP mode (valid only on SUCCESS)
 * Return       : SUCCESS/FAILURE
 */
static int icm_wal_rtrcfg_is_dev_ap(ICM_DEV_INFO_T* pdev,
        char *dev_ifname,
        bool *isap)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    return SUCCESS;
}
/*
 * Function     : icm_wal_rtrcfg_get_iface_addr
 * Description  : get the interface address
 * Input params : pointer to ICM_DEV_INFO_T, interface name,
 * Out params   : interface address.
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_rtrcfg_get_iface_addr(ICM_DEV_INFO_T* pdev, char* ifname, u_int8_t *ifaddr)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    return SUCCESS;

}
/*
 * Function     : icm_wal_rtrcfg_get_radio_priv_int_param
 * Description  : Get a radio-private integer parameter
 * Input params : pointer to pdev info, radio interface name, required parameter
 * Return       : On success: Value of parameter
 *                On error  : -1
 */
int icm_wal_rtrcfg_get_radio_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname, int param)
{
    struct iwreq iwr;
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    return success;
}
/*
 * Function     : icm_wal_rtrcfg_get_vap_priv_int_param
 * Description  : Return private parameter of the given VAP from driver.
 * Input params : const char pointer pointing to interface name and required parameter
 * Return       : Success: value of the private param
 *                Failure: -1
 *
 */
int icm_wal_rtrcfg_get_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param)
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    return SUCCESS;
    }
/*
 * Function     : icm_wal_ioclt_set_vap_priv_int_param
 * Description  : Set a device-private integer parameter
 * Input params : pointer to pdev info, device interface name, parameter,
 *                value.
 * Return       : On success: 0
 *                On error  : -1
 */
int icm_wal_rtrcfg_set_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int32_t val)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    return 0;
}
/*
 * Function     : icm_wal_rtrcfg_get_channel_width
 * Description  : Get current channel width from driver
 * Input params : pointer to icm info structure
 * Return       : Channel width on success
 *                IEEE80211_CWM_WIDTHINVALID on failure
 */
enum ieee80211_cwm_width icm_wal_rtrcfg_get_channel_width(ICM_INFO_T* picm)
{
    enum ieee80211_cwm_width ch_width = 0;
    struct ifreq ifr;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);
    ICM_SPECTRAL_INFO_T *psinfo = NULL;

    if (picm == NULL) {
        err("icm: ICM Information structure is invalid");
        return IEEE80211_CWM_WIDTHINVALID;
    }

    return ch_width;
}

#endif /* ICM_RTR_DRIVER */
