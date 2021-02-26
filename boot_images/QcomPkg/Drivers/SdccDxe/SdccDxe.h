/** @file SdccDxe.h
   
  SDCC Driver header file

  Copyright (c) 2014-2017 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential
   
  Portions Copyright (c) 2006-2008, 2014-2015 Intel Corporation 

  All rights reserved. This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License 
  which accompanies this distribution. The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

/*=============================================================================
                              EDIT HISTORY


when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
2017/05/12   sb      Unified erase protocol support for eMMC
2016/01/26   jt      Update card info  
2015/10/13   rm      Added BlockIO2 functions
2014/10/14   bn      Added SDCC_CONFIG_PROTOCOL
2014/07/30   bn      Initial version. Branched from 8994 UEFI

=============================================================================*/

#ifndef _SDCCDXE_H_
#define _SDCCDXE_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/GPTListener.h>
#include <Library/RpmbListener.h>
#include <Library/RpmbLib.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EFICardInfo.h>
#include <Protocol/EFIClock.h>
#include <Protocol/EFIHWIO.h>
#include <Protocol/EFIRpmb.h>
#include <Protocol/EFIWriteProtect.h>
#include <Protocol/EFIEmmcSecureErase.h>
#include <Protocol/EFIEraseBlock.h>
#include <Protocol/EFISdccConfig.h>
#include "SdccCommon.h"

#define HCS            BIT30 //Host capacity support/1 = Supporting high capacity
#define CCS            BIT30 //Card capacity status/1 = High capacity card

#define SDCC_TPL TPL_CALLBACK

/* Set the Max transfer size to 8MB. 16384 blocks * 512 = 8388608 bytes */
/* We allocated 342 descriptors for a BAM transfer in sdcc_bam_init() */
/* Each descriptor can transfer 24,576 bytes (according to the BAM team) */
/* 342 desc * 24576 = 8404992 bytes max transfer size. */
#define SDCC_MAX_BLOCK_TRANSFER_SIZE   16384

typedef struct {
  UINT32  Reserved0:   7; // 0 
  UINT32  V170_V195:   1; // 1.70V - 1.95V
  UINT32  V200_V260:   7; // 2.00V - 2.60V
  UINT32  V270_V360:   9; // 2.70V - 3.60V
  UINT32  RESERVED_1:  5; // Reserved
  UINT32  AccessMode:  2; // 00b (byte mode), 10b (sector mode) 
  UINT32  Busy:        1; // This bit is set to LOW if the card has not finished the power up routine
}OCR;

typedef struct {
  UINT32  NOT_USED;   // 1 [0:0]
  UINT32  CRC;        // CRC7 checksum [7:1]
  UINT32  MDT;        // Manufacturing date [19:8]
  UINT32  RESERVED_1; // Reserved [23:20]
  UINT32  PSN;        // Product serial number [55:24]
  UINT8   PRV;        // Product revision [63:56]
  UINT8   PNM[5];     // Product name [64:103]
  UINT16  OID;        // OEM/Application ID [119:104]
  UINT8   MID;        // Manufacturer ID [127:120]
}CID;

typedef struct {
  UINT8   NOT_USED:           1; // Not used, always 1 [0:0]
  UINT8   CRC:                7; // CRC [7:1]

  UINT8   RESERVED_1:         2; // Reserved [9:8]
  UINT8   FILE_FORMAT:        2; // File format [11:10]
  UINT8   TMP_WRITE_PROTECT:  1; // Temporary write protection [12:12]
  UINT8   PERM_WRITE_PROTECT: 1; // Permanent write protection [13:13]
  UINT8   COPY:               1; // Copy flag (OTP) [14:14]
  UINT8   FILE_FORMAT_GRP:    1; // File format group [15:15]
  
  UINT16  RESERVED_2:         5; // Reserved [20:16]
  UINT16  WRITE_BL_PARTIAL:   1; // Partial blocks for write allowed [21:21]
  UINT16  WRITE_BL_LEN:       4; // Max. write data block length [25:22]
  UINT16  R2W_FACTOR:         3; // Write speed factor [28:26]
  UINT16  RESERVED_3:         2; // Reserved [30:29]
  UINT16  WP_GRP_ENABLE:      1; // Write protect group enable [31:31]
  
  UINT32  WP_GRP_SIZE:        7; // Write protect group size [38:32]
  UINT32  SECTOR_SIZE:        7; // Erase sector size [45:39]
  UINT32  ERASE_BLK_EN:       1; // Erase single block enable [46:46]
  UINT32  C_SIZE_MULT:        3; // Device size multiplier [49:47]
  UINT32  VDD_W_CURR_MAX:     3; // Max. write current @ VDD max [52:50]
  UINT32  VDD_W_CURR_MIN:     3; // Max. write current @ VDD min [55:53]
  UINT32  VDD_R_CURR_MAX:     3; // Max. read current @ VDD max [58:56]
  UINT32  VDD_R_CURR_MIN:     3; // Max. read current @ VDD min [61:59]
  UINT32  C_SIZELow2:         2; // Device size [63:62]
  
  UINT32  C_SIZEHigh10:       10;// Device size [73:64]
  UINT32  RESERVED_4:         2; // Reserved [75:74]
  UINT32  DSR_IMP:            1; // DSR implemented [76:76]
  UINT32  READ_BLK_MISALIGN:  1; // Read block misalignment [77:77]
  UINT32  WRITE_BLK_MISALIGN: 1; // Write block misalignment [78:78]
  UINT32  READ_BL_PARTIAL:    1; // Partial blocks for read allowed [79:79]
  UINT32  READ_BL_LEN:        4; // Max. read data block length [83:80]
  UINT32  CCC:                12;// Card command classes [95:84]

  UINT8   TRAN_SPEED          ;  // Max. bus clock frequency [103:96]
  UINT8   NSAC                ;  // Data read access-time 2 in CLK cycles (NSAC*100) [111:104]
  UINT8   TAAC                ;  // Data read access-time 1 [119:112]
  
  UINT8   RESERVED_5:         6; // Reserved [125:120]
  UINT8   CSD_STRUCTURE:      2; // CSD structure [127:126]
}CSD;

typedef struct {
  UINT8   NOT_USED:           1; // Not used, always 1 [0:0]
  UINT8   CRC:                7; // CRC [7:1]
  UINT8   RESERVED_1:         2; // Reserved [9:8]
  UINT8   FILE_FORMAT:        2; // File format [11:10]
  UINT8   TMP_WRITE_PROTECT:  1; // Temporary write protection [12:12]
  UINT8   PERM_WRITE_PROTECT: 1; // Permanent write protection [13:13]
  UINT8   COPY:               1; // Copy flag (OTP) [14:14]
  UINT8   FILE_FORMAT_GRP:    1; // File format group [15:15]
  UINT16  RESERVED_2:         5; // Reserved [20:16]
  UINT16  WRITE_BL_PARTIAL:   1; // Partial blocks for write allowed [21:21]
  UINT16  WRITE_BL_LEN:       4; // Max. write data block length [25:22]
  UINT16  R2W_FACTOR:         3; // Write speed factor [28:26]
  UINT16  RESERVED_3:         2; // Reserved [30:29]
  UINT16  WP_GRP_ENABLE:      1; // Write protect group enable [31:31]
  UINT16  WP_GRP_SIZE:        7; // Write protect group size [38:32]
  UINT16  SECTOR_SIZE:        7; // Erase sector size [45:39]
  UINT16  ERASE_BLK_EN:       1; // Erase single block enable [46:46]
  UINT16  RESERVED_4:         1; // Reserved [47:47]
  UINT32  C_SIZELow16:        16;// Device size [69:48]
  UINT32  C_SIZEHigh6:        6; // Device size [69:48]
  UINT32  RESERVED_5:         6; // Reserved [75:70]
  UINT32  DSR_IMP:            1; // DSR implemented [76:76]
  UINT32  READ_BLK_MISALIGN:  1; // Read block misalignment [77:77]
  UINT32  WRITE_BLK_MISALIGN: 1; // Write block misalignment [78:78]
  UINT32  READ_BL_PARTIAL:    1; // Partial blocks for read allowed [79:79]
  UINT16  READ_BL_LEN:        4; // Max. read data block length [83:80]
  UINT16  CCC:                12;// Card command classes [95:84]
  UINT8   TRAN_SPEED          ;  // Max. bus clock frequency [103:96]
  UINT8   NSAC                ;  // Data read access-time 2 in CLK cycles (NSAC*100) [111:104]
  UINT8   TAAC                ;  // Data read access-time 1 [119:112]
  UINT8   RESERVED_6:         6; // 0 [125:120]
  UINT8   CSD_STRUCTURE:      2; // CSD structure [127:126]
}CSD_SDV2;

typedef enum {
  UNKNOWN_CARD,
  MMC_CARD,              //MMC card
  SD_CARD,               //SD 1.1 card
  SD_CARD_2,             //SD 2.0 or above standard card
  SD_CARD_2_HIGH         //SD 2.0 or above high capacity card
} CARD_TYPE;

typedef struct  {
  UINT16    RCA;
  UINTN     BlockSize;
  UINTN     NumBlocks;
  UINTN     ClockFrequencySelect;
  CARD_TYPE CardType;
  OCR       OCRData;
  CID       CIDData;
  CSD       CSDData;
} CARD_INFO;

#define MMCHS_DEV_SIGNATURE SIGNATURE_32 ('s', 'd', 'c', 'c')

typedef struct {
  UINT32                     Signature;
  UINT32                     SlotNumber;
  EFI_HANDLE                 ClientHandle;
  EFI_HANDLE                 DeviceHandle;
  EFI_BLOCK_IO_PROTOCOL      BlkIo;
  EFI_MEM_CARDINFO_PROTOCOL  CardInfo;  
  EFI_SDCC_RPMB_PROTOCOL     SdccRPMB; 
  EFI_EMMC_WP_PROTOCOL       EmmcWriteProtect; 
  EFI_ERASE_BLOCK_PROTOCOL   EmmcErase;
  EFI_EMMC_SECURE_ERASE_PROTOCOL    EmmcSecureErase; 
  EFI_SDCC_CONFIG_PROTOCOL   SdccConfig;  
  EFI_BLOCK_IO2_PROTOCOL     BlkIo2;
} MMCHS_DEV;


/**
   Reset the Block Device.
   
   @param  This                 Indicates a pointer to the calling context.
   @param  ExtendedVerification Driver may perform diagnostics on reset.
   
   @retval EFI_SUCCESS          The device was reset.
   @retval EFI_DEVICE_ERROR     The device is not functioning properly
                                and could not be reset.

**/
EFI_STATUS EFIAPI MMCHSReset (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN BOOLEAN                 ExtendedVerification
   );

/**
   Read BufferSize bytes from Lba into Buffer.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the destination buffer for the data.
                      The caller is responsible for either having implicit or
                      explicit ownership of the buffer.
   
   @retval EFI_SUCCESS           The data was read correctly from the device.
   @retval EFI_DEVICE_ERROR      The device reported an error while
                                 performing the read.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHANGED     The MediaId does not matched the
                                 current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block
                                 size of the device.
   @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
                                 valid,or the buffer is not on proper alignment.
EFI_STATUS
**/
EFI_STATUS EFIAPI MMCHSReadBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   OUT VOID                   *Buffer
   );

/**
   Write BufferSize bytes from Lba into Buffer.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    The media ID that the write request is for.
   @param  Lba        The starting logical block address to be 
                      written. The   caller is responsible for
                      writing to only legitimate locations.
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the source buffer for the data.
   
   @retval EFI_SUCCESS           The data was written correctly to the device.
   @retval EFI_WRITE_PROTECTED   The device can not be written to.
   @retval EFI_DEVICE_ERROR      The device reported an error 
                                 while performing the write.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHNAGED     The MediaId does not matched 
                                 the current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple 
                                 of the block size of the device.
   @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not
                                 valid or the buffer is not on proper
                                 alignment.
**/ 
EFI_STATUS EFIAPI MMCHSWriteBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   IN VOID                    *Buffer
   );

/**
   Flush the Block Device.
   
   @param  This              Indicates a pointer to the calling context.
   
   @retval EFI_SUCCESS       All outstanding data was written to the device
   @retval EFI_DEVICE_ERROR  The device reported an error while 
                             writting back the data
   @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS EFIAPI MMCHSFlushBlocks (
   IN EFI_BLOCK_IO_PROTOCOL  *This
   );


/**
   Function: RPMBReadCounterPkt
 
   This function retrieves Read Counter Response packet from eMMC device.
   The Response packet includes the Write Counter as well as the MAC which
   is used later to validate the Response packet
  
   @param  RespPktBuffer [OUT]    Pointer to the response from the Read
                                  Counter command
    
   
   @retval  RPMB_NO_ERROR           Successfully read the Counter packet.
   @retval  RPMB_ERR_INVALID_PARAM  NULL pointer is passed in to the function.
   @retval  RPMB_ERR_READ           Failed to read the Counter packet.
    
**/
INT32 EFIAPI RPMBReadCounterPkt (
   EFI_SDCC_RPMB_PROTOCOL       *This,
   UINT32                       *RespPktBuffer   
   );

/**
 Function: RPMBProgProvisionKey
 
 This function programs the eMMC's key using the provided packet
 formatted as ProvisionKey command. 

 @param CmdPktBuffer  [IN] : Pointer to a ProvisionKey command packet
                             to be sent to eMMC to program its key
 @param RespPktBuffer [OUT]: Pointer to the response packet for the
                             ProvisionKey command
 
 @retval  RPMB_NO_ERROR            Successfully provision the eMMC's Key.
 @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function.
 @retval  RPMB_ERR_GENERAL         Failed to provision the key (Key has been
                                   provisioned).
  
**/
INT32 EFIAPI RPMBProgProvisionKey (
   EFI_SDCC_RPMB_PROTOCOL       *This,
   UINT32                       *CmdPktBuffer,
   UINT32                       *RespPktBuffer 
   );

/**
  Function: RPMBReadBlocks
 
  This function reads the sectors from RPMB partition using the
  cmd packet buffers provided.
 
  @param   CmdPktBuffer  [IN] : Pointer to a formatted packet for Read Request 
  @param   PktCount      [IN] : How many sectors to read                        
  @param   DataBuffer    [OUT]: Pointer to data read from eMMC
 
  @retval  RPMB_NO_ERROR            Successfully read the Counter packet
  @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function
  @retval  RPMB_ERR_READ            Failed to read the sectors
     
**/ 
INT32 EFIAPI RPMBReadBlocks (
   EFI_SDCC_RPMB_PROTOCOL       *This,
   UINT32                       *CmdPktBuffer,
   UINT32                       PktCount,
   UINT32                       *DataBuffer
   );

/**
  Function: RPMBWriteBlocks
 
  This function writes the sectors to the RPMB partition using
  the provided cmd packet buffers
                       
  @param  CmdPktBuffer  [IN] :  Pointer to a formatted packets to be sent
                                to eMMC to write to RPMB partition
  @param  PktCount      [IN] :  Number of half sectors to write
  @param  RespPktBuffer [OUT]:  Pointer to the response packet from eMMC
 
  @retval  RPMB_NO_ERROR            Successfully Write the RPMB sectors.
                                    Caller should authenticate the Response
                                    packet and validate the Write Counter
  @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function
  @retval  RPMB_ERR_WRITE           Failed to write the sectors
  
**/
INT32 EFIAPI RPMBWriteBlocks (
   EFI_SDCC_RPMB_PROTOCOL   *This,
   UINT32                   *CmdPktBuffer,
   UINT32                   PktCount, 
   UINT32                   *RespPktBuffer 
   );

/**
  Function: MMCHSGetCardInfo
 
   This function retrieves the card information. 
  
   @param  card_info              Pointer to a MEM_CARD_INFO
                                  variable that is used to store
                                  the retrieved card information
    
   @retval EFI_SUCCESS            The function completed successfully. 
   @retval EFI_INVALID_PARAMETER  The pass-in parameter is invalid.
**/
EFI_STATUS EFIAPI MMCHSGetCardInfo (
   EFI_MEM_CARDINFO_PROTOCOL   *This,
   MEM_CARD_INFO*              card_info
   );

/** 
  Function: EmmcWriteProtectGetDeviceCapabilities
 
  Returns information about the eMMC write protection capabilities

  @param This             A pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param Capabilities     Capabilities of the MMC device. 

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
*/
EFI_STATUS EFIAPI EmmcWriteProtectGetDeviceCapabilities (
   IN EFI_EMMC_WP_PROTOCOL *This, 
   OUT EFI_EMMC_WP_CAPABILITIES *Capabilities 
);

/**
  Function: EmmcSetWriteProtect
 
  This function applies write protection to the range specified. 
  Both the SectorStart and SectorCount must align to the WriteProtectGroupSize 
  (as calculated for GetDeviceCapabilities). If either is not aligned, 
  EFI_INVALID_PARAMETER is returned. If the parameters are valid, 
  this function sends the commands to the eMMC part to apply “Power-On” write 
  protection to the range. This interface only supports the “Power-On” write protect mode. 

  @param This         A pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param SectorStart  The start sector for the write protect operation
  @param SectorCount  The count of sectors to apply power-on write protection to.

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid, including invalid
                                 alignment of the start and count values.
  @return EFI_DEVICE_ERROR       The physical device reported an error applying
                                 the write protect commands. 
*/
EFI_STATUS EFIAPI EmmcSetWriteProtect (
   IN EFI_EMMC_WP_PROTOCOL *This, 
   IN UINT32 SectorStart, 
   IN UINT32 SectorCount 
);

/**
  Function: EmmcSendWriteProtectType
   
  Returns the type of protection for the group containing the sector specified by ‘Sector’. 
  CMD31 (SEND_WRITE_PROT_TYPE) is used to get the protection type for the write protect groups, 
  then mask off the two LSBs that indicates the protection type for the starting group specified 
  by the Sector address. See Table 26 in “JEDEC Standard No. 84-A441” for details.
 
  @param This         A pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param Sector       A sector address to the write protect group. 
                      Must be aligned to the write protect group size.
  @param ProtectType  The type of protect applied.

  @return EFI_SUCCESS             The function returned successfully.
  @return EFI_INVALID_PARAMETER   A parameter is invalid.
  @return EFI_DEVICE_ERROR        The physical device reported an error. 
*/
EFI_STATUS EFIAPI EmmcSendWriteProtectType (
   IN EFI_EMMC_WP_PROTOCOL *This, 
   IN UINT32 Sector, 
   OUT EFI_EMMC_WP_TYPE *ProtectType
   );


/*
  Function: EmmcEraseBlocks
 
  This function erases the eMMC sectors specified by LBA and
  Size. Both the LBA and Size must align to the 
  EraseLengthGranularity. If either is not aligned, 
  EFI_INVALID_PARAMETER is returned. If the parameters are 
  valid, this function sends the commands to the eMMC part to 
  apply ERASE erase to the specified sectors. 

  @param This         A pointer to the calling context. 
  @param MediaId      Current media ID 
  @param LBA          The start sector for the erase operation 
  @param Token        Pointer to the token associated with the 
                      transaction
  @param Size         The number of bytes to apply erase 

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid, including invalid
                                 alignment of the start and count values.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
**/
EFI_STATUS EFIAPI EmmcEraseBlocks (
   IN EFI_BLOCK_IO_PROTOCOL *This,
   IN UINT32 MediaId,
   IN EFI_LBA LBA,
   IN OUT EFI_ERASE_BLOCK_TOKEN *Token,
   IN UINTN Size
   );

/**
  Function: SdccEnableSdhciMode
 
  This function enables the Host Controller specification
 
  @param This      A pointer to the EFI_SDCC_CONFIG_PROTOCOL instance.

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid.
**/
EFI_STATUS EFIAPI SdccEnableSdhciMode (
   IN EFI_SDCC_CONFIG_PROTOCOL *This
   );

/*
  Function: EmmcSecureEraseGetDeviceInfo
 
  This function retrieves the eMMC's info needed to perform the erase operation

  @param This              A pointer to the EFI_EMMC_SECURE_ERASE_PROTOCOL 
                            instance.
  @param EraseDeviceInfo   eMMC's erase info such as the EraseGroupSize

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
**/
EFI_STATUS EFIAPI EmmcSecureEraseGetDeviceInfo (
   IN EFI_EMMC_SECURE_ERASE_PROTOCOL *This,
   OUT EFI_EMMC_SECURE_ERASE_DEVICE_INFO *EraseDeviceInfo   
   );

/*
  Function: EmmcSecureEraseBlocks
 
  This function erases the eMMC sectors specified by SectorStart and SectorCount. 
  Both the SectorStart and SectorCount must align to the EraseGroupSize 
  If either is not aligned, EFI_INVALID_PARAMETER is returned. 
  If the parameters are valid, this function sends the commands to the eMMC part to 
  apply ERASE erase to the specified sectors.

  @param This         A pointer to the EFI_EMMC_SECURE_ERASE_PROTOCOL instance.
  @param SectorStart  The start sector for the erase operation
  @param SectorCount  The number of sectors to apply ERASE erase.

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid, including invalid
                                 alignment of the start and count values.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
**/
EFI_STATUS EFIAPI EmmcSecureEraseBlocks (
   IN EFI_EMMC_SECURE_ERASE_PROTOCOL *This, 
   IN UINT32 SectorStart, 
   IN UINT32 SectorCount 
   );
   
/** 
   Function: DetectCard
 
   This function checks if a card is presence. Upon initial detection,
   it opens communication with the card by sdcc_handle_open. It 
   saves the returned Handle in the global variable MMCHSDevice
   and sets the MediaPresent flag for future use
  
   @param  MMCHSDevice            Pointer to a MMCHS_DEV variable 
   @param  partition_num          Physical partition number 
    
   @retval EFI_SUCCESS            The card dection completed successfully. 
   @retval EFI_NO_MEDIA           There is no memory card
   @retval EFI_DEVICE_ERROR       A memory card is detected, but there're errors
                                  when trying to access it.
**/
EFI_STATUS DetectCard (
   MMCHS_DEV  *MMCHSDevice, 
   UINT32     partition_num
   );

/** 
   Function: BlkIo2TimerEventNotify
 
   This function is an re-entrant function. It can be called from other
   function or can be set as a timer event notify function. Major Block 
   IO 2 work is done in this function.
  
   @param  Event                  Pointer to a MMCHS_DEV variable 
   @param  Context                Physical partition number 
    
**/   
   
VOID
BlkIo2TimerEventNotify(
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  );

/**
   Read BufferSize bytes from Lba into Buffer.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the destination buffer for the data.
                      The caller is responsible for either having implicit or
                      explicit ownership of the buffer.
   
   @retval EFI_SUCCESS           The data was read correctly from the device.
   @retval EFI_DEVICE_ERROR      The device reported an error while
                                 performing the read.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHANGED     The MediaId does not matched the
                                 current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block
                                 size of the device.
   @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
                                 valid,or the buffer is not on proper alignment.
**/
EFI_STATUS
MMCHSReadBlocksWrapper (
  IN EFI_BLOCK_IO_PROTOCOL   *This,
  IN UINT32                  MediaId,
  IN EFI_LBA                 Lba,
  IN UINTN                   BufferSize,
  IN VOID                    *Buffer
   );

/**
   Write BufferSize bytes from Lba into Buffer.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    The media ID that the write request is for.
   @param  Lba        The starting logical block address to be 
                      written. The   caller is responsible for
                      writing to only legitimate locations.
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the source buffer for the data.
   
   @retval EFI_SUCCESS           The data was written correctly to the device.
   @retval EFI_WRITE_PROTECTED   The device can not be written to.
   @retval EFI_DEVICE_ERROR      The device reported an error 
                                 while performing the write.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHNAGED     The MediaId does not matched 
                                 the current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple 
                                 of the block size of the device.
   @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not
                                 valid or the buffer is not on proper
                                 alignment.
**/    
EFI_STATUS
MMCHSWriteBlocksWrapper (
  IN EFI_BLOCK_IO_PROTOCOL   *This,
  IN UINT32                  MediaId,
  IN EFI_LBA                 Lba,
  IN UINTN                   BufferSize,
  IN VOID                    *Buffer
   );   

/**
  Reset the block device hardware.

  @param[in]  This                 Indicates a pointer to the calling context.
  @param[in]  ExtendedVerification Indicates that the driver may perform a more
                                   exhausive verfication operation of the device
                                   during reset.

  @retval EFI_SUCCESS          The device was reset.
  @retval EFI_DEVICE_ERROR     The device is not functioning properly and could
                               not be reset.

**/

EFI_STATUS EFIAPI MMCHSResetEx (
  IN EFI_BLOCK_IO2_PROTOCOL  *This,
  IN BOOLEAN                 ExtendedVerification
  );

/**
  Read BufferSize bytes from Lba into Buffer.
  
  This function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned.
  If EFI_DEVICE_ERROR, EFI_NO_MEDIA,_or EFI_MEDIA_CHANGED is returned and
  non-blocking I/O is being used, the Event associated with this request will
  not be signaled.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       MediaId    Id of the media, changes every time the media is 
                              replaced.
  @param[in]       Lba        The starting Logical Block Address to read from.
  @param[in, out]  Token	    A pointer to the token associated with the transaction.
  @param[in]       BufferSize Size of Buffer, must be a multiple of device block size.  
  @param[out]      Buffer     A pointer to the destination buffer for the data. The 
                              caller is responsible for either having implicit or 
                              explicit ownership of the buffer.

  @retval EFI_SUCCESS           The read request was queued if Token->Event is
                                not NULL.The data was read correctly from the
                                device if the Token->Event is NULL.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
                                the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId is not for the current media.
  @retval EFI_BAD_BUFFER_SIZE   The BufferSize parameter is not a multiple of the
                                intrinsic block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid, 
                                or the buffer is not on proper alignment.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.
**/
EFI_STATUS EFIAPI MMCHSReadBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                LBA,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  OUT VOID                  *Buffer
  );


/**
  Write BufferSize bytes from Lba into Buffer.

  This function writes the requested number of blocks to the device. All blocks
  are written, or an error is returned.If EFI_DEVICE_ERROR, EFI_NO_MEDIA,
  EFI_WRITE_PROTECTED or EFI_MEDIA_CHANGED is returned and non-blocking I/O is
  being used, the Event associated with this request will not be signaled.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       MediaId    The media ID that the write request is for.
  @param[in]       Lba        The starting logical block address to be written. The
                              caller is responsible for writing to only legitimate
                              locations.
  @param[in, out]  Token      A pointer to the token associated with the transaction.
  @param[in]       BufferSize Size of Buffer, must be a multiple of device block size.
  @param[in]       Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The write request was queued if Event is not NULL.
                                The data was written correctly to the device if
                                the Event is NULL.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid, 
                                or the buffer is not on proper alignment.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.

**/
EFI_STATUS EFIAPI MMCHSWriteBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL  *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                LBA,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  IN     VOID                   *Buffer
  );  
      
  
/**
  Flush the Block Device.
 
  If EFI_DEVICE_ERROR, EFI_NO_MEDIA,_EFI_WRITE_PROTECTED or EFI_MEDIA_CHANGED
  is returned and non-blocking I/O is being used, the Event associated with
  this request will not be signaled.  

  @param[in]      This     Indicates a pointer to the calling context.
  @param[in,out]  Token    A pointer to the token associated with the transaction

  @retval EFI_SUCCESS          The flush request was queued if Event is not NULL.
                               All outstanding data was written correctly to the
                               device if the Event is NULL.
  @retval EFI_DEVICE_ERROR     The device reported an error while writting back
                               the data.
  @retval EFI_WRITE_PROTECTED  The device cannot be written to.
  @retval EFI_NO_MEDIA         There is no media in the device.
  @retval EFI_MEDIA_CHANGED    The MediaId is not for the current media.
  @retval EFI_OUT_OF_RESOURCES The request could not be completed due to a lack
                               of resources.

**/
EFI_STATUS EFIAPI MMCHSFlushBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL   *This,
  IN OUT EFI_BLOCK_IO2_TOKEN      *Token
  );

/**
   Decode the error code returned from the SDCC APIs
   
   @param  ecode     Incoming error code
**/    
void MMCHSDecodeErrorCode (
   IN SDCC_STATUS                   ecode
   );    
   
#define MMCHS_DEV_FROM_BLOCKIO(a)  CR (a, MMCHS_DEV, BlkIo, MMCHS_DEV_SIGNATURE)

#define MMCHS_DEV_FROM_BLOCKIO2(a) CR (a, MMCHS_DEV, BlkIo2, MMCHS_DEV_SIGNATURE)

#define MMCHS_DEV_FROM_CARD_INFO(a) CR (a, MMCHS_DEV, CardInfo, MMCHS_DEV_SIGNATURE)

#define MMCHS_DEV_FROM_RPMB(a) CR (a, MMCHS_DEV, SdccRPMB, MMCHS_DEV_SIGNATURE)

#define MMCHS_DEV_FROM_WRITE_PROTECT(a) CR (a, MMCHS_DEV, EmmcWriteProtect, MMCHS_DEV_SIGNATURE)

#define MMCHS_DEV_FROM_ERASE(a) CR (a, MMCHS_DEV, EmmcErase, MMCHS_DEV_SIGNATURE)

#define MMCHS_DEV_FROM_SECURE_ERASE(a) CR (a, MMCHS_DEV, EmmcSecureErase, MMCHS_DEV_SIGNATURE)

#define EFI_EMMC_USER_PARTITION_GUID \
    { 0xb615f1f5, 0x5088, 0x43cd, { 0x80, 0x9c, 0xa1, 0x6e, 0x52, 0x48, 0x7d, 0x00 } }

#define EFI_SD_REMOVABLE_GUID \
    { 0xD1531D41, 0x3F80, 0x4091, { 0x8D, 0x0A, 0x54, 0x1F, 0x59, 0x23, 0x6D, 0x66 } }

#define EFI_EMMC_BOOT_PARTITION1_GUID \
    { 0x12C55B20, 0x25D3, 0x41C9, { 0x8E, 0x06, 0x28, 0x2D, 0x94, 0xC6, 0x76, 0xAD } }

#define EFI_EMMC_BOOT_PARTITION2_GUID \
    { 0x6B76A6DB, 0x0257, 0x48A9, { 0xAA, 0x99, 0xF6, 0xB1, 0x65, 0x5F, 0x7B, 0x00 } }
 
#define EFI_EMMC_RPMB_PARTITION_GUID \
    { 0xC49551EA, 0xD6BC, 0x4966, { 0x94, 0x99, 0x87, 0x1E, 0x39, 0x31, 0x33, 0xCD } }

#define EFI_EMMC_GPP_PARTITION1_GUID \
   { 0xB9251EA5, 0x3462, 0x4807, { 0x86, 0xC6, 0x89, 0x48, 0xB1, 0xB3, 0x61, 0x63 } }

#define EFI_EMMC_GPP_PARTITION2_GUID \
   { 0x24F906CD, 0xEE11, 0x43E1, { 0x84, 0x27, 0xDC, 0x7A, 0x36, 0xF4, 0xC0, 0x59 } }

#define EFI_EMMC_GPP_PARTITION3_GUID \
   { 0x5A5709A9, 0xAC40, 0x4F72, { 0x88, 0x62, 0x5B, 0x01, 0x04, 0x16, 0x6E, 0x76 } }

#define EFI_EMMC_GPP_PARTITION4_GUID \
   { 0xA44E27C9, 0x258E, 0x406E, { 0xBF, 0x33, 0x77, 0xF5, 0xF2, 0x44, 0xC4, 0x87 } }

#define EFI_EVENT_GROUP_DETECT_SD_CARD \
   { 0xB7972C36, 0x8A4C, 0x4A56, { 0x8B, 0x02, 0x11, 0x59, 0xB5, 0x2D, 0x4B, 0xFB } }


extern EFI_GUID gEfiSdRemovableGuid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiEmmcBootPartition1Guid; 
extern EFI_GUID gEfiEmmcBootPartition2Guid; 
extern EFI_GUID gEfiEmmcRpmbPartitionGuid; 
extern EFI_GUID gEfiEmmcGppPartition1Guid; 
extern EFI_GUID gEfiEmmcGppPartition2Guid; 
extern EFI_GUID gEfiEmmcGppPartition3Guid; 
extern EFI_GUID gEfiEmmcGppPartition4Guid; 
extern EFI_GUID gEfiSdccRpmbProtocolGuid;
extern EFI_GUID gEfiEventDetectSdCardGuid;
extern EFI_GUID gEfiEventExitBootServicesGuid;
extern EFI_GUID gQcomScmProtocolGuid; 

#endif /* _SDCCDXE_H_ */

