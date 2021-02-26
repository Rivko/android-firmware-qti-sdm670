#ifndef __DDR_CONFIG_H__
#define __DDR_CONFIG_H__


/*==============================================================================
                      Warning: This file is auto-generated
================================================================================
                   Copyright 2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"


/*==============================================================================
                                  DATA
==============================================================================*/
extern uint32 (*ddr_bimc_misc_config_base)[2];
extern uint32 (*ddr_bimc_config_base)[2];
extern uint32 (*ddr_site_config_base)[2];
extern uint32 (*ddr_phy_config_base)[2] ;
extern uint32 (*ddr_caphy_config_base)[2];
extern uint32 (*ddr_dqphy_config_base)[2];

extern uint32 (*ddr_bimc_misc_config_delta)[2];
extern uint32 (*ddr_bimc_config_delta)[2];
extern uint32 (*ddr_site_config_delta)[2];
extern uint32 (*ddr_phy_config_delta)[2] ;
extern uint32 (*ddr_caphy_config_delta)[2];
extern uint32 (*ddr_dqphy_config_delta)[2];


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
void ddr_set_config
(
  uint32 offset,
  uint32 (*ddr_config_base)[2], uint32 (*ddr_config_delta)[2]
);


#endif
