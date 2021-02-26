/*
 * Copyright (c) 2014, 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#if !defined(_ART_UTF_COMMON_H)
#define _ART_UTF_COMMON_H

#define __ATTRIB_PACK           __attribute__ ((packed))
#define __ATTRIB_PRINTF         __attribute__ ((format (printf, 1, 2)))
#define __ATTRIB_NORETURN       __attribute__ ((noreturn))

#ifndef INLINE
#define INLINE                  __inline__
#endif

#define MPATTERN                (10*4)
#define RATE_POWER_MAX_INDEX    8
#define RATE_MASK_ROW_MAX       9

#define PSAT_WHAL_NUM_11G_CAL_PIERS_MAX 3
#define PSAT_WHAL_NUM_11A_CAL_PIERS_MAX 8

#define AR6320_EEPROM_BOARDFLAGSEXT_OFFSET            28
#define AR6320_EEPROM_BOARDFLAGSEXT_LENGTH            4
#define WHAL_BOARD_SCPC_IS_DONE                       0x00000004
#define AR6320_EEPROM_PSATPWR2G_OFFSET                0x9A4
#define AR6320_EEPROM_PSATPWR2G_LENGTH                0x1
#define AR6320_EEPROM_2G_SCPC_CAL_DATA_OFFSET         0x280
#define AR6320_EEPROM_2G_SCPC_CAL_DATA_LENGTH         0x17A
#define AR6320_EEPROM_5G_SCPC_CAL_DATA_OFFSET         0xAE8
#define AR6320_EEPROM_5G_SCPC_CAL_DATA_LENGTH         0x360
#define AR6320_EEPROM_CHECKSUM_OFFSET                 0x2
#define AR6320_EEPROM_CHECKSUM_LENGTH                 0x2
#define AR6320_EEPROM_STRUCT_LENGTH                   8124

typedef enum {
    ZEROES_PATTERN = 0,
    ONES_PATTERN,
    REPEATING_10,
    PN7_PATTERN,
    PN9_PATTERN,
    PN15_PATTERN,
    USER_DEFINED_PATTERN
}TX_DATA_PATTERN;

typedef enum {
    TCMD_CONT_TX_OFF = 0,
    TCMD_CONT_TX_SINE,
    TCMD_CONT_TX_FRAME,
    TCMD_CONT_TX_TX99,
    TCMD_CONT_TX_TX100,
    TCMD_CONT_TX_OFFSETTONE,
    TCMD_CONT_TX_PSAT_CAL,
    TCMD_CONT_TX_CWTONE,
    TCMD_CONT_TX_CLPCPKT,
} TCMD_CONT_TX_MODE;

typedef enum {
    TCMD_WLAN_MODE_NOHT = 0,
    TCMD_WLAN_MODE_HT20 = 1,
    TCMD_WLAN_MODE_HT40PLUS = 2,
    TCMD_WLAN_MODE_HT40MINUS = 3,
    TCMD_WLAN_MODE_CCK = 4,
//#ifdef SUPPORT_VHT
    TCMD_WLAN_MODE_VHT20 = 5,
    TCMD_WLAN_MODE_VHT40PLUS = 6,
    TCMD_WLAN_MODE_VHT40MINUS = 7,
    TCMD_WLAN_MODE_VHT80_0 = 8,
    TCMD_WLAN_MODE_VHT80_1 = 9,
    TCMD_WLAN_MODE_VHT80_2 = 10,
    TCMD_WLAN_MODE_VHT80_3 = 11,
//#endif

    TCMD_WLAN_MODE_MAX,
    TCMD_WLAN_MODE_INVALID = TCMD_WLAN_MODE_MAX,

} TCMD_WLAN_MODE;

typedef enum {
    TCMD_CONT_RX_PROMIS =0,
    TCMD_CONT_RX_FILTER,
    TCMD_CONT_RX_REPORT,
    TCMD_CONT_RX_SETMAC,
    TCMD_CONT_RX_SET_ANT_SWITCH_TABLE,

    TC_CMD_RESP,
	TCMD_CONT_RX_GETMAC,
} TCMD_CONT_RX_ACT;

typedef enum {
    PHY_RF_MODE_NON_DBS_PHY0 = 0,
    PHY_RF_MODE_DBS_PHY0_PHY1,
    PHY_RF_MODE_SBS_PHY0_PHY1,
    PHY_RF_MODE_NON_DBS_PHY1,
} PHY_RF_MODE;
#endif //#if !defined(_ART_UTF_COMMON_H)
