/** @file
*  Arm Debug registers on ARM PLatforms
*
*  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*
*  Copyright (c) 2011-2013, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#ifndef __ARMDEBUG_H__
#define __ARMDEBUG_H__

#include <Base.h>
#include <Library/BaseLib.h>

#define ARM_DEBUG_DSCR_RXFULL (1<<30)
#define ARM_DEBUG_DSCR_TXFULL (1<<29)

UINTN
ArmDebugReadDSCR (
  VOID
);

VOID
ArmDebugDTRTx (
  IN  UINTN                     Data
);

UINTN
ArmDebugDTRRx (
  VOID
);

#endif
