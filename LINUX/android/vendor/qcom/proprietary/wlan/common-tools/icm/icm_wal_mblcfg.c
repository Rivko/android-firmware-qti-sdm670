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
 *       Filename:  icm_wal_mblcfg.c
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

#define _LINUX_TYPES_H

#include "icm.h"
#include "icm_wal.h"
#include "icm_api.h"
#include "icm_internal.h"
#include "driver_nl80211.h"

#ifndef ATH_DEFAULT
#define ATH_DEFAULT "wifi0"
#endif


/*
 * Function     : icm_wal_mblcfg_do_80211_scan
 * Description  : do an 802.11 scan and print scan results
 * Input params : pointer to icm
 * Return       : 0 on success, -1 on general errors, -2 on scan cancellation
 */

int icm_wal_mblcfg_do_80211_scan(ICM_INFO_T * picm)
{
    int ret = 0, i;
    struct nl80211_scan_config scan_params;

    /* The below locking doesn't matter for standalone mode */
    pthread_mutex_lock(&picm->scanner_thread_mutex);
    if (picm->is_80211scan_cancel_requested == TRUE) {
        pthread_mutex_unlock(&picm->scanner_thread_mutex);
        fprintf(stderr, "%-8.16s  Scan cancelled\n\n", picm->dev_ifname);
        picm->substate = ICM_STATE_INVALID;
        return -2;
    }

    pthread_mutex_unlock(&picm->scanner_thread_mutex);

    scan_params.chan_info_flag = 0;
    scan_params.scan_ies = NULL;
    scan_params.scan_ies_len = 0;

    /* get freqs from channel list from capability info/structure */
    scan_params.freqs = zalloc(sizeof(int) * picm->chlist.count + 1);
    if (scan_params.freqs == NULL) {
        icm_printf("ICM: Failed to allocate memory\n");
        return -1;
    }

    for (i = 0; i < picm->chlist.count; i++) {
        scan_params.freqs[i] = picm->chlist.ch[i].freq;
    }
    scan_params.freqs[i] = 0; /* freqs array is expected to be zero terminated */

    ret = driver_nl80211_vendor_scan(picm, &scan_params);
    if (ret) {
        err("icm: failed to start vendor scan");
        return ret;
    }
    // Set state after sending vendor_scan
    picm->substate = ICM_STATE_CHANNEL_SCAN;

    return 0;
}

/*
 * Function     : icm_wal_mblcfg_set_width_and_channel
 * Description  : set width and channel as per best channel
 *                selection done previously.
 *                It is the caller's responsibility to ensure
 *                that the best channel selection has already been
 *                carried out, or if this has not been done, then
 *                a default channel has been set instead.
 *                It is the best channel selection code's
 *                responsibility to ensure that the width and channel
 *                are correct.
 * Input params : pointer to icm info, device name
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_mblcfg_set_width_and_channel(ICM_INFO_T *picm, char *dev_ifname)
{
    struct nl80211_channel_config chan_config;
    struct nl80211_chan_args *chan_list;
    ICM_CH_BW_T channel_width;
    int channel = 0;
    int ch_index = 0;
    int sec_channel = 0;
    int cfreq1_channel = 0;
    int cfreq2_channel = 0;
    char modestr[24] = {'\0'};
    ICM_DEV_INFO_T* pdev = get_pdev();
    int i = 0;

    channel_width = picm->channel_width;
    if (picm->ch_selection_mode == ICM_CH_SELECTION_MODE_MANUAL) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_MAIN,  "In manual channel selection mode; using default channel\n");
        channel = picm->def_channel;
    } else if (picm->best_channel < 0) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_MAIN,  "Best channel not set/invalid. Considering "
                "default channel instead.\n");

        if (picm->def_channel == 0) {
            err("Both best channel and default channel are not set/invalid");
            return FAILURE;
        }

        channel = picm->def_channel;
    } else {
        channel = picm->best_channel;

        /* We are operating in auto mode, so we need to use the resolved
           channel width. Without doing so, we won't properly set the
           mode in the case we have to resort to 20 MHz fallback
           or we selected plus/minus from the generic 40 MHz mode. */
        channel_width = picm->selected_channel_width;
    }

    if (channel == 0)
        goto send;

    /* get channel index in picm->chlist.ch.. channel should be non-zero. */
    for (ch_index = 0; ch_index < picm->chlist.count; ch_index++)
        if (picm->chlist.ch[ch_index].channel == channel)
            break;

    if (ch_index == picm->chlist.count) {
        /* This is highly unlikely */
        err("selected channel %d is not present in supproted channel list", channel);
        channel = 0;
        goto send;
    }

    if (channel_width >= ICM_CH_BW_INVALID) {
        err("Invalid channel width enumeration %u", channel_width);
        channel = 0;
        goto send;
    }

    if (channel_width > ICM_CH_BW_40) {
        /* figure out cfreq1_channel value from ic_vhtop_ch_freq_seg1 of supported channel list. */
        cfreq1_channel = picm->chlist.ch[ch_index].ic_vhtop_ch_freq_seg1;
    }

    if (channel_width == ICM_CH_BW_80_PLUS_80) {
        /* Add default channel processing for cfreq2 */
        cfreq2_channel = picm->best_cfreq2_channel;
    }

    /* Derive secondary channel */
    if (channel_width == ICM_CH_BW_40MINUS) {
        sec_channel = channel - 4; // Secondary for 40- is (primary - 20 Mhz)
    } else if (channel_width == ICM_CH_BW_40PLUS) {
        sec_channel = channel + 4; // Secondary for 40+ is (primary + 20 Mhz)
    } else if (channel_width == ICM_CH_BW_80 || channel_width == ICM_CH_BW_160) {
        /* This will be treated as offset in 80 or 160 case and is mandatory
         * As per hostapd logic, this field should exist.
         * Note: this is not being used in calculation of seg0/seg1. */
        if (picm->chlist.ch[ch_index].ic_flags & IEEE80211_CHAN_VHT40PLUS) {
            sec_channel = channel + 4;
        } else if (picm->chlist.ch[ch_index].ic_flags & IEEE80211_CHAN_VHT40MINUS) {
            sec_channel = channel - 4;
        }
    }

    /* Set mode */
    if (icm_compose_phymode_str(picm->phy_spec,
                channel_width,
                modestr,
                sizeof(modestr)) == NULL) {
        channel = 0;
        goto send;
    }

send:
    chan_config.ifname = picm->dev_ifname;
    chan_config.reselect_reason = picm->reselect_reason;
    chan_config.num_channel = 1; /* TODO send entire list [Future task] */

    chan_list = zalloc(sizeof(*chan_list) * chan_config.num_channel);
    if (chan_list == NULL) {
        icm_printf("ICM: Failed to allocate memory\n");
        return -1;
    }

    for (i = 0; i < chan_config.num_channel; i++) {
        if (channel <= 14 )
            chan_list[i].band = NL80211_BAND_2GHZ;
        else
            chan_list[i].band = NL80211_BAND_5GHZ;

        chan_list[i].pri_ch = channel; /* use sort_chan_list instead */
        chan_list[i].ht_sec_ch = sec_channel;
#ifdef ICM_RTR_DRIVER
        chan_list[i].channel_width = channel_width;
#else
        chan_list[i].channel_width = convert_RTR_to_mbl_chan_width(channel_width);
#endif /* ICM_RTR_DRIVER */
        chan_list[i].vht_seg0_center_ch = cfreq1_channel;
        chan_list[i].vht_seg1_center_ch = cfreq2_channel;
    }
    chan_config.channel_list = chan_list;

    if (channel == 0)
        icm_printf("ICM: No Usable channel found. Setting 0 to indicate channel selection failed/completed");

    if (icm_wal_set_channel(picm, &chan_config)){
        err("Error in setting channel; ");
        return FAILURE;
    }

    if (channel == 0) {
        icm_printf("set_channel success with channel %d", channel);
        return FAILURE;
    }

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_MAIN,  "Successfully set channel %d mode %s\n",
            channel,
            modestr);

    if (channel_width == ICM_CH_BW_80_PLUS_80) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_MAIN,  "Successfully set secondary 80 MHz channel index %d mode %s\n",
                cfreq2_channel,
                modestr);
    }

    return SUCCESS;
}


/*
 * Function     : icm_wal_mblcfg_cancel_80211_scan
 * Description  : Cancel all 802.11 scans for the given icm
 * Input params : pointer to icm
 * Return       : success/failure
 */
int icm_wal_mblcfg_cancel_80211_scan(ICM_INFO_T * picm)
{
    //Print if error is returned
    return driver_nl80211_abort_scan(picm);
}

/*
 * Function     : icm_wal_mblcfg_set_channel
 * Description  : Set channel to driver
 * Input params : pointer to icm
 * Return       : success/failure
 */
int icm_wal_mblcfg_set_channel(ICM_INFO_T * picm, struct nl80211_channel_config *chan_config)
{
    return driver_nl80211_set_channel(picm, chan_config);
}

/*
 * Function     : icm_wal_mblcfg_set_channel
 * Description  : Set channel to driver
 * Input params : pointer to icm
 * Return       : success/failure
 */
int icm_wal_mblcfg_get_currdomain(ICM_INFO_T * picm)
{
    char country[4];

    if (driver_nl80211_get_country(picm, country) < 0) {
        icm_printf("Error in getting country code");
        return -1;
    }

    return 0;
}

/*
 * Function     : icm_wal_mblcfg_get_reg_domain
 * Description  : Get reg domain
 * Input params : pointer to icm
 * Return       : success/failure
 */
int icm_wal_mblcfg_get_reg_domain(ICM_INFO_T * picm)
{
    enum nl80211_dfs_regions dfs_domain;

    if (driver_nl80211_get_reg_domain(picm, &dfs_domain) < 0) {
        icm_printf("Error in getting country code");
        return FAILURE;
    }

    picm->dfs_domain = dfs_domain;
    return SUCCESS;
}

/*
 * Function     : icm_wal_cfg_init_channel_params
 * Description  : init channel related params
 *                Initialize base usability for all channels
 * Input params : pointer to icm
 * Return       : void
 *
 */
void icm_wal_cfg_init_channel_params(ICM_INFO_T* picm)
{
    int i = 0;

    for (i = 0; i < MAX_NUM_CHANNEL; i++) {
        ICM_CLEAR_CHANNEL_EXCLUDE(picm, i);
        ICM_SET_CHANNEL_BLUSABILITY(picm, i, MAX_USABILITY);
    }
}
