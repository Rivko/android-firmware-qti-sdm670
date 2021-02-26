/*==============================================================================

FILE:      icbcfg.c

DESCRIPTION: This file implements the ICB Configuration driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBLib/icbcfg.c#1 $ 
$DateTime: 2017/09/18 09:26:13 $
$Author: pwbldsvc $
$Change: 14402555 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/05/08  sds    Add support for alternate address translator
2016/10/11  sds    Restructure config/query interfaces
2015/07/20  sds    Add ICB_Config_PostInit().
2015/06/23  sds    Add support for multiple remap requests via RemapEx()
2015/06/18  sds    Make the desperation checks a little smarter.
2015/02/10  sds    Cleaned up address compression functions and added additional
                   desperation checks for segment mapping.
2014/11/05  sds    Add ICB_Unmap_L2_TCM() API.
2014/06/03  sds    Update max number of segments, and make segment algorithm
                   aware that this can be variable.
2014/05/19  sds    Added smarter mapping/remapping schemes
2013/11/08  vg     Added auto detection for MSM8926
2013/10/21  sds    Add additional method to attempt to map BRIC segments
2013/10/10  sds    BIMC remap support now checks for hardware support explicitly
2013/09/24  sds    Refactor code for better reuse and code size.
2013/09/23  sds    Fixes for BIMC remapper support
2013/08/30  sds    Added support for BIMC remap operations
2013/05/17  sds    Fixed the handling of non-power of two regions
2013/02/26  sds    Changed how available DDR regions are handled.
2012/08/07  sy     Fixed Linux compilation Error 
2012/02/07  av     Created
 
        Copyright (c) 2012-2016 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfg.h"
#include "icbcfgi.h"
#include "busywait.h"

/*============================================================================
                          DEFINES/MACROS
============================================================================*/
#define ICB_IN8(addr)         (*((volatile uint8 *) (addr)))
#define ICB_IN16(addr)        (*((volatile uint16 *) (addr)))
#define ICB_IN32(addr)       (*((volatile uint32 *) (addr)))


#define ICB_OUT8(addr, data)   (*((volatile uint8 *) (addr)) = ((uint8) (data)))
#define ICB_OUT16(addr, data)  (*((volatile uint16 *) (addr)) = ((uint16) (data)))
#define ICB_OUT32(addr, data)  (*((volatile uint32 *) (addr)) = ((uint32) (data)))


#define ICB_CFG_8(addr, bmsk, rmsk, data) \
            ICB_OUT8(addr, ((bmsk) != (rmsk) ? \
                            (((ICB_IN8(addr) & (rmsk)) & (uint8)(~(bmsk))) | ((data) & (bmsk))) : \
                            ((data) & (bmsk)) ))

#define ICB_CFG_16(addr, bmsk, rmsk, data) \
            ICB_OUT16(addr, ((bmsk) != (rmsk) ? \
                            (((ICB_IN16(addr) & (rmsk)) & (uint16)(~(bmsk))) | ((data) & (bmsk))) : \
                            ((data) & (bmsk)) ))

#define ICB_CFG_32(addr, bmsk, rmsk, data) \
            ICB_OUT32(addr, ((bmsk) != (rmsk) ? \
                            (((ICB_IN32(addr) & (rmsk)) & (uint32)(~(bmsk))) | ((data) & (bmsk))) : \
                            ((data) & (bmsk)) ))

#define ICB_CFG_IN8(addr, bmsk, rmsk, shft) \
            (((ICB_IN8(addr) & (rmsk)) & (uint8)(bmsk)) >> (shft))

#define ICB_CFG_IN16(addr, bmsk, rmsk, shft) \
            (((ICB_IN16(addr) & (rmsk)) & (uint16)(bmsk)) >> (shft))

#define ICB_CFG_IN32(addr, bmsk, rmsk, shft) \
            (((ICB_IN32(addr) & (rmsk)) & (uint32)(bmsk)) >> (shft))

#define ICB_CFG_OUT8(addr, bmsk, rmsk, shft, data) \
            ICB_OUT8(addr, ((bmsk) != (rmsk) ? \
                            (((ICB_IN8(addr) & (rmsk)) & (uint8)(~(bmsk))) | (((data) & (bmsk)) << (shft))) : \
                            (((data) & (bmsk)) << (shft)) ))

#define ICB_CFG_OUT16(addr, bmsk, rmsk, shft, data) \
            ICB_OUT16(addr, ((bmsk) != (rmsk) ? \
                            (((ICB_IN16(addr) & (rmsk)) & (uint16)(~(bmsk))) | (((data) & (bmsk)) << (shft))) : \
                            (((data) & (bmsk)) << (shft)) ))

#define ICB_CFG_OUT32(addr, bmsk, rmsk, shft, data) \
            ICB_OUT32(addr, ((bmsk) != (rmsk) ? \
                            (((ICB_IN32(addr) & (rmsk)) & (uint32)(~(bmsk))) | (((data) & (bmsk)) << (shft))) : \
                            (((data) & (bmsk)) << (shft)) ))

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#define MAX_WAIT_CNT 1000
#define WAIT_US 1

/*============================================================================
                                 FUNCTIONS
============================================================================*/

/*============================================================================
                          Definitions
============================================================================*/
#define MAX_CS 2
#define LAST_REGION (MAX_REGIONS-1)

#define ADDR_LOW(val)  ((uint32)(((uint64)(val)) & 0xFFFFFFFF))
#define ADDR_HIGH(val) ((uint32)((((uint64)(val)) >> 32) & 0xFFFFFFFF))

/*============================================================================
                          Definitions
============================================================================*/
typedef struct
{
   uint64 start_addr;
   int64  offset;
} icb_hw_addr_type;

typedef struct
{
   icb_hw_addr_type region[MAX_CHANNELS][MAX_REGIONS];
   uint32           error_mask[MAX_CHANNELS];
} icb_hw_map_type;

typedef enum
{
   REGION_MATCH_NONE,
   REGION_MATCH_EXACT,
   REGION_MATCH_START,
   REGION_MATCH_INTERNAL,
   REGION_MATCH_END
} icb_match_type;

/*============================================================================
                          Global data 
============================================================================*/
static icbcfg_device_config_type *icb_dev_config = NULL;
bool                              dev_config_valid = false;
icbcfg_remap_info_type            req_remaps[MAX_REGIONS];
icbcfg_remap_info_type            sorted_remaps[MAX_REGIONS];

/*============================================================================
                          Helper functions
============================================================================*/
//*============================================================================*/
/**
@brief
    Write out ICBCFG addr:data settings.
 
@param[in] dev_config  The settings list to use

@return    
      Success or failure.
*/ 
/*============================================================================*/
static icbcfg_error_type ICB_Configure_Settings( icbcfg_prop_type *prop_data )
{
   
   icbcfg_data_type* cfg_data;
   uint32 i, cnt = 0, data_32 = 0;
   uint16 data_16 = 0;
   uint8 data_8 = 0;
   icbcfg_error_type ret_val = ICBCFG_SUCCESS;
   
   /* Make sure we have a list first. */
   if( NULL == prop_data )
   {
      return ret_val;
   }

   cfg_data = prop_data->cfg_data;
               
   for (i=0; i < prop_data->len ; i++)
   {
      /* Check against NULL pointer */
      if(cfg_data[i].addr == NULL &&
         cfg_data[i].cfg_type != ICBCFG_DEFAULT) 
      {
          ret_val = ICBCFG_ERROR_INVALID_PARAM;
          break;
      }
  
      switch(cfg_data[i].cfg_type) 
      {           
       case ICBCFG_32_BIT_REG:
           /* 32 bit register configuration */
           ICB_CFG_32(cfg_data[i].addr, 
                      cfg_data[i].bmask,
                      cfg_data[i].rmask,
                      cfg_data[i].data);
           break;
        
       case ICBCFG_16_BIT_REG:
           /* 16 bit register configuration */
           ICB_CFG_16(cfg_data[i].addr, 
                      cfg_data[i].bmask,
                      cfg_data[i].rmask,
                      cfg_data[i].data);
           break;
  
       case ICBCFG_8_BIT_REG:
           /* 8 bit register configuration */
           ICB_CFG_8(cfg_data[i].addr, 
                     cfg_data[i].bmask,
                     cfg_data[i].rmask,
                     cfg_data[i].data);
           break;

       case ICBCFG_32_BIT_READ_REG:
           /* 32-bit register read */
           data_32 = ICB_CFG_IN32(cfg_data[i].addr,
                                  cfg_data[i].bmask,
                                  cfg_data[i].rmask,
                                  cfg_data[i].data);
           break;

       case ICBCFG_16_BIT_READ_REG:
           /* 16-bit register read */
           data_16 = ICB_CFG_IN16(cfg_data[i].addr,
                                  cfg_data[i].bmask,
                                  cfg_data[i].rmask,
                                  cfg_data[i].data);
           break;

       case ICBCFG_8_BIT_READ_REG: 
           /* 8-bit register read */
           data_8 = ICB_CFG_IN8(cfg_data[i].addr,
                                cfg_data[i].bmask,
                                cfg_data[i].rmask,
                                cfg_data[i].data);
           break;
  
       case ICBCFG_32_BIT_WRITE_REG:
           /* 32-bit register write */
           ICB_CFG_OUT32(cfg_data[i].addr,
                         cfg_data[i].bmask,
                         cfg_data[i].rmask,
                         cfg_data[i].data,
                         data_32);
           break;

       case ICBCFG_16_BIT_WRITE_REG:
           /* 16-bit register write */
           ICB_CFG_OUT16(cfg_data[i].addr,
                         cfg_data[i].bmask,
                         cfg_data[i].rmask,
                         cfg_data[i].data,
                         data_16);
           break;

       case ICBCFG_8_BIT_WRITE_REG: 
           /* 8-bit register write */
           ICB_CFG_OUT8(cfg_data[i].addr,
                        cfg_data[i].bmask,
                        cfg_data[i].rmask,
                        cfg_data[i].data,
                        data_8);
           break;

       case ICBCFG_32_BIT_WAIT_REG:
           /* 32-bit register read */
           data_32 = ICB_CFG_IN32(cfg_data[i].addr,
                                  cfg_data[i].bmask,
                                  cfg_data[i].rmask,
                                  0);

           for(cnt = 0; cnt < MAX_WAIT_CNT && data_32 != cfg_data[i].data; cnt++)
           {
              busywait(WAIT_US);
              data_32 = ICB_CFG_IN32(cfg_data[i].addr,
                                     cfg_data[i].bmask,
                                     cfg_data[i].rmask,
                                     0);
           }

           /* If we've exceeded our wait and we still don't have the value we want,
            * we've timed out and so report an error. */
           if( cnt >= MAX_WAIT_CNT && data_32 != cfg_data[i].data )
           {
              ret_val = ICBCFG_ERROR_INIT_FAILURE;
           }
           break;

       case ICBCFG_16_BIT_WAIT_REG:
           /* 16-bit register read */
           data_16 = ICB_CFG_IN16(cfg_data[i].addr,
                                  cfg_data[i].bmask,
                                  cfg_data[i].rmask,
                                  0);

           for(cnt = 0; cnt < MAX_WAIT_CNT && data_16 != cfg_data[i].data; cnt++)
           {
              busywait(WAIT_US);
              data_16 = ICB_CFG_IN16(cfg_data[i].addr,
                                     cfg_data[i].bmask,
                                     cfg_data[i].rmask,
                                     0);
           }

           /* If we've exceeded our wait and we still don't have the value we want,
            * we've timed out and so report an error. */
           if( cnt >= MAX_WAIT_CNT && data_16 != cfg_data[i].data )
           {
              ret_val = ICBCFG_ERROR_INIT_FAILURE;
           }
           break;


       case ICBCFG_8_BIT_WAIT_REG: 
           /* 8-bit register read */
           data_8 = ICB_CFG_IN8(cfg_data[i].addr,
                                  cfg_data[i].bmask,
                                  cfg_data[i].rmask,
                                  0);

           for(cnt = 0; cnt < MAX_WAIT_CNT && data_8 != cfg_data[i].data; cnt++)
           {
              busywait(WAIT_US);
              data_8 = ICB_CFG_IN8(cfg_data[i].addr,
                                     cfg_data[i].bmask,
                                     cfg_data[i].rmask,
                                     0);
           }

           /* If we've exceeded our wait and we still don't have the value we want,
            * we've timed out and so report an error. */
           if( cnt >= MAX_WAIT_CNT && data_8 != cfg_data[i].data )
           {
              ret_val = ICBCFG_ERROR_INIT_FAILURE;
           }
           break;
  
  
       case ICBCFG_DEFAULT:
            /* Nothing to do, return success */
            return ICBCFG_SUCCESS;
  
       default:
           /* Error */
           ret_val = ICBCFG_ERROR_INVALID_PARAM;
           break;
       }
   } 
   return ret_val;
}

//*============================================================================*/
/**
@brief
    Fetch the items we have in device config
 
@param[in]  dev         DAL string device ID.
@param[in]  dev_config  The pointer to store the device configuration pointer to

@return    
    Whether or not the config data is valid
*/ 
/*============================================================================*/
static bool get_device_configuration
(
   const char *                dev,
   icbcfg_device_config_type **dev_config
)
{
   if( !dev_config_valid )
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
         memset(req_remaps, 0, sizeof(req_remaps));
         memset(sorted_remaps, 0, sizeof(sorted_remaps));
         dev_config_valid = true;
      } while( false );
   }

   *dev_config = icb_dev_config;
   return dev_config_valid;
}

//*============================================================================*/
/**
@brief
    Convert from DDR info type to our internal region representation.
 
@param[in]  info
@param[in]  num_channels
@param[in]  ddr_regions

*/ 
//*============================================================================*/
void convert_ddr_regions
(
  ddr_info *              info,
  uint32                  num_channels,
  icb_mem_map_type *      ddr_regions
)
{
   uint32 cs, channel, region;
   region_type *channel_regions;
   uint32 *cs_sizes[MAX_CS] = {&info->ddr_size.ddr_cs0[0],
                               &info->ddr_size.ddr_cs1[0]};
   uint64 *cs_addrs[MAX_CS] = {&info->ddr_size.ddr_cs0_addr[0],
                               &info->ddr_size.ddr_cs1_addr[0]};

   /* Convert to our representation, sweeping together chip selects.
     * We can only sweep together chip selects if everything is interleaved
     * (CS0 and CS1) or nothing is, and the chip selects on the same channel
     * are contiguous. */
   memset(ddr_regions, 0, sizeof(icb_mem_map_type));
   for( channel = 0; channel < num_channels; channel++ )
   {
      region = 0;
      channel_regions = ddr_regions->channels[channel].regions;
      for( cs = 0; cs < MAX_CS; cs++ )
      { 
         uint64 cs_size;

         if( 0 == cs_sizes[cs][channel] )
         {
            continue;
         } 

         /* Size of the region depends on whether or not we're interleaved.
          * Multiply the size we're given by the number of channels to
          * determine the actual region size. */
         cs_size = (info->interleaved_memory & (1 << cs)) != 0 ?
                    ((uint64)cs_sizes[cs][channel] << 20) * num_channels:
                    (uint64)cs_sizes[cs][channel] << 20;

         channel_regions[region].interleaved = 
           (info->interleaved_memory & (1 << cs)) != 0;
         channel_regions[region].base_addr = 
           cs_addrs[cs][channel];
         channel_regions[region].size = cs_size;
         region++;
      }
   }
}

//*============================================================================*/
/**
@brief
    Compress a region struct to minimal regions and apply constraints
 
@param[in]     dev_config   a pointer to the device configuration data to use
@param[in]     constrain    whether or not to constrain the minimal region sweeping
                            to avoid our safe reset/hardware reserved region
@param[inout]  ddr_regions  the regions descriptor to sweep together

*/ 
//*============================================================================*/
void compress_regions
(
  icbcfg_device_config_type *dev_config,
  bool                       constrain,
  icb_mem_map_type *         ddr_regions
)
{
   uint64 map_ddr_start_addr = 0, map_ddr_end_addr = 0;
   uint32 channel;

   /* Condense regions to remove any size zero regions.
    * Also sort by base address, since hardware requires this in order. */
   for( channel = 0; channel < dev_config->num_channels; channel++ )
   {
      uint32 i, pass;
      region_type *channel_regions = ddr_regions->channels[channel].regions, temp_region;

      /* First, clear any zero size regions. */
      for( i = 1; i < MAX_REGIONS; i++ )
      {
         if( 0 == channel_regions[i].size )
         {
            channel_regions[i].base_addr   = 0;
            channel_regions[i].interleaved = false;
         }
      }

      /* Now sort, lowest addresses first. */
      for( pass = 0; pass < (MAX_REGIONS - 1); pass++ )
      {
         for( i = 0; i < (MAX_REGIONS - 1 - pass); i++ )
         {
            /* Swap if:
             * The next region has a lower base address and a size
             * The next region has a size and the current region does not. */
            if( (channel_regions[i+1].base_addr < channel_regions[i].base_addr && 0 != channel_regions[i+1].size) ||
                (0 != channel_regions[i+1].size && 0 == channel_regions[i].size) )
            {
               temp_region = channel_regions[i];
               channel_regions[i] = channel_regions[i+1];
               channel_regions[i+1] = temp_region;
            }
         }
      }
   }

   /* Sweep together segments in place, if possible. */
   for( channel = 0; channel < dev_config->num_channels; channel++ )
   {
      region_type *channel_regions = ddr_regions->channels[channel].regions;
      uint32 region, comp_region, used_region;

      /* Skip the first region, since we are doing an in-place sweep. */
      for( comp_region = 0, region = 0; region < MAX_REGIONS; region++ )
      { 
         if( 0 == channel_regions[region].size )
         {
            continue;
         }

         /* Do no "sweeping together" for regions with the same base address.
          * This is either a DDR misconfiguration or we're looking at the same region.
          * Apply constraints if requested to do so. */
         if( channel_regions[region].base_addr == 
             channel_regions[comp_region].base_addr )
         {
           /* Nothing to do here. */
         }
         /* Regions are only considered contiguous regions if they
         * share the same interleaving mode, and are address contiguous. */
         else if( !((channel_regions[region].interleaved ==
                     channel_regions[comp_region].interleaved) &&
                     ( /* This region is contiguous after the previous region */
                       (channel_regions[region].base_addr==
                       (channel_regions[comp_region].base_addr +
                        channel_regions[comp_region].size)) ||
                       /* This region is contiguous before the previous region */
                       (channel_regions[comp_region].base_addr ==
                       (channel_regions[region].base_addr +
                        channel_regions[region].size))
                     )))
         {
            /* Create a new region. We're either the first, or a new one. */
            comp_region++;
            channel_regions[comp_region].interleaved = channel_regions[region].interleaved;
            channel_regions[comp_region].base_addr = channel_regions[region].base_addr;
            channel_regions[comp_region].size = channel_regions[region].size;
         }
         /* Not the first region, and extendable.
          * Skip this if we have a region with a matching base due to DDR
          * misconfiguration. */
         else
         {
            /* Add to existing region. */
            channel_regions[comp_region].size += channel_regions[region].size;

            /* If the merged region is under the existing region, use the merged region's start address. */
            if( channel_regions[comp_region].base_addr > channel_regions[region].base_addr )
            {
               channel_regions[comp_region].base_addr = channel_regions[region].base_addr;
            }
         }

         /* Constrain segment addresses/sizes, if required. */
         if( constrain )
         {
            /* First find the appropriate region to match to, since we need to limit
             * sweeping together of regions that aren't valid.
             * If we are below the start or DDR space, make it the start of DDR.
             * If we are past the end of DDR space, adjust the size accordingly. */
            uint32 ridx;
            for( ridx = 0; ridx < dev_config->map_ddr_region_count; ridx++ )
            {
               /* If the start address of either region lies in the other, select it. */
               if( (channel_regions[comp_region].base_addr >= dev_config->map_ddr_regions[ridx].start_addr &&
                    channel_regions[comp_region].base_addr < dev_config->map_ddr_regions[ridx].end_addr) ||
                   (dev_config->map_ddr_regions[ridx].start_addr >= channel_regions[comp_region].base_addr &&
                    dev_config->map_ddr_regions[ridx].start_addr < (channel_regions[comp_region].base_addr +channel_regions[comp_region].size)) )
               {
                  map_ddr_start_addr = dev_config->map_ddr_regions[ridx].start_addr;
                  map_ddr_end_addr   = dev_config->map_ddr_regions[ridx].end_addr;
                  break;
               }
            }

            /* If we didn't find a region that matches, it lies outside of the allowable address space.
             * Remove it. */
            if( ridx == dev_config->map_ddr_region_count )
            {
               channel_regions[comp_region].interleaved = false;
               channel_regions[comp_region].base_addr = 0;
               channel_regions[comp_region].size = 0;

               /* Decrememnt comp region pointer, but only if we're not at zero. */
               if( 0 != comp_region )
               {
                  comp_region--;
               }
               continue;
            }

            /* Enforce memory map limits on DDR space. */
            if( channel_regions[comp_region].base_addr < map_ddr_start_addr )
            {
               channel_regions[comp_region].size -= map_ddr_start_addr - channel_regions[comp_region].base_addr;
               channel_regions[comp_region].base_addr = map_ddr_start_addr;
            }
            if( channel_regions[comp_region].base_addr + channel_regions[comp_region].size > map_ddr_end_addr )
            {
               channel_regions[comp_region].size = map_ddr_end_addr - channel_regions[comp_region].base_addr;
            }
         }
      }

      /* Check to see if we exceed the number of segments allowed or
       * if we have nothing in the array. */
      if( comp_region > dev_config->num_segments ||
          (0 == comp_region && 0 == channel_regions[0].size))
      {
        break;
      }

      /* Shift up to the front of the region array, if necessary.
       * Find the first non-zero sized region. */
      for( used_region = 0; used_region < MAX_REGIONS; used_region++ )
      {
         if( 0 != channel_regions[used_region].size )
         {
            break;
         }
      }
      if( 0 != used_region )
      {
         memmove(&channel_regions[0],
                 &channel_regions[used_region],
                 sizeof(channel_regions[0]) * (MAX_REGIONS - used_region));
         comp_region -= used_region;
      }

      /* Clear unused regions. */
      for( comp_region++; comp_region < MAX_REGIONS; comp_region++ )
      {
         channel_regions[comp_region].interleaved = false;
         channel_regions[comp_region].base_addr = 0;
         channel_regions[comp_region].size = 0;
      }
   }
}

//*============================================================================*/
/**
@brief
    Search for a match with the incoming region in the requested remap list.
 
@param[in]     remaps - The array of remaps to search
@param[in]     region - The region to match against
@param[inout]  remap  - The remap region matched, NULL if not found
@param[inout]  match  - Match type
*/ 
//*============================================================================*/
void find_remap_match
(
   icbcfg_remap_info_type * remaps,
   region_type *            region,
   icbcfg_remap_info_type **remap,
   icb_match_type *         match
)
{
   bool found = false;
   *match = REGION_MATCH_NONE;

   /* Search our requested remaps list for a match. */
   for( uint32 i = 0; i < MAX_REGIONS; i++ )
   {
      /* Skip zero size entries */
      if( !remaps[i].size )
      {
         continue;
      }

      /* Do we match the start of this remap? */
      if( region->base_addr == remaps[i].dest_addr )
      {
         /* Now we need to check if this region is an exact match or not. */
         if( region->size == remaps[i].size )
         {
            *match = REGION_MATCH_EXACT;
         }
         /* Otherwise, we're just a match at the start. */
         else
         {
            *match = REGION_MATCH_START;
         }
         found = true;
      }
      /* Does the remap start address lie within my region? */
      else if( remaps[i].dest_addr > region->base_addr &&
               remaps[i].dest_addr < (region->base_addr + region->size) )
      {
         /* Now we need to check if our end also lies in the the remap. */
         if( (remaps[i].dest_addr + remaps[i].size) <
             (region->base_addr + region->size) )
         {
            *match = REGION_MATCH_INTERNAL;
         }
         /* Otherwise, we're an "end" match. */
         else
         {
            *match = REGION_MATCH_END;
         }
         found = true;
      }

      if( found )
      {
         *remap = &remaps[i];
         break;
      }
   }
}

//*============================================================================*/
/**
@brief
    Generate a harware memory map.
 
@param[in]     dev_config
@param[in]     map
@param[inout]  hw_map
*/ 
//*============================================================================*/
void generate_hw_map
(
   icbcfg_device_config_type *dev_config,
   icb_mem_map_type *         map,
   icb_hw_map_type  *         hw_map
)
{
   /* Clear the hw map. */
   memset(hw_map, 0, sizeof(icb_hw_map_type));

   /* Generate a hardware map for each channel. */
   for( uint32 channel = 0;
        channel < dev_config->num_channels;
        channel++ )
   {
      uint32 map_idx = 0, hw_idx = 0;

      /* If we have an initial invalid region, mark the
       * the first region for error in the hardware map.
       * No need to set address or offset, since we've
       * memset the whole structure to zero already. */
      if( dev_config->map_ddr_regions[0].start_addr > 0 )
      {
         hw_map->error_mask[channel] = 1;
         hw_idx = 1;
      }

      /* Now parse the ddr memory map. */
      while( map_idx < MAX_REGIONS && hw_idx < MAX_REGIONS )
      {
         region_type *channel_regions = map->channels[channel].regions;
         region_type *curr_region = &channel_regions[map_idx];
         icbcfg_remap_info_type *remap;
         icb_match_type match;
         uint64 next_start_addr;

         /* Skip zero sized regions. */
         if( 0 == channel_regions[map_idx].size )
         {
            map_idx++;
            continue;
         }

         /* If this is not our first parsed region, we need to see if there's
          * a empty decode space we need to add. */
         if( 0 != map_idx &&
             hw_map->region[channel][hw_idx].start_addr != curr_region->base_addr )
         {
            hw_map->error_mask[channel] |= 1 << hw_idx;
            hw_idx++;
         }

         /* If we've gone off the end, break out. */
         if( hw_idx >= MAX_REGIONS )
         {
            break;
         }

         /* Copy the region in.
          * Check for a potential remap. We may need to split the region we're in.
          * Add the "end" in the next region if we have one available. */
         hw_map->region[channel][hw_idx].start_addr = curr_region->base_addr;

         find_remap_match( sorted_remaps, curr_region, &remap, &match );
         switch(match)
         {
            /* No remap region matched.
             * Next addr is the end of this region. */
            case REGION_MATCH_NONE:
               next_start_addr = curr_region->base_addr + curr_region->size;
               map_idx++;
               break;
            /* Exact remap region matched.
             * Next addr is the end of this region.
             * Set the offset. */
            case REGION_MATCH_EXACT:
               next_start_addr = curr_region->base_addr + curr_region->size;
               hw_map->region[channel][hw_idx].offset =
                  (int64)remap->src_addr - (int64)remap->dest_addr;
               map_idx++;
               break;
            /* A remap region matches the start of the current mapped region.
             * Next addr is the end of the remap region.
             * Set the offset and adjust the current region to match the remainder. */
            case REGION_MATCH_START:
               next_start_addr = curr_region->base_addr + remap->size;
               hw_map->region[channel][hw_idx].offset =
                  (int64)remap->src_addr - (int64)remap->dest_addr;
               curr_region->size -= remap->size;
               curr_region->base_addr += remap->size;
               break;
            /* A remap region is within or at the end of the current mapped region.
             * Next addr is the beginning of the remap region.
             * Do not set the offset as we are not mapping the remapped region yet.
             * Adjust the region to match the remainder. */
            case REGION_MATCH_INTERNAL:
            case REGION_MATCH_END:
            {
               next_start_addr = remap->dest_addr;
               curr_region->size -= remap->dest_addr - curr_region->base_addr;
               curr_region->base_addr += remap->dest_addr - curr_region->base_addr;
               break;
            }
            default:
               /* Something went very wrong, get out. */
               return;
         }
         hw_idx++;

         if( hw_idx < MAX_REGIONS )
         {
            hw_map->region[channel][hw_idx].start_addr = next_start_addr;
         }
      }

      /* Fill in any unused regions with the last base and mark error. */
      for( uint32 last = hw_idx; hw_idx < MAX_REGIONS; hw_idx++ )
      {
         hw_map->region[channel][hw_idx].start_addr = hw_map->region[channel][last].start_addr;
         hw_map->error_mask[channel] |= 1 << hw_idx;
      }
   }
}

//*============================================================================*/
/**
@brief
    Write a memory map to the hardware.
 
@param[in]  dev_config
@param[in]  map

*/ 
//*============================================================================*/
void map_segments
(
   icbcfg_device_config_type *dev_config,
   icb_mem_map_type *         map
)
{
   icb_hw_map_type hw_map;

   /* Get the map in hardware format. */
   generate_hw_map(dev_config, map, &hw_map);

   switch(dev_config->trans_type)
   {
      case ICBCFG_ADDR_TRANS_NOC:
         /* Disable all address translators while configuring. */
         for( uint32 channel = 0;
              channel < dev_config->num_channels &&
              NULL != dev_config->trans_bases[channel];
              channel++ )
         {
            HWIO_OUTX(dev_config->trans_bases[channel],MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW, 0);
         }

         /* Program each addr translator. */
         for( uint32 channel = 0;
              channel < dev_config->num_channels &&
              NULL != dev_config->trans_bases[channel];
              channel++ )
         {
            HWIO_OUTX(dev_config->trans_bases[channel],
                      MEMNOC_ADDR_TRANSLATOR_ERROR_LOW,
                      hw_map.error_mask[channel]);

            for( uint32 region = 0; region < dev_config->num_segments; region++ )
            {
               icb_hw_addr_type *hw_addr = &hw_map.region[channel][region];

               /* Skip setting the base for region 0.
               * The register doesn't exist and hardware assumes zero. */
               if( region != 0 )
               {
                  HWIO_OUTXI(dev_config->trans_bases[channel],
                             MEMNOC_ADDR_TRANSLATOR_BASEn_LOW,
                             region,
                             ADDR_LOW(hw_addr->start_addr));
                  HWIO_OUTXFI(dev_config->trans_bases[channel],
                              MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH,
                              region,
                              ADDRH_MSB,
                              ADDR_HIGH(hw_addr->start_addr));
               }

               /* Set new offset. */
               HWIO_OUTXI(dev_config->trans_bases[channel],
                          MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW,
                          region,
                          ADDR_LOW(hw_addr->offset));
               HWIO_OUTXFI(dev_config->trans_bases[channel],
                           MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH,
                           region,
                           ADDRH_MSB,
                           ADDR_HIGH(hw_addr->offset));
            }
         }

         /* Enable all address translators now that we're done. */
         for( uint32 channel = 0;
              channel < dev_config->num_channels &&
              NULL != dev_config->trans_bases[channel];
              channel++ )
         {
            HWIO_OUTX(dev_config->trans_bases[channel],
                      MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW,
                      HWIO_FMSK(MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW,ENABLE));
         }
         break;
      case ICBCFG_ADDR_TRANS_LLCC:
         /* Disable all address translators while configuring. */
         for( uint32 channel = 0;
              channel < dev_config->num_channels &&
              NULL != dev_config->trans_bases[channel];
              channel++ )
         {
            HWIO_OUTXF(dev_config->trans_bases[channel],
                       LLCC_BEAC_ADDR_TRANSLATOR_CFG, ENABLE,
                       0);
         }

         /* Program each addr translator. */
         for( uint32 channel = 0;
              channel < dev_config->num_channels &&
              NULL != dev_config->trans_bases[channel];
              channel++ )
         {
            HWIO_OUTXF(dev_config->trans_bases[channel],
                       LLCC_BEAC_ADDR_TRANSLATOR_CFG, ERROR,
                       hw_map.error_mask[channel]);

            for( uint32 region = 0; region < dev_config->num_segments; region++ )
            {
               icb_hw_addr_type *hw_addr = &hw_map.region[channel][region];

               /* Skip setting the base for region 0.
               * The register doesn't exist and hardware assumes zero. */
               if( region != 0 )
               {
                  HWIO_OUTXI(dev_config->trans_bases[channel],
                             LLCC_BEAC_ADDR_REGIONn_CFG2,
                             region,
                             ADDR_LOW(hw_addr->start_addr));
                  HWIO_OUTXFI(dev_config->trans_bases[channel],
                              LLCC_BEAC_ADDR_REGIONn_CFG3,
                              region,
                              BASE_HIGH,
                              ADDR_HIGH(hw_addr->start_addr));
               }

               /* Set new offset. */
               HWIO_OUTXI(dev_config->trans_bases[channel],
                          LLCC_BEAC_ADDR_REGIONn_CFG0,
                          region,
                          ADDR_LOW(hw_addr->offset));
               HWIO_OUTXFI(dev_config->trans_bases[channel],
                           LLCC_BEAC_ADDR_REGIONn_CFG1,
                           region,
                           OFFSET_HIGH,
                           ADDR_HIGH(hw_addr->offset));
            }
         }

         /* Enable all address translators now that we're done. */
         for( uint32 channel = 0;
              channel < dev_config->num_channels &&
              NULL != dev_config->trans_bases[channel];
              channel++ )
         {
            HWIO_OUTXF(dev_config->trans_bases[channel],
                       LLCC_BEAC_ADDR_TRANSLATOR_CFG, ENABLE,
                       HWIO_FMSK(LLCC_BEAC_ADDR_TRANSLATOR_CFG,ENABLE));
         }
         break;
      default:
         return;
   }

   /* Update query cache. */
   update_addr_translation();
}

//*============================================================================*/
/**
@brief
    Adjust the regions for a given channel based on a region to remove.
 
@param[inout]  dev_config
@param[inout]  input_region
@param[inout]  channel_regions
@param[in]     assume_props     assume properties of overlayed region?

@returns true if the region was found
         false otherwise
*/ 
//*============================================================================*/
bool adjust_channel
(
   icbcfg_device_config_type *dev_config,
   region_type *              input_region,
   region_type *              channel_regions,
   bool                       assume_props
)
{
   uint32 free_region, region;
   bool found = false;

   /* Find the first unallocated region, in case we need to split later. */
   for( free_region = 0; free_region < dev_config->num_segments; free_region++ )
   {
      if( 0 == channel_regions[free_region].size )
      {
         break;
      }
   }

   /* Run through the regions and check for overlap.
      * We check for:
      * 1.) Complete overlap
      * 2.) Remap start in region
      * 3.) Remap start and end in region
      * 4.) Only input_region end in region */
   for( region = 0; region < dev_config->num_segments; region++ )
   {
      /* Skip size zero regions. */
      if( 0 == channel_regions[region].size )
      {
         continue;
      }

      /* Check for a complete overlap. */
      if( input_region->base_addr <= channel_regions[region].base_addr &&
         (input_region->base_addr + input_region->size) >=
            (channel_regions[region].base_addr + channel_regions[region].size))
      {
         found = true;
         if( assume_props )
         {
            input_region->interleaved = channel_regions[region].interleaved;
         }

         /* Completely overlapped, remove this region. */
         channel_regions[region].base_addr = 0;
         channel_regions[region].size      = 0;
      }
      /* Start exists in region */
      else if( (input_region->base_addr > channel_regions[region].base_addr) &&
               input_region->base_addr < (channel_regions[region].base_addr + channel_regions[region].size) )
      {
         found = true;
         if( assume_props )
         {
            input_region->interleaved = channel_regions[region].interleaved;
         }

         /* Check to see if the end also exists in this region, and we have a free region to split to. */
         if( (input_region->base_addr + input_region->size) > channel_regions[region].base_addr &&
             (input_region->base_addr + input_region->size) < (channel_regions[region].base_addr + channel_regions[region].size) &&
             free_region < dev_config->num_segments )
         {
            channel_regions[free_region].base_addr = input_region->base_addr + input_region->size;
            channel_regions[free_region].size      = channel_regions[region].size -
                                                            (input_region->size + (input_region->base_addr - channel_regions[region].base_addr));
         }
         channel_regions[region].size = input_region->base_addr - channel_regions[region].base_addr;
      }
      /* End exists in region, but not the start address. */
      else if( (input_region->base_addr + input_region->size) > channel_regions[region].base_addr &&
               (input_region->base_addr + input_region->size) < (channel_regions[region].base_addr + channel_regions[region].size) )
      {
         found = true;
         if( assume_props )
         {
            input_region->interleaved = channel_regions[region].interleaved;
         }

         channel_regions[region].size -= (input_region->base_addr + input_region->size) - channel_regions[region].base_addr;
         channel_regions[region].base_addr = (input_region->base_addr + input_region->size);
      }
   }

   return found;
}

//*============================================================================*/
/**
@brief
    Adjust the memory map to add the new region.
 
@param[in]  dev_config
@param[in]  remap
@param[in]  map

*/ 
//*============================================================================*/
void adjust_memory_map
(
   icbcfg_device_config_type *dev_config,
   icbcfg_remap_info_type *   remap,
   icb_mem_map_type *         map
)
{
   uint32 channel;
   region_type curr_map;

   curr_map.base_addr   = remap->src_addr;
   curr_map.size        = remap->size;
   curr_map.interleaved = true;

   /* Run through each channel and remove the existing region. */
   for( channel = 0; channel < dev_config->num_channels; channel++ )
   {
      region_type *channel_regions = map->channels[channel].regions;

      /* Add the new region to the end of the map, if we found a match.
      * Assume the same interleaving mode as the segment it left */
      if( adjust_channel( dev_config, &curr_map, channel_regions, true ) )
      {
        uint32 other_channel;

        channel_regions[LAST_REGION].base_addr   = remap->dest_addr;
        channel_regions[LAST_REGION].size        = remap->size;
        channel_regions[LAST_REGION].interleaved = curr_map.interleaved;

        /* Now, if this region isn't interleaved, we need to check to see
         * if we're going to overlap something on the other channels.
         * If we do, we need to shrink the section we're overlapping. */
        if( !curr_map.interleaved )
        {
          for( other_channel = 0; other_channel < dev_config->num_channels; other_channel++ )
          {
            region_type *other_channel_regions = map->channels[other_channel].regions;

            /* Skip if channel is the current one. */
            if( other_channel == channel )
            {
              continue;
            }

            (void)adjust_channel( dev_config, &channel_regions[LAST_REGION], other_channel_regions, false );
          }
        }
      }
   }

   compress_regions(dev_config, false, map);
}

bool will_overlap
(
   icbcfg_remap_info_type *elem,
   icbcfg_remap_info_type *remaps,
   uint32                  start_idx
)
{
   bool ret = false;

   for( uint32 i = start_idx; i < MAX_REGIONS; i++ )
   {
      /* Skip unused entries. */
      if( remaps[i].src_addr == remaps[i].dest_addr || remaps[i].size == 0 )
      {
         continue;
      }

      /* Does our start or end lie in this region? */
      if( (elem->dest_addr >= remaps[i].src_addr &&
           (elem->dest_addr + elem->size) < (remaps[i].src_addr + remaps[i].size)) ||
          ((elem->dest_addr + elem->size) > remaps[i].src_addr &&
            (elem->dest_addr + elem->size) <= (remaps[i].src_addr + remaps[i].size)))
      {
         ret = true;
         break;
      }
   }

   return ret;
}

/*============================================================================*/

/*============================================================================
                          External functions
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
icbcfg_error_type 
ICB_Config_Init( const char * dev )
{
   icbcfg_error_type ret_val = ICBCFG_ERROR_INIT_FAILURE;

   do
   {
      icbcfg_device_config_type *dev_config;

      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      ret_val = ICB_Configure_Settings(dev_config->prop_data);
   } while( false );

   return ret_val;
}

//
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
icbcfg_error_type
ICB_Config_PostInit( const char * dev )
{
   icbcfg_error_type ret_val = ICBCFG_ERROR_INIT_FAILURE;

   do
   {
      icbcfg_device_config_type *dev_config;

      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      ret_val = ICB_Configure_Settings(dev_config->post_prop_data);
   } while( false );

   return ret_val;
}

//*============================================================================*/
/**
@brief 
      Configures DDR slave segments in BIMC. It reads ICB configuration data (number
      of channels, maximum memory map, etc) from DAL Device Property file and configures 
      ICB registers.
 
@param[in]  dev     DAL string device ID.
@param[in]  config  DDR channel region configuration

@return    
      icbcfg_error_type indicating success or error.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
icbcfg_error_type
ICB_Segments_Init( const char * dev, ddr_info * info )
{
   icbcfg_error_type ret_val = ICBCFG_ERROR_INIT_FAILURE;

   do 
   {
      icbcfg_device_config_type *dev_config;
      icb_mem_map_type ddr_regions;

      /* Validate config pointer. */
      if (NULL == info)
      {
         ret_val = ICBCFG_ERROR_INVALID_PARAM;
         break;
      }

      /* Fetch device configuration data. */
      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      /* Convert to our representation, sweeping together chip selects. */
      convert_ddr_regions(info, dev_config->num_channels, &ddr_regions);
      compress_regions(dev_config, true, &ddr_regions);
      
      /* Map it to segments and write it to the hardware. */
      map_segments(dev_config, &ddr_regions);

      ret_val = ICBCFG_SUCCESS;
   } while( false );

   return ret_val;
}

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
icbcfg_error_type ICB_RemapEx
(
   const char *            dev,
   ddr_info *              info,
   uint32                  index,
   icbcfg_remap_info_type *remap
)
{
   icbcfg_error_type          ret_val = ICBCFG_ERROR;

   do
   {
      icb_mem_map_type           ddr_regions;
      uint32                     channel, region = 0, src_regions[MAX_CHANNELS], i;
      bool                       found;
      icbcfg_device_config_type *dev_config;

      /* Validate input parameters. */
      if(NULL == dev ||
         NULL == info ||
         index >= ARRAY_SIZE(req_remaps) )
      {
         ret_val = ICBCFG_ERROR_INVALID_PARAM;
         break;
      }

      /* Fetch device configuration data. */
      if(!get_device_configuration(dev, &dev_config))
      {
         break;
      }

      /* Get the current DDR address space (minimal segments) */
      convert_ddr_regions(info, dev_config->num_channels, &ddr_regions);
      compress_regions(dev_config, false, &ddr_regions);
   
      /* Update the proper remap index the the remap the caller wants.
       * If the remap pointer is NULL or the source equals dest, clear that index. */
      if(NULL == remap ||
         remap->src_addr == remap->dest_addr)
      {
         memset(&req_remaps[index], 0, sizeof(icbcfg_remap_info_type));
         memset(src_regions, 0xFF, sizeof(src_regions));
      }
      /* Otherwise, copy the desired remap setting.
       * Just check if it's possible first. */
      else
      {
         /* If the source address of the segment to remap isn't in the 
          * DDR physical memory, bail out. */
         found = false;
         for(channel = 0; channel < dev_config->num_channels; channel++)
         {
            src_regions[channel] = MAX_REGIONS;
            for(region = 0; region < dev_config->num_segments; region++)
            {
               region_type *channel_regions = &ddr_regions.channels[channel].regions[region];
   
               /* Are we contained by this region? */
               if(remap->src_addr >= channel_regions->base_addr &&
                  (remap->src_addr + remap->size) <= 
                  (channel_regions->base_addr + channel_regions->size))
               {
                  /* Save the matched region index, so we can refer to it later, if necessary. */
                  src_regions[channel] = region;
                  found = true;
                  break;
               }
            }
         }
         if(!found)
         {
            break;
         }

         /* Save to the index specified. */
         req_remaps[index] = *remap;
      }

      /* Generate remap list sorted to avoid overlap during application. */
      memcpy(sorted_remaps, req_remaps, sizeof(sorted_remaps));
      for( uint32 i = 0; i < MAX_REGIONS - 1; i++ )
      {
         for( uint32 j = 0; j < MAX_REGIONS - (i + 1); j++ )
         {
            /* Swap if the current entry will overlap with something after it
             * or if it's an empty entry. */
            if( (sorted_remaps[i].src_addr == sorted_remaps[i].dest_addr || sorted_remaps[i].size == 0) ||
                will_overlap( &sorted_remaps[j], sorted_remaps, j+1 ) )
            {
               icbcfg_remap_info_type temp = sorted_remaps[j+1];
               sorted_remaps[j+1] = sorted_remaps[j];
               sorted_remaps[j] = temp;
            }
         }
      }
   
      /* Go through our list of remaps and apply them to the system memory map. */
      for(i = 0; i < MAX_REGIONS; i++)
      {
         icbcfg_remap_info_type *req_remap = &sorted_remaps[i];

         /* Skip any remap without anything to do. */
         if(req_remap->src_addr == req_remap->dest_addr)
         {
            continue;
         }

         /* Check if the system memory map will support the
          * destination address for the remapped region. */
         found = false;
         for(channel = 0; channel < dev_config->num_channels && !found; channel++)
         {
            /* Only check the regions in this channel if it also has a matching
             * region in the DDR setup. */
            if(src_regions[channel] >= MAX_REGIONS)
            {
               continue;
            }

            for(region = 0; region < dev_config->num_segments; region++)
            {
               region_type *channel_regions = &ddr_regions.channels[channel].regions[region];
   
               /* Are we contained by this region? */
               if(req_remap->dest_addr >= channel_regions->base_addr &&
                  (req_remap->dest_addr + req_remap->size) <= 
                  (channel_regions->base_addr + channel_regions->size) )
               {
                  found = true;
                  break;
               }
            }
         }
   
         /* If we don't find an existing region, we must adjust the system memory map. */
         if(!found)
         {
            adjust_memory_map(dev_config, req_remap, &ddr_regions);
         }
      }

      /* Generate a remap list sorted by dest addr. */
      for( uint32 i = 0; i < MAX_REGIONS - 1; i++ )
      {
         for( uint32 j = 0; j < MAX_REGIONS - (i + 1); j++ )
         {
            if( ((sorted_remaps[j].dest_addr > sorted_remaps[j+1].dest_addr) &&
                 (sorted_remaps[j+1].dest_addr != sorted_remaps[j+1].src_addr)) ||
                (sorted_remaps[j].dest_addr == sorted_remaps[j].src_addr) )
            {
               icbcfg_remap_info_type temp = sorted_remaps[j+1];
               sorted_remaps[j+1] = sorted_remaps[j];
               sorted_remaps[j] = temp;
            }
         }
      }
   
   
      /* Rewrite the memory map and update remapped segments. */
      compress_regions(dev_config, true, &ddr_regions);
      map_segments(dev_config, &ddr_regions);

      ret_val = ICBCFG_SUCCESS;
   } while(false);

   return ret_val;
}

//*============================================================================*/
/**
@brief 
      Configures memory address remapper. 

@param[in] dev  DAL string device ID.
@param[in] info The information about the DDR configuration
@param[in] remap The information about the region to remap

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
   return ICB_RemapEx(dev, info, 0, remap);
}

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
icbcfg_error_type ICB_UnmapTCM( const char * dev )
{
   (void)dev;
   return ICBCFG_ERROR_UNSUPPORTED;
}

/* vim: set ts=3 sts=3 sw=3 et :*/ 
