/*************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY
$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_rd.c#5 $
$DateTime: 2018/06/06 10:48:33 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
09/8/2016  amoussav     Histogram based Read training with support of DCC training algorthm intend to replace previous algorithm 
================================================================================*/
#include "ddrss.h"
#include "ddrss_training.h"
#include "mc.h"
#include <string.h>

// -------------------------------------------------------------------------
// DDR PHY RD DQ-DQS Training 
// -------------------------------------------------------------------------

void Training_DDRSS_RD_DQDQS_LPDDR4(BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz, 
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars
                                    ) 
{
    extern uint8 RD_Training_Switchboard[8][5];
    extern uint8 Triainig_pivot_plots_Switchboard[8][2];
    uint8  ch         = 0;
    uint8  cs         = 0;
    PHY_BUS  dq       = DQ;
    PHY_BUS  ca       = CA;
    uint8 freq_eq_max_training = (RD_Training_Switchboard[prfs_index][_C_PB]==1);
    uint8 num_ch               = ddr->shrm_ddr_ptr->misc.num_channel;
    
    // Switch to boot freq to do a low speed memory write to be used for read back during training
    ddr_external_set_clk_speed (200000);
    

#if DSF_PXI_TRAINING_EN

#else
    MC_All_Periodic_Ctrl (ddr, channel, chip_select, 0/* 1 for enable, 0 for disable */);
    //Enable MC_Auto_refresh
    MC_Periodic_Auto_Refresh_Ctrl(ddr, channel, chip_select, 1);
#endif

    // Memory write at low speed
    for (ch = 0; ch < num_ch; ch++)
    {
      if ((channel >> ch) & 0x1)
      {
        for (cs = 0; cs < NUM_CS; cs++)
        {
          if ((chip_select >> cs) & 0x1)
          {
#if DSF_PXI_TRAINING_EN 
            Update_PXI_Tables(ddr, ch, cs, 0);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
            DDRSS_mem_write_PXI (ch, cs);
          }
        }
      }
    }
        
    // Switch to the training frequency
    ddr_external_set_clk_speed (clk_freq_khz);
    
	// Manual ZQ-CAl 
	MC_ZQ_Calibration (ddr, channel, chip_select);
	
    for (ch = 0; ch < num_ch; ch++)
    {
      if ((channel >> ch) & 0x1)
      {
		  
        for (cs = 0; cs < NUM_CS; cs++)
        {
          //=================================================================
          //=================================================================
          //=================================================================
		  
          if( ( (                                     cs==1  ) &&
                (                  chip_select >> cs) & 0x1  ) && 
          	    (DDRSS_get_wr_rank_update_en(prfs_index,ch )==0) )
          {
              ddr_printf(DDR_NORMAL,"copy READ  CDC from rank 0 to rank 1 in prfs %d \n", prfs_index );
               DDRSS_cpy_cdcext_slave_rd_r0tor1(prfs_index, ch );
          }
          else 
          //=================================================================
          //=================================================================
          //=================================================================
		  
		  if ((chip_select >> cs) & 0x1)
          {
       ddr_printf(DDR_NORMAL,"\n\n\n>================    READ TRAINING START    ================<\n" );
       ddr_printf(DDR_NORMAL,"Frequency, %d, Channel, %d, \n\n",clk_freq_khz, ch );
            //ddr_printf(DDR_NORMAL, "START: Read training on Ch: %u Rank: %u\n", ch, cs);

         
            if(RD_Training_Switchboard[prfs_index][_C_PB]==1)//highest frequency
            {
                #if DSF_RD_DQDQS_TRAINING_LOG_BEFORE
                ddr_printf(DDR_NORMAL, "\nBEFORE_DDRSS_rd_dqdqs_1D_pbcf, rank, %d\n",  cs);
                DDRSS_rd_dqdqs_log (ddr,
                                    ch, 
                                    cs, 
                                    ca,
                                    dq,
                                    training_params_ptr,
                                    (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                    clk_freq_khz,
                                    prfs_index);
                #endif
                ddr_printf(DDR_NORMAL, "\nDDRSS_rd_dqdqs_1D_pbcf, rank, %d\n",  cs);
                DDRSS_rd_dqdqs_1D_pbcf (ddr, 
                                ch, 
                                cs, 
                                ca,
                                dq,
                                training_params_ptr,
                                (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                clk_freq_khz,
                                prfs_index,
                                freq_eq_max_training);
                
                                        
                #if DSF_RD_DQDQS_TRAINING_LOG_AFTER
                ddr_printf(DDR_NORMAL, "\nAFTER_DDRSS_rd_dqdqs_1D_pbcf, rank, %d\n",  cs);
                DDRSS_rd_dqdqs_log (ddr, 
                                    ch, 
                                    cs,
                                    ca,
                                    dq,
                                    training_params_ptr,
                                    (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                    clk_freq_khz,
                                    prfs_index);
                #endif   
            }//if(RD_Training_Switchboard[prfs_index][_C_PB]==1)

          }//if cs
		  else
		  {
		                ddr_printf(DDR_NORMAL, "\n dual_rank if for read, ch %d rank, %d\n",ch,  cs);
	  
		  }
		  
		  
		  
		  
        }//cs

        if(RD_Training_Switchboard[prfs_index][_V_FSP1]==1)//((freq_eq_max_training) && (clk_freq_khz >= 900000))
        {
          #if DSF_RD_HPVREF_TRAINING_LOG_BEFORE
          DDRSS_rd_Vref_hprx_log( ddr, 
                                  ch, 
                                  chip_select, 
                                  ca,
                                  dq,
                                  training_params_ptr,
                                  (ddrss_rdwr_dqdqs_local_vars *)local_vars
                                  );          
          #endif  
          ddr_printf(DDR_NORMAL, "\nDDRSS_rd_Vref_1D_hprx, both rank, \n");
          DDRSS_rd_Vref_1D_hprx( ddr, 
                              ch, 
                              chip_select, 
                              ca,
                              dq,
                              training_params_ptr,
                              (ddrss_rdwr_dqdqs_local_vars *)local_vars
                              );
         DDRSS_propagate_global_vref_hprx_to_all_PRFS (  ch);
         
          #if DSF_RD_HPVREF_TRAINING_LOG_AFTER 
          DDRSS_rd_Vref_hprx_log( ddr, 
                                  ch, 
                                  chip_select, 
                                  ca,
                                  dq,
                                  training_params_ptr,
                                  (ddrss_rdwr_dqdqs_local_vars *)local_vars
                                  );          
          #endif                                                      
        }// hprx
  
		for (cs = 0; cs < NUM_CS; cs++)
        {

          //=================================================================
          //=================================================================
          //=================================================================
          if( ( (                                     cs==1  ) &&
                (                  chip_select >> cs) & 0x1  ) && 
          	    (DDRSS_get_wr_rank_update_en(prfs_index,ch )==0) )
	
          {
              ddr_printf(DDR_NORMAL,"copy READ  CDC from rank 0 to rank 1 in prfs %d \n", prfs_index );
               DDRSS_cpy_cdcext_slave_rd_r0tor1(prfs_index, ch );
          }
          else 
          //=================================================================
          //=================================================================
          //=================================================================


			  if ((chip_select >> cs) & 0x1)
          {
            if(RD_Training_Switchboard[prfs_index][_C_CF]==1)//high frequency not the highest
            {
                #if DSF_RD_DQDQS_TRAINING_LOG_BEFORE
                ddr_printf(DDR_NORMAL, "\nBEFORE_DDRSS_rd_dqdqs_1D_cf, rank, %d\n",  cs);
                DDRSS_rd_dqdqs_log (ddr,
                                    ch, 
                                    cs, 
                                    ca,
                                    dq,
                                    training_params_ptr,
                                    (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                    clk_freq_khz,
                                    prfs_index);
                #endif
                ddr_printf(DDR_NORMAL, "\nDDRSS_rd_dqdqs_1D_cf, rank, %d\n",  cs);
				
                if (prfs_index==7)
				{
			      DDRSS_rd_2D_scrn (ddr, 
                                ch, 
                                cs, 
                                ca,
                                dq,
                                training_params_ptr,
                                (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                clk_freq_khz,
                                prfs_index,
                                freq_eq_max_training);
				}				
                DDRSS_rd_dqdqs_1D_cf (ddr, 
                                ch, 
                                cs, 
                                ca,
                                dq,
                                training_params_ptr,
                                (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                clk_freq_khz,
                                prfs_index,
                                freq_eq_max_training);
                
                                        
                #if DSF_RD_DQDQS_TRAINING_LOG_AFTER
               ddr_printf(DDR_NORMAL, "\nAFTER_DDRSS_rd_dqdqs_1D_cf, rank, %d\n",  cs);
               DDRSS_rd_dqdqs_log (ddr, 
                                    ch, 
                                    cs, 
                                    ca,
                                    dq,
                                    training_params_ptr,
                                    (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                    clk_freq_khz,
                                    prfs_index);
                #endif   
            }//if(RD_Training_Switchboard[prfs_index][_C_PB]==1)
          }//if cs
        }//cs
  
        if(RD_Training_Switchboard[prfs_index][_V_FSP0]==1)
        {
          ddr_printf(DDR_NORMAL, "DDRSS_rd_Vref_1D_mprx, both rank, \n");
          DDRSS_rd_Vref_1D_mprx( ddr, 
                              ch, 
                              chip_select, 
                              ca,
                              dq,
                              training_params_ptr,
                              (ddrss_rdwr_dqdqs_local_vars *)local_vars
                              );
         DDRSS_propagate_global_vref_mprx_to_all_PRFS (ch);
        }//mprx	  
 
      //ddr_printf(DDR_NORMAL,"\nFrequency, %d, Channel, %d, \n",clk_freq_khz, ch );
      ddr_printf(DDR_NORMAL,">================    READ TRAINING END      ================<\n" );
      }//if ch

    }//ch  
        
      #if DSF_RD_DQDQS_COARSE_FINE_PLOT_PIVOT
        if (Triainig_pivot_plots_Switchboard[prfs_index][_RD_P]==1)
      {
        uint64 tmp = 0;
        DDRSS_rd_dqdqs_coarse_fine_plot_pivot (ddr, 
                                               training_params_ptr, 
                                               clk_freq_khz,
                                               &tmp);
      }  // DSF_RD_DQDQS_COARSE_FINE_PLOT  
      #endif       
    //Disable MC_Auto_refresh
    MC_Periodic_Auto_Refresh_Ctrl(ddr, channel, chip_select, 0);     
  return;  
}

// ============================================================================================================================
// ======================================      R E A D  DQDQS  T R A I N I N G                =================================
// ============================================================================================================================
boolean DDRSS_rd_dqdqs_1D_pbcf(BOOT_DDR_STRUCT             *ddr, 
                       uint8                        ch, 
                       uint8                        cs, 
                       PHY_BUS                      ca,
                       PHY_BUS                      dq,
                       training_params_t            *training_params_ptr,
                       ddrss_rdwr_dqdqs_local_vars  *local_vars,
                       uint32                       clk_freq_khz,
                       uint8                        prfs_index,
                       uint8                        freq_eq_max_training
                       )
{
    // ============================================================================================================================
    // ======================================      D E F I N I T I O N S                          =================================
    // ============================================================================================================================
    int                             cdc_edge = 0 ;
    int                           dq_dcc_adj = 0 ;
    int                           dq_dcc_abs = 0 ;
    int                           dqsdcc_adj = 0 ;
    uint32     perbit[NUM_DQ_PCH][BYTE_SIZE] = {{0}} ;
    uint32                         rd_offset = 0 ;
    uint32                 reg_offset_ddr_ca = 0;    
    uint32                         mv_offset = 0 ;
    uint32                             slack = 0 ;
    extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
    extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    extern uint8 RD_Training_Switchboard[8][5];
    int                                 rf_x = 0 ;
    uint8                           loop_cnt = 0 ;
    uint8                                bit = 0 ;
    uint16                              knob = 0 ;
    uint16                        sweep_size = 0 ;
    uint8                              phy_x = 0 ;
    uint8                       remapped_bit = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint32        pr_max_midp[3][NUM_DQ_PCH] = {{0},{0},{0}};
    uint32        pr_max_eyew[3][NUM_DQ_PCH] = {{0},{0},{0}};
    uint32        pr_max_strt[3][NUM_DQ_PCH] = {{0},{0},{0}};
    uint32        pr_min_strt[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_min_strt = 199;
    uint32        pr_min_stop[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_midp[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
      uint32            fail_pat = 0;
    uint32        midp[NUM_DQ_PCH]={0};
    uint16        dcc_csr = 0;

   uint32   pr_bus_eyew[NUM_DQ_PCH]  = {0,0};
   uint32   pr_bus_skwr[NUM_DQ_PCH]  = {0,0};
#if DSF_RD_DQDQS_TRAINING_LOG_IN                
   uint32        pr_min_eyew[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
   uint32        pr_max_stop[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
   uint32   pr_x                     =  0   ;
   char     pr_string[4][25]  ={
                                   " passing region start",
                                   " passing region stop",
                                   " passing region midp",
                                   " passing region eyew"
                                    };
#endif 
    //uint32    read_back_pattern[DQ_TRAINING_PATTERN_SIZE] = {0};
    uint32  allbit         = ALL_DQ_PASS_PAT ;
    
    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================
    
    // Set DDR Base address registers
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ1_DDR_PHY_OFFSET;
    reg_offset_ddr_ca     =  REG_OFFSET_DDR_PHY_CH(ch)+ CA0_DDR_PHY_OFFSET;
      if (clk_freq_khz <= HIGHEST_MPVREF_PRFS_FREQ)
      {
        DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca,training_params_ptr->rd_dqdqs.coarse_vref_start_mprx);
      }        

      for (phy_x = 0; phy_x < dq; phy_x++)
      {
        // Initialize the global VREF
        DDR_PHY_hal_cfg_global_vref_hprx(reg_offset_ddr_ca,training_params_ptr->rd_dqdqs.coarse_vref_start_hprx);                     

        for (bit = 0; bit < BYTE_SIZE; bit++)
        {                  
          remapped_bit  = bit_remapping_bimc2phy_DQ[ch][phy_x][bit];
          DDR_PHY_hal_cfg_pbit_dq_delay (reg_offset_ddr_dq[phy_x],
                                         remapped_bit,
                                         0, // 1 for TX
                                         cs, //cs 
                                         READ_PERBIT_INIT);
                                           
          DDR_PHY_hal_cfg_local_vref(reg_offset_ddr_dq[phy_x], 
                                     cs, 
                                     remapped_bit,  
                                     0x8 );
                                           
        }//bit  
        
        // Set the local VREF for DQS
        DDR_PHY_hal_cfg_local_vref(reg_offset_ddr_dq[phy_x], cs, 0xA,  0x8 );
        
      }//phy_x
      rd_offset=READ_PERBIT_INIT; //COARSE AND FINE OFFSET

    
    //clearing the histogram initializing to Zero
    memset(local_vars->failcount_histogram, 0,3 * NUM_DQ_PCH * PINS_PER_PHY * HISTOGRAM_SIZE);
    memset(local_vars->pass_reg           , 0,3 * NUM_DQ_PCH * PINS_PER_PHY * 4);
    
    // ======================================      cdc ref                   =================================
    DDRSS_cdcref_init(local_vars->cdc_ref);
    // ============================================================================================================================
#if DSF_PXI_TRAINING_EN 
    Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif

    // ============================================================================================================================
    // ======================================     DQ RD     C D C           S W E E P I N G       =================================
    // ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
    // ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size_rd(clk_freq_khz);

    for (knob = 0;  knob < sweep_size;  knob++)
    {
        //  =================================Set knob start
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            //HWIO_OUTX ( reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG ,local_vars->cdc_ref[knob][CDC_L]);
            //ddr_printf (DDR_NORMAL, "Address %6X, Coarse %6X,\n",reg_offset_ddr_dq[phy_x],local_vars->cdc_ref[knob][COARSE_L]);
            //ddr_printf (DDR_NORMAL, "Address %6X, Fine   %6X,\n",reg_offset_ddr_dq[phy_x],local_vars->cdc_ref[knob][FINE_L]);
            DDR_PHY_hal_cfg_cdc_slave_rd(   reg_offset_ddr_dq[phy_x], 
                                            local_vars->cdc_ref[knob][COARSE_L], 
                                            0x1, // 0 for fine_delay 1 for coarse 
                                            HP_MODE, 
                                            cs);   
        
            DDR_PHY_hal_cfg_cdc_slave_rd(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][FINE_L], 
                                            0x0, // 0 for fine_delay 1 for coarse 
                                            HP_MODE, 
                                            cs);   
        }//phy_x
        //  =================================Set knob end
        for (loop_cnt = training_params_ptr->rd_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
        {
            fail_pat    = DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
            allbit      = DDRSS_fail2hist( fail_pat , local_vars->failcount_histogram, knob);
            
             if(allbit == ALL_DQ_FAIL_PAT) break;
        }//loop count
    }  //End of knob loop
// ============================================================================================================================
// ======================================      calculation                   =================================
// ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            for (rf_x = 0; rf_x < 3; rf_x++)
            {
                //remapped_bit  =  bit_remapping_bimc2phy_DQ  [ch][phy_x][bit];
                DDRSS_passing_region_scan(&(local_vars->pass_reg[rf_x][phy_x][bit][STRT]),
                                          &(local_vars->pass_reg[rf_x][phy_x][bit][STOP]),
                                           local_vars->failcount_histogram [rf_x][phy_x][bit],
                                           0,sweep_size);
                local_vars->pass_reg[rf_x][phy_x][bit][MIDP]  = ((local_vars->pass_reg[rf_x][phy_x][bit][STOP]  + local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) >>1 ); // midpoint equal to average
                local_vars->pass_reg[rf_x][phy_x][bit][EYEW]  = (local_vars->pass_reg[rf_x][phy_x][bit][STOP]   - local_vars->pass_reg[rf_x][phy_x][bit][STRT] )  ; // eywidth  equal to stop - start
                if ( pr_max_midp[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_max_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_max_eyew[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_max_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_min_stop[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_min_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
                if ( pr_max_strt[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_max_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
                if ( pr_min_midp[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_min_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_min_strt[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_min_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
#if DSF_RD_DQDQS_TRAINING_LOG_IN                
                if ( pr_min_eyew[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_min_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_max_stop[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_max_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
#endif
                if ( local_vars->pass_reg[rf_x][phy_x][bit][EYEW] == 0 ) {
                     ddr_printf(DDR_ERROR, "\nDDR ABORT: RD 1D PBCF ZERO EYE WIDTH, Frequency = %d, Channel = %d, Rank = %d, Byte = %d, Bit = %d\n", clk_freq_khz, ch, cs, phy_x, bit );
                     ddr_abort();
                }
            }//rf_x
        }//bit
        // ============================================================================================================================
        // ==========================================             S P E C   C h e c k            ======================================
        // ============================================================================================================================
        pr_bus_eyew[phy_x]  =local_vars->cdc_ref[pr_min_stop[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[2][phy_x]][DELAY_L ];
        pr_bus_skwr[phy_x]  =local_vars->cdc_ref[pr_max_midp[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[2][phy_x]][DELAY_L ];
        //ddr_printf (DDR_NORMAL, "\n pr_max_eyew[2][dq%d]=, %4d,"  ,phy_x,local_vars->cdc_ref[pr_max_eyew[2][phy_x]][DELAY_L]);
        //ddr_printf (DDR_NORMAL, "\n pr_bus_skwr[dq%d]   =, %4d,\n",phy_x,pr_bus_skwr[phy_x]);
		
        // ============================================================================================================================
        // ========================================== D U T Y   C Y C L E   D I S T O R T I O N  ======================================
        // ============================================================================================================================
        if(RD_Training_Switchboard[prfs_index][_DCC]==1)
        {
            dq_dcc_adj                                               =  (pr_max_eyew[0/*even*/][phy_x]-pr_max_eyew[1/*ODD*/][phy_x]);
            dqsdcc_adj                                               =   pr_max_midp[0/*even*/][phy_x]-pr_max_midp[1/*ODD*/][phy_x];
            cdc_edge   = DDRSS_wich_phase(reg_offset_ddr_dq[phy_x],cs,dqsdcc_adj);
            dcc_csr = DDRSS_rd_dcc2csr   (dqsdcc_adj);
            DDR_PHY_hal_dqsdcc_read_CSR (reg_offset_ddr_dq[phy_x],cs,dcc_csr);
           //cdc_edge   =  DDR_PHY_hal_dqsdcc_read (reg_offset_ddr_dq[phy_x],cs,dqsdcc_adj);
		   
		   dq_dcc_abs=(dq_dcc_adj<0) ? -dq_dcc_adj :dq_dcc_adj;

        #if DSF_SMALL_EYE_DETECTION_LOG
        if (prfs_index == 7)
        {
            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 5: RD DQ DCC = %d, Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", dq_dcc_adj, clk_freq_khz, ch, cs, phy_x );
        }
        #endif
        
        if((dq_dcc_abs >=   training_params_ptr->rd_dqdqs.max_dq_dcc) && (prfs_index==7) && (training_params_ptr->rd_dqdqs.max_dq_dcc_enable==1))
          {
             ddr_printf(DDR_ERROR, "\n SCREEN 5: RD DQ DCC SCREEN  Abort (dq_dcc_adj = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", dq_dcc_adj, clk_freq_khz, ch, cs, phy_x );
             training_params_ptr->small_eye_abort = 1;
          }
		   
		}   
             pr_min_min_strt= (pr_min_strt[1][phy_x] <pr_min_strt[0][phy_x]) ? pr_min_strt[1][phy_x] : pr_min_strt[0][phy_x];
            slack = rd_offset -  (pr_max_midp[cdc_edge][phy_x]- pr_min_midp[cdc_edge][phy_x]);
          /*   if( (   ( pr_min_min_strt-1 )    < slack ) && //panks
			    (   ( pr_min_min_strt-3 )    >  0    )
			  )
            {
               mv_offset =pr_min_min_strt-3 ;
           }
            else
            {
               mv_offset = slack;
            } */
			 mv_offset = (pr_min_min_strt < slack)? pr_min_min_strt : slack;
		  	 mv_offset = (mv_offset <= 3) ? mv_offset : (mv_offset - 3); //extra margin for left edge.
            midp[phy_x] = pr_min_midp[cdc_edge][phy_x]  -mv_offset;

        
        // ============================================================================================================================
        // Write the trained coarse and fine center to the active CDC register
        DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x], 
                                    local_vars->cdc_ref[midp[phy_x] ][COARSE_L],
                                    0x1,  // 0 for fine_delay 1 for coarse 
                                    HP_MODE, 
                                    cs);   
                                    
         DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x], 
                                    local_vars->cdc_ref[midp[phy_x] ][FINE_L],
                                    0x0,  // 0 for fine_delay 1 for coarse 
                                    HP_MODE, 
                                    cs);     
                                    
        // ============================================================================================================================
        // Write the trained coarse and fine to the EXT registers
        DDR_PHY_hal_cfg_cdcext_slave_rd(reg_offset_ddr_dq[phy_x] ,
                                        cs,//cs
                                        local_vars->cdc_ref[midp[phy_x] ][COARSE_L],
                                        local_vars->cdc_ref[midp[phy_x] ][FINE_L  ],
                                        prfs_index
                                        );

        // ============================================================================================================================
        // setting perbit cdc only for maximum frequency

        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            perbit[phy_x][bit]        = rd_offset   - mv_offset -   (local_vars->pass_reg[cdc_edge][phy_x][bit][MIDP]  -pr_min_midp[cdc_edge][phy_x]  )  ;
            if (perbit[phy_x][bit] > 0xf){ perbit[phy_x][bit]=0xf;}
            remapped_bit  =        bit_remapping_bimc2phy_DQ[ch][phy_x][bit];
            //ddr_printf (DDR_NORMAL,"RD Byte %d PHY bit %d Per bit %d\n",phy_x,remapped_bit,perbit[bit]);
            DDR_PHY_hal_cfg_pbit_dq_delay(reg_offset_ddr_dq[phy_x],
                                            remapped_bit,
                                            0, // 1 for Tx
                                            cs,//cs
                                            perbit[phy_x][bit]);
        }
            //ddr_printf (DDR_NORMAL,"\n");
    }//phy_x
    
#if DSF_RD_DQDQS_TRAINING_LOG_IN                
// ============================================================================================================================
// ======================================              L O G  P R I N T                       =================================
// ============================================================================================================================
#if DSF_RD_DQDQS_TRAINING_LOG_HIST_IN
      ddr_printf(DDR_NORMAL,"\nFrequency, %d, Channel, %d, \n",clk_freq_khz, ch );
    for (knob=0; knob < sweep_size-1; knob++)
    {
        for (phy_x = 0; phy_x < dq; phy_x++)
        {
            ddr_printf (DDR_NORMAL, "DQ%d,",phy_x);
            for (rf_x = 2; rf_x >= 0; rf_x--)
            {
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    if(knob==local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ){ddr_printf (DDR_NORMAL, " X,");}
                    else{ddr_printf (DDR_NORMAL, "%2d,",local_vars->failcount_histogram[rf_x][phy_x][bit][knob]);}
                }
                     if (         knob == pr_min_strt[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_S,");}
                else if (         knob == pr_max_strt[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_S,");}
                else if (         knob == pr_min_midp[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_M,");}
                else if (         knob == pr_max_midp[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_M,");}
                else if (         knob == pr_min_stop[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_E,");}
                else if (         knob == pr_max_stop[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_E,");}
                else if ( local_vars->cdc_ref[knob  ][FINE_L] == 0 ) {ddr_printf ( DDR_NORMAL, "      0,");}
                else                                                 {ddr_printf ( DDR_NORMAL, "       ,");}
            }
            ddr_printf(DDR_NORMAL,"%3d,%2d,%2d,%4d,%3X,",knob,
                                     local_vars->cdc_ref[knob][COARSE_L],
                                     local_vars->cdc_ref[knob][FINE_L  ],
                                     local_vars->cdc_ref[knob][DELAY_L ],
                                     local_vars->cdc_ref[knob][CDC_L   ]);
        }//phy_x
        ddr_printf(DDR_NORMAL, "\n");
    }//knob
 #endif   

    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        ddr_printf (DDR_NORMAL,"\n DQ_%d,midp, %d,coarse, %d, fine, %d,\n"
                        ,phy_x,midp[phy_x]
                        ,local_vars->cdc_ref[midp[phy_x] ][COARSE_L]
                        ,local_vars->cdc_ref[midp[phy_x] ][FINE_L]);
            // ============================================================================================================================
    
            for (bit = 0; bit < BYTE_SIZE; bit++)
            {
                ddr_printf (DDR_NORMAL, "%3d,",bit);
            }
            ddr_printf (DDR_NORMAL, " bit,\n");
            for (bit = 0; bit < BYTE_SIZE; bit++)
            {
                remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
                ddr_printf (DDR_NORMAL, "%3d,",remapped_bit);
            }
            ddr_printf (DDR_NORMAL, " PHY_bit,\n");
            // ============================================================================================================================
            for (pr_x = 0; pr_x < 4; pr_x++)
            {
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    ddr_printf (DDR_NORMAL, "%3d,",local_vars->pass_reg[2][phy_x][bit][pr_x]);
                }
                    ddr_printf (DDR_NORMAL, "%s\n",pr_string[pr_x]);
            }
            // ============================================================================================================================
            for (bit = 0; bit < BYTE_SIZE; bit++)
            {
                ddr_printf (DDR_NORMAL, "%3d,",perbit[phy_x][bit]);
            }
            // ============================================================================================================================
            ddr_printf (DDR_NORMAL, "%s\n"," perbit Delay,");
        ddr_printf (DDR_NORMAL, "pr_bus_eyew[%d](ps)=, %d ,\n",phy_x, pr_bus_eyew[phy_x]);
        ddr_printf (DDR_NORMAL, "pr_bus_skwr[%d](ps)=, %d ,\n",phy_x, pr_bus_skwr[phy_x]);
    }//phy_x
#endif 
    return(1);
}// END OF FUNCTION
// ============================================================================================================================
// ======================================      R E A D  DQDQS  T R A I N I N G                =================================
// ============================================================================================================================
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
                       )
{
    // ============================================================================================================================
    // ======================================      D E F I N I T I O N S                          =================================
    // ============================================================================================================================


    extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
    extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    uint8                           loop_cnt = 0 ;
    uint16                              knob = 0 ;
    uint16                        sweep_size = 0 ;
    uint8                              phy_x = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
     uint8     fail_histogram[3][NUM_DQ_PCH][HISTOGRAM_SIZE]={{{0}}};
  FAILCELL                          fail_cell;
    uint8     pr[NUM_DQ_PCH][NUM_PR_PARAMS];
    uint8 scaled_prfs_indx                   = 0;
    uint8 scale                              = 0;
    uint32 scaled_cdc_delay_in_ps            = 0;    
    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================

    // Set DDR Base address registers
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ1_DDR_PHY_OFFSET;


    //clearing the histogram initializing to Zero
    memset(fail_histogram, 0,3 * NUM_DQ_PCH *HISTOGRAM_SIZE);
    DDRSS_cdcref_init(local_vars->cdc_ref);//cdc ruler init

    // ============================================================================================================================
#if DSF_PXI_TRAINING_EN
    Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif

    // ============================================================================================================================
    // ======================================     DQ RD     C D C           S W E E P I N G       =================================
    // ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
    // ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size_rd(clk_freq_khz);
    for (knob = 0;  knob < sweep_size;  knob++)
    {
        //  =================================Set knob start
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            DDR_PHY_hal_cfg_cdc_slave_rd(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][COARSE_L],
                                            0x1, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            cs);

            DDR_PHY_hal_cfg_cdc_slave_rd(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][FINE_L],
                                            0x0, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            cs);
        }//phy_x
        fail_cell.w=0;
        for (loop_cnt = training_params_ptr->wr_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
        {
                   fail_cell.w |=  DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
        }//loop count
		for (phy_x = 0; phy_x < dq; phy_x++)
        {
            fail_histogram [0][phy_x][knob]=fail_cell.b[0][phy_x] ;
            fail_histogram [1][phy_x][knob]=fail_cell.b[1][phy_x] ;
            fail_histogram [2][phy_x][knob]=fail_cell.b[0][phy_x] | fail_cell.b[1][phy_x];
        }//phy_x
    }//knob
    // ============================================================================================================================
    // ======================================      calculation                   =================================
    // ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        DDRSS_passing_region_scan(  &pr[phy_x][STRT],
                                    &pr[phy_x][STOP],
                                    fail_histogram[2][phy_x],
                                    0,sweep_size);
        pr[phy_x][MIDP]   =     (pr[phy_x][STOP]  + pr[phy_x][STRT])>>1 ;
        pr[phy_x][EYEW]   =      pr[phy_x][STOP]  - pr[phy_x][STRT];
        // ============================================================================================================================
        // ==========================================             S P E C   C h e c k            ======================================
        // ============================================================================================================================
        
        if(pr[phy_x][EYEW] == 0)
          {
             ddr_printf(DDR_ERROR, "\nDDR ABORT: RD 1D CF ZERO EYE WIDTH, Frequency = %d, Channel = %d, Rank = %d, Byte = %d, Bit = %d\n", clk_freq_khz, ch, cs, phy_x );
             ddr_abort();
          }
          
        #if DSF_SMALL_EYE_DETECTION_LOG
        ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 1: RD MINIMUM EYE WIDTH = %d (ps) = %d (f steps), Vref check point = %d (step) = - (mV), Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", pr[phy_x][EYEW] * FINE_STEP_IN_PS, pr[phy_x][EYEW], DDRSS_get_active_rd_vref(ch, phy_x, clk_freq_khz), clk_freq_khz, ch, cs, phy_x);	
        #endif
        if((pr[phy_x][EYEW] < training_params_ptr->rd_dqdqs.min_eye_width) && (training_params_ptr->rd_dqdqs.min_eye_width_enable==1))
          {
             ddr_printf(DDR_ERROR, "\n SCREEN 1: RD MINIMUM EYE WIDTH Abort (EYEW = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", pr[phy_x][EYEW], clk_freq_khz, ch, cs, phy_x );
             training_params_ptr->small_eye_abort = 1;
          }
		
        // ============================================================================================================================
        // Write the trained coarse and fine center to the active CDC register
        DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x],
                                    local_vars->cdc_ref[   pr[phy_x][MIDP]   ][COARSE_L],
                                    0x1,  // 0 for fine_delay 1 for coarse
                                    HP_MODE,
                                    cs);
        DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x],
                                    local_vars->cdc_ref[   pr[phy_x][MIDP]   ][FINE_L],
                                    0x0,  // 0 for fine_delay 1 for coarse
                                    HP_MODE,
                                    cs);
        // Write the trained coarse and fine to the EXT registers
        DDR_PHY_hal_cfg_cdcext_slave_rd(reg_offset_ddr_dq[phy_x] ,
                                        cs,//cs
                                        local_vars->cdc_ref[   pr[phy_x][MIDP]   ][COARSE_L],
                                        local_vars->cdc_ref[   pr[phy_x][MIDP]   ][FINE_L  ],
                                        prfs_index
                                        );
     }//phy_x
	 
	 
#if DSF_RD_DQDQS_TRAINING_LOG_IN                
// ============================================================================================================================
// ======================================              L O G  P R I N T                       =================================
// ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        ddr_printf(DDR_NORMAL, "    strt= %d, stop= %d , eyew= %d (%d ps) ,\n",
                pr[phy_x][STRT],
                pr[phy_x][STOP],
                pr[phy_x][EYEW]
				,local_vars->cdc_ref[   pr[phy_x][EYEW]   ][DELAY_L]);
        ddr_printf (DDR_NORMAL  , "    DQ%d,midp, %d, coarse, %d, fine, %d, Delay(ps), %d,  \n"
                                , phy_x
                                , pr[phy_x][MIDP]
                                , local_vars->cdc_ref[ pr[phy_x][MIDP] ][COARSE_L]
                                , local_vars->cdc_ref[ pr[phy_x][MIDP] ][  FINE_L]
                                , local_vars->cdc_ref[ pr[phy_x][MIDP] ][ DELAY_L]);
        // ============================================================================================================================
        ddr_printf (DDR_NORMAL, "    pr_bus_eyew[%d](ps)=, %d ,\n",phy_x, pr[phy_x][EYEW]);
    }//phy_x
#endif

    // ============================================================================================================================
    // ======================================              scaling                     =================================
    // ============================================================================================================================

    if(clk_freq_khz < LOWEST_TRAINED_FPRS_FREQ)
    {
      for (scale = 1; scale <= NUM_SCALE_BANDS; scale++)
      {
        // Calculate the scaled PRFS index
        scaled_prfs_indx = prfs_index - scale ;
                                             
        for (phy_x = 0; phy_x < dq; phy_x++)
        {   
          scaled_cdc_delay_in_ps = (clk_freq_khz * local_vars->cdc_ref[ pr[phy_x][MIDP] ][ DELAY_L])/SCALE_FREQ(scale-1);
#if DSF_RD_DQDQS_TRAINING_LOG_IN                   
          ddr_printf(DDR_NORMAL,"Source delay %d ps  Target delay %d ps\n",
                                 local_vars->cdc_ref[ pr[phy_x][MIDP] ][ DELAY_L],scaled_cdc_delay_in_ps); 
#endif
                                 
          DDRSS_RD_midpoint_to_CDC (ddr,
                                    scaled_cdc_delay_in_ps,
                                    clk_freq_khz,
                                    ch,
                                    cs,
                                    phy_x,
                                    scaled_prfs_indx
                                   );
        } //phy_x
      }
    }    	 
	 
    return(1);
}// END OF FUNCTION


#if DSF_RD_DQDCC_EN

void DDRSS_set_ca_wr_dcc_to_compensate_rd_dq_dcc
                                  (BOOT_DDR_STRUCT                   *ddr, 
                                   uint8                        ch, 
                                   DDR_CHIPSELECT               chip_select,
                                   PHY_BUS                      ca,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars
                                  )
{
    int                               ca_dcc = 0;
    uint32                   io_ext_register = 0;
    uint8                              phy_x = 0 ;
    uint8                                 cs = 0 ;
    uint8                               ca_x = 0 ;
    uint8                          prfs_band = 0 ;
    uint32     reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    int                  max_dcc[NUM_CA_PCH] = {0};
    int                  min_dcc[NUM_CA_PCH] = {199};
// ============================================================================================================================
// ======================================      I N I T I A L I Z A T I O N                    =================================
// ============================================================================================================================
    reg_offset_ddr_ca[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;

// ============================================================================================================================
// ======================================              P R O C E S S I N G                    =================================
// ============================================================================================================================

    for(cs=0;cs<NUM_CS;cs++)
    {
        if ((chip_select >> cs) & 0x1)
        {
            for (phy_x = 0; phy_x < dq; phy_x++)
            {
                ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                // TODO: Read DCC from registers
                // ================================================================================================================
                //if (max_dcc[ca_x]   <   training_data_ptr->results.temp.rd_dq_dcc[ch][cs][phy_x])
                //{
                //    max_dcc[ca_x]   =   training_data_ptr->results.temp.rd_dq_dcc[ch][cs][phy_x];
                //}
                //if (min_dcc[ca_x]   >   training_data_ptr->results.temp.rd_dq_dcc[ch][cs][phy_x])
                //{
                //    min_dcc[ca_x]   =   training_data_ptr->results.temp.rd_dq_dcc[ch][cs][phy_x];
                //}
                // ================================================================================================================

            }
        }
    }
// ============================================================================================================================
// ======================================              S E T   A N D   S T O R E              =================================
// ============================================================================================================================
    
    for (ca_x = 0; ca_x < ca; ca_x++)
    {                  
        ca_dcc=(max_dcc[ca_x] + min_dcc[ca_x])>>1;
        if( ca_dcc >  2){ca_dcc =  2;}
        if( ca_dcc < -2){ca_dcc = -2;}
        if( ca_dcc < 0 )
        {
            ca_dcc= -ca_dcc;
            ca_dcc=  ca_dcc <<4;
        }
           
        for (prfs_band = prfs_indx; prfs_band < NUM_PRFS_BANDS; prfs_band++)
        {
          // TODO: Read DCC from register
          //io_ext_register  = ((training_data_ptr->results.dcc.iodqs_stat_ca [prfs_indx][ch][ca_x]) << 10);
          HWIO_OUTX (reg_offset_ddr_ca[ca_x] + (4 * prfs_band), DDR_PHY_DDRPHY_DCCEXT_PADDCC_0_CTL_CFG, io_ext_register);
        }            
           
        //SET_THE_DCC(reg_offset_ddr_ca[ca_x], training_data_ptr->results.wr_dqdqs.wr_io_dcc[ch][ca_x]);)
    }    
    
}
#endif
#if DSF_RD_HPVREF_TRAINING_EN
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
                                   )
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================
  extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
  extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    uint8                                 cs = 0 ;
    int         lvref[NUM_CS][NUM_DQ_PCH][BYTE_SIZE] = {{{0}}} ;
    uint32                g_vref[NUM_CA_PCH] = {0};
    int                                 rf_x = 0 ;
    uint8                           loop_cnt = 0 ;
    uint8                                bit = 0 ;
    uint16                              vref = 0 ;
    uint8                              phy_x = 0 ;
    uint8                       remapped_bit = 0 ;
    uint8                     min_eye_height = 0xff;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint32     reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint32                  midv[NUM_DQ_PCH] = {0};
    uint32        pr_max_midp[3][NUM_CA_PCH] = {{0},{0},{0}};
    uint32        pr_min_midp[3][NUM_CA_PCH] = {{199},{199},{199}};
    uint8                               ca_x = 0 ;
    uint8                     vref_max_value = 0;
    uint8                   vref_start_value = 0;
    uint8           decouple_bit[NUM_DQ_PCH] = {0};   
    uint8                        ioqual_vref = 0x10;
    uint32      fail_pat=0;
   uint32    csr_temp[4]={0};
//    uint8 vref_sweep_lut[2][44] = {   //0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43
//                                      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  10,  10,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17},
//                                      { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9}
//                            } ;


uint8 vref_sweep_lut[2][61] = {//0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60
                               { 0,0,0,0,0,0,0,0,0,0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9,10,10,10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,16,16,16,17,17,17},
                               { 0,1,2,3,4,5,6,7,8,9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9}
                              };


//uint8 vref_sweep_lut[2][75] = {//0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74
//                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17},
//                               { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10}
//                              };

							
#define GLOBAL_X            0
#define LOCAL_X             1
#define GLOBAL_V_OFFSET     8

 #if DSF_RD_HPVREF_TRAINING_LOG_IN                
   uint32        pr_max_eyew[3][NUM_CA_PCH] = {{0},{0},{0}};
   uint32        pr_min_strt[3][NUM_CA_PCH] = {{199},{199},{199}};
   uint32        pr_min_stop[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
   uint32        pr_min_eyew[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
   uint32        pr_max_strt[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
   uint32        pr_max_stop[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
   uint32           pr_bus_eyew[NUM_CA_PCH] = {0};
   uint32           pr_bus_skwr[NUM_CA_PCH] = {0};
   uint32                             pr_x  =  0   ;
   char     pr_string[4][25]  ={
                                   " passing region start",
                                   " passing region stop",
                                   " passing region midp",
                                   " passing region eyew"
                                    };
#endif 
    //uint32    read_back_pattern[DQ_TRAINING_PATTERN_SIZE] = {0};
    uint32  allbit         = ALL_DQ_PASS_PAT ;
    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_ca[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;
    // =========================================HP Vref training init==============================================================
    vref_start_value = 0;
    vref_max_value   = 61;

	
    csr_temp[0]= HWIO_INX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_4_PWRS_0_CFG);
    csr_temp[1]= HWIO_INX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_5_PWRS_0_CFG);
    csr_temp[2]= HWIO_INX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_6_PWRS_0_CFG);
    csr_temp[3]= HWIO_INX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_7_PWRS_0_CFG);
	 
	 
	 // Enable MP Vref for IO qualifier
     HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_4_PWRS_0_CFG, 0x7D8003);
     HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_5_PWRS_0_CFG, 0x7D8003);
     HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_6_PWRS_0_CFG, 0x7D8003);
     HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_7_PWRS_0_CFG, 0x7D8003);
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        // Set decouple bit (RMW).This sets the IOqualifier to MPVREF. This is required durining training. bit[0] = 0 => MPRX
        decouple_bit[phy_x] = HWIO_INX (reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG);
        decouple_bit[phy_x] = (decouple_bit[phy_x] | (1 << 1));
        HWIO_OUTX (reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG, decouple_bit[phy_x]);
    }
    // Set MPVREF value to IOqualifier to mprx vref reg.  This value is to be set in CA PHY
    for (ca_x = 0; ca_x < ca; ca_x++)
    {
        DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],ioqual_vref);
    } 
    // ============================================================================================================================
    for(cs=0;cs<NUM_CS;cs++)
    {
        if ((chip_select >> cs) & 0x1)
        {
#if DSF_PXI_TRAINING_EN 
            Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
            memset(local_vars->failcount_histogram, 0,3 * NUM_DQ_PCH * PINS_PER_PHY * HISTOGRAM_SIZE);//clearing the histoghram initializing to Zero
            // ====================================================================================================================
            // ======================================     DQ RD    VREF             S W E E P I N G       =========================
            // ======================================     TO   C A P T U R E      H I S T O G R A M       =========================
            // ====================================================================================================================
            for (vref  = vref_start_value;vref <  vref_max_value ; vref++)  
            {  
                //  =================================Set Vref start
                for (phy_x = 0; phy_x < dq; phy_x++)
                {
                    for (bit = 0; bit < BYTE_SIZE; bit++)
                    {
                        remapped_bit  = bit_remapping_bimc2phy_DQ[ch][phy_x][bit];
                        DDR_PHY_hal_cfg_local_vref  ( reg_offset_ddr_dq[phy_x],
                                                      cs,
                                                      remapped_bit,
                                                      vref_sweep_lut[LOCAL_X][vref]
                                                    );
                    }//bit
                }//phy_x
                for (ca_x = 0; ca_x < ca; ca_x++)
                {
                    // Sweep Coarse VREF for CA1
                    DDR_PHY_hal_cfg_global_vref_hprx(reg_offset_ddr_ca[ca_x],vref_sweep_lut[GLOBAL_X][vref]);
                }                                        
                //  =================================Set Vref end
                for (loop_cnt = training_params_ptr->rd_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
                {
                    fail_pat=DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
                    allbit= DDRSS_fail2hist( fail_pat , local_vars->failcount_histogram, vref);
                    if(allbit == ALL_DQ_FAIL_PAT) break;
                }//loop count
            }  //End of Vref loop
            // ====================================================================================================================
            // ======================================      calculation                   =================================
            // ====================================================================================================================
            for (phy_x = 0; phy_x < dq; phy_x++)
            {
                ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    for (rf_x = 0; rf_x < 3; rf_x++)
                    {
                        DDRSS_passing_region_scan(&(local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT]),
                                                  &(local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP]),
                                                    local_vars->failcount_histogram [rf_x][phy_x][bit],
                                                vref_start_value,  vref_max_value );
                        local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP]  = ((local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] + local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ) >>1 ); // midpoint equal to average
                        local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW]  = ( local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] - local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] )      ; // eywidth  equal to stop - start
                        if ( pr_max_midp[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ) { pr_max_midp[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ; }
                        if ( pr_min_midp[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ) { pr_min_midp[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ; }
                        if ( local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] < min_eye_height ) { min_eye_height = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ; }
#if DSF_RD_HPVREF_TRAINING_LOG_IN                                    
                        if ( pr_min_strt[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ) { pr_min_strt[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ; }
                        if ( pr_min_stop[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ) { pr_min_stop[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ; }
                        if ( pr_min_eyew[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ) { pr_min_eyew[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ; }
                        if ( pr_max_strt[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ) { pr_max_strt[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ; }
                        if ( pr_max_stop[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ) { pr_max_stop[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ; }
                        if ( pr_max_eyew[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ) { pr_max_eyew[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ; }
#endif  // DSF_RD_HPVREF_TRAINING_LOG_IN
                        if( local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] == 0 )
                        {
                             ddr_printf(DDR_ERROR, "\nDDR ABORT: RD 1D HPRX ZERO EYE HEIGHT, Frequency = %d, Channel = %d, Rank = %d, Byte = %d, Bit = %d\n", ddr->shrm_ddr_ptr->misc.current_clk_in_kHz, ch, cs, phy_x, bit );
                             ddr_abort();
                        }
                    }//rf_x
                }//bit
            }//phy_x
            
        #if DSF_SMALL_EYE_DETECTION_LOG
            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 2: RD MINIMUM EYE HEIGHT = %d , Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", min_eye_height, ddr->shrm_ddr_ptr->misc.current_clk_in_kHz, ch, cs);
        #endif
        
        if((min_eye_height < training_params_ptr->rd_dqdqs.min_eye_height) && (training_params_ptr->rd_dqdqs.min_eye_height_enable==1))
          {
            ddr_printf(DDR_ERROR, "\n SCREEN 2: RD MINIMUM EYE HEIGHT Abort  (eye_height = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", min_eye_height, ddr->shrm_ddr_ptr->misc.current_clk_in_kHz, ch, cs);
            training_params_ptr->small_eye_abort = 1;
          }
            
 #if DSF_RD_HPVREF_TRAINING_LOG_HIST_IN
            // ====================================================================================================================
            // =============================== H I S T O G R A M   L O G     S T A R T  ===========================================
            // ====================================================================================================================
            for (vref  = vref_start_value;vref < ( vref_max_value ); vref++)  
            {
                for (phy_x = 0; phy_x < dq; phy_x++)
                {
                    ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                    ddr_printf (DDR_NORMAL, "DQ%d,",phy_x);
                    for (rf_x = 2; rf_x >= 2; rf_x--)
                    {
                        for (bit = 0; bit < BYTE_SIZE; bit++)
                        {
                            if  (vref==local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ){ddr_printf (DDR_NORMAL, " X,");}
                            else{ddr_printf (DDR_NORMAL, "%2d,",local_vars->failcount_histogram[rf_x][phy_x][bit][vref]);}
                        }
                             if ( vref == pr_min_strt[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<min_S,");}
                        else if ( vref == pr_max_strt[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<max_S,");}
                        else if ( vref == pr_min_midp[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<min_M,");}
                        else if ( vref == pr_max_midp[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<max_M,");}
                        else if ( vref == pr_min_stop[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<min_E,");}
                        else if ( vref == pr_max_stop[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<max_E,");}
                        else                                        {ddr_printf ( DDR_NORMAL, "       ,");}
                    }
                    ddr_printf(DDR_NORMAL,"%6d,%6d,%6d,",vref,vref_sweep_lut[GLOBAL_X][vref],vref_sweep_lut[LOCAL_X][vref]);
                }//phy_x
                ddr_printf(DDR_NORMAL, "\n");
            }//vref
            ddr_printf(DDR_NORMAL, "\n");
            // ====================================================================================================================
            // =============================== H I S T O G R A M   L O G     E N D      ===========================================
            // ====================================================================================================================
#endif //DSF_RD_HPVREF_TRAINING_LOG_HIST_IN
        }//if 
    }//cs
    // ============================================================================================================================
    //Set  Coarse (global)VREF  
    for (ca_x = 0; ca_x < ca; ca_x++)
    {     
          midv[ca_x]    = (pr_min_midp[2][ca_x] +pr_max_midp[2][ca_x])>>1;
        g_vref[ca_x]    =   vref_sweep_lut[GLOBAL_X][ midv[ca_x]];
        DDR_PHY_hal_cfg_global_vref_hprx(reg_offset_ddr_ca[ca_x],g_vref[ca_x] ); 
    }//ca_x
    // ============================================================================================================================
    for(cs=0;cs<NUM_CS;cs++)
    {  
        if ((chip_select >> cs) & 0x1)
        {
            for (phy_x = 0; phy_x < dq; phy_x++)
            {       
                ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                // ================================================================================================================
                //Set  perbit(local) VREF  
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    lvref[cs][phy_x][bit]= local_vars->vref_pass_reg[2][phy_x][cs][bit][MIDP]
                                    -  midv[ca_x]
                                    + vref_sweep_lut[LOCAL_X][ midv[ca_x]];
                    if(lvref[cs][phy_x][bit]>15)
                    {
                        lvref[cs][phy_x][bit]=15;
                    }                                    
                    if(lvref[cs][phy_x][bit]<0)
                    {
                        lvref[cs][phy_x][bit]=0;
                    }                                    
                    remapped_bit  =        bit_remapping_bimc2phy_DQ[ch][phy_x][bit];
                    DDR_PHY_hal_cfg_local_vref  (reg_offset_ddr_dq[phy_x],
                                                 cs,
                                                 remapped_bit,
                                                (uint32)lvref[cs][phy_x][bit]
                                                );
                }
                // ================================================================================================================
                //exit vref training trainig mode
                // Restore decouple bit.
                decouple_bit[phy_x] = (decouple_bit[phy_x] & 0xFFFFFFFD);
                HWIO_OUTX(reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG, decouple_bit[phy_x]);
                // restore mprx vref to mprx vref reg.             
                DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],
                                            0x1F);
                // Restore the IO Qualifier local VREF
                DDR_PHY_hal_cfg_local_vref(reg_offset_ddr_dq[phy_x],
                                            cs,
                                            10,                                            
                                            DQS_VREF
                                            );
            // ====================================================================================================================
            }//phy_x
            //  Restore  MP Vref to one time setting
            HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_4_PWRS_0_CFG, csr_temp[0]);
            HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_5_PWRS_0_CFG, csr_temp[1]);
            HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_6_PWRS_0_CFG, csr_temp[2]);
            HWIO_OUTX (reg_offset_ddr_ca[0], DDR_PHY_DDRPHY_FPM_PRFS_7_PWRS_0_CFG, csr_temp[3]);
 

 #if DSF_RD_HPVREF_TRAINING_LOG_IN
            // ====================================================================================================================
            // ======================================              L O G  P R I N T                       =========================
            // ====================================================================================================================
            ddr_printf (DDR_NORMAL, "Rank, %d,\n",cs);
            for (phy_x = 0; phy_x < dq; phy_x++)
            {
                ca_x=phy_x>>1;
                ddr_printf (DDR_NORMAL, "\nDQ_%d,GlobalVref_in_CA[%d], %d, \n",phy_x, ca_x, g_vref[ca_x]);
                // ================================================================================================================
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    ddr_printf (DDR_NORMAL, "%4d,",bit);
                }
                ddr_printf (DDR_NORMAL, " bit\n");
                // ================================================================================================================
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
                    ddr_printf (DDR_NORMAL, "%4d,",remapped_bit);
                }
                ddr_printf (DDR_NORMAL, " PHY_bit\n");
                // ================================================================================================================
                for (pr_x = 0; pr_x < 4; pr_x++)
                {
                    for (bit = 0; bit < BYTE_SIZE; bit++)
                    {
                        ddr_printf (DDR_NORMAL, "%4d,",local_vars->vref_pass_reg[2][phy_x][cs][bit][pr_x]);
                    }
                    ddr_printf (DDR_NORMAL, "%s\n",pr_string[pr_x]);
                }
                // ================================================================================================================
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    ddr_printf (DDR_NORMAL, "%4d,",lvref[cs][phy_x][bit]);
                }
                // ================================================================================================================
                ddr_printf (DDR_NORMAL, "%s,\n"," local vref");

                ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                pr_bus_eyew[ca_x]  =pr_min_stop[2][ca_x]  - pr_max_strt[2][ca_x];
                pr_bus_skwr[ca_x]  =pr_max_midp[2][ca_x]  - pr_min_midp[2][ca_x];
                ddr_printf (DDR_NORMAL, "pr_bus_eyew[ca%d]=, %d ,\n",ca_x, pr_bus_eyew[ca_x]);
                ddr_printf (DDR_NORMAL, "pr_bus_skwr[ca%d]=, %d ,\n",ca_x, pr_bus_skwr[ca_x]);
            }//phy_x
#endif //DSF_RD_HPVREF_TRAINING_LOG_IN
        }//if cs
    }//cs
 return(1);
}// END OF FUNCTION
// ===============================================================================================================================
// ======================================      V R E V   H P R X    T R A I N I N G           ====================================
// ===============================================================================================================================
#endif

#if DSF_RD_MPVREF_TRAINING_EN
// ===============================================================================================================================
// ======================================      V R E V   M P R X    T R A I N I N G           ====================================
// ===============================================================================================================================
boolean DDRSS_rd_Vref_1D_mprx(BOOT_DDR_STRUCT              *ddr,
                            uint8                        ch,
                            DDR_CHIPSELECT               chip_select,
                            PHY_BUS                      ca,
                            PHY_BUS                      dq,
                            training_params_t            *training_params_ptr,
                            ddrss_rdwr_dqdqs_local_vars  *local_vars
                           )
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================
  extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
  extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    uint8                                 cs = 0 ;
    uint8                           loop_cnt = 0 ;
    uint16                              vref = 0 ;
    uint8                     min_eye_height = 0xff;
    uint32     reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint8                               ca_x = 0 ;
    uint8                     vref_max_value = 0;
    uint8                   vref_start_value = 0;
  FAILCELL                          fail_cell;
    uint8     pr[NUM_CA_PCH][NUM_PR_PARAMS];
    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_ca[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
    //MP Vref training init
    vref_start_value = training_params_ptr->rd_dqdqs.coarse_vref_min_mprx;    //0;
    vref_max_value   = training_params_ptr->rd_dqdqs.coarse_vref_max_mprx;      //48;
        
    // ============================================================================================================================

    memset(local_vars->failcount_histogram, 0,3 * NUM_DQ_PCH * PINS_PER_PHY * HISTOGRAM_SIZE);//clearing the histoghram initializing to Zero
    // ====================================================================================================================
    // ======================================     DQ RD    VREF             S W E E P I N G       =========================
    // ======================================     TO   C A P T U R E      H I S T O G R A M       =========================
    // ====================================================================================================================
    for (vref  = vref_start_value;vref <  vref_max_value ; vref++)
    {
        for (ca_x = 0; ca_x < ca; ca_x++)
        {
            DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],vref);
        }
        fail_cell.w=0;
        for(cs=0;cs<NUM_CS;cs++)
        {
            if ((chip_select >> cs) & 0x1)
            {
#if DSF_PXI_TRAINING_EN
                Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
                for (loop_cnt = training_params_ptr->rd_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
                {
                    fail_cell.w |=  DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
                }//loop count
                    for (ca_x = 0; ca_x < ca; ca_x++)
                {
                    local_vars-> failcount_histogram [0][ca_x][cs][vref]=fail_cell.b[0][ca_x] ;
                    local_vars-> failcount_histogram [1][ca_x][cs][vref]=fail_cell.b[1][ca_x] ;
                    local_vars-> failcount_histogram [2][ca_x][cs][vref]=fail_cell.b[0][ca_x] | fail_cell.b[1][ca_x];
                }//ca_x
                //ddr_printf (DDR_NORMAL,"%d ,",local_vars-> failcount_histogram [2][0][0][knob]);
                //ddr_printf (DDR_NORMAL,"%d ,",local_vars-> failcount_histogram [2][ca_x][0][knob]);
            }//if
        }//cs
    }  //End of Vref loop
    //ddr_printf (DDR_NORMAL,"\n");
    // ============================================================================================================================
    for (ca_x = 0; ca_x < ca; ca_x++)
    {
        DDRSS_passing_region_scan(  &pr[ca_x][STRT],
                                    &pr[ca_x][STOP],
                                    local_vars-> failcount_histogram[2][ca_x][0],
                                    vref_start_value, vref_max_value);
        pr[ca_x][MIDP]   =     (pr[ca_x][STOP]  + pr[ca_x][STRT])>>1 ;
        pr[ca_x][EYEW]   =     (pr[ca_x][STOP]  - pr[ca_x][STRT]) ;
        if( pr[ca_x][EYEW] == 0 )
        {
             ddr_printf(DDR_ERROR, "\nDDR ABORT: RD 1D MPRX ZERO EYE HEIGHT, Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", ddr->shrm_ddr_ptr->misc.current_clk_in_kHz, ch, cs );
             ddr_abort();
        }
        if ( pr[ca_x][EYEW] < min_eye_height ) { min_eye_height = pr[ca_x][EYEW] ; }
        DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],pr[ca_x][MIDP]);
        #if DSF_RD_MPVREF_TRAINING_LOG_IN
        // ====================================================================================================================
        // ======================================              L O G  P R I N T                       =========================
        // ====================================================================================================================
        ddr_printf (DDR_NORMAL, "DQ_%d, Start, %d, Stop, %d, MPVref, %d, EyeW, %d, \n"
                    ,ca_x
                    ,pr[ca_x][STRT]
                    ,pr[ca_x][STOP]
                    ,pr[ca_x][MIDP]
                    ,pr[ca_x][EYEW]
                    );
        // ================================================================================================================
        #endif
    }//ca_x
    
    #if DSF_SMALL_EYE_DETECTION_LOG
            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 2: RD MINIMUM EYE HEIGHT = %d , Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", min_eye_height, ddr->shrm_ddr_ptr->misc.current_clk_in_kHz, ch, cs);
        #endif
        
        if((min_eye_height < training_params_ptr->rd_dqdqs.min_eye_height) && (training_params_ptr->rd_dqdqs.min_eye_height_enable==1))
          {
            ddr_printf(DDR_ERROR, "\n SCREEN 2: RD MINIMUM EYE HEIGHT Abort  (eye_height = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", min_eye_height, ddr->shrm_ddr_ptr->misc.current_clk_in_kHz, ch, cs);
            training_params_ptr->small_eye_abort = 1;
          }
 return(1);
}// END OF FUNCTION


// ===============================================================================================================================
// ======================================      V R E V   M P R X    T R A I N I N G           ====================================
// ===============================================================================================================================
#endif

#if DSF_RD_DQDQS_TRAINING_LOG_BEFORE | DSF_RD_DQDQS_TRAINING_LOG_AFTER
// ===============================================================================================================================
// ======================================      V R E V   M P R X    T R A I N I N G    LOG    ====================================
// ===============================================================================================================================

boolean DDRSS_rd_dqdqs_log (BOOT_DDR_STRUCT             *ddr, 
                            uint8                        ch, 
                            uint8                        cs, 
                            PHY_BUS                      ca,
                            PHY_BUS                      dq,
                            training_params_t            *training_params_ptr,
                            ddrss_rdwr_dqdqs_local_vars  *local_vars,
                            uint32                       clk_freq_khz,
                            uint8                        prfs_index)
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================
    int               dq_dcc_adj[NUM_DQ_PCH] = {0,0};
    int               dqsdcc_adj[NUM_DQ_PCH] = {0,0};
  extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
  extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    int                                 rf_x = 0 ;
    uint8                           loop_cnt = 0 ;
    uint8                                bit = 0 ;
    uint16                              knob = 0 ;
    uint16                        sweep_size = 0 ;
    uint8                              phy_x = 0 ;
    uint8                       remapped_bit = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint32        pr_min_midp[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_max_midp[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        pr_max_eyew[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32     safe_cdc_value[NUM_DQ_PCH] = {0,0} ;
    uint32        safe_coarse[NUM_DQ_PCH] = {0,0} ;
    uint32          safe_fine[NUM_DQ_PCH] = {0,0} ;
    uint32        pr_min_stop[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_strt[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_eyew[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_max_strt[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        pr_max_stop[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
   uint32   safe_delay [NUM_DQ_PCH]  = {0,0};
   uint32   pr_bus_eyew[3][NUM_DQ_PCH]  = {{0,0},{0,0},{0,0}};
   uint32   pr_bus_skwr[3][NUM_DQ_PCH]  = {{0,0},{0,0},{0,0}};
   uint32   pr_x                     =  0   ;
   uint32 fail_pat=0;
   char     pr_string[4][25]  ={
                                   " passing region start",
                                   " passing region stop",
                                   " passing region midp",
                                   " passing region eyew"
                                    };
                                     


  uint32  allbit         = ALL_DQ_PASS_PAT ;
//  uint32  cdc_ref[HISTOGRAM_SIZE][4]={0};
// ============================================================================================================================
// ======================================      I N I T I A L I Z A T I O N                    =================================
// ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_dq[0]       =  REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]       =  REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;



  for (phy_x = 0; phy_x < dq; phy_x++)
  {
     // Get the safe CDC value from the Shadow (ext) register
      safe_cdc_value[phy_x] = HWIO_INX ( reg_offset_ddr_dq[phy_x] , DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG ) ;
      if(cs==1)
      {
          safe_cdc_value[phy_x]= safe_cdc_value[phy_x]>>10;
      }
        // Parse the DQ CDC for the safe mode
      safe_coarse [phy_x]=  safe_cdc_value[phy_x]     & 0x1F;
      safe_fine   [phy_x]= (safe_cdc_value[phy_x]>>5) & 0x1F;
      safe_coarse [phy_x]= safe_coarse[phy_x];
      safe_fine   [phy_x]= safe_fine  [phy_x];
  }
//clearing the histoghram initializing to Zero
#if DSF_PXI_TRAINING_EN 
    Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
  memset(local_vars->failcount_histogram, 0,3 * NUM_DQ_PCH * PINS_PER_PHY * HISTOGRAM_SIZE);
// ======================================      cdc ref                   =================================
    DDRSS_cdcref_init(local_vars->cdc_ref);
// ============================================================================================================================
// ============================================================================================================================
// ======================================     C A       C D C           S W E E P I N G       =================================
// ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
// ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size_rd(clk_freq_khz);
      

   for (knob = 0;  knob < sweep_size;  knob++)
    {
        //  =================================Set knob start
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            //HWIO_OUTX ( reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG ,local_vars->cdc_ref[knob][CDC_L]);
            //ddr_printf (DDR_NORMAL, "%6X,%6X,\n",reg_offset_ddr_dq[phy_x],local_vars->cdc_ref[knob][CDC_L]);
            DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x], 
                                         local_vars->cdc_ref[knob][COARSE_L], 
                                         0x1, // 0 for fine_delay 1 for coarse 
                                         HP_MODE, 
                                         cs);   
        
            DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x],
                                         local_vars->cdc_ref[knob][FINE_L], 
                                         0x0, // 0 for fine_delay 1 for coarse 
                                         HP_MODE, 
                                         cs);   
        }//phy_x
        //  =================================Set knob end
        for (loop_cnt = training_params_ptr->rd_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
        {
            fail_pat=DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
            allbit= DDRSS_fail2hist( fail_pat , local_vars->failcount_histogram, knob);
            if(allbit == ALL_DQ_FAIL_PAT) break;
        }//loop count
    }  //End of knob loop
// ============================================================================================================================
// ======================================      calculation                   =================================
// ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            for (rf_x = 0; rf_x < 3; rf_x++)
            {
                //remapped_bit  =  bit_remapping_bimc2phy_DQ  [ch][phy_x][bit];
                DDRSS_passing_region_scan( &local_vars->pass_reg[rf_x][phy_x][bit][STRT],
                                           &local_vars->pass_reg[rf_x][phy_x][bit][STOP],
                                           local_vars->failcount_histogram [rf_x][phy_x][bit],
                                                        0,sweep_size);
                local_vars->pass_reg[rf_x][phy_x][bit][MIDP]  =(local_vars->pass_reg[rf_x][phy_x][bit][STOP]   +
                                                                local_vars->pass_reg[rf_x][phy_x][bit][STRT]) >>1 ; // midpoin equal to average
                local_vars->pass_reg[rf_x][phy_x][bit][EYEW]  =(local_vars->pass_reg[rf_x][phy_x][bit][STOP]   -
                                                                local_vars->pass_reg[rf_x][phy_x][bit][STRT] )  ; // eywidth  equal to stop - start
                if ( pr_max_midp[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_max_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_max_eyew[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_max_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_min_strt[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_min_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
                if ( pr_min_stop[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_min_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
                if ( pr_min_midp[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_min_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_min_eyew[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_min_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_max_strt[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_max_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
                if ( pr_max_stop[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_max_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
            }
        }
        // ============================================================================================================================
        // ========================================== D U T Y   C Y C L E   D I S T O R T I O N  ======================================
        // ============================================================================================================================
        dq_dcc_adj[phy_x]=  (pr_max_eyew[0/*even*/][phy_x]-pr_max_eyew[1/*ODD*/][phy_x]);
        dqsdcc_adj[phy_x]=   pr_max_midp[0/*even*/][phy_x]-pr_max_midp[1/*ODD*/][phy_x];
    }
         
// ============================================================================================================================
// ==========================================Setting back coarse and fine values =====================================================
// ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        //DDR_PHY_hal_cfg_cdc_slave_rd_cdc( reg_offset_ddr_dq[phy_x],safe_cdc_value[phy_x]  );
        DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x], 
                                      safe_coarse[phy_x], 
                                      0x1, // 0 for fine_delay 1 for coarse 
                                      HP_MODE, 
                                      cs);   
    
        DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x],
                                       safe_fine[phy_x] , 
                                      0x0, // 0 for fine_delay 1 for coarse 
                                      HP_MODE, 
                                      cs);   // 0 for fine_delay_mode.          
        safe_delay [phy_x]  = safe_coarse[phy_x]* COARSE_STEP_IN_PS + safe_fine[phy_x] * FINE_STEP_IN_PS;
    }
// ============================================================================================================================
// ======================================              L O G  P R I N T                       =================================
// ============================================================================================================================
    for (knob=0; knob < sweep_size-1; knob++)
    {
        for (phy_x = 0; phy_x < dq; phy_x++)
        {
            ddr_printf (DDR_NORMAL, "DQ%d,",phy_x);
            for (rf_x = 2; rf_x >= 0; rf_x--)
            {
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                if(knob==local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ){ddr_printf (DDR_NORMAL, " X,");}
                else{ddr_printf (DDR_NORMAL, "%2d,",local_vars->failcount_histogram[rf_x][phy_x][bit][knob]);}
                }
                if ((safe_delay[phy_x] >= local_vars->cdc_ref[knob  ][DELAY_L ]) &&
                    (safe_delay[phy_x] <  local_vars->cdc_ref[knob+1][DELAY_L])) {ddr_printf ( DDR_NORMAL, "<<<<<<<,");}
                else if             (         knob == pr_min_strt[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_S,");}
                else if             (         knob == pr_max_strt[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_S,");}
                else if             (         knob == pr_min_midp[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_M,");}
                else if             (         knob == pr_max_midp[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_M,");}
                else if             (         knob == pr_min_stop[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_E,");}
                else if             (         knob == pr_max_stop[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_E,");}
                else if             (local_vars->cdc_ref[knob  ][FINE_L] == 0 )  {ddr_printf ( DDR_NORMAL, "      0,");}
                else                                                             {ddr_printf ( DDR_NORMAL, "       ,");}
            }
            ddr_printf(DDR_NORMAL,"%3d,%2d,%2d,%4d,%3X,",knob,
                                     local_vars->cdc_ref[knob][COARSE_L],
                                     local_vars->cdc_ref[knob][FINE_L  ],
                                     local_vars->cdc_ref[knob][DELAY_L ],
                                     local_vars->cdc_ref[knob][CDC_L   ]);
        }//phy_x
        ddr_printf(DDR_NORMAL, "\n");
    }//knob
    

    ddr_printf(DDR_NORMAL, "\n");
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        pr_bus_eyew[0][phy_x]  =local_vars->cdc_ref[pr_min_stop[0][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[0][phy_x]][DELAY_L ];
        pr_bus_skwr[0][phy_x]  =local_vars->cdc_ref[pr_max_midp[0][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[0][phy_x]][DELAY_L ];
        pr_bus_eyew[1][phy_x]  =local_vars->cdc_ref[pr_min_stop[1][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[1][phy_x]][DELAY_L ];
        pr_bus_skwr[1][phy_x]  =local_vars->cdc_ref[pr_max_midp[1][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[1][phy_x]][DELAY_L ];
        pr_bus_eyew[2][phy_x]  =local_vars->cdc_ref[pr_min_stop[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[2][phy_x]][DELAY_L ];
        pr_bus_skwr[2][phy_x]  =local_vars->cdc_ref[pr_max_midp[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[2][phy_x]][DELAY_L ];

        ddr_printf (DDR_NORMAL, "DQ_%d,\n",phy_x);
        // ============================================================================================================================
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
            ddr_printf (DDR_NORMAL, "%4d,",bit);
        }
        ddr_printf (DDR_NORMAL, " bit,\n");
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
            ddr_printf (DDR_NORMAL, "%4d,",remapped_bit);
        }
        ddr_printf (DDR_NORMAL, " PHY_bit,\n");
        // ============================================================================================================================





        for (pr_x = 0; pr_x < 4; pr_x++)
        {
            for (bit = 0; bit < BYTE_SIZE; bit++)
            {
                ddr_printf (DDR_NORMAL, "%4d,",local_vars->pass_reg[2][phy_x][bit][pr_x]);
            }
                ddr_printf (DDR_NORMAL, "%s\n",pr_string[pr_x]);
        }


        ddr_printf (DDR_NORMAL, "   rd_dq_dcc[%d][%d]=, %d ,\n"  ,cs ,phy_x,     dq_dcc_adj[phy_x]);
        ddr_printf (DDR_NORMAL, "   rd_dqsdcc[%d][%d]=, %d ,\n"  ,cs ,phy_x,     dqsdcc_adj[phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_eyew[phase0][%d](ps)=, %d ,\n",phy_x, pr_bus_eyew[0][phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_skwr[phase0][%d](ps)=, %d ,\n",phy_x, pr_bus_skwr[0][phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_eyew[phase1][%d](ps)=, %d ,\n",phy_x, pr_bus_eyew[1][phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_skwr[phase1][%d](ps)=, %d ,\n",phy_x, pr_bus_skwr[1][phy_x]);
        ddr_printf (DDR_NORMAL, "pr_bus_eyew[bothphase][%d](ps)=, %d ,\n",phy_x, pr_bus_eyew[2][phy_x]);
        ddr_printf (DDR_NORMAL, "pr_bus_skwr[bothphase][%d](ps)=, %d ,\n",phy_x, pr_bus_skwr[2][phy_x]);
    }//phy_x
    return(1);
}// END OF FUNCTION


#endif
#if DSF_RD_HPVREF_TRAINING_LOG_BEFORE | DSF_RD_HPVREF_TRAINING_LOG_AFTER
// ===============================================================================================================================
// ======================================      V R E F   H P R X    T R A I N I N G    LOG    ====================================
// ===============================================================================================================================

boolean DDRSS_rd_Vref_hprx_log(BOOT_DDR_STRUCT                   *ddr, 
                                   uint8                        ch, 
                                   DDR_CHIPSELECT               chip_select,
                                   PHY_BUS                      ca,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars
                                    )
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================
  extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
  extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    uint8                                 cs = 0 ; 

    int                                 rf_x = 0 ;
    uint8                           loop_cnt = 0 ;
    uint8                                bit = 0 ;
    uint16                              vref = 0 ;
    uint8                              phy_x = 0 ;
    uint8                       remapped_bit = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint32     reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint32        pr_max_midp[3][NUM_CA_PCH] = {{0},{0},{0}};
    uint32        pr_max_eyew[3][NUM_CA_PCH] = {{0},{0},{0}};
    uint32        pr_min_midp[3][NUM_CA_PCH] = {{199},{199},{199}};
    uint32        pr_min_strt[3][NUM_CA_PCH] = {{199},{199},{199}};
    uint8                               ca_x = 0 ;
    uint8                     vref_max_value = 0;
    uint8                   vref_start_value = 0;
    uint8           decouple_bit[NUM_DQ_PCH] = {0};    
    uint8                        ioqual_vref = 0x10;
//    uint8 vref_sweep_lut[2][44] = {   //0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43
//                                      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  10,  10,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17},
//                                      { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,  8,  9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9,   8,   9}
//                            } ;
uint8 vref_sweep_lut[2][61] = {//0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60
                               { 0,0,0,0,0,0,0,0,0,0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9,10,10,10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,16,16,16,17,17,17},
                               { 0,1,2,3,4,5,6,7,8,9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9, 7, 8, 9}
                              };
						
//uint8 vref_sweep_lut[2][75] = {//0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74
//                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17},
//                               { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10,  7,  8,  9, 10}
//                              };
#define GLOBAL_X            0
#define LOCAL_X             1
#define GLOBAL_V_OFFSET     8


    
   uint32        pr_min_stop[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
   uint32        pr_min_eyew[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
   uint32        pr_max_strt[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
   uint32        pr_max_stop[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
   uint32           pr_bus_eyew[NUM_CA_PCH] = {0};
   uint32           pr_bus_skwr[NUM_CA_PCH] = {0};
   uint8              hprx_temp[NUM_CA_PCH] = {0};
   uint32                             pr_x  =  0   ;
   uint32  fail_pat=0;
   char     pr_string[4][25]  ={
                                   " passing region start",
                                   " passing region stop",
                                   " passing region midp",
                                   " passing region eyew"
                                    };

    uint32  allbit         = ALL_DQ_PASS_PAT ;
    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_ca[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;

    // =========================================HP Vref training init==============================================================
    
    vref_start_value = 0;
    vref_max_value   = 61;
    
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        // Set decouple bit (RMW).This sets the IOqualifier to MPVREF. This is required durining training. bit[0] = 0 => MPRX
        decouple_bit[phy_x] = HWIO_INX (reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG);
        decouple_bit[phy_x] = (decouple_bit[phy_x] | (1 << 1));
        HWIO_OUTX (reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG, decouple_bit[phy_x]);
    }
    // Set MPVREF value to IOqualifier to mprx vref reg.  This value is to be set in CA PHY
    for (ca_x = 0; ca_x < ca; ca_x++)
    {
        DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],ioqual_vref);
    } 
	
    for (ca_x = 0; ca_x < ca; ca_x++)
    {
	  hprx_temp[ca_x]=  DDR_PHY_hal_get_global_vref_hprx(reg_offset_ddr_ca[ca_x]);
	}
    // ============================================================================================================================
    for(cs=0;cs<NUM_CS;cs++)
    {
        if ((chip_select >> cs) & 0x1)
        {
#if DSF_PXI_TRAINING_EN             
            Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif

            // ====================================================================================================================
            //clearing the histoghram initializing to Zero
            // ====================================================================================================================
             memset(local_vars->failcount_histogram, 0,3 * NUM_DQ_PCH * PINS_PER_PHY * HISTOGRAM_SIZE);
            // ====================================================================================================================
            // ======================================     DQ RD    VREF             S W E E P I N G       =========================
            // ======================================     TO   C A P T U R E      H I S T O G R A M       =========================
            // ====================================================================================================================
            for (vref  = vref_start_value;vref <  vref_max_value ; vref++)  
            {  
                //  =================================Set Vref start
                               for (ca_x = 0; ca_x < ca; ca_x++)
                {
                    // Sweep Coarse VREF for CA1
                    DDR_PHY_hal_cfg_global_vref_hprx(reg_offset_ddr_ca[ca_x],vref_sweep_lut[GLOBAL_X][vref]);
                    
                }                                        
                //  =================================Set Vref end
                for (loop_cnt = training_params_ptr->rd_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
                {
                    fail_pat=DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
                    allbit= DDRSS_fail2hist( fail_pat , local_vars->failcount_histogram, vref);
                    if(allbit == ALL_DQ_FAIL_PAT) break;
                }//loop count
            }  //End of Vref loop
            // ====================================================================================================================
            // ======================================      calculation                   =================================
            // ====================================================================================================================
                for (phy_x = 0; phy_x < dq; phy_x++)
                {
                    ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                    for (bit = 0; bit < BYTE_SIZE; bit++)
                    {
                        for (rf_x = 0; rf_x < 3; rf_x++)
                        {
                            DDRSS_passing_region_scan(&(local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT]),
                                                      &(local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP]),
                                                        local_vars->failcount_histogram [rf_x][phy_x][bit],
                                                    vref_start_value,  vref_max_value );
                            local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP]  = ((local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] + local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ) >>1 ); // midpoint equal to average
                            local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW]  = ( local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] - local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] )      ; // eywidth  equal to stop - start
                            if ( pr_max_midp[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ) { pr_max_midp[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ; }
                            if ( pr_max_eyew[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ) { pr_max_eyew[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ; }
                            if ( pr_min_strt[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ) { pr_min_strt[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ; }
                            if ( pr_min_stop[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ) { pr_min_stop[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ; }
                            if ( pr_min_midp[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ) { pr_min_midp[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ; }
                            if ( pr_min_eyew[rf_x][ca_x] > local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ) { pr_min_eyew[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][EYEW] ; }
                            if ( pr_max_strt[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ) { pr_max_strt[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STRT] ; }
                            if ( pr_max_stop[rf_x][ca_x] < local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ) { pr_max_stop[rf_x][ca_x] = local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][STOP] ; }
                        }
                    }
                }//phy_x
            // ====================================================================================================================
            // =============================== H I S T O G R A M   L O G     S T A R T  ===========================================
            // ====================================================================================================================
            for (vref  = 8;vref < ( vref_max_value ); vref++)  
            {
                for (phy_x = 0; phy_x < dq; phy_x++)
                {
                    ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                    ddr_printf (DDR_NORMAL, "DQ%d,",phy_x);
                    for (rf_x = 2; rf_x >= 2; rf_x--)
                    {
                        for (bit = 0; bit < BYTE_SIZE; bit++)
                        {
                            if  (vref==local_vars->vref_pass_reg[rf_x][phy_x][cs][bit][MIDP] ){ddr_printf (DDR_NORMAL, " X,");}
                            else{ddr_printf (DDR_NORMAL, "%2d,",local_vars->failcount_histogram[rf_x][phy_x][bit][vref]);}
                        }
                             if ( vref == pr_min_strt[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<min_S,");}
                        else if ( vref == pr_max_strt[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<max_S,");}
                        else if ( vref == pr_min_midp[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<min_M,");}
                        else if ( vref == pr_max_midp[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<max_M,");}
                        else if ( vref == pr_min_stop[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<min_E,");}
                        else if ( vref == pr_max_stop[rf_x][ca_x] ) {ddr_printf ( DDR_NORMAL, "<<max_E,");}
                        else                                        {ddr_printf ( DDR_NORMAL, "       ,");}
                    }
                    ddr_printf(DDR_NORMAL,"%6d,%6d,%6d,",vref,vref_sweep_lut[GLOBAL_X][vref],vref_sweep_lut[LOCAL_X][vref]);
                }//phy_x
                ddr_printf(DDR_NORMAL, "\n");
            }//vref
            ddr_printf(DDR_NORMAL, "\n");
            // ====================================================================================================================
            // =============================== H I S T O G R A M   L O G     E N D      ===========================================
            // ====================================================================================================================
        }//if 
    }//cs
    // ============================================================================================================================
    //Set  Coarse (global)VREF  
    for (ca_x = 0; ca_x < ca; ca_x++)
    {                
        DDR_PHY_hal_cfg_global_vref_hprx(reg_offset_ddr_ca[ca_x],hprx_temp[ca_x]); 
    }//ca_x
    // ============================================================================================================================
    for(cs=0;cs<NUM_CS;cs++)
    {  
        if ((chip_select >> cs) & 0x1)
        {
            for (phy_x = 0; phy_x < dq; phy_x++)
            {       
                ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                // ================================================================================================================
                //Set  perbi(local) VREF  

                // ================================================================================================================
                //exit vref training trainig mode
                // Restore decouple bit.
                decouple_bit[phy_x] = (decouple_bit[phy_x] & 0xFFFFFFFD);
                HWIO_OUTX(reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG, decouple_bit[phy_x]);
                // restore mprx vref to mprx vref reg.             
                DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],
                                            0x1F);
                // Restore the IO Qualifier local VREF
                DDR_PHY_hal_cfg_local_vref(reg_offset_ddr_dq[phy_x],
                                            cs,
                                            10,                                            
                                            DQS_VREF
                                            );
            // ====================================================================================================================
            }//phy_x

            // ====================================================================================================================
            // ======================================              L O G  P R I N T                       =========================
            // ====================================================================================================================
            ddr_printf (DDR_NORMAL, "RANK_%d\n",cs);
        
            for (phy_x = 0; phy_x < dq; phy_x++)
            {
                ddr_printf (DDR_NORMAL, "DQ_%d,\n",phy_x);
                // ================================================================================================================
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    ddr_printf (DDR_NORMAL, "%4d,",bit);
                }
                ddr_printf (DDR_NORMAL, "bit,\n");
                // ================================================================================================================
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                    remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
                    ddr_printf (DDR_NORMAL, "%4d,",remapped_bit);
                }
                ddr_printf (DDR_NORMAL, "PHY_bit,\n");
                // ================================================================================================================
                for (pr_x = 0; pr_x < 4; pr_x++)
                {
                    for (bit = 0; bit < BYTE_SIZE; bit++)
                    {
                        ddr_printf (DDR_NORMAL, "%4d,",local_vars->vref_pass_reg[2][phy_x][cs][bit][pr_x]);
                    }
                    ddr_printf (DDR_NORMAL, "%s,\n",pr_string[pr_x]);
                }
                ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                pr_bus_eyew[ca_x]  =pr_min_stop[2][ca_x]  - pr_max_strt[2][ca_x];
                pr_bus_skwr[ca_x]  =pr_max_midp[2][ca_x]  - pr_min_midp[2][ca_x];
                ddr_printf (DDR_NORMAL, "pr_bus_eyew[ca%d]=, %d ,\n",ca_x, pr_bus_eyew[ca_x]);
                ddr_printf (DDR_NORMAL, "pr_bus_skwr[ca%d]=, %d ,\n",ca_x, pr_bus_skwr[ca_x]);
            }//phy_x
        }//if cs
    }//cs

 return(1);
}// END OF FUNCTION
// ===============================================================================================================================
// ======================================      V R E F   H P R X    T R A I N I N G    LOG    ====================================
// ===============================================================================================================================
#endif


uint8 DDRSS_wich_phase (uint32 _inst_, uint8 rank, int dcc_adj)
{
    int cdc_edge = 0;
    //dcc_adj= even_anchor-odd_anchor;
    cdc_edge=1;// use the ODD histogram for perbit training
    if (dcc_adj< 0)
    {         
        dcc_adj= -dcc_adj;
        cdc_edge=0;// use the EVEN histogram for perbit training
    }
    if (9 < dcc_adj)
    {
        cdc_edge = 2;  // use the BOTH histogram for perbit training
    }
    return cdc_edge;
}      


uint16 DDRSS_rd_dcc2csr ( int dcc_adj)
{
    int dcc_edge = 0;
    //dcc_adj= even_anchor-odd_anchor;
    dcc_edge=0;
    if (dcc_adj< 0)
    {         
        dcc_adj= -dcc_adj;
        dcc_edge=1;
    }
    if (9 < dcc_adj)
    {
        dcc_adj  = 9;
    }
    return (dcc_adj<<(dcc_edge * 4));
}      


void DDRSS_propagate_global_vref_hprx_to_all_PRFS (uint8 ch)
{
    uint8   g_vref;
    uint8   ca_x;
    uint8   prfs_x;
    uint32  reg_offset_ddr_ca[NUM_CA_PCH];
    
    reg_offset_ddr_ca[0]=  REG_OFFSET_DDR_PHY_CH(ch)+ CA0_DDR_PHY_OFFSET;
    
    for (ca_x = 0; ca_x < NUM_CA_PCH; ca_x++)
    {   
        g_vref=DDR_PHY_hal_get_global_vref_hprx(reg_offset_ddr_ca[ca_x]);
        for (prfs_x = 0; prfs_x <NUM_PRFS_BANDS; prfs_x++)
        {   
             DDR_PHY_hal_cfg_global_vref_hprx_PRFS  (reg_offset_ddr_ca[ca_x],prfs_x ,g_vref);   
        }
    }
}

void DDRSS_propagate_global_vref_mprx_to_all_PRFS (uint8 ch)
{
    uint8   g_vref;
    uint8   ca_x;
    uint8   prfs_x;
    uint32  reg_offset_ddr_ca[NUM_CA_PCH];
    
    reg_offset_ddr_ca[0]=  REG_OFFSET_DDR_PHY_CH(ch)+ CA0_DDR_PHY_OFFSET;
    
    for (ca_x = 0; ca_x < NUM_CA_PCH; ca_x++)
    {   
        g_vref=DDR_PHY_hal_get_global_vref_mprx(reg_offset_ddr_ca[ca_x]);
        for (prfs_x = 0; prfs_x <NUM_PRFS_BANDS; prfs_x++)
        {   
             DDR_PHY_hal_cfg_global_vref_mprx_PRFS  (reg_offset_ddr_ca[ca_x],prfs_x ,g_vref);   
        }
    }
}
 void DDRSS_cpy_cdcext_slave_rd_r0tor1(uint8 prfs_x, uint8 ch  )
{
	uint8 coarse_delay;
	uint8 fine_delay;
    uint8 phy_x=0;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
	
    reg_offset_ddr_dq[0] = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1] = REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;
    for (phy_x = 0; phy_x < NUM_DQ_PCH; phy_x++)
    {
	  DDR_PHY_hal_get_cdcext_slave_rd(reg_offset_ddr_dq[phy_x],0, &coarse_delay,&fine_delay, prfs_x ) ;
	  DDR_PHY_hal_cfg_cdcext_slave_rd(reg_offset_ddr_dq[phy_x],1,  coarse_delay, fine_delay, prfs_x ) ;
	}
}

int DDRSS_get_rd_vref_phy_offset(uint8 ch, uint8 dq, uint32* const out_reg_offset)
{
	uint32 reg_offset_ddr_rd_vref[NUM_DQ_PCH];

	if ( (ch >= NUM_CH) || (dq >= NUM_DQ_PCH) )
	{
		return -1;
	}
	
	//same Vref for both DQs , used from CA PHY
	reg_offset_ddr_rd_vref[0] =  REG_OFFSET_DDR_PHY_CH(ch)+ CA0_DDR_PHY_OFFSET;
	reg_offset_ddr_rd_vref[1] =  REG_OFFSET_DDR_PHY_CH(ch)+ CA0_DDR_PHY_OFFSET;
        *out_reg_offset = reg_offset_ddr_rd_vref[dq];

	return 0;
}

int DDRSS_get_rd_cdc_phy_offset(uint8 ch,uint8 cs, uint8 dq, uint32* const out_reg_offset)
{
	uint32 reg_offset_ddr_rd_cdc[NUM_DQ_PCH] = {0};
	//NUM_CS is static, not all may be populated
	if ( (ch >= NUM_CH) || (cs >= NUM_CS) || (dq >= NUM_DQ_PCH) )
	{
		return -1;
	}

	reg_offset_ddr_rd_cdc[0] = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
	reg_offset_ddr_rd_cdc[1] = REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;
        *out_reg_offset = reg_offset_ddr_rd_cdc[dq];
	
	return 0;
}

uint8 DDRSS_get_active_rd_vref(uint8 ch, uint8 dq, uint32 freq_khz)
{
    uint8 vref;
	uint32 reg_offset_ddr_rd_vref;
	// bounds check inside DDRSS_get_rd_vref_phy_offset
	if (DDRSS_get_rd_vref_phy_offset(ch, dq, &reg_offset_ddr_rd_vref) < 0)
	{
		return -1;
	}
	
	if (freq_khz <= HIGHEST_MPVREF_PRFS_FREQ)
	{		
        	vref = DDR_PHY_hal_get_global_vref_mprx(reg_offset_ddr_rd_vref);
	}
	else
	{
        	vref = DDR_PHY_hal_get_global_vref_hprx(reg_offset_ddr_rd_vref);
	}

	return vref;
}

uint8 DDRSS_get_rd_vref(uint8 ch, uint8 dq, uint32 freq_khz)
{
	uint8 prfs_x = 0;
	uint8 mp_hp;
	uint8 vref;
	uint32 reg_offset_ddr_rd_vref;
	// bounds check inside DDRSS_get_rd_vref_phy_offset
	if (DDRSS_get_rd_vref_phy_offset(ch, dq, &reg_offset_ddr_rd_vref) < 0)
	{
		return -1;
	}

	prfs_x = DDRSS_Get_Freq_Index(NULL, freq_khz);	
	mp_hp = (freq_khz <= HIGHEST_MPVREF_PRFS_FREQ) ? MPVREF : HPVREF;
	vref = DDR_PHY_hal_get_rd_vref_PRFS(reg_offset_ddr_rd_vref, prfs_x, mp_hp);

	return vref;
}

int DDRSS_set_rd_vref(uint8 ch, uint8 dq, uint32 freq_khz, uint8 vref)
{
	uint8   prfs_x = 0;
	int ret = 0;
	uint32 reg_offset_ddr_rd_vref;

	// bounds check inside DDRSS_get_rd_vref_phy_offset
	if (DDRSS_get_rd_vref_phy_offset(ch, dq, &reg_offset_ddr_rd_vref) < 0)
	{
		return -1;
	}

	//prfs_x = DDRSS_Get_Freq_Index(NULL, freq_khz);
	for(prfs_x=0;prfs_x<8;prfs_x++) // due to HW limitation set the same RD Vref to all PRFS
	{
		if (freq_khz <= HIGHEST_MPVREF_PRFS_FREQ)
		{
			ret = DDR_PHY_hal_cfg_global_vref_mprx_PRFS(reg_offset_ddr_rd_vref,prfs_x, vref);
		}
		else 
		{
			ret = DDR_PHY_hal_cfg_global_vref_hprx_PRFS(reg_offset_ddr_rd_vref,prfs_x, vref);
		}
		if (ret < 0) 
		{
			ddr_printf(DDR_NORMAL, "failed to set Vref \r\n");
			return ret;
		}
	}

	return 0;
}

int DDRSS_get_cdcext_slave_rd(uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8* const coarse_delay, uint8* const fine_delay)
{
	uint8   prfs_x = 0;
	uint32 reg_offset_rd_cdc = 0;

	// bounds check inside DDRSS_get_rd_cdc_phy_offset
	if (DDRSS_get_rd_cdc_phy_offset(ch, cs, dq, &reg_offset_rd_cdc) < 0)
	{
		return -1;
	}

	prfs_x = DDRSS_Get_Freq_Index(NULL, freq_khz);
	DDR_PHY_hal_get_cdcext_slave_rd( reg_offset_rd_cdc, cs, coarse_delay, fine_delay, prfs_x );
	return 0;
}


int DDRSS_set_cdcext_slave_rd(uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8 coarse_delay, uint8 fine_delay)
{
	uint8   prfs_x = 0;
	uint32 reg_offset_rd_cdc = 0;
	// bounds check inside DDRSS_get_rd_cdc_phy_offset
	if (DDRSS_get_rd_cdc_phy_offset(ch, cs, dq, &reg_offset_rd_cdc) < 0)
	{
		return -1;
	}
	prfs_x = DDRSS_Get_Freq_Index(NULL, freq_khz);
	DDR_PHY_hal_cfg_cdcext_slave_rd( reg_offset_rd_cdc, cs, coarse_delay, fine_delay, prfs_x );

	return 0;
}

int DDRSS_get_active_cdc_slave_rd(uint8 ch, uint8 cs, uint8 dq, uint8* const coarse_delay, uint8* const fine_delay) 
{
	uint32 coarse,fine;
	uint32 reg_offset_rd_cdc = 0;

	// bounds check inside DDRSS_get_rd_cdc_phy_offset
	if (DDRSS_get_rd_cdc_phy_offset(ch, cs, dq, &reg_offset_rd_cdc) < 0)
	{
		return -1;
	}

	DDR_PHY_hal_get_cdc_slave_rd_b( reg_offset_rd_cdc, cs, &coarse, &fine);
	// DDR_PHY_hal_get_cdc_slave_rd_b defined corse and fine as uint32* (pointers)
	*coarse_delay = coarse;
	*fine_delay = fine;

	return 0;
}	  
		  
// ============================================================================================================================
// ======================================      R E A D  DQDQS  T R A I N I N G   SCRN             =================================
// ============================================================================================================================
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
                       )
{
    // ============================================================================================================================
    // ======================================      D E F I N I T I O N S                          =================================
    // ============================================================================================================================


    extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
    extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    uint8                           loop_cnt = 0 ;
    uint16                              knob = 0 ;
    uint16                        sweep_size = 0 ;
    uint8                              phy_x = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
     uint8     fail_histogram[3][NUM_DQ_PCH][HISTOGRAM_SIZE]={{{0}}};
  FAILCELL                          fail_cell;
    uint8     pr[NUM_DQ_PCH][NUM_PR_PARAMS];
	
    uint32     safe_cdc_value[NUM_DQ_PCH] = {0,0} ;
    uint32        safe_coarse[NUM_DQ_PCH] = {0,0} ;
    uint32          safe_fine[NUM_DQ_PCH] = {0,0} ;
    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================

    // Set DDR Base address registers
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ1_DDR_PHY_OFFSET;


    //clearing the histogram initializing to Zero
    memset(fail_histogram, 0,3 * NUM_DQ_PCH *HISTOGRAM_SIZE);
    DDRSS_cdcref_init(local_vars->cdc_ref);//cdc ruler init
  for (phy_x = 0; phy_x < dq; phy_x++)
  {
     // Get the safe CDC value from the Shadow (ext) register
      safe_cdc_value[phy_x] = HWIO_INX ( reg_offset_ddr_dq[phy_x] , DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG ) ;
      if(cs==1)
      {
          safe_cdc_value[phy_x]= safe_cdc_value[phy_x]>>10;
      }
        // Parse the DQ CDC for the safe mode
      safe_coarse [phy_x]=  safe_cdc_value[phy_x]     & 0x1F;
      safe_fine   [phy_x]= (safe_cdc_value[phy_x]>>5) & 0x1F;
      safe_coarse [phy_x]= safe_coarse[phy_x];
      safe_fine   [phy_x]= safe_fine  [phy_x];
  }
    // ============================================================================================================================
#if DSF_PXI_TRAINING_EN
    Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif

    // ============================================================================================================================
    // ======================================     DQ RD     C D C           S W E E P I N G       =================================
    // ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
    // ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size_rd(clk_freq_khz);
    for (knob = 0;  knob < sweep_size;  knob++)
    {
        //  =================================Set knob start
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            DDR_PHY_hal_cfg_cdc_slave_rd(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][COARSE_L],
                                            0x1, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            cs);

            DDR_PHY_hal_cfg_cdc_slave_rd(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][FINE_L],
                                            0x0, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            cs);
        }//phy_x
        fail_cell.w=0;
        for (loop_cnt = training_params_ptr->wr_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
        {
                   fail_cell.w |=  DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
        }//loop count
		for (phy_x = 0; phy_x < dq; phy_x++)
        {
            fail_histogram [0][phy_x][knob]=fail_cell.b[0][phy_x] ;
            fail_histogram [1][phy_x][knob]=fail_cell.b[1][phy_x] ;
            fail_histogram [2][phy_x][knob]=fail_cell.b[0][phy_x] | fail_cell.b[1][phy_x];
        }//phy_x
    }//knob
    // ============================================================================================================================
    // ======================================      calculation                   =================================
    // ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        DDRSS_passing_region_scan(  &pr[phy_x][STRT],
                                    &pr[phy_x][STOP],
                                    fail_histogram[2][phy_x],
                                    0,sweep_size);
        pr[phy_x][MIDP]   =     (pr[phy_x][STOP]  + pr[phy_x][STRT])>>1 ;
        pr[phy_x][EYEW]   =      pr[phy_x][STOP]  - pr[phy_x][STRT];

		
		#if DSF_SMALL_EYE_DETECTION_LOG
        if (prfs_index == 7)
        {
            ddr_printf (DDR_NORMAL, ", Byte = %d, EYE width = %d (ps) = %d (f steps)", phy_x, pr[phy_x][EYEW] * FINE_STEP_IN_PS, pr[phy_x][EYEW]);
		}
        #endif
		
        // ============================================================================================================================
        // ==========================================Setting back coarse and fine values =====================================================
        // ============================================================================================================================

        //DDR_PHY_hal_cfg_cdc_slave_rd_cdc( reg_offset_ddr_dq[phy_x],safe_cdc_value[phy_x]  );
        DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x], 
                                      safe_coarse[phy_x], 
                                      0x1, // 0 for fine_delay 1 for coarse 
                                      HP_MODE, 
                                      cs);   
    
        DDR_PHY_hal_cfg_cdc_slave_rd(reg_offset_ddr_dq[phy_x],
                                       safe_fine[phy_x] , 
                                      0x0, // 0 for fine_delay 1 for coarse 
                                      HP_MODE, 
                                      cs);   // 0 for fine_delay_mode.          
		
        // ============================================================================================================================

     }//phy_x
	 
 if(pr[0][EYEW] <pr[1][EYEW])\
 {
    return(pr[0][EYEW]);
 }
 else
 {
    return(pr[1][EYEW]);
 }
}// END OF FUNCTION



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
                       )
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================

    uint16                              vref = 0 ;
    uint8                              phy_x = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint32     reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint8                               ca_x = 0 ;
    uint8           decouple_bit[NUM_DQ_PCH] = {0};    
    uint8                        ioqual_vref = 0x10;

    int                   vref_chk_points[4] = { 2,-2, 3,-3 };
    uint8                        abort_en[4] = { 0, 0, 1, 1 };
    uint16                         eye_width = 0;
    #if DSF_SMALL_EYE_DETECTION_LOG
    char                   msg_string[4][50] = {
                                   "Small Eye SCREEN x: RD SIPI Aperture width +60mv",
                                   "Small Eye SCREEN x: RD SIPI Aperture width -60mv",
                                   "Small Eye SCREEN 4: RD min_eye_width_mask_top",
                                   "Small Eye SCREEN 4: RD min_eye_width_mask_bottom"
                                    };
    #endif

#define GLOBAL_X            0
#define LOCAL_X             1
#define GLOBAL_V_OFFSET     8


    

   uint8              hprx_temp[NUM_CA_PCH] = {0};
    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_ca[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;

    // =========================================HP Vref training init==============================================================
    

    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        // Set decouple bit (RMW).This sets the IOqualifier to MPVREF. This is required durining training. bit[0] = 0 => MPRX
        decouple_bit[phy_x] = HWIO_INX (reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG);
        decouple_bit[phy_x] = (decouple_bit[phy_x] | (1 << 1));
        HWIO_OUTX (reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG, decouple_bit[phy_x]);
    }
	
    for (ca_x = 0; ca_x < ca; ca_x++)
    {
        // Set MPVREF value to IOqualifier to mprx vref reg.  This value is to be set in CA PHY
        DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],ioqual_vref);
        
        // Get trained Vref
        hprx_temp[ca_x]=  DDR_PHY_hal_get_global_vref_hprx(reg_offset_ddr_ca[ca_x]);
	}
    // ============================================================================================================================
            for (vref  = 0;vref <  4 ; vref++)  
            {
                //  =================================Set Vref start
                for (ca_x = 0; ca_x < ca; ca_x++)
                {
                    // Set Vref check points
                    vref_chk_points[0] = hprx_temp[ca_x]+3;
                    vref_chk_points[1] = (hprx_temp[ca_x]<3) ? 0 : hprx_temp[0]-3;
                    vref_chk_points[2] = hprx_temp[ca_x]+training_params_ptr->rd_dqdqs.min_eye_width_mask_top;
                    vref_chk_points[3] = (hprx_temp[ca_x] < training_params_ptr->rd_dqdqs.min_eye_width_mask_bottom) ? 0 : hprx_temp[0]-training_params_ptr->rd_dqdqs.min_eye_width_mask_bottom;
                    
                    // Make Vref check point valid
                    if (vref_chk_points[vref]< 0){vref_chk_points[vref]= 0;}
                    if (vref_chk_points[vref]>17){vref_chk_points[vref]=17;}
                    
                    // Sweep Coarse VREF
                    DDR_PHY_hal_cfg_global_vref_hprx(reg_offset_ddr_ca[ca_x],vref_chk_points[vref]);
				
                    #if DSF_SMALL_EYE_DETECTION_LOG
                    if (prfs_index == 7)
                    {  
                        ddr_printf(DDR_NORMAL, "\n %s ,Vref check point = %d (step) = %d (mv), Frequency = %d, Channel = %d, Rank = %d", msg_string[vref], vref_chk_points[vref], vref_chk_points[vref] * 21, clk_freq_khz, ch, cs);
                    }   
                    #endif
                    
                }                                        
                //  =================================Set Vref end
                
                eye_width = DDRSS_rd_dqdqs_get_min_eyewith (ddr, 
                                ch, 
                                cs, 
                                ca,
                                dq,
                                training_params_ptr,
                                (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                clk_freq_khz,
                                prfs_index,
                                freq_eq_max_training);
                #if DSF_SMALL_EYE_DETECTION_LOG
                ddr_printf(DDR_NORMAL,"\n");
                #endif

				if ((abort_en[vref]==TRUE) && (eye_width<training_params_ptr->rd_dqdqs.min_eye_width) && (prfs_index == 7))
                {
                    if ((vref == 2) && (training_params_ptr->rd_dqdqs.min_eye_width_mask_top_enable==1) )
                    {
                        ddr_printf(DDR_ERROR, "\n SCREEN 4: RD min_eye_width_mask_top Abort (eye_height_mask_top = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", eye_width, clk_freq_khz, ch, cs );  
                        training_params_ptr->small_eye_abort = 1;
                    }
                    if ((vref == 3) && (training_params_ptr->rd_dqdqs.min_eye_width_mask_bottom_enable==1) )
                    {
                        ddr_printf(DDR_ERROR, "\n SCREEN 4: RD min_eye_width_mask_bottom Abort (eye_height_mask_bottom = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", eye_width, clk_freq_khz, ch, cs );  
                        training_params_ptr->small_eye_abort = 1;
                    }

				}
            }  //End of Vref loop


    // ============================================================================================================================
    //Set  Coarse (global)VREF  
    for (ca_x = 0; ca_x < ca; ca_x++)
    {                
        DDR_PHY_hal_cfg_global_vref_hprx(reg_offset_ddr_ca[ca_x],hprx_temp[ca_x]); 
    }//ca_x
    // ============================================================================================================================

            for (phy_x = 0; phy_x < dq; phy_x++)
            {       
                ca_x=phy_x>>1;//ca_x=0 for DQ=0,1 and ca_x=1 for DQ=2,3)
                // ================================================================================================================
                //exit vref training trainig mode
                // Restore decouple bit.
                decouple_bit[phy_x] = (decouple_bit[phy_x] & 0xFFFFFFFD);
                HWIO_OUTX(reg_offset_ddr_dq[phy_x], DDR_PHY_DDRPHY_PAD_DQS_1_CFG, decouple_bit[phy_x]);
                // restore mprx vref to mprx vref reg.             
                DDR_PHY_hal_cfg_global_vref_mprx(reg_offset_ddr_ca[ca_x],
                                            0x1F);
                // Restore the IO Qualifier local VREF
                DDR_PHY_hal_cfg_local_vref(reg_offset_ddr_dq[phy_x],
                                            cs,
                                            10,                                            
                                            DQS_VREF
                                            );
            // ====================================================================================================================
            }//phy_x

			
			
			
			

 return(1);
}// END OF FUNCTION

