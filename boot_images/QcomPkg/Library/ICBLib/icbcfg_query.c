/*==============================================================================

FILE:      icbcfg_query.c

DESCRIPTION: This file implements the ICB driver for querying DDR memory map.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBLib/icbcfg_query.c#2 $
$DateTime: 2018/04/25 09:25:45 $
$Author: pwbldsvc $
$Change: 16016613 $

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/05/08  sds    Add support for alternate address translator
2016/09/15  sds    Branched for sdm845
2016/01/21  sds    Add Get_AllowedMemRegion().
2013/10/29  tk     This file created for DDR memory map query functions
2013/10/21  sds    Add additional method to attempt to map BRIC segments
2013/10/10  sds    BIMC remap support now checks for hardware support explicitly
2013/09/24  sds    Refactor code for better reuse and code size.
2013/09/23  sds    Fixes for BIMC remapper support
2013/08/30  sds    Added support for BIMC remap operations
2013/05/17  sds    Fixed the handling of non-power of two regions
2013/02/26  sds    Changed how available DDR regions are handled.
2012/08/07  sy     Fixed Linux compilation Error
2012/02/07  av     Created

        Copyright (c) 2012-2013, 2016, 2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfg.h"
#include "icbcfgi.h"

/*============================================================================
                          DEFINES/MACROS
============================================================================*/
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#define MAX_SEGS 6

/*============================================================================
                          Type definitions
============================================================================*/
typedef struct
{
  uint64 base;
  uint64 offset;
  bool   error;
} addr_trans_seg_t;

/*============================================================================
                          Global data 
============================================================================*/
bool                              query_dev_config_valid = false;
static icbcfg_device_config_type *icb_dev_config = NULL;
static addr_trans_seg_t           addr_trans_segs[MAX_SEGS+1];

/*============================================================================
                          Helper functions
============================================================================*/
static void get_addr_translation
( 
  icbcfg_device_config_type *dev_config
)
{
   uint32 i, error;
   uint64 max_addr = 1ULL << dev_config->addr_width; /**< Max address, not inclusive */
   uint64 sign_bit = 1ULL << (dev_config->addr_width - 1);

   /* Cache the LLCC translator configuration */
   memset(addr_trans_segs, 0, sizeof(addr_trans_segs));

   /* Read in the configuration, based on the hardware type. */
   switch(dev_config->trans_type)
   {
      case ICBCFG_ADDR_TRANS_NOC:
         error = HWIO_INX(dev_config->trans_bases[0], MEMNOC_ADDR_TRANSLATOR_ERROR_LOW);
         for( i = 0; i < dev_config->num_segments; i++ )
         {
            /* Base register does not exist for first segment. Forced to 0x0. */
            if( i == 0 )
            {
               addr_trans_segs[0].base = 0ULL;
            }
            else
            {
               addr_trans_segs[i].base = ((uint64)HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_BASEn_LOW,i)) |
                                 (((uint64)HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH,i)) << 32);
            }
            addr_trans_segs[i].offset = ((uint64)HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW,i)) |
                                 (((uint64)HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH,i)) << 32);
            addr_trans_segs[i].error = (error & (1 << i)) != 0;

            /* sign extend the offset to make our math easier later. */
            addr_trans_segs[i].offset = (addr_trans_segs[i].offset ^ sign_bit) - sign_bit;
         }
         break;

      case ICBCFG_ADDR_TRANS_LLCC:
         error = HWIO_INXF(dev_config->trans_bases[0], LLCC_BEAC_ADDR_TRANSLATOR_CFG, ERROR);
         for( i = 0; i < dev_config->num_segments; i++ )
         {
            /* Base register does not exist for first segment. Forced to 0x0. */
            if( i == 0 )
            {
               addr_trans_segs[0].base = 0ULL;
            }
            else
            {
               addr_trans_segs[i].base = ((uint64)HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG2,i)) |
                                 (((uint64)HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG3,i)) << 32);
            }
            addr_trans_segs[i].offset = ((uint64)HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG0,i)) |
                                 (((uint64)HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG1,i)) << 32);
            addr_trans_segs[i].error = (error & (1 << i)) != 0;

            /* sign extend the offset to make our math easier later. */
            addr_trans_segs[i].offset = (addr_trans_segs[i].offset ^ sign_bit) - sign_bit;
         }
         break;

      default:
         /* Didn't match a hardware type? Bail. */
         return;
   }

   /* Add "cap" segments to hold the top of the address space. */
   for( ; i < ARRAY_SIZE(addr_trans_segs); i++ )
   {
      addr_trans_segs[i].base = max_addr;
      addr_trans_segs[i].error = true;
   }
}

//*============================================================================*/
/**
@brief
    Fetch the items we have in device config

@param[in]  dev         DAL string device ID.
@param[in]  dev_config  The structure to store the device configuration in

@return
    Whether or not the operation was successful.
*/
/*============================================================================*/
static bool get_device_configuration
(
   const char *                dev,
   icbcfg_device_config_type **dev_config
)
{
   if( !query_dev_config_valid )
   {
      do
      {
         DALSYS_PROPERTY_HANDLE_DECLARE(hICBProp);
         DALSYSPropertyVar          ICBPropVar;
         icbcfg_info_type *         icbcfg_info;
         DalChipInfoFamilyType      family;
         DalChipInfoVersionType     version;
         uint32                     i;

         if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev, hICBProp))
         {
            break;
         }

         if ((DAL_SUCCESS != DALSYS_GetPropertyValue(hICBProp,
                                                   "icbcfg_info", 0, &ICBPropVar)))
         {
            break;
         }
         
         icbcfg_info = (icbcfg_info_type *)ICBPropVar.Val.pStruct;
         if( NULL == icbcfg_info )
         {
            break;
         }

         family  = DalChipInfo_ChipFamily();
         version = DalChipInfo_ChipVersion();
         
         for( i = 0; i < icbcfg_info->num_configs; i++ )
         {
            icbcfg_device_config_type *device_config = icbcfg_info->configs[i];
            if( family != device_config->family )
            {
               continue;
            }

            /* We've found a matching version in two different conditions:
             * We have an exact match
             * We don't need an exact match and the version is great than or equal to 
             * what we're looking for. */
            if( version == device_config->version ||
                (false == device_config->match && version >= device_config->version) )
            {
               icb_dev_config = device_config;
               break;
            }
         }

         /* Check to see if we found a matching configuration. */
         if( i == icbcfg_info->num_configs )
         {
            break;
         }

         /* Bookkeeping */
         get_addr_translation( icb_dev_config );
         query_dev_config_valid = true;
      } while( false );
   }

   *dev_config = icb_dev_config;
   return query_dev_config_valid;
}

//*============================================================================*/
/**
@brief
    Compute the system memory map from the cache segment configuration

@param[in]  dev_config     device config information to use
@param[in]  sys_map        the structure to store the memory map in

*/
//*============================================================================*/
void get_system_memory_map
(
  icbcfg_device_config_type *dev_config,
  icb_mem_map_type *         sys_map
)
{
   uint32 i, region;
   region_type *regions = sys_map->channels[0].regions;
   uint64 last_base = 0;

   /* Clear the map structure passed to us. */
   memset(sys_map, 0, sizeof(icb_mem_map_type));

   for( i = 0, region = 0; i < ARRAY_SIZE(addr_trans_segs) ; i++ )
   {
      /* If the segment we're looking at is a valid decode
       * we need to either create a new region or add it to the existing region. */
      if( !addr_trans_segs[i].error )
      {
         /* Create a new region if the current region is at address 0. */
         if( 0 == regions[region].base_addr )
         {
            regions[region].base_addr = addr_trans_segs[i].base;
            regions[region].interleaved = true;
            last_base = addr_trans_segs[i].base;
         }
         /* Add to existing region. */
         else
         {
            regions[region].size += addr_trans_segs[i].base - last_base;
            last_base = addr_trans_segs[i].base;
         }
      }
      /* Not a valid decode.
       * "End" a region if we're pointing at a valid one, otherwise
       * we have nothing to do. */
      else
      {
         if( 0 != regions[region].base_addr )
         {
            regions[region].size += addr_trans_segs[i].base - last_base;
            region++;
         }
      }
   }

   /* Copy to remaining channels. We assume interleaved. */
   for( i = 1; i < dev_config->num_channels; i++ )
   {
      sys_map->channels[i] = sys_map->channels[0];
   }
}

//*============================================================================*/
/**
@brief
    Compute the system memory map from the cache segment configuration

@param[in]    dev_config   device config information to use
@param[in]    soc_addr     The SoC address to translate
@param[inout] mc_addr      Pointer to store the memory address

*/
//*============================================================================*/
icbcfg_error_type translate_address
(
 icbcfg_device_config_type *dev_config,
 uint64                     soc_addr,
 uint64 *                   mc_addr 
)
{
   icbcfg_error_type ret = ICBCFG_ERROR_INVALID_ADDRESS;
   uint32 i;

   /* Search the noc segments list for an address range match. */
   for( i = 0; i < dev_config->num_segments; i++ )
   {
      /* Check if our address lies between the two segments we're looking at. */
      if( addr_trans_segs[i].base   <= soc_addr &&
          addr_trans_segs[i+1].base >  soc_addr )
      {
         /* We have a match.
          * If it's not to an error segment, translate the address. */
         if( !addr_trans_segs[i].error )
         {
            /* This is only safe to do because we sign extended
             * the offset earlier. */
            *mc_addr = soc_addr + addr_trans_segs[i].offset;
            ret = ICBCFG_SUCCESS;
            break;
         }
         /* Mapped to error segment, fail. */
         else
         {
            break;
         }
      }
   }

   return ret;
}

//*============================================================================*/
/**
@brief
    Checks the overlap between two regions

@param[in]    region1
@param[in]    region2
@param[inout] 

*/
//*============================================================================*/

bool check_overlap(region_type *region1, region_type *region2)
{
 
  if(((region1->base_addr < region2->base_addr ) && (region1->base_addr + region1->size <= region2->base_addr)) || 
    ((region2->base_addr < region1->base_addr ) && (region2->base_addr + region2->size <= region1->base_addr )))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

//*============================================================================*/
/**
@brief
    Checks if the regions of all the channels are same.

@param[in]    dev_config   device config information to use
@param[in]    
@param[inout] 

*/
//*============================================================================*/

icbcfg_error_type compare_ch_segments
( 
  icbcfg_device_config_type *dev_config
)
{
  uint32 i, j, base_low, base_high, off_low, off_high;

  /* Read in the configuration, based on the hardware type. */
  switch(dev_config->trans_type)
  {
    case ICBCFG_ADDR_TRANS_NOC:
    for( i = 0; i < dev_config->num_segments; i++ )
    {
      if(i != 0)
      {
        base_low = HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_BASEn_LOW,i);
        base_high = HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH,i);
      }
      off_low = HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW,i);
      off_high = HWIO_INXI(dev_config->trans_bases[0],MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH,i);
      for( j = 1; j < dev_config->num_channels; j++ )
      {
        if((i != 0) && 
          ((base_low != HWIO_INXI(dev_config->trans_bases[j],MEMNOC_ADDR_TRANSLATOR_BASEn_LOW,i)) ||
          (base_high != HWIO_INXI(dev_config->trans_bases[j],MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH,i))))
          {
            return ICBCFG_ERROR;
          }
        if((off_low!= HWIO_INXI(dev_config->trans_bases[j],MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW,i) || 
          (off_high != HWIO_INXI(dev_config->trans_bases[j],MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH,i))))
          {
            return ICBCFG_ERROR;
          }
      }
    }
       break;

    case ICBCFG_ADDR_TRANS_LLCC:
       for( i = 0; i < dev_config->num_segments; i++ )
       {
         if(i != 0)
         {
           base_low = HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG2,i);
           base_high = HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG3,i);
         }
         off_low = HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG0,i);
         off_high = HWIO_INXI(dev_config->trans_bases[0],LLCC_BEAC_ADDR_REGIONn_CFG1,i);
         for( j = 1; j < dev_config->num_channels; j++ )
         {
            /* Base register does not exist for first segment. Forced to 0x0. */
            if((i != 0) && 
             ((base_low != HWIO_INXI(dev_config->trans_bases[j],LLCC_BEAC_ADDR_REGIONn_CFG2,i))|| 
             (base_high != HWIO_INXI(dev_config->trans_bases[j],LLCC_BEAC_ADDR_REGIONn_CFG3,i))))
            {
              return ICBCFG_ERROR;
            }
            if(off_low != HWIO_INXI(dev_config->trans_bases[j],LLCC_BEAC_ADDR_REGIONn_CFG0,i) ||
                off_high != HWIO_INXI(dev_config->trans_bases[j],LLCC_BEAC_ADDR_REGIONn_CFG1,i))
            {
              return ICBCFG_ERROR;
            }
          }
       }
       break;

    default:
       /* Didn't match a hardware type? Bail. */
       return ICBCFG_ERROR;
  }
  
  return ICBCFG_SUCCESS;
}

//*============================================================================*/
/**
@brief
    Checks the aliasing, if different SOC address points to same DDR location, it will return ERROR.

@param[in]    dev_config   device config information to use
@param[in]    
@param[inout] 

*/
//*============================================================================*/


icbcfg_error_type check_dupRegion( icbcfg_device_config_type * dev_config )
{
  icbcfg_error_type ret_val = ICBCFG_SUCCESS;
  uint32 i,j;
  region_type region1,region2;
  
  do
  {
    if(compare_ch_segments(dev_config) != ICBCFG_SUCCESS)
    {
      return ICBCFG_ERROR;
    }
    /*We want to loop through only till one less than the segments*/
    for( i = 0; i < dev_config->num_segments - 1; i++ )
    {
      if(addr_trans_segs[i].error)
      {
        continue;
      }
      if(addr_trans_segs[i].base < addr_trans_segs[i+1].base)
      {
        region1.base_addr = addr_trans_segs[i].base + addr_trans_segs[i].offset;
        region1.size = addr_trans_segs[i+1].base - addr_trans_segs[i].base;
        region1.interleaved = TRUE;
      }
      else
      {
        return ICBCFG_ERROR;
      }
      /*We want to loop through till the end. 
          Last segment has the base address as the 1ULL << dev_config->addr_width;Max address, not inclusive
          which makes the last segment size as 0x1000000000 - last_segment_valid_base*/
      for( j = i+1; j < dev_config->num_segments; j++ )
      {
        if(addr_trans_segs[j].error)
        {
          continue;
        }
        /*addr_trans_segs size is MAX_SEGS +1,so accessing j+1 is OK*/
        if(addr_trans_segs[j].base < addr_trans_segs[j+1].base)
        {
          region2.base_addr = addr_trans_segs[j].base + addr_trans_segs[j].offset;
          region2.size = addr_trans_segs[j+1].base - addr_trans_segs[j].base;
          region2.interleaved = TRUE;
        }
        else
        {
          return ICBCFG_ERROR;
        }

        if(check_overlap(&region1,&region2))
        {
          return ICBCFG_ERROR;
        }
      }
    }
  } while( false );
  return ret_val;
}
/*============================================================================
                          Internal functions
============================================================================*/
//*============================================================================*/
/**
@brief
      Informs query layer to update NoC translation cache.

@param[in]    dev  DAL string device ID.
*/
/*============================================================================*/
void update_addr_translation( void )
{
   /* Only update if we've been initialized. */
   if( query_dev_config_valid )
   {
      get_addr_translation( icb_dev_config );
   }
}

/*============================================================================
                          External functions
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
icbcfg_error_type ICB_Get_Memmap( const char * dev, icb_mem_map_type * info )
{
   icbcfg_error_type ret_val = ICBCFG_ERROR;

   do
   {
      icbcfg_device_config_type *dev_config;

      /* Validate config pointers. */
      if (!dev || !info)
      {
         ret_val = ICBCFG_ERROR_INVALID_PARAM;
         break;
      }

      /* Fetch device configuration data. */
      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      /* Get the system memory map, holes and all. */
      get_system_memory_map(dev_config, info);

      ret_val = ICBCFG_SUCCESS;
   } while( false );

   return ret_val;
}

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
icbcfg_error_type ICB_Get_AllowedMemRegion( const char * dev, icbcfg_mem_region_type * info )
{
   icbcfg_error_type ret_val = ICBCFG_ERROR;

   do
   {
      icbcfg_device_config_type *dev_config;

      /* Validate config pointers. */
      if (!dev || !info)
      {
         ret_val = ICBCFG_ERROR_INVALID_PARAM;
         break;
      }

      /* Fetch device configuration data. */
      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      /* Check for duplicate/aliased regions. */
      if (check_dupRegion( dev_config ) != ICBCFG_SUCCESS)
      {
        break;
      }
      /* Copy our allowed memory region definition. */
      *info = dev_config->map_ddr_regions[0];

      ret_val = ICBCFG_SUCCESS;
   } while( false );

   return ret_val;
}

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
icbcfg_error_type ICB_Get_AllowedMemRegionEx
(
   const char *              dev,
   icbcfg_mem_region_type ** info,
   uint32 *                  num_regions
)
{
   icbcfg_error_type ret_val = ICBCFG_ERROR;

   do
   {
      icbcfg_device_config_type *dev_config;

      /* Validate config pointers. */
      if (!dev || !info || !num_regions)
      {
         ret_val = ICBCFG_ERROR_INVALID_PARAM;
         break;
      }

      /* Fetch device configuration data. */
      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      /* Check for duplicate/aliased regions. */
	  if (check_dupRegion( dev_config ) != ICBCFG_SUCCESS)
      {
        break;
      }

      /* Copy our allowed memory region definition. */
      *info        = dev_config->map_ddr_regions;
      *num_regions = dev_config->map_ddr_region_count;

      ret_val = ICBCFG_SUCCESS;
   } while( false );

   return ret_val;
}

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
icbcfg_error_type ICB_Get_MCAddr( const char * dev, uint64 soc_addr, uint64 * mc_addr )
{
   icbcfg_error_type ret_val = ICBCFG_ERROR;

   do
   {
      icbcfg_device_config_type *dev_config;

      /* Validate config pointers. */
      if (!dev || !soc_addr || !mc_addr)
      {
         ret_val = ICBCFG_ERROR_INVALID_PARAM;
         break;
      }

      /* Fetch device configuration data. */
      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      ret_val = translate_address( dev_config, soc_addr, mc_addr );
   } while( false );

   return ret_val;
}





/* vim: set ts=3 sts=3 sw=3 et :*/
