/*============================================================================
  @file WINACPIUPDATE_H

AcpiPlatfrom Callback functions for chipinfo 
               Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
               
               
============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/4/16   vahidd   Intial Draft for Patching MADT tables.
=============================================================================*/


#ifndef WINACPIUPDATE_H
#define WINACPIUPDATE_H

/**
Device node information.
*/
#define EFI_DEVICE_FRONT_CAMERA "\\_SB.CAMF._STA"
#define EFI_DEVICE_BACK_CAMERA "\\_SB.CAMS._STA"
#define EFI_DEVICE_BT "\\_SB.BTH0._STA"
#define EFI_DEVICE_AUDIO "\\_SB.ADSP._STA"
#define EFI_DEVICE_MODEM "\\_SB.AMSS._STA"
#define EFI_DEVICE_USB "\\_SB.URS0.USB0._STA"
#define EFI_DEVICE_WLAN "WLEN"
#define EFI_DEVICE_SENSORS "\\_SB.SCSS._STA"
#define EFI_DEVICE_GPU "\\_SB.GPU0._STA"
#define EFI_DEVICE_QCCDI "\\_SB.CDI._STA"

char * DEVICE_LIST[EFI_FEATURE_MAX] = { 
	EFI_DEVICE_FRONT_CAMERA,
	EFI_DEVICE_BACK_CAMERA,
	EFI_DEVICE_BT,
	EFI_DEVICE_AUDIO,
	EFI_DEVICE_MODEM,
	EFI_DEVICE_USB,
	EFI_DEVICE_SENSORS,
	EFI_DEVICE_GPU,
	EFI_DEVICE_WLAN,
	EFI_DEVICE_QCCDI
};

/**
UEFI Variable information
*/
#define FEATURE_ENABLE_VAR_NAME L"FeatureEnableBitmask"

#define FEATURE_ENABLE_VAR_GUID \
  { \
    0x68f38861, 0x1414, 0x439b, { 0x94, 0xa0, 0xb2, 0x8, 0xfd, 0xe5, 0xe, 0x12 } \
  }

STATIC EFI_GUID   FeatureEnableVarGuid = FEATURE_ENABLE_VAR_GUID;

/**
Global variables
*/
static EFI_ACPI_SDT_PROTOCOL				*gAcpiSdt = NULL;
static EFI_QCOM_ACPIPLATFORM_PROTOCOL 		*pEfiAcpiPlatformProtocol = NULL;
static EFI_CHIPINFO_PROTOCOL 				*pChipInfoProtocol = NULL;

static UINT32								 gfeatureBitmask = 0x0;
static UINT32 								 cluster_num = 0x0;
static UINT32                                defectiveCPUsBitMask = 0x0;
static UINT32                                defectivePartsBitMask = 0x0;

// Needs re-visit for future chipsets. This should be obtained from ChipInfo instead of hard-coding as below
#define NUM_CPU 8
#define PLGF_ENABLED_OFFSET_IN_BYTES ((sizeof(ACPI_MADT_DESCRIPTION_HEADER)+ (12))

#pragma pack (1)

/* ACPI v 5.x*/
typedef struct
{
	UINT32  Signature;
	UINT32  Length;
	UINT8   Revision;
	UINT8   Checksum;
	UINT8   OEMID[6];
	UINT64  OEMTableID;
	UINT32  OEMRevision;
	UINT32  CreatorID;
	UINT32  CreatorRevision;
} ACPI_HEADER;

typedef struct {
	ACPI_HEADER                 Header;
	UINT32                      LocalApicAddress;
	UINT32                      Flags;
} APIC_DESCRIPTION_TABLE_HEADER;

typedef struct _ACPI_PROCESSOR_LOCAL_APIC_STRUCTURE   {
	UINT8 Type;
	UINT8 Length;
	UINT16 Reserved;
	UINT32 Identifier;
	UINT32 AcpiProcessorId;
	UINT32 Flags;
	UINT32 ParkingProtocolVersion;
	UINT32 PerformanceInterruptGsi;
	UINT64 MailboxPhysicalAddress;
	UINT64 ControllerPhysicalAddress;
	UINT64 GICVirtual;                      // GIC virtual CPU interface registers.
	UINT64 GICH;                            // GIC virtual interface control block registers.
	UINT32 VGICMaintenanceInterrupt;        // GVIS for Virtual GIC maintenance interrupt.
	UINT64 GICRedistributorBaseAddress;     // 64-bit address of the GIC Redistributor.
	UINT64 MPIDR;
	UINT8  ProcessorPowerEfficiencyClass;   // added towards ACPI 6 
	UINT8  Reserved2[3];
} ACPI_PROCESSOR_LOCAL_APIC_STRUCTURE;


typedef struct _ACPI_GIC_DISTRIBUTOR_STRUCTURE  {
	UINT8 Type;
	UINT8 Length;
	UINT16 Reserved1;
	UINT32 Identifier;
	UINT64 ControllerPhysicalAddress;
	UINT32 GsivBase;
	UINT8  GicVersion;
	UINT32 Reserved;
} ACPI_GIC_DISTRIBUTOR_STRUCTURE;

typedef struct _ACPI_GIC_REDISTRIBUTOR_STRUCTURE  {
	UINT8 Type;
	UINT8 Length;
	UINT16 Reserved;
	UINT64 GICRedistributorPhysicalBaseAddress;
	UINT32 GICRDiscoveryRangeLength;
} ACPI_GIC_REDISTRIBUTOR_STRUCTURE;


typedef struct _ACPI_GIC_MSI_FRAME_STRUCTURE  {
	UINT8 Type;
	UINT8 Length;
	UINT16 Reserved1;
	UINT32 Identifier;
	UINT64 ControllerPhysicalAddress;
	UINT32 Flags;
	UINT16 SPI_Count;
	UINT16 SPI_Base;
} ACPI_GIC_MSI_FRAME_STRUCTURE;

typedef struct {
	APIC_DESCRIPTION_TABLE_HEADER                         Header;
	ACPI_PROCESSOR_LOCAL_APIC_STRUCTURE                   LocalGic[NUM_CPU];
	ACPI_GIC_DISTRIBUTOR_STRUCTURE                        Distributor;
	ACPI_GIC_REDISTRIBUTOR_STRUCTURE                      ReDistributor;
	ACPI_GIC_MSI_FRAME_STRUCTURE                          MSIFrame0;
} ACPI_MULTIPLE_APIC_DESCRIPTION_TABLE;


#pragma pack ()

#endif
