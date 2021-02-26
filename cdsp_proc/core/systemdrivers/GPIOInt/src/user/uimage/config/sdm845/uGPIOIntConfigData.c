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

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/user/uimage/config/sdm845/uGPIOIntConfigData.c#1 $

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
  {117, 0,  96, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  0 */
  {118, 1,  97, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  1 */
  {119, 2,  98, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  2 */
  {120, 3,  99, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  3 */
  {121, 4,  100, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  4 */
  {122, 5,  101, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  5 */
  {123, 6,  102, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  6 */
  {124, 7,  103, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  7 */
  {125, 8,  104, UGPIOINTF_DIRECTINT_CONFIG}, /* Direct connect  8 */
  {58,  9,  87,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[0] */
  {59,  10, 88,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[1] */
  {60,  11, 89,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[2] */
  {61,  12, 90,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[3] */
  {62,  13, 91,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[4] */
  {63,  14, 92,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[5] */
  {78,  15, 93,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[6] */
  {79,  16, 94,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[7] */
  {80,  17, 95,  UGPIOINTF_DIRECTINT_CONFIG}, /* ssc_wkup_irq[8] */
  {0, 0, 0, 0}, /* End */
};









