#==============================================================================
# @file Loader.dsc
# SDM670 Loader package.
#
# Copyright (c) 2017-2018 Qualcomm Technologies, Inc. All rights reserved.
#==============================================================================

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = SDM670Pkg
  PLATFORM_GUID                  = AA99686E-4D75-3829-5220-F4874BA2F216
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

  OUTPUT_DIRECTORY               = Build/SDM670LG_Loader

!include QcomPkg/SDM670Pkg/LG/Common.dsc.inc

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
#
# SEC
#
  QcomPkg/SDM670Pkg/Library/XBLRamDumpLib/XBLRamDumpLib.inf {
    <LibraryClasses>
      QusbTargetLib|QcomPkg/SDM670Pkg/Library/QusbTargetLib/QusbTargetLib.inf
      SdccCommonLib|QcomPkg/Library/SdccCommonLib/SdccCommonRamdumpLib.inf
      ClockTargetLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
      ClockRamDumpLib|QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockRamDumpLib.inf
      ProcLib|QcomPkg/Library/ProcLib/ProcLib.inf
      TlmmLib|QcomPkg/Library/TlmmLib/TlmmLib.inf
      TLMMTargetLib|QcomPkg/SDM670Pkg/Library/TLMMTargetLib/TLMMTargetLib.inf
      IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
      HobLib|EmbeddedPkg/Library/PrePiHobLib/PrePiHobLib.inf
      PrePiHobListPointerLib|EmbeddedPkg/Library/PrePiHobListPointerLib/PrePiHobListPointerLib.inf
      ShLib|QcomPkg/Library/ShLib/ShLibMgr.inf
      RNGLib|QcomPkg/SDM670Pkg/Library/RNGLoaderLib/RNGLibHw.inf
      HashLib|QcomPkg/SDM670Pkg/Library/HashLibLoader/HashLibHw.inf
      CryptoLib|QcomPkg/SDM670Pkg/Library/CryptoLib/CryptoLib.inf
	  
      TargetResetLib|QcomPkg/Library/NullLibs/TargetResetLibNull/TargetResetLib.inf
      ReportStatusCodeLib|QcomPkg/Library/NullLibs/PeiReportStatusCodeLibFrameworkNull/PeiReportStatusCodeLibNull.inf
      PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
      SerialPortLib|QcomPkg/Library/SerialPortLib/SerialPortLib.inf
      DebugLib|QcomPkg/Library/DebugLib/DebugLib_Sec.inf

     # Display Libraries
      DisplayLib|QcomPkg/Library/DisplayLib/DisplayLib.inf

      # Display HAL Libraries
      HALDSILib|QcomPkg/Library/HALDSILib/HALDSILib.inf
      ExternalDisplayLib|QcomPkg/Library/ExternalDisplayLib/ExternalDisplayLib.inf
      HALMDSSLib|QcomPkg/Library/HALMDSSLib/HALMDSS4xx.inf
      # MDP Libraries
      MDPLibBoot|QcomPkg/Library/MDPLib/MDPLibBoot.inf
      MDPPlatformLibBoot|QcomPkg/SDM670Pkg/Library/MDPPlatformLibBoot/MDPPlatformLibBoot.inf
	  
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.SDCCDmaEnabled|TRUE
        gQcomTokenSpaceGuid.SDCCSdhciEnable|TRUE
        gQcomTokenSpaceGuid.SDCCEmmcSpeedMode|0
  }

