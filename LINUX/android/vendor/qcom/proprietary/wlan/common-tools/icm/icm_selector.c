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
 *       Filename:  icm_selector.c
 *
 *    Description:  Channel Selection Algorithm
 *
 *        Version:  1.0
 *        Created:  04/24/2012 05:13:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "icm.h"
#include "icm_internal.h"

#ifdef ICM_RTR_DRIVER
#include "spectral_ioctl.h"
#include "spectral_data.h"
#include "spec_msg_proto.h"
#include "ath_classifier.h"
#endif /* ICM_RTR_DRIVER */

#define MIN(a,b) (((a)<(b))?a:b)

#define WEATHER_RADAR_CHANNEL(freq)  (freq >= 5600) && (freq <= 5650)



int icm_get_reg_domain(ICM_INFO_T* picm)
{
    return icm_wal_get_reg_domain(picm);
}

int icm_selector_init(ICM_INFO_T* picm)
{
    int status = SUCCESS;
    ICM_DEV_INFO_T* pdev = get_pdev();

#ifdef ICM_RTR_DRIVER
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Clearing channel properties...\n");
    if ((status = icm_clear_spectral_chan_properties(picm)) != SUCCESS) {
        err("Cannot clear channel characterstics...");
        goto err;
    }
#endif /* ICM_RTR_DRIVER */

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Getting supported channels...\n");
    icm_init_channel_params(picm);
    if ((status = icm_get_supported_channels(picm)) != SUCCESS) {
        err("Cannot get supported channels...");
        goto err;
    }

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Getting DFS regulatory domain ...\n");
    if ((status = icm_get_reg_domain(picm)) != SUCCESS) {
        err("Cannot get DFS Domain");
        goto err;
    } else {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SELECTOR, "DFS regulatory domain is %d\n", picm->dfs_domain);
    }

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Getting IEEE channel properties...\n");
    if (icm_get_ieee_chaninfo(picm) != SUCCESS) {
        return FAILURE;
    }

#ifdef ICM_RTR_DRIVER
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Indicating to driver that external channel selection is in progress...\n");
    if (icm_set_ext_acs_inprogress(pdev, picm) != SUCCESS) {
        return FAILURE;
    }
#endif /* ICM_RTR_DRIVER */

    return SUCCESS;

err:
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "%s : fatal error\n", __func__);
    return FAILURE;
}

/*
 * Function     : icm_scan_and_select_channel
 * Description  : carry out 802.11 and spectral scans on required channels,
 *                and select best channel
 * Input params : pointer to icm info structrue, bool specifying whether to
 *                set best channel at the end.
 * Return       : status of type ICM_SCAN_SELECT_STATUS_T
 */
    ICM_SCAN_SELECT_STATUS_T
icm_scan_and_select_channel(ICM_INFO_T* picm, bool setchannel)
{
    int ret;
    int i = 0;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_CONFIG_T* pconf = &pdev->conf;
    ICM_SCAN_SELECT_STATUS_T status = ICM_SCAN_SELECT_STATUS_FAILURE;
    picm->best_channel = 0;

    /* Before we select the channel, we have to do
     * 0. Clear the channel characterstics (Done in init)
     * 1. Do Wireless Scan
     * 2. Get supported channels (Done in init)
     * 3. Do Spectral Scan
     * 4. Classifiy Spectral Data
     * 5. Update interference information
     * 6. Get the number of wireless network in channel
     * 7. Call channel selector
     */

    if (!picm->is_prev_scaninfo_available &&
            picm->scan_config.scan_type == 0) {
        err("No previous scan results available");
        status = ICM_SCAN_SELECT_STATUS_NOPREVINFO;
        goto err;
    }

    if (picm->scan_config.scan_type != 0) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Carrying out initialization\n");
        if (icm_selector_init(picm) != SUCCESS) {
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Selector initialization failed\n");
            goto err;
        }
    }

    if (picm->scan_config.scan_type & ICM_SCAN_TYPE_CHANNEL_SCAN) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Carrying out 802.11 scan....\n");
        ret = icm_do_80211_scan(picm);
        if (ret != 0) {
            if (ret == -2) {
                err("802.11 scan was cancelled...");
                status = ICM_SCAN_SELECT_STATUS_SCAN_CANCELLED;
            } else {
                err("802.11 scan failed...");
            }

            goto err;
        }

        picm->is_prev_scaninfo_available = TRUE;
    }

    if (picm->scan_config.scan_type & ICM_SCAN_TYPE_CHANNEL_SCAN) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Get channel properties...\n");
        if(pconf->chanload_enab) {
            if ((ret = icm_get_channel_loading(picm)) != SUCCESS) {
                err("Cannot get channel properties...");
                goto err;
            }
        }
    }

#ifdef ICM_RTR_DRIVER
    if ((picm->band == ICM_BAND_2_4G) &&
            (picm->scan_config.scan_type & ICM_SCAN_TYPE_SPECTRAL_SCAN)) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Carrying out spectral scan...\n");

        if ((ret = icm_do_spectral_scan(picm, !setchannel)) != SUCCESS) {
            err("Spectral scan failed...");
            goto err;
        }

        picm->is_prev_scaninfo_available = TRUE;
    }
#endif /* ICM_RTR_DRIVER */

    /* Update the secondary Channel usage for all channels */

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR, "Selecting the best channel...\n");
    icm_select_home_channel(picm);

    if (setchannel && picm->best_channel > 0) {
        for (i = 0; i < picm->numdevs; i++) {
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR,
                    "Configuring the best primary channel %d for %s\n", picm->best_channel, (char *)picm->dev_ifnames_list[i]);

            if (picm->selected_channel_width == ICM_CH_BW_80_PLUS_80) {
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR,
                        "Configuring the best secondary 80 MHz channel index %d for %s\n", picm->best_cfreq2_channel, (char *)picm->dev_ifnames_list[i]);
            }

            if (icm_set_width_and_channel(picm, (char *)picm->dev_ifnames_list[i]) != SUCCESS) {
                err("Could not set width/channel\n");
                goto err;
            }
        }
    }


#ifdef ICM_RTR_DRIVER
    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SELECTOR,
            "Indicating to driver that external channel selection is no longer "
            "in progress...\n");
    if (icm_clear_ext_acs_inprogress(pdev, picm) != SUCCESS) {
        return ICM_SCAN_SELECT_STATUS_FAILURE;
    }
#endif /* ICM_RTR_DRIVER */

    status = ICM_SCAN_SELECT_STATUS_SUCCESS;

err:
    return status;
}

/*
 * Function     : icm_penalize_11ax_srp
 * Description  : Penalize a channel due to presence of Spatial Reuse 802.11ax
 *                OBSS APs on it.
 * Input params : Channel, whether channel provided is pri20 or first channel in
 *                sec80, pointer to current value of usability, number of APs
 *                supporting SRP, number of APs allowing associated non-AP STAs
 *                to use SRP, number of APs allowing associated non-AP STAs to
 *                use OBSS PD.
 * Output params: Modified value of usability.
 */
void icm_penalize_11ax_srp(u_int16_t chan_num, u_int8_t is_sec80,
        u_int16_t *usability, u_int16_t num_ap_srp_supported,
        u_int16_t num_ap_client_srp_allowed,
        u_int16_t num_ap_client_obsspd_allowed)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    u_int16_t old_usability = 0;

    /*
     * 11AX TODO: This is an initial implementation. To be refined as 802.11ax
     * Spatial Reuse specifications evolve. We may or may not have to consider
     * additional OBSS PD parameters, operation widths, whether to carry out
     * near-total blackout of an SR channel if other alternatives exist (or even
     * whether to go in the opposite direction and consider SR channels as
     * favorable) etc.
     */

    ICM_ASSERT(pdev != NULL);

    ICM_ASSERT(usability != NULL);

    ICM_ASSERT(!((num_ap_srp_supported == 0) &&
                 (num_ap_client_srp_allowed == 0) &&
                 (num_ap_client_obsspd_allowed == 0)));

    old_usability = *usability;

    (*usability) /=
        ((num_ap_srp_supported * ICM_SRP_PENALIZATION_FACTOR) +
         (num_ap_client_srp_allowed * ICM_CLIENT_SRP_PENALIZATION_FACTOR) +
         (num_ap_client_obsspd_allowed * ICM_CLIENT_OBSSPD_PENALIZATION_FACTOR));

    /* 11AX TODO: Logging framework is to be modified with stats at
     * post-emulation phase when there will be more clarity and some refinements
     * can be carried out. For now, only a basic print is logged.
     */
    ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NO_PREFIX, ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "%s %hu - 11ax SR penalization: "
                "num_ap_srp_supported = %hu "
                "num_ap_client_srp_allowed=%hu "
                "num_ap_client_obsspd_allowed=%u\n "
                "old usability=%hu "
                "new usability=%hu\n",
                is_sec80?"Sec80 with first chan":"Pri20 chan", chan_num,
                num_ap_srp_supported,
                num_ap_client_srp_allowed,
                num_ap_client_obsspd_allowed,
                old_usability,
                *usability);
}

/* These are the factors used to degrade channel avaliablity in presence of
 * FHSS and MWO interference. The values are in percentage
 */
#define ICM_FHSS_INT_DEGRADE_FACTOR (60)
#define ICM_MWO_INT_DEGRADE_FACTOR (40)
#define ICM_NO_OF_EXT_CHAN (2)

/*
 * Function     : icm_select_best_chan_ng
 * Description  : Select best 2.4 GHz channel for the desired width.
 * Input params : pointer to icm info structure, width
 * Return       : Best primary 20 MHz IEEE channel number, or -1 on failure.
 */
int icm_select_best_chan_ng( ICM_INFO_T* picm, ICM_CH_BW_T ch_bw)
{
    u_int16_t overlap_chan_idx, chan_idx;
    u_int16_t adjusted_msrd_usability;
    ICM_CHANNEL_LIST_T *chanlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    ICM_DEV_INFO_T* pdev = get_pdev();
    int nf_threshold = ATH_DEFAULT_NF_THRESHOLD;
    ICM_SELDBG_DECISION_INFO_T *decdbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *prichandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *relchandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *chandbginfo = NULL;
    int                        relchandbginfo_idx = 0;
    ICM_CHANNEL_T *currch;
    u_int16_t num_ap_srp_supported = 0;
    u_int16_t num_ap_client_srp_allowed = 0;
    u_int16_t num_ap_client_obsspd_allowed = 0;
    int ret;

#ifdef ICM_RTR_DRIVER /* This needs Spectral support */
#if !ICM_SUPPORT_HIGH_NF_WAR
    /* Get the Nominal NF from the driver, if required */
    if (picm->get_nominal_nf) {
        /* Note : The ATH_DEFAULT_CW_NOISEFLOOR_DELTA is +dbm */
        nf_threshold = icm_get_nominal_noisefloor(picm) + ATH_DEFAULT_CW_NOISEFLOOR_DELTA;
    }
#endif /* !ICM_SUPPORT_HIGH_NF_WAR */
#endif /* ICM_RTR_DRIVER */

    /* 11ng has 3 non-overlapping channels:1,6 and 11 */
    u_int16_t ng_chan[] = {1,6,11};
    u_int16_t ng_chan_obss_disp[] = {1,6,11};
    /* Each non-overlapping channels overlap with other channels,
     * ex: channel 1 overlaps with 2,3 and 4
     */
    u_int16_t overlap_chan[3][2] = {{1,4},{3,9},{8,14}};
    /* Default number of channel to scan is 3, but it is 2 in case of 40 MHz mode */
    u_int16_t num_chan_to_scan = 3;
    u_int16_t num_chan_obss_disp = 3;
    u_int16_t usability;
    int best_chan = -1;
    /* Default extension channel for NG40PLUS mode */
    u_int16_t ext_chan[] = {5, 10};
    u_int16_t num_ap;

    /* Check the BW to be used and figure out the channel information */
    if (ch_bw == ICM_CH_BW_40PLUS) {
        /* Only channels 1 and 6 work in this case */
        num_chan_to_scan = 2;
        /* For channel 1 plus mode, the first channel is 1 and the
         * last channel is 8 */
        overlap_chan[0][0] = 1;
        overlap_chan[0][1] = 8;

        /* For channel 6 plus mode, the first channel is 3 and the
         * last channel is 13 */
        overlap_chan[1][0] = 3;
        overlap_chan[1][1] = 13;

    } else if (ch_bw == ICM_CH_BW_40MINUS) {
        /* Only channels 6 and 11 work in this case */
        num_chan_to_scan = 2;
        /* Channels to scan are 6 and 11 */
        ng_chan[0] = 6;
        ng_chan[1] = 11;
        /* extension channels are 2 and 7 */
        ext_chan[0] = 2;
        ext_chan[1] = 7;
        /* For channel 6 minus mode, the first channel is 1 and the
         * last channel is 8 */
        overlap_chan[0][0] = 1;
        overlap_chan[0][1] = 8;

        /* For channel 11 plus mode, the first channel is 3 and the
         * last channel is 13 */
        overlap_chan[1][0] = 3;
        overlap_chan[1][1] = 14;
    }
    /* Default is 20MHz channel */

    decdbginfo = (ICM_SELDBG_DECISION_INFO_T*)
        malloc(sizeof(ICM_SELDBG_DECISION_INFO_T) * \
                num_chan_to_scan);

    if (decdbginfo == NULL) {
        fprintf(stderr,
                "Could not allocate memory for Decision Debug Information "
                "Array\n");
        exit(EXIT_FAILURE);
    }

    ret = icm_init_seldbg_decinfo(decdbginfo,
            num_chan_to_scan,
            ICM_BAND_2_4G,
            picm->phy_spec,
            ch_bw);

    if (ret != SUCCESS) {
        fprintf(stderr,
                "Could not initialize Decision Debug Information "
                "Array\n");
        exit(EXIT_FAILURE);
    }

    /* The last channel depends on the overlap */
    /* Not all regulatory domains use channels above 11, if the last
     * channel is 11, make sure the overlap channel for that is within
     * limit
     */
    if (overlap_chan[num_chan_to_scan-1][1] > 11) {
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "Capping the upper channel to 11\n");
        overlap_chan[num_chan_to_scan-1][1] = ICM_GET_11BG_CHANNEL_LIST_PTR(picm)->count;
    }

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX, ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR, "\n");

#ifndef ANDROID
    for (chan_idx = 0; chan_idx < num_chan_obss_disp; chan_idx++) {
        currch = &chanlist->ch[ng_chan_obss_disp[chan_idx] - 1];
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NO_PREFIX, ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "----------------------------------------------------\n"
                "Channel %d - statistics from scan:\n"
                "Total OBSS APs with primary on this channel                  :%d\n"
                "20 MHz OBSS APs with primary on this channel                 :%d\n"
                "40 MHz OBSS APs with primary on this channel                 :%d\n"
                "OBSS APs with secondary 20 MHz on this channel               :%d\n"
                "OBSS APs with primary on this channel, supporting SRP        :%d\n"
                "OBSS APs with primary on this channel, allowing client SRP   :%d\n"
                "OBSS APs with primary on this channel, allowing client OBSSPD:%d\n\n",
                currch->channel,
                currch->num_wnw,
                currch->num_wnw_20,
                currch->num_wnw_40,
                currch->used_as_secondary_20,
                currch->num_wnw_srp_supported,
                currch->num_wnw_client_srp_allowed,
                currch->num_wnw_client_obsspd_allowed);
    }
#endif /* ANDROID */

    for (chan_idx = 0; chan_idx < num_chan_to_scan; chan_idx++) {
        /*
           Check added to ensure channel index is lesser than
           2 for bandwidths HT40+ and HT40-
         */
        if (ch_bw != ICM_CH_BW_20) {
            ICM_ASSERT(chan_idx < 2);
        }
        prichandbginfo = &(decdbginfo[chan_idx].prichaninfo);

        ICM_SELDBG_SETFIELD(prichandbginfo->chan_num, ng_chan[chan_idx]);
        ICM_SELDBG_SETFIELD(prichandbginfo->freq,
                chanlist->ch[ng_chan[chan_idx] - 1].freq);
        ICM_SELDBG_SETFIELD(prichandbginfo->relation,
                ICM_CHAN_RELATIONSHIP_SELF);
        ICM_SELDBG_SETFIELD(prichandbginfo->noise_floor,
                ICM_GET_CHANNEL_NOISEFLOOR(picm,
                    ng_chan[chan_idx]));
        ICM_SELDBG_SETFIELD(prichandbginfo->noise_floor_thresh,
                nf_threshold);

        if (ICM_GET_CHANNEL_EXCLUDE(picm, ng_chan[chan_idx]) == TRUE) {
            ICM_SET_CHANNEL_USABLITY(picm, ng_chan[chan_idx], 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            ICM_SELDBG_SETFIELD(prichandbginfo->is_excluded, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_EXCLD);

            continue;
        }

        /* Check to ensure our proposed primary is not on the secondary
           20 MHz of another BSS. */
        if (chanlist->ch[ng_chan[chan_idx] - 1].used_as_secondary_20) {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_obss_sec20, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_SEC20);
            ICM_SET_CHANNEL_USABLITY(picm, ng_chan[chan_idx], 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        }

        usability = ICM_GET_CHANNEL_BLUSABILITY(picm, ng_chan[chan_idx]);

        usability = icm_get_pcl_adjusted_usability(picm, ng_chan[chan_idx], usability);

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].baseline_usability,
                usability);

        /* find the least usability of the overlapping channel */
        num_ap = 0;
        for (overlap_chan_idx = overlap_chan[chan_idx][0]; overlap_chan_idx < overlap_chan[chan_idx][1]+1; overlap_chan_idx++ ) {
            relchandbginfo = NULL;

            if (overlap_chan_idx != ng_chan[chan_idx]) {
                ICM_ASSERT(overlap_chan_idx > 0);
                relchandbginfo_idx = overlap_chan_idx - 1;
                ICM_ASSERT(relchandbginfo_idx < ARRAY_LEN(decdbginfo[chan_idx].relchaninfo));
                relchandbginfo =
                    &(decdbginfo[chan_idx].relchaninfo[relchandbginfo_idx]);
                ICM_SELDBG_SETFIELD(relchandbginfo->chan_num, overlap_chan_idx);
                ICM_SELDBG_SETFIELD(relchandbginfo->freq,
                        chanlist->ch[overlap_chan_idx - 1].freq);
                ICM_SELDBG_SETFIELD(relchandbginfo->is_valid, true);
                ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec20);
                /* chan_idx check to avoid array out of bound */
                if (chan_idx < ICM_NO_OF_EXT_CHAN) {
                    if (overlap_chan_idx != ext_chan[chan_idx]) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_OVLP);
                    } else {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC20);
                        ICM_SELDBG_SETFIELD(relchandbginfo->noise_floor,
                                ICM_GET_CHANNEL_NOISEFLOOR(picm,
                                    overlap_chan_idx));
                        ICM_SELDBG_SETFIELD(relchandbginfo->noise_floor_thresh,
                                nf_threshold);

                    }
                }

                chandbginfo = relchandbginfo;
            } else {
                chandbginfo = prichandbginfo;
            }

            num_ap += chanlist->ch[overlap_chan_idx-1].num_wnw;

            num_ap_srp_supported +=
                chanlist->ch[overlap_chan_idx-1].num_wnw_srp_supported;
            num_ap_client_srp_allowed +=
                chanlist->ch[overlap_chan_idx-1].num_wnw_client_srp_allowed;
            num_ap_client_obsspd_allowed +=
                chanlist->ch[overlap_chan_idx-1].num_wnw_client_obsspd_allowed;

            if (chandbginfo != NULL) {
                ICM_SELDBG_SETFIELD(chandbginfo->num_obss_aps,
                        chanlist->ch[overlap_chan_idx-1].num_wnw);
            }

            /* Check if this is a non-20 MHz mode and there is any AP in non-home channel */
            if ((ch_bw != ICM_CH_BW_20) &&
                    (chanlist->ch[overlap_chan_idx-1].num_wnw) &&
                    (overlap_chan_idx!= ng_chan[chan_idx])) {
                /* There is a 20 MHz AP in this channel, do not use it */
                usability = 0;

                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20, true);
                }

                /* chan_idx check to avoid array out of bound error*/
                if (chan_idx < ICM_NO_OF_EXT_CHAN) {

                    if (overlap_chan_idx != ext_chan[chan_idx]) {
                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_OVLP_PRI20);
                    } else {
                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_BOND_PRI20);
                    }
                }

                break;
            }

            /* If the non-home channel is excluded by external entity, do not use it */
            if ((ch_bw != ICM_CH_BW_20) &&
                    (overlap_chan_idx != ng_chan[chan_idx]) &&
                    (ICM_GET_CHANNEL_EXCLUDE(picm, overlap_chan_idx) == TRUE)) {
                usability = 0;
                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_excluded, true);
                }

                if ((chan_idx < ICM_NO_OF_EXT_CHAN) &&
			(overlap_chan_idx != ext_chan[chan_idx])) {
                    ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                            ICM_SELDBG_REJCODE_OVLP_EXCLD);
                } else {
                    ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                            ICM_SELDBG_REJCODE_BOND_EXCLD);
                }

                break;
            }
        }

        relchandbginfo = NULL;

        if (usability == 0) {
            ICM_SET_CHANNEL_USABLITY(picm, ng_chan[chan_idx], 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        }

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps, num_ap + 1);
        usability /= (num_ap + 1);

        if ((num_ap_srp_supported != 0) ||
                (num_ap_client_srp_allowed != 0) ||
                (num_ap_client_obsspd_allowed != 0))
        {
            icm_penalize_11ax_srp(ng_chan[chan_idx], 0, &usability,
                    num_ap_srp_supported, num_ap_client_srp_allowed,
                    num_ap_client_obsspd_allowed);
        }

        /* check if there are interference in the channel */
        if ((ICM_GET_CHANNEL_NOISEFLOOR(picm, ng_chan[chan_idx]) > nf_threshold)
#ifdef ICM_RTR_DRIVER
                || (chanlist->ch[ng_chan[chan_idx]-1].flags & SPECT_CLASS_DETECT_CW)
#endif /* ICM_RTR_DRIVER */
           ) {
            ICM_SET_CHANNEL_USABLITY(picm, ng_chan[chan_idx], 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            ICM_SELDBG_SETFIELD(prichandbginfo->is_cw_intf, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_CW);
            continue;
#ifdef ICM_RTR_DRIVER
        } else if ( chanlist->ch[ng_chan[chan_idx]-1].flags & SPECT_CLASS_DETECT_MWO ) {
            /* check if there is MWO interfernece, if so degrade the channel by 60% */
            usability = (u_int16_t)(((u_int32_t)(usability)) *ICM_MWO_INT_DEGRADE_FACTOR/100);
            ICM_SELDBG_SETFIELD(prichandbginfo->is_mwo_intf, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].mwo_degrade_factor,
                    ICM_MWO_INT_DEGRADE_FACTOR);
        } else if (chanlist->ch[ng_chan[chan_idx]-1].flags & SPECT_CLASS_DETECT_FHSS) {
            /* Found FHSS interference. Degrade the usability by 40% */
            usability = (u_int16_t)(((u_int32_t)(usability)) *ICM_FHSS_INT_DEGRADE_FACTOR/100);
            ICM_SELDBG_SETFIELD(prichandbginfo->is_fhss_intf, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].fhss_degrade_factor,
                    ICM_FHSS_INT_DEGRADE_FACTOR);
#endif /* ICM_RTR_DRIVER */
        }

        adjusted_msrd_usability = ICM_GET_CHANNEL_FREE_TIME(picm, ng_chan[chan_idx]);
        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].prepnl_measured_usability,
                adjusted_msrd_usability);

        if (ICM_GET_CHANNEL_BLUSABILITY(picm, ng_chan[chan_idx]) != MAX_USABILITY) {
            /* Factor in penalization to scale down channel load that we should be considering */
            adjusted_msrd_usability = (u_int16_t)(((u_int32_t)(adjusted_msrd_usability)) *
                    ICM_GET_CHANNEL_BLUSABILITY(picm, ng_chan[chan_idx]) / MAX_USABILITY);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_penalized, true);
        }

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].measured_usability,
                adjusted_msrd_usability);

        if (usability > adjusted_msrd_usability) {
            usability = adjusted_msrd_usability;
        }

        /* This can get overwritten in the case of 40 MHz */
        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability,
                usability);
        ;
        /* In case of 40MHz channels, check if the extension channel is clear */
        if ((ch_bw != ICM_CH_BW_20) && (chan_idx < ICM_NO_OF_EXT_CHAN)) {
            ICM_ASSERT(ext_chan[chan_idx] > 0);
            relchandbginfo_idx = ext_chan[chan_idx] - 1;
            ICM_ASSERT(relchandbginfo_idx < ARRAY_LEN(decdbginfo[chan_idx].relchaninfo));
            relchandbginfo =
                &(decdbginfo[chan_idx].relchaninfo[relchandbginfo_idx]);

            /* Check if the extension channel has CW interference, if so set the usability to zero */
            if ((ICM_GET_CHANNEL_NOISEFLOOR(picm, ext_chan[chan_idx]) > nf_threshold)
#ifdef ICM_RTR_DRIVER
                    || (chanlist->ch[ext_chan[chan_idx]-1].flags & SPECT_CLASS_DETECT_CW)
#endif /* ICM_RTR_DRIVER */
               ) {
                ICM_SET_CHANNEL_USABLITY(picm, ng_chan[chan_idx], 0);
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_cw_intf, true);
                }
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                        ICM_SELDBG_REJCODE_BOND_CW);

                continue;
#ifdef ICM_RTR_DRIVER
            } else if (chanlist->ch[ext_chan[chan_idx]-1].flags & SPECT_CLASS_DETECT_MWO ) {
                /* check if there is MWO interfernece, if so degrade the channel by 60% */
                usability = (u_int16_t)(((u_int32_t)(usability)) *ICM_MWO_INT_DEGRADE_FACTOR/100);
                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_mwo_intf, true);
                }
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].mwo_degrade_factor,
                        ICM_MWO_INT_DEGRADE_FACTOR);
            } else if(chanlist->ch[ext_chan[chan_idx]-1].flags & SPECT_CLASS_DETECT_FHSS ) {
                /* check if there is MWO interfernece, if so degrade the channel by 40% */
                usability = (u_int16_t)(((u_int32_t)(usability)) *ICM_FHSS_INT_DEGRADE_FACTOR/100);
                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_fhss_intf, true);
                }
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].fhss_degrade_factor,
                        ICM_FHSS_INT_DEGRADE_FACTOR);
#endif /* ICM_RTR_DRIVER */
            }

            adjusted_msrd_usability = ICM_GET_CHANNEL_FREE_TIME(picm, ext_chan[chan_idx]);
            ICM_SELDBG_SETFIELD( \
                    decdbginfo[chan_idx].prepnl_measured_usability_ext,
                    adjusted_msrd_usability);
            if (ICM_GET_CHANNEL_BLUSABILITY(picm, ext_chan[chan_idx]) != MAX_USABILITY) {
                /* Factor in penalization to scale down channel load that we should be considering */
                adjusted_msrd_usability = (u_int16_t)(((u_int32_t)(adjusted_msrd_usability)) *
                        ICM_GET_CHANNEL_BLUSABILITY(picm, ext_chan[chan_idx]) / MAX_USABILITY);
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_penalized_ext,
                        true);
            }

            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].measured_usability_ext,
                    adjusted_msrd_usability);

            if (usability > adjusted_msrd_usability) {
                usability = adjusted_msrd_usability;
            }
        }
        ICM_SET_CHANNEL_USABLITY(picm, ng_chan[chan_idx], usability);
        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability,
                usability);
    }

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NONE,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "Channel usability summary below:\n");

    /* Rank the channels according to usability */
    usability = 0;
    for (chan_idx = 0; chan_idx < num_chan_to_scan; chan_idx++) {
        if (ICM_GET_CHANNEL_USABLITY(picm, ng_chan[chan_idx]) > usability) {
            best_chan = ng_chan[chan_idx];
            usability = ICM_GET_CHANNEL_USABLITY(picm, ng_chan[chan_idx]);
        }
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                " chan:%3d  usability:%5d\n",
                ng_chan[chan_idx],
                ICM_GET_CHANNEL_USABLITY(picm, ng_chan[chan_idx]));
    }

    /* If all channels are bad, select a channel without CW interference */
    if (best_chan == -1) {
        /* If there is a higher level logic, return at this point */
        if (pdev->conf.server_mode) {
            if (icm_seldbg_process(pdev,
                        decdbginfo,
                        num_chan_to_scan) != SUCCESS) {
                exit(EXIT_FAILURE);
            }

            free(decdbginfo);
            return best_chan;
        }

        if (ch_bw != ICM_CH_BW_20) {
            if (icm_seldbg_process(pdev,
                        decdbginfo,
                        num_chan_to_scan) != SUCCESS) {
                exit(EXIT_FAILURE);
            }

            free(decdbginfo);
            return -1;
        } else {
            /* Find the first channel that is not excluded */
            for (chan_idx = 0; chan_idx < num_chan_to_scan; chan_idx++) {
                if (ICM_GET_CHANNEL_EXCLUDE(picm, ng_chan[chan_idx]) != TRUE) {
                    best_chan = ng_chan[chan_idx];
                    break;
                }
            }

            /* If all are excluded (!), give up. The channel set will fail,
               and the external entity will need to review its logic. */
            if (best_chan == -1) {
                ICM_DPRINTF(pdev,
                        ICM_PRCTRL_FLAG_NONE,
                        ICM_DEBUG_LEVEL_DEFAULT,
                        ICM_MODULE_ID_SELECTOR,
                        "All valid channels excluded. Giving up.\n");
            }
        }
    }

    /* Set the best channel into the corresponding debug entry */
    if (best_chan != -1) {
        for (chan_idx = 0; chan_idx < num_chan_to_scan; chan_idx++) {
            if (best_chan == decdbginfo[chan_idx].prichaninfo.chan_num) {
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_bestchan,
                        true);
            } else {
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_bestchan,
                        false);
            }
        }
    }

    if (icm_seldbg_process(pdev,
                decdbginfo,
                num_chan_to_scan) != SUCCESS) {
        exit(EXIT_FAILURE);
    }

    free(decdbginfo);
    return best_chan;
}

/*
 * Function     : icm_select_best_chan_na
 * Description  : Select best 5GHz channel for the desired width. In the case of
 *                80+80, each 80 MHz segment is considered an independent
 *                environment to be evaluated. This provides for flexibility.
 *                Thus, for 80+80, the function should be called first for
 *                Segment 1 (primary 80 MHz), then Segment 2 (secondary 80 MHz).
 * Input params : -pointer to icm info structure
 *                -width
 *                -segment ID (valid only for 80+80, ignored otherwise) - 0 for
 *                primary 80 MHz, 1 for secondary 80 MHz.
 *                -IEEE channel number for primary 20 MHz already selected, for
 *                80+80 MHz mode (valid and required only for function call with
 *                Segment ID=2, to avoid selecting channel in/adjacent to
 *                Segment ID 1. Ignored otherwise).
 * Return       : For 80+80, Segment ID=2: IEEE channel number corresponding to
 *                centre frequency of best secondary 80 MHz, or -1 on failure.
 *                All other cases: Best primary 20 MHz IEEE channel number, or -1
 *                on failure.
 */
int icm_select_best_chan_na( ICM_INFO_T* picm,
        ICM_CH_BW_T ch_bw, u_int8_t seg_id, int pri20_chan_80_80)
{
    ICM_CHANNEL_LIST_T *chanlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    int best_chan;
    u_int16_t usability, usability_40_ppdu, usability_80_ppdu,
              usability_160_ppdu;
    u_int16_t prepnl_msrd_usability = 0;
    u_int16_t prepnl_msrd_usability_40_ppdu = 0;
    u_int16_t prepnl_msrd_usability_80_ppdu = 0;
    u_int16_t prepnl_msrd_usability_160_ppdu = 0;
    u_int16_t adjusted_msrd_usability = 0;
    u_int16_t chan_idx, chan_num, bondspan_chan_num;
    u_int16_t num_ap, num_ap_pri40, num_ap_sec40, num_ap_pri80, num_ap_sec80,
              num_ap_80, num_ap_160;
    u_int16_t num_ap_pri40_aci, num_ap_sec40_aci, num_ap_pri80_aci,
              num_ap_sec80_aci;
    u_int16_t num_ap_tot_40ppdu_compete, num_ap_tot_80ppdu_compete,
              num_ap_tot_160ppdu_compete;
    u_int16_t num_ap_srp_supported = 0;
    u_int16_t num_ap_client_srp_allowed = 0;
    u_int16_t num_ap_client_obsspd_allowed = 0;
    u_int8_t skip_channel = 0;
    u_int8_t sec80_80p80_increment_done = 0;
    int nf_threshold_base = ATH_DEFAULT_NF_THRESHOLD;
    int nf_threshold = 0;
    int nf_threshold_correction = 0;
    int nf = 0;
    ICM_CHANNEL_T *ch;
    char phy_spec_str[ICM_PHY_SPEC_STR_SIZE];

    int cw_check_min_adj = 0;
    int cw_check_max_adj = 1;

    int adj_check_min_adj = -1;
    int adj_check_max_adj = 2;
    int bondspan_chan_adj, bondspan_chan_idx, usable_chan_idx;
    int relchandbginfo_idx = 0;
    u_int16_t shift_cnt;
    ICM_SELDBG_DECISION_INFO_T *decdbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *prichandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *relchandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *chandbginfo = NULL;
    int ret;

    ICM_DEV_INFO_T *pdev = get_pdev();

    ICM_ASSERT(!((ch_bw > ICM_CH_BW_20) &&
                 (picm->phy_spec != ICM_PHY_SPEC_11NA) &&
                 (picm->phy_spec != ICM_PHY_SPEC_11AC) &&
                 (picm->phy_spec != ICM_PHY_SPEC_11AXA)));

    ICM_ASSERT(!((ch_bw > ICM_CH_BW_40) &&
                 (picm->phy_spec != ICM_PHY_SPEC_11AC) &&
                 (picm->phy_spec != ICM_PHY_SPEC_11AXA)));

    ICM_ASSERT(ch_bw < ICM_CH_BW_INVALID);

    if ((ch_bw == ICM_CH_BW_80_PLUS_80) && (seg_id == 1)) {
        return icm_best_cfreq2_selection_helper(picm, pri20_chan_80_80);
    }

    decdbginfo = (ICM_SELDBG_DECISION_INFO_T*)
        malloc(sizeof(ICM_SELDBG_DECISION_INFO_T) * \
                (chanlist->count));

    if (decdbginfo == NULL) {
        fprintf(stderr,
                "Could not allocate memory for Decision Debug Information "
                "Array\n");
        exit(EXIT_FAILURE);
    }

    ret = icm_init_seldbg_decinfo(decdbginfo,
            chanlist->count,
            ICM_BAND_5G,
            picm->phy_spec,
            ch_bw);

    if (ret != SUCCESS) {
        fprintf(stderr,
                "Could not initialize Decision Debug Information "
                "Array\n");
        exit(EXIT_FAILURE);
    }

    icm_phy_spec_to_str(picm->phy_spec, phy_spec_str, sizeof(phy_spec_str));

#ifdef ICM_RTR_DRIVER /* This needs Spectral support */
#if !ICM_SUPPORT_HIGH_NF_WAR
    /* Get the Nominal NF from the driver, if required */
    if (picm->get_nominal_nf) {
        /* Note : The ATH_DEFAULT_CW_NOISEFLOOR_DELTA is +dbm */
        /* XXX: Check if changes are required for VHT80 */
        nf_threshold_base = icm_get_nominal_noisefloor(picm) + ATH_DEFAULT_CW_NOISEFLOOR_DELTA;
    }
#endif /* !ICM_SUPPORT_HIGH_NF_WAR */
#endif /* ICM_RTR_DRIVER */

    if (ch_bw == ICM_CH_BW_40PLUS) {
        cw_check_min_adj = 0;
        cw_check_max_adj = 2;

        adj_check_min_adj = -1;
        adj_check_max_adj = 3;
    }

    if (ch_bw == ICM_CH_BW_40MINUS) {
        cw_check_min_adj = -1;
        cw_check_max_adj = 1;

        adj_check_min_adj = -2;
        adj_check_max_adj = 2;
    }

    /* For ICM_CH_BW_80 and higher, the min and max values will be
       dynamically figured out per-channel since the corresponding block
       coverages vary. */

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX, ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR, "\n");

    for (chan_idx = 0; chan_idx < chanlist->count; chan_idx++) {
        ch = &chanlist->ch[chan_idx];
        chan_num = ch->channel;

#ifndef ANDROID
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NO_PREFIX, ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "-------------------------------------------------------------------\n"
                "Channel %d - statistics from scan:\n"
                "Total OBSS APs with primary on this channel                  :%d\n"
                "20 MHz OBSS APs with primary on this channel                 :%d\n"
                "40 MHz OBSS APs with primary on this channel                 :%d\n"
                "80 MHz OBSS APs with primary on this channel                 :%d\n"
                "160 MHz OBSS APs with primary on this channel                :%d\n"
                "80+80 MHz OBSS APs with primary on this channel              :%d\n"
                "OBSS APs with primary on this channel, supporting SRP        :%d\n"
                "OBSS APs with primary on this channel, allowing client SRP   :%d\n"
                "OBSS APs with primary on this channel, allowing client OBSSPD:%d\n"
                "OBSS APs with secondary 20 MHz on this channel               :%d\n"
                "160/80+80 MHz OBSS APs with secondary 40 MHz on this channel :%d\n"
                "160 MHz OBSS APs with secondary 80 MHz on this channel       :%d\n"
                "80+80 MHz OBSS APs with secondary 80 MHz on this channel     :%d\n"
                "Measured free time indication for this channel alone         :%u\n\n",
                chan_num,
                ch->num_wnw,
                ch->num_wnw_20,
                ch->num_wnw_40,
                ch->num_wnw_80,
                ch->num_wnw_160,
                ch->num_wnw_80_80,
                ch->num_wnw_srp_supported,
                ch->num_wnw_client_srp_allowed,
                ch->num_wnw_client_obsspd_allowed,
                ch->used_as_secondary_20,
                ch->used_as_160_80p80_secondary_40,
                ch->used_as_160_secondary_80,
                ch->used_as_80p80_secondary_80,
                ICM_GET_CHANNEL_FREE_TIME(picm, chan_num));
#endif /* ANDROID */

        prichandbginfo = &(decdbginfo[chan_idx].prichaninfo);

        ICM_SELDBG_SETFIELD(prichandbginfo->chan_num, chan_num);
        ICM_SELDBG_SETFIELD(prichandbginfo->freq, ch->freq);
        ICM_SELDBG_SETFIELD(prichandbginfo->relation,
                ICM_CHAN_RELATIONSHIP_SELF);

        usability = ICM_GET_CHANNEL_BLUSABILITY(picm, chan_num);

        usability = icm_get_pcl_adjusted_usability(picm, chan_num, usability);

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].baseline_usability,
                usability);

        if (ICM_GET_CHANNEL_EXCLUDE(picm, chan_num) == TRUE) {
            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            ICM_SELDBG_SETFIELD(prichandbginfo->is_excluded, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_EXCLD);

            continue;
        } else {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_excluded, false);
        }

        /* Check if the channel is weather radar channel in ETSI domain.
         * This channel has 10 minutes CAC, so it is better not to use
         * it
         */
        if ((picm->dfs_domain == NL80211_DFS_ETSI) &&
                WEATHER_RADAR_CHANNEL((int)ch->freq)) {
            /* This is a radar channel and cannot be used */
            ICM_SELDBG_SETFIELD(prichandbginfo->is_etsi_weather, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_ETSIWTH);

            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        } else {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_etsi_weather, false);
        }

        /* Check if 40PLUS or 40MINUS have been requested and if the current channel is capable
           of these. */
        if ((ch_bw == ICM_CH_BW_40PLUS) &&
                (((picm->phy_spec == ICM_PHY_SPEC_11NA) && !(IEEE80211_IS_CHAN_11N_CTL_U_CAPABLE(ch))) ||
                 ((picm->phy_spec == ICM_PHY_SPEC_11AC) && !(IEEE80211_IS_CHAN_11AC_VHT40PLUS(ch))) ||
                 ((picm->phy_spec == ICM_PHY_SPEC_11AXA) && !(IEEE80211_IS_CHAN_11AXA_HE40PLUS(ch))))) {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_incapable, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_INCAP);

            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        }

        if ((ch_bw == ICM_CH_BW_40MINUS) &&
                (((picm->phy_spec == ICM_PHY_SPEC_11NA) && !(IEEE80211_IS_CHAN_11N_CTL_L_CAPABLE(ch))) ||
                 ((picm->phy_spec == ICM_PHY_SPEC_11AC) && !(IEEE80211_IS_CHAN_11AC_VHT40MINUS(ch))) ||
                 ((picm->phy_spec == ICM_PHY_SPEC_11AXA) && !(IEEE80211_IS_CHAN_11AXA_HE40MINUS(ch))))) {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_incapable, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_INCAP);

            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        }

        /* Check if 80/80+80/160 MHz has been requested and if the current
           channel is capable of the same.Note that we have Assert protection to
           ensure that the PHY spec in this case is 11AC/11AXA. So we don't
           factor it into the if check.
         */
        if (((ch_bw == ICM_CH_BW_80) && !(IEEE80211_IS_CHAN_11AC_VHT80(ch)) &&
                    !(IEEE80211_IS_CHAN_11AXA_HE80(ch))) ||
            ((ch_bw == ICM_CH_BW_80_PLUS_80) && (seg_id == 0) &&
                    !(IEEE80211_IS_CHAN_11AC_VHT80_80(ch)) &&
                    !(IEEE80211_IS_CHAN_11AXA_HE80_80(ch))) ||
            ((ch_bw == ICM_CH_BW_160) && !(IEEE80211_IS_CHAN_11AC_VHT160(ch)) &&
                    !(IEEE80211_IS_CHAN_11AXA_HE160(ch)))) {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_incapable, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_INCAP);

            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        }

        /* At this point, we know that our primary is capable of the requested
           PHY spec and width. */
        ICM_SELDBG_SETFIELD(prichandbginfo->is_incapable, false);

        /* Check to ensure our proposed primary is not on the secondary
           20 MHz of another BSS.
Note: Though there are further checks to see if our
secondary falls on the primary of another BSS and thus provide
an 'inverted check' of the above, this is insufficient
to cover all cases (e.g. if we want to become a VHT20 BSS).
Thus, there is a small overlap in our checks, but this is
fine since we are not in the performance path. */
        if (chanlist->ch[chan_idx].used_as_secondary_20) {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_obss_sec20, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_SEC20);

            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        } else {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_obss_sec20, false);
        }

        /* Check to ensure our proposed primary is not on the secondary
           40 MHz of another 160/80+80 MHz BSS. */
        if (chanlist->ch[chan_idx].used_as_160_80p80_secondary_40) {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_obss_sec40, true);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                    ICM_SELDBG_REJCODE_PRI_SEC40);

            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        } else {
            ICM_SELDBG_SETFIELD(prichandbginfo->is_obss_sec40, false);
        }

        /* XXX The below is a retrofit into the existing logic; we should look
           at reworking the flow of the entire selection process to make it
           generic and pluggable.

Note:We could collapse the below into a generic formula, but we don't
for the sake of readability. This code isn't in a performance path.
For similar reasons, we do not use macro definitions to denote
displacement limits.
         */
        if (ch_bw == ICM_CH_BW_80) {
            int cw_check_min_adj_firstchan = 0;
            int cw_check_max_adj_firstchan = 4;

            if (chan_num == (ch->ic_vhtop_ch_freq_seg1 - 6)) {
                /* This is the first channel in the 80 MHz block, being
                   at a displacement of -6 channels * 5 MHz = -30 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan;
                cw_check_max_adj = cw_check_max_adj_firstchan;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1 - 2)) {
                /* This is the second channel in the 80 MHz block, being
                   at a displacement of -2 channels * 5 MHz = -10 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 1;
                cw_check_max_adj = cw_check_max_adj_firstchan - 1;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1 + 2)) {
                /* This is the third channel in the 80 MHz block, being
                   at a displacement of +2 channels * 5 MHz = +10 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 2;
                cw_check_max_adj = cw_check_max_adj_firstchan - 2;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1 + 6)) {
                /* This is the fourth channel in the 80 MHz block, being
                   at a displacement of +6 channels * 5 MHz = +30 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 3;
                cw_check_max_adj = cw_check_max_adj_firstchan - 3;
            } else {
                /* We shouldn't get here! */
                ICM_ASSERT(0);
            }

            adj_check_min_adj = cw_check_min_adj - 1;
            adj_check_max_adj = cw_check_max_adj + 1;
        } else if (ch_bw == ICM_CH_BW_160) {
            int cw_check_min_adj_firstchan = 0;
            int cw_check_max_adj_firstchan = 8;

            if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 - 14)) {
                /* This is the first channel in the 160 MHz block, being
                   at a displacement of -14 channels * 5 MHz = -70 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan;
                cw_check_max_adj = cw_check_max_adj_firstchan;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 - 10)) {
                /* This is the second channel in the 160 MHz block, being
                   at a displacement of -10 channels * 5 MHz = -50 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 1;
                cw_check_max_adj = cw_check_max_adj_firstchan - 1;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 - 6)) {
                /* This is the third channel in the 160 MHz block, being
                   at a displacement of -6 channels * 5 MHz = -30 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 2;
                cw_check_max_adj = cw_check_max_adj_firstchan - 2;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 - 2)) {
                /* This is the fourth channel in the 160 MHz block, being
                   at a displacement of -2 channels * 5 MHz = -10 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 3;
                cw_check_max_adj = cw_check_max_adj_firstchan - 3;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 + 2)) {
                /* This is the fifth channel in the 160 MHz block, being
                   at a displacement of +2 channels * 5 MHz = +10 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 4;
                cw_check_max_adj = cw_check_max_adj_firstchan - 4;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 + 6)) {
                /* This is the sixth channel in the 160 MHz block, being
                   at a displacement of +6 channels * 5 MHz = +30 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 5;
                cw_check_max_adj = cw_check_max_adj_firstchan - 5;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 + 10)) {
                /* This is the seventh channel in the 160 MHz block, being
                   at a displacement of +10 channels * 5 MHz = +50 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 6;
                cw_check_max_adj = cw_check_max_adj_firstchan - 6;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1_160 + 14)) {
                /* This is the eighth channel in the 160 MHz block, being
                   at a displacement of +14 channels * 5 MHz = +70 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 7;
                cw_check_max_adj = cw_check_max_adj_firstchan - 7;
            } else {
                /* We shouldn't get here! */
                ICM_ASSERT(0);
            }

            adj_check_min_adj = cw_check_min_adj - 1;
            adj_check_max_adj = cw_check_max_adj + 1;
        } else if ((ch_bw == ICM_CH_BW_80_PLUS_80) && (seg_id == 0)) {
            /* Process primary 80 MHz segment */

            int cw_check_min_adj_firstchan = 0;
            int cw_check_max_adj_firstchan = 4;

            if (chan_num == (ch->ic_vhtop_ch_freq_seg1 - 6)) {
                /* This is the first channel in the primary 80 MHz block, being
                   at a displacement of -6 channels * 5 MHz = -30 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan;
                cw_check_max_adj = cw_check_max_adj_firstchan;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1 - 2)) {
                /* This is the second channel in the primary 80 MHz block, being
                   at a displacement of -2 channels * 5 MHz = -10 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 1;
                cw_check_max_adj = cw_check_max_adj_firstchan - 1;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1 + 2)) {
                /* This is the third channel in the primary 80 MHz block, being
                   at a displacement of +2 channels * 5 MHz = +10 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 2;
                cw_check_max_adj = cw_check_max_adj_firstchan - 2;
            } else if (chan_num == (ch->ic_vhtop_ch_freq_seg1 + 6)) {
                /* This is the fourth channel in the primary 80 MHz block, being
                   at a displacement of +6 channels * 5 MHz = +30 MHz from
                   the center */
                cw_check_min_adj = cw_check_min_adj_firstchan - 3;
                cw_check_max_adj = cw_check_max_adj_firstchan - 3;
            } else {
                /* We shouldn't get here! */
                ICM_ASSERT(0);
            }

            adj_check_min_adj = cw_check_min_adj - 1;
            adj_check_max_adj = cw_check_max_adj + 1;
        }

        /* Check if CW interference in current and extension channel (if present).
           In the case of 80 MHz, check all channels covered by the 80 MHz block
           to which this contender for primary channel belongs to. */
        for (bondspan_chan_adj = cw_check_min_adj; bondspan_chan_adj < cw_check_max_adj; bondspan_chan_adj++) {
            relchandbginfo = NULL;

            /* Check if this is a valid channel */
            bondspan_chan_idx = chan_idx + bondspan_chan_adj;
            /* If the channel index is less then zero, skip */
            if (bondspan_chan_idx < 0 &&  bondspan_chan_idx >= MAX_NUM_CHANNEL) continue;

            bondspan_chan_num = chanlist->ch[bondspan_chan_idx].channel;
            /* Sanity check -- Check if the channel exists */
            if (bondspan_chan_num != (chan_num + (bondspan_chan_adj * 4))) continue;

            if (bondspan_chan_num != chan_num) {
                relchandbginfo_idx = bondspan_chan_adj - adj_check_min_adj;
                relchandbginfo =
                    &(decdbginfo[chan_idx].relchaninfo[relchandbginfo_idx]);
                ICM_SELDBG_SETFIELD(relchandbginfo->chan_num,
                        bondspan_chan_num);
                ICM_SELDBG_SETFIELD(relchandbginfo->freq,
                        chanlist->ch[bondspan_chan_idx].freq);
                ICM_SELDBG_SETFIELD(relchandbginfo->is_valid, true);
                ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec20);
                ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec40);

                if (ch_bw == ICM_CH_BW_40 ||
                        ch_bw == ICM_CH_BW_40PLUS ||
                        ch_bw == ICM_CH_BW_40MINUS) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                            ICM_CHAN_RELATIONSHIP_SEC20);
                } else if (ch_bw == ICM_CH_BW_80) {
                    if (ICM_IS_GRTEQ80BW_CHAN_RELATION_SEC20(picm, ch,
                                bondspan_chan_num)) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC20);
                    } else {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC40);
                    }
                } else if (ch_bw == ICM_CH_BW_160) {
                    if (ICM_IS_GRTEQ80BW_CHAN_RELATION_SEC20(picm, ch,
                                bondspan_chan_num)) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC20);
                    } else if (ICM_IS_160BW_CHAN_RELATION_SEC40(picm, ch,
                                bondspan_chan_num))  {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC40);
                    } else {
                        /* The selection of the bonding span channel range
                         * ensures that the assumption in this else block holds.
                         */
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC80);
                    }
                } else if ((ch_bw == ICM_CH_BW_80_PLUS_80) && (seg_id == 0)) {
                    if (ICM_IS_GRTEQ80BW_CHAN_RELATION_SEC20(picm, ch,
                                bondspan_chan_num)) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC20);
                    } else {
                        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                                ICM_CHAN_RELATIONSHIP_SEC40);
                    }
                }
            }

            /* If the non-control channel is excluded, the channel cannot be used */
            if (bondspan_chan_num != chan_num) {
                if (ICM_GET_CHANNEL_EXCLUDE(picm, bondspan_chan_num) == TRUE) {
                    if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->is_excluded, true);
                    }
                    ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                            ICM_SELDBG_REJCODE_BOND_EXCLD);
                    usability = 0;
                    break;
                } else if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_excluded, false);
                }
            }

            /* If there is an AP in non-control channel, the channel cannot be used */
            if (bondspan_chan_num != chan_num) {
                if (ch_bw == ICM_CH_BW_40 ||
                        ch_bw == ICM_CH_BW_40PLUS ||
                        ch_bw == ICM_CH_BW_40MINUS) {
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }
                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_BOND_PRI20);
                        usability = 0;
                        break;
                    } else if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                false);
                    }
                } else if ((ch_bw == ICM_CH_BW_80 || ch_bw == ICM_CH_BW_160 ||
                            ((ch_bw == ICM_CH_BW_80_PLUS_80) && (seg_id == 0))) &&
                           ICM_IS_GRTEQ80BW_CHAN_RELATION_SEC20(picm, ch,
                                bondspan_chan_num)) {
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }
                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_BOND_PRI20);
                        usability = 0;
                        break;
                    } else if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                false);
                    }
                }
            }

            /* Check if there is CW interference */
            if (bondspan_chan_num > 140) {
                /* As the frequency increases, there is dip in the nominal noisefloor value
                   due to receive sensitivity. To compensate for this, correction of -7dbm is
                   added to the nominal noisefloor value */
                nf_threshold_correction = ATH_DEFAULT_NF_CORRECTION;
            } else {
                nf_threshold_correction = 0;
            }

            nf_threshold = nf_threshold_base + nf_threshold_correction;
            nf = ICM_GET_CHANNEL_NOISEFLOOR(picm, bondspan_chan_num);

            if (bondspan_chan_num == chan_num) {
                chandbginfo = prichandbginfo;
            } else {
                chandbginfo = relchandbginfo;
            }

            if (chandbginfo != NULL) {
                ICM_SELDBG_SETFIELD(chandbginfo->noise_floor_thresh,
                        nf_threshold);
                ICM_SELDBG_SETFIELD(chandbginfo->noise_floor, nf);
            }

            if (nf > nf_threshold) {
                if (chandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(chandbginfo->is_cw_intf, true);
                }

                if (bondspan_chan_num == chan_num) {
                    ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                            ICM_SELDBG_REJCODE_PRI_CW);
                } else {
                    ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                            ICM_SELDBG_REJCODE_BOND_CW);
                }

                usability = 0;
                break;
            } else if (chandbginfo != NULL) {
                ICM_SELDBG_SETFIELD(chandbginfo->is_cw_intf, false);
            }

            /* Check if the associated channel falls in weather radar channel */
            if (bondspan_chan_num != chan_num) {
                if ((picm->dfs_domain == NL80211_DFS_ETSI) &&
                        WEATHER_RADAR_CHANNEL(\
                            (int)chanlist->ch[bondspan_chan_idx].freq)) {
                    /* This is a radar channel and cannot be used */

                    if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->is_etsi_weather,
                                true);
                    }
                    ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                            ICM_SELDBG_REJCODE_BOND_ETSIWTH);

                    usability = 0;
                    break;
                } else if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_etsi_weather,
                            false);
                }
            }
        }

        /* Check if we broke away for some reason. If so, set the usability to zero and continue */
        if (usability == 0) {
            ICM_SET_CHANNEL_USABLITY(picm, chan_num, 0);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, 0);
            continue;
        }

        if (ch_bw < ICM_CH_BW_80) {
            prepnl_msrd_usability = ICM_GET_CHANNEL_FREE_TIME(picm, chan_num);

            /* Count the number of APs in the current and close by channels
             * NOTE: this is an overlap in case of 40/80 MHz channels because we already made sure that there is
             * no APs in the extension channels. But to keep things simple ---
             */
            num_ap = 0;
            for (bondspan_chan_adj = adj_check_min_adj; bondspan_chan_adj < adj_check_max_adj; bondspan_chan_adj++) {
                relchandbginfo = NULL;

                /* Check if this is a valid channel */
                bondspan_chan_idx = chan_idx + bondspan_chan_adj;

                if (bondspan_chan_idx < 0 &&  bondspan_chan_idx >= MAX_NUM_CHANNEL) {
                    continue;
                }

                bondspan_chan_num = chanlist->ch[bondspan_chan_idx].channel;

                /* Sanity check -- Check if the channel exists/needs to be
                   considered */
                if (bondspan_chan_num != (chan_num + (bondspan_chan_adj * 4))) {
                    continue;
                }

                if (bondspan_chan_num != chan_num) {
                    relchandbginfo_idx = bondspan_chan_adj - adj_check_min_adj;
                    relchandbginfo =
                        &(decdbginfo[chan_idx].relchaninfo[relchandbginfo_idx]);
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_valid, true);
                }

                if (((bondspan_chan_adj == adj_check_min_adj) ||
                            (bondspan_chan_adj == (adj_check_max_adj - 1)))&&
                        relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->chan_num,
                            bondspan_chan_num);
                    ICM_SELDBG_SETFIELD(relchandbginfo->freq,
                            chanlist->ch[bondspan_chan_idx].freq);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_incapable);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_excluded);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_pri20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec40);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor_thresh);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->maxregpower);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_etsi_weather);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_cw_intf);
                    /* Note: MWO has already been marked NA by
                       icm_init_seldbg_decinfo() since this is band dependent
                       rather than channel dependent. */
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_fhss_intf);
                    ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                            ICM_CHAN_RELATIONSHIP_ADJ);
                }

                num_ap += chanlist->ch[bondspan_chan_idx].num_wnw;

                if ((bondspan_chan_adj != adj_check_min_adj) &&
                    (bondspan_chan_adj != (adj_check_max_adj - 1))) {
                    /* Increment SR counts for bondspan channels that fall in
                     * the operating range (but not adjacent channels outside
                     * it).
                     * 11AX TODO: If later data suggests significantly enhanced
                     * ACI effects due to SR, then revisit.
                     */
                    num_ap_srp_supported +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_srp_supported;
                    num_ap_client_srp_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_srp_allowed;
                    num_ap_client_obsspd_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_obsspd_allowed;
                }

                if (bondspan_chan_num != chan_num) {
                    if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->num_obss_aps,
                                chanlist->ch[bondspan_chan_idx].num_wnw);
                    }
                } else {
                    ICM_SELDBG_SETFIELD(prichandbginfo->num_obss_aps,
                            chanlist->ch[bondspan_chan_idx].num_wnw);
                }

                if ((bondspan_chan_adj > adj_check_min_adj) &&
                        (bondspan_chan_adj < (adj_check_min_adj - 1)) &&
                        (bondspan_chan_num != chan_num)) {
                    /* Update measured usability for sec20 */
                    prepnl_msrd_usability =
                        MIN(prepnl_msrd_usability,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                }
            }

            usability /= (num_ap+1);

            if ((num_ap_srp_supported != 0) ||
                    (num_ap_client_srp_allowed != 0) ||
                    (num_ap_client_obsspd_allowed != 0))
            {
                icm_penalize_11ax_srp(chan_num, 0, &usability,
                        num_ap_srp_supported, num_ap_client_srp_allowed,
                        num_ap_client_obsspd_allowed);
            }

            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps, num_ap + 1);
        } else if (ch_bw == ICM_CH_BW_80) {
            /* The usability is computed based on
               a) potential 20/40 MHz PPDU Tx requirement
               AND
               b) potential 80 MHz PPDU Tx requirement

               |--Independent 20/40 MHz Tx--|---Independent 20/40 MHz Tx--|

               |------------------------80 MHz Tx-------------------------|

               The 20/40 MHz PPDU Tx requirement is affected by
               i)  Competing 80 MHz PPDU Tx
               ii) Competing 20/40 MHz PPDU Tx in only our own 40 MHz block,
               not those in the adjacent 40 MHz block under our 80 MHz
               block.
               iii)Transmissions in immediate adjacent channel outside 80 MHz
               block due to ACI. (XXX: To be quantified further for
               QCA 11ac chipsets).

               The 80 MHz PPDU Tx requirement is affected by
               i)  Competing 80 MHz PPDU Tx
               ii) Competing 20/40 MHz PPDU Tx in our own 40 MHz block.
               iii)Competing 20/40 MHz PPDU Tx in the adjacent 40 MHz block
               under our 80 MHz block.
               iv)Transmissions in both adjacent channels outside 80 MHz
               block due to ACI.

               We compute two separate usabilities for each requirement,
               and then combine the usabilities in a configurable ratio.
               While computing the two usabilities, we take into consideration
               how each is affected to decide which APs to count.

               Thus, the 20/40 MHz usability is computed by dividing the existing
               usability value by the sum of the following:
               i)  Competing 80 MHz APs
               ii) Competing 20/40 MHz APs in only our own 40 MHz block,
               not those in the adjacent 40 MHz block under our 80 MHz
               block.
               iii)Competing APs in immediate adjacent channel outside 80
               MHz block.

               The 80 MHz usability is computed separately by dividing the
               existing usability value by the sum of the following:
               i)  Competing 80 MHz APs
               ii) Competing 20/40 MHz APs in our own 40 MHz block.
               iii)Competing 20/40 MHz APs in the adjacent 40 MHz block
               under our 80 MHz block.
               iv) Competing APs in both adjacent channels outside
               80 MHz block.

               The final usability is then combined as indicated above.
             */

            num_ap_pri40 = num_ap_sec40 = num_ap_80 = 0;
            num_ap_pri40_aci = num_ap_sec40_aci = 0;
            num_ap_tot_40ppdu_compete = num_ap_tot_80ppdu_compete = 0;
            usability_40_ppdu = usability_80_ppdu = usability;

            prepnl_msrd_usability_40_ppdu = prepnl_msrd_usability_80_ppdu =
                ICM_GET_CHANNEL_FREE_TIME(picm, chan_num);

            for (bondspan_chan_adj = adj_check_min_adj;
                    bondspan_chan_adj < adj_check_max_adj;
                    bondspan_chan_adj++) {
                /* Note that the focus of the loop is to determine
                   how suitable the current chan_num is for becoming the
                   primary 20 MHz channel for the 80 MHz block.
                   In each pass of the loop, the effect of each
                   bondspan_chan_num on that suitability is
                   determined.  */
                relchandbginfo = NULL;

                /* Check if this is a valid channel */
                bondspan_chan_idx = chan_idx + bondspan_chan_adj;
                if (bondspan_chan_idx < 0 &&  bondspan_chan_idx >= MAX_NUM_CHANNEL) {
                    continue;
                }

                bondspan_chan_num = chanlist->ch[bondspan_chan_idx].channel;

                /* Sanity check -- Check if the channel exists/needs
                   to be considered */
                if (bondspan_chan_num != (chan_num + (bondspan_chan_adj * 4))) {
                    continue;
                }

                if (bondspan_chan_num != chan_num) {
                    relchandbginfo_idx = bondspan_chan_adj - adj_check_min_adj;
                    relchandbginfo =
                        &(decdbginfo[chan_idx].relchaninfo[relchandbginfo_idx]);
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_valid, true);
                }

                if (((bondspan_chan_adj == adj_check_min_adj) ||
                            (bondspan_chan_adj == (adj_check_max_adj - 1)))&&
                        relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->chan_num,
                            bondspan_chan_num);
                    ICM_SELDBG_SETFIELD(relchandbginfo->freq,
                            chanlist->ch[bondspan_chan_idx].freq);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_incapable);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_excluded);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_pri20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec40);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor_thresh);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->maxregpower);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_etsi_weather);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_cw_intf);
                    /* Note: MWO has already been marked NA by
                       icm_init_seldbg_decinfo() since this is band dependent
                       rather than channel dependent. */
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_fhss_intf);
                    ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                            ICM_CHAN_RELATIONSHIP_ADJ);
                }

                if (bondspan_chan_num != chan_num) {
                    if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->num_obss_aps,
                                chanlist->ch[bondspan_chan_idx].num_wnw);
                    }
                } else {
                    ICM_SELDBG_SETFIELD(prichandbginfo->num_obss_aps,
                            chanlist->ch[bondspan_chan_idx].num_wnw);
                }

                /* XXX: Increment num_ap_80 once scan support is in. */

                if (bondspan_chan_adj == adj_check_min_adj)
                {
                    /* ACI handling */
                    if ((chan_idx - bondspan_chan_idx) <= 2) {
                        /* We are in the 40 MHz block affected by this
                           out of bonding adjacent channel, and our block
                           is the 'Left Hand Side' block. */
                        num_ap_pri40_aci += chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else {
                        /* The other 40 MHz block is affected */
                        num_ap_sec40_aci += chanlist->ch[bondspan_chan_idx].num_wnw;
                    }
                } else if (bondspan_chan_adj == (adj_check_max_adj - 1)) {
                    /* ACI handling */
                    if ((bondspan_chan_idx - chan_idx) <= 2) {
                        /* We are in the 40 MHz block affected by this
                           out of bonding adjacent channel, and our block
                           is the 'Right Hand Side' block. */
                        num_ap_pri40_aci += chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else {
                        /* The other 40 MHz block is affected */
                        num_ap_sec40_aci += chanlist->ch[bondspan_chan_idx].num_wnw;
                    }
                } else if (bondspan_chan_num == chan_num) {
                    /* Competing AP in our 40 MHz block */
                    num_ap_pri40 += chanlist->ch[bondspan_chan_idx].num_wnw;
                } else if (((picm->phy_spec == ICM_PHY_SPEC_11AC) &&
                                IEEE80211_IS_CHAN_11AC_VHT40PLUS(ch)) ||
                           ((picm->phy_spec == ICM_PHY_SPEC_11AXA) &&
                                IEEE80211_IS_CHAN_11AXA_HE40PLUS(ch))) {
                    if (bondspan_chan_num == chan_num + 4) {
                        /* bondspan_chan_num is our potential secondary 20 MHz
                           channel in our 40 MHz block */
                        num_ap_pri40 += chanlist->ch[bondspan_chan_idx].num_wnw;

                        prepnl_msrd_usability_40_ppdu =
                            MIN(prepnl_msrd_usability_40_ppdu,
                                    ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                        prepnl_msrd_usability_80_ppdu =
                            MIN(prepnl_msrd_usability_80_ppdu,
                                    ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                    } else {
                        /* Given eliminations in previous if checks, this is
                           in the other 40 MHz block */
                        num_ap_sec40 += chanlist->ch[bondspan_chan_idx].num_wnw;

                        prepnl_msrd_usability_80_ppdu =
                            MIN(prepnl_msrd_usability_80_ppdu,
                                    ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                    }
                } else if (((picm->phy_spec == ICM_PHY_SPEC_11AC) &&
                                IEEE80211_IS_CHAN_11AC_VHT40MINUS(ch)) ||
                           ((picm->phy_spec == ICM_PHY_SPEC_11AXA) &&
                                IEEE80211_IS_CHAN_11AXA_HE40MINUS(ch))) {
                    if (bondspan_chan_num == chan_num - 4) {
                        /* bondspan_chan_num is our potential secondary 20 MHz
                           in our 40 MHz block */
                        num_ap_pri40 += chanlist->ch[bondspan_chan_idx].num_wnw;

                        prepnl_msrd_usability_40_ppdu =
                            MIN(prepnl_msrd_usability_40_ppdu,
                                    ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                        prepnl_msrd_usability_80_ppdu =
                            MIN(prepnl_msrd_usability_80_ppdu,
                                    ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                    } else {
                        /* Given eliminations in previous if checks, this is
                           in the other 40 MHz block */
                        num_ap_sec40 += chanlist->ch[bondspan_chan_idx].num_wnw;

                        prepnl_msrd_usability_80_ppdu =
                            MIN(prepnl_msrd_usability_80_ppdu,
                                    ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                    }
                }

                if ((bondspan_chan_adj != adj_check_min_adj) &&
                    (bondspan_chan_adj != (adj_check_max_adj - 1))) {
                    /* Increment SR counts for bondspan channels that fall in
                     * the operating range (but not adjacent channels outside
                     * it).
                     * 11AX TODO: If later data suggests significantly enhanced
                     * ACI effects due to SR, then revisit.
                     */
                    num_ap_srp_supported +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_srp_supported;
                    num_ap_client_srp_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_srp_allowed;
                    num_ap_client_obsspd_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_obsspd_allowed;
                }

            }

            num_ap_tot_40ppdu_compete = (num_ap_pri40 + num_ap_pri40_aci + \
                    num_ap_80);
            num_ap_tot_80ppdu_compete = (num_ap_pri40 + num_ap_pri40_aci + \
                    num_ap_80 + num_ap_sec40 +        \
                    num_ap_pri40_aci);

            usability_40_ppdu /= (num_ap_tot_40ppdu_compete + 1);
            usability_80_ppdu /= (num_ap_tot_80ppdu_compete + 1);

            usability = (u_int16_t)((((u_int32_t)usability_80_ppdu) *       \
                        (picm->usage_factor_80_bw80)/100) + \
                    (((u_int32_t)usability_40_ppdu) *       \
                     (picm->usage_factor_80_bw40)/100));

            if ((num_ap_srp_supported != 0) ||
                    (num_ap_client_srp_allowed != 0) ||
                    (num_ap_client_obsspd_allowed != 0))
            {
                icm_penalize_11ax_srp(chan_num, 0, &usability,
                        num_ap_srp_supported, num_ap_client_srp_allowed,
                        num_ap_client_obsspd_allowed);
            }

            prepnl_msrd_usability = (u_int16_t)((((u_int32_t)prepnl_msrd_usability_80_ppdu) * \
                        (picm->usage_factor_80_bw80)/100) +       \
                    (((u_int32_t)prepnl_msrd_usability_40_ppdu) *\
                     (picm->usage_factor_80_bw40)/100));

            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps_80w_40ppdu,
                    num_ap_tot_40ppdu_compete + 1);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps_80w_80ppdu,
                    num_ap_tot_80ppdu_compete + 1);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usage_factor_80_bw40,
                    picm->usage_factor_80_bw40);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usage_factor_80_bw80,
                    picm->usage_factor_80_bw80);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability_40ppdu,
                    usability_40_ppdu);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability_80ppdu,
                    usability_80_ppdu);
        } else if (ch_bw == ICM_CH_BW_160) {
            /* The usability is computed based on
               a) potential 20/40 MHz PPDU Tx requirement
               AND
               b) potential 80 MHz PPDU Tx requirement
               AND
               c) potential 160 MHz PPDU Tx requirement

               |--Independent--|---------------|--Independent--|--Independent--|
               20/40 MHz Tx                    20/40 MHz Tx    20/40 MHz Tx

               |-----Independent 80 MHz Tx-----|-----Independent 80 MHz Tx-----|

               |-----------------------160 MHz span Tx-------------------------|


               The 20/40 MHz PPDU Tx requirement is affected by
               i)  Competing 160 MHz PPDU Tx.
               ii) Competing 80 MHz PPDU Tx in the same 80 MHz segment as ours,
               or 80+80 MHz PPDU Tx with our 80 MHz segment as the secondary
               80 MHz.
               iii)Competing 20/40 MHz PPDU Tx in our own 40 MHz block. We treat
               20 and 40 MHz under the same granularity.
               iv) Transmissions in immediate adjacent channel outside 160 MHz
               block due to ACI, if we are at the edge. (XXX: To be
               quantified further).

               The 80 MHz PPDU Tx requirement is affected by
               i)  Competing 160 MHz PPDU Tx.
               ii) Competing 80 MHz PPDU Tx in the same 80 MHz segment as ours,
               or 80+80 MHz PPDU Tx with our 80 MHz segment as the secondary
               80 MHz.
               iii)Competing 20/40 MHz PPDU Tx in the same 80 MHz segment as
               ours.
               iv) Transmissions in adjacent channel outside 160 MHz block due
               to ACI.

               The 160 MHz PPDU Tx requirement is affected by
               i)  Competing 160 MHz PPDU Tx.
               ii) Competing 80/80+80 MHz PPDU Tx in either segment.
               iii)Competing 20/40 MHz PPDU Tx in our own primary 40 MHz block,
               or 20/40 MHz PPDU Tx anywhere in the secondary 80 MHz segment.
               iv) Transmissions in both adjacent channels outside 160 MHz
               block due to ACI.

               We compute three separate usabilities for each requirement,
               and then combine the usabilities in a configurable ratio.
               While computing the three usabilities, we take into consideration
               how each is affected to decide which APs to count.

               Thus, the 20/40 MHz usability is computed by dividing the existing
               usability value by the sum of the following:
               i)  Competing 160 MHz APs.
               ii) Competing 80 MHz APs in the same 80 MHz segment as ours,
               or 80+80 MHz APs with our 80 MHz segment as the secondary
               80 MHz.
               iii)Competing 20/40 MHz APs in our own 40 MHz block.
               iv) APs in immediate adjacent channel outside 160 MHz
               block due to ACI, if we are at the edge.

               The 80 MHz usability is computed separately by dividing the
               existing usability value by the sum of the following:
               i)  Competing 160 MHz APs.
               ii) Competing 80 MHz APs in the same 80 MHz segment as ours,
               or 80+80 MHz APs with our 80 MHz segment as the secondary
               80 MHz.
               iii)Competing 20/40 MHz APs in the same 80 MHz segment as
               ours.
               iv) APs in adjacent channel outside 160 MHz block due
               to ACI.

               The 160 MHz usability is computed separately by dividing the
            existing usability value by the sum of the following:
                i)  Competing 160 MHz APs.
                ii) Competing 80/80+80 MHz APs in either segment.
                iii)Competing 20/40 MHz APs in our own primary 40 MHz block,
                or 20/40 MHz APs anywhere in the secondary 80 MHz segment.
                    iv) APs in both adjacent channels outside 160 MHz
                    block due to ACI.

                    The final usability is then combined as indicated above.

                    XXX Finer grained measurement of OBSS impact can be considered if
                    required in the future.
                    */

                    num_ap_pri40 = num_ap_pri80 = num_ap_sec80 = num_ap_160 = 0;
            num_ap_pri40_aci = num_ap_pri80_aci = num_ap_sec80_aci = 0;
            num_ap_tot_40ppdu_compete = num_ap_tot_80ppdu_compete =
                num_ap_tot_160ppdu_compete = 0;
            usability_40_ppdu = usability_80_ppdu = usability_160_ppdu =
                usability;
            skip_channel = 0;
            sec80_80p80_increment_done = 0;

            prepnl_msrd_usability_40_ppdu =
                prepnl_msrd_usability_80_ppdu =
                prepnl_msrd_usability_160_ppdu =
                ICM_GET_CHANNEL_FREE_TIME(picm, chan_num);

            for (bondspan_chan_adj = adj_check_min_adj;
                    bondspan_chan_adj < adj_check_max_adj;
                    bondspan_chan_adj++) {
                /* Note that the focus of the loop is to determine
                   how suitable the current chan_num is for becoming the
                   primary 20 MHz channel for the 160 MHz block.
                   In each pass of the loop, the effect of each
                   bondspan_chan_num on that suitability is
                   determined.  */
                relchandbginfo = NULL;

                /* Check if this is a valid channel */
                bondspan_chan_idx = chan_idx + bondspan_chan_adj;
                if (bondspan_chan_idx < 0 &&  bondspan_chan_idx >= MAX_NUM_CHANNEL) {
                    continue;
                }

                bondspan_chan_num = chanlist->ch[bondspan_chan_idx].channel;

                /* Sanity check -- Check if the channel exists/needs
                   to be considered */
                if (bondspan_chan_num != (chan_num + (bondspan_chan_adj * 4))) {
                    continue;
                }

                if (bondspan_chan_num != chan_num) {
                    relchandbginfo_idx = bondspan_chan_adj - adj_check_min_adj;
                    relchandbginfo =
                        &(decdbginfo[chan_idx].relchaninfo[relchandbginfo_idx]);
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_valid, true);
                }

                if (((bondspan_chan_adj == adj_check_min_adj) ||
                            (bondspan_chan_adj == (adj_check_max_adj - 1)))&&
                        relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->chan_num,
                            bondspan_chan_num);
                    ICM_SELDBG_SETFIELD(relchandbginfo->freq,
                            chanlist->ch[bondspan_chan_idx].freq);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_incapable);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_excluded);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_pri20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec40);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor_thresh);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->maxregpower);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_etsi_weather);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_cw_intf);
                    /* Note: MWO has already been marked NA by
                       icm_init_seldbg_decinfo() since this is band dependent
                       rather than channel dependent. */
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_fhss_intf);
                    ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                            ICM_CHAN_RELATIONSHIP_ADJ);
                }

                if (bondspan_chan_num != chan_num) {
                    if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->num_obss_aps,
                                chanlist->ch[bondspan_chan_idx].num_wnw);
                    }
                } else {
                    ICM_SELDBG_SETFIELD(prichandbginfo->num_obss_aps,
                            chanlist->ch[bondspan_chan_idx].num_wnw);
                }

                if (bondspan_chan_adj == adj_check_min_adj)
                {
                    /* ACI handling */
                    if ((chan_idx - bondspan_chan_idx) <= 2) {
                        /* We are in the 40 MHz block affected by this
                           out of bonding adjacent channel */
                        num_ap_pri40_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else if ((chan_idx - bondspan_chan_idx) <= 4){
                        num_ap_pri80_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else {
                        num_ap_sec80_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    }
                } else if (bondspan_chan_adj == (adj_check_max_adj - 1)) {
                    /* ACI handling */
                    if ((bondspan_chan_idx - chan_idx) <= 2) {
                        /* We are in the 40 MHz block affected by this
                           out of bonding adjacent channel */
                        num_ap_pri40_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else if ((bondspan_chan_idx - chan_idx) <= 4){
                        num_ap_pri80_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else {
                        num_ap_sec80_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    }
                } else if (bondspan_chan_num == chan_num) {
                    num_ap_pri40 += (chanlist->ch[bondspan_chan_idx].num_wnw_20 +
                            chanlist->ch[bondspan_chan_idx].num_wnw_40);

                    num_ap_pri80 += (chanlist->ch[bondspan_chan_idx].num_wnw_80 +
                            chanlist->ch[bondspan_chan_idx].used_as_80p80_secondary_80);

                    num_ap_160 +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_160;
                } else if ((((picm->phy_spec == ICM_PHY_SPEC_11AC) &&
                                IEEE80211_IS_CHAN_11AC_VHT40PLUS(ch)) ||
                            ((picm->phy_spec == ICM_PHY_SPEC_11AXA) &&
                                IEEE80211_IS_CHAN_11AXA_HE40PLUS(ch))) &&
                           (bondspan_chan_num == chan_num + 4)) {
                    /* bondspan_chan_num is our potential secondary 20 MHz
                       channel in our 40 MHz block */
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }

                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_SEC20_OCC);

                        skip_channel = 1;
                        break;
                    }

                    prepnl_msrd_usability_40_ppdu =
                        MIN(prepnl_msrd_usability_40_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                    prepnl_msrd_usability_80_ppdu =
                        MIN(prepnl_msrd_usability_80_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                    prepnl_msrd_usability_160_ppdu =
                        MIN(prepnl_msrd_usability_160_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                } else if ((((picm->phy_spec == ICM_PHY_SPEC_11AC) &&
                                IEEE80211_IS_CHAN_11AC_VHT40MINUS(ch)) ||
                            ((picm->phy_spec == ICM_PHY_SPEC_11AXA) &&
                                IEEE80211_IS_CHAN_11AXA_HE40MINUS(ch))) &&
                           (bondspan_chan_num == chan_num - 4)) {
                    /* bondspan_chan_num is our potential secondary 20 MHz
                       channel in our 40 MHz block */
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }

                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_SEC20_OCC);

                        skip_channel = 1;
                        break;
                    }

                    prepnl_msrd_usability_40_ppdu =
                        MIN(prepnl_msrd_usability_40_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                    prepnl_msrd_usability_80_ppdu =
                        MIN(prepnl_msrd_usability_80_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                    prepnl_msrd_usability_160_ppdu =
                        MIN(prepnl_msrd_usability_160_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                } else if (((chan_num < ch->ic_vhtop_ch_freq_seg1_160) &&
                            (bondspan_chan_num <
                             ch->ic_vhtop_ch_freq_seg1_160)) ||
                        ((chan_num > ch->ic_vhtop_ch_freq_seg1_160) &&
                         (bondspan_chan_num >
                          ch->ic_vhtop_ch_freq_seg1_160)))  {
                    /* bondspan_chan_num is in our secondary 40 MHz block. */
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }

                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_SEC40_OCC);

                        skip_channel = 1;
                        break;
                    }

                    prepnl_msrd_usability_80_ppdu =
                        MIN(prepnl_msrd_usability_80_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                    prepnl_msrd_usability_160_ppdu =
                        MIN(prepnl_msrd_usability_160_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                } else {
                    /* bondspan_chan_num is in our secondary 80 MHz block.
                       The selection of the bonding span channel range
                       ensures that the assumption in this else block holds.
                     */
                    num_ap_sec80 +=
                        (chanlist->ch[bondspan_chan_idx].num_wnw_20 +
                         chanlist->ch[bondspan_chan_idx].num_wnw_40 +
                         chanlist->ch[bondspan_chan_idx].num_wnw_80);

                    if (!sec80_80p80_increment_done) {
                        /* Increment number of 80+80 MHz APs having their
                         * secondary 80 MHz on our secondary 80, only once
                         * (since this count would be incremented for every
                         * bonding channel in our secondary 80, for the same set
                         * of APs).
                         */
                        num_ap_sec80 +=
                            chanlist->ch[bondspan_chan_idx].used_as_80p80_secondary_80;
                        sec80_80p80_increment_done = 1;
                    }

                    num_ap_160 +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_160;

                    prepnl_msrd_usability_160_ppdu =
                        MIN(prepnl_msrd_usability_160_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                }

                if ((bondspan_chan_adj != adj_check_min_adj) &&
                    (bondspan_chan_adj != (adj_check_max_adj - 1))) {
                    /* Increment SR counts for bondspan channels that fall in
                     * the operating range (but not adjacent channels outside
                     * it).
                     * 11AX TODO: If later data suggests significantly enhanced
                     * ACI effects due to SR, then revisit.
                     */
                    num_ap_srp_supported +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_srp_supported;
                    num_ap_client_srp_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_srp_allowed;
                    num_ap_client_obsspd_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_obsspd_allowed;
                }
            }

            if (skip_channel) {
                usability = 0;
                ICM_SET_CHANNEL_USABLITY(picm, chan_num, usability);
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, usability);
                continue;
            }

            num_ap_tot_40ppdu_compete = (num_ap_pri40 + num_ap_pri40_aci + \
                    num_ap_pri80 + num_ap_160);
            num_ap_tot_80ppdu_compete = (num_ap_pri40 + num_ap_pri40_aci + \
                    num_ap_pri80_aci + num_ap_pri80 +   \
                    num_ap_160);
            num_ap_tot_160ppdu_compete = (num_ap_pri40 + num_ap_pri40_aci + \
                    num_ap_pri80_aci + num_ap_sec80_aci +\
                    num_ap_pri80 + num_ap_sec80 + \
                    num_ap_160);

            usability_40_ppdu /= (num_ap_tot_40ppdu_compete + 1);
            usability_80_ppdu /= (num_ap_tot_80ppdu_compete + 1);
            usability_160_ppdu /= (num_ap_tot_160ppdu_compete + 1);

            usability = (u_int16_t)((((u_int32_t)usability_160_ppdu) *       \
                        (picm->usage_factor_160_bw160)/100) + \
                    (((u_int32_t)usability_80_ppdu) *       \
                     (picm->usage_factor_160_bw80)/100) +\
                    (((u_int32_t)usability_40_ppdu) *       \
                     (picm->usage_factor_160_bw40)/100));

            if ((num_ap_srp_supported != 0) ||
                    (num_ap_client_srp_allowed != 0) ||
                    (num_ap_client_obsspd_allowed != 0))
            {
                icm_penalize_11ax_srp(chan_num, 0, &usability,
                        num_ap_srp_supported, num_ap_client_srp_allowed,
                        num_ap_client_obsspd_allowed);
            }

            prepnl_msrd_usability = (u_int16_t)((((u_int32_t)prepnl_msrd_usability_160_ppdu) * \
                        (picm->usage_factor_160_bw160)/100) + \
                    (((u_int32_t)prepnl_msrd_usability_80_ppdu) * \
                     (picm->usage_factor_160_bw80)/100) +\
                    (((u_int32_t)prepnl_msrd_usability_40_ppdu) * \
                     (picm->usage_factor_160_bw40)/100));

            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps_160w_40ppdu,
                    num_ap_tot_40ppdu_compete + 1);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps_160w_80ppdu,
                    num_ap_tot_80ppdu_compete + 1);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps_160w_160ppdu,
                    num_ap_tot_160ppdu_compete + 1);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usage_factor_160_bw40,
                    picm->usage_factor_160_bw40);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usage_factor_160_bw80,
                    picm->usage_factor_160_bw80);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usage_factor_160_bw160,
                    picm->usage_factor_160_bw160);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability_40ppdu,
                    usability_40_ppdu);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability_80ppdu,
                    usability_80_ppdu);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability_160ppdu,
                    usability_160_ppdu);
        } else if ((ch_bw == ICM_CH_BW_80_PLUS_80) && (seg_id == 0)) {
            /* Processing of selection of primary 80 MHz segment */

            /* The usability is computed based on
               a) potential 20/40 MHz PPDU Tx requirement
               AND
               b) potential 80 MHz PPDU Tx requirement

            Note: Potential 80+80 MHz PPDU Tx would be a function of the
            usabilities of each individual 80 MHz block. We do not consider
            the various combinations of 80+80, and focus instead on finding
            individual 80 MHz blocks with maximum usability at the 80 MHz
            level. The approach of considering combinations is not currently
            expected to yield significant benefits for the complexity
            involved.


            |--Independent--|------------|  ~ |-Independent-|--Independent--|
            20/40 MHz Tx                    20/40 MHz Tx    20/40 MHz Tx

            |-----Independent 80 MHz Tx--|  ~ |----Independent 80 MHz Tx----|

            |----------------------------|  ~ |-----------------------------|
            80+80 MHz Tx


            The 20/40 MHz PPDU Tx requirement is affected by
            i)  Competing 160 MHz PPDU Tx.
            ii) Competing 80 MHz PPDU Tx in the same 80 MHz segment as ours,
            or 80+80 MHz PPDU Tx with our 80 MHz segment as the secondary
            80 MHz.
            iii)Competing 20/40 MHz PPDU Tx in our own 40 MHz block. We treat
            20 and 40 MHz under the same granularity.
            iv) Transmissions in immediate adjacent channel outside primary
            80 MHz block due to ACI, if we are at the edge. (XXX: To be
            quantified further).

            The 80 MHz PPDU Tx requirement is affected by
            i)  Competing 160 MHz PPDU Tx.
            ii) Competing 80 MHz PPDU Tx in the same 80 MHz segment as ours,
            or 80+80 MHz PPDU Tx with our 80 MHz segment as the secondary
            80 MHz.
            iii)Competing 20/40 MHz PPDU Tx in the same 80 MHz segment as
            ours.
            iv) Transmissions in adjacent channel outside primary 80 MHz block
            due to ACI.

            We compute two separate usabilities for each requirement,
            and then combine the usabilities in a configurable ratio.
            While computing the two usabilities, we take into consideration
            how each is affected to decide which APs to count.

            Thus, the 20/40 MHz usability is computed by dividing the existing
            usability value by the sum of the following:
            i)  Competing 160 MHz APs.
            ii) Competing 80 MHz APs in the same 80 MHz segment as ours,
            or 80+80 MHz APs with our 80 MHz segment as the secondary
            80 MHz.
            iii)Competing 20/40 MHz APs in our own 40 MHz block.
            iv) APs in immediate adjacent channel outside primary 80 MHz
            block due to ACI, if we are at the edge.

            The 80 MHz usability is computed separately by dividing the
            existing usability value by the sum of the following:
            i)  Competing 160 MHz APs.
            ii) Competing 80 MHz APs in the same 80 MHz segment as ours,
            or 80+80 MHz APs with our 80 MHz segment as the secondary
            80 MHz.
            iii)Competing 20/40 MHz APs in the same 80 MHz segment as
            ours.
            iv) APs in adjacent channel outside primary 80 MHz block due
            to ACI.

            The final usability is then combined as indicated above.

            XXX Finer grained measurement of OBSS impact can be considered if
                required in the future.
                */

                num_ap_pri40 = num_ap_pri80 = num_ap_sec80 = num_ap_160 = 0;
            num_ap_pri40_aci = num_ap_pri80_aci = 0;
            num_ap_tot_40ppdu_compete = num_ap_tot_80ppdu_compete = 0;
            usability_40_ppdu = usability_80_ppdu = usability;
            skip_channel = 0;

            prepnl_msrd_usability_40_ppdu = prepnl_msrd_usability_80_ppdu =
                ICM_GET_CHANNEL_FREE_TIME(picm, chan_num);

            for (bondspan_chan_adj = adj_check_min_adj;
                    bondspan_chan_adj < adj_check_max_adj;
                    bondspan_chan_adj++) {
                /* Note that the focus of the loop is to determine
                   how suitable the current chan_num is for becoming the
                   primary 20 MHz channel for the 80+80 MHz block.
                   In each pass of the loop, the effect of each
                   bondspan_chan_num on that suitability is
                   determined.  */
                relchandbginfo = NULL;

                /* Check if this is a valid channel */
                bondspan_chan_idx = chan_idx + bondspan_chan_adj;
                if (bondspan_chan_idx < 0 &&  bondspan_chan_idx >= MAX_NUM_CHANNEL) {
                    continue;
                }

                bondspan_chan_num = chanlist->ch[bondspan_chan_idx].channel;

                /* Sanity check -- Check if the channel exists/needs
                   to be considered */
                if (bondspan_chan_num != (chan_num + (bondspan_chan_adj * 4))) {
                    continue;
                }

                if (bondspan_chan_num != chan_num) {
                    relchandbginfo_idx = bondspan_chan_adj - adj_check_min_adj;
                    relchandbginfo =
                        &(decdbginfo[chan_idx].relchaninfo[relchandbginfo_idx]);
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_valid, true);
                }

                if (((bondspan_chan_adj == adj_check_min_adj) ||
                            (bondspan_chan_adj == (adj_check_max_adj - 1)))&&
                        relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->chan_num,
                            bondspan_chan_num);
                    ICM_SELDBG_SETFIELD(relchandbginfo->freq,
                            chanlist->ch[bondspan_chan_idx].freq);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_incapable);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_excluded);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_pri20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec20);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_obss_sec40);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->noise_floor_thresh);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->maxregpower);
                    ICM_SELDBG_MARKFIELD_NA(relchandbginfo->is_etsi_weather);
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_cw_intf);
                    /* Note: MWO has already been marked NA by
                       icm_init_seldbg_decinfo() since this is band dependent
                       rather than channel dependent. */
                    ICM_SELDBG_MARKFIELD_NR(relchandbginfo->is_fhss_intf);
                    ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                            ICM_CHAN_RELATIONSHIP_ADJ);
                }

                if (bondspan_chan_num != chan_num) {
                    if (relchandbginfo != NULL) {
                        ICM_SELDBG_SETFIELD(relchandbginfo->num_obss_aps,
                                chanlist->ch[bondspan_chan_idx].num_wnw);
                    }
                } else {
                    ICM_SELDBG_SETFIELD(prichandbginfo->num_obss_aps,
                            chanlist->ch[bondspan_chan_idx].num_wnw);
                }

                if (bondspan_chan_adj == adj_check_min_adj)
                {
                    /* ACI handling */
                    if ((chan_idx - bondspan_chan_idx) <= 2) {
                        /* We are in the 40 MHz block affected by this
                           out of bonding adjacent channel */
                        num_ap_pri40_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else if ((chan_idx - bondspan_chan_idx) <= 4){
                        num_ap_pri80_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    }
                } else if (bondspan_chan_adj == (adj_check_max_adj - 1)) {
                    /* ACI handling */
                    if ((bondspan_chan_idx - chan_idx) <= 2) {
                        /* We are in the 40 MHz block affected by this
                           out of bonding adjacent channel */
                        num_ap_pri40_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    } else if ((bondspan_chan_idx - chan_idx) <= 4){
                        num_ap_pri80_aci +=
                            chanlist->ch[bondspan_chan_idx].num_wnw;
                    }
                } else if (bondspan_chan_num == chan_num) {
                    num_ap_pri40 +=
                        (chanlist->ch[bondspan_chan_idx].num_wnw_20 +
                         chanlist->ch[bondspan_chan_idx].num_wnw_40);

                    num_ap_pri80 +=
                        (chanlist->ch[bondspan_chan_idx].num_wnw_80 +
                         chanlist->ch[bondspan_chan_idx].used_as_80p80_secondary_80 +
                         chanlist->ch[bondspan_chan_idx].used_as_160_secondary_80);

                    num_ap_160 +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_160;
                } else if ((((picm->phy_spec == ICM_PHY_SPEC_11AC) &&
                                IEEE80211_IS_CHAN_11AC_VHT40PLUS(ch)) ||
                            ((picm->phy_spec == ICM_PHY_SPEC_11AXA) &&
                                IEEE80211_IS_CHAN_11AXA_HE40PLUS(ch))) &&
                           (bondspan_chan_num == chan_num + 4)) {
                    /* bondspan_chan_num is our potential secondary 20 MHz
                       channel in our 40 MHz block */
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }

                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_SEC20_OCC);

                        skip_channel = 1;
                        break;
                    }

                    prepnl_msrd_usability_40_ppdu =
                        MIN(prepnl_msrd_usability_40_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                    prepnl_msrd_usability_80_ppdu =
                        MIN(prepnl_msrd_usability_80_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                } else if ((((picm->phy_spec == ICM_PHY_SPEC_11AC) &&
                                IEEE80211_IS_CHAN_11AC_VHT40MINUS(ch)) ||
                            ((picm->phy_spec == ICM_PHY_SPEC_11AXA) &&
                                IEEE80211_IS_CHAN_11AXA_HE40MINUS(ch))) &&
                           (bondspan_chan_num == chan_num - 4)) {
                    /* bondspan_chan_num is our potential secondary 20 MHz
                       channel in our 40 MHz block */
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }

                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_SEC20_OCC);

                        skip_channel = 1;
                        break;
                    }

                    prepnl_msrd_usability_40_ppdu =
                        MIN(prepnl_msrd_usability_40_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));

                    prepnl_msrd_usability_80_ppdu =
                        MIN(prepnl_msrd_usability_80_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                } else {
                    /* bondspan_chan_num is in our secondary 40 MHz block. */
                    if (chanlist->ch[bondspan_chan_idx].num_wnw) {
                        if (relchandbginfo != NULL) {
                            ICM_SELDBG_SETFIELD(relchandbginfo->is_obss_pri20,
                                    true);
                        }

                        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].rejreason,
                                ICM_SELDBG_REJCODE_SEC40_OCC);

                        skip_channel = 1;
                        break;
                    }

                    prepnl_msrd_usability_80_ppdu =
                        MIN(prepnl_msrd_usability_80_ppdu,
                                ICM_GET_CHANNEL_FREE_TIME(picm, bondspan_chan_num));
                }

                if ((bondspan_chan_adj != adj_check_min_adj) &&
                    (bondspan_chan_adj != (adj_check_max_adj - 1))) {
                    /* Increment SR counts for bondspan channels that fall in
                     * the operating range (but not adjacent channels outside
                     * it).
                     * 11AX TODO: If later data suggests significantly enhanced
                     * ACI effects due to SR, then revisit.
                     */
                    num_ap_srp_supported +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_srp_supported;
                    num_ap_client_srp_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_srp_allowed;
                    num_ap_client_obsspd_allowed +=
                        chanlist->ch[bondspan_chan_idx].num_wnw_client_obsspd_allowed;
                }
            }

            if (skip_channel) {
                usability = 0;
                ICM_SET_CHANNEL_USABLITY(picm, chan_num, usability);
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, usability);
                continue;
            }

            num_ap_tot_40ppdu_compete = (num_ap_pri40 + num_ap_pri40_aci + \
                    num_ap_pri80 + num_ap_160);
            num_ap_tot_80ppdu_compete = (num_ap_pri40 + num_ap_pri40_aci + \
                    num_ap_pri80_aci + num_ap_pri80 +   \
                    num_ap_160);

            usability_40_ppdu /= (num_ap_tot_40ppdu_compete + 1);
            usability_80_ppdu /= (num_ap_tot_80ppdu_compete + 1);

            usability = (u_int16_t)((((u_int32_t)usability_80_ppdu) *          \
                        (picm->usage_factor_80p80_bw80)/100) +  \
                    (((u_int32_t)usability_40_ppdu) *          \
                     (picm->usage_factor_80p80_bw40)/100));

            if ((num_ap_srp_supported != 0) ||
                    (num_ap_client_srp_allowed != 0) ||
                    (num_ap_client_obsspd_allowed != 0))
            {
                icm_penalize_11ax_srp(chan_num, 0, &usability,
                        num_ap_srp_supported, num_ap_client_srp_allowed,
                        num_ap_client_obsspd_allowed);
            }

            prepnl_msrd_usability = (u_int16_t)((((u_int32_t) prepnl_msrd_usability_80_ppdu) * \
                        (picm->usage_factor_80p80_bw80)/100) +      \
                    (((u_int32_t) prepnl_msrd_usability_40_ppdu) * \
                     (picm->usage_factor_80p80_bw40)/100));

            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps_80p80w_40ppdu,
                    num_ap_tot_40ppdu_compete + 1);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].tot_num_aps_80p80w_80ppdu,
                    num_ap_tot_80ppdu_compete + 1);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usage_factor_80p80_bw40,
                    picm->usage_factor_80p80_bw40);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usage_factor_80p80_bw80,
                    picm->usage_factor_80p80_bw80);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability_40ppdu,
                    usability_40_ppdu);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability_80ppdu,
                    usability_80_ppdu);
        }

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].preadj_usability, usability);

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_allrelchanprocd, true);

        /* Check with the measured usability */

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].prepnl_measured_usability,
                prepnl_msrd_usability);

        adjusted_msrd_usability = prepnl_msrd_usability;

        /* XXX: For simplicity, in 5 GHz 80+80 (seg ID 0 only)/160/80/40/20 MHz,
           the penalization computation only considers baseline usability for the
           primary 20 MHz channel. This is because for a given primary 20 MHz
           channel, the inclusion and position of channels forming the operating
           span are strictly defined (except seg ID 1 in 80+80). Also, there are
           a large number of channels. Thus processing penalization for each
           channel individually is not currently foreseen to yield any practical
           gain. The primary 20 MHz baseline usability acts as the anchor for
           the entire span, and suitability of the entire span can be indicated
           by external agent by providing this baseline usability. If more
           complexity is later desired for some reason, this can be revisited.
         */
        if (ICM_GET_CHANNEL_BLUSABILITY(picm, chan_num) != MAX_USABILITY) {
            /* Factor in penalization to scale down channel load that we should
               be considering */
            adjusted_msrd_usability =
                (u_int16_t)(((u_int32_t)(adjusted_msrd_usability)) *
                        ICM_GET_CHANNEL_BLUSABILITY(picm, chan_num) /
                        MAX_USABILITY);
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_penalized, true);
        } else {
            ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_penalized, false);
        }

        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].measured_usability,
                adjusted_msrd_usability);

        if (usability > adjusted_msrd_usability) {
            usability = adjusted_msrd_usability;
        }

        ICM_SET_CHANNEL_USABLITY(picm, chan_num, usability);
        ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].usability, usability);

        /* The maximum regulatory power is set as per the STA and it can be 7dB more
         * for the AP. The maximum value allowed is 30dB
         */
        /* XXX: Check for 802.11ac related changes here */
        ICM_GET_CHANNEL_MAX_REGPWR(picm, chan_num) = MIN(30, ch->ic_maxregpower);
        ICM_SELDBG_SETFIELD(prichandbginfo->maxregpower,
                MIN(30, ch->ic_maxregpower));
    }

    /* At this point, we have computed the usability of all the channels, now to figure out the best */
    usability = 0;
    picm->num_usable_ch = 0;
    for (chan_idx = 0; chan_idx <  chanlist->count; chan_idx++) {
        ch = &chanlist->ch[chan_idx];
        chan_num = ch->channel;

        if (ICM_GET_CHANNEL_USABLITY(picm,chan_num)) {
            if (picm->num_usable_ch) {
                /* The list has been initiated */
                /* Insert the channel so that the channel is in descending order of usability */
                for (usable_chan_idx = 0; usable_chan_idx < picm->num_usable_ch; usable_chan_idx++) {
                    if (ICM_GET_CHANNEL_USABLITY(picm,chan_num) <
                            ICM_GET_CHANNEL_USABLITY(picm, picm->sort_chan_list[usable_chan_idx])) {
                        continue;
                    }
                    /* If the the usability is the same, compare the max reg power */
                    if (ICM_GET_CHANNEL_USABLITY(picm,chan_num) ==
                            ICM_GET_CHANNEL_USABLITY(picm, picm->sort_chan_list[usable_chan_idx])) {
                        if (ICM_GET_CHANNEL_MAX_REGPWR(picm, chan_num) <=
                                ICM_GET_CHANNEL_MAX_REGPWR(picm, usable_chan_idx)) {
                            continue;
                        }
                    }
                    /* Push the rest by one step down */
                    for (shift_cnt = picm->num_usable_ch; shift_cnt > usable_chan_idx; shift_cnt--) {
                        picm->sort_chan_list[shift_cnt] = picm->sort_chan_list[shift_cnt-1];
                    }
                    picm->sort_chan_list[shift_cnt] = chan_num;
                    break;
                }
                /* Check if the current channel is the worst of all */
                if (usable_chan_idx == picm->num_usable_ch) {
                    picm->sort_chan_list[picm->num_usable_ch] = chan_num;
                }
            } else {
                picm->sort_chan_list[picm->num_usable_ch] = chan_num;
            }
            picm->num_usable_ch++;
        }
    }

    best_chan = picm->num_usable_ch ? picm->sort_chan_list[0] : -1;

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NONE,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "Number of usable channels=%d. Sorted list below:\n",
            picm->num_usable_ch);

    for( usable_chan_idx = 0; usable_chan_idx < picm->num_usable_ch; usable_chan_idx++) {
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                " chan:%3d  usability:%5d  maxregpwr:%3d\n",
                picm->sort_chan_list[usable_chan_idx],
                ICM_GET_CHANNEL_USABLITY(picm,
                    picm->sort_chan_list[usable_chan_idx]),
                ICM_GET_CHANNEL_MAX_REGPWR(picm,
                    picm->sort_chan_list[usable_chan_idx]));
    }

    if (best_chan == -1) {
        /* If there is a higher level logic, return at this point */
        if (pdev->conf.server_mode) {
            goto done;
        }

        /* If we are not scanning in 20MHz mode, let the calling function look
         * again with lower bandwidths.
         */
        if (ch_bw != ICM_CH_BW_20) {
            goto done;
        }

        /* Still no best channel, check if acs_policy is set to PCL_MANDATORY.
         * If so, prceed with PCL based selection. */
        if ((picm->pcl != NULL) && (picm->pcl->len > 0) &&
            (picm->pcl->policy == QCA_WLAN_VENDOR_ATTR_EXTERNAL_ACS_POLICY_PCL_MANDATORY)) {
            icm_printf("No best channel found. Will try to get one from PCL list");
            goto done;
        }
        /* There is no good channel, just return the first un-excluded channel without CW interference */
        for (chan_idx = 0; chan_idx <  chanlist->count; chan_idx++) {
            ch = &chanlist->ch[chan_idx];
            chan_num = ch->channel;
            if (chan_num > 140) {
                /* As the frequency increases, there is dip in the nominal noisefloor value
                   due to receive sensitivity. To compensate for this, correction of -7dbm is
                   added to the nominal noisefloor value */

                nf_threshold = nf_threshold + ATH_DEFAULT_NF_CORRECTION;
            }
            if ((ICM_GET_CHANNEL_NOISEFLOOR(picm, chan_num) < nf_threshold) &&
                    (ICM_GET_CHANNEL_EXCLUDE(picm, chan_num) != TRUE)) {
                best_chan = chan_num;
                break;
            }
        }
    }

    if (best_chan == -1) {
        /* Still no best channel, return the first unexcluded one */
        for (chan_idx = 0; chan_idx <  chanlist->count; chan_idx++) {
            ch = &chanlist->ch[chan_idx];
            chan_num = ch->channel;
            if (ICM_GET_CHANNEL_EXCLUDE(picm, chan_num) != TRUE) {
                best_chan = chan_num;
                break;
            }
        }
    }

    if (best_chan == -1) {
        /* If all are excluded (!), give up. The channel set will fail,
           and the external entity will need to review its logic. */
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "All valid channels excluded. Giving up.\n");
    }

    /* Set the best channel into the corresponding debug entry */
    if (best_chan != -1) {
        for (chan_idx = 0; chan_idx < chanlist->count; chan_idx++) {
            if (best_chan == decdbginfo[chan_idx].prichaninfo.chan_num) {
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_bestchan,
                        true);
            } else {
                ICM_SELDBG_SETFIELD(decdbginfo[chan_idx].is_bestchan,
                        false);
            }
        }
    }

done:
    /* Set the current channel list index to zero */
    picm->cur_ch_idx = 0;

    if (icm_seldbg_process(pdev,
                decdbginfo,
                chanlist->count) != SUCCESS) {
        exit(EXIT_FAILURE);
    }

    free(decdbginfo);
    return best_chan;
}

/*
 * Function     : icm_compare_channels
 * Description  : Helper function to compare two channels, considering
 *                usability, and in the case of 5 GHz, max regulatory
 *                power.
 * Input params : pointer to icm , channel 1, usability for channel 1,
 *                channel 2, usability for channel 2.
 * Return       : ICM_CHAN_CMP_RESULT_T giving result of comparison
 */
ICM_CHAN_CMP_RESULT_T icm_compare_channels(ICM_INFO_T* picm,
        int channel1,
        u_int16_t usability1,
        int channel2,
        u_int16_t usability2)
{
    int maxregpwr1, maxregpwr2;

    if (picm == NULL) {
        return ICM_CHAN_CMP_RESULT_FAILURE;
    }

    if (usability1 <= 0 && usability2 <= 0) {
        return ICM_CHAN_CMP_RESULT_BOTH_BAD;
    } else if (usability1 > 0 && usability2 <= 0) {
        return ICM_CHAN_CMP_RESULT_CHAN1_BETTER;
    } else if (usability1 <= 0 && usability2 > 0) {
        return ICM_CHAN_CMP_RESULT_CHAN2_BETTER;
    } else {
        /* Both usability values are greater than 0 */

        if (usability1 > usability2) {
            return ICM_CHAN_CMP_RESULT_CHAN1_BETTER;
        } else if (usability1 < usability2) {
            return ICM_CHAN_CMP_RESULT_CHAN2_BETTER;
        } else {
            /* Both usability values are equal */
            if (picm->band == ICM_BAND_2_4G) {
                return ICM_CHAN_CMP_RESULT_BOTH_EQUAL;
            } else {
                /* For 5 GHz, consider max reg power as well */
                maxregpwr1 = ICM_GET_CHANNEL_MAX_REGPWR(picm, channel1);
                maxregpwr2 = ICM_GET_CHANNEL_MAX_REGPWR(picm, channel2);

                if (maxregpwr1 > maxregpwr2) {
                    return ICM_CHAN_CMP_RESULT_CHAN1_BETTER;
                } else if (maxregpwr1 < maxregpwr2) {
                    return ICM_CHAN_CMP_RESULT_CHAN2_BETTER;
                } else {
                    return ICM_CHAN_CMP_RESULT_BOTH_EQUAL;
                }
            }
        }
    }
}

int icm_select_home_channel(ICM_INFO_T* picm)
{
    int best_chan = -1;
    int best_cfreq2_chan = -1;
    int best_chan_40plus = -1;
    int best_chan_40minus = -1;
    u_int16_t best_usability_40plus = 0;
    u_int16_t best_usability_40minus = 0;
    char width_str[ICM_MAX_CH_BW_STR_SIZE];
    ICM_CHAN_CMP_RESULT_T chan_cmp_result;
    ICM_CH_BW_T cnddt_channel_width; /* Candidate Channel Width */
    bool is_search_active = true;    /* Whether a search for a Candidate
                                        Channel Width is active */

    ICM_DEV_INFO_T* pdev = get_pdev();

    /* The first candidate width is the one requested. */
    cnddt_channel_width = picm->channel_width;

    /* Start a loop to find an appropriate candidate width, degrading
       from higher widths to lower ones if we are in server mode */
    do {
        icm_ch_bw_to_str(cnddt_channel_width,
                width_str,
                sizeof(width_str));
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "Attempting to find best channel with candidate width %s\n",
                width_str);

        switch(cnddt_channel_width)
        {
            case ICM_CH_BW_20:
                {
                    if (picm->band == ICM_BAND_2_4G) {
                        best_chan = icm_select_best_chan_ng(picm, ICM_CH_BW_20);
                    } else {
                        best_chan = icm_select_best_chan_na(picm,
                                ICM_CH_BW_20, 0, 0);
                    }

                    if (best_chan == -1) {
                        /* Give up */
                        ICM_DPRINTF(pdev,
                                ICM_PRCTRL_FLAG_NONE,
                                ICM_DEBUG_LEVEL_DEFAULT,
                                ICM_MODULE_ID_SELECTOR,
                                "Failed to find best channel with "
                                "candidate width %s\n",
                                width_str);
                    }

                    is_search_active = false;
                }
                break;

            case ICM_CH_BW_40PLUS:
                {
                    if (picm->band == ICM_BAND_2_4G) {
                        best_chan = icm_select_best_chan_ng(picm, ICM_CH_BW_40PLUS);
                    } else {
                        best_chan = icm_select_best_chan_na(picm,
                                ICM_CH_BW_40PLUS, 0, 0);
                    }

                    if (best_chan == -1) {
                        ICM_DPRINTF(pdev,
                                ICM_PRCTRL_FLAG_NONE,
                                ICM_DEBUG_LEVEL_DEFAULT,
                                ICM_MODULE_ID_SELECTOR,
                                "Failed to find best channel with "
                                "candidate width %s\n",
                                width_str);

                        /* If there is a higher level logic, give up at this
                           point */
                        if (pdev->conf.server_mode) {
                            is_search_active = false;
                        } else {
                            /* Try with a lower width */
                            cnddt_channel_width = ICM_CH_BW_20;
                        }
                    } else {
                        is_search_active = false;
                    }
                }
                break;

            case ICM_CH_BW_40MINUS:
                {
                    if (picm->band == ICM_BAND_2_4G) {
                        best_chan = icm_select_best_chan_ng(picm, ICM_CH_BW_40MINUS);
                    } else {
                        best_chan = icm_select_best_chan_na(picm,
                                ICM_CH_BW_40MINUS, 0, 0);
                    }

                    if (best_chan == -1) {
                        ICM_DPRINTF(pdev,
                                ICM_PRCTRL_FLAG_NONE,
                                ICM_DEBUG_LEVEL_DEFAULT,
                                ICM_MODULE_ID_SELECTOR,
                                "Failed to find best channel with "
                                "candidate width %s\n",
                                width_str);

                        /* If there is a higher level logic, give up at this
                           point */
                        if (pdev->conf.server_mode) {
                            is_search_active = false;
                        } else {
                            /* Try with a lower width */
                            cnddt_channel_width = ICM_CH_BW_20;
                        }
                    } else {
                        is_search_active = false;
                    }
                }
                break;

            case ICM_CH_BW_40:
                {
                    /* Try with both ICM_CH_BW_40MINUS and ICM_CH_BW_40PLUS,
                       and choose the best channel from among the two. */

                    ICM_DPRINTF(pdev,
                            ICM_PRCTRL_FLAG_NONE,
                            ICM_DEBUG_LEVEL_DEFAULT,
                            ICM_MODULE_ID_SELECTOR,
                            "First, finding best channel for 40PLUS "
                            "width\n");

                    if (picm->band == ICM_BAND_2_4G) {
                        best_chan_40plus =
                            icm_select_best_chan_ng(picm, ICM_CH_BW_40PLUS);
                    } else {
                        best_chan_40plus =
                            icm_select_best_chan_na(picm,
                                    ICM_CH_BW_40PLUS, 0, 0);
                    }

                    if ((best_chan_40plus < 0) || (best_chan_40plus >= MAX_NUM_CHANNEL)) {
                        best_chan_40plus = 0;
                        best_usability_40plus = 0;
                    } else {
                        best_usability_40plus =
                            ICM_GET_CHANNEL_USABLITY(picm, best_chan_40plus);
                    }

                    ICM_DPRINTF(pdev,
                            ICM_PRCTRL_FLAG_NONE,
                            ICM_DEBUG_LEVEL_DEFAULT,
                            ICM_MODULE_ID_SELECTOR,
                            "Next, finding best channel for 40MINUS "
                            "width\n");

                    if (picm->band == ICM_BAND_2_4G) {
                        best_chan_40minus =
                            icm_select_best_chan_ng(picm, ICM_CH_BW_40MINUS);
                    } else {
                        best_chan_40minus =
                            icm_select_best_chan_na(picm,
                                    ICM_CH_BW_40MINUS, 0, 0);
                    }

                    if ((best_chan_40minus < 0) || (best_chan_40minus >= MAX_NUM_CHANNEL)) {
                        best_chan_40minus = 0;
                        best_usability_40minus = 0;
                    } else {
                        best_usability_40minus =
                            ICM_GET_CHANNEL_USABLITY(picm, best_chan_40minus);
                    }
                    chan_cmp_result =
                        icm_compare_channels(picm,
                                best_chan_40plus,
                                best_usability_40plus,
                                best_chan_40minus,
                                best_usability_40minus);

                    switch (chan_cmp_result)
                    {
                        case ICM_CHAN_CMP_RESULT_CHAN1_BETTER:
                            best_chan = best_chan_40plus;
                            cnddt_channel_width = ICM_CH_BW_40PLUS;

                            /* Reset usability computations for external
                               entity to use */
                            ICM_DPRINTF(pdev,
                                    ICM_PRCTRL_FLAG_NONE,
                                    ICM_DEBUG_LEVEL_DEFAULT,
                                    ICM_MODULE_ID_SELECTOR,
                                    "Resetting usability computations "
                                    "using 40PLUS width\n");

                            if (picm->band == ICM_BAND_2_4G) {
                                icm_select_best_chan_ng(picm, ICM_CH_BW_40PLUS);
                            } else {
                                icm_select_best_chan_na(picm,
                                        ICM_CH_BW_40PLUS, 0, 0);
                            }
                            is_search_active = false;
                            break;

                        case ICM_CHAN_CMP_RESULT_CHAN2_BETTER:
                            best_chan = best_chan_40minus;
                            cnddt_channel_width = ICM_CH_BW_40MINUS;
                            /* There is no need to reset usability computations,
                               since 40MINUS was the last width used in call
                               to best channel selection logic */
                            is_search_active = false;
                            break;

                        case ICM_CHAN_CMP_RESULT_BOTH_BAD:
                            /* If there is a higher level logic, give up at
                               this point */
                            if (pdev->conf.server_mode) {
                                ICM_DPRINTF(pdev,
                                        ICM_PRCTRL_FLAG_NONE,
                                        ICM_DEBUG_LEVEL_DEFAULT,
                                        ICM_MODULE_ID_SELECTOR,
                                        "Failed to find best channel with "
                                        "candidate width %s\n",
                                        width_str);
                                picm->best_channel = -1;
                                cnddt_channel_width = ICM_CH_BW_40;
                                is_search_active = false;
                            } else {
                                /* Try with 20 MHz */
                                cnddt_channel_width = ICM_CH_BW_20;
                            }
                            break;

                        case ICM_CHAN_CMP_RESULT_BOTH_EQUAL:
                            /* No firm basis was found for choosing between
                               PLUS or MINUS in this situation. Settling
                               for MINUS. */
                            ICM_DPRINTF(pdev,
                                    ICM_PRCTRL_FLAG_NONE,
                                    ICM_DEBUG_LEVEL_DEFAULT,
                                    ICM_MODULE_ID_SELECTOR,
                                    "Best channels from both 40PLUS and "
                                    "40MINUS are equally good. Selecting"
                                    " 40MINUS\n");
                            best_chan = best_chan_40minus;
                            cnddt_channel_width = ICM_CH_BW_40MINUS;
                            is_search_active = false;
                            break;

                        case ICM_CHAN_CMP_RESULT_FAILURE:
                            fprintf(stderr,
                                    "Error when trying to compare 40MHz "
                                    "channels\n");
                            best_chan = -1;
                            cnddt_channel_width = ICM_CH_BW_40;
                            is_search_active = false;
                            break;

                        default:
                            fprintf(stderr,
                                    "Unknown result when trying to "
                                    "compare 40MHz channels\n");
                            best_chan = -1;
                            cnddt_channel_width = ICM_CH_BW_40;
                            is_search_active = false;
                            break;
                    }
                }
                break;

            case ICM_CH_BW_80:
                {
                    if (picm->band == ICM_BAND_2_4G) {
                        /* We should not land here! */
                        ICM_ASSERT(0);
                    } else {
                        best_chan = icm_select_best_chan_na(picm,
                                ICM_CH_BW_80, 0, 0);
                    }

                    if (best_chan == -1) {
                        ICM_DPRINTF(pdev,
                                ICM_PRCTRL_FLAG_NONE,
                                ICM_DEBUG_LEVEL_DEFAULT,
                                ICM_MODULE_ID_SELECTOR,
                                "Failed to find best channel with "
                                "candidate width %s\n",
                                width_str);

                        /* If there is a higher level logic, give up at this
                           point */
                        if (pdev->conf.server_mode) {
                            is_search_active = false;
                        } else {
                            /* Try with a lower width */
                            cnddt_channel_width = ICM_CH_BW_40;
                        }
                    } else {
                        is_search_active = false;
                    }
                }
                break;

            case ICM_CH_BW_160:
                {
                    if (picm->band == ICM_BAND_2_4G) {
                        /* We should not land here! */
                        ICM_ASSERT(0);
                    } else {
                        best_chan = icm_select_best_chan_na(picm,
                                ICM_CH_BW_160, 0, 0);
                    }

                    if (best_chan == -1) {
                        ICM_DPRINTF(pdev,
                                ICM_PRCTRL_FLAG_NONE,
                                ICM_DEBUG_LEVEL_DEFAULT,
                                ICM_MODULE_ID_SELECTOR,
                                "Failed to find best channel with "
                                "candidate width %s\n",
                                width_str);

                        /* If there is a higher level logic, give up at this
                           point */
                        if (pdev->conf.server_mode) {
                            is_search_active = false;
                        } else {
                            /* Try with a lower width */
                            cnddt_channel_width = ICM_CH_BW_80;
                        }
                    } else {
                        is_search_active = false;
                    }
                }
                break;

            case ICM_CH_BW_80_PLUS_80:
                {
                    /* Note: While the actual 'width' of 80+80 MHz mode is
                     * 160 MHz, we process it under a separate enumeration for
                     * uniformity of application logic (considering it as
                     * a variation of the 160 MHz width, with discontinuity in
                     * the width).
                     */
                    if (picm->band == ICM_BAND_2_4G) {
                        /* We should not land here! */
                        ICM_ASSERT(0);
                    }

                    best_chan = icm_select_best_chan_na(picm,
                            ICM_CH_BW_80_PLUS_80, 0, 0);

                    if (best_chan == -1) {
                        ICM_DPRINTF(pdev,
                                ICM_PRCTRL_FLAG_NONE,
                                ICM_DEBUG_LEVEL_DEFAULT,
                                ICM_MODULE_ID_SELECTOR,
                                "Failed to find best channel with "
                                "candidate width %s\n",
                                width_str);

                        /* If there is a higher level logic, give up at this
                           point */
                        if (pdev->conf.server_mode) {
                            is_search_active = false;
                        } else {
                            /* Try with a lower width */
                            cnddt_channel_width = ICM_CH_BW_80;
                        }
                    } else {
                        best_cfreq2_chan = icm_select_best_chan_na(picm,
                                ICM_CH_BW_80_PLUS_80, 1, best_chan);

                        if (best_cfreq2_chan == -1) {
                            ICM_DPRINTF(pdev,
                                    ICM_PRCTRL_FLAG_NONE,
                                    ICM_DEBUG_LEVEL_DEFAULT,
                                    ICM_MODULE_ID_SELECTOR,
                                    "Failed to find best secondary 80 MHz "
                                    "channel index with candidate width %s"
                                    "\n",
                                    width_str);

                            /* Reset best_chan as well */
                            best_chan = -1;

                            /* If there is a higher level logic, give up at this
                               point */
                            if (pdev->conf.server_mode) {
                                is_search_active = false;
                            } else {
                                /* Try with a lower width */
                                cnddt_channel_width = ICM_CH_BW_80;
                            }
                        } else {
                            is_search_active = false;
                        }
                    }
                }
                break;

            default:
                ICM_DPRINTF(pdev,
                        ICM_PRCTRL_FLAG_NONE,
                        ICM_DEBUG_LEVEL_MAJOR,
                        ICM_MODULE_ID_SELECTOR,
                        "Unhandled candidate width %s\n",
                        width_str);
                is_search_active = false;
                best_chan = -1;
                break;
        }
    } while (is_search_active);

    picm->selected_channel_width = cnddt_channel_width;
    picm->best_channel = best_chan;
    if (cnddt_channel_width == ICM_CH_BW_80_PLUS_80) {
        picm->best_cfreq2_channel = best_cfreq2_chan;
    }

    if (best_chan != -1) {
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "Best primary channel is %d\n", best_chan);

        if (cnddt_channel_width == ICM_CH_BW_80_PLUS_80) {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NONE,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "Best secondary 80 MHz segment channel index is %d\n",
                    best_cfreq2_chan);
        }

        /* Generate a log whenever ICM overrode the channel width from what
           was indicated in the message. */
        if (picm->channel_width != picm->selected_channel_width) {
            icm_ch_bw_to_str(picm->selected_channel_width,
                    width_str,
                    sizeof(width_str));
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NONE,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "Selected channel width is %s\n", width_str);
        }
    } else {
        if (pdev->conf.server_mode) {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NONE,
                    ICM_DEBUG_LEVEL_MAJOR,
                    ICM_MODULE_ID_SELECTOR,
                    "Failed to find best channel. Higher layer logic "
                    "should take a decision.\n");
        } else {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NONE,
                    ICM_DEBUG_LEVEL_MAJOR,
                    ICM_MODULE_ID_SELECTOR,
                    "Failed all attempts to find best channel\n");
        }
    }
    return best_chan;
}

void icm_change_channel(ICM_INFO_T* picm, DCS_INT_TYPE int_type, u_int16_t dcs_enabled)
{
    ICM_DEV_INFO_T* pdev = get_pdev();

    /* Check if scan is in progress, if so return */
    if (picm->scan_in_progress) return;
    picm->scan_in_progress = 1;
    /* Check if the channel change is enabled for the given interference */
    if ((int_type == SPECTRAL_DCS_INT_CW) && (!(dcs_enabled & ATH_CAP_DCS_CWIM))) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SELECTOR, "DCS not enabled/not conveyed by driver for CW interference\n");
        picm->scan_in_progress = 0;
        return;
    }
    if ((int_type == SPECTRAL_DCS_INT_WIFI) && (!(dcs_enabled & ATH_CAP_DCS_WLANIM))) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SELECTOR, "DCS not enabled/not conveyed by driver for WLAN interference\n");
        picm->scan_in_progress = 0;
        return;
    }

    icm_scan_and_select_channel(picm, TRUE);

    picm->scan_in_progress = 0;
}

int icm_get_currchan(ICM_INFO_T* picm)
{
    return icm_wal_get_currchan(picm);
}

/*
 * Function     : icm_init_seldbg_dump
 * Description  : initialize selection debug information dump.
 * Input params : pointer to dev info structure
 * Return       : SUCCESS or FAILURE
 */
int icm_init_seldbg_dump(ICM_DEV_INFO_T* pdev)
{
    ICM_CONFIG_T* pconf = NULL;
    ICM_FILE_T *seldbgfile = NULL;

    ICM_ASSERT(pdev != NULL);

    pconf = &pdev->conf;
    ICM_ASSERT(pconf->seldbg_filename != NULL);

    seldbgfile = ICM_GET_ADDR_OF_SELDBGFILE_INFO(pdev);

    seldbgfile->file = fopen(pconf->seldbg_filename, "w");

    if (seldbgfile->file == NULL) {
        fprintf(stderr, "Could not open selection debug "
                "information file %s. Error=%d\n",
                pconf->seldbg_filename,
                errno);
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : icm_deinit_seldbg_dump
 * Description  : de-initialize selection debug information dump.
 * Input params : pointer to dev info structure
 */
void icm_deinit_seldbg_dump(ICM_DEV_INFO_T* pdev)
{
    ICM_CONFIG_T* pconf = NULL;
    ICM_FILE_T *seldbgfile = NULL;
    int ret = 0;

    ICM_ASSERT(pdev != NULL);

    pconf = &pdev->conf;

    seldbgfile = ICM_GET_ADDR_OF_SELDBGFILE_INFO(pdev);

    if (seldbgfile->file == NULL) {
        return;
    }

    ret = fclose(seldbgfile->file);

    if (ret != 0) {
        fprintf(stderr, "Could not close selection debug "
                "information file %s. Error=%d\n",
                pconf->seldbg_filename? pconf->seldbg_filename:"<Null>",
                errno);
        exit(EXIT_FAILURE);
    }

    seldbgfile->file = NULL;
    return;
}

/*
 * Function     : icm_init_seldbg_decinfo
 * Description  : Helper function for initializing array of
 *                ICM_SELDBG_DECISION_INFO_T entries with information that
 *                varies as per Band, PHY specification and width. Note that
 *                channel specific information (including channel number
 *                Max regulatory power, etc.) are not filled in, and are
 *                initialized by the main selection logic.
 *                One of the main reasons for this split is that determination
 *                of bonding span for 80 MHz happens dynamically deep inside
 *                the 11na selection logic.
 * Input params : Pointer to array of ICM_SELDBG_DECISION_INFO_T entries,
 *                Number of array members,
 *                ICM enumeration for Band,
 *                ICM enumeration for PHY Specification (currently unused),
 *                ICM enumeration for Width
 * Return       : SUCCESS/FAILURE
 */
int icm_init_seldbg_decinfo(ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize,
        ICM_BAND_T band,
        ICM_PHY_SPEC_T physpec,
        ICM_CH_BW_T width)
{
    int i = 0, j = 0;

    ICM_ASSERT(decdbginfo != NULL);

    memset(decdbginfo, 0, arraysize * sizeof(ICM_SELDBG_DECISION_INFO_T));

    for(i = 0; i < arraysize; i++) {
        ICM_SELDBG_SETFIELD(decdbginfo[i].band, band);
        ICM_SELDBG_SETFIELD(decdbginfo[i].physpec, physpec);
        ICM_SELDBG_SETFIELD(decdbginfo[i].width, width);
        ICM_SELDBG_SETFIELD(decdbginfo[i].is_allrelchanprocd,
                false);
        ICM_SELDBG_SETFIELD(decdbginfo[i].rejreason,
                ICM_SELDBG_REJCODE_NOTREJ);
        /* We know that at a minimum, there will be some amount of primary
           channel info even if the selector algo bails out on this array
           entry early on. */
        ICM_SELDBG_SETFIELD(decdbginfo[i].prichaninfo.is_valid, true);

        if (band == ICM_BAND_2_4G) {
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.is_obss_sec40);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.is_etsi_weather);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].usage_factor_80_bw40);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].usage_factor_80_bw80);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].usage_factor_160_bw40);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].usage_factor_160_bw80);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].usage_factor_160_bw160);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].usage_factor_80p80_bw40);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].usage_factor_80p80_bw80);
        } else if (band == ICM_BAND_5G) {
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.is_mwo_intf);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].mwo_degrade_factor);
            ICM_SELDBG_MARKFIELD_NU(decdbginfo[i].prichaninfo.is_fhss_intf);
            ICM_SELDBG_MARKFIELD_NU(decdbginfo[i].fhss_degrade_factor);
        }

        ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.center_chan_num);
        ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.center_freq);

        for(j = 0; j < ARRAY_LEN(decdbginfo[i].relchaninfo); j++) {
            ICM_SELDBG_SETFIELD(decdbginfo[i].relchaninfo[j].is_valid, false);

            if (band == ICM_BAND_2_4G) {
                ICM_SELDBG_MARKFIELD_NA(
                        decdbginfo[i].relchaninfo[j].is_obss_sec40);
                ICM_SELDBG_MARKFIELD_NA(
                        decdbginfo[i].relchaninfo[j].is_etsi_weather);
            } else if (band == ICM_BAND_5G) {
                ICM_SELDBG_MARKFIELD_NA(
                        decdbginfo[i].relchaninfo[j].is_mwo_intf);
                ICM_SELDBG_MARKFIELD_NU(
                        decdbginfo[i].relchaninfo[j].is_fhss_intf);
            }

            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].relchaninfo[j].center_chan_num);
            ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].relchaninfo[j].center_freq);
        }
    }

    return SUCCESS;
}

/*
 * Function     : icm_seldbg_process
 * Description  : process selection debug information.
 * Input params : pointer to dev info structure,
 *                pointer to array of ICM_SELDBG_DECISION_INFO_T entries,
 *                number of array members
 * Return       : SUCCESS or FAILURE
 */
int icm_seldbg_process(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize)
{
    ICM_CONFIG_T* conf = NULL;
    int ret = SUCCESS;

    ICM_ASSERT(pdev != NULL);
    ICM_ASSERT(decdbginfo != NULL);

    conf = &pdev->conf;

    if (conf->enable_seldbg_dump) {
        ret = icm_seldbg_filedump(pdev, decdbginfo, arraysize);
    }

    icm_seldbg_consoledump(pdev, decdbginfo, arraysize);

    return ret;
}

/*
 * Function     : icm_seldbg_filedump
 * Description  : dump selection debug information to CSV file.
 * Input params : pointer to dev info structure
 *                pointer to array of ICM_SELDBG_DECISION_INFO_T entries,
 *                number of array members
 * Return       : SUCCESS or FAILURE
 */
int icm_seldbg_filedump(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize)
{
    ICM_FILE_T *seldbgfile = NULL;
    static u_int32_t recordset_count = 0;
    u_int16_t chan_idx = 0, rel_chan_idx = 0;
    ICM_SELDBG_CHAN_INFO_T  *prichandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T  *relchandbginfo = NULL;

    ICM_ASSERT(pdev != NULL);
    ICM_ASSERT(decdbginfo != NULL);
    ICM_ASSERT(arraysize > 0);

    seldbgfile = ICM_GET_ADDR_OF_SELDBGFILE_INFO(pdev);

    if (seldbgfile->file == NULL) {
        fprintf(stderr, "Selection debug info dump file handle is NULL\n");
        return FAILURE;
    }

    recordset_count++;

    fprintf(seldbgfile->file,
            "Record Set No.,"
            "Primary chan,"
            "Primary freq,"
            "Band,"
            "PHY Spec,"
            "Width (MHz),"
            "Related chan,"
            "Related chan freq,"
            "Relation,"
            "Incapable of PHY Spec/Width?,"
            "Excluded?,"
            "OBSS APs with pri on chan?,"
            "No. of OBSS APs with pri on chan,"
            "Any OBSS APs with sec20 on chan?,"
            "Any OBSS APs with sec40 on chan?,"
            "Noise Floor (dBm),"
            "Noise Floor Threshold (dBm),"
            "Max regulatory power (dBm),"
            "ETSI Weather Radar channel?,"
            "CW Inf found?,"
            "Microwave Inf found?,"
            "FHSS Inf found?,"
            "FHSS Degrade Factor,"
            "MWO Degrade Factor,"
            "All related chans processed?,"
            "Baseline usability,"
            "Total APs (For width < 80),"
            "Total APs affecting 20/40MHz PPDUs (width=80),"
            "Total APs affecting 80MHz PPDUs (width=80),"
            "Total APs affecting 20/40MHz PPDUs (width=160),"
            "Total APs affecting 80MHz PPDUs (width=160),"
            "Total APs affecting 160MHz PPDUs (width=160),"
            "Total APs affecting 20/40MHz PPDUs (width=80+80; pri80 selection),"
            "Total APs affecting 80MHz PPDUs (width=80+80; pri80 selection),"
            "Sub-usability - 20/40MHz PPDU Tx,"
            "Sub-usability - 80MHz PPDU Tx,"
            "Sub-usability - 160MHz PPDU Tx,"
            "Usage factor - 20/40MHz PPDU Tx (width=80),"
            "Usage factor - 80MHz PPDU Tx (width=80,"
            "Usage factor - 20/40MHz PPDU Tx (width=160),"
            "Usage factor - 80MHz PPDU Tx (width=160),"
            "Usage factor - 160MHz PPDU Tx (width=160),"
            "Usage factor - 20/40MHz PPDU Tx (width=80+80; pri80 selection),"
            "Usage factor - 80MHz PPDU Tx (width=80+80; pri80 selection),"
            "Pre-adjustment computed usability,"
            "Pre-penalization measured usability,"
            "Is penalized?,"
            "Final measured usability,"
            "Pre-penalization measured usability - ext chan,"
            "Is ext chan penalized?,"
            "Final measured usability - ext chan,"
            "Pre-penalization measured usability - 80+80 Mhz sec80,"
            "Is 80+80 MHz sec80 (first chan from left) penalized?,"
            "Final measured usability - 80+80 MHz; sec80,"
            "Final computed usability,"
            "Rejection Status/Reason,"
            "Is best chan?,"
            "\n");

    for (chan_idx = 0; chan_idx < arraysize; chan_idx++) {
        prichandbginfo = &(decdbginfo[chan_idx].prichaninfo);
        /* Prints for primary channel and main decision parameters */

        /* Record Set No. */
        fprintf(seldbgfile->file, "%u,", recordset_count);

        /* Primary chan */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->chan_num,
                "%hu,");

        /* Primary freq */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->freq,
                "%.0f,");

        /* Band */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].band,
                icm_band_str);

        /* PHY Spec */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].physpec,
                icm_phy_spec_str);

        /* Width */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].width,
                icm_ch_bw_str);

        /* Related chan */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->chan_num,
                "%hu,");

        /* Related chan freq */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->freq,
                "%.0f,");

        /* Relation */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->relation,
                icm_chan_relationship_str);

        /* Incapable of PHY Spec/Width? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_incapable,
                icm_bool_str);

        /* Excluded? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_excluded,
                icm_bool_str);

        /* OBSS APs with pri on chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_obss_pri20,
                icm_bool_str);

        /* No. of OBSS APs with pri on chan */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->num_obss_aps,
                "%d,");

        /* Any OBSS APs with sec20 on chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_obss_sec20,
                icm_bool_str);

        /* Any OBSS APs with sec40 on chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_obss_sec40,
                icm_bool_str);

        /* Noise Floor (dBm) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->noise_floor,
                "%d,");

        /* Noise Floor Threshold (dBm) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->noise_floor_thresh,
                "%d,");

        /* Max regulatory power (dBm) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                prichandbginfo->maxregpower,
                "%hhd,");

        /* ETSI Weather Radar channel? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_etsi_weather,
                icm_bool_str);

        /* CW Inf found? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_cw_intf,
                icm_bool_str);

        /* Microwave Inf found? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_mwo_intf,
                icm_bool_str);

        /* FHSS Inf found? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                prichandbginfo->is_fhss_intf,
                icm_bool_str);

        /* FHSS Degrade Factor */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].fhss_degrade_factor,
                "%hhu,");

        /* MWO Degrade Factor */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].mwo_degrade_factor,
                "%hhu,");

        /* All related chans processed? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].is_allrelchanprocd,
                icm_bool_str);

        /* Baseline usability */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].baseline_usability,
                "%hu,");

        /* Total APs (For width < 80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps,
                "%d,");

        /* Total APs affecting 20/40MHz PPDUs (width=80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps_80w_40ppdu,
                "%d,");

        /* Total APs affecting 80MHz PPDUs (width=80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps_80w_80ppdu,
                "%d,");

        /* Total APs affecting 20/40MHz PPDUs (width=160) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps_160w_40ppdu,
                "%d,");

        /* Total APs affecting 80MHz PPDUs (width=160) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps_160w_80ppdu,
                "%d,");

        /* Total APs affecting 160MHz PPDUs (width=160) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps_160w_160ppdu,
                "%d,");

        /* Total APs affecting 20/40MHz PPDUs (width=80+80, for pri80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps_80p80w_40ppdu,
                "%d,");

        /* Total APs affecting 80MHz PPDUs (width=80+80, for pri80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].tot_num_aps_80p80w_80ppdu,
                "%d,");

        /* Sub-usability - 20/40MHz PPDUs */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usability_40ppdu,
                "%hu,");

        /* Sub-usability - 80MHz PPDUs */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usability_80ppdu,
                "%hu,");

        /* Sub-usability - 160MHz PPDUs */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usability_160ppdu,
                "%hu,");

        /* Usage factor - 20/40MHz PPDUs (width=80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usage_factor_80_bw40,
                "%hhu,");

        /* Usage factor - 80MHz PPDUs (width=80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usage_factor_80_bw80,
                "%hhu,");

        /* Usage factor - 20/40MHz PPDUs (width=160) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usage_factor_160_bw40,
                "%hhu,");

        /* Usage factor - 80MHz PPDUs (width=160) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usage_factor_160_bw80,
                "%hhu,");

        /* Usage factor - 160MHz PPDUs (width=160) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usage_factor_160_bw160,
                "%hhu,");

        /* Usage factor - 20/40MHz PPDUs (width=80+80, for pri80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usage_factor_80p80_bw40,
                "%hhu,");

        /* Usage factor - 80MHz PPDUs (width=80+80, for pri80) */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usage_factor_80p80_bw80,
                "%hhu,");

        /* Pre-adjustment computed usability */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].preadj_usability,
                "%hu,");

        /* Pre-penalization measured usability */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].prepnl_measured_usability,
                "%hu,");

        /* Is penalized? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].is_penalized,
                icm_bool_str);

        /* Final measured usability */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].measured_usability,
                "%hu,");

        /* Pre-penalization measured usability - ext chan*/
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].prepnl_measured_usability_ext,
                "%hu,");

        /* Is ext chan penalized? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].is_penalized_ext,
                icm_bool_str);

        /* Final measured usability - ext chan */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].measured_usability_ext,
                "%hu,");

        /* Pre-penalization measured usability - 80+80 MHz sec80 */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].prepnl_measured_usability_sec80,
                "%hu,");

        /* Is 80+80 MHz sec80 (first chan from left) penalized? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].is_penalized_sec80,
                icm_bool_str);

        /* Final measured usability - 80+80 MHz sec80 */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].measured_usability_sec80,
                "%hu,");

        /* Final computed usability  */
        ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                decdbginfo[chan_idx].usability,
                "%hu,");

        /* Rejection Status/Reason? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].rejreason,
                icm_seldbg_rejcode_str);

        /* Is best chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                decdbginfo[chan_idx].is_bestchan,
                icm_bool_str);

        fprintf(seldbgfile->file, "\n");

        for (rel_chan_idx = 0;
                rel_chan_idx < ARRAY_LEN(decdbginfo[chan_idx].relchaninfo);
                rel_chan_idx++)
        {
            relchandbginfo = &(decdbginfo[chan_idx].relchaninfo[rel_chan_idx]);

            /* Prints for related channel */

            if (relchandbginfo->is_valid_fstatus != ICM_SELDBG_FSTATUS_SET ||
                    relchandbginfo->is_valid == false) {
                continue;
            }

            /* Record Set No. */
            fprintf(seldbgfile->file, "%u,", recordset_count);

            /* Primary chan */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    prichandbginfo->chan_num,
                    "%hu,");

            /* Primary freq */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    prichandbginfo->freq,
                    "%.0f,");

            /* Band */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    decdbginfo[chan_idx].band,
                    icm_band_str);

            /* PHY Spec */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    decdbginfo[chan_idx].physpec,
                    icm_phy_spec_str);

            /* Width */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    decdbginfo[chan_idx].width,
                    icm_ch_bw_str);

            /* Related chan */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    relchandbginfo->chan_num,
                    "%hu,");

            /* Related chan freq */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    relchandbginfo->freq,
                    "%.0f,");

            /* Relation */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->relation,
                    icm_chan_relationship_str);

            /* Incapable of PHY Spec/Width? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_incapable,
                    icm_bool_str);

            /* Excluded? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_excluded,
                    icm_bool_str);

            /* OBSS APs with pri on chan? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_obss_pri20,
                    icm_bool_str);

            /* No. of OBSS APs with pri on chan */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    relchandbginfo->num_obss_aps,
                    "%d,");

            /* Any OBSS APs with sec20 on chan? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_obss_sec20,
                    icm_bool_str);

            /* Any OBSS APs with sec40 on chan? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_obss_sec40,
                    icm_bool_str);

            /* Noise Floor (dBm) */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    relchandbginfo->noise_floor,
                    "%d,");

            /* Noise Floor Threshold (dBm) */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    relchandbginfo->noise_floor_thresh,
                    "%d,");

            /* Max regulatory power (dBm) */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    relchandbginfo->maxregpower,
                    "%hhd,");

            /* ETSI Weather Radar channel? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_etsi_weather,
                    icm_bool_str);

            /* CW Inf found? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_cw_intf,
                    icm_bool_str);

            /* Microwave Inf found? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_mwo_intf,
                    icm_bool_str);

            /* FHSS Inf found? */
            ICM_SELDBG_PRINTENTRY_STRCONV(seldbgfile->file,
                    relchandbginfo->is_fhss_intf,
                    icm_bool_str);

            /* FHSS Degrade Factor */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    decdbginfo[chan_idx].fhss_degrade_factor,
                    "%hhu,");

            /* MWO Degrade Factor */
            ICM_SELDBG_PRINTENTRY(seldbgfile->file,
                    decdbginfo[chan_idx].mwo_degrade_factor,
                    "%hhu,");

            /* All related chans processed? */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Baseline usability */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Total APs (For width < 80) */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Total APs affecting 40MHz PPDUs (width=80) */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Total APs affecting 80MHz PPDUs (width=80) */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Sub-usability - 40MHz PPDUs */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Sub-usability - 80MHz PPDUs */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Usage factor - 80MHz PPDUs */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Pre-adjustment computed usability */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Pre-penalization measured usability */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Is penalized? */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Final measured usability */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Pre-penalization measured usability - ext chan*/
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Is ext chan penalized? */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Final measured usability - ext chan */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Pre-penalization measured usability - 80+80 MHz sec80 */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Is 80+80 MHz sec80 (first chan from left) penalized? */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Final measured usability - 80+80 MHz sec80 */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Final computed usability  */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Rejection Status/Reason? */
            fprintf(seldbgfile->file, "Not Applicable,");

            /* Is best chan? */
            fprintf(seldbgfile->file, "Not Applicable,");

            fprintf(seldbgfile->file, "\n");
        }
    }

    fflush(seldbgfile->file);
    return SUCCESS;
}

/*
 * Function     : icm_seldbg_get_rel_maxstrlen
 * Input param  : pointer to dev info structure
 * Description  : Helper function to get max length of descriptive string
 *                for ICM_CHAN_RELATIONSHIP_T. NULL character not considered.
 * Return       : On success, max length of descriptive string for
 *                ICM_CHAN_RELATIONSHIP_T.
 *                On failure, -1.
 */
int icm_seldbg_get_rel_maxstrlen(ICM_DEV_INFO_T* pdev)
{
    ICM_CHAN_RELATIONSHIP_T rel_iter = 0;
    u_int32_t maxlen = 0;
    u_int32_t currlen = 0;

    if (pdev == NULL) {
        return -1;
    }

    while (rel_iter <= ICM_CHAN_RELATIONSHIP_INVALID) {
        currlen = strlen(icm_chan_relationship_str[rel_iter]);

        if (currlen > maxlen) {
            maxlen = currlen;
        }

        rel_iter++;
    }

    return maxlen;
}

/*
 * Function     : icm_seldbg_get_fstatus_maxstrlen
 * Input param  : pointer to dev info structure
 * Description  : Helper function to get max length of descriptive string
 *                for ICM_SELDBG_FSTATUS_T. NULL character not considered.
 * Return       : On success, max length of descriptive string for
 *                ICM_SELDBG_FSTATUS_T.
 *                On failure, -1.
 */
int icm_seldbg_get_fstatus_maxstrlen(ICM_DEV_INFO_T* pdev)
{
    ICM_SELDBG_FSTATUS_T fstatus_iter = 0;
    u_int32_t maxlen = 0;
    u_int32_t currlen = 0;

    if (pdev == NULL) {
        return -1;
    }

    while (fstatus_iter <= ICM_SELDBG_FSTATUS_INVALID) {
        currlen = strlen(icm_seldbg_fstatus_str[fstatus_iter]);

        if (currlen > maxlen) {
            maxlen = currlen;
        }

        fstatus_iter++;
    }

    return maxlen;
}

/*
 * Function     : icm_seldbg_printlegend
 * Input param  : pointer to dev info structure
 * Description  : Helper function to print legend for selection debug
 *                information.
 */
void icm_seldbg_printlegend(ICM_DEV_INFO_T* pdev)
{
    u_int8_t                bool_iter = 0;
    ICM_CHAN_RELATIONSHIP_T rel_iter = 0;
    ICM_SELDBG_FSTATUS_T    fstatus_iter = 0;
    ICM_SELDBG_REJCODE_T    rejcode_iter = 0;
    int maxrelsize = 0;
    int maxfstatussize = 0;

    ICM_ASSERT(pdev != NULL);

    maxrelsize = icm_seldbg_get_rel_maxstrlen(pdev);
    if (maxrelsize == -1) {
        err("Could not get max size of ICM_CHAN_RELATIONSHIP_T");
        return;
    }

    maxfstatussize = icm_seldbg_get_fstatus_maxstrlen(pdev);
    if (maxfstatussize == -1) {
        err("Could not get max size of ICM_SELDBG_FSTATUS_T");
        return;
    }

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "\nicm: Legend and Notes for selection debug info:\n"
            "-----------------------------------------------\n\n");

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "%-*s%-*s%s\n",
            SHORTSTRWIDTH + 2 + maxrelsize + \
            ICM_SELDBG_INTERCOL_PADDING,
            "Channel Relationships",
            SHORTSTRWIDTH + 2 + maxfstatussize + \
            ICM_SELDBG_INTERCOL_PADDING,
            "Field Status",
            "Booleans");

    while ((rel_iter < ICM_CHAN_RELATIONSHIP_INVALID)  ||
            (fstatus_iter < ICM_SELDBG_FSTATUS_INVALID) ||
            (bool_iter <= (u_int8_t)true)) {
        if (rel_iter < ICM_CHAN_RELATIONSHIP_INVALID) {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s: %-*s",
                    SHORTSTRWIDTH,
                    icm_chan_relationship_shortstr[rel_iter],
                    maxrelsize + ICM_SELDBG_INTERCOL_PADDING,
                    icm_chan_relationship_str[rel_iter]);
            rel_iter++;
        } else {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s  %-*s",
                    SHORTSTRWIDTH,
                    " ",
                    maxrelsize + ICM_SELDBG_INTERCOL_PADDING,
                    " ");
        }

        if (fstatus_iter < ICM_SELDBG_FSTATUS_INVALID) {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s: %-*s",
                    SHORTSTRWIDTH,
                    icm_seldbg_fstatus_shortstr[fstatus_iter],
                    maxfstatussize + ICM_SELDBG_INTERCOL_PADDING,
                    icm_seldbg_fstatus_str[fstatus_iter]);
            fstatus_iter++;
        } else {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s  %-*s",
                    SHORTSTRWIDTH,
                    " ",
                    maxfstatussize + ICM_SELDBG_INTERCOL_PADDING,
                    " ");
        }

        if (bool_iter <= (u_int8_t)true) {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s: %s",
                    SHORTSTRWIDTH,
                    icm_bool_shortstr[bool_iter],
                    icm_bool_str[bool_iter]);
            bool_iter++;
        } else {
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s  %s",
                    SHORTSTRWIDTH,
                    " ",
                    " ");
        }

        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NO_PREFIX,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "\n");
    }

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "\nRejection reasons\n");

    while (rejcode_iter < ICM_SELDBG_REJCODE_INVALID)
    {
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NO_PREFIX,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "%-*s: %s\n",
                14,
                icm_seldbg_rejcode_shortstr[rejcode_iter],
                icm_seldbg_rejcode_str[rejcode_iter]);

        rejcode_iter++;
    }

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "\nTable Entries\n"
            "Pri Chan      : Primary Channel\n"
            "Reltd Chan    : Related Channel\n"
            "Reltn         : Relation with Primary\n"
            "Incap?        : Incapable of configured PHY Spec/Width?\n"
            "Excld?        : Excluded by external entity?\n"
            "Num OBSS Pri  : No. of OBSS APs with their primary on this "
            "channel\n"
            "OBSS Sec20?   : Any OBSS AP with Sec 20 on this channel?\n"
            "OBSS Sec40?   : Any OBSS AP with Sec 40 on this channel?\n"
            "NF            : Noise Floor (dBm)\n"
            "NF Thrsh      : Noise Floor Threshold (dBm)\n"
            "Max Reg Pwr   : Max Regulatory Power (dBm)\n"
            "ETSI Wthr?    : Is this an ETSI Weather Radar channel?\n"
            "CW?           : Continuous Wave Field Device Interference "
            "found?\n"
            "MWO?          : Microwave Interference found?\n"
            "FHSS?         : FHSS Interference found?\n"
            "Penalized?    : Penalized by external entity?\n"
            "Msrd Usab     : Final measured usability\n"
            "Msrd Usab Ext : Final measured usability on extension "
            "channel\n"
            "Usab          : Final computed usability\n"
            "Best Chan?    : Is this the best channel?\n");

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "\nNotes:\n"
            "- Since a primary can be discarded early, not all related "
            "channels may be processed\n\n");

}

/* This is quite specific to the needs of icm_seldbg_consoledump() so we let it
   remain local. */
#define ICM_SELDBG_DRAW_CDUMP_LINE(pdev)                                       \
    ICM_DPRINTF((pdev),                                                        \
            ICM_PRCTRL_FLAG_NO_PREFIX,                                     \
            ICM_DEBUG_LEVEL_DEFAULT,                                       \
            ICM_MODULE_ID_SELECTOR,                                        \
            "------------------------------------------------------------" \
            "------------------------------------------------------------" \
            "----------"                                                   \
            "\n")

#define ICM_SELDBG_DRAW_CFREQ2_CDUMP_LINE(pdev)                                \
    ICM_DPRINTF((pdev),                                                        \
            ICM_PRCTRL_FLAG_NO_PREFIX,                                     \
            ICM_DEBUG_LEVEL_DEFAULT,                                       \
            ICM_MODULE_ID_SELECTOR,                                        \
            "-------------------------------------------------------------"\
            "---------------------"                                        \
            "\n")


/*
 * Function     : icm_seldbg_consoledump
 * Description  : dump selection debug information to console/standard output.
 * Input params : pointer to dev info structure
 *                pointer to array of ICM_SELDBG_DECISION_INFO_T entries,
 *                number of array members
 * Return       : SUCCESS or FAILURE
 */
int icm_seldbg_consoledump(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize)
{
    u_int16_t chan_idx = 0, rel_chan_idx = 0;
    ICM_SELDBG_CHAN_INFO_T  *prichandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T  *relchandbginfo = NULL;

    ICM_ASSERT(pdev != NULL);
    ICM_ASSERT(decdbginfo != NULL);
    ICM_ASSERT(arraysize > 0);

#ifndef ANDROID
    icm_seldbg_printlegend(pdev);
#endif /* ANDROID */

    /* For brevity, we print the Band, PHY Spec, and desired width only once
       at the beginning. By design, the values in the first entry of decdbginfo
       apply to all entries. Also, for simplicity, we do not check for invalid
       fields here since we are sure they have to be populated, else something
       is wrong with the implementation. The field protection is only for
       uniformity, in this case. */
    ICM_SELDBG_DRAW_CDUMP_LINE(pdev);
    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "%-14s: %-114s|\n%-14s: %-114s|\n%-14s: %-114s|\n",
            "Band",
            icm_band_str[decdbginfo[0].band],
            "PHY Spec",
            icm_phy_spec_str[decdbginfo[0].physpec],
            "Desired b/w",
            icm_ch_bw_str[decdbginfo[0].width]);
    ICM_SELDBG_DRAW_CDUMP_LINE(pdev);

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "     |     |     |     |     |Num  |OBSS |OBSS |     |     |Max  |ETSI |     |     |     |Pena-|     |Msrd |     |Rejection |Best |\n"
            "Pri  |Reltd|Reltn|Incap|Excld|OBSS |Sec20|Sec40|NF   |NF   |Reg  |Wthr |CW   |MWO  |FHSS |lized|Msrd |Usab |Usab |Code      |Chan |\n"
            "Chan |Chan |     |?    |?    |Pri  |?    |?    |     |Thrsh|Pwr  |?    |?    |?    |?    |?    |Usab |Ext  |     |          |?    |"
            "\n");
    ICM_SELDBG_DRAW_CDUMP_LINE(pdev);

    for (chan_idx = 0; chan_idx < arraysize; chan_idx++) {
        prichandbginfo = &(decdbginfo[chan_idx].prichaninfo);
        /* Prints for primary channel and main decision parameters */

        /* Primary chan */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->chan_num,
                "%-*hu");

        /* Related chan */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->chan_num,
                "%-*hu");

        /* Relation */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->relation,
                icm_chan_relationship_shortstr);

        /* Incapable of PHY Spec/Width? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_incapable,
                icm_bool_shortstr);

        /* Excluded? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_excluded,
                icm_bool_shortstr);

        /* No. of OBSS APs with pri on chan */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->num_obss_aps,
                "%-*d");

        /* Any OBSS APs with sec20 on chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_obss_sec20,
                icm_bool_shortstr);

        /* Any OBSS APs with sec40 on chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_obss_sec40,
                icm_bool_shortstr);

        /* Noise Floor (dBm) */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->noise_floor,
                "%-*d");

        /* Noise Floor Threshold (dBm) */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->noise_floor_thresh,
                "%-*d");

        /* Max regulatory power (dBm) */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->maxregpower,
                "%-*hhd");

        /* ETSI Weather Radar channel? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_etsi_weather,
                icm_bool_shortstr);

        /* CW Inf found? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_cw_intf,
                icm_bool_shortstr);

        /* Microwave Inf found? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_mwo_intf,
                icm_bool_shortstr);

        /* FHSS Inf found? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->is_fhss_intf,
                icm_bool_shortstr);

        /* Is penalized? */
        /* Note: For the shortened console output, we assume that penalization
           if applied, applies everywhere.
XXX: Change this if no longer applicable in the future. */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].is_penalized,
                icm_bool_shortstr);

        /* Final measured usability */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].measured_usability,
                "%-*hu");

        /* Final measured usability - ext chan */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].measured_usability_ext,
                "%-*hu");

        /* Final computed usability  */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].usability,
                "%-*hu");

        /* Rejection code */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS_WIDE(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].rejreason,
                icm_seldbg_rejcode_shortstr);

        /* Is best chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].is_bestchan,
                icm_bool_shortstr);

        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NO_PREFIX,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "\n");

        for (rel_chan_idx = 0;
                rel_chan_idx < ARRAY_LEN(decdbginfo[chan_idx].relchaninfo);
                rel_chan_idx++)
        {
            relchandbginfo = &(decdbginfo[chan_idx].relchaninfo[rel_chan_idx]);

            /* Prints for related channel */

            if (relchandbginfo->is_valid_fstatus != ICM_SELDBG_FSTATUS_SET ||
                    relchandbginfo->is_valid == false) {
                continue;
            }

            /* Primary chan */
            ICM_SELDBG_PRINTENTRY_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    prichandbginfo->chan_num,
                    "%-*hu");

            /* Related chan */
            ICM_SELDBG_PRINTENTRY_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->chan_num,
                    "%-*hu");

            /* Relation */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->relation,
                    icm_chan_relationship_shortstr);

            /* Incapable of PHY Spec/Width? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_incapable,
                    icm_bool_shortstr);

            /* Excluded? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_excluded,
                    icm_bool_shortstr);

            /* No. of OBSS APs with pri on chan */
            ICM_SELDBG_PRINTENTRY_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->num_obss_aps,
                    "%-*d");

            /* Any OBSS APs with sec20 on chan? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_obss_sec20,
                    icm_bool_shortstr);

            /* Any OBSS APs with sec40 on chan? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_obss_sec40,
                    icm_bool_shortstr);

            /* Noise Floor (dBm) */
            ICM_SELDBG_PRINTENTRY_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->noise_floor,
                    "%-*d");

            /* Noise Floor Threshold (dBm) */
            ICM_SELDBG_PRINTENTRY_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->noise_floor_thresh,
                    "%-*d");

            /* Max regulatory power (dBm) */
            ICM_SELDBG_PRINTENTRY_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->maxregpower,
                    "%-*hhd");

            /* ETSI Weather Radar channel? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_etsi_weather,
                    icm_bool_shortstr);

            /* CW Inf found? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_cw_intf,
                    icm_bool_shortstr);

            /* Microwave Inf found? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_mwo_intf,
                    icm_bool_shortstr);
            /* FHSS Inf found? */
            ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                    ICM_MODULE_ID_SELECTOR,
                    relchandbginfo->is_fhss_intf,
                    icm_bool_shortstr);

            /* Is penalized? */
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s|",
                    SHORTSTRWIDTH,
                    icm_seldbg_fstatus_shortstr[ICM_SELDBG_FSTATUS_NOTAPPL]);

            /* Final measured usability */
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s|",
                    SHORTSTRWIDTH,
                    icm_seldbg_fstatus_shortstr[ICM_SELDBG_FSTATUS_NOTAPPL]);

            /* Final measured usability - ext chan */
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s|",
                    SHORTSTRWIDTH,
                    icm_seldbg_fstatus_shortstr[ICM_SELDBG_FSTATUS_NOTAPPL]);

            /* Final computed usability  */
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s|",
                    SHORTSTRWIDTH,
                    icm_seldbg_fstatus_shortstr[ICM_SELDBG_FSTATUS_NOTAPPL]);

            /* Rejection code */
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s|",
                    SHORTSTRWIDTH_WIDE,
                    icm_seldbg_fstatus_shortstr[ICM_SELDBG_FSTATUS_NOTAPPL]);

            /* Is best chan? */
            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "%-*s|",
                    SHORTSTRWIDTH,
                    icm_seldbg_fstatus_shortstr[ICM_SELDBG_FSTATUS_NOTAPPL]);

            ICM_DPRINTF(pdev,
                    ICM_PRCTRL_FLAG_NO_PREFIX,
                    ICM_DEBUG_LEVEL_DEFAULT,
                    ICM_MODULE_ID_SELECTOR,
                    "\n");
        }
    }

    ICM_SELDBG_DRAW_CDUMP_LINE(pdev);
    return SUCCESS;
}

/*
 * Function     : icm_best_cfreq2_selection_helper
 * Description  : Helper function to select best secondary 80 MHz center channel
 *                index.
 * Input params : -pointer to icm info structure
 *                -IEEE channel number for primary 20 MHz already selected, for
 *                80+80 MHz mode. It is the responsibility of the calling
 *                function (or function stack) to ensure this is valid.
 * Return       : IEEE channel number corresponding to centre frequency of best
 *                secondary 80 MHz, or -1 on failure.
 */
int icm_best_cfreq2_selection_helper(ICM_INFO_T* picm,
        int pri20_chan_80_80)

{
    ICM_CHANNEL_LIST_T *chanlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    /* List of channels which each form start of an 80 MHz segment.
     * E.g for an 80 MHz segment consisting of channels 36, 40, 44, 48, the
     * starting channel will be 36. */
    ICM_CHANNEL_T st80_chan[ICM_MAX_80MHZ_SEG];
    int st80_chan_count = 0;
    int best_sec80_start_chan;
    u_int16_t usability, usability_80_ppdu;
    u_int16_t chan_idx, chan_num;
    u_int16_t st80_chan_idx;
    u_int16_t bondspan_chan_num;
    u_int16_t num_ap_40, num_ap_80, num_ap_160;
    u_int16_t num_ap_srp_supported = 0;
    u_int16_t num_ap_client_srp_allowed = 0;
    u_int16_t num_ap_client_obsspd_allowed = 0;
    u_int16_t min_msrd_channel_usability = 0;
    u_int16_t tmp_msrd_channel_usability;
    u_int16_t adjusted_msrd_usability = 0;
    u_int16_t num_ap_aci = 0;
    u_int16_t num_ap_tot_80ppdu_compete;
    u_int8_t sec80_increment_done = 0;
    int nf_threshold_base = ATH_DEFAULT_NF_THRESHOLD;
    int nf_threshold = 0;
    int nf_threshold_correction = 0;
    int nf = 0;
    ICM_CHANNEL_T *ch;
    ICM_CHANNEL_T *ch_pri20_80_80;
    int emiwar80p80_enab = -1;

    int bond_range_start_idx = 0;
    int bond_range_end_idx = 0;
    int adj_plus_bond_range_start_idx = 0;
    int adj_plus_bond_range_end_idx = 0;
    int bondspan_chan_adj, bondspan_chan_idx, usable_chan_idx;

    u_int16_t shift_cnt;

    int8_t seglowest_maxregpower = 0;
    int8_t tmp_maxregpower = 0;

    ICM_SELDBG_DECISION_INFO_T *decdbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *prichandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T     *relchandbginfo = NULL;

    int ret;

    ICM_ASSERT((picm->phy_spec == ICM_PHY_SPEC_11AC) ||
               (picm->phy_spec == ICM_PHY_SPEC_11AXA));

    bond_range_start_idx = 0;
    bond_range_end_idx = 3;
    adj_plus_bond_range_start_idx = bond_range_start_idx - 1;
    adj_plus_bond_range_end_idx = bond_range_end_idx + 1;

    ICM_DEV_INFO_T *pdev = get_pdev();

    chan_idx = icm_get_channel_index(chanlist, pri20_chan_80_80);
    ICM_ASSERT(chan_idx >= 0);
    ch_pri20_80_80 = &chanlist->ch[chan_idx];

#ifdef ICM_RTR_DRIVER
    if ((emiwar80p80_enab = is_emiwar80p80_enab(picm)) < 0) {
        fprintf(stderr, "%-8.16s Unable to determine whether 80+80 EMI WAR is "
                "enabled.\n\n", picm->radio_ifname);
        return -1;
    }
#endif /* ICM_RTR_DRIVER */

    /* The starting channel of each 80 MHz segment will be used as an 'anchor'
     * channel to represent and carry information about that segment.
     */
    memset(st80_chan, 0, sizeof(st80_chan));
    for (chan_idx = 0; chan_idx < chanlist->count; chan_idx++) {
        ch = &chanlist->ch[chan_idx];
        if ((((picm->phy_spec == ICM_PHY_SPEC_11AC) &&
                    IEEE80211_IS_CHAN_11AC_VHT80_80(ch)) ||
             ((picm->phy_spec == ICM_PHY_SPEC_11AXA) &&
                    IEEE80211_IS_CHAN_11AXA_HE80_80(ch))) &&
             (ch->ic_vhtop_ch_freq_seg1 == (ch->channel + 6)) &&
             ((ch->ic_vhtop_ch_freq_seg1 <
               (ch_pri20_80_80->ic_vhtop_ch_freq_seg1 - 16)) ||
              (ch->ic_vhtop_ch_freq_seg1 >
               (ch_pri20_80_80->ic_vhtop_ch_freq_seg1 + 16))) &&
             ((!emiwar80p80_enab) ||
              ICM_IS_EMIWAR80P80_COMB_ALLOWED(ch_pri20_80_80->ic_vhtop_ch_freq_seg1,
                  ch->ic_vhtop_ch_freq_seg1)))
        {
            memcpy(&st80_chan[st80_chan_count],
                    ch, sizeof(st80_chan[st80_chan_count]));
            st80_chan_count++;
        }
    }

    if (st80_chan_count == 0) {
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "No 80 MHz segments available with current regulatory "
                "domain\n");
        return -1;
    }

    decdbginfo = (ICM_SELDBG_DECISION_INFO_T*)
        malloc(sizeof(ICM_SELDBG_DECISION_INFO_T) * \
                st80_chan_count);

    if (decdbginfo == NULL) {
        fprintf(stderr,
                "Could not allocate memory for Decision Debug Information "
                "Array\n");
        exit(EXIT_FAILURE);
    }

    ret = icm_init_cfreq2_seldbg_decinfo(decdbginfo, st80_chan_count,
            picm->phy_spec);

    if (ret != SUCCESS) {
        fprintf(stderr,
                "Could not initialize Decision Debug Information "
                "Array\n");
        exit(EXIT_FAILURE);
    }

#ifdef ICM_RTR_DRIVER /* This needs Spectral support */
#if !ICM_SUPPORT_HIGH_NF_WAR
    /* Get the Nominal NF from the driver, if required */
    if (picm->get_nominal_nf) {
        /* Note : The ATH_DEFAULT_CW_NOISEFLOOR_DELTA is +dbm */
        /* XXX: Check if changes are required for VHT80 */
        nf_threshold_base = icm_get_nominal_noisefloor(picm) +
            ATH_DEFAULT_CW_NOISEFLOOR_DELTA;
    }
#endif /* !ICM_SUPPORT_HIGH_NF_WAR */
#endif /* ICM_RTR_DRIVER */

    for (st80_chan_idx = 0; st80_chan_idx < st80_chan_count; st80_chan_idx++) {
        prichandbginfo = &(decdbginfo[st80_chan_idx].prichaninfo);

        ICM_SELDBG_SETFIELD(prichandbginfo->chan_num, pri20_chan_80_80);
        ICM_SELDBG_SETFIELD(prichandbginfo->freq,
                icm_convert_ieee2mhz(pri20_chan_80_80));

        relchandbginfo = &(decdbginfo[st80_chan_idx].relchaninfo[0]);

        ch = &(st80_chan[st80_chan_idx]);
        chan_num = ch->channel;
        ICM_SELDBG_SETFIELD(relchandbginfo->chan_num, chan_num);
        ICM_SELDBG_SETFIELD(relchandbginfo->freq,
                icm_convert_ieee2mhz(chan_num));
        ICM_SELDBG_SETFIELD(relchandbginfo->center_chan_num, chan_num + 6);
        ICM_SELDBG_SETFIELD(relchandbginfo->center_freq,
                icm_convert_ieee2mhz(chan_num + 6));
        ICM_SELDBG_SETFIELD(relchandbginfo->relation,
                ICM_CHAN_RELATIONSHIP_SEC80);

        /* Baseline usability will be set to that of the starting channel in the
         * candidate 80 MHz block. */
        usability = ICM_GET_CHANNEL_BLUSABILITY(picm, chan_num);
        ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].baseline_usability,
                usability);

        /* Note: Channel 80+80 MHz capability has already been determined
         * earlier in the function. We do not add Selection Debug information
         * showing list of incapable channels, since this will already have
         * been done for the primary 80 MHz selection display. The focus of this
         * round of selection is to re-evaluate desirability considering only
         * channels that are capable.
         */
        ICM_SELDBG_SETFIELD(relchandbginfo->is_incapable, false);

        for (bondspan_chan_adj = bond_range_start_idx;
                bondspan_chan_adj <= bond_range_end_idx;
                bondspan_chan_adj++) {
            chan_idx = icm_get_channel_index(chanlist, chan_num);
            ICM_ASSERT(chan_idx >= 0);

            /* Check if this is a valid channel */
            bondspan_chan_idx = chan_idx + bondspan_chan_adj;

            if (bondspan_chan_idx < 0 &&  bondspan_chan_idx >= MAX_NUM_CHANNEL) {
                continue;
            }

            bondspan_chan_num = chanlist->ch[bondspan_chan_idx].channel;
            /* Sanity check -- Check if the channel exists */
            if (bondspan_chan_num != (chan_num + (bondspan_chan_adj * 4))) {
                continue;
            }

            if (ICM_GET_CHANNEL_EXCLUDE(picm, bondspan_chan_num) == TRUE) {
                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_excluded, true);
                }
                ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].rejreason,
                        ICM_SELDBG_REJCODE_BOND_EXCLD);
                usability = 0;
                break;
            } else if (relchandbginfo != NULL) {
                ICM_SELDBG_SETFIELD(relchandbginfo->is_excluded, false);
            }

            /* XXX: Check that VHT80/VHT160/VHT80+80 Tx gets factored into
             * measured usability.
             */
            if (bondspan_chan_num == chan_num) {
                min_msrd_channel_usability = ICM_GET_CHANNEL_FREE_TIME(picm,
                        bondspan_chan_num);
            } else {
                tmp_msrd_channel_usability = ICM_GET_CHANNEL_FREE_TIME(picm,
                        bondspan_chan_num);

                if (tmp_msrd_channel_usability < min_msrd_channel_usability) {
                    min_msrd_channel_usability = tmp_msrd_channel_usability;
                }
            }


            /* The maximum regulatory power is set as per the STA and it can be
             * 7dB more for the AP. The maximum value allowed is 30dB
             */
            /* XXX: Check for 802.11ac related changes here */

            if (bondspan_chan_num == chan_num) {
                seglowest_maxregpower = MIN(30, ch->ic_maxregpower);
                ICM_SET_CHANNEL_MAX_REGPWR_SEC80(picm,
                        chan_num, seglowest_maxregpower);
                ICM_SELDBG_SETFIELD(relchandbginfo->maxregpower,
                        seglowest_maxregpower);
            } else {
                tmp_maxregpower =
                    MIN(30, chanlist->ch[bondspan_chan_idx].ic_maxregpower);
                if (tmp_maxregpower < seglowest_maxregpower) {
                    seglowest_maxregpower = tmp_maxregpower;
                    ICM_SET_CHANNEL_MAX_REGPWR_SEC80(picm,
                            chan_num, seglowest_maxregpower);
                    ICM_SELDBG_SETFIELD(relchandbginfo->maxregpower,
                            seglowest_maxregpower);
                }
            }

            /* Check if there is CW interference */
            if (bondspan_chan_num > 140) {
                /* As the frequency increases, there is dip in the nominal
                 * noisefloor value due to receive sensitivity. To compensate
                 * for this, correction of -7dbm is added to the nominal
                 * noisefloor value.
                 */
                nf_threshold_correction = ATH_DEFAULT_NF_CORRECTION;
            } else {
                nf_threshold_correction = 0;
            }

            nf_threshold = nf_threshold_base + nf_threshold_correction;
            nf = ICM_GET_CHANNEL_NOISEFLOOR(picm, bondspan_chan_num);

            if (nf > nf_threshold) {
                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_cw_intf, true);
                }

                ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].rejreason,
                        ICM_SELDBG_REJCODE_BOND_CW);

                usability = 0;
                break;
            } else if (relchandbginfo != NULL) {
                ICM_SELDBG_SETFIELD(relchandbginfo->is_cw_intf, false);
            }

            /* Check if the channel falls in weather radar channel */
            if ((picm->dfs_domain == NL80211_DFS_ETSI) &&
                    WEATHER_RADAR_CHANNEL(\
                        (int)chanlist->ch[bondspan_chan_idx].freq)) {
                /* This is a radar channel and cannot be used */

                if (relchandbginfo != NULL) {
                    ICM_SELDBG_SETFIELD(relchandbginfo->is_etsi_weather,
                            true);
                }
                ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].rejreason,
                        ICM_SELDBG_REJCODE_BOND_ETSIWTH);

                usability = 0;
                break;
            } else if (relchandbginfo != NULL) {
                ICM_SELDBG_SETFIELD(relchandbginfo->is_etsi_weather,
                        false);
            }
        }

        /* Check if we broke away for some reason. */
        if (usability == 0) {
            ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].usability, 0);
            continue;
        }

        /* The usability is computed based on potential 80 MHz PPDU Tx
           requirement.

           The 80 MHz PPDU Tx requirement is affected by
           i)  Competing 160/80+80 MHz PPDU Tx
           ii) Competing 80 MHz PPDU Tx
           iii)Competing 20/40 MHz PPDU Tx anywhere in our 80 MHz span.
           iv) Transmissions in both adjacent channels outside 80 MHz
           block due to ACI.

           The usability is computed separately by dividing the existing
           usability value by the sum of the following:
           i)   No. of competing 160/80+80 MHz APs
           ii)  No. of competing 80 MHz APs
           iii) No. of competing 20/40 MHz APs anywhere in our 80 MHz span.
           iv)  No. of APs in both adjacent channels outside 80 MHz
           block.
         */

        num_ap_40 = num_ap_80 = num_ap_160 = 0;
        num_ap_aci = 0;
        num_ap_tot_80ppdu_compete = 0;
        usability_80_ppdu = usability;

        sec80_increment_done = 0;

        for (bondspan_chan_adj = adj_plus_bond_range_start_idx;
                bondspan_chan_adj <= adj_plus_bond_range_end_idx;
                bondspan_chan_adj++) {
            chan_idx = icm_get_channel_index(chanlist, chan_num);
            ICM_ASSERT(chan_idx >= 0);

            /* Check if this is a valid channel */
            bondspan_chan_idx = chan_idx + bondspan_chan_adj;
            if (bondspan_chan_idx < 0 &&  bondspan_chan_idx >= MAX_NUM_CHANNEL) {
                continue;
            }

            bondspan_chan_num = chanlist->ch[bondspan_chan_idx].channel;

            /* Sanity check -- Check if the channel exists/needs
               to be considered */
            if (bondspan_chan_num != (chan_num + (bondspan_chan_adj * 4))) {
                continue;
            }

            if ((bondspan_chan_adj == adj_plus_bond_range_start_idx) ||
                    (bondspan_chan_adj == adj_plus_bond_range_end_idx))
            {
                num_ap_aci += chanlist->ch[bondspan_chan_idx].num_wnw;
            } else {
                num_ap_40 +=
                    (chanlist->ch[bondspan_chan_idx].num_wnw_20 +
                     chanlist->ch[bondspan_chan_idx].num_wnw_40);

                num_ap_80 +=
                    chanlist->ch[bondspan_chan_idx].num_wnw_80;

                num_ap_160 +=
                    chanlist->ch[bondspan_chan_idx].num_wnw_160;

                /* Increment SR counts for bondspan channels that fall in
                 * the operating range (but not adjacent channels outside
                 * it).
                 * 11AX TODO: If later data suggests significantly enhanced
                 * ACI effects due to SR, then revisit.
                 */
                num_ap_srp_supported +=
                    chanlist->ch[bondspan_chan_idx].num_wnw_srp_supported;
                num_ap_client_srp_allowed +=
                    chanlist->ch[bondspan_chan_idx].num_wnw_client_srp_allowed;
                num_ap_client_obsspd_allowed +=
                    chanlist->ch[bondspan_chan_idx].num_wnw_client_obsspd_allowed;

                if (!sec80_increment_done) {
                    /* Increment number of 160/80+80 MHz APs having their
                     * secondary 80 MHz on our secondary 80, only once
                     * (since this count would be incremented for every
                     * bonding channel in our secondary 80, for the same set
                     * of APs).
                     */
                    num_ap_80 +=
                        (chanlist->ch[bondspan_chan_idx].used_as_80p80_secondary_80 +
                         chanlist->ch[bondspan_chan_idx].used_as_160_secondary_80);
                    sec80_increment_done = 1;
                }
            }
        }

        num_ap_tot_80ppdu_compete = (num_ap_40 + num_ap_80 + num_ap_160
                + num_ap_aci);
        usability_80_ppdu /= (num_ap_tot_80ppdu_compete + 1);
        usability = usability_80_ppdu;

        if ((num_ap_srp_supported != 0) ||
                (num_ap_client_srp_allowed != 0) ||
                (num_ap_client_obsspd_allowed != 0))
        {
            icm_penalize_11ax_srp(chan_num, 1, &usability,
                    num_ap_srp_supported, num_ap_client_srp_allowed,
                    num_ap_client_obsspd_allowed);
        }

        ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].tot_num_aps_80w_80ppdu,
                num_ap_tot_80ppdu_compete + 1);
        ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].preadj_usability,
                usability);

        ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].is_allrelchanprocd, true);

        ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].prepnl_measured_usability_sec80,
                min_msrd_channel_usability);

        adjusted_msrd_usability = min_msrd_channel_usability;

        /* XXX: For 80+80 seg ID 1, i.e. secondary 80 MHz, the penalization
           computation only considers baseline usability for the first 20 MHz
           channel from the left. This is because inclusion and position of the
           remaining channels would be fixed relative to the first one (and
           there are multiple such channel combinations). Processing
           penalization for each channel individually is not currently foreseen
           to yield any practical gain. The baseline usability of the first
           channel acts as the anchor for the entire secondary 80 MHz span, and
           suitability of the entire span can be indicated by external agent by
           providing this baseline usability. If more complexity is later
           desired for some reason, this can be revisited.
         */

        if (ICM_GET_CHANNEL_BLUSABILITY(picm, chan_num) != MAX_USABILITY) {
            /* Factor in penalization to scale down channel load that we
             * should be considering */
            adjusted_msrd_usability =
                (u_int16_t)(((u_int32_t)(adjusted_msrd_usability)) *
                        ICM_GET_CHANNEL_BLUSABILITY(picm, chan_num) /
                        MAX_USABILITY);

            ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].is_penalized_sec80,
                    true);
        } else {
            ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].is_penalized_sec80,
                    false);
        }

        ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].measured_usability,
                adjusted_msrd_usability);

        /* Check calculated usability with the measured usability. */

        if (usability > adjusted_msrd_usability) {
            usability = adjusted_msrd_usability;
        }

        /* Note here that the usability applies to the entire 80 MHz segment,
         * since chan_num, the first channel in the segment, acts as an anchor
         * to hold information for the segment.
         */
        ICM_SET_CHANNEL_USABLITY_SEC80(picm, chan_num, usability);
        ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].usability, usability);

    }

    /* At this point, we have computed the usability of all the 80 MHz segments,
     * now to figure out the best.
     */
    usability = 0;
    picm->num_usable_ch_sec80 = 0;
    for (st80_chan_idx = 0; st80_chan_idx < st80_chan_count; st80_chan_idx++) {
        ch = &(st80_chan[st80_chan_idx]);
        chan_num = ch->channel;

        if (ICM_GET_CHANNEL_USABLITY_SEC80(picm,chan_num)) {
            if (picm->num_usable_ch_sec80) {
                /* The list has been initiated */
                /* Insert the channel so that the channel is in
                 * descending order of usability */
                for (usable_chan_idx = 0;
                        usable_chan_idx < picm->num_usable_ch_sec80;
                        usable_chan_idx++) {
                    if (ICM_GET_CHANNEL_USABLITY_SEC80(picm,chan_num) <
                            ICM_GET_CHANNEL_USABLITY_SEC80(picm,
                                picm->sort_chan_list_sec80[usable_chan_idx])) {
                        continue;
                    }

                    /* If the the usability is the same, compare the max
                       reg power
                     */
                    if (ICM_GET_CHANNEL_USABLITY_SEC80(picm,chan_num) ==
                            ICM_GET_CHANNEL_USABLITY_SEC80(picm,
                                picm->sort_chan_list_sec80[usable_chan_idx])) {
                        if (ICM_GET_CHANNEL_MAX_REGPWR_SEC80(picm, chan_num) <=
                                ICM_GET_CHANNEL_MAX_REGPWR_SEC80(picm,
                                    usable_chan_idx)) {
                            continue;
                        }
                    }

                    /* Push the rest by one step down */
                    for (shift_cnt = picm->num_usable_ch_sec80;
                            shift_cnt > usable_chan_idx;
                            shift_cnt--) {
                        picm->sort_chan_list_sec80[shift_cnt] =
                            picm->sort_chan_list_sec80[shift_cnt-1];
                    }

                    picm->sort_chan_list_sec80[shift_cnt] = chan_num;
                    break;
                }

                /* Check if the current channel is the worst of all */
                if (usable_chan_idx == picm->num_usable_ch_sec80) {
                    picm->sort_chan_list_sec80[picm->num_usable_ch_sec80] =
                        chan_num;
                }
            } else {
                picm->sort_chan_list_sec80[picm->num_usable_ch_sec80] = chan_num;
            }
            picm->num_usable_ch_sec80++;
        }
    }

    best_sec80_start_chan =
        picm->num_usable_ch_sec80 ? picm->sort_chan_list_sec80[0] : -1;

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NONE,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "Number of usable channels=%d. Sorted list below:\n",
            picm->num_usable_ch_sec80);

    for(usable_chan_idx = 0;
            usable_chan_idx < picm->num_usable_ch_sec80;
            usable_chan_idx++) {
        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NONE,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                " start chan:%3d  usability:%5d  maxregpwr:%3d\n",
                picm->sort_chan_list_sec80[usable_chan_idx],
                ICM_GET_CHANNEL_USABLITY_SEC80(picm,
                    picm->sort_chan_list_sec80[usable_chan_idx]),
                ICM_GET_CHANNEL_MAX_REGPWR_SEC80(picm,
                    picm->sort_chan_list_sec80[usable_chan_idx]));
    }

    /* Set the current channel list index to zero */
    picm->cur_ch_idx_sec80 = 0;

    /* Set the best channel into the corresponding debug entry */
    if (best_sec80_start_chan != -1) {
        for (st80_chan_idx = 0;
                st80_chan_idx < st80_chan_count;
                st80_chan_idx++) {
            if (best_sec80_start_chan ==
                    decdbginfo[st80_chan_idx].relchaninfo[0].chan_num) {
                ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].is_bestchan,
                        true);
            } else {
                ICM_SELDBG_SETFIELD(decdbginfo[st80_chan_idx].is_bestchan,
                        false);
            }
        }
    }

    if (icm_cfreq2_seldbg_process(pdev,
                decdbginfo,
                st80_chan_count,
                picm->phy_spec) != SUCCESS) {
        exit(EXIT_FAILURE);
    }

    free(decdbginfo);

    if (best_sec80_start_chan == -1) {
        return -1;
    } else {
        return (best_sec80_start_chan + 6);
    }
}

/*
 * Function     : icm_init_cfreq2_seldbg_decinfo
 * Description  : Helper function for initializing array of
 *                ICM_SELDBG_DECISION_INFO_T entries with debug information
 *                specific to selection of centre frequency of secondary 80 MHz
 *                segment. Note that channel specific information (including
 *                channel number, Max regulatory power, etc.) are not filled in,
 *                and are initialized by the main selection logic.
 *                One of the main reasons for this split is that determination
 *                of bonding span happens dynamically deep inside selection
 *                logic.
 * Input params : Pointer to array of ICM_SELDBG_DECISION_INFO_T entries,
 *                Number of array members,
 *                PHY specification.
 * Return       : SUCCESS/FAILURE
 */
int icm_init_cfreq2_seldbg_decinfo(ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize, ICM_PHY_SPEC_T physpec)
{
    int i = 0, j = 0;

    ICM_ASSERT(decdbginfo != NULL);

    memset(decdbginfo, 0, arraysize * sizeof(ICM_SELDBG_DECISION_INFO_T));

    for(i = 0; i < arraysize; i++) {
        ICM_SELDBG_SETFIELD(decdbginfo[i].band, ICM_BAND_5G);
        ICM_SELDBG_SETFIELD(decdbginfo[i].physpec, physpec);
        ICM_SELDBG_SETFIELD(decdbginfo[i].width, ICM_CH_BW_80_PLUS_80);
        ICM_SELDBG_SETFIELD(decdbginfo[i].is_allrelchanprocd,
                false);
        ICM_SELDBG_SETFIELD(decdbginfo[i].rejreason,
                ICM_SELDBG_REJCODE_NOTREJ);
        /* We know that at a minimum, there will be some amount of primary
           channel info even if the selector algo bails out on this array
           entry early on. */
        ICM_SELDBG_SETFIELD(decdbginfo[i].prichaninfo.is_valid, true);

        ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.is_mwo_intf);
        ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].mwo_degrade_factor);
        ICM_SELDBG_MARKFIELD_NU(decdbginfo[i].prichaninfo.is_fhss_intf);
        ICM_SELDBG_MARKFIELD_NU(decdbginfo[i].fhss_degrade_factor);

        ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.center_chan_num);
        ICM_SELDBG_MARKFIELD_NA(decdbginfo[i].prichaninfo.center_freq);

        for(j = 0; j < ARRAY_LEN(decdbginfo[i].relchaninfo); j++) {
            ICM_SELDBG_SETFIELD(decdbginfo[i].relchaninfo[j].is_valid, false);

            ICM_SELDBG_MARKFIELD_NA(
                    decdbginfo[i].relchaninfo[j].is_mwo_intf);
            ICM_SELDBG_MARKFIELD_NU(
                    decdbginfo[i].relchaninfo[j].is_fhss_intf);
        }
    }

    return SUCCESS;
}

/*
 * Function     : icm_cfreq2_seldbg_process
 * Description  : process selection debug information for secondary 80 MHz
 *                center frequency.
 * Input params : pointer to dev info structure,
 *                pointer to array of ICM_SELDBG_DECISION_INFO_T entries,
 *                number of array members,
 *                PHY specification
 * Return       : SUCCESS or FAILURE
 */
int icm_cfreq2_seldbg_process(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize,
        ICM_PHY_SPEC_T physpec)
{
    int ret = SUCCESS;

    ICM_ASSERT(pdev != NULL);
    ICM_ASSERT(decdbginfo != NULL);

    /* XXX Carry out file dump, if required */

    icm_cfreq2_seldbg_consoledump(pdev, decdbginfo, arraysize, physpec);

    return ret;
}

/*
 * Function     : icm_cfreq2_seldbg_consoledump
 * Description  : dump secondary 80 MHz center frequency selection
 *                debug information to console/standard output.
 * Input params : pointer to dev info structure
 *                pointer to array of ICM_SELDBG_DECISION_INFO_T entries,
 *                number of array members,
 *                PHY specification
 * Return       : SUCCESS or FAILURE
 */
int icm_cfreq2_seldbg_consoledump(ICM_DEV_INFO_T* pdev,
        ICM_SELDBG_DECISION_INFO_T *decdbginfo,
        int arraysize,
        ICM_PHY_SPEC_T physpec)
{
    u_int16_t chan_idx = 0;
    ICM_SELDBG_CHAN_INFO_T  *prichandbginfo = NULL;
    ICM_SELDBG_CHAN_INFO_T  *relchandbginfo = NULL;

    ICM_ASSERT(pdev != NULL);
    ICM_ASSERT(decdbginfo != NULL);
    ICM_ASSERT(arraysize > 0);

    icm_seldbg_printlegend(pdev);

    /* For brevity, we print the Band, PHY Spec, and desired width only once
       at the beginning. By design, the values in the first entry of decdbginfo
       apply to all entries. Also, for simplicity, we do not check for invalid
       fields here since we are sure they have to be populated, else something
       is wrong with the implementation. The field protection is only for
       uniformity, in this case. */
    ICM_SELDBG_DRAW_CFREQ2_CDUMP_LINE(pdev);
    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "%-14s: %-66s|\n%-14s: %-66s|\n%-14s: %-66s|\n%-14s: %-66s|\n",
            "Band",
            icm_band_str[ICM_BAND_5G],
            "PHY Spec",
            icm_phy_spec_str[physpec],
            "Desired b/w",
            icm_ch_bw_str[ICM_CH_BW_80_PLUS_80],
            "Segment",
            "Secondary 80 MHz segment");
    ICM_SELDBG_DRAW_CFREQ2_CDUMP_LINE(pdev);

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_SELECTOR,
            "     |     |     |     |Num  |Max  |ETSI |     |Pena-|     |     |Rejection |Best |\n"
            "Pri  |Reltd|Reltn|Excld|OBSS |Reg  |Wthr |CW   |lized|Msrd |Usab |Code      |Chan |\n"
            "Chan |Chan |     |?    |     |Pwr  |?    |?    |?    |Usab |     |          |?    |\n");
    ICM_SELDBG_DRAW_CFREQ2_CDUMP_LINE(pdev);

    for (chan_idx = 0; chan_idx < arraysize; chan_idx++) {
        prichandbginfo = &(decdbginfo[chan_idx].prichaninfo);
        relchandbginfo = &(decdbginfo[chan_idx].relchaninfo[0]);
        /* Prints for primary channel and main decision parameters */

        /* Primary chan */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                prichandbginfo->chan_num,
                "%-*hu");

        /* Related chan */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                relchandbginfo->center_chan_num,
                "%-*hu");

        /* Relation */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                relchandbginfo->relation,
                icm_chan_relationship_shortstr);

        /* Excluded? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                relchandbginfo->is_excluded,
                icm_bool_shortstr);

        /* No. of APs in 80 MHz span */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].tot_num_aps_80w_80ppdu,
                "%-*d");

        /* Least value of max regulatory power (dBm) across 80 MHz span */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                relchandbginfo->maxregpower,
                "%-*hhd");

        /* ETSI Weather Radar channel? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                relchandbginfo->is_etsi_weather,
                icm_bool_shortstr);

        /* CW Inf found? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                relchandbginfo->is_cw_intf,
                icm_bool_shortstr);
        /* Is penalized? */
        /* Note: For the shortened console output, we assume that penalization
           if applied, applies everywhere.
XXX: Change this if no longer applicable in the future. */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].is_penalized,
                icm_bool_shortstr);

        /* Final measured usability */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].measured_usability,
                "%-*hu");

        /* XXX Add measured usability on extension channnel if required */

        /* Final computed usability  */
        ICM_SELDBG_PRINTENTRY_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].usability,
                "%-*hu");

        /* Rejection code */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS_WIDE(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].rejreason,
                icm_seldbg_rejcode_shortstr);

        /* Is best chan? */
        ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev,
                ICM_MODULE_ID_SELECTOR,
                decdbginfo[chan_idx].is_bestchan,
                icm_bool_shortstr);

        ICM_DPRINTF(pdev,
                ICM_PRCTRL_FLAG_NO_PREFIX,
                ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SELECTOR,
                "\n");
    }

    ICM_SELDBG_DRAW_CFREQ2_CDUMP_LINE(pdev);

    return SUCCESS;
}

