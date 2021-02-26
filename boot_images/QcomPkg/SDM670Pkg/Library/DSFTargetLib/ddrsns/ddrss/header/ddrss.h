/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/header/ddrss.h#8 $
$DateTime: 2018/12/17 04:30:22 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
05/04/14   arindamm     First edit history header. Add new entries at top.
================================================================================*/

#ifndef __DDRSS_H__
#define __DDRSS_H__

#include "ddr_phy_seq_hwioreg.h"
#include "ddr_ss_seq_hwioreg.h"
#include "ddr_cc_seq_hwioreg.h"
#include "HAL_SNS_DDR.h"
#include "ddrss_training.h"
#include "ddr_phy.h"
#include "target_config.h"
#include "frwk_training.h"
#include "ddr_ss_seq_hwiobase.h"
#include "mc_common.h"
#include "dcb.h"

#define NUM_PHY_PCC_LL            64 // Number of PCC Link List Elements
#define NUM_CC_PCC_LL             60 // Number of PCC Link List Elements

#define DIT_RUNTIME_FREQ          1008 // 1555MHz
#define DIT_NORMAL_DELTA          20

#define DIT_FINE_STEP_LIMIT       FINE_STEPS_PER_COARSE -1 //4
#define DIT_FINE_STEP_IN_PS       FINE_STEP_IN_PS //10
#define DIT_INTERVAL_PER_SEC      4

#include "ddrss_training.h"
#include "mc_data.h"

#define CONVERT_CYC_TO_PS        1000000000
#define DDR_PHY_SS_BASE          0x01400000

#define DDR_PHY_OFFSET           0x1000      // DDR PHY Address offset (2k Bytes)
#define CA0_DDR_PHY_OFFSET       0x0000
#define DQ0_DDR_PHY_OFFSET       0x1000
#define DQ1_DDR_PHY_OFFSET       0x2000
#define DDR_CC_OFFSET            0x6000

#define REG_OFFSET_DDR_PHY_CH(ch) ((ch == 0)  ? DDR_PHY_SS_BASE :\
                                   				DDR_PHY_SS_BASE + 0x03000)

#define REG_OFFSET_DDR_CC_CH(ch)  DDR_PHY_SS_BASE
                                                
#define BROADCAST_BASE SEQ_DDR_SS_DDRSS_AHB2PHY_BROADCAST_AHB2PHY_BROADCAST_SWMAN1_OFFSET


typedef enum
{
  SAVE,
  RESTORE
} SAVE_RESTORE;

typedef struct __attribute__((packed)) ddi_train_record
{
  uint8 vref;
  uint8 coarse;
  uint8 fine;
} ddi_train_record;

typedef struct __attribute__((packed)) ddi_plot_record
{
  uint8 vref;
  uint8 coarse;
  uint8 fine;
  uint32 chcs[NUM_CH][NUM_CS];
} ddi_plot_record;


//Vref
int DDRSS_set_rd_vref(uint8 ch, uint8 dq, uint32 freq_khz, uint8 vref);
uint8 DDRSS_get_rd_vref(uint8 ch, uint8 dq, uint32 freq_khz);
uint8 DDRSS_get_active_rd_vref(uint8 ch, uint8 dq, uint32 freq_khz);
uint8 DDRSS_get_active_wr_vref(uint8 cs, uint8 ch, uint8 dq);
uint8 DDRSS_get_wr_vref(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz);
int DDRSS_set_wr_vref(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8 vref_steps);

//CDC
int DDRSS_get_cdcext_slave_rd(uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8* const coarse_delay, uint8* const fine_delay);
int DDRSS_set_cdcext_slave_rd(uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8 coarse_delay, uint8 fine_delay);
int DDRSS_get_active_cdc_slave_rd(uint8 ch, uint8 cs, uint8 dq, uint8* const coarse_delay, uint8* const fine_delay);
int DDRSS_set_cdcext_slave_wr(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8 coarse_delay, uint8 fine_delay);
int DDRSS_get_cdcext_slave_wr(uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8* const coarse_delay, uint8* const fine_delay);
int DDRSS_get_active_cdc_slave_wr(uint8 ch, uint8 cs, uint8 dq, uint8* const coarse_delay, uint8* const fine_delay);

//unit conversion
int DDRSS_convert_vref_mv_to_steps(BOOT_DDR_STRUCT *ddr, uint32 vref_mv, uint32 freq_khz, uint8 dir_rd_wr, uint8* const out_vref_steps);
uint32 DDRSS_convert_vref_steps_to_mv(uint8 steps, uint8 dir_rd_wr, uint32 freq_khz);
uint32 DDRSS_convert_cdc_coarse_fine_steps_to_ps(uint8 cdc_delay_coarse, uint8 cdc_delay_fine);
int DDRSS_convert_cdc_ps_to_coarse_fine_steps(const uint32 in_cdc_delay_ps, uint8* const out_cdc_delay_coarse, uint8* const out_cdc_delay_fine, uint8 dir_rd_wr);
int DDRSS_adjust_cdc_range(uint8* const cdc_delay_coarse, uint8* const cdc_delay_fine, uint8 dir_rd_wr);

// Training seq prototypes
void identify_training_frequencies (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars); 
                                    
void pre_training_setup            (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void Training_DDRSS_RCW            (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);

void Training_DDRSS_PLLPRECAL      (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz,  
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void Training_DDRSS_DCC            (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void Training_DDRSS_CA_VREF_LPDDR4 (BOOT_DDR_STRUCT *ddr,
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void Training_DDRSS_WRLVL          (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void Training_DDRSS_WR_TDQS2DQ     (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                   
void Training_DDRSS_RD_DQDQS_LPDDR4(BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void Training_DDRSS_WR_DQDQS_LPDDR4(BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void Training_DDRSS_DIT            (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);
                                    
void post_training_setup           (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars);

//***************************************
//****************************************


extern uint64 training_address[NUM_CH][NUM_CS];

// Define in ddrss_phy_wrapper.c
void DDR_PHY_CC_Config(BOOT_DDR_STRUCT *ddr);
void DDR_PHY_CC_init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel);//, uint32 clk_freq_khz);

void DDRSS_set_config(BOOT_DDR_STRUCT *ddr, uint32 offset, uint32 (*config)[2]);
   
/***************************************************************************************************
 Common training functions
 ***************************************************************************************************/
 void DDRSS_Register_Save_Restore (SAVE_RESTORE save_restore,
                                   uint16 stack_size,
                                   uint32 offset,
                                   uint32 *address_list,
                                   uint32 *value_stack);
 
void pre_training_mode_reg_writes (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);
 
void DDRSS_Training_Save_Restore (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, SAVE_RESTORE save_restore);
void DDRSS_DRAM_Vref_Restore     (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);
boolean DDRSS_Post_Boot_Training (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select);

uint8 DDRSS_Get_Freq_Index (BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz);

void DDRSS_Get_Training_Address(BOOT_DDR_STRUCT *ddr);

void DDRSS_MR_Write_per_die (BOOT_DDR_STRUCT *ddr,
                             uint8 ch,
                             uint8 cs,
                             uint8 MR_addr,
                             uint8 MR_data_die1, 
                             uint8 MR_data_die2);

void DDRSS_WR_midpoint_to_CDC (BOOT_DDR_STRUCT *ddr,
                               uint8 ch, 
                               uint8 cs, 
                               uint32 midpoint_cdc_ps,
                               uint8 half_cycle,
                               uint8 phy_inx,
                               uint32 target_freq,
                               uint8 freq_inx
                              );

uint16 DDRSS_dq_remapping (uint8 pattern);

void ddr_mem_copy(uint32 * source, uint32 * destination, uint32 size, uint32 burst);

void DDRSS_ddr_phy_sw_freq_switch    (BOOT_DDR_STRUCT *ddr, uint8 ch);

/***************************************************************************************************
 CA Vref training
 ***************************************************************************************************/
boolean DDRSS_Vrefonly_lpddr4(BOOT_DDR_STRUCT *ddr,
                              uint8            ch,  
                              uint8            cs, 
                              training_params_t        *training_params_ptr,
                              ddrss_ca_vref_local_vars *local_vars,
                              uint32           clk_freq_khz
                              );

void DDRSS_BIST_CA_Training_entry (uint8 ch);
void  DDRSS_BIST_cfg_CA_Pattern (uint8 ch, uint8  ca_training_pattern[3]);


/***************************************************************************************************
 DCC Training
 ***************************************************************************************************/
void DDRSS_dcc_boot (BOOT_DDR_STRUCT *ddr, 
                     DDR_CHANNEL channel, 
                     DDR_CHIPSELECT chip_select,                      
                     PHY_BUS dq, 
                     PHY_BUS ca, 
                     training_params_t *training_params_ptr,					 
                     uint32 training_sel, 
                     uint32 clk_freq_khz,
                     uint8 prfs_index);
/****************************************************************************************************
RCW Training helper functions prototypes
****************************************************************************************************/
void DDRSS_rcw (BOOT_DDR_STRUCT *ddr,
                uint8 ch,
                uint8 cs,
                training_params_t *training_params_ptr,
                uint8 rcw_start_clk_idx,
                uint8 rcw_clk_idx);

void  DDRSS_RCW_slave (BOOT_DDR_STRUCT *ddr,
					uint8 ch,
					uint8 cs,
					DDR_CHIPSELECT populated_chipselect,
					PHY_BUS dq,
					training_params_t *training_params_ptr,
					uint8 rcw_start_clk_idx,
                	uint32 current_freq,
                	ddrss_rcw_local_vars *local_vars);
void rcw_enable_disable (uint8 byte_not_done[NUM_DQ_PCH], uint8 ch, PHY_BUS dq);
void set_rcw_full_cycle (uint8 byte_not_done[NUM_DQ_PCH], uint8 num_cycle, uint8 ch,  PHY_BUS dq);
void set_rcw_cdc_delay ( uint32 rcw_delay,  uint16 coarse_fine, uint16 mode, uint8 ch, uint8 byte_not_done[NUM_DQ_PCH], PHY_BUS dq);
void cdc_delay_search (uint8 loop_cnt, uint8 max_delay, uint8 coarse_fine_sel,  uint8 *all_found, uint8 ch, uint8 cs, uint8 *byte_done, uint8 *byte_not_done,  PHY_BUS dq, ddrss_rcw_local_vars *local_vars);
void  read_rcw_status (uint8 *byte_one_cnt, uint8 *byte_zero_cnt, PHY_BUS dq, uint8 ch);
void read_tDQSCK( uint8 ch, uint8 cs, uint8 byte_lane, ddrss_rcw_local_vars *local_vars, uint8 coarse_fine_sel);



void DDRSS_scale_rcw_delay_write_ext_reg (BOOT_DDR_STRUCT *ddr,
                                                    uint8 ch,
                                                    DDR_CHIPSELECT populated_chipselect,
                                                    PHY_BUS dq,
                                                    training_params_t *training_params_ptr,
                                                    uint8 rcw_start_clk_idx,
                                                    ddrss_rcw_local_vars *local_vars);


/***************************************************************************************************
 Write Leveling
 ***************************************************************************************************/                       
void DDRSS_wrlvl_seq ( BOOT_DDR_STRUCT *ddr,
                       uint8 channel,
                       uint8 cs,
		       uint32 freq,
		       training_params_t *training_params_ptr,
		       uint8 prfs_index);

void DDRSS_wrlvl_ca (BOOT_DDR_STRUCT *ddr,
                     uint8 ch,
                     uint8 cs,
                     uint32 clk_freq_khz,
                     training_params_t *training_params_ptr,
					 uint8 prfs_index,
					 uint8 MR2_opcode);					 

void DDRSS_wrlvl_dqs (BOOT_DDR_STRUCT *ddr,
                     uint8 ch,
                     uint8 cs,
                     uint32 clk_freq_khz,
                     training_params_t *training_params_ptr,
                     uint8 prfs_index,
					 uint8 MR2_opcode);

void DDRSS_wrlvl_ca_dqs (BOOT_DDR_STRUCT *ddr,
                        DDR_CHANNEL channel, 
                        DDR_CHIPSELECT chip_select, 
                        uint8 ch,
                        uint8 cs,
                        uint32 clk_freq_khz,
                        training_params_t *training_params_ptr,
                        uint8 prfs_index);

void DDRSS_wrlvl_write_flash_params (BOOT_DDR_STRUCT *ddr,
                                     uint8 ch,
                                     uint8 cs,
                                     uint32 wrlvl_clk_freq_idx);
boolean DDRSS_RD_midpoint_to_CDC (
                                 BOOT_DDR_STRUCT *ddr,
                                 uint32 middle,
                                 uint32 clk_freq_in_khz,
                                 uint8  ch,
                                 uint8  cs,
                                 uint8  phy_inx,
                                 uint8 freq_inx
                                 );
/***************************************************************************************************
 Write and Read training
 ***************************************************************************************************/
 uint8 DDRSS_wich_phase (uint32 _inst_, uint8 rank, int dcc_adj);
 
uint16 DDRSS_rd_dcc2csr ( int dcc_adj);
uint16 DDRSS_wr_dcc2csr ( int dcc_adj);

 void DDRSS_passing_region_scan( uint8 *start, uint8 *stop, uint8 fail_count_histogram[HISTOGRAM_SIZE], uint32 histogram_start, uint32 histogram_max);

 boolean DDRSS_rd_dqdqs_1D_pbcf(BOOT_DDR_STRUCT                   *ddr, 
                                   uint8                        ch, 
                                   uint8                        cs, 
                                   PHY_BUS                      ca,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx,
                                   uint8 training_ddr_freq_indx_eq_training_table_index
                                   );
 boolean DDRSS_rd_dqdqs_1D_cf(BOOT_DDR_STRUCT             *ddr,
                        uint8                        ch,
                        uint8                        cs,
                        PHY_BUS                      ca,
                        PHY_BUS                      dq,
                        training_params_t            *training_params_ptr,
                        ddrss_rdwr_dqdqs_local_vars  *local_vars,
                        uint32                       clk_freq_khz,
                        uint8                        prfs_index,
                        uint8                        freq_eq_max_training
                        );
 uint16 DDRSS_rd_dqdqs_get_min_eyewith(BOOT_DDR_STRUCT             *ddr,
                        uint8                        ch,
                        uint8                        cs,
                        PHY_BUS                      ca,
                        PHY_BUS                      dq,
                        training_params_t            *training_params_ptr,
                        ddrss_rdwr_dqdqs_local_vars  *local_vars,
                        uint32                       clk_freq_khz,
                        uint8                        prfs_index,
                        uint8                        freq_eq_max_training
                        );
boolean DDRSS_rd_2D_scrn(BOOT_DDR_STRUCT             *ddr,
                       uint8                        ch,
                       uint8                        cs,
                       PHY_BUS                      ca,
                       PHY_BUS                      dq,
                       training_params_t            *training_params_ptr,
                       ddrss_rdwr_dqdqs_local_vars  *local_vars,
                       uint32                       clk_freq_khz,
                       uint8                        prfs_index,
                       uint8                        freq_eq_max_training
                       );						
boolean DDRSS_rd_Vref_1D_mprx(BOOT_DDR_STRUCT                   *ddr, 
                                   uint8                        ch, 
                                   DDR_CHIPSELECT               chip_select,
                                   PHY_BUS                      ca,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars
                                  );
boolean DDRSS_rd_dqdqs_log (BOOT_DDR_STRUCT                   *ddr, 
                                   uint8                        ch, 
                                   uint8                        cs, 
                                   PHY_BUS                      ca,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx);
boolean DDRSS_rd_Vref_hprx_log(BOOT_DDR_STRUCT                   *ddr, 
                                   uint8                        ch, 
                                   DDR_CHIPSELECT               chip_select,
                                   PHY_BUS                      ca,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars
                                    );


void DDRSS_rd_dqdqs_coarse_fine_plot_pivot (BOOT_DDR_STRUCT *ddr,
                                            training_params_t *training_params_ptr,
                                            uint32 clk_freq_khz,
                                            uint64* output_iter
                                            ); 
                                            
void DDRSS_wr_dqdqs_coarse_fine_plot_pivot (BOOT_DDR_STRUCT *ddr, 
                                            training_params_t *training_params_ptr,
                                            uint32 clk_freq_khz,
                                            uint64* output_iter
                                            );                                          
                                            
// ============================================================================================================================
// ======================================     V R E F   H P R X    T R A I N I N G            =================================
// ============================================================================================================================
boolean DDRSS_rd_Vref_1D_hprx(BOOT_DDR_STRUCT                   *ddr, 
                                   uint8                        ch, 
                                   DDR_CHIPSELECT               chip_select,
                                   PHY_BUS                      ca,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars
                                   ); 
 
void DDRSS_set_global_vref ( uint32 global_vref,  uint32 ch );

/***************************************************************************************************
VREF DQ WRITE TRAINING
 ***************************************************************************************************/
boolean DDRSS_wr_dqdqs_1D_pbcf(BOOT_DDR_STRUCT             *ddr,
                            uint8                        ch, 
                            uint8                        cs, 
                            PHY_BUS                      dq,
                            training_params_t            *training_params_ptr,
                            ddrss_rdwr_dqdqs_local_vars  *local_vars,
                            uint32                       clk_freq_khz,
                            uint8                        prfs_indx,
                            uint8 						 training_ddr_freq_indx_eq_training_table_index,
                            uint8 						 dd_offset
                           );
                         
boolean DDRSS_wr_dqdqs_2D(BOOT_DDR_STRUCT             *ddr,
                          uint8                        ch,
                          uint8                        cs,
                          PHY_BUS                      dq,
                          training_params_t            *training_params_ptr,
                          ddrss_rdwr_dqdqs_local_vars  *local_vars,
                          uint32                       clk_freq_khz,
                          uint8                        prfs_indx,
                          uint8                        training_ddr_freq_indx_eq_training_table_index
                          );
boolean DDRSS_wr_dqdqs_1D_cf(BOOT_DDR_STRUCT                   *ddr,
                                   uint8                        ch,
                                   uint8                        cs,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx,
                                   uint8 training_ddr_freq_indx_eq_training_table_index
                                   );                                   
boolean DDRSS_wr_dqdqs_2D_vcf(BOOT_DDR_STRUCT                   *ddr,
                                   uint8                        ch,
                                   uint8                        cs,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx,
                                   uint8                        training_ddr_freq_indx_eq_training_table_index,
                                   uint8                        sed_enable
                                   );


//convert steps into LP4 WR Vref split range format
uint8 vknob2vrange(uint8 vknob);
//convert LP4 WR Vref split range format into steps
uint8 vrange2vknob(uint8 lp4_mr_vref);

boolean DDRSS_wr_dqdqs_log(BOOT_DDR_STRUCT             *ddr,
                           uint8                        ch,
                           uint8                        cs,
                           PHY_BUS                      dq,
                           training_params_t            *training_params_ptr,
                           ddrss_rdwr_dqdqs_local_vars  *local_vars,
                           uint32                       clk_freq_khz,
                           uint8                        prfs_indx
                           );
                           
/***************************************************************************************************
VREF DQ WRITE TRAINING
 ***************************************************************************************************/

void DDRSS_wr_dqdqs_lpddr4 (BOOT_DDR_STRUCT *ddr, 
                            uint8 ch, 
                            uint8 cs, 
                            PHY_BUS dq,
                            training_params_t *training_params_ptr,
                            ddrss_rdwr_dqdqs_local_vars *local_vars,
                            uint32 clk_freq_khz,
                            uint8 prfs_index);
                            
void DDRSS_MR_Write_on_die (BOOT_DDR_STRUCT *ddr, 
                            uint8 ch, 
                            uint8 cs, 
                            uint8 MR_addr, 
                            uint8 MR_data,
                            uint8 die);

void DDRSS_BIST_MR_Write (BOOT_DDR_STRUCT *ddr, 
                            uint8 ch, 
                            uint8 cs );

void DDRSS_BIST_DCC (BOOT_DDR_STRUCT *ddr, 
                            uint8 ch, 
                            uint32 _inst_,							
                            uint32 loop_count,
                            uint32 clk_freq_khz);

void DDRSS_BIST_wrlvl (BOOT_DDR_STRUCT *ddr, 
                            uint8 ch, 
                            uint32 _inst_,							
                            uint32 loop_count,
                            uint32 clk_freq_khz);

void DDRSS_MC_cnt_clk_en(BOOT_DDR_STRUCT *ddr,
                             DDR_CHANNEL channel,
                             DDR_CHIPSELECT chip_select);
                             
void DDRSS_MC_cnt_clk_dis(BOOT_DDR_STRUCT *ddr,
                             DDR_CHANNEL channel,
                             DDR_CHIPSELECT chip_select);
                                        
boolean DDRSS_wr_dqdqs_lpddr4_tdqs2dq_cal (BOOT_DDR_STRUCT *ddr, 
                                           uint8 ch, 
                                           uint8 cs, 
                                           PHY_BUS dq,
                                           training_params_t *training_params_ptr, 
                                           uint32 clk_freq_khz);
                                  
void DDRSS_WR_half_full_cycle_search  (BOOT_DDR_STRUCT *ddr, 
                                       uint8 ch, 
                                       uint8 cs, 
                                       PHY_BUS dq,
                                       training_params_t *training_params_ptr,
                                       ddrss_rdwr_dqdqs_local_vars *local_vars,
                                       uint32 clk_freq_khz,
                                       uint8 prfs_index);
                                     
uint16 DRAM_Oscillator_Read (uint8 ch, 
                             uint8 cs, 
                             uint8 byte_lane, 
                             uint32 MR23_value);

/***************************************************************************************************
 Memory read/write routine used for training. 
 ***************************************************************************************************/
void __blocksCopy(uint32 *source, uint32 *destination, uint32 num_words);
void DDRSS_device_reset_cmd(void);

//================================================================================================//
// DDR PHY DQ READ/WRITE/VREF training
//================================================================================================//

void    DDRSS_cdcref_init                                     (uint32 cdc_ref[HISTOGRAM_SIZE][4])                                                              ;
void    DDRSS_cdcref_init_wr                                  (uint32 cdc_ref[HISTOGRAM_SIZE][4])                                                              ;


/***************************************************************************************************
 Periodic Training
 ***************************************************************************************************/
void  DDRSS_DIT_Read (BOOT_DDR_STRUCT *ddr, 
                      uint8 ch, 
                      uint8 cs, 
                      training_params_t *training_params_ptr,
                      uint8 dit_ave,
                      uint32 (*dit_count)[NUM_CS][NUM_DIE_PCH],
                      uint32 clk_freq_khz);

void  DDRSS_DIT_Runtime (BOOT_DDR_STRUCT *ddr,
                      uint8 ch,
                      uint8 cs,
                      training_params_t *training_params_ptr,
                      uint32 (*dit_count)[NUM_CS][NUM_DIE_PCH],
                      uint32 clk_freq_khz);

void DDRSS_DIT_Slope(BOOT_DDR_STRUCT *ddr, 
                     uint8 ch, 
                     uint8 cs, 
                     uint8 die, 
                     uint32 clk_freq_khz,
                     uint16 (*dit_mid_count));

void DDRSS_DIT_Capture(BOOT_DDR_STRUCT *ddr,
                       uint8 ch, 
                       uint8 cs, 
                       uint16 dit_runtime_count, 
                       uint32 (*MR_19_18_data));

boolean DDRSS_Periodic_Training_load_dit_struct_to_dtts(BOOT_DDR_STRUCT *ddr, 
                                                        uint8  ch, 
                                                        DDR_CHIPSELECT chip_select);
/***************************************************************************************************
 PHY Power Collapse
 ***************************************************************************************************/
void DDR_PHY_rtn_ca_pcc_link_list_load (uint32 ddr_phy_addr_base);
void DDR_PHY_rtn_dq_pcc_link_list_load (uint32 ddr_phy_addr_base);
void DDR_CC_rtn_pcc_link_list_load (uint32 ddr_cc_addr_base);
void DDR_PHY_DCC_Reset (uint8 ch);

/***************************************************************************************************
 Clock Plan Index Look up 
 ***************************************************************************************************/
uint16 DDRSS_Find_Clk_Index(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz);

/***************************************************************************************************
 Downtime requests in DDRSS DT Aggregator
 ***************************************************************************************************/
void DDRSS_Downtime_Request_Ctrl (uint8 enable);

/** used in SHRM_proc_run_ctrl. */
typedef enum
{
  RUN  = 0x0, 
  STALL = 0x1,
} SHRM_RUN_CTRL_OPCODE;

/***************************************************************************************************
 SHRM RPMH PORT OVERRIDE APIs Declaration 
 ***************************************************************************************************/
#define RSI_CLOCKSWITCH_PREWORK_CMD  0x09010000
#define RSI_CLOCKSWITCH_WORK_CMD     0x09090000
#define RSI_CLOCKSWITCH_POSTWORK_CMD 0x09110000

void SHRM_rpmh_port_override_control(uint8 en);
void SHRM_rsi_cmd_inject(uint32 cmd_payload);
void SHRM_proc_run_ctrl(BOOT_DDR_STRUCT *ddr, SHRM_RUN_CTRL_OPCODE run_en);
void SHRM_init(BOOT_DDR_STRUCT *ddr);
void    DDRSS_mem_write_PXI             ( uint8 ch, uint8 cs);
uint32  DDRSS_mem_read_Get_Fail_pat_PXI ( uint8 ch, uint8 cs);
uint32  DDRSS_fail2hist             ( uint32 fail_pat ,  uint8 fail_count_histogram_perbit[3][NUM_DQ_PCH][PINS_PER_PHY][HISTOGRAM_SIZE],uint16 knob);
uint32  DDRSS_fail2bit                  ( uint32 fail_pat ,  uint8 fail_perbit[3][NUM_DQ_PCH][PINS_PER_PHY]);



boolean DDRSS_wr_dqdqs_2D_vcf_max_rec(BOOT_DDR_STRUCT                   *ddr,
                                   uint8                        ch,
                                   uint8                        cs,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx,
                                   uint8 training_ddr_freq_indx_eq_training_table_index
                                   );

boolean  DDRSS_Histogram_Max_Area_Rectangle(training_params_t            *training_params_ptr,
                                            uint8 v_pr[PHASES][MAXVREF][NUM_DQ_PCH][NUM_PR_PARAMS],
                                            uint8 dq_x, 
                                            uint8 sweep_size, 
                                            uint8 *center);

uint32 sub_histogram_max_rect_area( uint8 *hist_sum_arr_ptr,
                                 uint8 *rectangle_left_cdc,
                                 uint8 *rectangle_right_cdc,
                                 uint8 *rectangle_bottom_vref,
                                 uint8 sweep_size );
void DDRSS_Create_Training_Data(uint8 bus_width_pch);
uint8 DDRSS_get_sweep_size(uint32 clk_freq_khz);
uint8 DDRSS_get_sweep_size_rd(uint32 clk_freq_khz);
uint8 DDRSS_get_sweep_size_wr(uint32 clk_freq_khz);
/***************************************************************************************************
 eCDT Decoder
 ***************************************************************************************************/                              
boolean DDRSS_eCDT_Decode_Entry(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16 *list);
boolean DDRSS_Periodic_Training_load_dit_struct_to_shrm(BOOT_DDR_STRUCT *ddr, uint8 ch, DDR_CHIPSELECT qualified_cs);

uint32 DDRSS_fail2bit( uint32 fail_pat ,  uint8 fail_perbit[3][NUM_DQ_PCH][PINS_PER_PHY]);
void DDRSS_propagate_global_vref_hprx_to_all_PRFS (uint8 ch);
void DDRSS_propagate_global_vref_mprx_to_all_PRFS (uint8 ch);
void DDRSS_cpy_cdcext_slave_rd_r0tor1(uint8 prfs_x, uint8 ch );
void DDRSS_cpy_cdcext_slave_wr_r0tor1(uint8 prfs_x, uint8 ch );
void DDRSS_cpy_dram_wr_vref_r0tor1  (BOOT_DDR_STRUCT *ddr,uint8 ch, uint8 fsp );

uint8 DDRSS_get_wr_rank_update_en(uint8 prfs_x, uint8 ch );
void DDRSS_store_all_wr_rank_update_en(uint8 ch, uint8 Str_array[8][NUM_DQ_PCH] );
void DDRSS_restore_all_wr_rank_update_en(uint8 ch, uint8 Str_array[8][NUM_DQ_PCH] );
void DDRSS_set2one_all_wr_rank_update_en(uint8 ch );
#endif /* __DDRSS_H__ */
 
