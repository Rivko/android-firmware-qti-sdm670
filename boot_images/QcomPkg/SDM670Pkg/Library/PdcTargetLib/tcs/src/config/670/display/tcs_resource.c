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
$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PdcTargetLib/tcs/src/config/670/display/tcs_resource.c#1 $
$DateTime: 2017/09/20 22:09:48 $
============================================================================*/
#include "pdcTcsCfg.h"
#include "tcs_resource.h"

/*===========================================================================
 *                            TYPES AND DEFINITIONS
 *===========================================================================*/
#define TCS_RESOURCE_CX   "cx.lvl"
#define TCS_RESOURCE_MX   "mx.lvl"
#define TCS_RESOURCE_XO   "xo.lvl"
#define TCS_RESOURCE_SOC  "vrm.soc"

/* Resource levels hardcoded until such time they are available at runtime */
#define RES_CX_OFF  0
#define RES_CX_RET  1
#define RES_CX_MOL  2 //SVS3 (min_svs) 

#define RES_MX_RET  1
#define RES_MX_MOL  3 //NOM

#define RES_XO_OFF  0
#define RES_XO_MOL  3 //ON

#define VRM_SOC_OFF 0
#define VRM_SOC_ON  1

/*===========================================================================
 *                            INTERNAL VARIABLES
 *===========================================================================*/
/* Resources used in this target */
pdc_tcs_resource g_pdcResourceList[TCS_TOTAL_RESOURCE_NUM] =
{
  {TCS_RESOURCE_CX},
  {TCS_RESOURCE_MX},
  {TCS_RESOURCE_XO},
  {TCS_RESOURCE_SOC, 0x4}    /* Enable vote is at offset of 4 */
};

/* TCS configuraiton */
pdc_tcs_config g_pdcTCSConfig[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS] =
{
  /* Sleep 1 TCS (TCS 0) */
  {
    /* Res Index      Resource data     Options                     Addr offset */
    {{RES_IDX_XO},      {RES_XO_OFF,      TCS_CFG_OPT_NONE,           0}},   /* Cmd 0 */
    {{RES_IDX_CX},      {RES_CX_RET,      TCS_CFG_OPT_NONE,           0}},   /* Cmd 1 */
    {{RES_IDX_MX},      {RES_MX_RET,      TCS_CFG_OPT_NONE,           0}},   /* Cmd 2 */
    {{RES_IDX_SOC},     {VRM_SOC_OFF,     TCS_CFG_OPT_NONE,           0}},   /* Cmd 3 */
  },

  /* Sleep 2 TCS (TCS 1) */
  {
    /* Res Index      Resource data     Options                     Addr offset */
    {{RES_IDX_XO},      {RES_XO_OFF,      TCS_CFG_OPT_NONE,           0}},   /* Cmd 0 */
    {{RES_IDX_CX},      {RES_CX_OFF,      TCS_CFG_OPT_NONE,           0}},   /* Cmd 1 */
    {{RES_IDX_MX},      {RES_MX_RET,      TCS_CFG_OPT_NONE,           0}},   /* Cmd 2 */
    {{RES_IDX_SOC},     {VRM_SOC_OFF,     TCS_CFG_OPT_NONE,           0}},   /* Cmd 3 */
  },

  /* Wake TCS (TCS 2) */
  {
    /* Res Index      Resource data     Options                     Addr offset */
    {{RES_IDX_MX},      {RES_MX_MOL,      TCS_CFG_OPT_CMD_RESP_REQ,   0}},   /* Cmd 0 */
    {{RES_IDX_CX},      {RES_CX_MOL,      TCS_CFG_OPT_CMD_RESP_REQ,   0}},   /* Cmd 1 */
    {{RES_IDX_XO},      {RES_XO_MOL,      TCS_CFG_OPT_CMD_RESP_REQ,   0}},   /* Cmd 2 */
    {{RES_IDX_SOC},     {VRM_SOC_ON,      TCS_CFG_OPT_CMD_RESP_REQ,   0}},   /* Cmd 3 */
  }
};

