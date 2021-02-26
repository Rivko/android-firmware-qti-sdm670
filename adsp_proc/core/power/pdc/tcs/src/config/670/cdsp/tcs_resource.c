/*============================================================================
  FILE:         tcs_resource.c
  
  OVERVIEW:     This file implements the target specific TCS resources
 
  DEPENDENCIES: None

                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/tcs/src/config/670/cdsp/tcs_resource.c#3 $
$DateTime: 2018/02/07 12:09:29 $
============================================================================*/
#include "pdcTcsCfg.h"
#include "tcs_resource.h"
#include "pwr_utils_lvl.h"

/*===========================================================================
 *                            TYPES AND DEFINITIONS
 *===========================================================================*/
#define TCS_RESOURCE_CX   "cx.lvl"
#define TCS_RESOURCE_MX   "mx.lvl"
#define TCS_RESOURCE_XO   "xo.lvl"


/* Resource vlvl levels */
#define RES_CX_RET  RAIL_VOLTAGE_LEVEL_RET      /* HLVL: 1 */
#define RES_CX_OFF  RAIL_VOLTAGE_LEVEL_OFF      /* HLVL: 0 */
#define RES_CX_MOL  RAIL_VOLTAGE_LEVEL_MIN_SVS  /* HLVL: 2 */

#define RES_MX_RET  RAIL_VOLTAGE_LEVEL_RET      /* HLVL: 1 */
#define RES_MX_MOL  RAIL_VOLTAGE_LEVEL_NOM      /* HLVL: 4 */

#define RES_XO_OFF  XO_LEVEL_PMIC_BUFFER_OFF    /* HLVL: 1 */
#define RES_XO_MOL  XO_LEVEL_ON                 /* HLVL: 3 */

/*===========================================================================
 *                            INTERNAL VARIABLES
 *===========================================================================*/
/* Resources used in this target */
pdc_tcs_resource g_pdcResourceList[TCS_TOTAL_RESOURCE_NUM] = 
{
  {TCS_RESOURCE_CX},
  {TCS_RESOURCE_MX},
  {TCS_RESOURCE_XO}
};

/* TCS configuraiton */
pdc_tcs_config g_pdcTCSConfig[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS] =
{
  /* Sleep TCS 0 */
  {
    /* Res Index        Resource data     Options                     Addr offset */
    {{RES_IDX_CX},      {RES_CX_RET,      TCS_CFG_OPT_NONE,           0}},    /* Cmd 0 */
    {{RES_IDX_MX},      {RES_MX_RET,      TCS_CFG_OPT_NONE,           0}},    /* Cmd 1 */
    {{RES_IDX_XO},      {RES_XO_OFF,      TCS_CFG_OPT_NONE,           0}},    /* Cmd 2 */ 
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},    /* Cmd 3 */   
  },

  /* Sleep TCS 1 */
  {
    /* Res Index        Resource data     Options                     Addr offset */
    {{RES_IDX_CX},      {RES_CX_OFF,      TCS_CFG_OPT_NONE,           0}},    /* Cmd 0 */
    {{RES_IDX_MX},      {RES_MX_RET,      TCS_CFG_OPT_NONE,           0}},    /* Cmd 1 */
    {{RES_IDX_XO},      {RES_XO_OFF,      TCS_CFG_OPT_NONE,           0}},    /* Cmd 2 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},    /* Cmd 3 */
  },

  /* Wake TCS 2 */
  {
    /* Res Index        Resource data     Options                     Addr offset */
    {{RES_IDX_XO},      {RES_XO_MOL,      TCS_CFG_OPT_CMD_RESP_REQ,   0}},    /* Cmd 0 */
    {{RES_IDX_MX},      {RES_MX_MOL,      TCS_CFG_OPT_CMD_RESP_REQ,   0}},    /* Cmd 1 */
    {{RES_IDX_CX},      {RES_CX_MOL,      TCS_CFG_OPT_CMD_RESP_REQ,   0}},    /* Cmd 2 */
    {{0},               {0,               TCS_CFG_OPT_NOT_USED,       0}},    /* Cmd 3 */
  }
};

