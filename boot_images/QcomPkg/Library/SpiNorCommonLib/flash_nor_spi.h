#ifndef FLASH_NOR_SPI_H
#define FLASH_NOR_SPI_H

/*==========================================================================
 *
 * FILE:      flash_nor_spi.h
 *
 * DESCRIPTION: Function and data structure declarations for SPI_NOR
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to the
 * module. Notice that changes are listed in reverse chronological
 * order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/03/12     sb      Clean up
 * 03/20/12     sv      Add Micron SPI NOR Support
 * 03/19/12     sv      SPI NOR power management support
 * 11/01/11     eo      Add Winbond NOR support
 * 10/06/11     sv      Add SPI NOR command log support
 * 07/20/11     sv      Add macros for block size, cleanups
 * 03/30/11     sv      Add/update status mask defines
 * 03/01/11     sv      Remove sleep_ops from dev_pdata
 * 02/01/11     eo      Add Maxcronix SPI NOR device support
 * 09/29/10     sv/nb   SPI NOR Tools Support/I2C NOR Support
 * 09/13/10     eo      Add formalized partition support
 * 08/18/10     eo      Add interim partition support
 * 06/21/10     eo      Add function ptr for sleep API interface
 * 04/05/10     sv      Add macros and structures for SPI NOR
 * 03/28/10     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_nor_entry.h"

#define NOR_SPI_PROGRAM_TIMEOUT       500000   /* Polling count timeout 
                                                * on write operation */
#define NOR_SPI_ERASE_TIMEOUT         1500000  /* Polling count timeout
                                                * on erase operation */

#define NOR_SPI_GBL_UNPROTECT_BLK     0       /* 8 bit: 0x0000xx global 
                                               * unprotect all sectors */

#define NOR_SPI_CMD_ADDR_BUF_LEN      16
#define NOR_SPI_CMD_LEN               1

#define NOR_SPI_READDID_CMD           0x9F 
#define NOR_SPI_READ_STATUS_CMD       0x05
#define NOR_SPI_WRITE_STATUS_CMD      0x01
#define NOR_SPI_WRITE_ENABLE_CMD      0x06

#define NOR_SPI_ERASE_64KB_BLK_CMD    0xD8
#define NOR_SPI_ERASE_32KB_BLK_CMD    0x52
#define NOR_SPI_ERASE_4KB_BLK_CMD     0x20
#define NOR_SPI_CHIP_ERASE_60_CMD     0x60
#define NOR_SPI_CHIP_ERASE_C7_CMD     0xC7

#define NOR_SPI_PROG_PAGE_CMD         0x02
#define NOR_SPI_READ_PAGE_CMD         0x03

#define NOR_SPI_ENTER_4B_ADDR_CMD     0xB7
#define NOR_SPI_EXIT_4B_ADDR_CMD      0xE9

#define NOR_SPI_READ_SECURITY_CMD     0x2B  /* Macronix: Program/Erase status 
                                             * error bit */
#define NOR_SPI_READ_FLAG_CMD         0x70  /* Micron: Program/Erase status 
                                             * error bit */
											 
#define NOR_SPI_DP_PWR_DWN_CMD        0xB9  /* Deep power down mode */
#define NOR_SPI_REL_DP_PWR_DWN_CMD    0xAB  /* Release Deep power down mode */
#define NOR_SPI_REL_DPD_DELAY_MAX     35    /* Max delay to Release from Deep 
                                             * Power Down mode */

#define NOR_SPI_ID_LEN                4
#define NOR_SPI_STATUS_REG_LEN        1
#define NOR_SPI_ADDR_LEN              3

#define NOR_SPI_STATUS_BUSY_MASK      0x01
#define NOR_SPI_STATUS_WR_EN_MASK     0x02  /* Write Enable Latch status */

#define NOR_SPI_STATUS_AT_ERROR_MASK  0x20  /* Atmel: prog/erase fail status */
#define NOR_SPI_STATUS_MX_PFAIL_MASK  0xEF  /* Macronix: Program Fail status */
#define NOR_SPI_STATUS_MX_EFAIL_MASK  0xEF  /* Macronix: Erase FAIL status */
#define NOR_SPI_STATUS_MI_PFAIL_MASK  0x7E  /* Micron: Program Fail status */
#define NOR_SPI_STATUS_MI_EFAIL_MASK  0x7E  /* Micron: Erase FAIL status */

#define NOR_SPI_MX_UNPROTECT_MASK     0x3C  /* Macronix: BP[3:0] bit5-bit2 */
#define NOR_SPI_WB_UNPROTECT_MASK     0x1C  /* Winbond: BP[2:0]  bit4-bit2 */
#define NOR_SPI_MI_UNPROTECT_MASK     0x1C  /* Micron: BP[2:0]  bit4-bit2 */
#define NOR_SPI_AT_UNPROTECT_MASK     0x0C  /* Atmel: Sector Protect bit3-bit2*/

#define BLK_SIZE_4K                   (4 * 1024)  /* Macro for 4K Block Size*/
#define BLK_SIZE_32K                  (32 * 1024) /* Macro for 32K Block Size*/
#define BLK_SIZE_64K                  (64 * 1024) /* Macro for 64K Block Size*/

#define NOR_SPI_CMD_LOG_BUF_SIZE  (NOR_SPI_CMD_LEN + NOR_SPI_ADDR_LEN)
                                       /* Size of each command log buffer */
#define NOR_SPI_MAX_CMD_LOG_ENTRY  20  /* Maximum SPI NOR command logs entries 
                                          that can be stored */

/* Enum for current SPI NOR operation in progress */
enum nor_spi_dev_op
{
  NOR_SPI_ERASE_OP,     /* Erase Operation */
  NOR_SPI_WRITE_OP,     /* Write Operation */
  NOR_SPI_INVALID_OP    /* Invalid Operation */
};

/* SPI_NOR specific/private/probe data that is shared across clients and is
   malloced once-only at probe time */
struct nor_spi_dev_pdata
{
  struct flash_nor_params *config_data;  /* Place to hold device details
                                            from config */
  struct nor_flash_meminfo *flash_mem_info; /* Flash memory allocation info */
  uint8 *cmd_addr_buffer;                /* buffer for tx/rx that needs command
                                            and address bytes*/
  uint8 *status_buffer;                  /* buffer to read the status 
                                            register value */
  uint8 erase_in_progress;               /* blocks all other flash operations
                                            when erase block in progress */
  uint8 num_addr_bytes;				     /* Number of address bytes */
};

/* SPI probe configuration */
struct nor_spi_probe_cfgs
{
  uint32 base_address;
  uint32 chip_sel;
  enum flash_type dev_type;
};

/* SPI bus device parameters configuration */
struct flash_spi_device_param_config
{
  uint8  clock_mode;             /* Clock high or low in idle */      
  uint8  clock_polarity;         /* Clock active high or active low */
  uint8  shift_mode;             /* Input first or output first */
  uint32 cs_deassert_time;       /* Chip select de-assert time */
  uint32 min_slave_freq_hz;      /* Min frequency slave shifts data in/out */
  uint32 max_slave_freq_hz;      /* Max frequency slave shifts data in/out */
  uint8  cs_polarity;            /* Chip select active high or active low */
  uint8  cs_mode;                /* Chip select keep asserted or de-assert */
};

/* SPI bus device board information */
struct flash_spi_device_board_info
{
  uint32 cs_num;                 /* Chip select number slave device uses. */
  uint8 core_mode;               /* SPI core mode operate as master or slave*/
};

/* SPI bus tranfer parameters */
struct flash_spi_device_transfer_param
{
  uint32 num_bits;             /* Specifies N: number of bits in one logical
                                  SPI transfer. */
  uint8 loopback_mode;         /* Debug only: enable/disable loopback mode */
  uint8 input_packing;         /* Debug only: enable/disable input packing */
  uint8 output_unpacking;      /* Debug only: enable/disable output unpacking */
  uint32 slave_timeout_us;     /* Slave device time out in micro seconds. */
};

/* SPI Core configuration */
struct flash_spi_core_config
{
  struct flash_spi_device_param_config dev_param; /* SPI device parameters 
                                                     configuration */
  struct flash_spi_device_board_info dev_board;   /* SPI device board 
                                                     information */
  struct flash_spi_device_transfer_param dev_transfer; /* SPI device transfer
                                                          parameters */
};

/* SPI transfer buffer information */
struct nor_spi_xfer_buf_info
{
  uint8 *spi_cmd_buf;
  uint32 cmd_buf_len;
  uint8 *spi_data_buf;
  uint32 data_buf_len;
};

/* SPI wrapper function pointers */
struct nor_spi_wrapper
{
  /* SPI Init API */
  int (*init)(flash_client_ctxt *client_ctxt);
  /* SPI Close API */
  int (*close)(void);
  /* SPI Device Configure API */
  int (*dev_cfg)(void);
  /* SPI read API */
  void (*read_op)(struct nor_spi_xfer_buf_info *spi_read_buf_info, 
    int *status);
  /* SPI write API */
  void (*write_op)(struct nor_spi_xfer_buf_info *spi_write_buf_info, 
    int *status);
};

/* SPI NOR command log entry structure */
struct flash_nor_spi_cmd_log
{
  uint8 spi_cmd_buf[NOR_SPI_CMD_LOG_BUF_SIZE]; /* command log buffer */
  uint16 spi_total_tx_bytes; /* Total bytes transmitted with this 
                                command(including data)*/
};

/* SPI NOR Debug info structure */
/* The SPI command logging happens in a circular fashion. It starts from
   index 0 and goes till NOR_SPI_MAX_CMD_LOG_ENTRY-1 after which it again
   logs at index 0.nor_spi_next_cmd_log_indx will point to the next index
   on which the logging needs to be done so nor_spi_next_cmd_log_indx - 1
   gives the index on which the last command info is logged.
*/
struct flash_nor_spi_dbg_info
{
  struct flash_nor_spi_cmd_log nor_spi_cmd_log[NOR_SPI_MAX_CMD_LOG_ENTRY];
                                    /* Command log entries */
  uint32 nor_spi_next_cmd_log_indx; /* Points to the index on which next 
                                       command info will be logged */
};

/* 
 * Logical page to Physical page translation mapping.
 */
#define NOR_LP_TO_PP(client_ctxt, lp)                                   \
  /* physical page = logical page + start page of partition */          \
  (lp + (client_ctxt->client_data.partition_block_start <<              \
   client_ctxt->client_data.block_size_shift))

/* 
 * Logical block to Physical block translation mapping.
 */
#define NOR_LB_TO_PB(client_ctxt, lb)                                   \
  /* physical block = logical block + start block of partition */       \
  (lb + client_ctxt->client_data.partition_block_start)

/* 
 * NOR device private data.
 */
#define GET_NOR_PDATA(client_ctxt)   ((struct nor_spi_dev_pdata *) \
  client_ctxt->flash_dev_ctxt->dev_info.common.dev_specific_data)

/* 
 * Information on memory allocated for NOR device driver.
 */
#define GET_NOR_MEM_INFO(client_ctxt) \
  (GET_NOR_PDATA(client_ctxt))->flash_mem_info

/* 
 * NOR device register status.
 */
#define CHECK_DEVICE_STATUS(pdata, status) ((status & \
  pdata->op_status_mask) == pdata->op_status_value)

/*
 * Probe the SPI_NOR device and do all one time initialization for SPI_NOR driver
 */
int nor_spi_probe(flash_client_ctxt *);

/* 
 * Set 4 byte address mode in SPI NOR flash device.
 */
int nor_spi_set_num_address_bytes(flash_client_ctxt *client_ctxt);

/*
 * Unlock all blocks from in SPI NOR device
 */
int nor_spi_unlock_all_blocks(flash_client_ctxt *client_ctxt);

/*
 * Read the SPI NOR from the device
 */
int nor_spi_read_id(flash_client_ctxt *client_ctxt, uint8 *ids);

/* Release SPI NOR from Deep Power Down mode*/
int nor_spi_release_deep_power_down(flash_client_ctxt *client_ctxt); 
  
/* Wrapper function for spi read operation */
void nor_spi_read(struct nor_spi_xfer_buf_info *spi_buf_info, int *status);

/* Wrapper function for spi write operation */
void nor_spi_write(struct nor_spi_xfer_buf_info *spi_buf_info, int *status);

#endif /* FLASH_NOR_SPI_H */
