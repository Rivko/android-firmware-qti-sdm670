/** 
@file  EFIPmicNpaTest.h
@brief PMIC NpaTest Protocol for UEFI.
*/
/*=============================================================================
Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Confidential and Proprietary.
    
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
08/22/16   pb    Changes to enable NPA test in 8998
04/18/14   va    Initial draft (Expose Npa Test protocol)

=============================================================================*/

#ifndef __EFIPMICNPATEST_H__
#define __EFIPMICNPATEST_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/
#include <Library/npa_resource.h>
/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicNpaTest_constants 
@{ */
/**
Protocol version.
*/

#define PMIC_NPATEST_REVISION 0x0000000000010001
/** @} */ /* end_addtogroup efi_pmicNpaTest_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicNpaTest_protocol */
#define EFI_PMIC_NPATEST_PROTOCOL_GUID \
    { 0xf4286061, 0x4326, 0x44b0, { 0x98, 0x93, 0xe7, 0x5c, 0xca, 0x36, 0x74, 0x5 } };

/** @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC NPATEST Protocol GUID.
*/
extern EFI_GUID gQcomPmicNpaTestProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_NPATEST_PROTOCOL   EFI_QCOM_PMIC_NPATEST_PROTOCOL;
/** @endcond */


typedef enum
{
  EFI_PM_NPA_VREG_LDO,
  EFI_PM_NPA_VREG_SMPS,
  EFI_PM_NPA_VREG_BOOST,
  EFI_PM_NPA_VREG_NCP,
  EFI_PM_NPA_VREG_VS,
  EFI_PM_NPA_CLK_BUFF,
  EFI_PM_NPA_PBS,
  EFI_PM_NPA_VREG_MAX,
}EFI_PM_NPA_RESOUCE_TYPE;

typedef struct
{
  void *RailData; /* Type of client to create */
  EFI_PM_NPA_RESOUCE_TYPE RailType;
}EFI_PM_NPA_PAM_CLIENT_CFG_TYPE;

typedef struct
{
    const char                              *GroupName;     /* Name of resource */
    unsigned                                 MaxMode;       /* Maximum state/length of vector resource */
    unsigned int                             ResourceAttributes;     /* Resource Attributes */
    EFI_PM_NPA_PAM_CLIENT_CFG_TYPE           *ResourceData;  /* User data field  */

    const char                              *NodeName;      /* Node name */
    unsigned int                             NodeAttributes;   /* Node Attributes */
    npa_user_data                           *NodeData;       /* User data */
    npa_node_dependency                     *NodeDependencies;     /* Dep array */
    unsigned int                             DependencyCount;
}EFI_PM_NPA_NODE_RESOURCE_INFO;


typedef struct  
{
    // [Disable, Enable] — max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    //	[NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    // [X uV] — max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    // 	[X uA] — max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;
}EFI_PM_NPA_VS_KVPS;


typedef struct  
{
    // [Disable (default), Enable] — max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    // [AUTO (default), IPEAK, NPM] - max aggregation (left to right)
    unsigned smps_sw_mode_key; // PM_NPA_KEY_SMPS_SOFTWARE_MODE
    unsigned smps_sw_mode_size; // 4
    unsigned smps_sw_mode_value;

    //	[NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    //	[NONE, EN1, EN2, EN3, EN4, SLEEPB] - ORed value of list
    unsigned pin_ctrl_power_mode_key; // PM_NPA_KEY_PIN_CTRL_POWER_MODE
    unsigned pin_ctrl_power_mode_size;
    unsigned pin_ctrl_power_mode_value;

    // [X uV] — max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    // 	[X uA] — max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;

    // [xx MHz] — max within a priority group
    // — Allows one execution environments vote to be higher or lower priority relative to another execution environment
    unsigned frequency_key; // PM_NPA_KEY_FREQUENCY
    unsigned frequency_size; // 4
    unsigned frequency_value;

    //Frequency_Reason — Optional	[xx] — Distinct indicators per execution environment
    //    One execution environment will not be allowed to use the indicators of another
    //    - Allows interspersed prioritization between execution environments for
    //    frequency aggregation
    //    — Ex: Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority
    unsigned frequency_reason_key; // PM_NPA_KEY_FREQUENCY_REASON
    unsigned frequency_reason_size; // 4
    unsigned frequency_reason_value;

    // [None, Quiet, Super Quiet] — max aggregation (left to right)  
    // Mask that allows SMPS to follow global quiet mode signal
    unsigned follow_quiet_mode_key; // PM_NPA_KEY_FOLLOW_QUIET_MODE
    unsigned follow_quiet_mode_size; // 4
    unsigned follow_quiet_mode_value;

    // [ALLOWED, DISALLOWED] - MAX value left to right
    unsigned bypass_allowed_key; // PM_NPA_KEY_BYPASS_ALLOWED_KEY
    unsigned bypass_allowed_size; // 4
    unsigned bypass_allowed_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_key; // PM_NPA_KEY_CORNER_VOLTAGE_KEY
    unsigned corner_size; // 4
    unsigned corner_value;

    // [x uV] - max aggregation
    unsigned head_room_voltage_key; // PM_NPA_KEY_HEAD_ROOM
    unsigned head_room_voltage_size; // 4
    unsigned head_room_voltage_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned floor_key; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned floor_size; // 4
    unsigned floor_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_voltage_level; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned corner_voltage_size; // 4
    unsigned corner_voltage_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_voltage_floor_level; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned corner_voltage_floor_size; // 4
    unsigned corner_voltage_floor_value;

}EFI_PM_NPA_SMPS_KVPS;


typedef struct  
{
    // [Disable (default), Enable] — max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    // [IPEAK (default), NPM] - max aggregation (left to right)
    unsigned ldo_sw_mode_key; // PM_NPA_KEY_LDO_SOFTWARE_MODE
    unsigned ldo_sw_mode_size; // 4
    unsigned ldo_sw_mode_value;

    //	[NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    //	[NONE, EN1, EN2, EN3, EN4, SLEEPB] - ORed value of list
    unsigned pin_ctrl_power_mode_key; // PM_NPA_KEY_PIN_CTRL_POWER_MODE
    unsigned pin_ctrl_power_mode_size;
    unsigned pin_ctrl_power_mode_value;

    // [X uV] — max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    // 	[X mA] — max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;

    // [x uV] - max aggregation
    unsigned head_room_voltage_key; // PM_NPA_KEY_HEAD_ROOM
    unsigned head_room_voltage_size; // 4
    unsigned head_room_voltage_value;

    // [ALLOWED, DISALLOWED] - MAX value left to right
    unsigned bypass_allowed_key; // PM_NPA_KEY_BYPASS_ALLOWED_KEY
    unsigned bypass_allowed_size; // 4
    unsigned bypass_allowed_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_key; // PM_NPA_KEY_CORNER_VOLTAGE_KEY
    unsigned corner_size; // 4
    unsigned corner_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned floor_key; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned floor_size; // 4
    unsigned floor_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_voltage_level; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned corner_voltage_size; // 4
    unsigned corner_voltage_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_voltage_floor_level; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned corner_voltage_floor_size; // 4
    unsigned corner_voltage_floor_value;
}EFI_PM_NPA_LDO_KVPS;


/*!
 * KVP (Key Value Pairs for Clock Buffers
 */
typedef struct
{
    // [Disable (default), Enable] — max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    //	[NONE, EN1] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;
} EFI_PM_NPA_CLK_BUFF_KVPS;

typedef struct  
{
    // [Disable (default), Enable] — max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

}EFI_PM_NPA_BOOST_KVPS;


//////////////////////????????????????????/////////////////////////////////

/*! 
 * Control Type 
 */
typedef enum
{
  EFI_PM_PRM_RPMH,
  EFI_PM_PRM_LOCAL,
  EFI_PM_PRM_INVALID_CTRL
}EFI_PM_PRM_CTRL_TYPE;

/*! 
 * RPMH Set Type 
 */
typedef struct
{
  uint32     address; 
  uint32     offset; 
  unsigned   active_set;    /*ActiveSetValue*/
  unsigned   sleep_set;     /*SleepSetValue*/
  boolean    dirty_active;  /*Active set dirty flag*/
  boolean    dirty_sleep;   /*Sleep set dirty flag*/
  uint32     msg_id;        /*RPMh Message Id for Vectors*/
  uint32     nas_req_cnt;
}EFI_PM_RPMH_DATA_TYPE;

/*! 
 * VRM Resource Type 
 */
typedef enum
{
  EFI_PM_PRM_RSRC_VRM_VREG,
  EFI_PM_PRM_RSRC_VRM_CLK,
  EFI_PM_PRM_RSRC_VRM_INVALID
}EFI_PM_PRM_RSRC_VRM_TYPE;

/*! 
 * PMIC Resource Type 
 */
typedef enum
{
  EFI_PM_PRM_RSRC_LDO,
  EFI_PM_PRM_RSRC_SMPS,
  EFI_PM_PRM_RSRC_VS,
  EFI_PM_PRM_RSRC_CLK,
  EFI_PM_PRM_RSRC_INVALID
}EFI_PM_PRM_RSRC_TYPE;


/*! 
 * Resource key types 
 */
typedef enum
{
  EFI_PM_RSRC_KEY_MODE,
  EFI_PM_RSRC_KEY_VOLTAGE,
  EFI_PM_RSRC_KEY_HEADROOM,
  EFI_PM_RSRC_KEY_ENABLE,
  EFI_PM_RSRC_KEY_VEC, // Check if we can add it somewhere else
  EFI_PM_RSRC_KEY_INVALID
}EFI_PM_PRM_RSRC_KEY_TYPE;

/*! 
 * PAM Layer Rail Data and type 
 */
typedef struct
{
  void                  *rail_data;
  EFI_PM_PRM_RSRC_VRM_TYPE  rail_type;
}EFI_PM_PRM_PAM_CLIENT_CFG_TYPE;


/*! 
 * PAM Layer Node Data 
 */
typedef struct EFI_PM_PRM_NODE_RESOURCE_INFO_TYPE
{
  const char                  *group_name;          /* Name of resource */
  unsigned                    max_mode;            /* Maximum state/length of vector resource */
  unsigned int                resource_attributes; /* Resource Attributes */
  EFI_PM_PRM_PAM_CLIENT_CFG_TYPE  *resource_data;       /* User data field  */

  const char                  *node_name;           /* Node name */
  unsigned int                node_attributes;     /* Node Attributes */
  npa_user_data               *node_data;           /* User data */
  npa_node_dependency         *node_dependencies;   /* Dep array */
  unsigned int                dependency_count;     /* Number of dependencies */
}EFI_PM_PRM_NODE_RESOURCE_INFO;


/*! 
 * Device Resource Data 
 */
typedef struct
{ //Resource data
  const char              *rsrc_name;     /* NPA resource name */
  const char              *rsrc_node;     /* Name of resource node */
  EFI_PM_PRM_RSRC_TYPE        rsrc_type;      /* Type of resource on pmic: LDO, SMPS, CLK, VS*/
  EFI_PM_PRM_RSRC_VRM_TYPE    rsrc_vrm_type;  /* Rsrc Type on VRM Side Vreg or Buff */
  const char              *cmd_db_name;   /* Name to refer command db*/
  const char              *rsrc_units;    /* Resource Units */ 
  const char              *pmic_chip;     /* PMIC Chip for Resource */
  EFI_PM_PRM_CTRL_TYPE        ctrl_type;      /* Type of control, local or rpmh */
  UINT8                   periph_id;      /* Driver layer peripheral ID for local Control */
  EFI_PM_PRM_RSRC_KEY_TYPE    rsrc_key;       /* Wheather resource is enable, headroom, etc */
  //Pam Request                           
  boolean                 pam_req;        /*Flag for notifying PAM Request*/
  //RPMh Data                             
  EFI_PM_RPMH_DATA_TYPE       rpmh_data;      /*Data for RPMh Set information*/
}EFI_PM_PRM_RSRC_DATA_TYPE;



/*PAM RESOURCEINFO */
typedef struct{
  UINT32 NumOfTestNodes;
  char*  RsrcTestName;
  EFI_PM_PRM_NODE_RESOURCE_INFO *NodeRsrcArr;
  //void   *node_rsrc_arr;
}EFI_PMIC_PRM_TEST_RESOURCEINFO;


/*! 
 * VREG Resource Data 
 */
typedef struct
{
  npa_resource_state mode;
  npa_resource_state voltage;
  npa_resource_state headroom;
  npa_resource_state enable;
}EFI_PM_PRM_VREG_DATA_TYPE; 


typedef struct
{
  npa_resource_state enable;
}EFI_PM_PRM_CLK_DATA_TYPE; 

///////////////////////????????????????????///////////////////////////////


/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_NPATEST_INT */ 
/** @ingroup
  @par Summary
  Initializes NPATEST module

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_NPATEST_INT)(void);

/* EFI_PM_NPATEST_INT */ 
/** @ingroup
  @par Summary
  Initializes NPATEST module

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_NPATEST_CREATETESTCLIENTS)(void);



/* EFI_PM_NPATEST_GETRESOURCEINFO */ 
/** @ingroup
  @par Summary
  Returns PAM Info

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PMIC_NPATEST_GETRESOURCEINFO)(
                    OUT EFI_PMIC_PRM_TEST_RESOURCEINFO *PrmResourceInfo);



/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicNpaTest_protocol
@par Summary
Qualcomm PMIC NPATEST Protocol interface.

@par Parameters
@inputprotoparams{pmic_smbchg_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_NPATEST_PROTOCOL {
  UINT64                              Revision;
  EFI_PM_NPATEST_INT                  PmicNpaTestInit;
  EFI_PMIC_NPATEST_CREATETESTCLIENTS  CreateTestClients;
  EFI_PMIC_NPATEST_GETRESOURCEINFO    GetResourceInfo;
};


#endif  /* __EFIPMICNPATEST_H__ */
