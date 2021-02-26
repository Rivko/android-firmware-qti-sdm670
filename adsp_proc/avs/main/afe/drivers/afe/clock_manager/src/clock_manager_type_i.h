/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2015-2017 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/clock_manager_type_i.h#1 $
 ====================================================================== */
#ifndef _CLOCK_MANAGER_TYPE_I_H_
#define _CLOCK_MANAGER_TYPE_I_H_

#include "Elite.h"
#include "adsp_afe_service_commands.h"
#include "ClockDefs.h"
#include "DDIClock.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "hwd_mmpm.h"
#include "lpasshwio_devcfg.h"
#include "hwd_devcfg.h"
#include "hal_clock_i.h"
#include "clock_manager.h"

#define MAX_CLKNAME_LEN             (128)

typedef enum
{
   CLOCK_MANAGER_DEINIT = 0,
   CLOCK_MANAGER_INIT
} clock_manager_state_t;

/*
* The clk config info forward define for clk set function
*/
typedef struct generic_clk_cfg_info generic_clk_cfg_info_t;

/*
* The clk set function define
*/
typedef DALResult (*generic_clk_set_func)(
   DalDeviceHandle *,
   uint32_t divider,
   generic_clk_cfg_info_t *
);

/*
* The clk config info define
*/
typedef struct generic_clk_cfg_info
{
   uint32_t     clkid;                 // Clock ID as defined, pri-mi2s-ibit, etc..
   ClockIdType  clk_reg_id;            // Clock regime id provided by clk driver by name look-up
   uint32_t     clk_attr;              // Attribute for this clk
   uint32_t     clk_freq_in_hz;        // Clock freq set to for this clock
   uint32_t     clk_enable_cnt;        // Number of times clock that is enabled
   bool_t       is_clock_enabled;       // Flag to check whether clock is actually enabled or not
   uint32_t     oe_clkid;              // If non-zero, this is PCM OE clk id 
   uint32_t     oe_clk_freq;           // If non-zero, this is PCM OE clk freq
   generic_clk_set_func  clk_set_enable_func;  // Pointer to clk enable function
   generic_clk_set_func  clk_set_disable_func; // Pointer to clk disable function
} generic_clk_cfg_info_t;


/*
* The clk map table
*/
typedef struct generic_map_tab_info
{
   uint32_t         clkid;                     // Clock ID as defined, pri-mi2s-ibit, etc.
   const char       clk_name[MAX_CLKNAME_LEN]; // Clock regime name
   generic_clk_set_func  clk_set_enable_func;  // Pointer to clk enable function
   generic_clk_set_func  clk_set_disable_func; // Pointer to clk disable function
} generic_map_tab_info_t;


/**
* generic_clk_cfg_global_info_t to keep track all clk manager info
*/
typedef struct generic_clk_cfg_global_info
{
   clock_manager_state_t            state;
   HwdGenericClkPropertyType        devcfg;                // Device cfg info
   uint32_t                         tcsr_virt_addr;        // Virt addr for tcsr base
   qurt_mem_region_t                tcsr_qmem_region;     // tcsr, qmem mapping region
   
   hal_clk_loc_clkid_pair_func      p_func_loc_clkid_pair; // Pointer to function to loc clkid pair
   hal_clk_mux_sel_func             p_func_mux_sel;        // Pointer to function to sel associated mux 

   DalDeviceHandle                  *dal_handle_ptr;       // Handle for clk regime driver
   uint32_t                         adsp_pm_clientid;      // Client id for addsp pm
   uint32_t                         adsp_pwr_vote_cnt;     // Power on/off for adsp pm 

   generic_clk_cfg_info_t           *clks_cfg_node_ptr;    // Pointer to cfg node..
   uint32_t                         num_cfg_nodes;         // Number of cfg nodes..
   const generic_map_tab_info_t     *clks_map_table_ptr;   // Pointer to the map table..
   uint32_t                         num_map_table_entries; // Number of entries in map table
} generic_clk_cfg_global_info_t;


#endif
