#==============================================================================
# @file SDM670Pkg_Loader.dsc
# SDM670 Loader package.
#
# Copyright (c) 2017, 2020 Qualcomm Technologies, Inc. All rights reserved.
# Portions Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#==============================================================================
[LibraryClasses.common]
  # EDK2 Libraries
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf
  ArmLib|ArmPkg/Library/ArmLib/AArch64/AArch64LibSec.inf
  ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerPhyCounterLib/ArmGenericTimerPhyCounterLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|ArmPkg/Library/BaseMemoryLibStm/BaseMemoryLibStm.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf

  # Qcom Generic Libraries
  AdcLoaderLib|QcomPkg/Library/AdcLib/build/AdcLoaderLib.inf
  #Bam Lib
  BamLoaderLib|QcomPkg/Library/BamLib/BamLoaderLib.inf
  BamTargetLib|QcomPkg/SDM670Pkg/Library/BamTargetLib/BamTargetLib.inf
  BootDisplayLib|QcomPkg/Library/BootDisplayLib/BootDisplayLib.inf
  BusyWaitLib|QcomPkg/Library/BusyWaitLib/BusyWaitLib.inf
  ChipInfoLoaderLib|QcomPkg/Library/ChipInfoLib/ChipInfoLoader.inf
  CoreUtilsLib|QcomPkg/Library/CoreUtilsLib/CoreUtilsLib.inf
  CacheMaintenanceLib|ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  DALCommonLoaderLib|QcomPkg/Library/DALCommonLib/DALCommonLoaderLib.inf
  DALFwkLib|QcomPkg/Library/DALFwkLib/DALFwkLib.inf
  FontLib|QcomPkg/Library/FontLib/FontLib.inf
  HFATLib|QcomPkg/Library/HFATLib/HFATLib.inf
  ColdplugEfsLib|QcomPkg/Library/ColdplugLib/ColdplugEfsLib.inf
  IcbCfgLib|QcomPkg/Library/ICBLib/ICBLib.inf
  ImageVersionLib|QcomPkg/Library/ImageVersionLib/ImageVersionLib.inf
  LoaderCacheLib|QcomPkg/XBLLoader/LoaderCacheLib.inf
  XBLConfigLib|QcomPkg/Library/XBLConfig/XBLConfigLib.inf
  XcfgLoaderElf|QcomPkg/Library/XBLConfig/XcfgLoaderElfLib.inf
  MuslLib|QcomPkg/Library/MuslLib/MuslLib.inf
  NULL|QcomPkg/Library/MuslLib/MuslLib.inf
  NULL|QcomPkg/Library/StackCanaryBaseLib/StackCanaryBaseLibLoader.inf
  PlatformInfoLoaderLib|QcomPkg/Library/PlatformInfoLib/PlatformInfoLoader.inf
  QusbLib|QcomPkg/Library/QusbLib/QusbLib.inf
  ROMFSLib|QcomPkg/Library/EfsLib/ROMFSLib.inf
  RailwayLib|QcomPkg/Library/RailwayLib/RailwayLib.inf
  SPILib|QcomPkg/Library/SPILib/SPILib.inf
  I2CLib|QcomPkg/Library/I2CLib/I2CLibLoader.inf
  I2CFWImageLib|QcomPkg/SDM670Pkg/Settings/I2C/I2CFWImageLib.inf
  SPIFWImageLib|QcomPkg/SDM670Pkg/Settings/SPI/SPIFWImageLib.inf
  SPMIApiLib|QcomPkg/Library/SPMIApiLib/SPMIApiLib.inf
  SdccCommonLib|QcomPkg/Library/SdccCommonLib/SdccCommonLib.inf
  SdccHalLib|QcomPkg/Library/SdccCommonLib/SdccHalLib/SdccHalLib.inf
  StackCanaryBaseLib|QcomPkg/Library/StackCanaryBaseLib/StackCanaryBaseLibLoader.inf
  ClockLoaderLib|QcomPkg/Library/ClockLoaderLib/ClockLoaderLib.inf
  UartLib|QcomPkg/Library/UartQupv3Lib/UartLoaderLib.inf
  UartSettingsLib|QcomPkg/SDM670Pkg/Settings/UART/UartSettingsLib.inf
  ServicesUtilsLib|QcomPkg/Library/ServicesLib/ServicesUtilsLib.inf
  SmemLib|QcomPkg/Library/SmemLib/build/SmemLibXBLLoader.inf
  SpiNorCommonLib|QcomPkg/Library/SpiNorCommonLib/SpiNorCommonLib.inf
  TimetickLib|QcomPkg/Library/TimetickLibB/TimetickLib.inf
  UfsJtagLib|QcomPkg/Library/UfsCommonLib/UfsJtagLib.inf
  UfsLoaderLib|QcomPkg/Library/UfsCommonLib/UfsLoaderLib.inf

  # TSENS Libraries
  TsensLoaderLib|QcomPkg/Library/TsensLib/TsensLoaderLib.inf
  TsensHwLib|QcomPkg/Library/TsensLib/hw/v2/TsensHwLib.inf
  TsensTargetLib|QcomPkg/SDM670Pkg/Library/TsensTargetLib/TsensTargetLib.inf
  BootTempCheckLib|QcomPkg/Library/BootTempCheckLib/BootTempCheckLib.inf

  # Security Libs (TBD: To remove, no longer needed)
  PrngLib|QcomPkg/SDM670Pkg/Library/RNGLoaderLib/RNGLibHw.inf
  UIELib|QcomPkg/Library/NullLibs/UIELibNull/UIELibNull.inf
  AptLib|QcomPkg/Library/NullLibs/AptLibNull/AptLibNull.inf
  CryptoLib|QcomPkg/SDM670Pkg/Library/CryptoLib/CryptoLib.inf  
  CryptoRDILib|QcomPkg/SDM670Pkg/Library/CryptoInterfaceLib/CryptoRDILib.inf 
  CryptoLoaderILib|QcomPkg/SDM670Pkg/Library/CryptoInterfaceLib/CryptoLoaderILib.inf    

  # Target Libraries
  CPRTargetLib|QcomPkg/SDM670Pkg/Library/CPRTargetLib/Loader/CPRTargetLib.inf
  XBLLoaderLib|QcomPkg/SDM670Pkg/Library/XBLLoaderLib/XBLLoaderLib.inf
  XBLCommonLib|QcomPkg/SDM670Pkg/Library/XBLLoaderLib/XBLCommonLib.inf
  ChipInfoTargetLib|QcomPkg/SDM670Pkg/Library/ChipInfoTargetLib/ChipInfoTargetLib.inf
  DDRTargetLib|QcomPkg/SDM670Pkg/Library/DDRTargetLib/DDRTargetLib.inf
  DSFTargetLib|QcomPkg/SDM670Pkg/Library/DSFTargetLib/DSFTargetLib.inf
  ClockTargetLib|QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockTargetLib.inf
  PmicLib|QcomPkg/SDM670Pkg/Library/PmicLib/PmicLib_lg.inf
  PmicLoaderLib|QcomPkg/SDM670Pkg/Library/PmicLib/loader/PmicLoaderLib_lg.inf
  IcbTargetLib|QcomPkg/SDM670Pkg/Library/ICBTargetLib/ICBTargetLib.inf
  TLMMTargetLib|QcomPkg/SDM670Pkg/Library/TLMMTargetLib/TLMMTargetLib.inf
  
  UfsTargetBootLib|QcomPkg/SDM670Pkg/Library/UfsTargetLib/UfsTargetBootLib.inf
  UfsTargetJtagLib|QcomPkg/SDM670Pkg/Library/UfsTargetLib/UfsTargetJtagLib.inf

  TlmmLib|QcomPkg/Library/TlmmLib/TlmmLib.inf
  QusbDciCommonLib|QcomPkg/SDM670Pkg/Library/QusbDciCommonLib/QusbDciCommonLib.inf
  UsbSharedLib|QcomPkg/SDM670Pkg/Library/UsbSharedLib/UsbSharedLibLoader.inf
  SmemTargetLib|QcomPkg/SDM670Pkg/Library/SmemTargetLib/SmemTargetLib.inf
  DisplayLib|QcomPkg/Library/NullLibs/DisplayLibNull/DisplayLibNull.inf
  UsbWrapperLayerLib|QcomPkg/SDM670Pkg/Library/UsbWrapperLayerLib/UsbWrapperLayerLib.inf

  # DAL Libraries
  DALGenLibLoader|QcomPkg/SDM670Pkg/Settings/DALConfig/DALGenLibLoader.inf
  DALGenLibTools|QcomPkg/SDM670Pkg/Settings/DALConfig/DALGenLibTools.inf
  DALConfigLoaderLib|QcomPkg/SDM670Pkg/Settings/DALConfig/DALConfigLoaderLib_lg.inf
  DALConfigToolsLib|QcomPkg/SDM670Pkg/Settings/DALConfig/DALConfigToolsLib.inf

  # Railway Libraries
  RailwayTargetLib|QcomPkg/SDM670Pkg/Library/RailwayTargetLib/RailwayTargetLib_lg.inf

  # SPMI Library
  SPMILib|QcomPkg/SDM670Pkg/Library/SPMILib/SPMILib.inf

  # SDCC
  SdccClockLib|QcomPkg/SDM670Pkg/Library/SdccClockLib/SdccLoaderClockLib.inf
  SdccTargetLib|QcomPkg/SDM670Pkg/Library/SdccTargetLib/SdccLoaderTargetLib.inf

[BuildOptions.AARCH64]
  GCC:*_*_AARCH64_ARCHCC_FLAGS  == -mllvm -inline-threshold-multicaller-size=20 -mtune=cortex-a53 -I$(WORKSPACE)/QcomPkg/SDM670Pkg/Include -I$(WORKSPACE)/QcomPkg/SDM670Pkg/Library/XBLLoaderLib -include $(WORKSPACE)/QcomPkg/SDM670Pkg/Include/Target_cust.h -DMDEPKG_NDEBUG -DQCOM_EDK2_PATCH -DGROOT_ONLY $(FEATURE_FLAGS)
  GCC:*_*_AARCH64_ARCHPP_FLAGS  == -I$(WORKSPACE)/QcomPkg/SDM670Pkg/Include -I$(WORKSPACE)/QcomPkg/SDM670Pkg/Library/XBLLoaderLib -include $(WORKSPACE)/QcomPkg/SDM670Pkg/Include/Target_cust.h -DMDEPKG_NDEBUG -DQCOM_EDK2_PATCH -DGROOT_ONLY $(FEATURE_FLAGS)

!if $(PRODMODE) == "PRODMODE"
  GCC:*_*_*_CC_FLAGS = -DPRODMODE
!endif

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]

[PcdsFixedAtBuild.common]

#
# Target Specific PCDs
#
  #################################
  ## PcdSmemBaseAddress is currently used by smem library
  #################################
  gQcomTokenSpaceGuid.PcdSmemBaseAddress|0x86000000  #64-bit smem addr
  gQcomTokenSpaceGuid.PcdSmemSize|0x00200000         # 2MB
  ## Generic SMEM information that SMEM lib might need
  ## LSB represents chip family, other bytes are unused for now.
  gQcomTokenSpaceGuid.PcdSmemInformation|0x00000005  # 5 = Nazgul

  # Timers
  gArmTokenSpaceGuid.PcdArmArchTimerFreqInHz|19200000
  gArmTokenSpaceGuid.PcdArmArchTimerSecIntrNum|29
  gArmTokenSpaceGuid.PcdArmArchTimerIntrNum|30
  gArmTokenSpaceGuid.PcdGicDistributorBase|0x9BC0000

  # Below PcdGicInterruptInterfaceBase is used for supporting the GICv2 Emulation in the Hyper visor
  # Which need mGicInterruptInterfaceBase to be initialized to 0x17A60000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase|0x17A60000
  
  gQcomTokenSpaceGuid.PcdQTimerAC_BasePhys|0x17C90000
  gQcomTokenSpaceGuid.PcdQTimerV1BasePhys|0x17CA0000
  gQcomTokenSpaceGuid.PcdQTimerV2BasePhys|0x17CB0000
  gQcomTokenSpaceGuid.PcdQTimerFrame|0
  gQcomTokenSpaceGuid.PcdQTimerEnabled|1

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
  gQcomTokenSpaceGuid.SDCCEventBasedSDDetectEnabled|FALSE
  gQcomTokenSpaceGuid.SDCCDmaEnabled|TRUE
  gQcomTokenSpaceGuid.SDCCEmmcHwResetEnabled|TRUE
  gQcomTokenSpaceGuid.SDCCMaxSlotNumber|2
  gQcomTokenSpaceGuid.SDCCSdhciEnable|TRUE
  ## SDCC eMMC speed mode - DDR50: 0, HS200: 1, HS400: 2 SDR: 3 ##
  gQcomTokenSpaceGuid.SDCCEmmcSpeedMode|2

