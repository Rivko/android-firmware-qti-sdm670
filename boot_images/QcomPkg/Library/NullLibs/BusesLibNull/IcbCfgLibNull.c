/** @file IcbCfgLibNull.c
  
  Stub functions for IcbCfgLib

  Copyright (c) 2014-2015, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/14/15   plc     Added icb_sdi_save api
 09/24/14   ck      Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "icbcfg.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

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
icbcfg_error_type ICB_Config_Init( const char * dev )
{
  return ICBCFG_SUCCESS;
}

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
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Config_PostInit( const char * dev )
{
  return ICBCFG_SUCCESS;
}

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
      Should only be called after any and all calls to ICB_Remap/RemapEx() are complete.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Segments_Init( const char * dev, ddr_info * info )
{
  return ICBCFG_SUCCESS;
}

//*============================================================================*/
/**
@brief 
      Configures the remap segments in BIMC. 

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
icbcfg_error_type ICB_RemapEx( const char * dev, ddr_info * info, uint32 index, icbcfg_remap_info_type * remap )
{
  return ICBCFG_SUCCESS;
}

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
icbcfg_error_type ICB_Get_Memmap( const char * dev, icb_mem_map_type * info )
{
  return ICBCFG_SUCCESS;
}

//*============================================================================*/
/**
@brief 
      Unmaps L2 TCM from BIMC, if present.

@param[in]    dev  DAL string device ID.

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type ICB_Unmap_L2_TCM( const char * dev )
{
  return ICBCFG_SUCCESS;
}

//*============================================================================*/
/**
@brief 
      Configures the remap segments in BIMC. 
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
icbcfg_error_type ICB_Remap( const char * dev, ddr_info * info, icbcfg_remap_info_type * remap )
{
  return ICBCFG_SUCCESS;
}

//*============================================================================*/
/**
@brief 
      Save BRIC SEGMENT and REMAP registers for SDI.
*/ 
/*============================================================================*/
void icb_sdi_save( void )
{
  return;
}

