/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This file has the direct connect GPIO interrupt support for the 8994 target
  UIMAGE. This file should not be included in Dev CFGs for dal. its strictly used for
  non dal uimage drivers.

REFERENCES

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/user/uimage/config/stub/uGPIOIntConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "uGPIOIntUser.h"

/*
 * LPASS Sensors Direct Connect Interrupt Config Map.
 * Please note this is different from the Audio DSP direct connect map.
 */

uGPIOIntConfigMapType uGPIOIntConfigMap[] =
{
  {UGPIOINT_NONE, 0, 0,0},                             /* Ending delimiter.*/
};









