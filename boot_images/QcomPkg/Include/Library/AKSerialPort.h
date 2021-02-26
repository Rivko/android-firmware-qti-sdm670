/** @file

  Copyright (c) 2015, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/11/15   ai      Created
=============================================================================*/
#ifndef __AK_SERIAL_PORT_H__
#define __AK_SERIAL_PORT_H__

#include <Uefi.h>
#include <Library/MpIoMsgInfo.h>

#define SERIALPORT      MPIO_PORT('S','e','r','l','P','o','r','t')

#define AK_SERIALPORT_SERVICE_INITIALIZE    0x0
#define AK_SERIALPORT_SERVICE_READ          0x1
#define AK_SERIALPORT_SERVICE_WRITE         0x2
#define AK_SERIALPORT_SERVICE_POLL          0x3
#define AK_SERIALPORT_SERVICE_FLUSH         0x4
#define AK_SERIALPORT_SERVICE_ENSYNCIO      0x5

#endif // __AK_SERIAL_PORT_H__

