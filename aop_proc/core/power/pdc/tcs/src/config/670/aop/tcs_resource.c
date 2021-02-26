/*============================================================================
  FILE:         tcs_resource.c
  
  OVERVIEW:     This file implements the target specific TCS resources
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/pdc/tcs/src/config/670/aop/tcs_resource.c#1 $
$DateTime: 2018/02/16 04:48:18 $
============================================================================*/
#include "pdcTcsCfg.h"
#include "tcs_resource.h"
#include "pm.h"

/*===========================================================================
 *                            TYPES AND DEFINITIONS
 *===========================================================================*/
#define TCS_RESOURCE_SMPA3  "smpa3"
#define TCS_RESOURCE_SMPA4  "smpa4"
#define TCS_RESOURCE_SMPA5  "smpa5"
#define TCS_RESOURCE_BOBB1  "bobb1"
#define TCS_RESOURCE_PBS   "pbs"


/* Resource levels hardcoded until such time they are available at runtime */
#define RES_PBS_OFF   0
#define RES_PBS_ON1   1
#define RES_PBS_ON2   2

#define ENABLE        4
#define MODE          8

#define RET_MODE_VAL  0x4
#define AUTO_MODE_VAL 0x6
#define LPM_MODE_VAL  0x5

#define BOB_MODE_PASS_VAL 0
#define BOB_MODE_AUTO_VAL 2

/*===========================================================================
 *                            INTERNAL VARIABLES
 *===========================================================================*/
/* Resources used in this target */
pdc_tcs_resource g_pdcResourceList[TCS_TOTAL_RESOURCE_NUM] = 
{
  {TCS_RESOURCE_SMPA3},
  {TCS_RESOURCE_SMPA4},
  {TCS_RESOURCE_SMPA5},
  {TCS_RESOURCE_BOBB1},
  {TCS_RESOURCE_PBS}
};

/* TCS configuraiton */
pdc_tcs_config g_pdcTCSConfig[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS] =
{
  /* Sleep TCS 0 */
  {
    /* Res Index      Resource data     Options                     Addr offset */
    {RES_IDX_SMPA4,    {RET_MODE_VAL,    TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 0 */
    {RES_IDX_SMPA3,    {LPM_MODE_VAL,    TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 1 */
    {RES_IDX_SMPA5,    {LPM_MODE_VAL,    TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 2 */
    {RES_IDX_BOBB1,    {BOB_MODE_PASS_VAL,TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 3 */
  },

  /* Sleep TCS 1 */
  {
    {RES_IDX_PBS,     {RES_PBS_ON1,     TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 1 */
    {RES_IDX_PBS,     {RES_PBS_ON1,     TCS_CFG_OPT_CMD_RESP_REQ,   ENABLE}}, /* Cmd 0 */
    {RES_IDX_PBS,     {RES_PBS_OFF,     TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 3 */
    {RES_IDX_PBS,     {RES_PBS_OFF,     TCS_CFG_OPT_CMD_RESP_REQ,   ENABLE}}, /* Cmd 2 */
  },

  /* Wake TCS 2 */
  {
    /* Res Index      Resource data     Options                     Addr offset */
    {RES_IDX_PBS,     {RES_PBS_ON2,     TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 0 */
    {RES_IDX_PBS,     {RES_PBS_ON1,     TCS_CFG_OPT_CMD_RESP_REQ,   ENABLE}}, /* Cmd 2 */
    {RES_IDX_PBS,     {RES_PBS_OFF,     TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 0 */
    {RES_IDX_PBS,     {RES_PBS_OFF,     TCS_CFG_OPT_CMD_RESP_REQ,   ENABLE}}, /* Cmd 3 */
  },

  /* Wake TCS 3 */
  {
    /* Res Index      Resource data     Options                     Addr offset */
    {RES_IDX_SMPA4,    {AUTO_MODE_VAL,    TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 0 */
    {RES_IDX_SMPA3,    {AUTO_MODE_VAL,    TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 1 */
    {RES_IDX_SMPA5,    {AUTO_MODE_VAL,    TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 2 */
    {RES_IDX_BOBB1,    {BOB_MODE_AUTO_VAL,TCS_CFG_OPT_CMD_RESP_REQ,   MODE}},   /* Cmd 3 */
  }
};

