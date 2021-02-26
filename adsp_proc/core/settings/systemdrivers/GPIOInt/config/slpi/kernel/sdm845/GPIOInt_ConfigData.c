/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This file has the direct connect GPIO interrupt support for the 8960 target.

REFERENCES

        Copyright © 2014 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/GPIOInt/config/slpi/kernel/sdm845/GPIOInt_ConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALReg.h"
#include "DDIGPIOInt.h"
#include "GPIOIntInterface.h"
#include "HALgpiointInterface.h"

/*
 * LPASS Interrupt Config Map
 */

GPIOIntConfigMapType interrupt_config_map[] =
{
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_0, 52,  GPIOINTF_EXTERNAL_CONFIG}, /* Direct connect  0 */
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_1, 53,  GPIOINTF_EXTERNAL_CONFIG}, /* Direct connect  1 */
  {117,          HAL_GPIOINT_SUMMARY,         96,  GPIOINTF_DIRECTINT_CONFIG}, /* Accelerometer Interrupt           */
  {118,          HAL_GPIOINT_SUMMARY,         97,  GPIOINTF_DIRECTINT_CONFIG}, /* Gyroscope Interrupt               */
  {119,          HAL_GPIOINT_SUMMARY,         98,  GPIOINTF_DIRECTINT_CONFIG}, /* Magnetometer Data Ready Interrupt */
  {120,          HAL_GPIOINT_SUMMARY,         99,  GPIOINTF_DIRECTINT_CONFIG}, /* ALSP & Gesture Interrupts Muxed   */
  {121,          HAL_GPIOINT_SUMMARY,         100, GPIOINTF_DIRECTINT_CONFIG}, /* Fingerprint Sensor Interrupt      */
  {122,          HAL_GPIOINT_SUMMARY,         101, GPIOINTF_DIRECTINT_CONFIG}, /* HRM Interrupt                     */
  {123,          HAL_GPIOINT_SUMMARY,         102, GPIOINTF_DIRECTINT_CONFIG}, /* Cap Sensor1 & Cap Sensor2 Muxed   */
  {124,          HAL_GPIOINT_SUMMARY,         103, GPIOINTF_DIRECTINT_CONFIG}, /* Hall Effect Sensor Interrupt      */
  {125,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Touch GPIO in MultiPD is a summary interrupt*/
};











