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

$Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/GPIOMgr/config/GPIOMgrConfigDataStub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/1/2015   cpaulo  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "GPIOMgrInterface.h"

GPIOMgrConfigMapType GPIOMgrConfigMap[] =
{
  {0, 52, GPIOMGR_UNUSED_GPIO, 1}, /* Direct connect  0 */
  {1, 53, GPIOMGR_UNUSED_GPIO, 1}, /* Direct connect  1 */
  {0, 0, 0, 0}, /* End */
};

GPIOMgrConfigPDType GPIOMgrConfigPD[] =
{
  {0, GPIOMGR_DEVICE_SPS},
  {1, GPIOMGR_DEVICE_SPS},
  {0, GPIOMGR_DEVICE_NONE}
};









