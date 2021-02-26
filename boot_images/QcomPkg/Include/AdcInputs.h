#ifndef __ADC_INPUTS_H__
#define __ADC_INPUTS_H__
/*============================================================================
  @file AdcInputs.h

  Analog-to-Digital Converter driver standard ADC input names.

  This header provides macros to provide common names to access ADC input
  properties.

               Copyright (c) 2008-2016 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/** BATT_ID - nPhysical units are in millivolts        */
#define ADC_INPUT_BATT_ID          "BATT_ID"

/** BATT_ID_OHMS - nPhysical units are in ohms         */
#define ADC_INPUT_BATT_ID_OHMS     "BATT_ID_OHMS"

/** BATT_ID_OHMS_FRESH - nPhysical units are in ohms  */
#define ADC_INPUT_BATT_ID_OHMS_FRESH  "BATT_ID_OHMS_FRESH"

/** BATT_THERM - nPhysical units are in degrees C      */
#define ADC_INPUT_BATT_THERM       "BATT_THERM"

/** AUX_THERM - nPhysical units are in degrees C       */
#define ADC_INPUT_AUX_THERM        "AUX_THERM"

/** SKIN_THERM - nPhysical units are in degrees C      */
#define ADC_INPUT_SKIN_THERM       "SKIN_THERM"

/** CHG_TEMP - nPhysical units are in 0.001 degrees C  */
#define ADC_INPUT_CHG_TEMP         "CHG_TEMP"

/** HARDWARE_ID - nPhysical units are in millivolts    */
#define ADC_INPUT_PMIC_HARDWARE_ID "PMIC_HARDWARE_ID"

/** PA_THERM - nPhysical units are in degrees C        */
#define ADC_INPUT_PA_THERM         "PA_THERM"

/** PA_THERM1 - nPhysical units are in degrees C       */
#define ADC_INPUT_PA_THERM1        "PA_THERM1"

/** PLATFORM_ID - nPhysical units are in millivolts    */
#define ADC_INPUT_PLATFORM_ID      "PLATFORM_ID"

/** PMIC_THERM - nPhysical units are in 0.001 degrees C*/
#define ADC_INPUT_PMIC_THERM       "PMIC_THERM"

/** PMIC_TEMP1 - nPhysical units are in 0.001 degrees C*/
#define ADC_INPUT_PMIC_TEMP1       ADC_INPUT_PMIC_THERM

/** PMIC_TEMP2 - nPhysical units are in 0.001 degrees C*/
#define ADC_INPUT_PMIC_TEMP2       "PMIC_TEMP2"

/** USB_IN - nPhysical units are in millivolts         */
#define ADC_INPUT_USB_IN           "USB_IN"

/** USB_IN_I - nPhysical units are in milliamps        */
#define ADC_INPUT_USB_IN_I         "USB_IN_I"

/** VBATT - nPhysical units are in millivolts          */
#define ADC_INPUT_VBATT            "VBATT"

/** VBATT when GSM PA ON - nPhysical units are in millivolts */
#define ADC_INPUT_VBATT_GSM        "VBATT_GSM"

/** VBATT when flash ramp up is done - nPhysical units are in millivolts */
#define ADC_INPUT_VBATT_FLASH      "VBATT_FLASH"

/** VCHG - nPhysical units are in millivolts           */
#define ADC_INPUT_VCHG             "VCHG"

/** VCOIN - nPhysical units are in millivolts          */
#define ADC_INPUT_VCOIN            "VCOIN"

/** VPH_PWR - nPhysical units are in millivolts        */
#define ADC_INPUT_VPH_PWR          "VPH_PWR"

/** XO_THERM - nPhysical units are in 2^-10 degrees C  */
#define ADC_INPUT_XO_THERM         "XO_THERM"

/** XO_THERM_GPS - nPhysical units are in 2^-10 degrees C */
#define ADC_INPUT_XO_THERM_GPS     "XO_THERM_GPS"

/** XO_THERM_MV - nPhysical units are in millivolts    */
#define ADC_INPUT_XO_THERM_MV       "XO_THERM_MV"

/** DC_IN - nPhysical units are in millivolts          */
#define ADC_INPUT_DC_IN            "DC_IN"

/** DC_IN_I - nPhysical units are in milliamps         */
#define ADC_INPUT_DC_IN_I          "DC_IN_I"

/** SYS_THERM1 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM1       "SYS_THERM1"

/** SYS_THERM2 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM2       "SYS_THERM2"

/** SYS_THERM3 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM3       "SYS_THERM3"

/** SYS_THERM4 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM4       "SYS_THERM4"

/** SYS_THERM5 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM5       "SYS_THERM5"

/** USB_DATA - nPhysical units are in millivolts       */
#define ADC_INPUT_USB_DATA         "USB_DATA"

/** USB_ID - nPhysical units are in ohms               */
#define ADC_INPUT_USB_ID           "USB_ID"

/** USB_DP - nPhysical units are in millivolts         */
#define ADC_INPUT_USB_DP           "USB_DP"

/** USB_DN - nPhysical units are in millivolts         */
#define ADC_INPUT_USB_DN           "USB_DN"

/** FG_GPIO - nPhysical units are in millivolts        */
#define ADC_INPUT_FG_GPIO          "FG_GPIO"

#endif

