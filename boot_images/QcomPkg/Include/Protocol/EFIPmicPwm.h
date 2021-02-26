/** 
  @file  EFIPmicPwm.h
  @brief PMIC Pulse Windth Modulation Protocol for UEFI.
*/
/*=============================================================================
Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/23/14   al      Added GetStatus 
 06/18/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 03/26/13   al      Adding glitch removal. 
 12/12/12   al      created

=============================================================================*/

#ifndef __EFIPMICPWM_H__
#define __EFIPMICPWM_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @ingroup efi_pmicPwm_constants 
  Protocol version.
*/

#define PMIC_PWM_REVISION 0x0000000000010003
/** @} */ /* end_addtogroup efi_pmicPwm_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicPwm_protocol */
#define EFI_PMIC_PWM_PROTOCOL_GUID \
    { 0xe08051ae, 0xac38, 0x46db, { 0xa6, 0x96, 0xc5, 0x69, 0x52, 0xb0, 0x56, 0x16 } }

	
/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the PMIC PWM Protocol GUID.
*/
extern EFI_GUID gQcomPmicPwmProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_PWM_PROTOCOL   EFI_QCOM_PMIC_PWM_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicPwm_data_types 
@{ */
/**
  Pulse-width modulation clock frequency selection.
*/
typedef enum{
    EFI_PM_PWM_CLK_OFF,      /**< Clock is OFF. */
    EFI_PM_PWM_CLK_1_0_KHZ,  /**< Clock is 1 kHz. */
    EFI_PM_PWM_CLK_32_0_KHZ, /**< Clock is 32 kHz. */
    EFI_PM_PWM_CLK_19_2_MHZ, /**< Clock is 19.2 MHz. */
/** @cond */
    EFI_PM_CLK_MAX
/** @endcond */
} EFI_PM_PWM_CLOCK_FREQ_TYPE;

/**
  Pulse-width modulation bit size.
*/
typedef enum{
    EFI_PM_PWM_SIZE_6BIT, /**< 6 bits. */
    EFI_PM_PWM_SIZE_9BIT, /**< 9 bits. */
/** @cond */
    EFI_PM_PWM_SIZE_MAX
/** @endcond */
} EFI_PM_PWM_BIT_SIZE_TYPE;
/** @} */ /* end_addtogroup efi_pmicPwm_data_types */



/** 
  EFI_PM_PWM_STATUS_TYPE
  PWM status
*/
typedef struct{
  BOOLEAN PwmEnable;
  /* PWM enabled or disabled */  
  UINT16 PwmValue;
  /* Value set for PWM duty cycle */
  EFI_PM_PWM_BIT_SIZE_TYPE BitSize; 
  /* PWM bit size */
  EFI_PM_PWM_CLOCK_FREQ_TYPE ClkFreq;
  /* PWM master clock frequency */
  UINT32 PreDiv;
  /* Frequency predivide to divide down PWM master clock */
  UINT32 Exp;
  /* Exponent values to divide down PWM master clock */
  BOOLEAN GlitchTemoval;
  /* PWM glitch removal enabled or disabled */
}EFI_PM_PWM_STATUS_TYPE;
/** @} */ /* end_addtogroup efi_pmicPwm_status_type */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_PWM_CONFIG */ 
/** @ingroup efi_pmPwm_config
  @par Summary
  Configures the pulse-width modulation.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  PwmValue         Pulse-width modulation value. Range: 0 to 511.
  @param[in]  PreDiv           Frequency predivider. Values are: 1, 3, 5, or 6.
  @param[in]  PreDivExponent   Exponent values to predivide the master clock. 
                               Range: 0 to 7.
  @param[in]  ClkFreq          Clock frequency selection; see 
                               #EFI_PM_PWM_CLOCK_FREQ_TYPE for details.
  @param[in]  BitSize          Bit size; see #EFI_PM_PWM_BIT_SIZE_TYPE for 
                               details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_PWM_CONFIG)(
  IN UINT32                     PmicDeviceIndex, 
  IN UINT16                     PwmValue,
  IN UINT32                     PreDiv,
  IN UINT32                     PreDivExponent,
  IN EFI_PM_PWM_CLOCK_FREQ_TYPE ClkFreq,
  IN EFI_PM_PWM_BIT_SIZE_TYPE   BitSize
);

/* EFI_PM_PWM_SET_PWM_VALUE */ 
/** @ingroup efi_pmPwm_set_pwm_value
  @par Summary
  Sets the value for the duty cycle.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  PwmValue         Pulse-width modulation value. Range: 0 to 511.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_PWM_SET_PWM_VALUE)(
   IN UINT32 PmicDeviceIndex, 
   IN UINT16 PwmValue
);

/* EFI_PM_PWM_GLITCH_REMOVAL */ 
/** @ingroup efi_pmPwm_glitch_removal
  @par Summary
  Enables/disables glitch removal.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1. 
  @param[in]  Enable           If TRUE, enable glitch removal;
                               if FALSE, disable glitch removal.
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_PWM_GLITCH_REMOVAL)(
   IN UINT32  PmicDeviceIndex, 
   IN BOOLEAN Enable
);


/* EFI_PM_PWM_GET_STATUS */ 
/** @ingroup efi_pm_pwm_get_status
  @par Summary
  Sets the value for duty cycle
  @param[in]   DeviceIndex        Primary or Secondary PMIC. 
  @param[out]  PwmStatus          PWM status

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_PWM_GET_STATUS)(
   IN  UINT32                 PmicDeviceIndex, 
   OUT EFI_PM_PWM_STATUS_TYPE *PwmStatus
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicPwm_protocol
  @par Summary
  Qualcomm PMIC Pulse-Width Modulation (PWM) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_pwm_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_PWM_PROTOCOL {
  UINT64                    Revision;
  EFI_PM_PWM_CONFIG         PwmConfig;
  EFI_PM_PWM_SET_PWM_VALUE  SetPwmValue;
  EFI_PM_PWM_GLITCH_REMOVAL GlitchRemoval;
  EFI_PM_PWM_GET_STATUS     GetStatus;
};

#endif  /* __EFIPMICPWM_H__ */
