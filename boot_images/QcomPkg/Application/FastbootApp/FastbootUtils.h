/** @file FastbootApp.h

  Copyright (c) 2014, Qualcomm Technologies, Inc . All rights reserved.<BR>
  Copyright (c) 2013-2014, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 ----------   ---     -----------------------------------------------------------
 09/12/2014   bs      Made Fastboot standalone app 
 08/15/2014   ml      Clean up
 08/15/2014   ml      Initial version

=============================================================================*/

#ifndef __APP_FASTBOOT_H
#define __APP_FASTBOOT_H

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

#define ANDROID_FASTBOOT_VERSION "0.4"



/* Returns the number of bytes left in the
 * download. You must be expecting a download to 
 * call this  function
 */
UINTN GetXfrSize(VOID);
extern EFI_HANDLE fabImageHandle;

/* Registers commands and publishes Variables */
EFI_STATUS FastbootEnvSetup(VOID *xfer_buffer, UINT32 max);

/* register a command handler 
 * - command handlers will be called if their prefix matches
 * - they are expected to call fastboot_okay() or fastboot_fail()
 *   to indicate success/failure before returning
 */
VOID FastbootRegister(CONST CHAR8 *prefix,
  VOID (*handle)(CONST CHAR8 *arg, 
  VOID *data, UINT32 size)
  );

/* Only callable from within a command handler
 * One of thse functions must be called to be a valid command
 */
VOID FastbootOkay(CONST CHAR8 *result);
VOID FastbootFail(CONST CHAR8 *reason);

/* USB Buffers Allocated in FastbootMain */
extern VOID *gTxBuffer;
extern VOID *gRxBuffer;

/* Initializes the Fastboot App */
EFI_STATUS
FastbootAppInit ( VOID );

/* Uninitializes the Fastboot App */
EFI_STATUS
FastbootAppUnInit( VOID );
  
/* Called when a message/download data passed to the app */
VOID DataReady (IN UINTN Size, IN VOID *Data);

#endif
