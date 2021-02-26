/** @file OfflineCrashDump.h
   
  Offline Crash Dump Definitions

  Copyright (c) 2016 by Qualcomm Technologies, Inc. All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/24/16   ly      Update PMIC register addresses and reset buffer size 
 08/11/16   bh      Move cookie logic to new lib
 06/28/16   na      Remove second parameter from ClearDLOADCookieRuntime()
 06/15/16   bd      Remove offlinecrashdump library dependency from QcomLib 
 06/25/15   bh      Change guid name
 05/27/15   ao      increase crash dump TZ buffer, QCDATA_1_SIZE 
 10/22/14   aus     Added support for EDL mode
 10/03/14   na      Fixing size of input address for 64-bit
 07/09/14  shireesh Added pmic PON_PBL_STATUS register to reset data buffer
 05/16/14  shireesh Added support for reset reason feature
 03/12/14   aus     Mass storage mode support
 05/10/13   niting  Add IsMemoryDumpEnabled
 05/09/13   vk      Add GetSavedMemoryCaptureMode
 04/03/13   niting  Added offline crash dump support
=============================================================================*/
#ifndef __OFFLINECRASHDUMP_H__
#define __OFFLINECRASHDUMP_H__

#include <Uefi.h>

extern EFI_GUID gEfiOfflineCrashDumpConfigTableGuid;
extern EFI_GUID gOSAVendorGuid;
extern EFI_GUID gQcomMemoryCaptureGuid;
extern EFI_GUID gQcomMemoryCaptureValueGuid;
extern EFI_GUID gQcomAbnormalResetOccurredValueGuid;
extern EFI_GUID gQcomResetReasonDataCaptureGuid;


// Offset of PON_WARM_RESET_REASON1 register in PMIC


#define PON_PBL_STATUS                 0x807
#define PON_PON_REASON1                0x8C0	
#define PON_WARM_RESET_REASON1         0x8C2
#define PON_ON_REASON                  0x8C4
#define PON_POFF_REASON1               0x8C5
#define PON_OFF_REASON                 0x8C7
#define PON_FAULT_REASON1              0x8C8
#define PON_FAULT_REASON2              0x8C9
#define PON_S3_RESET_REASON            0x8CA
#define PON_SOFT_RESET_REASON1         0x8CB


#define MEMORYDUMP_DISABLED  0x0
#define MEMORYDUMP_ENABLED   0x1
#define RESETREASON_ENABLED  (0x1 << 2)
#define DLOAD_MODE_BIT_MASK  0x10

// Bit 2 of OfflineMemoryDumpCapable indicate device capable of collecting reset reason data
#define MEMORYDUMP_RESETREASON_ENABLED   ( 1 << 0x2 )

/* comment below line for no debug */
// #define ENABLE_OCD_DEBUG

#define OFFLINE_CRASHDUMP_CONFIG_TABLE_VERSION 0x00000002
#define OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME L"OfflineMemoryDumpUseCapability"
#define OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARSIZE         0x1 /* 1 byte */
#define OFFLINE_MEMORY_DUMP_USE_CAPABILITY_ATTR \
        (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define OFFLINE_MEMORY_DUMP_USE_CAPABILITY_GPT_BMSK        0x1
#define OFFLINE_MEMORY_DUMP_USE_RESETDATA_CAPABILITY_BMSK  (0x1 << 1)

#define OFFLINE_MEMORY_DUMP_USE_CAPABILITY_BYTEOFFSET_BMSK 0x2

#define ENABLE_LEGACY_SBL_DUMP_VARNAME L"EnableLegacySBLDump"
#define ENABLE_LEGACY_SBL_DUMP_VARSIZE 0x1 /* 1 byte */

#define AP_REG_ADDR_VARNAME L"APRegAddress"
#define AP_REG_ADDR_VARSIZE 0x8 /* 8 bytes */

#define FORCE_CRASH_STRING L".reboot"
#define MASS_STORAGE_STRING     L"MassStorage"

///
/// Definition for Offline Crash Dump Configuration Table
///
typedef struct {
  ///
  /// The version of this table definition.  This should be set to 1.
  ///
  UINT32                Version;
  ///
  /// Inidicates if an abnormal reset occurred on the most recent system boot.
  /// Value 0       The most recent boot was normal.
  /// Bit 0         Most recent reset of system was abnormal, reset source unknown.
  /// Bits 1:31     Reserved additional sources to trigger resets. Must be zero.
  ///
  UINT32                AbnormalResetOccurred;
  ///
  /// A bitmask that describes the capability to create a dump of memory after a hard hang.
  /// Value 0       Firmware does not support dump.
  /// Bit 0         Firmware supports the access of a dump location via scanning GPT for a dedicated partition.
  /// Bit 1         Firmware supports access of the dump location via byte offset.
  /// Bits 2:31	    Reserved for future use.  Must be zero.
  ///
  UINT32                OfflineMemoryDumpCapable;
  ///
  /// Address of RESET_DATA_HEADER
  ///
  UINT64                ResetDataAddress; 
  ///
  /// Size of RESET_DATA_HEADER
  ///
  UINT64                ResetDataSize;
   
} EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE;

///
/// Definition for ResetDataBuffer
///
#define RESET_DATA_BUFFER_VERSION 0x1
//TZ diag buffer size 
#define QCDATA_1_SIZE             12*(1024)
//UEFI reset buffer
#define QCDATA_2_SIZE             2*(1024)
#define RESET_DATA2_VERSION 0x1   // RESET_DATA2_VERSION is filled as first field in QCData2Buffer
typedef struct {
  // Version of this reset data buffer
  UINT64                Version;
  
  // Offset from start of this structure to QC_DATA1 buffer
  UINT64                DataOffset1;
  
  // Size of QC-DATA1 in bytes
  UINT64                DataSize1;
  
  // Offset from start of this structure to QC_DATA2 buffer
  UINT64                DataOffset2;
  
  // Size of QC-DATA2 in bytes
  UINT64                DataSize2;
  
  // QC_DATA1 buffer ( Data from TZ diag)
  UINT8                 QCData1Buffer[QCDATA_1_SIZE];
  
  // QC_DATA2 buffer ( populated by UEFI)
  UINT8                 QCData2Buffer[QCDATA_2_SIZE];
} RESET_DATA_BUFFER;

#define RESET_DATA_HEADER_VERSION 0x1 
#define PLATFORMID_MAX_LENGTH     16
typedef struct {
  // Version of this reset data header
  UINT64                Version;

  // Platform ID from chip info - Ascii string
  CHAR8                 PlatformID[PLATFORMID_MAX_LENGTH];

  // Guid identifying reset data buffer (gQcomResetReasonDataCaptureGuid)
  EFI_GUID              Guid;

  // Size of reset data buffer
  UINT64                ResetDataBufferSize;
  
  // Reset data buffer
  RESET_DATA_BUFFER     ResetDataBuffer;
} RESET_DATA_HEADER;

/** Get/Set/Validity APIs for Memory Capture Mode */
#define OFFLINE_CRASH_DUMP_DISABLE 0x0
#define OFFLINE_CRASH_DUMP_LEGACY_ENABLE 0x1
#define OFFLINE_CRASH_DUMP_ENABLE 0x2

EFI_STATUS
EFIAPI
GetMemoryCaptureMode(
  UINT32 *MemoryCaptureMode
  );

EFI_STATUS
EFIAPI
GetMemoryCaptureModeRuntime(
  UINTN MemoryCaptureModeAddr,
  UINT32 *MemoryCaptureMode
  );

EFI_STATUS
SetMemoryCaptureMode(
  UINT32 Value
  );

EFI_STATUS
EFIAPI
SetMemoryCaptureModeRuntime(
  UINTN MemoryCaptureModeAddr,
  UINT32 Value
  );

BOOLEAN
EFIAPI
IsMemoryCaptureModeValid(
  UINT32 MemoryCaptureMode
  );

UINT32
EFIAPI
GetSavedMemoryCaptureMode ( VOID );

/** Get/Set/Validity APIs for Abnormal Reset Occurred */
#define ABNORMAL_RESET_DISABLE 0x0
#define ABNORMAL_RESET_ENABLE 0x1

EFI_STATUS
EFIAPI
GetAbnormalResetOccurred(
  UINT32 *AbnormalResetOccurred
  );

EFI_STATUS
SetAbnormalResetOccurred(
  UINT32 Value
  );

EFI_STATUS
EFIAPI
SetAbnormalResetOccurredRuntime(
  UINTN AbnormalResetOccurredAddr,
  UINT32 Value
  );

BOOLEAN
EFIAPI
IsAbnormalResetOccurredValid(
  UINT32 AbnormalResetOccurred
  );

/** Sets up APRegAddress UEFI variable */
EFI_STATUS
EFIAPI
SetAPRegAddress(UINT64 Address);

/**
 InCarveoutMode - Check if the device is in the carveout mode (memory capture mode)
**/
EFIAPI
BOOLEAN
InCarveoutMode(VOID);

/** Updates the Offline Crash Dump Configuration Table in the System Table based
 *  on the state of the cookies */
EFI_STATUS
EFIAPI
UpdateOfflineCrashDumpConfigTable( VOID );

/** This function initializes reset data buffers */
EFI_STATUS
EFIAPI
InitializeResetReasonDataBuffer( VOID );

/** Prints out values corresponding to offline crash dump for debug */
VOID
EFIAPI
PrintOfflineCrashDumpValues ( VOID );

/** Prints out values corresponding to offline crash dump for debug */
VOID
EFIAPI
PrintOfflineCrashDumpValuesDxe ( VOID );

/** Checks whether TZ memory dump is enabled (TRUE) or disabled (FALSE) */
EFIAPI
BOOLEAN
IsMemoryDumpEnabled( VOID );

/** Checks whether offline crash dump is enabled (TRUE) or disabled (FALSE) */
EFIAPI
BOOLEAN
IsOfflineCrashDumpEnabled( VOID );

/** sets up HOBs for crash dumps */
VOID
EFIAPI
HandleCrashDump ( VOID );

/** Enter EDL mode */
void EnterEDLMode ( VOID );

#endif /* __OFFLINECRASHDUMP_H__ */
