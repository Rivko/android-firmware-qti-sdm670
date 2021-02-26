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
 *       Filename:  icm_test.c
 *
 *    Description:  Test Stubs for ICM feature
 *
 *        Version:  1.0
 *        Created:  04/24/2012 02:22:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "icm.h"

#ifdef ICM_RTR_DRIVER
int icm_test_spectral_enab(ICM_INFO_T *picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : Spectral Enable");
    if (icm_is_spectral_enab(picm)) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "Spectral enabled\n");
    } else {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "Spectral disabled\n");
    }

    return 0;
}

int icm_test_spectral_active(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : Spectral Active");
    if (icm_is_spectral_active(picm)) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "Spectral Active\n");
    } else {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "Spectral Inactive\n");
    }
    return 0;
}
#endif /* ICM_RTR_DRIVER */

int icm_test_scan(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : Generic Scan");
    icm_do_80211_scan(picm);
    icm_display_scan_db(picm);
    return 0;
}


int icm_test_channel(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : GET CHANNELS");
    if (icm_get_supported_channels(picm) == SUCCESS) {
        icm_display_channels(picm);
    } else {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "GET CHANNELS failed");
    }
    return 0;
}

#ifdef ICM_RTR_DRIVER
int icm_test_sweeping_spectral_scan(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : Spectral Scan over the band");

    icm_init_channel_params(picm);

    if (icm_get_supported_channels(picm) == FAILURE) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "GET CHANNELS failed");
        return -1;
    }

    //icm_disable_dcs(picm);

    if (icm_configure_spectral_params(picm) != SUCCESS) {
        err("Cannot configure required Spectral Scan parameters...");
        return -1;
    }
    icm_start_spectral_scan(picm);
    icm_recv_spectral_data(picm);
    icm_display_channels(picm);

    return 0;
}
#endif /* ICM_RTR_DRIVER */

int icm_test_get_chan_properties(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : Channel characteristics");
    icm_get_channel_loading(picm);
    icm_display_chan_properties(picm);
    return 0;
}

#ifdef ICM_RTR_DRIVER
int icm_test_clear_chan_properties(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : Clear recorded channel characteristics");
    icm_clear_spectral_chan_properties(picm);

    return 0;
}
#endif /* ICM_RTR_DRIVER */

int icm_test_nw_count(ICM_INFO_T* picm)
{
    int i;
    int ap_count = 0;
    ICM_DEV_INFO_T* pdev = get_pdev();

    ICM_DPRINTF(pdev,
            ICM_PRCTRL_FLAG_NO_PREFIX,
            ICM_DEBUG_LEVEL_DEFAULT,
            ICM_MODULE_ID_TEST,
            LINESTR);

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_TEST, "Channel\t\tNum of Wireless nw\n");
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_TEST, "-------\t\t------------------\n");

    for (i = 0; i < 14; i++) {
        ap_count = icm_get_wireless_nw_in_channel(picm, i);
        if (ap_count) {
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_TEST, " %d \t\t %d\n", i, ap_count);
        }
    }
    return 0;
}

int icm_test_list_channels(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : Listing detail channel info");
    icm_get_supported_channels(picm);
    icm_get_ieee_chaninfo(picm);
    icm_display_channels(picm);
    return 0;
}

int icm_do_test(ICM_INFO_T* picm)
{
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_TEST, "TEST : icm features");
#ifdef ICM_RTR_DRIVER
    icm_test_clear_chan_properties(picm);
#endif /* ICM_RTR_DRIVER */
    icm_test_get_chan_properties(picm);
    icm_test_list_channels(picm);
    icm_test_scan(picm);
    icm_test_nw_count(picm);
#ifdef ICM_RTR_DRIVER
    icm_test_sweeping_spectral_scan(picm);
#endif /* ICM_RTR_DRIVER */
    icm_display_channels(picm);
    return 0;
}
