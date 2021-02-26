/**
  @file EFIRpmb.h
  @brief Sdcc Rpmb protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2011-2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
 11/15/13    bn      Added RPMB Max Transfer Size
 02/13/13    bn      Fixed klockwork warnings
 11/14/12    llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 11/12/11    bn      Added Reliable Write, RPMB partition size info to the protocol 
 10/27/11    bn      Initial version 
  
=============================================================================*/
#ifndef __EFIRPMB_H__
#define __EFIRPMB_H__

/* RPMB Error Codes */
/** @addtogroup efi_rpmb_constants 
@{ */
/** Function completed successfully. */
#define  RPMB_NO_ERROR              0
/** General failure; this error indicates different failures depending on the 
    operation. */
#define  RPMB_ERR_GENERAL           1
/** Authentication error. */
#define  RPMB_ERR_AUTHENTICATION    2
/** Write counter error. */
#define  RPMB_ERR_WRITE_COUNTER     3
/** Invalid address. */
#define  RPMB_ERR_INVALID_ADDR      4   
/** Write error. */
#define  RPMB_ERR_WRITE             5
/** Read error. */
#define  RPMB_ERR_READ              6
/** Authentication key is not yet programmed. */
#define  RPMB_ERR_KEY_NOT_PROGRAM   7
/** NULL pointer was passed to the function. */
#define  RPMB_ERR_INVALID_PARAM     8
/** @} */ /* end_addtogroup efi_rpmb_constants */

/** @cond */
typedef struct _EFI_SDCC_RPMB_PROTOCOL   EFI_SDCC_RPMB_PROTOCOL;
/** @endcond */

/** @ingroup efi_rpmb_constants
  Protocol version.
*/
#define EFI_RPMB_PROTOCOL_REVISION 0x0000000000010001

/* Protocol GUID definition */
/** @ingroup efi_rpmb_protocol */
#define EFI_RPMB_PROTOCOL_GUID \
   { 0xEF6FD01D, 0x8861, 0x4132, { 0x83, 0x4E, 0xDE, 0xCC, 0x0F, 0xF1, 0xAA, 0xFA } }

/** @cond */
/* External reference to the EFIRpmb Protocol GUID */
extern EFI_GUID gEfiSdccRpmbProtocolGuid;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_READ_COUNTER_PKT */ 
/** @ingroup efi_rpmb_read_counter_pkt
  @par Summary 
  Retrieves the Read Counter Response packet from the eMMC device. The 
  response packet includes the write counter as well as the Message 
  Authentication Code (MAC), which is used later to validate the response 
  packet.
  
  @param[in]  This           Pointer to the EFI_SDCC_RPMB_PROTOCOL instance.
  @param[out] RespPktBuffer  Pointer to the response packet for the Read 
                             Counter command.

  @return
  RPMB_NO_ERROR          -- Function completed successfully. \n
  RPMB_ERR_INVALID_PARAM -- NULL pointer was passed to the function. \n
  RPMB_ERR_READ          -- Failure to read the counter packet.
*/
typedef
INT32
(EFIAPI *EFI_READ_COUNTER_PKT)(
  IN EFI_SDCC_RPMB_PROTOCOL   *This,
  OUT UINT32                  *RespPktBuffer
  );

/* EFI_PROG_PROV_KEY */ 
/** @ingroup efi_rpmb_prog_prov_key
  @par Summary 
  Programs the eMMC's key using the provided packet formatted as the 
  ProvisionKey command. 

  @param[in]  This           Pointer to the EFI_SDCC_RPMB_PROTOCOL instance.
  @param[in]  CmdPktBuffer   Pointer to a ProvisionKey command packet sent to 
                             the eMMC to program its key.
  @param[out] RespPktBuffer  Pointer to the response packet for the
                             ProvisionKey command.

  @return
  RPMB_NO_ERROR          -- Function completed successfully. 
  @par
  RPMB_ERR_INVALID_PARAM -- NULL pointer was passed to the function. 
  @par
  RPMB_ERR_GENERAL       -- Failure to provision the key; key has already been 
                            provisioned.
*/
typedef
INT32
(EFIAPI *EFI_PROG_PROV_KEY)(
  IN EFI_SDCC_RPMB_PROTOCOL   *This,
  IN UINT32                   *CmdPktBuffer,
  OUT UINT32                  *RespPktBuffer
  );

/* EFI_READ_BLOCKS */ 
/** @ingroup efi_rpmb_read_blocks
  @par Summary 
  Reads the sectors from the RPMB partition using the provided command packet 
  buffers.
 
  @param[in]  This          Pointer to the EFI_SDCC_RPMB_PROTOCOL instance.
  @param[in]  CmdPktBuffer  Pointer to a formatted packet for the Read Request. 
  @param[in]  PktCount      How many sectors to read.
  @param[out] DataBuffer    Pointer to the data read from the eMMC.
 
  @return
  RPMB_NO_ERROR          -- Function completed successfully. \n
  RPMB_ERR_INVALID_PARAM -- NULL pointer was passed to the function. \n
  RPMB_ERR_READ          -- Failure to read the sectors.
*/ 
typedef
INT32
(EFIAPI *EFI_READ_BLOCKS)(
  IN EFI_SDCC_RPMB_PROTOCOL   *This,
  IN UINT32                   *CmdPktBuffer,
  IN UINT32                   PktCount,
  OUT UINT32                  *DataBuffer
  );

/* EFI_WRITE_BLOCKS */ 
/** @ingroup efi_rpmb_write_blocks
  @par Summary 
  Writes the sectors to the RPMB partition using the provided command packet 
  buffers.
                       
  @param[in]  This           Pointer to the EFI_SDCC_RPMB_PROTOCOL instance.
  @param[in]  CmdPktBuffer   Pointer to a formatted packet sent to the eMMC to 
                             write to the RPMB partition.
  @param[in]  PktCount       Number of half sectors to write.
  @param[out] RespPktBuffer  Pointer to the response packet from the eMMC.
 
  @return
  RPMB_NO_ERROR          -- Function completed successfully; caller is to 
                            authenticate the response packet and validate the 
                            write counter.
  @par
  RPMB_ERR_INVALID_PARAM -- NULL pointer was passed to the function.
  @par
  RPMB_ERR_WRITE         -- Failure to write the sectors.
*/
typedef
INT32
(EFIAPI *EFI_WRITE_BLOCKS)(
  IN EFI_SDCC_RPMB_PROTOCOL   *This,
  IN UINT32                   *CmdPktBuffer,
  IN UINT32                   PktCount,
  OUT UINT32                  *RespPktBuffer 
  );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_rpmb_protocol
  @par Summary
   Secure Digital Card Controller (SDCC) Replay Protected Memory Block (RPMB) 
   Protocol interface.

  @par Parameters
  @inputprotoparams{rpmb_proto_params.tex} 
*/
struct _EFI_SDCC_RPMB_PROTOCOL {
  UINT64                 Revision;
  UINT32                 ReliableWriteCount;
  UINT32                 RPMBPartitionSizeInBytes;
  EFI_READ_COUNTER_PKT   RPMBReadCounterPkt;
  EFI_PROG_PROV_KEY      RPMBProgProvisionKey;
  EFI_READ_BLOCKS        RPMBReadBlocks;
  EFI_WRITE_BLOCKS       RPMBWriteBlocks;
  UINT32                 RPMBMaxTransferSize;
};

#endif /* __EFIRPMB_H__ */

