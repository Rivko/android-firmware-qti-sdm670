/** 
  @file EFIPmicMpp.h
  @brief PMIC Multipurpose Pin (MPP) protocol interface.
*/
/*=============================================================================
  Copyright (c) 2011 - 2015 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Technologies Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/12/15   al      Updating enum for dbus
 12/08/14   al      Added inverted dbus logic  
 06/20/14   al      Arch update
 06/21/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup
 04/23/13   al      Add ConfigDtestOut
 02/27/13   al      Adding IRQ related APIs 
 01/22/13   al      Add rt. interrupt
 12/10/12   al      Using enums same as other Badger image
 7/27/12    al		Renamed all Enums,added device index and resource index
 05/03/12   vishalo Merge in Techpubs Doxygen changes
 04/19/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup
 01/26/12   vishalo Merge in Techpubs Doxygen changes
 10/21/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup
 02/24/12   sm      Added ConfigAnalogOutput, StatusGet and GetValue APIs
 05/11/11   dy      created

=============================================================================*/
#ifndef __EFIPMICMPP_H__
#define __EFIPMICMPP_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "PmicIrq.h"

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicMpp_constants 
@{ */
/**
  Protocol version.
*/
#define PMIC_MPP_REVISION 0x0000000000010005
/** @} */ /* end_addtogroup efi_pmicMpp_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicMpp_protocol */
#define EFI_PMICMPP_PROTOCOL_GUID \
	{ 0x47861dff, 0x556f, 0x4269, { 0x9b, 0x28, 0x6, 0x2e, 0xc3, 0xec, 0xb3, 0xd } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the PMIC MPP Protocol GUID 
*/
extern EFI_GUID gQcomPmicMppProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  PMIC MPP UEFI typedefs
*/
typedef struct _EFI_QCOM_PMIC_MPP_PROTOCOL   EFI_QCOM_PMIC_MPP_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicMpp_data_types 
@{ */
/** 
  MPP type.
*/
typedef enum
{
  EFI_PM_MPP_1,  /**< MPP 1. */
  EFI_PM_MPP_2,  /**< MPP 2. */
  EFI_PM_MPP_3,  /**< MPP 3. */
  EFI_PM_MPP_4,  /**< MPP 4. */
  EFI_PM_MPP_5,  /**< MPP 5. */
  EFI_PM_MPP_6,  /**< MPP 6. */
  EFI_PM_MPP_7,  /**< MPP 7. */
  EFI_PM_MPP_8,  /**< MPP 8. */
  EFI_PM_MPP_9,  /**< MPP 9. */
  EFI_PM_MPP_10, /**< MPP 10. */
  EFI_PM_MPP_11, /**< MPP 11. */
  EFI_PM_MPP_12, /**< MPP 12. */
  EFI_PM_MPP_13, /**< MPP 13. */
  EFI_PM_MPP_14, /**< MPP 14. */
  EFI_PM_MPP_15, /**< MPP 15. */
  EFI_PM_MPP_16, /**< MPP 16. */
  EFI_PM_MPP_INVALID, /**< Invalid MPP. @newpage */
} EFI_PM_MPP_WHICH_TYPE;

/*
   The types defined below are used to configure the following MPPs modes:
   DIGITAL INPUT, DIGITAL OUTPUT and BIDIRECTIONAL.
*/
/**
  Digital logic level.
*/
typedef enum
{
  EFI_PM_MPP__DLOGIC__LVL_VIO_0 = 0, /**< Voltage I/O 0. */
  EFI_PM_MPP__DLOGIC__LVL_VIO_1, /**< VIO 1. */
  EFI_PM_MPP__DLOGIC__LVL_VIO_2, /**< VIO 2. */
  EFI_PM_MPP__DLOGIC__LVL_VIO_3, /**< VIO 3. */
  EFI_PM_MPP__DLOGIC__LVL_VIO_4, /**< VIO 4. */
  EFI_PM_MPP__DLOGIC__LVL_VIO_5, /**< VIO 5. */
  EFI_PM_MPP__DLOGIC__LVL_VIO_6, /**< VIO 6. */
  EFI_PM_MPP__DLOGIC__LVL_VIO_7, /**< VIO 7. */
  EFI_PM_MPP__DLOGIC__LVL_INVALID = 8, /**< Invalid level. */
} EFI_PM_MPP_DLOGIC_LVL_TYPE;

/* MODE = DIGITAL INPUT. Configures the output logic. */
/**
  Digital logic input data bus.
*/
typedef enum
{
  EFI_PM_MPP__DLOGIC_IN__DBUS1,       /**< Data bus 1. */
  EFI_PM_MPP__DLOGIC_IN__DBUS2,       /**< Data bus 2. */
  EFI_PM_MPP__DLOGIC_IN__DBUS3,       /**< Data bus 3. */
  EFI_PM_MPP__DLOGIC_IN__DBUS4,       /**< Data bus 4. */ /* for badger */
  EFI_PM_MPP__DLOGIC_IN__DBUS_INVALID /**< Invalid data bus. */
}EFI_PM_MPP_DLOGIC_IN_DBUS_TYPE;

/* MODE = DIGITAL OUT. Configures the output logic. */
/**
  Digital logic output control.
*/
typedef enum
{
  EFI_PM_MPP__DLOGIC_OUT__CTRL_LOW,    /**< MPP output is logic low. */
  EFI_PM_MPP__DLOGIC_OUT__CTRL_HIGH,   /**< MPP output is logic high. */
  EFI_PM_MPP__DLOGIC_OUT__CTRL_MPP,    /**< MPP output is the 
                                             corresponding MPP input. */
  EFI_PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP,/**< MPP output is the corresponding
                                             inverted MPP input. */
  EFI_PM_MPP__DLOGIC_OUT__CTRL_INVALID /**< Invalid output control. @newpage */
}EFI_PM_MPP_DLOGIC_OUT_CTRL_TYPE;

/* MODE = BIDIRECTIONAL. Configures the pull up resistor. */
/**
  Digital logic I/O pull up.
*/
typedef enum
{
  EFI_PM_MPP__DLOGIC_INOUT__PUP_1K,     /**< Pull up is 1 K ohm. */
  EFI_PM_MPP__DLOGIC_INOUT__PUP_10K,    /**< Pull up is 10 K ohm. */
  EFI_PM_MPP__DLOGIC_INOUT__PUP_30K,    /**< Pull up is 30 K ohm. */
  EFI_PM_MPP__DLOGIC_INOUT__PUP_OPEN,   /**< Pull up is open. */   
  EFI_PM_MPP__DLOGIC_INOUT__PUP_INVALID /**< Invalid pull up. */
}EFI_PM_MPP_DLOGIC_INOUT_PUP_TYPE;

/* MODE = DIGITAL TEST OUTPUT. */
/**
  Digital logic output data bus.
*/
typedef enum
{
  EFI_PM_MPP__DLOGIC_OUT__DBUS1,                /**< Data bus 1. */
  EFI_PM_MPP__DLOGIC_OUT__DBUS1_INVERTED,       /**< Data bus 1 inverted. */
  EFI_PM_MPP__DLOGIC_OUT__DBUS2,                /**< Data bus 2. */
  EFI_PM_MPP__DLOGIC_OUT__DBUS2_INVERTED,       /**< Data bus 2 inverted. */
  EFI_PM_MPP__DLOGIC_OUT__DBUS3,                /**< Data bus 3. */
  EFI_PM_MPP__DLOGIC_OUT__DBUS3_INVERTED,       /**< Data bus 3 inverted. */
  EFI_PM_MPP__DLOGIC_OUT__DBUS4,                /**< Data bus 4. */
  EFI_PM_MPP__DLOGIC_OUT__DBUS4_INVERTED,       /**< Data bus 4 inverted. */
  EFI_PM_MPP__DLOGIC_OUT__DISABLED,             /**< MPP is always disabled  */
  EFI_PM_MPP__DLOGIC_OUT__ENABLED,              /**< MPP is always enabled  */
  EFI_PM_MPP__DLOGIC_OUT__PAIRED_MPP,           /**< Paired MPP  */
  EFI_PM_MPP__DLOGIC_OUT__INVERTED_PAIRED_MPP,  /**< Inverted Paired MPP   */
  EFI_PM_MPP__DLOGIC_OUT__DBUS_INVALID          /**< Invalid data bus. */
}EFI_PM_MPP_DLOGIC_OUT_DBUS_TYPE;


/*************************************************************************** 
   The types defined below are used to configure the following MPPs modes:
   ANALOG INPUT.
 ***************************************************************************/
/**
  Analog input channel.
*/
typedef enum
{
  EFI_PM_MPP__AIN__CH_AMUX5, /**< Analog multiplexer 5. */
  EFI_PM_MPP__AIN__CH_AMUX6, /**< AMUX 6. */
  EFI_PM_MPP__AIN__CH_AMUX7, /**< AMUX 7. */
  EFI_PM_MPP__AIN__CH_AMUX8, /**< AMUX 8. */
  EFI_PM_MPP__AIN__CH_ABUS1, /**< Analog bus 1. */
  EFI_PM_MPP__AIN__CH_ABUS2, /**< ABUS 2. */
  EFI_PM_MPP__AIN__CH_ABUS3, /**< ABUS 3. */
  EFI_PM_MPP__AIN__CH_ABUS4, /**< ABUS 4. */ /* added for badger */
  EFI_PM_MPP__AIN__CH_INVALID /**< Invalid input channel. @newpage */
}EFI_PM_MPP_AIN_CH_TYPE;


/*************************************************************************** 
   The types defined below are used to configure the following MPPs modes:
   CURRENT SINK.
 ***************************************************************************/
/**
  Current sink level.
*/
typedef enum
{
  EFI_PM_MPP__I_SINK__LEVEL_5mA,  /**< 5 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_10mA, /**< 10 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_15mA, /**< 15 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_20mA, /**< 20 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_25mA, /**< 25 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_30mA, /**< 30 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_35mA, /**< 35 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_40mA, /**< 40 mA. */
  EFI_PM_MPP__I_SINK__LEVEL_INVALID /**< Invalid sink level. */
}EFI_PM_MPP_I_SINK_LEVEL_TYPE;

/**
  Current sink switch.
*/
typedef enum
{
  EFI_PM_MPP__I_SINK__SWITCH_DIS,
  /**< Disabled and OFF. */
  EFI_PM_MPP__I_SINK__SWITCH_ENA,
  /**< Enabled and ON. */
  EFI_PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH,
  /**< Enabled if the MPP is high. */
  EFI_PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_LOW,
  /**< Enabled if the MPP is low. */
  EFI_PM_MPP__I_SINK__SWITCH_INVALID
  /**< Invalid sink switch. */
}EFI_PM_MPP_I_SINK_SWITCH_TYPE;

/**
  Analog output level.
*/
typedef enum
{     
  EFI_PM_MPP__AOUT__LEVEL_VREF_1p25_Volts,    /**< 1.25 V. */
  EFI_PM_MPP__AOUT__LEVEL_VREF_0p625_Volts,   /**< 0.625 V. */
  EFI_PM_MPP__AOUT__LEVEL_VREF_0p3125_Volts,  /**< 0.3125 V. */
  EFI_PM_MPP__AOUT__LEVEL_INVALID             /**< Invalid output level. */
}EFI_PM_MPP_AOUT_LEVEL_TYPE;

/**
  Analog output switch. 
*/
typedef enum
{  
  EFI_PM_MPP__AOUT__SWITCH_OFF,            /**< OFF. */
  EFI_PM_MPP__AOUT__SWITCH_ON,             /**< ON. */
  EFI_PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH, /**< ON if the MPP is high. */
  EFI_PM_MPP__AOUT__SWITCH_ON_IF_MPP_LOW,  /**< ON if the MPP is low. */
  EFI_PM_MPP__AOUT__SWITCH_INVALID /**< Invalid output switch. */
}EFI_PM_MPP_AOUT_SWITCH_TYPE;


/**
  Digital input configuration.
*/
typedef struct 
{
    EFI_PM_MPP_DLOGIC_LVL_TYPE MPP_DLOGIC_LVL;
    /**< Digital logic level; see #EFI_PM_MPP_DLOGIC_LVL_TYPE for details. */
    EFI_PM_MPP_DLOGIC_IN_DBUS_TYPE MPP_DLOGIC_IN_DBUS;
    /**< Digital logic input data bus; see #EFI_PM_MPP_DLOGIC_IN_DBUS_TYPE 
         for details. */
} EFI_PM_MPP_DIGITAL_INPUT_STATUS_TYPE;

/**
  Bidirectional configuration.
*/
typedef struct
{
    EFI_PM_MPP_DLOGIC_LVL_TYPE MPP_DLOGIC_LVL;
    /**< Digital logic level; see #EFI_PM_MPP_DLOGIC_LVL_TYPE for details. */
    EFI_PM_MPP_DLOGIC_INOUT_PUP_TYPE MPP_DLOGIC_INOUT_PUP;
    /**< Digital logic I/O pull up; see #EFI_PM_MPP_DLOGIC_INOUT_PUP_TYPE 
         for details. */
}  EFI_PM_MPP_BIDIRECTIONAL_STATUS_TYPE;

/**
  Analog input configuration.
*/
typedef struct
{
    EFI_PM_MPP_AIN_CH_TYPE MPP_AIN_CH;
    /**< Analog input channel; see #EFI_PM_MPP_AIN_CH_TYPE for details. @newpagetable */
}  EFI_PM_MPP_ANALOG_INPUT_STATUS_TYPE;

/**
  Analog output configuration.
*/
typedef struct
{
    EFI_PM_MPP_AOUT_LEVEL_TYPE MPP_AOUT_LEVEL;
    /**< Analog output level; see #EFI_PM_MPP_AOUT_LEVEL_TYPE for details. */
    EFI_PM_MPP_AOUT_SWITCH_TYPE MPP_AOUT_SWITCH;
    /**< Analog output switch; see #EFI_PM_MPP_AOUT_SWITCH_TYPE for 
         details. */
}  EFI_PM_MPP_ANALOG_OUTPUT_STATUS_TYPE;

/**
  Current sink configuration.
*/
typedef struct
{
    EFI_PM_MPP_I_SINK_LEVEL_TYPE  MPP_I_SINK_LEVEL;
    /**< Current sink level; see #EFI_PM_MPP_I_SINK_LEVEL_TYPE for details. */
    EFI_PM_MPP_I_SINK_SWITCH_TYPE MPP_I_SINK_SWITCH;
    /**< Current sink switch; see #EFI_PM_MPP_I_SINK_SWITCH_TYPE for 
         details. */
}  EFI_PM_MPP_CURRENT_SINK_STATUS_TYPE;

/**
  DTEST output configuration.
*/
typedef struct
{
    EFI_PM_MPP_DLOGIC_LVL_TYPE MPP_DLOGIC_LVL;
    /**< Digital logic level; see #EFI_PM_MPP_DLOGIC_LVL_TYPE for details. */
    EFI_PM_MPP_DLOGIC_OUT_DBUS_TYPE  MPP_DLOGIC_OUT_DBUS;
    /**< Digital logic output data bus; see #EFI_PM_MPP_DLOGIC_OUT_DBUS_TYPE 
         for details. @newpagetable */
} EFI_PM_MPP_DTEST_OUTPUT_STATUS_TYPE;

/**
  Digital output configuration.
*/
typedef struct 
{
    EFI_PM_MPP_DLOGIC_LVL_TYPE MPP_DLOGIC_LVL;
    /**< Digital logic level; see #EFI_PM_MPP_DLOGIC_LVL_TYPE for details. */
    EFI_PM_MPP_DLOGIC_OUT_CTRL_TYPE MPP_DLOGIC_OUT_CTRL;
    /**< Digital logic output control; see #EFI_PM_MPP_DLOGIC_OUT_CTRL_TYPE 
         for details. */
} EFI_PM_MPP_DIGITAL_OUTPUT_STATUS_TYPE;

/**
 MPP modes.
*/
typedef enum
{
    EFI_PM_MPP_CFG_DIGITAL_IN,    /**< Digital input.*/
    EFI_PM_MPP_CFG_DIGITAL_OUT,   /**< Digital output.*/
    EFI_PM_MPP_CFG_BIDIRECTIONAL, /**< Bidirectional.*/
    EFI_PM_MPP_CFG_ANALOG_IN,     /**< Analog input.*/
    EFI_PM_MPP_CFG_ANALOG_OUT,    /**< Analog output.*/
    EFI_PM_MPP_CFG_ISINK,         /**< Current sink.*/
    EFI_PM_MPP_CFG_DTEST_OUT      /**< DTEST output.*/
} EFI_PM_MPP_CONFIG_TYPE;


/**
  MPP status.
*/
typedef struct EFI_PM_MPP_STATUS_TYPE
{
    EFI_PM_MPP_CONFIG_TYPE UEFI_MPP_CONFIG;
    /**< MPP mode; see #EFI_PM_MPP_CONFIG_TYPE for details. */

    /*
     * Union of structures used to return MPP status.  The structure
     * that has valid data will depend on the MPP mode (input,
     * output, etc.) returned in mpp_config
    */
    /** Configuration settings. */
    union
    {
        EFI_PM_MPP_DIGITAL_INPUT_STATUS_TYPE MPP_DIGITAL_INPUT_STATUS;
        /**< Digital input; see #EFI_PM_MPP_DIGITAL_INPUT_STATUS_TYPE 
             for details. */
        /*~ IF ( _DISC_ == 0 ) s_pm_mpp_status_type.mpp_config_settings.mpp_digital_input_status */
        EFI_PM_MPP_DIGITAL_OUTPUT_STATUS_TYPE MPP_DIGITAL_OUTPUT_STATUS;
        /**< Digital output; see #EFI_PM_MPP_DIGITAL_OUTPUT_STATUS_TYPE 
             for details. */
        /*~ IF ( _DISC_ == 1 ) s_pm_mpp_status_type.mpp_config_settings.mpp_digital_output_status */
        EFI_PM_MPP_BIDIRECTIONAL_STATUS_TYPE MPP_BIDIRECTIONAL_STATUS;
        /**< Bidirectional; see #EFI_PM_MPP_BIDIRECTIONAL_STATUS_TYPE 
             for details. */
        /*~ IF ( _DISC_ == 2 ) s_pm_mpp_status_type.mpp_config_settings.mpp_bidirectional_status */
        EFI_PM_MPP_ANALOG_INPUT_STATUS_TYPE MPP_ANALOG_INPUT_STATUS;
        /**< Analog input; see #EFI_PM_MPP_ANALOG_INPUT_STATUS_TYPE 
             for details. */
        /*~ IF ( _DISC_ == 3 ) s_pm_mpp_status_type.mpp_config_settings.mpp_analog_input_status */
        EFI_PM_MPP_ANALOG_OUTPUT_STATUS_TYPE MPP_ANALOG_OUTPUT_STATUS;
        /**< Analog output; see #EFI_PM_MPP_ANALOG_OUTPUT_STATUS_TYPE 
             for details. */
        /*~ IF ( _DISC_ == 4 ) s_pm_mpp_status_type.mpp_config_settings.mpp_analog_output_status */
        EFI_PM_MPP_CURRENT_SINK_STATUS_TYPE MPP_CURRENT_SINK_STATUS;
        /**< Current sink; see #EFI_PM_MPP_CURRENT_SINK_STATUS_TYPE 
             for details. */
        /*~ IF ( _DISC_ == 5 ) s_pm_mpp_status_type.mpp_config_settings.mpp_current_sink_status */
        EFI_PM_MPP_DTEST_OUTPUT_STATUS_TYPE MPP_DTEST_OUTPUT_STATUS;
        /**< DTEST output; see #EFI_PM_MPP_DTEST_OUTPUT_STATUS_TYPE 
             for details. */
        /*~ IF ( _DISC_ == 6 ) s_pm_mpp_status_type.mpp_config_settings.mpp_dtest_output_status */
    } MPP_UEFI_CONFIG_SETTINGS;
      /**< @brief Configuration settings; see #MPP_UEFI_CONFIG_SETTINGS for details. 
           @nestedunion */
      /*~ FIELD s_pm_mpp_status_type.mpp_config_settings DISC s_pm_mpp_status_type.mpp_config */
} /** @cond */EFI_PM_MPP_STATUS_TYPE/** @endcond */;

/** @} */ /* end_addtogroup efi_pmicMpp_data_types */


/* EFI_PM_MPP_CONFIG_DIGITAL_INPUT */ 
/** @ingroup efi_pmicMpp_config_digital_input
  @par Summary
  Configures an MPP as a digital input.

  @param[in]  PMIC_DEVICE_INDEX For PMIC8941 -- Primary: 0. \n
                                For PMIC8841 -- Secondary: 1.
  @param[in]  Mpp               Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]      Level        Logic level; see #EFI_PM_MPP_DLOGIC_LVL_TYPE 
                               for details.
  @param[in]  DBus              Input data bus; see 
                                #EFI_PM_MPP_DLOGIC_IN_DBUS_TYPE for details.
   

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_CONFIG_DIGITAL_INPUT)(
  IN UINT32 PMIC_DEVICE_INDEX,
  IN EFI_PM_MPP_WHICH_TYPE                 Mpp,
  IN EFI_PM_MPP_DLOGIC_LVL_TYPE            Level,
  IN EFI_PM_MPP_DLOGIC_IN_DBUS_TYPE        DBus
);

/* EFI_PM_MPP_CONFIG_DIGITAL_OUTPUT */ 
/** @ingroup efi_pmicMpp_config_digital_output
  @par Summary
  Configures an MPP as a digital output.

  @param[in]  PMIC_DEVICE_INDEX For PMIC8941 -- Primary: 0. \n
                                For PMIC8841 -- Secondary: 1.
  @param[in]  Mpp               Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]      Level        Logic level; see #EFI_PM_MPP_DLOGIC_LVL_TYPE 
                               for details.
  @param[in]      OutputCtrl   Output control; see 
                               #EFI_PM_MPP_DLOGIC_OUT_CTRL_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_CONFIG_DIGITAL_OUTPUT)(
  IN UINT32 PMIC_DEVICE_INDEX,
  IN EFI_PM_MPP_WHICH_TYPE                 Mpp,
  IN EFI_PM_MPP_DLOGIC_LVL_TYPE            Level,
  IN EFI_PM_MPP_DLOGIC_OUT_CTRL_TYPE       OutputCtrl
);

/* EFI_PM_MPP_CONFIG_DIGITAL_INOUT */ 
/** @ingroup efi_pmicMpp_config_digital_inOut
  @par Summary
  Configures an MPP as a digital I/O.

  @param[in]  PmicDeviceIndex  For PMIC8941 -- Primary: 0. \n
                               For PMIC8841 -- Secondary: 1.
  @param[in]  Mpp              Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]      Level        Logic level; see #EFI_PM_MPP_DLOGIC_LVL_TYPE 
                               for details.
  @param[in]      PullUp       Logic I/O pull up; see 
                               #EFI_PM_MPP_DLOGIC_INOUT_PUP_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_CONFIG_DIGITAL_INOUT)(
  IN UINT32 PMIC_DEVICE_INDEX, 
  IN EFI_PM_MPP_WHICH_TYPE                 Mpp,
  IN EFI_PM_MPP_DLOGIC_LVL_TYPE            Level,
  IN EFI_PM_MPP_DLOGIC_INOUT_PUP_TYPE      PullUp
);

/* EFI_PM_MPP_CONFIG_ANALOG_INPUT */ 
/** @ingroup efi_pmicMpp_config_analog_input
  @par Summary
  Configures an MPP as an analog input.

  @param[in]  PMIC_DEVICE_INDEX For PMIC8941 -- Primary: 0. \n
                                For PMIC8841 -- Secondary: 1.
  @param[in]  Mpp               Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]      ChSelect     Analog input channel; see 
                               #EFI_PM_MPP_AIN_CH_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_CONFIG_ANALOG_INPUT)(
  IN UINT32 PMIC_DEVICE_INDEX,
  IN EFI_PM_MPP_WHICH_TYPE                 Mpp,
  IN EFI_PM_MPP_AIN_CH_TYPE                ChSelect
);

/* EFI_PM_MPP_CONFIG_ANALOG_OUTPUT */ 
/** @ingroup efi_pmicMpp_config_analog_output
  @par Summary
  Configures an MPP as an analog output.

  @param[in]  PMIC_DEVICE_INDEX For PMIC8941 -- Primary: 0. \n
                                For PMIC8841 -- Secondary: 1.
   @param[in]  Mpp              Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]      Level        Analog output level; see 
                               #EFI_PM_MPP_AOUT_LEVEL_TYPE for details.
  @param[in]      OnOff        Analog output switch; see 
                               #EFI_PM_MPP_AOUT_SWITCH_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_CONFIG_ANALOG_OUTPUT)(
  IN UINT32 PMIC_DEVICE_INDEX,
  IN EFI_PM_MPP_WHICH_TYPE        Mpp,
  IN EFI_PM_MPP_AOUT_LEVEL_TYPE   Level,
  IN EFI_PM_MPP_AOUT_SWITCH_TYPE  OnOff
);

/* EFI_PM_MPP_CONFIG_I_SINK */ 
/** @ingroup efi_pmicMpp_config_I_sink
  @par Summary
  Configures an MPP as a current sink.

  @param[in]  PMIC_DEVICE_INDEX For PMIC8941 -- Primary: 0. \n
                                For PMIC8841 -- Secondary: 1.
  @param[in]  Mpp               Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]      Level        Current sink level; see 
                               #EFI_PM_MPP_I_SINK_LEVEL_TYPE for details.
  @param[in]      OnOff        Current sink switch; see 
                               #EFI_PM_MPP_I_SINK_SWITCH_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/

typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_CONFIG_I_SINK)(
  IN UINT32 PMIC_DEVICE_INDEX,
  IN EFI_PM_MPP_WHICH_TYPE                 Mpp,
  IN EFI_PM_MPP_I_SINK_LEVEL_TYPE          Level,
  IN EFI_PM_MPP_I_SINK_SWITCH_TYPE         OnOff
);

/* EFI_PM_MPP_STATUS_GET */ 
/** @ingroup efi_pmicMpp_status_get
  @par Summary
  Gets the MPP setting parameters.

  @param[in]  PMIC_DEVICE_INDEX For PMIC8941 -- Primary: 0. \n
                                For PMIC8841 -- Secondary: 1.
  @param[in]  Mpp               Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]      Status       MPP status; see #S_EFI_PM_MPP_STATUS_TYPE 
                               for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_STATUS_GET)(
  IN UINT32 PMIC_DEVICE_INDEX,
  EFI_PM_MPP_WHICH_TYPE   Mpp,
  EFI_PM_MPP_STATUS_TYPE  *Status
);

/* EFI_PM_MPP_IRQ_ENABLE */ 
/** @ingroup efi_pmicMpp_irq_enable
  @par Summary
  Enables/disables IRQ for MPP.

  @param[in]  PMIC_DEVICE_INDEX  Primary: 0. Secondary: 1.
  @param[in]  Mpp               Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]  Enable            If TRUE, enable the interrupt for MPP; 
                                if FALSE, disable the interrupt for MPP.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_IRQ_ENABLE)(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_MPP_WHICH_TYPE      Mpp,
 IN  BOOLEAN                    Enable
);

/* EFI_PM_MPP_IRQ_CLEAR */ 
/** @ingroup efi_pmicMpp_irq_clear
  @par Summary
  Clears the MPP IRQ.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Mpp              Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_IRQ_CLEAR)(
 IN  UINT32                    PmicDeviceIndex,
 IN  EFI_PM_MPP_WHICH_TYPE     Mpp
);

/* EFI_PM_MPP_IRQ_SET_TRIGGER */ 
/** @ingroup efi_pmicMpp_irq_set_trigger
  @par Summary
  Configures the IRQ trigger for MPP.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Mpp              Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]  Trigger          IRQ trigger type. Values: \n
                               - EFI_PM_IRQ_TRIGGER_ACTIVE_LOW -- 
                                 Active low
                               - EFI_PM_IRQ_TRIGGER_ACTIVE_HIGH -- 
                                 Active high
                               - EFI_PM_IRQ_TRIGGER_RISING_EDGE -- 
                                 Rising edge
                               - EFI_PM_IRQ_TRIGGER_FALLING_EDGE -- 
                                 Falling edge
                               - EFI_PM_IRQ_TRIGGER_DUAL_EDGE -- 
                                 Dual edge
                               - EFI_PM_IRQ_TRIGGER_INVALID -- 
                                 Invalid trigger @vertspace{-13}

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_IRQ_SET_TRIGGER)(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_MPP_WHICH_TYPE      Mpp,
 IN  EFI_PM_IRQ_TRIGGER_TYPE    Trigger
);

/* EFI_PM_MPP_IRQ_STATUS */ 
/** @ingroup efi_pmicMpp_irq_status
  @par Summary
  Gets the MPP IRQ status.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Mpp              Which MPP; see #EFI_PM_MPP_WHICH_TYPE for 
                               details.
  @param[in]  Type             IRQ status to read. Values: \n
                               - EFI_PM_IRQ_STATUS_RT -- Real-time
                               - EFI_PM_IRQ_STATUS_LATCHED -- Latched
                               - EFI_PM_IRQ_STATUS_PENDING -- Pending
                               - EFI_PM_IRQ_STATUS_INVALID -- Invalid status
                               @vertspace{-13}
  @param[out] Status           IRQ status.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_IRQ_STATUS)(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_MPP_WHICH_TYPE      Mpp,
 IN  EFI_PM_IRQ_STATUS_TYPE     Type,
 OUT BOOLEAN                    *Status
);


/* EFI_PM_MPP_CONFIG_DTEST_OUT */ 
/** @ingroup efi_pmicMpp_config_dtest_output
  @par Summary
  Configures an MPP as a DTEST output.

  @param[in]  PmicDeviceIndex  Primary: 0. Secondary: 1.
  @param[in]  Mpp              Which MPP; see #EFI_PM_MPP_WHICH_TYPE for
                               details.
  @param[in]  Lvl              Dbus level to set; see
                               #EFI_PM_MPP_DLOGIC_LVL_TYPE for details.
  @param[in]  Dbus             DTEST/Dbus to set for MPP; see
                               #EFI_PM_MPP_DLOGIC_IN_DBUS_TYPE for details.
  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MPP_CONFIG_DTEST_OUT)(
 IN  UINT32                           PmicDeviceIndex,
 IN  EFI_PM_MPP_WHICH_TYPE            Mpp,
 IN  EFI_PM_MPP_DLOGIC_LVL_TYPE       Lvl,
 IN  EFI_PM_MPP_DLOGIC_OUT_DBUS_TYPE  Dbus
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicMpp_protocol
  @par Summary
  Qualcomm PMIC Multipurpose Pin (MPP) Protocol interface.

  @par Parameters
  @inputprotoparams{pmic_mpp_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_MPP_PROTOCOL {
  UINT64                              Revision;
  EFI_PM_MPP_CONFIG_DIGITAL_INPUT     ConfigDigitalInput;
  EFI_PM_MPP_CONFIG_DIGITAL_OUTPUT    ConfigDigitalOutput;
  EFI_PM_MPP_CONFIG_DIGITAL_INOUT     ConfigDigitalInOut;
  EFI_PM_MPP_CONFIG_ANALOG_INPUT      ConfigAnalogInput;
  EFI_PM_MPP_CONFIG_I_SINK            ConfigISink;
  EFI_PM_MPP_CONFIG_ANALOG_OUTPUT     ConfigAnalogOutput;
  EFI_PM_MPP_STATUS_GET               StatusGet;
  EFI_PM_MPP_IRQ_ENABLE               IrqEnable;
  EFI_PM_MPP_IRQ_CLEAR                IrqClear;
  EFI_PM_MPP_IRQ_SET_TRIGGER          IrqSetTrigger;
  EFI_PM_MPP_IRQ_STATUS               IrqStatus;
  EFI_PM_MPP_CONFIG_DTEST_OUT         ConfigDetstOut;

};

#endif  /* __EFIPMICMPP_H__ */
