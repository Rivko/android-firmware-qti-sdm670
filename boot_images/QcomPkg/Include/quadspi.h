#ifndef __QUADSPI_H__
#define __QUADSPI_H__
/*==================================================================================================

FILE: quadspi.h

DESCRIPTION: driver for the Atheros-based QSPI (quad SPI) first used in 8996 v2.1

                       Copyright (c) 2015 Qualcomm Technologies, Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include <stdbool.h>   // bool, etc.
#include <stdint.h>    // uint32_t, etc.

/*==================================================================================================
                                               MACROS
==================================================================================================*/

// The following macros are used to statically allocate DMA buffers.  DMA buffers must be
// located in memory that is accessible to the QSPI HW.  They should also be in uncached memory
// (the quadspi driver will not flush/invalidate client buffers).  For best performance DMA
// buffers should be 32-byte aligned.  For example:
//
// static uint8_t dma_buffer[64 * 1024] ALIGN(32) SECTION(".bss.BOOT_DDR_UNCACHED_ZI_ZONE");

#ifndef ALIGN
#define ALIGN(__value)  __attribute__((__aligned__(__value)))
#endif

#ifndef SECTION
#define SECTION(__name) __attribute__((section(__name)))
#endif

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   SDR_1BIT = 1,
   SDR_2BIT = 2,
   SDR_4BIT = 3,
   DDR_1BIT = 5,  // currently broken in HW
   DDR_2BIT = 6,  // currently broken in HW
   DDR_4BIT = 7,
} QSPI_MODE;

typedef struct
{
   uint8_t    opcode;
   uint8_t    addr_bytes;
   uint8_t    dummy_clocks;
   QSPI_MODE  cmd_mode;
   QSPI_MODE  addr_mode;
   QSPI_MODE  data_mode;
   bool       write;
} QSPI_CMD;

typedef struct
{
   uint8_t   chip_select;   // chip select to assert during transaction (0 or 1)
   bool      clk_polarity;  // clock polarity
   bool      clk_phase;     // clock phase
   uint32_t  clk_freq;      // bus clock frequency (4.8 MHz, 60 MHz, 66.625 MHz, or 75 MHz)
} QSPI_MASTER_CONFIG;

typedef struct
{
   QSPI_MODE  mode;        // specifies SDR/DDR and number of data lines
   bool       terminate;   // true if fragment terminates transfer (deassert chip select)
   bool       enable_dma;  // TRUE = use DMA mode, FALSE = use PIO mode
} QSPI_TRANSFER_CONFIG;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

extern bool quadspi_cmd(QSPI_CMD *cmd, uint32_t addr, uint8_t *data, uint32_t data_bytes,
                        bool enable_dma);
extern bool quadspi_deinit(void);
extern bool quadspi_init(QSPI_MASTER_CONFIG *cfg);
extern bool quadspi_read(uint8_t *buffer, uint32_t num_bytes, QSPI_TRANSFER_CONFIG *cfg);
extern bool quadspi_write(uint8_t *buffer, uint32_t num_bytes, QSPI_TRANSFER_CONFIG *cfg);

#endif  // __QUADSPI_H__
