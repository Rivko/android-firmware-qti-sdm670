#ifndef __PM_EXTERNAL_RESOURCES_H__
#define __PM_EXTERNAL_RESOURCES_H__

/*! \file 
 *  \n
 *  \brief  pm_external_resources.h ---- PMIC external resources header file 
 *  \n
 *  \n This file contains enumerations with resource names for different
 *  \n module types that should be used by external clients when accessing
 *  \n the resources that are required
 *  \n
 *  \n &copy; Copyright 2003-2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */
 
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tz/2.0.2/api/pmic/pm/pm_resources_and_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/

/* 
 * Indexes for MPPs are in CMI header file "pm_mpps.h"
 * Indexes for GPIOs are in CMI header file "pm_gpio.h" 
 */

/*! \enum voltage_regulator_enumeration.
 *  \brief This enum contains all voltage regulators as a zero indexed list
 *  LDO, SMPS, and VS are all VREGs and must share common enumeration
 *  so that they can be indexed correctly by the underlying access table.
 */
enum
{
/* LDO resources */
  PM_LDO_1,
  PM_LDO_2,
  PM_LDO_3,
  PM_LDO_4,
  PM_LDO_5,
  PM_LDO_6,
  PM_LDO_7,
  PM_LDO_8,
  PM_LDO_9,
  PM_LDO_10,
  PM_LDO_11,
  PM_LDO_12,
  PM_LDO_13,
  PM_LDO_14,
  PM_LDO_15,
  PM_LDO_16,
  PM_LDO_17,
  PM_LDO_18,
  PM_LDO_19,
  PM_LDO_20,
  PM_LDO_21,
  PM_LDO_22,
  PM_LDO_23,
  PM_LDO_24,
  PM_LDO_25,
  PM_LDO_26,
  PM_LDO_27,
  PM_LDO_28,
  PM_LDO_29,
  PM_LDO_30,
  PM_LDO_31,
  PM_LDO_32,
  PM_LDO_33,
/* SMPS Resources */
  PM_SMPS_1,
  PM_SMPS_2,
  PM_SMPS_3,
  PM_SMPS_4,
  PM_SMPS_5,    
  PM_SMPS_6,
  PM_SMPS_7,
  PM_SMPS_8,
  PM_SMPS_9,
  PM_SMPS_10,
/* Voltage Switcher Resources */
  PM_VS_LVS_1,
  PM_VS_LVS_2,
  PM_VS_LVS_3,
  PM_VS_LVS_4,
  PM_VS_LVS_5,
  PM_VS_LVS_6,
  PM_VS_LVS_7,
  PM_VS_LVS_8,
  PM_VS_LVS_9,
  PM_VS_LVS_10,
  PM_VS_LVS_11,
  PM_VS_LVS_12,
  PM_VS_MVS_1,
  PM_VS_OTG_1,
  PM_VS_HDMI_1,
  PM_VREG_INVALID
};

/*! \enum chargers
 *  \brief This enum contains definitions of all chargers in the target
 */
enum
{
  PM_CHG_1,
  PM_CHG_INVALID
};

/*! \enum High_current_LEDS
 *  \brief This enum contains definitions of all High current LED resources in the target
 */
enum
{
  PM_LEDH_1,
  PM_LEDH_2,
  PM_LEDH_3,
  PM_LEDH_INVALID
};

/*! \enum Low_current_LEDS
 *  \brief This enum contains definitions of all Low current LED resources in the target
 */
enum
{
  PM_LEDL_1,
  PM_LEDL_2,
  PM_LEDL_3,
  PM_LEDL_INVALID
};

/*! \enum PWMs
 *  \brief This enum contains definitions of all PWM resources in the target
 */
enum
{
  PM_PWM_1,
  PM_PWM_2,
  PM_PWM_3,
  PM_PWM_INVALID
};

/*! \enum HSED
 *  \brief This enum contains definitions of all HSED resources in the target
 */
enum
{    
  PM_HSED_1,
  PM_HSED_2,
  PM_HSED_3,
  PM_HSED_INVALID
};

/*! \enum external_chargers
 *  \brief This enum contains definitions of all External Charging Hardware Resources in the target
 */
enum
{
  PM_CHG_EXT_1,
  PM_CHG_EXT_INVALID
}; 

/*! \enum switched mode battery charger enum
 *  \brief This enum contains definitions of all Switched Mode Battery Charger Resources in the target
 */
enum
{
  PM_SMBC_1,
  PM_SMBC_INVALID
};

/*! \enum battery management system (BMS) enum
 *  \brief This enum contains definitions of all Battery Management System Resources in the target
 */
enum
{
  PM_BMS_1,
  PM_BMS_INVALID
};

/*! \enum battery temperature management (BTM) enum
 *  \brief This enum contains definitions of all Battery Temperature Management Resources in the target
 */
enum
{
  PM_BTM_1,
  PM_BTM_INVALID
};

/*! \enum ADC arbiter channels available on PMIC component
 *  \brief This enum contains definitions of all ADC Arbiter Channel Resources in the target
 *          Typically a processor will own a channel and the arbiter will provide equal access
 *          to the ADC for each processor.
 */
enum
{
    PM_ARB_ADC_1, 
    PM_ARB_ADC_2,
    PM_ARB_ADC_3,
    PM_ARB_ADC_INVALID
};

/*! \enum Light Pulse Generator LPG resources available on PMIC component
 *  \brief This enum contains definitions of all Light Pulse Generator LPG Resources in the target
 */
enum
{
    PM_LPG_1,
    PM_LPG_2,
    PM_LPG_3,
    PM_LPG_4,
    PM_LPG_5,
    PM_LPG_6,
    PM_LPG_7,
    PM_LPG_8,
    PM_LPG_INVALID
};

/*! \enum Interrupt resources available on PMIC component
 *  \brief This enum contains definitions of all Interrupt Resources on the target
 */
enum
{
    PM_INTERRUPT_1, //RPM/SEC
    PM_INTERRUPT_2, //MDM
    PM_INTERRUPT_3, //APP/USER
    PM_INTERRUPT_INVALID
};

/*! \enum Over Voltage Protection resources available on PMIC component
 *  \brief This enum contains definitions of all Over Voltage Protection OVP Resources on the target
 */
enum
{
    PM_OVP_1,
    PM_OVP_2,
    PM_OVP_INVALID
};

/*! \enum Mega XO resources available on PMIC component
 *  \brief This enum contains definitions the Mega XO resources in the target
 */
enum
{
    PM_MEGA_XO_1,
    PM_MEGA_XO_2,
    PM_MEGA_XO_3,
    PM_MEGA_XO_4,
    PM_MEGA_XO_5,
    PM_MEGA_XO_6,
    PM_MEGA_XO_7,
    PM_MEGA_XO_8,
    PM_MEGA_XO_9,
    PM_MEGA_XO_INVALID,
};

/*! \enum UICC resources available on PMIC component
 *  \brief This enum contains definitions the UICC resources in the target
 */
enum
{
  PM_UICC_1,
  PM_UICC_2,
  PM_UICC_INVALID
};

/*! \enum CLK resources available on PMIC component
 *  \brief This enum contains definitions the CLK resources in the target
 */
enum
{
    PM_CLK_1,
    PM_CLK_2, 
    PM_CLK_INVALID
};

typedef enum
{
   PM_OFF,
   PM_ON,
   PM_INVALID
}pm_on_off_type;

/*! \enum pm_irq_trigger_type
 *  \brief Type definition for different IRQ triggers
 */
typedef enum 
{
  PM_IRQ_TRIGGER_ACTIVE_LOW,
  PM_IRQ_TRIGGER_ACTIVE_HIGH,
  PM_IRQ_TRIGGER_RISING_EDGE,
  PM_IRQ_TRIGGER_FALLING_EDGE,
  PM_IRQ_TRIGGER_DUAL_EDGE,
  PM_IRQ_TRIGGER_INVALID
} pm_irq_trigger_type;

/*! \enum pm_irq_status_type
 *  \brief Type definition for different IRQ STATUS
 */
typedef enum
{
  PM_IRQ_STATUS_RT,
  PM_IRQ_STATUS_LATCHED,
  PM_IRQ_STATUS_PENDING,
  PM_IRQ_STATUS_ENABLE,
  PM_IRQ_STATUS_INVALID
} pm_irq_status_type;



#endif /* _PM_EXTERNAL_RESOURCES_H */

