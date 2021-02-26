# @file Apriori.fdf.inc
# Apriori for LA SDM845
#
# Copyright (c) 2015 -2017 Qualcomm Technologies, Inc. All rights reserved.
# Portions Copyright (c) 2009, Apple Inc. All rights reserved.
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#
#
#==============================================================================
#                              EDIT HISTORY
#
#
# when       who     what, where, why
# --------   ---     ----------------------------------------------------------
# 07/13/17   dc      Add UsbPwrCtrlDxe support
# 03/23/17   jp      Enabled Hash and RNG drivers
# 03/01/17   vk      Remove non LA drivers
# 04/18/16   clm     Add limits.
# 03/09/16   va      updating PmicDxe for La
# 03/04/16   bh      Move Fat/English/FVSimple under UFS/Sdcc
# 01/27/16   sm      Renamed ChargerAppLADxe to QcomChargerDxe
# 06/24/15   bh      Initial version
#
#==============================================================================

########################### APRIORI Start ####################################################
## Load order for drivers ##
## NOTE: After the APRIORI End section is where drivers must be added in order for them to
##       be included in the FD. The APRIORI just dictates a load order regardless of whether
##       the drivers are actually included in the FD or not

  APRIORI DXE {
        INF MdeModulePkg/Core/Dxe/DxeMain.inf

#
#    Arch Protocols
#
#    EfiSecurityArchProtocolGuid,
#    EfiCpuArchProtocolGuid,
#    EfiMetronomeArchProtocolGuid,
#    EfiTimerArchProtocolGuid,
#    EfiBdsArchProtocolGuid,
#    EfiWatchdogTimerArchProtocolGuid,
#    EfiRuntimeArchProtocolGuid,
#    EfiVariableArchProtocolGuid,
#    EfiVariableWriteArchProtocolGuid,
#    EfiCapsuleArchProtocolGuid,
#    EfiMonotonicCounterArchProtocolGuid,
#    EfiResetArchProtocolGuid,
#    EfiRealTimeClockArchProtocolGuid,
#

        INF QcomPkg/Drivers/EnvDxe/EnvDxe.inf

# Report Status Code Handler and Router
        INF MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
        INF MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf

#  Arch Protocol :  EfiRuntimeArchProtocolGuid,
        INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf

#  Arch Protocol :  EfiCpuArchProtocolGuid,
        INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
        INF ArmPkg/Drivers/ArmGic/ArmGicDxe.inf

#  Arch Protocol :  EfiTimerArchProtocolGuid,
        INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf

#  Arch Protocol :  EfiWatchdogTimerArchProtocolGuid,
        INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf

#  Arch Protocol :  EfiSecurityArchProtocolGuid,
        INF QcomPkg/Drivers/SecurityDxe/SecurityDxe.inf
        INF QcomPkg/Drivers/ASN1X509Dxe/ASN1X509Dxe.inf
        INF QcomPkg/Drivers/SecRSADxe/SecRSADxe.inf
        INF QcomPkg/Drivers/VerifiedBootDxe/VerifiedBootDxe.inf
        #INF QcomPkg/Drivers/VerifiedBootNullDxe/VerifiedBootNullDxe.inf (PreSil)

        # NOTE: DAL has dependency on SMEM
        INF QcomPkg/Drivers/SmemDxe/SmemDxe.inf
        INF QcomPkg/Drivers/DALSYSDxe/DALSYSDxe.inf

        INF QcomPkg/Drivers/HWIODxe/HWIODxe.inf
        INF QcomPkg/Drivers/ChipInfoDxe/ChipInfoDxe.inf
        INF QcomPkg/Drivers/PlatformInfoDxe/PlatformInfoDxe.inf

        INF QcomPkg/Drivers/HALIOMMUDxe/HALIOMMUDxe.inf

#  Arch Protocol :  EfiMetronomeArchProtocolGuid,
        INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf


        INF QcomPkg/Drivers/GLinkDxe/GLinkDxe.inf
        INF QcomPkg/Drivers/ULogDxe/ULogDxe.inf
        INF QcomPkg/Drivers/CmdDbDxe/CmdDbDxe.inf
        INF QcomPkg/Drivers/NpaDxe/NpaDxe.inf
        INF QcomPkg/Drivers/RpmhDxe/RpmhDxe.inf
        INF QcomPkg/Drivers/PdcDxe/PdcDxe.inf
        INF QcomPkg/Drivers/ClockDxe/ClockDxe.inf
        INF QcomPkg/Drivers/CPRDxe/CPRDxe.inf

        INF QcomPkg/Drivers/DiskIoDxe/DiskIoDxe.inf
        INF QcomPkg/Drivers/PartitionDxe/PartitionDxe.inf
        INF QcomPkg/Drivers/SdccDxe/SdccDxe.inf
        INF QcomPkg/Drivers/UFSDxe/UFSDxe.inf
        INF QcomPkg/Drivers/TzDxe/TzDxe.inf
        INF FatPkg/EnhancedFatDxe/Fat.inf
        INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
        INF MdeModulePkg/Universal/FvSimpleFileSystemDxe/FvSimpleFileSystemDxe.inf


#QDSS Driver (Debugtrace)
        INF QcomPkg/Drivers/QdssDxe/QdssDxe.inf

#  Arch Protocol :  EfiVariableArchProtocolGuid,
#  Arch Protocol :  EfiVariableWriteArchProtocolGuid,
        INF MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf

        INF QcomPkg/Drivers/DppDxe/DppDxe.inf

#  Arch Protocol :  EfiMonotonicCounterArchProtocolGuid,
        INF QcomPkg/Drivers/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf

#  Arch Protocol :  EfiResetArchProtocolGuid,
        INF QcomPkg/Drivers/ResetRuntimeDxe/ResetRuntimeDxe.inf

#  Arch Protocol :  EfiRealTimeClockArchProtocolGuid,
        INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf

        INF MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
        INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf

#  Arch Protocol :  EfiCapsuleArchProtocolGuid,
        INF MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  #INF QcomPkg/Drivers/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf

#  Arch Protocol :  EfiBdsArchProtocolGuid,
        INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
        INF QcomPkg/Drivers/FontDxe/FontDxe.inf
        #INF MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
        INF QcomPkg/Drivers/QcomBds/QcomBds.inf

        INF QcomPkg/Drivers/TLMMDxe/TLMMDxe.inf

        INF QcomPkg/Drivers/I2CDxe/I2CDxe.inf
        INF QcomPkg/Drivers/SPMIDxe/SPMIDxe.inf
        INF QcomPkg/Drivers/SPIDxe/SPIDxe.inf


        INF QcomPkg/Drivers/PmicDxe/PmicDxeLa.inf
        INF QcomPkg/Drivers/AdcDxe/AdcDxe.inf
		INF QcomPkg/Drivers/UsbPwrCtrlDxe/UsbPwrCtrlDxe.inf
        INF QcomPkg/Drivers/QcomChargerDxe/QcomChargerDxeLA.inf
        INF QcomPkg/Drivers/ChargerExDxe/ChargerExDxe.inf

        INF QcomPkg/Drivers/UsbfnDwc3Dxe/UsbfnDwc3Dxe.inf
        INF QcomPkg/Drivers/UsbConfigDxe/UsbConfigDxe.inf

        #
        # Debug Protocol
        #
        INF QcomPkg/Drivers/SimpleTextInOutSerialDxe/SimpleTextInOutSerial.inf
        INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
        INF MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
        INF MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf

        #
        #  Moved ChargerDxe/AdcDxe/ButtonsDxe above DisplayDxe
        #
        INF QcomPkg/Drivers/ButtonsDxe/ButtonsDxe.inf
        INF QcomPkg/Drivers/TsensDxe/TsensDxe.inf


        INF QcomPkg/Drivers/DisplayDxe/DisplayDxe.inf
        INF QcomPkg/Drivers/LimitsDxe/LimitsDxe.inf

        INF QcomPkg/Drivers/GpiDxe/GpiDxe.inf

#
#     Critical modules until here,
#
#     Non critical parts follow from here
#
        #INF QcomPkg/Drivers/MpPowerDxe/MpPowerDxe.inf
        INF QcomPkg/Drivers/HashDxe/HashDxe.inf
        INF QcomPkg/Drivers/RNGDxe/RngDxe.inf
		INF QcomPkg/Drivers/DDRInfoDxe/DDRInfoDxe.inf
  }

#
########################### APRIORI End ####################################################
#

