#ifndef MSG_RAM_LAYOUT_H
#define MSG_RAM_LAYOUT_H
/*===========================================================================
                             msg_ram_layout.h

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
#include "msmhwiobase.h"
#include "rpmh_common.h"
#include "comdef.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define MSG_RAM_DRV_SIZE       0x400  // 1K
#define MSG_RAM_INFO_0_SIZE  sizeof(aop_info_0_t)
#define MSG_RAM_SMEM_TABLE_SIZE  sizeof(aop_smem_table_t)
#define MSG_RAM_GAP_SIZE       0xFC00  // 63K
#define MSG_RAM_SECTION_SIZE   (MSG_RAM_DRV_SIZE + MSG_RAM_GAP_SIZE)
#define MSG_RAM_SECTION(i)     (void *)(AOP_SS_MSG_RAM_START_ADDRESS_BASE + (i * MSG_RAM_SECTION_SIZE))
#define MSG_RAM_CMD_DB_DATA_SIZE 2  // Size in uint32 units.
#define SMEM_TABLE_MAX_SIZE     0xA
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

typedef struct
{
  void *start;   
  uint32 size; 
} msg_ram_sec_t;

/* ============AOP INFO0 Section============ */

typedef struct
{
  uint32 stat_type;	  //type of sleep stat (ascii little endian)
  uint32 count;           //# times this mode has been entered 
  uint64 last_entered;    //last timestamp we entered this mode
  uint64 last_exited;     //last timestamp we exited this mode
  uint64 total_duration;  //total amount of time we've spent in this mode
} sleep_stats_t;

typedef struct
{
  uint32 boot_cookie_offset;   //boot cookie address
  uint32 sleep_stats_offset;   //sleep stats address
  uint32 wdog_cookie_offset;   //cti cookie offset
  uint32 cmd_db_data_offset[MSG_RAM_CMD_DB_DATA_SIZE];
  uint32 ddr_stats_dictionary_offset; //offset for the ddr_stats_dict_t structure
  uint32 reserved_addrs[10];
} dict_t;


#define FREQ_SWITCH_COUNT 50

enum event {
   FREQ_CHANGE, // 0 Starting the frequency change
   FREQ_FLUSH,  // 1 Starting a flush
   FREQ_DONE,   // 2 Done
};

typedef struct freq_switch {
  uint64 timestamp;

  uint8  event;                 // 0 Frequency start, 1 Flush Start, 2 Done
  uint8  powerstate_request;    // What is the Powerstate ARC state
  uint8  bcm_acv;               // Current BCM value for ACV
  uint8  bcm_alc;               // Current BCM value for ALC

  uint8  bcm_mc_request;        // What is BCM requesting for MC
  uint8  bcm_shub_request;      // What is BCM requesting for SHUB
  uint8  current_mc_cp;         // The current MC Clock Plan
  uint8  current_shub_cp;       // The current SHUB Clock Plan
} FREQ_SWITCH_TABLE;

typedef struct ddr_stats {

  uint32_t mc_count;
  uint32_t shub_count;
  uint32_t restore_count;
  uint32_t collapse_count;

  uint64_t mc_cp_history;             uint64_t shub_cp_history;   // Saves last 16 changes
  uint64_t mc_begin;                  uint64_t shub_begin;
  uint64_t previous_mc_begin;         uint64_t previous_shub_begin;
  uint64_t powerstate_collapse_begin; uint64_t powerstate_restore_begin;
  uint64_t mc_max;                    uint64_t mc_max_time;
  uint64_t shub_max;                  uint64_t shub_max_time;
  uint64_t powerstate_collapse_max;   uint64_t powerstate_collapse_max_time;
  uint64_t powerstate_restore_max;    uint64_t powerstate_restore_max_time;
  
  uint32_t flush_count; uint32_t pad; uint64_t flush_begin;
  uint64_t flush_max;                 uint64_t flush_max_time;

} DDR_STATS;

#define DDRMGR_FLAG_NEW             0xCA11C0DE
#define DDRMGR_FLAG_DONE            0x600D0000

#define DDRMGR_OPSCALE_SHUB_FLAG    0x00000001
#define DDRMGR_OVERRIDE_ALC_FLAG    0x00000002
#define DDRMGR_OPSCALE_BCM_FLAG     0x80000000

#define DDRMGR_TEST_BCM             0x00000001
#define DDRMGR_TEST_DDR             0x00000002
#define DDRMGR_TEST_CX_MX           0x00000004
#define DDRMGR_TEST_VRM             0x00000008
#define DDRMGR_TEST_ZERO_TEMP       0x00000010

#define DDRMGR_TEST_CLEAR_VOTES     0x20000000
#define DDRMGR_TEST_MIN_VOTES       0x40000000
#define DDRMGR_RUN_DDRMGR_TEST_CODE 0x80000000

typedef struct ddr_mgr_config_t
{
  uint32 ddr_mgr_test_info;         // Causes DDR_MGR to run internal tests (if enabled in build)
  uint32 ddr_mgr_test_count;        // Number of test interations run       (if enabled in build)
  uint32 ddr_mgr_operational_info;  // Enable/Disable SHUB op scaling as well as 
  uint32 ddr_config_status;         // Indicates when a new command is set/applied
  uint64_t alc_override_values;     // ALC Override values for the 16 CPs
  uint32_t ddraux_command_timeout;  // Timeout value for sequences in DDR_AUX.
  uint32_t log_stop_time;           // Stop local logging if processing is longer than this.
} DDR_MGR_CONFIG;

typedef struct
{
  uint32 ddr_event_offset;       // Offset for the first DDR_EVENT;
  uint32 ddr_event_count;        // Number of possible DDR events.
  uint32 ddr_stats_offset;       // sleep stats address
  uint32 ddr_mgr_config_offset;  // Offset of the DDR config structure.
} ddr_stats_dict_t;

typedef struct ddr_mgr_info_t {
  ddr_stats_dict_t  dictionary;     // Dictionary of the below entries
  FREQ_SWITCH_TABLE freq_switch_events[FREQ_SWITCH_COUNT]; // Tracks the last FREQ_SWITCH_COUNT(50) events
  DDR_MGR_CONFIG    ddr_mgr_config; // The DDR_MGR config offset.
  DDR_STATS         ddr_stats;      // Saves overall statistics on DDR.
} DDR_MGR_INFO_T;

typedef struct
{
  dict_t dictionary;            //dictionary for subsystems to lookup stats addresses
  uint32 boot_cookie;           //boot cookie indicating AOP is up and initialized
  uint32 reserved0;
  sleep_stats_t sleep_stats[2]; //sleep stats tracking sleep mode details
  uint32 wdog_cookie;
  uint32 reserved1[221];        //decrease anytime something is added to the structure
} aop_info_0_t;

/* ============SMEM TABLE Section============ */

typedef struct
{
  uint32 smem_id;
  uint32 addr;
} smem_addr_t;


typedef enum
{
  SMEM_TABLE_SUCCESS,
  SMEM_TABLE_NOT_INITIALIZED,
  SMEM_TABLE_ID_NOT_FOUND,
} smem_table_err;

typedef struct
{
  uint32      initialized;	           //filled in by XBL once the table is initialized
  uint32      num_addrs;                   //size of the smem lookup table
  smem_addr_t table[SMEM_TABLE_MAX_SIZE];  //table of smem addresses
  uint32      reserved0[234];              //decrease anytime something is added to the structure
} aop_smem_table_t;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

//msg ram layout
//#define AOP_SS_MSG_RAM_SIZE_BASE                                    0x00100000
//#define AOP_SS_MSG_RAM_START_ADDRESS_BASE                           0x01300000
//#define AOP_SS_MSG_RAM_END_ADDRESS_BASE                             0x013fffff

/* ============Access Functions ============ */

/**
 * <!-- msg_ram_get_smem_addr -->
 *
 * @brief Retrieve an smem address from message ram. These addresses are populated by XBL. 
 *
 * @param smem_id : The smem ID correpsonding to the address we are retrieving
 * @param smem_addr : A pointer to a uint32 that will be filled in the the smem address value
 *
 * @return SMEM_TABLE_SUCCESS if we found the address, smem_err code otherwise
 */ 
smem_table_err msg_ram_get_smem_addr(uint32 smem_id, uint32 *smem_addr);

/**
 * <!-- msg_ram_get_wdog_cookie -->
 *
 * @brief Retrieve the wdog cookie from msg ram 
 *
 * @return the wdog cookie value
 */ 
uint32 msg_ram_get_wdog_cookie();

/**
 * <!-- msg_ram_get_wdog_cookie -->
 *
 * @brief Set the wdog cookie in msg ram 
 *
 * @param the dog cookie value to set
 */ 
void msg_ram_set_wdog_cookie(uint32 cookie_val);

/**
 * <!-- msg_ram_cmd_db_set_init -->
 *
 * @brief Initialize the dictionary entry for cmd_db
 */
void msg_ram_cmd_db_set_init(uint32 data[MSG_RAM_CMD_DB_DATA_SIZE]);

/**
 * <!-- msg_ram_get_sleep_stats_area -->
 *
 * @brief Return the pointer to a msg ram memory area where sleep stats are populated
 *
 */ 
void *msg_ram_get_sleep_stats_area (void);

/**
 * <!-- msg_ram_get_frequency_stats_area -->
 *
 * @brief Return the pointer to a msg ram memory area where frequency stats are populated
 *
 */ 
void *msg_ram_get_frequency_stats_area (void);

/**
 * <!-- msg_ram_ddrstats_set_init -->
 *
 * @brief Initialize the dictionary entry for ddr_statistics
 */
void msg_ram_ddrstats_set_init(void *ddr_stats_dictionary);

/**
 * <!-- msg_ram_get_aop_area -->
 *
 * @brief Return the pointer to a msg ram memory area where the AOP can put debug
 *
 */ 
void *msg_ram_get_aop_area (void);
/**
 * <!-- msg_ram_get_frequency_stats_area -->
 *
 * @brief Return the pointer to a msg ram memory area where frequency stats are populated
 *
 */ 
void *msg_ram_get_aop_logging_area (void);

/**
 * <!-- msg_ram_set_boot_cookie -->
 *
 * @brief Set the boot cookie to indicate AOP is initialized 
 */ 
void msg_ram_set_boot_cookie(void);

/**
 * <!-- msg_ram_set_init -->
 *
 * @brief Initialize the dictionary and sleep stats types 
 */ 
void msg_ram_init(void);

#endif // MSG_RAM_LAYOUT_H
