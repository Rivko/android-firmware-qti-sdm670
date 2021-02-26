#ifndef __RPMBCG_H__
#define __RPMBCG_H__

/** @file RpmbCg.h
   
  This file provides SDCC's RPMB Content Generation (CG) 
  external definitions. 
  
  Copyright (c) 2011, 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
   
**/

/*=============================================================================
                              EDIT HISTORY


when         who     what, where, why
----------   ---     ---------------------------------------------------------
2015*04-20   jt/rm   Add support for UFS
2015-01-26   rm      Added endian-related macros
2011-11-12   bn      Added RPMB_MAX_BLOCK, Result error code. Removed ReasonCode
2011-10-26   bn      Initial Version 

=============================================================================*/
#include <Uefi.h>

#define  BLOCK_SIZE                512

#define  PROG_KEY_REQ              0x0001
#define  READ_COUNTER_REQ          0x0002
#define  WRITE_DATA_REQ            0x0003
#define  READ_DATA_REQ             0x0004
#define  READ_RESULT_REQ           0x0005

#define  PROG_KEY_RESP             0x0100
#define  READ_COUNTER_RESP         0x0200
#define  WRITE_DATA_RESP           0x0300
#define  READ_DATA_RESP            0x0400

/* Big Endian. Index to the fields of the RPMB Data Frame */
#define  REQ_INDEX                 (BLOCK_SIZE - 2)
#define  RESP_INDEX                (BLOCK_SIZE - 2)
#define  RESULT_INDEX              (BLOCK_SIZE - 4)
#define  BLK_CNT_INDEX             (BLOCK_SIZE - 6)
#define  ADDR_INDEX                (BLOCK_SIZE - 8)
#define  WRITE_COUNTER_INDEX       (BLOCK_SIZE - 12)
#define  NONCE_INDEX               (BLOCK_SIZE - 28)
#define  DATA_INDEX                (BLOCK_SIZE - 284)
#define  KEY_INDEX                 (BLOCK_SIZE - 316)   /*Same index for MAC or Key */
#define  MAC_INDEX                 (BLOCK_SIZE - 316)   /*Same index for MAC or Key */

#define  REQ_FIELD_LEN             0x02   /* 2 Bytes - Bit[0:1] */
#define  RESP_FIELD_LEN            0x02   /* 2 Bytes - Bit[0:1] */
#define  RESULT_FIELD_LEN          0x02   /* 2 Bytes - [2:3] */
#define  BLK_CNT_FIELD_LEN         0x02   /* 2 Bytes - [4:5] */
#define  ADDR_FIELD_LEN            0x02   /* 2 Bytes - [6:7] */
#define  WRITE_COUNTER_FIELD_LEN   0x04   /* 4 Bytes - [8:11] */
#define  NONCE_FIELD_LEN           0x10   /* 16 Bytes [12:27] */
#define  DATA_FIELD_LEN            0x100  /* 256 Bytes [28:283] */
#define  MAC_FIELD_LEN             0x20   /* 32 Bytes - [284:315]. Same field for Key or Mac */
#define  KEY_FIELD_LEN             0x20   /* 32 Bytes - [284:315]. Same field for Key or Mac */
#define  STUFF_BYTES_FIELD_LEN     0xC4   /* 196 Bytes - [316:511] */

#define  MAC_INPUT_LEN  (BLOCK_SIZE - MAC_FIELD_LEN - STUFF_BYTES_FIELD_LEN)

/* Support 64KB ping pong buffers */
#define  RPMB_MAX_BLOCK_READ        256
#define  RPMB_MAX_HALF_BLOCK_READ   RPMB_MAX_BLOCK_READ*2

#define  RPMB_MAX_BLOCK_WRITE       256
#define  RPMB_MAX_HALF_BLOCK_WRITE  RPMB_MAX_BLOCK_WRITE*2

/* RPMB CG Error Codes */
#define  RPMB_CG_NO_ERROR             0
#define  RPMB_CG_ERR_UNKNOWN          1
#define  RPMB_CG_ERR_INVALID_PARAM    2
#define  RPMB_CG_ERR_MAC              3
#define  RPMB_CG_ERR_WRITE_COUNTER    4   
#define  RPMB_CG_ERR_MAC_GENERATION   5
#define  RPMB_CG_ERR_RNG_GENERATION   6
#define  RPMB_CG_ERR_KEY_GENERATION   7
#define  RPMB_CG_ERR_INVALID_ACCESS   8
#define  RPMB_CG_ERR_INVALID_RESULT   9

typedef struct 
{ 
   UINT8 StuffBytes[196]; 
   UINT8 KeyMAC[32];
   UINT8 Data[256];
   UINT8 Nonce[16];
   UINT8 WriteCounter[4];
   UINT8 Address[2];
   UINT8 BlockCount[2]; 
   UINT8 Result[2];
   UINT8 RequestResponse[2]; 
} RPMB_DATA_FRAME;

#define REVERSE_WORD(x)     ((x & 0xFF) << 8) | ((x >> 8) & 0xFF)

#define REVERSE_DWORD(x)    (((x & 0xFF) << 24) | \
                             ((x & 0xFF000000) >> 24) | \
                             ((x & 0xFF00) << 8) | \
                             ((x & 0xFF0000) >> 8))
                      
static const INT32 g_endian = 1;
#define IS_BIG_ENDIAN() ((*(INT8*)&g_endian) == 0)

/******************************************************************************
* Name: GetRPMBKeyProvisionPkt
*
* Description:
* This function retrieves a Key from TZ then it creates the KeyProvision packet
* with this Key. The packet will be sent to the eMMC to program the eMMC's Key
*
* Arguments:                       
*    PktBuffer  [OUT]: Pointer to a 512-byte formatted packet for Key provision.
* 
* Returns:
*    RPMB_CG_NO_ERROR:            Provision Key packet created successfully
*    RPMB_CG_ERR_INVALID_PARAM:   Invalid pointer to the passed in Packet buffer
*    RPMB_CG_ERR_INVALID_ACCESS:  This API is locked from access
* 
******************************************************************************/
int GetRPMBKeyProvisionPkt (UINT8* PktBuffer);

/******************************************************************************
* Name: ProcessRPMBResponsePkt
*
* Description:
* This function validates the Write Response packet by authenticating
* the packet's HMAC. If authentication passes, it initializes the Write Counter
* on the first call. On subsequent calls, it validates the eMMC's Write Counter
* embedded in the Response packet and the CG's local Write Counter.
* return error code if the Write Counters do not match. 
*
* Arguments:
*    PktBuffer  [IN]: Pointer to a Write Response packet
* 
* Returns:
*    RPMB_CG_NO_ERROR:              The HMAC authentication and Write Counter
*                                   validation succeeded
*    RPMB_CG_ERR_INVALID_PARAM:     Invalid pointer to the passed in Packet buffer
*    RPMB_CG_ERR_MAC_GENERATION:    The HMAC generation failed.
*    RPMB_CG_ERR_MAC:               The HMAC authentication failed
*    RPMB_CG_ERR_WRITE_COUNTER:     The Write Counters validation failed
* 
******************************************************************************/
int ProcessRPMBResponsePkt (UINT8* PktBuffer);

/******************************************************************************
* Name: GenerateRPMBReadPkts
*
* Description:
* This function generates a read command packet to do an authenticated read.
*
* Arguments:
*    StartSector   [IN] : Starting sector to read from 
*    PktBuffer     [OUT]: Pointer to a formatted packet to be sent to eMMC
* 
* Returns:
*    RPMB_CG_NO_ERROR:            The Read packet generation succeeded
*    RPMB_CG_ERR_INVALID_PARAM:   Invalid passed in pointer
*    RPMB_CG_ERR_RNG_GENERATION:  Error in generating the Random number
*                                 (used by the Nonce field)
* 
******************************************************************************/
int GenerateRPMBReadPkts (UINT32 StartSector, UINT32 Count, UINT8* PktBuffer);

/******************************************************************************
* Name: GenerateRPMBWritePkts
*
* Description:
* This function takes the Response packet from previous Write. It generates the
* HMAC for the Response packet and compares the generated HMAC with the packet's
* HMAC. The packet is authenticated if the HMACs match.
* If the packet authentication succeeds, this function then checks the Result field
* of the Response packet. If the Result field is good, it saves the Write Counter
* embedded in the Response packet to the local Write Counter in CG.
* It then generates RPMB Data Frames as defined by the eMMC specification. Depending
* on the value of the Reliable Write Count (RW), the RPMB frames will be generated as follows:
* a. If RW > 1 and (2*Count) is a multiple of RW, the RPMB frames will be created so that
*    the driver must write to the RPMB partition with Write transfer size equals the RW Count value.
*    For example, if RW Count is 4 and Count = 8 sectors of data, 16 512-byte frames buffer
*    will be created. The driver must write 4 512-byte frames to the RPMB each write transfer
*    for a total of 4 transfers.
* b. If RW == 1 or (2*Count) is not a multiple of RW Count, the RPMB frames will be created so that
*    the driver must write a single 512-byte frame for each write transfer.
*
* Arguments:
*    StartSector   [IN]: Starting Sector
*    Count         [IN]: Number of sectors to write 
*    TableData     [IN]: Pointer to Data to be written to RPMB partition
*    RespPktBuffer [IN]: Pointer to privious Write Response packet. This
*                        packet is used to validate the Write Counter
*    PktBuffer    [OUT]: Pointer to a packet that CG has formated to Write
*                        Block command.
* 
* Returns:
*    RPMB_CG_NO_ERROR:              Write Packet generated successfully
*    RPMB_CG_ERR_INVALID_PARAM:     Invalid passed in pointer
*    RPMB_CG_ERR_MAC_GENERATION:    Error when generating the HMAC
*    RPMB_CG_ERR_MAC:               Generated HMAC mismatch with the HMAC in the
*                                   Write Response packet
*    RPMB_CG_ERR_WRITE_COUNTER:     CG's Counter mismatch with Write Counter in
*                                   the Write Response packet
* 
******************************************************************************/
int GenerateRPMBWritePkts (UINT32 StartSector, UINT32 Count, UINT8* TableData, 
                           UINT8 *RespPktBuffer, UINT8* PktBuffer);

/******************************************************************************
* Name: SetRPMBConfigInfo
*
* Description:
* This function sends the Reliable Write Count parameter that is supported by eMMC. 
* 
* Arguments:                       
*    ReliableWriteCount [IN]: Reliable Write Count supported by eMMC
* 
* Returns:
*    RPMB_CG_NO_ERROR
* 
******************************************************************************/
int SetRPMBConfigInfo (UINT32 ReliableWriteCount);

/******************************************************************************
* Name: LockRPMBKeyProvision
*
* Description:
* This function prevents any future access to the GetRPMBProvisionKeyPkt API
* 
* Returns:
*    RPMB_CG_NO_ERROR:   GetRPMBKeyProvisionPkt is locked from future access
* 
******************************************************************************/
int LockRPMBKeyProvision (void);

/******************************************************************************
* Name: SetDeviceType
*
* Description:
* This function sets the device type (eMMC/UFS) in the CG
* 
* Returns:
*    None
* 
******************************************************************************/
void SetDeviceType (UINT32 MediaId);

#endif /* __RPMBCG_H__ */
