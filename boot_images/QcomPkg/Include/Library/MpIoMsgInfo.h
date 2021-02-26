/** @file MpIoMsgInfo.h

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/13/15   ai      Initial Version

=============================================================================*/
#ifndef __MPIO_MESSAGE_INFO_H__
#define __MPIO_MESSAGE_INFO_H__

#include <Uefi.h>

#define MPIO_PORT(A, B, C, D, E, F, G, H) \
     (         (A)        | \
      (        (B) << 8 ) | \
      (        (C) << 16) | \
      (        (D) << 24) | \
      ((UINT64)(E) << 32) | \
      ((UINT64)(F) << 40) | \
      ((UINT64)(G) << 48) | \
      ((UINT64)(H) << 56))

#define MPIO_MESSAGE_INFO_REVISION     0x00010000

typedef struct {
  UINT32                  Version;      /* Structure revision */
  UINT64                  Port;         /* MPIO_PORT */
  UINT32                  Service;      /* Port specific subport */
  UINT32                  DataLength;   /* Lenth of the message excluding header */
  UINT8                   Message[0];   /* Start of Message */
} MPIO_MESSAGE_INFO;


#endif //__MPIO_MESSAGE_INFO_H__

