#------------------------------------------------------------------------------ 
#
# Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
# Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
# Copyright (c) 2011-2013, ARM Limited. All rights reserved.
#
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#------------------------------------------------------------------------------

#include <AsmMacroIoLib.h>

  INCLUDE AsmMacroIoLib.inc

EXPORT ArmDebugReadDSCR
EXPORT ArmDebugDTRTx
EXPORT ArmDebugDTRRx

  AREA ArmDebug, CODE, READONLY

#------------------------------------------------------------------------------

ArmDebugReadDSCR
  mrc     p14,0,R0,c0,c1,0
  bx      LR

ArmDebugDTRTx
  mcr     p14,0,R0,c0,c5,0
  bx      LR

ArmDebugDTRRx
  mrc     p14,0,R0,c0,c5,0     
  bx      LR

  END

