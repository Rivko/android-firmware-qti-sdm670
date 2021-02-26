/*=============================================================================

                               Boot Sahara
                               Source File
GENERAL DESCRIPTION
  This file provides the implementation to enter the Sahara Protocol.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright 2010-2014,2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/28/17   rohik   Added support for raw partitions dump collection  
07/05/17   aus     Fixed validate address range parameters
03/03/17   ds	   remove clobber prot
06/03/15   as      Support 64bit debug only.
03/27/15   kedara  Updates to ensure compiler does not optimize out sahara_log[]
02/07/15   kedara  remove null pointer checks for obsolete get_bin_image_id, get_bin_image_dest
07/24/14   ck      Added check to ensure hash table size is at least size of mi_boot_image_header_type
07/23/14   ck      Removed unreachable/unneeded code in boot_sahara_receive_image_elf
07/09/14   tj      Validate the number of program header entries to ensureit fits the
                   allocated buffer
06/20/14   kedara  Support 64 bit compilation using llvm.
03/10/14   kedara  Update logic to receive data in optimzed image loading.
03/07/14   kedara  Changes to not update rmb length when elf data is skipped.
02/03/14   kedara  Enable support for optimized elf image load.
11/24/13   plc     Fix casting issues with Aarch64
11/12/13   kedara  Skip loading of image if MBN header is incorrect.
                   explicit hash-verify for ehdr+phdr after hash-table auth.
                   Changes to do segment hash checks only if ehdr+phdr hash check 
                   was successful 
10/18/13   dhaval  explicit hash-verify for ehdr+phdr after hash-table auth.
                   Changes to do segment hash cheks only if ehdr+phdr hash check 
				   was successful 
10/16/13   jz      Added support for >4GB DDR dump, fixed 64bit memory debug support
10/15/13   kedara  Support PICe Flashless boot.
09/13/13   aus     Remove infinite loop in reset to obtain full SBL stack on errors.
09/03/13   aus     Update the sahara packet buffer to be global.
07/30/13   kedara  Update segment length in RMB reg after hashing for SSA boot.
07/01/13   dh      LAPE phase 1: Add boot_sahara_handle_memory_64bits_read 
                   to handle 64bits memory dump. For now we truncate 
                   64 bits address to 32 bits.
06/10/13   kedara  Zero Initialize sahara_elf_image_info before every image download.
05/31/13   kedara  remove address range check for local hash segment buffer.
04/15/13   kedara  Support MBA image authentication. Enable authentication even
                   when sahara_ssa_enabled flag is set to true.
01/30/13   kedara  Subsystem self authentication updates.
01/18/13   dh      boot_sahara_tx_cb and boot_sahara_rx_cb will call unrecoverable_error_handler
                   when BULK_ERR_CABLE_DISCONNECT error is returned in memory debug mode.
                   Resend hello packet if an unknown command is received 
                   during wait for hello response state
12/10/12   plc     Fix Klockwork Errors
12/05/12   dh      Add boot log message to boot_sahara_handle_hello
11/08/12   kedara  Updates to enable hashing and authentication of elf's.
11/06/12   dh      In case of BULK_ERR_CABLE_DISCONNECT detected when sending hello
                   packet, call sahara_dispatch_tbl->init to reinitialize hw
10/24/12   kedara  do not convert elf entry address from virt to physical.
                   ZI init done as soon as segment is loaded. Fix elf size
                   update for to handle ZI only segment.
10/12/12   kedara  Signal MBA image early, to start processing PMI elf segments.
10/01/12   dh      No need to check for sahara_auth_tbl if it's in memory debug mode
09/20/12   kedara  Load hash segment to fixed buffer in DDR ZI.
                   Image src field used to modify the offset from the image
                   header for binary images.
                   Added authentication support for binary images.
09/14/12   kedara  Skip adding hash segment size to total loaded segment sizes.
08/21/12   kedara  Added support for subsystem self authentication/
                   load elf in chunks
06/28/12   aus     Added length/data size validation checks and ported updates
                   from PBL
06/25/12   aus     Moved conditional length check to explicit length check
06/08/12   as      Removed check for expected_image to make sahara
                   more flexible.
05/17/12   dh      In boot_sahara_init, allow initialization if expected image type 
                   is unknown and mode is memory debug 
02/13/12   aus     Fix klocwork issue for NULL pointer dereferencing
09/28/11   aus     Code clean-up
09/13/11   aus     Added updates to handle receive buffer boundaries and moved
                   state transitions to process_packets()
08/25/11   aus     Fixed logging so that we log only transmitted command packets
03/28/11   niting  Added binary image header accessor functions.
03/22/11   niting  Removed boot_sahara_elf_init and boot_sahara_get_image_info 
                   since all data is now passed through sahara_interface.
03/22/11   niting  Moved definitions to compiler files.
03/22/11   niting  Abstracted memory debug interface; pre-filled shared data 
                   passed as part of entry interface.
03/22/11   niting  Changed boot_sahara_entry inputs to pass interface.
02/18/11   aus     Updates to command execute packets in Sahara protocol.
02/08/11   aus     Added support for checking the NULL type entry which corresponds to the hash of
                   elf header and program headers.
02/07/11   aus     Added support for Sahara command flow to start image transfer after 
                   dumping out the RAM dumps without a reset.
01/06/11   aus     Added a parameter to distinguish between command and data packets.
09/15/10   niting  Abstracted image_type from Sahara. 
09/15/10   niting  Added parallel authentication and load support
                   for ELF images.
09/10/10   niting  Added non-blocking transfer support. 
08/06/10   niting  Moved sahara initialization check to Sahara client.
07/27/10   niting  Added command mode.
07/26/10   niting  Updated bulk error definitions.
06/23/10   niting  Fixed reset issue for RAW_DATA timeout.
06/03/10   niting  Added memory debug support and checking of mode. 
06/02/10   niting  Cleanup and added splitting of data transfers that exceed 
                   a maximum transfer size. 
05/25/10   niting  Added timeout error handling. 
05/14/10   niting  Fixed reset condition.
05/11/10   niting  Added error checking and validation.
04/30/10   niting  Changed driver max packet size to read from API.
04/21/10   niting  Added support for ELF in Sahara.
04/12/10   niting  Cleanup.
04/09/10   niting  Added maximum command packet length in Hello packet.
03/17/10   niting  Initial version of Sahara protocol.

=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_sahara.h"
#include "boot_rmb.h"
#include "boot_util.h"
#include "boot_logger.h"
#include "boot_dload_debug.h"

/**
 * Infinite loop
 */
#define INFINITE_LOOP() while(1)
#define SAHARA_MAX_ADDRESS 0xFFFFFFFF

#define BOOT_MHI_SKIP_DATA 0xFFFFDEAD

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

//===========================================================================
// Type declarations
//===========================================================================
enum boot_sahara_state
{
  SAHARA_STATE_ENTRY,               // entry state
  SAHARA_STATE_WAIT_HELLO_RESP,     // waiting for HELLO before anything else
  SAHARA_STATE_WAIT_RESET,          // waiting for RESET from host
  SAHARA_STATE_WAIT_DONE,           // waiting for DONE from host
  SAHARA_STATE_DATA_BINARY_HDR,     // receiving binary header
  SAHARA_STATE_DATA_BINARY,         // receiving binary data
  SAHARA_STATE_DATA_ELF_HDR,        // receiving ELF header
  SAHARA_STATE_DATA_ELF_PROG_HDR,   // receiving ELF program headers
  SAHARA_STATE_DATA_ELF_HASH_TABLE, // receiving ELF hash table
  SAHARA_STATE_DATA_ELF_SEGMENTS,   // receiving ELF data segments
  SAHARA_STATE_WAIT_MEMORY_READ,    // waiting for MEMORY_READ packets
  SAHARA_STATE_WAIT_CMD_EXEC,       // waiting for CMD_EXEC packets from host
  SAHARA_STATE_WAIT_CMD_EXEC_DATA,  // waiting for CMD_EXEC_DATA packet from 
                                    //   host
};

enum boot_sahara_packet_direction
{
  SAHARA_PACKET_TRANSMIT = 0,       // Transmitting packet
  SAHARA_PACKET_RECEIVE,            // Receiving packet
  SAHARA_PACKET_MAX = 0x7FFFFFFF    // To ensure 32-bits wide
};

enum boot_ssa_length_update_state
{
  SSA_LENGTH_QUEUE = 0,       // Queue length for delayed update
  SSA_LENGTH_NO_QUEUE,        // Update length via RMB immediately
  SSA_LENGTH_FLUSH,           // Update stored length.
  SSA_LENGTH_MAX = 0x7FFFFFFF    // To ensure 32-bits wide
};

struct boot_sahara_packet_log
{
  enum boot_sahara_cmd_id command;             // command received or
                                               //   to be transmitted
  enum boot_sahara_state state;                // current Sahara state
  uint32 length;                               // length of packet in bytes
  enum boot_sahara_packet_direction direction; // direction of packet
  uint32 log_num;                              // packet log number
};

//===========================================================================
// Variables
//===========================================================================
// Sahara interface
static struct boot_sahara_interface* sahara_if = NULL;
struct boot_sahara_shared_data* sahara_shared_data = NULL;

// Sahara shared data structures
static struct boot_sahara_binary_image_info* sahara_binary_image_info = NULL;
static struct boot_sahara_elf_image_info* sahara_elf_image_info = NULL;

// ELF related info
static boolean sahara_shared_seg_found = FALSE;

// Stores state of Sahara protocol being executed
static enum boot_sahara_state sahara_state;

// This boolean controls the condition under which we receive a DONE command
// from the host
static boolean sahara_done = FALSE;

// This boolean controls whether the current expected image is the last image
//   to be transferred
static enum boot_sahara_mode sahara_mode = 
  SAHARA_MODE_LAST;

// Packet buffer to store command packets, binary image header, or ELF header
uint8 sahara_packet_buffer[SAHARA_MAX_PACKET_SIZE_IN_BYTES];

// Pointer to command execution response information
static uint32 sahara_cmd_exec_client_command = 0;
static uint32 sahara_cmd_exec_data_length = 0;
uint8* sahara_cmd_exec_data = NULL;

// Packet buffer to store restricted memory data
static uint8* sahara_mem_debug_buffer = NULL;

// Memory debug function table
static struct boot_sahara_mem_debug_tbl* sahara_mem_debug_tbl = NULL;

// Memory debug enable flag - set to TRUE if memory debug function table passed
boolean sahara_mem_debug_enabled = FALSE;

// H/W interface dispatch table
static struct boot_sahara_dispatch_tbl* sahara_dispatch_tbl = NULL;

// Protocol versioning information
static uint32 sahara_major_version = SAHARA_VERSION_MAJOR;
static uint32 sahara_major_version_supported = SAHARA_VERSION_MAJOR_SUPPORTED;

// Enable/Disable logging
#define SAHARA_ENABLE_LOGGING

// Logging information
#ifdef SAHARA_ENABLE_LOGGING
static struct boot_sahara_packet_log sahara_log[SAHARA_PACKET_LOG_LENGTH] __attribute__((used));
static uint32 sahara_packet_log_num = 0;
static uint32 sahara_packet_log_num_total = 0;
#endif /* SAHARA_ENABLE_LOGGING */

// Callback parameters
static uint32 sahara_rx_bytes_read = 0;
static enum boot_sahara_bulk_error sahara_rx_err_code = BULK_SUCCESS;
static volatile boolean sahara_rx_complete = FALSE;
static uint32 sahara_tx_bytes_read = 0;
static enum boot_sahara_bulk_error sahara_tx_err_code = BULK_SUCCESS;
static volatile boolean sahara_tx_complete = FALSE;

// Parallel Load/Authentication parameters
static boolean sahara_auth_elf = FALSE;
static boolean sahara_auth_bin = FALSE;
static boolean sahara_is_ehdr_phdr_hash_success = FALSE;
static struct boot_sahara_auth_tbl* sahara_auth_tbl = NULL;
static boolean sahara_parallel_init = FALSE;
static boolean sahara_parallel_hash = FALSE;
static void* sahara_parallel_buf = NULL;
static uint32 sahara_parallel_size = 0;

// Subsystem Self authentication [ssa] enable check
static boolean sahara_ssa_enabled = FALSE;
static boolean sahara_ssa_is_rx_for_elf_segments = FALSE;

// Variables to support PCIe boot
uint32 last_loaded_segment_end_offset = 0x0;
boolean sahara_start_of_segment_load = FALSE;
uint32 remaining_elf_segments_data = 0x0;

//===========================================================================
//
//                      LOCAL FUNCTION DECLARATIONS
//
//===========================================================================
static void boot_sahara_handle_done( uint8* packet_buffer, 
                                     uint32 length );
static void boot_sahara_handle_done_resp( void );
static void boot_sahara_handle_end_image_tx( enum boot_sahara_status status );
static void boot_sahara_handle_hello( void );
static void boot_sahara_handle_hello_resp( uint8* packet_buffer, 
                                           uint32 length );
static uint32 boot_sahara_handle_rx( uint8* packet_buffer, 
                                     uint32 length, 
                                     uint32 max_length,
                                     enum boot_sahara_bulk_error *err_code,
                                     enum boot_sahara_packet_type type);
static uint32 boot_sahara_handle_tx( uint8* packet_buffer,
                                     uint32 length,
                                     enum boot_sahara_bulk_error *err_code);
static void boot_sahara_handle_read_data( uint32 image_id, 
                                          uint32 data_offset, 
                                          uint32 data_length );
static void boot_sahara_handle_reset( uint8* packet_buffer, 
                                      uint32 length );
static void boot_sahara_handle_reset_resp( void );
static void boot_sahara_handle_memory_debug( void );
static void boot_sahara_handle_memory_read( uint8* packet_buffer, 
                                            uint32 length );
static void boot_sahara_handle_memory_64bits_read( uint8* packet_buffer, 
                                            uint32 length );
static void boot_sahara_handle_cmd_ready( void );
static void boot_sahara_handle_cmd_exec( uint8* packet_buffer, 
                                         uint32 length );
static void boot_sahara_handle_cmd_exec_resp( void );
static void boot_sahara_handle_cmd_exec_data( uint8* packet_buffer,
                                              uint32 length );
static void boot_sahara_handle_cmd_switch_mode( uint8* packet_buffer, 
                                                uint32 length );
static void boot_sahara_reset( void );
static void boot_sahara_process_packets( void );
static void boot_sahara_rx_cb ( uint32 bytes_read,
                                uint32 err_code );
static void boot_sahara_tx_cb ( uint32 bytes_written,
                                uint32 err_code );
static enum boot_sahara_status boot_sahara_populate_elf_meta_info
                              ( int32 hash_table_index);
static void boot_sahara_ssa_update_rmb_length
                              ( uint32 length, enum boot_ssa_length_update_state ssa_state);                              

static boolean boot_sahara_elf_verify_start_addr
                     ( Elf32_Ehdr* elf_header, struct progressive_boot_block* prog_headers);

static enum boot_sahara_status boot_sahara_verify_elf_hdr
                     ( Elf32_Ehdr* elf_header, struct progressive_boot_block* prog_headers);

/* ============================================================================
**  Function : boot_sahara_rx_cb
** ============================================================================
*/
/*!
*   @brief
*   Callback for driver bulk receive. Sets the number of bytes read, error
*   code (if any), and completion flag.
*   
*   @details
*   Callback for driver bulk receive. Sets the number of bytes read, error
*   code (if any), and completion flag.
*   
*   @param bytes_read   -  [IN] Number of bytes read by driver
*   @param err_code     -  [IN] Error code returned by driver
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_rx_cb
(
  uint32 bytes_read,
  uint32 err_code
)
{
  sahara_rx_bytes_read = bytes_read;
  sahara_rx_err_code = (enum boot_sahara_bulk_error) err_code;
  sahara_rx_complete = TRUE;
  
  /* Call unrecoverable_error_handler if
     BULK_ERR_CABLE_DISCONNECT error is returned in ram dump mode*/
  if(sahara_rx_err_code == BULK_ERR_CABLE_DISCONNECT && 
     sahara_mode == SAHARA_MODE_MEMORY_DEBUG)
  {
    sahara_dispatch_tbl->unrecoverable_error_handler();
  }
}

/* ============================================================================
**  Function : boot_sahara_tx_cb
** ============================================================================
*/
/*!
*   @brief
*   Callback for driver bulk transmit. Sets the number of bytes sent, error
*   code (if any), and completion flag.
*   
*   @details
*   Callback for driver bulk transmit. Sets the number of bytes sent, error
*   code (if any), and completion flag.
*   
*   @param bytes_written   -  [IN] Number of bytes written by driver
*   @param err_code        -  [IN] Error code returned by driver
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_tx_cb
(
  uint32 bytes_written, 
  uint32 err_code
)
{
  sahara_tx_bytes_read = bytes_written;
  sahara_tx_err_code = (enum boot_sahara_bulk_error) err_code;
  sahara_tx_complete = TRUE;
  
  /* Call unrecoverable_error_handler if
     BULK_ERR_CABLE_DISCONNECT error is returned in ram dump mode*/  
  if(sahara_tx_err_code == BULK_ERR_CABLE_DISCONNECT &&
     sahara_mode == SAHARA_MODE_MEMORY_DEBUG)
  {
    sahara_dispatch_tbl->unrecoverable_error_handler();
  }
}

/* ============================================================================
**  Function : boot_sahara_log_packet_buffer
** ============================================================================
*/
/*!
*   @brief
*   This function logs the command in the input packet buffer along with some
*   packet and state information.
*   
*   @details
*   This function stores the packet buffer command and length into log. The
*   direction of the packet (transmit or receive) and the current state
*   of the target state machine is also stored. The packets are stored
*   in a round robin fashion where if the log fills up, the index into the
*   log will reset to 0 and overwrite the log entries starting from the
*   index 0 of the log buffer.
*   
*   @param *packet_buffer   -  [IN/OUT] Input packet
*   @param length           -  [IN    ] Packet length
*   @param direction        -  [IN    ] Packet transmit or receive
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_log_packet_buffer
(
  uint8* packet_buffer,
  uint32 length,
  enum boot_sahara_packet_direction direction
)
{
#ifdef SAHARA_ENABLE_LOGGING
  if (sahara_packet_log_num >= SAHARA_PACKET_LOG_LENGTH)
  {
    sahara_packet_log_num = 0;
  }
  sahara_log[sahara_packet_log_num].command = 
    (enum boot_sahara_cmd_id) packet_buffer[0];
  sahara_log[sahara_packet_log_num].state = sahara_state;
  sahara_log[sahara_packet_log_num].length = length;
  sahara_log[sahara_packet_log_num].direction = direction;
  sahara_log[sahara_packet_log_num].log_num = sahara_packet_log_num_total;
  sahara_packet_log_num++;
  sahara_packet_log_num_total++;
  (void) sahara_log; // to remove compiler warning
#endif /* SAHARA_ENABLE_LOGGING */
}

/* ============================================================================
**  Function : boot_sahara_log_raw_data
** ============================================================================
*/
/*!
*   @brief
*   This function logs the raw data packet received or transmitted.
*   
*   @details
*   
*   @param length           -  [IN    ] Length of data packet
*   @param direction        -  [IN    ] Packet transmit or receive
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_log_raw_data
(
  uint32 length,
  enum boot_sahara_packet_direction direction
)
{
#ifdef SAHARA_ENABLE_LOGGING
  if (sahara_packet_log_num >= SAHARA_PACKET_LOG_LENGTH)
  {
    sahara_packet_log_num = 0;
  }
  sahara_log[sahara_packet_log_num].command = SAHARA_NO_CMD_ID;
  sahara_log[sahara_packet_log_num].state = sahara_state;
  sahara_log[sahara_packet_log_num].length = length;
  sahara_log[sahara_packet_log_num].direction = direction;
  sahara_log[sahara_packet_log_num].log_num = sahara_packet_log_num_total;
  sahara_packet_log_num++;
  sahara_packet_log_num_total++;
  (void) sahara_log; // to remove compiler warning
#endif /* SAHARA_ENABLE_LOGGING */
}

/* ============================================================================
**  Function : boot_sahara_is_valid_elf_header
** ============================================================================
*/
/*!
*   @brief
*   Validates the contents of the ELF header.
*   
*   @details
*   This function parses through the fields of the input ELF header to
*   check whether the contents are valid or not.
*   
*   @param ehdr   -  [IN] ELF header
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if the header is valid; FALSE otherwise
*   
*   @sa None
*/
static boolean boot_sahara_is_valid_elf_header
(
  Elf32_Ehdr ehdr
)
{
  boolean retval = TRUE;

  do
  {
    // Check magic numbers in ELF header
    if ( ehdr.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0 ||
         ehdr.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1 ||
         ehdr.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2 ||
         ehdr.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3 )
    {
      retval = FALSE;
      break;
    }
  
    if ( ehdr.e_ident[ELFINFO_CLASS_INDEX] != ELF_CLASS_32 )
    {
      retval = FALSE;
      break;
    }
  
    if ( ehdr.e_ident[ELFINFO_VERSION_INDEX] != ELF_VERSION_CURRENT )
    {
      retval = FALSE;
      break;
    }
  
    if ( ehdr.e_ehsize != sizeof(ehdr) )
    {
      retval = FALSE;
      break;
    }
  
    if ( ehdr.e_phentsize != sizeof(struct segment_record) )
    {
      retval = FALSE;
      break;
    }
	
	// ensure that there is atleast 1 prog-header entry and total number 
    // of prog-hdr segments are not more than allocated prog-hdr buffer in SBL
    if ( (ehdr.e_phnum == 0) || (ehdr.e_phnum > MI_PBT_MAX_SEGMENTS) )
    {
      retval = FALSE;
      break;
    }
	
  } while (0);
  
  return retval;
} /* boot_sahara_is_valid_elf_header() */


/* ============================================================================
**  Function : boot_sahara_elf_segment_is_loadable
** ============================================================================
*/
/*!
*   @brief
*   Returns whether or not to load the segment
*   
*   @details
*   Based on the segment type and flags, this function returns whether
*   the segment can be loaded. The type must be specified as a loadable
*   segment and the flags must be set to non-pageable. The flags must
*   cannot be set to "not used" or "shared".
*   
*   @param type    -  [IN] ELF segment type
*   @param flags   -  [IN] ELF segment flags
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns FALSE if segment should not be loaded; TRUE otherwise
*   
*   @sa None
*/
boolean boot_sahara_elf_segment_is_loadable
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
)
{
  return ( ( type == MI_PBT_ELF_PT_LOAD ) &&
           ( MI_PBT_PAGE_MODE_VALUE(flags) == MI_PBT_NON_PAGED_SEGMENT ) &&
           ( MI_PBT_ACCESS_TYPE_VALUE(flags) != MI_PBT_NOTUSED_SEGMENT ) &&
           // hash segment to be loaded explicitly to fixed buffer
           ( MI_PBT_SEGMENT_TYPE_VALUE(flags) != MI_PBT_HASH_SEGMENT ) &&
           ( MI_PBT_ACCESS_TYPE_VALUE(flags) != MI_PBT_SHARED_SEGMENT ) );
} /* boot_sahara_elf_segment_is_loadable() */

/* ============================================================================
**  Function : boot_sahara_exit
** ============================================================================
*/
/*!
*   @brief
*   This function shuts down the Sahara protocol and the hardware interface
*   
*   @details
*   This function uses the dispatch table to shutdown the driver and to
*   reset the state machine. The dispatch table pointer is also set to NULL.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_exit ( void )
{
  // Shut down hardware interface
  sahara_dispatch_tbl->shutdown();

  if ((sahara_state != SAHARA_STATE_WAIT_RESET) &&
      (sahara_state != SAHARA_STATE_WAIT_HELLO_RESP) &&
      (sahara_state != SAHARA_STATE_WAIT_MEMORY_READ) &&
      (sahara_state != SAHARA_STATE_WAIT_CMD_EXEC))
  {
    // Remove pointer to dispatch table
    sahara_dispatch_tbl = NULL;
  }

  // Reset Sahara state
  sahara_state = SAHARA_STATE_ENTRY;

  // Set state to allow sahara_process_packets to exit in case
  //   boot_sahara_reset returns - this allows Sahara client 
  //   to regain control
  sahara_done = TRUE;
}

/* ============================================================================
**  Function : boot_sahara_handle_rx
** ============================================================================
*/
/*!
*   @brief
*   Receives a single packet of size length.
*   
*   @details
*   This function uses the dispatch table to receive a bulk of data into
*   the input packet buffer. The buffer size passed to the driver is passed
*   as a multiple of the bulk_min_buffer_size stored in the dispatch table.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   @param *err_code        -  [IN/OUT] Reception error code
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns number of bytes received in RAW_DATA packet
*   
*   @sa None
*/
static uint32 boot_sahara_handle_rx
(
  uint8* packet_buffer,
  uint32 length,
  uint32 max_length,
  enum boot_sahara_bulk_error *err_code,
  enum boot_sahara_packet_type type
)
{
  uint32 rx_length = 0;
  uint32 max_packet_size = sahara_dispatch_tbl->get_max_packet_size();
  uint32 buff_ovf ;

  // Length parameter checked to ensure there is no integer overflow
  if (length > max_length) {
    *err_code = BULK_ERR_UNKNOWN;
    return 0;
  }

  if( FALSE == sahara_dispatch_tbl->elf_optimized_segment_load())
  {
    // *** Note: rounding up buffer will not work for optimized elf loading ***
    // Adjust buffer size to account for hardware buffer size limitation
    // and be a multiple of max_packet_size
    buff_ovf = length % max_packet_size;
  if (buff_ovf > 0)
  {
    length += (max_packet_size - buff_ovf);
  }
  // Length parameter checked to ensure there is no buffer overflow
    if (length > max_length) 
    {
      *err_code = BULK_ERR_UNKNOWN;
      return 0;
    }
  }
  // Initialize callback parameters
  sahara_rx_bytes_read = 0;
  sahara_rx_err_code = BULK_SUCCESS;
  sahara_rx_complete = FALSE;

  // Receive packet
  sahara_dispatch_tbl->rx_bulk(packet_buffer,
                               length,
                               boot_sahara_rx_cb,
                               (uint32*)err_code,
                               type
                               );

  if (sahara_parallel_hash)
  {
    sahara_auth_tbl->update_hash(sahara_parallel_buf,
                                 sahara_parallel_size);
    sahara_parallel_hash = FALSE;
  }

  while (!sahara_rx_complete)
  {
    sahara_dispatch_tbl->poll();
  }

  rx_length = sahara_rx_bytes_read;
  *err_code = sahara_rx_err_code;

  // Update length to be communicated to ssa subsystem if applicable
  // (only update the length for previous chunk/segment which would 
  // ensure that hash has matched or queued)
  if(BOOT_MHI_SKIP_DATA != (uint32) packet_buffer)
  {
  boot_sahara_ssa_update_rmb_length(rx_length, SSA_LENGTH_QUEUE);
  }
  
  if(TRUE == sahara_dispatch_tbl->elf_optimized_segment_load() &&
     TRUE == sahara_start_of_segment_load
   )
  {
    // update total to reflect consumed data
    remaining_elf_segments_data = remaining_elf_segments_data - rx_length;
  }
  return rx_length;
}

/* ============================================================================
**  Function : boot_sahara_handle_tx
** ============================================================================
*/
/*!
*   @brief
*   Transmits a single packet of size length.
*   
*   @details
*   This function uses the dispatch table to transmit a bulk of data passed in
*   the input packet buffer. The length of data to be transmitted is based
*   on the size of the input packet length. This function also logs the
*   transmitted packet.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   @param *err_code        -  [IN/OUT] Transmission error code
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns number of bytes transmitted
*   
*   @sa None
*/
static uint32 boot_sahara_handle_tx
(
  uint8* packet_buffer,
  uint32 length,
  enum boot_sahara_bulk_error *err_code
)
{
  uint32 tx_length = 0;

  // Initialize callback parameters
  sahara_tx_bytes_read = 0;
  sahara_tx_err_code = BULK_SUCCESS;
  sahara_tx_complete = FALSE;

  // Send packet
  sahara_dispatch_tbl->tx_bulk(packet_buffer,
                               length,
                               boot_sahara_tx_cb,
                               (uint32*)err_code
                               );

  while (!sahara_tx_complete)
  {
    sahara_dispatch_tbl->poll();
  }

  tx_length = sahara_tx_bytes_read;
  *err_code = sahara_tx_err_code;

  return tx_length;
}

/* ============================================================================
**  Function : boot_sahara_handle_end_image_tx
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for END_IMAGE_TX packet.
*   
*   @details
*   This function fills the fields in the command packet buffer for the
*   END_IMAGE_TX packet and transmits the packet. If there is any error,
*   the target will try to send another END_IMAGE_TX packet with the
*   corresponding error.
*   
*   @param status   -  [IN] Status of image transfer to be sent to host
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_end_image_tx
(
  enum boot_sahara_status status
)
{
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  uint32 tx_length;
  enum boot_sahara_status tx_status = SAHARA_STATUS_SUCCESS;
  struct sahara_packet_end_image_tx* packet_end_image_tx = 
    (struct sahara_packet_end_image_tx*)sahara_packet_buffer;

  // Setup END_IMAGE_TX packet
  packet_end_image_tx->command = SAHARA_END_IMAGE_TX_ID;
  packet_end_image_tx->length = sizeof(struct sahara_packet_end_image_tx);
  packet_end_image_tx->image_id = sahara_shared_data->expected_image;
  packet_end_image_tx->status = status;

  // Update sahara interface status
  sahara_if->sahara_status = status;

  // Log transmitted packet
  boot_sahara_log_packet_buffer((uint8 *)packet_end_image_tx,
                                packet_end_image_tx->length,
                                SAHARA_PACKET_TRANSMIT);
                                
  // Transmit END_IMAGE_TX packet
  tx_length = boot_sahara_handle_tx((uint8 *)packet_end_image_tx,
                                             packet_end_image_tx->length,
                                             &err_code);

  // Check for error in transmission of END_IMAGE_TX packet
  if (err_code == BULK_ERR_TIMEOUT)
  {
    // ERROR: Timeout while transmitting data
    tx_status = SAHARA_NAK_TIMEOUT_TX;
  }
  else if (err_code)
  {
    // ERROR: General transmission or reception error
    tx_status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
  }
  else if (tx_length != packet_end_image_tx->length)
  {
    // ERROR: Invalid tranmission length
    tx_status = SAHARA_NAK_INVALID_TX_LENGTH;
  }

  // Switch to WAIT_RESET state if error
  if ((status != SAHARA_STATUS_SUCCESS) || 
      (tx_status != SAHARA_STATUS_SUCCESS))
  {
    sahara_state = SAHARA_STATE_WAIT_RESET;
  }

  // Reset if END_IMAGE_TX transmission failed
  // Do not wait for RESET command
  if (tx_status != SAHARA_STATUS_SUCCESS)
  {
    // Update sahara interface status
    sahara_if->sahara_status = tx_status;

    boot_sahara_reset();
  }
}

/* ============================================================================
**  Function : boot_sahara_handle_read_data
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for READ_DATA packet
*   
*   @details
*   This function fills the fields in the command packet buffer for the
*   READ_DATA packet and transmits the packet. If there is any error,
*   the target will try to send an END_IMAGE_TX packet with the
*   corresponding error.
*   
*   @param image_id      -  [IN] ID of image to read data from
*   @param data_offset   -  [IN] Offset into image file to start reading data
*                                from
*   @param data_length   -  [IN] Length of data to read
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_read_data
(
  uint32 image_id,
  uint32 data_offset,
  uint32 data_length
)
{
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  uint32 tx_length;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  struct sahara_packet_read_data* packet_read_data = 
    (struct sahara_packet_read_data*)sahara_packet_buffer;

  // Setup READ_DATA packet
  packet_read_data->command = SAHARA_READ_DATA_ID;
  packet_read_data->length = sizeof(struct sahara_packet_read_data);
  packet_read_data->image_id = image_id;
  packet_read_data->data_offset = data_offset;
  packet_read_data->data_length = data_length;

  // Log transmitted packet
  boot_sahara_log_packet_buffer((uint8 *)packet_read_data,
                                packet_read_data->length,
                                SAHARA_PACKET_TRANSMIT);
                                
  // Transmit READ_DATA packet
  tx_length = boot_sahara_handle_tx((uint8 *)packet_read_data,
                                             packet_read_data->length,
                                             &err_code);

  // Check for error in transmission of READ_DATA packet
  if (err_code == BULK_ERR_TIMEOUT)
  {
    // ERROR: Timeout while transmitting data
    status = SAHARA_NAK_TIMEOUT_TX;
  }
  else if (err_code)
  {
    // ERROR: General transmission or reception error
    status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
  }
  else if (tx_length != packet_read_data->length)
  {
    // ERROR: Invalid tranmission length
    status = SAHARA_NAK_INVALID_TX_LENGTH;
  }

  if (status != SAHARA_STATUS_SUCCESS)
  {
    // If transmission error, send END_IMAGE_TX with error
    boot_sahara_handle_end_image_tx(status);
  }
}

/* ============================================================================
**  Function : boot_sahara_skip_data_in_optmize_load
** ============================================================================
*/
/*!
*   @brief
*   This function instructs pcie driver to skip data in between two
*   load segments
*   
*   @details
*   This function instructs pcie driver to skip data in between two
*   load segments by calling into mhi driver with Null pointer as image
*   destinition
*
*   @param image_offset      -  [IN    ] Offset into image to read data from
*   @param image_data_size   -  [IN    ] Size of data to read
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns retval_description_goes_here
*   
*   @sa None
*/
static enum boot_sahara_status boot_sahara_skip_data_in_optmize_load
(
  uint32 image_offset,      // offset into expected image file
  uint32 image_data_size    // size of image data to be received
)
{
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  uint32 data_to_skip;                                   
  
  uint32 rx_length;                // length of raw data packet received
  // error code returned from raw data read
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  
  if( TRUE == sahara_dispatch_tbl->elf_optimized_segment_load() &&
      TRUE == sahara_start_of_segment_load
    )
  {
    do
    {
      if(last_loaded_segment_end_offset !=0 )
      {
        // Find elf file data between current and previous segment of
        // type "load" that needs to be skipped.
        data_to_skip = image_offset - last_loaded_segment_end_offset;
        while(data_to_skip != 0)
        {
          rx_length = boot_sahara_handle_rx((uint8 *)BOOT_MHI_SKIP_DATA,
                                            data_to_skip,
                                            data_to_skip,
                                            &err_code,
                                            SAHARA_PACKET_TYPE_DATA /* Data Packet*/);      
          // Check for reception error of data
          if (err_code == BULK_ERR_TIMEOUT)
          {
            // ERROR: Timeout while receiving data
            status = SAHARA_NAK_TIMEOUT_RX;
            break;
          }
          else if (err_code)
          {
            // ERROR: General transmission or reception error
            status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
            break;
          }
          else if (rx_length > data_to_skip)
          {
            // ERROR: Invalid image data size received
            status = SAHARA_NAK_INVALID_DATA_SIZE;
            break;
          }
          data_to_skip = data_to_skip - rx_length;
        }
      }
      // Update segment end offset for next segment retrieval
      last_loaded_segment_end_offset = image_offset + image_data_size;    
    }while(0);
  }
  return status;
}


/* ============================================================================
**  Function : boot_sahara_receive_data
** ============================================================================
*/
/*!
*   @brief
*   This function splits up image data reads into multiple transfers based on
*   the maximum transfer size.
*   
*   @details
*   This function splits up image data reads into multiple transfers based on
*   the maximum transfer size. Multiple READ_DATA requests and RAW_DATA
*   transfers are issued based on the image_data_size and maximum allowable
*   transfer size.
*   If optimized elf load is enabled, a single READ_DATA is issued before 
*   calling into this api and data is retrieved over PCIe link.
*   
*   @param image_offset      -  [IN    ] Offset into image to read data from
*   @param image_data_size   -  [IN    ] Size of data to read
*   @param *image_dest       -  [IN/OUT] Destination of image data
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns retval_description_goes_here
*   
*   @sa None
*/
static enum boot_sahara_status boot_sahara_receive_data
(
  uint32 image_offset,      // offset into expected image file
  uint32 image_data_size,   // size of image data to be received
  uint32 image_data_size_max,    // maximum size available in the destination buffer
  uint8* image_dest         // destination address data to be received
)
{
  uint32 rx_length;                // length of raw data packet received
  uint32 image_transfer_size;      // used when image_data_size is larger than
                                   //   max transfer size
  uint32 image_transfer_size_left; // used when image_data_size is larger than
                                   //   max transfer size
  uint32 image_data_received_per_chunk; // used to keep track if read_data request is to be
                                        // sent when image data is bigger than max raw data size
  uint32 max_raw_data_size;                           

  // status of transfer
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

  // error code returned from raw data read
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;


  // If data size to be read is less than the max size
  if (image_data_size_max >= image_data_size)
  {
  // Split into multiple transfers if image_data_size is larger
  //  than maximum transfer size
  image_transfer_size_left = image_data_size;
    
    status = boot_sahara_skip_data_in_optmize_load(image_offset, image_data_size);
   
    max_raw_data_size = sahara_dispatch_tbl->get_max_raw_data_size();
    image_data_received_per_chunk = max_raw_data_size;
   
  while ((status == SAHARA_STATUS_SUCCESS) &&
         (image_transfer_size_left != 0))
  {
    // Determine transfer size
        if (image_transfer_size_left > max_raw_data_size)
    {
          image_transfer_size = max_raw_data_size;
    }
    else
    {
      image_transfer_size = image_transfer_size_left;
    }

      // Send READ_DATA for all cases where elf segment data is not being
      // currently loaded in optimized image load logic and there is no 
      // residual data to be read from a read_data command already sent
      
      if( (!(TRUE == sahara_dispatch_tbl->elf_optimized_segment_load() &&
          TRUE == sahara_start_of_segment_load)
           ) &&
          (image_data_received_per_chunk == max_raw_data_size)
      )
    {
    // Send READ_DATA request to read data
    boot_sahara_handle_read_data(sahara_shared_data->expected_image,
                                 image_offset,
                                 image_transfer_size);
                                   
        image_data_received_per_chunk = 0;
    }

    // Check for transmission error for READ_DATA packet
    if (sahara_state != SAHARA_STATE_WAIT_RESET)
    {
      // Retrieve segment data for image from host
      rx_length = boot_sahara_handle_rx(image_dest,
                                        image_transfer_size,
                                        image_data_size_max,
                                        &err_code,
                                        SAHARA_PACKET_TYPE_DATA /* Data Packet*/);

      boot_sahara_log_raw_data(rx_length, SAHARA_PACKET_RECEIVE);

      // Check for reception error of data
      if (err_code == BULK_ERR_TIMEOUT)
      {
        // ERROR: Timeout while receiving data
        status = SAHARA_NAK_TIMEOUT_RX;
        break;
      }
      else if (err_code)
      {
        // ERROR: General transmission or reception error
        status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
        break;
      }
      else if (rx_length != image_transfer_size)
      {
         // In case of optimized elf segment load MDM will essentially wait
         // forever to receive requested chunk of data. Done to handle lower layers
         // returning less than requested bytes in given api invocation and reduce 
         // wait time. Incase of host errors, host/AP should reset the mdm.
         if( TRUE == sahara_dispatch_tbl->elf_optimized_segment_load() &&
             rx_length < image_transfer_size
            )
          {
            image_transfer_size = rx_length;
          }
          else
          {
        // ERROR: Invalid image data size received
        status = SAHARA_NAK_INVALID_DATA_SIZE;
        break;
      }
        }

      // Setup parallel hash
      if (sahara_auth_elf && sahara_parallel_init)
      {
        sahara_parallel_hash = TRUE;
        sahara_parallel_buf = image_dest;
        sahara_parallel_size = image_transfer_size;
      }
    }
    else
    {
      // ERROR: Error while transmitting READ_DATA packet
      status = SAHARA_NAK_READ_DATA_ERROR;
      break;
    }

    // Update parameters for next transfer
    image_dest += image_transfer_size;
    image_offset += image_transfer_size;
    image_transfer_size_left -= image_transfer_size;
    image_data_size_max -= image_transfer_size;
      image_data_received_per_chunk += image_transfer_size;      
      
  }
  }
  else
  {
    // ERROR: Invalid image data size   
    status = SAHARA_NAK_INVALID_DATA_SIZE;
  }

  return status;
}

/* ============================================================================
**  Function : boot_sahara_receive_image_binary
** ============================================================================
*/
/*!
*   @brief
*   Receives an image of binary format
*   
*   @details
*   This function first issues a READ_DATA for the binary image header. The
*   destination of the image header is just command packet buffer. If
*   valid data is received, the target will copy the image header from
*   the command packet buffer and then parse the image header to extract
*   the information to read the rest of the image. The rest of the image will
*   be requested in a single READ_DATA request and directly transferred
*   into the destination address based on the image header information.
*   If any error occurs, the corresponding error will be returned;
*   otherwise, success status is returned.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns status of image transfer
*   
*   @sa None
*/
static enum boot_sahara_status boot_sahara_receive_image_binary( void )
{
  uint32 image_offset;       // offset into expected image file
  uint32 binary_image_header_size;  // size of image header to be received
  uint32 buff_ovf;           // used in buffer overflow calculation
  uint32 max_packet_size;    // used in buffer overflow calculation
  uint32 image_data_size;    // size of image data to be received
  uint32 image_data_size_max; // image data size rounded up to the bulk packet buffer size
  uint8* image_dest = NULL;  // destination address data to be received
  uint8* binary_image_header = NULL; // pointer to received image header
  uint32 image_id;  // image ID of the image to be received
  
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS; // status of transfer

  max_packet_size = sahara_dispatch_tbl->get_max_packet_size();

  do
  {
    // Get binary image header
    sahara_state = SAHARA_STATE_DATA_BINARY_HDR;

    // Setup image header read
    image_offset = 0;
    binary_image_header_size = sahara_dispatch_tbl->get_bin_header_size();
    sahara_binary_image_info->image_id = sahara_shared_data->expected_image;
  
    status = boot_sahara_receive_data(image_offset, 
                                      binary_image_header_size, 
                                      SAHARA_MAX_PACKET_SIZE_IN_BYTES,
                                      (uint8*) sahara_packet_buffer);
    // Check for error
    if (status != SAHARA_STATUS_SUCCESS)
    {
      break;
    }

    // copy image header into shared data
    sahara_dispatch_tbl->memcpy(sahara_binary_image_info->header,
                                &sahara_packet_buffer[0],
                                binary_image_header_size);
    
    binary_image_header = sahara_binary_image_info->header;

    // Get binary image
    sahara_state = SAHARA_STATE_DATA_BINARY;

    // read image directly into memory based on destination stored
    // in image header
    image_offset = binary_image_header_size + 
       sahara_dispatch_tbl->get_bin_image_src(binary_image_header);
    image_data_size = 
      sahara_dispatch_tbl->get_bin_image_size(binary_image_header);
    image_dest =
      (uint8*)(uintnt)sahara_dispatch_tbl->get_bin_image_dest(binary_image_header);
    image_id = sahara_dispatch_tbl->get_bin_image_id(binary_image_header);

    // Sahara does NOT check for image type embedded
    // in the image that is received from the host. This was
    // done to provide flexibility to the user to be able to
    // pass any image type. For non-secure images this is
    // not a problem and for secure image it will fail 
    // authentication if image type is not right. 

    // Validate destination address range
    // Take into account the added overflow due to buffer size
    // passed into H/W receive
    image_data_size_max = image_data_size;
    buff_ovf = image_data_size % max_packet_size;
    if (buff_ovf > 0)
    {

      image_data_size_max += max_packet_size - buff_ovf;
      if ((uint32)image_data_size_max < image_data_size)
      {
        // ERROR: Integer overflow
      status = SAHARA_NAK_INVALID_DEST_ADDR;
      break;
      }
    }

    // Validate received image ID is same as expected image ID
    if (image_id != sahara_shared_data->expected_image)
    {
      if (sahara_dispatch_tbl->skip_image_load())
      {
        // Do not error fatal in case of invalid image ID.
        // Mark it as success and move to loading the next image.
        status = SAHARA_STATUS_SUCCESS;
      }
      else 
      {
        // ERROR: Invalid image ID
        status = SAHARA_NAK_UNEXPECTED_IMAGE_ID;  
      }
      break;
    }

    if (! sahara_dispatch_tbl->valid_address_range((uint64)(image_dest), 
                                                   (uint64)(image_data_size_max),(uint32)0x1))
    {
      if (sahara_dispatch_tbl->skip_image_load())
      {
        // Do not error fatal in case of invalid image ID.
        // Mark it as success and move to loading the next image.
        status = SAHARA_STATUS_SUCCESS;
      }
      else 
      {
        // ERROR: Invalid destination address
        status = SAHARA_NAK_INVALID_DEST_ADDR;  
      }
      break;
    }

    status = boot_sahara_receive_data(image_offset, 
                                      image_data_size, 
                                      image_data_size_max, 
                                      image_dest);
    // Check for error
    if (status != SAHARA_STATUS_SUCCESS)
    {
      break;
    }

    if (sahara_auth_bin)
    {
      // Authenticate binary image
      if (!sahara_auth_tbl->authenticate(binary_image_header))
      {
        // ERROR: Failed to authenticate binary image
        status = SAHARA_NAK_IMAGE_AUTH_FAILURE;
        break;
      }        
    }

  } while (0);

  // Reset if timeout occurred during image data transfer
  if (status == SAHARA_NAK_TIMEOUT_RX)
  {
    sahara_state = SAHARA_STATE_WAIT_RESET;
    boot_sahara_reset();
  }

  return status;
}

/*===========================================================================
**  Function :  boot_sahara_elf_verify_start_addr
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies elf entry_point (image start address) falls 
*   within loadable elf segments area [loadable segment's (phy/virt address
*   + file_size)].
*   
*   @param *elf_header      -  [IN] pointer to elf_header
*   @param *prog_headers    -  [IN] pointer to prog_header
*
* @par Dependencies
*   first hash in the hash table always represents hash of ehdr+phdr 
* 
* @retval
*   boolean status - TRUE if elf image entry_point/start address is within 
*                    loadable segment's range otherwise false 
* 
*/
static boolean boot_sahara_elf_verify_start_addr
(
  Elf32_Ehdr* elf_header,
  struct progressive_boot_block* prog_headers
)
{
  uint32 phdr_index = 0;
  boolean status = FALSE;
  struct segment_record* sahara_elf_phdrs = NULL; // pointer to program headers
  // Setup pointer to program header array
  sahara_elf_phdrs = prog_headers->segment;
  
  /* scan through program header segments */
  for (phdr_index = 0; (phdr_index < elf_header->e_phnum); phdr_index++)
  { 
    /* Only scan loadable segments with filesz>0 to check against entry_point  */
    if(boot_sahara_elf_segment_is_loadable(
            sahara_elf_phdrs[phdr_index].type,
            sahara_elf_phdrs[phdr_index].memory_attribute))
    {
      if( (sahara_elf_phdrs[phdr_index].mem_size > 0) && (sahara_elf_phdrs[phdr_index].size > 0) )
      {
      /* check again physical address range, most images have entry point
       within physical address range with completely different mapping
       for virtual space */
        if ( ((uint32)sahara_elf_phdrs[phdr_index].size < 
             (SAHARA_MAX_ADDRESS - (uint32)sahara_elf_phdrs[phdr_index].p_address)) && 
         (elf_header->e_entry >= sahara_elf_phdrs[phdr_index].p_address) && 
         (elf_header->e_entry < ((uint32)sahara_elf_phdrs[phdr_index].p_address + 
                        (uint32)sahara_elf_phdrs[phdr_index].size)) 
        ) 
      { 
      status = TRUE;
      break;
    }
      /* check again for virtual address range. for example, RPM image 
       entry_point falls within virtual range. */
        if ( ((uint32)sahara_elf_phdrs[phdr_index].size < 
             (SAHARA_MAX_ADDRESS - (uint32)sahara_elf_phdrs[phdr_index].v_address)) && 
         (elf_header->e_entry >= sahara_elf_phdrs[phdr_index].v_address) && 
         (elf_header->e_entry < ((uint32)sahara_elf_phdrs[phdr_index].v_address + 
                        (uint32)sahara_elf_phdrs[phdr_index].size)) 
        ) 
      { 
      status = TRUE;
      break;
    }
      }    
    }
  }
  return status;
}

/*===========================================================================
**  Function :  boot_sahara_verify_elf_hdr
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies hash of elf_hdr with an expected
*   hash value. If ehdr+phdr hash verification fails then the corresponding 
*   error will be returned; otherwise, success status is returned.
*   
*   @param *elf_header      -  [IN] pointer to elf_header
*   @param *prog_headers    -  [IN] pointer to prog_header
*
* @par Dependencies
*   first hash in the hash table always represents hash of ehdr+phdr 
* 
* @retval
*   None
* 
*/
static enum boot_sahara_status boot_sahara_verify_elf_hdr
(
  Elf32_Ehdr* elf_header,
  struct progressive_boot_block* prog_headers
)
{
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS; // status of transfer
  void* digest_buf_ptr = NULL; // points to hash digest buffer
  sahara_is_ehdr_phdr_hash_success = FALSE;
  
  do 
  {
    // ensure ehdr buffer exist 
    if ( elf_header == NULL )
    {
      status = SAHARA_NAK_INVALID_ELF_HDR;
      break;
    }

    // Validate ELF header contents 
    if (!boot_sahara_is_valid_elf_header(*elf_header))
    {
      // ERROR: Invalid ELF header received
      status = SAHARA_NAK_INVALID_ELF_HDR;
      break;      
    }

    // elf-header basic checks are ok, continue with elf_hdr+prog_hdr 
    //  hash verification
    // Verify hash of ELF and program headers
    if (!sahara_auth_tbl->init_hash())
    {
      // ERROR: Failed to verify hash for a given segment of ELF image
      status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
      break;
    }
    // Hash ELF header
    if (!sahara_auth_tbl->update_hash(elf_header,
                                  sizeof(Elf32_Ehdr)))
    {
      // ERROR: Failed to verify hash for a given segment of ELF image
      status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
      break;
    }

    // Hash program headers
    if (!sahara_auth_tbl->update_hash(prog_headers->segment,
                                  prog_headers->numsegments*
                                  sizeof(struct segment_record)))
    {
      // ERROR: Failed to verify hash for a given segment of ELF image
      status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
      break;
    }

    // Finalize hash to retrieve digest buffer
    digest_buf_ptr = sahara_auth_tbl->finalize_hash();
    if (digest_buf_ptr == NULL)
    {
      // ERROR: Failed to verify hash for a given segment of ELF image
      status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
      break;
    }

    // Compare hash values, 0 (phdr_index) corresponds to hash of 
    // ehdr+phdr in hash table (first hash in hash-table)
    if (!sahara_auth_tbl->compare_hash(digest_buf_ptr, 0))
    {
      // ERROR: Failed to verify hash for a given segment of ELF image
      status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
      break;
    }
  
    if (!boot_sahara_elf_verify_start_addr(elf_header, prog_headers))
    {
      // ERROR: Invalid Entry_point in elf header
      status = SAHARA_NAK_INVALID_ELF_HDR;
      break;
    }
    sahara_is_ehdr_phdr_hash_success = TRUE;
  } while (0);
  return status;
}

	
/* ============================================================================
**  Function : boot_sahara_receive_image_elf
** ============================================================================
*/
/*!
*   @brief
*   Receives an image of ELF format
*   
*   @details
*   This function first issues a READ_DATA to read the ELF header. The
*   destination of the ELF header is just command packet buffer. If
*   valid data is received, the target will copy the ELF header from
*   the command packet buffer and then parse the ELF header to extract
*   the information to read the program headers. Another READ_DATA
*   will be issued to read the program headers into the designated
*   shared data location. Once the program headers are received, a
*   READ_DATA will be issued for each loadable ELF segment. The destination
*   address for each segment will be extracted from the program headers
*   and data will be transferred directly to destination address. Once
*   all segments have been transferred successfully, the function will exit.
*   While parsing each program header, the function also stores the location
*   of the hash table and the shared segment index (if a shared segment is
*   found). If any error occurs, the corresponding error will be returned;
*   otherwise, success status is returned.
*      
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns status of image transfer
*   
*   @sa None
*/
static enum boot_sahara_status boot_sahara_receive_image_elf( void )
{
  uint32 image_offset;      // offset into expected image file
  uint32 image_header_size; // size of image header to be received
  uint32 buff_ovf;          // used in buffer overflow calculation
  uint32 max_packet_size;   // used in buffer overflow calculation
  uint32 image_data_size;   // size of image data to be received
  uint32 image_data_size_max; // used in buffer overflow calculation
  uint8* image_dest;        // destination address data to be received
  uint32 phdr_index;        // index into program header table to 
                            //   retrieve segments
  uint32 bss_len;          // number of bytes to ZI
  
  struct segment_record* sahara_elf_phdrs = NULL; // pointer to program headers
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS; // status of transfer
  int32 hash_table_index = -1; // used to indicate index of hash table segment
                               //   in program header table

  // Temporary variables to dereference pointers
  struct progressive_boot_block* prog_headers = sahara_elf_image_info->prog_headers;
  Elf32_Ehdr* elf_header = &(sahara_elf_image_info->elf_header);
  uint8* hash_table_header = NULL;
  uint32 original_hash_index =0, rx_length;
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;

  max_packet_size = sahara_dispatch_tbl->get_max_packet_size();
  
  do
  {
    if (prog_headers == NULL)
    {
      // ERROR: Uninitialized program header location
      status = SAHARA_NAK_UNINIT_PHDR_LOC;
      break;
    }
    else
    {
      // Get ELF header
      sahara_state = SAHARA_STATE_DATA_ELF_HDR;

      // Setup ELF header read
      image_offset = 0;
      image_header_size = sizeof(Elf32_Ehdr);
      sahara_binary_image_info->image_id = sahara_shared_data->expected_image;
    
      status = boot_sahara_receive_data(image_offset, 
                                        image_header_size, 
                                        SAHARA_MAX_PACKET_SIZE_IN_BYTES,
                                        (uint8*) sahara_packet_buffer);
      // Check for error
      if (status != SAHARA_STATUS_SUCCESS)
      {
        break;
      }
    }

    // copy Elf header into shared data
    sahara_dispatch_tbl->memcpy(elf_header,
                                (Elf32_Ehdr*) &sahara_packet_buffer[0],
                                sizeof(Elf32_Ehdr)
            );

    // Validate ELF header contents
    if (!boot_sahara_is_valid_elf_header(*elf_header))
    {
      // ERROR: Invalid ELF header received
      status = SAHARA_NAK_INVALID_ELF_HDR;
      break;      
    }

    // Determine physical entry point
    sahara_elf_image_info->entry_address = elf_header->e_entry;

    // Get program headers
    sahara_state = SAHARA_STATE_DATA_ELF_PROG_HDR;

    // Determine location and size of program headers
    image_offset = elf_header->e_phoff;
    image_header_size = elf_header->e_phnum *
                        elf_header->e_phentsize;

    // Setup storage for program headers
    image_dest =
      (uint8*) &(prog_headers->segment[0]);

    status = boot_sahara_receive_data(image_offset, 
                                      image_header_size, 
                                      (sizeof(struct segment_record) * MI_PBT_MAX_SEGMENTS),
                                      image_dest);
    // Check for error
    if (status != SAHARA_STATUS_SUCCESS)
    {
      break;
    }
    
    // Now fill out remaining header information, magic numbers to indicate
    // that the progressive header has been successfully read and parsed
    prog_headers->magic1 = MI_PBT_MAGIC1;
    prog_headers->magic2 = MI_PBT_MAGIC2;
    prog_headers->version = MI_PBT_VERSION;

    // Store the number of segments found
    prog_headers->numsegments = 
      elf_header->e_phnum;

    // Store the entry point address from elf header
    prog_headers->elf_image_entry_point = 
      elf_header->e_entry;

    // Setup pointer to program header array
    sahara_elf_phdrs = prog_headers->segment;

    // Get ELF Data segments
    sahara_state = SAHARA_STATE_DATA_ELF_SEGMENTS;

    // Load hash segment if either sahara_auth_elf or SSA is enabled
    // However authenticate hash only based on sahara_auth_elf flag
    if (sahara_auth_elf || sahara_ssa_enabled)
    {
      phdr_index = 0;
      // Determine hash table program header index
      while ((hash_table_index < 0) && 
             (phdr_index < elf_header->e_phnum))
      {  
        // Store location of hash table in shared data
        if ((MI_PBT_SEGMENT_TYPE_VALUE(
              sahara_elf_phdrs[phdr_index].memory_attribute) ==
             MI_PBT_HASH_SEGMENT))
        {
          hash_table_index = phdr_index;
          sahara_elf_image_info->hash_table = 
                        (uint8*)sahara_dispatch_tbl->get_hash_segment_buffer();
        }
        phdr_index++;
      }

      if (hash_table_index < 0)
      {
        // ERROR: Failed to find hash table in ELF image
        status = SAHARA_NAK_HASH_TABLE_NOT_FOUND;
        break;
      }

      // Load hash table
      image_offset = sahara_elf_phdrs[hash_table_index].segment_offset;
      image_dest = sahara_elf_image_info->hash_table;
      image_data_size = sahara_elf_phdrs[hash_table_index].size;

      // Validate destination address range
      // Take into account the added overflow due to buffer size
      // passed into H/W receive
      image_data_size_max = image_data_size;
      buff_ovf = image_data_size % max_packet_size;
      if (buff_ovf > 0)
      {
        image_data_size_max += max_packet_size - buff_ovf;
        if ((uint32)image_data_size_max < image_data_size)
        {
          // ERROR: Integer overflow
          status = SAHARA_NAK_INVALID_DEST_ADDR;
          break;
        }
      }
      
      // Validate available space to store hash segment
      if ((uint32) HASH_SEGMENT_SIZE < image_data_size)
      {
        // ERROR: Hash segment too big.
        status = SAHARA_NAK_INVALID_IMG_HASH_TABLE_SIZE;
        break;
      }      


      // Validate that hash segment is at least the size of
      // mi_boot_image_header_type
      if (image_data_size < sizeof(mi_boot_image_header_type))
      {
        // ERROR: Hash segment too small.
        status = SAHARA_NAK_INVALID_IMG_HASH_TABLE_SIZE;
        break;
      }

      // Hash segment loaded to local buffer. hence no need to
      // validate image_dest addr for hash segment loading.
      // Also an attempt to validate against whitelist table 
      // will result in error.

      // Skip segment if file size = 0
      if (image_data_size != 0)
      {
        status = boot_sahara_receive_data(image_offset, 
                                          image_data_size, 
                                          image_data_size_max,
                                          image_dest);
        // Check for error
        if (status != SAHARA_STATUS_SUCCESS)
        {
          break;
        }
      }

      // Check for error
      if (status != SAHARA_STATUS_SUCCESS)
      {
        break;
      }      
    }
    
    if (sahara_auth_elf)
    {
      // start with flag set to FALSE which tracks whether
      //ehdr+phdr hashing is successfully verified or not */
	  sahara_is_ehdr_phdr_hash_success = FALSE;
	  
      hash_table_header = (uint8*)image_dest;
      // Authenticate hash table
      if (!sahara_auth_tbl->authenticate(hash_table_header))
      {
        // ERROR: Failed to authenticate hash table
        status = SAHARA_NAK_HASH_TABLE_AUTH_FAILURE;
        break;
      }

      // Compute hash table offsets
      sahara_auth_tbl->compute_hash_offsets(
        (uint8*)(sahara_dispatch_tbl->get_bin_header_size() + image_dest),
        prog_headers->numsegments,
        prog_headers->segment);
		
      // After hash table auth success and before load/hash elf segments,  
      // explicitly hash-verify ehdr+phdr
	  status = boot_sahara_verify_elf_hdr( elf_header, prog_headers ); 
      if (status != SAHARA_STATUS_SUCCESS)
  	  {
	    break;
	  }	  
    }	

    // If SSA is enabled populate the meta data.
    // Call the api just *before* loading the elf data segments, and
    // *after* loading the elf header, program header, hash segment.

    // signal subsystem(MBA) through RMB about elf-meta-data only
    // after hash-segment authentication is complete to avoid consumption
    // of elf-meta-info before auth
    status = boot_sahara_populate_elf_meta_info(hash_table_index);
    if (status != SAHARA_STATUS_SUCCESS)
	{
	  break;
	}
	
    // Initialize total elf segments size.
    sahara_elf_image_info->total_elf_segments_size = 0x0;

    last_loaded_segment_end_offset = 0;
    sahara_start_of_segment_load = TRUE;

     if( TRUE == sahara_dispatch_tbl->elf_optimized_segment_load())
     {
       // Sort program header table based upon segment offsets, before requesting
       // any data segment from host.      
       status = sahara_dispatch_tbl->preprocess_header( prog_headers->segment,
                                                       prog_headers->numsegments,
                                                       &image_offset,
                                                       &remaining_elf_segments_data);

       if(status == SAHARA_STATUS_SUCCESS && remaining_elf_segments_data !=0 )
       {
         // Send READ_DATA request to read data of all remaining elf segments
         boot_sahara_handle_read_data(sahara_shared_data->expected_image,
                                     image_offset,
                                     remaining_elf_segments_data);
       }
     }    

    // Valid program headers received, retrieve each segment
    for (phdr_index = 0;
         (phdr_index < elf_header->e_phnum) &&
         (SAHARA_STATUS_SUCCESS == status);
         phdr_index++)
    {
      if ((hash_table_index == phdr_index) && 
          (sahara_auth_elf || sahara_ssa_enabled ))
      {
        // Hash table is already loaded
        continue;
      }
      // Only load the segments which are loadable
      // This will never be a hash segment as it is not loadable
      if (boot_sahara_elf_segment_is_loadable(
            sahara_elf_phdrs[phdr_index].type,
            sahara_elf_phdrs[phdr_index].memory_attribute))
      {
        image_offset = sahara_elf_phdrs[phdr_index].segment_offset;
        image_dest = (uint8*)(uintnt)sahara_elf_phdrs[phdr_index].p_address;
        image_data_size = sahara_elf_phdrs[phdr_index].size;

        if (! sahara_dispatch_tbl->valid_address_range((uint64)(image_dest),
                                                       (uint64)(sahara_elf_phdrs[phdr_index].mem_size),(uint32)0x1))
        {
          // ERROR: Invalid destination address
          status = SAHARA_NAK_INVALID_DEST_ADDR;
          break;
        }

        // Validate destination address range
        // Take into account the added overflow due to buffer size
        // passed into H/W receive
        image_data_size_max = image_data_size;
        buff_ovf = image_data_size % max_packet_size;
        if (buff_ovf > 0)
        {
          image_data_size_max += max_packet_size - buff_ovf;
          if ((uint32)image_data_size_max < image_data_size)
          {
            // ERROR: Integer overflow
            status = SAHARA_NAK_INVALID_DEST_ADDR;
            break;
          }
        }
        
        // Process only segments with non-zero memsize
        if(sahara_elf_phdrs[phdr_index].mem_size > 0)  
        {
          // Skip segment if file size = 0
          if (image_data_size != 0)
          {
            // Initialize hash of segment
            if (!sahara_auth_tbl->init_hash())
            {
              // ERROR: Failed to verify hash for a given segment of ELF image
              status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
              break;
            }
            sahara_parallel_init = TRUE;

            status = boot_sahara_receive_data(image_offset, 
                                            image_data_size, 
                                            image_data_size_max, 
                                            image_dest);
            // Check for error
            if (status != SAHARA_STATUS_SUCCESS)
            {
              break;
            }

            // proceed only if ehdr+phdr hash check was successful 
            if (sahara_is_ehdr_phdr_hash_success != TRUE)
            {
              status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
              break;
            }
 
            // Hash the final part of the segment
            if (sahara_parallel_hash == TRUE)
            {
              sahara_auth_tbl->update_hash(sahara_parallel_buf,
                                           sahara_parallel_size);
            }
            
            // Finalize hash of segment
            sahara_parallel_buf = sahara_auth_tbl->finalize_hash();
            if (sahara_parallel_buf == NULL)
            {
              // ERROR: Failed to verify hash for a given segment of ELF image
              status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
              break;
            }
            
            status = sahara_dispatch_tbl->get_orig_hash_index( phdr_index,
                                                               &original_hash_index);
            // Compare hash values
            if (!sahara_auth_tbl->compare_hash(sahara_parallel_buf,
                                               original_hash_index))
            {
              // ERROR: Failed to verify hash for a given segment of ELF image
              status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
              break;
            }
            
            sahara_parallel_init = FALSE;
            sahara_parallel_hash = FALSE;
            
            // Update remaining length to be communicated to ssa subsystem if applicable
            boot_sahara_ssa_update_rmb_length(0x0, SSA_LENGTH_FLUSH );
          }
        
          // Zero initialize bss region for loadable segments.
          // needed at this point to avoid getting data aborts while elf
          // Loading. Eg Q6 mba would lock xpu after loading Q6 elf. Doing
          // ZI init later might cause data abort.
          bss_len = sahara_elf_phdrs[phdr_index].mem_size -
                    sahara_elf_phdrs[phdr_index].size;
          if (bss_len > 0)
          {
            qmemset((void*)(uintnt)(sahara_elf_phdrs[phdr_index].p_address +
                              sahara_elf_phdrs[phdr_index].size),
                      0,
                      bss_len);
            // Update length to be communuicated to ssa subsystem if
            // applicable
            boot_sahara_ssa_update_rmb_length(bss_len, SSA_LENGTH_NO_QUEUE );
          }
        }    
      }
      else if (MI_PBT_ACCESS_TYPE_VALUE(
                 sahara_elf_phdrs[phdr_index].memory_attribute) ==
               MI_PBT_SHARED_SEGMENT)
      {
         /* Current design only allows for one shared segment */
         if (sahara_shared_seg_found)
         {
           // ERROR: Multiple shared segments found in ELF image
           status = SAHARA_NAK_MULTIPLE_SHARED_SEG;
           break;
         }
         sahara_shared_seg_found = TRUE;
         sahara_elf_image_info->shared_seg_index = phdr_index;
      }
      /* PHDR_TYPE (supported by older tools) and NULL_TYPE (supported by newer tools) 
         are both used for the entry which has the Hash of ELF and Program headers.  */      
      else if ((sahara_elf_phdrs[phdr_index].type == MI_PBT_ELF_PT_PHDR ) || 
               ((sahara_elf_phdrs[phdr_index].type == MI_PBT_ELF_PT_NULL ) && 
                (MI_PBT_SEGMENT_TYPE_VALUE(sahara_elf_phdrs[phdr_index].memory_attribute) ==
                MI_PBT_PHDR_SEGMENT)))
      {
        if(sahara_auth_elf)
        {
         // ehdr+phdr hash check should have been completed before reaching to 
         // this point
          if (sahara_is_ehdr_phdr_hash_success != TRUE)
          {
            status = SAHARA_NAK_HASH_VERIFICATION_FAILURE;
            break;
          } 
        }
      }
    }

    // Reset flag value
    sahara_start_of_segment_load = FALSE;
    
    // Restore sahara_ssa_is_rx_for_elf_segments to default value.
    sahara_ssa_is_rx_for_elf_segments = FALSE;
    
  } while (0);

  if(status != SAHARA_STATUS_SUCCESS &&
     TRUE == sahara_dispatch_tbl->elf_optimized_segment_load()
    )
  { 
    // If any error occurred while receiving image, flush pending
    // data to be read
    while(remaining_elf_segments_data != 0x0)
    {
      rx_length = boot_sahara_handle_rx((uint8 *)BOOT_MHI_SKIP_DATA,
                                      remaining_elf_segments_data,
                                      remaining_elf_segments_data,
                                      &err_code,
                                      SAHARA_PACKET_TYPE_DATA /* Data Packet*/);    
      remaining_elf_segments_data = remaining_elf_segments_data - rx_length;
    }
  }
  
  // Reset if timeout occurred during image data transfer
  if (status == SAHARA_NAK_TIMEOUT_RX)
  {
    sahara_state = SAHARA_STATE_WAIT_RESET;
    boot_sahara_reset();
  }

  return status;
}

/* ============================================================================
**  Function : boot_sahara_receive_image
** ============================================================================
*/
/*!
*   @brief
*   Start receiving an image based on expected image.
*   
*   @details
*   This function receives and image based on the expected image type. If any
*   error occurs during the transfer of the image, the corresponding error
*   code will be transmitted in an END_IMAGE_TX packet.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_receive_image( void )
{
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS; // status of transfer

  switch (sahara_shared_data->expected_image_type)
  {
    case SAHARA_IMAGE_TYPE_BINARY:
      status = boot_sahara_receive_image_binary();
      break;
    case SAHARA_IMAGE_TYPE_ELF:
      status = boot_sahara_receive_image_elf();
      break;
    default:
      // ERROR: Invalid image type received
      status = SAHARA_NAK_INVALID_IMAGE_TYPE;
      break;
  }

  // Handle any special post processing after image loading
  if(status == SAHARA_STATUS_SUCCESS )
  {
    status = sahara_dispatch_tbl->image_load_post_proc();
  }

  // Only send status if no transmission or reception errors
  if (sahara_state != SAHARA_STATE_WAIT_RESET)
  {
    // Send END_IMAGE_TX with status
    boot_sahara_handle_end_image_tx(status);
  }

  // Update to WAIT_DONE state if successfully received image 
  if (sahara_state != SAHARA_STATE_WAIT_RESET)
  {
    sahara_shared_data->image_rx_complete = TRUE;
    sahara_state = SAHARA_STATE_WAIT_DONE;
  }
}

/* ============================================================================
**  Function : boot_sahara_handle_hello_resp
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for HELLO_RESP packet
*   
*   @details
*   This function parses the fields in the HELLO_RESP packet. If there is any
*   error or mismatch of protocol between the target and the host,
*   the target will send an END_IMAGE_TX packet with the corresponding error.
*   Otherwise, the target will proceed to receive the expected image.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_hello_resp
(
  uint8* packet_buffer,
  uint32 length
)
{
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  struct sahara_packet_hello_resp* packet_hello_resp = 
    (struct sahara_packet_hello_resp*)packet_buffer;

  do
  {
      boot_log_message("Sahara: Hello Response Received");
      
      if ((length != sizeof(struct sahara_packet_hello_resp)) ||
          (length != packet_hello_resp->length))
      {
        // Check if length corresponds to V1
        if (length != SAHARA_PACKET_HELLO_RESP_LENGTH_V1)
        {
          // ERROR: Invalid packet size received
          status = SAHARA_NAK_INVALID_PACKET_SIZE;
          break;
        }
        else if (packet_hello_resp->version != 0x1)
        {
          // Host is not running version 1 of protocol
          // ERROR: Invalid packet size received
          status = SAHARA_NAK_INVALID_PACKET_SIZE;
          break;
        }
      }
  
      // Check host status
      if (SAHARA_STATUS_SUCCESS != packet_hello_resp->status)
      {
        if (status < SAHARA_NAK_LAST_CODE)
        {
          // ERROR: Error on host
          status = (enum boot_sahara_status)packet_hello_resp->status;
          break;
        }
        else
        {
          // ERROR: Unknown host error received in HELLO_RESP
          status = SAHARA_NAK_UNKNOWN_HOST_ERROR;
          break;
        }
      }
  
      // Check protocol version running on host
      if (packet_hello_resp->version < packet_hello_resp->version_supported)
      {
        // ERROR: Invalid host protocol version
        status = SAHARA_NAK_INVALID_HOST_PROTOCOL;
        break;
      }
      else if ((packet_hello_resp->version > sahara_major_version) ||
               (packet_hello_resp->version < sahara_major_version_supported))
      {
        // ERROR: Protocol mismatch between host and target
        status = SAHARA_NAK_PROTOCOL_MISMATCH;
        break;
      }
  
      // Check if valid mode is returned
      if (length != SAHARA_PACKET_HELLO_RESP_LENGTH_V1)
      {  
        if (packet_hello_resp->mode < SAHARA_MODE_LAST)
        {
          sahara_mode = (enum boot_sahara_mode) packet_hello_resp->mode;
        }
        else
        {
          // ERROR: Invalid mode received from host
          status = SAHARA_NAK_INVALID_HOST_MODE;
          break;
        }
      }
  } while (0);

  if (status != SAHARA_STATUS_SUCCESS)
  {
    // Send END_IMAGE_TX with error
    boot_sahara_handle_end_image_tx(status);
  }
  else if (sahara_mode == SAHARA_MODE_MEMORY_DEBUG)
  {
    if (!sahara_mem_debug_enabled)
    {
      // ERROR: Memory debug not supported
      status = SAHARA_NAK_MEMORY_DEBUG_NOT_SUPPORTED;
      boot_sahara_handle_end_image_tx(status);
    }
    else
    {
      if (!sahara_mem_debug_tbl->mem_debug_supported())
      {
        // ERROR: Memory debug not supported
        status = SAHARA_NAK_MEMORY_DEBUG_NOT_SUPPORTED;
        boot_sahara_handle_end_image_tx(status);
      }
      else
      {
        boot_sahara_handle_memory_debug();
      }
    }
  }
  else if (sahara_mode == SAHARA_MODE_COMMAND)
  {
    // Send CMD_READY packet to host
    boot_sahara_handle_cmd_ready();
  }
  else
  {  
    // Start receiving expected image
    boot_sahara_receive_image();
  }
}

/* ============================================================================
**  Function : boot_sahara_handle_hello
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for HELLO packet
*   
*   @details
*   This function fills the fields in the command packet buffer for the
*   HELLO packet and transmits the packet. If there is any error,
*   the target will attempt to keep resending the HELLO packet until
*   a successful transmission is complete.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_hello( void )
{
  // error code returned from raw data read
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;

  uint32 tx_length;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  struct sahara_packet_hello* packet_hello = 
    (struct sahara_packet_hello*)sahara_packet_buffer;

  // Setup HELLO packet
  packet_hello->command = SAHARA_HELLO_ID;
  packet_hello->length = sizeof(struct sahara_packet_hello);
  packet_hello->version = sahara_major_version;
  packet_hello->version_supported = sahara_major_version_supported;
  packet_hello->cmd_packet_length = SAHARA_MAX_PACKET_SIZE_IN_BYTES;
  packet_hello->mode = sahara_mode;
  packet_hello->reserved0 = 0;
  packet_hello->reserved1 = 0;
  packet_hello->reserved2 = 0;
  packet_hello->reserved3 = 0;
  packet_hello->reserved4 = 0;
  packet_hello->reserved5 = 0;

  // Continue to send HELLO packets until successful transmission
  do
  {
    // Log transmitted packet
    boot_sahara_log_packet_buffer((uint8 *)packet_hello,
                                  packet_hello->length,
                                  SAHARA_PACKET_TRANSMIT);
                                
    // Transmit HELLO packet
    tx_length = boot_sahara_handle_tx((uint8 *)packet_hello,
                                      packet_hello->length,
                                      &err_code);
  
    // Check for transmission error of HELLO
    if (err_code == BULK_ERR_TIMEOUT)
    {
      // ERROR: Timeout while receiving data
      status = SAHARA_NAK_TIMEOUT_RX;
    }
    else if (err_code == BULK_ERR_CABLE_DISCONNECT)
    {
      // ERROR: Cable disconnected
      /* Call hw init again */
      status = SAHARA_NAK_TARGET_INIT_FAILURE;
      sahara_dispatch_tbl->init();
    }
    else if (err_code)
    {
      // ERROR: General transmission or reception error
      status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
    }
    else if (tx_length != packet_hello->length)
    {
      // ERROR: Invalid tranmission length
      status = SAHARA_NAK_INVALID_TX_LENGTH;
    }
    else
    {
      status = SAHARA_STATUS_SUCCESS;
    }
  } while (status != SAHARA_STATUS_SUCCESS);
 
  sahara_state = SAHARA_STATE_WAIT_HELLO_RESP;
  boot_log_message("Sahara: Hello pkt sent");
}

/* ============================================================================
**  Function : boot_sahara_handle_done_resp
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for DONE_RESP packet
*   
*   @details
*   This function fills the fields in the command packet buffer for the
*   DONE_RESP packet and transmits the packet. If there is any error,
*   the target will attempt to keep resending the DONE_RESP packet until
*   a successful transmission is complete. Once complete, if all images have
*   been transferred, the hardware driver will be shutdown.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_done_resp( void )
{
  // error code returned from raw data read
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;

  uint32 tx_length;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  struct sahara_packet_done_resp* packet_done_resp = 
    (struct sahara_packet_done_resp*)sahara_packet_buffer;

  // Setup DONE_RESP packet
  packet_done_resp->command = SAHARA_DONE_RESP_ID;
  packet_done_resp->length = sizeof(struct sahara_packet_done_resp);
  packet_done_resp->image_tx_status = sahara_mode;

  // Keep transmitting DONE_RESP packet until transmission is successful
  do
  {
    // Log transmitted packet
    boot_sahara_log_packet_buffer((uint8 *)packet_done_resp,
                                  packet_done_resp->length,
                                  SAHARA_PACKET_TRANSMIT);  
  
    // Transmit DONE_RESP packet
    tx_length = boot_sahara_handle_tx((uint8 *)packet_done_resp,
                                               packet_done_resp->length,
                                               &err_code);

    // Check for transmission error of DONE_RESP
    if (err_code == BULK_ERR_TIMEOUT)
    {
      // ERROR: Timeout while receiving data
      status = SAHARA_NAK_TIMEOUT_RX;

      // Reset if timeout occurs - no return
      sahara_state = SAHARA_STATE_WAIT_RESET;
      boot_sahara_reset();
    }
    else if (err_code)
    {
      // ERROR: General transmission or reception error
      status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
    }
    else if (tx_length != packet_done_resp->length)
    {
      // ERROR: Invalid tranmission length
      status = SAHARA_NAK_INVALID_TX_LENGTH;
    }
    else
    {
      status = SAHARA_STATUS_SUCCESS;
    }
  } while (status != SAHARA_STATUS_SUCCESS);

  if (sahara_mode == SAHARA_MODE_IMAGE_TX_COMPLETE)
  {
    // Shutdown Sahara if all images have been transferred
    boot_sahara_exit();
  }
}

/* ============================================================================
**  Function : boot_sahara_handle_done
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for DONE packet
*   
*   @details
*   This function parses the fields in the DONE packet. If there is any
*   error in the packet, the target will send an END_IMAGE_TX packet
*   with the corresponding error. Otherwise, the target will proceed to
*   send a DONE_RESP.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_done
(
  uint8* packet_buffer,
  uint32 length
)
{
  struct sahara_packet_done* packet_done = 
    (struct sahara_packet_done*)packet_buffer;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

    if ((length != sizeof(struct sahara_packet_done)) ||
        (length != packet_done->length))
    {
      // ERROR: Invalid packet size received
      status = SAHARA_NAK_INVALID_PACKET_SIZE;
      boot_sahara_handle_end_image_tx(status);
    }
    else
    {
      // end of image tranfer
      boot_sahara_handle_done_resp();
      sahara_done = TRUE;
    }
  }

/* ============================================================================
**  Function : boot_sahara_handle_reset_resp
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for RESET_RESP packet
*   
*   @details
*   This function fills the fields in the command packet buffer for the
*   RESET_RESP packet and transmits the packet. If there is any error,
*   the target does not attempt to resend. The target will proceed to
*   shutdown the hardware driver and then reset.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_reset_resp( void )
{
  // error code returned from raw data read
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;

  struct sahara_packet_reset_resp* packet_reset_resp = 
    (struct sahara_packet_reset_resp*)sahara_packet_buffer;

  // Setup RESET_RESP packet
  packet_reset_resp->command = SAHARA_RESET_RESP_ID;
  packet_reset_resp->length = sizeof(struct sahara_packet_reset_resp);

  // Log transmitted packet
  boot_sahara_log_packet_buffer((uint8 *)packet_reset_resp,
                                packet_reset_resp->length,
                                SAHARA_PACKET_TRANSMIT);
                                
  // Transmit RESET_RESP packet - do not check if transmission was valid
  boot_sahara_handle_tx((uint8 *)packet_reset_resp,
                                 packet_reset_resp->length,
                                 &err_code);

  // Reset the target
  boot_sahara_reset();
}

/* ============================================================================
**  Function : boot_sahara_handle_reset
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for RESET packet
*   
*   @details
*   This function parses the fields in the RESET packet. If there is any
*   error in the packet, the target will send an END_IMAGE_TX packet
*   with the corresponding error. Otherwise, the target will proceed to
*   send a RESET_RESP.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_reset
(
  uint8* packet_buffer,
  uint32 length
)
{
  struct sahara_packet_reset* packet_reset = 
    (struct sahara_packet_reset*)packet_buffer;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

    boot_log_message("Sahara: Reset request received");
  
  if ((length != sizeof(struct sahara_packet_reset)) ||
      (length != packet_reset->length))
  {
    // ERROR: Invalid packet size received
    status = SAHARA_NAK_INVALID_PACKET_SIZE;
    boot_sahara_handle_end_image_tx(status);
  }
  else
  {
    // reset target - No return
    boot_sahara_handle_reset_resp();
  }
}

/* ============================================================================
**  Function : boot_sahara_handle_memory_debug
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for MEMORY_DEBUG packet.
*   
*   @details
*   This function sends the memory region table and table length in the
*   MEMORY_DEBUG packet to the host. The state then transitions to wait
*   for MEMORY_READ packets.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_memory_debug( void )
{
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  uint32 tx_length;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

  // Setup MEMORY_DEBUG packet
  struct sahara_packet_memory_64bits_debug* packet_memory_debug = 
    (struct sahara_packet_memory_64bits_debug*)sahara_packet_buffer;
  packet_memory_debug->command = SAHARA_MEMORY_DEBUG_64BITS_ID;

  // Initialize memory region table
  sahara_mem_debug_tbl->mem_debug_init();

  packet_memory_debug->length = sizeof(*packet_memory_debug);

  packet_memory_debug->memory_table_addr = 
    sahara_mem_debug_tbl->mem_debug_table_addr();
  packet_memory_debug->memory_table_length = 
    sahara_mem_debug_tbl->mem_debug_table_len();

  // Log transmitted packet
  boot_sahara_log_packet_buffer((uint8 *)packet_memory_debug,
                                packet_memory_debug->length,
                                SAHARA_PACKET_TRANSMIT);
                                
  // Transmit MEMORY_DEBUG packet
  tx_length = boot_sahara_handle_tx((uint8 *)packet_memory_debug,
                                    packet_memory_debug->length,
                                    &err_code);

  // Check for error in transmission of MEMORY_DEBUG packet
  if (err_code == BULK_ERR_TIMEOUT)
  {
    // ERROR: Timeout while transmitting data
    status = SAHARA_NAK_TIMEOUT_TX;
  }
  else if (err_code)
  {
    // ERROR: General transmission or reception error
    status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
  }
  else if (tx_length != packet_memory_debug->length)
  {
    // ERROR: Invalid tranmission length
    status = SAHARA_NAK_INVALID_TX_LENGTH;
  }

  if (status != SAHARA_STATUS_SUCCESS)
  {
    // If transmission error, send END_IMAGE_TX with error
    boot_sahara_handle_end_image_tx(status);
  }
  else
  {
    // Wait for MEMORY_READ packets
    sahara_state = SAHARA_STATE_WAIT_MEMORY_READ;
  }
}


/* ============================================================================
**  Function : boot_sahara_handle_memory_64bits_read
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for MEMORY_64BITS_READ packet
*   
*   @details
*   This function parses the incoming MEMORY_64BITS_READ packet and extracts the
*   memory location and length of data to read and send to host. The address
*   and length validity are checked before sending the data. If invalid
*   address or length is received, then an END_IMAGE_TX with the corresponding
*   error is sent.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_memory_64bits_read
( 
  uint8* packet_buffer, 
  uint32 length
)
{
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  uint32 tx_length;
  struct sahara_packet_memory_64bits_read* packet_memory_read = 
    (struct sahara_packet_memory_64bits_read*)packet_buffer;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  uint32 memory_transfer_size;      // used when memory region is restricted
  uint32 memory_transfer_size_left; // used when memory region is restricted
  uint32 memory_transfer_addr;      // used when memory region is restricted
  
    if ((length != sizeof(struct sahara_packet_memory_64bits_read)) ||
        (length != packet_memory_read->length))
    {
      // ERROR: Invalid packet size received
      status = SAHARA_NAK_INVALID_PACKET_SIZE;
      boot_sahara_handle_end_image_tx(status);
    }
    else if (!sahara_mem_debug_tbl->mem_debug_verify_addr(
               packet_memory_read->memory_addr,
               packet_memory_read->memory_length))
    {
      // ERROR: Invalid memory read access
      status = SAHARA_NAK_INVALID_MEMORY_READ;
      boot_sahara_handle_end_image_tx(status);
    }
    else
    {
      if (sahara_mem_debug_tbl->mem_debug_is_restricted_addr(
            packet_memory_read->memory_addr,
            packet_memory_read->memory_length))
      {
        memory_transfer_size_left = packet_memory_read->memory_length;
        memory_transfer_addr = packet_memory_read->memory_addr;

		
        while ((status == SAHARA_STATUS_SUCCESS) &&
               (memory_transfer_size_left != 0))
        {
          // Determine transfer size
          if (memory_transfer_size_left > 
              SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES)
          {
            memory_transfer_size = SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES;
          }
          else
          {
            memory_transfer_size = memory_transfer_size_left;
          }

          // Copy over data to temporary buffer - actual copy is performed
          //   by Sahara client
          sahara_mem_debug_buffer = 
            (uint8*)(uintnt)sahara_mem_debug_tbl->mem_debug_copy_restricted(
                                             memory_transfer_addr,
                                             memory_transfer_size);

          // Check if restricted memory address was copied
          // Note: Address 0 cannot be a restricted address
          if (sahara_mem_debug_buffer == NULL)
          {
            // ERROR: Invalid memory read access
            status = SAHARA_NAK_INVALID_MEMORY_READ;
            boot_sahara_handle_end_image_tx(status);
          }

          // Transmit memory
          tx_length = boot_sahara_handle_tx(sahara_mem_debug_buffer,
                                            memory_transfer_size,
                                            &err_code);

          // Check for error in transmission of memory data
          if (err_code == BULK_ERR_TIMEOUT)
          {
            // ERROR: Timeout while transmitting data
            status = SAHARA_NAK_TIMEOUT_TX;
          }
          else if (err_code)
          {
            // ERROR: General transmission or reception error
            status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
          }
          else if (tx_length != memory_transfer_size)
          {
            // ERROR: Invalid tranmission length
            status = SAHARA_NAK_INVALID_TX_LENGTH;
          }
          else
          {
            // Update parameters for next transfer
            memory_transfer_addr += memory_transfer_size;
            memory_transfer_size_left -= memory_transfer_size;
          }
        }  
      }
      else
      {
        tx_length = boot_sahara_handle_tx((uint8 *)((uintnt)packet_memory_read->memory_addr),
                                          packet_memory_read->memory_length,
                                          &err_code);
      
        // Check for error in transmission of memory data
        if (err_code == BULK_ERR_TIMEOUT)
        {
          // ERROR: Timeout while transmitting data
          status = SAHARA_NAK_TIMEOUT_TX;
        }
        else if (err_code)
        {
          // ERROR: General transmission or reception error
          status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
        }
        else if (tx_length != packet_memory_read->memory_length)
        {
          // ERROR: Invalid tranmission length
          status = SAHARA_NAK_INVALID_TX_LENGTH;
        }
      }

      if (status != SAHARA_STATUS_SUCCESS)
      {
        // If transmission error, send END_IMAGE_TX with error
        boot_sahara_handle_end_image_tx(status);
      }
    }
  }

  
/* ============================================================================
**  Function : boot_sahara_handle_memory_read
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for MEMORY_READ packet
*   
*   @details
*   This function parses the incoming MEMORY_READ packet and extracts the
*   memory location and length of data to read and send to host. The address
*   and length validity are checked before sending the data. If invalid
*   address or length is received, then an END_IMAGE_TX with the corresponding
*   error is sent.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_memory_read
( 
  uint8* packet_buffer, 
  uint32 length
)
{
  uint32 memory_transfer_size;      // used when memory region is restricted
  uint32 memory_transfer_size_left; // used when memory region is restricted
  uint32 memory_transfer_addr;      // used when memory region is restricted

  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  uint32 tx_length;
  struct sahara_packet_memory_read* packet_memory_read = 
    (struct sahara_packet_memory_read*)packet_buffer;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

    if ((length != sizeof(struct sahara_packet_memory_read)) ||
        (length != packet_memory_read->length))
    {
      // ERROR: Invalid packet size received
      status = SAHARA_NAK_INVALID_PACKET_SIZE;
      boot_sahara_handle_end_image_tx(status);
    }
    else if (!sahara_mem_debug_tbl->mem_debug_verify_addr(
               packet_memory_read->memory_addr,
               packet_memory_read->memory_length))
    {
      // ERROR: Invalid memory read access
      status = SAHARA_NAK_INVALID_MEMORY_READ;
      boot_sahara_handle_end_image_tx(status);
    }
    else
    {
      if (sahara_mem_debug_tbl->mem_debug_is_restricted_addr(
            packet_memory_read->memory_addr,
            packet_memory_read->memory_length))
      {
        memory_transfer_size_left = packet_memory_read->memory_length;
        memory_transfer_addr = packet_memory_read->memory_addr;

        while ((status == SAHARA_STATUS_SUCCESS) &&
               (memory_transfer_size_left != 0))
        {
          // Determine transfer size
          if (memory_transfer_size_left > 
              SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES)
          {
            memory_transfer_size = SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES;
          }
          else
          {
            memory_transfer_size = memory_transfer_size_left;
          }

          // Copy over data to temporary buffer - actual copy is performed
          //   by Sahara client
          sahara_mem_debug_buffer = 
            (uint8*)(uintnt)sahara_mem_debug_tbl->mem_debug_copy_restricted(
                                             memory_transfer_addr,
                                             memory_transfer_size);

          // Check if restricted memory address was copied
          // Note: Address 0 cannot be a restricted address
          if (sahara_mem_debug_buffer == NULL)
          {
            // ERROR: Invalid memory read access
            status = SAHARA_NAK_INVALID_MEMORY_READ;
            boot_sahara_handle_end_image_tx(status);
          }

          // Transmit memory
          tx_length = boot_sahara_handle_tx(sahara_mem_debug_buffer,
                                            memory_transfer_size,
                                            &err_code);

          // Check for error in transmission of memory data
          if (err_code == BULK_ERR_TIMEOUT)
          {
            // ERROR: Timeout while transmitting data
            status = SAHARA_NAK_TIMEOUT_TX;
          }
          else if (err_code)
          {
            // ERROR: General transmission or reception error
            status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
          }
          else if (tx_length != memory_transfer_size)
          {
            // ERROR: Invalid tranmission length
            status = SAHARA_NAK_INVALID_TX_LENGTH;
          }
          else
          {
            // Update parameters for next transfer
            memory_transfer_addr += memory_transfer_size;
            memory_transfer_size_left -= memory_transfer_size;
          }
        }
      }
      else
      {
        // Transmit memory
        tx_length = boot_sahara_handle_tx((uint8 *)(uintnt)packet_memory_read->memory_addr,
                                          packet_memory_read->memory_length,
                                          &err_code);
  
        // Check for error in transmission of memory data
        if (err_code == BULK_ERR_TIMEOUT)
        {
          // ERROR: Timeout while transmitting data
          status = SAHARA_NAK_TIMEOUT_TX;
        }
        else if (err_code)
        {
          // ERROR: General transmission or reception error
          status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
        }
        else if (tx_length != packet_memory_read->memory_length)
        {
          // ERROR: Invalid tranmission length
          status = SAHARA_NAK_INVALID_TX_LENGTH;
        }
      }

      if (status != SAHARA_STATUS_SUCCESS)
      {
        // If transmission error, send END_IMAGE_TX with error
        boot_sahara_handle_end_image_tx(status);
      }
    }
  }

/* ============================================================================
**  Function : boot_sahara_handle_cmd_ready
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for CMD_READY packet.
*   
*   @details
*   This function sends the CMD_READY packet to the host and transitions
*   to the WAIT_CMD_EXEC state to await command execution packets.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_cmd_ready( void )
{
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  uint32 tx_length;
  struct sahara_packet_cmd_ready* packet_cmd_ready = 
    (struct sahara_packet_cmd_ready*)sahara_packet_buffer;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

  // Setup RESET_RESP packet
  packet_cmd_ready->command = SAHARA_CMD_READY_ID;
  packet_cmd_ready->length = sizeof(struct sahara_packet_cmd_ready);

  // Log transmitted packet
  boot_sahara_log_packet_buffer((uint8 *)packet_cmd_ready,
                                packet_cmd_ready->length,
                                SAHARA_PACKET_TRANSMIT);
                                
  // Transmit CMD_READY packet
  tx_length = boot_sahara_handle_tx((uint8 *)packet_cmd_ready,
                                    packet_cmd_ready->length,
                                    &err_code);

  // Check for error in transmission of CMD_READY packet
  if (err_code == BULK_ERR_TIMEOUT)
  {
    // ERROR: Timeout while transmitting data
    status = SAHARA_NAK_TIMEOUT_TX;
  }
  else if (err_code)
  {
    // ERROR: General transmission or reception error
    status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
  }
  else if (tx_length != packet_cmd_ready->length)
  {
    // ERROR: Invalid tranmission length
    status = SAHARA_NAK_INVALID_TX_LENGTH;
  }

  if (status != SAHARA_STATUS_SUCCESS)
  {
    // If transmission error, send END_IMAGE_TX with error
    boot_sahara_handle_end_image_tx(status);
  }
  else
  {
    // Wait for CMD_EXEC packets
    sahara_state = SAHARA_STATE_WAIT_CMD_EXEC;
  }
}

/* ============================================================================
**  Function : boot_sahara_handle_cmd_switch_mode
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for CMD_SWITCH_MODE packet.
*   
*   @details
*   This function switches to the given mode specified in the packet
*   if the current state is WAIT_CMD_EXEC. The target then enters
*   WAIT_HELLO_RESP state and returns to process_packets. At this point
*   the target will send a HELLO packet to the host -- see implementation
*   details of process_packets.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_cmd_switch_mode
(
  uint8* packet_buffer,
  uint32 length
)
{
  struct sahara_packet_cmd_switch_mode* packet_cmd_switch_mode = 
    (struct sahara_packet_cmd_switch_mode*)packet_buffer;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

    if ((length != sizeof(struct sahara_packet_cmd_switch_mode)) ||
        (length != packet_cmd_switch_mode->length))
    {
      // ERROR: Invalid packet size received
      status = SAHARA_NAK_INVALID_PACKET_SIZE;

      // Send END_IMAGE_TX with error
      boot_sahara_handle_end_image_tx(status);
    }
    else
    {
      // Set mode
      sahara_mode = (enum boot_sahara_mode) packet_cmd_switch_mode->mode;

      // process_packets will automatically send HELLO -- see implementation
      sahara_state = SAHARA_STATE_WAIT_HELLO_RESP;
    }
  }

/* ============================================================================
**  Function : boot_sahara_handle_cmd_exec
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for CMD_EXEC packet.
*   
*   @details
*   This function parses out the client_command to execute from the packet
*   and executes the given command via the dispatch table exec_cmd API.
*   The response buffer is filled by this call and then sent to the host.
*   Host should be aware of the expected response length for a given
*   command since this information is not transmitted by the target.
*   If there is a mismatch of response length, the host should handle it
*   appropriately.
*   
*   @param *packet_buffer   -  [IN/OUT] Buffer that contains the packet
*   @param length           -  [IN    ] Length of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_cmd_exec
(
  uint8* packet_buffer,
  uint32 length
)
{
  struct sahara_packet_cmd_exec* packet_cmd_exec = 
    (struct sahara_packet_cmd_exec*)packet_buffer;

  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  
    if ((length != sizeof(struct sahara_packet_cmd_exec)) ||
        (length != packet_cmd_exec->length))
    {
      // ERROR: Invalid packet size received
      status = SAHARA_NAK_INVALID_PACKET_SIZE;
      boot_sahara_handle_end_image_tx(status);
    }
    else
    {
      // Execute command
      sahara_cmd_exec_data = sahara_dispatch_tbl->exec_cmd(
                 packet_cmd_exec->client_command,
                 &sahara_cmd_exec_data_length,
                 (uint32*)&status);

      sahara_cmd_exec_client_command = packet_cmd_exec->client_command;

      if (status != SAHARA_STATUS_SUCCESS)
      {
        // If transmission error, send END_IMAGE_TX with error
        boot_sahara_handle_end_image_tx(status);
      }
      else
      {
        // Send CMD_EXEC_RESP with corresponding response length
        boot_sahara_handle_cmd_exec_resp();
      }
    }
  }

/* ============================================================================
**  Function : boot_sahara_handle_cmd_exec_resp
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for CMD_EXEC_RESP packet.
*   
*   @details
*   This function sends a CMD_EXEC_RESP packet to the host based on the
*   executed client_command. The response length field is set based on the
*   actual command executed. If the response length is > 0, then the target
*   will expect a CMD_EXEC_DATA packet from the host before sending out the
*   response data.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_cmd_exec_resp( void )
{
  // error code returned from raw data read
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;

  uint32 tx_length;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  struct sahara_packet_cmd_exec_resp* packet_cmd_exec_resp = 
    (struct sahara_packet_cmd_exec_resp*)sahara_packet_buffer;
  packet_cmd_exec_resp->command = SAHARA_CMD_EXEC_RESP_ID;
  packet_cmd_exec_resp->length = sizeof(struct sahara_packet_cmd_exec_resp);
  packet_cmd_exec_resp->client_command = sahara_cmd_exec_client_command;
  packet_cmd_exec_resp->resp_length = sahara_cmd_exec_data_length;

  // Log transmitted packet
  boot_sahara_log_packet_buffer((uint8 *)packet_cmd_exec_resp,
                                packet_cmd_exec_resp->length,
                                SAHARA_PACKET_TRANSMIT);
                                
  tx_length = boot_sahara_handle_tx((uint8 *)packet_cmd_exec_resp,
                                    packet_cmd_exec_resp->length,
                                    &err_code);

  // Check for error in transmission of CMD_EXEC_RESP packet
  if (err_code == BULK_ERR_TIMEOUT)
  {
    // ERROR: Timeout while transmitting data
    status = SAHARA_NAK_TIMEOUT_TX;
  }
  else if (err_code)
  {
    // ERROR: General transmission or reception error
    status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
  }
  else if (tx_length != packet_cmd_exec_resp->length)
  {
    // ERROR: Invalid tranmission length
    status = SAHARA_NAK_INVALID_TX_LENGTH;
  }

  if (status != SAHARA_STATUS_SUCCESS)
  {
    // If transmission error, send END_IMAGE_TX with error
    boot_sahara_handle_end_image_tx(status);
  }
  else if (sahara_cmd_exec_data_length > 0)
  {
    // Wait for CMD_EXEC_DATA packet before sending response data
    sahara_state = SAHARA_STATE_WAIT_CMD_EXEC_DATA;
  }
}

/* ============================================================================
**  Function : boot_sahara_handle_cmd_exec_data
** ============================================================================
*/
/*!
*   @brief
*   Packet handler for CMD_EXEC_DATA packet.
*   
*   @details
*   This function sends the RAW_DATA response for the last executed command.
*   Once complete, the target will then transition to a state to wait for
*   another command.
*   
*   @param *packet_buffer   -  [IN/OUT] Param_description_goes_here
*   @param length           -  [IN    ] Param_description_goes_here
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_handle_cmd_exec_data
(
  uint8* packet_buffer,
  uint32 length
)
{
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  uint32 tx_length;
  struct sahara_packet_cmd_exec_data* packet_cmd_exec_data = 
    (struct sahara_packet_cmd_exec_data*)packet_buffer;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;

    if ((length != sizeof(struct sahara_packet_cmd_exec_data)) ||
        (length != packet_cmd_exec_data->length))
    {
      // ERROR: Invalid packet size received
      status = SAHARA_NAK_INVALID_PACKET_SIZE;
      boot_sahara_handle_end_image_tx(status);
    }
    else if (packet_cmd_exec_data->client_command != 
             sahara_cmd_exec_client_command)
    {
      // ERROR: Invalid client command received for data response
      status = SAHARA_NAK_EXEC_DATA_INVALID_CLIENT_CMD;
      boot_sahara_handle_end_image_tx(status);
    }
    else
    {
      // Log transmitted packet
      boot_sahara_log_packet_buffer((uint8*) sahara_cmd_exec_data,
                                    sahara_cmd_exec_data_length,
                                    SAHARA_PACKET_TRANSMIT); 
                                
      // Transmit command execution response data
      tx_length = boot_sahara_handle_tx((uint8*) sahara_cmd_exec_data,
                                        sahara_cmd_exec_data_length,
                                        &err_code);

      // Check for error in transmission of response data
      if (err_code == BULK_ERR_TIMEOUT)
      {
        // ERROR: Timeout while transmitting data
        status = SAHARA_NAK_TIMEOUT_TX;
      }
      else if (err_code)
      {
        // ERROR: General transmission or reception error
        status = SAHARA_NAK_GENERAL_TX_RX_ERROR;
      }
      else if (tx_length != sahara_cmd_exec_data_length)
      {
        // ERROR: Invalid tranmission length
        status = SAHARA_NAK_INVALID_TX_LENGTH;
      }

      if (status != SAHARA_STATUS_SUCCESS)
      {
        // If transmission error, send END_IMAGE_TX with error
        boot_sahara_handle_end_image_tx(status);
      }
      else
      {
        // Wait to receive another command
        sahara_state = SAHARA_STATE_WAIT_CMD_EXEC;
      }
    }
  }

/* ============================================================================
**  Function : boot_sahara_process_packets
** ============================================================================
*/
/*!
*   @brief
*   This function processes incoming Sahara command packets and calls the
*   corresponding packet handler
*   
*   @details
*   This function continually loops until a DONE packet is received and a
*   DONE_RESP is successfully sent to the host. Incoming packets will
*   continually be processed and forwarded to the appropriate packet handler.
*   Each handler will appropriately process the packet, respond, and update
*   the internal state machine. If the incoming packet is unnrecognized, an
*   END_IMAGE_TX will be sent with the corresponding error code.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_process_packets( void )
{
  uint32 length;

  // error code returned from raw data read
  enum boot_sahara_bulk_error err_code = BULK_SUCCESS;
  boolean sahara_valid_state;
  while (!sahara_done)
  {
    // Set state as invalid 
    sahara_valid_state = FALSE;
    
    length = boot_sahara_handle_rx(
              (uint8 *)sahara_packet_buffer, 
              SAHARA_MAX_PACKET_SIZE_IN_BYTES,
              SAHARA_MAX_PACKET_SIZE_IN_BYTES,
              &err_code,
              SAHARA_PACKET_TYPE_COMMAND /* Command Packet*/);

    if (length > 0)
    {
      // Log incoming command packet
      boot_sahara_log_packet_buffer((uint8 *)sahara_packet_buffer,
                                    length,
                                    SAHARA_PACKET_RECEIVE);

      // Decode command packet
      switch(sahara_packet_buffer[0])
      {
        case SAHARA_HELLO_RESP_ID:
          if (sahara_state == SAHARA_STATE_WAIT_HELLO_RESP)
          {
            boot_sahara_handle_hello_resp(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;
          }
          break;
          
        case SAHARA_DONE_ID:
          if (sahara_state == SAHARA_STATE_WAIT_DONE)
          {
            boot_sahara_handle_done(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;          
          }
          break;
          
        case SAHARA_RESET_ID:
          if ((sahara_state == SAHARA_STATE_WAIT_HELLO_RESP) ||
              (sahara_state == SAHARA_STATE_WAIT_RESET) ||
              (sahara_state == SAHARA_STATE_WAIT_MEMORY_READ) ||
              (sahara_state == SAHARA_STATE_WAIT_CMD_EXEC))
          {
            boot_sahara_handle_reset(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;            
          }
          break;
          
        case SAHARA_MEMORY_READ_ID:
          if (sahara_state == SAHARA_STATE_WAIT_MEMORY_READ)
          {
            boot_sahara_handle_memory_read(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;
          }
          break;
          
        case SAHARA_MEMORY_READ_64BITS_ID:
          if (sahara_state == SAHARA_STATE_WAIT_MEMORY_READ)
          {
            boot_sahara_handle_memory_64bits_read(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;
          }
          break;                 
          
        case SAHARA_CMD_SWITCH_MODE_ID:
          if ((sahara_state == SAHARA_STATE_WAIT_CMD_EXEC) || 
              (sahara_state == SAHARA_STATE_WAIT_MEMORY_READ))
          {
            boot_sahara_handle_cmd_switch_mode(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;
          }
          break;
          
        case SAHARA_CMD_EXEC_ID:
          if (sahara_state == SAHARA_STATE_WAIT_CMD_EXEC)        
          {
            boot_sahara_handle_cmd_exec(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;
          }    
          break;
          
        case SAHARA_CMD_EXEC_DATA_ID:
          if (sahara_state == SAHARA_STATE_WAIT_CMD_EXEC_DATA)    
          {
            boot_sahara_handle_cmd_exec_data(sahara_packet_buffer, length);
            sahara_valid_state = TRUE;
          }
          break;
          
        default:
          break;
      }
      
      /* If sahara is not waiting for hello response and 
       it receives invalid command, send NAK */
      if (sahara_valid_state == FALSE && 
          sahara_state != SAHARA_STATE_WAIT_HELLO_RESP)
      {
        boot_sahara_handle_end_image_tx(SAHARA_NAK_INVALID_CMD);      
      }
    }
    
    /* If sahara is waiting for hello response and 
       it receives invalid command, resend hello */    
    if (sahara_state == SAHARA_STATE_WAIT_HELLO_RESP)
    {
      boot_sahara_handle_hello();
    }
  }
  // Exit out of Sahara
  sahara_state = SAHARA_STATE_ENTRY;
  return;
}

/* ============================================================================
**  Function : boot_sahara_init
** ============================================================================
*/
/*!
*   @brief
*   This function initializes the state for the Sahara protocol
*   
*   @details
*   This function performs basic initialization in order to transfer an image.
*   Based on the image type, the current image in shared data will be updated.
*   This function also initializes the internal state machine and hardware
*   driver.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  TRUE if initialization was successful; FALSE otherwise
*   
*   @sa None
*/
static boolean boot_sahara_init ( void )
{
  boolean status = TRUE;

  do
  {
    // Check parameters
    if (sahara_if == NULL)
    {
      status = FALSE;
      break;
    }
    if ((sahara_if->sahara_shared_data == NULL) ||
        (sahara_if->dispatch_tbl == NULL))
    {
      status = FALSE;
      break;
    }

    sahara_shared_data = sahara_if->sahara_shared_data;
    sahara_dispatch_tbl = sahara_if->dispatch_tbl;

    if (sahara_if->sahara_shared_data->cur_image == NULL)
    {
      status = FALSE;
      break;
    }

    // Verify dispatch table
    if ((sahara_dispatch_tbl->init == NULL) ||
        (sahara_dispatch_tbl->shutdown == NULL) ||
        (sahara_dispatch_tbl->poll == NULL) ||
        (sahara_dispatch_tbl->rx_bulk == NULL) ||
        (sahara_dispatch_tbl->tx_bulk == NULL) ||
        (sahara_dispatch_tbl->shutdown == NULL) ||
        (sahara_dispatch_tbl->get_max_packet_size == NULL) ||
        (sahara_dispatch_tbl->reset == NULL) ||
        (sahara_dispatch_tbl->valid_address_range == NULL) ||
        (sahara_dispatch_tbl->exec_cmd == NULL) ||
        (sahara_dispatch_tbl->get_max_raw_data_size == NULL) ||
        (sahara_dispatch_tbl->auth_enabled == NULL) ||
        (sahara_dispatch_tbl->memcpy == NULL) ||
        (sahara_dispatch_tbl->memset == NULL) ||
        (sahara_dispatch_tbl->get_bin_header_size == NULL) ||
        (sahara_dispatch_tbl->get_bin_image_size == NULL) ||
        (sahara_dispatch_tbl->get_bin_code_size == NULL) ||
        (sahara_dispatch_tbl->get_bin_cert_chain_size == NULL) ||
        (sahara_dispatch_tbl->get_bin_signature_size == NULL) ||
        (sahara_dispatch_tbl->get_hash_segment_buffer == NULL))
    {
      status = FALSE;
      break;
    }

    // Determine if image should be authenticated
    sahara_shared_data->is_secure = sahara_dispatch_tbl->auth_enabled();
    sahara_auth_tbl = sahara_if->auth_tbl;
   if (((sahara_auth_tbl == NULL) ||
        (sahara_auth_tbl->authenticate == NULL)) && 
        (sahara_if->sahara_mode != SAHARA_MODE_MEMORY_DEBUG))

    {
      status = FALSE;
      break;
    }
    else if (sahara_shared_data->is_secure &&
        (sahara_shared_data->expected_image_type == SAHARA_IMAGE_TYPE_ELF))
    {
        sahara_auth_elf = TRUE;
    }
    else if (sahara_shared_data->is_secure &&
            (sahara_shared_data->expected_image_type == SAHARA_IMAGE_TYPE_BINARY))
    {
        sahara_auth_bin = TRUE;
    }

    sahara_mem_debug_tbl = sahara_if->mem_debug_tbl;
    if (sahara_mem_debug_tbl == NULL)
    {
      sahara_mem_debug_enabled = FALSE;
    }
    else
    {
      if ((sahara_mem_debug_tbl->mem_debug_supported == NULL) ||
          (sahara_mem_debug_tbl->mem_debug_init == NULL) ||
          (sahara_mem_debug_tbl->mem_debug_verify_addr == NULL) ||
          (sahara_mem_debug_tbl->mem_debug_table_addr == NULL) ||
          (sahara_mem_debug_tbl->mem_debug_table_len == NULL) ||
          (sahara_mem_debug_tbl->mem_debug_is_restricted_addr == NULL) ||
          (sahara_mem_debug_tbl->mem_debug_copy_restricted == NULL))
      {
        status = FALSE;
        break;
      }
      else
      {
        sahara_mem_debug_enabled = TRUE;
      }
    }

    if (sahara_shared_data->expected_image_type == SAHARA_IMAGE_TYPE_BINARY)
    {
      sahara_binary_image_info = 
        (struct boot_sahara_binary_image_info*)sahara_shared_data->cur_image;

      // Initialize binary image info
      sahara_binary_image_info->image_id = 0;
      // Check the binary image header has been initialized to a valid address
      if (sahara_binary_image_info->header == NULL)
      {
        status = FALSE;
        break;
      }
      sahara_dispatch_tbl->memset(sahara_binary_image_info->header, 
                                  0, 
                                  sahara_dispatch_tbl->get_bin_header_size());
    }
    else if (sahara_shared_data->expected_image_type == SAHARA_IMAGE_TYPE_ELF)
    {
      sahara_elf_image_info = 
        (struct boot_sahara_elf_image_info*)sahara_shared_data->cur_image;
      // Check program headers buffer has been initialized to a valid address
      if (sahara_elf_image_info->prog_headers == NULL)
      {
        status = FALSE;
        break;
      }

      // Initialize ELF image info
      sahara_elf_image_info->image_id = 0;
      sahara_dispatch_tbl->memset(&(sahara_elf_image_info->elf_header), 
                                  0,
                                  sizeof(Elf32_Ehdr));
      sahara_elf_image_info->hash_table = NULL;
      sahara_dispatch_tbl->memset(&(sahara_elf_image_info->shared_seg_index), 
                                  0,
                                  sizeof(uint32));
      sahara_dispatch_tbl->memset(&(sahara_elf_image_info->entry_address), 
                                  0,
                                  sizeof(uint32));
      /* zero initialize progressive_boot_block up to max possible 
	     prog-header entries */ 
      sahara_dispatch_tbl->memset((sahara_elf_image_info->prog_headers),
                                  0,
                                  sizeof(struct progressive_boot_block));
    }
    else if (sahara_if->sahara_mode != SAHARA_MODE_MEMORY_DEBUG)
    {
      // Image type not supported and not in memory debug mode
      status = FALSE;
      break;
    }

    // Initialize Sahara state information
    sahara_if->sahara_shared_data->image_rx_complete = FALSE;
    sahara_mode = (enum boot_sahara_mode) sahara_if->sahara_mode;
    sahara_state = SAHARA_STATE_ENTRY;
    sahara_done = FALSE;
  
    // Initialize hardware driver interface
    if(BULK_SUCCESS != sahara_dispatch_tbl->init())
    {
      status = FALSE;
        break;
    }
  } while (0);

  if(NULL != sahara_if)
  {
    if (status == FALSE)
    {
      // Report error in initialize to Sahara client
      sahara_if->sahara_status = SAHARA_NAK_TARGET_INIT_FAILURE;
    }
    else
    {
      // Initialize sahara status
      sahara_if->sahara_status = SAHARA_STATUS_SUCCESS;
    }
  }

  return status;
}

/* ============================================================================
**  Function : boot_sahara_reset
** ============================================================================
*/
/*!
*   @brief
*   Shutdown Sahara protocol and driver, and reset the target.
*   
*   @details
*   Shutdown Sahara protocol and driver, and reset the target.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void boot_sahara_reset ( void )
{


  void (*sahara_reset)(void) = sahara_dispatch_tbl->reset;
  
  // Shutdown Sahara
  boot_sahara_exit();

  // Reset target
  sahara_reset();

  // Reset should not return. Make sure this would be in while 1 loop 
  // if it returns 
  INFINITE_LOOP();
}


/*===========================================================================
**  Function :  boot_sahara_populate_elf_meta_info
** ==========================================================================
*/
/*!
* 
* @brief
*  This function will populate a buffer with the meta data for the current
*  elf image being processed. Meta data format:
*  elf header followed by program header table followed by
*  hash segment. It also sets pmi image start address.
*
* @param
*  int32 hash_table_index  index of hash segment entry in prog header
*
* @par Dependencies
*  Must be called only after elf header and program header is loaded.
*  If the buffer lies in DDR, api must be called after DDR init.
*
* @retval returns status of image transfer
*
*/
static enum boot_sahara_status boot_sahara_populate_elf_meta_info
(
  int32 hash_table_index
)
{
  uint8 *meta_buff_ptr = NULL;
  // Temporary variables to dereference pointers
  struct progressive_boot_block* prog_headers = sahara_elf_image_info->prog_headers;
  Elf32_Ehdr* elf_header = &(sahara_elf_image_info->elf_header);
  // Setup pointer to program header array
  struct segment_record* sahara_elf_phdrs = prog_headers->segment;
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS; // status of transfer

  do
  {
    if(sahara_ssa_enabled != TRUE)
    { 
      // Proceed only if subsytem expects the meta data
      break;
    }
    // Get pointer to meta buffer
    meta_buff_ptr = boot_rmb_get_meta_info_buffer();

    if (NULL == meta_buff_ptr)
    {
      // ERROR: meta buffer address not correct.
      status = SAHARA_NAK_INVALID_DEST_ADDR;
      break;
    }
    // Copy elf header to buffer 
    sahara_dispatch_tbl->memcpy(meta_buff_ptr,
                                elf_header,
                                sizeof(Elf32_Ehdr));

    // Update buffer to point to next free location 
    meta_buff_ptr = meta_buff_ptr + sizeof(Elf32_Ehdr);
    
    if (prog_headers == NULL)
    {
      // ERROR: Uninitialized program header location
      status = SAHARA_NAK_UNINIT_PHDR_LOC;
      break;
    }
    // Copy program header table to buffer
    sahara_dispatch_tbl->memcpy(meta_buff_ptr,
                               &(prog_headers->segment[0]),
                   (elf_header->e_phnum * elf_header->e_phentsize));

    // Update buffer to point to next free location
    meta_buff_ptr = meta_buff_ptr + 
                    (elf_header->e_phnum * elf_header->e_phentsize);

    // Ensure hash table has been found and loaded by now.
    if (hash_table_index < 0)
    {
      // ERROR: hash table not found / not loaded.
      status = SAHARA_NAK_HASH_TABLE_NOT_FOUND;
      break;
    }
    // Copy hash segment to buffer
    sahara_dispatch_tbl->memcpy(meta_buff_ptr,
                    (const void *)sahara_dispatch_tbl->get_hash_segment_buffer(),
                    sahara_elf_phdrs[hash_table_index].size);

    // Signal subsystem authenticator image to retrieve meta info buffer
    boot_rmb_populate_meta_info();
    
    /* Set the image start address. It is assumed the elf segments are in
      increasing order and first segment start address is lowest address
    */
    boot_rmb_set_pmi_code_start_addr((uint32 *)(uintnt)elf_header->e_entry);
    
    /*Signal MBA image to start processing PMI elf segments and
      jump to elf
    */
    boot_rmb_set_pmi_elf_load_ready();
    
    // Enable flag to update loaded data length being communicated
    // to subsystem (mpss) via rmb
    sahara_ssa_is_rx_for_elf_segments = TRUE;
    
    }while(0);
  return status;
}


/* ============================================================================
**  Function : boot_sahara_entry
** ============================================================================
*/
/*!
*   @brief
*   This function provides an entry into the Sahara protocol and jumps into
*   the protocol.
*   
*   @details
*   This function initializes the Sahara protocol followed by sending a HELLO
*   packet to the host. After that, the target will proceed to wait for
*   incoming packets and handle them appropriately.
*   
*   @param sahara_interface      -  [IN/OUT] Interface to be used by protocol
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   boot_sahara_init() will be called which calls the dispatch table's init()
*   routine - this is intended to initialize any hardware that is required
*   for the protocol to execute.
*   
*   @retval  Boolean
*   
*   @sa None
*/
boolean boot_sahara_entry
(
  struct boot_sahara_interface* sahara_interface
)
{
  boolean status = TRUE;

  sahara_if = sahara_interface;

  // Validate interface and initialize Sahara protocol if not previously executed
  status = boot_sahara_init();

  if (status)
  {
    // Send hello packet
    boot_sahara_handle_hello();
    
    // Enter Sahara packet handler
    boot_sahara_process_packets();
    
    // Reset authentication information
    sahara_auth_tbl = NULL;
    sahara_auth_elf = FALSE;
    sahara_auth_bin = FALSE;
  }
  
  return status;
}

/*===========================================================================
**  Function :  boot_sahara_set_is_ssa_enabled
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set/reset Flag to Turn on/Turn off Subsystem
*   Self authentication mode while loading elf images.
*
* @param[in] 
*  boolean subsys_self_auth_value  - TRUE if Subsystem Self auth mode to be
*                                  turned on else FALSE
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*   None
* 
*/
void boot_sahara_set_is_ssa_enabled
( 
  boolean subsys_self_auth_value
)
{
  sahara_ssa_enabled = subsys_self_auth_value;
}

/*===========================================================================
**  Function :  boot_sahara_ssa_update_rmb_length
** ==========================================================================
*/
/*!
* 
* @brief
*   This function adds the input length to existing length value being
*   communicated via rmb to subsystem (eg mba running on mpss)
*
* @param[in] 
*  uint32 length  - Length to be added to running total being communicated
*                   via rmb registers
*  enum boot_ssa_length_update_state - Indicates whether the api should queue
*                                      length update or flush stored value,
*                                      or immediately write input length
*                                      to rmb registers.
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*   None
* 
*/
void boot_sahara_ssa_update_rmb_length 
( 
  uint32 length, 
  enum boot_ssa_length_update_state ssa_state
)
{
  static uint32 delayed_length_update = 0;
  uint32 update_val = 0;
  
  if(sahara_ssa_is_rx_for_elf_segments == TRUE)
  {
    if( ssa_state == SSA_LENGTH_QUEUE)
    {
      update_val = delayed_length_update;
      delayed_length_update = length;
    }
    else if( ssa_state == SSA_LENGTH_NO_QUEUE)
    {
      update_val = length;
    }
    else if( ssa_state == SSA_LENGTH_FLUSH)
    {
      update_val = delayed_length_update;
      delayed_length_update = 0x0;
    }

    sahara_elf_image_info->total_elf_segments_size += update_val;
    /*Set total length of loaded segments */
    boot_rmb_set_pmi_code_length( 
          (uint32)sahara_elf_image_info->total_elf_segments_size );
  }
}
