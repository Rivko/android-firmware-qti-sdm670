/** 
  @file  EFIPmicVreg.h
  @brief PMIC VREG Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2012 - 2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/28/14   al      Adding get_statusÅ
 06/18/13  llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 10/23/12   al      Adding phase control
 10/23/12   al      Updating copyright info
 9/14/12    al      Added SetPwrMode api
 8/12/12    al      mentioned millivolt to be explicit
 7/27/12    al      Renamed all Enums,added device index and resource index
 05/08/12   vishalo Merge in Techpubs Doxygen change
 05/04/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/03/12   vishalo Merge in Techpubs Doxygen change
 03/24/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/26/12   vishalo Merge in Techpubs Doxygen change
 10/25/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 09/15/11   sm      Added SETLEVEL and GETLEVEL functions.
 05/11/11   dy      Created
=============================================================================*/
#ifndef __EFIPMICVREG_H__
#define __EFIPMICVREG_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicVreg_constants 
@{ */
/**
  Protocol version.
*/
#define PMIC_VREG_REVISION 0x0000000000010005
/** @} */ /* end_addtogroup efi_pmicVreg_constants */

/* Protocol GUID definition */
/** @ingroup efi_pmicVreg_protocol */
#define EFI_PMICVREG_PROTOCOL_GUID \
{ 0x22d38d3d, 0xe8b6, 0x4f8f, { 0x9c, 0x26, 0xbc, 0xeb, 0x7, 0xd6, 0xcb, 0x68 } }

/** @cond */

/**
  External reference to the PMIC VREG Protocol GUID.
*/
extern EFI_GUID gQcomPmicVregProtocolGuid;


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  PMIC VREG UEFI typedefs
*/
typedef struct _EFI_QCOM_PMIC_VREG_PROTOCOL   EFI_QCOM_PMIC_VREG_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicVreg_data_types 
@{ */
/**
  VREG resource ID.
*/
typedef enum
{
/* LDO resources */
  EFI_PM_LDO_1,     /**< Low dropout 1. */
  EFI_PM_LDO_2,     /**< LDO 2. */
  EFI_PM_LDO_3,     /**< LDO 3. */
  EFI_PM_LDO_4,     /**< LDO 4. */
  EFI_PM_LDO_5,     /**< LDO 5. */
  EFI_PM_LDO_6,     /**< LDO 6. */
  EFI_PM_LDO_7,     /**< LDO 7. */
  EFI_PM_LDO_8,     /**< LDO 8. */
  EFI_PM_LDO_9,     /**< LDO 9. */
  EFI_PM_LDO_10,    /**< LDO 10. */
  EFI_PM_LDO_11,    /**< LDO 11. */
  EFI_PM_LDO_12,    /**< LDO 12. */
  EFI_PM_LDO_13,    /**< LDO 13. */
  EFI_PM_LDO_14,    /**< LDO 14. */
  EFI_PM_LDO_15,    /**< LDO 15. */
  EFI_PM_LDO_16,    /**< LDO 16. */
  EFI_PM_LDO_17,    /**< LDO 17. */
  EFI_PM_LDO_18,    /**< LDO 18. */
  EFI_PM_LDO_19,    /**< LDO 19. */
  EFI_PM_LDO_20,    /**< LDO 20. */
  EFI_PM_LDO_21,    /**< LDO 21. */
  EFI_PM_LDO_22,    /**< LDO 22. */
  EFI_PM_LDO_23,    /**< LDO 23. */
  EFI_PM_LDO_24,    /**< LDO 24. */
  EFI_PM_LDO_25,    /**< LDO 25. */
  EFI_PM_LDO_26,    /**< LDO 26. */
  EFI_PM_LDO_27,    /**< LDO 27. */
  EFI_PM_LDO_28,    /**< LDO 28. */
  EFI_PM_LDO_29,    /**< LDO 29. */
  EFI_PM_LDO_30,    /**< LDO 30. */
  EFI_PM_LDO_31,    /**< LDO 31. */
  EFI_PM_LDO_32,    /**< LDO 32. */
  EFI_PM_LDO_33,    /**< LDO 33. */
  EFI_PM_LDO_INVALID,/**< INVALID LDO*/
/* SMPS Resources */
  EFI_PM_SMPS_1,    /**< Switched-mode power supply 1. */
  EFI_PM_SMPS_2,    /**< SMPS 2. */
  EFI_PM_SMPS_3,    /**< SMPS 3. */
  EFI_PM_SMPS_4,    /**< SMPS 4. */
  EFI_PM_SMPS_5,    /**< SMPS 5. */    
  EFI_PM_SMPS_6,    /**< SMPS 6. */
  EFI_PM_SMPS_7,    /**< SMPS 7. */
  EFI_PM_SMPS_8,    /**< SMPS 8. */
  EFI_PM_SMPS_9,    /**< SMPS 9. */
  EFI_PM_SMPS_10,   /**< SMPS 10. */
  EFI_PM_SMPS_11,   /**< SMPS 11. */
  EFI_PM_SMPS_12,   /**< SMPS 12. */
  EFI_PM_SMPS_13,   /**< SMPS 13. */
  EFI_PM_SMPS_14,   /**< SMPS 14. */
  EFI_PM_SMPS_15,   /**< SMPS 15. */
  EFI_PM_SMPS_16,   /**< SMPS 16. */
  EFI_PM_SMPS_17,   /**< SMPS 17. */
  EFI_PM_SMPS_18,   /**< SMPS 18. */
  EFI_PM_SMPS_19,   /**< SMPS 19. */
  EFI_PM_SMPS_20,   /**< SMPS 20. */
  EFI_PM_SMPS_INVALID,/**< INVALID SMPS*/
/* Voltage Switcher Resources */
  EFI_PM_VS_LVS_1,  /**< Voltage supply low voltage switch 1. */
  EFI_PM_VS_LVS_2,  /**< VS LVS 2. */
  EFI_PM_VS_LVS_3,  /**< VS LVS 3. */
  EFI_PM_VS_LVS_4,  /**< VS LVS 4. */
  EFI_PM_VS_LVS_5, /**< VS LVS  5. */
  EFI_PM_VREG_INVALID /**< Invalid VREG. */
} EFI_PM_VREG_ID_TYPE;

/**
  Software power mode.
*/
typedef enum 
{
    EFI_PM_PWR_SW_MODE_PFM = 0,     /**< Low Power mode. */
    EFI_PM_PWR_SW_MODE_BYPASS = 1,  /**< Bypass mode bit 5. */
    EFI_PM_PWR_SW_MODE_AUTO = 2,    /**< Automatic mode bit 6. */
    EFI_PM_PWR_SW_MODE_NPM = 4,     /**< Normal mode bit 7. */
    EFI_PM_PWR_SW_MODE_INVALID      /**< Invalid power mode. */
} EFI_PM_PWR_SW_MODE_TYPE;


typedef enum
{
  EFI_PM_OFF,    /**< ON  */ 
  EFI_PM_ON,     /**< OFF */
  EFI_PM_INVALID
}EFI_PM_ON_OFF_TYPE;

typedef struct
{
  EFI_PM_ON_OFF_TYPE         PullDown;         /**<  VREG pull down status        */
  EFI_PM_PWR_SW_MODE_TYPE    SwMode;           /**<  VREG software mode status    */
  EFI_PM_ON_OFF_TYPE         PinCtrled;        /**<  VREG pin controlled status   */
  EFI_PM_ON_OFF_TYPE         SwEnable;         /**<  VREG enabled/disabled status */
  BOOLEAN                    VregOk;           /**<  VREG_OK status               */
} EFI_PM_VREG_STATUS_TYPE; 
/** @} */ /* end_addtogroup efi_pmicVreg_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_VREG_CONTROL */ 
/** @ingroup efi_pmicVreg_control
  @par Summary
  Enables/disables a VREG.

  @param[in]  PmicDeviceIndex  For PMIC8941 -- Primary: 0. \n
                               For PMIC8841 -- Secondary: 1.
  @param[in]  VregId    VREG ID; see #EFI_PM_VREG_ID_TYPE for details.
  @param[in]  Enable           If TRUE, enable the VREG; if FALSE, disable 
                               the VREG.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_VREG_CONTROL)(
  IN UINT32                  PmicDeviceIndex,
  IN EFI_PM_VREG_ID_TYPE     VregId,
  IN BOOLEAN                 Enable
);

/* EFI_PM_VREG_SET_LEVEL */ 
/** @ingroup efi_pmicVreg_set_level
  @par Summary
  Sets the voltage for a VREG.

  @param[in]  PmicDeviceIndex  For PMIC8941 -- Primary: 0. \n
                               For PMIC8841 -- Secondary: 1.
  @param[in]  VregId        VREG ID; see #EFI_PM_VREG_ID_TYPE for details.
  @param[in]  VoltageInMv      Voltage level in millivolts.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_VREG_SET_LEVEL)(
  IN UINT32                PmicDeviceIndex,
  IN EFI_PM_VREG_ID_TYPE   VregId,
  IN UINT32                VoltageInMv
);

/* EFI_PM_VREG_GET_LEVEL */ 
/** @ingroup efi_pmicVreg_get_level
  @par Summary
  Gets the voltage of a VREG.

  @param[in]  PmicDeviceIndex  For PMIC8941 -- Primary: 0. \n
                               For PMIC8841 -- Secondary: 1.
  @param[in]   VregId    VREG ID; see #EFI_PM_VREG_ID_TYPE for details.
  @param[out]  Voltage   Voltage level in microvolt. 

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_VREG_GET_LEVEL)(
  IN UINT32                 PmicDeviceIndex, 
  IN  EFI_PM_VREG_ID_TYPE   VregId,
  OUT UINT32                *Voltage
);

/* EFI_PM_VREG_SET_PWR_MODE */ 
/** @ingroup efi_pmicVreg_set_pwr_mode
  @par Summary
  Sets the power mode of a VREG.

  @param[in]  PmicDeviceIndex  For PMIC8941 -- Primary: 0. \n
                               For PMIC8841 -- Secondary: 1.
  @param[in]   VregId    VREG ID; see #EFI_PM_VREG_ID_TYPE for details.
  @param[in]  SwMode           Software power mode; see 
                               #EFI_PM_PWR_SW_MODE_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_VREG_SET_PWR_MODE)( //EFI_PmicVregSetPwrMode
  IN UINT32                  PmicDeviceIndex, 
  IN EFI_PM_VREG_ID_TYPE     VregId,
  IN EFI_PM_PWR_SW_MODE_TYPE SwMode
);

/* EFI_PM_VREG_MULTIPHASE_CTRL */ 
/** @ingroup efi_pmicVreg_multiphase_ctrl
  @par Summary
  Controls the number of phases in ganged rails.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  VregId           VREG ID of the gang leader; see 
                               #EFI_PM_VREG_ID_TYPE for details.
  @param[in]  NumberOfPhase    Number of phases to be enabled in ganged rails.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_VREG_MULTIPHASE_CTRL)( 
  IN UINT32                  PmicDeviceIndex, 
  IN EFI_PM_VREG_ID_TYPE     VregId,
  IN UINT32                  NumberOfPhase
);

/* EFI_PM_VREG_GET_STATUS */ 
/** @ingroup EFI_PM_VREG_GET_STATUS
  @par Summary
  This API returns the pull down, mode, pin cntrl and enabled status of vreg 

  @param[in]   PmicDeviceIndex                
                                For Primary    :0
                                For Secondary  :1

  @param[in]   VregId           VREG ID; see #EFI_PM_VREG_ID_TYPE for details. 
  @param[out]  VregStatus       Status of rail. Refer #EFI_PM_VREG_STATUS_TYPE for more info

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_VREG_GET_STATUS)( 
  IN  UINT32                   PmicDeviceIndex, 
  IN  EFI_PM_VREG_ID_TYPE      VregId,
  OUT EFI_PM_VREG_STATUS_TYPE  *VregStatus
);

/* EFI_PM_VREG_SET_LEVEL */ 
/** @ingroup efi_pmicVreg_set_level_in_micro_volt
  @par Summary
  Sets the voltage for a VREG.

  @param[in]  PmicDeviceIndex        For Primary: 0. \n
                                     For Secondary 1.
  @param[in]  VregId                 VREG ID; see #EFI_PM_VREG_ID_TYPE for details.
  @param[in]  VoltageInMicroVolt     Voltage level in microvolts.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_VREG_SET_LEVEL_IN_MICRO_VOLT)(
  IN UINT32                PmicDeviceIndex,
  IN EFI_PM_VREG_ID_TYPE   VregId,
  IN UINT32                VoltageInMicroVolt
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicVreg_protocol
  @par Summary
  Qualcomm PMIC Voltage Regulator (VREG) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_vreg_proto_params.tex} 
*/
struct _EFI_QCOM_PMIC_VREG_PROTOCOL {
  UINT64                               Revision;
  EFI_PM_VREG_CONTROL                  VregControl;
  EFI_PM_VREG_SET_LEVEL                VregSetLevel;
  EFI_PM_VREG_GET_LEVEL                VregGetLevel;
  EFI_PM_VREG_SET_PWR_MODE             VregSetPwrMode;
  EFI_PM_VREG_MULTIPHASE_CTRL          VregMultiphaseCtrl;
  EFI_PM_VREG_GET_STATUS               VregGetStatus;
  EFI_PM_VREG_SET_LEVEL_IN_MICRO_VOLT  VregSetLevelUv;
};


#endif  /* __EFIPMICVREG_H__ */
