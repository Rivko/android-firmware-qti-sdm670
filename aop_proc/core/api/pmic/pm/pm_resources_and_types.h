#ifndef PM_RESOURCES_AND_TYPES__H
#define PM_RESOURCES_AND_TYPES__H

/** @file pm_resources_and_types.h
 * 
 *  This file contains enumerations with resource names for different
 *  module types that should be used by external clients when accessing
 *  the resources that are required
*/
/*
 *  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/pmic/pm/pm_resources_and_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   hw      moving some npa type to this file so we don't need to include pmapp_npa.h
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/** @addtogroup pm_resources_and_types
@{ */

/**
 * PMIC power module voltage level.
 */
typedef uint32 pm_volt_level_type;

/**< On/off setting. */
typedef enum
{
   PM_OFF = 0,  /**< Off. */
   PM_ON = 1,   /**< On. */
   PM_INVALID
}pm_on_off_type;







/** Contains peripheral information. 
 */
typedef struct 
{
    uint16 base_address;		   /**< Base address. */
    uint8  peripheral_type;		   /**< Peripheral type. */
    uint8  peripheral_subtype;     /**< Peripheral subtype. */
    uint8  analog_major_version;   /**< Analog major version. */
    uint8  analog_minor_version;   /**< Analog minor version. */
    uint8  digital_major_version;  /**< Digital major version. */
    uint8  digital_minor_version;  /**< Digital minor version. */
}peripheral_info_type;

/** 
  Types of IRQ bit fields of power rails. 
 */
typedef enum 
{
  PM_PWR_IRQ_VREG_FAULT,  /**< IRQ VREG Fault. */
  PM_PWR_IRQ_VREG_READY,  /**< IRQ VREG Ready. */
  PM_PWR_IRQ_INVALID
}pm_pwr_irq_type;

/**
 *  Type definition for the IRQ triggers.
 */
typedef enum 
{
  PM_IRQ_TRIGGER_ACTIVE_LOW,    /**< IRQ trigger active low. */
  PM_IRQ_TRIGGER_ACTIVE_HIGH,   /**< IRQ trigger active high. */ 
  PM_IRQ_TRIGGER_RISING_EDGE,   /**< IRQ trigger rising edge. */
  PM_IRQ_TRIGGER_FALLING_EDGE,  /**< IRQ trigger falling edge. */
  PM_IRQ_TRIGGER_DUAL_EDGE,     /**< IRQ trigger dual edge. */
  PM_IRQ_TRIGGER_INVALID
} pm_irq_trigger_type;

/** @} */ /* end_addtogroup pm_resources_and_types */

#endif /* PM_RESOURCES_AND_TYPES__H */

