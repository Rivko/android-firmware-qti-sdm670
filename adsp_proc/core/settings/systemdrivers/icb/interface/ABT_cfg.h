#ifndef __ABT_CFG_H
#define __ABT_CFG_H
/*============================================================================

FILE:      ABT_cfg.h

DESCRIPTION: Definitions for DEVCFG platform data for Bus Timeout Driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/systemdrivers/icb/interface/ABT_cfg.h#2 $
$DateTime: 2018/04/06 00:39:57 $
$Author: pwbldsvc $
$Change: 15863303 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2018/04/03  ak     Add deferred ABT programming support
2016/05/20  bc     Created

                Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
============================================================================*/
#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"
#include "ClockDefs.h"


/*============================================================================
                          MACRO DEFINITION
============================================================================*/
#ifndef TRUE
  #define TRUE true
#endif
#ifndef FALSE
  #define FALSE false
#endif


/*============================================================================
                          TYPE DEFINITION
============================================================================*/
/**
 * Deferral enum
 */
typedef enum
{
  ABT_DEFER_NONE = 0,         /**< Do not defer. */
  ABT_DEFER_ONCE,             /**< Write once on first defer request, but not again. */
  ABT_DEFER_ALWAYS,           /**< Write deferral always. */
  ABT_DEFER_COUNT,
  ABT_DEFER_SIZE = 0x7FFFFFFF
} ABT_deferral_type;

/**
 * ABT configuration Data type
 */
typedef struct
{
    char*    name;                     /**< ABT Slave name */
    uint8_t  slave_id;                 /**< ABT Slave internal id */
    void*    base_addr;                /**< ABT Slave base address */
    char**   clk_name_array;           /**< ABT Slave clk name array */
    ClockIdType* clk_id_array;          /**< ABT Slave clk Id array */
    uint8_t* clk_disable_array;        /**< ABT Slave clk disable array */
    uint32_t num_of_clk;               /**< Number of clocks to be voted */    
    uint8_t  slave_enable;             /**< Slave enable */
    uint8_t  intr_enable;              /**< Slave Interrupt enable */
    uint8_t  timeout_val;              /**< Slave timeout value */    
    uint8_t  is_noc;                   /**< Flag indicating that this is a NOC, not ABT */
    uint8_t  wakeup_id;                /**< ABT Slave wakeup bit position */
    ABT_deferral_type  deferral_pref;     /**< Deferral handle preference */
    const char*            power_domain;    /**< Associated power domain name (if any) */
    ClockPowerDomainIdType power_domain_id; /**< Associated power domain id (if any) */
    uint32_t  timebase;                 /**timebase register */
}ABT_slave_info_type;

/**
 * ABT Platform HWIO type
 */
typedef struct
{
    char*    name;                /**< Name of region */
    uint8_t* base_addr;           /**< Base  address */    
}ABT_platform_hwio_info_type;

/**
 * ABT Platform Data type
 */
typedef struct
{
    char*    name;                    /**< Platform name */
    void**   intr_map_addr;           /**< ABT Interrupt map reg address */
    void**   intr_status_addr;        /**< ABT Interrupt pending status 
                                         register address*/
    void*    globa_en_addr;           /**< ABT Slave Global EN reg address */
    uint32_t intr_vector;             /**< Summary Interrupt vector */
    uint32_t intr_priority;           /**< Interrupt priority */
    uint32_t num_status_reg;          /**< Number of ABT Interrupt enable/status registers */
    ABT_platform_hwio_info_type* hwio_map_info;          /**< HWIO map info */    
    uint32_t num_hwio_regions;        /**< Number of HWIO region mappings */    
    void**   wakeup_addr;             /**< ABT Interrupt pending status 
                                         register address*/
    uint32_t num_wakeup_reg;          /**< Number of ABT Interrupt enable/status registers */
}ABT_platform_info_type;


/**
 * ABT Device Property Data type
 */
typedef struct
{
    uint8_t len;                        /**< Length of property data array */
    ABT_slave_info_type* slave_info;  /**< Pointer to cfgdata array*/
    ABT_platform_info_type *platform_info; /**< Pointer to platform info */
}ABT_propdata_type;

/**
 * ICBCFG Error types.
 */ 
typedef enum
{
  ABT_SUCCESS = 0,      				/**< Success */
  ABT_ERROR_INVALID_PARAM,              /**< Invalid Parameters */
  ABT_ERROR_INIT_FAILURE,               /**< Initialization Failure */
  /* Add new errors here */         

  ABT_ERROR_SIZE = 0x7FFFFFFF,          /* force 32-bit enum */
  ABT_ERROR = -1
}ABT_error_type;

#endif /* __ABT_CFG_H */
