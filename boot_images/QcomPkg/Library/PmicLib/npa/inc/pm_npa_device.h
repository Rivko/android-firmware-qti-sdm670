#ifndef PM_NPA_DEVICE_H
#define PM_NPA_DEVICE_H
/*===========================================================================


P M    N P A    D E V I C E    H E A D E R    F I L E

DESCRIPTION
This file contains prototype definitions npa device layer

Copyright (c) 2012-2015 by QUALCOMM, Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/13/14   va      Updating HDMI EXT NPA node and PCIE 
03/26/14   al      Initializing handle to prevent warning during compilation
01/22/13   vk      Added suppressible request support
04/11/12    hs     Removed duplicated type def pm_npa_freq_type.
02/12/12   wra     KVP based device setting initial file 
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/


#include "pm_uefi.h"
#include "pm_smps.h"
#include "npa.h"
#include "npa_resource.h"


/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/
/* Device Remote Resource */
#define PMIC_NPA_RMT_PROTOCOL_RPM_TYPE    "/protocol/rpm/rpm"
#define PMIC_NPA_RPM_INTERNAL_TYPE        "/protocol/rpm/internal"

typedef enum
{
  PM_NPA_VREG_LDO,
  PM_NPA_VREG_SMPS,
  PM_NPA_VREG_NCP,
  PM_NPA_VREG_VS,
  PM_NPA_CLK_BUFF,
  PM_NPA_PBS,
  PM_NPA_VREG_MAX,
}pm_npa_resouce_type;

/*!
 * Frequency Reason
 */
typedef enum
{    
    PM_NPA_FREQ_REASON_NONE = 0, // default
    PM_NPA_FREQ_REASON_BT = 1,
    PM_NPA_FREQ_REASON_GPS = 2,
    PM_NPA_FREQ_REASON_WLAN = 4,
    PM_NPA_FREQ_REASON_WAN = 8,
    PM_NPA_FREQ_REASON_INVALID
}pm_npa_freq_reason_type;

#define PM_NPA_KEY_SOFTWARE_ENABLE 0x6E657773 // swen - software enable
#define PM_NPA_KEY_LDO_SOFTWARE_MODE 0X646D736C // lsmd - LDO software mode
#define PM_NPA_KEY_SMPS_SOFTWARE_MODE 0X646D7373 // ssmd - SMPS software mode
#define PM_NPA_KEY_PIN_CTRL_ENABLE 0x6E656370 //pcen - pin control enable
#define PM_NPA_KEY_PIN_CTRL_POWER_MODE 0x646d6370 // pcmd - pin control mode
#define PM_NPA_KEY_CURRENT 0x616D //ma
#define PM_NPA_KEY_MICRO_VOLT 0x7675 //uv
#define PM_NPA_KEY_FREQUENCY 0x71657266 //freq
#define PM_NPA_KEY_FREQUENCY_REASON 0x6E736572 //resn
#define PM_NPA_KEY_FOLLOW_QUIET_MODE 0x6D71 //qm
#define PM_NPA_KEY_HEAD_ROOM 0x7268 // hr
#define PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY 0x62636370  // pccb - clk buffer pin control
#define PM_NPA_KEY_BYPASS_ALLOWED_KEY 0x61707962  //bypa - bypass allowed
#define PM_NPA_KEY_CORNER_LEVEL_KEY 0x6E726F63 // corn - coner voltage
#define PM_NPA_KEY_ACTIVE_FLOOR 0x636676
#define PM_NPA_KEY_CORNER_VOLT_LEVEL        0x6C766C76   //corner voltage level
#define PM_NPA_KEY_CORNER_VOLT_LEVEL_FLOOR  0x6C6676     //corner voltage level floor


/*! 
 * Generic enable,
 */
typedef enum
{
    PM_NPA_GENERIC_DISABLE, // default
    PM_NPA_GENERIC_ENABLE,
    PM_NPA_GENERIC_INVALID
}pm_npa_generic_enable_type;

/*!
 * Bypass Allowed
 */
typedef enum
{
    PM_NPA_BYPASS_ALLOWED, // default
    PM_NPA_BYPASS_DISALLOWED,
    PM_NPA_BYPASS_INVALID
}pm_npa_bypass_allowed_type;

/*! 
 * Quiet Mode
 */
typedef enum
{
    PM_NPA_QUIET_MODE__DISABLE, // default
    PM_NPA_QUIET_MODE__QUIET,
    PM_NPA_QUIET_MODE__SUPER_QUIET,
    PM_NPA_QUIET_MODE__INVALID
}pm_npa_quiet_mode_type;

/*!
 * LDO Software Mode
 */
typedef enum
{
    PM_NPA_SW_MODE_LDO__IPEAK, // default
    PM_NPA_SW_MODE_LDO__NPM,
    PM_NPA_SW_MODE_LDO__INVALID
}pm_npa_sw_mode_ldo_type;
/*!
 * SMPS Software Mode
 */
typedef enum
{
    PM_NPA_SW_MODE_SMPS__AUTO, // default
    PM_NPA_SW_MODE_SMPS__IPEAK,
    PM_NPA_SW_MODE_SMPS__NPM,
    PM_NPA_SW_MODE_SMPS__INVALID
}pm_npa_sw_mode_smps_type;

/*!
 * Pin Control Enable
 */
typedef enum
{
    PM_NPA_PIN_CONTROL_ENABLE__NONE = 0, // default
    PM_NPA_PIN_CONTROL_ENABLE__EN1 = 1,
    PM_NPA_PIN_CONTROL_ENABLE__EN2 = 2,
    PM_NPA_PIN_CONTROL_ENABLE__EN3 = 4,
    PM_NPA_PIN_CONTROL_ENABLE__EN4 = 8,
    PM_NPA_PIN_CONTROL_ENABLE__INVALID
}pm_npa_pin_control_enable_type;

/*!
 * Pin Control Power Mode
 */
typedef enum
{
    PM_NPA_PIN_CONTROL_POWER_MODE__NONE = 0, // default
    PM_NPA_PIN_CONTROL_POWER_MODE__EN1 = 1,
    PM_NPA_PIN_CONTROL_POWER_MODE__EN2 = 2,
    PM_NPA_PIN_CONTROL_POWER_MODE__EN3 = 4,
    PM_NPA_PIN_CONTROL_POWER_MODE__EN4 = 8,
    PM_NPA_PIN_CONTROL_POWER_MODE__SLEEPB = 16,
    PM_NPA_PIN_CONTROL_POWER_MODE__INVALID
}pm_npa_pin_control_power_mode_type;


/*!
 * Pin Control Power Mode
 */
typedef enum
{
    PM_NPA_CORNER_MODE__NONE = 0, // default
    PM_NPA_CORNER_MODE__1 = 1,
    PM_NPA_CORNER_MODE__2 = 2,
    PM_NPA_CORNER_MODE__3 = 3,
    PM_NPA_CORNER_MODE__4 = 4,
    PM_NPA_CORNER_MODE__5 = 5,
    PM_NPA_CORNER_MODE__6 = 6,
    PM_NPA_CORNER_MODE__INVALID
}pm_npa_corner_mode_type;

/*!
 * Pin Control Enable
 */
typedef enum
{
    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE = 0, // default
    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__EN1 = 1,
    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__INVALID
}pm_npa_clk_buff_pin_control_enable_type;


/*===========================================================================

DEFINITIONS

===========================================================================*/

#define PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index )  \
    "/pmic/device/" #rtype "/" #chipid "/" #rtype #index "/vec"


#define PMIC_NPA_NODE_DEP_LIST( chipid, rtype, index )      \
    {                                                       \
        PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),  \
        NPA_CLIENT_VECTOR,                                  \
        NULL                                                \
    },                                                      \
    {                                                       \
        PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),  \
        NPA_CLIENT_SUPPRESSIBLE_VECTOR,                     \
        NULL                                                \
    }

#endif /* PM_NPA_DEVICE_H */

