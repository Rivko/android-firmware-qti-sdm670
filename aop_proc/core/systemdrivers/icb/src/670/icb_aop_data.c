/*===========================================================================

FILE:        icb_aop_data.c

DESCRIPTION: This file describes the target data for ICB on AOP.

   Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
             All Rights Reserved
             QUALCOMM Proprietary/GTDR
-------------------------------------------------------------------------------

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/icb/src/670/icb_aop_data.c#1 $ 

                      EDIT HISTORY FOR FILE

 when         who   what, where, why
 ----------   ---   -----------------------------------------------------------
 2017/06/05   sds   SNOC/MMNOC corner matching
 2016/09/20   sds   Initial version

=============================================================================*/
#include "icb_aopi.h"

/****************************************************************************/
/*                             DEFINITIONS                                  */
/****************************************************************************/
#define SNOC_VCD 3
#define MMNOC_VCD 4
#define CNOC_VCD 5

/****************************************************************************/
/*                          DATA DECLARATIONS                               */
/****************************************************************************/
/****************************************************************************/
/*                                  BCMs                                    */
/****************************************************************************/
icb_bcm_t bcm_cn0 =
{
  .name    = "CN0",
  .vote    = BCM_VOTETABLE_VOTE_VALID_BMSK | 0x1,
  .ref_cnt = 0,
};

icb_bcm_t bcm_sn10 =
{
  .name    = "SN10",
  .vote    = BCM_VOTETABLE_VOTE_VALID_BMSK | 0x1,
  .ref_cnt = 0,
};

icb_bcm_t bcm_sn0 =
{
  .name    = "SN0",
  .vote    = BCM_VOTETABLE_VOTE_VALID_BMSK | 0x1,
  .ref_cnt = 0,
};

icb_bcm_t bcm_sh0 =
{
  .name    = "SH0",
  .vote    = BCM_VOTETABLE_VOTE_VALID_BMSK | 0x1,
  .ref_cnt = 0,
};

icb_bcm_t bcm_mc0 =
{
  .name    = "MC0",
  .vote    = BCM_VOTETABLE_VOTE_VALID_BMSK | 0x1,
  .ref_cnt = 0,
};

icb_bcm_t bcm_mm1 =
{
  .name    = "MM1",
  .vote    = BCM_VOTETABLE_VOTE_VALID_BMSK | 0x1,
  .ref_cnt = 0,
};

/* BCM list */
static icb_bcm_t *bcms[] =
{
  &bcm_cn0,
  &bcm_sn10,
  &bcm_sn0,
  &bcm_sh0,
  &bcm_mc0,
  &bcm_mm1,
};

/****************************************************************************/
/*                                ROUTES                                    */
/****************************************************************************/
/* AOSS to DCC */
static icb_bcm_t *route_aoss_dcc_cfg_bcms[] =
{
  &bcm_cn0,
};

static icb_route_t route_aoss_dcc_cfg =
{
  .master   = ICBID_MASTER_AOSS,
  .slave    = ICBID_SLAVE_DCC_CFG,
  .num_bcms = ARRAY_SIZE(route_aoss_dcc_cfg_bcms),
  .bcms     = route_aoss_dcc_cfg_bcms,
};

/* AOSS to EBI */
static icb_bcm_t *route_aoss_ebi_bcms[] =
{
  &bcm_cn0,
  &bcm_sn10,
  &bcm_sn0,
  &bcm_sh0,
  &bcm_mc0,
};

static icb_route_t route_aoss_ebi =
{
  .master   = ICBID_MASTER_AOSS,
  .slave    = ICBID_SLAVE_EBI1,
  .num_bcms = ARRAY_SIZE(route_aoss_ebi_bcms),
  .bcms     = route_aoss_ebi_bcms,
};

/* AOSS to MEMNOC SF */
static icb_bcm_t *route_aoss_memnoc_sf_bcms[] =
{
  &bcm_cn0,
  &bcm_sn10,
  &bcm_sn0,
};

static icb_route_t route_aoss_memnoc_sf =
{
  .master   = ICBID_MASTER_AOSS,
  .slave    = ICBID_SLAVE_SNOC_MEM_NOC_SF,
  .num_bcms = ARRAY_SIZE(route_aoss_memnoc_sf_bcms),
  .bcms     = route_aoss_memnoc_sf_bcms,
};

/* MDP to MEMNOC HF */
static icb_bcm_t *route_mdp0_mem_noc_hf_bcms[] =
{
  &bcm_mm1,
};

static icb_route_t route_mdp0_mem_noc_hf =
{
  .master   = ICBID_MASTER_MDP0,
  .slave    = ICBID_SLAVE_MNOC_HF_MEM_NOC,
  .num_bcms = ARRAY_SIZE(route_mdp0_mem_noc_hf_bcms),
  .bcms     = route_mdp0_mem_noc_hf_bcms,
};

/* Route list */
static icb_route_t *routes[] =
{
  &route_aoss_dcc_cfg,
  &route_aoss_ebi,
  &route_aoss_memnoc_sf,
  &route_mdp0_mem_noc_hf,
};

/****************************************************************************/
/*                          CORNER MATCHED VCDS                             */
/****************************************************************************/
static icb_match_vcd_t match_vcds[] =
{
  [0] =
  {
    .vcd = SNOC_VCD,
  },
  [1] =
  {
    .vcd = MMNOC_VCD,
  },
  [2] =
  {
    .vcd = CNOC_VCD,
  },
};

/****************************************************************************/
/*                          SYSTEM INFORMATION                              */
/****************************************************************************/
static icb_info_t icb_info =
{
  .num_bcms   = ARRAY_SIZE(bcms),
  .bcms       = bcms,
  .num_routes = ARRAY_SIZE(routes),
  .routes     = routes,
  .num_match_vcds = ARRAY_SIZE(match_vcds),
  .match_vcds     = match_vcds,
};

/****************************************************************************/
/*                          ACCESSOR FUNCTION                               */
/****************************************************************************/
/**
@copydoc icb_get_target_data()
*/
icb_info_t *icb_get_target_data( void )
{
  return &icb_info;
}
