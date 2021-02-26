/** 
  @file  EFIPmicLpg.h
  @brief PMIC Light Pulse Generator Protocol for UEFI.
*/
/*=============================================================================
Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/30/15   al      Added API to configure glitch removal   
 12/08/14   al      Added API to set LPG DTEST 
 06/23/14   al      Added GetStatus for testing 
 06/18/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/09/13   al      Api to enable/disable LpgPwm
 01/30/13   al      LPG channel enum changes
 10/23/12   al		Updating copyright info
 8/21/12    al		Added enums for 8974
 7/27/12    al		Renamed all Enums,added device index and resource index
 05/08/12   vishalo Merge in Techpubs Doxygen change
 05/04/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/03/12   vishalo  Merged Tech pubs changes 
 10/13/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 03/05/12   sm      Added LpgSetPWMValue API
 05/11/11   dy      created

=============================================================================*/

#ifndef __EFIPMICLPG_H__
#define __EFIPMICLPG_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicLpg_constants 
@{ */
/**
  Protocol version.
*/

#define PMIC_LPG_REVISION 0x0000000000010007
/** @} */ /* end_addtogroup efi_pmicLpg_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicLpg_protocol */
#define EFI_PMICLPG_PROTOCOL_GUID \
    { 0xfe8d6a5b, 0xd34a, 0x476f, { 0xb9, 0x72, 0x63, 0xc8, 0x95, 0x12, 0x49, 0xa4 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the PMIC LPG Protocol GUID.
*/
extern EFI_GUID gQcomPmicLpgProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_LPG_PROTOCOL   EFI_QCOM_PMIC_LPG_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicLpg_data_types 
@{ */
/**
  Pulse-width modulation predivider value.
*/
typedef enum{
    EFI_PM_LPG_PWM_PRE_DIV_1=0, /**< Value 1. */
    EFI_PM_LPG_PWM_PRE_DIV_3=1, /**< Value 3. */
    EFI_PM_LPG_PWM_PRE_DIV_5=2, /**< Value 5. */
    EFI_PM_LPG_PWM_PRE_DIV_6=3, /**< Value 6. */
/** @cond */
    EFI_PM_LPG_PWM_PRE_DIV_MAX
/** @endcond */
} EFI_PM_LPG_PWM_PRE_DIVIDE_TYPE;

/**
  LPG channel.
*/
typedef enum{
    EFI_PM_LPG_CHN_NONE,    /**< No channel. */
    EFI_PM_LPG_CHAN_1 ,     /**< Channel 1. */
    EFI_PM_LPG_CHAN_2 ,     /**< Channel 2. */
    EFI_PM_LPG_CHAN_3 ,     /**< Channel 3. */
    EFI_PM_LPG_CHAN_4 ,     /**< Channel 4. */
    EFI_PM_LPG_CHAN_5 ,     /**< Channel 5. */
    EFI_PM_LPG_CHAN_6 ,     /**< Channel 6. */
    EFI_PM_LPG_CHAN_7 ,     /**< Channel 7. */
    EFI_PM_LPG_CHAN_8,      /**< Channel 8. */
    EFI_PM_LPG_CHAN_INVALID /**< Invalid channel. */
}EFI_PM_LPG_CHAN_TYPE;

/** @cond */
/**
  LPG source: LUT or register.
 */
typedef enum{
    EFI_PM_LPG_PWM_SRC_LUT =0,        /**< Lookup table. */
    EFI_PM_LPG_PWM_SRC_PWM_REGISTER , /**< Register. */
    EFI_PM_LPG_PWM_SRC_INVALID        /**< Invalid channel. */
}EFI_PM_LPG_SRC_TYPE;
/** @endcond */

/**
  LPG pulse-width modulation frequency exponent value.
 */
typedef enum{
    EFI_PM_LPG_PWM_FREQ_EXPO_0,  /**< Frequency exponent 0. */
    EFI_PM_LPG_PWM_FREQ_EXPO_1,  /**< Frequency exponent 1. */
    EFI_PM_LPG_PWM_FREQ_EXPO_2,  /**< Frequency exponent 2. */
    EFI_PM_LPG_PWM_FREQ_EXPO_3,  /**< Frequency exponent 3. */
    EFI_PM_LPG_PWM_FREQ_EXPO_4,  /**< Frequency exponent 4. */
    EFI_PM_LPG_PWM_FREQ_EXPO_5,  /**< Frequency exponent 5. */
    EFI_PM_LPG_PWM_FREQ_EXPO_6,  /**< Frequency exponent 6. */
    EFI_PM_LPG_PWM_FREQ_EXPO_7,  /**< Frequency exponent 7. @newpage */
/** @cond */
    EFI_PM_LPG_PWM_FREQ_EXPO_MAX
/** @endcond */
} EFI_PM_LPG_PWM_FREQ_EXPO_TYPE;

/**
  Pulse-width modulation clock selection.
*/
typedef enum{
    EFI_PM_LPG_PWM_CLOCK_OFF,      /**< Clock is OFF. */
    EFI_PM_LPG_PWM_CLOCK_1_0_KHZ,  /**< Clock is 1 kHz. */
    EFI_PM_LPG_PWM_CLOCK_32_0_KHZ, /**< Clock is 32 kHz. */
    EFI_PM_LPG_PWM_CLOCK_19_2_MHZ, /**< Clock is 19.2 MHz. */
/** @cond */
    EFI_PM_LPG_PWM_CLOCK_MAX
/** @endcond */
} EFI_PM_LPG_PWM_CLOCK_TYPE;

/**
  Pulse-width modulation bit size.
*/
typedef enum{
    EFI_PM_LPG_PWM_6BIT, /**< 6 bits. */
    EFI_PM_LPG_PWM_7BIT, /**< 7 bits. */
    EFI_PM_LPG_PWM_8BIT, /**< 8 bits. */
    EFI_PM_LPG_PWM_9BIT, /**< 9 bits. */
/** @cond */
    EFI_PM_LPG_PWM_SIZE_MAX
/** @endcond */
} EFI_PM_LPG_PWM_BIT_SIZE_TYPE;

/*! \struct pm_lpg_status_type
 *  \brief LPG status
 */
typedef struct{
   BOOLEAN PwmOutput;
   /* LPG PWM output enabled or disabled */ 
   UINT16 PwmValue;
   /* LPG PWM value for duty cycle */
   EFI_PM_LPG_PWM_PRE_DIVIDE_TYPE PreDiv;
   /* LPG PWM frequency pre-divider */
   EFI_PM_LPG_PWM_FREQ_EXPO_TYPE Exp;
   /* LPG PWM frequency divider exponent */
   EFI_PM_LPG_PWM_CLOCK_TYPE PwmFreqClk;
   /* LPG PWM clock frequency */
   EFI_PM_LPG_PWM_BIT_SIZE_TYPE  PwmBitSize;
   /* LPG PWM bit size */
   EFI_PM_LPG_SRC_TYPE LpgSrcType;
   /* LPG source */
   BOOLEAN GlitchRemoval;
   /* Glitch removal enable/disable*/
   BOOLEAN FullScale;
   /* LPG full scale enabled or disabled */
   BOOLEAN EnPhaseStagger;
   /* LPG phase stagger enabled or disabled */
   BOOLEAN RampDirection;
   /* LPG ramp direction HI or LO */
   BOOLEAN PatternRepeat;
   /* LPG pattern repeat enabled or disabled */
   BOOLEAN RampToggle;
   /* LPG ramp toggle enabled or disabled */
   BOOLEAN EnPauseHi;
   /* LPG pause high enabled or disabled */
   BOOLEAN EnPauseLo;
   /* LPG pause low enabled or disabled */
   UINT32 LutLowIndex; 
   /* LPG LUT low index value */
   UINT32 LutHighIndex;
   /* LPG LUT high index value */
   UINT32 RampStepDuration;
  /* LPG interval count value*/
   UINT32 HiMultiplier;
   /* Pause Hi multiuplier */
   UINT32 LowMultiplier;
   /* Pause low multiuplier */
}EFI_PM_LPG_STATUS_TYPE;


/*! \struct EFI_PM_LPG_CHAN_DTEST_TYPE
 *  \brief LPG DTEST
 */
typedef enum 
{
    EFI_PM_LPG_CHAN_DTEST_1,
    /*LPG DTEST 1*/
    EFI_PM_LPG_CHAN_DTEST_2,
    /*LPG DTEST 2*/
    EFI_PM_LPG_CHAN_DTEST_3,
    /*LPG DTEST 3*/
    EFI_PM_LPG_CHAN_DTEST_4,
    /*LPG DTEST 4*/
    EFI_PM_LPG_CHAN_DTEST_INVALID
    /*LPG DTEST INVALID*/
} EFI_PM_LPG_CHAN_DTEST_TYPE;

/*! \struct EFI_PM_LPG_CHAN_LPG_OUT_TYPE
 *  \brief LPG out of each LPG channel
 */
typedef enum 
{
    EFI_PM_LPG_CHAN_LPG_OUT_DISABLED,
    /*LPG OUT DISABLED*/
    EFI_PM_LPG_CHAN_LPG_OUT_1,
    /*LPG OUT 1*/
    EFI_PM_LPG_CHAN_LPG_OUT_2,
    /*LPG OUT 2*/
    EFI_PM_LPG_CHAN_LPG_OUT_3,
    /*LPG OUT 3*/
    EFI_PM_LPG_CHAN_LPG_OUT_4,
    /*LPG OUT 4*/
    EFI_PM_LPG_CHAN_LPG_OUT_INVALID
    /*LPG OUT INVALID*/
} EFI_PM_LPG_CHAN_LPG_OUT_TYPE;

/** @} */ /* end_addtogroup efi_pmicLpg_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_LPG_CONFIG */ 
/** @ingroup efi_pmicLpg_config
  @par Summary
  Configures the LPG.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  WhichLpg         Which LPG to configure; see 
                               #EFI_PM_LPG_CHAN_TYPE for details.
  @param[in]  PwmValue        Pulse-width modulation value.
  @param[in]  PreDiv          Predivider; see #EFI_PM_LPG_PWM_PRE_DIVIDE_TYPE 
                              for details.
  @param[in]  PreDivExponent   Predivider exponent; see 
                               #EFI_PM_LPG_PWM_FREQ_EXPO_TYPE for details.
  @param[in]  Clock           Clock selection; see #EFI_PM_LPG_PWM_CLOCK_TYPE 
                              for details.
  @param[in]  Size            Bit size; see #EFI_PM_LPG_PWM_BIT_SIZE_TYPE 
                              for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_LPG_CONFIG)(
  IN UINT32                         PmicDeviceIndex, 
  IN EFI_PM_LPG_CHAN_TYPE           WhichLpg,
  IN UINT16                         PwmValue,
  IN EFI_PM_LPG_PWM_PRE_DIVIDE_TYPE PreDiv,
  IN EFI_PM_LPG_PWM_FREQ_EXPO_TYPE  PreDivExponent,
  IN EFI_PM_LPG_PWM_CLOCK_TYPE      Clock,
  IN EFI_PM_LPG_PWM_BIT_SIZE_TYPE   Size
);

/* EFI_PM_LPG_SET_PWM_VALUE */ 
/** @ingroup efi_pmicLpg_set_pwm_value
  @par Summary
  Sets the pulse-width modulation value.
  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  WhichLpg         Which LPG to configure; see 
                               #EFI_PM_LPG_CHAN_TYPE for details.
  @param[in]  PwmValue        Pulse-width modulation value.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_LPG_SET_PWM_VALUE)(
   IN UINT32               PmicDeviceIndex, 
   IN EFI_PM_LPG_CHAN_TYPE WhichLpg,
   IN UINT16               PwmValue
);


/* EFI_PM_LPG_PWM_ENABLE */ 
/** @ingroup efi_pmicLpg_enable_lpg_pwm
  @par Summary
  Enables/disables the LPG pulse-width modulation.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1. 
  @param[in]  WhichLpg         Which LPG to configure; see 
                               #EFI_PM_LPG_CHAN_TYPE for details.
  @param[in]  Enable           If TRUE, enable the LPG PWM;
                               if FALSE, disable the LPG PWM.
  
  Enables/Disabled LPG PWM

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_LPG_PWM_ENABLE)(
   IN UINT32                PmicDeviceIndex,
   IN EFI_PM_LPG_CHAN_TYPE  WhichLpg,
   IN BOOLEAN               Enable
);

 /**
 * @brief This function reads the LPG status
 * 
 * @param[in]  PmicDeviceIndex : PMIC device index. 0 for primary and 1 for secondary
 * @param[in]  WhichLpg        : Which LPG to configure.
 * @param[out] LpgStatus       : LPG status.  Refer enum #EFI_PM_LPG_STATUS_TYPE for more info
 * 
 * @return
 *       EFI_SUCCESS        -- Function completed successfully. 
 *       EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LPG_GET_STATUS)(
  IN  UINT32                  PmicDeviceIndex,
  IN  EFI_PM_LPG_CHAN_TYPE    WhichLpg,
  OUT EFI_PM_LPG_STATUS_TYPE  *LpgStatus
);

 /**
 * @brief This function reads the LPG status
 * 
 * @param[in]  PmicDeviceIndex : PMIC device index. 0 for primary and 1 for secondary
 * @param[in]  WhichLpg        : Which LPG to configure.
 * @param[in]  LpgDtest        : LPG DTEST line. Refer enum #EFI_PM_LPG_CHAN_DTEST_TYPE for more info
 * @param[in]  LpgOut          : LPG OUT. Refer enum #EFI_PM_LPG_CHAN_LPG_OUT_TYPE for more info
 * 
 * @return
 *       EFI_SUCCESS        -- Function completed successfully. 
 *       EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LPG_SET_LPG_DTEST)(
  IN  UINT32                       PmicDeviceIndex,
  IN  EFI_PM_LPG_CHAN_TYPE         WhichLpg,
  IN  EFI_PM_LPG_CHAN_DTEST_TYPE   Dtest,
  IN  EFI_PM_LPG_CHAN_LPG_OUT_TYPE LpgOut
);


 /**
 * @brief This function configures glitch removal. If set then PWM outputs are updated only on PWM period
 *  boundaries else PWM outputs are updated immediately
 * 
 * @param[in]  PmicDeviceIndex : PMIC device index. 0 for primary and 1 for secondary
 * @param[in]  WhichLpg        : Which LPG to configure.
 * @param[in]  Enable          : TRUE - glitch removal, PWM outputs are updated only onPWM period boundaries
                                 FALSE - no glitch removal, PWM outputs are updated immediately
 * 
 * @return
 *       EFI_SUCCESS        -- Function completed successfully. 
 *       EFI_PROTOCOL_ERROR -- Error occurred during the operation.
 */
typedef
EFI_STATUS (EFIAPI *EFI_PM_LPG_CONFIG_GLITCH_REMOVAL)(
  IN  UINT32                       PmicDeviceIndex,
  IN  EFI_PM_LPG_CHAN_TYPE         WhichLpg,
  IN  BOOLEAN                      Enable
  );
/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicLpg_protocol
  @par Summary
  Qualcomm PMIC Light Pulse Generator (LPG) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_lpg_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_LPG_PROTOCOL {
  UINT64                               Revision;
  EFI_PM_LPG_CONFIG                    LpgConfig;
  EFI_PM_LPG_SET_PWM_VALUE             LpgSetPWMValue;
  EFI_PM_LPG_PWM_ENABLE                LpgPwmEnable;
  EFI_PM_LPG_GET_STATUS                GetStatus;
  EFI_PM_LPG_SET_LPG_DTEST             SetLpgDtest;
  EFI_PM_LPG_CONFIG_GLITCH_REMOVAL     ConfigGlitchRemoval;
};

#endif  /* __EFIPMICLPG_H__ */
