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
 *       Filename:  icm_wal_ioctl.c
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
#ifdef ICM_RTR_DRIVER

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
#include <sys/wait.h>

#define _LINUX_TYPES_H

#include "icm.h"
#include "icm_wal.h"
#include "icm_internal.h"

#include "if_athioctl.h"
#include "spectral_ioctl.h"
#include "spectral_data.h"
#include "spec_msg_proto.h"
#include "ath_classifier.h"
#include "spectral_ioctl.h"
#include "spectral.h"
#include "ieee80211_external.h"

#include "icm_api.h"
#include "spectral_data.h"

#ifndef ATH_DEFAULT
#define ATH_DEFAULT "wifi0"
#endif


/*
 * Function     : icm_wal_iotcl_is_spectral_enab
 * Description  : check is spectral is enabled
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_ioctl_is_spectral_enab(ICM_INFO_T* picm)
{
    u_int32_t status =  SUCCESS;
    struct ifreq ifr;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;

    psinfo->atd.ad_id = SPECTRAL_IS_ENABLED | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void*)&status;
    psinfo->atd.ad_out_size = sizeof(u_int32_t);
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        status = FAILURE;
        perror("icm : ioctl fail (SPECTRAL_IS_ENABLED)");
    }

    return status;
}

/*
 * Function     : icm_wal_ioctl_get_spectral_params
 * Description  : Get values of Spectral parameters
 * Input params : pointer to icm info structure
 * Output params: pointer to Spectral params structure, to be populated
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_ioctl_get_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp)
{
    struct ifreq ifr;
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

    psinfo = &picm->sinfo;
    psinfo->atd.ad_id = SPECTRAL_GET_CONFIG | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void *)sp;
    psinfo->atd.ad_out_size = sizeof(*sp);
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        perror("icm : SIOCGATHPHYERR ioctl fail (SPECTRAL_GET_CONFIG)");
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function     : icm_wal_ioctl_set_spectral_params
 * Description  : Set values of Spectral parameters
 * Input params : pointer to icm info structure, pointer to Spectral params
 *                structure containing values to be set
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_ioctl_set_spectral_params(ICM_INFO_T *picm, SPECTRAL_PARAMS_T *sp)
{
    struct ifreq ifr;
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

    psinfo = &picm->sinfo;
    psinfo->atd.ad_id = SPECTRAL_SET_CONFIG | ATH_DIAG_IN;
    psinfo->atd.ad_in_data = (void*)sp;
    psinfo->atd.ad_in_size = sizeof(*sp);
    psinfo->atd.ad_out_data = NULL;
    psinfo->atd.ad_out_size = 0;

    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        perror("icm : SIOCGATHPHYERR ioctl fail (SPECTRAL_SET_CONFIG)");
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
int icm_wal_ioctl_start_spectral_scan(ICM_INFO_T* picm)
{
    u_int32_t status = SUCCESS;
    struct ifreq ifr;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;

    picm->substate = ICM_STATE_SPECTRAL_SCAN;

    psinfo->atd.ad_id = SPECTRAL_ACTIVATE_SCAN | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void*)&status;
    psinfo->atd.ad_out_size = sizeof(u_int32_t);
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        status = FAILURE;
        perror("icm : ioctl fail (SPECTRAL_ACTIVATE_SCAN)");
    }

    return status;
}

/*
 * Function     : icm_wal_ioctl_set_spectral_debug
 * Description  : set the spectral module debug level
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_ioctl_set_spectral_debug(ICM_INFO_T* picm, int dbglevel)
{
    u_int32_t status = SUCCESS;
    struct ifreq ifr;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */

    psinfo->atd.ad_id = SPECTRAL_SET_DEBUG_LEVEL | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_size = sizeof(u_int32_t);
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        status = FAILURE;
        perror("icm : ioctl fail (SPECTRAL_SET_DEBUG_LEVEL)");
    }

    return status;
}


/*
 * Function     : icm_wal_ioctl_stop_spectral_scan
 * Description  : stop the spectrla scan on current channel
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_ioctl_stop_spectral_scan(ICM_INFO_T* picm)
{
    u_int32_t status = SUCCESS;
    struct ifreq ifr;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */

    psinfo->atd.ad_id = SPECTRAL_STOP_SCAN | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void*)&status;
    psinfo->atd.ad_out_size = sizeof(u_int32_t);
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        status = FAILURE;
        perror("icm : ioctl fail (SPECTRAL_STOP_SCAN)");
    }

    return status;
}

/*
 * Function     : icm_wal_get_channel_loading
 * Description  : get the channel loading information
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_ioctl_get_channel_loading(ICM_INFO_T* picm)
{

    u_int32_t status =  SUCCESS;
    struct ifreq ifr;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    psinfo->atd.ad_id = SPECTRAL_GET_CHANINFO | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void*)&picm->slist.chan_properties[0];
    psinfo->atd.ad_out_size = sizeof(CHANNEL_PROPERTIES_T) *  MAX_NUM_CHANNELS;
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        status = FAILURE;
        perror("icm : ioctl fail (SPECTRAL_GET_CHANINFO)");
    }

    return status;
}
/*
 * Function     : icm_wal_ioctl_clear_spectral_chan_properties
 * Description  : clean the spectral channel properties
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */

int icm_wal_ioctl_clear_spectral_chan_properties(ICM_INFO_T* picm)
{
    u_int32_t status = SUCCESS;
    struct ifreq ifr;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */

    psinfo->atd.ad_id = SPECTRAL_CLEAR_CHANINFO | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void*)&status;
    psinfo->atd.ad_out_size = sizeof(u_int32_t);
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        status = FAILURE;
        perror("icm : ioctl fail (SPECTRAL_CLEAR_CHANINFO)");
    }

    return status;


}

/*
 * Function     : icm_wal_ioctl_set_icm_active
 * Description  : set the spectral module debug level
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_ioctl_set_icm_active(ICM_INFO_T *picm, u_int32_t val)
{
    u_int32_t status = SUCCESS;
    struct ifreq ifr;

    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    /* XXX : We should set priority to 0 */

    psinfo->atd.ad_id = SPECTRAL_SET_ICM_ACTIVE | ATH_DIAG_IN;
    psinfo->atd.ad_in_data = (void *)&val;
    psinfo->atd.ad_in_size = sizeof(u_int32_t);
    psinfo->atd.ad_out_size = 0;
    psinfo->atd.ad_out_data = NULL;
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        status = FAILURE;
        perror("icm : ioctl fail (SPECTRAL_SET_ICM_ACTIVE)");
    }

    return status;

}

/*
 * Function     : icm_wal_ioctl_get_nominal_noisefloor
 * Description  : get nominal noisefloor
 * Input params : pointer to icm info structrue
 * Return       : success/failure
 *
 */
int icm_wal_ioctl_get_nominal_noisefloor(ICM_INFO_T *picm)
{

    int nominal_nf =  ATH_DEFAULT_NOMINAL_NF;
    struct ifreq ifr;

    ICM_SPECTRAL_INFO_T* psinfo = &picm->sinfo;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    psinfo->atd.ad_id = SPECTRAL_GET_NOMINAL_NOISEFLOOR | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void*)&nominal_nf;
    psinfo->atd.ad_out_size = sizeof(nominal_nf);
    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        nominal_nf = ATH_DEFAULT_NOMINAL_NF;
        perror("icm : ioctl fail (SPECTRAL_GET_NOMINAL_NOISEFLOOR)");
    }

    return nominal_nf;

}
/*
 * Function     : icm_wal_ioctl_do_80211_scan
 * Description  : do an 802.11 scan and print scan results
 * Input params : pointer to icm
 * Return       : 0 on success, -1 on general errors, -2 on scan cancellation
 */

int icm_wal_ioctl_do_80211_scan(ICM_INFO_T * picm)
{
    struct iwreq wrq;
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

    /* no scan options */
    wrq.u.data.pointer = NULL;
    wrq.u.data.flags = 0;
    wrq.u.data.length = 0;
    os_strlcpy(wrq.ifr_name,  picm->dev_ifname, IFNAMSIZ);

    /* Init timeout value -> 250ms between set and first get */
    tv.tv_sec = 0;
    tv.tv_usec = 250000;

    /* cleanup the args */
    memset(&scanopt, 0, sizeof(scanopt));

    /* The below locking doesn't matter for standalone mode */
    pthread_mutex_lock(&picm->scanner_thread_mutex);
    if (picm->is_80211scan_cancel_requested == TRUE) {
        pthread_mutex_unlock(&picm->scanner_thread_mutex);
        fprintf(stderr, "%-8.16s  Scan cancelled\n\n", picm->dev_ifname);
        picm->substate = ICM_STATE_INVALID;
        return -2;
    }
    pthread_mutex_unlock(&picm->scanner_thread_mutex);

    /* Configure requisite Scan Band so that we don't scan on channels we aren't
       interested in. */
    if (icm_configure_scanband(pdev, picm) != SUCCESS) {
        picm->substate = ICM_STATE_INVALID;
        return -1;
    }

    /* If 11ac offload, temporarily enable delivery of scan channel
       events from firmware - necessary for getting NF and cycle
       counts. */
    if (icm_enable_11ac_scanchevents(pdev, picm, is_11ac) != SUCCESS) {
        picm->substate = ICM_STATE_INVALID;
        icm_restore_scanband(pdev, picm);
        return -1;
    }

    /* Enable Sending of additional IEs to host */
    icm_enable_sending_additional_ies(pdev, picm);

    /* initialize scanning */
    if (ioctl(pnlinfo->sock_fd, SIOCSIWSCAN, &wrq) < 0) {

        if ((errno != EPERM) || (scanflags != 0)) {
            fprintf(stderr, "%-8.16s  Interface doesn't support scanning : %s\n\n",
                    picm->dev_ifname, strerror(errno));
            picm->substate = ICM_STATE_INVALID;
            icm_restore_scanband(pdev, picm);
            icm_disable_11ac_scanchevents(pdev, picm, is_11ac);
            icm_disable_sending_additional_ies(pdev, picm);
            return -1;
        }
        tv.tv_usec = 0;
    }

    timeout -= tv.tv_usec;


    /* Forever */
    while (1) {
        fd_set rfds;        /* File descriptor for select */
        int last_fd;        /* Last fd */
        int ret;

        /* We must regenerate rfds each time */
        FD_ZERO(&rfds);
        last_fd = -1;

        pthread_mutex_lock(&picm->scanner_thread_mutex);
        if (picm->is_80211scan_cancel_requested == TRUE) {
            pthread_mutex_unlock(&picm->scanner_thread_mutex);
            icm_cancel_80211_scan(picm);
            if (buffer) {
                free(buffer);
            }
            fprintf(stderr, "%-8.16s  Scan cancelled\n\n", picm->dev_ifname);
            picm->substate = ICM_STATE_INVALID;
            icm_restore_scanband(pdev, picm);
            icm_disable_11ac_scanchevents(pdev, picm, is_11ac);
            icm_disable_sending_additional_ies(pdev, picm);
            return -2;
        }
        pthread_mutex_unlock(&picm->scanner_thread_mutex);


        /* Wait untill some thing happens */
        ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);

        /* Check for errors */
        if (ret < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "unhandled signal - exiting...\n");
            picm->substate = ICM_STATE_INVALID;
            icm_restore_scanband(pdev, picm);
            icm_disable_11ac_scanchevents(pdev, picm, is_11ac);
            icm_disable_sending_additional_ies(pdev, picm);
            return (-1);
        }

        /* Check for timeout */
        if (ret == 0) {
            unsigned char *newbuf;
realloc:
            /* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
            newbuf = realloc(buffer, buflen);
            if (newbuf == NULL) {
                if (buffer) {
                    free(buffer);
                }
                fprintf(stderr, "%s: Allocation failed\n", __func__);
                picm->substate = ICM_STATE_INVALID;
                icm_restore_scanband(pdev, picm);
                icm_disable_11ac_scanchevents(pdev, picm, is_11ac);
                icm_disable_sending_additional_ies(pdev, picm);
                return (-1);
            }

            buffer = newbuf;

            /* Try to read the results */
            wrq.u.data.pointer = buffer;
            wrq.u.data.flags = 0;
            wrq.u.data.length = buflen;
            /* To be safe, we don't assume that the ifname is already set */
            os_strlcpy(wrq.ifr_name,  picm->dev_ifname, IFNAMSIZ);

            if (ioctl(pnlinfo->sock_fd, SIOCGIWSCAN, &wrq) < 0) {

                /* check if buffer is too small */
                if ((errno == E2BIG) && (range.we_version_compiled > 16)) {

                    /* check if the driver gave us any hints */
                    if (wrq.u.data.length > buflen) {
                        buflen = wrq.u.data.length;
                    } else {
                        buflen *= 2;
                    }

                    /* Try again */
                    goto realloc;
                }

                /* check if results not available yet */
                if (errno == EAGAIN) {
                    /* Restart timer for only 100ms */
                    tv.tv_sec = 0;
                    tv.tv_usec = 100000;
                    timeout -= tv.tv_usec;
                    if (timeout > 0) {
                        /* Try again */
                        continue;
                    }
                }

                /* Bad error */
                free(buffer);
                fprintf(stderr, "%-8.16s  Failed to read scan data : %s\n\n", picm->dev_ifname, strerror(errno));
                picm->substate = ICM_STATE_INVALID;
                icm_restore_scanband(pdev, picm);
                icm_disable_11ac_scanchevents(pdev, picm, is_11ac);
                icm_disable_sending_additional_ies(pdev, picm);
                return (-1);
            } else {
                pthread_mutex_lock(&picm->scanner_thread_mutex);
                if (picm->is_80211scan_cancel_requested == TRUE) {
                    pthread_mutex_unlock(&picm->scanner_thread_mutex);
                    free(buffer);
                    fprintf(stderr, "%-8.16s  Scan cancelled\n\n", picm->dev_ifname);
                    picm->substate = ICM_STATE_INVALID;
                    icm_restore_scanband(pdev, picm);
                    icm_disable_11ac_scanchevents(pdev, picm, is_11ac);
                    icm_disable_sending_additional_ies(pdev, picm);
                    return -2;
                }
                /* Beyond this point, no cancellation can be done */
                picm->is_80211scan_done = TRUE;

                pthread_mutex_unlock(&picm->scanner_thread_mutex);

                /* We have the results, handle them */
                break;
            }
        }
    }

    icm_restore_scanband(pdev, picm);
    icm_disable_11ac_scanchevents(pdev, picm, is_11ac);
    icm_disable_sending_additional_ies(pdev, picm);

    if (wrq.u.data.length) {
        struct iw_event iwe;
        ICM_EVENT_PARSE_CONTEXT_T ctx;
        struct iwscan_state state = {.ap_num = 1,.val_index = 0 };
        ICM_EV_PARSE_RESULT_T ret;

        memset(&ctx, 0, sizeof(ICM_EVENT_PARSE_CONTEXT_T));
        icm_init_event_parse_context(&ctx, (char *)buffer, wrq.u.data.length);

        do {
            /* Extract an event and print it */
            ret = icm_get_next_event(&ctx, &iwe, range.we_version_compiled);
            if (ret == ICM_EV_PARSE_RESULT_SUCCESS) {
                icm_update_scan_results(picm, &iwe, &state, &range, has_range);
            }
        } while (ret == ICM_EV_PARSE_RESULT_SUCCESS || ret == ICM_EV_PARSE_RESULT_UNKNOWN_EV);
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "\n");
    } else {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_DEFAULT, ICM_MODULE_ID_SCAN, "%-8.16s  No scan results\n\n", picm->dev_ifname);
    }

    free(buffer);

    picm->substate = ICM_STATE_INVALID;
    return 0;
}
/*
 * Function     : icm_wal_ioctl_cancel_80211_scan
 * Description  : Cancel all 802.11 scans for the given icm
 * Input params : pointer to icm
 * Return       : success/failure
 */
int icm_wal_ioctl_cancel_80211_scan(ICM_INFO_T * picm)
{
    struct iwreq iwr;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    memset(&iwr, 0, sizeof(iwr));

    os_strlcpy(iwr.ifr_name, picm->dev_ifname, sizeof(iwr.ifr_name));
    iwr.u.data.pointer = NULL;
    iwr.u.data.length = 0;
    iwr.u.data.flags = IEEE80211_IOC_CANCEL_SCAN;

    /* Send big param request */
    if (ioctl(iocinfo->sock_fd, IEEE80211_IOCTL_P2P_BIG_PARAM, &iwr) < 0) {
        perror("icm : ioctl to cancel request");
        return FAILURE;
    }

    ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "%s: Cancelled scan\n", __func__);

    return SUCCESS;
}
/*
 * Function     : icm_wal_ioctl_do_80211_priv
 * Description  : interface to 80211 priv ioctl
 * Input params : pointer to icm, channel number, pointer to
 *                iwreq, interface name
 * Return       : success/failure
 */

int icm_wal_ioctl_do_80211_priv(ICM_INFO_T *picm, struct iwreq *iwr, const char *ifname, int op, void *data, size_t len)
{
#define N(a)    (sizeof(a)/sizeof(a[0]))

    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    memset(iwr, 0, sizeof(*iwr));
    os_strlcpy(iwr->ifr_name, ifname, IFNAMSIZ);

    if (len < IFNAMSIZ) {
        /*
         * Argument data fits inline; put it there.
         */
        memcpy(iwr->u.name, data, len);
    } else {
        /*
         * Argument data too big for inline transfer; setup a
         * parameter block instead; the kernel will transfer
         * the data for the driver.
         */
        iwr->u.data.pointer = data;
        iwr->u.data.length = len;
    }

    if (ioctl(pnlinfo->sock_fd, op, iwr) < 0) {

        static const char *opnames[] = {
            "ioctl[IEEE80211_IOCTL_SETPARAM]",
            "ioctl[IEEE80211_IOCTL_GETPARAM]",
            "ioctl[IEEE80211_IOCTL_SETKEY]",
            "ioctl[SIOCIWFIRSTPRIV+3]",
            "ioctl[IEEE80211_IOCTL_DELKEY]",
            "ioctl[SIOCIWFIRSTPRIV+5]",
            "ioctl[IEEE80211_IOCTL_SETMLME]",
            "ioctl[SIOCIWFIRSTPRIV+7]",
            "ioctl[IEEE80211_IOCTL_SETOPTIE]",
            "ioctl[IEEE80211_IOCTL_GETOPTIE]",
            "ioctl[IEEE80211_IOCTL_ADDMAC]",
            "ioctl[SIOCIWFIRSTPRIV+11]",
            "ioctl[IEEE80211_IOCTL_DELMAC]",
            "ioctl[SIOCIWFIRSTPRIV+13]",
            "ioctl[IEEE80211_IOCTL_CHANLIST]",
            "ioctl[SIOCIWFIRSTPRIV+15]",
            "ioctl[IEEE80211_IOCTL_GETRSN]",
            "ioctl[SIOCIWFIRSTPRIV+17]",
            "ioctl[IEEE80211_IOCTL_GETKEY]",
        };

        op -= SIOCIWFIRSTPRIV;

        if (0 <= op && op < N(opnames))
            perror(opnames[op]);
        else
            perror("ioctl[unknown???]");
        return -1;
    }
    return 0;
#undef N
}
/*
 * Function     : icm_wal_ioctl_get_currdomain
 * Description  : Obtain the current RF regulatory domain being used by
 *                the radio and place it in the rfreg_domain of the
 *                ICM_INFO_T object.
 * Input params : pointer to icm
 * Return       : 0 on success, -1 on failure
 */
    int
icm_wal_ioctl_get_currdomain(ICM_INFO_T* picm)
{
    struct iwreq wrq;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    if (picm == NULL) {
        return -1;
    }

    memset(&wrq, 0, sizeof(wrq));
    os_strlcpy(wrq.ifr_name,  picm->radio_ifname, IFNAMSIZ);
    wrq.u.mode = SPECIAL_PARAM_SHIFT | SPECIAL_PARAM_COUNTRY_ID;

    if (ioctl(pnlinfo->sock_fd, ATH_HAL_IOCTL_GETPARAM, &wrq) < 0) {
        fprintf(stderr, "%-8.16s Could not get regulatory domain.\n\n", picm->radio_ifname);
        perror("ioctl(domain)");
        return -1;
    }

    picm->rfreg_domain = *((int32_t*) wrq.u.name);
    return 0;
}
/*
 * Function     : icm_wal_ioctl_get_ieee_chaninfo
 * Description  : prints ieee related channel information
 * Input params : pointer to ieee80211_ath_channel
 * Return       : success/failure
 */
int icm_wal_ioctl_get_ieee_chaninfo(ICM_INFO_T* picm)
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

    memset(&iwr, 0, sizeof(struct iwreq));
    os_strlcpy(iwr.ifr_name, ifname, IFNAMSIZ);
    config = (struct ieee80211_wlanconfig *)&chans_160;
    memset(config, 0, sizeof(*config));
    config->cmdtype = IEEE80211_WLANCONFIG_GETCHANINFO_160;
    iwr.u.data.pointer = (void *) &chans_160;
    iwr.u.data.length = sizeof(chans_160);

    if (icm_get_80211_priv(picm, ifname, IEEE80211_IOCTL_GETCHANINFO, &chans, sizeof(chans)) < 0) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "unable to get channel information\n");
        return FAILURE;
    }

    if (ioctl(iocinfo->sock_fd, IEEE80211_IOCTL_CONFIG_GENERIC, &iwr) < 0) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "unable to get channel information for 160 MHz\n");
        return FAILURE;
    }

    if (!allchans) {
        struct ieee80211req_chanlist active;

        if (icm_get_80211_priv(picm, ifname, IEEE80211_IOCTL_GETCHANLIST, &active, sizeof(active)) < 0) {
            ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_SCAN, "unable to get active channel list\n");
            return FAILURE;
        }

        memset(&achans, 0, sizeof(achans));
        memset(&achans_160, 0, sizeof(achans_160));

        for (i = 0; i < chans.ic_nchans; i++) {
            c = &chans.ic_chans[i];
            if (isset(active.ic_channels,
                        icm_convert_mhz2channel(c->ic_freq)) || allchans) {
                achans.ic_chans[achans.ic_nchans++] = *c;
            }

            c_160 = &chans_160.ic_chans[i];
            if (isset(active.ic_channels, icm_convert_mhz2channel(c_160->ic_freq)) || allchans) {
                achans_160.ic_chans[achans_160.ic_nchans++] = *c_160;
            }
        }
    } else {
        achans = chans;
        achans_160 = chans_160;
    }

    for (i = 0; i < achans.ic_nchans; i++) {
        icm_update_channel_ieee_info(picm,
                achans.ic_chans[i].ic_freq, &achans.ic_chans[i]);
    }

    for (i = 0; i < achans_160.ic_nchans; i++) {
        icm_append_160MHz_channel_ieee_info(picm,
                achans_160.ic_chans[i].ic_freq, &achans_160.ic_chans[i]);
    }

    icm_printf("%s exit \n", __func__);
    return SUCCESS;
}

int icm_wal_ioctl_get_reg_domain(ICM_INFO_T* picm)
{
    struct iwreq iwr;

    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name,  picm->dev_ifname, IFNAMSIZ);
    iwr.u.mode = IEEE80211_PARAM_DFSDOMAIN;

    if (ioctl(pnlinfo->sock_fd, IEEE80211_IOCTL_GETPARAM, &iwr) < 0) {
        perror("IEEE80211_IOCTL_GETPARAM");
        return FAILURE;
    }

    picm->dfs_domain = iwr.u.param.value;
    return SUCCESS;
}

int icm_wal_ioctl_get_currchan(ICM_INFO_T* picm)
{
    struct iwreq  wrq;
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_NLSOCK_T *pnlinfo = ICM_GET_ADDR_OF_NLSOCK_INFO(pdev);

    if (picm == NULL) {
        return -1;
    }

    memset(&wrq, 0, sizeof(wrq));
    os_strlcpy(wrq.ifr_name,  picm->dev_ifname, IFNAMSIZ);

    if (ioctl(pnlinfo->sock_fd, SIOCGIWFREQ, &wrq) < 0) {
        fprintf(stderr, "%-8.16s Could not get channel info.\n\n", picm->dev_ifname);
        return -1;
    }

    return icm_convert_mhz2channel((u_int32_t)wrq.u.freq.m/100000);
}

/*
 * Function     : icm_wal_ioctl_init_channel_params
 * Description  : init channel related params
 *                Set the number of valid channels to zero
 *                Set the channel index for spectral to starting index
 * Input params : pointer to icm
 * Return       : void
 *
 */
void icm_wal_ioctl_init_channel_params(ICM_INFO_T* picm)
{
    int i = 0;

    picm->chlist_a.count = 0;
    picm->chlist_a.channel_index = 0;
    memset(&picm->chlist_a.ch, 0, sizeof(picm->chlist_a.ch));
    picm->chlist_bg.count = 0;
    picm->chlist_bg.channel_index = 0;
    memset(&picm->chlist_bg.ch, 0, sizeof(picm->chlist_bg.ch));

    for (i = 0; i < MAX_NUM_CHANNEL; i++)
    {
        ICM_CLEAR_CHANNEL_EXCLUDE(picm, i);
        ICM_SET_CHANNEL_BLUSABILITY(picm, i, MAX_USABILITY);
    }
}

/*
 * Function     : icm_wal_ioctl_set_width_and_channel
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
int icm_wal_ioctl_set_width_and_channel(ICM_INFO_T *picm, char *dev_ifname)
{
    char cmd[CMD_BUF_SIZE] = {'\0'};
    char modestr[24] = {'\0'};
    int ret = 0;
    ICM_CH_BW_T channel_width;
    int channel = 0;
    int cfreq2_channel = 0;
    ICM_DEV_INFO_T* pdev = get_pdev();
    channel_width = picm->channel_width;
    if (picm->ch_selection_mode == ICM_CH_SELECTION_MODE_MANUAL) {
        ICM_DPRINTF(pdev, ICM_PRCTRL_FLAG_NONE, ICM_DEBUG_LEVEL_MAJOR, ICM_MODULE_ID_MAIN,  "In manual channel selection mode; using default channel\n");
        channel = picm->def_channel;
    } else if (picm->best_channel <= 0) {
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

    /* Set mode */

    if (channel_width >= ICM_CH_BW_INVALID) {
        err("Invalid channel width enumeration %u", channel_width);
        return FAILURE;
    }

    if (channel_width == ICM_CH_BW_80_PLUS_80) {
        /* XXX Add default channel processing for cfreq2 */
        cfreq2_channel = picm->best_cfreq2_channel;
    }

    if (icm_compose_phymode_str(picm->phy_spec,
                channel_width,
                modestr,
                sizeof(modestr)) == NULL) {
        return FAILURE;
    }
    snprintf(cmd, sizeof(cmd), "%s %s %s %s", "iwpriv",
            dev_ifname, "mode",  modestr);
    ret = system(cmd);

    if (ret == -1) {
        perror("icm : system");
        return FAILURE;
    }

    if (WEXITSTATUS(ret) != 0) {
        err("Error in setting mode; command was: %s", cmd);
        return FAILURE;
    }

    if (channel_width == ICM_CH_BW_80_PLUS_80) {
        /* Set cfreq2 */
        snprintf(cmd, sizeof(cmd), "%s %s %s %1d", "iwpriv",
                dev_ifname, "cfreq2", icm_convert_ieee2mhz(cfreq2_channel));
        ret = system(cmd);

        if (ret == -1) {
            perror("icm : system");
            return FAILURE;
        }

        if (WEXITSTATUS(ret) != 0) {
            err("Error in setting channel; command was: %s", cmd);
            return FAILURE;
        }
    }

    /* Set channel */
    snprintf(cmd, sizeof(cmd), "%s %s %s %1d", "iwconfig",
            dev_ifname, "channel", channel);
    ret = system(cmd);

    if (ret == -1) {
        perror("icm : system");
        return FAILURE;
    }

    if (WEXITSTATUS(ret) != 0) {
        err("Error in setting channel; command was: %s", cmd);
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
 * Function     : icm_wal_ioctl_get_paramrange
 * Description  : gets parameter range information
 * Input params : pointer to icm
 * Output params: pointer to iw_range structure
 * Return       : success/failure
 */
int icm_wal_ioctl_get_paramrange(ICM_INFO_T *picm, struct iw_range *range)
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

    assert(WIRELESS_EXT > 15);

    if (picm == NULL || range == NULL) {
        return FAILURE;
    }

    memset(buf, 0, sizeof(buf));

    wrq.u.data.pointer = (caddr_t)buf;
    wrq.u.data.length = sizeof(buf);
    wrq.u.data.flags = 0;
    os_strlcpy(wrq.ifr_name,  picm->dev_ifname, IFNAMSIZ);

    if (ioctl(pnlinfo->sock_fd, SIOCGIWRANGE, &wrq) < 0) {
        return FAILURE;
    }

    temp_range = (struct iw_range*)buf;

    if(wrq.u.data.length != sizeof(struct iw_range) ||
            temp_range->we_version_compiled <= 15) {
        fprintf(stderr, "%-8.16s  Driver compiled with too old "
                "a version of WIRELESS_EXT\n\n",
                picm->dev_ifname);

        return FAILURE;

    }

    if (temp_range->we_version_compiled > WIRELESS_EXT_MAX_VERSION) {
        fprintf(stderr, "%-8.16s  Driver compiled for version %d. "
                "We can handle only upto version %d\n\n",
                picm->dev_ifname,
                temp_range->we_version_compiled,
                WIRELESS_EXT_MAX_VERSION);

        return FAILURE;
    }

    memcpy((char *)range, buf, sizeof(struct iw_range));

    return SUCCESS;
}
/*
 * Function     : icm_wal_ioctl_is_dev_up
 * Description  : find whether a device is up
 * Input params : pointer to ICM_DEV_INFO_T, device name
 * Out params   : pointer to bool indicating whether device is
 up (valid only on SUCCESS)
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_ioctl_is_dev_up(ICM_DEV_INFO_T* pdev,
        char *dev_ifname,
        bool *isup)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);
    struct ifreq ifr;

    *isup = FALSE;

    os_strlcpy(ifr.ifr_name, dev_ifname, sizeof(ifr.ifr_name));

    if (ioctl(iocinfo->sock_fd, SIOCGIFFLAGS, &ifr) < 0) {
        perror("icm : ioctl");
        return FAILURE;
    }

    if (ifr.ifr_flags & IFF_UP) {
        *isup = TRUE;
    }

    return SUCCESS;
}
/*
 * Function     : icm_wal_ioctl_is_dev_ap
 * Description  : find whether a device is in AP mode
 * Input params : pointer to ICM_DEV_INFO_T, device name
 * Out params   : pointer to bool indicating whether device is
 in AP mode (valid only on SUCCESS)
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_ioctl_is_dev_ap(ICM_DEV_INFO_T* pdev,
        char *dev_ifname,
        bool *isap)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);
    struct iwreq iwr;

    *isap = FALSE;

    os_strlcpy(iwr.ifr_name, dev_ifname, sizeof(iwr.ifr_name));

    if (ioctl(iocinfo->sock_fd, SIOCGIWMODE, &iwr) < 0) {
        perror("icm : ioctl");
        return FAILURE;
    }

    if (iwr.u.mode > IW_MODE_MESH) {
        err("Unknown mode");
        return FAILURE;
    }

    if (iwr.u.mode == IW_MODE_MASTER) {
        *isap = TRUE;
    }

    return SUCCESS;
}
/*
 * Function     : icm_wal_ioctl_get_iface_addr
 * Description  : get the interface address
 * Input params : pointer to ICM_DEV_INFO_T, interface name,
 * Out params   : interface address.
 * Return       : SUCCESS/FAILURE
 */
int icm_wal_ioctl_get_iface_addr(ICM_DEV_INFO_T* pdev, char* ifname, u_int8_t *ifaddr)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;
    os_strlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

    if (ioctl(iocinfo->sock_fd, SIOCGIFHWADDR, &ifr) < 0) {
        perror("icm : ioctl");
        return FAILURE;
    }

    memcpy(ifaddr, ifr.ifr_ifru.ifru_hwaddr.sa_data, ETH_ALEN);

    return SUCCESS;

}
/*
 * Function     : icm_wal_ioctl_get_radio_priv_int_param
 * Description  : Get a radio-private integer parameter
 * Input params : pointer to pdev info, radio interface name, required parameter
 * Return       : On success: Value of parameter
 *                On error  : -1
 */
int icm_wal_ioctl_get_radio_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname, int param)
{
    struct iwreq iwr;
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.mode = param | ATH_PARAM_SHIFT;
    if (ioctl(iocinfo->sock_fd, ATH_IOCTL_GETPARAM, &iwr) < 0) {
        perror("ATH_IOCTL_GETPARAM");
        return -1;
    }

    return iwr.u.param.value;
}
/*
 * Function     : icm_wal_ioctl_get_vap_priv_int_param
 * Description  : Return private parameter of the given VAP from driver.
 * Input params : const char pointer pointing to interface name and required parameter
 * Return       : Success: value of the private param
 *                Failure: -1
 *
 */
int icm_wal_ioctl_get_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param)
{
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.mode = param;

    if (ioctl(iocinfo->sock_fd, IEEE80211_IOCTL_GETPARAM, &iwr) < 0) {
        perror("IEEE80211_IOCTL_GETPARAM");
        return -1;
    }
    /* returns value of the VAP private param(eg. phy spec channel width )*/
    return iwr.u.param.value;
}
/*
 * Function     : icm_wal_ioclt_set_vap_priv_int_param
 * Description  : Set a device-private integer parameter
 * Input params : pointer to pdev info, device interface name, parameter,
 *                value.
 * Return       : On success: 0
 *                On error  : -1
 */
int icm_wal_ioctl_set_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int32_t val)
{
    struct iwreq iwr;
    ICM_IOCSOCK_T *iocinfo = ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev);

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.mode = param;
    memcpy(iwr.u.name + sizeof(int32_t), &val, sizeof(val));

    if (ioctl(iocinfo->sock_fd, IEEE80211_IOCTL_SETPARAM, &iwr) < 0) {
        perror("IEEE80211_IOCTL_SETPARAM");
        return -1;
    }

    return 0;
}
/*
 * Function     : icm_wal_ioctl_get_channel_width
 * Description  : Get current channel width from driver
 * Input params : pointer to icm info structure
 * Return       : Channel width on success
 *                IEEE80211_CWM_WIDTHINVALID on failure
 */
enum ieee80211_cwm_width icm_wal_ioctl_get_channel_width(ICM_INFO_T* picm)
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

    psinfo = &picm->sinfo;
    psinfo->atd.ad_id = SPECTRAL_GET_CHAN_WIDTH | ATH_DIAG_DYN;
    psinfo->atd.ad_in_data = NULL;
    psinfo->atd.ad_in_size = 0;
    psinfo->atd.ad_out_data = (void*)&ch_width;
    psinfo->atd.ad_out_size = sizeof(u_int32_t);

    os_strlcpy(ifr.ifr_name, psinfo->atd.ad_name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t)&psinfo->atd.ad_name;

    if (ioctl(pnlinfo->sock_fd, SIOCGATHPHYERR, &ifr) < 0) {
        ch_width = IEEE80211_CWM_WIDTHINVALID;
        perror("icm: SIOCGATHPHYERR ioctl fail (SPECTRAL_GET_CHAN_WIDTH)");
    }

    return ch_width;
}
#endif /* ICM_RTR_DRIVE */
