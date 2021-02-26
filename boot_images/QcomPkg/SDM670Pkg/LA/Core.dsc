#==============================================================================
# @file Core.dsc
# SDM670 Core package.
#
# Copyright (c) 2015 - 2018, 2020 Qualcomm Technologies, Inc. All rights reserved.
# Portions Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#==============================================================================

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = SDM670Pkg
  PLATFORM_GUID                  = E7711F33-1CBA-46A9-8E88-82D3BBF533C9
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  SUPPORTED_ARCHITECTURES        = ARM|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = QcomPkg/SDM670Pkg/LA/Core.fdf

[Defines.ARM]
  OUTPUT_DIRECTORY               = Build/SDM670LA_Core_32

[Defines.AARCH64]
  OUTPUT_DIRECTORY               = Build/SDM670LA_Core

[LibraryClasses.common]
  #
  # EDK2 Libraries
  #
  ArmDisassemblerLib|ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerVirtCounterLib/ArmGenericTimerVirtCounterLib.inf
  ArmGicArchLib|ArmPkg/Library/ArmGicArchLib/ArmGicArchLib.inf
  ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  ArmMmuLib|ArmPkg/Library/ArmMmuLib/ArmMmuBaseLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibOptDxe/BaseMemoryLibOptDxe.inf
  BaseSynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  CpuExceptionHandlerLib|ArmPkg/Library/ArmExceptionLib/ArmRelocateExceptionLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  DebugLib|QcomPkg/Library/DebugLib/DebugLib.inf
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  EblAddExternalCommandLib|EmbeddedPkg/Library/EblAddExternalCommandLib/EblAddExternalCommandLib.inf
  EblNetworkLib|EmbeddedPkg/Library/EblNetworkLib/EblNetworkLib.inf
  EfiFileLib|EmbeddedPkg/Library/EfiFileLib/EfiFileLib.inf
  FdtLib|EmbeddedPkg/Library/FdtLib/FdtLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  LzmaDecompressLib|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  ZlibDecompressLib|QcomPkg/Library/ZlibDecompressLib/ZlibDecompressLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  PrePiLib|EmbeddedPkg/Library/PrePiLib/PrePiLib.inf
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
  UsbSharedLib|QcomPkg/SDM670Pkg/Library/UsbSharedLib/UsbSharedLibCore.inf
  LockBoxLib|MdeModulePkg/Library/LockBoxNullLib/LockBoxNullLib.inf
  
  #
  # Qualcomm target independent libraries
  #
  NULL|QcomPkg/Library/QcomBaseLib/QcomBaseLib.inf
  NULL|QcomPkg/Library/StackCanaryBaseLib/StackCanaryBaseLibCore.inf
  AslrLib|QcomPkg/Library/AslrLib/AslrLib.inf
  DbiDumpLib|QcomPkg/Library/DbiDumpLib/DbiDumpLib.inf
  # MuslLib: POSIX string APIs
  NULL|QcomPkg/Library/MuslLib/MuslLib.inf
  MuslLib|QcomPkg/Library/MuslLib/MuslLib.inf
  UefiCfgLib|QcomPkg/Library/UefiConfigLib/UefiCfgLib.inf
  # Shared library instace for clients usage
  ShLib|QcomPkg/Library/ShLib/ShLib.inf
  RamPartitionTableLib|QcomPkg/Library/RamPartitionTableLib/RamPartitionTableLib.inf
  DDRInfoPartitionLib|QcomPkg/Library/DDRInfoPartitionLib/DDRInfoPartitionLib.inf
  PeCoffExtraActionLib|QcomPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf

  CookieLib|QcomPkg/Library/CookieLib/CookieLib.inf
  SDILib|QcomPkg/Library/SDILib/SDILib.inf

  AptLib|QcomPkg/Library/NullLibs/AptLibNull/AptLibNull.inf
  BootConfigLib|QcomPkg/SDM670Pkg/Library/BootConfigLib/BootConfigLib.inf
  BusyWaitLib|QcomPkg/Library/BusyWaitLib/BusyWaitLib.inf
  ChargerLib|QcomPkg/Library/ChargerLib/ChargerLibLA.inf
  ChipInfoCoreLib|QcomPkg/Library/ChipInfoLib/ChipInfoCore.inf
  DDRTargetLib|QcomPkg/Library/NullLibs/DDRTargetLibNull/DDRTargetLibNull.inf
  EblCmdLib|QcomPkg/Library/EblCmdLib/EblCmdLib.inf
  EfiResetSystemLib|QcomPkg/Library/ResetSystemLib/LA/ResetSystemLib.inf
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
  IcbCfgLib|QcomPkg/Library/NullLibs/BusesLibNull/IcbCfgLibNull.inf
  ImageVersionLib|QcomPkg/Library/ImageVersionLib/ImageVersionLib.inf
  IORTLib|QcomPkg/Library/IORTLib/IORTLib.inf
  PIMEMTargetLib|QcomPkg/Library/NullLibs/PIMEMTargetLibNull/PIMEMTargetLibNull.inf
  ParserLib|QcomPkg/Library/ParserLib/ParserLib.inf
  PeHashLib|QcomPkg/Library/PeHashLib/PeHashLib.inf
  PlatformInfoCoreLib|QcomPkg/Library/PlatformInfoLib/PlatformInfoCore.inf
  PmicApiLib|QcomPkg/Library/PmicApiLib/PmicApiLib.inf
  PrintLib|QcomPkg/Library/PrintShLib/PrintShLib.inf
  ProcLib|QcomPkg/Library/ProcLib/ProcLib.inf
  QcomBaseLib|QcomPkg/Library/QcomBaseLib/QcomBaseLib.inf
  QcomLib|QcomPkg/Library/QcomLib/QcomLib.inf
  MountFvUtilsLib|QcomPkg/Library/MountFvUtilsLib/MountFvUtilsLib.inf
  BMPLib|QcomPkg/Library/BMPLib/BMPLib.inf
  QcomUtilsLib|QcomPkg/Library/QcomUtilsLib/QcomUtilsLib.inf
  QusbLib|QcomPkg/Library/QusbLib/QusbLib.inf
  RFSLib|QcomPkg/Library/RFSLib/RFSLib.inf
  ROMFSLib|QcomPkg/Library/EfsLib/ROMFSLib.inf
  RandomStackCanaryLib|QcomPkg/Library/RandomStackCanaryLib/RandomStackCanaryLib.inf
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
  ClockLoaderLib|QcomPkg/Library/ClockLoaderLib/ClockLoaderLib.inf
  ICBLib|QcomPkg/Library/ICBLib/ICBLib.inf
  ICBArbLib|QcomPkg/Library/ICBArbLib/ICBArbLib.inf
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

  # Display Common Libraries
  MDPLib|QcomPkg/Library/MDPLib/MDPLib.inf
  HALMDSSLib|QcomPkg/Library/HALMDSSLib/HALMDSS4xx.inf
  HALDSILib|QcomPkg/Library/HALDSILib/HALDSILib.inf
  ExternalDisplayLib|QcomPkg/Library/ExternalDisplayLib/ExternalDisplayLib.inf

  #GPI Library
  GpiLib|QcomPkg/Library/GpiLib/GpiLib.inf
  GpiDrvLib|QcomPkg/SDM670Pkg/Library/GpiDrvLib/GpiDrvLib.inf

  # I2C Library
  I2CLib|QcomPkg/Library/I2CLib/I2CLib.inf
  I2CApiLib|QcomPkg/Library/I2CApiLib/I2CApiLib.inf
  I2CFWImageLib|QcomPkg/SDM670Pkg/Settings/I2C/I2CFWImageLib.inf
  SPILib|QcomPkg/Library/SPILib/SPILib.inf
  SPIApiLib|QcomPkg/Library/SPIApiLib/SPIApiLib.inf
  SPIFWImageLib|QcomPkg/SDM670Pkg/Settings/SPI/SPIFWImageLib.inf

  # NPA API Library
  NpaApiLib|QcomPkg/Library/NpaApiLib/NpaApiLib.inf

  # CmdDb API Library
  CmdDbApiLib|QcomPkg/Library/CmdDbApiLib/CmdDbApiLib.inf

  # RPMH API Library
  RpmhApiLib|QcomPkg/Library/RpmhApiLib/RpmhApiLib.inf

  #Qdss SW event library
  QdssLib|QcomPkg/Library/QdssLib/QdssLib.inf

  # ADC Libraries
  AdcDxeWrapperLib|QcomPkg/Library/AdcLib/build/AdcDxeWrapperLib.inf

  HWConfigLib|QcomPkg/Library/HWConfigLib/HWConfigLib.inf

  # Qualcomm target dependent libraries
  CPRTargetLib|QcomPkg/SDM670Pkg/Library/CPRTargetLib/Core/CPRTargetLib.inf
  ClockTargetLib|QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockTargetLib.inf
  HashLib|QcomPkg/SDM670Pkg/Library/HashLibCore/HashLibHw_core.inf
  PmicLib|QcomPkg/SDM670Pkg/Library/PmicLib/PmicLib.inf
  PrngLib|QcomPkg/SDM670Pkg/Library/RNGLib/RNGLibHw_core.inf
  #QdssStmTargetLib|QcomPkg/SDM670Pkg/Library/QdssStmTargetLib/QdssStmTargetLib.inf
  #QdssTFunnelTargetLib|QcomPkg/SDM670Pkg/Library/QdssTmcTargetLib/QdssTmcTargetLib.inf
  #QdssTmcTargetLib|QcomPkg/SDM670Pkg/Library/QdssTFunnelTargetLib/QdssTFunnelTargetLib.inf
  #QusbDciCommonLib|QcomPkg/SDM670Pkg/Library/QusbDciCommonLib/QusbDciCommonLib.inf
  #QusbLdrLib|QcomPkg/SDM670Pkg/Library/QusbLdrLib/QusbLdrLib.inf
  #QusbTargetLib|QcomPkg/SDM670Pkg/Library/QusbTargetLib/QusbTargetLib.inf
  RNGLib|QcomPkg/SDM670Pkg/Library/RNGLib/RNGLibHw_core.inf
  SdccClockLib|QcomPkg/SDM670Pkg/Library/SdccClockLib/SdccCoreClockLib.inf
  SdccTargetLib|QcomPkg/SDM670Pkg/Library/SdccTargetLib/SdccCoreTargetLib.inf
  TLMMTargetLib|QcomPkg/SDM670Pkg/Library/TLMMTargetLib/TLMMTargetLib.inf
  UfsTargetUefiLib|QcomPkg/SDM670Pkg/Library/UfsTargetLib/UfsTargetUefiLib.inf
  WatchDogLib|QcomPkg/SDM670Pkg/Library/WatchDogLib/WatchDogLib.inf

  #UsbWrapperLayerLib|QcomPkg/SDM670Pkg/Library/UsbWrapperLayerLib/UsbWrapperLayerLib.inf
  HALclkLib|QcomPkg/SDM670Pkg/Library/HALclkLib/HALclkLibXBL.inf
  PmicShutdownLibBoot|QcomPkg/SDM670Pkg/Library/PmicLib/core/shutdown/pm_shutdown.inf
  PmicShutdownRuntimeLib|QcomPkg/SDM670Pkg/Library/PmicLib/core/shutdown/pm_runtime_shutdown.inf
  SmemTargetLib|QcomPkg/SDM670Pkg/Library/SmemTargetLib/SmemTargetLib.inf

  # DAL Target Libraries
  DALGenLibCore|QcomPkg/SDM670Pkg/Settings/DALConfig/DALGenLibCore.inf
  #DALGenLibTools|QcomPkg/SDM670Pkg/Settings/DALConfig/DALGenLibTools.inf
  DALConfigLib|QcomPkg/SDM670Pkg/Settings/DALConfig/DALConfigCoreLib.inf
  #DALConfigToolsLib|QcomPkg/SDM670Pkg/Settings/DALConfig/DALConfigToolsLib.inf

  # Display Chipset Specific Libraries
  MDPPlatformLib|QcomPkg/SDM670Pkg/Library/MDPPlatformLib/MDPPlatformLib.inf

  BamTargetLib|QcomPkg/SDM670Pkg/Library/BamTargetLib/BamTargetLib.inf

  # MpPower Library
  #MpPowerLib|QcomPkg/SDM670Pkg/Library/MpPowerLib/MpPowerLib.inf

  # SPMI Library
  SPMILib|QcomPkg/SDM670Pkg/Library/SPMILib/SPMILib.inf


[LibraryClasses.Arm]
  ArmLib|ArmPkg/Library/ArmLib/ArmV7/ArmV7Lib.inf

[LibraryClasses.AARCH64]
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64Lib.inf

[LibraryClasses.common.SEC]
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLibBase.inf
  ExtractGuidedSectionLib|EmbeddedPkg/Library/PrePiExtractGuidedSectionLib/PrePiExtractGuidedSectionLib.inf
  HobLib|EmbeddedPkg/Library/PrePiHobLib/PrePiHobLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
  PrePiHobListPointerLib|EmbeddedPkg/Library/PrePiHobListPointerLib/PrePiHobListPointerLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  ReportStatusCodeLib|MdePkg/Library/BaseReportStatusCodeLibNull/BaseReportStatusCodeLibNull.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf

  #
  # Qualcomm SEC Libraries
  #
  DebugLib|QcomPkg/Library/DebugLib/DebugLib_Sec.inf
  PrintHostLib|QcomPkg/Library/PrintHostLib/PrintHostLib.inf
  ProcLib|QcomPkg/Library/ProcLib/ProcLib.inf
  SecFuseLib|QcomPkg/Library/SecFuseLib/SecFuseLib.inf
  SerialPortLib|QcomPkg/Library/SerialPortLib/SerialPortLib.inf
  UartLib|QcomPkg/Library/UartQupv3Lib/UartCoreLib.inf
  UartSettingsLib|QcomPkg/SDM670Pkg/Settings/UART/UartSettingsLib.inf
  ShLib|QcomPkg/Library/ShLib/ShLibMgr.inf

  # Data Execution Prevention
  DepLib|QcomPkg/Library/DepLib/DepLib.inf

  PrePiMemoryAllocationLib|QcomPkg/Library/PrePiMemoryAllocationLib/PrePiMemoryAllocationLib.inf
  MemoryAllocationLib|QcomPkg/Library/PrePiMemoryAllocationLib/PrePiMemoryAllocationLib.inf
  AllocPagePoolLib|QcomPkg/Library/AllocPagePoolLib/AllocPagePoolLib.inf

  # Target SEC Libraries
  TargetInitLib|QcomPkg/SDM670Pkg/Library/TargetInitLib/TargetInitLib.inf

  SplitBootLib|QcomPkg/XBLCore/SplitBootNullLib.inf

[LibraryClasses.ARM.SEC]
  ArmLib|ArmPkg/Library/ArmLib/ArmV7/ArmV7LibPrePi.inf

[LibraryClasses.AARCH64.SEC]
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64LibPrePi.inf

[LibraryClasses.common.PEI_CORE]
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf

[LibraryClasses.common.DXE_CORE]
  #Stubs required for PeCoffExtraActionLib PeCoffLib in DXE Core
  PeCoffExtraActionLib|ArmPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf
  PeCoffLib|EmbeddedPkg/Library/DxeHobPeCoffLib/DxeHobPeCoffLib.inf

  HobLib|MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf

  DebugLib|QcomPkg/Library/DebugLib/DebugLib.inf

[LibraryClasses.common.DXE_DRIVER]
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  GenericBdsLib|QcomPkg/Library/GenericBdsLib/GenericBdsLib.inf
  PlatformBdsLib|QcomPkg/Library/PlatformBdsLib/PlatformBdsLib.inf
  BdsTargetLib|QcomPkg/SDM670Pkg/Library/BdsTargetLibNull/BdsTargetLibNull.inf
  QcomBdsLib|QcomPkg/Library/QcomBdsLib/QcomBdsLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  OfflineCrashDumpLib|QcomPkg/Library/OfflineCrashDumpLib/OfflineCrashDumpDxeLib.inf

  #Arm Architectural Libraries
  DefaultExceptionHandlerLib|ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  ArmDisassemblerLib|ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  OfflineCrashDumpLib|QcomPkg/Library/OfflineCrashDumpLib/OfflineCrashDumpDxeLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  ExtractGuidedSectionLib|MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  PeCoffLib|EmbeddedPkg/Library/DxeHobPeCoffLib/DxeHobPeCoffLib.inf
  OfflineCrashDumpLib|QcomPkg/Library/OfflineCrashDumpLib/OfflineCrashDumpDxeLib.inf
  GenericBdsLib|IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf

[BuildOptions.ARM]
  # to enable thumb we need to evaluate if size is more advantage or speed
  RVCT:*_*_ARM_ARCHCC_FLAGS  == --cpu Cortex-A53 --thumb --fpu vfpv3
  RVCT:*_*_ARM_ARCHASM_FLAGS == --cpu Cortex-A53

[BuildOptions.AARCH64]
  GCC:*_*_AARCH64_ARCHCC_FLAGS  == -fno-common -mtune=cortex-a53 -I $(WORKSPACE)/QcomPkg/SDM670Pkg/Include -include $(WORKSPACE)/QcomPkg/SDM670Pkg/Include/Target_cust.h -include $(WORKSPACE)/QcomPkg/Include/Library/DebugLib.h -DQCOM_EDK2_PATCH -DDISABLE_DEP $(FEATURE_FLAGS) -DENABLE_DEP_64 -DENABLE_XN -DENABLE_ASLR -fstack-protector-strong
  GCC:*_*_AARCH64_ARCHPP_FLAGS  == -I $(WORKSPACE)/QcomPkg/SDM670Pkg/Include -include $(WORKSPACE)/QcomPkg/Msm8998Pkg/Include/Target_cust.h -include $(WORKSPACE)/QcomPkg/Include/Library/DebugLib.h -DQCOM_EDK2_PATCH -DDISABLE_DEP $(FEATURE_FLAGS) -DENABLE_DEP_64 -DENABLE_XN -DENABLE_ASLR -fstack-protector-strong

!if $(PRODMODE) == "PRODMODE"
  RVCT:*_*_*_CC_FLAGS = -DPRODMODE
  GCC:*_*_*_CC_FLAGS = -DPRODMODE
!endif

!ifdef $(PRE_SIL)
  GCC:*_*_*_CC_FLAGS = -DPRE_SIL
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
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwarePerformanceDataTableS3Support|FALSE

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

  ## Indicates if StatusCode is reported via Serial port.
  #   TRUE  - Reports StatusCode via Serial port.
  #   FALSE - Does not report StatusCode via Serial port.
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE

  ## Indicates if StatusCode is stored in memory.
  #  The memory is boot time memory in PEI Phase and is runtime memory in DXE Phase.
  #   TRUE  - Stores StatusCode in memory.
  #   FALSE - Does not store StatusCode in memory.
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE

  ## Indicates if PEI phase StatusCode will be replayed in DXE phase.
  #   TRUE  - Replays PEI phase StatusCode in DXE phased.
  #   FALSE - Does not replay PEI phase StatusCode in DXE phase.
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeReplayIn|FALSE

  #
  # Target board Specific PCDs
  #

# TIANOCORE UPDATE: ShellPkg Disabled
#  gEfiShellPkgTokenSpaceGuid.PcdShellForceNoMapNoStartup|TRUE

  gArmTokenSpaceGuid.PcdArmGicV3WithV2Legacy|FALSE

[PcdsFixedAtBuild.common]
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Qualcomm Technologies, Inc."
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L"5.0"

  gEmbeddedTokenSpaceGuid.PcdEmbeddedPrompt|"EBL"
  gEmbeddedTokenSpaceGuid.PcdPrePiCpuMemorySize|48
  gEmbeddedTokenSpaceGuid.PcdPrePiCpuIoSize|0
  gEfiMdePkgTokenSpaceGuid.PcdMaximumUnicodeStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumAsciiStringLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdMaximumLinkedListLength|1000000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|10000000
  gEfiMdePkgTokenSpaceGuid.PcdDebugClearMemoryValue|0xAF

  # Disable perf
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|1
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
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiExposedTableVersions|0xC
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"QCOM  "
  #Signature_64('Q','C','O','M','E','D','K','2') = 0x324B44454D4F4351 = '2','K','D','E','M','O','C','Q'
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x324B44454D4F4351
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemRevision|0x00000001
  #Signature_32('Q', 'C', 'O', 'M') = 0x4D4F4351
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x4D4F4351
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorRevision|0x00000001
  gQcomTokenSpaceGuid.DisableWriteProtect|TRUE


#
# Display PCDs
#
  gQcomTokenSpaceGuid.PcdPanelType|0
  gQcomTokenSpaceGuid.PcdHPDPollCount|200
  gQcomTokenSpaceGuid.PcdBacklightLevel|50
  gQcomTokenSpaceGuid.PcdDisplayForceSwRenderer|FALSE
  # gFBResLimit Table 0: 4k, 1: UHD, 2: WQHD, 3: FHD, 4: HD
  gQcomTokenSpaceGuid.PcdFrameBufMaxRes|0
  # External display type:  0: No external display, 1: DisplayPort, 2: HDMI
  gQcomTokenSpaceGuid.PcdExtDisplayType|0
  # External display mode Index : 0: Preferred mode, 1: Largest mode
  gQcomTokenSpaceGuid.PcdSetExtMode|0
  gQcomTokenSpaceGuid.PcdReservedMemorySize|0
  gQcomTokenSpaceGuid.PcdReservedMemoryAlignment|64
  gQcomTokenSpaceGuid.PcdBacklightLevel|50
  gQcomTokenSpaceGuid.PcdPresentationMode|0

#
# LPM PCDs
#

 gQcomTokenSpaceGuid.PcdLpm|0x030580A5
 
#
# Target Specific PCDs
#

  #########################################################################
  #                               Memory Map
  #########################################################################

  gQcomTokenSpaceGuid.PcdMaxMemory|0x0

  #################################
  ## Presilicon definitions.
  #################################
  gQcomTokenSpaceGuid.PcdMemoryBase|0x80000000    # Starting address
  gQcomTokenSpaceGuid.PcdMemorySize|0x60000000    # 1.5 GB

  #################################
  ## HLOS Base Offset and size.
  #################################
  gQcomTokenSpaceGuid.PcdHLOSMemoryBaseOffset|0x98200000 # remaining memory after this is HLOS

  #################################
  ## UEFI Memory Base Offset and Size.
  #################################
  gQcomTokenSpaceGuid.PcdUefiMemPoolBaseOffset|0x94E00000
  gQcomTokenSpaceGuid.PcdUefiMemPoolSize|0x03300000     # 51 MB

  # The HOB list will start here.
  # NOTE: Hob.c is still referencing PcdPrePiHobBase
  gQcomTokenSpaceGuid.PcdPrePiHobBase|0x94E00000

  # Offset to UEFI memory
  gQcomTokenSpaceGuid.PcdPreAllocatedMemorySize|0x00000000

 
  #################################
  ## FD Base offset (refer to .fdf for FD size)
  #################################
  gQcomTokenSpaceGuid.PcdEmbeddedFdBaseAddress|0x9FC00000

  #################################
  ## Shared Memory Base offset and size.
  ## TODO: PcdSmemBaseAddress is currently used by smem library
  #################################
  gQcomTokenSpaceGuid.PcdSmemBaseAddress|0x86000000  #64-bit smem addr
  gQcomTokenSpaceGuid.PcdSmemSize|0x00200000     # 2MB
  ## Generic SMEM information that SMEM lib might need
  ## LSB represents chip family, other bytes are unused for now.
  gQcomTokenSpaceGuid.PcdSmemInformation|0x00000005  # 5 = Nazgul

  # Maximum number of memory regions defined in the above memory map
  gQcomTokenSpaceGuid.PcdMaxMemRegions|64

  #################################
  ## IMem Base Address and size.
  #################################

  gQcomTokenSpaceGuid.PcdIMemCookiesBase|0x146BF000
  gQcomTokenSpaceGuid.PcdIMemCookiesSize|0x00001000

  # Cookie (memory capture mode) Address for crash dump support
  gQcomTokenSpaceGuid.PcdHLOSCrashCookieAddr|0x146BF018

  # Uefi Debug Cookie Address and value
  gQcomTokenSpaceGuid.PcdUefiDebugCookieOffset|0x944
  gQcomTokenSpaceGuid.PcdUefiDebugCookie|0x55444247

  gArmTokenSpaceGuid.PcdCpuResetAddress|0x08000000    # warm reset should use this

  gArmPlatformTokenSpaceGuid.PcdCoreCount|8

  # Secure boot efuse register address
  gQcomTokenSpaceGuid.PcdSecbootRegAddr|0x0005E030
  gQcomTokenSpaceGuid.PcdSecbootAuthEnMask|0x20


  # Timer Debug
  gQcomTokenSpaceGuid.PcdTimerDxeDebug|FALSE

  # Timers
  gEmbeddedTokenSpaceGuid.PcdInterruptBaseAddress|0x9BC0000

  # Timers
  # ARM Architectural Timer Interrupt(GIC PPI) numbers
  gArmTokenSpaceGuid.PcdArmArchTimerSecIntrNum|17
  gArmTokenSpaceGuid.PcdArmArchTimerIntrNum|18
  gArmTokenSpaceGuid.PcdArmArchTimerHypIntrNum|0
  gArmTokenSpaceGuid.PcdArmArchTimerVirtIntrNum|19

  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|19200000

  gArmTokenSpaceGuid.PcdGicDistributorBase|0x17A00000
  gArmTokenSpaceGuid.PcdGicRedistributorsBase|0x17A60000

  # Below PcdGicInterruptInterfaceBase is used for supporting the GICv2 Emulation in the Hyper visor
  # Which need mGicInterruptInterfaceBase to be initialized to 0x17A60000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x17A60000

  gQcomTokenSpaceGuid.PcdInterruptTimerIndex|3
  gQcomTokenSpaceGuid.PcdInterruptTimerPeriod|100000
  gQcomTokenSpaceGuid.PcdTimer1IRQ|0x28
  gQcomTokenSpaceGuid.PcdTimerTestClockFreq|918000000 # Pcycles runs at 918 MHz
  gQcomTokenSpaceGuid.PcdTimerTestClockFreqDiv|64 # Pcycles divided by value
  gQcomTokenSpaceGuid.PcdQTimerAC_BasePhys|0x17C90000
  gQcomTokenSpaceGuid.PcdQTimerV1BasePhys|0x17CA0000
  gQcomTokenSpaceGuid.PcdQTimerV2BasePhys|0x17CB0000
  gQcomTokenSpaceGuid.PcdQTimerFrame|0
  gQcomTokenSpaceGuid.PcdQTimerEnabled|1
  gQcomTokenSpaceGuid.PcdPerfomanceCounterEnd|0xFFFFFFFF
  gQcomTokenSpaceGuid.PcdPerfomanceCounterStart|0x00000000
  gQcomTokenSpaceGuid.PcdInterruptVectorDGT|17
  gQcomTokenSpaceGuid.PcdInterruptVectorGPT0|18
  gQcomTokenSpaceGuid.PcdInterruptVectorGPT1|19
  gQcomTokenSpaceGuid.PcdPerformanceCounterIndex|3
  gQcomTokenSpaceGuid.PcdRealTimeClockResolution|1
  gQcomTokenSpaceGuid.PcdRealTimeClockAccuracy|200000000
  gQcomTokenSpaceGuid.PcdRealTimeClockSetToZero|1

  #TODO: Need to correct based on frequency plan
  gQcomTokenSpaceGuid.PcdAppsProcFrequencyMhz|1190

  #MPM_SLEEP_TIMETICK_COUNT_VAL for time from power on
  gQcomTokenSpaceGuid.PcdTimeTickClkAddress|0x0C221000

  gQcomTokenSpaceGuid.PcdPsHoldAddress|0x0C264000

  #I2C
  gQcomTokenSpaceGuid.EEPROMI2CCore|12
  gQcomTokenSpaceGuid.EEPROMSlaveAddress|0x50

  # SPI Loopback
  gQcomTokenSpaceGuid.SPILoopbackCore|10

  # SPMI
  gQcomTokenSpaceGuid.SPMIBasePhysical0|0x0C400000
  gQcomTokenSpaceGuid.SPMIBasePhysical1|0
  gQcomTokenSpaceGuid.SPMIMemoryMapSize|0x2800
  gQcomTokenSpaceGuid.SPMIOwner|0
  gQcomTokenSpaceGuid.SPMIDebugChannel|0x000001FF

  # SDCC
  gQcomTokenSpaceGuid.SDCCExtSlotNumber|1
  gQcomTokenSpaceGuid.SDCCEmmcSlotNumber|0
  gQcomTokenSpaceGuid.SDCCRpmbKey|{0x20, 0xAA, 0x9C, 0xF4, 0x99, 0x4D, 0xD4, 0xFE, 0xA5, 0x85, 0xBE, 0x96, 0x6, 0x1, 0xD1, 0xA9, 0xC3, 0x3, 0x4F, 0x91, 0x62, 0x7C, 0x64, 0x53, 0x38, 0xC1, 0x1F, 0xF2, 0x76, 0x4D, 0x2E, 0xC4}
  gQcomTokenSpaceGuid.SDCCEventBasedSDDetectEnabled|TRUE
  gQcomTokenSpaceGuid.SDCCDmaEnabled|TRUE
  gQcomTokenSpaceGuid.SDCCEmmcHwResetEnabled|TRUE
  gQcomTokenSpaceGuid.SDCCMaxSlotNumber|2
  gQcomTokenSpaceGuid.SDCCSdhciEnable|TRUE
  ## SDCC eMMC speed mode - DDR50: 0, HS200: 1, HS400: 2 SDR: 3 ##
  gQcomTokenSpaceGuid.SDCCEmmcSpeedMode|2
  gQcomTokenSpaceGuid.SDCCRegBase|"+PERIPH_SS_SDC1_SDCC_SDCC5_HC"

  #
  # ARM Pcds
  #
  gArmTokenSpaceGuid.PcdArmUncachedMemoryMask|0x0000000040000000
  gArmTokenSpaceGuid.PcdCpuVectorBaseAddress|0x9FF8C000

  # Timer sync not required
  gQcomTokenSpaceGuid.PcdSyncTimerToMPP|FALSE

  # UART
  gQcomTokenSpaceGuid.UartPlatform|"SDM845"

  # USB
  gQcomTokenSpaceGuid.InitUsbControllerOnBoot|FALSE
  gQcomTokenSpaceGuid.HostModeSupported|FALSE

#!if ( gQcomTokenSpaceGuid.PcdExtDisplayType == 1 )  
  # DP over USB type-C: Enable high speed USB configuration
  gQcomTokenSpaceGuid.HighSpeedOnlySupported|FALSE
#!endif

!if $(PRODMODE) == "PRODMODE"
  # Version info needed on LCD Display
  gQcomTokenSpaceGuid.VersionDisplay|FALSE
!else
  # Version info needed on debug port
  gQcomTokenSpaceGuid.VersionDisplay|TRUE
!endif

  # offset to 4KB shared memory in System IMEM             : 0x8603F000
  # offset to 200 bytes HLOS region within this 4KB memory : 0x658
  # offset 0  : 4 byte physical address for DebugLog Override
  gQcomTokenSpaceGuid.PcdDebugLogOverrideAddress|0x08642658

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
  
  gEfiMdeModulePkgTokenSpaceGuid.PcdProgressCodeOsLoaderLoad | 0x03058000
  gEfiMdeModulePkgTokenSpaceGuid.PcdProgressCodeOsLoaderStart| 0x03058001

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
# SEC
#
  QcomPkg/XBLCore/XBLCore.inf


  #
  # PEI Phase modules
  #
    # we don't implement PEI

#
# DXE
#
  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      DebugLib|QcomPkg/Library/DebugLib/DebugLib.inf
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
      NULL|EmbeddedPkg/Library/LzmaHobCustomDecompressLib/LzmaHobCustomDecompressLib.inf
  }

  ArmPkg/Drivers/CpuDxe/CpuDxe.inf

  # Report Status Code Router & Handler
  MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf

  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf

  #
  # Security Dxe
  #
  QcomPkg/Drivers/SecurityDxe/SecurityDxe.inf
  #MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf

  #
  # SecRSA Dxe
  #

  QcomPkg/Drivers/SecRSADxe/SecRSADxe.inf

  #
  # ASN1X509 Dxe
  #

  QcomPkg/Drivers/ASN1X509Dxe/ASN1X509Dxe.inf

  #
  # VerifiedBoot Dxe
  #

  QcomPkg/Drivers/VerifiedBootDxe/VerifiedBootDxe.inf

  # VerfiedBootNull Dxe support for presilicon/bringup
  #QcomPkg/Drivers/VerifiedBootNullDxe/VerifiedBootNullDxe.inf



  #
  # TzDxe Driver
  #
  QcomPkg/Drivers/TzDxe/TzDxeLA.inf

  #
  # MdtpDxe Driver
  #
  #QcomPkg/Drivers/MdtpDxe/MdtpDxe.inf

  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf

  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf

  #
  # Variable Services
  #
  MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
  QcomPkg/Drivers/VariableDxe/VariableDxe.inf

  #
  # DPP Services
  #

  QcomPkg/Drivers/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  QcomPkg/Drivers/SimpleTextInOutSerialDxe/SimpleTextInOutSerial.inf

  QcomPkg/Drivers/ResetRuntimeDxe/ResetRuntimeDxe.inf

  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf

  MdeModulePkg/Universal/PrintDxe/PrintDxe.inf{
    <LibraryClasses>
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  }
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  QcomPkg/Drivers/FontDxe/FontDxe.inf

  #
  # TLMM Driver
  #
  QcomPkg/Drivers/TLMMDxe/TLMMDxe.inf

  #
  # EnvDxe - FBPT, Debug script support
  #
  QcomPkg/Drivers/EnvDxe/EnvDxe.inf {
  <LibraryClasses>
    DebugLib|QcomPkg/Library/DebugLib/DebugLib.inf
  }

  #
  # OS Configuration
  #
  QcomPkg/Drivers/OSConfigDxe/OSConfigDxe.inf

  #
  # SMEM Driver
  #
  QcomPkg/Drivers/SmemDxe/SmemDxe.inf

  #
  # GLink Driver
  #
  #QcomPkg/Drivers/GLinkDxe/GLinkDxe.inf {
  #<LibraryClasses>
  #SmdLib|QcomPkg/Library/SmdLib/SmdLib.inf
  #GLinkLib|QcomPkg/Library/GLinkLib/GLinkLib.inf
  #}

  #
  # FAT filesystem + GPT/MBR partitioning
  #
  QcomPkg/Drivers/DiskIoDxe/DiskIoDxe.inf
  QcomPkg/Drivers/PartitionDxe/PartitionDxe.inf
  FatPkg/EnhancedFatDxe/Fat.inf
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  MdeModulePkg/Universal/FvSimpleFileSystemDxe/FvSimpleFileSystemDxe.inf
  #
  # DDRGetConfig
  #
  QcomPkg/Drivers/DDRInfoDxe/DDRInfoDxe.inf
  #
  # USB
  #
  QcomPkg/Drivers/UsbfnDwc3Dxe/UsbfnDwc3Dxe.inf {
    #<PcdsFixedAtBuild>
    #gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xFFFFFFFF
    <LibraryClasses>
    UsbfnDwc3Lib|QcomPkg/SDM670Pkg/Library/UsbfnDwc3Lib/UsbfnDwc3Lib.inf
  }
  #QcomPkg/Drivers/XhciPciEmulationDxe/XhciPciEmulationDxe.inf
  #QcomPkg/Drivers/XhciDxe/XhciDxe.inf
  #QcomPkg/Drivers/UsbBusDxe/UsbBusDxe.inf
  #QcomPkg/Drivers/UsbKbDxe/UsbKbDxe.inf
  #QcomPkg/Drivers/UsbMassStorageDxe/UsbMassStorageDxe.inf
  QcomPkg/Drivers/UsbMsdDxe/UsbMsdDxe.inf
  QcomPkg/Drivers/UsbDeviceDxe/UsbDeviceDxe.inf
  QcomPkg/Drivers/UsbConfigDxe/UsbConfigDxe.inf  {
    #<PcdsFixedAtBuild>
    #gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xFFFFFFFF
    <LibraryClasses>
    UsbConfigLib|QcomPkg/SDM670Pkg/Library/UsbConfigLib/UsbConfigLib.inf
  }
  QcomPkg/Drivers/UsbInitDxe/UsbInitDxe.inf

  #
  # DAL Drivers
  #
  QcomPkg/Drivers/DALSYSDxe/DALSYSDxe.inf

  #
  # Clock DXE driver
  #
  QcomPkg/Drivers/ClockDxe/ClockDxe.inf {
    <LibraryClasses>
    ClockLib|QcomPkg/SDM670Pkg/Library/ClockLib/ClockLibXBL.inf
  }

  #
  # HAL IOMMU DXE driver
  #
  QcomPkg/Drivers/HALIOMMUDxe/HALIOMMUDxe.inf
 
  #
  # HWIO DXE driver
  #
  QcomPkg/Drivers/HWIODxe/HWIODxe.inf

  #
  # Hash Driver
  #
  QcomPkg/Drivers/HashDxe/HashDxe.inf

  #
  # Rng Driver
  #
  QcomPkg/Drivers/RNGDxe/RngDxe.inf

  #
  # Display DXE Driver
  #
  QcomPkg/Drivers/DisplayDxe/DisplayDxe.inf

  #
  # SoC Drivers
  #

  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf
  ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  QcomPkg/Drivers/ChipInfoDxe/ChipInfoDxe.inf
  #QcomPkg/Drivers/QdssDxe/QdssDxe.inf

  #
  # GPI Driver
  #
  QcomPkg/Drivers/GpiDxe/GpiDxe.inf

  #
  # I2C Driver
  #
  QcomPkg/Drivers/I2CDxe/I2CDxe.inf

  #
  # SPI Driver
  #
  QcomPkg/Drivers/SPIDxe/SPIDxe.inf

  #
  # NPA DXE driver
  #
  QcomPkg/Drivers/NpaDxe/NpaDxe.inf {
  <LibraryClasses>
  # RPM Libraries
  #RpmLib|QcomPkg/Library/RpmLib/RpmLib.inf
  #SmdLib|QcomPkg/Library/SmdLib/SmdLib.inf
  # NPA Library
  NpaLib|QcomPkg/Library/NpaLib/NpaLib.inf
  }

  #
  # ULog DXE driver
  #
  QcomPkg/Drivers/ULogDxe/ULogDxe.inf {
  <LibraryClasses>
  # ULog Library
  ULogLib|QcomPkg/Library/ULogLib/ULogLib.inf
  }

  #
  # CmdDb Driver
  #
  QcomPkg/Drivers/CmdDbDxe/CmdDbDxe.inf {
    <LibraryClasses>
    CmdDbLib|QcomPkg/Library/CmdDbLib/CmdDbLib.inf
    CmdDbTargetLib|QcomPkg/SDM670Pkg/Library/CmdDbTargetLib/CmdDbTargetLib.inf
  }

  #
  # RPMH DXE driver
  #
  QcomPkg/Drivers/RpmhDxe/RpmhDxe.inf {
    <LibraryClasses>
    # Rpmh Library
    RpmhLib|QcomPkg/Library/RpmhLib/RpmhLib.inf
  }

  #
  # CPR DXE driver
  #
  QcomPkg/Drivers/CPRDxe/CPRDxe.inf {
  <LibraryClasses>
  # CPR Library
  CPRLib|QcomPkg/SDM670Pkg/Library/CPRTargetLib/Core/CPRTargetLib.inf
  }

  #
  # Pdc DXE driver
  #
  QcomPkg/Drivers/PdcDxe/PdcDxe.inf {
    <LibraryClasses>
    PdcTargetLib|QcomPkg/SDM670Pkg/Library/PdcTargetLib/PdcTargetLib.inf
  }

  #
  # SPMI Driver
  #
  QcomPkg/Drivers/SPMIDxe/SPMIDxe.inf

  #
  # Crashdump Driver
  #
  #QcomPkg/Drivers/CrashDumpDxe/CrashDumpDxe.inf {
  #  <LibraryClasses>
  #    PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  #}

  #
  # PlatformInfo Driver
  #
  QcomPkg/Drivers/PlatformInfoDxe/PlatformInfoDxe.inf


  #
  # Button Driver
  #
  QcomPkg/Drivers/ButtonsDxe/ButtonsDxe.inf {
   <LibraryClasses>
    #ButtonsDxe Private Library
   ButtonsLib|QcomPkg/SDM670Pkg/Library/ButtonsLib/ButtonsLib.inf
  }

  #
  # Touch Firmware Update
  #


  #
  # PMIC Driver
  #QcomPkg/Drivers/PmicDxe/NullLib/PmicDxeNull.inf
  #
  QcomPkg/Drivers/PmicDxe/PmicDxeLa.inf {
    <LibraryClasses>
     #PMIC Library
    PmicCoreLib|QcomPkg/SDM670Pkg/Library/PmicLib/core/la/PmicCoreLibLa.inf
  }

  #
  # Charger extended Driver
  #
  QcomPkg/Drivers/ChargerExDxe/ChargerExDxe.inf

  #
  # Charger Driver
  #
  QcomPkg/Drivers/QcomChargerDxe/QcomChargerDxeLA.inf

  #
  # ADC Driver
  #
  QcomPkg/Drivers/AdcDxe/AdcDxe.inf {
    <LibraryClasses>
    # ADC Private Library
    AdcCoreLib|QcomPkg/Library/AdcLib/build/AdcCoreLib.inf
  }

  #
  # TSENS Driver
  #
  QcomPkg/Drivers/TsensDxe/TsensDxe.inf {
    <LibraryClasses>
    # TSENS Private Library
    TsensCoreLib|QcomPkg/Library/TsensLib/TsensCoreLib.inf
    TsensHwLib|QcomPkg/Library/TsensLib/hw/v2/TsensHwLib.inf
  }

  #
  # Limits Driver
  #
  QcomPkg/Drivers/LimitsDxe/LimitsDxe.inf {
    <LibraryClasses>
    LimitsTargetLib|QcomPkg/SDM670Pkg/Library/LimitsTargetLib/LimitsTargetLib.inf
  }

  #
  # MpPowerDxe
  #
  #QcomPkg/Drivers/MpPowerDxe/MpPowerDxe.inf

  #
  # QBlizzard
  #
#  QcomPkg/Drivers/MpCoreDxe/MpCoreDxe.inf {
#    <LibraryClasses>
#    AuxKernelLib|QcomPkg/Library/AuxKernelLib/AuxKernelLib.inf
#  }
#  QcomPkg/Drivers/MpAKServicesDxe/MpAKServicesDxe.inf {
#    <LibraryClasses>
#    AuxKernelLib|QcomPkg/Library/AuxKernelLib/AuxKernelLib.inf
#  }
#  QcomPkg/Drivers/SampleMpTest/QBlizzardTest.inf {
#    <LibraryClasses>
#    DebugLib|QcomPkg/Library/DebugLib/DebugLib.inf
#    UefiDriverEntryPoint|QcomPkg/Library/AKUefiDriverEntryPoint/AKUefiDriverEntryPoint.inf
#    SerialPortLib|QcomPkg/Library/AKSerialPortLib/AKSerialPortLib.inf
#    UefiBootServicesTableLib|QcomPkg/Library/AKBootServicesTableLib/AKBootServicesTableLib.inf
#    DxeServicesTableLib|QcomPkg/Library/AKDxeServicesTableLib/AKDxeServicesTableLib.inf
#    AuxKernelLib|QcomPkg/Library/AuxKernelLib/AuxKernelLib.inf
#  }
#  QcomPkg/Drivers/SampleMpTest/SampleMpTest.inf {
#    <LibraryClasses>
#    DebugLib|QcomPkg/Library/DebugLib/DebugLib.inf
#    UefiDriverEntryPoint|QcomPkg/Library/AKUefiDriverEntryPoint/AKUefiDriverEntryPoint.inf
#    SerialPortLib|QcomPkg/Library/AKSerialPortLib/AKSerialPortLib.inf
#    UefiBootServicesTableLib|QcomPkg/Library/AKBootServicesTableLib/AKBootServicesTableLib.inf
#    DxeServicesTableLib|QcomPkg/Library/AKDxeServicesTableLib/AKDxeServicesTableLib.inf
#    AuxKernelLib|QcomPkg/Library/AuxKernelLib/AuxKernelLib.inf
#  }

  #
  # MMCHS Drivers
  #
  QcomPkg/Drivers/SdccDxe/SdccDxe.inf

  #
  # UFS Drivers
  #
  QcomPkg/Drivers/UFSDxe/UFSDxe.inf

  #
  # Debug Protocol
  #
  QcomPkg/Drivers/SerialDxe/SerialDxe.inf {
    <LibraryClasses>
      SerialPortLib|QcomPkg/Library/SerialPortLib/SerialPortLib.inf
      UartLib|QcomPkg/Library/UartQupv3Lib/UartCoreLib.inf
      UartSettingsLib|QcomPkg/SDM670Pkg/Settings/UART/UartSettingsLib.inf
  }

  #
  # Bds
  #
  QcomPkg/Drivers/QcomBds/QcomBds.inf

  #QCOM Charger app
  QcomPkg/Application/QcomChargerApp/QcomChargerApp.inf {
  	<PcdsFixedAtBuild>
    gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
  }


  # EBL Shell
  EmbeddedPkg/Ebl/Ebl.inf


################################################################################
#
# UserExtensions.Buildit.BuildInfo - Extra info for buildit.py
#
################################################################################
[UserExtensions.Buildit.BuildInfo]
  COMPILER_ARCH = AARCH64
  OUTPUT_NAME   = uefi.elf
  SIGN_FD       = uefi
  MERGE         = xbl.elf
  SIGN          = sbl1
  COMPILER_WIN  = CLANG39WIN
  COMPILER_LINUX = CLANG39LINUX
