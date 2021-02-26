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

#include <AsmMacroIoLibV8.h>

.text
.align 3
GCC_ASM_EXPORT(ArmDebugReadDSCR)
GCC_ASM_EXPORT(ArmDebugDTRTx)
GCC_ASM_EXPORT(ArmDebugDTRRx)

#------------------------------------------------------------------------------

ASM_PFX(ArmDebugReadDSCR):
  mrs     x0, mdccsr_el0
  ret

ASM_PFX(ArmDebugDTRTx):
  msr     dbgdtrtx_el0, x0
  ret

ASM_PFX(ArmDebugDTRRx):
  mrs     x0, dbgdtrrx_el0     
  ret

ASM_FUNCTION_REMOVE_IF_UNREFERENCED
