/**
    @file  i2c_api.h
    @brief I2C APIs
 */
/*=============================================================================
         Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/**

 @file i2c_api.h

 @addtogroup i2c_api
 @{

 @brief Inter-Integrated Circuit (I2C) and Improved I2C (I3C)

 @details I2C and I3C are 2-wire buses used to connect high and low speed
          peripherals to a processor or a microcontroller. Common peripherals
          include touch screen controllers, accelerometers, gyros, and ambient
          light and temperature sensors.

          The 2-wire bus comprises a data line, a clock line, and basic START,
          STOP, ACK/NACK/TRANSITION and PARITY framing format to drive transfers
          on the bus. A peripheral is also referred to as a slave. The processor
          or microcontroller implements the master as defined in the I2C
          specification Rev 6 and the MIPI I3C specification Rev 1.0. This
          API provides the software interface to access the I2C and I3C master
          hardware.

 @code {.c}

 //
 // The code sample below demonstrates the use of this interface.
 //

 typedef struct client_ctxt
 {
   i2c_slave_config *config;
   i2c_descriptor   *desc;
   uint32           num_descs;
 } client_ctxt;

 void sample (void)
 {
   void     *client_handle  = NULL;
   uint8_t  buffer[4]       = { 1, 2, 3, 4 };
   uint32   transferred     = 0;

   i2c_status       res = I2C_SUCCESS;
   i2c_descriptor   desc[2];
   i2c_slave_config config;

   client_ctxt ctxt;

   // ASYNCHRONOUS MODE
   // Obtain a client specific connection handle to the i2c bus instance 1
   res = i2c_open (I2C_INSTANCE_001, &client_handle);

   // Configure the bus speed and slave address
   config.bus_frequency_khz             = I2C_FAST_MODE_FREQ_KHZ;
   config.slave_address                 = 0x36;
   config.mode                          = I2C;
   config.slave_max_clock_stretch_us    = 500;
   config.core_configuration1           = 0;
   config.core_configuration2           = 0;

   // <S>  - START
   // <P>  - STOP
   // <Sr> - Repeat Start or Re-Start
   // <A>  - Acknowledge
   // <T>  - Transition
   // <N>  - Not-Acknowledge
   // <R>  - Read Transfer
   // <W>  - Write Transfer

   // Single write transfer of 4 bytes
   // I2C: <S><slave_address><W><A><data1><A><data2><A>...<dataN><A><P>
   // I3C: <S><slave_address><W><A><data1><T><data2><T>...<dataN><T><P>
   desc[0].buffer      = buffer;
   desc[0].length      = 4;
   desc[0].flags       = I2C_WRITE_TRANSFER;

   ctxt.config         = &config;
   ctxt.desc           = desc;
   ctxt.num_descs      = 1;

   res = i2c_transfer (client_handle, &config, &desc[0], 1, client_callback, &ctxt, 0, NULL);

   // Single read transfer of 4 bytes
   // I2C: <S><slave_address><R><A><data1><A><data2><A>...<dataN><N><P>
   // I3C: <S><slave_address><R><A><data1><T><data2><T>...<dataN><T><P>
   desc[0].buffer      = buffer;
   desc[0].length      = 4;
   desc[0].flags       = I2C_READ_TRANSFER;

   ctxt.config         = &config;
   ctxt.desc           = desc;
   ctxt.num_descs      = 1;

   res = i2c_transfer (client_handle, &config, &desc[0], 1, client_callback, &ctxt, 0, NULL);

   // Read 4 bytes from a register 0x01 on a sensor device
   // I2C: <S><slave_address><W><A><0x01><A><Sr><slave_address><R><A><data1><A><data2><A>...<dataN><N><P>
   // I3C: <S><slave_address><W><A><0x01><T><Sr><slave_address><R><A><data1><T><data2><T>...<dataN><T><P>
   uint8_t reg = 0x01;

   desc[0].buffer      = &reg;
   desc[0].length      = 1;
   desc[0].flags       = I2C_WR_RD_WRITE;

   desc[1].buffer      = buffer;
   desc[1].length      = 4;
   desc[1].flags       = I2C_WR_RD_READ;

   ctxt.config         = &config;
   ctxt.desc           = desc;
   ctxt.num_descs      = 2;

   res = i2c_transfer (client_handle, &config, &desc[0], 2, client_callback, &ctxt, 0, NULL);

   // Read 4 bytes from eeprom address 0x0102
   // I2C: <S><slave_address><W><A><0x01><A><0x02><A><Sr><slave_address><R><A><data1><A><data2><A>...<dataN><N><P>
   // I3C: <S><slave_address><W><A><0x01><T><0x02><T><Sr><slave_address><R><A><data1><T><data2><T>...<dataN><T><P>
   uint8_t reg[2]      = { 0x01, 0x02 };

   desc[0].buffer      = reg;
   desc[0].length      = 2;
   desc[0].flags       = I2C_WR_RD_WRITE;

   desc[1].buffer      = buffer;
   desc[1].length      = 4;
   desc[1].flags       = I2C_WR_RD_READ;

   ctxt.config         = &config;
   ctxt.desc           = desc;
   ctxt.num_descs      = 2;

   res = i2c_transfer (client_handle, &config, &desc[0], 2, client_callback, &ctxt, 0, NULL);

   // Close the connection handle to the i2c bus instance
   res = i2c_close (client_handle);

   // SYNCHRONOUS MODE
   // Obtain a client specific connection handle to the i2c bus instance 2
   res = i2c_open (I2C_INSTANCE_002, &client_handle);

   // Read 4 bytes from a register 0x01 on a sensor device
   // I2C: <S><slave_address><W><A><0x01><A><Sr><slave_address><R><A><data1><A><data2><A>...<dataN><N><P>
   // I3C: <S><slave_address><W><A><0x01><T><Sr><slave_address><R><A><data1><T><data2><T>...<dataN><T><P>
   uint8_t reg = 0x01;

   desc[0].buffer      = &reg;
   desc[0].length      = 1;
   desc[0].flags       = I2C_WR_RD_WRITE;

   desc[1].buffer      = buffer;
   desc[1].length      = 4;
   desc[1].flags       = I2C_WR_RD_READ;

   ctxt.config         = &config;
   ctxt.desc           = desc;
   ctxt.num_descs      = 2;

   res = i2c_transfer (client_handle, &config, &desc[0], 2, NULL, NULL, 0, &transferred);
   if (res == I2C_SUCCESS)
   {
       if (transferred == (desc[0].length + desc[1].length))
       {
           // transfer passed
       }
       else
       {
           // transfer failed. number of bytes transferred = transferred
       }
   }

   // Close the connection handle to the i2c bus instance
   res = i2c_close (client_handle);
 }

 void client_callback (uint32_t status, uint32 transferred, void *ctxt)
 {
   uint32 i;
   client_ctxt *c_ctxt = (client_ctxt *) ctxt;
   i2c_descriptor *desc;

   if ((i2c_status) status != I2C_SUCCESS)
   {
     // Transfer failed
   }

   for (i = 0; i < c_ctxt->num_descs; i++)
   {
     desc = (c_ctxt->desc + i);
     if (transferred >= desc->length)
     {
       // descriptor i passed
       transferred -= desc->length;
     }
     else
     {
       // descriptor i failed
       // subsequenct descriptors failed
       break;
     }
   }
 }

 @endcode
*/

/** @} */ /* end_addtogroup i2c_api */

#ifndef __I2C_API_H__
#define __I2C_API_H__

#include "i2c_types.h"

/** @addtogroup i2c_api
@{ */

/** @name Preprocessor Definitions and Constants
@{ */

/** FLAGs for transfer descriptor. */
#define I2C_FLAG_START      0x00000001 /**< Specifies that the transfer begins with a START - S. */
#define I2C_FLAG_STOP       0x00000002 /**< Specifies that the transfer ends with a STOP - P. */
#define I2C_FLAG_WRITE      0x00000004 /**< Must be set to indicate a WRITE transfer. */
#define I2C_FLAG_READ       0x00000008 /**< Must be set to indicate a READ transfer. */
#define I2C_FLAG_TIMESTAMP  0x00000010 /**< Must be set to capture the START or
                                            STOP timestamp. When set with the START
                                            bit, START timestamp is recorded. When
                                            set with STOP bit, STOP timestamp is
                                            recorded. When set with both START and
                                            STOP, only START timestamp is recorded. */

/** Bus speeds supported by the master implementation. */
#define I2C_STANDARD_MODE_FREQ_KHZ          100     /**< I2C stadard speed 100 KHz. */
#define I2C_FAST_MODE_FREQ_KHZ              400     /**< I2C fast mode speed 400 KHz. */
#define I2C_FAST_MODE_PLUS_FREQ_KHZ         1000    /**< I2C fast mode plus speed 1 MHz */
#define I3C_SDR_DATA_RATE_12500_KHZ         12500   /**< I3C SDR speed 12.5 MHz. */
#define I3C_SDR_DATA_RATE_10000_KHZ         10000   /**< I3C SDR speed 10 MHz. */

/** Flags used for a simple I2C write trasnfer. */
#define I2C_WRITE_TRANSFER  (I2C_FLAG_START | I2C_FLAG_WRITE | I2C_FLAG_STOP)

/** Flags used for a simple I2C read trasnfer. */
#define I2C_READ_TRANSFER   (I2C_FLAG_START | I2C_FLAG_READ  | I2C_FLAG_STOP)

/** Flags used for the write transfer of a Write followed by Read sequence. */
#define I2C_WR_RD_WRITE     (I2C_FLAG_START | I2C_FLAG_WRITE)

/** Flags used for the read transfer of a Write followed by Read sequence. */
#define I2C_WR_RD_READ      I2C_READ_TRANSFER

/** Flags used for I2C address query. #i2c_descriptor buffer and length are set
    to NULL and 0 respectively. */
#define I2C_ADDRESS_QUERY   (I2C_FLAG_START)

/** Flags used to send a separate I2C STOP bit on the bus. #i2c_descriptor buffer
    and length are set to NULL and 0 respectively. */
#define I2C_STOP_COMMAND    (I2C_FLAG_STOP)

/** Flags used for I2C bus clear. #i2c_descriptor buffer and length are set to
    NULL and 0 respectively. */
#define I2C_BUS_CLEAR       0

/** Flags used for the read transfer of a Write followed by Read sequence. */
#define I2C_WR_RD_READ      I2C_READ_TRANSFER

#define I2C_TRANSFER_MASK      (I2C_FLAG_WRITE | I2C_FLAG_READ)

#define I2C_SUCCESS(x)  (x == I2C_SUCCESS)
#define I2C_ERROR(x)    (x != I2C_SUCCESS)

#define VALID_FLAGS(x) (((x & I2C_TRANSFER_MASK) == I2C_FLAG_READ) || ((x & I2C_TRANSFER_MASK) == I2C_FLAG_WRITE))

/** @} */ /* end_namegroup */
/** @} */ /* end_addtogroup i2c_api */

/** @addtogroup i2c_api
@{ */

/** @name Type Definitions
@{ */

/**
  Instance of the I2C core that the client wants to use. This instance is
  passed in i2c_open().
*/
typedef enum
{
    I2C_INSTANCE_001 = 1,      /**< Instance 01. */
    I2C_INSTANCE_002,          /**< Instance 02. */
    I2C_INSTANCE_003,          /**< Instance 03. */
    I2C_INSTANCE_004,          /**< Instance 04. */
    I2C_INSTANCE_005,          /**< Instance 05. */
    I2C_INSTANCE_006,          /**< Instance 06. */
    I2C_INSTANCE_007,          /**< Instance 07. */
    I2C_INSTANCE_008,          /**< Instance 08. */
    I2C_INSTANCE_009,          /**< Instance 09. */
    I2C_INSTANCE_010,          /**< Instance 10. */
    I2C_INSTANCE_011,          /**< Instance 11. */
    I2C_INSTANCE_012,          /**< Instance 12. */
    I2C_INSTANCE_013,          /**< Instance 13. */
    I2C_INSTANCE_014,          /**< Instance 14. */
    I2C_INSTANCE_015,          /**< Instance 15. */
    I2C_INSTANCE_016,          /**< Instance 16. */
#if 0	// We only expose top level QUP SE's
    I2C_INSTANCE_017,          /**< Instance 17. */
    I2C_INSTANCE_018,          /**< Instance 18. */
    I2C_INSTANCE_019,          /**< Instance 19. */
    I2C_INSTANCE_020,          /**< Instance 20. */
    I2C_INSTANCE_021,          /**< Instance 21. */
    I2C_INSTANCE_022,          /**< Instance 22. */
    I2C_INSTANCE_023,          /**< Instance 23. */
    I2C_INSTANCE_024,          /**< Instance 24. */
#endif
    I2C_INSTANCE_MAX,          /**< Instance Max. */

} i2c_instance;

/**
  Function status codes returned by the APIs or the transfer_status field of
  the callback.
*/
typedef enum
{
    I2C_SUCCESS = 0,
    I2C_ERROR_INVALID_PARAMETER,
    I2C_ERROR_UNSUPPORTED_CORE_INSTANCE,
    I2C_ERROR_API_INVALID_EXECUTION_LEVEL,
    I2C_ERROR_API_NOT_SUPPORTED,
    I2C_ERROR_API_ASYNC_MODE_NOT_SUPPORTED,
    I2C_ERROR_API_PROTOCOL_MODE_NOT_SUPPORTED,

    I2C_ERROR_HANDLE_ALLOCATION,
    I2C_ERROR_HW_INFO_ALLOCATION,
    I2C_ERROR_BUS_NOT_IDLE,
    I2C_ERROR_TRANSFER_TIMEOUT,
	I2C_ERROR_MEM_ALLOC_FAIL,

    I2C_ERROR_INPUT_FIFO_OVER_RUN,
    I2C_ERROR_OUTPUT_FIFO_UNDER_RUN,
    I2C_ERROR_INPUT_FIFO_UNDER_RUN,
    I2C_ERROR_OUTPUT_FIFO_OVER_RUN,

    I2C_ERROR_COMMAND_OVER_RUN,
    I2C_ERROR_COMMAND_ILLEGAL,
    I2C_ERROR_COMMAND_FAIL,
    I2C_ERROR_INVALID_CMD_OPCODE,

    I2C_ERROR_START_STOP_UNEXPECTED,
    I2C_ERROR_DATA_NACK,
    I2C_ERROR_ADDR_NACK,
    I2C_ERROR_ARBITRATION_LOST,

    I2C_ERROR_PLATFORM_INIT_FAIL,
    I2C_ERROR_PLATFORM_DEINIT_FAIL,
    I2C_ERROR_PLATFORM_CRIT_SEC_FAIL,
    I2C_ERROR_PLATFORM_SIGNAL_FAIL,
    I2C_ERROR_PLATFORM_GET_CONFIG_FAIL,
    I2C_ERROR_PLATFORM_GET_CLOCK_CONFIG_FAIL,
    I2C_ERROR_PLATFORM_REG_INT_FAIL,
    I2C_ERROR_PLATFORM_DE_REG_INT_FAIL,
    I2C_ERROR_PLATFORM_CLOCK_ENABLE_FAIL,
    I2C_ERROR_PLATFORM_GPIO_ENABLE_FAIL,
    I2C_ERROR_PLATFORM_CLOCK_DISABLE_FAIL,
    I2C_ERROR_PLATFORM_GPIO_DISABLE_FAIL,

    I2C_TRANSFER_CANCELED,
    I2C_TRANSFER_FORCE_TERMINATED,
    I2C_TRANSFER_COMPLETED,
    I2C_TRANSFER_INVALID,

    I2C_ERROR_HANDLE_ALREADY_IN_QUEUE,
    I2C_ERROR_DMA_REG_FAIL,
    I2C_ERROR_DMA_EV_CHAN_ALLOC_FAIL,
    I2C_ERROR_DMA_TX_CHAN_ALLOC_FAIL,
    I2C_ERROR_DMA_RX_CHAN_ALLOC_FAIL,
    I2C_ERROR_DMA_TX_CHAN_START_FAIL,
    I2C_ERROR_DMA_RX_CHAN_START_FAIL,
    I2C_ERROR_DMA_TX_CHAN_STOP_FAIL,
    I2C_ERROR_DMA_RX_CHAN_STOP_FAIL,
    I2C_ERROR_DMA_TX_CHAN_RESET_FAIL,
    I2C_ERROR_DMA_RX_CHAN_RESET_FAIL,
    I2C_ERROR_DMA_EV_CHAN_DE_ALLOC_FAIL,
    I2C_ERROR_DMA_TX_CHAN_DE_ALLOC_FAIL,
    I2C_ERROR_DMA_RX_CHAN_DE_ALLOC_FAIL,
    I2C_ERROR_DMA_INSUFFICIENT_RESOURCES,
    I2C_ERROR_DMA_PROCESS_TRANSFER_FAIL,
    I2C_ERROR_DMA_EVT_OTHER,
    I2C_ERROR_FW_LOAD_FALIURE,
    I2C_ERROR_INVALID_FW_VERSION,
} i2c_status;

/**
  Protocol modes. Underlying implementation may not support some modes. Refer
  i2c_transfer() for more details.
*/
typedef enum
{
    I2C = 0,
    SMBUS,
    I3C_SDR,
    I3C_HDR_DDR,
    I3C_BROADCAST_CCC,
    I3C_DIRECT_CCC,
    I3C_IBI_READ,

} i2c_mode;

/** @} */ /* end_namegroup */
/** @} */ /* end_addtogroup i2c_api */

/** @addtogroup i2c_api
@{ */

/** @name Structure Definitions
@{ */

/**
  Slave configuration parameters that the client uses to communicate to a slave.
*/
typedef struct
{
    uint32      bus_frequency_khz;          /**< Bus speed in kHz. */
    uint32      slave_address;              /**< 7-bit I2C slave address. 7-bit I3C dynamic address */
    i2c_mode    mode;                       /**< Protocol mode. Refer #i2c_mode */
    uint32      slave_max_clock_stretch_us; /**< Maximum time in microseconds that an I2C slave may hold the SCL line low. Not applicable for I3C*/
    uint32      core_configuration1;        /**< Core Specific Configuration. Recommended 0. */
    uint32      core_configuration2;        /**< Core Specific Configuration. Recommended 0. */

} i2c_slave_config;

/**
  Transfer descriptor that the client uses to perform a read or a write.
  Clients pass this descriptor or an array of these descriptors to the
  i2c_transfer() API.
*/
typedef struct
{
    uint8   *buffer;        /**< Buffer for the data transfer. */
    uint32  length;         /**< Length of the data to be transferred in bytes. */
    uint32  flags;          /**< Flags used for the transfer descriptor. */

} i2c_descriptor;

/** @} */ /* end_namegroup */
/** @} */ /* end_addtogroup i2c_api */

/** @addtogroup i2c_api
@{ */

/** @name Function Declarations
@{ */

/**
  Transfer callback. Applicable only to asynchronous transfer mode. Refer to
  i2c_transfer().

  Declares the type of callback function that needs to be defined by the client.
  The callback is called when the data is completely transferred on the bus or
  the transfer ends due to an error or cancellation.

  Clients pass the callback function pointer and the callback context to the
  driver in the i2c_transfer () API.

  @note
  The callback is called in a thread that processes multiple clients. Calling
  any of the APIs defined here in this callback is not recommended. Processing
  in the callback function must be kept to a minimum to avoid system latencies.
  In some implementations the callback occurs from interrupt context and calling
  any of the APIs defined here will result in an
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL error.

  @param[out] transfer_status   The completion status of the transfer; see
                                #i2c_status for actual values.
  @param[out] transferred       Total number of bytes transferred. If
                                transfer_status is I2C_SUCCESS, then this value
                                is equal to the sum of lengths of all the
                                descriptors passed in a call to i2c_transfer().
                                If transfer_status is not I2C_SUCCESS, this
                                value is less than the sum of lengths of all the
                                descriptors passed in a call to i2c_transfer().
  @param[out] ctxt              The callback context parameter that was passed
                                in the call to i2c_transfer().
*/
typedef void (*callback_func) (uint32 transfer_status, uint32 transferred, void *ctxt);

/**
  Called by the client code to initialize the respective I2C instance. On
  success, i2c_handle points to the handle for the I2C instance.  The API
  allocates resources for use by the client handle and the I2C instance.
  These resources are release in the i2c_close() call.

  @param[in]  instance            I2C instance that the client intends to
                                  initialize; see #i2c_instance for details.
  @param[out] i2c_handle          Pointer location to be filled by the
                                  driver with a handle to the instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_UNSUPPORTED_CORE_INSTANCE --  Unsupported core instance. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level. \n
  I2C_ERROR_HANDLE_ALLOCATION -- Handle allocation error. \n
  I2C_ERROR_HW_INFO_ALLOCATION -- Hardware information allocation error. \n
  I2C_ERROR_PLATFORM_INIT_FAIL -- Platform initialization failure. \n
  I2C_ERROR_PLATFORM_CRIT_SEC_FAIL -- Critical section initialization failure. \n
  I2C_ERROR_PLATFORM_GET_CONFIG_FAIL -- Could not get HW configuration. \n
  I2C_ERROR_PLATFORM_REG_INT_FAIL -- Platform registration internal failure. \n
  I2C_ERROR_DMA_REG_FAIL -- Registration failure with bus interface. \n
  I2C_ERROR_DMA_EV_CHAN_ALLOC_FAIL -- Event channel allocation failure. \n
  I2C_ERROR_DMA_TX_CHAN_ALLOC_FAIL -- TX channel allocation failure. \n
  I2C_ERROR_DMA_RX_CHAN_ALLOC_FAIL -- RX channel allocation failure. \n
  I2C_ERROR_DMA_TX_CHAN_START_FAIL -- TX channel start failure. \n
  I2C_ERROR_DMA_RX_CHAN_START_FAIL -- RX channel start failure.
*/
i2c_status
i2c_open
(
    i2c_instance instance,
    void **i2c_handle
);

/**
  De-initializes the I2C instance and releases any resources allocated by the
  i2c_open() API.

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level. \n
  I2C_ERROR_PLATFORM_DEINIT_FAIL -- Platform de-initialization failure. \n
  I2C_ERROR_PLATFORM_DE_REG_INT_FAIL -- Platform de-registration internal failure. \n
  I2C_ERROR_DMA_TX_CHAN_STOP_FAIL -- TX channel stop failure. \n
  I2C_ERROR_DMA_RX_CHAN_STOP_FAIL -- RX channel stop failure. \n
  I2C_ERROR_DMA_TX_CHAN_RESET_FAIL -- TX channel reset failure. \n
  I2C_ERROR_DMA_RX_CHAN_RESET_FAIL -- RX channel reset failure. \n
  I2C_ERROR_DMA_EV_CHAN_DE_ALLOC_FAIL -- Event channel de-allocation failure. \n
  I2C_ERROR_DMA_TX_CHAN_DE_ALLOC_FAIL -- TX channel de-allocation failure. \n
  I2C_ERROR_DMA_RX_CHAN_DE_ALLOC_FAIL -- RX channel de-allocation failure.
*/
i2c_status
i2c_close
(
    void *i2c_handle
);

/**
  Transfer callback. Applicable only to asynchronous transfer mode. Refer to
  i2c_transfer(). Underlying implmentation may not support the API and return
  I2C_ERROR_API_NOT_SUPPORTED.

  Cancels a transfer. A transfer that has been initiated successfully
  by calling i2c_transfer() may be canceled. Based on the internal state
  of the transfer, this function will either immediately cancel the transfer or end
  the transfer at a later time. If the function returns I2C_TRANSFER_CANCELED,
  the transfer was canceled successfully and the client callback will
  not be called. If the function returns I2C_TRANSFER_FORCE_TERMINATED,
  the client must wait for the callback to occur in order to make sure that the
  transfer has ended. If the function returns I2C_TRANSFER_COMPLETED, the
  transfer has already completed and the callback has already been called.
  If the function returns I2C_TRANSFER_INVALID, the client is trying to cancel a
  transfer that is not queued to the HW with an earlier call to
  i2c_transfer().

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level. \n
  I2C_ERROR_API_NOT_SUPPORTED -- API is not supported. \n
  I2C_TRANSFER_INVALID -- Invalid transfer. \n
  I2C_TRANSFER_CANCELED -- Transfer is canceled successfully. \n
  I2C_TRANSFER_COMPLETED -- Transfer completed before canceling. \n
  I2C_TRANSFER_FORCE_TERMINATED -- Transfer aborted on the bus.
*/
i2c_status
i2c_cancel_transfer
(
    void *i2c_handle
);

/**
  Enables the clocks, gpios and any other power resources used by the I2C
  instance.

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level. \n
  I2C_ERROR_PLATFORM_CLOCK_ENABLE_FAIL -- Platform clock enable failure. \n
  I2C_ERROR_PLATFORM_GPIO_ENABLE_FAIL -- Platform GPIO enable failure.
*/
i2c_status
i2c_power_on
(
    void *i2c_handle
);

/**
  Disables the clocks, gpios and any other power resources used by the I2C
  instance.

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level. \n
  I2C_ERROR_PLATFORM_CLOCK_DISABLE_FAIL -- Platform clock disable failure. \n
  I2C_ERROR_PLATFORM_GPIO_DISABLE_FAIL -- Platform GPIO disable failure.
*/
i2c_status
i2c_power_off
(
    void *i2c_handle
);

/**
  Locks the I2C bus for exclusive access. Underlying implmentation may not
  support the API and return I2C_ERROR_API_NOT_SUPPORTED.

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_NOT_SUPPORTED -- API is not supported. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level.
*/
i2c_status
i2c_lock
(
    void *i2c_handle
);

/**
  Releases the I2C bus exclusive access. Underlying implmentation may not
  support the API and return I2C_ERROR_API_NOT_SUPPORTED.

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_NOT_SUPPORTED -- API is not supported. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level.
*/
i2c_status
i2c_unlock
(
    void *i2c_handle
);

/**
  Performs an I2C transfer synchronously or asynchronously. A callback
  function is passed as a parameter to this function.

  If the callback parameter is NULL, the function executes synchronously. When
  the function returns, the transfer is complete.

  If the callback parameter is not NULL, the functions executes
  asynchronously. The function returns by queueing the transfer to the
  underlying driver. Once the transfer is complete, the driver calls the
  callback provided. Refer to #callback_func for callback definition. If the
  function returns a failure, the transfer has not been queued and no
  callback will occur. If the transfer returns I2C_SUCCESS, the transfer has
  been queued and a further status of the transfer can only be obtained when
  the callback is called. After a client calls this API, it must wait for the
  completion callback to occur before it can call the API again. If the
  client wishes to queue mutliple transfers, it must use an array of
  descriptors of type i2c_descriptor instead of calling the API multiple
  times.

  Underlying implementation may not support asynchronous mode. The return
  value will indicate the lack of support in which case client must pass a
  NULL value for func.

  @param[in]  i2c_handle          Handle to the I2C instance.
  @param[in]  config              Slave configuration. See #i2c_slave_config
                                  for details.
  @param[in]  desc                I2C transfer descriptor. See
                                  #i2c_descriptor for details. This
                                  can be an array of descriptors.
  @param[in]  num_descriptors     Number of descriptors in the descriptor array.
  @param[in]  func                The callback function that is called at the
                                  completion of the transfer. This callback
                                  occurs in a dispatch context. The callback
                                  must do minimal processing and must
                                  not call any API defined here. If the
                                  callback is NULL, the function executes
                                  synchronously.
  @param[in]  ctxt                The context that the client passes here is
                                  returned as is in the callback function.
  @param[in]  delay_us            A delay in microseconds that specifies the
                                  time to wait before starting the transfer.
  @param[out] transferred         Total number of bytes transferred. This
                                  value is valid only for synchronous mode.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level. \n
  I2C_ERROR_API_ASYNC_MODE_NOT_SUPPORTED -- Asynchronous transfer mode is not supported. \n
  I2C_ERROR_API_PROTOCOL_MODE_NOT_SUPPORTED -- The mode parameter passed in #i2c_slave_config is not supported. \n
  I2C_ERROR_TRANSFER_TIMEOUT -- Transfer timed out. \n
  I2C_ERROR_HANDLE_ALREADY_IN_QUEUE -- Client IO is pending. \n
  I2C_ERROR_DMA_INSUFFICIENT_RESOURCES -- Insufficient resources to execute the transfer. \n
  I2C_ERROR_DMA_PROCESS_TRANSFER_FAIL -- Transfer processing failure in bus interface.
*/
i2c_status
i2c_transfer
(
    void *i2c_handle,
    i2c_slave_config *config,
    i2c_descriptor *desc,
    uint16 num_descriptors,
    callback_func func,
    void *ctxt,
    uint32 delay_us,
    uint32 *transferred
);

/**
  Client calls this API to get the timestamp for the START bit for the
  previous transfer that completed successfully. Underlying implmentation
  may not support the API and return I2C_ERROR_API_NOT_SUPPORTED.

  In order to obtain the timestamp, the client must have set the
  I2C_FLAG_TIMESTAMP and I2C_FLAG_START bit in a previous READ or WRITE
  transfer. Once the transfer completed successfully, the client can call
  this API to obtain the timestamp value.

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_NOT_SUPPORTED -- API is not supported. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level.
*/
uint32
i2c_get_start_timestamp
(
    void *i2c_handle
);

/**
  Client calls this API to get the timestamp for the STOP bit for the
  previous transfer that completed successfully. Underlying implmentation
  may not support the API and return I2C_ERROR_API_NOT_SUPPORTED.

  In order to obtain the timestamp, the client must have set the
  I2C_FLAG_TIMESTAMP and I2C_FLAG_STOP bit in a previous READ or WRITE
  transfer. Once the transfer completed successfully, the client can call
  this API to obtain the timestamp value.

  @param[in] i2c_handle           Handle to the I2C instance.

  @return
  I2C_SUCCESS -- Function was successful. \n
  I2C_ERROR_INVALID_PARAMETER -- Invalid parameter. \n
  I2C_ERROR_API_NOT_SUPPORTED -- API is not supported. \n
  I2C_ERROR_API_INVALID_EXECUTION_LEVEL -- Invalid execution level.
*/
uint32
i2c_get_stop_timestamp
(
    void *i2c_handle
);

/* i2c_read */
/** @ingroup i2c_read
  @par Summary
  Performs an i2c read. Optionally reads from an offset.

  @param[in]  i2c_handle            Handle to the I2C core.
  @param[in]  config                Slave configuration. See #i2c_config for
                                    details.
  @param[in]  offset                If offset_len is non zero, this parameter
                                    specifies either the register or memory
                                    offset to read data from.
                                    #i2c_transfer for details.
  @param[in]  offset_len            Length of the offset. For simple slaves,
                                    this is 0. For register addressed slaves, it
                                    is 1 and for memory addressed slaves it is
                                    2.
  @param[out] buffer                Buffer into which data is read.
  @param[in]  buffer_len            Length of data that needs to be read.
                                    the slave.
  @param[out] read                  Number of bytes actually read.
  @param[in]  timeout_ms            Timeout value in milliseconds for transfer
                                    to complete.

  @return 
  See #i2c_status for details.
*/
i2c_status
i2c_read(
    void *i2c_handle,
    i2c_slave_config *config,
    uint16 offset,
    uint16 offset_len,
    uint8  *buffer,
    uint16 buffer_len,
    uint32 *read,
    uint32 timeout_ms
);

/* i2c_write */
/** @ingroup i2c_write
  @par Summary
  Performs an i2c write. Optionally writes to an offset.

  @param[in]  i2c_handle            Handle to the I2C core.
  @param[in]  config                Slave configuration. See #i2c_config for
                                    details.
  @param[in]  offset                If offset_len is non zero, this parameter
                                    specifies either the register or memory
                                    offset to write data to.
                                    #i2c_transfer for details.
  @param[in]  offset_len            Length of the offset. For simple slaves,
                                    this is 0. For register addressed slaves, it
                                    is 1 and for memory addressed slaves it is
                                    2.
  @param[out] buffer                Buffer from which data is written.
  @param[in]  buffer_len            Length of data that needs to be written.
                                    the slave.
  @param[out] written               Number of bytes actually written.
  @param[in]  timeout_ms            Timeout value in milliseconds for transfer
                                    to complete.

  @return 
  See #i2c_status for details.
*/
i2c_status
i2c_write(
    void *i2c_handle,
    i2c_slave_config *config,
    uint16 offset,
    uint16 offset_len,
    uint8  *buffer,
    uint16 buffer_len,
    uint32 *written,
    uint32 timeout_ms
);


/** @} */ /* end_namegroup */
/** @} */ /* end_addtogroup i2c_api */

#endif /* __I2C_API_H__ */
