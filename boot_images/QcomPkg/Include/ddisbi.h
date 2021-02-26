#ifndef DDISBI_H
#define DDISBI_H
/*
===========================================================================

FILE:         ddisbi.h

DESCRIPTION:  This file contains the definitions of the constants,
              types, and interfaces that comprise the interface to the
              SBI Device Access Library.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
04/07/11   MJS     Fix compile warning by removing apostrophe from #error
02/02/11   MJS     Added support for Q6 g++ compiler.
02/24/10   MJS     Added device not powered error code.
02/16/10   MJS     Added Reserve API.
01/11/10   MJS     Allow API backwards-compatibility with older implementation.
10/22/09   MJS     Added PMIC Arbiter API extension.
05/26/09   MJS     Added read/write API.
10/28/08   MJS     Initial revision.
===========================================================================
             Copyright (c) 2008, 2009, 2010, 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifndef FEATURE_LIBRARY_ONLY
#error This file can only be included from non-shipped library code, as it is not shipped to customers.
#endif

#include "DalDevice.h"
#include "DALStdErr.h"

#define DALSBI_INTERFACE_VERSION DALINTERFACE_VERSION(1,3)

/* SSBI2 address width default is 10 bits */
#define SBI_SSBI2_ADDRESS_WIDTH_DEFAULT 10

/* Enumeration of SBI controller types */
typedef enum
{
  SBI_CTLR_SBI,   /* SBI controller */
  SBI_CTLR_SSBI,  /* SSBI controller */
  SBI_CTLR_SSBI2, /* SSBI2 controller */ 
  SBI_CTLR_PMIC_ARBITER, /* SSBI2 PMIC Arbiter with Configuration and Command interfaces */
  SBI_CTLR_PMIC_ARBITER_CMD, /* SSBI2 PMIC Arbiter with Command-only interface */
  _PLACEHOLDER_SbiControllerType = 0x7fffffff
} SbiControllerType;

/* Enumeration defining the (S)SBI(2) transfer modes */
typedef enum
{
  SBI_MODE_DEFAULT, /* Default transfer mode: FTM for SBI controller and 
                       SSBI for SSBI(2) controller */
  SBI_MODE_FTM,   /* Fast Transfer Mode, valid for SBI, SSBI and SSBI2 controllers */
  SBI_MODE_SSBI,  /* SSBI mode, valid for SSBI and SSBI2 controllers */
  SBI_MODE_SSBI2, /* SSBI2 mode, valid for SSBI2 controllers */  
  _PLACEHOLDER_SbiTransferModeType = 0x7fffffff
} SbiTransferModeType;

/* SBI Frequency types; valid only for SBI controllers */
typedef enum
{
   SBI_FREQ_9p6MHZ,
   SBI_FREQ_6p4MHZ,
   SBI_FREQ_4p8MHZ,
   SBI_FREQ_3p84MHZ,
   SBI_FREQ_3p2MHZ,
   SBI_FREQ_2p74MHZ,
   SBI_FREQ_2p4MHZ,
   SBI_FREQ_2p13MHZ,
   SBI_FREQ_1p92MHZ,
   SBI_FREQ_1MHZ,
  _PLACEHOLDER_SbiFreqType = 0x7fffffff
} SbiFreqType;

/* Enumeration of SBI read or write direction */
typedef enum
{
  SBI_READ,
  SBI_WRITE,
  _PLACEHOLDER_SbiReadWriteDirType = 0x7fffffff
} SbiReadWriteDirType;

/* Enumeration of possible hosts connected to (S)SBI arbiter ports */
typedef enum
{
  SBI_ARB_PORT_INVALID,
  SBI_ARB_PORT_RXFRONT_0,
  SBI_ARB_PORT_RXFRONT_1,
  SBI_ARB_PORT_AGC_0,
  SBI_ARB_PORT_AGC_1,
  SBI_ARB_PORT_MICRO,
  SBI_ARB_PORT_MDSP,
  SBI_ARB_PORT_ADSP,
  SBI_ARB_PORT_TSADC,
  SBI_ARB_PORT_SVS,
  SBI_ARB_PORT_UNUSED_0,
  SBI_ARB_PORT_UNUSED_1,
  SBI_ARB_PORT_UNUSED_2,
  SBI_ARB_PORT_LPASS_Q6_SAW,
  SBI_ARB_PORT_SMPSS_CPU0_SAW,
  SBI_ARB_PORT_SMPSS_CPU1_SAW,
  SBI_ARB_PORT_NUM_IDS,
  _PLACEHOLDER_SbiArbPortType = 0x7fffffff
} SbiArbPortType;

/* Enumeration of SBI error return codes */
typedef enum
{
  SBI_SUCCESS = DAL_SUCCESS,
  SBI_ERROR_INVALID_ADDRESS_WIDTH,
  SBI_ERROR_INVALID_FREQ,
  SBI_ERROR_INVALID_TRANSFER_MODE,
  SBI_ERROR_INVALID_SLAVE_ID,
  SBI_ERROR_INVALID_PORT,
  SBI_ERROR_PARAMETER_SIZE_MISMATCH,
  SBI_ERROR_ZERO_LENGTH_PARAMETER,
  SBI_ERROR_NULL_POINTER_PASSED,
  SBI_ERROR_VERIFY_BYTE_MISMATCH,
  SBI_ERROR_INVALID_OPERATION,
  SBI_ERROR_READ_WRITE_FAILURE,
  SBI_ERROR_DEVICE_NOT_POWERED,
  SBI_ERROR = DAL_ERROR,
  _PLACEHOLDER_SbiResultType = 0x7fffffff
} SbiResultType;

/* Structure defining (S)SBI(2) slave settings */
typedef struct
{
  /* The type of transfer mode */
  SbiTransferModeType eMode;
  /* The slave device ID */
  uint32              uSlaveId;   /* Valid only for SBI controller or FTM mode */
  /* The SBI bus frequency */
  SbiFreqType         eSbiFreq;   /* Valid only for SBI controller */
  /* The bit width of the address field for SSBI2 transfers */
  uint32              uAddrWidth; /* valid only for SSBI2 transfer mode */
  /* Reserved fields */
  uint32              uReserved0;
  uint32              uReserved1;
} SbiSlaveSettingsType;

/* Structure specifying data passed to ReadWrite function */
typedef struct
{
  SbiReadWriteDirType eDir;  /* Direction of operation: read or write */
  uint32 uAddr;              /* Address to read or write */
  uint32 uData;              /* Data to write or data read */
} SbiReadWriteDataType;

typedef struct DalSBI DalSBI;
struct DalSBI
{
   struct DalDevice DalDevice;
   DALResult (*GetControllerType)(DalDeviceHandle * _h, void * pCtlrType, uint32 olen1);
   DALResult (*SetClientSlaveSettings)(DalDeviceHandle * _h, void * pSlaveSettings, uint32 ilen1);
   DALResult (*GetClientSlaveSettings)(DalDeviceHandle * _h, void * pSlaveSettings, uint32 olen1);
   DALResult (*Read)(DalDeviceHandle * _h, void * pAddrBuffer,  uint32  uAddrBufferLen, void * pDataBuffer,  uint32  uDataBufferLen,  uint32*  puDataBufferLenReq);
   DALResult (*Write)(DalDeviceHandle * _h, void * pAddrBuffer,  uint32  uAddrBufferLen, void * pDataBuffer,  uint32  uDataBufferLen);
   DALResult (*ReadByte)(DalDeviceHandle * _h, uint32  uAddr, void * puData, uint32 olen2);
   DALResult (*WriteByte)(DalDeviceHandle * _h, uint32  uAddr, uint32  uData);
   DALResult (*WriteVerifyByte)(DalDeviceHandle * _h, uint32  uAddr, uint32  uData);
   DALResult (*SendITMMessage)(DalDeviceHandle * _h, uint32  uData);
   DALResult (*TurnOffSlave)(DalDeviceHandle * _h, uint32  uAddr, uint32  uData);
   DALResult (*EnableHardwareRequest)(DalDeviceHandle * _h, SbiArbPortType eChannel, DALBOOL  bEnable);
   DALResult (*ReadWrite)(DalDeviceHandle * _h,  const SbiReadWriteDataType*  pInBuffer,  uint32  pInBufferLen,  SbiReadWriteDataType*  pOutBuffer,  uint32  pOutBufferLen);
   DALResult (*GetPVCAddressIndex)(DalDeviceHandle * _h, SbiArbPortType  ePort, uint32  uAddr,  uint32 * puIndex, uint32 olen3);
   DALResult (*Reserve)(DalDeviceHandle * _h, DALBOOL bReserve);
};

typedef struct DalSBIHandle DalSBIHandle; 
struct DalSBIHandle 
{
   uint32 dwDalHandleId;
   const DalSBI * pVtbl;
   void * pClientCtxt;
};

#define DAL_SBIDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALSBI_INTERFACE_VERSION,hDalDevice)


/*===========================================================================

DalSBI_GetControllerType()

Description:
  This function retrieves the type of hardware controller.

Parameters:
  _h - Pointer to a DAL device handle
  pCtlrType - Pointer to location to store hardware controller type

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_GetControllerType(DalDeviceHandle * _h,  SbiControllerType*  pCtlrType)
{
   return ((DalSBIHandle *)_h)->pVtbl->GetControllerType( _h, (void * )pCtlrType,sizeof( SbiControllerType ));
}


/*===========================================================================

DalSBI_SetClientSlaveSettings()

Description:
  This function sets the specified SBI client settings.

Parameters:
  _h - Pointer to a DAL device handle
  pSlaveSettings - Pointer to the SBI client settings to set

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_SetClientSlaveSettings(DalDeviceHandle * _h,  const SbiSlaveSettingsType*  pSlaveSettings)
{
   return ((DalSBIHandle *)_h)->pVtbl->SetClientSlaveSettings( _h, (void * )pSlaveSettings,sizeof(  SbiSlaveSettingsType ));
}


/*===========================================================================

DalSBI_GetClientSlaveSettings()

Description:
  This function retrieves the current SBI client settings.

Parameters:
  _h - Pointer to a DAL device handle
  pSlaveSettings - Pointer to location to store the current SBI client settings

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_GetClientSlaveSettings(DalDeviceHandle * _h,  SbiSlaveSettingsType*  pSlaveSettings)
{
   return ((DalSBIHandle *)_h)->pVtbl->GetClientSlaveSettings( _h, (void * )pSlaveSettings,sizeof( SbiSlaveSettingsType ));
}


/*===========================================================================

DalSBI_Read()

Description:
  This function reads bytes from the SBI bus.

Parameters:
  _h                 - Pointer to a DAL device handle
  pcaAddrBuffer      - Pointer to an array of SBI addresses to read
  uAddrBufferLen     - The length of the address buffer array in bytes
  paucDataBuffer     - Pointer to an array to store the bytes read from the bus
  uDataBufferLen     - The length of the data buffer array in bytes
  puDataBufferLenReq - Pointer to location to store the number of bytes stored
                       in the data buffer array

Dependencies:
  pcaAddrBuffer corresponds to a pointer to a byte array when the address
  width of the bus is 7 or 8 bits (in 3-wire SBI, SSBI or SSBI2 mode), and 
  corresponds to a pointer to a short array (16-bit) when the address width
  of the bus is greater than 8 bits (in 10-bit SSBI2 mode)

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_Read(DalDeviceHandle * _h,  const void*  pcaAddrBuffer,  uint32  uAddrBufferLen,  uint8*  paucDataBuffer,  uint32  uDataBufferLen,  uint32*  puDataBufferLenReq)
{
   return ((DalSBIHandle *)_h)->pVtbl->Read( _h, (void * )pcaAddrBuffer, uAddrBufferLen, (void * )paucDataBuffer, uDataBufferLen, puDataBufferLenReq);
}


/*===========================================================================

DalSBI_Write()

Description:
  This function writes bytes to the SBI bus.

Parameters:
  _h              - Pointer to a DAL device handle
  pcaAddrBuffer   - Pointer to an array of SBI addresses to write
  uAddrBufferLen  - The length of the address buffer array in bytes
  pcaucDataBuffer - Pointer to an array to data to write to the bus
  uDataBufferLen  - The length of the data buffer array in bytes

Dependencies:
  pcaAddrBuffer corresponds to a pointer to a byte array when the address 
  width of the bus is 7 or 8 bits (in 3-wire SBI, SSBI or SSBI2 mode), and 
  corresponds to a pointer to a short array (16-bit) when the address width
  of the bus is greater than 8 bits (in 10-bit SSBI2 mode)

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_Write(DalDeviceHandle * _h,  const void*  pcaAddrBuffer,  uint32  uAddrBufferLen,  const uint8*  pcaucDataBuffer,  uint32  uDataBufferLen)
{
   return ((DalSBIHandle *)_h)->pVtbl->Write( _h, (void * )pcaAddrBuffer, uAddrBufferLen, (void * )pcaucDataBuffer, uDataBufferLen);
}


/*===========================================================================

DalSBI_ReadByte()

Description:
  This function reads a single byte from the SBI bus.

Parameters:
  _h      - Pointer to a DAL device handle
  uAddr   - The address to read
  pucData - Pointer to location to store the data read from the bus

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_ReadByte(DalDeviceHandle * _h, uint32  uAddr,  uint8*  pucData)
{
   return ((DalSBIHandle *)_h)->pVtbl->ReadByte( _h, uAddr, (void * )pucData,sizeof( uint8 ));
}


/*===========================================================================

DalSBI_WriteByte()

Description:
  This function writes a single byte to the SBI bus.

Parameters:
  _h    - Pointer to a DAL device handle
  uAddr - The address to read
  uData - The data to write to the bus

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_WriteByte(DalDeviceHandle * _h, uint32  uAddr, uint32  uData)
{
   return ((DalSBIHandle *)_h)->pVtbl->WriteByte( _h, uAddr, uData);
}


/*===========================================================================

DalSBI_WriteVerifyByte()

Description:
  This function writes a single byte to the SBI bus and then reads the byte
   back to verify the data was correctly written to the bus.

Parameters:
  _h    - Pointer to a DAL device handle
  uAddr - The address to read
  uData - The data to write to the bus

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_WriteVerifyByte(DalDeviceHandle * _h, uint32  uAddr, uint32  uData)
{
   return ((DalSBIHandle *)_h)->pVtbl->WriteVerifyByte( _h, uAddr, uData);
}


/*===========================================================================

DalSBI_SendITMMessage()

Description:
  This function sends a single message on a 3-wire SBI bus using Interrupt
  Transfer Mode framing.  The transfer mode of the bus after the ITM
  transaction is complete will be the same as prior to the ITM transaction.

Parameters:
  _h    - Pointer to a DAL device handle
  uData - The data to write to the bus

Dependencies:
  SID and Message are bit-packed into a the 6-bit uData as follows:

  +-----+-----------+
  |   5 | 4 3 2 1 0 |
  +-----+-----------+
  | SID |   Message |
  +-----+-----------+

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_SendITMMessage(DalDeviceHandle * _h, uint32  uData)
{
   return ((DalSBIHandle *)_h)->pVtbl->SendITMMessage( _h, uData);
}


/*===========================================================================

DalSBI_TurnOffSlave()

Description:
  This function writes a single byte to an single-wire SBI bus that
  is expected to turn off the TCXO on the SBI slave.  The controller
  will send out a wakeup pulse prior to the next read or write transaction.

Parameters:
  _h    - Pointer to a DAL device handle
  uAddr - The address to write to turn off the slave TCXO
  uData - The data to write to turn off the slave TCXO

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_TurnOffSlave(DalDeviceHandle * _h, uint32  uAddr, uint32  uData)
{
   return ((DalSBIHandle *)_h)->pVtbl->TurnOffSlave( _h, uAddr, uData);
}


/*===========================================================================

DalSBI_EnableHardwareRequest()

Description:
  This function enables or disables hardware requests for a 3-wire SBI
  controller.

Parameters:
  _h      - Pointer to a DAL device handle
  ePort   - The arbiter port to enable or disable
  bEnable - Boolean whether to enable or disable hardware requests

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_EnableHardwareRequest(DalDeviceHandle * _h, SbiArbPortType ePort, DALBOOL  bEnable)
{
   return ((DalSBIHandle *)_h)->pVtbl->EnableHardwareRequest( _h, ePort, bEnable);
}

/*===========================================================================

DalSBI_ReadWrite()

Description:
  This function performs an atomic sequence of mixed byte reads and writes
  on the SBI bus.
   
Parameters:
  _h                - Pointer to a DAL device handle
  paReadWriteData   - Pointer to an array of SbiReadWriteDataType data 
                      structures containing the data and addresses to read 
                      or write. Note: this parameter is an input/output
                      parameter.
  uReadWriteDataLen - Length of paReadWriteData array in bytes

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_ReadWrite(DalDeviceHandle * _h,  SbiReadWriteDataType*  paReadWriteData, uint32 uReadWriteDataLen)
{
  if ( NULL != ((DalSBIHandle *)_h)->pVtbl->ReadWrite )
  {
     return ((DalSBIHandle *)_h)->pVtbl->ReadWrite( _h, paReadWriteData, uReadWriteDataLen, paReadWriteData, uReadWriteDataLen);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*===========================================================================

DalSBI_GetPVCAddressIndex()

Description:
  Retrieves the index corresponding to one of the SSBI addresses that 
  a hardware port is allowed to generate SSBI write commands to.
  For security purposes, each hardware port (or PMIC Voltage Control (PVC) port)
  that can generate SSBI write commands through the PMIC Arbiter 
  is only allowed to generate writes to specific pre-defined SSBI addresses.
  The PMIC Arbiter contains a table of the allowed SSBI addresses for each 
  hardware port, into which the hardware port will index when generating 
  a SSBI write command.  This function retrieves the table index corresponding 
  to the specified SSBI address for the specified hardware port.

Parameters:
  _h      - Pointer to a DAL device handle
  ePort   - The hardware (PVC) port
  uAddr   - The address to lookup the index index for
  puIndex - Pointer to the location to store the 0-based table index

Dependencies:
  None

Return:
  SBI_SUCCESS if successful, error code if an error occurred

===========================================================================*/
static __inline DALResult
DalSBI_GetPVCAddressIndex(DalDeviceHandle * _h, SbiArbPortType  ePort, uint32  uAddr,  uint32 * puIndex)
{
  if ( NULL != ((DalSBIHandle *)_h)->pVtbl->GetPVCAddressIndex )
  {  
    return ((DalSBIHandle *)_h)->pVtbl->GetPVCAddressIndex( _h, ePort, uAddr, puIndex,sizeof( uint32 ));
  }
  else
  {
    return DAL_ERROR;
  }
}

#endif /* DDISBI_H */

