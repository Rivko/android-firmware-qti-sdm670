/*==============================================================================

FILE:      icbcfg_query_data.c

DESCRIPTION: This file implements the ICB Configuration driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/core.tz/2.0.2/settings/systemdrivers/icb/config/670/icbcfg_query_data.c#2 $ 
$DateTime: 2018/07/12 23:29:32 $
$Author: pwbldsvc $
$Change: 16634472 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/01/31  sds    Updated to integrated format
2016/09/19  sds    Created
 
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfgi.h"
#include "icbcfg_query_target.h"
#include "HALhwio.h"

/*---------------------------------------------------------------------------*/
/*          Macro and constant definitions                                   */
/*---------------------------------------------------------------------------*/
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "icbcfg/boot"                   */
/*---------------------------------------------------------------------------*/

/* ICBcfg Boot Configuration Data*/


/*---------------------------------------------------------------------------*/
/* SDM670                                                                  */
/*---------------------------------------------------------------------------*/

icbcfg_mem_region_type map_ddr_regions_sdm670[] =
{
  { 0x080000000ULL, 0x400000000ULL },
};

/* Translation block base addresses. */
uint8 *trans_bases_sdm670[] = 
{
  (uint8 *)HWIO_ADDR(LLCC0_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  (uint8 *)HWIO_ADDR(LLCC1_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
};

icbcfg_device_config_type sdm670 =
{
  /* Chip version information for this device data. */
  CHIPINFO_FAMILY_SDM670,   /**< Chip family */
  false,                       /**< Exact match for version? */
  CHIPINFO_VERSION(0,0),       /**< Chip version */

  /* Device information. */
  ARRAY_SIZE(map_ddr_regions_sdm670), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm670,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  2,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_LLCC,                /**< Address translator hardware type. */
  trans_bases_sdm670,                 /**< Translation block base addresses. */

  /* Prop lists. */
  NULL,           /**< Init time prop list. */
  NULL,           /**< Post-init time prop list. */
};

/* Red Dragon device config data */
icbcfg_device_config_type qcs605 =
{
  /* Chip version information for this device data. */
  CHIPINFO_FAMILY_QCS605,   /**< Chip family */
  false,                       /**< Exact match for version? */
  CHIPINFO_VERSION(0,0),       /**< Chip version */

  /* Device information. */
  ARRAY_SIZE(map_ddr_regions_sdm670), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm670,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  2,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_LLCC,                /**< Address translator hardware type. */
  trans_bases_sdm670,                 /**< Translation block base addresses. */

  /* Prop lists. */
  NULL,           /**< Init time prop list. */
  NULL,           /**< Post-init time prop list. */
};

/*kingsman config file */
icbcfg_device_config_type sxr1130 =
{
  /* Chip version information for this device data. */
  CHIPINFO_FAMILY_SXR1130,   /**< Chip family */
  false,                       /**< Exact match for version? */
  CHIPINFO_VERSION(0,0),       /**< Chip version */

  /* Device information. */
  ARRAY_SIZE(map_ddr_regions_sdm670), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm670,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  2,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_LLCC,                /**< Address translator hardware type. */
  trans_bases_sdm670,                 /**< Translation block base addresses. */

  /* Prop lists. */
  NULL,           /**< Init time prop list. */
  NULL,           /**< Post-init time prop list. */
};
/* Definitions list */
icbcfg_device_config_type *configs_sdm670[] =
{
  &sdm670,
  &qcs605,
  &sxr1130,
};

/* Exported target definitions */
icbcfg_info_type icbcfg_info =
{
  ARRAY_SIZE(configs_sdm670),
  configs_sdm670,
};
