#==============================================================================
# @file Loader.dsc
# SDM670 Loader package.
#
# Copyright (c) 2017,2018 Qualcomm Technologies, Inc. All rights reserved.
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

  OUTPUT_DIRECTORY               = Build/SDM670LA_Loader

!include QcomPkg/SDM670Pkg/LA/Common.dsc.inc

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]
  QcomPkg/XBLLoader/XBLLoader.inf {
    <LibraryClasses>
      DevPrgLLib|QcomPkg/Library/NullLibs/DevPrgLLibNull/DevPrgLLibNull.inf
      DevPrgDLib|QcomPkg/Library/NullLibs/DevPrgDLibNull/DevPrgDLibNull.inf
      BootDdrTrainingLib|QcomPkg/XBLLoader/BootDdrTrainingLib.inf
      BootDebugLib|QcomPkg/XBLLoader/BootDebugLib.inf
      QusbLdrLib|QcomPkg/SDM670Pkg/Library/QusbLdrLib/QusbLdrLib.inf
      VSenseTargetLib|QcomPkg/Library/NullLibs/VSenseTargetLibNull/VSenseTargetLibNull.inf
      LoaderAptLib|QcomPkg/Library/LoaderAptLib/LoaderAptLib.inf
      SdccCommonLib|QcomPkg/Library/SdccCommonLib/SdccCommonReducedLib.inf
      ProcLib|QcomPkg/Library/ProcLib/ProcLib.inf
      CeLib|QcomPkg/Library/NullLibs/CryptoDriversLibNull/CeLibNull.inf
      ColdplugLib|QcomPkg/Library/ColdplugLib/ColdplugLib.inf

      # Only required for XBL loader
      #CPRTargetLib|QcomPkg/Library/NullLibs/PowerLibNull/CprLibNull.inf

      #This PCD values should match with the ones defined in boot_sbl_shared.h for sbl_build_type.
      <PcdsFixedAtBuild>
        gQcomTokenSpaceGuid.PcdBuildType|0x0
        gQcomTokenSpaceGuid.SDCCDmaEnabled|TRUE
        gQcomTokenSpaceGuid.SDCCSdhciEnable|TRUE
        ## SDCC eMMC speed mode - DDR50: 0, HS200: 1, HS400: 2 SDR: 3 ##
        gQcomTokenSpaceGuid.SDCCEmmcSpeedMode|2
  }

