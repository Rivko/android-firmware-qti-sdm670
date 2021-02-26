/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY
$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_pllprecal.c#3 $
================================================================================*/

#include "ddrss.h"
#include "ddrss_mccc.h"

void Training_DDRSS_PLLPRECAL      (BOOT_DDR_STRUCT *ddr, 
                                    DDR_CHANNEL channel, 
                                    DDR_CHIPSELECT chip_select, 
                                    uint32 clk_freq_khz,  
                                    uint8 prfs_index, 
                                    training_params_t *training_params_ptr,
                                    uint8             *local_vars)
{
   if (!((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && ((ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)))){ 
    uint8 ch				        = 0;
    uint8 num_ch			        = ddr->shrm_ddr_ptr->misc.num_channel;
    uint32 active_pll               = 0;
    uint32 reg_offset_ddrcc         = 0;  
    uint32 DDRCC_threshold          = 0;
    uint32 PLL0_Cal[4]              ={0};
    uint32 PLL1_Cal[4]              ={0};
    uint32 DDRCC_prfs_indx_fmax[8]  ={0};
    uint8 index                     = 0;
    
    reg_offset_ddrcc = REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;

    DDRCC_threshold = (CONVERT_CYC_TO_PS /HWIO_INXF (reg_offset_ddrcc , DDR_CC_DDRCC_TOP_CTRL_CFG1, GCC_MODE_THRESHOLD));      
    if (clk_freq_khz > DDRCC_threshold)
    {          
        Get_LUT_band_DDRCC(reg_offset_ddrcc, clk_freq_khz, DDRCC_prfs_indx_fmax);
#if DSF_PLLPRECAL_TRAINING_LOG_IN
        for (int i=0; i < 8; i++)
        {
            ddr_printf(DDR_NORMAL,"++PRFS_FMAX @DDRCCBand %d : %d Mhz\n", i, DDRCC_prfs_indx_fmax[i]); 
        }
#endif        
        
        for (index = 0; index < 8; index++) 
        {
            if (clk_freq_khz < DDRCC_prfs_indx_fmax[index])
                break;
        }
#if DSF_PLLPRECAL_TRAINING_LOG_IN
        ddr_printf(DDR_NORMAL,"++Freq: %d MHz,   DDRCC PRFS Band: %d \n", clk_freq_khz, index); 
#endif        
        //Disable PLL Pre-Calibration Mux
        for (ch = 0; ch < num_ch; ch++) 
        {
            reg_offset_ddrcc = REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;
            
            //PLL outputs need to be unclamped 
            HWIO_OUTXF2 (reg_offset_ddrcc , DDR_CC_DDRCC_TOP_CTRL_CFG,PLL0_SLEEP_B,PLL1_SLEEP_B, 0x1, 0x1);    
            
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL0_SYSTEM_MUXES, 0x0);    
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL1_SYSTEM_MUXES, 0x0);
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL0_CALIBRATION_SETTINGS,0x40);
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL1_CALIBRATION_SETTINGS,0x40);
            
            //PLL outputs need to be clamped 
            HWIO_OUTXF2 (reg_offset_ddrcc , DDR_CC_DDRCC_TOP_CTRL_CFG,PLL0_SLEEP_B,PLL1_SLEEP_B, 0x0, 0x0);    
        }

        ddr_external_set_clk_speed (clk_freq_khz);

#if DSF_PLLPRECAL_TRAINING_LOG_IN        
        ddr_printf(DDR_NORMAL,"++frequcny: %d Mhz\n", clk_freq_khz);       
#endif

        for (ch = 0; ch < num_ch; ch++) 
        {
            reg_offset_ddrcc = REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;

            // Determine the current active pll
            active_pll = HWIO_INXF (reg_offset_ddrcc, DDR_CC_DDRCC_PLLCTRL_STATUS, ACTIVE_PLL);	
            if (active_pll == 0)
            {
                PLL0_Cal[ch] = HWIO_INX (reg_offset_ddrcc, DDR_CC_DDRCC_QLINKPLL0_BAND_SEL_CAL);
            }
            else
            {
                PLL1_Cal[ch] = HWIO_INX (reg_offset_ddrcc, DDR_CC_DDRCC_QLINKPLL1_BAND_SEL_CAL);
            }
        }  
        
        //Switch to other PLL
        ddr_external_set_clk_speed (clk_freq_khz);      
        for (ch = 0; ch < num_ch; ch++)
        {
            reg_offset_ddrcc = REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;

            // Determine the current active pll
            active_pll = HWIO_INXF (reg_offset_ddrcc, DDR_CC_DDRCC_PLLCTRL_STATUS, ACTIVE_PLL);	
            if (active_pll == 0)
            {
                PLL0_Cal[ch] = HWIO_INX (reg_offset_ddrcc, DDR_CC_DDRCC_QLINKPLL0_BAND_SEL_CAL);
            }
            else
            {
                PLL1_Cal[ch] = HWIO_INX (reg_offset_ddrcc, DDR_CC_DDRCC_QLINKPLL1_BAND_SEL_CAL);
            }
        } 

        for (ch = 0; ch < num_ch; ch+=2) 
        {        
            if(ch < NUM_CH) /* To Fix Klocwork complain of ch taking more than value for an array(PLL0/1_Cal) of size 4*/
	    {
            reg_offset_ddrcc = REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;
#if DSF_PLLPRECAL_TRAINING_LOG_IN            
            ddr_printf(DDR_NORMAL,"++Channel: %d, PLL Calibration value: PLL0: %x,  PLL1: %x \n",ch, PLL0_Cal[ch], PLL1_Cal[ch]);
#endif            
            
            // in DDRCC offest between each PRFS band is 0x3C (This is not same as DDRPHY offset)
            HWIO_OUTX(reg_offset_ddrcc +(index*0x3C), DDR_CC_DDRCC_FPMEXT_PERF0_CTL_12_CFG, PLL0_Cal[ch]);
            HWIO_OUTX(reg_offset_ddrcc +(index*0x3C), DDR_CC_DDRCC_FPMEXT_PERF0_CTL_13_CFG, PLL1_Cal[ch]);
            }  
        }
        
#if DSF_PLLPRECAL_TRAINING_LOG_IN        
        ddr_printf(DDR_NORMAL, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
        
    }
    else
    {
        //Enable PLL Pre-Calibration Mux
        for (ch = 0; ch < num_ch; ch++) 
        {
            reg_offset_ddrcc = REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;
            
            //PLL outputs need to be unclamped 
            HWIO_OUTXF2 (reg_offset_ddrcc , DDR_CC_DDRCC_TOP_CTRL_CFG,PLL0_SLEEP_B,PLL1_SLEEP_B, 0x1, 0x1);    
            
            //Set FPMEXT_CTL_12_ADDRESS to "DDRCC_QLINKPLL0_ANALOG_CONTROLS_THREE" which is 0x720/4= 0x1C8
            HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_FPM_EXT_ADDR_CFG0, FPMEXT_CTL_12_ADDRESS, 0x1C8);
            //Set FPMEXT_CTL_12_ADDRESS to "DDRCC_QLINKPLL1_ANALOG_CONTROLS_THREE" which is 0x8F0/4= 0x23C
            HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_FPM_EXT_ADDR_CFG0, FPMEXT_CTL_13_ADDRESS, 0x23C);
            
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL0_SYSTEM_MUXES, 0x20);    
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL1_SYSTEM_MUXES, 0x20);
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL0_CALIBRATION_SETTINGS,0x44);
            HWIO_OUTX (reg_offset_ddrcc , DDR_CC_DDRCC_QLINKPLL1_CALIBRATION_SETTINGS,0x44);
            
            //PLL outputs need to be clamped 
            HWIO_OUTXF2 (reg_offset_ddrcc , DDR_CC_DDRCC_TOP_CTRL_CFG,PLL0_SLEEP_B,PLL1_SLEEP_B, 0x0, 0x0);    
        }    
    }
}
}
