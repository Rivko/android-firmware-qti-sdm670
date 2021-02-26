#ifndef BOOT_SAHARA_H
#define BOOT_SAHARA_H

/*=============================================================================

                               Boot Sahara
                               Header File
GENERAL DESCRIPTION
  This file provides the APIs to enter the Sahara Protocol.
  
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
03/03/17   ds	   remove clobber prot
10/29/14   kpa     Support 64 bit memory map
08/08/14   ck      Updated to support 64 bit
07/24/14   ck      Added SAHARA_NAK_INVALID_IMG_HASH_TABLE_SIZE
12/09/13   jz      Added include file boot_elf_header.h
11/12/13   kedara  Added skip_image_load.
10/16/13   jz      Added sahara_packet_memory_64bits_debug
10/15/13   kedara  Support PICe Flashless boot.
04/16/13   kedara  Added image_load_post_proc.
01/18/13   dh      Add unrecoverable_error_handler to dispatch table.
                   Increment SAHARA_INTERFACE_DISPATCH_VERSION to 0x10001
11/06/12   dh      Add BULK_ERR_CABLE_DISCONNECT error type
09/20/12   kedara  Added function to obtain the image src field from the
                   image header.
                   Added authentication error code.
08/21/12   kedara  Added support for subsystem self authentication/
                   load elf in chunks
06/27/12   kedara  Updated boot_sahara_entry return type
06/08/12   as      Removed sahara definitions and moved in the min files to
                   to make it flexible in case there are changes in the parameters.
06/05/12   tnk     Support of SBL antiroll back fuse read command in Sahara
03/28/11   niting  Added binary image header accessor functions.
03/22/11   niting  Updated Sahara interface, deprecated boot_sahara_packet.h
                   and boot_sahara_bulk.h
01/06/11   aus     Added a parameter to distinguish between command and data packets
09/15/10   niting  Abstracted image_type from Sahara. 
09/15/10   niting  Added parallel authentication and load support.
08/12/10   niting  Moved max raw data size to Sahara client and dispatch table.
07/27/10   niting  Added command mode. 
07/27/10   niting  Added valid_address_range to dispatch table to verify 
                   writeable addresses. 
07/26/10   niting  Changed return type of bulk_init to uint32.
06/07/10   niting  Change image_tx_status to mode.
04/30/10   niting  Changed driver max packet size to read from API.
04/21/10   niting  Added support for ELF in Sahara.
04/12/10   niting  Cleanup
03/17/10   niting  Initial version of Sahara protocol.

=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "MIprogressive.h"
#include "boot_elf_header.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Sahara Protocol Version */
#define SAHARA_VERSION_MAJOR 2
#define SAHARA_VERSION_MAJOR_SUPPORTED 1
#define SAHARA_VERSION_MINOR 5

/* Sahara Interface Version */
#define SAHARA_INTERFACE_VERSION 0x00010000
#define SAHARA_INTERFACE_DISPATCH_VERSION  0x00010002
#define SAHARA_INTERFACE_AUTH_VERSION      0x00010000
#define SAHARA_INTERFACE_MEMDEBUG_VERSION  0x00010000

/* Sahara V1 HELLO and HELLO_RESP packet lengths in bytes */
#define SAHARA_PACKET_HELLO_LENGTH_V1      0x14
#define SAHARA_PACKET_HELLO_RESP_LENGTH_V1 0x14

/* Sahara command IDs */
enum boot_sahara_cmd_id
{
  SAHARA_NO_CMD_ID          = 0x00,
  SAHARA_HELLO_ID           = 0x01, /* sent from target to host */
  SAHARA_HELLO_RESP_ID      = 0x02, /* sent from host to target */
  SAHARA_READ_DATA_ID       = 0x03, /* sent from target to host */
  SAHARA_END_IMAGE_TX_ID    = 0x04, /* sent from target to host */
  SAHARA_DONE_ID            = 0x05, /* sent from host to target */
  SAHARA_DONE_RESP_ID       = 0x06, /* sent from target to host */
  SAHARA_RESET_ID           = 0x07, /* sent from host to target */
  SAHARA_RESET_RESP_ID      = 0x08, /* sent from target to host */
  SAHARA_MEMORY_DEBUG_ID    = 0x09, /* sent from target to host */
  SAHARA_MEMORY_READ_ID     = 0x0A, /* sent from host to target */
  SAHARA_CMD_READY_ID       = 0x0B, /* sent from target to host */
  SAHARA_CMD_SWITCH_MODE_ID = 0x0C, /* sent from host to target */
  SAHARA_CMD_EXEC_ID        = 0x0D, /* sent from host to target */
  SAHARA_CMD_EXEC_RESP_ID   = 0x0E, /* sent from target to host */
  SAHARA_CMD_EXEC_DATA_ID   = 0x0F, /* sent from host to target */
  SAHARA_MEMORY_DEBUG_64BITS_ID = 0x10, /* sent from target to host */
  SAHARA_MEMORY_READ_64BITS_ID  = 0x11, /* sent from host to target */
  
  /* place all new commands above this */
  SAHARA_LAST_CMD_ID,
  SAHARA_MAX_CMD_ID             = 0x7FFFFFFF /* To ensure 32-bits wide */
};

/* Status codes for Sahara */
enum boot_sahara_status
{
  /* Success */
  SAHARA_STATUS_SUCCESS =                     0x00,

  /* Invalid command received in current state */
  SAHARA_NAK_INVALID_CMD =                    0x01,

  /* Protocol mismatch between host and target */
  SAHARA_NAK_PROTOCOL_MISMATCH =              0x02,

  /* Invalid target protocol version */
  SAHARA_NAK_INVALID_TARGET_PROTOCOL =        0x03,

  /* Invalid host protocol version */
  SAHARA_NAK_INVALID_HOST_PROTOCOL =          0x04,

  /* Invalid packet size received */
  SAHARA_NAK_INVALID_PACKET_SIZE =            0x05,

  /* Unexpected image ID received */
  SAHARA_NAK_UNEXPECTED_IMAGE_ID =            0x06,

  /* Invalid image header size received */
  SAHARA_NAK_INVALID_HEADER_SIZE =            0x07,

  /* Invalid image data size received */
  SAHARA_NAK_INVALID_DATA_SIZE =              0x08,

  /* Invalid image type received */
  SAHARA_NAK_INVALID_IMAGE_TYPE =             0x09,

  /* Invalid tranmission length */
  SAHARA_NAK_INVALID_TX_LENGTH =              0x0A,

  /* Invalid reception length */
  SAHARA_NAK_INVALID_RX_LENGTH =              0x0B,

  /* General transmission or reception error */
  SAHARA_NAK_GENERAL_TX_RX_ERROR =            0x0C,

  /* Error while transmitting READ_DATA packet */
  SAHARA_NAK_READ_DATA_ERROR =                0x0D,

  /* Cannot receive specified number of program headers */
  SAHARA_NAK_UNSUPPORTED_NUM_PHDRS =          0x0E,

  /* Invalid data length received for program headers */
  SAHARA_NAK_INVALID_PDHR_SIZE =              0x0F,

  /* Multiple shared segments found in ELF image */
  SAHARA_NAK_MULTIPLE_SHARED_SEG =            0x10,

  /* Uninitialized program header location */
  SAHARA_NAK_UNINIT_PHDR_LOC =                0x11,

  /* Invalid destination address */
  SAHARA_NAK_INVALID_DEST_ADDR =              0x12,

  /* Invalid data size receieved in image header */
  SAHARA_NAK_INVALID_IMG_HDR_DATA_SIZE =      0x13,

  /* Invalid ELF header received */
  SAHARA_NAK_INVALID_ELF_HDR =                0x14,

  /* Unknown host error received in HELLO_RESP */
  SAHARA_NAK_UNKNOWN_HOST_ERROR =             0x15,

  /* Timeout while receiving data */
  SAHARA_NAK_TIMEOUT_RX =                     0x16,

  /* Timeout while transmitting data */
  SAHARA_NAK_TIMEOUT_TX =                     0x17,

  /* Invalid mode received from host */
  SAHARA_NAK_INVALID_HOST_MODE =              0x18,

  /* Invalid memory read access */
  SAHARA_NAK_INVALID_MEMORY_READ =            0x19,

  /* Host cannot handle read data size requested */
  SAHARA_NAK_INVALID_DATA_SIZE_REQUEST =      0x1A,

  /* Memory debug not supported */
  SAHARA_NAK_MEMORY_DEBUG_NOT_SUPPORTED =     0x1B,

  /* Invalid mode switch */
  SAHARA_NAK_INVALID_MODE_SWITCH =            0x1C,

  /* Failed to execute command */
  SAHARA_NAK_CMD_EXEC_FAILURE =               0x1D,

  /* Invalid parameter passed to command execution */
  SAHARA_NAK_EXEC_CMD_INVALID_PARAM =         0x1E,

  /* Unsupported client command received */
  SAHARA_NAK_EXEC_CMD_UNSUPPORTED =           0x1F,

  /* Invalid client command received for data response */
  SAHARA_NAK_EXEC_DATA_INVALID_CLIENT_CMD =   0x20,

  /* Failed to authenticate hash table */
  SAHARA_NAK_HASH_TABLE_AUTH_FAILURE =        0x21,

  /* Failed to verify hash for a given segment of ELF image */
  SAHARA_NAK_HASH_VERIFICATION_FAILURE =      0x22,

  /* Failed to find hash table in ELF image */
  SAHARA_NAK_HASH_TABLE_NOT_FOUND =           0x23,

  /* Target failed to initialize */
  SAHARA_NAK_TARGET_INIT_FAILURE =            0x24,

  /* Failed to authenticate generic image */
  SAHARA_NAK_IMAGE_AUTH_FAILURE  =            0x25,  

  /* Invalid ELF hash table size.  Too bit or small. */
  SAHARA_NAK_INVALID_IMG_HASH_TABLE_SIZE =    0x26,

  /* Place all new error codes above this */
  SAHARA_NAK_LAST_CODE,

  SAHARA_NAK_MAX_CODE = 0x7FFFFFFF /* To ensure 32-bits wide */
};

/* Status of all image transfers */
enum boot_sahara_mode
{
  SAHARA_MODE_IMAGE_TX_PENDING  = 0x0,
  SAHARA_MODE_IMAGE_TX_COMPLETE = 0x1,
  SAHARA_MODE_MEMORY_DEBUG      = 0x2,
  SAHARA_MODE_COMMAND           = 0x3,

  /* place all new commands above this */
  SAHARA_MODE_LAST,
  SAHARA_MODE_MAX = 0x7FFFFFFF
};

/* Executable commands when target is in command mode */
enum boot_sahara_exec_cmd_id
{
  SAHARA_EXEC_CMD_NOP                    = 0x00,
  SAHARA_EXEC_CMD_SERIAL_NUM_READ        = 0x01,
  SAHARA_EXEC_CMD_MSM_HW_ID_READ         = 0x02,
  SAHARA_EXEC_CMD_OEM_PK_HASH_READ       = 0x03,
  SAHARA_EXEC_CMD_SWITCH_TO_DMSS_DLOAD   = 0x04,
  SAHARA_EXEC_CMD_SWITCH_TO_STREAM_DLOAD = 0x05,  
  SAHARA_EXEC_CMD_READ_DEBUG_DATA          = 0x06,
  SAHARA_EXEC_CMD_GET_SOFTWARE_VERSION_SBL = 0x07,
  
  /* place all new commands above this */
  SAHARA_EXEC_CMD_LAST,
  SAHARA_EXEC_CMD_MAX = 0x7FFFFFFF
};

enum boot_sahara_packet_type
{
  SAHARA_PACKET_TYPE_COMMAND = 0,     /* Command packet */
  SAHARA_PACKET_TYPE_DATA,            /* Data packet */
  SAHARA_PACKET_UNKNOWN = 0x7FFFFFFF  /* To ensure 32-bits wide */
};

enum boot_sahara_image_type
{
  SAHARA_IMAGE_TYPE_BINARY = 0,     /* Binary format */
  SAHARA_IMAGE_TYPE_ELF,            /* ELF format */
  SAHARA_IMAGE_UNKNOWN = 0x7FFFFFFF /* To ensure 32-bits wide */
};

/*=============================================================================
  Sahara protocol packet defintions
=============================================================================*/
/* ***NOTE: all length fields are in bytes */

/* HELLO command packet type - sent from target to host
     indicates start of protocol on target side */
struct sahara_packet_hello
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 version;                 /* target protocol version number */
  uint32 version_supported;       /* minimum protocol version number supported */
                                  /*   on target */
  uint32 cmd_packet_length;       /* maximum packet size supported for command */
                                  /*   packets */
  uint32 mode;                    /* expected mode of target operation */
  uint32 reserved0;               /* reserved field */
  uint32 reserved1;               /* reserved field */
  uint32 reserved2;               /* reserved field */
  uint32 reserved3;               /* reserved field */
  uint32 reserved4;               /* reserved field */
  uint32 reserved5;               /* reserved field */
};

/* HELLO_RESP command packet type - sent from host to target
     response to hello, protocol version running on host and status sent */
struct sahara_packet_hello_resp
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 version;                 /* host protocol version number */
  uint32 version_supported;       /* minimum protocol version number supported */
                                  /*   on host */
  uint32 status;                  /* OK or error condition */
  uint32 mode;                    /* mode of operation for target to execute */
  uint32 reserved0;               /* reserved field */
  uint32 reserved1;               /* reserved field */
  uint32 reserved2;               /* reserved field */
  uint32 reserved3;               /* reserved field */
  uint32 reserved4;               /* reserved field */
  uint32 reserved5;               /* reserved field */
};

/* READ_DATA command packet type - sent from target to host
     sends data segment offset and length to be read from current host 
     image file */
struct sahara_packet_read_data
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 image_id;                /* ID of image to be transferred */
  uint32 data_offset;             /* offset into image file to read data from */
  uint32 data_length;             /* length of data segment to be retreived */
                                  /*   from image file */
};

/* END_IMAGE_TX command packet type - sent from target to host
     indicates end of a single image transfer and status of transfer */
struct sahara_packet_end_image_tx
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 image_id;                /* ID of image to be transferred */
  uint32 status;                  /* OK or error condition */
};

/* DONE packet type - sent from host to target
     indicates end of single image transfer */
struct sahara_packet_done
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
};

/* DONE_RESP packet type - sent from target to host
     indicates end of all image transfers */
struct sahara_packet_done_resp
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 image_tx_status;         /* indicates if all images have been  */
                                  /*   transferred; */
                                  /*   0 = IMAGE_TX_PENDING */
                                  /*   1 = IMAGE_TX_COMPLETE */
};

/* RESET packet type - sent from host to target
     indicates to target to reset */
struct sahara_packet_reset
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
};

/* RESET_RESP packet type - sent from target to host
     indicates to host that target has reset */
struct sahara_packet_reset_resp
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
};

/* MEMORY_DEBUG packet type - sent from target to host
     sends host the location and length of memory region table */
struct sahara_packet_memory_debug
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 memory_table_addr;       /* location of memory region table */
  uint32 memory_table_length;     /* length of memory table */
};

/* MEMORY_DEBUG packet type - sent from target to host
     sends host the location and length of memory region table */
struct sahara_packet_memory_64bits_debug
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint64 memory_table_addr;       /* location of memory region table */
  uint64 memory_table_length;     /* length of memory table */
};

/* MEMORY_READ packet type - sent from host to target
     sends memory address and length to read from target memory */
struct sahara_packet_memory_read
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 memory_addr;             /* memory location to read from */
  uint32 memory_length;           /* length of data to send */
};

/* SAHARA_MEMORY_READ_64BITS_ID packet type - sent from host to target
     sends 64 bits memory address and length to read from target memory */
struct sahara_packet_memory_64bits_read
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint64 memory_addr;             /* memory location to read from */
  uint64 memory_length;           /* length of data to send */
};


/* CMD_READY packet type - sent from target to host
     indicates to host that target is ready to accept commands */
struct sahara_packet_cmd_ready
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
};

/* CMD_SWITCH_MODE packet type - sent from host to target
     indicates to target to switch modes */
struct sahara_packet_cmd_switch_mode
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 mode;                    /* mode of operation for target to execute */
};

/* CMD_EXEC packet type - sent from host to target
     indicates to target to execute given client_command */
struct sahara_packet_cmd_exec
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 client_command;          /* command ID for target Sahara client  */
                                  /*   execute */
};

/* CMD_EXEC_RESP packet type - sent from target to host
     indicates to host that target has successfully executed command
     and length of data response */
struct sahara_packet_cmd_exec_resp
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 client_command;          /* command ID for target Sahara client to */
                                  /*   execute */
  uint32 resp_length;             /* length of response returned from command */
                                  /*   execution */
};

/* CMD_EXEC_DATA packet type - sent from host to target
     indicates that host is ready to receive data after command execution */
struct sahara_packet_cmd_exec_data
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
  uint32 client_command;          /* command ID for target Sahara client to */
                                  /*   execute */
};

/* COMMAND packet type - indicates a generic command packet */
struct sahara_packet_command
{
  uint32 command;                 /* command ID */
  uint32 length;                  /* packet length incl command and length */
};


/*=============================================================================
  Sahara protocol public defintions
=============================================================================*/

/* read completion callback - currently not used */
typedef void (* sahara_rx_cb_type) (uint32 bytes_read, uint32 err_code);

/* write completion callback - currently not used */
typedef void (* sahara_tx_cb_type) (uint32 bytes_written, uint32 err_code);

/* rx_bulk function definition */
typedef uint32  (*sahara_rx_bulk_type)  (uint8 *rx_buf,
                                         uint32 len,
                                         sahara_rx_cb_type rx_cb,
                                         uint32 *err_code,
                                         enum boot_sahara_packet_type type);

/* tx_bulk function definition */
typedef uint32  (*sahara_tx_bulk_type)  (uint8 *tx_buf,
                                         uint32 len,
                                         sahara_tx_cb_type tx_cb,
                                         uint32 * err_code);

/* Status of hardware bulk transfers */
enum boot_sahara_bulk_error
{
  BULK_SUCCESS               = 0x0,       /* success */
  BULK_ERR_PERM              = 0x1,       /* operation not permitted */
  BULK_ERR_NO_MEM            = 0x2,       /* out of heap memory */
  BULK_ERR_CANCEL            = 0x3,       /* transfer cancelled */
  BULK_ERR_SUSPENDED         = 0x4,       /* device is suspended */
  BULK_ERR_ENUMERATION       = 0x5,       /* enumeration failed */
  BULK_ERR_TIMEOUT           = 0x6,       /* operation timed out */
  BULK_ERR_BAD_PARAM         = 0x7,       /* bad parameter */
  BULK_ERR_UNKNOWN           = 0x8,       /* general error */
  BULK_ERR_READ              = 0x9,       /* read error */
  BULK_ERR_WRITE             = 0xA,       /* write error */
  BULK_ERR_CABLE_DISCONNECT  = 0xB,       /* usb cable disconnect error */
  BULK_LAST_ERROR            = 0x7FFFFFFF /* last error code */
};


/*=============================================================================
  Sahara protocol client defintions
=============================================================================*/
/* NOTE: Unless specified, all length fields/parameters are in bytes */

/* This structure can be used to retrieve information of the last
   image transferred to memory */
struct boot_sahara_binary_image_info{
  /* image ID - set by Sahara */
  uint32 image_id;

  /* image header - pointer must be initialized by caller of
     boot_sahara_entry before entering into Sahara to a valid
     array large enough to hold the expected binary image header */
  uint8* header;
};

struct boot_sahara_elf_image_info{
  /* image type - set by Sahara */
  uint32 image_id;

  /* ELF image header - set by Sahara */
  Elf32_Ehdr elf_header;

  /* Program headers - pointer must be initialized by caller of
     boot_sahara_entry before entering into Sahara */
  struct progressive_boot_block* prog_headers;

  /* Hash table - set by Sahara */
  uint8* hash_table;

  /* Shared segment index in program headers - set by Sahara */
  uint32 shared_seg_index;
  
  /* ELF image physical entry point - set by Sahara */
  uint32 entry_address;

  /* size of prog_headers buffer - must be initialized by caller of
     boot_sahara_entry before entering into Sahara */
  uint32 prog_headers_size;
  
  /*Total size of loaded elf segments - set by Sahara */
  uint32  total_elf_segments_size;
  
};

struct boot_sahara_shared_data{
  /* Expected Image - must be initialized by caller of
     boot_sahara_entry before entering into Sahara */
  uint32 expected_image;

  /* Expected Image Type - must be initialized by caller of
     boot_sahara_entry before entering into Sahara */
  enum boot_sahara_image_type expected_image_type;

  /* Pointer to current image info
     Must of type boot_sahara_binary_image_info* or boot_sahara_elf_image_info*
     Must be initialized by caller of
     boot_sahara_entry before entering into Sahara
   */
  void* cur_image;

  /* Indicates if image transfer is complete - set by Sahara */
  boolean image_rx_complete;

  /* Is secure boot enabled - set by Sahara by calling auth_enabled
                              during initialization */
  boolean is_secure;
};

/* Dispatch table to support H/W driver */
struct boot_sahara_dispatch_tbl
{
  /* Revision number of dispatch table
   *   Should be set to SAHARA_INTERFACE_DISPATCH_VERSION
   */
  uint32       revision;

  /* Initialize driver */
  uint32       (*init)                    (void);

  /* Shutdown driver */
  void         (*shutdown)                (void);

  /* Polls the driver for data */
  void         (*poll)                    (void);

  /* Receives bulk data from driver */
  uint32       (*rx_bulk)                 (uint8 *rx_buf,
                                           uint32 len,
                                           sahara_rx_cb_type rx_cb,
                                           uint32 * err_code,
                                           enum boot_sahara_packet_type type);

  /* Transmits bulk data to driver */
  uint32       (*tx_bulk)                 (uint8 *tx_buf,
                                           uint32 len,
                                           sahara_tx_cb_type tx_cb,
                                           uint32 * err_code);

  /* Returns the packet size in bytes supported by driver */
  uint32       (*get_max_packet_size)     (void);

  /* Resets the target */
  void         (*reset)                   (void);
  
  /* Validates address range is writeable */
  boolean      (*valid_address_range)     (uint64 start_addr,
                                           uint64 size, uint32 buf_typ);

  /* Executes corresponding client command and fills the returns the
     response for Sahara to send to host; also returns the 
     length of the response
     Inputs:
       client_command  - Passed by host in CMD_EXEC packet
                       - Represents Sahara client command ID
                       - Must be parsed and interpretted by Sahara client
     
       resp_length     - response length of executed command; memory for
                         resp_length allocated by Sahara, not by
                         Sahara client
     
       status          - status of executed command
     
     Return: pointer to response data; must be set by exec_cmd 
             to address of data response
  */
  uint8*       (*exec_cmd)                (uint32 client_command,
                                           uint32 *resp_length,
                                           uint32 *status);

  /* Returns the maximum raw data transfer size in bytes supported
     by the Sahara client */
  uint32       (*get_max_raw_data_size)   (void);

  /* Returns TRUE if authentication is enabled */
  boolean      (*auth_enabled)            (void);

  /* memcpy utility function required by Sahara */
  void         (*memcpy)                  (void* dst_ptr, 
                                           const void* src_ptr, 
                                           uint32 len);

  /* memset utility function required by Sahara */
  void         (*memset)                  (void* dst_ptr,
                                           uint32 val,
                                           uint32 len);

  /* Accessor functions to extract out the fields of a binary
     image header required by Sahara.
     These APIs must be supplied by the caller of boot_sahara_entry
     before entring into Sahara. This is required to support both
     the 40-byte normal header and the 80-byte extended header.
     These are required for ELF image transfers as well since
     the hash table segment is prepended with a binary image header.
  */
  uint64       (*get_bin_header_size)     (void);
  uint32       (*get_bin_image_id)        (const uint8 *header);
  uint64       (*get_bin_image_dest)      (const uint8 *header);
  uint64       (*get_bin_image_size)      (const uint8 *header);
  uint64       (*get_bin_code_size)       (const uint8 *header);
  uint32       (*get_bin_cert_chain_size) (const uint8 *header);
  uint32       (*get_bin_signature_size)  (const uint8 *header);
  uint64       (*get_bin_image_src)       (const uint8 *header);
  
  /* Handles unrecoverable error */
  void         (*unrecoverable_error_handler) (void);
  
  /* Handles any post processing after loading image */
  enum boot_sahara_status  (*image_load_post_proc) (void); 
 
  /* Returns if sahara should skip loading image */
  boolean      (*skip_image_load)  (void);

  /* Api to return the address of buffer to be used for
     loading hash */
  uint8* (*get_hash_segment_buffer)(void);

  /* Api to decide elf loading logic */
  boolean (*elf_optimized_segment_load) (void);  

  /* Api to pre-process elf headers */
  enum boot_sahara_status  (*preprocess_header) (
                              struct segment_record* sahara_elf_phdrs,
                              uint32 numsegments,
                              uint32 *segment_start_offset,
                              uint32 *remaining_size);

  /* Api to get index into hash table for pcie boot */
  enum boot_sahara_status  (*get_orig_hash_index) (
                              uint32 sorted_index,
                              uint32 *original_index_ptr);


};

/* Function table to support parallel authentication and load */
struct boot_sahara_auth_tbl
{
  /* Revision number of authentication table
   *   Should be set to SAHARA_INTERFACE_AUTH_VERSION
   */
  uint32  revision;

  /* Initializes the hash module for the update_hash and finalize_hash
     operations. This function must be called before the first update_hash
     function is called on a set of data.
 
     Return: TRUE if the operation was successful, FALSE otherwise
  */
  boolean (*init_hash)            (void);

  /* Computes the hash of a region of data and updates the hash digest buffer
     Inputs:
       buf_ptr        - Buffer that will be hashed
       buf_size       - Size in bytes that will be hashed
 
     Return: TRUE if the operation was successful, FALSE otherwise
  */
  boolean (*update_hash)          (const void *buf_ptr,
                                   uint32 buf_size);

  /* Finalizes the hash module and return the digest buffer.
     Return: The digest buffer if no errors were encountered, NULL otherwise
  */
  void*   (*finalize_hash)        (void);

  /* Compares the hash of the buffer with the expected indexed hash entry.
     Inputs:
       digest_buf_ptr - Hash digest of buffer to compare
       hash_index     - Index into expected hash table array to compare
                        against
 
     Return: TRUE if hash matches, FALSE otherwise
  */
  boolean (*compare_hash)         (const void *digest_buf_ptr,
                                   uint32 hash_index);

  /* Computes the hash of a region of data and compares it against
     the expected hash value for the given hash entry index.
     Inputs:
       buf_ptr    - Buffer that will be hashed
       buf_size   - Size in bytes that will be hashed
       hash_index - Index into expected hash table array to compare against

     Return: TRUE if the hash compared successfully and no errors were
             encountered; FALSE otherwise
  */
  boolean (*verify_hash)          (const void *buf_ptr,
                                   uint32 buf_size,
                                   uint32 hash_index);

  /* Computes the hash table entry offsets for each segment listed in the
     program headers. The offsets into the hash table are stored and maintained
     outside the scope of Sahara. This must be called before calling compare_hash
     or verify_hash.
     Inputs:
       hash_tbl_ptr - Pointer to hash table
       num_segments - Number of program headers
       prog_hdr_ptr - Pointer to program headers
     Return: None
  */
  void    (*compute_hash_offsets) (const uint8 *hash_tbl_ptr,
                                   const uint32 num_segments,
                                   const struct segment_record *prog_hdr_ptr);

  /* Authenticate an image given the image header
     Inputs:
       header_ptr - Pointer to the image header
     Return: TRUE if authentication was successful or authentication was
             disabled; FALSE otherwise
  */
  boolean (*authenticate)         (uint8 *header_ptr);
};

struct boot_sahara_mem_debug_tbl
{
  /* Returns TRUE if memory debug is supported in current mode of operation
     i.e. if secure boot is enabled, memory debug not enabled
  */
  boolean (*mem_debug_supported)          (void);

  /* Initializes the memory debug region table and enables access to each
     accessible region.
  */
  void    (*mem_debug_init)               (void);

  /* Returns the address of the memory debug region table. */
  uintnt  (*mem_debug_table_addr)         (void);

  /* Returns the length in bytes of the memory debug region table. */
  uint32  (*mem_debug_table_len)          (void);

  /* Verifies if the input base to base+len is a valid address range
     for memory debug.
     Inputs:
       base - Base address to check for validity
       len  - Length in bytes to check starting from base

     Return: TRUE if valid address range; FALSE otherwise
  */
  boolean (*mem_debug_verify_addr)        (uint64 base, 
                                           uint64 len);
  /* Verifies if the input base to base+len is a restricted address range
     i.e. the memory range cannot be directly accessed by the hardware
     interface; thus needs to be first copied to a temporary buffer.
     Inputs:
       base - Base address to check for restriction
       len  - Length in bytes to check starting from base

     Return: TRUE if restricted address range; FALSE otherwise
  */
  boolean (*mem_debug_is_restricted_addr) (uint64 base, 
                                           uint64 len);

  /* Copies restricted address range to a temporary buffer supplied
     by the Sahara client.
     Inputs:
       base - Address to start copying data from
       len  - Length in bytes to copy from base

     Return: Address of temporary buffer if copy was successful
             Set to 0 if copy failed or input address range is invalid
  */
  uint64  (*mem_debug_copy_restricted)    (const uint64 addr,
                                           uint64 len);
};

struct boot_sahara_interface
{
  /* Revision number of the boot_sahara_entry() interface used
   *   Should be set to SAHARA_INTERFACE_VERSION
   */
  const uint32 revision;

  /* Set to 0 by protocol if no errors occurred.
   *   Otherwise will return a nonzero value.
   */
  uint32 sahara_status;

  /* stores the information for the image to be transferred
     Caller must fill in:
       1. expected_image
       2. expected_image_type
       3. cur_image - should pass in address of a local or static structure
     Sahara protocol will set the following:
       1. image_rx_complete
       2. is_secure
   */
  struct boot_sahara_shared_data* sahara_shared_data;

  /* Mode of operation to execute in i.e. SAHARA_MODE_IMAGE_TX_PENDING */
  uint32 sahara_mode;

  /* Dispatch table used for transfers */
  struct boot_sahara_dispatch_tbl* dispatch_tbl;

  /* Function table used for authentication */
  struct boot_sahara_auth_tbl* auth_tbl;

  /* Function table used for memory debug */
  struct boot_sahara_mem_debug_tbl* mem_debug_tbl;

};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* ============================================================================
**  Function : boot_sahara_get_image_info
** ============================================================================
*/
/*!
*   @brief
*   Returns the pointer to information of the current image transfer
*   
*   @details
*   This function returns the pointer to information of the current image
*   transfer if the image reception was successful. Otherwise NULL is returned.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns pointer to shared data structure, NULL if
*             no images transferred yet
*   
*   @sa None
*/
#ifdef FEATURE_SAHARA_BOOT
struct boot_sahara_shared_data* boot_sahara_get_image_info
( 
  void 
);
#else
static inline struct boot_sahara_shared_data* boot_sahara_get_image_info
( 
  void 
)
{
  return NULL;
}
#endif

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
#ifdef FEATURE_SAHARA_BOOT
boolean boot_sahara_entry
(
  struct boot_sahara_interface* sahara_interface
);
#else
static inline boolean boot_sahara_entry
(
  struct boot_sahara_interface* sahara_interface
)
{ 
  return FALSE;
}
#endif

/* ============================================================================
**  Function : sbl_sahara_get_interface
** ============================================================================
*/
/*!
*   @brief
*   Returns pointer to the Sahara interface.
*   
*   @details
*   Returns pointer to the Sahara interface.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns pointer to the Sahara interface.
*   
*   @sa None
*/
#ifdef FEATURE_SAHARA_BOOT
struct boot_sahara_interface* sbl_sahara_get_interface
( 
  void 
);
#else
static inline struct boot_sahara_interface* sbl_sahara_get_interface
( 
  void 
)
{
  return NULL;
}
#endif

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
#ifdef FEATURE_SAHARA_BOOT
void boot_sahara_set_is_ssa_enabled
( 
  boolean subsys_self_auth_value
);
#else
static inline void boot_sahara_set_is_ssa_enabled
( 
  boolean subsys_self_auth_value
){}
#endif

/* ============================================================================
**  Function : boot_sahara_elf_segment_is_loadable
** ============================================================================
*/
/*!
*   @brief
*   Returns whether to load the segment
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
#ifdef FEATURE_SAHARA_BOOT
boolean boot_sahara_elf_segment_is_loadable
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
);
#else
static inline boolean boot_sahara_elf_segment_is_loadable
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
)
{
  return FALSE;
}
#endif

#endif  /* BOOT_SAHARA_H */
