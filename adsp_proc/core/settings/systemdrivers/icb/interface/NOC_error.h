#ifndef __NOC_ERROR_H__
#define __NOC_ERROR_H__
/*============================================================================

FILE:      NOC_Error.h

DESCRIPTION: APIs and data structure declarations 
             for NOC Error Handler

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/interface/NOC_error.h#2 $
$DateTime: 2017/12/06 09:58:14 $
$Author: pwbldsvc $
$Change: 14996571 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2017/06/01  sds    Add deferred NoC error programming support
2017/03/01  sds    Add timeout enable register support
2017/02/06  sds    Rewrite for QNoC 4.x
2015/11/23  pm     Added Sideband Manager support
2014/12/09  tb     Split OEM modifiable data from internal data
2013/05/10  pm     Added NOC_ERRLOG6 macro to support FlexNoc 2.8
2012/10/03  av     Created

                Copyright (c) 2014 - 2017 Qualcomm Technologies Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "com_dtypes.h"
#include "stdint.h"
#include "stdbool.h"
#include "ClockDefs.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/
/* Reserved value of offset that indicates that the register does not apply to
 * this version of hardware
 */
#define REGISTER_NOT_APPLICABLE 0xFFFF 
#define INTERRUPT_UNUSED        0xFFFFFFFF

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
   /* New fields for new NoC SidebandManager versions go here */
} NOC_sideband_hw_type;

/**
 * NOC Error Syndrome Register Data Types
 */
typedef struct
{
  uint32_t FAULTINSTATUS0_LOW;
  uint32_t FAULTINSTATUS0_HIGH;
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
 * Deferral enum
 */
typedef enum
{
  NOCERR_DEFER_NONE = 0,         /**< Do not defer. */
  NOCERR_DEFER_ONCE,             /**< Write once on first defer request, but not again. */
  NOCERR_DEFER_ALWAYS,           /**< Write deferral always. */
  NOCERR_DEFER_COUNT,
  NOCERR_DEFER_SIZE = 0x7FFFFFFF
} NOCERR_deferral_type;

/**
 * Internal NOC error info data type
 */
typedef struct
{
  const char*            name;              /**< NOC name */
  NOC_hw_type*           hw;                /**< NOC register offsets */
  void*                  base_addr;         /**< NOC base address */
  uint32_t               intr_vector;       /**< NOC interrupt vector */
  uint32_t               intr_priority;     /**< Local interrupt priority */
  uint32_t               num_sbms;          /**< Number of NoC sideband managers */
  NOC_sideband_hw_type **sb_hw;             /**< NOC sideband register offsets */
  void**                 sb_base_addrs;     /**< NOC sideband base addresses */
  uint32_t               num_tos;           /**< Number of timeout enable registers */
  void**                 to_addrs;          /**< NOC timeout enable registers */
  NOCERR_syndrome_type   syndrome;          /**< Error syndrome */
  NOCERR_deferral_type   deferral_pref;     /**< Deferral handle preference */
  const char*            power_domain;      /**< Associated power domain name (if any) */
  ClockPowerDomainIdType power_domain_id;   /**< Associated power domain id (if any) */
  char**                 clk_name_array;    /**< Associated clk name array */
  bool*                  clk_disable_array; /**< Associated clk disable array */
  uint32_t               num_of_clk;        /**< Number of clocks to be voted */
} NOCERR_info_type;

/**
 * OEM NOC error Info data type
 */
typedef struct
{
  uint32_t faultin_en0_low;
  uint32_t faultin_en0_high;
} NOCERR_sbm_info_type_oem;

typedef struct
{
  char*                     name;         /**< NOC name */
  bool                      intr_clear;   /**< NOC errorlogger interrupt error clear on init */
  bool                      intr_enable;  /**< NOC errorlogger interrupt enable */
  bool                      error_fatal;  /**< NOC interrupt error fatal */ 
  NOCERR_sbm_info_type_oem *sbms;         /**< NOC sideband manager cfgs */
  uint32_t *                to_reg_vals;  /**< NoC timeout enable register values */
} NOCERR_info_type_oem;


typedef struct
{
  uint32_t          len;             /**< Length of cfgdata array */
  NOCERR_info_type* NOCInfo;         /**< Pointer to internal cfgdata array*/
  uint32_t          num_clock_regs;  /**< Number of external timeout clock regs */
  void**            clock_reg_addrs; /**< External timeout clock regs */
} NOCERR_propdata_type;

/**
 * NOC Error OEM Property Data type
 */
typedef struct
{
  uint32_t              len;            /**< Length of cfgdata array */
  NOCERR_info_type_oem* NOCInfoOEM;     /**< Pointer to OEM cfgdata array*/
  uint32_t *            clock_reg_vals; /**< NOC timeout external register values */
} NOCERR_propdata_type_oem;

/*============================================================================
                        NOC HW Register DEFINITIONS 
============================================================================*/
#ifdef USES_QNOC_4_0
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
#endif

/* NOC SidebandManager HW Register Offsets */
#ifdef USES_QNOC_SB_4_0_L
static NOC_sideband_hw_type QNOC_SB_4_0_L = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = REGISTER_NOT_APPLICABLE,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = REGISTER_NOT_APPLICABLE,
};
#endif

#ifdef USES_QNOC_SB_4_0
static NOC_sideband_hw_type QNOC_SB_4_0 = {
   .swid_low             = 0x0,
   .swid_high            = 0x4,
   .faultin_en0_low      = 0x40,
   .faultin_en0_high     = 0x44,
   .faultin_status0_low  = 0x48,
   .faultin_status0_high = 0x4C,
};
#endif

/*============================================================================
                                 FUNCTIONS
============================================================================*/

#endif /* __NOC_ERROR_H__ */
