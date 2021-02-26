/** @file SBIInterface.h

  SBI UEFI Interface

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/07/11   MJS     Fix compile warning by removing apostrophe from #error
 02/01/11   unr     Initial revision for UEFI SBI

=============================================================================*/

#ifndef __SBIINTERFACE_H__
#define __SBIINTERFACE_H__

#ifndef FEATURE_LIBRARY_ONLY
#error This file can only be included from non-shipped library code, as it is not shipped to customers.
#endif

/* SSBI2 address width default is 10 bits */
#define SBI_SSBI2_ADDRESS_WIDTH_DEFAULT 10

/**
  @brief
  SBI Devices
 */
typedef enum
{
  SBIDevPmicSSBI1,           /**<-- PMIC Arbiter 1 */
  SBIDevPmicSSBI2,           /**<-- PMIC Arbiter 2 */
  SBIDevCodecSSBI            /**<-- CODEC */
} SBI_DEVICE;

/**
  @brief
  SBI controller types
 */
typedef enum
{
  EFI_SBI_CTLR_SBI,               /**<-- SBI controller */
  EFI_SBI_CTLR_SSBI,              /**<-- SSBI controller */
  EFI_SBI_CTLR_SSBI2,             /**<-- SSBI2 controller */ 
  EFI_SBI_CTLR_PMIC_ARBITER,      /**<-- SSBI2 PMIC Arbiter with Configuration
                                         and Command interfaces */
  EFI_SBI_CTLR_PMIC_ARBITER_CMD,  /**<-- SSBI2 PMIC Arbiter with Command-only
                                     interface */
  _EFI_PLACEHOLDER_SbiControllerType = 0x7fffffff
} SBI_CONTROLLER_TYPE;

/**
  @brief
  (S)SBI(2) transfer modes
 */
typedef enum
{
  EFI_SBI_MODE_DEFAULT, /**<-- Default transfer mode: FTM for SBI controller and 
                               SSBI for SSBI(2) controller */
  EFI_SBI_MODE_FTM,     /**<-- Fast Transfer Mode, valid for SBI, SSBI and SSBI2
                               controllers */
  EFI_SBI_MODE_SSBI,    /**<-- SSBI mode,valid for SSBI and SSBI2 controllers */
  EFI_SBI_MODE_SSBI2,   /**<-- SSBI2 mode, valid for SSBI2 controllers */  
  _EFI_PLACEHOLDER_SbiTransferModeType = 0x7fffffff
} SBI_TRANSFER_MODE_TYPE;

/**
  @brief
  SBI Frequency types; valid only for SBI controllers
 */
typedef enum
{
  EFI_SBI_FREQ_9p6MHZ,
  EFI_SBI_FREQ_6p4MHZ,
  EFI_SBI_FREQ_4p8MHZ,
  EFI_SBI_FREQ_3p84MHZ,
  EFI_SBI_FREQ_3p2MHZ,
  EFI_SBI_FREQ_2p74MHZ,
  EFI_SBI_FREQ_2p4MHZ,
  EFI_SBI_FREQ_2p13MHZ,
  EFI_SBI_FREQ_1p92MHZ,
  EFI_SBI_FREQ_1MHZ,
  _EFI_PLACEHOLDER_SbiFreqType = 0x7fffffff
} SBI_FREQ_TYPE;

/**
  @brief
  SBI read or write direction
 */
typedef enum
{
  EFI_SBI_READ,
  EFI_SBI_WRITE,
  _EFI_PLACEHOLDER_SbiReadWriteDirType = 0x7fffffff
} SBI_READ_WRITE_DIR_TYPE;

/**
  @brief
  Possible hosts connected to (S)SBI arbiter ports
 */
typedef enum
{
  EFI_SBI_ARB_PORT_INVALID,
  EFI_SBI_ARB_PORT_RXFRONT_0,
  EFI_SBI_ARB_PORT_RXFRONT_1,
  EFI_SBI_ARB_PORT_AGC_0,
  EFI_SBI_ARB_PORT_AGC_1,
  EFI_SBI_ARB_PORT_MICRO,
  EFI_SBI_ARB_PORT_MDSP,
  EFI_SBI_ARB_PORT_ADSP,
  EFI_SBI_ARB_PORT_TSADC,
  EFI_SBI_ARB_PORT_SVS,
  EFI_SBI_ARB_PORT_UNUSED_0,
  EFI_SBI_ARB_PORT_UNUSED_1,
  EFI_SBI_ARB_PORT_UNUSED_2,
  EFI_SBI_ARB_PORT_LPASS_Q6_SAW,
  EFI_SBI_ARB_PORT_SMPSS_CPU0_SAW,
  EFI_SBI_ARB_PORT_SMPSS_CPU1_SAW,
  EFI_SBI_ARB_PORT_NUM_IDS,
  _EFI_PLACEHOLDER_SbiArbPortType = 0x7fffffff
} SBI_ARB_PORT_TYPE;

/**
  @brief
  SBI return codes
 */
typedef enum
{
  EFI_SBI_SUCCESS = EFI_SUCCESS,          /**<-- Equal to DAL_SUCCESS. If
                                                 DAL_SUCCESS changes the enum
                                                 will need to be synced with the
                                                 version in ddisbi.h */
  EFI_SBI_ERROR_INVALID_ADDRESS_WIDTH,
  EFI_SBI_ERROR_INVALID_FREQ,
  EFI_SBI_ERROR_INVALID_TRANSFER_MODE,
  EFI_SBI_ERROR_INVALID_SLAVE_ID,
  EFI_SBI_ERROR_INVALID_PORT,
  EFI_SBI_ERROR_PARAMETER_SIZE_MISMATCH,
  EFI_SBI_ERROR_ZERO_LENGTH_PARAMETER,
  EFI_SBI_ERROR_NULL_POINTER_PASSED,
  EFI_SBI_ERROR_VERIFY_BYTE_MISMATCH,
  EFI_SBI_ERROR_INVALID_OPERATION,
  EFI_SBI_ERROR_READ_WRITE_FAILURE,
  EFI_SBI_ERROR_DEVICE_NOT_POWERED,
  EFI_SBI_ERROR = -1,                     /**<-- Equal to DAL_ERROR. If
                                                 DAL_ERROR changes the enum
                                                 will need to be synced with the
                                                 version in ddisbi.h */
  _EFI_PLACEHOLDER_SbiResultType = 0x7fffffff
} SBI_RESULT_TYPE;

/**
  @brief
  Structure defining (S)SBI(2) slave settings
 */
typedef struct
{
  
  SBI_TRANSFER_MODE_TYPE  eMode;      /**<-- The type of transfer mode */
  UINT32                  uSlaveId;   /**<-- The slave device ID. Valid only for
                                             SBI controller or FTM mode */
  SBI_FREQ_TYPE           eSbiFreq;   /**<-- The SBI bus frequency. Valid only
                                             for SBI controller */
  UINT32                  uAddrWidth; /**<-- The bit width of the address field
                                             for SSBI2 transfers. Valid only for
                                             SSBI2 transfer mode */
  UINT32                  uReserved0; /**<-- Reserved field */
  UINT32                  uReserved1; /**<-- Reserved field */
} SBI_SLAVE_SETTINGS_TYPE;

/**
  @brief
  Structure specifying data passed to ReadWrite function
 */
typedef struct
{
  SBI_READ_WRITE_DIR_TYPE eDir;   /**<-- Direction of operation: read or write */
  UINT32 uAddr;                   /**<-- Address to read or write */
  UINT32 uData;                   /**<-- Data to write or data read */
} SBI_READ_WRITE_DATA_TYPE;

/**
  EFI_STATUS SBIInit ()

  @brief
  Initializes the SBI device specified by eDev

  @param[in]  eDev                  One of the SBI Devices defined in SBI_DEVICE

  @param[out] pSBIHandle            A pointer location to be filled by the
                                    driver with a handle to the SBI device.

  @retval EFI_SUCCESS               Success
  @retval EFI_DEVICE_ERROR          SBI Device Initialization failed

 */
EFI_STATUS SBIInit (
  SBI_DEVICE    eDev,
  VOID        **pSBIHandle
);

/**
  SBI_RESULT_TYPE SBIGetControllerType ()

  @brief
  This function retrieves the type of hardware controller.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  pCtlrType             Pointer to location to store client settings

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIGetControllerType (
  VOID                *pSBIHandle,
  SBI_CONTROLLER_TYPE *pCtlrType
);

/**
  SBI_RESULT_TYPE SBISetClientSlaveSettings ()

  @brief
  This function sets the specified SBI client settings.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  pSlaveSettings        Pointer to the SBI client settings to set

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBISetClientSlaveSettings (
  VOID                    *pSBIHandle,
  SBI_SLAVE_SETTINGS_TYPE *pSlaveSettings
);

/**
  SBI_RESULT_TYPE SBIGetClientSlaveSettings ()

  @brief
  This function retrieves the current SBI client settings.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[out] pSlaveSettings        Pointer to location to store the current
                                    SBI client settings

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIGetClientSlaveSettings (
  VOID                    *pSBIHandle,
  SBI_SLAVE_SETTINGS_TYPE *pSlaveSettings
);

/**
  SBI_RESULT_TYPE SBIRead ()

  @brief
  This function reads bytes from the SBI bus.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  pAddrBuffer           Pointer to an array of SBI addresses to
                                    read. pAddrBuffer corresponds to a pointer
                                    to a byte array when the address width of
                                    the bus is 7 or 8 bits (in 3-wire SBI, SSBI
                                    or SSBI2 mode), and corresponds to a pointer
                                    to a short array (16-bit) when the address
                                    width
  of the bus is greater than 8 bits (in 10-bit SSBI2 mode)
  @param[in]  uAddrBufferLen        The length of the address buffer array in
                                    bytes
  @param[out] pDataBuffer           Pointer to an array to store the bytes read
                                    from the bus
  @param[in]  uDataBufferLen        The length of the data buffer array in bytes
  @param[out] puDataBufferLenReq    Pointer to location to store the number of
                                    bytes stored in the data buffer array

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIRead (
  VOID        *pSBIHandle,
  VOID        *pAddrBuffer,
  UINT32       uAddrBufferLen,
  UINT8       *pDataBuffer,
  UINT32       uDataBufferLen,
  UINT32      *puDataBufferLenReq
);

/**
  SBI_RESULT_TYPE SBIWrite ()

  @brief
  This function writes bytes to the SBI bus.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  pAddrBuffer           Pointer to an array of SBI addresses to
                                    write. pAddrBuffer corresponds to a pointer
                                    to a byte array when the address width of
                                    the bus is 7 or 8 bits (in 3-wire SBI, SSBI
                                    or SSBI2 mode), and corresponds to a pointer
                                    to a short array (16-bit) when the address
                                    width of the bus is greater than 8 bits (in
                                    10-bit SSBI2 mode)
  @param[in]  uAddrBufferLen        The length of the address buffer array in
                                    bytes
  @param[in]  pDataBuffer           Pointer to an array to data to write to the
                                    bus
  @param[in]  uDataBufferLen        The length of the data buffer array in bytes


  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIWrite (
  VOID        *pSBIHandle,
  VOID        *pAddrBuffer,
  UINT32       uAddrBufferLen,
  CONST UINT8 *pDataBuffer,
  UINT32       uDataBufferLen
);

/**
  SBI_RESULT_TYPE SBIReadByte ()

  @brief
  This function reads a single byte from the SBI bus.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  uAddr                 The address to read
  @param[in]  uData                 Pointer to location to store the data read
                                    from the bus

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIReadByte (
  VOID    *pSBIHandle,
  UINT32   uAddr,
  UINT8   *puData
);

/**
  SBI_RESULT_TYPE SBIWriteByte ()

  @brief
  This function writes a single byte to the SBI bus.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  uAddr                 The address to write  
  @param[in]  uData                 Data to write to the bus

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIWriteByte (
  VOID    *pSBIHandle,
  UINT32   uAddr,
  UINT32   uData
);

/**
  SBI_RESULT_TYPE SBIWriteVerifyByte ()

  @brief
  This function writes a single byte to the SBI bus and then reads the byte
  back to verify the data was correctly written to the bus.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  uAddr                 The address to write 
  @param[in]  uData                 The data to write to the bus

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIWriteVerifyByte (
  VOID    *pSBIHandle,
  UINT32   uAddr,
  UINT32   uData
);

/**
  SBI_RESULT_TYPE SBIReadWrite ()

  @brief
  This function performs an atomic sequence of mixed byte reads and writes
  on the SBI bus.
   
  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  pReadWriteDataBuffer  Pointer to an array of SbiReadWriteDataType
                                    data structures containing the data and
                                    addresses to read or write. Note: this
                                    parameter is an input/output parameter.
  @param[in]  uReadWriteDataLen     Length of paReadWriteData array in bytes

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIReadWrite (
  VOID                     *pSBIHandle,
  SBI_READ_WRITE_DATA_TYPE *pReadWriteData,
  UINT32                    uReadWriteDataLen
);

/**
  SBI_RESULT_TYPE SBIGetPVCAddressIndex ()

  @brief
  Retrieves the index corresponding to one of the SSBI addresses that 
  a hardware port is allowed to generate SSBI write commands to.
  For security purposes, each hardware port (or PMIC Voltage Control (PVC) port)
  that can generate SSBI write commands through the PMIC Arbiter 
  is only allowed to generate writes to specific pre-defined SSBI addresses.
  The PMIC Arbiter contains a table of the allowed SSBI addresses for each 
  hardware port, into which the hardware port will index when generating 
  a SSBI write command.  This function retrieves the table index corresponding 
  to the specified SSBI address for the specified hardware port.

  @param[in]  pSBIHanlde            Pointer to a SBI device handle
  @param[in]  ePort                 The hardware (PVC) port
  @param[in]  uAddr                 The address to lookup the index index for
  @param[out] puIndex               Pointer to the location to store the 0-based
                                    table index

  @retval SBI_RESULT_TYPE status codes as defined in the enum

 */
SBI_RESULT_TYPE SBIGetPVCAddressIndex (
  VOID              *pSBIHandle,
  SBI_ARB_PORT_TYPE  ePort,
  UINT32             uAddr,
  UINT32            *puIndex
);

/**
  EFI_STATUS SBIDeInit ()

  @brief
  De-Initializes the SBI device corresponding to the pSBIHandle

  @param[out] pSBIHandle            Pointer to the SBI handle

  @retval EFI_SUCCESS               Success
  @retval EFI_DEVICE_ERROR          SBI Device De-Initialization failed

 */
EFI_STATUS SBIDeInit (
  VOID *pSBIHandle
);

#endif	/* __SBIINTERFACE_H__ */
