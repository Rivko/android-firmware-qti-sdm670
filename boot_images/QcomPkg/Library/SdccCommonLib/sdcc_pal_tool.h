#ifndef __SDCC_PAL_TOOL_H
#define __SDCC_PAL_TOOL_H
/**********************************************************************
 * sdcc_pal_tool.h
 *
 * This file provides SDCC PAL definitions for TARGET TOOL platform
 *
 * Copyright (c) 2014,2016 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     -----------------------------------------------
2017-05-12   sb      Add partial enumeration for eMMC 
2016-01-18   eo      Reduced sdcc code size
2014-06-18   bn      Initial version. Branched from 8994 SBL
==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "sdcc_priv.h"

/*************************************************************/
/*              DEFINITIONS AND CONSTANTS                    */
/*************************************************************/
/* SDCC related stuff */
#define sdcc_handle_silent_reinit(x)   TRUE

/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/

/******************************************************************************
 * Name: sdcc_set_partition
 *
 * Description:
 *    This function sets the partition region on the memory device by sending
 *    a command to the device specified by the handle parameter.
 *
 * Arguments:
 *    handle       [IN] : a pointer to the SDCC device that was returned from
 *                        sdcc_handle_open()
 *
 * Returns:
 *    SDCC_NO_ERROR if successful, error code otherwise
 *
 ******************************************************************************/
SDCC_STATUS sdcc_set_partition( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_config_rca
*
* Description:
*    This function is to set/get the RCA from the card. 
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to the device structure
*
* Returns:
*    rca value
*
******************************************************************************/
UINT16 sdcc_config_rca( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_init_memory_device
*
* Description:
*    This function sends CMD2 to get the device ID and product related
*    information.  It then configures the device Relative Card Address and
*    completes the card identification process.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*   Card is in Identification mode. 
*
*****************************************************************************/
SDCC_STATUS sdcc_init_memory_device( sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_get_memory_info
*
* Description:
*    This function is to get the specific information related to the
*    memory device via CMD9 and/or CMD10 before it goes into data
*    transfer mode. 
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in stand-by mode.
*
******************************************************************************/
SDCC_STATUS sdcc_get_memory_info( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_config_memory_device
*
* Description:
*    This function is to programm the card into data transfer mode.  It
*    selects the card, sets up the bus width and block data transfer mode. 
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS sdcc_config_memory_device( sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_vreg_switch_on
*
* Description:
*    This function switches on the voltage regulator for SDCC.
*
* Arguments:
*    driveno     [IN] : drive number
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_vreg_switch_on( INT16 driveno );

/******************************************************************************
* Name: sdcc_handle_find_card
*
* Description: 
*    This function finds out the card type specified in handle parameter. 
*
* Arguments:
*    handle [IN] : a pointer to the SDCC device that was returned from
*                  sdcc_handle_open()
*
* Returns:
*    card type that indicates if device is SD, MMC, SDIO, or unknown
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_handle_find_card( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_wait_card_ready
*
* Description:
*    This function is to check the card busy status by calling "send
*    status" command.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_wait_card_ready( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
 * Name: sdcc_do_transfer
 *
 * Description:
 *    This is a wrapper function that decides whether to use the Qualcomm
 *    or Host Controller standard interface for communicating with the 
 *    memory device.
 *
 * Arguments:
 *    p_device             [IN] : SDCC context data for this SD-card
 *    p_command            [IN] : command to send to the SD-card to perform 
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_do_transfer(sdcc_dev_type *p_device, sdcc_cmd_type *p_command,
                             UINT8 *buffer, UINT16 transfer_size);

/******************************************************************************
* Name: sdcc_handle_optimized_open
*
* Description:
*    This function does optimized initialization of the MMC device.
*    It returns NULL if there is no memory device attached.
*    It then sets up the device for data transfer.
*    A non-null pointer to the SDCC device is returned.
*
* Arguments:
*    phy_partition_num [IN] : physical partition number.
*
* Returns:
*    A pointer to the SDCC device if successful.  NULL if failed.
*
******************************************************************************/
struct sdcc_device *
sdcc_handle_optimized_open ( UINT32 phy_partition_num );

/******************************************************************************
* Name: sdcc_is_partial_enumeration
*
* Description:
*    This function checks if partial Initialization is to be done.
*
* Arguments:
*    driveno       [IN] : drive number
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_is_partial_enumeration ( INT16 driveno );

/******************************************************************************
 * Name: sdcc_send_app_command
 *
 * Description:
 *    This function is to send an App-command to the SD-card
 *
 * Arguments:
 *    p_app_command          [IN] : pointer to a structure describing the
 *                                  App-Command to send.  The command should
 *                                  be one of the SD_ACMDXXXX enums.
 *    p_device               [IN] : pointer to the device structure
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_send_app_command (sdcc_dev_type *, sdcc_cmd_type *);

/******************************************************************************
* Name: sdcc_set_block_count
*
* Description:
*    This function sends CMD23 to set the number of blocks and reliable write parameter
*    for a block read or write command
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device structure
*    cmd_arg  [IN]   : Bit[31] - Reliable write request
*                      Bit[0:15] - Number of blocks
* Returns:
*    Returns error code
*
*****************************************************************************/
SDCC_STATUS
sdcc_set_block_count(sdcc_dev_type *sdcc_pdev, UINT32 cmd_arg);

/******************************************************************************
 * Name: sdcc_rpmb_do_transfer
 *
 * Description:
 *    This function is a wrapper function. It decides whether to use Qualcomm
 *    or standard Host Controller interface to read/write to the RPMB partition.
 *
 * Arguments:
 *    p_device             [IN] : SDCC context data for this SD-card 
 *    p_command            [IN] : command to send to the SD-card to perform
 *                                the read or write
 *    buffer           [IN/OUT] : buffer of data to read or write
 *    transfer_size        [IN] : The number of blocks to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS
sdcc_rpmb_do_transfer (sdcc_dev_type *p_device, sdcc_cmd_type *p_command,
                       UINT8 *buffer, UINT16 transfer_size);

/******************************************************************************
* Name: wait_card_ready_check_wp_violation
*
* Description:
*    This function sends CMD13 SEND_STATUS to the memory device to get the card
*    status.  It then checks if WP_VIOLATION error bit is set. It continues to
*    send CMD13 until the card is ready (card returns to Transfer state in the
*    CMD13's response) or it will time out after SDCC_PROG_DONE_MAX tries.
*
* Arguments:
*    sdcc_pdev          [IN/OUT] : pointer to device structure
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS wait_card_ready_check_wp_violation(sdcc_dev_type *sdcc_pdev);

#endif /* __SDCC_PAL_TOOL_H */

