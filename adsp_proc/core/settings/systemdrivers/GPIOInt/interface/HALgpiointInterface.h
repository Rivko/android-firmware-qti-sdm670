#ifndef _HALGPIOINTINTERFACE_H
#define _HALGPIOINTINTERFACE_H
/*
===========================================================================

FILE:         HALgpiointsettings.h

DESCRIPTION:  
  Enums required for settings interface

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/GPIOInt/interface/HALgpiointInterface.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/13/17   dcf     Created.

===========================================================================
       Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/


/*
 * HAL_gpioint_InterruptType
 *
 * Enumeration of supported GPIO interrupt types for supporting direct
 * connection versus summary.
 */
typedef enum
{
  HAL_GPIOINT_DIRECTCONNECT_0  =  0,  
  HAL_GPIOINT_DIRECTCONNECT_1  =  1, 
  HAL_GPIOINT_DIRECTCONNECT_2  =  2, 
  HAL_GPIOINT_DIRECTCONNECT_3  =  3, 
  HAL_GPIOINT_DIRECTCONNECT_4  =  4, 
  HAL_GPIOINT_DIRECTCONNECT_5  =  5, 
  HAL_GPIOINT_DIRECTCONNECT_6  =  6, 
  HAL_GPIOINT_DIRECTCONNECT_7  =  7, 
  HAL_GPIOINT_DIRECTCONNECT_8  =  8, 
  HAL_GPIOINT_DIRECTCONNECT_9  =  9, 
  HAL_GPIOINT_DIRECTCONNECT_10 = 10, 
  HAL_GPIOINT_DIRECTCONNECT_11 = 11, 
  HAL_GPIOINT_DIRECTCONNECT_12 = 12, 
  HAL_GPIOINT_DIRECTCONNECT_13 = 13, 
  HAL_GPIOINT_DIRECTCONNECT_14 = 14, 
  HAL_GPIOINT_DIRECTCONNECT_15 = 15, 
  HAL_GPIOINT_SPECIALCONN      = 0x7E,
  HAL_GPIOINT_SUMMARY          = 0x7F
} HAL_gpioint_InterruptType;

#endif /* _HALGPIOINTINTERFACE_H */
