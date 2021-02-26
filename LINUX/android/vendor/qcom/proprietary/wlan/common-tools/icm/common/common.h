/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * Notifications and licenses are retained for attribution purposes only
 *
 * IEEE 802.11 Frame type definitions
 * Copyright (c) 2002-2015, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2007-2008 Intel Corporation
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef COMMON_H
#define COMMON_H

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif
#ifndef ETH_HLEN
#define ETH_HLEN 14
#endif

#ifdef __GNUC__
#define PRINTF_FORMAT(a,b) __attribute__ ((format (printf, (a), (b))))
#define STRUCT_PACKED __attribute__ ((packed))
#else
#define PRINTF_FORMAT(a,b)
#define STRUCT_PACKED
#endif

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

#define SSID_MAX_LEN 32

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#include <endian.h>

#ifdef CONFIG_USE_INTTYPES_H
#include <inttypes.h>
#else
#include <stdint.h>
#endif
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef u16 __bitwise be16;
typedef u16 __bitwise le16;
typedef u32 __bitwise be32;
typedef u32 __bitwise le32;
typedef u64 __bitwise be64;
typedef u64 __bitwise le64;


#ifndef os_strchr
#define os_strchr(s, c) strchr((s), (c))
#endif

#ifndef os_strcmp
#define os_strcmp(s1, s2) strcmp((s1), (s2))
#endif

extern u_int32_t
hecap_val_extract(u_int8_t *hecap_subfield, u_int8_t idx, u_int32_t
                num_bits);

/* TODO: Once a single set of converged externally includable header files
 * containing protocol definitions are available for both RTR AND MBL, remove
 * the below and replace with converged definitions.
 */

#define WLAN_EID_SSID                      0
#define WLAN_EID_HT_OPERATION              61
#define WLAN_EID_VHT_OPERATION             192
#define WLAN_EID_VENDOR                    221
#define WLAN_EID_EXTN_ELEM                 255

/* Element ID Extension numbers */
#define WLAN_EXTN_EID_SRP                  39


/* HT Operation element */
struct ieee80211_ht_operation {
    u8 primary_chan;
    /* Five octets of HT Operation Information */
    u8 ht_param; /* B0..B7 */
    le16 operation_mode; /* B8..B23 */
    le16 param; /* B24..B39 */
    u8 basic_mcs_set[16];
} STRUCT_PACKED;


struct ieee80211_vht_operation {
    u8 vht_op_info_chwidth;
    u8 vht_op_info_chan_center_freq_seg0_idx;
    u8 vht_op_info_chan_center_freq_seg1_idx;
    le16 vht_basic_mcs_set;
} STRUCT_PACKED;

/* 11AX TODO: Currently, QCA uses vendor IDs for HE Capabilities and HE
 * Operations. Transition to ANA assigned IDs once the host driver changes.
 */

/* ATH HE OUI (in little endian, i.e. OTA format) */
#define ATH_HE_OUI                          0x741300

#define ATH_HE_CAP_SUBTYPE                  0x01
#define HECAPBLTS_PHYINFO_SIZE              9
#define HECAPBLTS_MACINFO_SIZE              5
#define HECAPBLTS_TXRX_MCS_NSS_SIZE         2
#define HECAPBLTS_PPET16_PPET8_MAX_SIZE     25

#define HECAPBLTS_PHYINFO_SRP_BYTEIDX       7
#define HECAPBLTS_PHYINFO_SRP_BITIDX        62
#define HECAPBLTS_PHYINFO_SRP_NUMBITS       1

/*
 * 802.11ax HE Capability (in QCA Vendor IE format)
 */
struct ieee80211_ie_hecapabilities {
        u_int8_t                  elem_id;
        u_int8_t                  elem_len;
        u_int8_t                  oui[3];
        u_int8_t                  subtype;
        u_int8_t                  hecap_macinfo[HECAPBLTS_MACINFO_SIZE];
        u_int8_t                  hecap_phyinfo[HECAPBLTS_PHYINFO_SIZE];
        u_int8_t                  hecap_txrx[HECAPBLTS_TXRX_MCS_NSS_SIZE];
        u_int8_t                  hecap_ppet[HECAPBLTS_PPET16_PPET8_MAX_SIZE];
} STRUCT_PACKED;

/* Determine whether the given IE is an HE Capabilities IE (in QCA vendor IE
 * format). For this, check that element length is greater than 4, and that the
 * 4 byte combination of OUI[3] and subtype match the expected value. The caller
 * is expected to ensure that the memory location pointed to has a minimum total
 * length of 6.
 */
#define ICM_IS_HE_CAP_OUI(frm)      (((frm)[1] > 4) &&                         \
                                    (htole32(*((u32*)((frm) + 2))) ==          \
                                     ((ATH_HE_CAP_SUBTYPE << 24) | ATH_HE_OUI)))

/*Get whether the STA supports SRP-based SR operation*/
#define HECAPBLTS_PHY_GET_SRPALLOWED(hecap_phy)                                \
        hecap_val_extract((hecap_phy)[HECAPBLTS_PHYINFO_SRP_BYTEIDX],          \
            HECAPBLTS_PHYINFO_SRP_BITIDX, HECAPBLTS_PHYINFO_SRP_NUMBITS)


#define ATH_HE_OP_SUBTYPE           0x02
#define HEOPER_MCS_NSS              3

/*
 * 802.11ax HE Operation IE (in QCA Vendor IE format).
 */
struct ieee80211_ie_heoperation {
        u8        elem_id;
        u8        elem_len;
        u8        oui[3];
        u8        subtype;
        u32       heop_param;
        u8        heop_mcs_nss[HEOPER_MCS_NSS];
        struct {
            u8 vht_op_info_chwidth;
            u8 vht_op_info_chan_center_freq_seg0_idx;
            u8 vht_op_info_chan_center_freq_seg1_idx;
        } STRUCT_PACKED hevhtop;
} STRUCT_PACKED;

/* Determine whether the given IE is an HE Operation IE (in QCA vendor IE
 * format). For this, check that element length is greater than 4, and that the
 * 4 byte combination of OUI[3] and subtype match the expected value. The caller
 * is expected to ensure that the memory location pointed to has a minimum total
 * length of 6.
 */
#define ICM_IS_HE_OP_OUI(frm)      (((frm)[1] > 4) &&                          \
                                    (htole32(*((u32*)((frm) + 2))) ==          \
                                     ((ATH_HE_OP_SUBTYPE << 24) | ATH_HE_OUI)))


/* Spatial Reuse Parameter Set: SR Control fields */
#define SRP_SRCONTROL_SRP_DISALLOW_MASK             0x01
#define SRP_SRCONTROL_OBSSPD_DISALLOW_MASK          0x02
#define SRP_SRCONTROL_NSRG_OFFSET_PRESENT_MASK      0x04
#define SRP_SRCONTROL_SRG_INFO_PRESENT_MASK         0x08

/*
 * 802.11ax Spatial Reuse Parameter Set
 */
struct ieee80211_sr_paramset {
    uint8_t srp_id;
    uint8_t srp_len;
    uint8_t srp_id_extn;
    uint8_t sr_control;
    union {
        struct {
            uint8_t non_srg_obsspd_max_offset;
            uint8_t srg_obss_pd_min_offset;
            uint8_t srg_obss_pd_max_offset;
            uint8_t srg_bss_color_bitmap[8];
            uint8_t srg_partial_bssid_bitmap[8];
        } STRUCT_PACKED nonsrg_srg_info;
        struct {
            uint8_t non_srg_obsspd_max_offset;
        } STRUCT_PACKED nonsrg_info;
        struct {
            uint8_t srg_obss_pd_min_offset;
            uint8_t srg_obss_pd_max_offset;
            uint8_t srg_bss_color_bitmap[8];
            uint8_t srg_partial_bssid_bitmap[8];
        } STRUCT_PACKED srg_info;
    };
} STRUCT_PACKED;


/* First octet of HT Operation Information within HT Operation element */
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_OFF_MASK    ((u8) BIT(0) | BIT(1))
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE       ((u8) BIT(0))
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW       ((u8) BIT(0) | BIT(1))
#define HT_INFO_HT_PARAM_STA_CHNL_WIDTH         ((u8) BIT(2))
#define HT_INFO_HT_PARAM_RIFS_MODE          ((u8) BIT(3))
/* B4..B7 - Reserved */

/* HT Protection (B8..B9 of HT Operation Information) */
#define HT_PROT_NO_PROTECTION           0
#define HT_PROT_NONMEMBER_PROTECTION    1
#define HT_PROT_20MHZ_PROTECTION        2
#define HT_PROT_NON_HT_MIXED            3
/* Bits within ieee80211_ht_operation::operation_mode (BIT(0) maps to B8 in
 * HT Operation Information) */
#define HT_OPER_OP_MODE_HT_PROT_MASK ((u16) (BIT(0) | BIT(1))) /* B8..B9 */
#define HT_OPER_OP_MODE_NON_GF_HT_STAS_PRESENT  ((u16) BIT(2)) /* B10 */
/* BIT(3), i.e., B11 in HT Operation Information field - Reserved */
#define HT_OPER_OP_MODE_OBSS_NON_HT_STAS_PRESENT    ((u16) BIT(4)) /* B12 */
/* BIT(5)..BIT(15), i.e., B13..B23 - Reserved */

/* Last two octets of HT Operation Information (BIT(0) = B24) */
/* B24..B29 - Reserved */
#define HT_OPER_PARAM_DUAL_BEACON           ((u16) BIT(6))
#define HT_OPER_PARAM_DUAL_CTS_PROTECTION       ((u16) BIT(7))
#define HT_OPER_PARAM_STBC_BEACON           ((u16) BIT(8))
#define HT_OPER_PARAM_LSIG_TXOP_PROT_FULL_SUPP      ((u16) BIT(9))
#define HT_OPER_PARAM_PCO_ACTIVE            ((u16) BIT(10))
#define HT_OPER_PARAM_PCO_PHASE             ((u16) BIT(11))
/* B36..B39 - Reserved */

#define BSS_MEMBERSHIP_SELECTOR_VHT_PHY 126
#define BSS_MEMBERSHIP_SELECTOR_HT_PHY 127

/* VHT Defines */
#define VHT_CAP_MAX_MPDU_LENGTH_7991                ((u32) BIT(0))
#define VHT_CAP_MAX_MPDU_LENGTH_11454               ((u32) BIT(1))
#define VHT_CAP_MAX_MPDU_LENGTH_MASK                ((u32) BIT(0) | BIT(1))
#define VHT_CAP_MAX_MPDU_LENGTH_MASK_SHIFT          0
#define VHT_CAP_SUPP_CHAN_WIDTH_160MHZ              ((u32) BIT(2))
#define VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ     ((u32) BIT(3))
#define VHT_CAP_SUPP_CHAN_WIDTH_MASK                ((u32) BIT(2) | BIT(3))
#define VHT_CAP_RXLDPC                              ((u32) BIT(4))
#define VHT_CAP_SHORT_GI_80                         ((u32) BIT(5))
#define VHT_CAP_SHORT_GI_160                        ((u32) BIT(6))
#define VHT_CAP_TXSTBC                              ((u32) BIT(7))
#define VHT_CAP_RXSTBC_1                            ((u32) BIT(8))
#define VHT_CAP_RXSTBC_2                            ((u32) BIT(9))
#define VHT_CAP_RXSTBC_3                            ((u32) BIT(8) | BIT(9))
#define VHT_CAP_RXSTBC_4                            ((u32) BIT(10))
#define VHT_CAP_RXSTBC_MASK                         ((u32) BIT(8) | BIT(9) | \
        BIT(10))
#define VHT_CAP_RXSTBC_MASK_SHIFT                   8
#define VHT_CAP_SU_BEAMFORMER_CAPABLE               ((u32) BIT(11))
#define VHT_CAP_SU_BEAMFORMEE_CAPABLE               ((u32) BIT(12))
#define VHT_CAP_BEAMFORMEE_STS_MAX                  ((u32) BIT(13) | \
        BIT(14) | BIT(15))
#define VHT_CAP_BEAMFORMEE_STS_MAX_SHIFT            13
#define VHT_CAP_BEAMFORMEE_STS_OFFSET               13
#define VHT_CAP_SOUNDING_DIMENSION_MAX              ((u32) BIT(16) | \
        BIT(17) | BIT(18))
#define VHT_CAP_SOUNDING_DIMENSION_MAX_SHIFT        16
#define VHT_CAP_SOUNDING_DIMENSION_OFFSET           16
#define VHT_CAP_MU_BEAMFORMER_CAPABLE               ((u32) BIT(19))
#define VHT_CAP_MU_BEAMFORMEE_CAPABLE               ((u32) BIT(20))
#define VHT_CAP_VHT_TXOP_PS                         ((u32) BIT(21))
#define VHT_CAP_HTC_VHT                             ((u32) BIT(22))

#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_1        ((u32) BIT(23))
#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_2        ((u32) BIT(24))
#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_3        ((u32) BIT(23) | BIT(24))
#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_4        ((u32) BIT(25))
#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_5        ((u32) BIT(23) | BIT(25))
#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_6        ((u32) BIT(24) | BIT(25))
#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MAX      ((u32) BIT(23) | \
        BIT(24) | BIT(25))
#define VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MAX_SHIFT 23
#define VHT_CAP_VHT_LINK_ADAPTATION_VHT_UNSOL_MFB   ((u32) BIT(27))
#define VHT_CAP_VHT_LINK_ADAPTATION_VHT_MRQ_MFB     ((u32) BIT(26) | BIT(27))
#define VHT_CAP_RX_ANTENNA_PATTERN                  ((u32) BIT(28))
#define VHT_CAP_TX_ANTENNA_PATTERN                  ((u32) BIT(29))

#define VHT_OPMODE_CHANNEL_WIDTH_MASK           ((u8) BIT(0) | BIT(1))
#define VHT_OPMODE_CHANNEL_RxNSS_MASK           ((u8) BIT(4) | BIT(5) | \
        BIT(6))
#define VHT_OPMODE_NOTIF_RX_NSS_SHIFT           4

#define VHT_RX_NSS_MAX_STREAMS              8

/* VHT channel widths */
#define VHT_CHANWIDTH_USE_HT    0
#define VHT_CHANWIDTH_80MHZ 1
#define VHT_CHANWIDTH_160MHZ    2
#define VHT_CHANWIDTH_80P80MHZ  3

#endif /* COMMON_H */
