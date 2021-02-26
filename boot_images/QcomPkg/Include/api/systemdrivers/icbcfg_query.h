#ifndef __ICBCFG_QUERY_H__
#define __ICBCFG_QUERY_H__
/*============================================================================

FILE:      icbcfg_query.h

DESCRIPTION: Function and data structure declarations for ICB CFG driver Interface

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdrivers/icbcfg_query.h#2 $
$DateTime: 2018/04/25 09:25:45 $
$Author: pwbldsvc $
$Change: 16016613 $ 

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2016/10/11  sds    Restructured to split query from common interface
2016/09/15  sds    Branched for sdm845

         Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                       QUALCOMM Proprietary/GTDR
============================================================================*/
#include "icbcfg_common.h"

/*============================================================================
                          DEFINEs/MACROs
============================================================================*/

/*============================================================================
                          TYPE DEFINITION
============================================================================*/

/*============================================================================
                                 FUNCTIONS
============================================================================*/
//*============================================================================*/
/**
@brief 
      Returns the current memory map.

@param[in]    dev  DAL string device ID.
@param[inout] info The information about the DDR configuration

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Get_Memmap( const char * dev, icb_mem_map_type * info );

//*============================================================================*/
/**
@brief 
      Returns the allowed memory map.
      Composed of the minimum allowed DDR start address and the
      maximum DDR end address.

@param[in]    dev  DAL string device ID.
@param[inout] info The information about the DDR region.

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Get_AllowedMemRegion( const char * dev, icbcfg_mem_region_type * info );

//*============================================================================*/
/**
@brief 
      Returns the allowed memory map.
      Composed of the minimum allowed DDR start address and the
      maximum DDR end address.

@param[in]    dev         DAL string device ID.
@param[inout] info        The pointer to be 
@param[inout] num_regions A pointer to the number of regions in the info array

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Get_AllowedMemRegionEx( const char * dev, icbcfg_mem_region_type ** info, uint32 * num_regions );

//*============================================================================*/
/**
@brief 
      Translate SoC addr space to memory controller addr space

@param[in]    dev      DAL string device ID.
@param[in]    soc_addr The address in SoC space
@param[inout] mc_addr  The address in memory controller space

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Get_MCAddr( const char * dev, uint64 soc_addr, uint64 * mc_addr );


#endif /* ICBCFG_QUERY_H */
