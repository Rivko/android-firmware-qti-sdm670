#ifndef __NOC_ERROR_H__
#define __NOC_ERROR_H__
/*============================================================================

FILE:      NOC_Error.h

DESCRIPTION: APIs and data structure declarations 
             for NOC Error Handler

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.tz/2.0.2/settings/systemdrivers/icb/interface/NOC_error.h#1 $
$DateTime: 2018/02/06 02:57:17 $
$Author: pwbldsvc $
$Change: 15399902 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2018/01/11  sds    Expand SBM definition
2017/10/02  sds    Add support for chip versioned configs
2017/03/01  sds    Add timeout enable register support
2017/02/06  sds    Rewrite for QNoC 4.x
2015/11/23  pm     Added Sideband Manager support
2014/12/09  tb     Split OEM modifiable data from internal data
2013/05/10  pm     Added NOC_ERRLOG6 macro to support FlexNoc 2.8
2012/10/03  av     Created

                Copyright (c) 2014 - 2018 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "com_dtypes.h"
#include "stdint.h"
#include "stdbool.h"
#include "ChipInfo.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/
/* Reserved value of offset that indicates that the register does not apply to
 * this version of hardware
 */
#define REGISTER_NOT_APPLICABLE 0xFFFF 

/*============================================================================
                          TYPE DEFINITION
============================================================================*/
/**
 * NOC Register Interface
 */
typedef struct
{
   uint16_t swid_low;      
   uint16_t swid_high;   
   uint16_t main_ctl_low;        
   uint16_t err_valid_low;
   uint16_t err_clear_low;    
   uint16_t errlog0_low;
   uint16_t errlog0_high;   
   uint16_t errlog1_low;
   uint16_t errlog1_high;   
   uint16_t errlog2_low;
   uint16_t errlog2_high;   
   uint16_t errlog3_low;       
   uint16_t errlog3_high;       
   /* New fields for new NoC versions go here */
} NOC_hw_type;

/**
 * NOC SidebandManager Register Interface
 */
typedef struct
{
   uint16_t swid_low;
   uint16_t swid_high;
   uint16_t faultin_en0_low;
   uint16_t faultin_en0_high;
   uint16_t faultin_status0_low;
   uint16_t faultin_status0_high;
   uint16_t faultin_en1_low;
   uint16_t faultin_en1_high;
   uint16_t faultin_status1_low;
   uint16_t faultin_status1_high;
   /* New fields for new NoC SidebandManager versions go here */
} NOC_sideband_hw_type;

/**
 * NOC Error Syndrome Register Data Types
 */
typedef struct
{
  uint32_t FAULTINSTATUS0_LOW;
  uint32_t FAULTINSTATUS0_HIGH;
  uint32_t FAULTINSTATUS1_LOW;
  uint32_t FAULTINSTATUS1_HIGH;
} NOCERR_sbm_syndrome_type;

typedef struct
{
  uint32_t ERRLOG0_LOW;
  uint32_t ERRLOG0_HIGH;
  uint32_t ERRLOG1_LOW;
  uint32_t ERRLOG1_HIGH;
  uint32_t ERRLOG2_LOW;
  uint32_t ERRLOG2_HIGH;
  uint32_t ERRLOG3_LOW;
  uint32_t ERRLOG3_HIGH;
  NOCERR_sbm_syndrome_type *sbms;
} NOCERR_syndrome_type;

/**
 * Internal NOC error Info data type
 */
typedef struct
{
  char*                  name;            /**< NOC name */
  NOC_hw_type*           hw;              /**< NOC register offsets */
  void*                  base_addr;       /**< NOC base address */
  uintptr_t              intr_vector;     /**< NOC interrupt vector */
  uint32_t               num_sbms;        /**< Number of NoC sideband managers */
  NOC_sideband_hw_type **sb_hw;           /**< NOC sideband register offsets */
  void**                 sb_base_addrs;   /**< NOC sideband base addresses */
  uint32_t               num_tos;         /**< Number of timeout enable registers */
  void**                 to_addrs;        /**< NOC timeout enable registers */
  NOCERR_syndrome_type   syndrome;        /**< Error syndrome */
} NOCERR_info_type;

/**
 * OEM NOC error Info data type
 */
typedef struct
{
  uint32_t faultin_en0_low;
  uint32_t faultin_en0_high;
  uint32_t faultin_en1_low;
  uint32_t faultin_en1_high;
} NOCERR_sbm_info_type_oem;

typedef struct
{
  char*                     name;         /**< NOC name */
  bool                      intr_enable;  /**< NOC errorlogger interrupt enable */
  bool                      error_fatal;  /**< NOC interrupt error fatal */ 
  NOCERR_sbm_info_type_oem *sbms;         /**< NOC sideband manager cfgs */
  uint32_t *                to_reg_vals;  /**< NoC timeout enable register values */
} NOCERR_info_type_oem;


typedef struct
{
  /* Chip version information for this device data. */
  ChipInfoFamilyType   family;
  bool                 match;
  ChipInfoVersionType  version;

  /* Associated target data */
  uint32_t          len;             /**< Length of cfgdata array */
  NOCERR_info_type* NOCInfo;         /**< Pointer to internal cfgdata array*/
  uint32_t          num_clock_regs;  /**< Number of external timeout clock regs */
  void**            clock_reg_addrs; /**< External timeout clock regs */
} NOCERR_propdata_type;

typedef struct
{
  uint32_t              num_configs;
  NOCERR_propdata_type *configs;
} NOCERR_config_info_type;

/**
 * NOC Error OEM Property Data type
 */
typedef struct
{
  /* Chip version information for this device data. */
  ChipInfoFamilyType   family;
  bool                 match;
  ChipInfoVersionType  version;

  /* Associated target data */
  uint32_t              len;            /**< Length of cfgdata array */
  NOCERR_info_type_oem* NOCInfoOEM;     /**< Pointer to OEM cfgdata array*/
  uint32_t *            clock_reg_vals; /**< NOC timeout external register values */
} NOCERR_propdata_type_oem;

typedef struct
{
  uint32_t                  num_configs;
  NOCERR_propdata_type_oem *configs;
} NOCERR_config_info_type_oem;

/*============================================================================
                        NOC HW Register DEFINITIONS 
============================================================================*/
/* NOC HW Register Offsets Version 4.0 */
static NOC_hw_type QNOC_4_0 = {
  .swid_low      = 0x0,
  .swid_high     = 0x4,
  .main_ctl_low  = 0x8,
  .err_valid_low = 0x10,
  .err_clear_low = 0x18,
  .errlog0_low   = 0x20,
  .errlog0_high  = 0x24,
  .errlog1_low   = 0x28,
  .errlog1_high  = 0x2C,
  .errlog2_low   = 0x30,
  .errlog2_high  = 0x34,
  .errlog3_low   = 0x38,
  .errlog3_high  = 0x3C,
};

/* NOC SidebandManager HW Register Offsets */
static NOC_sideband_hw_type QNOC_SB_4_0_L = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = REGISTER_NOT_APPLICABLE,
   .faultin_en1_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
};

static NOC_sideband_hw_type QNOC_SB_4_0 = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = 0x44,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = 0x4C,
   .faultin_en1_low      = REGISTER_NOT_APPLICABLE,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = REGISTER_NOT_APPLICABLE,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
};

static NOC_sideband_hw_type QNOC_SB_4_0_2L = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = 0x44,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = 0x4C,
   .faultin_en1_low      = 0x50,
   .faultin_en1_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status1_low  = 0x58,
   .faultin_status1_high = REGISTER_NOT_APPLICABLE,
};

static NOC_sideband_hw_type QNOC_SB_4_0_2 = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = 0x44,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = 0x4C,
   .faultin_en1_low      = 0x50,
   .faultin_en1_high     = 0x54,
   .faultin_status1_low  = 0x58,
   .faultin_status1_high = 0x5C,
};

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Performs initialization for NOC error handler.
      It enables interrupts required to handle NOC errors.
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void NOC_Error_Init(void);

#endif /* __NOC_ERROR_H__ */
