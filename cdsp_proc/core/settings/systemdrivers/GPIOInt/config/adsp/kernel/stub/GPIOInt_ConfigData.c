/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This file has the direct connect GPIO interrupt support for the 8960 target.

REFERENCES

        Copyright © 2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/settings/systemdrivers/GPIOInt/config/adsp/kernel/stub/GPIOInt_ConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIGPIOInt.h"
#include "HALgpioint.h"
#include "GPIOInt.h"

/*
 * LPASS Interrupt Config Map
 */

GPIOIntConfigMapType interrupt_config_map[] =
{
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_0, 24,GPIOINTF_EXTERNAL_CONFIG}, /* Direct connect  0 owned by audio Codec*/
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_1, 25,GPIOINTF_EXTERNAL_CONFIG}, /* Direct connect  1 owned by audio Codec*/
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_2, 26,0}, /* Direct connect  2*/
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_3, 27,0}, /* Direct connect  3*/
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_4, 28,0}, /* Direct connect  4*/
  {GPIOINT_NONE, HAL_GPIOINT_DIRECTCONNECT_5, 29,0}, /* Direct connect  5*/
  {87 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {88 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {10 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {11 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {73 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {65 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {68 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {74 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {9  ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {67 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {66 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {75 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {64 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {79 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {80 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {47 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/
  {48 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/   
  {45 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/   
  {46 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/   
  {77 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/   
  {28 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/   
  {76 ,          HAL_GPIOINT_SUMMARY,         GPIOINT_NONE,0}, /* Sensors GPIO in MultiPD is a summary interrupt*/   

};









