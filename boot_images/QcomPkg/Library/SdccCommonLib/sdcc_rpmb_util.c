/** @file sdcc_rpmb_util.c
   
  This file implements the SDCC RPMB driver that provides access 
  to the RPMB partition 
  
  Copyright (c) 2014 - 2016, Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     -----------------------------------------------------------
2016-03-11   jt      Remove RpmbHalfSectorGranularity 
2015-01-28   rm      Move some definination to RpmbCg.c 
2014-08-27   bn      Warning clean up
2014-06-18   bn      Initial version. Branched from 8994 UEFI 

=============================================================================*/  
//#include "sdcc_errno.h"
#include "sdcc_mmc_util.h"
#include "sdcc_rpmb_util.h"
#include "sdcc_pal_tool.h"
#include "sdcc_handle_util.h"
#include "DALSys.h"
#include <Library/RpmbCg.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include "SdccCommon.h"

/*lint -save -e641 Suppress 'Converting enum to int' warning */

/* Little Endian. First byte is littlest */
#define GET_WORD_FROM_BYTE_PTR(x)  ((UINT16)*(x+1) | ((UINT16)*(x) << 8))

extern UINT32 ReliableWriteCount;

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
                                   UINT32* RespPktBuffer)
{
   sdcc_dev_type  *sdcc_pdev = NULL;
   sdcc_cmd_type  sdcc_cmd;
   UINT8   pTempCmdPktBuffer[BLOCK_SIZE]; 
   UINT16  u16Temp, RequestResponse;
   
   SDCC_STATUS rc = SDCC_NO_ERROR;

   RPMB_DATA_FRAME *pRespPktDataFrame = (RPMB_DATA_FRAME *)RespPktBuffer;
   RPMB_DATA_FRAME *pCmdPktDataFrame = (RPMB_DATA_FRAME *)pTempCmdPktBuffer;

   if ((NULL == handle) || (NULL == handle->hdev) || 
       (NULL == pRespPktDataFrame))
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: Invalid Parameter. \n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   if (SDCC_CARD_MMC != sdcc_pdev->card_type &&
       SDCC_CARD_MMCHC != sdcc_pdev->card_type )
   {   
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: Unknown Card Type \n"));
      return SDCC_ERR_CARD_UNDETECTED;
   }
   
   /* switch to the RPMB partition */
   rc = sdcc_set_partition( handle );
      
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: Failed to switch to RPMB partition \n"));
      goto ErrorExit;
   }
   
/* Send the Read Counter Request packet */
/*---------------------------*/
   
   /* CMD23 Set Block Count. Read Counter Request command's is 1 block */
   rc = sdcc_set_block_count(sdcc_pdev, 0x1);

   if (SDCC_NO_ERROR != rc)
   {        
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: Set Block Count failed"));
      goto ErrorExit;
   }

   /* Packet Generation: Format a Read Counter Request packet */
   SetMem(pCmdPktDataFrame, sizeof(RPMB_DATA_FRAME), 0x00);

   RequestResponse = READ_COUNTER_REQ;
      
   /* RPMB data frame is Big Endian. Reverse the byte order if
      system is Little Endian */
   RequestResponse = 
      IS_BIG_ENDIAN() ? RequestResponse : REVERSE_WORD(RequestResponse);
   
   /* Set RequestResponse field of the RPMB Data Frame */
   CopyMem(pCmdPktDataFrame->RequestResponse, &RequestResponse, 
              sizeof(pCmdPktDataFrame->RequestResponse));

   /* CMD25 Write Block. Send the Read Counter Request to eMMC.
      This Request instructs the eMMC that it should return the Counter
      Response packet as soon as it receives any Read Block command (CMD18) */
   sdcc_cmd.cmd       = SD_CMD25_WRITE_MULTIPLE_BLOCK;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;     
   sdcc_cmd.cmd_arg   = 0;
   sdcc_cmd.flags     = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;

   rc = sdcc_rpmb_do_transfer(sdcc_pdev, &sdcc_cmd, (UINT8 *)pCmdPktDataFrame, 1);

   if (SDCC_NO_ERROR != rc)
   {        
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: sdcc_rpmb_do_transfer write failed"));
      goto ErrorExit;
   }
   
/* Retrieve the Read Counter Response packet */
/*---------------------------*/
   
   /* CMD23 Set Block Count: Read 1 block which will contain the
      Counter Response packet */
   rc = sdcc_set_block_count(sdcc_pdev, 0x1);

   if (SDCC_NO_ERROR != rc)
   {        
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: Set Block Count failed"));
      goto ErrorExit;
   }
              
   sdcc_cmd.cmd       = SD_CMD18_READ_MULTIPLE_BLOCK;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = 0;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;

   /* Retrieve the Read Counter Response packet */
   rc = sdcc_rpmb_do_transfer(sdcc_pdev, &sdcc_cmd, (UINT8 *)pRespPktDataFrame, 1);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: sdcc_rpmb_do_transfer read failed"));
      goto ErrorExit;
   }

   /* Get the 2-byte Response field. RPMB Data Frame uses Big Endian*/
   CopyMem(&u16Temp, pRespPktDataFrame->RequestResponse, 
           sizeof(pRespPktDataFrame->RequestResponse));
   
   /* Reverse the byte order if the system is Little Endian */
   u16Temp = IS_BIG_ENDIAN() ? u16Temp : REVERSE_WORD(u16Temp);
   
   /* Error check. Verify that the Response field is good */
   if (READ_COUNTER_RESP != u16Temp)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter: Bad Response field = 0x%x \n", u16Temp));
      rc = SDCC_ERR_RPMB_RESPONSE;
      goto ErrorExit;
   }

   /* Get 2-byte Result field */
   CopyMem(&u16Temp, pRespPktDataFrame->Result, 
           sizeof(pRespPktDataFrame->Result));

   /* RPMB Data Frame is Big Endian. Reverse the byte order
      if the system is Little Endian */
   u16Temp = IS_BIG_ENDIAN() ? u16Temp : REVERSE_WORD(u16Temp);

   /* Verify the Result field. Bit 7 didicates counter has reached its max */
   if (u16Temp != 0x00)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_counter_pkt: Bad Result field = 0x%x \n", u16Temp));
      rc = SDCC_ERR_RPMB_RESULT;
      goto ErrorExit;
   }        

ErrorExit:
   return rc;
}

/******************************************************************************
* Name: rpmb_read_sectors
*
* Description:
* This function reads the sectors from RPMB partition using the passed in cmd
* packet buffer. The data is returned in the DataBuffer.
*
* Arguments:
*    handle [IN]          : a pointer to the SDCC device that was returned
*                           from sdcc_handle_open()
*    CmdPktBuffer [IN]    : Pointer to a formatted packet for Read Request 
*    HalfSectorCount [IN] : Number of half sectors to read.                        
*    DataBuffer [OUT]     : Pointer to data received from eMMC
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
                               UINT32 HalfSectorCount, UINT32* DataBuffer)
{
   sdcc_dev_type  *sdcc_pdev   = NULL;
   sdcc_cmd_type  sdcc_cmd;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   UINT8  *pReadBuffer = (UINT8 *)AllocatePool (HalfSectorCount * BLOCK_SIZE);

   if ((NULL == handle) || (NULL == handle->hdev) ||
       (NULL == CmdPktBuffer) || (NULL == DataBuffer))
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_sectors: Invalid Handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   if (SDCC_CARD_MMC != sdcc_pdev->card_type &&
       SDCC_CARD_MMCHC != sdcc_pdev->card_type)
   {  
      DEBUG ((EFI_D_ERROR, "rpmb_read_sectors: Unknown Card Type \n"));
      return SDCC_ERR_CARD_UNDETECTED;
   }
   
   /* Switch to RPMB partition */
   rc = sdcc_set_partition(handle);
      
   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_sectors: Failed to switch to RPMP partition \n"));
      goto ErrorExit;
   }

/* Send the Read command */
/*---------------------------*/

   /* CMD23. Set Block Count to 1 block which is the Read command */
   rc = sdcc_set_block_count(sdcc_pdev, 0x1);

   /* CMD25: Send the Read command */
   sdcc_cmd.cmd       = SD_CMD25_WRITE_MULTIPLE_BLOCK;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   /* The Start Sector is specified in the CmdPktBuffer */     
   sdcc_cmd.cmd_arg   = 0;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;

   rc = sdcc_rpmb_do_transfer(sdcc_pdev, &sdcc_cmd, (UINT8 *)CmdPktBuffer, 1);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_sectors: sdcc_rpmb_do_transfer write failed \n"));        
      goto ErrorExit;
   }

/* Read the sectors */
/*---------------------------*/

   /* CMD23: set the block count to the number of half sectors to read */
   rc = sdcc_set_block_count(sdcc_pdev, HalfSectorCount);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_sectors: Set Block Count failed \n"));        
      goto ErrorExit;
   }

   sdcc_cmd.cmd       = SD_CMD18_READ_MULTIPLE_BLOCK;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg   = 0;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;

   rc = sdcc_rpmb_do_transfer(sdcc_pdev, &sdcc_cmd, (byte *)DataBuffer, HalfSectorCount);   

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_read_sectors: sdcc_rpmb_do_transfer read failed \n"));   
      goto ErrorExit;
   }

ErrorExit:

   if (pReadBuffer)
   {
      FreePool (pReadBuffer);
   }

   return rc;

}

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
                 UINT32 HalfSectorCount, UINT32 WriteType, UINT32 *RespPktBuffer)
{
   sdcc_dev_type  *sdcc_pdev   = NULL;
   sdcc_cmd_type  sdcc_cmd;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   UINT32  Index = 0, BlkCount = 0;
   UINT8   pTempCmdPktBuffer[BLOCK_SIZE];
   UINT16  RequestResponse;
   
   RPMB_DATA_FRAME *pRespPktDataFrame = (RPMB_DATA_FRAME *)RespPktBuffer;
   RPMB_DATA_FRAME *pCmdPktDataFrame = (RPMB_DATA_FRAME *)pTempCmdPktBuffer;

   if ((NULL == handle) || (NULL == handle->hdev) ||
       (NULL == CmdPktBuffer) || (NULL == pRespPktDataFrame))
   {
      DEBUG ((EFI_D_ERROR, "rpmb_write_sectors: Invalid Handle"));
      return SDCC_ERR_INVALID_PARAM;
   }

   sdcc_pdev = handle->hdev;

   if (SDCC_CARD_MMC != sdcc_pdev->card_type &&
       SDCC_CARD_MMCHC != sdcc_pdev->card_type)
   {  
      DEBUG ((EFI_D_ERROR, "rpmb_write_sectors: Unknown Card Type \n"));
      return SDCC_ERR_CARD_UNDETECTED;
   }

   /* Switch to RPMB partition */
   rc = sdcc_set_partition(handle);
       
   if ( SDCC_NO_ERROR != rc )
   {
      DEBUG ((EFI_D_ERROR, "rpmb_write_sectors: Failed to switch to RPMB partition \n"));
      goto ErrorExit;
   }
 
/* Write the data */
/*---------------------------*/
   /* Get reliable write count (if not already set) */
   if (ReliableWriteCount == 0)
   {
      ReliableWriteCount = sdcc_pdev->mem.mem_info.reliable_write_sector_count;

      /* For RPMB, reliable_write_sector_count = 1 supports either
         256 bytes (1 512 byte frame) or 512 bytes (2 512 byte frames). Use the
         larger of the two values for better performance */
      if (ReliableWriteCount == 1)
      {
         ReliableWriteCount = 2; 
      }
   }

   /* If the size of the buffer is multiple of RW, format the frames so that it will be
      written with the write size equals RW size. Only the last frame has the HMAC.
      If RW equals 1 or the buffer size is not mulitple of RW, format the RPMB frame
      so that it will be single frame write. Every RPMB frame has its own HMAC */

   if ((HalfSectorCount % ReliableWriteCount) == 0x00)
   {
      BlkCount = ReliableWriteCount;
   }
   else
   {
      BlkCount = 1;
   }

   Index = 0;

   while (Index < HalfSectorCount)
   {
      /* CMD23: Specify the number of sectors to write and enable
         the Reliable Write mode */
      sdcc_set_block_count(sdcc_pdev, 
         (SDCC_EMMC_RELIABLE_WRITE_REQUEST | BlkCount));
 
      /* CMD25: Write Mulitple Block */
      sdcc_cmd.cmd = SD_CMD25_WRITE_MULTIPLE_BLOCK;
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;
      sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;

      /* The Start Sector is specified in the CmdPktBuffer.
         The address in the command is ignored */ 
      sdcc_cmd.cmd_arg = 0;
       
      /* Send the data */
      rc = sdcc_rpmb_do_transfer(sdcc_pdev, &sdcc_cmd, 
                     ((UINT8 *)CmdPktBuffer + (Index*512)), BlkCount);
   
      if (SDCC_NO_ERROR != rc)
      {
         DEBUG ((EFI_D_ERROR, "rpmb_write_sectors: failed sdcc_rpmb_do_transfer write \n"));          
         goto ErrorExit;
      }

      Index = Index + BlkCount;

   }

/* Send a command to instruct the eMMC that when receiving the next
   Block Read command (CMD18), return the Result packet of the recent write */
/*---------------------------*/
 
   /* CMD23: Set Block Count to 1 which is the Read Result Request's size */
   rc = sdcc_set_block_count(sdcc_pdev, 0x1);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_write_sectors: Set Block Count failed \n"));          
      goto ErrorExit;
   }
   
   /* Packet Generation: Format a Read Counter Request packet */
   SetMem(pCmdPktDataFrame, sizeof(RPMB_DATA_FRAME), 0x00);

   RequestResponse = READ_RESULT_REQ;
      
   /* RPMB data frame is Big Endian. Reverse the byte order if
      system is Little Endian */
   RequestResponse = 
      IS_BIG_ENDIAN() ? RequestResponse : REVERSE_WORD(RequestResponse);
   
   /* Set RequestResponse field of the RPMB Data Frame */
   CopyMem(pCmdPktDataFrame->RequestResponse, &RequestResponse, 
              sizeof(pCmdPktDataFrame->RequestResponse));
 
   sdcc_cmd.cmd       = SD_CMD25_WRITE_MULTIPLE_BLOCK;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;    
   sdcc_cmd.cmd_arg   = 0;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER;
    
   rc = sdcc_rpmb_do_transfer(sdcc_pdev, &sdcc_cmd, (UINT8 *)pCmdPktDataFrame, 1);
 
   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_write_sectors: failed sdcc_rpmb_do_transfer write \n"));       
      goto ErrorExit;
   }
 
/* Read the Result packet */
/*---------------------------*/
 
   /* CMD23: Set Block Count to read 1 block which is the Result packet */
   sdcc_set_block_count(sdcc_pdev, 1);
    
   /* CMD18 */
   sdcc_cmd.cmd = SD_CMD18_READ_MULTIPLE_BLOCK;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_cmd.cmd_arg = 0;
   sdcc_cmd.flags = SDCC_TRANSFER_FLAG_IS_READ_TRANSFER;
    
   rc = sdcc_rpmb_do_transfer(sdcc_pdev, &sdcc_cmd, (UINT8 *)pRespPktDataFrame, 1); 

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "rpmb_write_sectors: failed sdcc_rpmb_do_transfer read \n"));    
      goto ErrorExit;
   }

ErrorExit:

   return rc;
}

/*lint -restore */
