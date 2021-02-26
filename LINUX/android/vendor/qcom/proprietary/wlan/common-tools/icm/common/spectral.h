/*
 * Copyright (c) 2011,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2011 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 * =====================================================================================
 *
 *       Filename:  spectral.h
 *
 *    Description:  Spectral scan related
 *
 *        Version:  1.0
 *        Created:  Wednesday 05 August 2009 05:37:02  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */


#ifndef _SPECTRAL_H_
#define _SPECTRAL_H_


#include "classifier.h"
#include "spec_msg_proto.h"
#include "spectral_data.h"

#define SPECTRAL_MIN(a,b) ((a)<(b)?(a):(b))
#define SPECTRAL_MAX(a,b) ((a)>(b)?(a):(b))
#define SPECTRAL_DIFF(a,b) (SPECTRAL_MAX(a,b) - SPECTRAL_MIN(a,b))
#define SPECTRAL_ABS_DIFF(a,b) (SPECTRAL_MAX(a,b) - SPECTRAL_MIN(a,b))


#define SPECTRAL_TSMASK         0xFFFFFFFF              /* Mask for time stamp from descriptor */
#define SPECTRAL_TSSHIFT        32                      /* Shift for time stamp from descriptor */
#define SPECTRAL_TSF_WRAP       0xFFFFFFFFFFFFFFFFULL   /* 64 bit TSF wrap value */
#define SPECTRAL_64BIT_TSFMASK  0x0000000000007FFFULL   /* TS mask for 64 bit value */


#define SPECTRAL_MAX_EVENTS     1024            /* Max number of spectral events which can be q'd */

#define SPECTRAL_MAX_PULSE_BUFFER_SIZE  1024    /* Max number of pulses kept in buffer */
#define SPECTRAL_MAX_PULSE_BUFFER_MASK  0x3ff


/* statistical data */
struct spectral_stats {
    u_int32_t       num_spectral_detects;      /* total num. of spectral detects */
    u_int32_t       num_bad_length_discards;
};

/* This should match the table from if_ath.c */
enum {
    ATH_DEBUG_SPECTRAL       = 0x00000100,   /* Minimal SPECTRAL debug */
    ATH_DEBUG_SPECTRAL1      = 0x00000200,   /* Normal SPECTRAL debug */
    ATH_DEBUG_SPECTRAL2      = 0x00000400,   /* Maximal SPECTRAL debug */
    ATH_DEBUG_SPECTRAL3      = 0x00000800,   /* matched filterID display */
};

#define HAL_CAP_RADAR   0
#define HAL_CAP_AR  1
#define HAL_CAP_STRONG_DIV 2


/* These definations are related to the data format for spectral scan */
#define SPECTRAL_SCAN_BITMASK (0x10)
#define SPECTRAL_SCAN_BW_INFO_OFFSET_FROM_END (3)

/* Defines the maximum values for the spectral scan register field */
#define SPECTRAL_FFT_PERIOD_MAX_VAL  (15)
#define SPECTRAL_SCAN_PERIOD_MAX_VAL (128)
#define SPECTRAL_SCAN_COUNT_MAX_VAL  (128)

/* Data length boundry conditions */
#define SPECTRAL_20HT_MIN_LEN (62)
#define SPECTRAL_20HT_MAX_LEN (65)
#define SPECTRAL_40HT_MIN_LEN (137)
#define SPECTRAL_40HT_MAX_LEN (140)


/* classifier related */

#define SPECTRAL_HT20_NUM_BINS                      56
#define SPECTRAL_HT20_FFT_LEN                       56
#define SPECTRAL_HT20_DC_INDEX                      (SPECTRAL_HT20_FFT_LEN / 2)
#define SPECTRAL_HT40_NUM_BINS                      64
#define SPECTRAL_HT40_TOTAL_NUM_BINS                128
#define SPECTRAL_HT40_FFT_LEN                       128
#define SPECTRAL_HT40_DC_INDEX                      (SPECTRAL_HT40_FFT_LEN / 2)
#define SPECTRAL_HT20_DATA_LEN                      60
#define SPECTRAL_HT20_TOTAL_DATA_LEN                (SPECTRAL_HT20_DATA_LEN + 3)
#define SPECTRAL_HT40_DATA_LEN                      135
#define SPECTRAL_HT40_TOTAL_DATA_LEN                (SPECTRAL_HT40_DATA_LEN + 3)


#define CLASSIFY_TIMEOUT_S             2
//#define CLASSIFY_TIMEOUT_MS            (CLASSIFY_TIMEOUT_S * 1000)
//#define CLASSIFY_TIMEOUT_MS            (200)
#define DEBUG_TIMEOUT_S                1
//#define DEBUG_TIMEOUT_MS               (DEBUG_TIMEOUT_S * 1000)
#define DEBUG_TIMEOUT_MS               (100)

#ifndef __ATTRIB_PACK
#define __ATTRIB_PACK   __attribute__ ((packed))
#endif



typedef struct ht20_bin_mag_data {
    u_int8_t bin_magnitude[SPECTRAL_HT20_NUM_BINS];
} __ATTRIB_PACK HT20_BIN_MAG_DATA;


typedef struct ht40_bin_mag_data {
    u_int8_t bin_magnitude[SPECTRAL_HT40_NUM_BINS];
} __ATTRIB_PACK HT40_BIN_MAG_DATA;


typedef struct max_mag_index_data {
    u_int8_t
        max_mag_bits01:2,
        bmap_wt:6;
    u_int8_t max_mag_bits29;
    u_int8_t
        max_index_bits05:6,
        max_mag_bits1110:2;
}__ATTRIB_PACK MAX_MAG_INDEX_DATA;

typedef struct ht20_fft_packet {
    HT20_BIN_MAG_DATA lower_bins;
    MAX_MAG_INDEX_DATA  lower_bins_max;
    u_int8_t       max_exp;
} __ATTRIB_PACK HT20_FFT_PACKET;

typedef struct ht40_fft_packet {
    HT40_BIN_MAG_DATA lower_bins;
    HT40_BIN_MAG_DATA upper_bins;
    MAX_MAG_INDEX_DATA  lower_bins_max;
    MAX_MAG_INDEX_DATA  upper_bins_max;
    u_int8_t       max_exp;
} __ATTRIB_PACK HT40_FFT_PACKET;

#ifdef  __ATTRIB_PACK
#undef  __ATTRIB_PACK
#endif

struct spectral_pulseparams {
    u_int64_t       p_time;                 /* time for start of pulse in usecs*/
    u_int8_t        p_dur;                  /* Duration of pulse in usecs*/
    u_int8_t        p_rssi;                 /* Duration of pulse in usecs*/
};


#define HAL_SPECTRAL_DEFAULT_SS_COUNT     128
#define HAL_SPECTRAL_DEFAULT_SS_SHORT_REPORT 1
#define HAL_SPECTRAL_DEFAULT_SS_PERIOD     35
#define HAL_SPECTRAL_DEFAULT_SS_FFT_PERIOD      1

/* This should be kept in sync with the definition in HAL. This is not expected
   to vary frequently. */
#ifndef AH_MAX_CHAINS
#define AH_MAX_CHAINS 3
#endif

/* Spectral configuration parameters */
typedef struct spectral_params {
    u_int16_t   ss_fft_period;               /* Skip interval for FFT reports */
    u_int16_t   ss_period;                   /* Spectral scan period */
    u_int16_t   ss_count;                    /* # of reports to return from
                                                ss_active */
    u_int16_t   ss_short_report;             /* Set to report ony 1 set of FFT
                                                results */
    u_int8_t    radar_bin_thresh_sel;
    u_int16_t   ss_spectral_pri;             /* Priority, and are we doing a
                                                noise power cal ? */
    u_int16_t   ss_fft_size;                 /* Defines the number of FFT data
                                                points to compute, defined
                                                as a log index:
                                                num_fft_pts = 2^ss_fft_size */
    u_int16_t   ss_gc_ena;                   /* Set, to enable targeted gain
                                                change before starting the
                                                spectral scan FFT */
    u_int16_t   ss_restart_ena;              /* Set, to enable abort of receive
                                                frames when in high priority
                                                and a spectral scan is queued */
    u_int16_t   ss_noise_floor_ref;          /* Noise floor reference number
                                                signed) for the calculation
                                                of bin power (dBm).
                                                Though stored as an unsigned
                                                value, this should be treated
                                                as a signed 8-bit int. */
    u_int16_t   ss_init_delay;               /* Disallow spectral scan triggers
                                                after tx/rx packets by setting
                                                this delay value to roughly
                                                SIFS time period or greater.
                                                Delay timer counts in units of
                                                0.25us */
    u_int16_t   ss_nb_tone_thr;              /* Number of strong bins
                                                (inclusive) per sub-channel,
                                                below which a signal is declared
                                                a narrowband tone */
    u_int16_t   ss_str_bin_thr;              /* Bin/max_bin ratio threshold over
                                                which a bin is declared strong
                                                (for spectral scan bandwidth
                                                analysis). */
    u_int16_t   ss_wb_rpt_mode;              /* Set this bit to report spectral
                                                scans as EXT_BLOCKER
                                                (phy_error=36), if none of the
                                                sub-channels are deemed
                                                narrowband. */
    u_int16_t   ss_rssi_rpt_mode;            /* Set this bit to report spectral
                                                scans as EXT_BLOCKER
                                                (phy_error=36), if the ADC RSSI
                                                is below the threshold
                                                ss_rssi_thr */
    u_int16_t   ss_rssi_thr;                 /* ADC RSSI must be greater than or
                                                equal to this threshold
                                                (signed Db) to ensure spectral
                                                scan reporting with normal phy
                                                error codes (please see
                                                ss_rssi_rpt_mode above).
                                                Though stored as an unsigned
                                                value, this should be treated
                                                as a signed 8-bit int. */
    u_int16_t   ss_pwr_format;               /* Format of frequency bin
                                                magnitude for spectral scan
                                                triggered FFTs:
0: linear magnitude
1: log magnitude
(20*log10(lin_mag),
1/2 dB step size) */
    u_int16_t   ss_rpt_mode;                 /* Format of per-FFT reports to
                                                software for spectral scan
                                                triggered FFTs.
0: No FFT report
(only pulse end summary)
1: 2-dword summary of metrics
for each completed FFT
2: 2-dword summary +
1x-oversampled bins(in-band)
per FFT
3: 2-dword summary +
2x-oversampled bins (all)
per FFT */
    u_int16_t   ss_bin_scale;                /* Number of LSBs to shift out to
                                                scale the FFT bins for spectral
                                                scan triggered FFTs. */
    u_int16_t   ss_dBm_adj;                  /* Set (with ss_pwr_format=1), to
                                                report bin magnitudes converted
                                                to dBm power using the
                                                noisefloor calibration
                                                results. */
    u_int16_t   ss_chn_mask;                 /* Per chain enable mask to select
                                                input ADC for search FFT. */
    int8_t      ss_nf_cal[AH_MAX_CHAINS*2];  /* nf calibrated values for
                                                ctl+ext */
    int8_t      ss_nf_pwr[AH_MAX_CHAINS*2];  /* nf pwr values for ctl+ext */
    int32_t     ss_nf_temp_data;             /* temperature data taken during
                                                nf scan */

}SPECTRAL_PARAMS_T;


#if 0
struct spectral_event {
    u_int32_t   se_ts;      /* Original 15 bit recv timestamp */
    u_int64_t   se_full_ts; /* 64-bit full timestamp from interrupt time */
    u_int8_t    se_rssi;    /* rssi of spectral event */
    u_int8_t    se_bwinfo;  /* rssi of spectral event */
    u_int8_t    se_dur;     /* duration of spectral pulse */
    u_int8_t    se_chanindex;   /* Channel of event */
    STAILQ_ENTRY(spectral_event)    se_list;    /* List of spectral events */
};
#endif

#define int8_t   char
#define uint8_t  unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int

struct samp_msg_params {
    int8_t rssi;
    int8_t lower_rssi;
    int8_t upper_rssi;
    uint16_t bwinfo;
    uint16_t datalen;
    uint32_t tstamp;
    uint16_t max_mag;
    uint16_t max_index;
    uint8_t max_exp;
    int peak;
    int pwr_count;
    u_int8_t **bin_pwr_data;
    u_int16_t freq;
    struct INTERF_SRC_RSP interf_list;
    int16_t noise_floor;
};

/*
 * Spectral classifier related code and macros
 * Need to purged later or maintain this as a
 * seprate feature
 */



/* Function exports */
extern void spectral_add_interf_samp_msg(struct samp_msg_params *params);
extern void spectral_create_samp_msg(struct samp_msg_params *params);
extern void fake_ht40_data_packet(HT40_FFT_PACKET *ht40pkt);
extern void send_fake_ht40_data();
extern u_int8_t return_max_value(u_int8_t* datap, u_int8_t numdata, u_int8_t *max_index);
extern void process_mag_data(MAX_MAG_INDEX_DATA *imag, u_int16_t *mmag, u_int8_t *bmap_wt, u_int8_t *max_index);

#endif  /* _SPECTRAL_H_ */
