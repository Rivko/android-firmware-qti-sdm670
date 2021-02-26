#ifndef _SPIDEVICE_H_
#define _SPIDEVICE_H_
/*==================================================================================================

FILE: SpiDevice.h

DESCRIPTION:
   This file defines an API for the SPI common device layer.

====================================================================================================

   Edit History

When      Who  What, where, why
--------  ---  ------------------------------------------------------------
06/11/12  dk   Added review comments.
03/27/12  ag   Added batch transfer updates.
09/26/11  ag   Created

====================================================================================================
                     Copyright (c) 2011-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     Qualcomm Proprietary/GTDR
==================================================================================================*/

#include "SpiDeviceTypes.h"
#include "SpiDeviceError.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   SPIDEVICE_ERR_BASE = SPIDEVICE_RES_ERROR_CLS_DEVICE,
   SPIDEVICE_ERR_CLOCK_SETTING_FAILED,
   SPIDEVICE_ERR_INVALID_CONFIG_POINTER,
   SPIDEVICE_ERR_INVALID_DEV_PARAMETERS,
   SPIDEVICE_ERR_INVALID_BUFFER_POINTER,
   SPIDEVICE_ERR_INVALID_BUFFER_LEN,
   SPIDEVICE_ERR_INVALID_RUNSTATE,
   SPIDEVICE_ERR_INVALID_DEV_HANDLE,
   SPIDEVICE_ERR_INVALID_PTR,
   SPIDEVICE_ERR_INVALID_NUM_BATCH_TRANSFER_VECS,
   SPIDEVICE_TIMEOUT,
   SPIDEVICE_FIFO_ERROR,
   SPIDEVICE_TRANSFER_CANCELED,
   SPIDEVICE_INVALID_PARAMETER,
} SPIDEVICE_ERROR;

typedef enum
{
   SPI_CLK_NORMAL,     // SPI CLK turned off during idle state
   SPI_CLK_ALWAYS_ON,  // SPI CLK left running during idle state
} SpiClockModeType;

typedef enum
{
   SPI_CLK_IDLE_LOW,   // clock signal is driven low when idle
   SPI_CLK_IDLE_HIGH,  // clock signal is driven high when idle
} SpiClockPolarityType;

typedef enum
{
   SPI_INPUT_FIRST_MODE,   // leading edge of clock is used for sampling input data
   SPI_OUTPUT_FIRST_MODE,  // leading edge of clock is used for driving output data
} SpiShiftModeType;

typedef enum
{
   SPI_CS_ACTIVE_LOW,   // chip select is driven low when asserted
   SPI_CS_ACTIVE_HIGH,  // chip select is driven high when asserted
} SpiCSPolarityType;

typedef enum
{
   SPI_CS_DEASSERT,       // chip select is deasserted between N-bit transfers
   SPI_CS_KEEP_ASSERTED,  // chip select is asserted as long as the core is in the run state
} SpiCSModeType;

typedef enum
{
   SPI_CORE_MODE_SLAVE,   // SPI core is set in slave mode
   SPI_CORE_MODE_MASTER,  // SPI core is set in master mode
} SpiCoreModeType;

typedef enum
{
   SPI_LOOPBACK_DISABLED,  // loopback mode is disabled
   SPI_LOOPBACK_ENABLED,   // loopback mode is enabled
} SpiLoopbackModeType;

typedef enum
{
   SPIDEVICE_POWER_STATE_0,  // bus and clocks off
   SPIDEVICE_POWER_STATE_1,  // bus on clocks off
   SPIDEVICE_POWER_STATE_2,  // bus on clocks on
} SpiDevice_PowerStates;

typedef enum
{
   CS_HIGH,  // chip select is manually forced high
   CS_LOW,   // chip select is manually forced low
   CS_AUTO,  // chip select is automatically controlled by the SPI core
} SpiCSControl;

typedef struct
{
   void    *QupHandle;       // opaque handle used to identify QUP instance
   void    *QupVirtAddr;     // QUP virtual address
   void    *QupPhysAddr;     // QUP physical address

   boolean  InterruptBased;  // TRUE to use interrupts, FALSE to use polling
   boolean  BamSupported;    // TRUE if BAM mode is supported
   boolean  UseCmdDesc;      // TRUE to use command descriptors for register configuration
   uint32   BamThreshold;    // a transfer size greater or equal to this triggers BAM mode

   void    *TcsrVirtAddr;           // set this to NULL if TCSR interrupt is not required
   uint32   TcsrInterruptBitMask;   // TCSR interrupt register bit mask
   uint32   TcsrInterruptBitShift;  // TCSR interrupt register bit shift
} SpiDevice_Config;

typedef struct
{
   SpiClockModeType eClockMode;          // Clock mode type to be used for the SPI core.
                                         // This parameter determines if the Clock could
                                         // be running even if there is no ongoing transfer.
   SpiClockPolarityType eClockPolarity;  // Clock polarity type to be used for the SPI core.
   SpiShiftModeType eShiftMode;          // Shift mode type to be used for SPI core.  This
                                         // parameter specifies whether the SPI core operates
                                         // in OUTPUT or INPUT FIRST Mode. This specifies whether
                                         // the shift register latches the DATA at the input pin
                                         // on the rising or falling edge.
   uint32 u32DeassertionTime;            // The minimum time delay in nano second, between two
                                         // word(N-bits) transfers.  This sets the minimum time
                                         // in nanoseconds that the chip select is deasserted
                                         // between SPI transfers (in master mode).  The
                                         // deassertion clock ticks will be configured so that
                                         // the deassertion time will be as close as possible to,
                                         // but not less than, the time specified by this
                                         // parameter.
   uint32 u32MinSlaveFrequencyHz;        // The Minimum frequency above which the spi core
                                         // should be run.  Normally this is set to zero.
   uint32 u32MaxSlaveFrequencyHz;        // This is desired frequency of operation.  If the core
                                         // cannot run at this frequency, a value below this but
                                         // greater than u32MinSlaveFrequencyHz will be chosen
                                         // depending on the frequencies available in the
                                         // clkregim SPI clock steps configuration.
   SpiCSPolarityType eCSPolarity;        // CS polarity type to be used for the SPI core.  This
                                         // field tells whethere the Chip select is active high
                                         // or active low.
   SpiCSModeType eCSMode;                // CS Mode to be used for the SPI core.  This specifies
                                         // whether the Chip select is asserted through the
                                         // entire Transaction or whether it is deasserted in
                                         // between SPI transfers of each transaction.
   boolean bHSMode;                      // Flag to specify whether to enable high speed mode.
} SpiDeviceParametersType;

typedef struct
{
   uint32          nSlaveNumber;  // slave number (which chip select the slave uses)
   SpiCoreModeType eCoreMode;     // specifies wether the core acts as master or slave
} SpiDeviceBoardInfoType;

typedef struct
{
   uint32              nNumBits;       // number of bits (N) in one logical SPI transfer
   SpiLoopbackModeType eLoopbackMode;  // specifies whether loopback mode is enabled
} SpiTransferParametersType;

typedef struct
{
   SpiDeviceParametersType   deviceParameters;
   SpiDeviceBoardInfoType    deviceBoardInfo;
   SpiTransferParametersType transferParameters;
} SpiDeviceInfoType;

typedef struct
{
  void *virtualAddr;   // virtual address of the data buffer
  void *physicalAddr;  // physical address of the data buffer
} SpiDataAddrType;

typedef struct
{
   uint32 nNumInstances;           // number of SPI cores in the system
   uint32 uInputFifoSize;          // input FIFO size of HW in bytes
   uint32 uOutputFifoSize;         // output FIFO size of HW in bytes
   uint32 uInputBlockSize;         // input block size in bytes
   uint32 uOutputBlockSize;        // output block size in bytes
   uint32 uMaxNumInputTransfers;   // max number of input transfers per transaction
   uint32 uMaxNumOutputTransfers;  // max number of output transfers per transaction
} SpiInfoType;

typedef struct
{
   SpiDataAddrType *pData;
   uint32           uLen;
} SpiBufferType;

typedef struct
{
   SpiBufferType *pReadBuffVec;
   uint32         uNumReadVecs;
   SpiBufferType *pWriteBuffVec;
   uint32         uNumWriteVecs;
} SpiLinkedTransferType;

typedef struct
{
   void   *pUser;
   uint32  result;
} SpiDeviceBamResult;

typedef struct
{
   uint32  nTransferStatus;
   void   *pArg;
} SpiDevice_AsyncResult;

typedef void (*SPI_DEV_ASYNC_CB)(SpiDevice_AsyncResult *);

typedef void* SPIDEVICE_HANDLE;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*  @brief Initializes the device.
 *
 *  This Function Initializes the device and creates the
 *  necessary data structures to support other calls into the
 *  device.
 *
 *  @param[in]  pConfig  pointer to device configuration
 *  @param[out] phDev    pointer to device handle
 *  @return              error status
 */
uint32
SpiDevice_Init
(
   SpiDevice_Config *pConfig,
   SPIDEVICE_HANDLE *phDev
);

/*  @brief Deinitializes the device.
 *
 *  This Function Deinitializes the device and releases
 *  resources acquired during init.
 *
 *  @param[in] hDev  device handle
 *  @return          error status
 */
uint32
SpiDevice_DeInit
(
   SPIDEVICE_HANDLE hDev
);

/*  @brief Peform a bi-directional transfer
 *
 *  Read a buffer from SPI device and/or Write a buffer of data to a SPI device
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *                           client function can set it to NULL if it
 *                           needs to use the settings used by previous
 *                           call with the same device context
 *  @param[in] pReadBuffer   pointer where incoming data will be stored
 *  @param[in] uReadBufLen   number of bytes of valid data to be
 *                           stored in pReadbuffer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @return                  error status
 */
uint32
SpiDevice_WriteRead
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen
);

/*  @brief Read a buffer from spi device.
 *
 *  @param[in] hDev         device handle
 *  @param[in] pQupConfig   pointer to the QUP config for data transfer
 *  @param[in] pReadBuffer  pointer where incoming data will be stored
 *  @param[in] uReadBufLen  number of bytes of valid data to be
 *                          stored in pReadbuffer
 *  @return                 error status
 */
uint32
SpiDevice_Read
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen
);

/*  @brief Write a buffer to spi device.
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @return                  error status
 */
uint32
SpiDevice_Write
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen
);

/*  @brief Peform a bi-direction transfer
 *
 *  Schedules a read/write/read-write(bi-directional) transfer.
 *  Once the transfer is complete or an error occurs
 *  the callback will be called.
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *  @param[in] pReadBuffer   pointer where incoming data will be stored
 *  @param[in] uReadBufLen   number of bytes of valid data to be
 *                           stored in pReadbuffer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @param[in] pCallbackFn   pointer to a callback function to be called
 *                           when transfer finishes or aborts
 *  @param[in] pArg          pointer to be passed to the callback function
 *  @return                  error status
 */
uint32
SpiDevice_AsyncWriteRead
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen,
   SPI_DEV_ASYNC_CB   pCallbackFn,
   void              *pArg
);

/*  @brief Schedules a buffer read from spi device.
 *
 *  Schedules a buffer read from spi device.
 *  Once the read is complete or an error occurs
 *  the callback will be called.
 *
 *  @param[in] hDev         device handle
 *  @param[in] pQupConfig   pointer to the QUP config for data transfer
 *  @param[in] pReadBuffer  pointer where incoming data will be stored
 *  @param[in] uReadBufLen  number of bytes of valid data to be
 *                          stored in pReadbuffer
 *  @param[in] pCallbackFn  pointer to a callback function to be called
 *                          when transfer finishes or aborts
 *  @param[in] pArg         pointer to be passed to the callback function
 *  @return                 error status
 */
uint32
SpiDevice_AsyncRead
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pReadBuffer,
   uint32             uReadBufLen,
   SPI_DEV_ASYNC_CB   pCallbackFn,
   void              *pArg
);

/*  @brief Schedules a buffer write to spi device.
 *
 *  Schedules a buffer write to spi device. Once the write is
 *  complete or an error occurs the callback will be called.
 *
 *  @param[in] hDev          device handle
 *  @param[in] pQupConfig    pointer to the QUP config for data transfer
 *  @param[in] pWriteBuffer  pointer where outgoing data is stored
 *  @param[in] uWriteBufLen  number of bytes of valid data stored in
 *                           pWritebuffer
 *  @param[in] pCallbackFn   pointer to a callback function to be called
 *                           when transfer finishes or aborts
 *  @param[in] pArg          pointer to be passed to the callback function
 *  @return                  error status
 */
uint32
SpiDevice_AsyncWrite
(
   SPIDEVICE_HANDLE   hDev,
   SpiDeviceInfoType *pQupConfig,
   SpiDataAddrType   *pWriteBuffer,
   uint32             uWriteBufLen,
   SPI_DEV_ASYNC_CB   pCallbackFn,
   void              *pArg
);

/*  @brief Does a batch of transfers in a sequence.
 *
 *  @param[in]  hDev              device handle
 *  @param[in]  pTransfer         transfer handle
 *  @param[in]  uNumTransferVecs  number of transfer vectors
 *  @param[in]  pQupConfig        pointer to the QUP config for data transfer
 *  @param[out] puNumCompleted    pointer to return number of completed transfers
 *  @return                       error status
 */
uint32
SpiDevice_BatchTransfer
(
   SPIDEVICE_HANDLE       hDev,
   SpiLinkedTransferType *pTransfer,
   uint32                 uNumTransferVecs,
   SpiDeviceInfoType     *pQupConfig,
   uint32                *puNumCompleted
);

/*  @brief Cancels ongoing transfer.
 *
 *  Cancels ongoing transfer by doing necessary operations related to hw core.
 *
 *  @param[in] hDev  device handle
 *  @return          void
 */
void
SpiDevice_CancelTransfer
(
   SPIDEVICE_HANDLE hDev
);

/*  @brief Set the power state of the device.
 *
 *  @param[in] hDev         device handle
 *  @param[in] ePowerState  desired power state
 *  @return                 error status
 */
uint32
SpiDevice_SetPowerState
(
   SPIDEVICE_HANDLE      hDev,
   SpiDevice_PowerStates ePowerState
);

/*  @brief Gets the HW device information.
 *
 *  @param[in]  hDev     device handle
 *  @param[out] spiInfo  HW info
 *  @return              error status
 */
uint32
SpiDevice_GetHwInfo
(
   SPIDEVICE_HANDLE  hDev,
   SpiInfoType      *spiInfo
);

/*  @brief QUP ISR (interrupt service routine)
 *
 *  This function should be called in interrupt context when a QUP interrupt
 *  is signaled.  It is used for block mode only.  It is responsible for moving
 *  data to/from the HW FIFOs and signaling when the transfer completes.
 *
 *  @param[in] pdata  pointer to data
 *  @return           0
 */
uint32
SpiDevice_IsrHook
(
   void *pdata
);

/*  @brief QUP IST (interrupt service thread)
 *
 *  This function was used when interrupt servicing was split into two parts
 *  (one part to clear the interrupt and one part to read/write the FIFOs).
 *  It is currently not used.
 *
 *  @param[in] pdata  pointer to data
 *  @return           0
 */
uint32
SpiDevice_IstHook
(
   void *pdata
);

/*  @brief BAM completion callback routine
 *
 *  This callback function is used to signal completion of a BAM transfer.
 *
 *  @param[in] pResult  transfer result
 *  @return             void
 */
void
SpiDevice_BamCallback
(
   SpiDeviceBamResult *pResult
);

/*  @brief Configures the SPI chip select (manual or auto).
 *
 *  @param[in] hDev         device handle
 *  @param[in] uChipSelect  chip select line (0 - 3)
 *  @param[in] eControl     selected control method (high/low/auto)
 *  @return                 error status
 */
uint32
SpiDevice_SetCSControl
(
   SPIDEVICE_HANDLE hDev,
   uint32           uChipSelect,
   SpiCSControl     eControl
);

#endif  // _SPIDEVICE_H_
