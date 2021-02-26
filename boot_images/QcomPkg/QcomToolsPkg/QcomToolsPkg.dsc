#/** @file QcomToolsPkg.dsc
# QcomTools package.
#
# Copyright (c) 2016 Qualcomm Technologies, Inc. All rights reserved.
# Portions Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#**/
#
#==============================================================================

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = QcomToolsPkg
  PLATFORM_GUID                  = 57D20271-12AD-46D8-960D-46FA7C24F41C
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/QcomTools
  SUPPORTED_ARCHITECTURES        = ARM|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = QcomPkg/QcomToolsPkg/QcomToolsPkg.fdf

[LibraryClasses.common]
  #
  # EDK2 Libraries
  #
  ArmDisassemblerLib|ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf  
  ArmGicArchLib|ArmPkg/Library/ArmGicArchLib/ArmGicArchLib.inf
  ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|ArmPkg/Library/BaseMemoryLibStm/BaseMemoryLibStm.inf
  BaseSynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  BdsLib|ArmPkg/Library/BdsLib/BdsLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  CpuExceptionHandlerLib|ArmPkg/Library/ArmExceptionLib/ArmExceptionLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  EblAddExternalCommandLib|EmbeddedPkg/Library/EblAddExternalCommandLib/EblAddExternalCommandLib.inf
  EblNetworkLib|EmbeddedPkg/Library/EblNetworkLib/EblNetworkLib.inf
  EfiFileLib|EmbeddedPkg/Library/EfiFileLib/EfiFileLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  LzmaDecompressLib|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  ZlibDecompressLib|QcomPkg/Library/ZlibDecompressLib/ZlibDecompressLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrePiLib|EmbeddedPkg/Library/PrePiLib/PrePiLib.inf
  GenericBdsLib|QcomPkg/Library/GenericBdsLib/GenericBdsLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UncachedMemoryAllocationLib|ArmPkg/Library/UncachedMemoryAllocationLib/UncachedMemoryAllocationLib.inf
  
  #
  # Qualcomm target independent libraries
  #
  NULL|QcomPkg/Library/QcomBaseLib/QcomBaseLib.inf
  NULL|QcomPkg/Library/StackCanaryBaseLib/StackCanaryBaseLibCore.inf
  # MuslLib: POSIX string APIs
  NULL|QcomPkg/Library/MuslLib/MuslLib.inf
  MuslLib|QcomPkg/Library/MuslLib/MuslLib.inf
  UefiCfgLib|QcomPkg/Library/UefiConfigLib/UefiCfgLib.inf
  # Shared library instace for clients usage
  ShLib|QcomPkg/Library/ShLib/ShLib.inf
  RamPartitionTableLib|QcomPkg/Library/RamPartitionTableLib/RamPartitionTableLib.inf
  PeCoffExtraActionLib|QcomPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf

  AptLib|QcomPkg/Library/NullLibs/AptLibNull/AptLibNull.inf
  BootConfigLib|QcomPkg/Library/NullLibs/BootConfigLibNull/BootConfigLibNull.inf
  BusyWaitLib|QcomPkg/Library/BusyWaitLib/BusyWaitLib.inf
  ChargerLib|QcomPkg/Library/ChargerLib/ChargerLibLA.inf
  ChipInfoCoreLib|QcomPkg/Library/ChipInfoLib/ChipInfoCore.inf
  DDRTargetLib|QcomPkg/Library/NullLibs/DDRTargetLibNull/DDRTargetLibNull.inf
  EblCmdLib|QcomPkg/Library/EblCmdLib/EblCmdLib.inf
  EfiResetSystemLib|QcomPkg/Library/ResetSystemLib/LA/ResetSystemLib.inf
  FBPTLib|QcomPkg/Library/FBPTLib/FBPTLib.inf
  FuseControlLib|QcomPkg/Library/FuseControlLib/FuseControlLib.inf
  FwCommonLib|QcomPkg/Library/NullLibs/FwCommonLibNull/FwCommonLibNull.inf
  FwProvisionLib|QcomPkg/Library/NullLibs/FwCommonLibNull/FwCommonLibNull.inf
  FwUpdateLib|QcomPkg/Library/NullLibs/FwCommonLibNull/FwCommonLibNull.inf
  GuidedFvLib|QcomPkg/Library/GuidedFvLib/GuidedFvLib.inf
  HALclkSharedLib|QcomPkg/Library/HALclkSharedLib/HALclkSharedLibUEFI.inf
  HFATLib|QcomPkg/Library/HFATLib/HFATLib.inf
  HmacLib|QcomPkg/Library/HmacLib/HmacLib.inf
  ColdplugEfsLib|QcomPkg/Library/ColdplugLib/ColdplugEfsLib.inf
  ColdplugLib|QcomPkg/Library/ColdplugLib/ColdplugLib.inf
  IcbTargetLib|QcomPkg/Library/NullLibs/BusesLibNull/IcbCfgLibNull.inf
  ImageVersionLib|QcomPkg/Library/ImageVersionLib/ImageVersionLib.inf
  PIMEMTargetLib|QcomPkg/Library/NullLibs/PIMEMTargetLibNull/PIMEMTargetLibNull.inf
  ParserLib|QcomPkg/Library/ParserLib/ParserLib.inf
  PeHashLib|QcomPkg/Library/PeHashLib/PeHashLib.inf
  PlatformInfoCoreLib|QcomPkg/Library/PlatformInfoLib/PlatformInfoCore.inf
  PmicApiLib|QcomPkg/Library/PmicApiLib/PmicApiLib.inf
  PrintLib|QcomPkg/Library/PrintShLib/PrintShLib.inf
  ProcLib|QcomPkg/Library/ProcLib/ProcLib.inf
  QcomBaseLib|QcomPkg/Library/QcomBaseLib/QcomBaseLib.inf
  QcomLib|QcomPkg/Library/QcomLib/QcomLib.inf
  QcomUtilsLib|QcomPkg/Library/QcomUtilsLib/QcomUtilsLib.inf
  QusbLib|QcomPkg/Library/QusbLib/QusbLib.inf
  QcomBdsLib|QcomPkg/Library/QcomBdsLib/QcomBdsLib.inf
  RFSLib|QcomPkg/Library/RFSLib/RFSLib.inf
  ROMFSLib|QcomPkg/Library/EfsLib/ROMFSLib.inf
  RandomStackCanaryLib|QcomPkg/Library/NullLibs/RandomStackCanaryLibNull/RandomStackCanaryLibNull.inf
  RealTimeClockLib|QcomPkg/Library/RealTimeClockLibLegacy/RealTimeClockLib.inf
  RpmbCgLib|QcomPkg/Library/RpmbCgLib/RpmbCgLib.inf
  RpmbLib|QcomPkg/Library/RpmbLib/RpmbLib.inf
  RpmbListenerLib|QcomPkg/Library/RpmbLib/RpmbListenerLib.inf
  ScmCmdLib|QcomPkg/Library/ScmCmdLib/ScmCmdLib.inf
  SdccCommonLib|QcomPkg/Library/SdccCommonLib/SdccCommonLib.inf
  SdccHalLib|QcomPkg/Library/SdccCommonLib/SdccHalLib/SdccHalLib.inf
  SecBootAuthLib|QcomPkg/Library/NullLibs/SecBootAuthLibNull/SecBootAuthLibNull.inf
  SecBootBaseLib|QcomPkg/Library/SecBootBaseLib/SecBootBaseLib.inf
  SecBootLib|QcomPkg/Library/SecBootLib/SecBootLib.inf
  SecBootSigLib|QcomPkg/Library/SecBootSigLib/SecBootSigLib.inf
  SecCfgLib|QcomPkg/Library/SecCfgLib/SecCfgLib.inf
  SecHwTestLib|QcomPkg/Library/SecHwTestLib/SecHwTestLib.inf
  SecImgAuthTargetLib|QcomPkg/Library/NullLibs/SecImgAuthLibNull/SecImgAuthLibNull.inf
  SerialPortLib|QcomPkg/Library/SerialPortLib/SerialPortShLib.inf
  ServicesUtilsLib|QcomPkg/Library/ServicesLib/ServicesUtilsLib.inf
  SmBiosLib|QcomPkg/Library/SmBiosLib/SmBiosLib.inf
  SmemLib|QcomPkg/Library/SmemLib/build/SmemLibXBLCore.inf
  SpiNorCommonLib|QcomPkg/Library/SpiNorCommonLib/SpiNorCommonLib.inf
  TargetResetLib|QcomPkg/Library/NullLibs/TargetResetLibNull/TargetResetLib.inf
  TestInterfaceLib|QcomPkg/Library/TestInterfaceLib/TestInterfaceLib.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf
  TimerSleepLib|QcomPkg/Library/TimerSleepLib/TimerSleepLib.inf
  TimetickLib|QcomPkg/Library/TimetickLibB/TimetickLib.inf
  TimetickRunTimeLib|QcomPkg/Library/TimetickLibB/TimetickRunTimeLib.inf
  TlmmLib|QcomPkg/Library/TlmmLib/TlmmLib.inf
  ToolSupportLib|QcomPkg/Library/ToolSupportLib/ToolSupportLib.inf
  TzRngLib|QcomPkg/Library/TzRngLib/TzRngLib.inf
  TzRuntimeLib|QcomPkg/Library/TzRuntimeLib/TzRuntimeLib.inf
  UIELib|QcomPkg/Library/NullLibs/UIELibNull/UIELibNull.inf
  UfsCommonLib|QcomPkg/Library/UfsCommonLib/UfsCommonLib.inf
  UfsJtagLib|QcomPkg/Library/UfsCommonLib/UfsJtagLib.inf
  XBLSharedMemLib|QcomPkg/Library/XBLSharedMemLib/XBLSharedMemLib.inf

  # BAM Lib Library
  BamLib|QcomPkg/Library/BamLib/BamCoreLib.inf
  BamCoreLib|QcomPkg/Library/BamLib/BamCoreLib.inf

  # SPMI Common Libraries
  SPMIApiLib|QcomPkg/Library/SPMIApiLib/SPMIApiLib.inf
  SPMIRuntimeLib|QcomPkg/Library/SPMIRuntimeLib/SPMIRuntime.inf

  # ULog Libraries
  ULogApiLib|QcomPkg/Library/ULogApiLib/ULogApiLib.inf
  CoreUtilsLib|QcomPkg/Library/CoreUtilsLib/CoreUtilsLib.inf

  # DAL Libraries
  DALModEnvLib|QcomPkg/Library/DALModEnvLib/DALModEnvLib.inf
  DALCommonDxeLib|QcomPkg/Library/DALCommonLib/DALCommonDxeLib.inf
  DALFwkLib|QcomPkg/Library/DALFwkLib/DALFwkLib.inf

  # I2C Library
  I2CLib|QcomPkg/Library/I2CLib/I2CLib.inf
  I2CApiLib|QcomPkg/Library/I2CApiLib/I2CApiLib.inf
  SPILib|QcomPkg/Library/SPILib/SPILib.inf
  SPIApiLib|QcomPkg/Library/SPIApiLib/SPIApiLib.inf

  # NPA API Library
  NpaApiLib|QcomPkg/Library/NpaApiLib/NpaApiLib.inf

  #Qdss SW event library
  QdssLib|QcomPkg/Library/QdssLib/QdssLib.inf

  # ADC Libraries
  AdcDxeWrapperLib|QcomPkg/Library/AdcLib/build/AdcDxeWrapperLib.inf

  HWConfigLib|QcomPkg/Library/HWConfigLib/HWConfigLib.inf
  
  MountFvLib|QcomPkg/Library/MountFvLib/MountFvLib.inf
  MountFvUtilsLib|QcomPkg/Library/MountFvUtilsLib/MountFvUtilsLib.inf

[LibraryClasses.Arm]
  ArmLib|ArmPkg/Library/ArmLib/ArmV7/ArmV7Lib.inf

[LibraryClasses.AARCH64]
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64Lib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  OfflineCrashDumpLib|QcomPkg/Library/OfflineCrashDumpLib/OfflineCrashDumpDxeLib.inf
  CookieLib|QcomPkg/Library/CookieLib/CookieLib.inf

[BuildOptions.ARM]
  # to enable thumb we need to evaluate if size is more advantage or speed
  RVCT:*_*_ARM_ARCHCC_FLAGS  == --cpu Cortex-A53 --thumb --fpu vfpv3
  RVCT:*_*_ARM_ARCHASM_FLAGS == --cpu Cortex-A53

[BuildOptions.AARCH64]
  GCC:*_*_AARCH64_ARCHCC_FLAGS  == -fno-common -mtune=cortex-a53 -include $(WORKSPACE)/QcomPkg/Include/Library/DebugLib.h -DQCOM_EDK2_PATCH 
  GCC:*_*_AARCH64_ARCHPP_FLAGS  == -include $(WORKSPACE)/QcomPkg/Include/Library/DebugLib.h -DQCOM_EDK2_PATCH 

!if $(PRODMODE) == "PRODMODE"
  RVCT:*_*_*_CC_FLAGS = -DPRODMODE
  GCC:*_*_*_CC_FLAGS = -DPRODMODE
!endif


################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]
  gEfiMdePkgTokenSpaceGuid.PcdComponentNameDisable|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnosticsDisable|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdComponentName2Disable|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdDriverDiagnostics2Disable|TRUE

  #
  # Control what commands are supported from the UI
  # Turn these on and off to add features or save size
  #
  gEmbeddedTokenSpaceGuid.PcdEmbeddedDirCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedHobCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedHwDebugCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPciDebugCmd|TRUE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedIoEnable|FALSE
  gEmbeddedTokenSpaceGuid.PcdEmbeddedScriptCmd|FALSE

  gEmbeddedTokenSpaceGuid.PcdCacheEnable|TRUE

  gEmbeddedTokenSpaceGuid.PcdPrePiProduceMemoryTypeInformationHob|TRUE

  gEfiMdeModulePkgTokenSpaceGuid.PcdTurnOffUsbLegacySupport|TRUE

  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE

  gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|TRUE


  #
  # Target board Specific PCDs
  #

[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Qualcomm Technologies, Inc."
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"4.2"

  gEmbeddedTokenSpaceGuid.PcdEmbeddedPrompt|"EBL"
  gEmbeddedTokenSpaceGuid.PcdPrePiCpuMemorySize|48
  gEmbeddedTokenSpaceGuid.PcdPrePiCpuIoSize|0
  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|10000000
  gEfiMdePkgTokenSpaceGuid.PcdDebugClearMemoryValue|0xAF

  # Disable perf
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdPostCodePropertyMask|0
  gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|320

  # PcdDebugPropertyMask - From DebugLib.h:
  #   DEBUG_ASSERT_ENABLED       0x01
  #   DEBUG_PRINT_ENABLED        0x02
  #   DEBUG_CODE_ENABLED         0x04
  #   CLEAR_MEMORY_ENABLED       0x08
  #   ASSERT_BREAKPOINT_ENABLED  0x10
  #   ASSERT_DEADLOOP_ENABLED    0x20
!if $(PRODMODE) == "PRODMODE"
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2b
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2f
!endif

  # Pcd{Fixed}DebugPrintErrorLevel - From DebugLib.h:
  #   DEBUG_INIT      0x00000001  // Initialization
  #   DEBUG_WARN      0x00000002  // Warnings
  #   DEBUG_LOAD      0x00000004  // Load events
  #   DEBUG_FS        0x00000008  // EFI File system
  #   DEBUG_POOL      0x00000010  // Alloc & Free's
  #   DEBUG_PAGE      0x00000020  // Alloc & Free's
  #   DEBUG_INFO      0x00000040  // Verbose
  #   DEBUG_DISPATCH  0x00000080  // PEI/DXE Dispatchers
  #   DEBUG_VARIABLE  0x00000100  // Variable
  #   DEBUG_BM        0x00000400  // Boot Manager
  #   DEBUG_BLKIO     0x00001000  // BlkIo Driver
  #   DEBUG_NET       0x00004000  // SNI Driver
  #   DEBUG_UNDI      0x00010000  // UNDI Driver
  #   DEBUG_LOADFILE  0x00020000  // UNDI Driver
  #   DEBUG_EVENT     0x00080000  // Event messages
  #   DEBUG_GCD       0x00100000  // Global Coherency Database changes
  #   DEBUG_CACHE     0x00200000  // Memory range cachability changes
  #   DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may significantly impact boot performance
  #   DEBUG_ERROR     0x80000000  // Error
!if $(PRODMODE) == "PRODMODE"
  # Only enable Errors and Boot Manager if Production Mode Flag is set
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000400
  gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel|0x80000400
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x800fee0f
  gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel|0x800fee0f
!endif

  # PcdReportStatusCodePropertyMask - From ReportStatusCodeLib.h:
  #   REPORT_STATUS_CODE_PROPERTY_PROGRESS_CODE_ENABLED   0x00000001
  #   REPORT_STATUS_CODE_PROPERTY_ERROR_CODE_ENABLED      0x00000002
  #   REPORT_STATUS_CODE_PROPERTY_DEBUG_CODE_ENABLED      0x00000004
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x06

  gEmbeddedTokenSpaceGuid.PcdEmbeddedAutomaticBootCommand|""
  gEmbeddedTokenSpaceGuid.PcdEmbeddedDefaultTextColor|0x07
  gEmbeddedTokenSpaceGuid.PcdEmbeddedMemVariableStoreSize|0x10000

  gEmbeddedTokenSpaceGuid.PcdFlashFvMainBase|0
  gEmbeddedTokenSpaceGuid.PcdFlashFvMainSize|0

#
# Optional feature to help prevent EFI memory map fragments
# Turned on and off via: PcdPrePiProduceMemoryTypeInformationHob
# Values are in EFI Pages (4K). DXE Core will make sure that
# at least this much of each type of memory can be allocated
# from a single memory range. This way you only end up with
# maximum of two fragements for each type in the memory map
# (the memory used, and the free memory that was prereserved
# but not used).
#
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIReclaimMemory|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIMemoryNVS|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiReservedMemoryType|0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesData|80
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesCode|40
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesCode|400
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesData|800
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderCode|10
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderData|0

#
# SMBIOS related
#
  # OEM's can set their product info here
  gQcomTokenSpaceGuid.PcdSystemMfrStr|"Qualcomm"
  gQcomTokenSpaceGuid.PcdSystemProductNameStr|""
  gQcomTokenSpaceGuid.PcdSystemProductFamilyStr|""

#
# ACPI related
#
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"QCOM  "
  #Signature_64('Q','C','O','M','E','D','K','2') = 0x324B44454D4F4351 = '2','K','D','E','M','O','C','Q'
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x324B44454D4F4351
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemRevision|0x00000001
  #Signature_32('Q', 'C', 'O', 'M') = 0x4D4F4351
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x4D4F4351
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorRevision|0x00000001
  gQcomTokenSpaceGuid.DisableWriteProtect|TRUE

#
# Target Specific PCDs
#

  #########################################################################
  #                               Memory Map
  #########################################################################

!if $(PRODMODE) == "PRODMODE"
  # Version info needed on LCD Display
  gQcomTokenSpaceGuid.VersionDisplay|FALSE
!else
  # Version info needed on debug port
  gQcomTokenSpaceGuid.VersionDisplay|TRUE
!endif

  # Magic signature value that is used to indicate DebugLog Override
  # capability in DebugLib of DXE
  gQcomTokenSpaceGuid.PcdDebugLogOverrideMagic|0x534B4950

  # Hard/Warm Reset
  gQcomTokenSpaceGuid.PcdUefiHardResetConfigure|TRUE
  gQcomTokenSpaceGuid.PcdUefiWarmResetConfigure|TRUE
  gQcomTokenSpaceGuid.PcdUefiHardResetEnabled|TRUE
  gQcomTokenSpaceGuid.PcdUefiWarmResetEnabled|TRUE

  # PON Debounce
  gQcomTokenSpaceGuid.PcdPONDebounce|0x00000001 # max value 0x07


  # ChargerTaskSupported
  gQcomTokenSpaceGuid.PcdChargerTaskSupported|FALSE

  # Default to EL1
  gQcomTokenSpaceGuid.PcdSwitchToEL1|TRUE

[PcdsPatchableInModule]
  ## This PCD defines the Console output column and the default value is 25 according to UEFI spec
  #gEfiMdeModulePkgTokenSpaceGuid.PcdConOutRow|53

  ## This PCD defines the Console output row and the default value is 80 according to UEFI spec
  #gEfiMdeModulePkgTokenSpaceGuid.PcdConOutColumn|75

  # This PCD should be set to 0 then video resolution could be at highest resolution.
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|0
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|0


################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  #
  # Tools Applications
  #
  QcomPkg/QcomToolsPkg/CmdApp/CmdApp.inf
  QcomPkg/QcomToolsPkg/ListVars/ListVars.inf
  QcomPkg/QcomToolsPkg/MenuApp/MenuApp.inf
  #QcomPkg/QcomToolsPkg/PayloadApp/PayloadApp.inf
  QcomPkg/QcomToolsPkg/Pgm/Pgm.inf
  QcomPkg/QcomToolsPkg/DelBootVars/DelBootVars.inf
  QcomPkg/QcomToolsPkg/RPMBProvision/RPMBProvision.inf
  QcomPkg/QcomToolsPkg/RPMBErase/RPMBErase.inf {
   <LibraryClasses>
     DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }
  QcomPkg/QcomToolsPkg/UEFINVErase/UEFINVErase.inf
  QcomPkg/QcomToolsPkg/UsbfnMsdApp/UsbfnMsdApp.inf
  QcomPkg/QcomToolsPkg/SecurityToggleApp/SecurityToggleApp.inf
  QcomPkg/QcomToolsPkg/DebugPolicyToggleApp/DebugPolicyToggleApp.inf
  #QCOM Driver enumeration controller
  QcomPkg/QcomToolsPkg/WinDsdtUpdateControllerUI/WinDsdtUpdateControllerUI.inf
  EmbeddedPkg/Ebl/Ebl.inf


# 2.0 Shell
  ShellPkg/Application/Shell/Shell.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
      ShellCommandLib|ShellPkg/Library/UefiShellCommandLib/UefiShellCommandLib.inf
      NULL|ShellPkg/Library/UefiShellLevel2CommandsLib/UefiShellLevel2CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel1CommandsLib/UefiShellLevel1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellLevel3CommandsLib/UefiShellLevel3CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDriver1CommandsLib/UefiShellDriver1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellInstall1CommandsLib/UefiShellInstall1CommandsLib.inf
      NULL|ShellPkg/Library/UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.inf
      HandleParsingLib|ShellPkg/Library/UefiHandleParsingLib/UefiHandleParsingLib.inf
      NULL|ShellPkg/Library/UefiShellNetwork1CommandsLib/UefiShellNetwork1CommandsLib.inf
      FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
      SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
      ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
      DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
      BcfgCommandLib|ShellPkg/Library/UefiShellBcfgCommandLib/UefiShellBcfgCommandLib.inf

    <PcdsFixedAtBuild>
      gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF
      gEfiShellPkgTokenSpaceGuid.PcdShellLibAutoInitialize|FALSE
      gEfiMdePkgTokenSpaceGuid.PcdUefiLibMaxPrintBufferSize|16000
  }
################################################################################
#
# UserExtensions.Buildit.BuildInfo - Extra info for buildit.py
#
################################################################################
[UserExtensions.Buildit.BuildInfo]
  COMPILER_ARCH = AARCH64
  OUTPUT_NAME   = tools.fv
  COMPILER_WIN  = CLANG39WIN
  COMPILER_LINUX = CLANG39LINUX
