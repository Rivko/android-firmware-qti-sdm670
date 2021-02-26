#ifndef _GPIOINTINTERFACE_H_
#define _GPIOINTINTERFACE_H_
/*===========================================================================

  GPIOInt Settings   

DESCRIPTION
  This file contains enums, constants and structure defintions required
  for the GPIOInt settings interface

REFERENCES

       Copyright © 2016 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/GPIOInt/interface/GPIOIntInterface.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/16   jg      Initial version
===========================================================================*/

#include "DALStdDef.h"
#include "HALgpiointInterface.h"

#define GPIOINTF_PROXY                        0x0001
#define GPIOINTF_MPMINT                       0x0002
#define GPIOINTF_WAKEUP                       0x0004
#define GPIOINTF_REGISTERED                   0x0008
#define GPIOINTF_EXTERNAL_CONFIG              0x0010 /** Reserved for external entity */
#define GPIOINTF_UGPIOINT_CONFIG              0x0020 /** Reserved for uGPIOInt */
#define GPIOINTF_DIRECTINT_CONFIG             0x0040 /** Direct line to IntC */

/*
 * GPIOIntConfigMapType
 * Structure storing the interrupt data map accross all processors.
 * uint32          gpio;                     GPIO pin number
 * uint32          interrupt_id; l1 main interrupt line for direct connects.
 * uint32          polarity;                 polarity of the interrupt.
 */
typedef struct
{
  uint32                                 gpio;
  HAL_gpioint_InterruptType              interrupt;
  uint32                                 interrupt_id;
  uint32                                 flags;
} GPIOIntConfigMapType;

#endif /* _GPIOINTINTERFACE_H_ */
