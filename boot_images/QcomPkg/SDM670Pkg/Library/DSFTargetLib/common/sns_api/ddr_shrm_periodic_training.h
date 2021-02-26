/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/

#ifndef DDR_SHRM_PERIODIC_TRAINING_H
#define DDR_SHRM_PERIODIC_TRAINING_H

#include "ddr_phy_technology.h"
#include "ddr_phy_seq_hwioreg.h"


#define NUM_CH                    2 // Number of DDR channels
#define NUM_CS                    2 // Number of ranks (chip selects)
#define NUM_DQ_PCH                2
#define NUM_DIE_PCH               2 // Number of DRAM Die per Channel
#define NUM_SLABS          (FINE_STEPS_PER_COARSE * 2) //12
#define NUM_DIT_FREQS       2

#define DIT_ACQUIRE_MODE   0x1
#define DIT_TRACKING_MODE  0x2

#define DIT_INTERRUPT_MASK  0xF
#define DIT_INTERRUPT_SHIFT 26

#define DIT_CNT_1SEC                         32000
#define DIT_MRRD_CNT_1_MSEC                  3200
#define DSF_PERIODIC_TRAINING_ALGO_DEBUG_EN 0


typedef struct
{
     uint16 dit_count[NUM_CH][NUM_DIT_FREQS][NUM_CS][NUM_DIE_PCH]; // 8996: 2 * (2 * 2 * 2) = 16.
     uint32 dit_training_freq;                      // 8996: 4. 
     uint16 dit_runtime_count;                      // 8996: 2. 
     uint8  dit_fine_step_limit;                    // 8996: 1. 
     uint16 dit_interval_per_sec;                   // 8996: 2. 
     uint8  dit_loop_count;                         // 8996: 1. 
     uint16  dit_acq_ndx;                           // 8996: 2. 
     uint16  dit_trac_ndx;                          // 8996: 2. 
     uint16  dit_trac_prfs_ndx;                     // Napali 2.

} SHRM_BOOT_DIT_STRUCT; // 8996: Total = 30.


typedef struct
{
    SHRM_BOOT_DIT_STRUCT boot_dit;
    uint32 osc_slab [NUM_CH][NUM_DIT_FREQS][NUM_CS][NUM_DIE_PCH][NUM_SLABS];  //(4*2*2*1*12) *4 = 768
    uint8 boot_cdc_fine [NUM_CH][NUM_CS][NUM_DQ_PCH];                                     //(4*2*2) = 16
    uint8 prev_cdc_fine_step [NUM_CH][NUM_CS][NUM_DIE_PCH];                   //(4*2*1) *1 = 8
#if DSF_PERIODIC_TRAINING_ALGO_DEBUG_EN

    uint8 ch_to_be_serviced;
    uint8 acq_trac_mode;
    uint8 shrm_execution_flag;
    uint32 dit_die_count_acc[NUM_CH][NUM_CS][NUM_DIE_PCH];
	uint32 dit_die_count_min[NUM_CH][NUM_CS][NUM_DIE_PCH];
	uint32 dit_die_count_max[NUM_CH][NUM_CS][NUM_DIE_PCH];
    uint32 osc_avg[NUM_CS][NUM_DIE_PCH];
#endif
} SHRM_PERIODIC_TRAINING_DATA_STRUCT;

extern SHRM_PERIODIC_TRAINING_DATA_STRUCT *shrm_periodic_training_data_ptr;

#endif
