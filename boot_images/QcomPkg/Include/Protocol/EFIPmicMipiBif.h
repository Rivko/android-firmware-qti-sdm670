/** 
@file  EFIPmicMipiBif.h
@brief PMIC MIPI BIF Implementation for UEFI.
*/
/*=============================================================================
Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc.
All rights reserved.
Qualcomm Technologies Confidential and Proprietary.
    
=============================================================================*/

/*=============================================================================
EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
04/18/14   al      Updated copyright 
01/13/14   al      Added API to hard reset BCL line and read status
06/18/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup. 
04/27/13   sm      Added parameters in SendWords and SendReceive APIs
02/14/13   dy      Created new protocol

=============================================================================*/

#ifndef __EFIPMICMIPIBIF_H__
#define __EFIPMICMIPIBIF_H__

/*===========================================================================
INCLUDE FILES
===========================================================================*/

/*===========================================================================
MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_pmicMipibif_constants 
@{ */
/**
Protocol version.
*/

#define PMIC_MIPIBIF_REVISION 0x0000000000010001
/** @} */ /* end_addtogroup efi_pmicMipibif_constants */

/*  Protocol GUID definition */
/** @ingroup efi_pmicMipibif_protocol */
#define EFI_PMIC_MIPIBIF_PROTOCOL_GUID \
    { 0x5266542e, 0xed66, 0x4b59, { 0x86, 0xd5, 0xa7, 0xfb, 0xe8, 0x35, 0x1f, 0xea } }

/** @cond */
/*===========================================================================
EXTERNAL VARIABLES
===========================================================================*/
/**
External reference to the PMIC MIPI BIF Protocol GUID.
*/
extern EFI_GUID gQcomPmicMipiBifProtocolGuid;

/*===========================================================================
TYPE DEFINITIONS
===========================================================================*/
/**
Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_MIPIBIF_PROTOCOL   EFI_QCOM_PMIC_MIPIBIF_PROTOCOL;
/** @endcond */

/** @addtogroup efi_pmicMipiBif_data_types 
@{ */
/** BSI timing configuration. */
typedef enum
{
   PM_UEFI_BSI_TAU_CFG__150_US = 0, /**< 150 microsecond. */
   PM_UEFI_BSI_TAU_CFG__122_US,     /**< 122 microsecond. */
   PM_UEFI_BSI_TAU_CFG__61_US,      /**< 61 microsecond. */
   PM_UEFI_BSI_TAU_CFG__31_US,      /**< 31 microsecond. */
   PM_UEFI_BSI_TAU_CFG__15_US,      /**< 15 microsecond. */
   PM_UEFI_BSI_TAU_CFG__8_US,       /**< 8 microsecond. */
   PM_UEFI_BSI_TAU_CFG__4_US,       /**< 4 microsecond. */
   PM_UEFI_BSI_TAU_CFG__2_US        /**< 2 microsecond. */
}PM_UEFI_BSI_TAU_CFG_TYPE;

/** BSI sampling rate. */
typedef enum
{
   PM_UEFI_BSI_SAMPLING_RATE__4X,  /**< 4x. */
   PM_UEFI_BSI_SAMPLING_RATE__8X,  /**< 8x. */
}PM_UEFI_BSI_SAMPLING_RATE_TYPE;

/** BSI data format. */
typedef enum
{
   PM_UEFI_BSI_DATA_FORMAT__11_BIT,  /**< 11-bit. */
   PM_UEFI_BSI_DATA_FORMAT__17_BIT   /**< 17-bit. */
}PM_UEFI_BSI_DATA_FORMAT_TYPE;


/**BSI configuration status */ 
typedef struct PM_UEFI_BSI_STATUS_TYPE
{
    BOOLEAN                         BsiEnable;    /**< BSI enabled/disabled         */
    PM_UEFI_BSI_TAU_CFG_TYPE        TauCfg;       /**< TAU config type              */
    PM_UEFI_BSI_DATA_FORMAT_TYPE    RxDataFormat; /**< Rx data format               */
    PM_UEFI_BSI_DATA_FORMAT_TYPE    TxDataFormat; /**< Tx data format               */
    BOOLEAN                         RxEnable;     /**< MIPI-BIF Rx enabled/disabled */
    BOOLEAN                         TxEnable;     /**< MIPI-BIF Tx enabled/disabled */
    PM_UEFI_BSI_SAMPLING_RATE_TYPE  SamplingRate; /**< MIPI-BIF sampling rate       */
    BOOLEAN                         BclForceLow;  /**< BCL force status             */  
} PM_UEFI_BSI_STATUS_TYPE;

/** @} */ /* end_addtogroup efi_pmicMipiBif_data_types */


/*===========================================================================
FUNCTION DEFINITIONS
===========================================================================*/

/* EFI_PM_MIPIBIF_INIT */ 
/** @ingroup efi_pmMipiBif_init
  @par Summary
  Initializes the MIPI BIF Protocol.

  @param[in]  PmicDeviceIndex    Primary: 0.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_DEVICE ERROR   -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MIPIBIF_INIT)(
  IN UINT32  PmicDeviceIndex
);

/* EFI_PM_MIPIBIF_SET_MODE */ 
/** @ingroup efi_pmMipiBif_set_mode
  @par Summary
  Configures the MIPI BIF modes. 

  @param[in]  PmicDeviceIndex    Primary: 0.
  @param[in]  TauCfg             Timing parameter; see #PM_UEFI_BSI_TAU_CFG_TYPE 
                                 for details.
  @param[in]  SamplingRate       Battery Serial Interface (BSI) sampling 
                                 rate (4x or 8x); see 
                                 #PM_UEFI_BSI_SAMPLING_RATE_TYPE for details.
  @param[in]  DataFormat         MIPI BIF data format (11- or 17-bit); see 
                                 #PM_UEFI_BSI_DATA_FORMAT_TYPE for details.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_DEVICE ERROR   -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MIPIBIF_SET_MODE)(
  IN UINT32                          PmicDeviceIndex, 
  IN PM_UEFI_BSI_TAU_CFG_TYPE        TauCfg,
  IN PM_UEFI_BSI_SAMPLING_RATE_TYPE  SamplingRate,
  IN PM_UEFI_BSI_DATA_FORMAT_TYPE    DataFormat
);

/* EFI_PM_MIPIBIF_SEND_WORDS */ 
/** @ingroup efi_pmMipiBif_send_words
  @par Summary
  Sends MIPI BIF data.

  @param[in]  PmicDeviceIndex    Primary: 0.
  @param[in]  SlaveDeviceAddress  Slave device address.
  @param[in]  WriteAddr           Slave address to write.
  @param[in]  WriteData           Data to write.


  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_TIMEOUT        -- Operation timed out while sending data. \n
  EFI_DEVICE ERROR   -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MIPIBIF_SEND_WORDS)(
  IN UINT32  PmicDeviceIndex, 
  IN UINT32  SlaveDeviceAddress,
  IN UINT32  WriteAddr,
  IN UINT32  WriteData
);

/* EFI_PM_MIPIBIF_SEND_RECEIVE */ 
/** @ingroup efi_pmMipiBif_send_receive
  @par Summary
  Reads a single BIF word with device addressing.

  @param[in]  PmicDeviceIndex    Primary: 0.
  @param[in]  SlaveDeviceAddress  Slave device address.
  @param[in]  ReadAddr           Address to read.
  @param[out] pReadWord           Data read.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_TIMEOUT        -- Operation timed out while sending/receiving data. \n
  EFI_DEVICE ERROR   -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MIPIBIF_SEND_RECEIVE)(
  IN  UINT32  PmicDeviceIndex, 
  IN  UINT32  SlaveDeviceAddress,
  IN  UINT32  ReadAddr,
  OUT UINT32  *pReadWord
);


/* EFI_PM_MIPIBIF_SLAVE_HARD_RESET */ 
/** @ingroup efi_pmMipiBif_slave_hard_reset
  @par Summary
  This API hard resets the slave device 

  @param[in]  PmicDeviceIndex    Primary: 0.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_TIMEOUT        -- Operation timed out while sending/receiving data. \n
  EFI_DEVICE ERROR   -- Error occurred during the operation.
*/
typedef
EFI_STATUS (EFIAPI *EFI_PM_MIPIBIF_SLAVE_HARD_RESET)(
  IN  UINT32  PmicDeviceIndex
);
    
    
/* EFI_PM_MIPIBIF_GET_STATUS */ 
/**@ingroup efi_pmMipiBif_get_status
  @par Summary
  Reads BSI configuration status

  @param[in]  PmicDeviceIndex    Primary: 0.
  @param[out] BsiStatus          Reads BSI configuration. Refer  structure
                                 PM_UEFI_BSI_STATUS_TYPE for more info.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_TIMEOUT        -- Operation timed out while sending/receiving data. \n
  EFI_DEVICE ERROR   -- Error occurred during the operation.
*/   
typedef
EFI_STATUS (EFIAPI *EFI_PM_MIPIBIF_GET_STATUS)(
  IN  UINT32                    PmicDeviceIndex,
  OUT PM_UEFI_BSI_STATUS_TYPE   *BsiStatus
);    
/*===========================================================================
PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_pmicMipiBif_protocol
@par Summary
  Qualcomm PMIC Mobile Industry Processor Interface (MIPI) Battery 
  Interface (BIF) Protocol interface.

@par Parameters
  @inputprotoparams{pmic_mipi_bif_proto_params.tex} 
*/

struct _EFI_QCOM_PMIC_MIPIBIF_PROTOCOL {
  UINT64                                   Revision;
  EFI_PM_MIPIBIF_INIT                      Init;
  EFI_PM_MIPIBIF_SET_MODE                  SetMode;
  EFI_PM_MIPIBIF_SEND_WORDS                SendWords;
  EFI_PM_MIPIBIF_SEND_RECEIVE              SendReceive;
  EFI_PM_MIPIBIF_SLAVE_HARD_RESET          SlaveHardReset;
  EFI_PM_MIPIBIF_GET_STATUS                GetStatus;
};

#endif  /* __EFIPMICMIPIBIF_H__ */
