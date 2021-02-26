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
 *       Filename:  icm_scan.c
 *
 *    Description:  ICM Scan related functions
 *
 *        Version:  1.0
 *        Created:  04/19/2012 01:18:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <icm.h>
#include <sys/time.h>

#ifdef ICM_RTR_DRIVER
#include "ath_classifier.h"
#include "ieee80211_external.h"
#endif /* ICM_RTR_DRIVER */

#include "icm_api.h"
#include <errno.h>
#include <assert.h>
#include <math.h>
#include "icm_internal.h"

#define IW_ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))


#ifdef ICM_RTR_DRIVER
/*
 * Function     : icm_parse_and_update_htinfo
 * Description  : Parse and store HTINFO for the given BSS
 * Input params : pointer to icm, pointer to scan state, buffer and buffer len
 * Return       : void
 *
 */
void icm_parse_and_update_htinfo(ICM_INFO_T* picm, struct iwscan_state *state, unsigned char* iebuf, int buflen)
{
    struct ieee80211_ie_htinfo_cmn *pie;
    struct ieee80211_ie_htinfo htinfo;
    int chn_idx = 0;

    pie = &htinfo.hi_ie;
    memcpy(&htinfo, iebuf, sizeof(htinfo));

    if ((htinfo.hi_id == IEEE80211_ELEMID_HTINFO_ANA)) {

        /* Update the BSS HT Info */
        picm->slist.elem[state->ap_num].htinfo.is_valid             = 1;
        picm->slist.elem[state->ap_num].htinfo.control_channel      = pie->hi_ctrlchannel;
        picm->slist.elem[state->ap_num].htinfo.ext_channel_offset   = pie->hi_extchoff;
        picm->slist.elem[state->ap_num].htinfo.tx_channel_width     = pie->hi_txchwidth;
        picm->slist.elem[state->ap_num].htinfo.obss_nonht_present   = pie->hi_obssnonhtpresent;
        picm->slist.elem[state->ap_num].htinfo.tx_burst_limit       = pie->hi_txburstlimit;
        picm->slist.elem[state->ap_num].htinfo.non_gf_present       = pie->hi_nongfpresent;

        /*
         * Update Control and Extension Channel information in the Channel List
         * Prerequiste : The Channel list should be populated ahead of this
         */
        if (pie->hi_ctrlchannel) {

            ICM_CHANNEL_LIST_T* pchlist = NULL;
            int i = 0;
            int extension_channel = 0;

            /* Get the relevant operating band */
            if (picm->band == ICM_BAND_2_4G) {
                pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
            } else {
                pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
            }

            chn_idx = icm_get_channel_index(pchlist,
                    picm->slist.elem[state->ap_num].htinfo.control_channel);

            if (picm->slist.elem[state->ap_num].htinfo.tx_channel_width) {
                if (chn_idx != -1) {
                    pchlist->ch[chn_idx].num_wnw_40++;
                    pchlist->ch[chn_idx].num_wnw_20--;
                }
            }

            /* Get the Extension channel */
            if (pie->hi_extchoff == SEC_CHANNEL_OFFSET_ABOVE) {
                extension_channel = pie->hi_ctrlchannel + SEC_CHANNEL_OFFSET_ABOVE_IDX;
            } else if (pie->hi_extchoff == SEC_CHANNEL_OFFSET_BELOW) {
                extension_channel = pie->hi_ctrlchannel + SEC_CHANNEL_OFFSET_BELOW_IDX;
            } else {
                extension_channel = 0;
            }

            /* If Extension channel is present, mark it */
            if (extension_channel) {
                for(i = 0; i < pchlist->count; i++) {
                    if (pchlist->ch[i].channel == extension_channel) {
                        pchlist->ch[i].used_as_secondary_20++;
                        break;
                    }
                }
            } /* End of If */
        } /* End of If */
    } /* End of If */
}
#endif /* ICM_RTR_DRIVER */

/*
 * Function     : icm_mark_secondary_40MHz_channels
 * Description  : Based VHTOP and HTINFO IEs,mark the secondary 40 channels
 *                Used only for 160 MHz operation
 * Input params : pointer to icm, pointer to scan state, control channe, segment center freq,
 *                extension channel offset
 * Return       : void
 *
 */
void icm_mark_secondary_40MHz_channels(ICM_INFO_T* picm,
        struct iwscan_state *state,
        int ctrl_channel,
        int segment_cf,
        int ext_chan_offset)
{

    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_CHANNEL_LIST_T *pchlist = NULL;
    ICM_CHANNEL_T *ch = NULL;
    int i = 0;
    int extension_channel = 0;
    int start_channel = 0;;
    int end_channel = 0;
    int chn_idx = 0;

    ICM_ASSERT(picm->band == ICM_BAND_5G);

    /* Get the relevant operating band.
     * However the VHT is only valid for 5GHz band
     */
    if (picm->band == ICM_BAND_2_4G) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }


    /* Get the extension channel */
    if (ext_chan_offset == SEC_CHANNEL_OFFSET_ABOVE) {
        extension_channel = ctrl_channel + SEC_CHANNEL_OFFSET_ABOVE_IDX;
    } else if (ext_chan_offset == SEC_CHANNEL_OFFSET_BELOW) {
        extension_channel = ctrl_channel + SEC_CHANNEL_OFFSET_BELOW_IDX;
    } else {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT,
                ICM_MODULE_ID_SCAN,
                "Warning: Invalid extension channel indicated by OBSS. "
                "Guessing from regulatory rules.\n");
        chn_idx = icm_get_channel_index(pchlist,
                picm->slist.elem[state->ap_num].htinfo.control_channel);
        /*
         * In case we are not capable of using the channel advertised by the external AP,
         * then we need not and cannot increment the relevant stats. We just return back
         * to the caller
         */
        if (chn_idx < 0) {
            return;
        }
        ch = &pchlist->ch[chn_idx];

        /* Note: A check of HT40PLUS suffices here, since we are only querying
         * regulatory for our guesswork. We need not have equivalent checks for
         * other PHYs like 11ac/11ax.
         */
        if (IEEE80211_IS_CHAN_11NA_HT40PLUS(ch)) {
            extension_channel = ctrl_channel + SEC_CHANNEL_OFFSET_ABOVE_IDX;
        } else {
            extension_channel = ctrl_channel + SEC_CHANNEL_OFFSET_BELOW_IDX;
        }
    }

    if (ctrl_channel < segment_cf) {
        start_channel = segment_cf - VHT_160MHZ_CHANNEL_BOUNDARY_OFFSET;

        /* Loop through the channel list and find the channels
         * between start_channel and segment_cf.
         */

        for (i = 0; i < pchlist->count; i++) {
            if ((pchlist->ch[i].channel >= start_channel) &&
                    (pchlist->ch[i].channel <= segment_cf)) {

                if ((pchlist->ch[i].channel != ctrl_channel) &&
                        (pchlist->ch[i].channel != extension_channel)) {
                    pchlist->ch[i].used_as_160_80p80_secondary_40++;
                }
            }
        }

    } else if (ctrl_channel > segment_cf) {
        end_channel = segment_cf + VHT_160MHZ_CHANNEL_BOUNDARY_OFFSET;

        /* Loop through the channel list and find the channels
         * between start_channel and segment_cf.
         */
        for (i = 0; i < pchlist->count; i++) {
            if ((pchlist->ch[i].channel >= segment_cf) &&
                    (pchlist->ch[i].channel <= end_channel)) {

                if ((pchlist->ch[i].channel != ctrl_channel) &&
                        (pchlist->ch[i].channel != extension_channel)) {
                    pchlist->ch[i].used_as_160_80p80_secondary_40++;

                }
            }
        } /* End of for */
    } /* End of Else if */
}

/*
 * Function     : icm_analyze_vht_channels
 * Description  : Based VHTOP and HTINFO IEs, analyze the usage pattern of VHT Channels
 * Input params : pointer to icm, pointer to scan state
 * Return       : void
 *
 */
void icm_analyze_vht_channels(ICM_INFO_T* picm, struct iwscan_state *state)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_CHANNEL_LIST_T *pchlist = NULL;
    int i = 0;
    int chn_idx = 0;

    ICM_ASSERT(picm->band == ICM_BAND_5G);

    /* Get the relevant operating band.
     * However the VHT is only valid for 5GHz band
     */
    if (picm->band == ICM_BAND_2_4G) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    chn_idx = icm_get_channel_index(pchlist,
            picm->slist.elem[state->ap_num].htinfo.control_channel);

    if ((picm->slist.elem[state->ap_num].htinfo.is_valid) &&
            (picm->slist.elem[state->ap_num].vhtop.is_valid)) {

        switch (picm->slist.elem[state->ap_num].vhtop.channel_width) {
            case VHT_CHANNEL_WIDTH_20MHZ_OR_40MHZ:
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "VHT 20/40MHz (No Sec 40)\n");
                break;
            case VHT_CHANNEL_WIDTH_80MHZ:
                {
                    if (chn_idx != -1) {
                        pchlist->ch[chn_idx].num_wnw_80++;
                        pchlist->ch[chn_idx].num_wnw_40--;
                    }
                    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "VHT 80MHz (No Sec 40)\n");
                }
                break;
            case VHT_CHANNEL_WIDTH_160_MHZ:
                {
                    int ctrl_channel       = picm->slist.elem[state->ap_num].htinfo.control_channel;
                    int segment_cf         = picm->slist.elem[state->ap_num].vhtop.channel_cf_seg1;
                    int ext_chan_offset    = picm->slist.elem[state->ap_num].htinfo.ext_channel_offset;
                    /* Mark all 40MHz Secondary Channels */
                    icm_mark_secondary_40MHz_channels(picm, state, ctrl_channel, segment_cf, ext_chan_offset);

                    if (chn_idx != -1) {
                        pchlist->ch[chn_idx].num_wnw_160++;
                        pchlist->ch[chn_idx].num_wnw_40--;
                    }

                    for (i = 0; i < pchlist->count; i++)
                    {
                        if (((ctrl_channel < segment_cf) &&
                                    (pchlist->ch[i].channel >= (segment_cf + 2)) &&
                                    (pchlist->ch[i].channel <= (segment_cf + 14))) ||
                                ((ctrl_channel > segment_cf) &&
                                 (pchlist->ch[i].channel <= (segment_cf - 2)) &&
                                 (pchlist->ch[i].channel >= (segment_cf - 14)))) {
                            pchlist->ch[i].used_as_160_secondary_80++;
                        }
                    }

                }
                break;
            case VHT_CHANNEL_WIDTH_80MHZ_PLUS_80MHZ:
                {

                    int ctrl_channel       = picm->slist.elem[state->ap_num].htinfo.control_channel;
                    int segment_one_cf     = picm->slist.elem[state->ap_num].vhtop.channel_cf_seg1;
                    int segment_two_cf     = picm->slist.elem[state->ap_num].vhtop.channel_cf_seg2;

                    if (chn_idx != -1) {
                        pchlist->ch[chn_idx].num_wnw_80_80++;
                        pchlist->ch[chn_idx].num_wnw_40--;
                    }

                    if (ctrl_channel < segment_one_cf) {
                        for (i = 0; i < pchlist->count; i++) {
                            if ((pchlist->ch[i].channel == (segment_one_cf + 2)) ||
                                    (pchlist->ch[i].channel == (segment_one_cf + 6))) {
                                pchlist->ch[i].used_as_160_80p80_secondary_40++;
                            }
                        }
                    } else if (ctrl_channel > segment_one_cf) {
                        for (i = 0; i < pchlist->count; i++) {
                            if ((pchlist->ch[i].channel == (segment_one_cf - 2)) ||
                                    (pchlist->ch[i].channel == (segment_one_cf - 6))) {
                                pchlist->ch[i].used_as_160_80p80_secondary_40++;
                            }
                        }
                    } else {
                        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "Unknown VHTOP and HTINFO Combination");
                    }

                    for (i = 0; i < pchlist->count; i++)
                    {
                        if ((pchlist->ch[i].channel == (segment_two_cf + 2)) ||
                                (pchlist->ch[i].channel == (segment_two_cf + 6)) ||
                                (pchlist->ch[i].channel == (segment_two_cf - 2)) ||
                                (pchlist->ch[i].channel == (segment_two_cf - 6))) {
                            pchlist->ch[i].used_as_80p80_secondary_80++;
                        }
                    }
                }
                break;
            default:
                break;
        }

    }
}
#ifdef ICM_RTR_DRIVER

/*
 * Function     : icm_parse_and_update_vhtop
 * Description  : Parse and store VHTOP for the given BSS
 * Input params : pointer to icm, pointer to scan state, buffer and buffer len
 * Return       : void
 *
 */
void icm_parse_and_update_vhtop(ICM_INFO_T* picm, struct iwscan_state *state, unsigned char *iebuf, int buflen)
{
    struct ieee80211_ie_vhtop *p = (struct ieee80211_ie_vhtop *)iebuf;

    if (p->elem_id == IEEE80211_ELEMID_VHTOP) {
        /* Update the BSS VHTOP Information */
        picm->slist.elem[state->ap_num].vhtop.is_valid          = 1;

        if (ICM_IS_REVSIG_VHT160(p) || ICM_IS_REVSIG_VHT80_80(p)) {
            if (ICM_IS_REVSIG_VHT160(p)) {
                picm->slist.elem[state->ap_num].vhtop.channel_width     = VHT_CHANNEL_WIDTH_160_MHZ;
                picm->slist.elem[state->ap_num].vhtop.channel_cf_seg1   = p->vht_op_ch_freq_seg2;
                picm->slist.elem[state->ap_num].vhtop.channel_cf_seg2   = 0;
            }
        } else {
            picm->slist.elem[state->ap_num].vhtop.channel_width     = VHT_CHANNEL_WIDTH_80MHZ_PLUS_80MHZ;
            picm->slist.elem[state->ap_num].vhtop.channel_cf_seg1   = p->vht_op_ch_freq_seg1;
            picm->slist.elem[state->ap_num].vhtop.channel_cf_seg2   = p->vht_op_ch_freq_seg2;
        }
    } else {
        picm->slist.elem[state->ap_num].vhtop.channel_width     = p->vht_op_chwidth;
        picm->slist.elem[state->ap_num].vhtop.channel_cf_seg1   = p->vht_op_ch_freq_seg1;
        picm->slist.elem[state->ap_num].vhtop.channel_cf_seg2   = p->vht_op_ch_freq_seg2;

    }

    /* Mark Secondary 40MHz channels */
    icm_analyze_vht_channels(picm, state);

}

#else /* ICM_RTR_DRIVER */
/*
 * Function     : icm_cfg_parse_and_update_htinfo
 * Description  : Parse and store HTINFO for the given BSS
 * Input params : pointer to icm, pointer to scan state, buffer and buffer len
 * Return       : void
 *
 */
void icm_cfg_parse_and_update_htinfo(ICM_INFO_T* picm, struct iwscan_state *state, u8* ie)
{
    u8 *pos = ie;       /* Type */
    struct ieee80211_ht_operation *oper = pos + 2; /* value */
    ICM_HTINFO_T *htop = &picm->slist.elem[state->ap_num].htinfo;
    int chn_idx = 0;

    htop->is_valid = TRUE;
    htop->control_channel = oper->primary_chan;
    htop->ext_channel_offset = (oper->ht_param & HT_INFO_HT_PARAM_SECONDARY_CHNL_OFF_MASK);
    htop->tx_channel_width = (oper->ht_param & HT_INFO_HT_PARAM_STA_CHNL_WIDTH);
    htop->obss_nonht_present = (oper->operation_mode & HT_OPER_OP_MODE_OBSS_NON_HT_STAS_PRESENT);
    htop->tx_burst_limit = (oper->operation_mode & ((u16) BIT(3))); /* no definition in ieee802_11_defs.h */
    htop->non_gf_present = (oper->operation_mode & HT_OPER_OP_MODE_NON_GF_HT_STAS_PRESENT);

    /*
     * Update Control and Extension Channel information in the Channel List
     * Prerequiste : The Channel list should be populated ahead of this
     */
    if (htop->control_channel) {

        ICM_CHANNEL_LIST_T* pchlist = NULL;
        int i = 0;
        int extension_channel = 0;

        /* Get the relevant operating band */
        if (picm->band == ICM_BAND_2_4G) {
            pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
        } else {
            pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
        }

        chn_idx = icm_get_channel_index(pchlist, htop->control_channel);

        if (htop->tx_channel_width) {
            if (chn_idx != -1) {
                pchlist->ch[chn_idx].num_wnw_40++;
                pchlist->ch[chn_idx].num_wnw_20--;
            }
        }

        /* Get the Extension channel */
        if (htop->ext_channel_offset == SEC_CHANNEL_OFFSET_ABOVE) {
            extension_channel = htop->control_channel + SEC_CHANNEL_OFFSET_ABOVE_IDX;
        } else if (htop->ext_channel_offset == SEC_CHANNEL_OFFSET_BELOW) {
            extension_channel = htop->control_channel + SEC_CHANNEL_OFFSET_BELOW_IDX;
        } else {
            extension_channel = 0;
        }

        /* If Extension channel is present, mark it */
        if (extension_channel) {
            for(i = 0; i < pchlist->count; i++) {
                if (pchlist->ch[i].channel == extension_channel) {
                    pchlist->ch[i].used_as_secondary_20++;
                    break;
                }
            }
        }
    }
}

/*
 * Function     : icm_parse_and_update_vhtop
 * Description  : Parse and store VHTOP for the given BSS
 * Input params : pointer to icm, pointer to scan state, buffer and buffer len
 * Return       : void
 *
 */
void icm_cfg_parse_and_update_vhtop(ICM_INFO_T* picm, struct iwscan_state *state, u8 *ie)
{
    u8 *pos = ie;       /* Type */
    struct ieee80211_vht_operation *oper = pos + 2; /* Value */
    ICM_VHTOP_T *vhtop = &picm->slist.elem[state->ap_num].vhtop;

    vhtop->is_valid = TRUE;

    if (ICM_IS_REVSIG_VHT160(oper) || ICM_IS_REVSIG_VHT80_80(oper)) {
        if (ICM_IS_REVSIG_VHT160(oper)) {
            vhtop->channel_width     = VHT_CHANNEL_WIDTH_160_MHZ;
            vhtop->channel_cf_seg1   = oper->vht_op_info_chan_center_freq_seg1_idx;
            vhtop->channel_cf_seg2   = 0;
        } else {
            vhtop->channel_width     = VHT_CHANNEL_WIDTH_80MHZ_PLUS_80MHZ;
            vhtop->channel_cf_seg1   = oper->vht_op_info_chan_center_freq_seg0_idx;
            vhtop->channel_cf_seg2   = oper->vht_op_info_chan_center_freq_seg1_idx;
        }
    } else {
        vhtop->channel_width     = oper->vht_op_info_chwidth;
        vhtop->channel_cf_seg1   = oper->vht_op_info_chan_center_freq_seg0_idx;
        vhtop->channel_cf_seg2   = oper->vht_op_info_chan_center_freq_seg1_idx;

    }

    /* Mark Secondary 40MHz channels */
    icm_analyze_vht_channels(picm, state);
    return;
}
#endif /* ICM_RTR_DRIVER */

/*
 * Function     : icm_analyze_he_channels
 * Description  : Based on HE VHTOP and HTINFO IEs, analyze the usage pattern of
 *                HE Channels.
 * Input params : pointer to icm, pointer to scan state
 * Return       : void
 *
 */
void icm_analyze_he_channels(ICM_INFO_T* picm, struct iwscan_state *state)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_CHANNEL_LIST_T *pchlist = NULL;
    int i = 0;
    int chn_idx = 0;
    ICM_HTINFO_T *htinfo = NULL;
    ICM_HEVHTOP_T *hevhtop = NULL;

    ICM_ASSERT(picm != NULL && state != NULL);

    /* Get the relevant operating band.
     */
    if (picm->band == ICM_BAND_2_4G) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    ICM_ASSERT(pchlist != NULL);

    htinfo = &(picm->slist.elem[state->ap_num].htinfo);
    hevhtop = &(picm->slist.elem[state->ap_num].hevhtop);

    if (htinfo->is_valid && hevhtop->is_valid) {
        chn_idx = icm_get_channel_index(pchlist, htinfo->control_channel);

        switch (hevhtop->channel_width) {
            case VHT_CHANNEL_WIDTH_20MHZ_OR_40MHZ:
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT,
                        ICM_MODULE_ID_SCAN, "HE 20/40MHz (No Sec 40)\n");
                break;
            case VHT_CHANNEL_WIDTH_80MHZ:
                {
                    if (chn_idx != -1) {
                        pchlist->ch[chn_idx].num_wnw_80++;
                        pchlist->ch[chn_idx].num_wnw_40--;
                    }
                    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE,
                            ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN,
                            "HE 80MHz (No Sec 40)\n");
                }
                break;
            case VHT_CHANNEL_WIDTH_160_MHZ:
                {
                    int ctrl_channel = htinfo->control_channel;
                    int segment_cf = hevhtop->channel_cf_seg1;
                    int ext_chan_offset = htinfo->ext_channel_offset;

                    /* Mark all 40MHz Secondary Channels */
                    icm_mark_secondary_40MHz_channels(picm, state, ctrl_channel,
                            segment_cf, ext_chan_offset);

                    if (chn_idx != -1) {
                        pchlist->ch[chn_idx].num_wnw_160++;
                        pchlist->ch[chn_idx].num_wnw_40--;
                    }

                    for (i = 0; i < pchlist->count; i++)
                    {
                        if (((ctrl_channel < segment_cf) &&
                               (pchlist->ch[i].channel >= (segment_cf + 2)) &&
                               (pchlist->ch[i].channel <= (segment_cf + 14))) ||
                            ((ctrl_channel > segment_cf) &&
                               (pchlist->ch[i].channel <= (segment_cf - 2)) &&
                               (pchlist->ch[i].channel >= (segment_cf - 14)))) {
                            pchlist->ch[i].used_as_160_secondary_80++;
                        }
                    }

                }
                break;
            case VHT_CHANNEL_WIDTH_80MHZ_PLUS_80MHZ:
                {
                    int ctrl_channel = htinfo->control_channel;
                    int segment_one_cf = hevhtop->channel_cf_seg1;
                    int segment_two_cf = hevhtop->channel_cf_seg2;

                    if (chn_idx != -1) {
                        pchlist->ch[chn_idx].num_wnw_80_80++;
                        pchlist->ch[chn_idx].num_wnw_40--;
                    }

                    if (ctrl_channel < segment_one_cf) {
                        for (i = 0; i < pchlist->count; i++) {
                            if ((pchlist->ch[i].channel ==
                                    (segment_one_cf + 2)) ||
                                (pchlist->ch[i].channel ==
                                    (segment_one_cf + 6))) {
                                pchlist->ch[i].used_as_160_80p80_secondary_40++;
                            }
                        }
                    } else if (ctrl_channel > segment_one_cf) {
                        for (i = 0; i < pchlist->count; i++) {
                            if ((pchlist->ch[i].channel ==
                                    (segment_one_cf - 2)) ||
                                (pchlist->ch[i].channel ==
                                     (segment_one_cf - 6))) {
                                pchlist->ch[i].used_as_160_80p80_secondary_40++;
                            }
                        }
                    } else {
                        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE,
                                ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN,
                                "Unknown HE-VHTOP and HTINFO Combination");
                    }

                    for (i = 0; i < pchlist->count; i++)
                    {
                        if ((pchlist->ch[i].channel == (segment_two_cf + 2)) ||
                            (pchlist->ch[i].channel == (segment_two_cf + 6)) ||
                            (pchlist->ch[i].channel == (segment_two_cf - 2)) ||
                            (pchlist->ch[i].channel == (segment_two_cf - 6))) {
                            pchlist->ch[i].used_as_80p80_secondary_80++;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
}

/*
 * Function     : icm_parse_and_update_hecap
 * Description  : Parse and store values of interest in HE Cap for the given
 *                BSS. The caller is expected to have verified that the buffer
 *                being passed contains a valid HE Cap.
 * Input params : pointer to icm, pointer to scan state, buffer containing IE
 * Return       : void
 */
void icm_parse_and_update_hecap(ICM_INFO_T* picm, struct iwscan_state *state,
        u8 *iebuf)
{
    struct ieee80211_ie_hecapabilities *hecap = NULL;
    u_int8_t *hecap_phy_section;
    ICM_CHANNEL_LIST_T *pchlist = NULL;
    int chn_idx = 0;
    ICM_SCAN_INFO_T *scaninfo = NULL;

    ICM_ASSERT(picm != NULL && state != NULL && iebuf != NULL);

    scaninfo = &(picm->slist.elem[state->ap_num]);

    hecap = (struct ieee80211_ie_hecapabilities *)iebuf;
    hecap_phy_section = &hecap->hecap_phyinfo[0];

    scaninfo->hecap.is_valid = 1;

    scaninfo->hecap.srp_supported =
        HECAPBLTS_PHY_GET_SRPALLOWED(&hecap_phy_section);

    /* Get the relevant operating band.
     */
    if (picm->band == ICM_BAND_2_4G) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    ICM_ASSERT(pchlist != NULL);

    chn_idx = icm_get_channel_index(pchlist, scaninfo->channel);

    if (scaninfo->hecap.srp_supported) {
        pchlist->ch[chn_idx].num_wnw_srp_supported++;
    }
}

/*
 * Function     : icm_parse_and_update_hevhtop
 * Description  : Parse and store HE VHTOP for the given BSS. The caller is
 *                expected to have verified that the buffer being passed
 *                contains a valid HE VHTOP.
 * Input params : pointer to icm, pointer to scan state, buffer
 * Return       : void
 *
 */
void icm_parse_and_update_hevhtop(ICM_INFO_T* picm, struct iwscan_state *state,
        u8 *iebuf)
{
    struct ieee80211_ie_heoperation *heop = NULL;
    ICM_HEVHTOP_T *hevhtop = NULL;

    ICM_ASSERT(picm != NULL && state != NULL && iebuf != NULL);

    heop = (struct ieee80211_ie_heoperation *)iebuf;
    hevhtop = &(picm->slist.elem[state->ap_num].hevhtop);

    hevhtop->is_valid = 1;

    /* 11AX TODO: While the old VHTOP signalling (pre non-0 CCFS1) is
     * deprecated, for completeness we still provision for some APs that might
     * use it for some time.  Phase this out once the 802.11ax standard is
     * finalized.
     */
    if (ICM_IS_REVSIG_HE160(&(heop->hevhtop)) ||
            ICM_IS_REVSIG_HE80_80(&(heop->hevhtop))) {
        if (ICM_IS_REVSIG_HE160(&(heop->hevhtop))) {
           hevhtop->channel_width = VHT_CHANNEL_WIDTH_160_MHZ;
           hevhtop->channel_cf_seg1 =
               heop->hevhtop.vht_op_info_chan_center_freq_seg1_idx;
           hevhtop->channel_cf_seg2  = 0;
        } else {
            hevhtop->channel_width = VHT_CHANNEL_WIDTH_80MHZ_PLUS_80MHZ;
            hevhtop->channel_cf_seg1 =
                heop->hevhtop.vht_op_info_chan_center_freq_seg0_idx;
            hevhtop->channel_cf_seg2 =
                heop->hevhtop.vht_op_info_chan_center_freq_seg1_idx;
        }
    } else {
        hevhtop->channel_width = heop->hevhtop.vht_op_info_chwidth;
        hevhtop->channel_cf_seg1 =
            heop->hevhtop.vht_op_info_chan_center_freq_seg0_idx;
        hevhtop->channel_cf_seg2 =
            heop->hevhtop.vht_op_info_chan_center_freq_seg1_idx;
    }

    /* Mark Secondary 40MHz channels */
    icm_analyze_he_channels(picm, state);
}

/*
 * Function     : icm_parse_and_update_srp
 * Description  : Parse and store values of interest in SRP IE for the given
 *                BSS. The caller is expected to have verified that the buffer
 *                being passed contains a valid SRP IE.
 * Input params : pointer to icm, pointer to scan state, buffer containing IE
 * Return       : void
 *
 */
void icm_parse_and_update_srp(ICM_INFO_T* picm, struct iwscan_state *state,
        unsigned char *iebuf)
{
    struct ieee80211_sr_paramset *srp = NULL;
    ICM_CHANNEL_LIST_T *pchlist = NULL;
    int chn_idx = 0;
    ICM_SCAN_INFO_T *scaninfo = NULL;

    ICM_ASSERT(picm != NULL && state != NULL && iebuf != NULL);

    scaninfo = &(picm->slist.elem[state->ap_num]);

    srp = (struct ieee80211_sr_paramset *)iebuf;

    scaninfo->srp.is_valid = 1;
    scaninfo->srp.client_srp_disallowed =
        srp->sr_control & SRP_SRCONTROL_SRP_DISALLOW_MASK;
    scaninfo->srp.client_obsspd_disallowed =
        srp->sr_control & SRP_SRCONTROL_OBSSPD_DISALLOW_MASK;

    /* Get the relevant operating band.
     */
    if (picm->band == ICM_BAND_2_4G) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    ICM_ASSERT(pchlist != NULL);

    chn_idx = icm_get_channel_index(pchlist, scaninfo->channel);

    if ((chn_idx >= 0) && !(scaninfo->srp.client_srp_disallowed)) {
        pchlist->ch[chn_idx].num_wnw_client_srp_allowed++;
    }

    if ((chn_idx >= 0) && !(scaninfo->srp.client_obsspd_disallowed)) {
        pchlist->ch[chn_idx].num_wnw_client_obsspd_allowed++;
    }
}

/*
 * Function     : icm_parse_gen_ie
 * Description  : Check IWGENIE for HTINFO and VHTOP IEs
 * Input params : pointer to icm, pointer to scan state, buffer and buffer len
 * Return       : void
 *
 */
void icm_parse_gen_ie(ICM_INFO_T *picm, struct iwscan_state *state, unsigned char* buffer, int buflen)
{
    int offset = 0;

    /* Loop on each IE, each IE is minimum 2 bytes */
    while (offset <= (buflen - 2)) {

        /* Check IE type */
        switch (buffer[offset]) {
#ifdef ICM_RTR_DRIVER
            case IEEE80211_ELEMID_HTINFO_ANA:
                icm_parse_and_update_htinfo(picm, state, buffer + offset, buflen);
                break;
            case IEEE80211_ELEMID_VHTOP:
                /* 11AX TODO: Currently, we expect the 11ax VHTOP to be in sync
                 * with the 11ac VHTOP. So we won't parse one if the other has
                 * already been parsed. However, revisit this if exceptions are
                 * seen in future 11ax drafts.
                 */
                if ((picm->band == ICM_BAND_5G) &&
                      (picm->slist.elem[state->ap_num].hevhtop.is_valid == 0)) {
                    icm_parse_and_update_vhtop(picm,
                            state,
                            buffer + offset,
                            buflen);
                }
                break;
#else /* ICM_RTR_DRIVER */
            case WLAN_EID_HT_OPERATION:
                icm_cfg_parse_and_update_htinfo(picm, state, buffer + offset);
                break;
            case WLAN_EID_VHT_OPERATION:
                /* 11AX TODO: Currently, we expect the 11ax VHTOP to be in sync
                 * with the 11ac VHTOP. So we won't parse one if the other has
                 * already been parsed. However, revisit this if exceptions are
                 * seen in future 11ax drafts.
                 */
                if ((picm->band == ICM_BAND_5G) &&
                      (picm->slist.elem[state->ap_num].hevhtop.is_valid == 0)) {
                    icm_cfg_parse_and_update_vhtop(picm, state, buffer + offset);
                }
                break;
#endif /* ICM_RTR_DRIVER */
            case WLAN_EID_VENDOR:
                if (ICM_IS_HE_CAP_OUI(buffer + offset)) {
                    icm_parse_and_update_hecap(picm, state, buffer + offset);
                } else if (ICM_IS_HE_OP_OUI(buffer + offset) &&
                      (picm->slist.elem[state->ap_num].vhtop.is_valid == 0)) {
                    /* 11AX TODO: Currently, we expect the 11ax VHTOP to be in
                     * sync with the 11ac VHTOP. So we won't parse one if the
                     * other has already been parsed. However, revisit this if
                     * exceptions are seen in future 11ax drafts.
                     */
                    icm_parse_and_update_hevhtop(picm, state, buffer + offset);
                }

                break;
            case WLAN_EID_EXTN_ELEM:
                /* Check Element ID Extension number */
                if (buffer[offset + 2] == WLAN_EXTN_EID_SRP)
                {
                   icm_parse_and_update_srp(picm, state, buffer + offset);
                }
                break;
            default:
                break;
        }
        /* Skip over this IE to the next one in the list. */
        offset += buffer[offset + 1] + 2;
    }
}

/*
 * Function     : icm_get_event_descriptor
 * Description  : get event descriptors (original and adjusted). The original MUST
 *                must be treated as a const.
 * Input params : command ID, Wireless Extensions version
 * Output params: pointer to filled in ICM_EVENT_DESCRIPTOR_ADJ_T
 * Return       : on success: pointer to const ICM_EVENT_DESCRIPTOR_T
 *                on failure: NULL.
 */
    ICM_EVENT_DESCRIPTOR_T const*
icm_get_event_descriptor(int cmd,
        int we_ver,
        ICM_EVENT_DESCRIPTOR_ADJ_T *desc_adj)
{
    ICM_EVENT_DESCRIPTOR_T const *desc = NULL;
    int index = 0;

    if (desc_adj == NULL) {
        return NULL;
    }

    if (cmd <= SIOCIWLAST && cmd >= SIOCIWFIRST)
    {
        index = cmd - SIOCIWFIRST;
        if (index < IW_ARRAY_LEN(we_ioc_descriptors)) {
            desc = &(we_ioc_descriptors[index]);
        }
    } else if (cmd >= IWEVFIRST){
        index = cmd - IWEVFIRST;
        if (index < IW_ARRAY_LEN(we_ev_descriptors)) {
            desc = &(we_ev_descriptors[index]);
        }
    }

    if (desc == NULL) {
        return NULL;
    }

    /* Adjustments */
    desc_adj->ev_len  = desc->ev_len;

    if((we_ver <= 18) && (desc->ev_type == ICM_EV_TYPE_POINT)) {
        desc_adj->ev_len += IW_EV_POINT_OFF;
    }

    return desc;
}

/*
 * Function     : icm_get_supported_channels
 * Description  : retrive supported channels from the driver
 *                and classify into 11bg and 11a channels
 * Input params : pointer to icm
 * Return       : void
 *
 */
int icm_get_supported_channels(ICM_INFO_T* picm)
{
#ifdef ICM_RTR_DRIVER
    struct ieee80211req_chaninfo chans;
#endif /* ICM_RTR_DRIVER */
    double  freq;
    int i = 0,j = 0,k = 0;
    int channel;
    int status = SUCCESS;
    ICM_DEV_INFO_T* pdev = get_pdev();

    /* For MBL directly jump to processing of channel. supported channel
     * is already available as part of capability info */
    if (pdev->conf.walflag == ICM_WAL_CFG)
        goto process_chlist;

#ifdef ICM_RTR_DRIVER
    /* init the channel list to zero */
    picm->chlist.count = 0;

    if (icm_get_currdomain(picm) < 0) {
        fprintf(stderr, "%8.16s  no regulatory information.\n\n", picm->radio_ifname);
        return FAILURE;
    } else {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%-8.16s  resolved regulatory domain=%d\n", picm->radio_ifname, picm->rfreg_domain);

        /* This uses the private ioctl as SIOCGIWRANGE can only return 32
         * channels. In some regulatory domains, this is insufficient for a
         * DBSR device (where both 2.4 GHz and 5 GHz channels are returned).
         */
        if (icm_get_80211_priv(picm, picm->dev_ifname, IEEE80211_IOCTL_GETCHANINFO, &chans, sizeof(chans)) < 0) {
            fprintf(stderr, "%-8.16s  no frequency information.\n\n", picm->dev_ifname);
            status = FAILURE;
        } else {
            if (chans.ic_nchans > 0) {
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%-8.16s  %d available frequencies :\n", picm->dev_ifname, chans.ic_nchans);

                for (k = 0; k < chans.ic_nchans; k++) {

                    // Convert frequency from MHz to Hz
                    freq = ((double) chans.ic_chans[k].ic_freq) * 1000000;
                    channel = icm_convert_mhz2channel(chans.ic_chans[k].ic_freq);
                    picm->chlist.ch[k].channel = channel;
                    picm->chlist.ch[k].freq = freq;

                    // Special handling for channels which get auto-excluded
                    // due to them not being available when operating in AP
                    // mode.
                    if (icm_is_non_ap_channel(picm, channel)) {
                        ICM_SET_CHANNEL_EXCLUDE(picm, channel);
                    }

                    picm->chlist.count++;

                    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%-8.16s  Frequency %f Hz (Channel %d) (Auto Excluded=%d)\n",
                            picm->dev_ifname, freq, channel,
                            ICM_GET_CHANNEL_EXCLUDE(picm, channel));

                }
            } else {
                status = FAILURE;
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "No channel information available\n");
            }
        }
    }
#endif /* ICM_RTR_DRIVER */
process_chlist:
    for (k = 0; k < picm->chlist.count; k++) {
        if (IS_11BG_CHANNEL(picm->chlist.ch[k].channel)) {
            picm->chlist.ch[k].band = ICM_BAND_2_4G;
        } else {
            picm->chlist.ch[k].band = ICM_BAND_5G;
        }
    }

    if ((picm->pcl != NULL) && (picm->pcl->len > 0) &&
        (picm->pcl->policy == QCA_WLAN_VENDOR_ATTR_EXTERNAL_ACS_POLICY_PCL_MANDATORY)) {
        for (i = 0; i < picm->chlist.count; i++) {
            for (j = 0; j < picm->pcl->len; j++) {
                /* Exclude channels which have zero PCL weight and policy is PCL_MANDATORY */
                if (picm->pcl->list[j] == picm->chlist.ch[i].channel) {
                    if (picm->pcl->weight[j] == 0)
                        ICM_SET_CHANNEL_EXCLUDE(picm, picm->pcl->list[j]);
                    break;
                }
            }
            /* Exclude channels which are not in PCL and policy is PCL_MANDATORY */
            if (j == picm->pcl->len)
                ICM_SET_CHANNEL_EXCLUDE(picm, picm->chlist.ch[i].channel);
        }
    }

    /* Sort 11BG and 11A Channels */
    for (k = 0, i = 0, j = 0; k < picm->chlist.count; k++) {
        if (picm->chlist.ch[k].channel <= MAX_11BG_CHANNEL) {
            if (picm->chlist.ch[k].channel > 0) {
                memcpy(&picm->chlist_bg.ch[i], &picm->chlist.ch[k], sizeof(ICM_CHANNEL_T));
                picm->chlist_bg.ch[i].band = ICM_BAND_2_4G;

                /*
                 * In 11BG, channels 1,6,11 are
                 * non-overlapping channels, spectral will be
                 * done on only these channels
                 */

                if ((picm->chlist_bg.ch[i].channel == 1) ||
                        (picm->chlist_bg.ch[i].channel == 6) ||
                        (picm->chlist_bg.ch[i].channel == 11))
                {

                    picm->chlist_bg.ch[i].non_overlapping_ch = TRUE;

                    if (!pdev->conf.server_mode) {
                        if ((picm->channel_width == ICM_CH_BW_40PLUS) &&
                                (picm->chlist_bg.ch[i].channel == 11)) {
                            picm->chlist_bg.ch[i].non_overlapping_ch = FALSE;
                        }
                        if ((picm->channel_width == ICM_CH_BW_40MINUS) &&
                                (picm->chlist_bg.ch[i].channel == 1)) {
                            picm->chlist_bg.ch[i].non_overlapping_ch = FALSE;
                        }
                    }

                } else {
                    picm->chlist_bg.ch[i].non_overlapping_ch = FALSE;
                }
                i++;
            }
        } else {
            memcpy(&picm->chlist_a.ch[j], &picm->chlist.ch[k], sizeof(ICM_CHANNEL_T));
            picm->chlist_a.ch[j].band = ICM_BAND_5G;
            /* By default, enabling spectral channel for all
             * channel for the time being
             */
            picm->chlist_a.ch[j].non_overlapping_ch = TRUE;
            j++;
        }
    }
    picm->chlist_bg.count = i;
    picm->chlist_a.count = j;

    return status;
}

#ifdef ICM_RTR_DRIVER
/*
 * Function     : icm_enable_11ac_scanchevents
 * Description  : Helper function to enable scan channel events
 *                if the radio is 11ac offload based.
 *                The function also helps with checking if this
 *                step is required.
 * Input params : pointer to pdev, pointer to icm, whether 11ac
 *                offload is enabled
 * Return       : SUCCESS/FAILURE
 */

int icm_enable_11ac_scanchevents(ICM_DEV_INFO_T* pdev,
        ICM_INFO_T * picm,
        int is_11ac_offload)
{
    int ret = 0;

    if (is_11ac_offload) {
        ret = set_vap_priv_int_param(pdev,
                picm->dev_ifname,
                IEEE80211_PARAM_SCAN_CHAN_EVENT,
                1);

        if (ret < 0) {
            fprintf(stderr, "%-8.16s  Could not enable Scan Channel Events on Interface\n\n",
                    picm->dev_ifname);
            return FAILURE;

        }
    }

    return SUCCESS;
}

/*
 * Function     : icm_get_scanband
 * Description  : Helper function to get value of scan band
 * Input params : pointer to pdev, pointer to icm
 * Return       : Value of scanband on success,
 *                -1 on failure.
 */
int icm_get_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm)
{
    int scanband = 0;

    scanband = get_vap_priv_int_param(pdev,
            picm->dev_ifname,
            IEEE80211_PARAM_SCAN_BAND);

    if (scanband < 0) {
        fprintf(stderr, "%-8.16s  Could not get scanband on Interface\n\n",
                picm->dev_ifname);
        return -1;
    }

    return scanband;
}

/*
 * Function     : icm_set_scanband
 * Description  : Helper function to set requisite scan band
 * Input params : pointer to pdev, pointer to icm, required scanband
 * Return       : SUCCESS/FAILURE
 */
int icm_set_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm,
        int scanband)
{
    int ret = 0;

    ret = set_vap_priv_int_param(pdev,
            picm->dev_ifname,
            IEEE80211_PARAM_SCAN_BAND,
            scanband);

    if (ret < 0) {
        fprintf(stderr, "%-8.16s  Could not set scanband %d on Interface\n\n",
                picm->dev_ifname,
                scanband);
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : icm_configure_scanband
 * Description  : Helper function to check and configure required scan band
 * Input params : pointer to pdev, pointer to icm
 * Return       : SUCCESS/FAILURE
 */
int icm_configure_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm)
{
    int ret = 0;

    ICM_ASSERT(picm->sbmgmt_ctx.is_valid == false);

    if ((picm->sbmgmt_ctx.oprtv_scanband
                = icm_get_scanband(pdev, picm)) < 0) {
        fprintf(stderr, "%-8.16s Unable to determine current scan band "
                "setting.\n\n", picm->radio_ifname);
        return FAILURE;
    }

    if (picm->band == ICM_BAND_2_4G) {
        picm->sbmgmt_ctx.req_scanband = SCAN_BAND_2G_ONLY;
    } else if (picm->band == ICM_BAND_5G) {
        picm->sbmgmt_ctx.req_scanband = SCAN_BAND_5G_ONLY;
    }

    picm->sbmgmt_ctx.is_valid = true;

    if (picm->sbmgmt_ctx.req_scanband == picm->sbmgmt_ctx.oprtv_scanband) {
        /* No further action required */
        return SUCCESS;
    }

    if ((ret = icm_set_scanband(pdev, picm, picm->sbmgmt_ctx.req_scanband))
            != SUCCESS) {
        picm->sbmgmt_ctx.is_valid = false;
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : icm_restore_scanband
 * Description  : Helper function to restore scan band
 * Input params : pointer to pdev, pointer to icm
 * Return       : SUCCESS/FAILURE
 */
int icm_restore_scanband(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm)
{
    int ret = 0;
    int status = FAILURE;

    ICM_ASSERT(picm->sbmgmt_ctx.is_valid == true);

    if (picm->sbmgmt_ctx.req_scanband == picm->sbmgmt_ctx.oprtv_scanband) {
        /* No further action required */
        status = SUCCESS;
    } else {
        ret = icm_set_scanband(pdev,
                picm,
                picm->sbmgmt_ctx.oprtv_scanband);

        if (ret == SUCCESS) {
            status = SUCCESS;
        }
    }

    picm->sbmgmt_ctx.is_valid = false;
    return status;
}

/*
 * Function     : icm_disable_11ac_scanchevents
 * Description  : Helper function to disable scan channel events
 *                if the radio is 11ac offload based.
 *                The function also helps with checking if this
 *                step is required.
 * Input params : pointer to pdev, pointer to icm, whether 11ac
 *                offload is enabled
 * Return       : SUCCESS/FAILURE
 */

int icm_disable_11ac_scanchevents(ICM_DEV_INFO_T *pdev,
        ICM_INFO_T *picm,
        int is_11ac_offload)
{
    int ret = 0;

    if (is_11ac_offload) {
        ret = set_vap_priv_int_param(pdev,
                picm->dev_ifname,
                IEEE80211_PARAM_SCAN_CHAN_EVENT,
                0);

        if (ret < 0) {
            fprintf(stderr, "%-8.16s  Could not disable Scan Channel Events on Interface\n\n",
                    picm->dev_ifname);
            return FAILURE;

        }
    }

    return SUCCESS;
}

/*
 * Function     : icm_enable_sending_additional_ies
 * Description  : Helper function to enable sending additional IEs
 *                The function also helps with checking if this
 *                step is required.
 * Input params : pointer to pdev, pointer to icm
 * Return       : SUCCESS/FAILURE
 */
int icm_enable_sending_additional_ies(ICM_DEV_INFO_T *pdev, ICM_INFO_T *picm)
{
    int ret = 0;

    ret = set_vap_priv_int_param(pdev,
            picm->dev_ifname,
            IEEE80211_PARAM_SEND_ADDITIONAL_IES,
            1);
    if (ret < 0) {
        fprintf(stderr, "%-8.16s  Could not enable Sending of additional IEs on Interface\n\n",
                picm->dev_ifname);
        return FAILURE;
    }

    return SUCCESS;
}


/*
 * Function     : icm_disable_sending_additional_ies
 * Description  : Helper function to disable sending of additional IEs
 *                The function also helps with checking if this
 *                step is required.
 * Input params : pointer to pdev, pointer to icm
 * Return       : SUCCESS/FAILURE
 */
int icm_disable_sending_additional_ies(ICM_DEV_INFO_T *pdev, ICM_INFO_T *picm)
{
    int ret = 0;
    ret = set_vap_priv_int_param(pdev,
            picm->dev_ifname,
            IEEE80211_PARAM_SEND_ADDITIONAL_IES,
            0);
    if (ret < 0) {
        fprintf(stderr, "%-8.16s  Could not disable sending additional IEs on Interface\n\n",
                picm->dev_ifname);
        return FAILURE;
    }

    return SUCCESS;
}

#endif /* ICM_RTR_DRIVER */

/*
 * Function     : icm_do_80211_scan
 * Description  : do an 802.11 scan and print scan results
 * Input params : pointer to icm
 * Return       : 0 on success, -1 on general errors, -2 on scan cancellation
 */

int icm_do_80211_scan(ICM_INFO_T * picm)
{
    return icm_wal_do_80211_scan(picm);
}

/*
 * Function     : icm_cancel_80211_scan
 * Description  : Cancel all 802.11 scans for the given icm
 * Input params : pointer to icm
 * Return       : success/failure
 */
int icm_cancel_80211_scan(ICM_INFO_T * picm)
{
    return icm_wal_cancel_80211_scan(picm);;
}

/*
 * Function     : icm_update_scan_results
 * Description  : do scan and update scan results
 * Input params : pointer to icm, event, scan state, range
 * Return       : void
 *
 */
    void
icm_update_scan_results(ICM_INFO_T * picm,
        struct iw_event *event,
        struct iwscan_state *state,
        struct iw_range *iw_range,
        int has_range)
{
#ifdef ICM_SCAN_DISSECTION_DEBUG
    int i = 0;
#endif
    ICM_DEV_INFO_T* pdev = get_pdev();

    if (state->ap_num >= (MAX_SCAN_ENTRIES - 1)) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "error : scan database full\n");
        return;
    }

    /* Now, let's decode the event */
    switch (event->cmd) {

        case SIOCGIWAP:
            state->ap_num++;
            memcpy(picm->slist.elem[state->ap_num].bssid,
                    &event->u.ap_addr.sa_data,
                    ETH_ALEN);
            picm->slist.elem[state->ap_num].valid = TRUE;
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWAP. addr=%02X:%02X:%02X:%02X:%02X:%02X\n",
                    __func__,
                    (u_int8_t)event->u.ap_addr.sa_data[0],
                    (u_int8_t)event->u.ap_addr.sa_data[1],
                    (u_int8_t)event->u.ap_addr.sa_data[2],
                    (u_int8_t)event->u.ap_addr.sa_data[3],
                    (u_int8_t)event->u.ap_addr.sa_data[4],
                    (u_int8_t)event->u.ap_addr.sa_data[5]);
#endif
            break;

        case SIOCGIWNWID:
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWNWID\n",__func__);
#endif
            break;

        case SIOCGIWFREQ:
            {
                double freq;        /* Frequency/channel */
                int channel = -1;   /* Converted to channel */
                ICM_CHANNEL_LIST_T *pchlist = NULL;
                int chn_idx = 0;
                freq = icm_freq_to_float(&(event->u.freq));
                /* Convert to channel if possible */
#if 0
                if (has_range) {
                    channel = icm_freq_to_channel(freq, iw_range);
                }
#endif

                channel = icm_convert_mhz2channel((u_int32_t)event->u.freq.m/100000);

                picm->slist.elem[state->ap_num].freq = freq;
                picm->slist.elem[state->ap_num].channel = channel;

                /* Get the relevant operating band */
                if (picm->band == ICM_BAND_2_4G) {
                    pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
                } else {
                    pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
                }

                chn_idx = icm_get_channel_index(pchlist, channel);

                if (chn_idx != -1) {
                    pchlist->ch[chn_idx].num_wnw++;
                    pchlist->ch[chn_idx].num_wnw_20++;
                }

#ifdef ICM_SCAN_DISSECTION_DEBUG
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWFREQ: freq=%f channel=%d\n",
                        __func__,
                        freq,
                        channel);
#endif
            }
            break;

        case SIOCGIWMODE:
            if (event->u.mode >= ICM_NUM_OPER_MODE) {
                event->u.mode = ICM_NUM_OPER_MODE;
            }
            picm->slist.elem[state->ap_num].mode = event->u.mode;
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWMODE mode=%u\n",
                    __func__,
                    event->u.mode);
#endif
            break;

        case SIOCGIWNAME:
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWNAME\n",__func__);
#endif
            break;

        case SIOCGIWESSID:
            {
                char essid[IW_ESSID_MAX_SIZE];
                memset(essid, '\0', sizeof(essid));
                if ((event->u.essid.pointer) &&
                        (event->u.essid.length)) {
                    memcpy(essid, event->u.essid.pointer,
                            event->u.essid.length);
                    memcpy(picm->slist.elem[state->ap_num].ssid,
                            essid, sizeof(essid));
                    picm->slist.elem[state->ap_num].ssid_len =
                        event->u.essid.length;
#ifdef ICM_SCAN_DISSECTION_DEBUG
                    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWESSID. essid length=%d\n",__func__,
                            event->u.essid.length);
                    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWESSID. essid=%s",__func__, essid);
                    for (i = 0; i < IW_ESSID_MAX_SIZE; i++) {
                        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%c", essid[i]);
                    }
                    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "\n");
#endif
                }
            }
            break;

        case SIOCGIWENCODE:
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWENCODE\n",__func__);
#endif
            break;

        case SIOCGIWRATE:
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWRATE\n",__func__);
#endif
            break;

        case SIOCGIWMODUL:
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: SIOCGIWMODUL\n",__func__);
#endif
            break;

        case IWEVQUAL:
            {
                if ((event->u.qual.updated & IW_QUAL_LEVEL_UPDATED) &&
                        (event->u.qual.updated & IW_QUAL_NOISE_UPDATED) &&
                        (event->u.qual.updated & IW_QUAL_DBM)) {
                    picm->slist.elem[state->ap_num].rssi = event->u.qual.level -
                        event->u.qual.noise;
                    picm->slist.elem[state->ap_num].rssi_valid = TRUE;
                } else {
                    picm->slist.elem[state->ap_num].rssi = -128;
                    picm->slist.elem[state->ap_num].rssi_valid = FALSE;
                }

#ifdef ICM_SCAN_DISSECTION_DEBUG
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: IWEVQUAL\n",__func__);
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "Is valid=%d. RSSI=%d\n",
                        picm->slist.elem[state->ap_num].rssi_valid,
                        picm->slist.elem[state->ap_num].rssi);
#endif
            }
            break;

#ifndef WE_ESSENTIAL
        case IWEVGENIE:
#ifdef ICM_SCAN_DISSECTION_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: IWEVGENIE\n",__func__);
#endif
            icm_parse_gen_ie(picm, state, event->u.data.pointer, event->u.data.length);
            /* Informations Elements are complex, let's do only some of them */
            break;
#endif                          /* WE_ESSENTIAL */

        case IWEVCUSTOM:
            {
                char custom[IW_CUSTOM_MAX + 1];
                if ((event->u.data.pointer) && (event->u.data.length))
                    memcpy(custom, event->u.data.pointer,
                            event->u.data.length);
                custom[event->u.data.length] = '\0';
                //ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "                    Extra:%s\n", custom);
#ifdef ICM_SCAN_DISSECTION_DEBUG
                ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s: IWEVCUSTOM custom=%s\n",__func__, custom);
#endif

            }
            break;

        default:
            break;
    }                           /* switch(event->cmd) */

}

/*
 * Function     : icm_init_scan_params
 * Description  : initializes the scan related parameters
 * Input params : pointer to icm
 * Return       : void
 *
 */
void icm_init_scan_params(ICM_INFO_T* picm)
{
    memset(&picm->slist, 0, sizeof(picm->slist));
}

/*
 * Function     : icm_init_channel_params
 * Description  : init channel related params
 *                Set the number of valid channels to zero
 *                Set the channel index for spectral to starting index
 * Input params : pointer to icm
 * Return       : void
 *
 */
void icm_init_channel_params(ICM_INFO_T* picm)
{
    icm_wal_init_channel_params(picm);
}


/*
 * Function     : icm_get_wireless_nw_in_channel
 * Description  : get the number of wireless nw in given
 *                channel, using the scan results.
 * Input params : pointer to icm, channel number
 * Return       : channel number
 *
 */
int icm_get_wireless_nw_in_channel(ICM_INFO_T* picm, int channel)
{
    int i;
    int wnw_count = 0;

    /* walk through the scan results and count the number of
     * wireless network in given channel */
    for (i = 0; i < MAX_SCAN_ENTRIES; i++) {
        if (IS_SCAN_ELEM_VALID(picm, i)) {
            if (channel == ICM_GET_SCAN_ELEM_CHANNEL(picm, i)) {
                wnw_count++;
            }
        }
    }

    return wnw_count;
}

/*
 * Function     : icm_update_wnw_in_channel_list
 * Description  : using the scan results, supported channel
 *                list, update the number of wnw in the channel list
 * Input params : pointer to icm, channel number
 * Return       : channel number
 */
void icm_update_wnw_in_channel_list(ICM_INFO_T* picm, ICM_BAND_T band)
{
    int i;
    ICM_CHANNEL_LIST_T* pchlist;

    if (band == ICM_BAND_2_4G) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    ICM_ASSERT(pchlist != NULL);

    for (i = 0; i < pchlist->count; i++) {
        pchlist->ch[i].num_wnw = icm_get_wireless_nw_in_channel(picm, pchlist->ch[i].channel);
    }

    return;
}

/*
 * Function     : icm_do_80211_priv
 * Description  : interface to 80211 priv ioctl
 * Input params : pointer to icm, channel number, pointer to
 *                iwreq, interface name
 * Return       : success/failure
 */

    int
icm_do_80211_priv(ICM_INFO_T *picm, struct iwreq *iwr, const char *ifname, int op, void *data, size_t len)
{
    return icm_wal_do_80211_priv(picm, iwr, ifname, op, data, len);
}


/*
 * Function     : icm_get_currdomain
 * Description  : Obtain the current RF regulatory domain being used by
 *                the radio and place it in the rfreg_domain of the
 *                ICM_INFO_T object.
 * Input params : pointer to icm
 * Return       : 0 on success, -1 on failure
 */
    int
icm_get_currdomain(ICM_INFO_T* picm)
{
    return icm_wal_get_currdomain(picm);;
}

/*
 * Function     : icm_mark_non_ap_channels_excluded
 * Description  : For certain countries, there are different regulatory
 *                domains for APs versus stations. Since ICM may do a scan
 *                with the STA list of channels but then be used to set an
 *                AP channel, we need to mark those channels which are not
 *                valid for APs as excluded.
 * Input params : pointer to icm, channel number
 * Return       : TRUE if the channel is excluded for APs, otherwise FALSE
 */
    int
icm_is_non_ap_channel(ICM_INFO_T* picm, int channel)
{
    switch (picm->rfreg_domain)
    {
        // These values are had-coded, as it does not seem to be possible
        // to get them easily from a header file. They are defined in
        // hal/ah_regdomain.h, but it appears this file cannot be included
        // directly in user space (at least not without including "private"
        // header files). They are also defined in wlan_api.h, but we don't
        // want to depend on a service within masterd here.
        case 840:  // CTRY_UNITED_STATES
        case 124:  // CTRY_CANADA
        case 36:   // CTRY_AUSTRALIA
            {
                // @todo Find a more permanent way to do this. For now, this is
                //       sufficient to workaround the issue.
                //
                return (channel == 120 || channel == 124 || channel == 128 ||
                        channel == 144);
            }

        default:
            return FALSE;
    }
}

#ifdef ICM_RTR_DRIVER
/*
 * Function     : icm_get_80211_priv
 * Description  : interface to 80211 priv ioctl
 * Input params : pointer to icm, interface name
 *                type and data
 * Return       : success/failure
 */
    int
icm_get_80211_priv(ICM_INFO_T* picm, const char *ifname, int op, void *data, size_t len)
{
    struct iwreq iwr;

    if (icm_do_80211_priv(picm, &iwr, ifname, op, data, len) < 0)
        return -1;
    if (len < IFNAMSIZ)
        memcpy(data, iwr.u.name, len);
    return iwr.u.data.length;
}

/*
 * Function     : icm_print_ieee_chaninfo
 * Description  : prints ieee related channel information
 * Input params : pointer to ieee80211_ath_channel
 * Return       : success/failure
 */
    void
icm_print_ieee_chaninfo(const struct ieee80211_ath_channel *c)
{
    char buf[14];
    ICM_DEV_INFO_T* pdev = get_pdev();

    buf[0] = '\0';
    if (IEEE80211_IS_CHAN_FHSS(c))
        strlcat(buf, " FHSS", sizeof(buf));
    if (IEEE80211_IS_CHAN_11NA(c))
        strlcat(buf, " 11na", sizeof(buf));
    else if (IEEE80211_IS_CHAN_A(c))
        strlcat(buf, " 11a", sizeof(buf));
    else if (IEEE80211_IS_CHAN_11NG(c))
        strlcat(buf, " 11ng", sizeof(buf));
    /* XXX 11g schizophrenia */
    else if (IEEE80211_IS_CHAN_G(c) || IEEE80211_IS_CHAN_PUREG(c))
        strlcat(buf, " 11g", sizeof(buf));
    else if (IEEE80211_IS_CHAN_B(c))
        strlcat(buf, " 11b", sizeof(buf));
    if (IEEE80211_IS_CHAN_TURBO(c))
        strlcat(buf, " Turbo", sizeof(buf));
    if(IEEE80211_IS_CHAN_11N_CTL_CAPABLE(c))
        strlcat(buf, " C", sizeof(buf));
    if(IEEE80211_IS_CHAN_11N_CTL_U_CAPABLE(c))
        strlcat(buf, " CU", sizeof(buf));
    if(IEEE80211_IS_CHAN_11N_CTL_L_CAPABLE(c))
        strlcat(buf, " CL", sizeof(buf));
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "Channel %3u : %u%c%c%c Mhz%-14.14s",
            icm_convert_mhz2channel(c->ic_freq), c->ic_freq,
            IEEE80211_IS_CHAN_HALF(c) ? 'H' : (IEEE80211_IS_CHAN_QUARTER(c) ? 'Q' :  ' '),
            IEEE80211_IS_CHAN_PASSIVE(c) ? '*' : ' ',IEEE80211_IS_CHAN_DFSFLAG(c) ?'~':' ', buf);


}
#endif /* ICM_RTR_DRIVER */

/*
 * Function     : icm_get_ieee_chaninfo
 * Description  : prints ieee related channel information
 * Input params : pointer to ieee80211_ath_channel
 * Return       : success/failure
 */
int icm_get_ieee_chaninfo(ICM_INFO_T* picm)
{
    return icm_wal_get_ieee_chaninfo(picm); ;
}

#ifdef ICM_RTR_DRIVER
/*
 * Function     : icm_update_channel_ieee_info
 * Description  : updates the IEEE channel info for given channel
 * Input params : pointer to icm, freq, pointer to channel info
 * Return       : True if a channel match is found, else false.
 */
bool icm_update_channel_ieee_info(ICM_INFO_T* picm, int freq,
        const struct ieee80211_ath_channel *c)
{
    int channel = icm_convert_mhz2channel(freq);
    int found = FALSE;
    ICM_CHANNEL_LIST_T* pchlist;
    int i = 0;

    if (channel < 14) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    for (i = 0; i < pchlist->count; i++) {
        if (pchlist->ch[i].channel == channel) {
            found = TRUE;
            pchlist->ch[i].ic_flags = c->ic_flags;
            pchlist->ch[i].ic_flagext = c->ic_flagext;
            pchlist->ch[i].ic_ieee = c->ic_ieee;
            pchlist->ch[i].ic_maxregpower = c->ic_maxregpower;
            pchlist->ch[i].ic_maxpower = c->ic_maxpower;
            pchlist->ch[i].ic_minpower = c->ic_minpower;
            pchlist->ch[i].ic_regClassId = c->ic_regClassId;
            pchlist->ch[i].ic_antennamax = c->ic_antennamax;
            if ((IEEE80211_IS_CHAN_11AC_VHT80(c) ||
                    IEEE80211_IS_CHAN_11AC_VHT80_80(c) ||
                    IEEE80211_IS_CHAN_11AXA_HE80(c) ||
                    IEEE80211_IS_CHAN_11AXA_HE80_80(c)) &&
                    !pchlist->ch[i].ic_vhtop_ch_freq_seg1) {
                pchlist->ch[i].ic_vhtop_ch_freq_seg1 = c->ic_vhtop_ch_freq_seg1;
            }

            if ((IEEE80211_IS_CHAN_11AC_VHT80_80(c) ||
                    IEEE80211_IS_CHAN_11AXA_HE80_80(c)) &&
                    !pchlist->ch[i].ic_vhtop_ch_freq_seg2) {
                pchlist->ch[i].ic_vhtop_ch_freq_seg2 = c->ic_vhtop_ch_freq_seg2;
            }
            pchlist->ch[i].freq = freq;
            break;
        }
    }

    return found;
}

/*
 * Function     : icm_append_160MHz_channel_ieee_info
 * Description  : append 160 MHz specific IEEE channel info for given channel.
 *                icm_update_channel_ieee_info() must be called prior to this.
 * Input params : pointer to icm, freq, pointer to channel info
 * Return       : True if a channel match is found, else false.
 */
bool icm_append_160MHz_channel_ieee_info(ICM_INFO_T* picm, int freq,
        const struct ieee80211_ath_channel *c)
{
    int channel = icm_convert_mhz2channel(freq);
    int found = FALSE;
    ICM_CHANNEL_LIST_T* pchlist;
    int i = 0;

    if (channel < 14) {
        return FALSE;
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    for (i = 0; i < pchlist->count; i++) {
        if (!IEEE80211_IS_CHAN_11AC_VHT160(c) &&
                !IEEE80211_IS_CHAN_11AXA_HE160(c)) {
            continue;
        }

        if (pchlist->ch[i].channel == channel) {
            ICM_ASSERT(pchlist->ch[i].ic_ieee != 0);
            found = TRUE;
            pchlist->ch[i].ic_vhtop_ch_freq_seg1_160 = c->ic_vhtop_ch_freq_seg1;

            if (IEEE80211_IS_CHAN_11AC_VHT160(c)) {
                pchlist->ch[i].ic_flags |= IEEE80211_CHAN_11AC_VHT160;
            }

            if (IEEE80211_IS_CHAN_11AXA_HE160(c)) {
                pchlist->ch[i].ic_flags |= IEEE80211_CHAN_11AXA_HE160;
            }

            break;
        }
    }

    return found;
}
#endif /* ICM_RTR_DRIVER */

/*
 * Function     : hecap_val_extract
 * Description  : Extract the desired number of bits of capability information
 *                from the desired index in an HE Capabilities subfield
 * Input params : Pointer to start of HE Capabilities subfield, desired index,
 *                desired number of bits.
 * Return       : Extracted HE Capability value
 */
u_int32_t
hecap_val_extract(u_int8_t *hecap_subfield, u_int8_t idx, u_int32_t
                num_bits)  {

    u_int8_t curr_bits = 0, byte_cnt = 0, temp_val;
    u_int8_t cur_idx = 0;
    u_int32_t val = 0;

    temp_val = *(hecap_subfield);
    idx = idx % NUM_BITS_IN_BYTE;
    curr_bits = NUM_BITS_IN_BYTE - idx;
    curr_bits = (num_bits > curr_bits) ? NUM_BITS_IN_BYTE - idx: num_bits;

    while ((num_bits + idx) > NUM_BITS_IN_BYTE ) {
        val |= ((temp_val >> idx ) & ((1 << (curr_bits)) - 1)) << cur_idx;
        num_bits = num_bits - curr_bits;
        idx = idx + curr_bits;
        if (idx == NUM_BITS_IN_BYTE) {
            idx = 0;
            byte_cnt ++;
            temp_val = *(hecap_subfield + byte_cnt);
        }
        cur_idx = cur_idx + curr_bits;

       curr_bits = NUM_BITS_IN_BYTE - idx;
       curr_bits = (num_bits > curr_bits) ? NUM_BITS_IN_BYTE - idx: num_bits ;
    }

    if ((idx + num_bits) <= NUM_BITS_IN_BYTE ) {
        val|= ((temp_val >> idx)  & ((1 << curr_bits) -1)) << cur_idx;
    }

    return val;
}

/*
 * Function     : icm_get_paramrange
 * Description  : gets parameter range information
 * Input params : pointer to icm
 * Output params: pointer to iw_range structure
 * Return       : success/failure
 */
int icm_get_paramrange(ICM_INFO_T *picm, struct iw_range *range)
{
    return icm_wal_get_paramrange(picm, range);
}

/*
 * Function     : icm_freq_to_float
 * Description  : convert internal representation of frequency
 *                to floating point
 * Input params : pointer to iw_freq structure
 * Return       : on success, floating point representation of frequency
 *                on failure, -1
 */
double icm_freq_to_float(const struct iw_freq *freq)
{
    if (freq == NULL) {
        return -1;
    }

    return ((double)freq->m) * pow(10, freq->e);
}

/*
 * Function     : icm_freq_to_channel
 * Description  : convert frequency to channel
 * Input params : frequency, pointer to iw_range structure
 * Return       : on success, channel
 *                on failure, -1
 */
int icm_freq_to_channel(double freq, const struct iw_range *range)
{
    int    count;
    double temp;

    if(freq < KILO_VAL || range == NULL) {
        return -1;
    }

    for (count = 0; count < range->num_frequency; count++)
    {
        temp = icm_freq_to_float(&(range->freq[count]));

        if(freq == temp) {
            return(range->freq[count].i);
        }
    }

    return -1;
}

/*
 * Function     : icm_init_event_context
 * Description  : initialize event stream parsing context
 * Input params : pointer to event stream parsing context, pointer to buffer
 *                containing event stream, length of event stream
 * Return       : success/failure
 */
int icm_init_event_parse_context(ICM_EVENT_PARSE_CONTEXT_T *ctx,
        char *evbuf,
        int evbuflen)
{
    if (ctx == NULL) {
        return FAILURE;
    }

    ctx->curr_ev = evbuf;
    ctx->curr_ev_val_offset = IW_EV_LCP_PK_LEN;
    ctx->stream_end = evbuf + evbuflen;
    ctx->is_valid = 1;

    return SUCCESS;
}


/*
 * Function     : icm_get_next_event
 * Description  : get next event from event stream
 * Input params : pointer to event stream parsing context,
 *                Wireless Extensions version
 * Output param : pointer to filled in iw_event structure
 * Return       : ICM_EV_PARSE_RESULT_T giving the result code.
 */
    ICM_EV_PARSE_RESULT_T
icm_get_next_event(ICM_EVENT_PARSE_CONTEXT_T *ctx,
        struct iw_event *event,
        int we_ver)
{
    ICM_EVENT_DESCRIPTOR_T const *desc = NULL;
    ICM_EVENT_DESCRIPTOR_ADJ_T desc_adj;
#ifdef ICM_EV_STREAM_DEBUG
    ICM_DEV_INFO_T* pdev = get_pdev();
#endif
    /* Pointer to start of current event value */
    char *curr_ev_val = NULL;

    /* Pointer to end of current event value */
    char *curr_ev_val_end = NULL;

    /* Length of current event value */
    u_int32_t curr_ev_val_len = 0;

    /* Length of data pointed to by pointer in ICM_EV_TYPE_POINT event */
    u_int32_t ptr_data_len = 0;

    /* Expected length of data pointed to by pointer in
       ICM_EV_TYPE_POINT event */
    u_int32_t expected_ptr_data_len = 0;


    if (!ctx->is_valid) {
        return ICM_EV_PARSE_RESULT_INVAL_CTX;
    }

    if (event == NULL) {
        return ICM_EV_PARSE_RESULT_INVAL_ARG;
    }

    if((ctx->curr_ev + IW_EV_LCP_PK_LEN) > ctx->stream_end) {
        return ICM_EV_PARSE_RESULT_STREAM_END;
    }

    /* Get event header */
    memcpy((u_int8_t*)event, ctx->curr_ev, IW_EV_LCP_PK_LEN);

#ifdef ICM_EV_STREAM_DEBUG
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s:\n"
            "ctx->curr_ev = %p, ctx->curr_ev_val_offset = %u,"
            " ctx->stream_end = %p\n"
            "event->cmd = 0x%X, event->len = %u\n",
            __func__,
            ctx->curr_ev,
            ctx->curr_ev_val_offset,
            ctx->stream_end,
            event->cmd,
            event->len);
#endif /* ICM_EV_STREAM_DEBUG */

    if(event->len <= IW_EV_LCP_PK_LEN) {
        /* We are dead */
        ctx->is_valid = 0;
        return ICM_EV_PARSE_RESULT_INVAL_EVLEN;
    }

    desc = icm_get_event_descriptor(event->cmd, we_ver, &desc_adj);

    if (desc == NULL) {
        /* Unknown event. Skip. */
        ctx->curr_ev_val_offset = IW_EV_LCP_PK_LEN;
        ctx->curr_ev += event->len;

        return ICM_EV_PARSE_RESULT_UNKNOWN_EV;
    }

#ifdef ICM_EV_STREAM_DEBUG
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s:\n"
            "desc->ev_type = %u, desc_adj.ev_len = %u\n",
            __func__,
            desc->ev_type,
            desc_adj.ev_len);
#endif /* ICM_EV_STREAM_DEBUG */

    curr_ev_val = ctx->curr_ev + ctx->curr_ev_val_offset;
    curr_ev_val_len = desc_adj.ev_len - IW_EV_LCP_PK_LEN;

    if((curr_ev_val + curr_ev_val_len) > ctx->stream_end)
    {
        /* End */
        return ICM_EV_PARSE_RESULT_STREAM_END;
    }

    if ((we_ver > 18) && (desc->ev_type == ICM_EV_TYPE_POINT)) {
        memcpy((u_int8_t *)event + IW_EV_LCP_LEN + IW_EV_POINT_OFF,
                curr_ev_val,
                curr_ev_val_len);
    } else {
        memcpy((u_int8_t *)event + IW_EV_LCP_LEN,
                curr_ev_val,
                curr_ev_val_len);
    }

    curr_ev_val_end = curr_ev_val + curr_ev_val_len;

#ifdef ICM_EV_STREAM_DEBUG
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s:\n"
            "curr_ev_val = %p, curr_ev_val_end = %p curr_ev_val_len= %u\n",
            __func__,
            curr_ev_val,
            curr_ev_val_end,
            curr_ev_val_len);
#endif /* ICM_EV_STREAM_DEBUG */


    if (desc->ev_type == ICM_EV_TYPE_POINT)
    {
        ptr_data_len = event->len - desc_adj.ev_len;
        event->u.data.pointer = NULL;

        if (ptr_data_len > 0)
        {
            expected_ptr_data_len = event->u.data.length * desc->ev_unit_size;

            if(expected_ptr_data_len == ptr_data_len) {
                /* Check to ensure sender has not sent more units
                   than allowed (except if we permit no checking of limits) */
                if (desc->ev_is_nolimit ||
                        (event->u.data.length <= desc->ev_max_units)) {
                    event->u.data.pointer = curr_ev_val_end;
                }
            }

#ifdef ICM_EV_STREAM_DEBUG
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%s:\n"
                    "ptr_data_len = %u, expected_ptr_data_len = %u\n",
                    __func__,
                    ptr_data_len,
                    expected_ptr_data_len);
#endif /* ICM_EV_STREAM_DEBUG */
        }

        /* Get next event */
        ctx->curr_ev_val_offset = IW_EV_LCP_PK_LEN;
        ctx->curr_ev += event->len;
    } else {
        /* desc->ev_type != ICM_EV_TYPE_POINT */

        if((curr_ev_val_end + curr_ev_val_len) <= (ctx->curr_ev + event->len)) {
            /* More event values present */
            ctx->curr_ev_val_offset += curr_ev_val_len;
        } else {
            /* Get next event */
            ctx->curr_ev_val_offset = IW_EV_LCP_PK_LEN;
            ctx->curr_ev += event->len;
        }
    }

    return ICM_EV_PARSE_RESULT_SUCCESS;
}


/*
 * Function     : icm_init_scanner_thread
 * Description  : initialize scanner thread
 * Input params : pointer to icm
 * Return       : SUCCESS/FAILURE
 */
int icm_init_scanner_thread(ICM_INFO_T *picm)
{
    int ret = 0;

    if ((ret = pthread_attr_init(&picm->scanner_thread_attr)) < 0) {
        perror("icm : pthread_attr_init");
        return FAILURE;
    }

    if ((ret = pthread_attr_setdetachstate(&picm->scanner_thread_attr,
                    PTHREAD_CREATE_DETACHED)) < 0) {
        perror("icm : pthread_attr_setdetachstate");
        return FAILURE;
    }

    if ((ret = pthread_mutex_init(&picm->scanner_thread_mutex,
                    NULL)) < 0) {
        perror("icm : pthread_mutex_init");
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : icm_cleanup_scanner_thread
 * Description  : de-initialize scanner thread
 * Input params : pointer to icm
 * Return       : SUCCESS/FAILURE
 */
int icm_cleanup_scanner_thread(ICM_INFO_T *picm)
{
    int ret = 0;
    int status = SUCCESS;

    if (icm_is_scanner_thread_active(picm) == TRUE) {
#ifdef ICM_RTR_DRIVER
        if ((ret = pthread_cancel(picm->scanner_thread)) < 0) {
            status = FAILURE;
        }
#endif /*ICM_RTR_DRIVER */
    }

    /* TODO: Handle all cancellation procedures */

    icm_set_scanner_thread_active(picm, FALSE);

    if ((ret = pthread_attr_destroy(&picm->scanner_thread_attr)) < 0) {
        perror("icm : pthread_attr_destroy");
        status = FAILURE;
    }

    if ((ret = pthread_mutex_destroy(&picm->scanner_thread_mutex)) < 0) {
        perror("icm : pthread_mutex_destroy");
        return FAILURE;
    }

    return status;
}

/*
 * Function     : icm_set_scanner_thread_active
 * Description  : set whether scanner thread is active
 * Input params : pointer to icm
 * Return       : TRUE/FALSE
 */
void icm_set_scanner_thread_active(ICM_INFO_T *picm, bool value)
{
    pthread_mutex_lock(&picm->scanner_thread_mutex);
    picm->is_scanner_thread_active = value;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);
}

/*
 * Function     : icm_is_scanner_thread_active
 * Description  : report whether scanner thread is active
 * Input params : pointer to icm
 * Return       : TRUE/FALSE
 */
bool icm_is_scanner_thread_active(ICM_INFO_T *picm)
{
    bool value = FALSE;

    pthread_mutex_lock(&picm->scanner_thread_mutex);
    value = picm->is_scanner_thread_active;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);

    return value;
}

/*
 * Function     : icm_set_80211scan_done
 * Description  : mark whether 802.11 scan is done
 * Input params : pointer to icm
 * Return       : TRUE/FALSE
 */
void icm_set_80211scan_done(ICM_INFO_T *picm, bool value)
{
    pthread_mutex_lock(&picm->scanner_thread_mutex);
    picm->is_80211scan_done = value;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);
}

/*
 * Function     : icm_is_80211scan_done
 * Description  : report whether 802.11 scan is done
 * Input params : pointer to icm
 * Return       : TRUE/FALSE
 */
bool icm_is_80211scan_done(ICM_INFO_T *picm)
{
    bool value = FALSE;

    pthread_mutex_lock(&picm->scanner_thread_mutex);
    value = picm->is_80211scan_done;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);

    return value;
}

/*
 * Function     : icm_set_80211scan_cancel_requested
 * Description  : mark whether 802.11 scan cancellation is requested
 * Input params : pointer to icm
 * Return       : TRUE/FALSE
 */
void icm_set_80211scan_cancel_requested(ICM_INFO_T *picm, bool value)
{
    pthread_mutex_lock(&picm->scanner_thread_mutex);
    picm->is_80211scan_cancel_requested = value;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);
}

/*
 * Function     : icm_is_80211scan_cancel_requested
 * Description  : report whether 802.11 scan cancellation is requested
 * Input params : pointer to icm
 * Return       : TRUE/FALSE
 */
bool icm_is_80211scan_cancel_requested(ICM_INFO_T *picm)
{
    bool value = FALSE;

    pthread_mutex_lock(&picm->scanner_thread_mutex);
    value = picm->is_80211scan_cancel_requested;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);

    return value;
}


/*
 * Function     : icm_activate_scanner_thread
 * Description  : activate scanner thread to perform a scan job
 * Input params : pointer to icm
 * Return       : SUCCESS/FAILURE
 */
int icm_activate_scanner_thread(ICM_INFO_T *picm)
{
    int pret = 0;

    if (icm_is_scanner_thread_active(picm) == TRUE) {
        return FAILURE;
    }

    pret = pthread_create(&picm->scanner_thread,
            &picm->scanner_thread_attr,
            icm_process_scan_job,
            (void*)picm);

    if (pret < 0) {
        perror("icm : pthread_create");
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : ICM_CHANNEL_T_comparator
 * Description  : Helper function which acts as an ICM_CHANNEL_T
 *                comparator for the standard qsort sorting library.
 * Input params : pointers to the two objects being compared
 * Return       : If obj1 < obj2,   1
 *                If obj1 > obj2,  -1
 *                If obj1 == obj2, then if chan1 < chan2 -1
 *                                      if chan1 > chan2  1
 *
 */

int ICM_CHANNEL_T_comparator(const void* obj1, const void *obj2)
{
    ICM_CHANNEL_T *chan1 = (ICM_CHANNEL_T *)obj1;
    ICM_CHANNEL_T *chan2 = (ICM_CHANNEL_T *)obj2;

    /* Comparator for descending order of usability */
    if (chan1->usablity < chan2->usablity) {
        return 1;
    } else  if (chan1->usablity > chan2->usablity) {
        return -1;
    } else {
        /* If the usability is equal, we would like to sort
           on channel number, and in ascending order */
        if (chan1->channel < chan2->channel) {
            return -1;
        } else if (chan1->channel > chan2->channel) {
            return 1;
        } else {
            /* We should NEVER come here */
            err("Unexpected sort condition on ICM_CHANNEL_T");
            return 0;
        }
    }
}

/*
 * Function     : icm_sort_channel_listing
 * Description  : Helper function to sort ICM_CHANNEL_LIST_T
 *                in order of usability (highest to lowest).
 * Input params : pointer to icm info, pointer to unsorted ICM_CHANNEL_LIST_T
 * Input params : pointer to sorted ICM_CHANNEL_LIST_T
 * Return       : success/failure
 */
int icm_sort_channel_listing(ICM_INFO_T *picm,
        ICM_CHANNEL_LIST_T *unsortedlist,
        ICM_CHANNEL_LIST_T *sortedlist)
{
    int i = 0;

    if (picm == NULL || unsortedlist == NULL || sortedlist == NULL) {
        return FAILURE;
    }

    memcpy(sortedlist, unsortedlist, sizeof(ICM_CHANNEL_LIST_T));

    /* For ease of sorting, we re-purpose the usability field in
       sortedlist, setting it to the computed usability */
    for (i = 0; i < sortedlist->count; i++)
    {
        sortedlist->ch[i].usablity =
            ICM_GET_CHANNEL_USABLITY(picm,
                    sortedlist->ch[i].channel);
    }

    qsort(sortedlist->ch,
            sortedlist->count,
            sizeof(ICM_CHANNEL_T),
            ICM_CHANNEL_T_comparator);

    return SUCCESS;
}

/*
 * Function     : ICM_SCAN_INFO_T_comparator
 * Description  : Helper function which acts as an ICM_SCAN_INFO_T
 *                comparator for the standard qsort sorting library.
 * Input params : pointers to the two objects being compared
 * Return       : If obj1 < obj2,   1
 *                If obj1 > obj2,  -1
 *                If obj1 == obj2,  0
 */

int ICM_SCAN_INFO_T_comparator(const void* obj1, const void *obj2)
{
    ICM_SCAN_INFO_T *scaninfo1 = (ICM_SCAN_INFO_T *)obj1;
    ICM_SCAN_INFO_T *scaninfo2 = (ICM_SCAN_INFO_T *)obj2;

    /* If RSSI is invalid, it gets pushed to the bottom
       anyway */
    if (scaninfo1->rssi < scaninfo2->rssi) {
        return 1;
    } else  if (scaninfo1->rssi > scaninfo2->rssi) {
        return -1;
    } else {
        return 0;
    }
}

/*
 * Function     : icm_sort_scaninfo
 * Description  : Helper function to inplace-sort ICM_SCAN_INFO_T
 *                in order of RSSI (highest to lowest).
 * Input params : pointer to unsorted ICM_SCAN_INFO_T array, size of
 *                ICM_SCAN_INFO_T array.
 * Return       : success/failure
 */
int icm_sort_scaninfo(ICM_SCAN_INFO_T *scaninfo, int arraysize)
{
    if (scaninfo == NULL) {
        return FAILURE;
    }

    qsort(scaninfo,
            arraysize,
            sizeof(ICM_SCAN_INFO_T),
            ICM_SCAN_INFO_T_comparator);

    return SUCCESS;
}

/*
 * Function     : icm_prepare_channelinfo
 * Description  : Prepare array of CHANNEL_INFO_T elements to
 *                send back to external entity.
 * Input params : pointer to icm info
 * Output params: pointer to array of CHANNEL_INFO_T elements (will
 *                be allocated by icm_prepare_channelinfo, but needs
 *                to be freed by caller), pointer to location of value
 *                giving number of elements in CHANNEL_INFO_T array
 * Return       : success/failure
 */
int icm_prepare_channelinfo(ICM_INFO_T *picm,
        CHANNEL_INFO_T **pchannels,
        u_int8_t *pnumChannels)
{
    u_int8_t numChannels = 0;
    CHANNEL_INFO_T *channel = NULL;
    void  *buf = NULL;
    int buflen = 0;
    u_int8_t  *currbuf = NULL;
    AP_INFO_T *accessPoint = NULL;
    ICM_CHANNEL_LIST_T *pchlist = NULL;
    ICM_CHANNEL_LIST_T sortedchlist;
    ICM_SCAN_INFO_T *pscaninfo = NULL;
    int i = 0, j = 0, k = 0;
    int total_num_aps = 0;
    int num_aps_temp = 0;
    int num_wnw_temp = 0;

    *pnumChannels = 0;
    *pchannels = NULL;

    if (picm->band == ICM_BAND_2_4G) {
        pchlist = ICM_GET_11BG_CHANNEL_LIST_PTR(picm);
    } else {
        pchlist = ICM_GET_11A_CHANNEL_LIST_PTR(picm);
    }

    icm_sort_channel_listing(picm, pchlist, &sortedchlist);

    if (sortedchlist.count > 255) {
        /* Can't be :) But just for a bit of future proofing */
        sortedchlist.count = 255;
    }

    if (picm->scan_config.max_channels == 0) {
        numChannels = sortedchlist.count;
    } else {
        numChannels = MIN_INT(picm->scan_config.max_channels,
                sortedchlist.count);
    }

    for (i = 0; i < numChannels; i++)
    {
        num_wnw_temp = sortedchlist.ch[i].num_wnw;

        if (num_wnw_temp > 255) {
            num_wnw_temp = 255;
        }

        if (picm->scan_config.max_aps_per_channel == 0) {
            num_aps_temp = num_wnw_temp;
        } else {
            num_aps_temp = MIN_INT(picm->scan_config.max_aps_per_channel,
                    num_wnw_temp);
        }

        total_num_aps += num_aps_temp;
    }

    buflen = (sizeof(CHANNEL_INFO_T) * numChannels) +
        (sizeof(AP_INFO_T) * total_num_aps);

    buf = (void *)malloc(buflen);

    if (buf == NULL) {
        err("Could not allocate memory for channel info array");
        return FAILURE;
    }

    memset(buf, 0, buflen);

    currbuf = (u_int8_t *)buf;

    for (i = 0; i < numChannels; i++)
    {
        channel = (CHANNEL_INFO_T *)currbuf;

        channel->channelId = sortedchlist.ch[i].channel;
        channel->usability = ICM_GET_CHANNEL_USABLITY(picm, channel->channelId);

        num_wnw_temp = sortedchlist.ch[i].num_wnw;

        if (num_wnw_temp > 255) {
            num_wnw_temp = 255;
        }

        if (picm->scan_config.max_aps_per_channel == 0) {
            num_aps_temp = num_wnw_temp;
        } else {
            num_aps_temp = MIN_INT(picm->scan_config.max_aps_per_channel,
                    num_wnw_temp);
        }

        channel->numAPs = num_aps_temp;

        pscaninfo = (ICM_SCAN_INFO_T *)malloc(sizeof(ICM_SCAN_INFO_T) *
                sortedchlist.ch[i].num_wnw);

        if (pscaninfo == NULL) {
            err("Could not allocate memory for scan info array");
            free(buf);
            return FAILURE;
        }

        memset(pscaninfo,
                0,
                sizeof(ICM_SCAN_INFO_T) * sortedchlist.ch[i].num_wnw);

        for (j = 0, k = 0;
                j < MAX_SCAN_ENTRIES && k < sortedchlist.ch[i].num_wnw;
                j++)
        {
            if (IS_SCAN_ELEM_VALID(picm, j)) {
                if (sortedchlist.ch[i].channel ==
                        ICM_GET_SCAN_ELEM_CHANNEL(picm, j)) {
                    memcpy(&pscaninfo[k],
                            ICM_GET_SCAN_ELEM(picm, j),
                            sizeof(ICM_SCAN_INFO_T));
                    k++;
                }
            }
        }

        icm_sort_scaninfo(pscaninfo, sortedchlist.ch[i].num_wnw);

        currbuf += sizeof(CHANNEL_INFO_T);

        for (j = 0; j < channel->numAPs; j++) {
            accessPoint = (AP_INFO_T *)currbuf;
            accessPoint->ssidLen = pscaninfo[j].ssid_len;
            memcpy(accessPoint->ssid, pscaninfo[j].ssid, pscaninfo[j].ssid_len);
            memcpy(accessPoint->bssid, pscaninfo[j].bssid, ETH_ALEN);
            accessPoint->rssi = pscaninfo[j].rssi;
            /* Not yet implemented */
            accessPoint->channelUtilization = 0;
            currbuf += sizeof(AP_INFO_T);
        }

        free(pscaninfo);
    }

    *pchannels = (CHANNEL_INFO_T *)buf;
    *pnumChannels = numChannels;

    return SUCCESS;
}

/*
 * Function     : icm_process_scan_job
 * Description  : Pthread executable function which processes a requested
 *                scan job.
 * Input params : void pointer pointing to location of an ICM_INFO_T
 *                structure
 * Return       : NULL. The scan results (including success/failure)
 *                will be stored into the ICM_INFO_T structure.
 *
 */
void* icm_process_scan_job(void *arg)
{
    ICM_INFO_T *picm = (ICM_INFO_T *)arg;
    ICM_DEV_INFO_T* pdev = get_pdev();
    int send_status = FAILURE;
    int scan_status = FAILURE;
    u_int8_t rsp_status = ICM_RSP_STATUS_FAILURE;
    u_int8_t numChannels = 0;
    CHANNEL_INFO_T *channels = NULL;
    int ret = 0;

    pthread_mutex_lock(&picm->scanner_thread_mutex);
    picm->is_scanner_thread_active = TRUE;
    picm->is_80211scan_cancel_requested = FALSE;
    picm->is_80211scan_done = FALSE;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "About to start requested scan job on interface %s\n",
            picm->dev_ifname);

    if ((scan_status = icm_scan_and_select_channel(picm, FALSE)) !=
            ICM_SCAN_SELECT_STATUS_SUCCESS) {
        if (scan_status == ICM_SCAN_SELECT_STATUS_SCAN_CANCELLED) {
            rsp_status = ICM_RSP_STATUS_SCAN_CANCELLED;
        }
        goto out;
    }

    ret = icm_prepare_channelinfo(picm, &channels, &numChannels);

    if (ret != SUCCESS) {
        goto out;
    }

    rsp_status = ICM_RSP_STATUS_SUCCESS;

out:
    send_status = icm_get_ranked_channels_rsp(pdev,
            picm->reqmsg_params.transactionId,
            rsp_status,
            numChannels,
            channels);

    if (channels != NULL) {
        free(channels);
    }

    if (scan_status == SUCCESS &&
            send_status == SUCCESS &&
            rsp_status == ICM_RSP_STATUS_SUCCESS) {
        picm->scanner_thread_status = SUCCESS;
    } else {
        picm->scanner_thread_status = FAILURE;
    }

    icm_send_event(pdev,
            ICM_EVENT_RECIPIENT_MAIN,
            ICM_EVENT_SCAN_JOB_DONE);

    pthread_mutex_lock(&picm->scanner_thread_mutex);
    picm->is_scanner_thread_active = FALSE;
    picm->is_80211scan_cancel_requested = FALSE;
    pthread_mutex_unlock(&picm->scanner_thread_mutex);

    pthread_exit(NULL);
}

