#ifndef __SDCC_UTIL_H
#define __SDCC_UTIL_H
/**********************************************************************
 * sdcc_util.h
 *
 * This file provides SDCC util definitions.
 *
 * Copyright (c) 2014, 2016
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     ----------------------------------------------- 
2016-01-18   eo      Reduced sdcc code size
2014-06-18   bn      Initial release. Branched from 8994 SBL

==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "sdcc_priv.h"
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include "SdccCommon.h"


/*************************************************************/
/*              DEFINITIONS AND CONSTANTS                    */
/*************************************************************/

/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/

/******************************************************************************
 * Name: sdcc_deinit
 *
 * Description:
 *    This function de-selects the device, turns off the power supply to the
 *    device and switches off the controller clock.
 *
 * Arguments:
 *    driveno     [IN] : drive number
 *
 * Returns:
 *    SDCC_NO_ERROR if successful.  Error code otherwise
 *
 ******************************************************************************/
SDCC_STATUS sdcc_deinit( sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_get_slot_handle
*
* Description:
*    This function return the handle to the sdcc data structure
*
* Arguments:
*    driveno          [IN] : sdcc drive number
*
* Returns:
*    None
*
******************************************************************************/
struct sdcc_slot_data *sdcc_get_slot_handle (UINT32 driveno);

/******************************************************************************
* Name: sdcc_command
*
* Description:
*    This is a wrapper function for sending the command and poll 
*    the status. It decides whether to use the Qualcomm or standard 
*    Host Controller interface to communicate with the memory device
*
* Arguments:
*    sdcc_pdev         [IN] : pointer to device structure
*    sdcc_cmd          [IN] : pointer to command data structure
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_command(sdcc_dev_type *sdcc_pdev, sdcc_cmd_type *sdcc_cmd);

/******************************************************************************
* Name: sdcc_send_status
*
* Description:
*    This function is to let the currently selected device send
*    the device status.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*
* Returns:
*    Returns the current device status
*
* Note:
*    - Still used by sdcc_api.c
*
******************************************************************************/
SDCC_CARD_STATUS sdcc_send_status( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_wait_prog_done
*
* Description:
*    This function is to poll the status to make sure the device is not
*    busy.  CMD12(stop) and CMD38(erase) need to call this to make sure
*    the device programming has finished.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    timeout_in_usec    [IN] : time wait for busy signal in microseconds unit
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_wait_prog_done( sdcc_dev_type *sdcc_pdev, UINT32 timeout_in_usec );


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
* Name: sdcc_select_card
*
* Description:
*    This function is to select/deselect the specified card.
*
* Arguments:
*    sdcc_pdev          [IN] : pointer to device structure
*    select             [IN] : select if TRUE, deselect if FALSE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS 
sdcc_select_card ( sdcc_dev_type  *sdcc_pdev, BOOLEAN select);
/******************************************************************************
* Name: sdcc_reset
*
* Description:
* This function is to reset the SDCC controller
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_reset( UINT32 driveno );

/******************************************************************************
* Name: sdcc_config_clk
*
* Description:
*    This function is to configure the SDCC controller clock depending
*    on current mode.
*
* Arguments:
*    sdcc_pdev        [IN] : pointer to device type 
*    mode             [IN] : card mode
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_config_clk (sdcc_dev_type *sdcc_pdev, SDCC_CARD_MODE mode);

#endif /* __SDCC_UTIL_H */
