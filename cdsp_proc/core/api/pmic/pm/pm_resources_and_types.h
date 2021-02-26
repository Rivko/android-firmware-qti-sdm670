#ifndef PM_RESOURCES_AND_TYPES__H
#define PM_RESOURCES_AND_TYPES__H
/** @file pm_resources_and_types.h
*/
/* This file contains enumerations with resource names for different
 *  module types that should be used by external clients when accessing
 *   the resources that are required
 
 Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/api/pmic/pm/pm_resources_and_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   hw      moving some npa type to this file so we don't need to include pmapp_npa.h
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "DALStdDef.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/** @addtogroup pm_resources_and_types
@{ */
/**
 * PMIC power module voltage level
 */
typedef uint32 pm_volt_level_type;

typedef enum
{
   PM_OFF,
   PM_ON,
   PM_INVALID
}pm_on_off_type;

/** SMPS peripheral index. This enum type contains all required LDO
    regulators. */
enum
{
  PM_SMPS_1,   /**< SMPS 1. */
  PM_SMPS_2,   /**< SMPS 2. */
  PM_SMPS_3,   /**< SMPS 3. */
  PM_SMPS_4,   /**< SMPS 4. */
  PM_SMPS_5,   /**< SMPS 5. */    
  PM_SMPS_6,   /**< SMPS 6. */
  PM_SMPS_7,   /**< SMPS 7. */
  PM_SMPS_8,   /**< SMPS 8. */
  PM_SMPS_9,   /**< SMPS 9. */
  PM_SMPS_10,  /**< SMPS 10. */ 
  PM_SMPS_11,  /**< SMPS 11. */ 
  PM_SMPS_12,  /**< SMPS 12. */ 
  PM_SMPS_INVALID
};

/**
 *  Voltage switch peripheral index. This enum type contains all required
 *  VS regulators. 
 */
enum
{
  PM_VS_LVS_1,
  PM_VS_LVS_2,
  PM_VS_LVS_3,
  PM_VS_LVS_4,
  PM_VS_LVS_5,
  PM_VS_LVS_6,
  PM_VS_LVS_7,
  PM_VS_LVS_8,
  PM_VS_LVS_9,
  PM_VS_LVS_10,
  PM_VS_LVS_11,
  PM_VS_LVS_12,
  PM_VS_MVS_1,
  PM_VS_OTG_1,
  PM_VS_HDMI_1,
  PM_VS_INVALID
};

/**
 * LDO peripheral index. This enum type contains all LDO regulators that are
 * required. 
 */
enum
{
  PM_LDO_1,       /**< LDO 1. */
  PM_LDO_2,       /**< LDO 2. */
  PM_LDO_3,       /**< LDO 3. */
  PM_LDO_4,       /**< LDO 4. */
  PM_LDO_5,       /**< LDO 5. */
  PM_LDO_6,       /**< LDO 6. */
  PM_LDO_7,       /**< LDO 7. */
  PM_LDO_8,       /**< LDO 8. */
  PM_LDO_9,       /**< LDO 9. */
  PM_LDO_10,      /**< LDO 10. */
  PM_LDO_11,      /**< LDO 11. */
  PM_LDO_12,      /**< LDO 12. */
  PM_LDO_13,      /**< LDO 13. */
  PM_LDO_14,      /**< LDO 14. */
  PM_LDO_15,      /**< LDO 15. */
  PM_LDO_16,      /**< LDO 16. */
  PM_LDO_17,      /**< LDO 17. */
  PM_LDO_18,      /**< LDO 18. */
  PM_LDO_19,      /**< LDO 19. */
  PM_LDO_20,      /**< LDO 20. */
  PM_LDO_21,      /**< LDO 21. */
  PM_LDO_22,      /**< LDO 22. */
  PM_LDO_23,      /**< LDO 23. */
  PM_LDO_24,      /**< LDO 24. */
  PM_LDO_25,      /**< LDO 25. */
  PM_LDO_26,      /**< LDO 26. */
  PM_LDO_27,      /**< LDO 27. */
  PM_LDO_28,      /**< LDO 28. */
  PM_LDO_29,      /**< LDO 29. */
  PM_LDO_30,      /**< LDO 30. */
  PM_LDO_31,      /**< LDO 31. */
  PM_LDO_32,      /**< LDO 32. */
  PM_LDO_33,      /**< LDO 33. */
  PM_LDO_INVALID  /**< Invalid LDO. */
};

 /** Definitions of the CLK resources in the target.
 */
typedef enum
{
  PM_RF_CLK1,       /**< RF Clock 1. */
  PM_RF_CLK2,       /**< RF Clock 2. */
  PM_RF_CLK3,       /**< RF Clock 3. */
  PM_SLEEP_CLK1,    /**< Sleep Clock 1. */ 
  PM_CLK_XO,        /**< XO Core clock. */
  PM_CLK_BB_1,      /**< BB Clock 1. */ 
  PM_CLK_BB_2,      /**< BB Clock 2. */
  PM_CLK_DIFF_1,    /**< DIFF Clock 1. */
  PM_CLK_DIV_1,     /**< DIV Clock 1. */
  PM_CLK_DIV_2,     /**< DIV Clock 2. */
  PM_CLK_DIV_3,     /**< DIV Clock 3. */
  PM_CLK_DIST,      /**< DIST Clock. */
  PM_CLK_LN_BB,     /**< LN_BB Clock. */
  PM_ALL_CLKS,      /**< All clocks. */ 
  PM_CLK_INVALID    /**< Invalid clock. */
}pm_clk_type;

/**
 * BOB peripheral index. 
 */

enum
{
  PM_BOB_1,       /**< BOB 1. */
  PM_BOB_INVALID  /**< Invalid BOB. */
};

/* Values of modes in enum reflect  
 * register values in target
 */
typedef enum 
{
    PM_SW_MODE_BYPASS    = 3, /**< Bypass mode. */
    PM_SW_MODE_RETENTION = 4, /**< Retention mode. */
    PM_SW_MODE_LPM       = 5, /**< Low Power mode. */
    PM_SW_MODE_AUTO      = 6, /**< Auto mode */
    PM_SW_MODE_NPM       = 7, /**< Normal Power mode */
    PM_SW_MODE_INVALID
}pm_sw_mode_type;

/** BOB Software mode setting. */
typedef enum
{
  PM_BOB_MODE_PASS = 0,
  PM_BOB_MODE_PFM = 1,
  PM_BOB_MODE_AUTO = 2,
  PM_BOB_MODE_PWM = 3,
  PM_BOB_MODE_INVALID,
}pm_bob_mode_type; 



/** 
  @struct peripheral_info_type
  @brief Contains Peripheral Information such as Type, Subtype, 
         Analog and Digital Major Versions. 
 */
typedef struct 
{
    uint16 base_address;
    uint8  peripheral_type;
    uint8  peripheral_subtype;
    uint8  analog_major_version;
    uint8  analog_minor_version;
    uint8  digital_major_version;
    uint8  digital_minor_version;
}peripheral_info_type;

/** @} */ /* end_addtogroup pm_resources_and_types */

#endif /* PM_RESOURCES_AND_TYPES__H */

