/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/common/ddrss/header/ddrss_training.h#5 $
$DateTime: 2018/12/17 04:30:22 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
05/08/14   jeasley      Update DDRSS_wrlvl prototype to use zero based channel and cs.
05/04/14   arindamm     First edit history header. Add new entries at top.
08/06/17   vijaysi     Number of channels for Warlock updated. NUM_CH=2
================================================================================*/

#ifndef __DDRSS_TRAINING_H__
#define __DDRSS_TRAINING_H__

#include "ddr_phy_technology.h"
#include "ddr_shrm_periodic_training.h"
#define VREF_STEPS                80
#define HISTOGRAM_SIZE            (COARSE_CDC_MAX_VALUE * FINE_STEPS_PER_COARSE)
#define MAXCDC                    (COARSE_CDC_MAX_VALUE * FINE_STEPS_PER_COARSE)
#define MAXVREF            		  80
#define COARSE_L                   0
#define FINE_L                     1
#define DELAY_L                    2
#define CDC_L                      3

#define  STRT 0
#define  STOP 1
#define  MIDP 2
#define  EYEW 3
#define  ACPT 4
#define  NUM_PR_PARAMS 5
//#define MAX_BUS_WIDTH 32
#define BYTE_SIZE     8
#define ALL_DQ_FAIL_PAT      0xFFFFFFFF
#define ALL_DQ_PASS_PAT      0x00000000
#define DQ_TRAINING_PATTERN_SIZE 128
#define NUM_SCALE_BANDS           2
#define SCALE_BAND(scale)         ((scale==0)? (2) \
                                             : (1))
                                              
#define SCALE_FREQ(freq)          ((freq==0) ? (F_RANGE_2) \
                                             : (F_RANGE_1))

#define HIGHEST_TRAINED_FPRS_FREQ F_RANGE_7

#define ODT_FPRS_THRESHOLD_FREQ   F_RANGE_4
#define HIGHEST_MPRX_FPRS_FREQ    F_RANGE_5
#define HIGHEST_MPVREF_PRFS_FREQ  F_RANGE_3
#define HIGHEST_GCC_PLL_FREQ      F_RANGE_1

#define RCW_THRESHOLD_FREQ        F_RANGE_4
#define LOWEST_PRFS_FREQ          F_RANGE_0//lowest PRFS threshold

#define HIGHEST_TRAINING_TABLE_INDX      7//4 
#define RCW_TRAINING_TABLE_START_INDX    4//1 

#define LOWEST_TRAINED_FPRS_FREQ  F_RANGE_3
#define HIGHEST_RANK_EN_FREQ      F_RANGE_4 

#define LOWEST_SCALED_FPRS_FREQ   F_RANGE_2
#define LOWEST_FPRS_FREQ          F_RANGE_0 

#define LOWEST_DCC_COPY_PRFS_DRRCC   2
#define LOWEST_WRLVL_COPY_PRFS       2
#define WRLVL_PASS1_FREQ             1017600
#define WRLVL_PASS1_PRFS_INDEX       4
 	     
#define NUM_DQ_TRAINING_STACK        120
#define NUM_CA_TRAINING_STACK        100

#define MRW_BIST_EN 1
#define MRW_MC_EN 0


typedef enum
{
    TRAINING_TYPE_CA_VREF   = 0,
    TRAINING_TYPE_WR_DQDQS  = 1,
    TRAINING_TYPE_RD_DQDQS  = 2,
    TRAINING_TYPE_MAX       = 0x7f
} TRAINING_TYPE;

typedef enum 
{
  DQ = (uint8) 2,
  CA = (uint8) 1
} PHY_BUS;
typedef enum
 {
 	RCW_DIS,
	RCW_ENA  
 }RCW_EN_DIS;
 
#define NUM_CA_BITS                 6
#define NUM_CA_PHY_BIT             10
#define NUM_DQ_PHY_BIT             10
#define BYTE_SIZE                   8
#define READ_PERBIT_INIT            0xF
#define PHYBIT_NC                   0xFF

#define NUM_CH                    2 // Number of DDR channels updated for Warlock
#define NUM_CS                    2 // Number of ranks (chip selects)
#define NUM_DQ_PCH                2 // Number of DQ PHYs
#define NUM_CA_PCH                1 // Number of CA PHYs Per Channel
#define NUM_PLL                   2 // Number of PLLs in DDRCC
#define NUM_DIE_PCH               2 // Number of DRAM Die per Channel
#define NUM_FSP                   2 // Number of FSP

#define DQ_PER_BYTE               8 // Number of bits per BYTE

#define NUM_PRFS_BANDS            8
#define TRAINING_THRESHOLD        533000 

#define TRAINING_START_PRFS       3
#define FSP_THRESHOLD_PRFS_BAND   3
#define MAX_TRAINING_FREQ_NUM     8
#define MAX_TRAINING_FREQ_INDEX   7

#define DTTS_ACQUIRE_MODE        0x1
#define DTTS_TRACKING_MODE       0x2

#define DTTS_WAKEUP_DISABLE_ALL        0x70000
#define DTTS_WAKEUP_ENABLE_ALL         0x00000
#define DTTS_WAKEUP_ENABLE_SHKE_ONLY   0x60000

#define SHKE_TRIGGER_ENABLE         0x1
#define SHKE_TRIGGER_DISABLE        0x2
#define DTTS_FW_IDLE                0

#define RX_DCC                      10
#define PHASE                       2
#define PHASES                      3
#define CLK_DCC_OFFSET_RANGE        7     //2*3(range) + 1

#define ODT_FSP_EN_FREQ_THRESHOLD 1000000 //Match with settings in bimc_data.c
#define LOWEST_PRFS_FREQ          F_RANGE_0//lowest PRFS threshold
#define HIGHEST_PRFS_FREQ         F_RANGE_7 //Highest PRFS threshold

#define FEATURE_DISABLE            0
#define FEATURE_ENABLE             1
#define DCC_TRAINING_WRLVL_WR90_IO 7
#define DCC_TRAINING_WRLVL_WR90    3
#define DCC_TRAINING_WR90          2
#define DCC_TRAINING_WRLVL         1
#define DCC_TRAINING_WR90_IO       6

#define FSP_OP_BOOT               0x0
#define FSP_OP_HIGH               0x1

#define MPVREF                    0
#define HPVREF                    1

#define MR23_data                 0x3F

#define IO_QUAL_OFFSET_DOUBLE_STEP_SIZE 0x5
#define LOCAL_VREF_MID_VALUE            0x08
#define LOCAL_VREF_MAX_VALUE            0x0F

#define LP_MODE                   0
#define HP_MODE                   1
#define SWEEP_LEFT                0
#define SWEEP_RIGHT               1

#define DCC_TRAINING_SEL          7         // Selecting all three DCC training flows
#define MR12_VREF_RANGE_BIT       0         // MR12 OP6 -> 0: Vref Range 0; 1: MR12 Vref Range 1
#define VREF_RANGE_THRESHOLD      30

// PHY Power Collapse Broadcast for dual channel devices
#define PC_PHY_CA_DQ_BROADCAST    0x1FBF
#define PC_CC_BROADCAST           0x2040

#define PINS_PER_PHY                    10
#define PINS_PER_PHY_CONNECTED_NO_DBI    8
#define PINS_PER_PHY_CONNECTED_WITH_DBI  9
#define NUM_FULL                  3
#define NUM_HALF                  2

//RCW related
#define   RCW_COARSE_DELAY        1
#define   RCW_FINE_DELAY          0


// Defines for Periodic Training
#define LOG2(i) (( (i)>=8u)? ( (i)>=32u)?5 :  ( (i)>=16u)?4 : 3 :  ( (i)>=4u)?2 :  ( (i)>=2u)?1 : 0 )
#define PT_SLAB_THRESHOLD_PS      10
#define NUM_DIT_COUNT 12 // NOTE: NUM_DIT_COUN must be an even number
#define DIT_WATCHDOG  16

// Defines for boot training status (shrm_ddr_ptr->misc.boot_training_status)
#define TRAINING_NOT_STARTED    (0)
#define TRAINING_IN_PROGRESS    (1)
#define TRAINING_COMPLETED      (2)
#define CA_TRAINING_IN_PROGRESS (3)


extern uint32 freq_range[NUM_PRFS_BANDS]; //for 1353MHz expansion. 
extern uint8 connected_bit_mapping_no_DBI_A  [PINS_PER_PHY_CONNECTED_NO_DBI];
extern uint8 connected_bit_mapping_no_DBI_B  [PINS_PER_PHY_CONNECTED_NO_DBI];
extern uint8 connected_bit_mapping_with_DBI_A [PINS_PER_PHY_CONNECTED_WITH_DBI];
extern uint8 connected_bit_mapping_with_DBI_B [PINS_PER_PHY_CONNECTED_WITH_DBI];
//extern uint8 connected_bit_mapping_CA [PINS_PER_PHY_CONNECTED_CA];


/***************************************************************************************************
 Training Params Struct
 ***************************************************************************************************/
typedef struct
{
   struct
   {
      uint8 max_loopcnt;
   } dcc;

   struct
   {
      uint8 max_loopcnt;
   } dit;

   struct
   {
      uint8 max_loopcnt;
      uint8 cdc_training_enable;
      uint8 vref_start_value;
      uint8 vref_max_value;
      uint8 vref_step;
      uint8 coarse_cdc_start_value;
      uint8 coarse_cdc_max_value;
      uint8 coarse_cdc_step;
      uint8 fine_cdc_max_value;
      uint8 fine_perbit_start_value;
   } ca_vref;

   struct
   {
      uint8 max_loopcnt;
      uint8 max_coarse_cdc;
      uint8 max_fine_cdc;
      uint8 coarse_cdc_step;
      uint8 fine_cdc_step;
      uint8 feedback_percent;
   } wrlvl;

   struct
   {
       uint8  max_loopcnt;
       uint8  max_coarse_cdc;
       uint8  max_fine_cdc;
	   uint8  max_num_cycle;
       uint8  coarse_cdc_step;
       uint8  fine_cdc_step;
   } rcw;

   struct
   {
      uint8 max_loopcnt;
      uint8 coarse_vref_start_mprx;
      uint8 coarse_vref_min_mprx;
      uint8 coarse_vref_max_mprx;
      uint8 coarse_vref_start_hprx;
      uint8 coarse_vref_min_hprx;
      uint8 coarse_vref_max_hprx;
      uint8 vref_start_value;
      uint8 vref_max_value;
      uint8 vref_step;
      uint8 coarse_cdc_start_value;
      uint8 coarse_cdc_max_value;
      uint8 fine_cdc_start_value;
      uint8 fine_cdc_max_value;
      uint8 cdc_step_2D;
      uint8 min_eye_width_mask_top_enable;
      uint8 min_eye_width_mask_top;
      uint8 min_eye_width_mask_bottom_enable;
      uint8 min_eye_width_mask_bottom;
      uint8 min_eye_height_enable;
      uint8 min_eye_height;
      uint8 min_eye_width_enable;
      uint8 min_eye_width;
      uint8 max_dq_dcc_enable;
      uint8 max_dq_dcc;
   } rd_dqdqs;

   struct
   {
      uint8 max_loopcnt;
      uint8 vref_start_value;
      uint8 vref_max_value;
      uint8 vref_step;
      uint8 coarse_cdc_start_value;
      uint8 coarse_cdc_max_value;
      uint8 fine_cdc_start_value;
      uint8 fine_cdc_max_value;
      uint8 dbi_offset;
      uint8 cdc_step_2D;
      uint8 min_eye_width_mask_top_enable;
      uint8 min_eye_width_mask_top;
      uint8 min_eye_width_mask_bottom_enable;
      uint8 min_eye_width_mask_bottom;
      uint8 min_eye_height_enable;
      uint8 min_eye_height;
      uint8 min_eye_width_enable;
      uint8 min_eye_width;
      uint8 max_dqs_io_dcc_enable;
      uint8 max_dqs_io_dcc;
   } wr_dqdqs;

   uint8 small_eye_abort;
} training_params_t;


/***************************************************************************************************
 Training data struct
 This is the organization of ddr->flash_params.training_data struct
 ***************************************************************************************************/
struct training_results
{
   struct
   {
     uint16 dit_count[NUM_CH][NUM_DIT_FREQS][NUM_CS][NUM_DIE_PCH]; // Napali: 2 * (4 * 2 * 1) = 16.
     uint32 dit_training_freq;                      // Napali: 4.
     uint16 dit_runtime_count;                      // Napali: 2. 
     uint8  dit_fine_step_limit;                    // Napali: 1. 
     uint16 dit_interval_per_sec;                   // Napali: 2. 
     uint8  dit_loop_count;                         // Napali: 1. 
     uint16  dit_acq_ndx;                           // Napali: 2. 
     uint16  dit_trac_ndx;                          // Napali: 2. 
     uint16  dit_trac_prfs_ndx;                     // Napali: 2.
     uint32 tdqs2dq_in_ps[NUM_CH][NUM_CS][NUM_DQ_PCH];// Napali: 4 * 2 * 2* 4 = 64
   //  uint8  wr_half_cycle_start [MAX_TRAINING_FREQ_NUM][NUM_CH][NUM_CS][NUM_DQ_PCH]; // Napali: (5 *4 * 2 * 2)  bytes = 80 bytes
   //  uint8  wr_coarse_cdc_start [MAX_TRAINING_FREQ_NUM][NUM_CH][NUM_CS][NUM_DQ_PCH]; // Napali: (5 *4 * 2 * 2)  bytes = 80 bytes
   } dit; // Napali: Total = 96.
   
   struct
   {
     uint32 rd_opcode_offset[MAX_NUM_CLOCK_PLAN][NUM_CS];
   } rcw;
   
   struct
   {
     uint8 wr_dram_vref[NUM_CH][NUM_CS][NUM_DIE_PCH][NUM_FSP];
   } wr_dram;
   
   struct
   {
     uint32 ca_training_data_stack[NUM_CH][NUM_CA_PCH][NUM_CA_TRAINING_STACK]; // Napali : 4 * 4 * 1 * 100 = 1600
     uint32 dq_training_data_stack[NUM_CH][NUM_DQ_PCH][NUM_DQ_TRAINING_STACK]; // Napali : 4 * 4 * 2 * 100 = 3200
   } training_stack; // Napali 4800 
   
}; // Napali: Sum Total = old: 4751 new: 4896

typedef struct
{
   struct training_results results; // Napali : Total = 4992 uint8s.
} training_data;

void DDRSS_set_training_params (training_params_t *training_params_ptr);

/***************************************************************************************************
 CA Vref training
 ***************************************************************************************************/

extern uint8 ca_training_pattern_lpddr3[CA_PATTERN_NUM][6];
extern uint8 ca_training_pattern_lpddr4[CA_PATTERN_NUM][3];

typedef struct {
  uint8 vref_fail_count_table [NUM_CA_PCH][80];  // 80B
} ddrss_ca_vref_local_vars; //Napali total size: 80B.


/***************************************************************************************************
 Write Leveling
 ***************************************************************************************************/
// Structure for wrlvl convert routine
typedef struct {
  uint32 coarse_dqs_delay;
  uint32 fine_dqs_delay;
  uint32 dqs_half_cycle;
} wrlvl_params_struct;

/**************************************************************************************************
RCW training local Vars
****************************************************************************************************/
//This Structure contains the local variables for RCW training used by Training_DDRSS_RCW() function and its callees.

typedef struct
{
	uint8 rcw_num_cycles[NUM_CH][NUM_CS][NUM_DQ_PCH] ; // 4 * 2 * 2 = 16 bytes
	uint8 rcw_coarse_cdc[NUM_CH][NUM_CS][NUM_DQ_PCH];   // 4 * 2 * 2 = 16 bytes
	uint8 rcw_fine_cdc[NUM_CH][NUM_CS][NUM_DQ_PCH];     // 4 * 2 * 2 = 16 bytes
}ddrss_rcw_local_vars; // 48 bytes


/***************************************************************************************************
 Write and Read training
 ***************************************************************************************************/
// Structure containing what would otherwise be local variables used by the ddrss_rd_dqdqs(), ddrss_wr_dqdqs() functions and its callees.
typedef struct {
    struct 
    {
      uint8 coarse_dq_passband_info[NUM_DQ_PCH][COARSE_VREF][COARSE_CDC];        // 2 * 57 * 25    = 2850 bytes
      uint8 coarse_dq_full_half_cdc[NUM_DQ_PCH][NUM_FULL][NUM_HALF][COARSE_CDC]; // 2 * 3 * 2 * 25 =  300 bytes
    } coarse_schmoo;
    
    uint32 fail_hist_2d [MAXVREF][MAXCDC];                                      // 4 * 80 * 24 * 6     = 46080 bytes
    uint8            dq_dcc_adj   [NUM_DQ_PCH][NUM_CS];                         // 2 * 2               =     4 bytes
    uint8            dqsdcc_adj   [NUM_DQ_PCH][NUM_CS];                         // 2 * 2               =     4 bytes
    uint8  failcount_histogram [3][NUM_DQ_PCH][PINS_PER_PHY][HISTOGRAM_SIZE];   // 3 * 2 * 10 * 24 * 6 =  8640 bytes
    uint8             pass_reg [3][NUM_DQ_PCH][PINS_PER_PHY][4];                // 3 * 2 * 10 * 4      =   240 bytes
    uint8        vref_pass_reg [3][NUM_DQ_PCH][NUM_CS][PINS_PER_PHY][4];        // 3 * 2 *  2 * 10 * 4 =   480 bytes
    uint32             cdc_ref                              [HISTOGRAM_SIZE][4];// 4 * 24 * 6 * 4      =  2304 bytes
    uint8           dq_half_cycle    [NUM_DQ_PCH];                              // 2                   =     2 bytes
    uint8           dq_full_cycle    [NUM_DQ_PCH];                              // 2                   =     2 bytes

} ddrss_rdwr_dqdqs_local_vars; // 60906 bytes

typedef union  {
	uint32 w;						//32 bit whole
	uint8 b [PHASE][NUM_DQ_PCH];   //8bit for each dq and each phase
	uint16 p[PHASE];				//16bit for each phase
} FAILCELL;

typedef struct {
	uint8 dbi_flag;  // Coarse vref of SMALLER vref CA
	uint8 dq_pin_num_max;    // Difference value of two CA vref
	uint8 *connected_bit_mapping_A;
} dbi_struct;

#endif /* __DDRSS_TRAINING_H__ */
