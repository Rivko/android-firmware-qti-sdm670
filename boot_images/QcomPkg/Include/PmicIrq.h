/** 
  @file  PmicIrq.h
  @brief PMIC IRQ related to UEFI.
*/
/*=============================================================================
Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/16/13   al      created

=============================================================================*/
#ifndef __PMIC_IRQ_H__
#define __PMIC_IRQ_H__

/*! \enum EFI_PM_IRQ_TRIGGER_TYPE
 *  \brief Type definition for different IRQ triggers
 */
typedef enum 
{
  EFI_PM_IRQ_TRIGGER_ACTIVE_LOW,
  EFI_PM_IRQ_TRIGGER_ACTIVE_HIGH,
  EFI_PM_IRQ_TRIGGER_RISING_EDGE,
  EFI_PM_IRQ_TRIGGER_FALLING_EDGE,
  EFI_PM_IRQ_TRIGGER_DUAL_EDGE,
  EFI_PM_IRQ_TRIGGER_INVALID
} EFI_PM_IRQ_TRIGGER_TYPE;


/*! \enum EFI_PM_IRQ_STATUS_TYPE
 *  \brief Type definition for different IRQ STATUS
 */
typedef enum
{
  EFI_PM_IRQ_STATUS_RT,
  EFI_PM_IRQ_STATUS_LATCHED,
  EFI_PM_IRQ_STATUS_PENDING,
  EFI_PM_IRQ_STATUS_INVALID
} EFI_PM_IRQ_STATUS_TYPE;





#endif /*__PMIC_IRQ_H__*/

