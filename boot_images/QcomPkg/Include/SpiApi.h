/** 
  @file  SpiApi.h
  @brief SPI APIs
*/
/*=============================================================================
  Copyright (c) 2014, 2020 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/23/20   bng    Changes specific to QUPv3
 09/23/14   sg     Created

=============================================================================*/

#ifndef __SPIAPI_H__
#define __SPIAPI_H__

#include "SpiDeviceTypes.h"
#include "SpiDevice.h"

/** @addtogroup spi_data_types 
@{ */

/**
  SPI core passed to the spi_open() function. 
*/
typedef enum {
  SPI_INSTANCE_001,          /**< Instance 01. */
  SPI_INSTANCE_002,          /**< Instance 02. */
  SPI_INSTANCE_003,          /**< Instance 03. */
  SPI_INSTANCE_004,          /**< Instance 04. */
  SPI_INSTANCE_005,          /**< Instance 05. */
  SPI_INSTANCE_006,          /**< Instance 06. */
  SPI_INSTANCE_007,          /**< Instance 07. */
  SPI_INSTANCE_008,          /**< Instance 08. */
  SPI_INSTANCE_009,          /**< Instance 09. */
  SPI_INSTANCE_010,          /**< Instance 10. */
  SPI_INSTANCE_011,          /**< Instance 11. */
  SPI_INSTANCE_012,          /**< Instance 12. */
  SPI_INSTANCE_013,
  SPI_INSTANCE_014,
  SPI_INSTANCE_015,
  SPI_INSTANCE_016,
  SPI_INSTANCE_017,
  SPI_INSTANCE_018,
  SPI_INSTANCE_019,
  SPI_INSTANCE_020,
  SPI_INSTANCE_MAX,
} spi_instance;

/**
  Function status codes.
*/
typedef enum {
	 /*parameter and system errors*/
	SPI_SUCCESS  = 0x0000,
    SPI_ERROR,
    SPI_ERROR_INVALID_PARAM,
    SPI_ERROR_HW_INFO_ALLOCATION,
    SPI_ERROR_MEM_ALLOC,
    SPI_ERROR_MUTEX,
    SPI_ERROR_HANDLE_ALLOCATION,
    SPI_ERROR_INVALID_EXECUTION_LEVEL,
	SPI_ERROR_UNSUPPORTED_IN_ISLAND_MODE,
	SPI_ERROR_UNCLOCKED_ACCESS,
	SPI_ERROR_INVALID_FW_VERSION,
	SPI_ERROR_FW_LOAD_FALIURE,
	
	/*FIFO related error */
	SPI_ERROR_INPUT_FIFO_UNDER_RUN,
	SPI_ERROR_INPUT_FIFO_OVER_RUN,
	SPI_ERROR_OUTPUT_FIFO_UNDER_RUN,
	SPI_ERROR_OUTPUT_FIFO_OVER_RUN,
	SPI_ERROR_COMMAND_OVER_RUN,
	SPI_TRANSFER_FORCE_TERMINATED,
	SPI_ERROR_COMMAND_ILLEGAL,
	SPI_ERROR_COMMAND_FAIL,
	SPI_ERROR_INVALID_CMD_OPCODE,

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

    SPI_ERROR_INIT,         				
   
    SPI_ERROR_DEINIT, 						
   
    SPI_ERROR_POWER_ON,						
   
    SPI_ERROR_POWER_OFF,					
   
    SPI_ERROR_BUS_ACQUISITION_TIMEOUT,		
   
    SPI_ERROR_BUSY,							
   
    SPI_ERROR_OPERATION_DENIED,				
   
    SPI_ERROR_OPERATION_FAILED,   			
         
    SPI_ERROR_TRANSFER,						
   
    SPI_ERROR_DRV_FWK_INIT,					
   
    SPI_ERROR_DRV_FWK_NULL_HANDLE			
   
 

} spi_status;

/** @addtogroup spi_data_types 
@{ */

typedef enum
{
   SPI_MODE_0,   /*CPOL = 0, CPHA = 0*/
   SPI_MODE_1,   /*CPOL = 0, CPHA = 1*/
   SPI_MODE_2,   /*CPOL = 1, CPHA = 0*/
   SPI_MODE_3,   /*CPOL = 1, CPHA = 1*/
}spi_mode_t;


typedef enum
{
   CS_ACTIVE_LOW,/**< During idle state CS  line is held low*/
   CS_ACTIVE_HIGH,/**< During idle state CS line is held high*/
   CS_ACTIVE_INVALID = 0x7FFFFFFF
}spi_cs_polarity_t;

/*Note - Endian ness - it is unclear  whether we can
 * support this in all (FIFO/GSI) modes !
 * Order in which bytes from tx/rx buffer words are put on the bus.
 */
typedef enum
{
    SPI_NATIVE = 0,
    SPI_LITTLE_ENDIAN = 0,
    SPI_BIG_ENDIAN /* network*/
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
/* spi_open */ 
/** @ingroup spi_open
  @par Summary
  This function is called by the client code to initialize the respective 
  SPI core instance used by the client. On success, spi_handle points to the handle 
  for the SPI core. 
  
  @param[in]  instance              SPI core instance that the client intends to
                                    initialize; see #spi_instance for 
                                    details.
  @param[out] spi_handle            Pointer location to be filled by the
                                    driver with a handle to the core.

  @return
  See #spi_status for details.
*/
spi_status 
spi_open( 
	spi_instance instance, 
	void **spi_handle 
);

/**
  spi_transfer ()

  @brief
  Transfer implementation of EFI_QCOM_SPI_PROTOCOL
 */
spi_status 
spi_transfer( 
		void	*spi_handle,
		SpiDeviceInfoType	*devInfo,
		CONST UINT8	*write_buffer,
		UINT32	write_len,
		UINT8	*read_buffer,
		UINT32	read_len
);


/* spi_close */ 
/** @ingroup spi_close
  @par Summary
  De-initializes the SPI core.

  @param[in] spi_handle             Handle to the SPI core.

  @return
  See #spi_status for details.
*/
spi_status
spi_close(
    void *spi_handle
);

#endif	/* __SPIAPI_H__ */
