/** 
  @file  EFIDebug.h
  @brief Debug Functionality for UEFI.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/03/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 03/14/13   niting  Added dump of UefiSecApp log buffer

=============================================================================*/
#ifndef __EFIDEBUG_H__
#define __EFIDEBUG_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** @ingroup efi_debug_constants
  Protocol version. 
*/
#define EFI_QCOM_DEBUG_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_debug_protocol */
#define EFI_QCOM_DEBUG_PROTOCOL_GUID \
  { 0x6d3a7f58, 0x41ac, 0x3f49, { 0xdb, 0xb5, 0xc2, 0x85, 0xfb, 0x02, 0x7e, 0xb9 } }

/** @ingroup efi_debug_data_types
  Debug log polarities. 
*/
typedef enum
{
  EFI_DEBUG_LOG_UEFISECAPP, /**< UEFI secure application. */
  /** @cond */
  EFI_DEBUG_LOG_MAX
  /** @endcond */
} EFI_DEBUG_LOG_TYPE;

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/* External reference to the Qcom Debug Protocol GUID */
/** @cond */
extern EFI_GUID gEfiQcomDebugProtocolGuid;
/** @endcond */


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* Protocol declaration */
/** @cond */
typedef struct _EFI_QCOMDEBUG_PROTOCOL EFI_QCOMDEBUG_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_GETDEBUGLOG */
/** @ingroup efi_debug_getdebuglog
  @par Summary
  Copies the debug log buffer to the input buffer.
  
  @param[in]  This                Pointer to the EFI_QCOMDEBUG_PROTOCOL instance.
  @param[in]  DebugLogBuffer      Buffer pointer.
  @param[in]  DebugLogBufferSize  Pointer to the buffer size in bytes. 
                                  - On input, the input buffer size 
                                  - On output, the size of the data copied or 
                                    required if input DumpLogBufferSize is too 
                                    small @tablebulletend
  @param[in]  LogType             Identifies which log to retrieve.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_BUFFER_TOO_SMALL  -- Supplied buffer is too small.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_GETDEBUGLOG)(
  IN     EFI_QCOMDEBUG_PROTOCOL           *This,
  IN OUT UINT8                            *DebugLogBuffer,
  IN OUT UINT32                           *DebugLogBufferSize,
  IN     UINT32                           LogType
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_debug_protocol
  @par Summary
  Qualcomm Debug Protocol interface.
  
  @par Parameters
  @inputprotoparams{debug_proto_params.tex} 
*/
struct _EFI_QCOMDEBUG_PROTOCOL {
  UINT64                       Revision;
  EFI_GETDEBUGLOG              GetDebugLog;
};

#endif /* __EFIDEBUG_H__ */
