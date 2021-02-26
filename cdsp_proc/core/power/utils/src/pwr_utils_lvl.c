/*===========================================================================
                             pwr_utils_lvl.c

DESCRIPTION:
  This file implements the APIs to conver SW corner/levels (vlvl) to HW
  corner/level (hlvl) and vice versa.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "pwr_utils_lvl.h"
#include "cmd_db.h"
#include "CoreVerify.h"
#include <string.h>
#include <stdlib.h>


//---------------------------------------------------------------------------
//  Internal Type Definitions
//---------------------------------------------------------------------------
/* Internal structure type containing supported level mapping for different
 * rail resources. */
typedef struct pwr_utils_lvl_res_s
{
  const char *name;          /* resource name e.g. cx.lvl, mx.lvl, etc */
  uint16 *vlvls;             /* Supported vlvls with hlvl denoted by index */
  size_t count;              /* vlvl count */
} pwr_utils_lvl_res_t;


//---------------------------------------------------------------------------
//  Variables
//---------------------------------------------------------------------------
/* List of rail resources - At init time it will be set to point appropriate
 * list based on target */
static pwr_utils_lvl_res_t *g_res = NULL;
static size_t g_res_count = 0;

/* Target specific rail resources 
 *
 * Typically we keep this information in dedicated target file or devcfg 
 * BUT power utils functionalitiies are supposed to be lighter in terms of
 * code footprint, so keep it here. 
 *
 * Also, if multiple target share same set of rail resouces, they can share
 * a common list.
 * */
static pwr_utils_lvl_res_t res_sd845[] = 
{
  {"cx.lvl",  NULL, 0},
  {"mx.lvl",  NULL, 0},
  {"ebi.lvl", NULL, 0},
  {"lcx.lvl", NULL, 0},
  {"lmx.lvl", NULL, 0},
  {"gfx.lvl", NULL, 0},
  {"mss.lvl", NULL, 0},
  {"ddr.lvl", NULL, 0},
  {"xo.lvl",  NULL, 0}
};


//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
//  Public Function Definition
//---------------------------------------------------------------------------

/**
 * <!-- pwr_utils_lvl_init -->
 *
 * @brief This function should be called during subsystem initialization time
 *        as it sets up rails and supported corner for a given target.
 */
void pwr_utils_lvl_init (void)
{
  uint32 i, j;
  pwr_utils_lvl_res_t *res;
  const char *res_name;
  uint32 data_len;
  uint8 aux_len;

  /* When we get chip id APIs, set pointer using those */
  g_res = res_sd845;
  g_res_count = sizeof (res_sd845)/sizeof (res_sd845[0]);

  for (i = 0; i < g_res_count; i++)
  {
    res = &g_res[i];
    res_name = res->name;
    data_len = cmd_db_query_len (res_name);
    aux_len = (uint8)data_len;

    if (data_len == 0)
    {
      /* nothing to do - vlvls already set to NULL with count 0 at 
       * declaraion */
      continue;
    }

    res->vlvls = (uint16 *)malloc (data_len);
    CORE_VERIFY (res->vlvls != NULL);

    cmd_db_query_aux_data (res_name, &aux_len, (uint8 *)res->vlvls);
    CORE_VERIFY (data_len == (uint32)aux_len);

    /* Calculating actual levels this resource supports */
    for (j = 0; j < (data_len / sizeof(uint16)); j++)
    {
      if ((res->vlvls[j] == 0) && (j != 0))
      {
        break;
      }
    }
    res->count = j;
  }
}

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
int pwr_utils_lvl_resource_idx (const char *res_name)
{
  uint32 i;

  if ((res_name == NULL) ||(g_res == NULL) || (g_res_count == 0))
  {
    return -1;
  }

  for (i = 0; i < g_res_count; i++)
  {
    if (strcmp(g_res[i].name, res_name) == 0)
    {
      return i;
    }
  }

  return -1;       /* Resource not found */
}

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
int pwr_utils_hlvl (int resource_idx, int vlvl, int *mapped_vlvl)
{
  uint32 i;
  int temp;
  pwr_utils_lvl_res_t *res;
  mapped_vlvl = mapped_vlvl == NULL ? &temp : mapped_vlvl;

  if ((size_t)resource_idx >= g_res_count || (vlvl < 0))
  {
    *mapped_vlvl = RAIL_VOLTAGE_LEVEL_INVALID;
    return -1;
  }

  res = &g_res[resource_idx];

  for (i = 0; i < res->count; i++)
  {
    if (res->vlvls[i] >= vlvl)
    {
      *mapped_vlvl = res->vlvls[i];
      return i;
    }
  }

  /* Requested vlvl is greater than max supported vlvl for this resource */
  *mapped_vlvl = RAIL_VOLTAGE_LEVEL_OVERLIMIT;
  return -1;
}

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
)
{
  int res_idx = pwr_utils_lvl_resource_idx (resource);
  
  /* pwr_utils_hlvl validates res_idx - so no need here */
  return pwr_utils_hlvl (res_idx, vlvl, mapped_vlvl);
}

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
int pwr_utils_resource_lvls_count (int resource_idx)
{
  if ((size_t)resource_idx >= g_res_count)
  {
    return -1;
  }
  else
  {
    return g_res[resource_idx].count;
  }
}

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
int pwr_utils_named_resource_lvls_count (const char *resource)
{
  return pwr_utils_resource_lvls_count (pwr_utils_lvl_resource_idx (resource));
}

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
int pwr_utils_vlvl (int resource_idx, int hlvl)
{
  pwr_utils_lvl_res_t *res;

  if ((size_t)resource_idx >= g_res_count)
  {
    return RAIL_VOLTAGE_LEVEL_INVALID;
  }

  res = &g_res[resource_idx];

  return ((size_t)hlvl < res->count ? 
          res->vlvls[hlvl] : RAIL_VOLTAGE_LEVEL_INVALID);
}

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
int pwr_utils_vlvl_named_resource (const char *resource, int hlvl)
{
  int res_idx = pwr_utils_lvl_resource_idx (resource);
  return pwr_utils_vlvl (res_idx, hlvl);
}

