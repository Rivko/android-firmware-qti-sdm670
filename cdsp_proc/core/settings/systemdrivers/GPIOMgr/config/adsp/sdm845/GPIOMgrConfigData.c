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

$Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/GPIOMgr/config/adsp/sdm845/GPIOMgrConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/1/2015   cpaulo  First draft created. 
===========================================================================*/
#include "com_dtypes.h"
#include "GPIOMgrInterface.h"

GPIOMgrConfigMapType GPIOMgrConfigMap[] =
{
  {0, 24, GPIOMGR_FLAG_RESERVED, 1}, /* Direct connect  0 */
  {1, 25, GPIOMGR_FLAG_RESERVED, 1}, /* Direct connect  1 */
  {2, 26, GPIOMGR_UNUSED_GPIO, 0}, /* Direct connect  2 */
  {3, 27, GPIOMGR_UNUSED_GPIO, 0}, /* Direct connect  3 */
  {4, 28, GPIOMGR_UNUSED_GPIO, 1}, /* Direct connect  4 */
  {5, 29, GPIOMGR_UNUSED_GPIO, 1}, /* Direct connect  5 */
  {0, 0, 0, 0}, /* End */
};

GPIOMgrConfigPDType GPIOMgrConfigPD[] =
{
  {0, GPIOMGR_DEVICE_LPA_DSP},
  {1, GPIOMGR_DEVICE_LPA_DSP},
  {0, GPIOMGR_DEVICE_NONE}
};









