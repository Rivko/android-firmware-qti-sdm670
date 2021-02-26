#ifndef __MDSSPlatform_4xx_H__
#define __MDSSPlatform_4xx_H__
/*=============================================================================
 
  File: MDSSPlatform_4xx.h
 
  Internal header file for MDSS 4xx library
  
 
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

#define RES_MAX_FAMILY_MAJOR                      5
#define RES_MAX_FAMILY_MINOR                      2

#define XO_DEFAULT_FREQ_IN_HZ                     19200000


 /*=========================================================================
     MDSS 4.x.x Tables
 ==========================================================================*/


 MDPExternalClockEntry            sDSI0ExtClocks_4xx[] = 
  {
    {"disp_cc_mdss_esc0_clk",      0, 1, 0, 0, 0, 0},     // Index 0 : primary source :XO , Secondary source : dsi pll
    {"disp_cc_mdss_pclk0_clk",     1, 0, 0, 0, 0, 0},     // Index 1 : Source DSI0_PLL
    {"disp_cc_mdss_byte0_clk",     1, 0, 0, 0, 0, 0},     // Index 2 : Source DSI0_PLL
    {"disp_cc_mdss_byte0_intf_clk",1, 0, 2, 0, 0, 0, 2},  // Index 3 : Source DSI0_PLL
    {"\0",                         0, 0, 0, 0, 0, 0},  
  };     
  MDPExternalClockEntry            sDSI1ExtClocks_4xx[] = 
  {
    {"disp_cc_mdss_esc1_clk",      0, 1, 0, 0, 0, 0},     // Index 0 : primary source :XO , Secondary source : dsi pll
    {"disp_cc_mdss_pclk1_clk",     2, 0, 0, 0, 0, 0},     // Index 1 : Source DSI1_PLL
    {"disp_cc_mdss_byte1_clk",     2, 0, 0, 0, 0, 0},     // Index 2 : Source DSI1_PLL
    {"disp_cc_mdss_byte1_intf_clk",2, 0, 2, 0, 0, 0, 2},  // Index 3 : Source DSI1_PLL
    {"\0",                         0, 0, 0, 0, 0, 0},  
  };

  /*For Dual DSI Split display, DSI0/DSI1 share same PLL-- DSI0_PLL */
 MDPExternalClockEntry            sDSI1SharedSourceExtClocks_4xx[] = 
  {
    {"disp_cc_mdss_esc1_clk",      0, 1, 0, 0, 0, 0},     // Index 0 : primary source :XO , Secondary source : dsi pll
    {"disp_cc_mdss_pclk1_clk",     1, 0, 0, 0, 0, 0},     // Index 1 : Source DSI0_PLL
    {"disp_cc_mdss_byte1_clk",     1, 0, 0, 0, 0, 0},     // Index 2 : Source DSI0_PLL
    {"disp_cc_mdss_byte1_intf_clk",1, 0, 2, 0, 0, 0, 2},  // Index 3 : Source DSI0_PLL
    {"\0",                         0, 0, 0, 0, 0, 0},  
  }; 
 
  /* MDSS Clock list
   * Note: Clocks are ordered in order to ensure Ahb access is available prior to accessing the core.
   * At uefi, no rate set need for dsip_cc_mdss_axi_clk. But at ramdump mode, 200Mhz rate need to be 
   * set, otherwise flicking shows up. Therefore 200Mhz is keep at table for the purpose of ramdump.
  */
  const MDPClockEntry MDSSClocks_4xx[] =
  {
      {"disp_cc_mdss_axi_clk",      240000000, NULL, 0x0},  /* Frequency setting is only for Ramdump mode. The freq vote will be a no op
                                                             * in regular mode where Clk driver will vote @ Fmax_nominal for mmnoc_axi.
                                                             * The branch still needs to be enabled but clk is not controlled directly. 
                                                             */
      {"gcc_disp_ahb_clk",                  0, NULL, MDP_CLOCKFLAG_SHARED},
      {"disp_cc_mdss_ahb_clk",              0, NULL, 0x0},
      {"disp_cc_mdss_mdp_clk",      430000000, NULL, 0x0},
      {"disp_cc_mdss_vsync_clk",            0, NULL, 0x0},
      {"\0",                                0, NULL }
  };
  
  
  /* DSI0 Clock list
   */
  const MDPClockEntry DSI0Clocks_4xx[] =
  {
      {"disp_cc_mdss_esc0_clk",         0, NULL, 0x0},
      {"disp_cc_mdss_byte0_clk",        0, NULL, 0x0},
      {"disp_cc_mdss_byte0_intf_clk",   0, NULL, 0x0},
      {"disp_cc_mdss_pclk0_clk",        0, NULL, 0x0},
      {"\0", 0, NULL}
  };
  
  /* DSI1 Clock list
   */
  const MDPClockEntry DSI1Clocks_4xx[] =
  {
      {"disp_cc_mdss_esc1_clk",         0, NULL, 0x0},
      {"disp_cc_mdss_byte1_clk",        0, NULL, 0x0},
      {"disp_cc_mdss_byte1_intf_clk",   0, NULL, 0x0},
      {"disp_cc_mdss_pclk1_clk",        0, NULL, 0x0},
      {"\0", 0, NULL}
  };
  
  /* DP Clock list
   */
  const MDPClockEntry DPClocks_4xx[] =
  {
      {"disp_cc_mdss_dp_pixel_clk",     0, NULL, 0x0},
      {"disp_cc_mdss_dp_link_clk",      0, NULL, 0x0},
      {"disp_cc_mdss_dp_crypto_clk",    0, NULL, 0x0},
      {"disp_cc_mdss_dp_gtc_clk",       0, NULL, 0x0},
      {"disp_cc_mdss_dp_aux_clk",       0, NULL, 0x0},
      {"disp_cc_mdss_dp_link_intf_clk", 0, NULL, 0x0},
      {"disp_cc_mdss_dp_crypto_clk",    0, NULL, 0x0},
      {"\0", 0, NULL}
  
  };

  const MDPClockEntry DispCcXoClk_4xx[] =
  {
      {"gcc_disp_xo_clk",     0, NULL, 0x0},
      {"\0", 0, NULL}
  
  };
  
  /* MDSS power domain list
   */
  const MDPPowerDomainEntry MDSS4xxPowerDomain[] = 
  {
      {"mdss_core_gdsc",        0x0},
      {"\0",                    0x0}
  };
  
  /* MDSS resource list for MDSS 4xx
  */
  const MDP_ResourceList sMDP4xxResources =
  {
      (MDPPowerDomainEntry*)  &MDSS4xxPowerDomain,   /* Power domain    */
      (MDPClockEntry*)        &DispCcXoClk_4xx,      /* gcc_disp_cc_xo_clk */
      (MDPClockEntry*)        &MDSSClocks_4xx,       /* MDP clocks      */
      (MDPClockEntry*)        &DSI0Clocks_4xx,       /* DSI clocks      */
      (MDPClockEntry*)        &DSI1Clocks_4xx,       /* DSI clocks      */
      (MDPClockEntry*)        &DPClocks_4xx,         /* DP clocks      */
  };
 
  
  /* MDP External resource list for MDP590
  */
  const MDP_ExtClockResourceList sMDP4xxExtClockResources =
  {
      (MDPExternalClockEntry*)    &sDSI0ExtClocks_4xx,               /* DSI0 Ext clocks      */
      (MDPExternalClockEntry*)    &sDSI1ExtClocks_4xx,               /* DSI1 Ext clocks      */
      (MDPExternalClockEntry*)    &sDSI1SharedSourceExtClocks_4xx,   /* DSI shared clocks for dual DSI */
  };
 
 /* Display resource list 
 */
 const DisplayResourceList sDisplayMDP4xxResources =
 {
    (MDP_ResourceList*)            &sMDP4xxResources,               /* MDP Resources          */
    (MDP_ExtClockResourceList*)    &sMDP4xxExtClockResources,       /* MDP Ext Resources      */
 };


 /*=========================================================================
       Top level Tables
 ==========================================================================*/

/* Reesource list for MDSS 4xx
*/
const MDP_HwMinorFamilyResourceList asHarwareFamilyMinor4xx[RES_MAX_FAMILY_MINOR] = {
    {MDSS_DEVICE_VERSION_04_00, (DisplayResourceList*)&sDisplayMDP4xxResources},     /* MDSS Family 4.0.x, SDM845 */
    {MDSS_DEVICE_VERSION_04_01, (DisplayResourceList*)&sDisplayMDP4xxResources},     /* MDSS Family 4.1.x, SDM670 */
};



/* MDP resource list  based on MDP major version.
   MDP4.x does not have a MDP version, use the MDP major version for clock list indexing
*/
const MDP_HwMajorFamilyResourceList asHarwareFamilyMajor[RES_MAX_FAMILY_MAJOR] = {
    {NULL},                                                     /* MDSS Family 0xx   */
    {NULL},                                                     /* MDSS Fmaily 1xx (Badger Family)       */
    {NULL},                                                     /* MDSS Family 2xx   */ 
    {NULL},                                                     /* MDSS Family 3xx   */ 
    {(MDP_HwMinorFamilyResourceList*)&asHarwareFamilyMinor4xx}, /* MDSS Fmaily 4xx (Napali Family)  */
}; 


#endif // __MDSSPlatform_4xx_H__

