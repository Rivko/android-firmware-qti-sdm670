#ifndef __SDCC_SD_UTIL_H
#define __SDCC_SD_UTIL_H
/**********************************************************************
 * File: sdcc_sd_util.h
 *
 * Services: 
 *    This file provides SDCC util definitions for SD device.
 *
 * Description:
 *    This file contains the SD specific utility function prototypes.
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     -----------------------------------------------
2014-06-18   bn      Initial version. Branched from 8994 SBL

==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "sdcc_priv.h"
#include "SdccCommon.h"

/*SDCC_EXTRACT_COMMON_BEGIN*/
/*************************************************************/
/*               DEFINITIONS AND CONSTANTS                   */
/*************************************************************/
#define SDCC_SD_FUNC_GRP         6 /* number of function groups */
#define SDCC_SD_WIDE_BUS_WIDTH   5 /* 0101b: bit 0 (DAT0), bit 2 (DAT0-3) */

/* Related to CMD8_SEND_IF_COND */
#define SDCC_SDHC_SEND_IF_COND_WAIT_TIME_MS   1
#define SDCC_HC_VOLT_SUPPLIED       0x000001AA /* Voltage Host Supplied */
#define SDCC_HC_HCS                 0x40000000 /* Host Capacity Support */
#define SDCC_HC_CCS                 0x40000000 /* Card Capacity Status */

#define SDCC_BUSY_STATUS_DEF 0x01   /* 00h - bits 511:376 are defined */
                                    /* 01h - bits 511:272 are defined */
#define SDCC_ACCESS_MODE_HS  0x8003 /* access mode func supports high speed */
#define SDCC_BUSY_HS         0x0002 /* busy status for high speed */
#define SDCC_HS_ARG          0x1    /* mode 0: high speed argument */

/* Related to CMD6_SWITCH_FUNC */
#define SDCC_SD_SWITCH_CLK_WAIT_TIME_MS   1
#define SDCC_QUERY_SUPPORTED_FUNCS 0x00FFFF00 /* Query supported funcs */
#define SDCC_QUERY_HS        0x00FFFF01 /* Query switchable for high speed */
#define SDCC_SWITCH_HS       0x80FFFF01 /* Switch to High-Speed */

/* Related to ACMD13 SD Status */
#define SDCC_SPEED_CLASS0    0x00   /* SD Speed Class 0 */
#define SDCC_SPEED_CLASS2    0x01   /* SD Speed Class 2 */
#define SDCC_SPEED_CLASS4    0x02   /* SD Speed Class 4 */
#define SDCC_SPEED_CLASS6    0x03   /* SD Speed Class 6 */

/* start year of manufacturing date in CID register */
/* Refer to section 5.2 in SD spec. v2.0 */
#define SDCC_SD_CID_MDT_START_YEAR         2000

/* Physical partition on the device has its own unique ID.  The maximum */
/* physical partition ID is 7. */
/* Refer to section 2.4.7.1 in eSD Addendum v2.10 */
#define SDCC_SD_MAX_PHYSICAL_PARTITION_ID  7

/* [31:24] is the physical partition ID for CMD43 SELECT_PARTITION */
/* Refer to section 2.4.2.2 in eSD Addendum v2.10 */
#define SDCC_SD_SELECT_PARTITION_ID_SHFT   24

/* The following definitions are used for check the out of range status. */
/* Refer to section 4.10.1 in SD spec. v2.0 */
#define SDCC_SD_OUT_OF_RANGE_SHFT          31
#define SDCC_SD_OUT_OF_RANGE_BITMASK       0x1UL
#define SDCC_SD_OUT_OF_RANGE_ERROR         0x1UL
/*SDCC_EXTRACT_COMMON_END*/

/*************************************************************/
/*                           MACROS                          */
/*************************************************************/
/* The following macro translates the value of OUT_OF_RANGE error bit */
/* specified in section 4.10.1 in SD spec. v2.0. */
#define SDCC_XLATE_OUT_OF_RANGE_ERROR(x) \
   (((x) >> SDCC_SD_OUT_OF_RANGE_SHFT) & SDCC_SD_OUT_OF_RANGE_BITMASK)


/******************************************************************/
/*                     TYPES DECLARATIONS                         */
/******************************************************************/
/* The SD CARD Configuration register (SCR) is according SD spec. v2.0. */
typedef struct sdcc_scr
{
   UINT8  scr_structure;         /* SCR structure */
   UINT8  sd_spec;               /* SD memory card - spec. version */
   UINT8  data_stat_after_erase; /* data status after erases */
   UINT8  sd_security;           /* SD security support */
   UINT8  sd_bus_widths;         /* DAT bus widths supports */
} sdcc_scr_type;

/* Switch Function status data structure */
typedef struct sdcc_switch_data
{
   UINT16 max_curr_consumption;   /* maximum current consumption */
   UINT16 funcgrp_support_func[SDCC_SD_FUNC_GRP]; /* func grp i support funcs*/
   UINT8  funcgrp_status[SDCC_SD_FUNC_GRP]; 
                            /* mode 0 - func can be switched in func grp i */
                            /* mode 1 - result of switch cmd in func grp i */
   UINT8  data_struct_vers;       /* data structure version */
   UINT16 funcgrp_busy_status[SDCC_SD_FUNC_GRP]; /* busy status of func grp i*/
} sdcc_switch_data_type;

/* SD Status that contains status bit relating to the SD Memory Card */
/* proprietary features and may be used for future application-specific */
/* usage. */
typedef struct sdcc_sd_status
{
   UINT8  dat_bus_width;       /* shows the currently defined data bus width */
                               /* 00b: 1 bit width, 10b: 4 bit width */
   UINT8  secured_mode;        /* card is in Secured ode of operation */
                               /* 0: Not in the mode, 1: In Secured Mode */
   UINT16 sd_card_type;        /* In the future, the 8 LSBs will be used to */
                               /* define different variations of the SD */
                               /* Memory Card (Each bit will define */
                               /* different SD Types).  The 8 MSBs will be */
                               /* used to define SD Cards that do not comply */
                               /* with current SD Physical Layer Spec. */
   UINT32 size_of_protected_area; /* refer to SD Physical Layer Spec. */
   UINT8  speed_class;         /* speed class of the SD card */
   UINT8  performance_move;    /* refer to SD Physical Layer Spec. */
   UINT8  au_size;             /* size of allocation unit */
   UINT16 erase_size;          /* number of AUs to be erased at a time */
   UINT8  erase_timeout;       /* timeout value for erasing areas specified */
                               /* by UNIT_OF_ERASE_AU */
   UINT8  erase_offset;        /* fixed offset value added to erase time */
} sdcc_sd_status_type;


/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/
/******************************************************************************
* Name: sdcc_find_sd_device
*
* Description: 
*    This function looks for the SD device of the specified driveno.
*
* Parameters:
*    handle [IN] : a pointer to the SDCC device 
* 
* Returns:
*    card type that indicates if device is SD or unknown
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_find_sd_device ( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_get_scr
*
* Description:
*    This function reads the SD card configuration register.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*    scr             [OUT] : SD configuration register
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_get_scr( sdcc_dev_type   *sdcc_pdev,
                          sdcc_scr_type   *scr );

/******************************************************************************
* Name: sdcc_decode_scr
*
* Description:
*    This function is to parse out the SD Card Configuration register data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains scr data
*    scr          [OUT] : pointer to the structure to save the scr data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_scr( const UINT8      *data,
                         sdcc_scr_type   *scr );

/******************************************************************************
* Name: sdcc_get_sd_status
*
* Description:
*    This function gets the SD Status that contains the status bits by sending
*    ACMD13 to the SD card.
*
* Arguments:
*    sdcc_pdev      [IN] : pointer to device type
*    sd_status     [OUT] : SD Status contains the status bits
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_get_sd_status( sdcc_dev_type         *sdcc_pdev,
                                sdcc_sd_status_type   *sd_status );

/******************************************************************************
* Name: sdcc_decode_sd_status
*
* Description:
*    This function is to parse out the SD Status.
*
* Arguments:
*    data           [IN] : pointer to buffer that contains SD Status data
*    sd_status     [OUT] : pointer to the structure to save the SD Status to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_sd_status( const UINT8            *data,
                               sdcc_sd_status_type   *sd_status );

/******************************************************************************
* Name: sdcc_sd_switch_to_hs
*
* Description:
*    This function sends CMD6 to query (mode 0) which functions the SD card
*    supports.  It also sends CMD6 to set (mode 1) the card to high speed
*    if the card supports it.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_sd_switch_to_hs( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_decode_switch_data
*
* Description:
*    This function is to parse out the CMD6 status data structure.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains CMD6 status data
*    switch_status [OUT]: pointer to the structure to save the CMD6 status
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_switch_data( const UINT8              *data,
                                 sdcc_switch_data_type   *switch_status );

/******************************************************************************
* Name: sdcc_get_sd_speed_class
*
* Description:
*    This function gets the SD Status which contains the speed class
*    information.  It then updates the speed class field in the device
*    structure.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_get_sd_speed_class( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_set_sd_bus_width
*
* Description:
*    This function sets the SD data bus width to 4-bit mode by 
*    sending ACMD6 to the SD card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device type
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_set_sd_bus_width( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_decode_sd_cid
*
* Description:
*    This function is to parse out the Card Identification (CID) register.
*
* Arguments:
*    raw_cid        [IN] : pointer to raw CID info
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_sd_cid( const UINT32   *raw_cid,
                            sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_decode_sd_mfr_date
*
* Description:
*    This function decodes the manufacturing date as mm/yyyy format.
*
* Arguments:
*    mfr_date       [IN] : raw manufacturing date
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_sd_mfr_date( UINT16         mfr_date,
                                 sdcc_dev_type *sdcc_pdev );

#endif /* __SDCC_SD_UTIL_H */

