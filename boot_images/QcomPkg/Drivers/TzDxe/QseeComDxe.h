/** @file  
               QseeComDxe.h

GENERAL DESCRIPTION
   Declare interface for QseeCom interface.

	
  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All
  Rights Reserved. Qualcomm Technologies Proprietary and
  Confidential.

*/

/*===========================================================================
                             EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 01/08/16   RJ      Initial version

===========================================================================*/

#ifndef __QSEE_COM_DXE_H__
#define __QSEE_COM_DXE_H__

#include <TzeLoaderDxe.h>
/*
 * TZ Log ring buffer position
 */
struct tzdbg_log_pos_t {
  UINT16 wrap;
  UINT16 offset;
};

 /*
 * TZ Log ring buffer
 */
typedef struct tzdbg_log_t {
  struct tzdbg_log_pos_t log_pos;
  UINT8 log_buf[];
}tzdbg_log_t;

#define QSEE_LOG_BUF_SIZE (4096)

EFI_STATUS QseeComProtocolInit(EFI_HANDLE ImageHandle,EFI_SYSTEM_TABLE *SystemTable);

#endif  /* __QSEE_COM_DXE_H__  */

