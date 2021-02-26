/*
 * Copyright (c) 2014,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2014 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */
#ifndef _SPECTRAL_DATA_H_
#define _SPECTRAL_DATA_H_

#include "spec_msg_proto.h"

#ifndef MAX_SPECTRAL_MSG_ELEMS
#define MAX_SPECTRAL_MSG_ELEMS 10
#endif

#define MAX_SPECTRAL_CHAINS  3

#define SPECTRAL_SIGNATURE  0xdeadbeef

#ifndef NETLINK_ATHEROS
#define NETLINK_ATHEROS              (NETLINK_GENERIC + 1)
#endif


#ifdef WIN32
#pragma pack(push, spectral_data, 1)
#define __ATTRIB_PACK
#else
#ifndef __ATTRIB_PACK
#define __ATTRIB_PACK __attribute__ ((packed))
#endif
#endif

typedef struct spectral_classifier_params {
    int spectral_20_40_mode; /* Is AP in 20-40 mode? */
    int spectral_dc_index;
    int spectral_dc_in_mhz;
    int upper_chan_in_mhz;
    int lower_chan_in_mhz;
} __ATTRIB_PACK SPECTRAL_CLASSIFIER_PARAMS;


/* 11ac chipsets will have a max of 520 bins in each 80 MHz segment.
 *
 * For 11ac chipsets prior to AR900B version 2.0, a max of 512 bins are delivered.
 * However, there can be additional bins reported for AR900B version 2.0 and
 * QCA9984 as described next:
 *
 * AR900B version 2.0: An additional tone is processed on the right hand side in
 * order to facilitate detection of radar pulses out to the extreme band-edge of
 * the channel frequency. Since the HW design processes four tones at a time,
 * this requires one additional Dword to be added to the search FFT report.
 *
 * QCA9984: When spectral_scan_rpt_mode=2, i.e 2-dword summary + 1x-oversampled
 * bins (in-band) per FFT, then 8 more bins (4 more on left side and 4 more on
 * right side)are added.
 */
#define MAX_NUM_BINS 520

typedef struct spectral_data {
    int16_t     spectral_data_len;
    int16_t     spectral_rssi;
    int16_t     spectral_bwinfo;
    int32_t     spectral_tstamp;
    int16_t     spectral_max_index;
    int16_t     spectral_max_mag;
} __ATTRIB_PACK SPECTRAL_DATA;

struct spectral_scan_data {
    u_int16_t chanMag[128];
    u_int8_t  chanExp;
    int16_t   primRssi;
    int16_t   extRssi;
    u_int16_t dataLen;
    u_int32_t timeStamp;
    int16_t   filtRssi;
    u_int32_t numRssiAboveThres;
    int16_t   noiseFloor;
    u_int32_t center_freq;
};

typedef struct spectral_msg {
    int16_t      num_elems;
    SPECTRAL_DATA data_elems[MAX_SPECTRAL_MSG_ELEMS];
} SPECTRAL_MSG;

typedef struct spectral_samp_data {
    int16_t     spectral_data_len;                              /* indicates the bin size */
    int16_t     spectral_data_len_sec80;                        /* indicates the bin size for secondary 80 segment */
    int16_t     spectral_rssi;                                  /* indicates RSSI */
    int16_t     spectral_rssi_sec80;                            /* indicates RSSI for secondary 80 segment */
    int8_t      spectral_combined_rssi;                         /* indicates combined RSSI from all antennas */
    int8_t      spectral_upper_rssi;                            /* indicates RSSI of upper band */
    int8_t      spectral_lower_rssi;                            /* indicates RSSI of lower band */
    int8_t      spectral_chain_ctl_rssi[MAX_SPECTRAL_CHAINS];   /* RSSI for control channel, for all antennas */
    int8_t      spectral_chain_ext_rssi[MAX_SPECTRAL_CHAINS];   /* RSSI for extension channel, for all antennas */
    u_int8_t    spectral_max_scale;                             /* indicates scale factor */
    int16_t     spectral_bwinfo;                                /* indicates bandwidth info */
    int32_t     spectral_tstamp;                                /* indicates timestamp */
    int16_t     spectral_max_index;                             /* indicates the index of max magnitude */
    int16_t     spectral_max_index_sec80;                       /* indicates the index of max magnitude for secondary 80 segment */
    int16_t     spectral_max_mag;                               /* indicates the maximum magnitude */
    int16_t     spectral_max_mag_sec80;                         /* indicates the maximum magnitude for secondary 80 segment */
    u_int8_t    spectral_max_exp;                               /* indicates the max exp */
    int32_t     spectral_last_tstamp;                           /* indicates the last time stamp */
    int16_t     spectral_upper_max_index;                       /* indicates the index of max mag in upper band */
    int16_t     spectral_lower_max_index;                       /* indicates the index of max mag in lower band */
    u_int8_t    spectral_nb_upper;                              /* Not Used */
    u_int8_t    spectral_nb_lower;                              /* Not Used */
    SPECTRAL_CLASSIFIER_PARAMS classifier_params;               /* indicates classifier parameters */
    u_int16_t   bin_pwr_count;                                  /* indicates the number of FFT bins */

    /* For chipsets prior to AR900B version 2.0, a max of 512 bins are delivered.
     * However, there can be additional bins reported for AR900B version 2.0 and
     * QCA9984 as described next:
     *
     * AR900B version 2.0: An additional tone is processed on the right hand
     * side in order to facilitate detection of radar pulses out to the extreme
     * band-edge of the channel frequency. Since the HW design processes four
     * tones at a time, this requires one additional Dword to be added to the
     * search FFT report.
     *
     * QCA9984: When spectral_scan_rpt_mode=2, i.e 2-dword summary +
     * 1x-oversampled bins (in-band) per FFT, then 8 more bins (4 more on left
     * side and 4 more on right side)are added.
     */
    u_int8_t    lb_edge_extrabins;                              /* Number of extra bins on left band edge.*/
    u_int8_t    rb_edge_extrabins;                              /* Number of extra bins on right band edge.*/

    u_int16_t   bin_pwr_count_sec80;                            /* indicates the number of FFT bins in secondary 80 segment */
    u_int8_t    bin_pwr[MAX_NUM_BINS];                          /* contains FFT magnitudes */
    u_int8_t    bin_pwr_sec80[MAX_NUM_BINS];                    /* contains FFT magnitudes for the secondary 80 segment */
    struct INTERF_SRC_RSP interf_list;                          /* list of interfernce sources */
    int16_t noise_floor;                                        /* indicates the current noise floor */
    int16_t noise_floor_sec80;                                  /* indicates the current noise floor for secondary 80 segment*/
    u_int32_t   ch_width;                                       /* Channel width 20/40/80/160 MHz */
} __ATTRIB_PACK SPECTRAL_SAMP_DATA;

typedef enum _dcs_int_type {
    SPECTRAL_DCS_INT_NONE,
    SPECTRAL_DCS_INT_CW,
    SPECTRAL_DCS_INT_WIFI
} DCS_INT_TYPE;

/* This should match the defination in drivers/wlan_modules/include/ath_dev.h */
#define ATH_CAP_DCS_CWIM     0x1
#define ATH_CAP_DCS_WLANIM   0x2

/*
 * SAMP : Spectral Analysis Messaging Protocol
 *        Message format
 */
typedef struct spectral_samp_msg {
    u_int32_t      signature;               /* Validates the SAMP message */
    u_int16_t      freq;                    /* Operating frequency in MHz */
    u_int16_t      vhtop_ch_freq_seg1;      /* VHT Segment 1 centre frequency
                                               in MHz */
    u_int16_t      vhtop_ch_freq_seg2;      /* VHT Segment 2 centre frequency
                                               in MHz */
    u_int16_t      freq_loading;            /* How busy was the channel */
    u_int16_t      dcs_enabled;             /* Whether DCS is enabled */
    DCS_INT_TYPE   int_type;                /* Interference type indicated by DCS */
    u_int8_t       macaddr[6];              /* Indicates the device interface */
    SPECTRAL_SAMP_DATA samp_data;           /* SAMP Data */
} __ATTRIB_PACK SPECTRAL_SAMP_MSG;

#ifdef WIN32
#pragma pack(pop, spectral_data)
#endif
#ifdef __ATTRIB_PACK
#undef __ATTRIB_PACK
#endif

#endif
