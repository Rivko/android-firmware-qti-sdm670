#ifndef __PWR_UTILS_LVL_H__
#define __PWR_UTILS_LVL_H__
/*===========================================================================
                        pwr_utils_lvl.h

DESCRIPTION:
  This file provides APIs to convert SW virtual corner/level (vlvl) to HW 
  corner/level (hlvl) and vice versa.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include "voltage_level.h"

//---------------------------------------------------------------------------
//  Type Definitions
//---------------------------------------------------------------------------
/* Enum of all SW corners. These values are expected to remain same through
 * out the lifetime of the chip */

/* Voltage rail SW conrners are already defined in pre-existing public 
 * header file - voltage_level.h. So skipping it */

/* Enum of all DDR resource levels/corners */
enum
{
  DDRSS_LEVEL_OFF = 0x0,               /* D3 + SHRM Gated + SC flush */
  DDRSS_LEVEL_PHY_COLLAPSED = 0x20,    /* D3 PHY Collapsed */
  DDRSS_LEVEL_PLL_DISABLED = 0x80,     /* PLL Disabled */
  DDRSS_LEVEL_ACTIVE = 0xA0,
  DDRSS_LEVEL_MAX = DDRSS_LEVEL_ACTIVE,
  DDRSS_LEVEL_INVALID = -1,
  DDRSS_LEVEL_NUM_LEVELS = 4
};

/* Enum for XO resource levels/corners */
enum
{
  XO_LEVEL_CRYSTAL_OFF = 0x0,
  XO_LEVEL_PMIC_BUFFER_OFF = 0x20,
  XO_LEVEL_SOC_BUFFER_OFF = 0x50,
  XO_LEVEL_ON = 0x80,
  XO_LEVEL_MAX = XO_LEVEL_ON,
  XO_LEVEL_INVALID = -1,
  XO_LEVEL_NUM_LEVELS = 4
};


//===========================================================================
//                       Function Declarations
//===========================================================================

/**
 * <!-- pwr_utils_lvl_init -->
 *
 * @brief This function should be called during subsystem initialization time
 *        as it sets up resources and supported levels on a given target.
 */
void pwr_utils_lvl_init (void);

/**
 * <!-- pwr_utils_lvl_resource_idx -->
 *
 * @brief Returns the resource index corresponding to input name that
 *        can be used for subsequently for querying hlvls and vlvls.
 *
 * @note This resource index is for power utils lvl functions only. It 
 *       has no relevance outside of it.
 *
 * @param res_name: Resource name e.g. cx.lvl, mx.lvl, etc
 * 
 * @return Resource index (>=0) if successful else -1.
 */
int pwr_utils_lvl_resource_idx (const char *res_name);

/**
 * <!-- pwr_utils_resource_lvls_count -->
 *
 * @brief This function returns the number of HLVLs supported by the input
 *        resource.
 *
 * @see pwr_utils_lvl_resource_idx
 *
 * @param resource_idx: Resource index for which we are querying number of
 *                      supported hlvls.
 *
 * @return Number of hlvls (>=0) if successful else -1.
 */
int pwr_utils_resource_lvls_count (int resource_idx);

/**
 * <!-- pwr_utils_named_resource_lvls_count -->
 *
 * @brief This function return the number of HLVLs supported by the input
 *        resource.
 *
 * @see pwr_utils_lvl_resource_idx
 *
 * @note Unlike @pwr_utils_resource_lvls_count, this function has to perform 
 *       some extra internal look up each time to obtain HLVL count. So if
 *       client expects to query same resource multiple times, he should 
 *       consider using pwr_utils_resource_lvls_count instead for improved 
 *       efficiency.
 *
 * @param resource: Resource name for which we are querying hlvl count e.g. 
 *                  cx.lvl, mx.lvl.
 *
 * @return Number of hlvls (>=0) if successful else -1.
 */
int pwr_utils_named_resource_lvls_count (const char *resource);

/**
 * <!-- pwr_utils_hlvl -->
 *
 * @brief This function returns the HW corner (hlvl) that corresponds to 
 *        input SW corner (vlvl) and resource id.
 *
 * @see pwr_utils_lvl_resource_idx
 *
 * @param resource_idx: Resoruce index for which we are querying hlvl usually
 *                      obtained by pwr_utils_lvl_resource_idx.
 * @param vlvl: SW corner level we want to map to hlvl.
 * @param mapped_vlvl: If not NULL and call is successful, it will contain the 
 *                     vlvl used for returning hlvl.
 *                     = vlvl if exact vlvl is supported by the resource.
 *                     > vlvl if round up happened.
 *                     = RAIL_VOLTAGE_LEVEL_OVERLIMIT when 
 *                       vlvl > max_supported_vlvl (error condition)
 *
 * @return HW corner level (>=0) if successful else -1.
 */
int pwr_utils_hlvl (int resource_idx, int vlvl, int *mapped_vlvl);

/**
 * <!-- pwr_utils_hlvl_named_resource -->
 *
 * @brief This function returns the HW corner (hlvl) that corresponds to
 *        input SW corner (vlvl) and resource.
 *
 * @note Unlike @pwr_utils_hlvl, this function has to perform some extra
 *       internal look up each time to obtain HW corner. So if client
 *       expects to query same resource multiple times, he should consider
 *       using pwr_utils_hlvl instead for improved efficiency.
 *
 * @param resource: Resource name for which we are querying hlvl e.g. 
 *                  cx.lvl, mx.lvl.
 * @param vlvl: SW corner we want to map to hlvl.
 * @param mapped_vlvl: If not NULL and call is successful, it will contain the 
 *                     vlvl used for returning hlvl.
 *                     = vlvl if exact vlvl is supported by the resource.
 *                     > vlvl if round up happened.
 *                     = RAIL_VOLTAGE_LEVEL_OVERLIMIT when 
 *                       vlvl > max_supported_vlvl (error condition)
 *
 * @return HW corner level (>=0) if successful else -1.
 */
int pwr_utils_hlvl_named_resource 
(
  const char *resource, 
  int vlvl, 
  int *mapped_vlvl
);

/**
 * <!-- pwr_utils_vlvl -->
 *
 * @brief This function returns the SW corner (vlvl) corresponding to 
 *        input HW corner (hlvl) and rail id.
 *
 * @see pwr_utils_lvl_resource_idx
 *
 * @param resource_idx: Resource index for which we are querying vlvl,
 *                      usually obtained by pwr_utils_lvl_resource_idx.
 * @param hlvl: HW corner which we want to map to SW corner (vlvl).
 *
 * @return Valid SW corner if successful else < 0
 */
int pwr_utils_vlvl (int resource_idx, int hlvl);

/**
 * <!-- pwr_utils_vlvl_named_resource -->
 *
 * @brief This function returns the SW corner (vlvl) for the input resource
 *        name and HW corner (hlvl).
 *
 * @note Unlike @pwr_utils_vlvl, this function has to perform some extra
 *       internal look up each time to obtain SW corner. So if client
 *       expects to query for same resource multiple times, he should consider
 *       using pwr_utils_vlvl instead for improved efficiency.
 *
 * @param resource: Resource name for which we are querying vlvl e.g. 
 *                  cx.lvl, mx.lvl.
 * @param hlvl: HW corner which we want to map to SW corner (vlvl).
 *
 * @return Valid SW corner if successful else < 0
 */
int pwr_utils_vlvl_named_resource (const char *resource, int hlvl);

#endif
