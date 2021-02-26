#ifndef EFIPMICLCDB_H
#define EFIPMICLCDB_H

/** @ingroup
 * @file  EFIPmicLcdb.h
 * @brief PMIC LCDB for UEFI.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc. All rights reserved.
 * Qualcomm Technologies Confidential and Proprietary.
 */

/*===========================================================================

                        EDIT HISTORY

when        who     what, where, why
--------    ----    ---------------------------------------------------------
11/14/16    mr      Initial Revision
===========================================================================*/

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/** @ingroup @addtogroup efi_pmiclcdb_constants
@{ */
/** @ingroup
Protocol version.
*/

#define PMIC_LCDB_REVISION 0x0000000000010001
/** @ingroup @} */ /* end_addtogroup efi_pmicLcdb_constants */

/*  Protocol GUID definition */
/** @ingroup @ingroup efi_pmicLcdb_protocol */
#define EFI_PMIC_LCDB_PROTOCOL_GUID \
    { 0xb7639e28, 0xc06b, 0x47f0, { 0x9a, 0x13, 0x35, 0xee, 0xba, 0x3b, 0x51, 0xf8 } }


/** @ingroup @cond */
/*===========================================================================

                        EXTERNAL VARIABLES DEFINITIONS

===========================================================================*/
/** @ingroup
External reference to the PMIC LCDB Protocol GUID.
*/
extern EFI_GUID gQcomPmicLcdbProtocolGuid;


/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/
/** @ingroup @endcond */

/** @ingroup @addtogroup efi_pmicLcdb_data_types
@{ */

typedef struct
{
    /* LCDB Periph Status (overall) - 0xEC08, 0xEC0C */
    BOOLEAN  lcdb_sc_det;
    BOOLEAN  lcdb_vreg_ok;
    UINT8    lcdb_seq_status;
    /* BOOST Module Status - 0xEC09, 0xEC0D */
    BOOLEAN  boost_vreg_ok;
    BOOLEAN  boost_sc_det;
    UINT8    boost_status;
    UINT32   boost_output_voltage;
    /* LDO Module Status - 0xEC0A */
    BOOLEAN  ldo_vreg_ok;
    BOOLEAN  ldo_sc_detect;
    UINT8    ldo_status;
    BOOLEAN  ldo_ireg_active;
    BOOLEAN  ldo_vgs_gt_2vt;
    /* NCP Module Status - 0xEC0B */
    BOOLEAN  ncp_vreg_ok;
    BOOLEAN  ncp_sc_detect;
    UINT8    ncp_status;
} EFI_PM_LCDB_STS_TYPE;

typedef enum
{
    EFI_PM_LCDB_ISENSE_TAP__X0P8,
    EFI_PM_LCDB_ISENSE_TAP__X0P9,
    EFI_PM_LCDB_ISENSE_TAP__X1,
    EFI_PM_LCDB_ISENSE_TAP__X1P1,
    EFI_PM_LCDB_ISENSE_TAP__INVALID
} EFI_PM_LCDB_ISENSE_TAP;

typedef enum
{
    EFI_PM_LCDB_ISENSE_GAIN__X0P5,
    EFI_PM_LCDB_ISENSE_GAIN__X1,
    EFI_PM_LCDB_ISENSE_GAIN__X1P5,
    EFI_PM_LCDB_ISENSE_GAIN__X2,
    EFI_PM_LCDB_ISENSE_GAIN__INVALID
} EFI_PM_LCDB_ISENSE_GAIN;

typedef struct
{
    EFI_PM_LCDB_ISENSE_TAP  isense_tap;
    EFI_PM_LCDB_ISENSE_GAIN  isense_gain;
} EFI_PM_LCDB_CURR_SENSE_TYPE;

typedef enum
{
    EFI_PM_LCDB_PS_THRESHOLD__50MA,
    EFI_PM_LCDB_PS_THRESHOLD__60MA,
    EFI_PM_LCDB_PS_THRESHOLD__70MA,
    EFI_PM_LCDB_PS_THRESHOLD__80MA,
    EFI_PM_LCDB_PS_THRESHOLD__INVALID
} EFI_PM_LCDB_PS_THRESHOLD;

typedef struct
{
    BOOLEAN  en_ps;
    BOOLEAN  sel_dig_ps;
    BOOLEAN  sel_ps_table;
    EFI_PM_LCDB_PS_THRESHOLD  ps_threshold;
} EFI_PM_LCDB_PS_CTL_TYPE;

typedef enum
{
    EFI_PM_LCDB_CONFIG__BOOST_LDO_NCP,
    EFI_PM_LCDB_CONFIG__BOOST_LDO,
    EFI_PM_LCDB_CONFIG__BOOST_NCP,
    EFI_PM_LCDB_CONFIG__BOOST_ONLY,
    EFI_PM_LCDB_CONFIG__LDO_ONLY,
    EFI_PM_LCDB_CONFIG__NCP_ONLY,
    EFI_PM_LCDB_CONFIG__INVALID
} EFI_PM_LCDB_CONFIG_SEL;

typedef enum
{
    EFI_PM_LCDB_PFM_HYSTERESIS__15MV,
    EFI_PM_LCDB_PFM_HYSTERESIS__25MV,
    EFI_PM_LCDB_PFM_HYSTERESIS__35MV,
    EFI_PM_LCDB_PFM_HYSTERESIS__45MV,
    EFI_PM_LCDB_PFM_HYSTERESIS__INVALID
} EFI_PM_LCDB_PFM_HYSTERESIS;

typedef enum
{
    EFI_PM_LCDB_PFM_PEAK_CURRENT__300MA,
    EFI_PM_LCDB_PFM_PEAK_CURRENT__400MA,
    EFI_PM_LCDB_PFM_PEAK_CURRENT__500MA,
    EFI_PM_LCDB_PFM_PEAK_CURRENT__600MA,
    EFI_PM_LCDB_PFM_PEAK_CURRENT__INVALID
} EFI_PM_LCDB_PFM_PEAK_CURRENT;

typedef struct
{
    BOOLEAN  en_pfm;
    EFI_PM_LCDB_PFM_HYSTERESIS  pfm_hysteresis;
    EFI_PM_LCDB_PFM_PEAK_CURRENT  pfm_peak_current;
    BOOLEAN  spare;
    BOOLEAN  byp_bst_soft_start_comp;
} EFI_PM_LCDB_PFM_CTL_TYPE;

typedef enum
{
    EFI_PM_LCDB_NCP_ILIM_SS_SD_CTL__260MA,
    EFI_PM_LCDB_NCP_ILIM_SS_SD_CTL__460MA,
    EFI_PM_LCDB_NCP_ILIM_SS_SD_CTL__640MA,
    EFI_PM_LCDB_NCP_ILIM_SS_SD_CTL__810MA,
    EFI_PM_LCDB_NCP_ILIM_SS_SD_CTL__INVALID
} EFI_PM_LCDB_NCP_ILIM_SS_SD_CTL;

typedef struct
{
    BOOLEAN  en_ncp_ilim;
    BOOLEAN  byp_ncpgd_to_ilim;
    BOOLEAN  en_ncp_ilim_gain;
    EFI_PM_LCDB_NCP_ILIM_SS_SD_CTL  ncp_ilim_max_ss_sd;
} EFI_PM_LCDB_NCP_ILIM_CTL_TYPE;

typedef enum
{
    EFI_PM_LCDB_NCP_SOFTSTART_TIME__0MS,
    EFI_PM_LCDB_NCP_SOFTSTART_TIME__0P5MS,
    EFI_PM_LCDB_NCP_SOFTSTART_TIME__1MS,
    EFI_PM_LCDB_NCP_SOFTSTART_TIME__2MS,
    EFI_PM_LCDB_NCP_SOFTSTART_TIME__INVALID
} EFI_PM_LCDB_NCP_SOFTSTART_CTL;


/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
/*EFI_PM_LCDB_GET_STATUS*/
/** @ingroup
  @par Summary
  This API is to get the LCDB Module Status.

  @param[in]  PmicDeviceIndex   Primary: 0 Secondary: 1
  @param[out] LcdbStatus        LCDB Module status

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_GET_STATUS) (
    IN  UINT8 PmicDeviceIndex,
    OUT EFI_PM_LCDB_STS_TYPE *LcdbStatus
    );

/* EFI_PM_LCDB_MODULE_ENABLE */
/** @ingroup
  @par Summary
  This API is to enable/disable LCDB Module.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] ModuleEn           TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_MODULE_ENABLE) (
    IN UINT8 PmicDeviceIndex,
    IN BOOLEAN ModuleEn
    );

/* EFI_PM_LCDB_SET_MODULE_RDY */
/** @ingroup
  @par Summary
  This API is to configure LCDB Module Ready settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] ModuleRdy          TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_SET_MODULE_RDY) (
    IN UINT8 PmicDeviceIndex,
    IN BOOLEAN ModuleRdy
    );

/* EFI_PM_LCDB_SET_CONFIG_SEL */
/** @ingroup
  @par Summary
  This API is to configure/select LCDB Module settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] ConfigSel          For more detail, check EFI_PM_LCDB_CONFIG_SEL

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_SET_CONFIG_SEL) (
    IN UINT8 PmicDeviceIndex,
    IN EFI_PM_LCDB_CONFIG_SEL ConfigSel
    );

/* EFI_PM_LCDB_SET_BOOST_OUTPUT_VOLTAGE */
/** @ingroup
  @par Summary
  This API is to configure LCDB BOOST Voltage Output settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] OpVoltageMv        Value in mV, 4700 mV to 6250 mV

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_SET_BOOST_OUTPUT_VOLTAGE) (
    IN UINT8 PmicDeviceIndex,
    IN UINT32 OpVoltageMv
    );

/* EFI_PM_LCDB_MODULE_HW_ENABLE */
/** @ingroup
  @par Summary
  This API is to configure OLEDB H/w Enable settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] HwEnRdy            TRUE/FALSE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_MODULE_HW_ENABLE) (
    IN UINT8 PmicDeviceIndex,
    IN BOOLEAN HwEnRdy
    );

/* EFI_PM_LCDB_CURR_SENSE_CFG */
/** @ingroup
  @par Summary
  This API is to configure OLEDB H/w Enable settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] CurrSense          For more detail, check EFI_PM_LCDB_CURR_SENSE_TYPE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */

typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_CURR_SENSE_CFG) (
    IN UINT8 PmicDeviceIndex,
    IN EFI_PM_LCDB_CURR_SENSE_TYPE *CurrSense
    );

/* EFI_PM_LCDB_PS_CTL */
/** @ingroup
  @par Summary
  This API is to configure OLEDB H/w Enable settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] PsCtl              For more detail, check EFI_PM_LCDB_PS_CTL_TYPE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */

typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_PS_CTL) (
    IN UINT8 PmicDeviceIndex,
    IN EFI_PM_LCDB_PS_CTL_TYPE *PsCtl
    );

/* EFI_PM_LCDB_PFM_CTL */
/** @ingroup
  @par Summary
  This API is to configure OLEDB PFM Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] PfmCtl             For more detail, check EFI_PM_LCDB_PFM_CTL_TYPE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_PFM_CTL) (
    IN UINT8 PmicDeviceIndex,
    IN EFI_PM_LCDB_PFM_CTL_TYPE *PfmCtl
    );

/* EFI_PM_LCDB_SET_LDO_OUTPUT_VOLTAGE */
/** @ingroup
  @par Summary
  This API is to configure LCDB LDO Voltage Output settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] OpVoltageMv        Value in mV, 4000 mV to 6000 mV

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_SET_LDO_OUTPUT_VOLTAGE) (
    IN UINT8 PmicDeviceIndex,
    IN UINT32 OpVoltageMv
    );

/* EFI_PM_LCDB_SET_NCP_OUTPUT_VOLTAGE */
/** @ingroup
  @par Summary
  This API is to configure LCDB NCP Voltage Output settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] NcpVoutEn          TRUE/FALSE
  @param[in] OpVoltageMv        Value in mV (-ve), -4000 mV to -6000 mV

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_SET_NCP_OUTPUT_VOLTAGE) (
    IN UINT8 PmicDeviceIndex,
    IN BOOLEAN NcpVoutEn,
    IN INT32 OpVoltageMv
    );

/* EFI_PM_LCDB_NCP_ILIM_CTL */
/** @ingroup
  @par Summary
  This API is to configure OLEDB NCP ILIMIT Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] NcpIlimCtl         For more detail, check EFI_PM_LCDB_NCP_ILIM_CTL_TYPE

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_NCP_ILIM_CTL) (
    IN UINT8 PmicDeviceIndex,
    IN EFI_PM_LCDB_NCP_ILIM_CTL_TYPE *NcpIlimCtl
    );

/* EFI_PM_LCDB_NCP_SOFT_START_CTL */
/** @ingroup
  @par Summary
  This API is to configure OLEDB NCP SoftStart Control settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] NcpSoftstartTime   For more detail, check EFI_PM_LCDB_NCP_SOFTSTART_CTL

  @return
  EFI_SUCCESS                   Function completed successfully.
  EFI_DEVICE_ERROR              Physical device reported an error.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LCDB_NCP_SOFT_START_CTL) (
    IN UINT8 PmicDeviceIndex,
    IN EFI_PM_LCDB_NCP_SOFTSTART_CTL NcpSoftstartTime
    );


/*===========================================================================

                        PROTOCOL INTERFACE

===========================================================================*/
/** @ingroup @ingroup efi_pmicLcdb_protocol
@par Summary
Qualcomm PMIC LCDB Protocol interface.

@par Parameters
@inputprotoparams{pmic_lcdb_proto_params.tex}
*/

typedef struct _EFI_QCOM_PMIC_LCDB_PROTOCOL {
    UINT64                                  Revision;
    EFI_PM_LCDB_GET_STATUS                  LcdbGetStatus;
    EFI_PM_LCDB_MODULE_ENABLE               LcdbModuleEnable;
    // EFI_PM_LCDB_SET_MODULE_RDY              LcdbSetModuleRdy;
    EFI_PM_LCDB_SET_CONFIG_SEL              LcdbSetConfigSel;
    EFI_PM_LCDB_SET_BOOST_OUTPUT_VOLTAGE    LcdbSetBoostOpVoltage;
    EFI_PM_LCDB_MODULE_HW_ENABLE            LcdbModuleHwEnable;
    EFI_PM_LCDB_CURR_SENSE_CFG              LcdbCurrSenseCfg;
    EFI_PM_LCDB_PS_CTL                      LcdbPsCtl;
    EFI_PM_LCDB_PFM_CTL                     LcdbPfmCtl;
    EFI_PM_LCDB_SET_LDO_OUTPUT_VOLTAGE      LcdbSetLdoOpVoltage;
    EFI_PM_LCDB_SET_NCP_OUTPUT_VOLTAGE      LcdbSetNcpOpVoltage;
    EFI_PM_LCDB_NCP_ILIM_CTL                LcdbNcpIlimCtl;
    EFI_PM_LCDB_NCP_SOFT_START_CTL          LcdbNcpSoftstartCtl;
} EFI_QCOM_PMIC_LCDB_PROTOCOL;
extern EFI_QCOM_PMIC_LCDB_PROTOCOL PmicLcdbProtocolImplementation;

#endif  /* EFIPMICLCDB_H */
