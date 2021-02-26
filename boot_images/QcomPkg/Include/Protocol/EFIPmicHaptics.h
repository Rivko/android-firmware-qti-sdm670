#ifndef __EFI_PM_HAPTICS_H__
#define __EFI_PM_HAPTICS_H__

/*! \file  EFI_PM_HAPTICS.H 
*  \n
*  \brief   PMIC-HAPTICS MODULE RELATED DECLARATION
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting HAPTICS motor driver for the Qualcomm PMIC 
*  chip set.
*
*
*  \n &copy; Copyright 2015 QUALCOMM Technologies Inc, All Rights Reserved
*/

/* =======================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/15   al      Adding Haptics protocol
========================================================================== */


/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @ingroup @addtogroup efi_pmicHaptics_constants 
@{ */
/** @ingroup
Protocol version.
*/

#define PMIC_HAPTICS_REVISION 0x0000000000010003
/** @ingroup @} */ /* end_addtogroup efi_pmicIbb_constants */

/*  Protocol GUID definition */
/** @ingroup @ingroup efi_pmicHaptics_protocol */
#define EFI_PMIC_HAPTICS_PROTOCOL_GUID \
    { 0x0133062c, 0xc2a3, 0x4c1a, { 0x85, 0x52, 0xe7, 0xcb, 0x0e, 0x5f, 0xde, 0xa9 } }

/** @ingroup @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/

/** @ingroup
External reference to the PMIC HAPTICS Protocol GUID.
*/
extern EFI_GUID gQcomPmicHapticsProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/** @ingroup
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_HAPTICS_PROTOCOL   EFI_QCOM_PMIC_HAPTICS_PROTOCOL;
/** @ingroup @endcond */

/** @ingroup @addtogroup efi_pmicHaptics_data_types 
@{ */


/*=============================================================================
                    LOCAL VARIABLE DEFINITIONS
=============================================================================*/

typedef enum 
{
  EFI_PM_HAPTICS_PLAY,
  EFI_PM_HAPTICS_PAUSE,
  EFI_PM_HAPTICS_INVALID
}EFI_PM_HAPTICS_VIBRATE_TYPE;


typedef enum 
{
  EFI_PM_HAPTICS_ACTUATOR_LRA,
  EFI_PM_HAPTICS_ACTUATOR_ERM,
  EFI_PM_HAPTICS_ACTUATOR_INVALID
}EFI_PM_HAPTICS_ACTUATOR_TYPE;

typedef enum 
{
  EFI_PM_HAPTICS_LRA_RES_SIN,
  EFI_PM_HAPTICS_LRA_RES_SQUARE,
  EFI_PM_HAPTICS_LRA_RES_INVALID
}EFI_PM_HAPTICS_LRA_RES_TYPE;

typedef enum 
{
  EFI_PM_HAPTICS_SLEW_2NS,
  EFI_PM_HAPTICS_SLEW_10NS,
  EFI_PM_HAPTICS_SLEW_INVALID
}EFI_PM_HAPTICS_SLEW_RATE;


typedef enum 
{
  EFI_PM_HAPTICS_PWM_CAP_26PF,
  EFI_PM_HAPTICS_PWM_CAP_13PF,
  EFI_PM_HAPTICS_PWM_CAP_8P7PF,
  EFI_PM_HAPTICS_PWM_CAP_6P5PF,
  EFI_PM_HAPTICS_PWM_CAP_INVALID
}EFI_PM_HAPTICS_PWM_CAP_SEL_TYPE;


typedef enum 
{
  EFI_PM_HAPTICS_WF_REPEAT_1_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_2_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_4_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_8_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_16_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_32_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_64_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_128_TIMES,
  EFI_PM_HAPTICS_WF_REPEAT_INVALID
}EFI_PM_HAPTICS_WAVEFORM_REPEAT_TYPE;

typedef enum 
{
  EFI_PM_HAPTICS_WF_S_REPEAT_1_TIMES,
  EFI_PM_HAPTICS_WF_S_REPEAT_2_TIMES,
  EFI_PM_HAPTICS_WF_S_REPEAT_4_TIMES,
  EFI_PM_HAPTICS_WF_S_REPEAT_8_TIMES,
  EFI_PM_HAPTICS_WF_S_REPEAT_INVALID
}EFI_PM_HAPTICS_WAVEFORM_S_REPEAT_TYPE;


/*===========================================================================

                 HAPTICS DRIVER FUNCTION DEFINITIONS

===========================================================================*/


/* EFI_PM_HAPTICS_ENABLE */
/** @ingroup This function enables haptics
  @par Summary
  This function enables haptics.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  Enable          TRUE: Enables, FALSE: Disables it
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_ENABLE)(
    IN UINT32  PmicDeviceIndex,  
    IN BOOLEAN Enable
    );



/* EFI_PM_HAPTICS_SET_ACTUATOR_TYPE */
/** @ingroup efi_pm_haptics_set_actuator_type
  @par Summary
  This API sets the actuator type for haptics
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ActuatorType     Actuator type. see #EFI_PM_HAPTICS_ACTUATOR_TYPE for details.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_SET_ACTUATOR_TYPE)(
    IN UINT32                       PmicDeviceIndex, 
    IN EFI_PM_HAPTICS_ACTUATOR_TYPE ActuatorType
    );


/* EFI_PM_HAPTICS_SET_LRA_RESONANCE_TYPE */
/** @ingroup efi_pm_haptics_set_lra_resonance_type
  @par Summary
  This API sets the LRA resonance type for haptics
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  ActuatorType     LraRes type. see #EFI_PM_HAPTICS_LRA_RES_TYPE for details.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_SET_LRA_RESONANCE_TYPE)( 
    IN UINT32                      PmicDeviceIndex, 
    IN EFI_PM_HAPTICS_LRA_RES_TYPE LraRes
    );



/* EFI_PM_HAPTICS_SET_SLEW_RATE */
/** @ingroup efi_pm_haptics_set_slew_rate
  @par Summary
  This function sets the slew rate for haptics
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  SlewRate          Slew rate; see #EFI_PM_HAPTICS_SLEW_RATE for details.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_SET_SLEW_RATE)( 
     IN UINT32                   PmicDeviceIndex, 
     IN EFI_PM_HAPTICS_SLEW_RATE SlewRate
     );



/* EFI_PM_HAPTICS_PLAY */
/** @ingroup efi_pm_haptics_play
  @par Summary
  This function sets the vibration action for haptics.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  LedType           type; see #EFI_PM_HAPTICS_VIBRATE_TYPE for details.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_PLAY_TYPE)(  
    IN UINT32                      PmicDeviceIndex,  
    IN EFI_PM_HAPTICS_VIBRATE_TYPE Vibrate
    );



/* EFI_PM_HAPTICS_SET_VMAX_VOLT */
/** @ingroup efi_pm_haptics_set_vmax_volt
  @par Summary
  This API sets the max voltage for haptics
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  VmaxCfgVolt      Maximum voltage for haptics in milli voltage
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_SET_VMAX_VOLT)( 
    IN UINT32 PmicDeviceIndex, 
    IN UINT32 VmaxCfgVolt
    );


/* EFI_PM_HAPTICS_INTERNAL_PWM_FREQ_SEL */
/** @ingroup efi_pm_haptics_internal_pwm_freq_sel
  @par Summary
  This function sets the internal PWM frequency
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  FreqSelDataInKhz  Frequencey in range 25KhZ to 100KhZ
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_INTERNAL_PWM_FREQ_SEL)(
    IN UINT32 PmicDeviceIndex, 
    IN UINT32 FreqSelDataInKhz
    );



/* EFI_PM_HAPTICS_EXTERNAL_PWM_FREQ_SEL */
/** @ingroup efi_pm_haptics_external_pwm_freq_sel
  @par Summary
  This function sets the external PWM frequency
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  FreqSelDataInKhz  Frequencey in range 25KhZ to 100KhZ
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_EXTERNAL_PWM_FREQ_SEL)(
    IN UINT32 PmicDeviceIndex,
    IN UINT32 FreqSelDataInKhz
    );


/* EFI_PM_HAPTICS_PWM_CAP_SEL */
/** @ingroup efi_pm_haptics_pwm_cap_sel
  @par Summary
  This function sets the PWM CAP for haptics vibration
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  CapSelData       Cap type; see #EFI_PM_HAPTICS_PWM_CAP_SEL_TYPE for details.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_PWM_CAP_SEL)(
    IN UINT32                           PmicDeviceIndex, 
    IN EFI_PM_HAPTICS_PWM_CAP_SEL_TYPE  CapSelData
    );

/*===========================================================================

FUNCTION pm_haptics_waveform_repeat

DESCRIPTION
  This function enables haptics various modes of waveforms(WF).

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: haptics
      - Selects which haptics peripheral.
   3) Name: wf_repeat_data BITS [6:4]
        PM_HAPTICS_WF_REPEAT_1_TIMES,
        PM_HAPTICS_WF_REPEAT_2_TIMES,
        PM_HAPTICS_WF_REPEAT_4_TIMES,
        PM_HAPTICS_WF_REPEAT_8_TIMES,
        PM_HAPTICS_WF_REPEAT_16_TIMES,
        PM_HAPTICS_WF_REPEAT_32_TIMES,
        PM_HAPTICS_WF_REPEAT_64_TIMES,
        PM_HAPTICS_WF_REPEAT_128_TIMES,
   4)Name: wf_s_repeat_data BITS [6:4]
        PM_HAPTICS_WF_S_REPEAT_1_TIMES,
        PM_HAPTICS_WF_S_REPEAT_2_TIMES,
        PM_HAPTICS_WF_S_REPEAT_4_TIMES,
        PM_HAPTICS_WF_S_REPEAT_8_TIMES,
            
RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG__INVALID_HAPTICS_INDEXED = The requested HAPTICS doesn't exist.
    PM_ERR_FLAG__SBI_OPT_ERR              = The SBI driver failed to communicate
                                            with the PMIC.
    PM_ERR_FLAG__PAR3_OUT_OF_RANGE        = Input Parameter three is out of range.
    PM_ERR_FLAG__SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/

/* EFI_PM_HAPTICS_WF_REPEAT */
/** @ingroup efi_pm_haptics_wf_repeat
  @par Summary
  This function sets the waveform repeat number.
  
  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  WfRepeatData          Wf repat type; see   #EFI_PM_HAPTICS_WAVEFORM_REPEAT_TYPE for details.
  @param[in]  WfSRepeatData         WfS repeat type; see #EFI_PM_HAPTICS_WAVEFORM_S_REPEAT_TYPE for details.
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_HAPTICS_WF_REPEAT) (
    IN UINT32                                PmicDeviceIndex, 
    IN EFI_PM_HAPTICS_WAVEFORM_REPEAT_TYPE   WfRepeatData, 
    IN EFI_PM_HAPTICS_WAVEFORM_S_REPEAT_TYPE WfSRepeatData
    );


/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup @ingroup efi_pmicHaptics_protocol
@par Summary
Qualcomm PMIC HAPTICS Protocol interface.

@par Parameters
@inputprotoparams{pmic_haptics_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_HAPTICS_PROTOCOL {
  UINT64                                   Revision;
  EFI_PM_HAPTICS_ENABLE                    Enable;
  EFI_PM_HAPTICS_SET_ACTUATOR_TYPE         SetActuatorType;
  EFI_PM_HAPTICS_SET_LRA_RESONANCE_TYPE    SetLraResonanceType;
  EFI_PM_HAPTICS_SET_SLEW_RATE             SetSlewRate;
  EFI_PM_HAPTICS_PLAY_TYPE                 PlayType;
  EFI_PM_HAPTICS_SET_VMAX_VOLT             SetVmaxVolt;
  EFI_PM_HAPTICS_INTERNAL_PWM_FREQ_SEL     InternalPwmFreqSel;
  EFI_PM_HAPTICS_EXTERNAL_PWM_FREQ_SEL     ExternalPwmFreqSel;
  EFI_PM_HAPTICS_PWM_CAP_SEL               PwmCapSel; 
  EFI_PM_HAPTICS_WF_REPEAT                 WfRepeat;
};

#endif  /* __EFIPMICHAPTICS_H__ */


