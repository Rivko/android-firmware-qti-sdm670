#ifndef DDR_MGMT_H
#define DDR_MGMT_H
/*===========================================================================
                             ddr_mgmt.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdio.h>
#include "DALStdDef.h"
#include "msmhwio.h"
#include "clock.h"
#include "rpmh_arc.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define AGGREGATED_SIGNAL 0
#define SEQUENCE_SIGNAL   1


#define BCM_MC_CLOCK_DOMAIN     0
#define BCM_SHUB_CLOCK_DOMAIN   1

// BCM_ALC/ACV_CLOCK_DOMAIN defines are in the ddr_mgmt_target.h file

#define CX_RM RPMH_ARC_VDD_CX
#define MX_RM RPMH_ARC_VDD_MX
#define VDDA_RM RPMH_ARC_VDD_EBI
#define DDRSS_POWERSTATE_RM RPMH_ARC_DDR_SS

#define RSI_GPR 0
#define RSI_GPW 1
#define RSI_REQ 2

#define RSI_FF    0
#define RSI_RESP  1

#define RSI_NOTLAST  0
#define RSI_LAST     1

#define RSI_OPCODE_CLOCK_PREWORK     0
#define RSI_OPCODE_CLOCK_WORK        1
#define RSI_OPCODE_CLOCK_POSTWORK    2
#define RSI_OPCODE_VOLTSLEW_PREWORK  3
#define RSI_OPCODE_VOLTSLEW_WORK     4
#define RSI_OPCODE_VOLTSLEW_POSTWORK 5
#define RSI_OPCODE_POWERSTATE        6
#define RSI_OPCODE_CACHE_STAT_QUERY  7
#define RSI_OPCODE_CACHEFLUSH        8
#define RSI_OPCODE_CACHEFLUSH_ABORT  9
#define RSI_OPCODE_FAL_ACV           10
#define RSI_OPCODE_FAL_ALC           11

#define RSI_OPCODE_PCB_COLLAPSE      12
     
#define RSI_CLOCK_SHUB   0
#define RSI_CLOCK_MC     1

#define RSI_RESOURCE_DDRSS        0
#define RSI_RESOURCE_SYSTEM_CACHE 1
#define RSI_RESOURCE_SHUB         0
#define RSI_RESOURCE_MC           1

#define RSI_VOLTAGERAIL_MX   0
#define RSI_VOLTAGERAIL_CX   1
#define RSI_VOLTAGERAIL_VDDA 2

#define SHRM_ACTIVE_LINES 0x2
#define SHRM_DIRTY_LINES  0x3

#define READ_TCS          0  // TCS 0 for READ
#define FLUSH_TCS         0  // TCS 0 for FLUSH
#define FLUSH_ABORT_TCS   1  // TCS 1 for FLUSH ABORT
#define FAL_TCS           2  // TCS 2 for FAL commands
#define VOLTAGE_CMD_START 3  // TCS 3-5 For Voltage
#define SHUB_CMD_START    6  // TCS 6-8 for SHUB
#define COLLAPSE_PS_CMD   9  // TCS 9 for COLLAPSE Powerstate
#define MC_CMD_START      10 // TCS 10-12 for MC
#define RESTORE_PS_CMD    13 // TCS 13 for RESTORE Powerstate

#define XO_ON         (3)
#define XO_BUFFER_OFF (1)

#define DDR_AUX_SEQUENCE_VOLTAGE_SHIFT (SEQ_BRANCH_MX_Changing)

#define FREQUENCY_SEQUENCER 0
#define VOLTAGE_SEQUENCER   1

#define FSM_EVENT_NONE 0x80000000

#define ARC_RM_DEST_OL_SHIFT 28
#define ARC_RM_SEQU_OL_SHIFT 24
#define ARC_RM_AGGR_OL_SHIFT 20
#define ARC_RM_SOLV_OL_SHIFT 16
#define ARC_RM_CURR_OL_SHIFT 12

#define DDRA_GCC_MC_SHIFT    8 // HWIO_SEQ_GCC_CLKPLAN_MC_CP_SHFT   HW_IO has this as 0
#define DDRA_GCC_SHUB_SHIFT  4 // HWIO_SEQ_GCC_CLKPLAN_SHUB_CP_SHFT
#define DDRA_GCC_SHRM_SHIFT  0 // HWIO_SEQ_GCC_CLKPLAN_SHRM_CP_SHFT HW_IO ahs this as 8

#define NUM_ENTRIES(x) (sizeof(x)/sizeof(x[0]))

#define FREQ_PLAN_COPIED          0x01
#define FREQ_PLAN_TEST_DISABLED   0x02

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
// The following should be updated when new sequences are introduced to ddr_aux.rinit.
enum frequency_start_addresses {     
  START_ADDR_RESTORE = 0x152,
  START_ADDR_COLLAPSE = 0x13C,
  START_ADDR_SHUB_ONLY_CHANGE = 0xBC,
  START_ADDR_MC_ONLY_CHANGE = 0xC4,
  START_ADDR_SHUB_MC_COMBINATION = 0xF0,
  START_ADDR_DCC_MEMTEST = 0xCC
};

enum voltage_sequence_names {
 ERROR_SEQUENCE = 0x02,
 VOLTAGE_SWITCH = 0x16E,
 VDDA_ONLY      = 0x1A6,
 GCC_INIT       = 0x1DC
};

typedef enum {
  CLIENT_DDR_FREQUENCY,
  CLIENT_DEPENDENCY,
  CLIENT_TEMP_EVENT,
  NUM_CLIENTS
} AOP_INTERNAL_CLIENT;

typedef enum {
  DDR_PENDING     = 1,
  VOLTAGE_PENDING = 2
} TC_PENDING;

typedef enum
{
  TC_IDLE,
  TC_FREQ_CHANGING,
  TC_VOLTAGE_CHANGING
} TC_STATE;

typedef enum {
  RESOURCE_MX,  // This order matches the ID from RSI commands.
  RESOURCE_CX,  // It also matches the order of the bits for combo sequence in DDR_AUX.
  RESOURCE_VDDA,
  NUM_VOLTAGE_RESOURCES,
  RESOURCE_PMIC = NUM_VOLTAGE_RESOURCES,
  NUM_NPA_RESOURCES,
  RESOURCE_DDR = NUM_NPA_RESOURCES,
  RESOURCE_XO
} VOLTAGE_RESOURCES;

enum arc_pending {
  ARC_PENDING_NONE,
  ARC_PENDING_SEQUENCE,
  ARC_PENDING_DEFERRED_SEQUENCE,
};

typedef enum arc_work_t
{
  PREWORK,
  POSTWORK,
  WHATEVER
} arc_work_t;

struct resource_states {
  unsigned int current;  // Current OP for the resource. 
  unsigned int sequence_level;  // Current OP for the resource. 
  unsigned int in_sequence_level;  // Next sequence level
  unsigned int mol;
  enum arc_pending pending; // NONE - ARC Not Pending
                            // SEQUENCE - ARC at Sequence Prework
                            // DEFERRED_SEQUENCE - ARC at Sequence prework, but we are not ready to process it
  boolean sequencing;       // True if we told the ARC to start its sequencing.
  bool votes_pending;
  uint32_t vote_req_id;
  void (*voltage_vote_callback)(uint32_t);
};

enum DDR_OLS {
  Dx_PLL_Off_MC_Collapse_PHY_Collapse_SHUB_Collapse_LLC_Collapse,
  D3_PLL_Off_MC_Collapse_PHY_Collapse_SHUB_Collapse,
  //D2_PLL_Off_MC_Collapse_PHY_Collapse,
  //D1_PLL_Off_MC_Collapse,
  D0p5_PLL_Off,
  D0_On,
};

enum fal_state {
  FAL_IDLE,
  FAL_PENDING,
  FAL_SEQUENCING
};

struct ddr_state {
  boolean ddr_rm_pending;     // True if DDR_RM sequence is waiting on AOP.
  boolean bcm_mc_pending;     // True if BCM sequence is waiting on AOP for MC.
  boolean bcm_shub_pending;   // True if BCM sequence is waiting on AOP for SHUB.
  enum fal_state bcm_alc_pending;    // True if BCM sequence is waiting on AOP for ALC.
  enum fal_state bcm_acv_pending;    // True if BCM sequence is waiting on AOP for ACV.
  boolean ddr_rm_vote_on;     // Our vote to the DDR_RM ARC
  boolean fal_in_progress;    // Our vote to the DDR_RM ARC
  
  unsigned int ddr_dest_mc_lvl;     // What CP are we going to for MC
  unsigned int ddr_dest_shub_lvl;   // What CP are we going to for SHUB
  unsigned int ddr_next_mc_lvl;     // Intermediate frequency for MC
  unsigned int ddr_next_shub_lvl;   // Intermediate frequency for SHUB
  boolean         ddr_mc_lvl_double;   // Set to true if we have to redo MC transition

  unsigned int current_mc_cp;       // The current CP of MC
  unsigned int current_shub_cp;     // The current CP of SHUB

  unsigned int ddr_arc_level;       // Current RM  Desired OL of DDRM RM ARC.
  unsigned int ddr_bcm_mc_level;    // Current BCM Desired Clock Plan for MC
  unsigned int ddr_bcm_shub_level;  // Current BCM Desired Clock Plan for SHUB
  unsigned int ddr_bcm_acv_level;   // Current BCM Desired Clock Plan for ACV
  unsigned int ddr_bcm_alc_level;   // Current BCM Desired Clock Plan for ALC

  unsigned int ddr_reg_mc_level;    // The Desired CP last read from BCM
  unsigned int ddr_reg_shub_level;  // The Desired CP last read from BCM

  boolean         ddr_flush_complete;  // Indicates the flush is complete.
  unsigned int ddr_pcb_mask;        // What we will flush
  unsigned int ddr_lb_activations;  // What is active in LLC.

  unsigned int ddr_xo_vote;         // last vote sent for XO 
  boolean         ddr_xo_on;        // Have we seen a completion for XO?

  boolean         ddr_dcc_vote;     // last vote sent for path to dcc to DDR
  boolean         ddr_dcc_on;       // Have we seen a completion for that path?
  boolean         dcc_on_request;   // Request_ID of the last DCC Path ON vote.
  boolean         memtest_needed;   // True if memtest is required.

  unsigned int ddr_rm_vote;         // last vote sent for DDR Resource Mgr
  unsigned int ddr_cx_vote;         // last vote sent for CX for Frequency
  unsigned int ddr_mx_vote;         // last vote sent for MX for Frequency
  unsigned int ddr_vdda_vote;       // last vote sent for VDDA for Frequency
  unsigned int ddr_pmic_mode_vote;  // last vote sent for pmic mode
  unsigned int ddr_shrm_level;      // The SHRM frequency level

  boolean         ddr_shrm_on;      // True if SHRM is on.

  boolean         tcs_0_pending;    // True if we have started TCS0.
  boolean         tcs_1_pending;    // True if we have started TCS1.
  boolean         tcs_2_pending;    // True if we have started TCS2.
  unsigned int ddr_mc_floor;        
};

enum SHRM_LEVELS {
  SHRM_LEVEL_OFF,
  SHRM_LEVEL_MIN,
  SHRM_LEVEL_LOW,
  SHRM_LEVEL_HIGH
};

typedef enum {
  ARC_IDLE,
  ARC_AGGREGATED,
  ARC_SEQUENCING
} ARC_STATES;

struct arc_actor {
  unsigned int vote;
  unsigned int current_value;
  unsigned int sequence_value;
  ARC_STATES state;
};

enum PCBCompletionType
{
  FireForget,
  CompletionRequired
};

typedef struct
{
  uint8 clk_idx;        // BCM's clock plan corresponding to this frequency.
  uint32 freq_mhz;      // The frequency in mhz
  uint32 clk_period;    // The period of that frequency (1000000/freq_mhz)
  boolean freq_enable;  // Is the frequency enabled
  clk_mode mode;        // Clock source - informational, not used by AOP.
  uint32 vddcx;         // The required CX/MX Operating Point.
  uint32 vddmx;         // The required CX/MX Operating Point.
  uint32 vdda;          // The required VDDA Operating Point
  uint32 ddr_automode_cfg; // The PMIC mode to request from the PMIC team.
  uint8 max_up_idx;     // The maximum clock plan index I can go to on way up.
  uint8 min_down_idx;   // The minimum clock plan index I can go to on way down.
  boolean double_freq_change; // Force a fake freq switch here
  uint8 alc_override;   // When we are overriding ALC values, what should it be.
  uint8 copied;       // Was the SMEM entry valid.  if so, it was copied.
} LOCAL_FREQ_STATE;

/**
 * DDR configure settings data from XBL
 */
typedef struct
{
  uint8 nNumMCStates;
  uint8 nNumSHUBStates;
  LOCAL_FREQ_STATE *pMCCfg;
  LOCAL_FREQ_STATE *pSHUBCfg;
} LOCAL_DDRCfgType;

enum voltage_seq_branch {
  SEQ_BRANCH_SHUB_Changing,           //  0     0x0001
  SEQ_BRANCH_MC_Changing,             //  1     0x0002
  SEQ_BRANCH_SHRM_Changing,           //  2     0x0004
  SEQ_BRANCH_RSVD_3,                  //  3     0x0008

  SEQ_BRANCH_MX_Changing,             //  4     0x0010
  SEQ_BRANCH_CX_Changing,             //  5     0x0020
  SEQ_BRANCH_VDDA_Changing,           //  6     0x0040
  SEQ_BRANCH_RSVD_7,                  //  7     0x0080

  SEQ_BRANCH_SHRM_Operational,        //  8     0x0100
  SEQ_BRANCH_SHRM_Freq_Down,          //  9     0x0200
  SEQ_BRANCH_SHRM_Freq_Up,            // 10     0x0400
  SEQ_BRANCH_RSVD_11,                 // 11     0x0800

  SEQ_BRANCH_Perform_Memory_Test,     // 12     0x1000
  SEQ_BRANCH_RSVD_13,                 // 13     0x2000
  SEQ_BRANCH_TURN_SHRM_ON,            // 14     0x4000
  SEQ_BRANCH_TURN_SHRM_OFF            // 15     0x8000
};


typedef enum {
  SEQUENCER_IDLE,
  CHANGE_VOLTAGE,
  CHANGE_FREQUENCY,
  NUM_SEQUENCER_FSM_STATES
} SEQUENCER_FSM_STATES;

typedef enum {
  FREQUENCY_IDLE,
  FREQUENCY_PROCESS_REQUEST,
  FREQUENCY_WAIT_FOR_VOLTAGE,
  FREQUENCY_WAIT_FOR_FREQUENCY,
  FREQUENCY_READING_FLUSH_DATA,
  FREQUENCY_WAIT_FLUSH_COMPLETE,
  FREQUENCY_WAIT_FOR_DCC,
  FREQUENCY_WAIT_FOR_MEMTEST,
  NUM_FREQUENCY_FSM_STATES
} FREQUENCY_FSM_STATES;

typedef enum {
  FSM_VOLTAGE_CHANGE_READY        = 1,
  FSM_PERFORM_FREQUENCY_CHANGE    = 2,
  FSM_VOLTAGE_SEQUENCE_COMPLETE   = 4,
  FSM_FREQUENCY_SEQUENCE_COMPLETE = 8
} SEQUENCER_FSM_EVENTS;

typedef enum {
  FREQ_FSM_FREQUENCY_CHANGE_REQUEST    = 1,
  FREQ_FSM_VOLTAGE_REQUIREMENTS_MET    = 2,
  FREQ_FSM_DCC_COMPLETE                = 4,
  FREQ_FSM_FREQUENCY_SEQUENCE_COMPLETE = 8,
  FREQ_FSM_AMC_COMPLETE                = 16,
  FREQ_FSM_PROCESS_COMPLETE            = 32,
} FREQUENCY_FSM_EVENTS;

enum CSR_TYPE {
  READ,
  WRITE,
  READWRITE
};

enum CSR_NAMES {
  RMm_AGGR_BLOCK_EN,
  RMm_AGGR_UNBLOCK,
  RMm_OPERATIONAL_LEVEL,

  DDR_AUX_AOP_INTR_STAT,
  DDR_AUX_AOP_INTR_DETAIL,
  DDR_AUX_SEQ_BRANCH_EVENT,
  DDR_AUX_SEQ_GCC_CLKPLAN,
  DDR_AUX_SEQn_CFG,
  DDR_AUX_SEQ_VCD_SEL,
  DDR_AUX_RSI_TCSm_CFG,
  DDR_AUX_RSI_TCSm_STAT,
  DDR_AUX_RSI_TCSm_CMDt,
  DDR_AUX_RSI_TCSm_AMC_MODE,
  DDR_AUX_RSI_TCS0_RESP_DATAm,
};

typedef struct csr {
  enum CSR_NAMES enum_name;
  char *csr_name;
  enum CSR_TYPE type;
  unsigned long base_addr;
  unsigned long m_mult;
  unsigned long n_mult;
  unsigned long value[16][16];

} CSR;


//---------------------------------------------------------------------------
// Function Declarations
//---------------------------------------------------------------------------

uint32 arc_send_vote(VOLTAGE_RESOURCES arc, AOP_INTERNAL_CLIENT client, unsigned int level,
               enum PCBCompletionType completion);
unsigned long encode_tcs_cmd(unsigned int response_req,
                             unsigned int last, unsigned int opcode,
                             unsigned int id, unsigned int data);
void service_fal_complete();

#endif // DDR_MGMT_H
