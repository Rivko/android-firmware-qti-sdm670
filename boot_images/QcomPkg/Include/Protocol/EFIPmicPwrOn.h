#ifndef __EFIPMICPWRON_H__
#define __EFIPMICPWRON_H__

/** @file  EFIPmicPwrOn.h 
    @brief PMIC-POWERON MODULE RELATED DECLARATION

     This file contains functions and variable declarations to support 
     the PMIC POWERON module.
 */
/*=============================================================================
  Copyright (c) 2012-2016 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/16   cs      Adding PON protocol API to enable/disable edge trigger
05/10/16   al      Moving common to PON protocol and removing deprecated ones
01/15/16   al      API to read and write to PON spare registers
05/05/15   al      Adding API to read PON real time irq status
01/14/15   al      Adding API to enable/disable PON trigger
05/19/14   sm      Added API to get PBL_PON Status
                   Deprecated API GetBatteryRemovedStatus
12/09/13   al      Typo fixed in comment
11/21/13   al      Adding watchdog APIs
03/15/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
01/24/13   al      Adding API to get pmic on/off/reset reason
11/01/12   al	   Battery removal status
7/27/12    al	   Renamed all Enums,added device index and resource index
04/11/12   dy      Added GetWatchdogStatus API
04/03/12   al      Added prototype for AutopowerUp post HardReset 
03/24/12   sm      Added Doxygen comments
03/20/12   sm      New file.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "Protocol/EFIPmicPon.h"

/*===========================================================================*/

/** @ingroup efi_pmicPwrOn_constants 
  Protocol version. 
*/
#define PMIC_PWRON_REVISION 0x0000000000010009

/* PMIC POWERON UEFI typedefs */
/** @cond */
typedef struct _EFI_QCOM_PMIC_PWRON_PROTOCOL   EFI_QCOM_PMIC_PWRON_PROTOCOL;
/** @endcond */

/* PMIC UEFI POWERON Procotol GUID */
/** @ingroup efi_pmicPwrOn_protocol */
extern EFI_GUID gQcomPmicPwrOnProtocolGuid;


/** @addtogroup efi_pmicPwrOn_data_types 
@{ */
/*! \enum pm_pon_reset_cfg_type
   \brief Reset configure type.
 */
typedef enum
{
  EFI_PM_PWRON_RESET_CFG_WARM_RESET,
  EFI_PM_PWRON_RESET_CFG_NORMAL_SHUTDOWN,
  /**< Shutdown to a state of main battery removal */
  EFI_PM_PWRON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
  /**< Shutdown to a state of main battery and coin cell removal*/
  EFI_PM_PWRON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /**< Shutdown + Auto pwr up */
  EFI_PM_PWRON_RESET_CFG_HARD_RESET,
  /**< Main Batt and coin cell remove Shutdown + Auto pwr up */
  EFI_PM_PWRON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  EFI_PM_PWRON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  EFI_PM_PWRON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /**< Warm Reset and Main Batt/coin cell remove Shutdown */
  EFI_PM_PWRON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN,
  EFI_PM_PWRON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  EFI_PM_PWRON_RESET_CFG_WARM_RESET_AND_SHUTDOWN,
  /**< Warm Reset then Main Batt/coin cell remove Shutdown + Auto pwr up */
  EFI_PM_PWRON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  EFI_PM_PWRON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  EFI_PM_PWRON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  EFI_PM_PWRON_RESET_CFG_INVALID  
}EFI_PM_PWRON_RESET_CFG_TYPE;

/**
   PON PBL Status types.
*/
typedef enum
{
  EFI_PM_PWRON_PON_PBL_STATUS_XVDD_RB_OCCURRED = 6,
  /*Poweron status for xVdd_rb occurred*/
  EFI_PM_PWRON_PON_PBL_STATUS_DVDD_RB_OCCURRED = 7,
  /*Poweron status for dVdd_rb occurred*/
  EFI_PM_PWRON_PON_PBL_STATUS_INVALID,
  /*Invalid Reason*/
}EFI_PM_PWRON_PON_PBL_STATUS_TYPE;


/*! \enum pm_pon_trigger_type
   \brief Power On Triggers
 */
typedef enum 
{
   /*! 1 - Trigger from SMPL.                  */
   EFI_PM_PON_TRIGGER_SMPL,
   /*! 2 - Trigger from RTC.                   */
   EFI_PM_PON_TRIGGER_RTC,
   /*! 3 - Trigger from DC Charger.            */
   EFI_PM_PON_TRIGGER_DC_CHG,
   /*! 4 - Trigger from USB Charger.           */
   EFI_PM_PON_TRIGGER_USB_CHG,
   /*! 5 - Trigger from PON1.                  */
   EFI_PM_PON_TRIGGER_PON1,
   /*! 6 - Trigger by CBL_PWR   .              */
   EFI_PM_PON_TRIGGER_CBLPWR,
   /*! 7 - Trigger by Keypad.                  */
   EFI_PM_PON_TRIGGER_KPDPWR,
   EFI_PM_PON_TRIGGER_INVALID
}EFI_PM_PON_TRIGGER_TYPE;

typedef enum {
  /*! Bit 1 - 5   */
  EFI_PM_PON_OPTION_DC_CHG_REDGE_PON,      //Rising edge
  EFI_PM_PON_OPTION_USB_CHG_REDGE_PON,     //Rising edge 
  EFI_PM_PON_OPTION_CBLPWR_FEDGE_PON,      //Falling Edge
  EFI_PM_PON_OPTION_PON1_REDGE_PON,        //Rising edge 
  EFI_PM_PON_OPTION_KPDPWR_FEDGE_PON,      //Falling edge
  /*INVALID*/
  EFI_PM_PON_OPTION_INVALID
}EFI_PM_PON_OPTION_BIT_TYPE;

/*! \enum pm_pon_irq_type
   \brief different types of irq bit fields of pon irq module
 */
typedef enum {
  EFI_PM_PON_IRQ_KPDPWR_ON          = 0,  
  /*KPDPWR_N has been asserted for longer than debounce timer*/
  EFI_PM_PON_IRQ_RESIN_ON           = 1,  
  /*RESIN_N has been asserted for longer than debounce timer*/
  EFI_PM_PON_IRQ_CBLPWR_ON          = 2,
  /*CBLPWR_N has been asserted for longer than debounce timer*/  
  EFI_PM_PON_IRQ_KPDPWR_BARK        = 3,
  /*warning that a reset event has been triggered by KPDPWR_N*/
  EFI_PM_PON_IRQ_RESIN_BARK         = 4,
  /*warning that a reset event has been triggered by RESIN_N*/
  EFI_PM_PON_IRQ_KPDPWR_RESIN_BARK  = 5,
  /*warning that a reset event has been triggered by asserting RESIN_N and KPDPWR_N simultaneously*/
  EFI_PM_PON_IRQ_PMIC_WD_BARK       = 6,
  /*warning that a reset event has been triggered by the PMIC Watchdog timer*/
  EFI_PM_PON_IRQ_SOFT_RESET 	      = 7,
  /*warning that a reset event has been triggered by the PMIC Watchdog timer*/
  EFI_PM_PON_IRQ_INVALID
  /*INVALID*/
}EFI_PM_PON_IRQ_TYPE;


/*! \struct EFI_PM_PON_WARM_RESET_REASON_TYPE
   \brief Warm Reset reason type.
 */
typedef struct
{
  UINT32 SOFT : 1;               /* Software trigger */
  UINT32 PS_HOLD : 1;            /* PS_HOLD trigger */             
  UINT32 PMIC_WD : 1;            /* PMIC Watchdog trigger */             
  UINT32 GP1 : 1;                /* Keypad_Reset1 trigger */             
  UINT32 GP2 : 1;                /* Keypad_Reset2 trigger */             
  UINT32 KPDPWR_AND_RESIN : 1;   /* KPDPWR_N and RESIN_N trigger */
  UINT32 RESIN : 1;              /* RESIN_N trigger */
  UINT32 KPDPWR : 1;             /* KPDPWR_N trigger */
  UINT32 RESERVED : 4;           /* Reserved bits */
  UINT32 TFT : 1;                /* TFT trigger */
  UINT32 RESERVED1 : 3;          /* Reserved bits */
}EFI_PM_PON_WARM_RESET_REASON_TYPE;
  

/** @} */
/* end_addtogroup efi_pmicPON_data_types */

/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/


/* EFI_PM_PWRON_GET_PON_PBL_STATUS */ 
/** @ingroup efi_pm_PwrOn_get_pon_pbl_status
  @par Summary
  Gets the PON PBL status.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  PblStatusType    PBL Status type See 
                               #EFI_PM_PWRON_PON_PBL_STATUS_TYPE for details.
  @param[out] Status           Returns the boolean value of the pon 
                               pbl status. 
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS (EFIAPI *EFI_PM_PWRON_GET_PON_PBL_STATUS)(
  IN  UINT32                            PmicDeviceIndex, 
  IN  EFI_PM_PWRON_PON_PBL_STATUS_TYPE  PblStatusType,
  OUT BOOLEAN                         *Status
);


/* EFI_PM_PWRON_SET_PON_TRIGGER */ 
/** @ingroup efi_pm_pwron_set_pon_trigger
  @par Summary
  Sets the PON trigger.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  PonTrigger       PON trigger type See  #EFI_PM_PON_TRIGGER_TYPE for details.
  @param[in]  Enable           TRUE to enable and FALSE to disable PON trigger
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS (EFIAPI *EFI_PM_PWRON_SET_PON_TRIGGER)(
  IN UINT32 PmicDeviceIndex, 
  IN EFI_PM_PON_TRIGGER_TYPE PonTrigger,
  OUT BOOLEAN Enable
);


/* EFI_PM_PWRON_GET_PON_RT_STATUS */ 
/** @ingroup efi_pm_pwron_get_pon_rt_status
  @par Summary
  Sets the PON trigger.

  @param[in]  PmicDeviceIndex  Primary: 0.
  @param[in]  PonIrqType       PON irq type See  #EFI_PM_PON_IRQ_TYPE for details.
  @param[out] RtStatus         Read TRUE or FALSE status
 
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS (EFIAPI *EFI_PM_PWRON_GET_PON_RT_STATUS)(
  IN UINT32              PmicDeviceIndex, 
  IN EFI_PM_PON_IRQ_TYPE PonIrqType,
  OUT BOOLEAN            *RtStatus
);

/**
 * @brief Returns reason for Power On 
 * 
 * @param[in] PmicDeviceIndex. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] Reason:  
 *                PON reason type. Refer struct
 *                EFI_PM_PWRON_GET_PON_REASON.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
typedef 
EFI_STATUS (EFIAPI *EFI_PM_PON_GET_WARM_RESET_REASON)(
   IN  UINT32                             PmicDeviceIndex, 
   OUT EFI_PM_PON_WARM_RESET_REASON_TYPE* Reason
  );

/**
 * @brief Returns all pon reasons 
 * 
 * @param[in]  PmicDeviceIndex. Primary PMIC: 0 Secondary PMIC: 1
 * @param[out] PonReasons: 
 *             Bit <0:7>    PON_REASON1 
 *             BIT <16:23>  WARM_RESET_REASON1 
 *             BIT <32: 39> ON_REASON
 *             BIT <40:47>  POFF_REASON1
 *             BIT <56:63>  OFF_REASON
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
typedef 
EFI_STATUS (EFIAPI *EFI_PM_PON_GET_ALL_PON_REASONS)(
   IN  UINT32      PmicDeviceIndex, 
   OUT UINT64*     PonReasons
  );


/* EFI_PM_PWRON_SET_PON_TRIGGER */
/** @ingroup efi_pm_pwron_enable_edge_pon_trigger
@par Summary
Sets the PON trigger.

@param[in]  PmicDeviceIndex  Primary: 0.
@param[in]  PonTrigger       PON trigger type See  #EFI_PM_PON_TRIGGER_TYPE for details.
@param[in]  Enable           TRUE to enable and FALSE to disable PON edge trigger

@return
EFI_SUCCESS        -- Function completed successfully. \n
EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS(EFIAPI *EFI_PM_PWRON_ENABLE_EDGE_PON_TRIGGER)(
IN UINT32 PmicDeviceIndex,
IN EFI_PM_PON_OPTION_BIT_TYPE PonTrigger,
IN BOOLEAN Enable
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicPwrOn_protocol
  @par Summary
  Qualcomm PMIC Power On Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_power_on_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_PWRON_PROTOCOL {
  UINT64                                       Revision;
  EFI_PM_PWRON_GET_PON_PBL_STATUS              GetPonPblStatus;
  EFI_PM_PWRON_SET_PON_TRIGGER                 SetPonTrigger;
  EFI_PM_PWRON_GET_PON_RT_STATUS               GetPonRtStatus;
  EFI_PM_PON_GET_SPARE_REG                     GetSpareReg;
  EFI_PM_PON_SET_SPARE_REG                     SetSpareReg;
  EFI_PM_PON_GET_PON_REASON                    GetPonReason;
  EFI_PM_PON_WARM_RESET_STATUS                 WarmResetStatus;
  EFI_PM_PON_GET_ALL_PON_REASONS               GetAllPonReasons;
  EFI_PM_PWRON_ENABLE_EDGE_PON_TRIGGER         EnableEdgePonTrigger;
};

#endif /* __EFIPMICPWRON_H__ */

