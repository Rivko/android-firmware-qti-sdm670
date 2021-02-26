/*============================================================================
  FILE:         tcs_resource.c
  
  OVERVIEW:     This file implements the target specific TCS resources
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/tcs/src/config/670/adsp/tcs_resource.c#3 $
$DateTime: 2017/11/05 22:07:10 $
============================================================================*/
#include "pdcTcsCfg.h"
#include "tcs_resource.h"
#include "pwr_utils_lvl.h"

/*===========================================================================
 *                            TYPES AND DEFINITIONS
 *===========================================================================*/
#define TCS_RESOURCE_SSC_CX   "lcx.lvl"

/* Resource levels hardcoded until such time they are available at runtime */
#define RES_SSC_CX_RET  RAIL_VOLTAGE_LEVEL_RET      /* HLVL: 1 */
#define RES_SSC_CX_MOL  RAIL_VOLTAGE_LEVEL_MIN_SVS  /* HLVL: 2 */

#define RES_CX_OFF  RAIL_VOLTAGE_LEVEL_OFF      /* HLVL: 0 */
#define RES_CX_MOL  RAIL_VOLTAGE_LEVEL_MIN_SVS  /* HLVL: 2 */

/*===========================================================================
 *                            INTERNAL VARIABLES
 *===========================================================================*/
/* Resources used in this target */
pdc_tcs_resource g_pdcResourceList[TCS_TOTAL_RESOURCE_NUM] = 
{
  {TCS_RESOURCE_SSC_CX}
};

/* TCS configuraiton */
pdc_tcs_config g_pdcTCSConfig[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS] =
{
  /* Sleep TCS 0 */
  {
    /* Res Index        Resource data     Options                     Addr offset */
    {{RES_IDX_SSC_CX},  {RES_SSC_CX_RET,  TCS_CFG_OPT_NONE,           0}},   /* Cmd 0 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 1 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 2 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 3 */
  },

  /* Sleep TCS 1 */
  {
    /* Res Index        Resource data     Options                     Addr offset */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 0 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 1 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 2 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 3 */
  },

  /* Wake TCS 2 */
  {
    /* Res Index        Resource data     Options                     Addr offset */
    {{RES_IDX_SSC_CX},  {RES_SSC_CX_MOL,  TCS_CFG_OPT_CMD_RESP_REQ,   0}},   /* Cmd 0 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 1 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 2 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},   /* Cmd 3 */
  }
};

