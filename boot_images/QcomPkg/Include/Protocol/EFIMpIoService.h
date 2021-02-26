/** @file
  Blocking Multi-processor I/O Sevice

  This protocol faciliates communication from an auxiliary core
  to the primary core and response from the primary core to
  auxiliary cores

  Copyright (c) 2015, Qualcomm Technologies Inc. All rights reserved.

**/           

#ifndef __EFI_MPIO_SERVICE_H__
#define __EFI_MPIO_SERVICE_H__

#include <Uefi.h>

/* MPIO Protocol Guid */
extern EFI_GUID gQcomMPIOProtocolGuid;

#define EFI_MPIO_PROTOCOL_REVISION    0x00010000

typedef struct _EFI_MPIO_PROTOCOL  EFI_MPIO_PROTOCOL;

/**
  Register Port and Event. Caller must CheckEvent() to see if it has been signaled,
  and if not, must the wait on event using WaitForEvent()

  @param[in]  This                 Indicates a pointer to the calling context.
  @param[in]  Port                 The Port ID from which messages will be sent,
                                   and on which messages from auxiliary cores
                                   will be recived.
  @param[in]  Event                The Event to be signaled when there is activity
                                   on the port.

  @retval EFI_SUCCESS          The registration was successful.
  @retval EFI_ALREADY_STARTED  The Port ID has already been assigned an unsignaled
                               event.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MPIO_REGISTER) (
  IN EFI_MPIO_PROTOCOL            *This,
  IN UINT64                       Port,
  IN EFI_EVENT_NOTIFY             NotifyFunction, 
  IN CONST VOID                   *NotifyContext
  );

/**
  Read BufferSize bytes from Port into Buffer.
  
  This function should be called when the Event associated with the Port is signaled.
  Reads data at the specified port into Buffer. 

  Calling GetMessage() without a signaled event is an error.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       Port       Id of the port.
  @param[in]       BufferSize Size of Buffer, must not exceed 
                              EFI_PAGE_SIZE - sizeof(EFI_MPIO_PORT_HEADER) 
  @param[out]      Buffer     A pointer to the destination buffer for the data. The 
                              caller is responsible for either having implicit or 
                              explicit ownership of the buffer.

  @retval EFI_SUCCESS           The read request was successful.
  @retval EFI_INVALID_PARAMETER The port does not have any data to be read.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer is not large enough to hold the data.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_MPIO_RECV_MESG_FROM_AP) (
  IN     EFI_MPIO_PROTOCOL       *This,
  IN     UINT64                  Port,
     OUT UINT32                  *SubPort,
     OUT UINT32                  *ApCpuNumber,
  IN OUT UINTN                   *BufferSize,
     OUT VOID                    *Buffer
  );

/**
  Write BufferSize bytes into Buffer.

  This function should be called after GetMessage() to send acknowledgement or
  additional data to the auxiliary core. The auxiliary core is blocked until 
  this response is sent. 
  
  Calling SendMessage() without first calling GetMessage() as a result of an 
  event notification is an error.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       Port       Id of the port.
  @param[in]       BufferSize Size of Buffer, must not exceed 
                              EFI_PAGE_SIZE - sizeof(EFI_MPIO_PORT_HEADER)
  @param[in]       Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The write request was successful.
  @retval EFI_INVALID_PARAMETER The port has not been signaled or data has not
                                been read from the port.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer is not large enough to hold the data.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MPIO_SEND_MESG_TO_AP) (
  IN     EFI_MPIO_PROTOCOL        *This,
  IN     UINT64                   Port,
  IN     UINT32                   SubPort         OPTIONAL,
  IN     UINT32                   ApCpuNumber,
  IN     UINTN                    BufferSize,
  IN     VOID                     *Buffer
  );

/**
  The Multiprocessor I/O protocol provides blocking I/O service
  between the primary and auxiliary cores. I/O between auxiliary
  cores must go via the primary core.
  
**/
struct _EFI_MPIO_PROTOCOL {
  UINT32                      Revision;
  EFI_MPIO_REGISTER           RegisterPort;
  EFI_MPIO_RECV_MESG_FROM_AP  RecvMsgFromAp;
  EFI_MPIO_SEND_MESG_TO_AP    SendMsgToAp;
};


#endif  // __EFI_MPIO_SERVICE_H

