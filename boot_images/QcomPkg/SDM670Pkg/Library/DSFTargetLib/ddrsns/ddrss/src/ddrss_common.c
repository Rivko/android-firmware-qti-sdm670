/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_common.c#10 $
$DateTime: 2019/08/01 06:25:20 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
05/11/14   arindamm     First edit history header. Add new entries at top.
================================================================================*/

#include "ddrss.h"
#include "target_config.h"
#include "ddr_phy.h"
#include "mc_pxi.h"
#include "mc.h"
//#include "msmhwio_tlmm.h"

#define GPIO_PXI 0
#define TRAIN_WR_CDC_FINE_STEPS_OFFSET 5

uint64 training_address[NUM_CH][NUM_CS];
uint32 PRFS_index_table[] = {F_RANGE_0, F_RANGE_1, F_RANGE_2, F_RANGE_3, F_RANGE_4, F_RANGE_5, F_RANGE_6, F_RANGE_7};
 
uint8  fail_flag_per_byte[4] = {0};
extern uint8 bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY_CONNECTED_NO_DBI];
extern uint8 byte_remapping_table [NUM_CH][NUM_DQ_PCH];

uint8  ca_training_pattern_lpddr4[CA_PATTERN_NUM][3] = {
  { 0x00, 0x3F, 0x00 },
  { 0x3F, 0x00, 0x3F },
  { 0x15, 0x2a, 0x15 },
  { 0x2a, 0x15, 0x2a }};
  
#define NUM_FIXED_PATTERNS 22

uint64 fixed_patterns[NUM_FIXED_PATTERNS] =
{
    0x0000000000000000,
    0x0000000000000000,
    0xFF00FF00FF00FF00,
    0x00FF00FF00FF00FF,
    0xF00FF00FF00FF00F,
    0x0FF00FF00FF00FF0,
    0xF0F1F0F1F0F1F0F1,
    0xF1F0F1F0F1F0F1F0,
    0xAA55AA55AA55AA55,
    0x55AA55AA55AA55AA,
    0x5AA55AA55AA55AA5,
    0xA55AA55AA55AA55A,
    0x5A5B5A5B5A5B5A5B,
    0x5B5A5B5A5B5A5B5A,
    0xFE01FE0101FE01FE,
    0xFD02FD0202FD02FD,
    0xFB04FB0404FB04FB,
    0xF708F70808F708F7,
    0xEF10EF1010EF10EF,
    0xDF20DF2020DF20DF,
    0xBF40BF4040BF40BF,
    0x7F807F80807F807F,
};

#define NUM_DESIGN_PATTERNS 2

uint64 design_patterns[NUM_DQ_PCH][NUM_DESIGN_PATTERNS] =
{
    {
        0x0102040810204080,
        0xFEFDFBF7EFDFBF7F,
    },
    {
        0x0102040810204080,
        0xFEFDFBF7EFDFBF7F,
    },
};

uint64 pseudo_random_data_seeds[NUM_DQ_PCH] =
{
    0xFEDCBA9876543210,
    0xFEDCBA9876543210,
};

uint32 write_in_pattern[] __attribute__((aligned(8))) = {
0xDEADBEEF, 0x5A5A5A5A, 0xA5A5A5A5, 0xFEEDFACE, 0xCAFEBABE, 0xA5A5A5A5, 0x5A5A5A5A, 0x0BADF00D, 
0xDEADBEEF, 0x5A5A5A5A, 0xA5A5A5A5, 0xFEEDFACE, 0xCAFEBABE, 0xA5A5A5A5, 0x5A5A5A5A, 0x0BADF00D,
0x22222222, 0x5a5a5a5a, 0xa5a5a5a5, 0x5a5a5a5a, 0x0f0f0f0f, 0xf0f0f0f0, 0x00000000, 0xffffffff, 
0xa5a5a5a5, 0x5a5a5a5a, 0xa5a5a5a5, 0x5a5a5a5a, 0x0f0f0f0f, 0xf0f0f0f0, 0x00000000, 0xffffffff,
0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 
0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
0x66666666, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 
0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
0xfcfcfcfc, 0x00000000, 0xfcfcfcfc, 0x00000000, 0xfcfcfcfc, 0x00000000, 0xfcfcfcfc, 0x00000000, 
0xfcfcfcfc, 0x00000000, 0xfcfcfcfc, 0x00000000, 0xfcfcfcfc, 0x00000000, 0xfcfcfcfc, 0x00000000,
0x55555555, 0xa5a5a5a5, 0x5a5a5a5a, 0xa5a5a5a5, 0xf0f0f0f0, 0x0f0f0f0f, 0xffffffff, 0x00000000, 
0x5a5a5a5a, 0xa5a5a5a5, 0x5a5a5a5a, 0xa5a5a5a5, 0xf0f0f0f0, 0x0f0f0f0f, 0xffffffff, 0x00000000,
0xf7f7f7f7, 0x08080808, 0xf7f7f7f7, 0x08080808, 0xf7f7f7f7, 0x08080808, 0xf7f7f7f7, 0x08080808, 
0xf7f7f7f7, 0x08080808, 0xf7f7f7f7, 0x08080808, 0xf7f7f7f7, 0x08080808, 0xf7f7f7f7, 0x08080808,
0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 
0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 
0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A,
0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A, 0x5B5B5B5B, 0x5A5A5A5A,

0x3789D91E, 0x19E69FC4, 0x484E3BE0, 0xCD6014C4, 0x77D1D4B1, 0xAB30DF90, 0xCB1F041D, 0xFA3D26AB, 
0x63E23084, 0x03DD0851, 0x18577023, 0x913FB1B5, 0xE81D6955, 0xCF7163D4, 0x2A452F75 ,0x01E6629F,
0x797B2280, 0x9ACE7CD7, 0xA06B1A1D, 0xBE79D5DB, 0xEBBFF686, 0x6EBB85FF, 0xBF31749A, 0x29F2E6C7, 
0x8C91AAFC, 0xAB1AAB75, 0xAC48D44E, 0x504C17D0, 0xB1C5B12A, 0xAC329613, 0xBA5D149B ,0x592054F1,
0x753B266D, 0x20944DEB, 0x3220F4C2, 0xDFDF25CD, 0x80D363CB, 0x63EEBF1B, 0x4C1A820C, 0x1E54639A, 
0x127F2554, 0xEB3C2935, 0xCDAAFAEF, 0xC84B4320, 0x2A0B9168, 0xA4CC98BF, 0xF3609880 ,0xD79CAE43,
0x713BB864, 0x72FA35F8, 0x6DED7DF2, 0x23A354E0, 0xCB34F01A, 0xB035D5A0, 0x86C06BD6, 0xEFAB7C43, 
0x74DFBE03, 0xC1E91236, 0x09CAD86F, 0x1C0D78EC, 0xDA6B8136, 0xA18EA81C, 0xEA4D7192 ,0x8B331A3B,
0x81810414, 0xA4555EFD, 0x31722982, 0xFC0E7445, 0x883A76E5, 0x4A9718FF, 0x9577C0A3, 0x49F2D41B, 
0x5D8D9AC4, 0xB48E6E84, 0x4451DBED, 0x0C03D7D7, 0x398540DD, 0xBDC56DB1, 0x29555DDF ,0x47B64FF0,
0x3DF87F32, 0x4697A951, 0x78FFBEF0, 0xA71F8CBD, 0xC449842F, 0x036EDD7C, 0x4E020585, 0xFB39FB64, 
0x4FF1CEC2, 0x7D3A52FB, 0xF8A2D60B, 0x1FD68B15, 0x092A6BD6, 0x2386376D, 0xCCFC9C10 ,0xB8257024,
0xC1A1FDED, 0x186DD915, 0xA0AC5EFE, 0x5363D067, 0x2507A9F2, 0xADD4BC69, 0xFC266ABB, 0x835F251B, 
0x9FA61759, 0x071198C9, 0x1000275A, 0xC7883CC7, 0xF3C7AB76, 0x20E052FD, 0xE3AA85F3 ,0x042B74AA
};

//================================================================================================//
// search through frequency table to automatically identify training frequency
//================================================================================================//
void identify_training_frequencies(BOOT_DDR_STRUCT *ddr, 
                                   DDR_CHANNEL channel, 
				                           DDR_CHIPSELECT chip_select, 
				                           uint32 freq, 
				                           uint8 prfs_index, 
				                           training_params_t *training_params_ptr,
                                   uint8             *local_vars
                                   ) 
{
 
    uint8  training_ddr_freq_indx = (sizeof(ddr->shrm_ddr_ptr->misc.training_freq_in_khz)/sizeof(uint32));
    uint32 PRFS_BAND_THRESHOLD[NUM_PRFS_BANDS]  = {F_RANGE_0,  F_RANGE_1,    F_RANGE_2,    F_RANGE_3,    F_RANGE_4,    F_RANGE_5, F_RANGE_6,   F_RANGE_7 };    
  //uint32 training_freq_table[MAX_TRAINING_FREQ_NUM] = {0};
    uint8  training_table_index = 0; 
    uint8  clock_plan_max_idx;
    
    prfs_index = MAX_TRAINING_FREQ_INDEX;
        
    // TODO : connect to ecdt 
    clock_plan_max_idx = 11;//ddr->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels;
    
    // if 2 freq are in the same band please update the clock plan min
    for (training_ddr_freq_indx = clock_plan_max_idx; training_ddr_freq_indx > (clock_plan_max_idx-MAX_TRAINING_FREQ_NUM); training_ddr_freq_indx--)
    { 
      if (prfs_index>=1)
      {
        if ((ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[training_ddr_freq_indx].clk_freq_in_khz) < PRFS_BAND_THRESHOLD[prfs_index])
        {             
          if ((ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[training_ddr_freq_indx].clk_freq_in_khz) > PRFS_BAND_THRESHOLD[prfs_index -1]) //make sure we have a frequency in this band
          
          {
            //training_freq_table[training_table_index] = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[training_ddr_freq_indx].clk_freq_in_khz;
            ddr->shrm_ddr_ptr->misc.training_freq_in_khz[training_table_index] = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[training_ddr_freq_indx].clk_freq_in_khz;
            training_table_index++;
            prfs_index--;
          }          
        }
      }
    }
}

//================================================================================================//
// Memory read from DRAM.
// This function will read data of a given size to a given address supplied by the caller.
//================================================================================================//
void ddr_mem_copy(uint32 * source, uint32 * destination, uint32 size, uint32 burst)
{
    uint32 i;

    if(burst)
    {
        /* perform a burst write */
        #ifndef CDVI_SVTB
        __blocksCopy(source, destination, size);
        #endif
    }
    else
    {
        for(i = 0; i < size; i++)
        {
            destination[i] = source[i];
        }
    }
}

#if DSF_PXI_TRAINING_EN
//================================================================================================//
// Memory write
//================================================================================================//
void DDRSS_mem_write_PXI ( uint8 ch, uint8 cs)
{
    uint32 REG_OFFSET_MC_PXI              =  0;
	uint32 REG_OFFSET_MC_PXI_SECURE       =  0;
	uint8 sw_status = 0;
    
	REG_OFFSET_MC_PXI            = REG_OFFSET_MC_PXI(ch);
    REG_OFFSET_MC_PXI_SECURE     = REG_OFFSET_MC_PXI_SECURE(ch);
	  
	  //set PRBS seed (reset PRBS generator)
      MC_PXI_GPR_Config (ch, PXI_DQWD, 0, PRBS_SEED_BYTE0);
      MC_PXI_GPR_Config (ch, PXI_DQWD, 1, PRBS_SEED_BYTE1);
	  
	  //PXI_RAW_OPEN, PXI_IN_SEL, PXI_OUT_SEL (Get control of bus)
      HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_SEL, 0x7);

	  //PXI Go
#if GPIO_PXI
	  HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_GPIO_MASK, 0x1F0010);
	  HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_GPIO_CTRL, 0x1);
/*	  switch (ch)
	  {
		  case 3:
		    HWIO_OUTF (TLMM_GPIO_CFG13, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG13, FUNC_SEL, 0x4);
		    HWIO_OUTF (TLMM_GPIO_CFG12, FUNC_SEL, 0x4);
			break;
		  case 2:
		    HWIO_OUTF (TLMM_GPIO_CFG11, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG11, FUNC_SEL, 0x6);
		    HWIO_OUTF (TLMM_GPIO_CFG10, FUNC_SEL, 0x6);
			break;
		  case 1:
		    HWIO_OUTF (TLMM_GPIO_CFG9, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG9, FUNC_SEL, 0x6);
		    HWIO_OUTF (TLMM_GPIO_CFG8, FUNC_SEL, 0x8);
			break;
		  case 0:
		  default:
		    HWIO_OUTF (TLMM_GPIO_CFG7, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG7, FUNC_SEL, 0x7);
		    HWIO_OUTF (TLMM_GPIO_CFG6, FUNC_SEL, 0x3);
			break;
	  }*/
	  pxi_gpio_trigger(ch);
#else
	  MC_PXI_Control_Config (ch, 0x1E, 0x0, 0x0);
#endif

      //wait for all tables to go idle
      while ((HWIO_INX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_STATUS) & 0x0000001F) != 0x0000001F);

	  // clear PXI Go
#if GPIO_PXI
	HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_GPIO_CTRL, 0x0);
#else
	  MC_PXI_Control_Config (ch, 0x0, 0x0, 0x0);
#endif

	  //Give control back to MC
	  HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_SEL, 0x0);
}

#else
//================================================================================================//
// Memory write
//================================================================================================//
void DDRSS_mem_write_PXI ( uint8 ch, uint8 cs)
{
   ddr_mem_copy (write_in_pattern, (uint32 *)(size_t)training_address[ch][cs], DQ_TRAINING_PATTERN_SIZE, 1);
}
#endif

//================================================================================================//
// Used for saving and restoring registers modified by training
//================================================================================================//
uint32 dq_address_list[] =
{
    HWIO_DDR_PHY_DDRPHY_CMCDCWR_TRAFFIC_BYP_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_CMCDCWRLVL_MODE_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_UPDATE_INTF_CFG_ADDR(0),
    0
};
#define NUM_DQ_STACK (sizeof(dq_address_list) / sizeof(uint32))
uint32 dq_value_stack[NUM_DQ_STACK] = {0};

uint32 ca_address_list[] =
{
    HWIO_DDR_PHY_DDRPHY_CMCDCWR_TRAFFIC_BYP_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_CMCDCWRLVL_MODE_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_FPM_PRFS_4_PWRS_0_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_FPM_PRFS_5_PWRS_0_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_FPM_PRFS_6_PWRS_0_CFG_ADDR(0),
    HWIO_DDR_PHY_DDRPHY_FPM_PRFS_7_PWRS_0_CFG_ADDR(0),
    0
};
#define NUM_CA_STACK (sizeof(ca_address_list) / sizeof(uint32))
uint32 ca_value_stack[NUM_CA_STACK] = {0};

/*  Write all the mode register settings needed before initial training or training restore */
void pre_training_mode_reg_writes (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{   

	// DRAM MR22 Setup
    
    /* Set FPS-WR back to 0 */                 
    MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_13, (0x0 | MR13_BASE)); //enable RRO and disable DM 
    /* Initialize MR22 for SoC ODT */
    /* FSP0, MR22 = 0x0 */
    MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_22, 0x0);

	MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_12, 0x5D);
    
    /* Set FPS-WR to 1 */
    MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_13, (0x40 | MR13_BASE)); //enable RRO and disable DM
    
    if (ddr->shrm_ddr_ptr->detected_ddr_device[0].device_type == DDR_TYPE_LPDDR4X) 
    {
		if (chip_select == DDR_CS_BOTH) 
		{	
			//If we have byte mode or hybrid mode, make sure there is no double termination 
			if((((MC_MR_Read(channel,DDR_CS0,JEDEC_MR_0)) & 0x2) ==0x2))  // Byte-mode check in Bit1 of MR0; check in byte-mode DRAM datasheet
			{  
				//check if device supports byte mode, both ranks return same value if device supports byte mode	
				//if both byte mode CS0=0x84, CS1=0xFC
				if(((((MC_MR_Read(channel,DDR_CS0,JEDEC_MR_8)) & 0xC0)>>6) ==DEVICE_WIDTH_IS_x8) && ((((MC_MR_Read(channel,DDR_CS1,JEDEC_MR_8)) & 0xC0)>>6) ==DEVICE_WIDTH_IS_x8))
				{
					MC_MR_Write (ddr, channel, DDR_CS0, JEDEC_MR_22, 0x84);
					MC_MR_Write (ddr, channel, DDR_CS1, JEDEC_MR_22, 0x6C);
				}
				
				//if CS0 is byte mode and CS1 is x16
				if(((((MC_MR_Read(channel,DDR_CS0,JEDEC_MR_8)) & 0xC0)>>6) ==DEVICE_WIDTH_IS_x8) && ((((MC_MR_Read(channel,DDR_CS1,JEDEC_MR_8)) & 0xC0)>>6) ==DEVICE_WIDTH_IS_x16))
				{
					MC_MR_Write (ddr, channel, DDR_CS0, JEDEC_MR_22, 0x4C);
					MC_MR_Write (ddr, channel, DDR_CS1, JEDEC_MR_22, 0x24);
				}
		
				//if CS0 is x16 and CS1 is x8
				if(((((MC_MR_Read(channel,DDR_CS0,JEDEC_MR_8)) & 0xC0)>>6) ==DEVICE_WIDTH_IS_x16) && ((((MC_MR_Read(channel,DDR_CS1,JEDEC_MR_8)) & 0xC0)>>6) ==DEVICE_WIDTH_IS_x8))
				{
					MC_MR_Write (ddr, channel, DDR_CS0, JEDEC_MR_22, 0x24);
					MC_MR_Write (ddr, channel, DDR_CS1, JEDEC_MR_22, 0x4C);
				}	
			}
			else
			{
				MC_MR_Write(ddr, channel, DDR_CS0, JEDEC_MR_22, 0x04);
				MC_MR_Write(ddr, channel, DDR_CS1, JEDEC_MR_22, 0x2C);                                       
			}
        }
        else if (chip_select == DDR_CS0) 
        {
            MC_MR_Write(ddr, channel, DDR_CS0, JEDEC_MR_22, 0x04);//Single rank devices
        }
    }
    
    else if (ddr->shrm_ddr_ptr->detected_ddr_device[0].device_type == DDR_TYPE_LPDDR4) 
    {
        MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_22, 0x23);
    }

    MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_12, 0x18);
	
    MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_14, 0x1A);
    /* Set FPS-WR back to 0 */                 
    MC_MR_Write(ddr, channel, chip_select, JEDEC_MR_13, (0x0 | MR13_BASE)); //enable RRO and disable DM 	
}

void pre_training_setup(BOOT_DDR_STRUCT *ddr, 
                        DDR_CHANNEL channel, 
                        DDR_CHIPSELECT chip_select, 
                        uint32 freq, uint8 prfs_index, 
                        training_params_t *training_params_ptr,
                        uint8             *local_vars
                        ) 

{
#if DSF_PERIODIC_TRAINING_EN
    uint8 dit_freq_indx = 0;
#endif
	
#if DSF_PXI_TRAINING_EN
    uint8 ch = 0;
#endif
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    //Enable PHY XO Clock
    DDRSS_PHY_XO_Clk_Ctrl(1);
    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
    
  //uint32 reg_offset_osu;
  //uint32 reg_offset_mpe;
        
    /* Disable periodic SHKE events */
    MC_All_Periodic_Ctrl (ddr, channel, chip_select, 0/* 1 for enable, 0 for disable */);
    
    /* Disable power saving features */
    MC_Power_Saving_Ctrl (ddr, channel, chip_select, 0/* 1 for enable, 0 for disable */);
	
	pre_training_mode_reg_writes(ddr, channel, chip_select);
    
    // Save registers modified by for training purposes
    DDRSS_Register_Save_Restore(
                    SAVE,
                    NUM_DQ_STACK,
                    REG_OFFSET_DDR_PHY_CH(0) + DQ0_DDR_PHY_OFFSET,
                    dq_address_list,
                    dq_value_stack);
    DDRSS_Register_Save_Restore(
                    SAVE,
                    NUM_CA_STACK,
                    REG_OFFSET_DDR_PHY_CH(0) + CA0_DDR_PHY_OFFSET,
                    ca_address_list,
                    ca_value_stack);
       
     // Enable broadcast mode for all DQ and CA PHYs on both channels
     HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
               AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY0_CAsDQs);
     //Warlock in dual channel and does not contain AHB2PHY1 instance
     //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
     //          AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, 0x1FBF);      
     
     // Set TRAFFIC_CFG registers to bypass clock gating for training
     HWIO_OUTX (BROADCAST_BASE, DDR_PHY_DDRPHY_CMCDCWR_TRAFFIC_BYP_CFG, 0x0067FFFF);
    // HWIO_OUTX (BROADCAST_BASE, DDR_PHY_DDRPHY_CMCDCWRLVL_MODE_CFG, 0x00300);//wrlvl mode cfg : CGC overide mode   
     
     // Enable broadcast mode for CA only
    HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
        AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY0_CAs);
	//Warlock is dual channel and does not contain AHB2PHY1 instance
    //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
    //    AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY1_CAs);
        
     // Enable MP Vref for IO qualifier
     HWIO_OUTX (BROADCAST_BASE, DDR_PHY_DDRPHY_FPM_PRFS_4_PWRS_0_CFG, 0x7D8003);
     HWIO_OUTX (BROADCAST_BASE, DDR_PHY_DDRPHY_FPM_PRFS_5_PWRS_0_CFG, 0x7D8003);
     HWIO_OUTX (BROADCAST_BASE, DDR_PHY_DDRPHY_FPM_PRFS_6_PWRS_0_CFG, 0x7D8003);
     HWIO_OUTX (BROADCAST_BASE, DDR_PHY_DDRPHY_FPM_PRFS_7_PWRS_0_CFG, 0x7D8003);
      
#if DSF_PXI_TRAINING_EN

	MC_HW_Self_Refresh_Ctrl (ddr->shrm_ddr_ptr, channel, chip_select, 0);
    for (ch = 0; ch < NUM_CH; ch++)
    {   
        Initialize_PXI_Tables(ddr, ch);
    }

#else
    // Load the DRAM physical training addresses
    DDRSS_Get_Training_Address(ddr);
    
    // Repopulate write_in_pattern with x8 training data
    DDRSS_Create_Training_Data(ddr->shrm_ddr_ptr->misc.bus_width_pch);
#endif

  training_data_ptr->results.dit.dit_fine_step_limit  = DIT_FINE_STEP_LIMIT;
  training_data_ptr->results.dit.dit_interval_per_sec = DIT_INTERVAL_PER_SEC;

#if DSF_PERIODIC_TRAINING_EN
  //may need to change following to determine ACQ and TRAC indexes
  if (ddr->shrm_ddr_ptr->ddr_runtime.max_ddr_frequency >= DSF_MIN_PERIODIC_TRAINING_FREQ)
  {
    for (dit_freq_indx = 0; dit_freq_indx < ddr->shrm_ddr_ptr->ddr_runtime.ddr_num_clock_levels; dit_freq_indx++)
    { 
      if ((ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[dit_freq_indx].clk_freq_in_khz) == ddr->shrm_ddr_ptr->ddr_runtime.max_ddr_frequency)
      {
	     break;
      }
    }
    training_data_ptr->results.dit.dit_acq_ndx  = dit_freq_indx - 1;
    training_data_ptr->results.dit.dit_trac_ndx = dit_freq_indx;
  }
  else //no periodic training required
  {
    training_data_ptr->results.dit.dit_acq_ndx  = 0;
    training_data_ptr->results.dit.dit_trac_ndx = 0;
  }  
#else
  training_data_ptr->results.dit.dit_acq_ndx  = 0;
  training_data_ptr->results.dit.dit_trac_ndx = 0;	
#endif      
      

}

void post_training_setup(BOOT_DDR_STRUCT *ddr, 
                         DDR_CHANNEL channel, 
                         DDR_CHIPSELECT chip_select, 
                         uint32 freq, 
                         uint8 prfs_index, 
                         training_params_t *training_params_ptr,
                         uint8             *local_vars
                         ) 

{
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);
    
    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
    if (training_params_ptr->small_eye_abort == 1)
    {
       ddr_abort(); 
    }
    
    // Restore registers modified for training purposes
    HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
        AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY0_DQs);
	//Warlock is dual channel and does not contain AHB2PHY1 instance
    //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
    //    AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY1_DQs);
        
    DDRSS_Register_Save_Restore(
                    RESTORE,
                    NUM_DQ_STACK,
                    BROADCAST_BASE,
                    dq_address_list,
                    dq_value_stack);
                    
    HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
        AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY0_CAs);
	//Warlock is dual channel and does not contain AHB2PHY1 instance
    //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
    //    AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY1_CAs);
        
    DDRSS_Register_Save_Restore(
                    RESTORE,
                    NUM_CA_STACK,
                    BROADCAST_BASE,
                    ca_address_list,
                    ca_value_stack);
    
    // Save the restore data onto the stack
    ddr_printf (DDR_NORMAL,"\nSaving the DDR PHY Training Results\n\n");
    DDRSS_Training_Save_Restore(ddr, channel, SAVE);
    
    // Save read opcode offset for RCW
    ddr_external_memscpy(training_data_ptr->results.rcw.rd_opcode_offset,
        sizeof(training_data_ptr->results.rcw.rd_opcode_offset),
        ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset,
        sizeof(ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset));
	
    // Post Boot Training setup.
    DDRSS_Post_Boot_Training(ddr, channel, chip_select);
    
	/* Enable periodic SHKE events */
    MC_All_Periodic_Ctrl (ddr, channel, chip_select, 1/* 1 for enable, 0 for disable */);
    
    /* Enable power saving features */ 
    MC_Power_Saving_Ctrl (ddr, channel, chip_select, 1/* 1 for enable, 0 for disable */);
    
	//After training revert PHY_XO_CLK gating back to one time settings 
    DDRSS_PHY_XO_Clk_Ctrl(ddr->shrm_ddr_ptr->ddr_runtime.phy_xoclk);
    ddr_printf (DDR_NORMAL,"\nEND of DDR PHY Training\n\n");
}   
//================================================================================================//
// Frequency Scaling
//================================================================================================//

// Divide and remainder calculation functions for WRLVL CDC conversion
uint32 calc (uint32 step, uint32 i_delay, uint8 set_rem_out)
{
   uint32 div_rem = 0;

   // Divide or Mod depending on set_rem_out decode
   if ((step <= i_delay) && (step != 0)) {
      // Mod delay by step
      if (set_rem_out == 1) {
         div_rem   = (i_delay % step);
      }
      else {
         // Divide delay by step
         div_rem   = (i_delay / step);
         if (set_rem_out == 2) {
             div_rem = div_rem  * step;
         }
      }
   }
   // Bypass the result if illegal
   else {
       if (set_rem_out == 1) {
           div_rem = i_delay;
       }
   }

   // Return the result
   return div_rem;
}

boolean DDRSS_RD_midpoint_to_CDC (BOOT_DDR_STRUCT *ddr,
                                  uint32 middle,
                                  uint32 clk_freq_in_khz,
                                  uint8  ch,
                                  uint8  cs,
                                  uint8  phy_inx,
                                  uint8 freq_inx
                                 )
{
  uint32 coarse_cdc;
  uint32 fine_delay;
  uint32 fine_cdc;
  uint32 ddr_phy_chn_base;

  ddr_phy_chn_base = REG_OFFSET_DDR_PHY_CH(ch);

  // Start from freq band 3 (800MHz) up to band7 (1866MHz).
  if((clk_freq_in_khz > LOWEST_SCALED_FPRS_FREQ) && (clk_freq_in_khz < HIGHEST_TRAINED_FPRS_FREQ))
  {
    // When coarse delay exceed max value of CDC CSR, transfer the exceeding part to fine delay
    if (middle > ((COARSE_CDC_MAX_VALUE + 1) * COARSE_STEP_IN_PS))
    {
       coarse_cdc = COARSE_CDC_MAX_VALUE;
       fine_delay = (middle - (COARSE_CDC_MAX_VALUE + 1) * COARSE_STEP_IN_PS);
    }
    else
    {
       coarse_cdc = (middle / COARSE_STEP_IN_PS);
       fine_delay = (middle % COARSE_STEP_IN_PS);
    }
    
    fine_cdc = fine_delay / FINE_STEP_IN_PS;
    
    // When even fine delay exceeds max value of CDC CSR, return FALSE
    if (fine_cdc > FINE_CDC_MAX_VALUE) 
    {
        return FALSE;
    }
    else
    {
      DDR_PHY_hal_cfg_cdcext_slave_rd (ddr_phy_chn_base + DQ0_DDR_PHY_OFFSET + (phy_inx * DDR_PHY_OFFSET),
                                       0,
                                       coarse_cdc,
                                       fine_cdc,
                                       freq_inx
                                      ); 
      DDR_PHY_hal_cfg_cdcext_slave_rd (ddr_phy_chn_base + DQ0_DDR_PHY_OFFSET + (phy_inx * DDR_PHY_OFFSET),
                                       1,
                                       coarse_cdc,
                                       fine_cdc,
                                       freq_inx
                                      ); 
#if DSF_RD_DQDQS_TRAINING_LOG_IN
      ddr_printf(DDR_NORMAL,"SCALE ch = %d byte = %d source freq = %d scaled prfs = %d Coarse = %d Fine = %d\n",
                 ch,phy_inx,clk_freq_in_khz,freq_inx,coarse_cdc,fine_cdc);  
#endif                                       
    }
  }
  return TRUE;
}

void DDRSS_WR_midpoint_to_CDC (BOOT_DDR_STRUCT *ddr,
                               uint8 ch,
                               uint8 cs,
                               uint32 cdc_midpoint_ps,
                               uint8 half_cycle,
                               uint8 phy_inx,
                               uint32 target_freq,
                               uint8 freq_inx
                              )
{
  uint32 coarse_cdc;
  uint32 fine_delay;
  uint32 fine_cdc;
  uint32 ddr_phy_chn_base;
  uint32 period;
  uint32 T2;
 
  ddr_phy_chn_base = REG_OFFSET_DDR_PHY_CH(ch);
   
  period = CONVERT_CYC_TO_PS/target_freq;
  T2     = period/2;
  
  if (cdc_midpoint_ps > T2)
  {
    half_cycle++;
    cdc_midpoint_ps -= T2;
  }
  
  // When coarse delay exceed max value of CDC CSR, transfer the exceeding part to fine delay
  if (cdc_midpoint_ps > ((COARSE_CDC_MAX_VALUE + 1) * COARSE_STEP_IN_PS))
  {
     coarse_cdc = COARSE_CDC_MAX_VALUE;
     fine_delay = (cdc_midpoint_ps - (COARSE_CDC_MAX_VALUE + 1) * COARSE_STEP_IN_PS);
  }
  else
  {
     coarse_cdc = (cdc_midpoint_ps / COARSE_STEP_IN_PS);
     fine_delay = (cdc_midpoint_ps % COARSE_STEP_IN_PS);
  }
  
  fine_cdc = fine_delay / FINE_STEP_IN_PS;

  DDR_PHY_hal_cfg_cdcext_slave_wr (ddr_phy_chn_base + DQ0_DDR_PHY_OFFSET + (phy_inx * DDR_PHY_OFFSET),
                                   0, //cs
                                   coarse_cdc,
                                   fine_cdc,
                                   freq_inx
                                  );
                           
  DDR_PHY_hal_cfg_wrext_ctl_update (ddr_phy_chn_base + DQ0_DDR_PHY_OFFSET + (phy_inx * DDR_PHY_OFFSET),
                                    freq_inx,
                                    0,//cs
                                    half_cycle
                                   ); 
  DDR_PHY_hal_cfg_cdcext_slave_wr (ddr_phy_chn_base + DQ0_DDR_PHY_OFFSET + (phy_inx * DDR_PHY_OFFSET),
                                   1,//cs
                                   coarse_cdc,
                                   fine_cdc,
                                   freq_inx
                                  );
                           
  DDR_PHY_hal_cfg_wrext_ctl_update (ddr_phy_chn_base + DQ0_DDR_PHY_OFFSET + (phy_inx * DDR_PHY_OFFSET),
                                    freq_inx,
                                    1,//cs
                                    half_cycle
                                   ); 
#if DSF_WR_DQDQS_TRAINING_LOG_IN
      ddr_printf(DDR_NORMAL,"SCALE ch = %d byte = %d  scaled prfs = %d Coarse = %d Fine = %d Half = %d\n",
                 ch,phy_inx,freq_inx,coarse_cdc,fine_cdc,half_cycle);  
#endif                                      
}
#if DSF_PXI_TRAINING_EN

#else
//================================================================================================//
// Get the flat 32-bit system address to which training patterns will be written to and read from.
// Adjust address for execution from RPM if needed.
// ===============================================================================================//
void DDRSS_Get_Training_Address(BOOT_DDR_STRUCT *ddr)
{

    // Channel 0
    training_address[0][0] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[0];
    training_address[0][1] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[0]; 
       
    // Channel 1       
    training_address[1][0] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[0] + (TRAINING_INTERLEAVE_ADDRESS_OFFSET * 1);                     
    training_address[1][1] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[0] + (TRAINING_INTERLEAVE_ADDRESS_OFFSET * 1); 
      
    // Channel 2      
    //training_address[2][0] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[0] + (TRAINING_INTERLEAVE_ADDRESS_OFFSET * 2);                    
    //training_address[2][1] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[0] + (TRAINING_INTERLEAVE_ADDRESS_OFFSET * 2); 
     
    // Channel 3     
    //training_address[3][0] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[0] + (TRAINING_INTERLEAVE_ADDRESS_OFFSET * 3);                    
    //training_address[3][1] = ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[0] + (TRAINING_INTERLEAVE_ADDRESS_OFFSET * 3); 
                                                                                         
}
#endif

void DDRSS_passing_region_scan( uint8 *start, uint8 *stop, uint8 fail_count_histogram[HISTOGRAM_SIZE], uint32 histogram_start, uint32 histogram_max)
{
    uint8 lenght_cur = 0;
    uint8 start_cur = histogram_start;
    uint8 stop_cur = 0;
    uint8 lenght_stop = 0;
    uint8 sweep;

    *stop  = 0;
    *start = 0xFF;
    for(sweep = histogram_start + 1; sweep < histogram_max - 1; sweep++)
    {
        if((fail_count_histogram[sweep - 1] != 0) && (fail_count_histogram[sweep] == 0))
        {
            start_cur  = sweep;
            stop_cur   = sweep;
            lenght_cur = 0;
        }
        if((fail_count_histogram[sweep] == 0) && (fail_count_histogram[sweep + 1] != 0))
        {
            stop_cur   = sweep;
            lenght_cur = stop_cur - start_cur + 1;
        }
        if((sweep == histogram_max - 2) && (fail_count_histogram[sweep] == 0) && (fail_count_histogram[sweep + 1] == 0))
        {
            stop_cur   = sweep + 1;
            lenght_cur = stop_cur - start_cur + 1;
            }
        if(lenght_cur > lenght_stop)
        {
            lenght_stop = lenght_cur;
            *start      = start_cur ;
            *stop       = stop_cur  ;
        }
    }
}




void DDRSS_cdcref_init(uint32 cdc_ref[HISTOGRAM_SIZE][4])
{
    int coarse_x    =   0;
    int fine_x      =   0;
    int knob        =   0;
    
    for (coarse_x=0; coarse_x< COARSE_CDC_MAX_VALUE; coarse_x++)
    {
        for (fine_x=0; fine_x< FINE_STEPS_PER_COARSE; fine_x++)
        { 
            knob                    = coarse_x * FINE_STEPS_PER_COARSE  + fine_x ;
            cdc_ref[knob][COARSE_L] = coarse_x  ;
            cdc_ref[knob][FINE_L  ] = fine_x    ;
            cdc_ref[knob][DELAY_L ] = coarse_x * COARSE_STEP_IN_PS      + fine_x * FINE_STEP_IN_PS;  ;
            cdc_ref[knob][CDC_L   ] = (fine_x   <<5)                    | coarse_x  ;
        }
    }
}

void DDRSS_cdcref_init_wr(uint32 cdc_ref[HISTOGRAM_SIZE][4])
{
    int coarse_x    =   0;
    int fine_x      =   0;
    int knob        =   0;
    
    for (coarse_x=0; coarse_x< COARSE_CDC_MAX_VALUE; coarse_x++)
    {
        for (fine_x=0; fine_x< FINE_STEPS_PER_COARSE; fine_x++)
        { 
            knob                    = coarse_x * FINE_STEPS_PER_COARSE  + fine_x ;
            cdc_ref[knob][COARSE_L] = coarse_x  ;
            cdc_ref[knob][FINE_L  ] = fine_x  +5 ;
            cdc_ref[knob][DELAY_L ] =  cdc_ref[knob][COARSE_L] * COARSE_STEP_IN_PS      + cdc_ref[knob][FINE_L  ] * FINE_STEP_IN_PS;  ;
            cdc_ref[knob][CDC_L   ] = (cdc_ref[knob][COARSE_L]   <<5)                   | cdc_ref[knob][FINE_L  ]  ;
        }
    }
}

uint8 DDRSS_Get_Freq_Index (BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz)
{
   uint8 prfs_index;

   for (prfs_index = 0; (prfs_index < sizeof(PRFS_index_table)/sizeof(uint32)); prfs_index++)
   {
      if (clk_freq_khz < PRFS_index_table[prfs_index])
         break;
   }

   return prfs_index;
}

uint16 DDRSS_dq_remapping (uint8 pattern)
{
    uint8  bit;
    uint16 return_data = 0;    
    uint8  bit_data = 0;
    uint8  byte   = 0;
    uint8 ch = 0;
    
    for (bit = 0; bit < PINS_PER_PHY_CONNECTED_NO_DBI; bit ++)
    {
        bit_data = (pattern >> bit) & 0x1;
        return_data = return_data | (bit_data << bit_remapping_bimc2phy_DQ [ch][byte][bit]);        
    }
    
    return return_data;
}

void DDRSS_MR_Write_on_die (BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 MR_addr, uint8 MR_data,uint8 die)
{ 
  // TODO : Update MR_Write_on_die
  //enabling just the “die”
  //HWIO_OUTXF2 ( REG_OFFSET_DPE(ch) , DPE_CONFIG_13, LP4_CHB_DISABLE, LP4_CHA_DISABLE, (1-die), die);
  //HWIO_OUTX   ( REG_OFFSET_DPE(ch) , DPE_CONFIG_4 , 1 );//load
  //HWIO_OUTX   ( REG_OFFSET_DPE(ch) , DPE_CONFIG_4 , 0 );
    
  //BIMC_MR_Write (CH_1HOT(ch), CS_1HOT(cs), MR_addr, MR_data);
  //enabling both die
  //HWIO_OUTXF2 ( REG_OFFSET_DPE(ch) , DPE_CONFIG_13 , LP4_CHB_DISABLE, LP4_CHA_DISABLE, 0, 0);
  //HWIO_OUTX   ( REG_OFFSET_DPE(ch) , DPE_CONFIG_4 , 1 ); //load
  //HWIO_OUTX   ( REG_OFFSET_DPE(ch) , DPE_CONFIG_4 , 0 );
}

#if DSF_PXI_TRAINING_EN
//================================================================================================//
// Memory read with results per bit
// This function will do a memory read and report pass/fail per BIT against the expected pattern
//================================================================================================//

uint32 DDRSS_mem_read_Get_Fail_pat_PXI   ( uint8 ch, uint8 cs)
{
    int     word_x          = 0;

    uint32  fail_in_word 	= 0;

    uint32 REG_OFFSET_MC_PXI              =  0;
	uint32 REG_OFFSET_MC_PXI_SECURE       =  0;
	uint8 sw_status = 0;
    uint8 reg_num = 0;
	
	REG_OFFSET_MC_PXI            = REG_OFFSET_MC_PXI(ch);
    REG_OFFSET_MC_PXI_SECURE     = REG_OFFSET_MC_PXI_SECURE(ch);
	  
	  //set PRBS seed (reset PRBS generator)
      MC_PXI_GPR_Config (ch, PXI_DQRD, 0, PRBS_SEED_BYTE0);
      MC_PXI_GPR_Config (ch, PXI_DQRD, 1, PRBS_SEED_BYTE1);
	  
	  //Clear read counter counting number of read valid beats
	  //MC_PXI_RD_Beat_Status_Clear (ch);
	  
	  //PXI_RAW_OPEN, PXI_IN_SEL, PXI_OUT_SEL (Get control of bus)
	  HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_SEL, 0x7);

	  //PXI Go
#if GPIO_PXI
     HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_GPIO_MASK, 0x1F0008);
	  HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_GPIO_CTRL, 0x1);
/*     switch (ch)
	  {
		  case 3:
		    HWIO_OUTF (TLMM_GPIO_CFG13, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG13, FUNC_SEL, 0x4);
		    HWIO_OUTF (TLMM_GPIO_CFG12, FUNC_SEL, 0x4);
			break;
		  case 2:
		    HWIO_OUTF (TLMM_GPIO_CFG11, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG11, FUNC_SEL, 0x6);
		    HWIO_OUTF (TLMM_GPIO_CFG10, FUNC_SEL, 0x6);
			break;
		  case 1:
		    HWIO_OUTF (TLMM_GPIO_CFG9, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG9, FUNC_SEL, 0x6);
		    HWIO_OUTF (TLMM_GPIO_CFG8, FUNC_SEL, 0x8);
			break;
		  case 0:
		  default:
		    HWIO_OUTF (TLMM_GPIO_CFG7, GPIO_OE, 0x1);
		    HWIO_OUTF (TLMM_GPIO_CFG7, FUNC_SEL, 0x7);
		    HWIO_OUTF (TLMM_GPIO_CFG6, FUNC_SEL, 0x3);
			break;
	  }*/
	  pxi_gpio_trigger(ch);
#else
      MC_PXI_Control_Config (ch, 0x1D, 0x0, 0x0);
#endif

      //wait for all tables to go idle
      while ((HWIO_INX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_STATUS) & 0x0000001F) != 0x0000001F);

	  // clear PXI Go
#if GPIO_PXI
     HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_GPIO_CTRL, 0x0);
#else
	  MC_PXI_Control_Config (ch, 0x0, 0x0, 0x0);
#endif
	   
	  //Give control back to MC
	  HWIO_OUTX (REG_OFFSET_MC_PXI, DTTS_CSR_PXI_SEL, 0x0);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      


    for (word_x=0; word_x < 2; word_x++)// 4 => pattern size
    {
        reg_num = word_x + 2;
        fail_in_word |= HWIO_INXI (REG_OFFSET_MC_PXI_SECURE ,DTTS_RAM_PXI_RD_n_ELOG_TABLE_RD_ONLY, reg_num);
    }//for
	
	//=====================================================================================================
  return (fail_in_word);
}
#else
//================================================================================================//
// Memory read with results per bit
// This function will do a memory read and report pass/fail per BIT against the expected pattern
//================================================================================================//
uint32 DDRSS_mem_read_Get_Fail_pat_PXI    ( uint8 ch, uint8 cs)
{
    int		word_x			= 0;
    uint32	fail_in_word 	= 0;
    uint32	read_back_pattern[DQ_TRAINING_PATTERN_SIZE] __attribute__((aligned(8))) =  {0};
    //=====================================================================================================
    ddr_mem_copy ((uint32 *)(size_t)training_address[ch][cs], read_back_pattern, DQ_TRAINING_PATTERN_SIZE, 1);
    for (word_x=0;word_x <DQ_TRAINING_PATTERN_SIZE;word_x++)
    {
        fail_in_word |= write_in_pattern[word_x] ^ read_back_pattern[word_x];
    }//for
	return (fail_in_word);
} 
//=====================================================================================================

#endif

 uint32 DDRSS_fail2hist( uint32 fail_pat ,  uint8 fail_count_histogram_perbit[3][NUM_DQ_PCH][PINS_PER_PHY][HISTOGRAM_SIZE],uint16 knob)
{
  uint8    rf_x;
  uint8    dq_x;
  uint8    bit_x;
  uint32   fail_in_word[2]={ALL_DQ_PASS_PAT,ALL_DQ_PASS_PAT};
  uint32   failed_always =ALL_DQ_FAIL_PAT;
  uint32   failed_once   =ALL_DQ_PASS_PAT;
    fail_in_word[1] =   fail_pat >> 16;
    fail_in_word[0] =   fail_pat &  0x0000FFFF;
    for (rf_x=0;rf_x<2;rf_x++)
    {
        for(dq_x=0;dq_x<NUM_DQ_PCH;dq_x++)
        {
            for(bit_x=0;bit_x<BYTE_SIZE;bit_x++)
            {
                if(  fail_in_word[rf_x]  & 0x1)
                {
                    fail_count_histogram_perbit[rf_x][dq_x][bit_x][knob]++;
                    fail_count_histogram_perbit[   2][dq_x][bit_x][knob]++;
                }
                fail_in_word[rf_x]=fail_in_word[rf_x]>>1;
            }
        }
    }
  //return
  if      (failed_once   == ALL_DQ_PASS_PAT) return ALL_DQ_PASS_PAT;
  else if (failed_always == ALL_DQ_FAIL_PAT) return ALL_DQ_FAIL_PAT;
       else                                  return 0x55555555;
}

void DDRSS_BIST_MR_Write (BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs)
{
//uint32 dq0_ddr_phy_base              = 0;     // DDR PHY DQ0 Base Address
  uint32 ca0_ddr_phy_base              = 0;     // DDR PHY CA0 Base Address
//uint32 reg_offset_ddrcc              = 0;     //DDRCC Base address
  
  // Set DQ0 PHY BASE for the channel and cs
//dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
  ca0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
//reg_offset_ddrcc = REG_OFFSET_DDR_PHY_CH(ch) + DDR_CC_OFFSET ; 
  
      // you can use below opcode incase DRAM clk and CKE are not ON
      // HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_0, 0x0D); //CMD_CK_ON Command to turn on DRAM clock, CKE must be all off
      // HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_1, 0x0B); //CMD_CKE_ALL_ON Command to turn on both CKE1 and CKE0
      // HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_2, 0x0); //OCR_WR_P0_R0_BL0 Open Channel Request: Write to Rank1, Burst Length type 2 (32 for LPDDR4)
       
       //OPCODE setting, reserved bit is used to continue on with previous opdcode setting, incase of unknown opcode setting please use above opcode along with BIST reset
       HWIO_OUTX (ca0_ddr_phy_base ,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, 0x0E0E0E01);  
       
         // Set DRAM to write leveling mode, command encode bits and address for MR register in DRAM, command encode bits for MR write
       HWIO_OUTX (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_0_CFG, 0xC9);  
       HWIO_OUTX (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_1_CFG, 0x80); //MR2 bit7 WRLVL mode entry 
       HWIO_OUTX (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_2_CFG, 0xC3); 
       HWIO_OUTX (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_3_CFG, 0x00); 
       
         //set bist mode to USR DATA ,define delay b/w opcode and data, define delay b/w 2 consecutive opcodes, BIST op loop count
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL, 0xF);
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_DATA_DLY_CNT_LOAD_VAL, 0x4); 
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_LOOP_CNT_LOAD_VAL, 0x1);
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE, 0x1); 
       
         //enable BIST
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x1); 
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0); 
       
         // reset BIST USR data 
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_0_CFG, DATA, 0x00000);
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_1_CFG, DATA, 0x00000);
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_2_CFG, DATA, 0x00000);
       HWIO_OUTXF (ca0_ddr_phy_base,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_3_CFG, DATA, 0x00000);
}

void DDRSS_BIST_DCC (BOOT_DDR_STRUCT *ddr, uint8 ch, uint32 _inst_, uint32 loop_count, uint32 clk_freq_khz)
{
   uint32 inst_offset_lut               = 0;
   uint32 lut                            =0;  

    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, WPRE_OFF, 0x1);
    
    HWIO_OUTX (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, 0x01010101);
    HWIO_OUTX (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, 0x01010101);
    
      //BIST USR data
    for (lut=0;lut<16;lut++) 
    {      
      // Add LUT offset to the base address
      inst_offset_lut = _inst_ + (lut * 0x4);
    
      HWIO_OUTX (inst_offset_lut,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_0_CFG, 0x0);
    } 

    HWIO_OUTXF2 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE, BIST_DATA_DLY_CNT_LOAD_VAL, 0x1,0x0); 
    HWIO_OUTXF3 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL, BIST_LOOP_CNT_LOAD_VAL,BIST_CHK_TIMER_LOAD_VAL, 0x7,loop_count,0x3);    
    //Turn on the BIST clk at all time 
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x1);

    //TODO::we may need freq switch to load SND  
    ddr_external_set_clk_speed(clk_freq_khz); 
    ddr_external_set_clk_speed(clk_freq_khz);   

    
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0);    
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, FSM_BIST_STATE_EN, 0x0);
    //HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_CHK_SW_CLK_ON, 0x1);   
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x1);  
}


void DDRSS_BIST_wrlvl (BOOT_DDR_STRUCT *ddr, uint8 ch, uint32 _inst_, uint32 loop_count, uint32 clk_freq_khz)
{
   uint32 inst_offset_lut               = 0;
   uint32 lut                            =0;  

   // HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, WPRE_OFF, 0x1);
       //TODO:do for all PRFS bands
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);
    
    HWIO_OUTX (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, 0x0);
    HWIO_OUTX (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, 0x0);
    
      //BIST USR data
    for (lut=0;lut<16;lut++) 
    {      
      // Add LUT offset to the base address
      inst_offset_lut = _inst_ + (lut * 0x4);
    
      HWIO_OUTX (inst_offset_lut,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_0_CFG, 0x0);
    } 

    HWIO_OUTXF2 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE, BIST_DATA_DLY_CNT_LOAD_VAL, 0x1,0x4); 
    HWIO_OUTXF3 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL, BIST_LOOP_CNT_LOAD_VAL,BIST_CHK_TIMER_LOAD_VAL, 0xF,loop_count,0x3);    
    
	//??? need to make sure all BIST setup is done before "BIST_TOP_CGC_SND_SW_CLK_ON"=1 since freq. switch is needed after this step
	//Turn on the BIST clk at all time 
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x1);

  //  //TODO::we may need freq switch to load SND  
  //  ddr_external_set_clk_speed(clk_freq_khz); 
  //  ddr_external_set_clk_speed(clk_freq_khz);   
  //
  //  
  //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
  //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0);    
  //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, FSM_BIST_STATE_EN, 0x0);
  //  //HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_CHK_SW_CLK_ON, 0x1);   
  //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x1);   
}


void DDRSS_MC_cnt_clk_en(BOOT_DDR_STRUCT *ddr,DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
    uint8 cs;
    uint8 ch;
    uint8  index            = 0;
    uint8 num_ch           = ddr->shrm_ddr_ptr->misc.num_channel;
    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);    
        // Enable Memory Controller continuous clock 
            // Loop through selected channel(s)
      for (ch = 0; ch < num_ch; ch++) 
      {
      
        if ((channel >> ch) & 0x1) 
        {
          for(cs = 0; cs < NUM_CS; cs++)
          {
           if((chip_select >> cs) & 0x1)
            {
             for (index = 0; index < 4; index++) 
             {        
              HWIO_OUTXFI2(REG_OFFSET_MC_OSU(ch), MPE_APM_OPT_CFG_m_CS_n, index, cs, IDLE_POWER_DOWN_EN, 0);
              HWIO_OUTXFI2(REG_OFFSET_MC_OSU(ch), MPE_APM_OPT_CFG_m_CS_n, index, cs, ACTIVE_POWER_DOWN_EN, 0);  
             }
            }          
          } 
         }
        }
          
         HWIO_OUTXFI (REG_OFFSET_MC_MPE(ch), MPE_APM_OPT_CTRL_m, 0, CLOCK_STOP_EN, 0);
         HWIO_OUTX (REG_OFFSET_MC_MPE(ch), MPE_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(REG_OFFSET_MC_MPE(ch), MPE_LOAD_CONFIG) != 0x0); 
         
         HWIO_OUTXF2 (REG_OFFSET_MC_OSU(ch), OSU_QFI_CLK_CTRL, QFI_CLK_SET, QFI_CLK_OVERRIDE_EN, 1, 1);
         HWIO_OUTX   (REG_OFFSET_MC_OSU(ch), OSU_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(REG_OFFSET_MC_OSU(ch), OSU_LOAD_CONFIG) != 0x0);
}

void DDRSS_MC_cnt_clk_dis(BOOT_DDR_STRUCT *ddr,DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
    uint8 cs;
    uint8 ch;
    uint8  index            = 0;
    uint8 num_ch           = ddr->shrm_ddr_ptr->misc.num_channel;
    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
    
    for (ch = 0; ch < num_ch; ch++) 
    {
    
      if ((channel >> ch) & 0x1) 
      {
        for(cs = 0; cs < NUM_CS; cs++)
        {
         if((chip_select >> cs) & 0x1)
          {
           for (index = 0; index < 4; index++) 
           {        
            HWIO_OUTXFI2(REG_OFFSET_MC_OSU(ch), MPE_APM_OPT_CFG_m_CS_n, index, cs, IDLE_POWER_DOWN_EN, 1);
            HWIO_OUTXFI2(REG_OFFSET_MC_OSU(ch), MPE_APM_OPT_CFG_m_CS_n, index, cs, ACTIVE_POWER_DOWN_EN, 1);  
           }
          }          
        } 
       }
     }
    
    HWIO_OUTXF2 (REG_OFFSET_MC_OSU(ch), OSU_QFI_CLK_CTRL, QFI_CLK_SET, QFI_CLK_OVERRIDE_EN, 0, 0);
    HWIO_OUTX   (REG_OFFSET_MC_OSU(ch), OSU_LOAD_CONFIG, 0x1);
    while (HWIO_INX(REG_OFFSET_MC_OSU(ch), OSU_LOAD_CONFIG) != 0x0);      
    HWIO_OUTXFI (REG_OFFSET_MC_MPE(ch), MPE_APM_OPT_CTRL_m, 0, CLOCK_STOP_EN, 1);
    HWIO_OUTX   (REG_OFFSET_MC_MPE(ch), MPE_LOAD_CONFIG, 0x1); 
    while (HWIO_INX(REG_OFFSET_MC_MPE(ch), MPE_LOAD_CONFIG) != 0x0); 
    
}    
    
boolean  DDRSS_Histogram_Max_Area_Rectangle(training_params_t            *training_params_ptr,
                                            uint8 v_pr[PHASES][MAXVREF][NUM_DQ_PCH][NUM_PR_PARAMS],
                                            uint8 dq_x, 
                                            uint8 sweep_size, 
                                            uint8 *center)
{
    uint8 start                   = 0;
    uint8 stop                    = 0;
    uint8 max_cdc                 = sweep_size;
    uint8 cdc_index               = 0;
    uint8 vref_index              = 0;
    uint8 histogram_sum_array[HISTOGRAM_SIZE] = {0};
    uint32 temp_max_area           = 0 ;
    uint32 max_area_rectangle      = 0;
    uint8 left_cdc                = 0;
    uint8 right_cdc               = 0;
    uint8 top_vref                = 0;
    uint8 bottom_vref             = 0;
    uint8 temp_left_cdc           = 0;
    uint8 temp_right_cdc          = 0;
    uint8 temp_bottom_vref        = 0;


    //for(vref_index = 0; vref_index < max_vref; vref_index++)
    for (vref_index  = training_params_ptr->wr_dqdqs.vref_start_value;  
         vref_index  < training_params_ptr->wr_dqdqs.vref_max_value;  
         vref_index += training_params_ptr->wr_dqdqs.vref_step)
    {

        start = v_pr[2][vref_index][dq_x][STRT];
        stop  = v_pr[2][vref_index][dq_x][STOP];
        //nacpt=((v_pr[2][vref_index][dq_x][STOP]-v_pr[2][vref_index][dq_x][STRT]+1) <vth);
        ddr_printf(DDR_NORMAL,"Vref= %d, start= %d, Stop= %d  \n",vref_index,start, stop  );

        if (stop == -1)
        continue;
        //  FILL THE HISTOGRAM ARRAY
        for(cdc_index = start - 1 ; cdc_index /*>=0*/; cdc_index--)
        {
            if(histogram_sum_array[cdc_index] != 0)
            {
                histogram_sum_array[cdc_index] = 0;
            }
            else
            break;
        }
        for(cdc_index = start ; cdc_index /*<= stop*/; cdc_index++)
        {
            histogram_sum_array[cdc_index]++ ;
        }
        for(cdc_index = stop +1 ; cdc_index < max_cdc; cdc_index++)
        {
            if(histogram_sum_array[cdc_index] != 0)
            {
                histogram_sum_array[cdc_index] = 0;
            }
            else
            break;
        }
        //CALL THE STACK BASED HISTGRAM ANALYZER
        temp_max_area = sub_histogram_max_rect_area(histogram_sum_array,
                                                          &temp_left_cdc,
                                                          &temp_right_cdc,
                                                          &temp_bottom_vref,
                                                          sweep_size
                                                          );
        if (temp_max_area == max_area_rectangle)
        {
            if((temp_right_cdc - temp_left_cdc) >= (right_cdc - left_cdc) )
            {                                                    // copy the coordinates of the current largest rectangle
                max_area_rectangle = temp_max_area;
                bottom_vref = (vref_index+1) - temp_bottom_vref ;
                top_vref    = vref_index;
                left_cdc = temp_left_cdc;
                right_cdc = temp_right_cdc;
            }
        }
        else if (temp_max_area > max_area_rectangle)
        {
                max_area_rectangle = temp_max_area;
                bottom_vref = (vref_index+1) - temp_bottom_vref ;
                top_vref    = vref_index;
                left_cdc = temp_left_cdc;
                right_cdc = temp_right_cdc;
        }

    }
    center[0]=(top_vref +bottom_vref)>>1;
    center[1]=(right_cdc+left_cdc   )>>1;
     ddr_printf (DDR_NORMAL, "Area=%d \ntop=%d \nbottom=%d \nleft=%d \nright=%d \n ",max_area_rectangle,top_vref,bottom_vref,left_cdc,right_cdc);
     ddr_printf (DDR_NORMAL, "V=%d \ncdc=%d \n",center[0],center[1]);

    if(
       ((right_cdc - left_cdc) < 2) ||     //width
       (( top_vref - bottom_vref) < 4) ||  // height
         (max_area_rectangle == 0)
       )
    {
      // ddr_printf(DDR_NORMAL,"FAIL\n");
        return(-1);

    }
    else
    {
       // ddr_printf(DDR_NORMAL,"Max area rectangle = %d \n", max_area_rectangle);
       // ddr_printf(DDR_NORMAL,"Rectangle's left cdc = %d   right cdc = %d\n", left_cdc, right_cdc);
       // ddr_printf(DDR_NORMAL,"Rectangle's bottom vref = %d    top Vref = %d\n", bottom_vref, top_vref);
        return(1);

    }
}



//-----------------------------------------------------------------------------
// This is a stacked based algorithm to find the maximum rectangular area in a histogram
//-----------------------------------------------------------------------------
uint32 sub_histogram_max_rect_area( uint8 *hist_sum_arr_ptr,
                                 uint8 *rectangle_left_cdc,
                                 uint8 *rectangle_right_cdc,
                                 uint8 *rectangle_bottom_vref,
                                 uint8 sweep_size )
{
    int8 popd_s_top     = -1; //popped stack top . -1 => empty stack //NOTE: THIS VARIABLE HAS TO BE A INT
    int8 s_top          = -1; // stack top. -1 => empty stack //NOTE: THIS VARIABLE HAS TO BE AN INT
    uint8 hist_stk[HISTOGRAM_SIZE]   = { 0 }; // stack to hold the sum_array indices
    uint32 area           = 0;
    uint32 max_area       = 0;
    uint8 width          = 0;
    uint8 best_width     = 0;
    uint8 index          = 0;

    for (index = 0; index < sweep_size; index++)
    {
        //Check if empty stack or height of current index in hist_sum_arr is greater or equal than the index pointed by stack
        if ((s_top == -1) || (hist_sum_arr_ptr[index] >= hist_sum_arr_ptr[hist_stk[s_top]]))
        {
            hist_stk[++s_top] = index; // add the index to the top of the stack
        }
        else
        {
              //if height of current index is less than that of index pointed by stack top,
              //pop from the stack until stack is empty or height pointed
              //stack top is less than or equal to that of current index.
            while ((s_top != -1) && (hist_sum_arr_ptr[index] < hist_sum_arr_ptr[hist_stk[s_top]]))
            {
                popd_s_top = s_top; //store the stack top in a temp variable
                s_top--; // pop the stack.

                // AREA CALCULATION:
                // if Stack = empty ,
                //     then area = height of index pointed by temp Top * index
                // if Stack is Not empty
                //     then area = height of index pointed by temp Top * (index - index pointed by current stack top  - 1)

                area = hist_sum_arr_ptr[hist_stk[popd_s_top]] * ((s_top == -1) ?
                                                                  index:
                                                                  (index - hist_stk[s_top] - 1));
                width = ((s_top == -1) ?
                    index :
                    (index - hist_stk[s_top] - 1));
                //if (area > max_area)
                //if ((area > max_area) && (width >= start_width_value) && (hist_sum_arr_ptr[hist_stk[popd_s_top]] >= 4) )
                if ((area > max_area) && (hist_sum_arr_ptr[hist_stk[popd_s_top]] >= 4) )
                {
                    max_area = area;
                    *rectangle_bottom_vref = hist_sum_arr_ptr[hist_stk[popd_s_top]];
                    //*rv = 0;
                    *rectangle_right_cdc = index - 1;
                    *rectangle_left_cdc  = ((s_top == -1) ? 0 : (hist_stk[s_top] + 1));
                    best_width = width;
                }
                //else if ((area == max_area) && (width >= best_width)&& (width >= start_width_value) && (hist_sum_arr_ptr[hist_stk[popd_s_top]] >= 4))
                else if ((area == max_area) && (width >= best_width) && (hist_sum_arr_ptr[hist_stk[popd_s_top]] >= 4))
                {
                    max_area = area;
                    *rectangle_bottom_vref = hist_sum_arr_ptr[hist_stk[popd_s_top]];
                    *rectangle_right_cdc = index - 1;
                    *rectangle_left_cdc  = ((s_top == -1) ? 0 : (hist_stk[s_top] + 1));
                    best_width = width;
                }
            }

            hist_stk[++s_top] = index;
        }
    }
    // After the sum array has been traversed, index = COARSE_CDC_MAX_VALUE
    //The last element in the stack will be the smallest in the array, thus width of  rectangular area with its height = index.
    while (s_top != -1)
    {
        popd_s_top = s_top;
        s_top--;
        area = hist_sum_arr_ptr[hist_stk[popd_s_top]] * ((s_top == -1) ?
                                                          index :
                                                          (index - hist_stk[s_top] - 1));
        width = ((s_top == -1) ?
            index :
            (index - hist_stk[s_top] - 1));
        //if (area > max_area)
        if ((area > max_area) && (hist_sum_arr_ptr[hist_stk[popd_s_top]] >= 4))
        {
            max_area = area;
            *rectangle_bottom_vref = hist_sum_arr_ptr[hist_stk[popd_s_top]];
            //*rv = 0;
            *rectangle_right_cdc   = index- 1;
            *rectangle_left_cdc    = ((s_top == -1) ? 0 : (hist_stk[s_top] + 1));
        }
        else if ((area == max_area) && (width >= best_width)&& (hist_sum_arr_ptr[hist_stk[popd_s_top]] >= 4))
        {
            max_area = area;
            *rectangle_bottom_vref = hist_sum_arr_ptr[hist_stk[popd_s_top]];
            *rectangle_right_cdc = index - 1;
            *rectangle_left_cdc  = ((s_top == -1) ? 0 : (hist_stk[s_top] + 1));
            best_width = width;
        }
    }
    return max_area;
}

#define VREF_WR_LP4_MR14_RANGE_BIT_N 6
#define VREF_WR_LP4_MR14_VALUE_MASK 0x3F
#define VREF_WR_LP4_MAX_VALUE 50
#define DIR_RD 0
#define DIR_WR 1
uint8 vknob2vrange(uint8 vknob)
{
	uint8 vref_range;

	if (vknob > VREF_RANGE_THRESHOLD + VREF_WR_LP4_MAX_VALUE)
	{
		vknob = VREF_RANGE_THRESHOLD + VREF_WR_LP4_MAX_VALUE;
	}
	if (vknob < VREF_RANGE_THRESHOLD)
	{
		vref_range =  vknob;    // Range 0
	}
	else //for range 1
	{
		vref_range = ( 1 << VREF_WR_LP4_MR14_RANGE_BIT_N) | (vknob - VREF_RANGE_THRESHOLD) ;
	}
	return(vref_range);
}

uint8 vrange2vknob(uint8 lp4_mr_vref)
{
	return ( (((lp4_mr_vref &(1<<VREF_WR_LP4_MR14_RANGE_BIT_N)) >> VREF_WR_LP4_MR14_RANGE_BIT_N) * VREF_RANGE_THRESHOLD) + (lp4_mr_vref & VREF_WR_LP4_MR14_VALUE_MASK) );
}

// DDRSS_adjust_cdc_range modifies uint8* cdc_delay_coarse, uint8* cdc_delay_fine
//adjust fine CDC in accordance with DSF expected range RD(0:5), WR(5:10)
int DDRSS_adjust_cdc_range(uint8* const cdc_delay_coarse, uint8* const cdc_delay_fine, uint8 dir_rd_wr)
{
	uint8 fine_range_max;
	int8 ret = 0;

	fine_range_max = (dir_rd_wr == DIR_WR) ? (FINE_STEPS_PER_COARSE -1) + TRAIN_WR_CDC_FINE_STEPS_OFFSET : (FINE_STEPS_PER_COARSE -1);
	if (*cdc_delay_fine > fine_range_max)
	{
		*cdc_delay_fine = fine_range_max;
	} 
	if ( (dir_rd_wr == DIR_WR) && (*cdc_delay_fine < TRAIN_WR_CDC_FINE_STEPS_OFFSET))
	{
		*cdc_delay_fine += FINE_STEPS_PER_COARSE;
		if (*cdc_delay_coarse > 0)
		{
			*cdc_delay_coarse -= 1;
		}
		else
		{
			//TODO:FIXME: this is supposed to be the minimal delay point but breaks on untrained frequencies , since those don't follow the range, 
			//trained position for WR at 547MHz may be at coarse 0 , fine < 5. (plus half cycle delay)
			//ret = -1;
			*cdc_delay_fine -= FINE_STEPS_PER_COARSE;
		}
	}
	return ret;
}
int DDRSS_convert_cdc_ps_to_coarse_fine_steps(const uint32 in_cdc_delay_ps, uint8* const out_cdc_delay_coarse, uint8* const out_cdc_delay_fine, uint8 dir_rd_wr)
{
	uint32 coarse_cdc;
	uint32 fine_cdc;
	int8 ret = 0;

	coarse_cdc = (in_cdc_delay_ps / COARSE_STEP_IN_PS);
	if(coarse_cdc > COARSE_CDC_MAX_VALUE - 1)
	{
		coarse_cdc = COARSE_CDC_MAX_VALUE - 1;
		ret = -1;
	}
	fine_cdc = (in_cdc_delay_ps % COARSE_STEP_IN_PS) / FINE_STEP_IN_PS;
	//may add fine cdc max HW value bound check
	
	*out_cdc_delay_coarse = coarse_cdc;
	*out_cdc_delay_fine = fine_cdc;	
	ret = DDRSS_adjust_cdc_range(out_cdc_delay_coarse, out_cdc_delay_fine, dir_rd_wr);
	
	return ret;
}

uint32 DDRSS_convert_cdc_coarse_fine_steps_to_ps(uint8 cdc_delay_coarse, uint8 cdc_delay_fine)
{
	return (cdc_delay_coarse*COARSE_STEP_IN_PS + cdc_delay_fine*FINE_STEP_IN_PS);
}

//WR Vref assumes only LP4x , add support for LP4 when needed
int DDRSS_convert_vref_mv_to_steps(BOOT_DDR_STRUCT *ddr, uint32 vref_mv, uint32 freq_khz, uint8 dir_rd_wr, uint8* const out_vref_steps)
{
	uint32 vref_steps = 0;
	uint8 vref_max_value = 0;
	int8 ret = 0;
	uint8 mp_hp;
	uint16 vref_rd_offset[] = {VREF_RD_MP_OFFSET_IN_MV, VREF_RD_HP_OFFSET_IN_MV};
	uint16 vref_rd_scale[] = {VREF_RD_MP_STEP_IN_MV, VREF_RD_HP_STEP_IN_MV};
	
	mp_hp = (freq_khz <= HIGHEST_MPVREF_PRFS_FREQ) ? MPVREF : HPVREF;

	if (dir_rd_wr == DIR_RD) 
	{
		if (vref_mv < vref_rd_offset[mp_hp])
		{
				vref_mv = vref_rd_offset[mp_hp];
				ret = -1;
		}
		vref_steps = (vref_mv - vref_rd_offset[mp_hp]) / vref_rd_scale[mp_hp];
		vref_max_value = (freq_khz <= HIGHEST_MPVREF_PRFS_FREQ) ? ddr->training_params.rd_dqdqs.coarse_vref_max_mprx-1 : ddr->training_params.rd_dqdqs.coarse_vref_max_hprx-1;
	} 
	else if (dir_rd_wr == DIR_WR)
	{
		if (vref_mv < VREF_WR_LP4x_OFFSET_IN_MV)
		{
			vref_mv = VREF_WR_LP4x_OFFSET_IN_MV;
			ret = -1;
		}
		vref_steps = (vref_mv - VREF_WR_LP4x_OFFSET_IN_MV) / VREF_WR_LP4x_STEP_IN_MV;
		vref_max_value = VREF_RANGE_THRESHOLD + VREF_WR_LP4_MAX_VALUE;
		//truncate to the training step size
		//vref_steps = vref_steps - (vref_steps % train_params.wr_dqdqs.coarse_vref_step);
	}
	if ( vref_steps > vref_max_value)
	{
		vref_steps = vref_max_value;
		ret = -1;
	}
	*out_vref_steps = (uint8)vref_steps;

	return ret;
}

uint32 DDRSS_convert_vref_steps_to_mv(uint8 steps, uint8 dir_rd_wr, uint32 freq_khz)
{
	uint32 vref_mv = 0;
	
	if (dir_rd_wr == DIR_RD)
	{
		if (freq_khz <= HIGHEST_MPVREF_PRFS_FREQ)
		{
			vref_mv = VREF_RD_MP_OFFSET_IN_MV + steps * VREF_RD_MP_STEP_IN_MV;
		}
		else
		{
			vref_mv = VREF_RD_HP_OFFSET_IN_MV + steps * VREF_RD_HP_STEP_IN_MV;
		}
	}
	else if (dir_rd_wr == DIR_WR)
	{
		vref_mv = VREF_WR_LP4x_OFFSET_IN_MV + steps * VREF_WR_LP4x_STEP_IN_MV;
	}
	
	return vref_mv;
}

//================================================================================================//
// Generate x8 training data from fixed patterns and pseudo-random data seed
// uint64* x8_training_data - pointer to x8 data to create
// uint64* seed_ptr - pointer to seed for pseudo-random data generation
//================================================================================================//
void DDRSS_Generate_x8_Training_Data(uint64* x8_training_data, uint64 seed)
{
    uint8 pattern = 0, index = 0;
    
    // Load fixed patterns
    for(pattern = 0; pattern < NUM_FIXED_PATTERNS; pattern++)
    {
        x8_training_data[index] = fixed_patterns[pattern];
        index++;
    }
    
    // Fill remainder with pseudo-random data
    while(index < (sizeof(write_in_pattern) / sizeof(uint64)))
    {
        x8_training_data[index] = seed;
        seed = (seed >> 1) ^ (-(seed & (uint64)0x00000001) & (uint64)0xf0f0f0f0f0f0f0f0);
        index++;
    }
}

//================================================================================================//
// Populate training data write buffer
// uint8 x8_training_data[NUM_DQ_PCH][DQ_TRAINING_PATTERN_BUFFER_SIZE] - x8 training data per byte
// uint8 bus_width_pch - width of DDR data bus per channel
//================================================================================================//
void DDRSS_Populate_Write_Buffer(uint8 x8_training_data[NUM_DQ_PCH][sizeof(write_in_pattern)], uint8 bus_width_pch)
{
    uint8* pBuffer = (uint8*)&write_in_pattern;
    uint8 byte = 0;
    uint16 beat = 0, index = 0;

    // Load x8 training data into write buffer
    for(beat = 0; beat < (sizeof(write_in_pattern) / (bus_width_pch / 8)); beat++)
    {
        for(byte = 0; byte < (bus_width_pch / 8); byte++)
        {
            pBuffer[index] = x8_training_data[byte][beat];
            index++;
        }
    }
}

//================================================================================================//
// Generate x8 training data and populate write buffer
// uint8 bus_width_pch - width of DDR data bus per channel
//================================================================================================//
void DDRSS_Create_Training_Data(uint8 bus_width_pch)
{
    uint8 byte = 0, pattern = 0;
    uint8 x8_training_data[NUM_DQ_PCH][sizeof(write_in_pattern)] = {{0}};
    
    for(byte = 0; byte < (bus_width_pch / 8); byte++)
    {
        // Insert design patterns
        for(pattern = 0; pattern < NUM_DESIGN_PATTERNS; pattern++)
        {
            fixed_patterns[pattern] = design_patterns[byte][pattern];
        }
        
        DDRSS_Generate_x8_Training_Data((uint64*)&x8_training_data[byte], pseudo_random_data_seeds[byte]);
    }
    
    DDRSS_Populate_Write_Buffer(x8_training_data, bus_width_pch);
}

uint32 DDRSS_fail2bit( uint32 fail_pat ,  uint8 fail_perbit[3][NUM_DQ_PCH][PINS_PER_PHY])
{
  uint8    rf_x;
  uint8    dq_x;
  uint8    bit_x;
  uint32   fail_in_word[2]={ALL_DQ_PASS_PAT,ALL_DQ_PASS_PAT};
  uint32   failed_always =ALL_DQ_FAIL_PAT;
  uint32   failed_once   =ALL_DQ_PASS_PAT;
    fail_in_word[1] =   fail_pat >> 16;
    fail_in_word[0] =   fail_pat &  0x0000FFFF;
    for (rf_x=0;rf_x<2;rf_x++)
    {
        for(dq_x=0;dq_x<NUM_DQ_PCH;dq_x++)
        {
            for(bit_x=0;bit_x<BYTE_SIZE;bit_x++)
            {
                if(  fail_in_word[rf_x]  & 0x1)
                {
                    fail_perbit[rf_x][dq_x][bit_x]++;
                    fail_perbit[   2][dq_x][bit_x]++;
                }
                fail_in_word[rf_x]=fail_in_word[rf_x]>>1;
            }
        }
    }
  //return
  if      (failed_once   == ALL_DQ_PASS_PAT) return ALL_DQ_PASS_PAT;
  else if (failed_always == ALL_DQ_FAIL_PAT) return ALL_DQ_FAIL_PAT;
       else                                  return 0x55555555;
}

//================================================================================================//
//  Function: rd_dqdqs_coarse_fine_plot().
//  Brief description: 
//================================================================================================//
void DDRSS_rd_dqdqs_coarse_fine_plot_pivot (BOOT_DDR_STRUCT *ddr,
                                            training_params_t *training_params_ptr,
                                            uint32 clk_freq_khz,
                                            uint64* output_iter
                                            )
{
  uint8  ch                   = 0;
  uint8  cs                   = 0;
  uint8  byte_lane            = 0;
  uint16  line                = 0;
  uint32 dq0_ddr_phy_base     = 0;
  uint32 ca0_ddr_phy_base     = 0;
  uint8  vref                 = 0;
  uint8  coarse_cdc           = 0;
  uint8  coarse_cdc_max_value = 0;
  uint8  fine_cdc             = 0;
  uint8  mp_hp                = 0;
  uint8  full_cycle_steps     = 0;
  uint8  vref_max_value       = 0;
  uint8  vref_start_value     = 0;
  uint32 period               = 0;
  uint8  pll                  = 0;
 
  uint8 decouple_bit[NUM_DQ_PCH]                       = {0};    
  uint8 ioqual_vref                                    = 0x10;
  uint8 trained_vref[NUM_CH][NUM_CS][NUM_DQ_PCH]       = {{{0}}};
  uint8 trained_coarse_cdc[NUM_CH][NUM_CS][NUM_DQ_PCH] = {{{0}}};
  uint8 trained_fine_cdc  [NUM_CH][NUM_CS][NUM_DQ_PCH] = {{{0}}};
  uint32 fail_pat    [NUM_CH][NUM_CS]                  =  {{0}};

  uint64 starting_offset = *output_iter;
  *output_iter += sizeof(uint16);
  ddi_plot_record cur_record;
  ddi_train_record train_record;
  uint16 ddi_param_count = 0;

  // populate the training addresses for read/write testing
  DDRSS_Get_Training_Address(ddr);
  
  DDR_CHIPSELECT qualified_cs = (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
  
    // Switch to the training frequency
    ddr_external_set_clk_speed (clk_freq_khz);
  
    // Calculate the period for DCD 
    period = (CONVERT_CYC_TO_PS / clk_freq_khz);

    // Determine starting VREF and minimum and maximum limits based on MPVREF or HPVREF
    mp_hp = (clk_freq_khz <= HIGHEST_MPVREF_PRFS_FREQ) ? MPVREF : HPVREF; 

    // Calculate the VREF max value (if not max frequency, use the previously trained VREF)
    vref_max_value = (mp_hp == HPVREF) ? training_params_ptr->rd_dqdqs.coarse_vref_max_hprx : training_params_ptr->rd_dqdqs.coarse_vref_max_mprx;

    // Calculate coarse CDC max value based on period
    full_cycle_steps = ((CONVERT_CYC_TO_PS / clk_freq_khz) / COARSE_STEP_IN_PS);
    coarse_cdc_max_value = (full_cycle_steps > COARSE_CDC_MAX_VALUE) ? COARSE_CDC_MAX_VALUE - 1 : full_cycle_steps - 1;
           
    // Calculate the VREF start values (if not max frequency, use the previously trained VREF)
    vref_start_value = (mp_hp == HPVREF) ? training_params_ptr->rd_dqdqs.coarse_vref_min_hprx : training_params_ptr->rd_dqdqs.coarse_vref_min_mprx;

    // Print CSV file header
    ddr_printf(DDR_NORMAL,"SuperIteration Loop Repeat,Frequency,PLL,MP Vref,RD90 Coarse,RD90 Fine,Read/Write,CH0 CS0 P0,CH0 CS0 P1,CH0 CS1 P0,CH0 CS1 P1,CH1 CS0 P0,CH1 CS0 P1,CH1 CS1 P0,CH1 CS1 P1,CH2 CS0 P0,CH2 CS0 P1,CH2 CS1 P0,CH2 CS1 P1,CH3 CS0 P0,CH3 CS0 P1,CH3 CS1 P0,CH3 CS1 P1\n");
   
    for (ch=0;ch<NUM_CH;ch++)
    {
      ca0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
      dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
      
      for (cs=0;cs<NUM_CS;cs++)
      {
        if ((qualified_cs>> cs) & 0x1)
        {          
            for(byte_lane = 0; byte_lane < NUM_DQ_PCH; byte_lane++) 
            {      
              if (cs == 0)
              {
                trained_coarse_cdc[ch][cs][byte_lane] = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                   DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG,
                                                                   HP_COARSE_DELAY_RANK_0); 
                trained_fine_cdc[ch][cs][byte_lane]   = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                   DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG,
                                                                   HP_FINE_DELAY_RANK_0);                                                                 
              }
              else
              {
                trained_coarse_cdc[ch][cs][byte_lane] = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                   DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG,
                                                                   HP_COARSE_DELAY_RANK_1);
                trained_fine_cdc[ch][cs][byte_lane]   = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                   DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG,
                                                                   HP_FINE_DELAY_RANK_1);                                                                 
              }    
              if (mp_hp == HPVREF)  
              {    
                // Store the global VREF to restore after plotting
                trained_vref[ch][0][byte_lane] = HWIO_INXF (ca0_ddr_phy_base, 
                                                            DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, 
                                                            GLOBAL_HP_VREF_TRIM);
                
                // Set decouple bit (RMW).This sets the IOqualifier to MPVREF. This is required durining training. bit[0] = 0 => MPRX
                decouple_bit[byte_lane] = HWIO_INX ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), 
                                                     DDR_PHY_DDRPHY_PAD_DQS_1_CFG);
                decouple_bit[byte_lane] = (decouple_bit[byte_lane] | (1 << 1));
                HWIO_OUTX ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), 
                          DDR_PHY_DDRPHY_PAD_DQS_1_CFG, 
                          decouple_bit[byte_lane]);
                    
                // Set MPVREF value to IOqualifier to mprx vref reg.  This value is to be set in CA PHY 
                DDR_PHY_hal_cfg_global_vref_mprx(ca0_ddr_phy_base,ioqual_vref);           
              }
              else 
              {
                // Store the global VREF to restore after plotting
                trained_vref[ch][0][byte_lane] = HWIO_INXF (ca0_ddr_phy_base, 
                                                            DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, 
                                                            GLOBAL_MP_VREF_TRIM);
              }
          }
	}
      }
    }
	
    // print trained data to to ddi
    for (ch=0;ch<NUM_CH;ch++)
    {
        for (cs=0;cs<NUM_CS;cs++)
        {
			for (byte_lane = 0;byte_lane < NUM_DQ_PCH;byte_lane++)
            {
				train_record.vref = trained_vref[ch][0][byte_lane];
				train_record.coarse = trained_coarse_cdc[ch][cs][byte_lane];
				train_record.fine = trained_fine_cdc[ch][cs][byte_lane];
				//truncate fine CDC to fit plotted range
				if (train_record.fine >= FINE_STEPS_PER_COARSE)
				{
					train_record.fine = FINE_STEPS_PER_COARSE -1;
				}				
				save_ddi_data2ddr_partition((uint8*)&train_record, sizeof(ddi_train_record), *output_iter);
				*output_iter += sizeof(ddi_train_record);
				ddi_param_count++;
			}
		}
	}

    ddr_printf(DDR_ERROR,"ddi_param_count wr %d",ddi_param_count);
    save_ddi_data2ddr_partition((uint8*)&ddi_param_count, sizeof(uint16), starting_offset);
    starting_offset = *output_iter;
    *output_iter += sizeof(uint16);
    
    // Main VREF/CDC sweep loop     
    for (vref  = vref_start_value;vref < vref_max_value; vref++)  
    {         
      // Set the VREF loop value for all channels
      for (ch=0;ch<NUM_CH;ch++)
      {
        // Set DQ0 base for addressing
        ca0_ddr_phy_base    = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
        
        if (mp_hp == HPVREF)  
        { 
          DDR_PHY_hal_cfg_global_vref_hprx(ca0_ddr_phy_base,vref);           
        }
        else
        {
          DDR_PHY_hal_cfg_global_vref_mprx(ca0_ddr_phy_base,vref);                       
        }
      } 
    
      // Set the Coarse CDC loop value for all channels
      for (coarse_cdc = 0; coarse_cdc <= coarse_cdc_max_value; coarse_cdc++)
      {
        for (ch=0;ch<NUM_CH;ch++)
        {
            for (cs=0;cs<NUM_CS;cs++)
            {
                if ((qualified_cs>> cs) & 0x1)
                {   
                    // Set DQ0 base for addressing
                    dq0_ddr_phy_base    = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
                    
                    for (byte_lane=0;byte_lane<NUM_DQ_PCH;byte_lane++)
                    {
                      DDR_PHY_hal_cfg_cdc_slave_rd((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), 
                                                    coarse_cdc, 
                                                    1, 
                                                    1, 
                                                    cs);
                    }
                }
            }
        }
        

        // Set the Fine CDC loop value for all channels        
        for (fine_cdc = 0;fine_cdc <FINE_STEPS_PER_COARSE; fine_cdc++)
        {
          for (ch=0;ch<NUM_CH;ch++)
          {
            for (cs=0;cs<NUM_CS;cs++)
            {
                if ((qualified_cs>> cs) & 0x1)
                {  
                    // Set DQ0 base for addressing
                    dq0_ddr_phy_base    = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;

                    for (byte_lane=0;byte_lane<NUM_DQ_PCH;byte_lane++)
                    {
                        DDR_PHY_hal_cfg_cdc_slave_rd((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), 
                                                      fine_cdc, 
                                                      0, 
                                                      1, 
                                                      cs);   
                    }
                }
            }
          }
             
          // Execute Read Burst transactions on all channels and ranks
          for (ch=0;ch<NUM_CH;ch++)
          {
            for (cs=0;cs<NUM_CS;cs++)
            {
                if ((qualified_cs>> cs) & 0x1)
                { 
                    fail_pat[ch][cs]	= DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
                }
            } // cs
          } // ch   
          
          // Print the pass fail results for all channels and ranks
          for (ch=0;ch<NUM_CH;ch++)
          {
            if (ch == 0)
            {                
                ddr_printf(DDR_NORMAL,"%d,%d,%d,%d,%d,%d,READ",
                           line,
                           clk_freq_khz,
                           pll,
                           vref,
                           coarse_cdc,
                           fine_cdc
                           );
            }
            for (cs=0;cs<NUM_CS;cs++)
            {  
              ddr_printf(DDR_NORMAL,",%04x,%04x", fail_pat[ch][cs] & 0xFFFF, fail_pat[ch][cs]>>16);
			  cur_record.chcs[ch][cs] = fail_pat[ch][cs];
            }
          }
          cur_record.vref = vref;
          cur_record.coarse = coarse_cdc;
          cur_record.fine = fine_cdc;
          save_ddi_data2ddr_partition((uint8*)&cur_record, sizeof(ddi_plot_record), *output_iter);
          *output_iter += sizeof(ddi_plot_record);
          line++;
          ddr_printf(DDR_NORMAL,"\n");           
	      } // fine_cdc     
      } // coarse_cdc 
    } // VREF
    save_ddi_data2ddr_partition((uint8*)&line, sizeof(uint16), starting_offset);
    
    // Print the trained center
    for (ch=0;ch<NUM_CH;ch++)
    {
      for (cs=0;cs<NUM_CS;cs++)
      {
        for (byte_lane=0;byte_lane<NUM_DQ_PCH;byte_lane++)
        {
            ddr_printf(DDR_NORMAL,"%d,%d,%d,%d,%d,%d,READ",
                     line,
                     clk_freq_khz,
                     pll,
                     trained_vref      [ch][0][byte_lane],
                     trained_coarse_cdc[ch][cs][byte_lane],
                     trained_fine_cdc  [ch][cs][byte_lane]
                    );

            switch (ch)
            {
              case 0 :
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");             
                    }
                }
                break;
              case 1 : 
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    }
                }
                break;
              case 2 : 
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000\n");
                    }
                }
              break;
              case 3 : 
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00\n");
                    }
                }
                break;            
            }
            line++;
        }
      }
    }
    
    // Restore the Global VREF and IO qualifier local VREF
    for (ch=0;ch<NUM_CH;ch++)
    {
      dq0_ddr_phy_base    = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
      ca0_ddr_phy_base    = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
     
      for(byte_lane = 0; byte_lane < NUM_DQ_PCH; byte_lane++) 
      {     
        if(mp_hp == HPVREF)
        {
            // Restore decouple bit.
            decouple_bit[byte_lane] = (decouple_bit[byte_lane] & 0xFFFFFFFD);
            HWIO_OUTX((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), 
                      DDR_PHY_DDRPHY_PAD_DQS_1_CFG, 
                      decouple_bit[byte_lane]);
	  				
	  				// restore mprx vref to mprx vref reg.  
            HWIO_OUTXF (ca0_ddr_phy_base, 
                       DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, 
                       GLOBAL_MP_VREF_TRIM,
                       0x1F);                

	  					
            // Restore the stored global VREF
            HWIO_OUTXF (ca0_ddr_phy_base, 
                        DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, 
                        GLOBAL_HP_VREF_TRIM,
                        trained_vref[ch][0][byte_lane]
                        );
          
            // Restore the IO Qualifier local VREF
            DDR_PHY_hal_cfg_local_vref((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                        0,
                                        10,       // Slice                                  
                                        DQS_VREF  // VREF
                                        );
        }
        else
        {
          // Store the global VREF to restore after plotting
          HWIO_OUTXF (ca0_ddr_phy_base , 
                      DDR_PHY_DDRPHY_PAD_GLOBAL_VREF_TRIM_CFG, 
                      GLOBAL_MP_VREF_TRIM,
                      trained_vref[ch][0][byte_lane]);
 
      }
    }
  }  
  //ddr_printf (DDR_NORMAL,"\nEnd RD Plot for %d MHz\n\n",clk_freq_khz);       
   
}

//================================================================================================//
//  Function: wr_dqdqs_coarse_fine_plot().
//  Brief description: 
//================================================================================================//
void DDRSS_wr_dqdqs_coarse_fine_plot_pivot (BOOT_DDR_STRUCT *ddr, 
                                            training_params_t *training_params_ptr,
                                            uint32 clk_freq_khz,
                                            uint64* output_iter
                                            )
{
  uint8  ch                   = 0;
  uint8  cs                   = 0;
  uint8  byte_lane            = 0;
  uint32 dq0_ddr_phy_base     = 0;
  uint32 period               = 0;  
  uint8  vref                 = 0;
  uint16  line                = 0;
  uint16  fsp_op              = 0;
  uint8  pll                  = 0;
  uint8  coarse_cdc           = 0;
  uint8  coarse_cdc_max_value = 0;
  uint8  fine_cdc             = 0;
  uint8  full_cycle_steps     = 0;
  uint8  vref_max_value       = 0;
  uint8  vref_start_value     = 0;
  uint8 vref_step_value       = 0;
  uint8 trained_coarse_cdc[NUM_CH][NUM_CS][NUM_DQ_PCH] = {{{0}}};
  uint8 trained_fine_cdc  [NUM_CH][NUM_CS][NUM_DQ_PCH] = {{{0}}};
  uint8 trained_vref      [NUM_CH][NUM_CS][NUM_DQ_PCH] = {{{0}}};
  uint32 fail_pat         [NUM_CH][NUM_CS]             =  {{0}};

  uint64 starting_offset = *output_iter;
  *output_iter += sizeof(uint16);
  ddi_plot_record cur_record;
  ddi_train_record train_record;
  uint8 ddi_param_count = 0;

  uint8 MR14_value            = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_0 = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_1 = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_2 = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_3 = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_4 = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_5 = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_6 = 0;
  uint8 PERIODIC_TRAINING_CLK_prfs_7 = 0;
  
  uint8 QFI2EXT_PRFS_0_CNTL_1 = 0;
  uint8 QFI2EXT_PRFS_1_CNTL_1 = 0;
  uint8 QFI2EXT_PRFS_2_CNTL_1 = 0;
  uint8 QFI2EXT_PRFS_3_CNTL_1 = 0;
  uint8 QFI2EXT_PRFS_4_CNTL_1 = 0;
  uint8 QFI2EXT_PRFS_5_CNTL_1 = 0;
  uint8 QFI2EXT_PRFS_6_CNTL_1 = 0;
  uint8 QFI2EXT_PRFS_7_CNTL_1 = 0;

  // populate the training addresses for read/write testing
  DDRSS_Get_Training_Address(ddr);

  training_data *training_data_ptr;
  training_data_ptr = (training_data *)(&ddr->training_results.training_data);  
  DDR_CHIPSELECT qualified_cs = (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
  
  // Calculate the period for DCD 
  period = (CONVERT_CYC_TO_PS / clk_freq_khz);  
  
 
  // Calculate coarse CDC max value based on period
  full_cycle_steps = (CONVERT_CYC_TO_PS / clk_freq_khz) / COARSE_STEP_IN_PS;
  coarse_cdc_max_value = ((full_cycle_steps + 1) > COARSE_CDC_MAX_VALUE) ? COARSE_CDC_MAX_VALUE : (full_cycle_steps + 4);
  
  // Vref range
  vref_start_value  = training_params_ptr->wr_dqdqs.vref_start_value;
  vref_max_value    = training_params_ptr->wr_dqdqs.vref_max_value;
  vref_step_value   = training_params_ptr->wr_dqdqs.vref_step;
  // Print CSV file header
  ddr_printf(DDR_NORMAL,"SuperIteration Loop Repeat,Frequency,PLL,Vref,WR Coarse,WR Fine,Read/Write,CH0 CS0 P0,CH0 CS0 P1,CH0 CS1 P0,CH0 CS1 P1,CH1 CS0 P0,CH1 CS0 P1,CH1 CS1 P0,CH1 CS1 P1,CH2 CS0 P0,CH2 CS0 P1,CH2 CS1 P0,CH2 CS1 P1,CH3 CS0 P0,CH3 CS0 P1,CH3 CS1 P0,CH3 CS1 P1\n");

    dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(0) + DQ0_DDR_PHY_OFFSET;
    
    PERIODIC_TRAINING_CLK_prfs_0 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON);
    PERIODIC_TRAINING_CLK_prfs_1 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_1_CFG, PERIODIC_TRAINING_CLK_ON);
    PERIODIC_TRAINING_CLK_prfs_2 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_2_CFG, PERIODIC_TRAINING_CLK_ON);
    PERIODIC_TRAINING_CLK_prfs_3 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_3_CFG, PERIODIC_TRAINING_CLK_ON);
    PERIODIC_TRAINING_CLK_prfs_4 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG, PERIODIC_TRAINING_CLK_ON);
    PERIODIC_TRAINING_CLK_prfs_5 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG, PERIODIC_TRAINING_CLK_ON);
    PERIODIC_TRAINING_CLK_prfs_6 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG, PERIODIC_TRAINING_CLK_ON);
    PERIODIC_TRAINING_CLK_prfs_7 = HWIO_INXF (dq0_ddr_phy_base , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG, PERIODIC_TRAINING_CLK_ON);
        
    QFI2EXT_PRFS_0_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG);
    QFI2EXT_PRFS_1_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_1_CNTL_1_CFG);
    QFI2EXT_PRFS_2_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_2_CNTL_1_CFG);
    QFI2EXT_PRFS_3_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG);
    QFI2EXT_PRFS_4_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG);
    QFI2EXT_PRFS_5_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG);
    QFI2EXT_PRFS_6_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG);
    QFI2EXT_PRFS_7_CNTL_1 = HWIO_INX (dq0_ddr_phy_base , DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG);
          
    // Read the trained CDC and VREF  
    for (ch=0;ch<NUM_CH;ch++)
    {
        dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
        for (byte_lane = 0;byte_lane < NUM_DQ_PCH;byte_lane++)
        {
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_1_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_2_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_3_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);

            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG,0x41);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_1_CNTL_1_CFG,0x41);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_2_CNTL_1_CFG,0x41);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG,0x41);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG,0x41);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG,0x41);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG,0x41);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG,0x41);                
        }

    }    
      
    // Switch to the training frequency
    ddr_external_set_clk_speed (clk_freq_khz);

    // Read the trained CDC and VREF  
    for (ch=0;ch<NUM_CH;ch++)
    {
        dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
              
        for (cs=0;cs<NUM_CS;cs++)
        {  
            if ((qualified_cs>> cs) & 0x1)
            {
                for (byte_lane = 0;byte_lane < NUM_DQ_PCH;byte_lane++)
                {
                    if (cs == 0)
                    {
                        trained_coarse_cdc[ch][cs][byte_lane] = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                         DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG,
                                                                         HP_COARSE_DELAY_RANK_0); 
                        trained_fine_cdc[ch][cs][byte_lane]   = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                         DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG,
                                                                         HP_FINE_DELAY_RANK_0);                                                                 
                    }
                    else
                    {
                        trained_coarse_cdc[ch][cs][byte_lane] = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                         DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG,
                                                                         HP_COARSE_DELAY_RANK_1);
                        trained_fine_cdc[ch][cs][byte_lane]   = HWIO_INXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), 
                                                                         DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG,
                                                                         HP_FINE_DELAY_RANK_1);                                                                 
                    }          
                    fsp_op = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0 ;
                    trained_vref[ch][cs][byte_lane]  = training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][0][fsp_op];
                }
            }
        } 

        for (byte_lane=0;byte_lane<NUM_DQ_PCH;byte_lane++)
        {    
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_TOP_CTRL_1_CFG,PERIODIC_TRAINING_CLK_ON,0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_IOCTLR_CTRL_CFG,IO_CAL_AUTO,   0x0);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,UPDATE_R0_MSK, 0x1);             
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,UPDATE_R1_MSK, 0x1);             
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,MSTR_PHY,      0x0); 
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,PERIODIC_TRAINING_ACK_SEL,0x1); 
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,STATUS_CGC_ENABLE, 0x1); 
        }
    }

	// print trained data to to ddi
    for (ch=0;ch<NUM_CH;ch++)
    {
        for (cs=0;cs<NUM_CS;cs++)
        {
			for (byte_lane = 0;byte_lane < NUM_DQ_PCH;byte_lane++)
            {
				train_record.vref = ((trained_vref[ch][cs][byte_lane] & 0x40)>>6)*0x1E + (trained_vref[ch][cs][byte_lane] & 0x3F);
				train_record.coarse = trained_coarse_cdc[ch][cs][byte_lane];
				train_record.fine = trained_fine_cdc[ch][cs][byte_lane];
				//truncate fine CDC to fit plotted range
				if (train_record.fine < TRAIN_WR_CDC_FINE_STEPS_OFFSET)
				{
					train_record.fine = TRAIN_WR_CDC_FINE_STEPS_OFFSET;
				}
				else if (train_record.fine >= FINE_STEPS_PER_COARSE + TRAIN_WR_CDC_FINE_STEPS_OFFSET)
				{
					train_record.fine = FINE_STEPS_PER_COARSE + TRAIN_WR_CDC_FINE_STEPS_OFFSET -1;
				}				
				//truncate vref to the step size
				train_record.vref = train_record.vref - (train_record.vref % vref_step_value);
				save_ddi_data2ddr_partition((uint8*)&train_record, sizeof(ddi_train_record), *output_iter);
				*output_iter += sizeof(ddi_train_record);
				ddi_param_count++;
			}
		}
	}
    
	ddr_printf(DDR_ERROR,"ddi_param_count %d",ddi_param_count);
    save_ddi_data2ddr_partition((uint8*)&ddi_param_count, sizeof(uint16), starting_offset);
    starting_offset = *output_iter;
    *output_iter += sizeof(uint16);
      
    // Main VREF/CDC sweep loop     
    for (vref = vref_start_value; vref <= vref_max_value; vref=vref+vref_step_value)  
    {    
        for (ch=0;ch<NUM_CH;ch++)
        {
          // Set DQ0 base for addressing
          dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;

            for (cs=0;cs<NUM_CS;cs++)
            {   
                if ((qualified_cs>> cs) & 0x1)
                {                      
                    MR14_value=vknob2vrange(vref);
                    MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, MR14_value);
                }
            }
        }      
                  
        for (coarse_cdc = 0; coarse_cdc <= coarse_cdc_max_value; coarse_cdc++)
        {
          for (ch=0;ch<NUM_CH;ch++)
          {
            // Set DQ0 base for addressing
            dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
            
            for (cs=0;cs<NUM_CS;cs++)
            {
                if ((qualified_cs>> cs) & 0x1)
                {
                    for (byte_lane =0;byte_lane<NUM_DQ_PCH;byte_lane++)
                    {
                    DDR_PHY_hal_cfg_cdc_slave_wr((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                                  coarse_cdc,
                                                  1, 1, cs); // 1 for coarse_delay_mode. 1 for hp_mode.
                    }
                }
            }
          }
          
          for (fine_cdc = TRAIN_WR_CDC_FINE_STEPS_OFFSET;fine_cdc <FINE_STEPS_PER_COARSE+TRAIN_WR_CDC_FINE_STEPS_OFFSET; fine_cdc++)       
          {
            for (ch=0;ch<NUM_CH;ch++)
            {
              // Set DQ0 base for addressing
              dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
              
                for (cs=0;cs<NUM_CS;cs++)
                {
                    if ((qualified_cs>> cs) & 0x1)
                    {
                        for (byte_lane =0;byte_lane<NUM_DQ_PCH;byte_lane++)
                        {
                          DDR_PHY_hal_cfg_cdc_slave_wr((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                                        fine_cdc,
                                                        0, 1, cs); // 0 for coarse_delay_mode. 1 for hp_mode.
                        }
                    }
                }
            }
              
            // Execute Write/Read Burst transactions
            for (ch=0;ch<NUM_CH;ch++)
            {
                for (cs=0;cs<NUM_CS;cs++)
                {
                    if ((qualified_cs>> cs) & 0x1)
                    {
                        DDRSS_mem_write_PXI(ch, cs);
                        fail_pat[ch][cs]	= DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
                    }
                } // cs
            } // ch 
            
            // Print the pass fail results for all channels and ranks
            for (ch=0;ch<NUM_CH;ch++)
            {
                if (ch == 0)
                {                
                    ddr_printf(DDR_NORMAL,"%d,%d,%d,%d,%d,%d,WRITE",
                             line,
                             clk_freq_khz,
                             pll,
                             vref,
                             coarse_cdc,
                             fine_cdc
                             );
                }  
                for (cs=0;cs<NUM_CS;cs++)
                {  
                    ddr_printf(DDR_NORMAL,",%04x,%04x",
                               fail_pat[ch][cs] & 0xFFFF,
                               fail_pat[ch][cs]>>16                      
                              );

                    cur_record.chcs[ch][cs] = fail_pat[ch][cs];
                }
            } 
            cur_record.vref = vref;
            cur_record.coarse = coarse_cdc;
            cur_record.fine = fine_cdc;
            save_ddi_data2ddr_partition((uint8*)&cur_record, sizeof(ddi_plot_record), *output_iter);
            *output_iter += sizeof(ddi_plot_record);
            line++;
            ddr_printf(DDR_NORMAL,"\n");         
          } // fine_cdc
        } // coarse_cdc      
    } // VREF
    save_ddi_data2ddr_partition((uint8*)&line, sizeof(uint16), starting_offset);

    // Print the trained center
    for (ch=0;ch<NUM_CH;ch++)
    {
        for (cs=0;cs<NUM_CS;cs++)
        {
          for (byte_lane=0;byte_lane<NUM_DQ_PCH;byte_lane++)
          {
            vref = ((trained_vref[ch][cs][byte_lane] & 0x40)>>6)*0x1E + (trained_vref[ch][cs][byte_lane] & 0x3F);
            ddr_printf(DDR_NORMAL,"%d,%d,%d,%d,%d,%d,WRITE",
                       line,
                       clk_freq_khz,
                       pll,
                       vref,
                       trained_coarse_cdc[ch][cs][byte_lane],
                       trained_fine_cdc  [ch][cs][byte_lane]
                      );

            switch (ch)
            {
              case 0 :
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");             
                    }
                }
                break;
              case 1 : 
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000,0000,0000\n");
                    }
                }
                break;
              case 2 : 
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000,0000,0000\n");              
                    }
                }
              break;
              case 3 : 
                if (cs == 0) {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff,0000,0000\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00,0000,0000\n");
                    }
                } else {
                    if (byte_lane == 0) {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,00ff,00ff\n");
                    } else {
                        ddr_printf(DDR_NORMAL,",0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,0000,ff00,ff00\n");            
                    }
                }
                break;            
            }
            line++;
          }
        }
    }

    for (ch=0;ch<NUM_CH;ch++)
    {
        // Set DQ0 base for addressing
        dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
        
        for (byte_lane=0;byte_lane<NUM_DQ_PCH;byte_lane++)
        {   
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_0);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_1_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_1);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_2_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_2);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_3_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_3);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_4);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_5);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_6);
            HWIO_OUTXF (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG, PERIODIC_TRAINING_CLK_ON, PERIODIC_TRAINING_CLK_prfs_7);

            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG,QFI2EXT_PRFS_0_CNTL_1);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_1_CNTL_1_CFG,QFI2EXT_PRFS_1_CNTL_1);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_2_CNTL_1_CFG,QFI2EXT_PRFS_2_CNTL_1);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG,QFI2EXT_PRFS_3_CNTL_1);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG,QFI2EXT_PRFS_4_CNTL_1);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG,QFI2EXT_PRFS_5_CNTL_1);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG,QFI2EXT_PRFS_6_CNTL_1);
            HWIO_OUTX (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET) , DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG,QFI2EXT_PRFS_7_CNTL_1); 
        }
        
        for (cs=0;cs<NUM_CS;cs++)
        {
            if ((qualified_cs>> cs) & 0x1)
            {
                MR14_value= trained_vref[ch][cs][0];
                MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, MR14_value);
            }
        }
    }

}

uint8 DDRSS_get_sweep_size_wr(uint32 clk_freq_khz)
{
    uint8 sweep_size =0;
    sweep_size= (1000000000/  (FINE_STEP_IN_PS*clk_freq_khz))+20 ;
    if (HISTOGRAM_SIZE<sweep_size)
    {
        sweep_size=HISTOGRAM_SIZE;
    }
    return(sweep_size);
}
    
uint8 DDRSS_get_sweep_size_rd(uint32 clk_freq_khz)
{
    uint8 sweep_size =0;
    sweep_size= (1000000000/  (FINE_STEP_IN_PS*clk_freq_khz)) ;
    if (84<sweep_size)
    {
        sweep_size=84;
    }
    return(sweep_size);
}
  
uint8 DDRSS_get_sweep_size(uint32 clk_freq_khz)
{
    uint8 sweep_size =0;
    sweep_size= (1000000000/  (FINE_STEP_IN_PS*clk_freq_khz))+20 ;
    if (HISTOGRAM_SIZE<sweep_size)
    {
        sweep_size=HISTOGRAM_SIZE;
    }
    return(sweep_size);
}   
