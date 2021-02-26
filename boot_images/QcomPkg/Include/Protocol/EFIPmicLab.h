/** @ingroup
@file  EFIPmicLab.h
@brief PMIC lab for UEFI.
*/

/*===========================================================================

Copyright (c) 2014-2017 Qualcomm Technologies, Inc. All rights reserved.
Qualcomm Technologies Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY

when        who     what, where, why
--------    ---     ---------------------------------------------------------
10/03/17    ks      Added support for new IBB Type/Subtype
03/05/15    al      Adding API for setting ISENSE and PS_CTL
12/09/14    al      Sync with latest
06/09/14    al      Arch update
05/29/14    al      Incremented revision
05/29/14    al      Adding API to config Vdisp
05/14/14    al      New file.
===========================================================================*/

#ifndef __EFIPMICLAB_H__
#define __EFIPMICLAB_H__


/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/
/** @ingroup @addtogroup efi_pmicLab_constants
@{ */
/** @ingroup
Protocol version.
*/

#define PMIC_LAB_REVISION 0x0000000000010006
/** @ingroup @} */ /* end_addtogroup efi_pmicLab_constants */

/*  Protocol GUID definition */
/** @ingroup @ingroup efi_pmicLab_protocol */
#define EFI_PMIC_LAB_PROTOCOL_GUID \
    { 0xa966cc4f, 0xee6c, 0x424d, { 0xa7, 0x62, 0x8e, 0xd3, 0xb5, 0xa4, 0xa5, 0x1 } }

/** @ingroup @cond */

/*===========================================================================

                        EXTERNAL VARIABLES

===========================================================================*/
/** @ingroup
External reference to the PMIC LAB Protocol GUID.
*/
extern EFI_GUID gQcomPmicLabProtocolGuid;


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/** @ingroup
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_LAB_PROTOCOL   EFI_QCOM_PMIC_LAB_PROTOCOL;
/** @ingroup @endcond */

/** @ingroup @addtogroup efi_pmicLab_data_types
@{ */


typedef enum
{
    EFI_PM_LAB_ISENSE_0P5,   /*0.5 ibb current sense */
    EFI_PM_LAB_ISENSE_0P8,   /*0.8 ibb current sense*/
    EFI_PM_LAB_ISENSE_0P9,   /*0.9 ibb current sense*/
    EFI_PM_LAB_ISENSE_1P0,   /*1.0 ibb current sense*/
    EFI_PM_LAB_ISENSE_1P1,   /*1.1 ibb current sense*/
    EFI_PM_LAB_ISENSE_1P5,   /*1.5 ibb current sense*/
    EFI_PM_LAB_ISENSE_2P0,   /*2.0 ibb current sense*/
    EFI_PM_LAB_ISENSE_INVALID,
} EFI_PM_LAB_ISENSE_TYPE;

typedef enum
{
    EFI_PM_LAB_ISENSE_TAP_0P8,   /*0.8 ibb current sense*/
    EFI_PM_LAB_ISENSE_TAP_0P9,   /*0.9 ibb current sense*/
    EFI_PM_LAB_ISENSE_TAP_1P0,   /*1.0 ibb current sense*/
    EFI_PM_LAB_ISENSE_TAP_1P1,   /*1.1 ibb current sense*/
    EFI_PM_LAB_ISENSE_TAP_INVALID,
} EFI_PM_LAB_ISENSE_TAP;

typedef enum
{
    EFI_PM_LAB_ISENSE_GAIN_0P5,   /*0.5 ibb current sense */
    EFI_PM_LAB_ISENSE_GAIN_1P0,   /*1.0 ibb current sense*/
    EFI_PM_LAB_ISENSE_GAIN_1P5,   /*1.5 ibb current sense*/
    EFI_PM_LAB_ISENSE_GAIN_2P0,   /*2.0 ibb current sense*/
    EFI_PM_LAB_ISENSE_GAIN_INVALID,
} EFI_PM_LAB_ISENSE_GAIN;

typedef enum
{
    EFI_PM_LAB_LDO_STATUS__DISABLE,
    EFI_PM_LAB_LDO_STATUS__ENABLE,
    EFI_PM_LAB_LDO_STATUS__NPM,
    EFI_PM_LAB_LDO_STATUS__BYPASS,
    EFI_PM_LAB_LDO_STATUS__ACT_BYPASS,
    EFI_PM_LAB_LDO_STATUS__INVALID
} EFI_PM_LAB_LDO_STATUS;

typedef struct
{
    BOOLEAN                AmoledModeEn;        /**< FALSE is LCD and TRUE is amoled  */
    BOOLEAN                LabModuleRdy;        /**< ready to be enabled              */
    BOOLEAN                LabModuleEn;         /**< module enabled or disabled       */
    BOOLEAN                LabIbbEnRdy;         /**< allow IBB module to enable lab   */
    BOOLEAN                EnFastPrecharge;     /**< allow fast precharge             */
    UINT32                 MaxPrechgTimeUsec;   /**< maximum pre_charge time          */
    BOOLEAN                OverrideOutputVolt;  /**< override output voltage          */
    UINT32                 OutputVolt;          /**< output voltage set               */
    BOOLEAN                LabVregOk;           /**< LAB VREG OK                      */
    BOOLEAN                ShortCircuit;        /**< TRUE: short circuit fault. FALSE: no short circuit                  */
    BOOLEAN                LabStatusEn;         /**< TRUE: Current LAB status enabled; FALSE: Current LAB status disable  */
    EFI_PM_LAB_ISENSE_TAP  IsenseTap;           /**< Current sense tapping                         */
    EFI_PM_LAB_ISENSE_GAIN IsenseGain;          /**< Current sense gain                            */
    UINT32                 PsThreshold;         /**< Threshold for when pulse-skip mode is entered */
    BOOLEAN                SelectPsTable;       /**< Select PS table                               */
    BOOLEAN                SelectDigitalPs;     /**< Select digital PS                             */
    BOOLEAN                EnablePs;            /**< Enable pulse                                  */

    /* For LAB 3.0/+ */
    BOOLEAN                BoostStatus;
    EFI_PM_LAB_LDO_STATUS  LdoStatus;
    /* Lab LDO Status */
    BOOLEAN                LdoSoftstartDone;
    BOOLEAN                LdoOverCurrentDetected;
    BOOLEAN                LdoVregOk;
    /* Lab BOOST Status */
    UINT32                 BoostVset;
    /* Lab Fault Status */
    BOOLEAN                FaultShutdownSts;
} EFI_PM_LAB_STATUS_TYPE;

typedef enum
{
    EFI_PM_LAB_SWIRE_CTL__EN_VOUT,
    EFI_PM_LAB_SWIRE_CTL__EN_PD,
    EFI_PM_LAB_SWIRE_CTL__INVALID
} EFI_PM_LAB_SWIRE_PGM_CTL;

typedef enum
{
    EFI_PM_LAB_VOUT_PROGRAM__VOUT_REQ,
    EFI_PM_LAB_VOUT_PROGRAM__VOUT_DEFAULT,
    EFI_PM_LAB_VOUT_PROGRAM__INVALID
} EFI_PM_LAB_VOUT_PROGRAM;

typedef enum
{
    EFI_PM_LAB_SW_HIGH_PSRR_CTL__ENABLE_OVERRIDE,   /* Enable bit, once enabled SW triggers HIGH_PSRR entry/exit request. */
    EFI_PM_LAB_SW_HIGH_PSRR_CTL__ENABLE_FORCE,      /* Works only if SW Override Enabled */
    EFI_PM_LAB_SW_HIGH_PSRR_CTL__INVALID
} EFI_PM_LAB_SW_HIGH_PSRR_CTL;

typedef enum
{
    EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL__350MV,
    EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL__400MV,
    EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL__450MV,
    EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL__500MV,
    EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL__INVALID
} EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL;

typedef enum
{
    EFI_PM_LAB_VPH_HW_SRC_SEL__VPH_HIGH__HIGH_PSRR_EN,
    EFI_PM_LAB_VPH_HW_SRC_SEL__VPH_HIGH,
    EFI_PM_LAB_VPH_HW_SRC_SEL__HIGH_PSRR_EN,
    EFI_PM_LAB_VPH_HW_SRC_SEL__INVALID
} EFI_PM_LAB_VPH_HW_HIGH_PSRR_SRC_SEL;

typedef struct
{
    EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL vref_high_psrr_sel;
    EFI_PM_LAB_VPH_HW_HIGH_PSRR_SRC_SEL hw_high_psrr_src_sel;
} EFI_PM_LAB_VPH_ENVELOP_CTRL_TYPE;


/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/* EFI_PM_LAB_LCD_AMOLED_SEL */
/** @ingroup
  @par Summary
  This API configures module for AMOLED or LCD

  @param[in] PmicDeviceIndex     Primary: 0 Secondary: 1
  @param[in] AmoledModeEn       TRUE enables for AMOLED and FALSE for LCD

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_LCD_AMOLED_SEL)(
                                               IN UINT32 PmicDeviceIndex,
                                               IN BOOLEAN AmoledModeEn
                                               );

/* EFI_PM_LAB_MODULE_RDY */
 /** @ingroup
  @par Summary
  This API configures module to be enabled or not enabled

  @param[in] PmicDeviceIndex     Primary: 0 Secondary: 1
  @param[in] LabRdy              TRUE if ready to be enabled else FALSE

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_MODULE_RDY) (
                                            IN UINT32 PmicDeviceIndex,
                                            IN BOOLEAN LabRdy
                                            );


/* EFI_PM_LAB_MODULE_EN */
 /** @ingroup
  @par Summary
  This API enables or disabled LAB module

  @param[in] PmicDeviceIndex   Primary: 0 Secondary: 1
  @param[in] ModuleEn           TRUE allows IBB module to enable LAB and FALSE won't

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_MODULE_EN)(
                                          IN UINT32 PmicDeviceIndex,
                                          IN BOOLEAN ModuleEn
                                          );


/* EFI_PM_LAB_IBB_RDY_EN */
/** @ingroup
  @par Summary
  This API allows LAB to be enabled by IBB or not

  @param[in] PmicDeviceIndex     Primary: 0 Secondary: 1
  @param[in] IbbRdyEn           TRUE allows IBB module to enable LAB and FALSE doesn't


  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_IBB_RDY_EN) (
                                            IN UINT32 PmicDeviceIndex,
                                            IN BOOLEAN IbbRdyEn
                                            );


/* EFI_PM_LAB_CONFIG_PRECHARGE_CTRL*/
 /** @ingroup
  @par Summary
  This API configures precharge mode and sets max precharge time

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] FastPrecharge     TRUE configures the fast_precharge and FLASE slow_precharge
  @param[in] MaxTimeUsec      Max pre-charge time. Valid range is 200 to 500usec


  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_CONFIG_PRECHARGE_CTRL)(
                                                      IN UINT32 PmicDeviceIndex,
                                                      IN BOOLEAN FastPrecharge,
                                                      IN UINT32 MaxTimeUsec
                                                      );


/* EFI_PM_LAB_GET_STATUS */
 /** @ingroup
  @par Summary
  This API reads the LAB module setting status

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[out] LabStatus         Refer struct #EFI_PM_LAB_STATUS_TYPE for more info

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_GET_STATUS) (
    IN UINT32 PmicDeviceIndex,
    OUT EFI_PM_LAB_STATUS_TYPE* LabStatus
    );



/* EFI_PM_LAB_CONFIG_OUTPUT_VOLT */
/** @ingroup
  @par Summary
  This API allows to select default voltage and set output for Vdisp
  voltage

  @param[in] PmicDeviceIndex.     Primary: 0 Secondary: 1
  @param[in] OverrideOutputVolt   TRUE sets Output voltage given by SET_OUTPUT_VOLTAGE field otherwise default
  @param[in] OutputMillivolt      Output voltage. Range is 4600 to 6100 milli volt.

  @return
  EFI_SUCCESS            -- Function completed successfully. \n
  EFI_DEVICE_ERROR       -- Physical device reported an error. \n
*/
typedef
EFI_STATUS (EFIAPI * EFI_PM_LAB_CONFIG_OUTPUT_VOLT)(
    IN UINT32   PmicDeviceIndex,
    IN BOOLEAN  OverrideOutputVolt,
    IN UINT32   OutputMillivolt
    );


/**
 * @brief This configures LAB current sense
 *
 * @details
 *  This API configures current sense tapping point and current
 *  sense gain
 *
 * @param[in] PmicDeviceIndex   . : Primary: 0 Secondary: 1
 * @param[in] IsenseTap           : Current sense tapping point.values 0.8; 0.9; 1.0; 1.1; Refer #EFI_PM_LAB_ISENSE_TYPE for more info.
 *                                  Unsupported values will set it to default.
 * @param[in] IsenseGain          : Current sense gain. values 0.5; 1.0; 1.5; 2.0; Refer #EFI_PM_LAB_ISENSE_TYPE for more info.
 *                                  Unsupported values will set it to default.
 *
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LAB_CONFIG_CURRENT_SENSE)(
    IN UINT32                  PmicDeviceIndex,
    IN EFI_PM_LAB_ISENSE_TYPE  IsenseTap,
    IN EFI_PM_LAB_ISENSE_TYPE  IsenseGain
    );


 /**
 * @brief This configures LAB pulse skipping parameters
 *
 * @details
 *  This API configures pulse skipping parameters
 *
 * @param[in] PmicDeviceIndex.  : Primary: 0 Secondary: 1
 * @param[in] PsThreshold       : Threshold when pulse skipping mode is entered. Range is 20 to 50 mAmps
 * @param[in] SelectPsTable     : TRUE to slected PS table, otherwise FALSE
 * @param[in] SelectDigitalPs   : TRUE to slected DIGITAL PS, otherwise FALSE for ANALOG
 * @param[in] EnablePs          : TRUE to enable PS, otherwise FALSE to disable it
 *
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_LAB_CONFIG_PULSE_SKIP_CTRL)(
    IN UINT32   PmicDeviceIndex,
    IN UINT32   PsThreshold,
    IN BOOLEAN  SelectPsTable,
    IN BOOLEAN  SelectDigitalPs,
    IN BOOLEAN  EnablePs
    );


/* For LAB 3.0/+ Revision */
/* EFI_PM_LAB_SWIRE_CTRL */
/** @ingroup
  @par Summary
  This API configures LAB S-wire Signal controls.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] SwireCtl           Refer struct #EFI_PM_LAB_SWIRE_PGM_CTL for more info
  @param[in] Enable             TRUE/FALSE

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_SWIRE_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_LAB_SWIRE_PGM_CTL  SwireCtl,
    IN BOOLEAN  Enable
    );

/* EFI_PM_LAB_SET_DEFAULT_VOLTAGE */
/** @ingroup
  @par Summary
  This API configures LAB DEFAULT/RUNTIME Output Voltage.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] OpVoltageMv        4600mV to 6100mV

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_SET_DEFAULT_VOLTAGE) (
    IN UINT32  PmicDeviceIndex,
    IN UINT32  OpVoltageMv
    );

/* EFI_PM_LAB_CURRENT_LIMIT_CFG */
/** @ingroup
  @par Summary
  This API configures LAB Module Current Limit settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] EnCurrentLimit     TRUE/FALSE
  @param[in] CurrentMax         200mA to 1600mA

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_CURRENT_LIMIT_CFG) (
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN EnCurrentLimit,
    IN UINT32  CurrentMax
    );

/* EFI_PM_LAB_SW_HIGH_PSRR_CTRL */
/** @ingroup
  @par Summary
  This API configures LAB Module's PSRR settings.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] SwHighPsrrCtl      Refer struct #EFI_PM_LAB_SW_HIGH_PSRR_CTL for more info
  @param[in] Enable             TRUE/FALSE

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_DEVICE_ERROR       -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_SW_HIGH_PSRR_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_LAB_SW_HIGH_PSRR_CTL  SwHighPsrrCtl,
    IN BOOLEAN  Enable
    );

/* EFI_PM_LAB_PD_CTRL */
/** @ingroup
  @par Summary
  This API configures LAB PullDown enable.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] PdEnable           TRUE/FALSE

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_PD_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN BOOLEAN  PdEnable
    );

/* EFI_PM_LAB_VPH_ENVELOP_CTRL */
/** @ingroup
  @par Summary
  This API configures LAB Module's supply mode, VPH/HIGH_PSRR.

  @param[in] PmicDeviceIndex    Primary: 0 Secondary: 1
  @param[in] VrefHighPsrrSel    Refer struct #EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL for more info
  @param[in] HwHighPsrrSrcSel   Refer struct #EFI_PM_LAB_VPH_HW_HIGH_PSRR_SRC_SEL for more info

  @return
  EFI_SUCCESS                   -- Function completed successfully.
  EFI_DEVICE_ERROR              -- Physical device reported an error
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LAB_VPH_ENVELOP_CTRL) (
    IN UINT32  PmicDeviceIndex,
    IN EFI_PM_LAB_VPH_VREF_HIGH_PSRR_SEL VrefHighPsrrSel,
    IN EFI_PM_LAB_VPH_HW_HIGH_PSRR_SRC_SEL HwHighPsrrSrcSel
    );


/*===========================================================================

                        PMIC LAB PROTOCOL INTERFACE

===========================================================================*/
/** @ingroup @ingroup efi_pmicLab_protocol
@par Summary
Qualcomm PMIC LAB Protocol interface.

@par Parameters
@inputprotoparams{pmic_lab_proto_params.tex}
*/

struct _EFI_QCOM_PMIC_LAB_PROTOCOL {
    UINT64                              Revision;
    EFI_PM_LAB_LCD_AMOLED_SEL           LcdAmoledSel;
    EFI_PM_LAB_MODULE_RDY               ModuleRdy;
    EFI_PM_LAB_MODULE_EN                ModuleEn;
    EFI_PM_LAB_IBB_RDY_EN               IbbRdyEn;
    EFI_PM_LAB_CONFIG_PRECHARGE_CTRL    PrechargeCtrl;
    EFI_PM_LAB_GET_STATUS               GetStatus;
    EFI_PM_LAB_CONFIG_OUTPUT_VOLT       ConfigOutVolt;
    EFI_PMIC_LAB_CONFIG_CURRENT_SENSE   ConfigCurrentSense;
    EFI_PMIC_LAB_CONFIG_PULSE_SKIP_CTRL ConfigPulseSkipCtrl;
    /* For LAB 3.0/+ Revision */
    EFI_PM_LAB_SWIRE_CTRL               LabSwireCtrl;
    EFI_PM_LAB_SET_DEFAULT_VOLTAGE      LabSetDefaultVoltage;
    EFI_PM_LAB_CURRENT_LIMIT_CFG        LabCurrentLimitCfg;
    EFI_PM_LAB_SW_HIGH_PSRR_CTRL        LabSwHighPsrrCtrl;
    EFI_PM_LAB_PD_CTRL                  LabPdCtrl;
    EFI_PM_LAB_VPH_ENVELOP_CTRL         LabVphEnvelopCtrl;
};

#endif  /* __EFIPMICLAB_H__ */
