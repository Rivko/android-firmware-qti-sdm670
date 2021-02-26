#ifndef __ICBCFG_H__
#define __ICBCFG_H__
/*============================================================================

FILE:      icbcfg.h

DESCRIPTION: Function and data structure declarations for ICB CFG driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdrivers/icbcfg.h#1 $
$DateTime: 2017/09/18 09:26:13 $
$Author: pwbldsvc $
$Change: 14402555 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2016/10/11  sds    Restructure config/query interfaces
2015/07/20  sds    Add ICB_Config_PostInit().
2015/06/23  sds    Add support for multiple remaps.
2014/11/05  sds    Add ICB_Unmap_L2_TCM() API.
2014/06/03  sds    Update max number of segments
2013/10/10  sds    BIMC remap support now checks for hardware support explicitly
2013/07/23  sds    Added BIMC remap support
2012/03/27  av     Added HWIO macro support 
2012/02/07  av     Created

         Copyright (c) 2012-2016 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                       QUALCOMM Proprietary/GTDR
============================================================================*/
#include "icbcfg_common.h"
#include "ddr_common.h"
#include <stdbool.h>
#include "icbcfg_query.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

/*============================================================================
                          TYPE DEFINITION
============================================================================*/
/**
 * ICBCFG remap types
*/
typedef enum
{
  ICBCFG_REMAP_INTERLEAVE_DEFAULT = 0,  /**< Remap all memory in the region. */
  ICBCFG_REMAP_INTERLEAVE_ODD,          /**< Remap ODD 1k pages in the region. */
  ICBCFG_REMAP_INTERLEAVE_EVEN,         /**< Remap EVEN 1k pages in the region. */
  ICBCFG_REMAP_INTERLEAVE_COUNT
} icbcfg_interleave_type;

typedef struct
{
  uint64                 src_addr;     /**< The source address of the region to remap (address at the memory controller) */
  uint64                 size;         /**< The size of the region to remap */
  uint64                 dest_addr;    /**< The destination address of the region to remap (desired address in system memory map) */
  icbcfg_interleave_type interleaved;  /**< Type of interleaving on the memory region */
  bool                   deinterleave; /**< Deinterleave as well as remap? */
} icbcfg_remap_info_type;

/*============================================================================
                                 FUNCTIONS
============================================================================*/

//*============================================================================*/
/**
@brief 
      Initializes ICB config driver. It reads ICB configuration data (register 
      address and config data) from DAL Device Property file and configures 
      ICB registers.
 
@param[in]  dev  DAL string device ID.

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Config_Init( const char * dev );

//*============================================================================*/
/**
@brief 
      ICB configuration data for after everything else is done (segment/remap).
      It reads ICB configuration data (register address and config data) from
      DAL Device Property file and configures ICB registers.
 
@param[in]  dev  DAL string device ID.

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      Should only be called after any and all calls to ICB_Remap/RemapEx() are complete.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Config_PostInit( const char * dev );

//*============================================================================*/
/**
@brief 
      Configures DDR slave segments in BIMC. It reads ICB configuration data (number
      of channels, maximum memory map, etc) from DAL Device Property file and configures 
      ICB registers.
 
@param[in]  dev   DAL string device ID.
@param[in]  info  DDR channel region configuration

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Segments_Init( const char * dev, ddr_info * info );

//*============================================================================*/
/**
@brief 
      Configures memory address remapper. 

@param[in] dev   DAL string device ID.
@param[in] info  The information about the DDR configuration
@param[in] index The logical remap slot to use
@param[in] remap The information about the region to remap
                 Allowed to be NULL to clear an index

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_RemapEx( const char * dev, ddr_info * info, uint32 index, icbcfg_remap_info_type * remap );

//*============================================================================*/
/**
@brief 
      Disable address mapping for LLC as TCM.

@param[in] dev   DAL string device ID.

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_UnmapTCM( const char * dev );

/*============================================================================
                            DEPRECATED FUNCTIONS
============================================================================*/
//*============================================================================*/
/**
@brief 
      Configures memory address remapper. 
      Uses remap index 0.

@param[in] dev  DAL string device ID.
@param[in] info The information about the DDR configuration
@param[in] remap The information about the region to remap
                 Allowed to be NULL to clear the remap

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Remap( const char * dev, ddr_info * info, icbcfg_remap_info_type * remap );

#endif /* ICBCFG_H */
