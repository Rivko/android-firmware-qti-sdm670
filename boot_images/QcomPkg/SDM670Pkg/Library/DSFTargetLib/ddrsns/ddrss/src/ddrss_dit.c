/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_dit.c#4 $
$DateTime: 2018/04/02 01:54:15 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
-------   ---          --------------------------------------------------------

================================================================================*/
#include "ddrss.h"
#include "mc_common.h"
#include "mc.h"
#include <string.h>

void Training_DDRSS_DIT(BOOT_DDR_STRUCT *ddr, 
                        DDR_CHANNEL channel, 
                        DDR_CHIPSELECT chip_select, 
                        uint32 clk_freq_khz, 
                        uint8 prfs_index, 
                        training_params_t *training_params_ptr,
                        uint8             *local_vars
                        )
{  

    uint8 ch;
    uint8 cs;
    uint8 die                   = 0;
    uint8 num_ch           = ddr->shrm_ddr_ptr->misc.num_channel;
    uint32 dit_count[NUM_CH][NUM_CS][NUM_DIE_PCH] = {{{0}}};
    uint8 device_width				= 0;
  
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);
    channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

    uint8 freq_indx = 0;
    uint8 trac_ndx = 0;
    uint32 acq_freq     = 0;
    uint32 track_freq   = 0;

    // TODO: determine whether averaging is needed
    uint8 dit_ave = 0;  
    trac_ndx    = training_data_ptr->results.dit.dit_trac_ndx;
    acq_freq = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[training_data_ptr->results.dit.dit_acq_ndx].clk_freq_in_khz;
    track_freq = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[trac_ndx].clk_freq_in_khz;
    //================================================================================================//
    // DIT Training
    //================================================================================================//
    if (ddr->shrm_ddr_ptr->ddr_runtime.periodic_training_enable && track_freq >= DSF_MIN_PERIODIC_TRAINING_FREQ && clk_freq_khz >= acq_freq)
    {
        if (clk_freq_khz == track_freq)
        {
        training_data_ptr->results.dit.dit_training_freq = clk_freq_khz;
        DDRSS_DIT_Runtime(ddr, 0, 0, training_params_ptr, dit_count, clk_freq_khz);
            freq_indx = 1;
        }
        for (ch = 0; ch < num_ch; ch++)
        {
            if((channel >> ch) & 0x1)
            {
                for(cs = 0; cs < NUM_CS; cs++)
                {
                    if((chip_select >> cs) & 0x1)
                    {
                        ddr_printf(DDR_NORMAL, "START: DIT training on Ch: %u Rank: %u\n", ch, cs);
                        DDRSS_DIT_Read (ddr, ch, cs, training_params_ptr, dit_ave, dit_count, clk_freq_khz);

                        device_width = ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_io_width[cs];

                        for (die=0; die <= device_width; die++)
                        {
                            training_data_ptr->results.dit.dit_count[ch][freq_indx][cs][die] = dit_count[ch][cs][die];
                        }

                        ddr_printf(DDR_NORMAL, "END: DIT training on Ch: %u Rank: %u\n", ch, cs);
                    }
                }
            }
        }           
    }
}
//-------------------------------------------------------------------------------------------------------------
// DIT Training Routines
//-------------------------------------------------------------------------------------------------------------

void DDRSS_DIT_Read (BOOT_DDR_STRUCT *ddr,
                     uint8  ch,
                     uint8  cs,
                     training_params_t *training_params_ptr,
                     uint8  dit_ave,
                     uint32 (*dit_count)[NUM_CS][NUM_DIE_PCH],
                     uint32 clk_freq_khz
                     )
{
    uint32 dit_die_count[2]     = {0};
    uint32 dit_die_count_acc[2] = {0};
    uint32 dit_die_count_max[2] = {0};
    uint32 dit_die_count_min[2] = {0xffffffff,0xffffffff};
    uint32 MR18_data            = 0;
    uint32 MR19_data            = 0;
    uint32 MR_19_18_data[2]     = {0};
    uint16 dit_runtime_count    = 0;
    uint8 loopcnt               = 0;
    uint8 die                   = 0;
    uint8 max_loopcnt           = 0;
    uint8 device_width          = 0;
    uint8 invalid_count_flag    = 0;
    uint8 dit_watchdog          = 0;
    
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    dit_runtime_count = training_data_ptr->results.dit.dit_runtime_count;


    // Read the max_loopcnt from the training params data structure
    max_loopcnt = training_params_ptr->dit.max_loopcnt;

    // Write the sample count to the results data structure for DTTS
    training_data_ptr->results.dit.dit_loop_count = max_loopcnt;
    //training_data_ptr->results.dit.dit_trac_ndx = 10; // TODO Tao Wang
    device_width = ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_io_width[cs];

    // Reset the loop count
    loopcnt = 0;

    // Read and average multiple DRAM Oscillator counts
    while ((loopcnt < max_loopcnt) && (dit_watchdog < DIT_WATCHDOG))
    {
        // Read the DRAM DQS oscillator counter
        DDRSS_DIT_Capture(ddr, ch, cs, dit_runtime_count, MR_19_18_data);

        MR18_data = MR_19_18_data[0];
        MR19_data = MR_19_18_data[1];

        // Discard illegal count results
        if (device_width == DEVICE_WIDTH_IS_x8)
        {
            invalid_count_flag = (
                // Die 0 both MR18 and MR19 all zeros
                (((MR18_data & 0x000000FF) <= 0x40)   && ((MR19_data & 0x000000FF) == 0x00))   ||

                // Die 1 both MR18 and MR19 all zeros
                ((((MR18_data>>8) & 0x00FF) <= 0x40) && (((MR19_data>>8) & 0x00FF) == 0x00)) ||

                // Die 0 both MR18 and MR19 all ones
                (((MR18_data & 0xFF) == 0xFF)         && ((MR19_data & 0xFF) == 0xFF))         ||

                // Die 1 both MR18 and MR19 all ones
                ((((MR18_data>>8) & 0x00FF) == 0xFF) && (((MR19_data>>8) & 0X00FF) == 0xFF)));
        }
        else
        {
            invalid_count_flag = (
                // Isolate the die counter results
                ((MR18_data & 0x00FF) <= 0x40) && ((MR19_data & 0x00FF) == 0x00))  ||

                (((MR18_data & 0xFF) == 0xFF) && ((MR19_data & 0xFF) == 0xFF));       
        }

        if(!invalid_count_flag)
        {
            // Calculate the minimum and maximum counts
            for (die=0; die <= device_width; die++)
            {
                dit_die_count[die] = ((MR18_data & (0xFF << (8 * die))) >> (8 * die)) | 
                    (((MR19_data & (0xFF << (8 * die))) >> (8 * die)) << 8 );
                // Accumulate the per die counter results
                dit_die_count_acc[die] += dit_die_count[die];

                // Check for the maximum
                if (dit_die_count[die] > dit_die_count_max[die])
                    dit_die_count_max[die] = dit_die_count[die];

                // Check for the minimum
                if (dit_die_count[die] < dit_die_count_min[die])
                    dit_die_count_min[die] = dit_die_count[die];
            }
            loopcnt++;
        }
        dit_watchdog++;
    } // while

    // Process the accumulated count results
    for (die=0; die <= device_width; die++)
    {
        // Remove the minimum and maximum counts
        dit_die_count[die] = dit_die_count_acc[die] - (dit_die_count_max[die] + dit_die_count_min[die]);

        // Average the accumulated counts (after removing the min and max)
        dit_die_count[die] = dit_die_count[die]/(max_loopcnt-2);

        // Average any previous values with the new results
        dit_count[ch][cs][die] = (dit_die_count[die] + dit_count[ch][cs][die])/(dit_ave + 1);
    }
}

void DDRSS_DIT_Slope(BOOT_DDR_STRUCT *ddr,
                     uint8  ch,
                     uint8  cs,
                     uint8  die,
                     uint32 clk_freq_khz,
                     uint16 (*dit_mid_count))
{

    uint16  dit_count                         = 0;
    uint8   range                             = 0;
    uint8   index                             = 0;
    uint8   mid                               = 0;
    uint32  dit_acquisition_time              = 0;
    uint32  dit_dram_clk_period               = 0;

    uint32  dit_ps_per_count                  = 0;
    uint32  dit_delta_count[NUM_DIT_COUNT+1]  = {0};
    uint8 freq_indx = 0;

    // Training data structure pointer
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    // Fetch the boot training frequency
    if (clk_freq_khz == training_data_ptr->results.dit.dit_training_freq)
        freq_indx = 1;

    // Fetch the boot training DIT count result
    dit_count = training_data_ptr->results.dit.dit_count[ch][freq_indx][cs][die];

    // Scale the DIT count to the current clock frequency

    // Calculate the clock period
    dit_dram_clk_period =  CONVERT_CYC_TO_PS/clk_freq_khz;

    // Calculate the acquisition time
    //dit_acquisition_time = (DIT_RUNTIME_FREQ * dit_dram_clk_period);//TOFIX
    dit_acquisition_time = (training_data_ptr->results.dit.dit_runtime_count * dit_dram_clk_period);

    // Populate the center of the array with the scaled DIT count
    dit_delta_count[NUM_DIT_COUNT/2] = dit_count;

    // Calculate the time (ps) per DIT count
    dit_ps_per_count = dit_acquisition_time/dit_count;

    // Calculate the -5*FINE_STEP_IN_PS to -1*FINE_STEP_IN_PS range of the array
    for (range=1;range<=NUM_DIT_COUNT/2;range++)
    {
        // Determine the index for the delta count array
        index = range + NUM_DIT_COUNT/2;

        // Calculate the Delta count when the clock tree changes in units of 10ps (10ps * two ring oscillator half cycles)
        dit_delta_count[index] = dit_acquisition_time / (dit_ps_per_count + (range * FINE_STEP_IN_PS * 2));
    }

    index = (NUM_DIT_COUNT/2)-1;

    // Calculate the 1*FINE_STEP_IN_PS to 5*FINE_STEP_IN_PS range of the array
    for (range=1;range<=NUM_DIT_COUNT/2;range++)
    {
        // Calculate the Delta count when the clock tree changes in units of 10ps (10ps * two ring oscillator half cycles)
        dit_delta_count[index] = dit_acquisition_time / (dit_ps_per_count - (range * FINE_STEP_IN_PS * 2));
        index--;
    }

    // Poplulate the return array with the midpoints between the delta counts
    for (mid=0;mid<=NUM_DIT_COUNT-1;mid++)
    {
        // Calculate the midpoints
        dit_mid_count[mid] = (dit_delta_count[mid] + dit_delta_count[mid+1])/2;
    }
}

void DDRSS_DIT_Capture(BOOT_DDR_STRUCT *ddr,
                       uint8 ch,
                       uint8 cs,
                       uint16 dit_runtime_count,
                       uint32 (*MR_19_18_data))
{
    uint32 MR23_value      = 0;
    uint32 shke_sw_cmd_trigger_status = 0;
    uint32 read_count_wait_time = 0;
    DDR_CHANNEL ch_1hot    = DDR_CH_NONE;
    DDR_CHIPSELECT cs_1hot = DDR_CS_NONE;

    // Convert the channel to one-hot for BIMC functions
    ch_1hot = CH_1HOT(ch);
    cs_1hot = CS_1HOT(cs);

    // Decode the runtime count for the DRAM MR23 register
    if (dit_runtime_count < 2048)
    {
        MR23_value = dit_runtime_count / 16;
    }
    else
    {
        if (dit_runtime_count == 2048)
            MR23_value = 0x40;
        else if (dit_runtime_count == 4096)
            MR23_value = 0x80;
        else if (dit_runtime_count == 8192)
            MR23_value = 0xC0;
    }

    // TODO:Set DQS interval timer runtime setting
    MC_MR_Write (ddr, ch_1hot ,cs_1hot, JEDEC_MR_23, MR23_value);

    MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 0, CMD_MC_SW_STALL, 0, 0);
    MC_Cmd_Stagger_Cfg(ch, 0, 0x0, 0x0, 0x0);

    // Poll for start asserted
    MC_Cmd_Set_Cfg(ch, cs_1hot, 1, CMD_MPC_DQS_OSC_START, 0, 0);
    MC_Cmd_Stagger_Cfg(ch, 1, 0x0, 0x0, 0x0);
    MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 2, CMD_MC_SW_STALL_EXIT, 0, 0);
    MC_Cmd_Stagger_Cfg(ch, 2, 0x0, 0x0, 0x0);
    HWIO_OUTX (REG_OFFSET_MC_SHKE(ch), SHKE_SW_CMD_SEQ, ((0x3 << 28) | 0x7));
    HWIO_OUTX (REG_OFFSET_MC_SHKE(ch), SHKE_SW_CMD_TRIGGER, 0x1); 
    do
    {
        shke_sw_cmd_trigger_status = HWIO_INX (REG_OFFSET_MC_SHKE(ch), SHKE_SW_CMD_TRIGGER);
    } while (shke_sw_cmd_trigger_status != 0x0);
    
    // Wait for the DRAM Oscillator to acquire the count
    read_count_wait_time = (dit_runtime_count*1000/ddr->shrm_ddr_ptr->misc.current_clk_in_kHz) + 1;
    seq_wait(read_count_wait_time,US);

    // Read the DRAM Oscillator counter
    MR_19_18_data[0] = MC_MR_Read (ch_1hot, cs_1hot, JEDEC_MR_18);
    MR_19_18_data[1] = MC_MR_Read (ch_1hot, cs_1hot, JEDEC_MR_19);

}

void DDRSS_DIT_Runtime(BOOT_DDR_STRUCT *ddr,
                     uint8  ch,
                     uint8  cs,
                     training_params_t *training_params_ptr,
                     uint32 (*dit_count)[NUM_CS][NUM_DIE_PCH],
                     uint32 clk_freq_khz
                     )
{
    uint32 dit_die_count[2]     = {0};
    uint32 MR18_data            = 0;
    uint32 MR19_data            = 0;
    uint32 MR_19_18_data[2]     = {0};
    uint32 dit_dram_clk_period  = 0;
    uint16 dit_runtime_table[4] = {1008, 2048, 4096, 8192};
    uint16 dit_runtime_count    = 0;
    uint32 dit_delta_count[2]   = {0};
    uint32 dit_ps_per_count[2]  = {0};
    uint32 dit_acquisition_time = 0;
    uint8 loopcnt               = 0;
    uint8 die                   = 0;
    uint8 device_width			= 0;
    uint8 dit_delta_count_flag  = 1;
    uint8 invalid_count_flag    = 0;
    uint8 dit_watchdog          = 0;

    // Training data structure pointer
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    // Calculate the clock period
    dit_dram_clk_period =  CONVERT_CYC_TO_PS/clk_freq_khz;

    device_width = ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_io_width[cs];

    // Search for the optimal run time count for the first pass of the DIT Read function
    // Normalize the results if needed by increasing the acquisition count
    while (dit_delta_count_flag && loopcnt < 4 && dit_watchdog < DIT_WATCHDOG)
    {
        // Calculate the MR23 acquisition count
        dit_runtime_count    = dit_runtime_table[loopcnt];

        DDRSS_DIT_Capture(ddr, ch, cs, dit_runtime_count, MR_19_18_data);

        MR18_data = MR_19_18_data[0];
        MR19_data = MR_19_18_data[1];

        // Discard illegal count results
        if (device_width == DEVICE_WIDTH_IS_x8)
	  	{
            invalid_count_flag = (
                // Die 0 both MR18 and MR19 all zeros
                (((MR18_data & 0x000000FF) <= 0x40)   && ((MR19_data & 0x000000FF) == 0x00))   ||

                // Die 1 both MR18 and MR19 all zeros
                ((((MR18_data>>8) & 0x00FF) <= 0x40) && (((MR19_data>>8) & 0x00FF) == 0x00)) ||

                // Die 0 both MR18 and MR19 all ones
                (((MR18_data & 0xFF) == 0xFF)         && ((MR19_data & 0xFF) == 0xFF))         ||

                // Die 1 both MR18 and MR19 all ones
                ((((MR18_data>>8) & 0x00FF) == 0xFF) && (((MR19_data>>8) & 0X00FF) == 0xFF)));
        }
        else
        {
            invalid_count_flag = (
                // Isolate the die counter results into 16 bits
                ((MR18_data & 0x00FF) <= 0x40)   && ((MR19_data & 0x00FF) == 0x00))  ||

                (((MR18_data & 0xFF) == 0xFF)         && ((MR19_data & 0xFF) == 0xFF));       
        }

        if(!invalid_count_flag)
        {
            dit_acquisition_time = dit_runtime_count * dit_dram_clk_period;

            // Calculate the dit delta count
            for (die=0; die <= device_width; die++)
            {
                dit_die_count[die] = ((MR18_data & (0xFF << (8 * die))) >> (8 * die)) | 
                    (((MR19_data & (0xFF << (8 * die))) >> (8 * die)) << 8 );
                dit_ps_per_count[die] = dit_acquisition_time / dit_die_count[die];
                dit_delta_count[die]  = dit_acquisition_time / (dit_ps_per_count[die] + (FINE_STEP_IN_PS * 2));
                dit_delta_count[die]  = dit_die_count[die] - dit_delta_count[die];
            }

            // Update the results data structure with the revised runtime count
            if (training_data_ptr->results.dit.dit_runtime_count < dit_runtime_count)
                training_data_ptr->results.dit.dit_runtime_count = dit_runtime_count;

            if (device_width == DEVICE_WIDTH_IS_x8)
            {
                dit_delta_count_flag = (dit_delta_count[0] < DIT_NORMAL_DELTA) && (dit_delta_count[1] < DIT_NORMAL_DELTA)  ;
            }
            else
            {
                dit_delta_count_flag = (dit_delta_count[0] < DIT_NORMAL_DELTA) ;
            }
            loopcnt++;
        }
        dit_watchdog++;
    }
}
