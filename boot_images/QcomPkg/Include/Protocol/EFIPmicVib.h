/** @file EFIPmicVib.h

  PMIC VIB Protocol definitions

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/18/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 07/27/12   al      Renamed all Enums,added device index and resource index
 02/13/12   sm      New File
=============================================================================*/
#ifndef __EFIPMICVIB_H__
#define __EFIPMICVIB_H__

/** @ingroup efi_pmicVibrator_constants 
  Protocol version.
*/

#define PMIC_VIB_REVISION 0x0000000000010001

/** @cond */
/* PMIC VIB UEFI typedefs */
typedef struct _EFI_QCOM_PMIC_VIB_PROTOCOL   EFI_QCOM_PMIC_VIB_PROTOCOL;
/** @endcond */

/* PMIC UEFI VIB Procotol GUID */
/** @ingroup efi_pmicVibrator_protocol */
extern EFI_GUID gQcomPmicVibProtocolGuid;

/** @addtogroup efi_pmicVibrator_data_types 
@{ */
/**
  Vibrator motor polarity.
 */
typedef enum
{
  EFI_PM_VIB_POL__ACTIVE_HIGH, /**< Active high. */
  EFI_PM_VIB_POL__ACTIVE_LOW,  /**< Active low. */
  EFI_PM_VIB_POL__INVALID      /**< Invalid polarity. */
} EFI_PM_VIB_POL_TYPE;

/**
  Vibrator motor mode.
 */
typedef enum
{
  EFI_PM_VIB_MODE__MANUAL, /**< Manual. */
  EFI_PM_VIB_MODE__DBUS1,  /**< Data bus 1. */
  EFI_PM_VIB_MODE__DBUS2,  /**< Data bus 2. */
  EFI_PM_VIB_MODE__DBUS3,  /**< Data bus 3. */
  EFI_PM_VIB_MODE__INVALID /**< Invalid mode. */
} EFI_PM_VIB_MODE_TYPE;

/**
  Vibrator motor.
 */
typedef enum
{
  EFI_PM_VIB__1,      /**< Haptic vibrator 1. */
  EFI_PM_VIB__INVALID /**< Invalid motor. */
}EFI_PM_VIB_WHICH_TYPE;
/** @} */ /* end_addtogroup efi_pmicVibrator_data_types */
                
/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_VIB_SET_VOLT */
/** @ingroup efi_pmicVibrator_set_volt
  @par Summary
  Sets the vibrator motor voltage.

  @param[in]  PMIC_DEVICE_INDEX Primary: 0.
  @param[in]  VIB               Haptic vibrator; see #EFI_PM_VIB_WHICH_TYPE 
                                for details. 
  @param[in]  ValuemV           Voltage in mV.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_VIB_SET_VOLT)(
  IN UINT32 PMIC_DEVICE_INDEX, 
  IN EFI_PM_VIB_WHICH_TYPE VIB,
  IN UINT16 ValuemV
);

/* EFI_PM_VIB_EN_VIB */
/** @ingroup efi_pmicVibrator_en_vib
  @par Summary
  Configures the haptic vibrator for a specific mode and also enables/disables
  the motor. 

  @param[in]  PMIC_DEVICE_INDEX Primary: 0.
  @param[in]  VIB               Motor; see #EFI_PM_VIB_WHICH_TYPE for details. 
  @param[in]  Mode              Mode; see #EFI_PM_VIB_MODE_TYPE for details.
  @param[in]  Enable            If TRUE, enable the motor; if FALSE, disable 
                                the motor.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_VIB_EN_VIB)(
  IN UINT32 PMIC_DEVICE_INDEX, 
  IN EFI_PM_VIB_WHICH_TYPE VIB,
  IN EFI_PM_VIB_MODE_TYPE Mode,
  IN BOOLEAN Enable
);



/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicVibrator_protocol
  @par Summary
  Qualcomm PMIC Vibrator Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_vibrator_proto_params.tex} 
 */
struct _EFI_QCOM_PMIC_VIB_PROTOCOL {
  UINT64                       Revision;
  EFI_PM_VIB_SET_VOLT          SetVolt;
  EFI_PM_VIB_EN_VIB            EnVib;
};

#endif    /* __EFIPMICVIB_H__ */

