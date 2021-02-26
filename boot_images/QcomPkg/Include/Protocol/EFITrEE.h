/**
  @file EFITrEE.h
  @brief Declaration of TrEE DXE Driver Interface 
*/
/*=============================================================================
  Copyright (c) 2011-2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 05/19/15    sk     Added support for TCG 2.0
 12/05/14    sm     Updated types for compilation.
 05/06/13    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 04/22/13  bmuthuku Send signal to fTPM to disable NV flushing. Add function ptrs.
 02/04/13    shl    Added get application ID function in tmp protocol
 12/10/12    shl    Moved in tpm related structure defs.
 09/27/12    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 07/18/11    shl    Initial version.

=============================================================================*/

#ifndef _EFITREE_H_
#define _EFITREE_H_

#include <Uefi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Include/IndustryStandard/Acpi10.h>
#include <IndustryStandard/UefiTcgPlatform.h>

//typedef UINT8 BYTE;

//==========================================================================================
//  The following are required for the header event as per latest TCG 2.0 spec
//  The header event is the 1st event that goes into the log (not TPM)
//  This must only be done the first time

/* TIANOCORE UPDATE */
#if 0
typedef struct tdTCG_EfiSpecIdEventAlgorithmSize 
{
  UINT16          algorithmId;
  UINT16          digestSize;
} TCG_EfiSpecIdEventAlgorithmSize;
typedef struct tdTCG_EfiSpecIdEventStruct 
{
  UINT8           signature[16];  
  UINT32          platformClass;
  UINT8           specVersionMinor;
  UINT8           specVersionMajor;
  UINT8           specErrata;
  UINT8           uintnSize;
  UINT32          numberOfAlgorithms;
  TCG_EfiSpecIdEventAlgorithmSize digestSizes[1];   // numberOfAlgorithms = 1 i.e. SHA256
  UINT8           vendorInfoSize;
  //BYTE[VendorInfoSize]  vendorInfo; 
} TCG_EfiSpecIDEventStruct; 
#endif

//==========================================================================================

/** @cond */
typedef struct {
    UINT32   Heartbeat;
    UINT32   Error;
    UINT32   Cancel;
    UINT32   Start;
    UINT64   InterruptControl;
    UINT32   CommandSize;
    UINT64   Command;
    UINT32   ResponseSize;
    UINT64   Response;
} __attribute__ ((packed)) TPM2_ACPI_CONTROL_AREA;


typedef struct {
    EFI_ACPI_DESCRIPTION_HEADER   Header;
    UINT32                        Flags;
    UINT64                        ControlArea;
    UINT32                        Start;
    UINT32                        PlatformParameters[4];
}  __attribute__ ((packed))  ACPI_TPM2_TABLE;

/* define EFI_QCOM_SCM_PROTOCOL */
typedef struct  _EFI_TREE_PROTOCOL EFI_TREE_PROTOCOL;

/* define EFI_TPM_PROTOCOL */
typedef struct  _EFI_TPM_PROTOCOL EFI_TPM_PROTOCOL;
/** @endcond */

/*  TrEE UEFI GUID */
/** @ingroup efi_tree_protocol */
extern EFI_GUID gEfiTrEEProtocolGuid;
/** @cond */
extern EFI_GUID gEfiTpmProtocolGuid;
/** @endcond */

/** @ingroup efi_tree_constants
  Protocol version. 
*/
#define TrEE_REVISION 0x00010000

/** @addtogroup efi_tree_data_types 
@{ */
/** TrEE version. */
typedef struct _TREE_VERSION
{
  UINT8 Major;  /**< Major version. */
  UINT8 Minor;  /**< Minor version. */
} /** @cond */TREE_VERSION/** @endcond */;

/** Physical address. */
//typedef UINT64 EFI_PHYSICAL_ADDRESS;
/** Event log bitmap. */
typedef UINT32 TREE_EVENT_LOG_BITMAP;
/** Event log format. */
typedef UINT32 TREE_EVENT_LOG_FORMAT;
/** @} */ /* end_addtogroup efi_tree_data_types */

/** @addtogroup efi_tree_constants 
@{ */

/** Event log format TCG 1.2. */
#define TREE_EVENT_LOG_FORMAT_TCG_1_2 0x00000001

/** Event log format TCG 1.2. */
#define TREE_EVENT_LOG_FORMAT_TCG_2   0x00000002

/** Boot hash algorithm SHA1. */
#define TREE_BOOT_HASH_ALG_SHA1   0x00000001
/** Boot hash algorithm SHA256. */
#define TREE_BOOT_HASH_ALG_SHA256 0x00000002
/** Boot hash algorithm SHA384. */
#define TREE_BOOT_HASH_ALG_SHA384 0x00000004
/** Boot hash algorithm SHA512. */
#define TREE_BOOT_HASH_ALG_SHA512 0x00000008

/** Maximum event size. */
#define MAX_EVENT_SIZE 256
/** @} */ /* end_addtogroup efi_tree_constants */

/** @addtogroup efi_tree_data_types 
@{ */
/** PCR index. */
typedef UINT32 TrEE_PCRINDEX;
/** Event type. */
typedef UINT32 TrEE_EVENTTYPE;

/** Boot service capability. */
typedef struct _TREE_BOOT_SERVICE_CAPABILITY
{
  UINT8                 Size;
  /**< Size. */
  TREE_VERSION          StructureVersion;
  /**< Structure version; see #_TREE_VERSION for details. */
  TREE_VERSION          ProtocolVersion;
  /**< Protocol version; see #_TREE_VERSION for details. */
  UINT32                HashAlgorithmBitmap;
  /**< Hash algorithm bitmap. */
  TREE_EVENT_LOG_BITMAP SupportedEventLogs;
  /**< Supported event logs. */
  BOOLEAN               TrEEPresentFlag;
  /**< Flag indicating whether or not TrEE is present. */
  UINT16                MaxCommandSize;
  /**< Maximum command size. */
  UINT16                MaxResponseSize;
  /**< Maximum response size. */
  UINT32                ManufacturerID;
  /**< Manufacturer ID. */
  UINT32                NumberOfPcrBanks;
  /**< One only, for SHA256. */
  UINT32                ActivePcrBanks;
  /**< One only, for SHA256. */
} TREE_BOOT_SERVICE_CAPABILITY;
/** @} */ /* end_addtogroup efi_tree_data_types */

/** @addtogroup efi_tree_constants 
@{ */
/** This bit is set when an event is extended but not logged. */
#define TREE_EXTEND_ONLY 0x0000000000000001
/** This bit is set when the intent is to measure a 
  Portable Executable/Common Object File Format (PE/COFF) image. 
*/
#define PE_COFF_IMAGE    0x0000000000000010
/** @} */ /* end_addtogroup efi_tree_constants */

/** @addtogroup efi_tree_data_types 
@{ */
/** TrEE event header. */
#pragma pack(1)
typedef struct _TrEE_EVENT_HEADER
{
  UINT32         HeaderSize;    /**< Header size. */
  UINT16         HeaderVersion; /**< Header version. */
  TrEE_PCRINDEX  PCRIndex;      /**< Platform Configuration Register (PCR) 
                                     index. */
  TrEE_EVENTTYPE EventType;     /**< Event type. */
} /** @cond */TrEE_EVENT_HEADER/** @endcond */;

/** TrEE event information. */
typedef struct _TrEE_EVENT 
{
  UINT32            Size;     /**< Size. */
  TrEE_EVENT_HEADER Header;   /**< Header; see #_TrEE_EVENT_HEADER for 
                                   details. */
  UINT8             Event[1]; /**< Event. */
} TrEE_EVENT;
#pragma pack()
/** @} */ /* end_addtogroup efi_tree_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_TREE_GET_CAPABILITY */
/** @ingroup efi_tree_get_capability
  @par Summary
  Queries the TrEE capability.

  @param[in]     This                Pointer to the EFI_TREE_PROTOCOL instance.
  @param[in,out] ProtocolCapability  Pointer to a capability buffer where
                                     the capability structure is located; 
                                     see #_TREE_BOOT_SERVICE_CAPABILITY for 
                                     details. This buffer is filled if the call 
                                     is successful.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed. \n
  EFI_BUFFER_TOO_SMALL  -- Passed capability buffer is too small.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_CAPABILITY)( 
  IN EFI_TREE_PROTOCOL                 *This, 
  IN OUT TREE_BOOT_SERVICE_CAPABILITY  *ProtocolCapability
);

/* EFI_TREE_GET_EVENT_LOG */
/** @ingroup efi_tree_get_event_log
  @par Summary
  Queries the TrEE event log.

  @param[in]  This               Pointer to the EFI_TREE_PROTOCOL instance.
  @param[in]  EventLogFormat     Event log format.
  @param[out] EventLogLocation   Pointer to the event log location.
  @param[out] EventLogLastEntry  Pointer to the start of the last event entry.
  @param[out] EventLogTruncated  Flag indicating whether or not the event log 
                                 is truncated.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_EVENT_LOG)(
  IN EFI_TREE_PROTOCOL       *This,
  IN TREE_EVENT_LOG_FORMAT    EventLogFormat, 
  OUT EFI_PHYSICAL_ADDRESS   *EventLogLocation,
  OUT EFI_PHYSICAL_ADDRESS   *EventLogLastEntry,
  OUT BOOLEAN                *EventLogTruncated
);

/* EFI_TREE_HASH_LOG_EXTEND_EVENT */
/** @ingroup efi_tree_hash_log_extend_event
  @par Summary
  Hashes, logs, and extends the event. 

  @param[in]  This           Pointer to the EFI_TREE_PROTOCOL instance.
  @param[in]  Flags          Bitmap indicating extend-only and data format.
  @param[in]  DataToHash     Start address of the data to be hashed.
  @param[in]  DataToHashLen  Length of the data to be hashed.
  @param[in]  Event          Pointer to the event to be logged and 
                             extended; see #_TrEE_EVENT for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed. \n
  EFI_DEVICE_ERROR         Secure Channel Manager call failed. \n
  EFI_VOLUME_FULL       -- Failed to log in an event due to log is full, 
                           extend is successful.
*/
typedef
EFI_STATUS
(EFIAPI * EFI_TREE_HASH_LOG_EXTEND_EVENT)(
  IN EFI_TREE_PROTOCOL      *This,
  IN UINT64                  Flags,
  IN EFI_PHYSICAL_ADDRESS    DataToHash,
  IN UINT64                  DataToHashLen,
  IN TrEE_EVENT             *Event
);

/* EFI_TREE_SUBMIT_COMMAND */
/** @ingroup efi_tree_submit_command
  @par Summary
  Submits a command to the firmware Trusted Platform Module (TPM). This 
  function assumes the caller knows the command stream and how to read the 
  output block. 

  @param[in]  This                      Pointer to the EFI_TREE_PROTOCOL instance.
  @param[in]  InputParameterBlockSize   Size of the input parameter block.
  @param[in]  InputParameterBlock       Pointer to the input parameter block.
  @param[in]  OutputParameterBlockSize  Size of the output parameter block.
  @param[in]  OutputParameterBlock      Pointer to the output parameter block.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed. \n
  EFI_DEVICE_ERROR      -- Secure Channel Manager call failed.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_SUBMIT_COMMAND)(
  IN EFI_TREE_PROTOCOL      *This, 
  IN UINT32                  InputParameterBlockSize,
  IN UINT8                  *InputParameterBlock,
  IN UINT32                  OutputParameterBlockSize,
  IN UINT8                  *OutputParameterBlock
);

/* EFI_TREE_GET_CURRENT_ACTIVE_PCR_BANKS */
/** @ingroup efi_tree_get_current_active_pcr_banks
  @par Summary
  The EFI_TREE_PROTOCOL GetCurrentActivePCRBanks function provides information
  about the active platform PCRs. In addition if a request has been made in this 
  boot sequence for changing the active PCRs but will be in effect at the next 
  boot, that information is also returned.

  @param[in]    This              An EFI_TREE_PROTOCOL instance.
  @param[out]   ActivePcrBanks    Pointer to the variable receiving the bitmap of currently active PCR banks.

  @return       
  EFI_SUCCESS           -- Success.
  EFI_INVALID_PARAMETER -- Null pointer passed in

**/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_CURRENT_ACTIVE_PCR_BANKS)(
  IN  EFI_TREE_PROTOCOL*  This,
  OUT UINT32*     ActivePcrBanks
);

/* EFI_TREE_SET_CURRENT_ACTIVE_PCR_BANKS */
/** @ingroup efi_tree_set_current_active_pcr_banks
  @par Summary
  The EFI_TREE_PROTOCOL SetCurrentActivePCRBanks function call makes the request 
  to update the PCR banks that are to be used. This will get into effect at the 
  next platform reboot. 
  
  @param[in]  This                An EFI_TREE_PROTOCOL instance.
  @param[in]  ActivePcrBanks      Bitmap of the requested active PCR banks.
  
  @return     
  EFI_SUCCESS           -- Success.
  EFI_INVALID_PARAMETER -- Null pointer passed in

**/
typedef
EFI_STATUS
(EFIAPI *EFI_TREE_SET_CURRENT_ACTIVE_PCR_BANKS)(
  IN EFI_TREE_PROTOCOL*   This,
  IN UINT32       ActivePcrBanks
);



/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_tree_protocol
  @par Summary
  Trusted Execution Environment (TrEE) Protocol interface. 

  @par Parameters
  @inputprotoparams{tree_proto_params.tex} 

  @par struct EFI TPM PROTOCOL
  Trusted Platform Module Protocol interface. 
  @par
  This protocol is an adjunct to the TrEE Protocol.

  @par GUID
@verbatim
extern EFI_GUID gEfiTpmProtocolGuid;
@endverbatim

  @vertspace{2}
  @par Protocol interface structure
@verbatim
typedef struct _EFI_TPM_PROTOCOL { 
  EFI_TPM_GET_APPLICATION_ID     GetApplicationId;
  EFI_TPM_SET_CONTROL_AREA       SetControlArea;
  EFI_TPM_FLUSH_DISABLE          FlushDisable;
} EFI_TPM_PROTOCOL;
@endverbatim

  @vertspace{2}
  @par Parameters
  @inputprotoparams{tree_tpm_proto_params.tex} 
*/
extern struct _EFI_TREE_PROTOCOL
{ 
  EFI_TREE_GET_CAPABILITY        GetCapability;
  EFI_TREE_GET_EVENT_LOG         GetEventLog; 
  EFI_TREE_HASH_LOG_EXTEND_EVENT HashLogExtendEvent; 
  EFI_TREE_SUBMIT_COMMAND        SubmitCommand;
  EFI_TREE_GET_CURRENT_ACTIVE_PCR_BANKS       GetCurrentActivePCRBanks;
  EFI_TREE_SET_CURRENT_ACTIVE_PCR_BANKS       SetCurrentActivePCRBanks;
} EFI_QCOM_TREE_PROTOCOL;

/*===========================================================================
  TPM FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_TPM_GET_APPLICATION_ID */
/** @ingroup efi_tpm_tree_get_application_id
  @par Summary
  Gets the WinSecApp application ID.

  @param[in]  This           Pointer to the EFI_TPM_PROTOCOL instance.
  @param[out] ApplicationId  Application ID.

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed.

*/
typedef
EFI_STATUS
(EFIAPI *EFI_TPM_GET_APPLICATION_ID)( 
  IN  EFI_TPM_PROTOCOL      *This, 
  OUT UINT32                *ApplicationId
);

/* EFI_TPM_SET_CONTROL_AREA */
/** @ingroup efi_tpm_tree_set_control_area
  @par Summary
  Sets the TPM's control area.

  @param[in]  This           Pointer to the EFI_TPM_PROTOCOL instance.
  @param[in]  ControlArea    Address of the control area.

  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed.

*/
typedef
EFI_STATUS
(EFIAPI *EFI_TPM_SET_CONTROL_AREA)( 
  IN  EFI_TPM_PROTOCOL      *This, 
  IN  PHYSICAL_ADDRESS      ControlArea
);

/* EFI_TPM_FLUSH_DISABLE */
/** @ingroup efi_tpm_flush_disable
  @par Summary
  Disables NV flushing.

  @param[in]  This           Pointer to the EFI_TPM_PROTOCOL instance.
  
  @return 
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- NULL parameter was passed.

*/
typedef
EFI_STATUS
(EFIAPI *EFI_TPM_FLUSH_DISABLE)( 
  IN  EFI_TPM_PROTOCOL      *This  
);


/* TPM Protocol; structure is documented with TrEE Protocol above */
/** @cond */
struct _EFI_TPM_PROTOCOL
{ 
  EFI_TPM_GET_APPLICATION_ID     GetApplicationId;
  EFI_TPM_SET_CONTROL_AREA       SetControlArea;
  EFI_TPM_FLUSH_DISABLE          FlushDisable;    
};
/** @endcond */

#endif /* _EFITREE_H_ */

