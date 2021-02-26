#ifndef __MDSSPlatform_3xx_H__
#define __MDSSPlatform_3xx_H__
/*=============================================================================
 
  File: MDSSPlatform_3xx.h
 
  Internal header file for MDSS 3xx library
  
 
  Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "MDPLib_i.h"

/*===========================================================================
                                Defines 
===========================================================================*/

#define RES_MAX_FAMILY_MAJOR                      4
#define RES_MAX_FAMILY_MINOR                      1

#define XO_DEFAULT_FREQ_IN_HZ                     19200000


 /*=========================================================================
     MDSS 3.x.x Tables
 ==========================================================================*/


 MDPExternalClockEntry            sDSI0ExtClocks_3xx[] = 
  {
    {"mmss_mdss_esc0_clk",      0, 1, 0, 0, 0, 0},     // Index 0 : primary source :XO , Secondary source : dsi pll
    {"mmss_mdss_pclk0_clk",     1, 0, 0, 0, 0, 0},     // Index 1 : Source DSI0_PLL
    {"mmss_mdss_byte0_clk",     1, 0, 0, 0, 0, 0},     // Index 2 : Source DSI0_PLL
    {"mmss_mdss_byte0_intf_clk",1, 0, 2, 0, 0, 0, 2},  // Index 3 : Source DSI0_PLL
    {"\0",                      0, 0, 0, 0, 0, 0},  
  };     
  MDPExternalClockEntry            sDSI1ExtClocks_3xx[] = 
  {
    {"mmss_mdss_esc1_clk",      0, 1, 0, 0, 0, 0},     // Index 0 : primary source :XO , Secondary source : dsi pll
    {"mmss_mdss_pclk1_clk",     2, 0, 0, 0, 0, 0},     // Index 1 : Source DSI1_PLL
    {"mmss_mdss_byte1_clk",     2, 0, 0, 0, 0, 0},     // Index 2 : Source DSI1_PLL
    {"mmss_mdss_byte1_intf_clk",2, 0, 2, 0, 0, 0, 2},  // Index 3 : Source DSI1_PLL
    {"\0",                      0, 0, 0, 0, 0, 0},  
  };
 const MDPExternalClockEntry            sHDMISourceExtClocks_3xx[] =
  {
    {"mmss_mdss_extpclk_clk",   1, 0, 0, 0, 0, 0 },  // Index 0 : primary source :XO , Secondary source : dsi pll
    {"\0",                      0, 0, 0, 0, 0, 0 },
  };

  /*For Dual DSI Split display, DSI0/DSI1 share same PLL-- DSI0_PLL */
 MDPExternalClockEntry            sDSI1SharedSourceExtClocks_3xx[] = 
  {
    {"mmss_mdss_esc1_clk",      0, 1, 0, 0, 0, 0},     // Index 0 : primary source :XO , Secondary source : dsi pll
    {"mmss_mdss_pclk1_clk",     1, 0, 0, 0, 0, 0},     // Index 1 : Source DSI0_PLL
    {"mmss_mdss_byte1_clk",     1, 0, 0, 0, 0, 0},     // Index 2 : Source DSI0_PLL
    {"mmss_mdss_byte1_intf_clk",1, 0, 2, 0, 0, 0, 2},  // Index 3 : Source DSI0_PLL
    {"\0",                      0, 0, 0, 0, 0, 0},  
  }; 
 
  /* MDSS Clock list
   * Note: Clocks are ordered in order to ensure Ahb access is available prior to accessing the core.
   */
  const MDPClockEntry MDSSClocks_3xx[] =
  {
      {"mmss_mnoc_ahb_clk",              0, NULL, MDP_CLOCKFLAG_SHARED},
      {"mmss_mnoc_axi_clk",              0, NULL, MDP_CLOCKFLAG_SHARED},
      {"mmss_bimc_smmu_ahb_clk",         0, NULL, MDP_CLOCKFLAG_SHARED},
      {"mmss_bimc_smmu_axi_clk",         0, NULL, MDP_CLOCKFLAG_SHARED},
      {"mmss_s0_axi_clk",                0, NULL, MDP_CLOCKFLAG_SHARED},
      {"mmss_mdss_axi_clk",      240000000, NULL, 0x0},
      {"mmss_mdss_ahb_clk",              0, NULL, 0x0},
      {"mmss_mdss_mdp_clk",      300000000, NULL, 0x0},
      {"mmss_mdss_vsync_clk",            0, NULL, 0x0},
      {"\0",                             0, NULL }
  };
  
  
  /* DSI0 Clock list
   */
  const MDPClockEntry DSI0Clocks_3xx[] =
  {
      {"mmss_mdss_esc0_clk",         0, NULL, 0x0},
      {"mmss_mdss_byte0_clk",        0, NULL, 0x0},
      {"mmss_mdss_byte0_intf_clk",   0, NULL, 0x0},
      {"mmss_mdss_pclk0_clk",        0, NULL, 0x0},
      {"\0", 0, NULL}
  };
  
  /* DSI1 Clock list
   */
  const MDPClockEntry DSI1Clocks_3xx[] =
  {
      {"mmss_mdss_esc1_clk",         0, NULL, 0x0},
      {"mmss_mdss_byte1_clk",        0, NULL, 0x0},
      {"mmss_mdss_byte1_intf_clk",   0, NULL, 0x0},
      {"mmss_mdss_pclk1_clk",        0, NULL, 0x0},
      {"\0", 0, NULL}
  };
  
  /* DP Clock list
   */
  const MDPClockEntry DPClocks_3xx[] =
  {
      {"mmss_mdss_dp_pixel_clk",     0, NULL, 0x0},
      {"mmss_mdss_dp_link_clk",      0, NULL, 0x0},
      {"mmss_mdss_dp_crypto_clk",    0, NULL, 0x0},
      {"mmss_mdss_dp_gtc_clk",       0, NULL, 0x0},
      {"mmss_mdss_dp_aux_clk",       0, NULL, 0x0},
      {"mmss_mdss_dp_link_intf_clk", 0, NULL, 0x0},
      {"mmss_mdss_hdmi_dp_ahb_clk",  0, NULL, 0x0},
      {"\0", 0, NULL}
  
  };
  
  /* HDMI Clock List
   */
  const MDPClockEntry HDMIClocks_3xx[] =
  {
      {"mmss_mdss_hdmi_clk",         0, NULL, 0x0}, /* Enabling this clock will set the value to 19.2 MHz */
      {"mmss_mdss_hdmi_dp_ahb_clk",  0, NULL, 0x0},
      {"mmss_mdss_extpclk_clk",      0, NULL, 0x0}, /* The clock gets the source from HDMI PLL and 
                                               will get set to frequency corresponding to the HDMI resolution */
      {"\0", 0, NULL, 0x0}
  };
 
  /* MDSS power domain list
   */
  const MDPPowerDomainEntry MDSS3xxPowerDomain[] = 
  {
      {"VDD_BIMC_SMMU", MDP_CLOCKFLAG_SHARED},
      {"VDD_MDSS",                       0x0},
      {"\0",                             0x0}
  };
  
  /* MDSS resource list for MDSS 3xx
  */
  const MDP_ResourceList sMDP3xxResources =
  {
      (MDPPowerDomainEntry*)  &MDSS3xxPowerDomain,   /* Power domain    */
      (MDPClockEntry*)        &MDSSClocks_3xx,       /* MDP clocks      */
      (MDPClockEntry*)        &DSI0Clocks_3xx,       /* DSI clocks      */
      (MDPClockEntry*)        &DSI1Clocks_3xx,       /* DSI clocks      */
      (MDPClockEntry*)        &DPClocks_3xx,         /* DP clocks      */
      (MDPClockEntry*)        &HDMIClocks_3xx        /* HDMI clocks     */
  };
 
  
  /* MDP External resource list for MDP590
  */
  const MDP_ExtClockResourceList sMDP3xxExtClockResources =
  {
      (MDPExternalClockEntry*)    &sDSI0ExtClocks_3xx,               /* DSI0 Ext clocks      */
      (MDPExternalClockEntry*)    &sDSI1ExtClocks_3xx,               /* DSI1 Ext clocks      */
      (MDPExternalClockEntry*)    &sDSI1SharedSourceExtClocks_3xx,   /* DSI shared clocks for dual DSI */
      (MDPExternalClockEntry*)    &sHDMISourceExtClocks_3xx,         /* HDMI Ext clocks      */
  };
 
 /* Display resource list 
 */
 const DisplayResourceList sDisplayMDP3xxResources =
 {
    (MDP_ResourceList*)            &sMDP3xxResources,               /* MDP Resources          */
    (MDP_ExtClockResourceList*)    &sMDP3xxExtClockResources,       /* MDP Ext Resources      */
 };


 /*=========================================================================
       Top level Tables
 ==========================================================================*/

/* Reesource list for MDSS 3xx
*/
const MDP_HwMinorFamilyResourceList asHarwareFamilyMinor3xx[RES_MAX_FAMILY_MINOR] = {
    {MDSS_DEVICE_VERSION_03_00, (DisplayResourceList*)&sDisplayMDP3xxResources},     /* MDSS Family 3.0.x, MSM8998 */
};



/* MDP resource list  based on MDP major version.
   MDP3.x does not have a MDP version, use the MDP major version for clock list indexing
*/
const MDP_HwMajorFamilyResourceList asHarwareFamilyMajor[RES_MAX_FAMILY_MAJOR] = {
    {NULL},                                                     /* MDSS Family 0xx   */
    {NULL},                                                     /* MDSS Fmaily 1xx (Badger Family)       */
    {NULL},                                                     /* MDSS Family 2xx   */ 
    {(MDP_HwMinorFamilyResourceList*)&asHarwareFamilyMinor3xx}, /* MDSS Fmaily 3xx (Honeybadger Family)  */
}; 


#endif // __MDSSPlatform_3xx_H__

