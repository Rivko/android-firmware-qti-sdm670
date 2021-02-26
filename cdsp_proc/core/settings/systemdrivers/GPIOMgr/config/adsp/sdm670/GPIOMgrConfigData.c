/*==============================================================================

  D A L   G P I O   M A N A G E R

DESCRIPTION
  This file has the direct connect GPIO interrupt mapping required for
  this target.

REFERENCES

    Copyright © 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/GPIOMgr/config/adsp/sdm670/GPIOMgrConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/1/2015   cpaulo  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "GPIOMgrInterface.h"

GPIOMgrConfigMapType GPIOMgrConfigMap[] =
{
  {0, 70, GPIOMGR_UNUSED_GPIO,   2 }, /* Sensor direct connect 0 */
  {1, 71, GPIOMGR_UNUSED_GPIO,   2 }, /* Sensor direct connect 2 */
  {0, 0, 0, 0}, /* End */
};

GPIOMgrConfigPDType GPIOMgrConfigPD[] =
{
  {0, GPIOMGR_DEVICE_LPA_DSP },
  {1, GPIOMGR_DEVICE_LPA_DSP },
  {2, GPIOMGR_DEVICE_SPS     },
  {0, GPIOMGR_DEVICE_NONE    }
};









