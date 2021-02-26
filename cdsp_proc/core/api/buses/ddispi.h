#ifndef __DDISPI_H__
#define __DDISPI_H__
/**
  @file DDISpi.h 
  @brief 
  This file contains the interface declaration for SPI DAL device driver.
*/ 

/*
===========================================================================

FILE:   DDISPI.H

DESCRIPTION:
    This file contains the interface declaration for SPI DAL device driver.

===========================================================================

        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/api/buses/ddispi.h#1 $$DateTime: 2018/07/30 01:21:36 $$Author: pwbldsvc $

When        Who    What, where, why
--------    ---    -----------------------------------------------------------
2013-02-12  ms	   Fixed : More than 32KB transfer request not supported.
2011-06-11  ddk	   Added minor review comments.
2011-06-05  ag	   Doxygen Markups.
2010-02-24  wx     Use SpiDataAddrType instead of void * for buffer address.
2008-08-11  wx     Created

===========================================================================
        Copyright c 2008-2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

/**
@mainpage SPI DAL Documentation

@section S1 Introduction

Serial Peripheral Interface (SPI) DAL provides an abstracted interface to clients to access the SPI hw block for sending and receiving data.

@section S2 Interfaces
The clients are expected to first attach to the SPI hw by calling
\c DAL_SpiDeviceAttach. It is expected that the client will validate the
return value of the \c DAL_SpiDeviceAttach prior to making any calls to any
of the interfaces documented here.

@subsection SS21 Parameters
All SPI DAL functions take device handle the client obtained from the attach process as their first parameter. There will be some level of validation on the parameters. However, the handles passed are assumed to be valid and that the client has done adequate validation against them at the time of the attach process.
*/


/**
@defgroup spi_api SPI APIs
This group groups all the APIs in the Shapes API module. 

@defgroup spi_dal_api SPI DAL APIs for clients
@ingroup spi_api
This section defines the various data structures used in the SPI DAL API. 

@defgroup spi_dal_data_structures Data Structures.
@ingroup spi_api
This section defines the various data structures used in the SPI DAL API. 

@defgroup spi_dal_enum Enumeration Constants  
@ingroup spi_api
This section defines the various enums used in the SPI DAL API. 
*/

#include "DalDevice.h"
#include "spi_defs_legacy.h"

#define DALSPI_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/**
@addtogroup spi_dal_enum
@{ */
/**
List of values returned by DalSpi_StartTransfer, DalSpi_GetTransferStatus and a few other
SPI APIs.
*/
typedef enum SpiTransferStatus SpiTransferStatus;
enum SpiTransferStatus
{
   SPI_QUEUED = 1,/**< The transaction is queued, will be processed by the core in order in which it arrived.*/
   SPI_COMPLETE,/**< The transaction is complete.*/
   SPI_IN_PROGRESS,/**< The core is currently processing the transfer, this only returned 
					 for DalSpi_GetTransferStatus API.*/
   SPI_FAILURE,/**< There was an error while the SPI core processed the transfer.*/
   SPI_BUSY,/**< If queueIfBusy flag is set as false, then if the core is currently processing a transaction, 
			     this value is returned, and the transfer is not  queued. The client has to retry the call.*/
   SPI_FULL,/**< The internal queueing buffer is full, and the transaction cannot be queued. 
			  The client has to retry the call.*/
   SPI_INVALID_ID,/**< An incorrect transfer id was provided while requesting for the status of a transfer.*/
   SPI_INVALID_PARAMETER/**< Spi transfer parameter provided by the Client  was invalid.*/
};
 /** 
@} 
*/

typedef struct SpiTransferType SpiTransferType;
/**
@addtogroup spi_dal_data_structures 
@{ */
/**
 Data structure that holds information regarding a SPI transfer.
*/ 
struct SpiTransferType
{
   uint32 u32NumOutputTransfers;/**< Number of transfers of N bits of output data.*/

   uint32 u32NumInputTransfers;/**< Number of transfers of Nbits of input data.*/

   DALSYSEventHandle dalSysEvent;/**< If the client needs to start an asynchronous transfer, 
								   it needs to allocate a Dal event object, and pass the handle 
								   to that object in this field. The DalSpi driver will wake this 
								   event once the transfer is complete. However, if the client 
								   wants to start a synchronous transfer, then it needs to set 
								   this field to a NULL.*/

   uint32 queueIfBusy;/**< If set to true, the transfer is queued in the driver if the 
						core is busy.*/

   // table of use cases (no error and queue is not full):
   // SPI core is free
   // hEvent    queueIfBusy
   // non-null  x               start the transfer, return SPI_QUEUED, notify client later
   // null      x               start the transfer, block until transfer complete, return SPI_COMPLETE
   // 
   // SPI core is busy
   // hEvent    queueIfBusy
   // non-null  true            queue the transfer, return SPI_QUEUED, notify client later
   // null      true            queue the transfer, block until transfer complete, return SPI_COMPLETE
   // x         false           return SPI_BUSY
   //
   // x means does not care

   // Specify the transfer mode to do the transfer.
   // SPI_TRANSFER_MODE_DEFAULT means let SPI DAL choose for the client
   SpiTransferModeType transferMode;/**< Spi transfer mode type to be used for the transfer.*/

   // Specify the priority of this transfer.
   // This field is ignored at this time. Set this to be 0.
   uint32 u32Priority;/**< This field is ignored by the driver right now. It should be set to 0*/
};
 /** 
@} 
*/


DALResult GetInputBlockSize(DalDeviceHandle h,
    uint32 * inputBlockSize);

DALResult GetOutputBlockSize(DalDeviceHandle h,
    uint32 * outputBlockSize);

DALResult ConfigureDevice(DalDeviceHandle h,
     const SpiDeviceInfoType * deviceInfo);

DALResult GetDeviceConfiguration(DalDeviceHandle h,
    SpiDeviceInfoType * deviceInfo);

DALResult StartTransfer(DalDeviceHandle h,
    SpiTransferType * transfer,
     SpiDataAddrType * outputAddr,  int outputDataLen,
     SpiDataAddrType * inputAddr,  int inputDataLen,  int* inputDataLenReq, 
     uint32* transferId);

DALResult GetTransferStatus(DalDeviceHandle h,
     uint32 transferId);

DALResult Send(DalDeviceHandle h,
     unsigned char* outputData, int outputDataLen);

DALResult Receive(DalDeviceHandle h, 
     uint32 u32NumInputTransfers,  unsigned char* inputData,
    int inputDataLen,  int* inputDataLenReq);

DALResult SendAndReceive(DalDeviceHandle h,
     unsigned char* outputData, 
     int outputDataLen,  uint32 u32NumInputTransfers, 
     unsigned char* inputData, int inputDataLen, 
     int* inputDataLenReq);

DALResult Reset(DalDeviceHandle h);
typedef struct DalSpi DalSpi;
struct DalSpi
{
   DalDevice DalDevice;
   DALResult (*GetInputBlockSize)(DalDeviceHandle * _h, void * inputBlockSize, uint32 olen1);
   DALResult (*GetOutputBlockSize)(DalDeviceHandle * _h, void * outputBlockSize, uint32 olen1);
   DALResult (*ConfigureDevice)(DalDeviceHandle * _h,  const SpiDeviceInfoType *  deviceInfo);
   DALResult (*GetDeviceConfiguration)(DalDeviceHandle * _h, void * deviceInfo, uint32 olen1);
   DALResult (*StartTransfer)(DalDeviceHandle * _h,  SpiTransferType *  transfer, SpiDataAddrType * outputAddr,  int  outputDataLen, SpiDataAddrType * inputAddr,  int  inputDataLen, int * inputDataLenReq, uint32 olen6, void * transferId, uint32 olen7);
   DALResult (*GetTransferStatus)(DalDeviceHandle * _h,  uint32  transferId);
   DALResult (*Send)(DalDeviceHandle * _h, SpiDataAddrType * outputAddr,  int  outputDataLen);
   DALResult (*Receive)(DalDeviceHandle * _h,  uint32  u32NumInputTransfers, SpiDataAddrType * inputAddr,  int  inputDataLen, int* inputDataLenReq, uint32 olen4);
   DALResult (*SendAndReceive)(DalDeviceHandle * _h, SpiDataAddrType * outputAddr,  int  outputDataLen,  uint32  u32NumInputTransfers, SpiDataAddrType * inputAddr,  int  inputDataLen, int * inputDataLenReq, uint32 olen6);
   DALResult (*Reset)(DalDeviceHandle * _h);
};

typedef struct DalSpiHandle DalSpiHandle; 
struct DalSpiHandle 
{
   uint32 dwDalHandleId;
   const DalSpi * pVtbl;
   void * pClientCtxt;
};

/**
@addtogroup spi_dal_api
@{ */
/**
*DAL_SpiDeviceAttach()
*
*	@brief
*	Just a convenient wrapper around DAL_DeviceAttachEx.
*
*
*	@param DevId - DalDevice Id of the SPI device to which the client intends to 
*				   attach.
*	@param hDalDevice - A pointer location to be filled by the driver with a handle 
*			to the DalDevice.
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
#define DAL_SpiDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALSPI_INTERFACE_VERSION,hDalDevice)
/** 
@} 
*/
/**
@addtogroup spi_dal_api
@{ */
/**
	@brief
	Provides the FIFO input block size in unit of word, ie, 32 bits.
	Note: We use term word for 32 bits instead of double word for 
	simplicity here.


  	@param _h - Pointer to a DAL device handle
  	@param inputBlockSize - Pointer to store the value of requested input block size

	@dependencies
  	None

	@return
	DAL_SUCCESS if successful, DAL_ERROR if an error occured

*/
static __inline DALResult
DalSpi_GetInputBlockSize(DalDeviceHandle * _h,  uint32 *  inputBlockSize)
{
   return ((DalSpiHandle *)_h)->pVtbl->GetInputBlockSize( _h, (void * )inputBlockSize,sizeof( uint32  ));
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_GetOutputBlockSize()
*
*@brief
*	Provides the FIFO output block size in unit of word, ie, 32 bits.
*	Note: We use term word for 32 bits instead of double word for 
*	simplicity here.
*
*
*  @param _h - Pointer to a DAL device handle
*  @param outputBlockSize - Pointer to store the value of requested output block size
*
*@dependencies
*  None
*
*@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
**/
static __inline DALResult
DalSpi_GetOutputBlockSize(DalDeviceHandle * _h,  uint32 *  outputBlockSize)
{
   return ((DalSpiHandle *)_h)->pVtbl->GetOutputBlockSize( _h, (void * )outputBlockSize,sizeof( uint32  ));
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_ConfigureDevice()
*
*@brief
*	Configures the spi core for a SPI stransaction. No spi core setting
*	is done at this time. The real configuration to the spi core is conducted
*	just before a SPI stransaction.
*    SPI DAL keeps the setting for this device object until another 
*    ConfigureDevice call to alter the setting or the device is detached.
*    queueIfBusy is used to indicate if the client want to queue the
*    transfer if the SPI core is used by other client at the moment
*    If successful, client can continue to do communication under this setting.
*
*
*  @param _h - Pointer to a DAL device handle
*  @param deviceInfo- Pointer to data containing configuration information
*
*@dependencies
*  None
*
*@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
static __inline DALResult
DalSpi_ConfigureDevice(DalDeviceHandle * _h,  const SpiDeviceInfoType *  deviceInfo)
{
   return ((DalSpiHandle *)_h)->pVtbl->ConfigureDevice( _h, deviceInfo);
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_GetDeviceConfiguration()
*
*@brief
*	Get the current device configuration for this SPI DAL instance
*	This is either the one configured by ConfigureDevice or the 
*	default configuration.
*
*
*  @param _h - Pointer to a DAL device handle
*  @param deviceInfo- Pointer to address to store the device configuration information
*
*@dependencies
*  None
*
*@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
static __inline DALResult
DalSpi_GetDeviceConfiguration(DalDeviceHandle * _h,  SpiDeviceInfoType *  deviceInfo)
{
   return ((DalSpiHandle *)_h)->pVtbl->GetDeviceConfiguration( _h, (void * )deviceInfo,sizeof( SpiDeviceInfoType  ));
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_StartTransfer()
*
*@brief
*  	Conduct a bidirectional data transfer.
*  	The data transfer will be queued for its desired SPI core with 
*  	its desired configuration.
*  	
*  	This function is synchronous if transfer.hEvent is null. Otherwise it is 
*  	async. It return immediately. It notifies the client of the transfer status 
*  	changes by transfer.hEvent. Client can check the status by GetTransferStatus.
* 
*  	outputData and inputData's buffer pointers are SpiAddrType types.
*  	Their usages are as follows:
*  	If unpacking is enabled, the output buffer is interpreted as word array, 
*  	  and is casted into (uint32 *)
*  	If packing is enabled, the input buffer is interpreted as word array, 
*  	  and is casted into (uint32 *)
*  	Otherwise, there are no packing/unpacking here:
*  	  N <= 8, (void *) is interpreted as byte array, and is casted into (uint8 *)
*  	  8 < N <=16 (void *) is interpreted as half-word array, and is casted into (uint16 *)
*  	  N > 16 (void *) is interpreted as word array, and is casted into (uint32 *)
*  	The above rules also applies to the same name parameter in Send, Receive, and SendAndReceive
*  	To illustrate the rules, here is the table to show data flow with different 
*  	  N and unpacking settings for outputData. inputData works similarly but governed
*  	  with Packing option:
* 
*  	  A 32Bit Data in the FIFO = 0x1234abcd, Word passed in by client
*  	                           =     0xabcd, Half Word passed in by client
*  	                           =       0xcd, Byte passed in by client
*  	  
*  	  N               UnPk       Transfers
*  	                  En         #1           #2      #3      #4      Comments
*  	   
*  	  32              X          0x1234abcd                           Client passes in Word Array.
*  	  16              0          0xabcd                               Client passes in Half Word Array.
*  	                  1          0xabcd       0x1234                  Client passes in Word Array.
*  	  8               0          0xcd                                 Client passes in Byte Array.
*  	                  1          0xcd         0xab    0x34    0x12    Client passes in Word Array.
*  	                  
*  	  28              X          0x234abcd                            Client passes in Word Array.
*  	  (16 < N < 32)
* 
*  	  12              0          0xbcd                                Client passes in Half Word Array.
*  	  (8<N<16)        1          0xbcd        0x234                   Client passes in Word Array.
* 
*  	  4               0          0xd                                  Client passes in Byte Array.
*  	  (N<8)           1          0xd          0xb     0x4     0x2     Client passes in Word Array.
*  	  X indiate does not matter.
*
*
*  @param _h - Pointer to a DAL device handle
*  @param transfer - Pointer to information describing the transfer. 
*  @param outputAddr - Pointer to output data.
*  @param outputDataLen - Lenght of output data buffer.
*  @param inputAddr - Pointer to input Data.
*  @param inputDataLen - Length of input data buffer.
*  @param inputDataLenReq - Length of input data received.
*  @param transferId - Pointer to location where the transferId assigned by the SPI  driver is
*               stored.
*
*  Note: inputDataLen and outputDataLen represents length in bytes. If its more than 32KBytes
*	     transfer request will not be supported due to BAM mode limitation.
*
*@dependencies
*  None
*
*@return
*     SPI_COMPLETE   The transfer is successfully completed.
*     SPI_FAILURE    The transfer failed.
*     SPI_QUEUED     The transfer is still in the queue.
*     SPI_IN_PROGRESS The transfer is being processed by the SPI core.
*     NOTE: SpiTransferStatus is casted DALResult and retured to client.
*	 Client's can interpret this result as per their needs.
*/
static __inline DALResult
DalSpi_StartTransfer(DalDeviceHandle * _h,  SpiTransferType *  transfer,  SpiDataAddrType*  outputAddr,  int  outputDataLen,  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq,  uint32*  transferId)
{
   return ((DalSpiHandle *)_h)->pVtbl->StartTransfer( _h, transfer, outputAddr, outputDataLen, inputAddr, inputDataLen, inputDataLenReq,sizeof( int ), (void * )transferId,sizeof( uint32 ));
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_GetTransferStatus()
*
*@brief
*  Check the status of the transaction.
*
*
*  @param _h - Pointer to a DAL device handle
*  @param transferId - Every transfer is identified with a unique id. To get the
*  status of a transfer the client provides this id.
*
*@dependencies
*  None
*
*@return
*     SPI_COMPLETE   The transfer is successfully completed.
*     SPI_FAILURE    The transfer failed.
*     SPI_QUEUED     The transfer is still in the queue.
*     SPI_IN_PROGRESS The transfer is being processed by the SPI core.
*     SPI_INVALID_ID The transfer ID is invalid.
*     NOTE: SpiTransferStatus is casted DALResult and retured to client.
*	 Client's can interpret this result as per their needs.
*
*/
static __inline DALResult
DalSpi_GetTransferStatus(DalDeviceHandle * _h,  uint32  transferId)
{
   return ((DalSpiHandle *)_h)->pVtbl->GetTransferStatus( _h, transferId);
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_Send()
*
*@brief
*	Conduct a data write-only transfer, send out output data.
*	The data transfer will be queued for its desired SPI core with 
*	its desired configuration.
*	This function is synchrous. It blocks until all data are written out or failure happens.
*
*
*  @param _h - Pointer to a DAL device handle
*  @param outputAddr - Pointer to output data.
*  @param outputDataLen - Lenght of output data buffer.
*
*@dependencies
*  None
*
*  Note: outputDataLen represents length in bytes. If its more than 32KBytes
*	     transfer request will not be supported due to BAM mode limitation.
*
*@return
*     SPI_COMPLETE   The transfer is successfully completed.
*     SPI_FAILURE    The transfer failed.
*     SPI_FULL       SPI DAL's working queue is full. It cannot process
*				    this request.
*     NOTE: SpiTransferStatus is casted DALResult and retured to client.
*	 Client's can interpret this result as per their needs.
*
**/
static __inline DALResult
DalSpi_Send(DalDeviceHandle * _h,  SpiDataAddrType*  outputAddr,  int  outputDataLen)
{
   return ((DalSpiHandle *)_h)->pVtbl->Send( _h, outputAddr, outputDataLen);
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_Receive()
*
*@brief
* 	Conduct a data read-only transfer, get input data.
* 	The data transfer will be queued for its desired SPI core with 
* 	its desired configuration.
* 	This function is synchrous. It blocks until all data are read or failure happens
*
*
*  @param _h - Pointer to a DAL device handle
*  @param inputAddr - Pointer to input Data.
*  @param inputDataLen - Length of input data buffer.
*  @param inputDataLenReq - Length of input data received.
*  @param u32NumInputTransfers - No of spi transfer that needs to be executed.
*
*@dependencies
*  None
*
*  Note: inputDataLen represents length in bytes. If its more than 32KBytes
*	     transfer request will not be supported due to BAM mode limitation.
*
*@return
*     SPI_COMPLETE   The transfer is successfully completed.
*     SPI_FAILURE    The transfer failed.
*     SPI_FULL       SPI DAL's working queue is full. It cannot process this request.
*     NOTE: SpiTransferStatus is casted DALResult and retured to client.
*	 Client's can interpret this result as per their needs.
*
**/
static __inline DALResult
DalSpi_Receive(DalDeviceHandle * _h,  uint32  u32NumInputTransfers,  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq)
{
   return ((DalSpiHandle *)_h)->pVtbl->Receive( _h, u32NumInputTransfers, inputAddr, inputDataLen, inputDataLenReq,sizeof( int ));
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_SendAndReceive()
*
*@brief
* 	Conduct a bidirectional data transfer.
* 	The data transfer will be conducted for its desired SPI core with 
* 	its desired configuration.
* 	This function is synchrous. It blocks until all data are read or failure happens
*
*
*  @param _h - Pointer to a DAL device handle
*  @param outputAddr - Pointer to output data.
*  @param outputDataLen - Lenght of output data buffer.
*  @param u32NumInputTransfers - No of spi transfer that needs to be executed.
*  @param inputAddr - Pointer to input Data.
*  @param inputDataLen - Length of input data buffer.
*  @param inputDataLenReq - Length of input data received.
*
*@dependencies
*  None
*
*  Note: inputDataLen and outputDataLen represents length in bytes. If its more than 32KBytes
*	     transfer request will not be supported due to BAM mode limitation.
*
*@return
*     SPI_COMPLETE   The transfer is successfully completed.
*     SPI_FAILURE    The transfer failed.
*     SPI_FULL       SPI DAL's working queue is full. It cannot process this request.
*     NOTE: SpiTransferStatus is casted DALResult and retured to client.
*	 Client's can interpret this result as per their needs.
*
*/
static __inline DALResult
DalSpi_SendAndReceive(DalDeviceHandle * _h,  SpiDataAddrType*  outputAddr,  int  outputDataLen,  uint32  u32NumInputTransfers,  SpiDataAddrType*  inputAddr,  int  inputDataLen,  int*  inputDataLenReq)
{
   return ((DalSpiHandle *)_h)->pVtbl->SendAndReceive( _h, outputAddr, outputDataLen, u32NumInputTransfers, inputAddr, inputDataLen, inputDataLenReq,sizeof( int ));
}
/** 
@} 
*/
/**
@ingroup spi_dal_api
@{ */
/**
*DalSpi_Reset()
*
*@brief
* 	Abort all the transactions in the transaction queue. Soft reset the SPI core.
* 	This function is provided to slave mode transfer. In slave mode, the core
* 	replies on the master device to proceed the transfer. If master does not
* 	responds, the slave could stick in a transfer forever. This function
* 	can be used to get the slave out of the foreverness.
*
*
*  @param _h - Pointer to a DAL device handle
*
*@dependencies
*  None
*
*@return
*	DAL_SUCCESS    The operation is successfully completed.
*	DAL_ERROR      The operation failed.
*
*/
static __inline DALResult
DalSpi_Reset(DalDeviceHandle * _h)
{
   return ((DalSpiHandle *)_h)->pVtbl->Reset( _h);
}
/** 
@} 
*/
#endif
