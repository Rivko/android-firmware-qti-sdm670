/**
  @file spi_api.h
  @brief SPI Driver API
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/api/buses/spi_api.h#2 $
  $DateTime: 2017/08/01 13:24:07 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#ifndef __SPIAPI_H__
#define __SPIAPI_H__

#include "comdef.h"

#define SPI_SUCCESS(x)  (x == SPI_SUCCESS)
#define SPI_ERROR(x)    (x != SPI_SUCCESS)

typedef void (*callback_fn) (uint32 transfer_status, void *callback_ctxt);

typedef enum
{
   SPI_INSTANCE_1 = 1,
   SPI_INSTANCE_2,
   SPI_INSTANCE_3,
   SPI_INSTANCE_4,
   SPI_INSTANCE_5,
   SPI_INSTANCE_6,
   SPI_INSTANCE_7,
   SPI_INSTANCE_8,
   SPI_INSTANCE_9,
   SPI_INSTANCE_10,
   SPI_INSTANCE_11,
   SPI_INSTANCE_12,
   SPI_INSTANCE_13,
   SPI_INSTANCE_14,
   SPI_INSTANCE_15,
   SPI_INSTANCE_16,
   SPI_INSTANCE_17,
   SPI_INSTANCE_18,
   SPI_INSTANCE_19,
   SPI_INSTANCE_20,
   SPI_INSTANCE_21,
   SPI_INSTANCE_22,
   SPI_INSTANCE_23,
   SPI_INSTANCE_24,
   SPI_INSTANCE_MAX,
} spi_instance_t;

/* Function return status and error codes */
typedef enum
{
    SPI_SUCCESS  = 0x0000,

    /*parameter and system errors*/
    SPI_ERROR,
    SPI_ERROR_INVALID_PARAM,
    SPI_ERROR_HW_INFO_ALLOCATION,
    SPI_ERROR_MEM_ALLOC,
    SPI_ERROR_MUTEX,
    SPI_ERROR_HANDLE_ALLOCATION,
    SPI_ERROR_INVALID_EXECUTION_LEVEL,
	SPI_ERROR_UNSUPPORTED_IN_ISLAND_MODE,
	SPI_ERROR_UNCLOCKED_ACCESS,

    /*transfer and dma related errors*/
    SPI_ERROR_TRANSFER_TIMEOUT,
    SPI_ERROR_PENDING_TRANSFER,
    SPI_ERROR_DMA_PROCESS_TRE_FAIL,
    SPI_ERROR_DMA_INSUFFICIENT_RESOURCES,
    SPI_ERROR_DMA_EVT_OTHER,
    SPI_ERROR_DMA_QUP_NOTIF,
    SPI_ERROR_DMA_REG_FAIL,
    SPI_ERROR_DMA_DEREG_FAIL,
    SPI_ERROR_DMA_TX_CHAN_ALLOC_FAIL,
    SPI_ERROR_DMA_RX_CHAN_ALLOC_FAIL,
    SPI_ERROR_DMA_EV_CHAN_ALLOC_FAIL,
    SPI_ERROR_DMA_TX_CHAN_START_FAIL,
    SPI_ERROR_DMA_RX_CHAN_START_FAIL,
    SPI_ERROR_DMA_TX_CHAN_STOP_FAIL,
    SPI_ERROR_DMA_RX_CHAN_STOP_FAIL,
    SPI_ERROR_DMA_TX_CHAN_RESET_FAIL,
    SPI_ERROR_DMA_RX_CHAN_RESET_FAIL,
    SPI_ERROR_DMA_TX_CHAN_DEALLOC_FAIL,
    SPI_ERROR_DMA_RX_CHAN_DEALLOC_FAIL,
    SPI_ERROR_DMA_EV_CHAN_DEALLOC_FAIL,

    /*platform errors*/
    SPI_ERROR_PLAT_GET_CONFIG_FAIL,
    SPI_ERROR_PLAT_REG_INT_FAIL,
    SPI_ERROR_PLAT_CLK_ENABLE_FAIL,
    SPI_ERROR_PLAT_GPIO_ENABLE_FAIL,
    SPI_ERROR_PLAT_CLK_DISABLE_FAIL,
    SPI_ERROR_PLAT_GPIO_DISABLE_FAIL,
    SPI_ERROR_PLAT_CLK_SET_FREQ_FAIL,
    SPI_ERROR_PLAT_INTERRUPT_REGISTER,
    SPI_ERROR_PLAT_INTERRUPT_DEREGISTER,
	SPI_ERROR_PLAT_SET_RESOURCE_FAIL,
	SPI_ERROR_PLAT_RESET_RESOURCE_FAIL,

    SPI_ERROR_QDI_ALLOC_FAIL,
    SPI_ERROR_QDI_COPY_FAIL,
    SPI_ERROR_QDI_MMAP_FAIL,
	
	SPI_ERROR_CORE_NOT_OPEN,
    /**< SW code errors. */
} spi_status_t;

/*
* Note: In the legacy (QUP v2 SPI) driver, the "Input-first/ Output first"
* terminology was used for denoting Clock Phase. We are using the more
* widely accepted MODE_0 to MODE_3 convention in this driver.
* If you're converting from the old SPI API to the new one, use this guide.
*
* Clk Polarity: SPI_CLK_IDLE_HIGH corresponds to CPOL = 0
* Clk Phase: SPI_OUTPUT_FIRST_MODE correposnds to CPHA = 0

* For new SPI slaves, refer to the slave specifications to decide which
* mode to use.
*/

typedef enum
{
   SPI_MODE_0,   /*CPOL = 0, CPHA = 0*/
   SPI_MODE_1,   /*CPOL = 0, CPHA = 1*/
   SPI_MODE_2,   /*CPOL = 1, CPHA = 0*/
   SPI_MODE_3,   /*CPOL = 1, CPHA = 1*/
}spi_mode_t;


typedef enum
{
   SPI_CS_ACTIVE_LOW,/**< During idle state CS  line is held low*/
   SPI_CS_ACTIVE_HIGH,/**< During idle state CS line is held high*/
   SPI_CS_ACTIVE_INVALID = 0x7FFFFFFF
}spi_cs_polarity_t;

/*Note - Endian ness - it is unclear  whether we can
 * support this in all (FIFO/GSI) modes !
 * Order in which bytes from tx/rx buffer words are put on the bus.
 */
typedef enum
{
    SPI_NATIVE = 0,
    SPI_LITTLE_ENDIAN = 0,
    SPI_BIG_ENGIAN /* network*/
}spi_byte_order_t;


/* SPI configuration passed to the spi_full_duplex() function. */
typedef struct
{
   spi_mode_t spi_mode;
   spi_cs_polarity_t spi_cs_polarity;
   spi_byte_order_t endianness;
   uint8 spi_bits_per_word;  /*4 <= N <= 32*/
   uint8 spi_slave_index;
   /**< Slave index, beginning at 0, if mulitple SPI devices are connected to the same master
   At most 7 slaves are allowed. If an invalid number (7 or higher) is set, CS
   signal will not be used*/
   uint32 clk_freq_hz;
   /**< Host will set the SPI clk frequency closest to the requested frequency */
   uint8 cs_clk_delay_cycles;
   /*Num of clk cycles to wait after asserting CS before starting txfer*/
   uint8 inter_word_delay_cycles;
   /*Num of clk cycles to wait between SPI words*/
   boolean cs_toggle;
   /*1 = CS deasserts between words
     0 = CS stays asserted between words.*/
   boolean loopback_mode;
   /**<Normally 0. If set, SPI controller will enable loopback mode, used primarily for testing */
} spi_config_t;

typedef struct
{
    uint8  *tx_buf;
    uint8  *rx_buf;
    uint32  len;
    boolean leave_cs_asserted;
}spi_descriptor_t;

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/
/* spi_open
Open a SPI port and get a SPI handle. This sets up structures and objects for future
transactions, but does not talk to the SPI hardware. */
spi_status_t spi_open (spi_instance_t instance, void **spi_handle);

/* spi_power_on and off
Setup and power up/down clocks and TLMM pins.*/
spi_status_t spi_power_on (void *spi_handle);
spi_status_t spi_power_off (void *spi_handle);

/* spi transfer: Perform full duplex, half duplex or simplex transfer over SPI BUS.
This is the only available transfer function. The descriptor can have NULL tx OR rx buffers if
half duplex transfer is desired. Callback is invoked upon completion of the full chain of
descriptors or an error condition.*/
spi_status_t spi_full_duplex (void *spi_handle, spi_config_t *config, spi_descriptor_t *desc, uint32 num_descriptors, callback_fn c_fn, void *c_ctxt, boolean get_timestamp);

/* spi_close */
spi_status_t spi_close (void *spi_handle);

/*Get the timestamps on the last executed SPI transfer which had get_timestamp enabled.
If the transfer consisted of a number of descriptors, only the timestamp of the LAST
transferred buffer is captured*/
spi_status_t spi_get_timestamp (void *spi_handle, uint64 *start_time, uint64 *completed_time);

spi_status_t spi_setup_island_resource (spi_instance_t instance, uint32 frequency_hz);

//If a resource was previously set up for island operation, this releases that resource.
spi_status_t spi_reset_island_resource (spi_instance_t instance);

#endif  /* Double inclusion protection*/

