#ifndef EFIPMICOLEDB_H
#define EFIPMICOLEDB_H

/** @ingroup
 * @file  EFIPmicOledb.h
 * @brief PMIC OLEDB for UEFI.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc. All rights reserved.
 * Qualcomm Technologies Confidential and Proprietary.
 */

/*===========================================================================

                        EDIT HISTORY

when        who     what, where, why
--------    ----    ---------------------------------------------------------
04/11/17    ks      Added EFI_PM_OLEDB_DISP_OFF_PBS_TRIGGER (CR-2004206)
02/07/17    sv      Added EFI_PmicOledbPdCtr protocol
11/14/16    mr      Initial Revision
===========================================================================*/

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/** @ingroup @addtogroup efi_pmicOledb_constants
@{ */
/** @ingroup
Protocol version.
*/

#define PMIC_OLEDB_REVISION 0x0000000000010001
/** @ingroup @} */ /* end_addtogroup efi_pmicOledb_constants */

/*  Protocol GUID definition */
/** @ingroup @ingroup efi_pmicOledb_protocol */
#define EFI_PMIC_OLEDB_PROTOCOL_GUID \
    { 0x55e2e265, 0xcdb2, 0x46df, { 0x88, 0xa5, 0x60, 0xb, 0xed, 0xef, 0x9c, 0x62 } }


/** @ingroup @cond */
/*===========================================================================

                        EXTERNAL VARIABLES DEFINITIONS

===========================================================================*/
/** @ingroup
External reference to the PMIC OLEDB Protocol GUID.
*/
extern EFI_GUID gQcomPmicOledbProtocolGuid;


/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/
/** @ingroup @endcond */

/** @ingroup @addtogroup efi_pmicOledb_data_types
@{ */

typedef struct
{
   BOOLEAN  sc_fault;
   BOOLEAN  ol_fault;
   BOOLEAN  ilim_fault;
} EFI_PM_OLEDB_STS_TYPE;

typedef enum
{
    EFI_PM_OLEDB_SWIRE_CONTROL__PD_EN,
    EFI_PM_OLEDB_SWIRE_CONTROL__VOUT_EN,
    EFI_PM_OLEDB_SWIRE_CONTROL__INVALID
} EFI_PM_OLEDB_SWIRE_CONTROL;

typedef enum
{
    EFI_PM_OLEDB_VOUT_PROGRAM__VOUT_REQ,
    EFI_PM_OLEDB_VOUT_PROGRAM__VOUT_DEFAULT,
    EFI_PM_OLEDB_VOUT_PROGRAM__INVALID
} EFI_PM_OLEDB_VOUT_PROGRAM;

typedef enum
{
    EFI_PM_OLEDB_SOFTSTART_TIME_VREF__1400uS,
    EFI_PM_OLEDB_SOFTSTART_TIME_VREF__875uS,
    EFI_PM_OLEDB_SOFTSTART_TIME_VREF__700uS,
    EFI_PM_OLEDB_SOFTSTART_TIME_VREF__525uS,
    EFI_PM_OLEDB_SOFTSTART_TIME_VREF__INVALID
} EFI_PM_OLEDB_SOFTSTART_TIME_VREF;

typedef struct
{
   EFI_PM_OLEDB_SOFTSTART_TIME_VREF  ss_time_vref;
   uint8  ss_iout_offset;
   uint8  ss_ilim_time;
} EFI_PM_OLEDB_SOFTSTART_CTL_TYPE;

typedef enum
{
    EFI_PM_OLEDB_PSM_VREF__440mV,
    EFI_PM_OLEDB_PSM_VREF__510mV,
    EFI_PM_OLEDB_PSM_VREF__580mV,
    EFI_PM_OLEDB_PSM_VREF__650mV,
    EFI_PM_OLEDB_PSM_VREF__715mV,
    EFI_PM_OLEDB_PSM_VREF__780mV,
    EFI_PM_OLEDB_PSM_VREF__850mV,
    EFI_PM_OLEDB_PSM_VREF__920mV,
    EFI_PM_OLEDB_PSM_VREF__INVALID
} EFI_PM_OLEDB_PSM_VREF;

typedef struct
{
   BOOLEAN  en_psm;
   BOOLEAN  psm_hys_ctrl;
   EFI_PM_OLEDB_PSM_VREF  psm_vref;
} EFI_PM_OLEDB_PSM_CTL_TYPE;

typedef enum
{
    EFI_PM_OLEDB_PFM_ILIM__130mA,
    EFI_PM_OLEDB_PFM_ILIM__200mA,
    EFI_PM_OLEDB_PFM_ILIM__270mA,
    EFI_PM_OLEDB_PFM_ILIM__340mA,
    EFI_PM_OLEDB_PFM_ILIM__INVALID
} EFI_PM_OLEDB_PFM_ILIM;

/* NFET off time at PFM or at ILIM event of PWM */
typedef enum
{
    EFI_PM_OLEDB_PFM_TOFF__110nS_25nS,
    EFI_PM_OLEDB_PFM_TOFF__240nS_45nS,
    EFI_PM_OLEDB_PFM_TOFF__350nS_70nS,
    EFI_PM_OLEDB_PFM_TOFF__480nS_90nS,
    EFI_PM_OLEDB_PFM_TOFF__INVALID
} EFI_PM_OLEDB_PFM_TOFF;

typedef struct
{
   BOOLEAN  en_psm;
   BOOLEAN  pfm_hys_ctrl;
   EFI_PM_OLEDB_PFM_ILIM  pfm_ilim;
   EFI_PM_OLEDB_PFM_TOFF  pfm_toff;
} EFI_PM_OLEDB_PFM_CTL_TYPE;


/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
/* EFI_PM_OLEDB_GET_STATUS */
/** @ingroup
  @par Summary
  This API is to get the OLEDB Module Status.

  @param[in]  PmicDeviceIndex   Primary: 0 Secondary: 1
  @param[out] OledbStatus       OLEDB Module Status

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_GET_STATUS) (
    IN  UINT8 PmicDeviceIndex,
    OUT EFI_PM_OLEDB_STS_TYPE *OledbStatus
    );

/* EFI_PM_OLEDB_MODULE_ENABLE */
/** @ingroup
  @par Summary
  This API is to enable/disable OLEDB Module.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] ModuleEn           TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_MODULE_ENABLE) (
    UINT8 PmicDeviceIndex,
    BOOLEAN ModuleEn
    );

/* EFI_PM_OLEDB_SET_MODULE_RDY */
/** @ingroup
  @par Summary
  This API is to configure OLEDB Module Ready settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] ModuleRdy          TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_SET_MODULE_RDY) (
    UINT8 PmicDeviceIndex,
    BOOLEAN ModuleRdy
    );

/* EFI_PM_OLEDB_SET_EXT_PIN_CTL */
/** @ingroup
  @par Summary
  This API is to configure OLEDB External Pin Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] ExtPinCtl          TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_SET_EXT_PIN_CTL) (
    UINT8 PmicDeviceIndex,
    BOOLEAN ExtPinCtl
    );

/* EFI_PM_OLEDB_SWIRE_CTL */
/** @ingroup
  @par Summary
  This API is to set OLEDB SWIRE Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] SwireControl       For more detail, check EFI_PM_OLEDB_SWIRE_CONTROL
  @param[in] Enable             TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_SWIRE_CTL) (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_SWIRE_CONTROL SwireControl,
    BOOLEAN Enable
    );

/* EFI_PM_OLEDB_VOUT_PROGRAMMING */
/** @ingroup
  @par Summary
  This API is to configure OLEDB Voltage Output settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] VoutProgram        For more detail, check EFI_PM_OLEDB_VOUT_PROGRAM
  @param[in] OpVoltageMv        Value in mV, 5000mV to 8100mV

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_VOUT_PROGRAMMING) (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_VOUT_PROGRAM VoutProgram,
    UINT32 OpVoltageMv
    );

/* EFI_PM_OLEDB_PD_CTRL */
/** @ingroup
  @par Summary
  This API is to enable/disable OLEDB Pull-down.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] PdEnable           TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_PD_CTRL) (
    UINT8 PmicDeviceIndex,
    BOOLEAN PdEnable
    );
	
/* EFI_PM_OLEDB_SOFTSTART_CTL */
/** @ingroup
  @par Summary
  This API is to set OLEDB SoftStart Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] SoftstartCtl       For more detail, check EFI_PM_OLEDB_SOFTSTART_CTL_TYPE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_SOFTSTART_CTL) (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_SOFTSTART_CTL_TYPE *SoftstartCtl
    );

/* EFI_PM_OLEDB_PSM_CTL */
/** @ingroup
  @par Summary
  This API is to set OLEDB PSM Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] PsmCtl             For more detail, check EFI_PM_OLEDB_PSM_CTL_TYPE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_PSM_CTL) (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_PSM_CTL_TYPE *PsmCtl
    );

/* EFI_PM_OLEDB_PFM_CTL */
/** @ingroup
  @par Summary
  This API is to set OLEDB PFM Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] PfmCtl             For more detail, check EFI_PM_OLEDB_PFM_CTL_TYPE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_PFM_CTL) (
    UINT8 PmicDeviceIndex,
    EFI_PM_OLEDB_PFM_CTL_TYPE *PfmCtl
    );

   /* EFI_PM_OLEDB_DISP_OFF_PBS_TRIGGER */
/** @ingroup
  @par Summary
  This API will trigger PBS sequence to execute Display power off sequence.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_OLEDB_DISP_OFF_PBS_TRIGGER) (
    UINT8 PmicDeviceIndex
    );


/*===========================================================================

                        PROTOCOL INTERFACE

===========================================================================*/
/** @ingroup @ingroup efi_pmicOledb_protocol
@par Summary
Qualcomm PMIC OLEDB Protocol interface.

@par Parameters
@inputprotoparams{pmic_oledb_proto_params.tex}
*/

typedef struct _EFI_QCOM_PMIC_OLEDB_PROTOCOL {
    UINT64                              Revision;
    EFI_PM_OLEDB_GET_STATUS             OledbGetStatus;
    EFI_PM_OLEDB_MODULE_ENABLE          OledbModuleEnable;
    EFI_PM_OLEDB_SET_MODULE_RDY         OledbSetModuleRdy;
    EFI_PM_OLEDB_SET_EXT_PIN_CTL        OledbSetExtPinCtl;
    EFI_PM_OLEDB_SWIRE_CTL              OledbSwireCtl;
    EFI_PM_OLEDB_VOUT_PROGRAMMING       OledbVoutProgramming;
    EFI_PM_OLEDB_PD_CTRL                OledbPdCtrl;
    EFI_PM_OLEDB_SOFTSTART_CTL          OledbSoftstartCtl;
    EFI_PM_OLEDB_PSM_CTL                OledbPsmCtl;
    EFI_PM_OLEDB_PFM_CTL                OledbPfmCtl;
    EFI_PM_OLEDB_DISP_OFF_PBS_TRIGGER   OledbDispOffPbsTrigger;
} EFI_QCOM_PMIC_OLEDB_PROTOCOL;
extern EFI_QCOM_PMIC_OLEDB_PROTOCOL PmicOledbProtocolImplementation;

#endif  /* EFIPMICOLEDB_H */
