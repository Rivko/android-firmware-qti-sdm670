#ifndef __SDCC_RPMB_API_H__
#define __SDCC_RPMB_API_H__

/** @file sdcc_rpmb_api.h
   
  This file provides SDCC's RPMB external definitions.
  
  Copyright (c) 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential 
   
**/

/*=============================================================================
                              EDIT HISTORY


when         who    what, where, why
----------   ---    -----------------------------------------------------------
2014-06-18   bn     Initial Version. Branched from 8994 UEFI 

=============================================================================*/


/******************************************************************************
* Name: rpmb_read_counter_pkt
*
* Description:
* This function sends the Read Counter command and retrieves the Read Counter
* Response packet from the eMMC device. The Response packet can be used to
* retrieve the eMMC Write Counter.
*
* Arguments:
*    handle [IN]         : A pointer to the SDCC device that was returned
*                          from sdcc_handle_open()
*    RespPktBuffer [OUT] : A pointer to the Response packet for the  
*                          Read Counter Request
*
* Returns:
*    SDCC_NO_ERROR:             Successfully read the Counter packet
*    SDCC_ERR_INVALID_PARAM:    Bad pointer passed in
*    SDCC_ERR_CARD_UNDETECTED:  No eMMC card detected
*    SDCC_ERR_RPMB_RESPONSE:    Unexpected Response field of the Response packet
*    SDCC_ERR_RPMB_RESULT:      Error in the Result field of the Response packet
* 
******************************************************************************/
SDCC_STATUS rpmb_read_counter_pkt (struct sdcc_device *handle, 
                                   UINT32* RespPktBuffer);


/******************************************************************************
* Name: rpmb_read_sectors
*
* Description:
* This function reads the sectors from RPMB partition using the passed in cmd
* packet buffer. The data is returned in the DataBuffer.
*
* Arguments:
*    handle [IN]           : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    CmdPktBuffer [IN]     : Pointer to a formatted packet for Read Request 
*    HalfSectorCount [IN]  : Number of half sectors to read.                        
*    DataBuffer [OUT]      : Pointer to data received from eMMC
* 
* Returns:
*    SDCC_NO_ERROR:             Successfully read the sectors 
*    SDCC_ERR_INVALID_PARAM:    Bad pointer passed in
*    SDCC_ERR_CARD_UNDETECTED:  No eMMC card detected
*    SDCC_ERR_RPMB_RESPONSE:    Unexpected Response field of the Response packet
*    SDCC_ERR_RPMB_NONCE:       The Nonce field of the Response does not
*                               match the Nonce sent in the Request packet
*    SDCC_ERR_RPMB_RESULT:      Error in the Result field of the Response packet
* 
******************************************************************************/
SDCC_STATUS rpmb_read_sectors (struct sdcc_device *handle, UINT32* CmdPktBuffer,
                               UINT32 HalfSectorCount, UINT32* DataBuffer);


/******************************************************************************
* Name: rpmb_write_sectors
*
* Description:
* This function writes the sectors to the RPMB partition. The data and RPMB
* command headers are pre-formated in the CmdPktBuffer. The Response packet
* from the eMMC device is passed back to the caller which may used
* to validate the next RPMB write.
*
* Arguments: 
*    handle [IN]          :  Pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    CmdPktBuffer [IN]    :  Pointer to a formatted packet to be sent to eMMC
*    HalfSectorCount [IN] :  Number of half sectors to write
*    WriteType [IN]       :  Indicates if the RPMB write is the key provision
*                            write or data write
*    RespPktBuffer [OUT]  :  Pointer to the response packet retrieved from 
*                            the eMMC device
* 
* Returns:
*    SDCC_NO_ERROR:             Successfully write the sectors 
*    SDCC_ERR_INVALID_PARAM:    Bad pointer passed in
*    SDCC_ERR_CARD_UNDETECTED:  No eMMC card detected
*    SDCC_ERR_RPMB_RESPONSE:    Unexpected Response field of the Response packet
*    SDCC_ERR_RPMB_NONCE:       The Nonce field of the Response does not
*                               match the Nonce sent in the Request packt
*    SDCC_ERR_RPMB_RESULT:      Error in the Result field of the Response packet
******************************************************************************/
SDCC_STATUS rpmb_write_sectors (struct sdcc_device *handle, UINT32* CmdPktBuffer, 
                UINT32 HalfSectorCount, UINT32 WriteType, UINT32 *RespPktBuffer);


#endif /* __SDCC_RPMB_API_H__ */

