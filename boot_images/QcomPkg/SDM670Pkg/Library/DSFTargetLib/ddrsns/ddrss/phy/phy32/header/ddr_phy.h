////////////////////////////////////////////////////////////////////////////////////////////////
//===========================================================================
//  Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
//  QUALCOMM Proprietary and Confidential. 
//===========================================================================
////////////////////////////////////////////////////////////////////////////////////////////////
#include "HAL_SNS_DDR.h"
#include "ddr_ss_seq_hwiobase.h"
#include "ddr_ss_seq_hwioreg.h"
#include "ddrss.h"

void   DDR_PHY_hal_cfg_ca_vref_dq_prep( uint32 _inst_ );
void   DDR_PHY_hal_cfg_ca_vref_ca_prep( uint32 _inst_ );

uint32 DDR_PHY_hal_sta_ca_vref        ( uint32 _inst_ );
void   DDR_PHY_hal_BIST_send_START    ( uint32 _inst_ );
void   DDR_PHY_hal_BIST_cfg_Vref      ( uint32 _inst_ , uint32 vref );
void   DDR_PHY_hal_BIST_cfg_CA_Pattern_ca( uint32 _inst_ , uint8  ca_trn_pat[3]);
void   DDR_PHY_hal_BIST_cfg_CA_Pattern_b0 ( uint32 _inst_ );
void   DDR_PHY_hal_BIST_cfg_CA_Pattern_b1 ( uint32 _inst_ );
void   DDR_PHY_hal_Send_BIST_START    ( uint32 _inst_ );


void   DDR_PHY_hal_cfg_ca_vref_dq_out_entry( uint32 _inst_ );
void   DDR_PHY_hal_cfg_ca_vref_dq_in_entry ( uint32 _inst_ );
void   DDR_PHY_hal_cfg_ca_vref_dq_out_exit ( uint32 _inst_ );
void   DDR_PHY_hal_cfg_ca_vref_ca_exit     ( uint32 _inst_ );
uint32 DDR_PHY_hal_sta_ca_vref             ( uint32 _inst_ );


void DDR_PHY_hal_cfg_sw_iocal            ( uint32 _inst_ );

void DDR_PHY_hal_cfg_ca_vref_dq_exit( uint32 _inst_ );
void DDR_PHY_hal_dqsdcc_read_CSR    ( uint32 _inst_, uint8 rank, uint16 dcc_adj);
void DDR_PHY_hal_wr_io_dqsdcc_CSR   ( uint32 _inst_, uint8 rank,uint8 prfs_x, uint16 dcc_adj);
void DDR_PHY_hal_wr_t90_dq_dcc_CSR  ( uint32 _inst_, uint8 prfs_x, uint16 dcc_adj);
void DDR_PHY_hal_wr_wrlvl_dcc_CSR   ( uint32 _inst_, uint8 prfs_x, uint16 dcc_adj);

void DDR_PHY_hal_hpcdc_enable               (BOOT_DDR_STRUCT *ddr, uint8 enable);

void DDR_PHY_hal_cfg_cdcext_slave_rcw  ( uint32 _inst_, uint16 rank, uint32 delay, uint16 coarse_delay, uint16 hp_mode, uint16 prfs_no );
void DDR_PHY_hal_cfg_cdcext_slave_rd   ( uint32 _inst_, uint8  rank, uint8  coarse_delay, uint8  fine_delay, uint8 prfs_no );
void DDR_PHY_hal_get_cdcext_slave_rd   ( uint32 _inst_, uint8  rank, uint8 *coarse_delay, uint8 *fine_delay, uint8 prfs_no );
void DDR_PHY_hal_get_cdc_slave_rd_b ( uint32 _inst_, uint8 rank, uint32 *coarse,  uint32 *fine );

void DDR_PHY_hal_cfg_cdcext_slave_wrlvl( uint32 _inst_, uint16 rank, uint32 delay, uint16 coarse_delay, uint16 hp_mode, uint16 prfs_no );
void DDR_PHY_hal_cfg_cdcext_slave_wr   ( uint32 _inst_, uint8  rank, uint8  coarse_delay, uint8  fine_delay, uint8 prfs_no );
void DDR_PHY_hal_get_cdcext_slave_wr   ( uint32 _inst_, uint8  rank, uint8 *coarse_delay, uint8 *fine_delay, uint8 prfs_no );

void DDR_PHY_hal_cfg_wrlvlext_ctl_dqs_dq_struct_half( uint32 _inst_, uint8 period_index, uint8 dqs_half, uint8 dq_half );

void DDR_PHY_hal_cfg_cdc_slave_rcw( uint32 _inst_, uint32 delay, uint16 coarse_fine_delay, uint16 mode);
uint8 DDR_PHY_hal_sta_rcw_coarse( uint32 _inst_, uint8 rank );
uint8 DDR_PHY_hal_sta_rcw_fine( uint32 _inst_, uint8 rank );
uint8 DDR_PHY_hal_sta_rcw_num_cycle( uint32 _inst_);
void DDR_PHY_RCW_pre_training_init (uint8 ch,  PHY_BUS dq);

void DDR_PHY_hal_cfg_rcw (uint32 _inst_, RCW_EN_DIS rcw_setting);
void DDR_PHY_hal_cfg_full_rcw(uint32 _inst_,  uint8 full_cycle);
void DDR_PHY_rd_mon_status( uint32 _inst_, uint8 *pass );
void DDR_PHY_hal_cfg_rcw_QFI_ext( uint32 _inst_, uint32 tDQSCK_in_full_cycle, uint16 prfs_no );
void DDR_PHY_hal_cfg_rcw_QFI_ext_V2( uint32 _inst_, DDR_CHIPSELECT populated_chipselect,uint8 tDQSCK_perCS_full_cycle[NUM_CS] ,uint16 prfs_no );
void DDR_PHY_hal_cfg_cdc_rcw        ( uint32 _inst_, uint32 enable, uint32 num_cycle );


void DDR_PHY_hal_cfg_cdc_slave_rd   ( uint32 _inst_, uint32  delay, uint16 coarse_delay, uint16 hp_mode, uint8 rank );
void DDR_PHY_hal_cfg_cdc_slave_wrlvl( uint32 _inst_, uint32  delay, uint16 coarse_delay, uint16 hp_mode, uint8 rank );
void DDR_PHY_hal_cfg_cdc_slave_wrlvl_CA( uint32 _inst_, uint16 coarse_delay,uint8 rank);
void DDR_PHY_hal_wrlvl_DQ_burst_BIST ( uint32 _inst_, uint32 loop_count, uint32 clk_freq_khz);
void DDR_PHY_hal_cfg_cdc_slave_wr   ( uint32 _inst_, uint32 delay,  uint16 coarse_delay, uint16 hp_mode, uint8 rank );
void DDR_PHY_hal_cfg_cdc_slave_wr_b( uint32 _inst_,  uint8 rank, uint32 coarse,uint32 fine );
void DDR_PHY_hal_get_cdc_slave_wr_b( uint32 _inst_,  uint8 rank, uint32 *coarse,uint32 *fine );

void DDR_PHY_hal_cfg_pbit_dq_delay  ( uint32 _inst_, uint16 dq_num, uint16 txrx, uint16   sel, uint16 delay );
void DDR_PHY_hal_cfg_pbit_dqs_delay ( uint32 _inst_, uint16   txrx, uint16 rank, uint16 delay );

//Read Vref
uint8 DDR_PHY_hal_get_global_vref_hprx       ( uint32 _inst_);
int DDR_PHY_hal_get_rd_vref_PRFS( uint32 _inst_, uint32 prfs_x, uint8 mp_hp);
void DDR_PHY_hal_cfg_local_vref             ( uint32 _inst_, uint16 cs, uint32 slice, uint32 vref_val ) ;
void DDR_PHY_hal_cfg_global_vref_hprx       ( uint32 _inst_, uint32 vref_val)                           ;
int   DDR_PHY_hal_cfg_global_vref_hprx_PRFS ( uint32 _inst_, uint32 prfs_x, uint8 vref_val)             ;
int   DDR_PHY_hal_cfg_global_vref_mprx_PRFS ( uint32 _inst_, uint32 prfs_x, uint8 vref_val)             ;


void  DDR_PHY_hal_cfg_global_vref_hprx_entry ( uint32 _inst_)                                            ;
void  DDR_PHY_hal_cfg_global_vref_hprx_exit  ( uint32 _inst_)                                            ;
void  DDR_PHY_hal_cfg_global_vref_mprx       ( uint32 _inst_, uint32 vref_val)                           ;
uint8 DDR_PHY_hal_get_global_vref_mprx       ( uint32 _inst_)                                            ;
int   DDR_PHY_hal_dqsdcc_read                ( uint32 _inst_, uint8 rank, int dcc_adj)                   ;

uint32 DDR_PHY_hal_sta_wrlvl_coarse     ( uint32 _inst_, uint8 rank );
uint32 DDR_PHY_hal_sta_wrlvl_fine       ( uint32 _inst_, uint8 rank );
uint32 DDR_PHY_hal_sta_wrlvl_training   ( uint32 _inst_ );
uint32 DDR_PHY_hal_sta_wrlvl_retmr      ( uint32 _inst_, uint8 rank );
uint32 DDR_PHY_hal_sta_wrlvl_half       ( uint32 _inst_, uint8 rank );
uint32 DDR_PHY_hal_sta_wrlvl_full       ( uint32 _inst_, uint8 rank );
void DDR_PHY_hal_cfg_wrlvl_delay_set( uint32 _inst_, uint8 rank, uint32 fine_delay, uint32 coarse_delay );
void DDR_PHY_hal_cfg_cdcext_wrlvl_update( uint32 _inst_, uint8 period_index, uint8 rank, uint32 fine_delay, uint32 coarse_delay );
void DDR_PHY_hal_cfg_cdcext_wrlvl_ca_update( uint32 _inst_, uint8 period_index, uint8 rank, uint32 fine_delay, uint32 coarse_delay );
void DDR_PHY_hal_cfg_wrlvl_set( uint32 _inst_, uint32 rank, uint32 dqs_retmr, uint32 dqs_half_cycle, uint32 dqs_full_cycle );
void DDR_PHY_hal_cfg_wrlvl_retmr( uint32 _inst_, uint32 rank, uint32 dqs_retmr );
void DDR_PHY_hal_cfg_wrlvlext_ctl_update( uint32 _inst_, uint8 period_index, uint8 rank, uint32 retmr, uint32 half_cycle, uint32 full_cycle );
void DDR_PHY_hal_cfg_wrext_ctl_update( uint32 _inst_, uint8 period_index, uint8 rank, uint32 half_cycle );
void DDR_PHY_hal_cfg_wrlvl_dq_set( uint32 _inst_, uint32 rank, uint32 dq_retmr, uint32 dq_half_cycle, uint32 dq_full_cycle );
void DDR_PHY_hal_cfg_wrlvl_half( uint32 _inst_, uint32 rank, uint32 dq_half_cycle );
void DDR_PHY_hal_cfg_wrlvl_full( uint32 _inst_, uint32 rank, uint32 dq_full_cycle );
void DDR_PHY_hal_cfg_wrlvlext_retmr( uint32 _inst_, uint8 period_index, uint8 rank, uint32 retmr);

void DDR_PHY_hal_cfg_wr_dqs_dcc_adj( uint32 _inst_, uint8 rank, uint16 wr_dqs_dcc_adj);

uint32 DDR_PHY_rtn_training_dcc_wrlvl(BOOT_DDR_STRUCT *ddr, uint32 _inst_);
uint32 DDR_PHY_rtn_training_dcc_t90(BOOT_DDR_STRUCT *ddr, uint32 _inst_);
uint32 DDR_PHY_rtn_training_dcc_io_dqs(BOOT_DDR_STRUCT *ddr, uint32 _inst_);

void DDR_PHY_hal_cfg_dq_vref( uint32 _inst_ );
void DDR_PHY_hal_cfg_dq_vref_dqs2dq_max( uint32 _inst_, uint8 rank, uint8 prfs_num, uint8 delay, uint8 coarse_dly );
void DDR_PHY_hal_cfg_dq_vref_dqs2dq_min( uint32 _inst_, uint8 rank, uint8 prfs_num, uint8 delay, uint8 coarse_dly );
void DDR_PHY_hal_cfg_dq_vref_cen_cfg_min( uint32 _inst_, uint8 rank, uint8 prfs_num, uint8 full_cycle, uint8 half_cycle, uint8 retimer );
void DDR_PHY_hal_cfg_dq_vref_cen_cfg_max( uint32 _inst_, uint8 rank, uint8 prfs_num, uint8 full_cycle, uint8 half_cycle, uint8 retimer );

/***************************************************************************************************
 DDR PHY PCC
 ***************************************************************************************************/
void DDR_PHY_rtn_pcc_pre_collapse (BOOT_DDR_STRUCT *ddr, uint8 pcc_mode, uint32 clk_freq_khz);
void DDR_PHY_rtn_pcc_post_collapse (BOOT_DDR_STRUCT *ddr, uint8 pcc_mode, uint32 clk_freq_khz);
void DDR_PHY_hal_pcc_start (BOOT_DDR_STRUCT *ddr, uint8 pcc_mode);
void DDR_PHY_hal_pcc_stop (BOOT_DDR_STRUCT *ddr, uint8 pcc_mode);
void DDR_PHY_hal_pcc_complete (BOOT_DDR_STRUCT *ddr, uint8 pcc_mode, uint32 clk_freq_khz);
uint8  DDR_PHY_hal_get_wr_rank_update_en(uint32 _inst_, uint8 prfs_x);
void   DDR_PHY_hal_cfg_wr_rank_update_en(uint32 _inst_, uint8 prfs_x, uint8 wr_rank_update_en);

void Get_LUT_band_DDRCC (uint32 _inst_, uint32 freq, uint32 *DDRCC_prfs_indx_fmax);

