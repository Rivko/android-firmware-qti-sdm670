/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */
#include <SmbusSharedLib.h>
#include <DALSys.h>

EFI_I2C_REQUEST_PACKET2 rp;
i2c_buffer_descriptor gBufferDescriptors[2];

// 36 is the maximum number of bytes needed to represent an smbus transfer
// This happens in a block writei/read when with 32 byte transfer and PEC
UINT8 gRequestBuffer[36];

UINT8
SmbusCrc8 (
  IN UINT8 Poly,
  IN UINT8 *Data,
  IN UINTN Length
)
{
  UINT32 Crc = Poly;
  UINTN i;
  if (Length == 0) {
      return 0;
  }
  while (Length-- != 0) {
    Crc ^= *Data++;
    Crc <<= 8;
    for (i = 0; i< 8; i++){
      if ((Crc & 0x8000) != 0) {
        Crc ^= SMBUS_CRC8_POLYNOMIAL;
      }
      Crc <<= 1;
    }
    Crc >>= 8;
  }
  return Crc;
}

VOID
AppendPecCrcToRequest(
  IN OUT    EFI_I2C_REQUEST_PACKET        *RequestPacket)
{
  //This length includes the PEC byte
  UINTN Length = RequestPacket->Operation[0].LengthInBytes;
  UINT8 Crc;

  Crc = SmbusCrc8(0,
                  gRequestBuffer,
                  Length);

  RequestPacket->Operation[0].Buffer[Length-1] = Crc;
  return;
}

EFI_STATUS
SmbusInitiateTransfer(
  IN      i2c_instance              Instance,
  IN      EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND  Command,
  IN      EFI_SMBUS_OPERATION       Operation,
  IN      BOOLEAN                   PecCheck,
  IN OUT  UINTN                     *Length,
  IN OUT  VOID                      *Buffer
)
{
    i2c_status I2cStatus;
    EFI_STATUS Status;
    i2c_config I2cConfig;
    VOID*     i2c_handle = NULL;

    // Cannot have a NULL buffer for operations outside of Quick Read/Write
    // Cannot have a Length > 0 for Quick Read/Write
    if (Operation == EfiSmbusQuickWrite || Operation == EfiSmbusQuickRead) {
        if ((NULL != Length) && (*Length > 0)) {
            return EFI_INVALID_PARAMETER;
        }
    } else if (Buffer == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    EFI_I2C_REQUEST_PACKET *RequestPacket = NULL;

    // Create a request packet based on the operation
    Status = GetRequestPacket(Command, Operation, PecCheck, SlaveAddress, Length, Buffer, &RequestPacket);

    // Ensure nothing went wrong in package creation
    if(Status != EFI_SUCCESS){
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    // Create a transfer packet from the request packet
    i2c_transfer_list       Transfers[2];
    Status = ConvertRequestPacketToTransferPacket(RequestPacket, (i2c_transfer_list*)Transfers);
    if(EFI_ERROR(Status)){
        return Status;
    }

    //
    // Set the default controller frequency, slave address, and slave type
    //
    I2cConfig.bus_frequency_khz    = QUPE_DEFAULT_BUS_FREQ_KHZ;
    I2cConfig.slave_address        = SlaveAddress.SmbusDeviceAddress;
    I2cConfig.slave_address_type   = I2C_07_BIT_SLAVE_ADDRESS;

    // Open a handle
    I2cStatus = i2c_open(Instance, &i2c_handle);
    Status = I2cStatusToEfiStatus(I2cStatus);
    if(EFI_ERROR(Status)){
        return Status;
    }

    UINT32 read, written;
    // Perform the transfer
    I2cStatus = i2c_transfer(i2c_handle,
                             &I2cConfig,
                             Transfers,
                             RequestPacket->OperationCount,
                             &written,
                             &read,
                             I2C_TRANSFER_TIMEOUT);
    Status = I2cStatusToEfiStatus(I2cStatus);

    // Close the opened instance
    i2c_close(i2c_handle);

    if(EFI_ERROR(Status)){
        return Status;
    }
    // Populate the return buffer
    Status = ProcessResultsFromTransferList(Operation,
                                            Transfers,
                                            PecCheck,
                                            Length,
                                            Buffer);
    return Status;
}

/* -------------------- GetRequestPacket functions -----------------------------
 *
 *  These functions generate a RequestPacket based upon the associated
 *  command. The format of the RequestPacket is outlined in the
 *  UEFI Platform Initialization Specification Volume 5.
 *
 *
 * ---------------------------------------------------------------------------*/

static
EFI_STATUS
GetRequestPacketForEfiSmbusQuickRead(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  UINTN OperationCount = 1;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 0;
  rp.Operation[0].Flags = I2C_FLAG_READ;
  return EFI_SUCCESS;
}


static
EFI_STATUS
GetRequestPacketForEfiSmbusQuickWrite(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  UINTN OperationCount = 1;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 0;
  rp.Operation[0].Flags = 0;
  return EFI_SUCCESS;
}


static
EFI_STATUS
GetRequestPacketForEfiSmbusReceiveByte(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  if(*Length < sizeof(UINT8)) {
    return EFI_BUFFER_TOO_SMALL;
  }
  UINTN OperationCount = 1;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 1;
  rp.Operation[0].Flags = I2C_FLAG_SMBUS_OPERATION | I2C_FLAG_READ;
  rp.Operation[0].Buffer = gRequestBuffer + 1;

  if (PecCheck){
    rp.Operation[0].LengthInBytes++;
    rp.Operation[0].Flags |= I2C_FLAG_SMBUS_PEC;
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
GetRequestPacketForEfiSmbusSendByte(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  if(*Length < sizeof(UINT8)){
    return EFI_BUFFER_TOO_SMALL;
  }
  UINTN OperationCount = 1;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 1;
  rp.Operation[0].Flags = I2C_FLAG_SMBUS_OPERATION;
  rp.Operation[0].Buffer = gRequestBuffer + 1;
  rp.Operation[0].Buffer[0] = ((UINT8 *) Buffer)[0];

  if (PecCheck){
    rp.Operation[0].LengthInBytes++;
    rp.Operation[0].Flags |= I2C_FLAG_SMBUS_PEC;
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
GetRequestPacketForEfiSmbusReadByte(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  if(*Length < (sizeof(UINT8))){
    return EFI_BUFFER_TOO_SMALL;
  }
  UINTN OperationCount = 2;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 1;
  rp.Operation[0].Flags = I2C_FLAG_SMBUS_OPERATION;
  rp.Operation[0].Buffer = gRequestBuffer + 1;
  rp.Operation[0].Buffer[0] = Command;
  rp.Operation[1].LengthInBytes = 1;
  rp.Operation[1].Flags = I2C_FLAG_READ;
  rp.Operation[1].Buffer = gRequestBuffer + 2;
  if(rp.Operation[1].Buffer == NULL){
    return EFI_OUT_OF_RESOURCES;
  }

  if (PecCheck){
    rp.Operation[0].Flags |= I2C_FLAG_SMBUS_PEC;
    rp.Operation[1].LengthInBytes++;
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
GetRequestPacketForEfiSmbusWriteByte(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  if(*Length < (sizeof(UINT8))){
    return EFI_BUFFER_TOO_SMALL;
  }
  UINTN OperationCount = 1;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 2;
  rp.Operation[0].Flags = I2C_FLAG_SMBUS_OPERATION;
  rp.Operation[0].Buffer = gRequestBuffer + 1;
  rp.Operation[0].Buffer[0] = Command;
  rp.Operation[0].Buffer[1] = ((UINT8 *) Buffer)[0];

  if (PecCheck){
    rp.Operation[0].LengthInBytes++;
    rp.Operation[0].Flags |= I2C_FLAG_SMBUS_PEC;
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
GetRequestPacketForEfiSmbusReadBlock(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  UINTN OperationCount = 2;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 1;
  rp.Operation[0].Flags = (I2C_FLAG_SMBUS_OPERATION | I2C_FLAG_SMBUS_BLOCK);
  rp.Operation[0].Buffer = gRequestBuffer + 1;
  rp.Operation[0].Buffer[0] = Command;
  rp.Operation[1].LengthInBytes = *Length + 1;
  rp.Operation[1].Flags = I2C_FLAG_READ;
  rp.Operation[1].Buffer = gRequestBuffer + 2;

  if (PecCheck){
    rp.Operation[0].Flags |= I2C_FLAG_SMBUS_PEC;
    rp.Operation[1].LengthInBytes++;
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
GetRequestPacketForEfiSmbusWriteBlock(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  UINTN OperationCount = 1;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = (*Length) + 2;
  rp.Operation[0].Flags = I2C_FLAG_SMBUS_OPERATION | I2C_FLAG_SMBUS_BLOCK;
  rp.Operation[0].Buffer = gRequestBuffer + 1;
  rp.Operation[0].Buffer[0] = Command;
  rp.Operation[0].Buffer[1] = *Length;
  CopyMem(rp.Operation[0].Buffer + 2, Buffer, *Length);

  if (PecCheck){
    rp.Operation[0].LengthInBytes++;
    rp.Operation[0].Flags |= I2C_FLAG_SMBUS_PEC;
  }

  return EFI_SUCCESS;
}

static
EFI_STATUS
GetRequestPacketForEfiSmbusProcessCall(
  IN  EFI_SMBUS_DEVICE_COMMAND  Command,
  IN  BOOLEAN                   PecCheck,
  IN  UINTN                     *Length,
  IN  VOID                      *Buffer
){
  if(*Length < sizeof(UINT16)){
    return EFI_BUFFER_TOO_SMALL;
  }
  UINTN OperationCount = 2;

  rp.OperationCount = OperationCount;
  rp.Operation[0].LengthInBytes = 3;
  rp.Operation[0].Flags = I2C_FLAG_SMBUS_OPERATION | I2C_FLAG_SMBUS_PROCESS_CALL;
  rp.Operation[0].Buffer = gRequestBuffer + 1;
  rp.Operation[0].Buffer[0] = Command;
  rp.Operation[0].Buffer[1] = ((UINT8 *)Buffer)[0];
  rp.Operation[0].Buffer[2] = ((UINT8 *)Buffer)[1];
  rp.Operation[1].LengthInBytes = 2;
  rp.Operation[1].Flags = I2C_FLAG_READ;
  rp.Operation[1].Buffer = gRequestBuffer + 4;

  if (PecCheck){
    rp.Operation[1].LengthInBytes++;
    rp.Operation[0].Flags |= I2C_FLAG_SMBUS_PEC;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
GetRequestPacket(
  IN              EFI_SMBUS_DEVICE_COMMAND      Command,
  IN              EFI_SMBUS_OPERATION           Operation,
  IN              BOOLEAN                       PecCheck,
  IN              EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN OUT  UINTN                                 *Length,
  IN OUT  VOID                                  *Buffer,
  OUT             EFI_I2C_REQUEST_PACKET        **RequestPacket)
{
  EFI_STATUS Status;
  // Create a request packet based on the operation
  // Results get populated in gRequestBuffer, which will hold all buffers needed for
  // the transaction
  // The first byte of gRequestBuffer is the write bit appended to the end of the
  // slave address, this gets set here
  switch (Operation) {
    case EfiSmbusQuickRead:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 0;
      Status = GetRequestPacketForEfiSmbusQuickRead (Command, PecCheck, Length, Buffer);
      break;
    case EfiSmbusQuickWrite:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 1;
      Status = GetRequestPacketForEfiSmbusQuickWrite (Command, PecCheck, Length, Buffer);
      break;
    case EfiSmbusReceiveByte:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 0;
      Status = GetRequestPacketForEfiSmbusReceiveByte (Command, PecCheck, Length, Buffer);
      break;
    case EfiSmbusSendByte:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 1;
      Status = GetRequestPacketForEfiSmbusSendByte (Command, PecCheck, Length, Buffer);
      if (PecCheck) {
        AppendPecCrcToRequest((EFI_I2C_REQUEST_PACKET *)&rp);
      }
      break;
    case EfiSmbusReadByte:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 0;
      Status = GetRequestPacketForEfiSmbusReadByte (Command, PecCheck, Length, Buffer);
      break;
    case EfiSmbusWriteByte:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 1;
      Status = GetRequestPacketForEfiSmbusWriteByte (Command, PecCheck, Length, Buffer);
      if (PecCheck) {
        AppendPecCrcToRequest((EFI_I2C_REQUEST_PACKET *)&rp);
      }
      break;
    case EfiSmbusReadBlock:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 0;
      Status = GetRequestPacketForEfiSmbusReadBlock (Command, PecCheck, Length, Buffer);
      break;
    case EfiSmbusWriteBlock:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 1;
      Status = GetRequestPacketForEfiSmbusWriteBlock (Command, PecCheck, Length, Buffer);
      if (PecCheck) {
        AppendPecCrcToRequest((EFI_I2C_REQUEST_PACKET *)&rp);
      }
      break;
    case EfiSmbusProcessCall:
      gRequestBuffer[0] = (SlaveAddress.SmbusDeviceAddress << 1) | 0;
      Status = GetRequestPacketForEfiSmbusProcessCall (Command, PecCheck, Length, Buffer);
      break;
    // TODO: Support this feature once the interaction with the Buffer parameter
    // and size restrictions is understood
    case EfiSmbusBWBRProcessCall:
      Status = EFI_UNSUPPORTED;
    default:
      Status = EFI_INVALID_PARAMETER;
  }

  *RequestPacket = (EFI_I2C_REQUEST_PACKET *)&rp;

  return Status;
}

EFI_STATUS
ProcessResultsFromRequestPacket(
  IN  EFI_SMBUS_OPERATION       Operation,
  IN  EFI_I2C_REQUEST_PACKET    *RequestPacket,
  IN  BOOLEAN                   PecCheck,
  OUT UINTN                     *Length,
  OUT VOID                      *Buffer
){

  EFI_STATUS Status = EFI_SUCCESS;
  UINTN PecOffset;
  BOOLEAN ReadCommand = TRUE;

  // Depending on the operation, set the length and value of output buffer
  switch (Operation) {
    case EfiSmbusReceiveByte:
      *Length = sizeof(UINT8);
      PecOffset = 1;
      ((UINT8 *)Buffer)[0] = RequestPacket->Operation[0].Buffer[0];
      Status = EFI_SUCCESS;
      break;
    case EfiSmbusReadByte:
      *Length = sizeof(UINT8);
      PecOffset = 2;
      ((UINT8 *)Buffer)[0] = RequestPacket->Operation[1].Buffer[0];
      Status = EFI_SUCCESS;
      break;
    case EfiSmbusReadBlock:
      if (RequestPacket->Operation[1].Buffer[0] > *Length) {
        Status = EFI_BUFFER_TOO_SMALL;
        goto Done;
      }
      *Length = min(RequestPacket->Operation[1].Buffer[0],
                    RequestPacket->Operation[1].LengthInBytes);
      PecOffset = *Length + 2;
      CopyMem(Buffer, (RequestPacket->Operation[1].Buffer + 1), *Length);
      Status = EFI_SUCCESS;
      break;
    case EfiSmbusProcessCall:
      *Length = sizeof(UINT16);
      PecOffset = 5;
      ((UINT8 *)Buffer)[0] = RequestPacket->Operation[1].Buffer[0];
      ((UINT8 *)Buffer)[1] = RequestPacket->Operation[1].Buffer[1];
      Status = EFI_SUCCESS;
      break;
    // TODO: Support this feature once the interaction with the Buffer parameter
    // and size restrictions is understood
    case EfiSmbusBWBRProcessCall:
      break;
    default:
      ReadCommand = FALSE;
      break;
  }
  // Perform a verification if PecCheck is enabled
  if (PecCheck & ReadCommand){
    // We run the crc over all bytes transmitted and recieved, including the
    // PEC byte and the slave address and the write bit
    if (0 != SmbusCrc8(0, gRequestBuffer, PecOffset + 2)){
      Status = EFI_CRC_ERROR;
    }
  }

  // Otherwise don't use the buffer as output
Done:
  if(EFI_ERROR(Status)){
    *Length = 0;
  }
  return Status;
}

EFI_STATUS
ProcessResultsFromTransferList(
  IN  EFI_SMBUS_OPERATION       Operation,
  IN i2c_transfer_list          *Transfers,
  IN  BOOLEAN                   PecCheck,
  OUT UINTN                     *Length,
  OUT VOID                      *Buffer
){

  EFI_STATUS Status = EFI_SUCCESS;
  UINTN PecOffset;
  BOOLEAN ReadCommand = TRUE;

  // Depending on the operation, set the length and value of output buffer
  switch (Operation) {
    case EfiSmbusReceiveByte:
      *Length = sizeof(UINT8);
      PecOffset = 1;
      ((UINT8 *)Buffer)[0] = Transfers[0].desc->buffer[0];
      Status = EFI_SUCCESS;
      break;
    case EfiSmbusReadByte:
      *Length = sizeof(UINT8);
      PecOffset = 2;
      ((UINT8 *)Buffer)[0] = Transfers[1].desc->buffer[0];
      Status = EFI_SUCCESS;
      break;
    case EfiSmbusReadBlock:
      if (Transfers[1].desc->buffer[0] > *Length) {
        Status = EFI_BUFFER_TOO_SMALL;
        goto Done;
      }
      *Length = min(Transfers[1].desc->buffer[0],
                    Transfers[1].desc->length);
      PecOffset = *Length + 2;
      CopyMem(Buffer, (Transfers[1].desc->buffer + 1), *Length);
      Status = EFI_SUCCESS;
      break;
    case EfiSmbusProcessCall:
      *Length = sizeof(UINT16);
      PecOffset = 5;
      ((UINT8 *)Buffer)[0] = Transfers[1].desc->buffer[0];
      ((UINT8 *)Buffer)[1] = Transfers[1].desc->buffer[1];
      Status = EFI_SUCCESS;
      break;
    // TODO: Support this feature once the interaction with the Buffer parameter
    // and size restrictions is understood
    case EfiSmbusBWBRProcessCall:
      break;
    default:
      ReadCommand = FALSE;
      break;
  }
  // Perform a verification if PecCheck is enabled
  if (PecCheck & ReadCommand){
    // We run the crc over all bytes transmitted and recieved, including the
    // PEC byte and the slave address and the write bit
    if (0 != SmbusCrc8(0, gRequestBuffer, PecOffset + 2)){
      Status = EFI_CRC_ERROR;
    }
  }

  // Otherwise don't use the buffer as output
Done:
  if(EFI_ERROR(Status)){
    *Length = 0;
  }
  return Status;
}

EFI_STATUS
ConvertRequestPacketToTransferPacket(
  IN  EFI_I2C_REQUEST_PACKET    *RequestPacket,
  OUT i2c_transfer_list         *Transfers
){
  UINTN                 i;
  UINTN                 OperationCount;
  i2c_buffer_descriptor *buffer_descriptor;
  EFI_STATUS            Status;
  UINTN                 Length;

  if (NULL == RequestPacket){
    Status = EFI_INVALID_PARAMETER;
    goto Done;
  }

  OperationCount = RequestPacket->OperationCount;

  ASSERT(OperationCount <= 2);

  // Create transfer list from request packet
  for (i = 0; i < OperationCount; i++){
    Length = RequestPacket->Operation[i].LengthInBytes;
    buffer_descriptor = &gBufferDescriptors[i];
    buffer_descriptor->length = Length;
    buffer_descriptor->buffer = RequestPacket->Operation[i].Buffer;
    Transfers[i].desc = buffer_descriptor;
    Transfers[i].write = !(RequestPacket->Operation[i].Flags & I2C_FLAG_READ);
    Transfers[i].no_of_descs = 1;
    Transfers[i].delay_ms = 0;
  }

Done:
  return EFI_SUCCESS;
}

EFI_STATUS
ConvertTransferListToRequestPacket(
  IN   i2c_transfer_list         *Transfers,
  IN   UINTN                     TransferCount,
  OUT  EFI_I2C_REQUEST_PACKET    *RequestPacket
){
  return EFI_UNSUPPORTED;
}

EFI_STATUS
I2cStatusToEfiStatus(
  i2c_status I2cStatus
){
  EFI_STATUS Status;

  // Convert from i2c_status to EFI_STATUS
  switch (I2cStatus) {
    case I2C_SUCCESS:
    case I2C_ERROR_SLAVE_DATA_NACK:
      Status = EFI_SUCCESS;
      break;
    case I2C_ERROR_INVALID_PARAMETER:
      Status = EFI_INVALID_PARAMETER;
      break;
    case I2C_ERROR_INIT:
    case I2C_ERROR_DEINIT:
    case I2C_ERROR_POWER_ON:
    case I2C_ERROR_ALLOC_BUFFER_DESCRIPTOR:
    case I2C_ERROR_ALLOC_TRANSFER:
    case I2C_ERROR_SLAVE_ADDRESS_NACK:
    case I2C_ERROR_DRV_FWK_INIT:
    case I2C_ERROR_DRV_FWK_NULL_HANDLE:
    default:
      Status = EFI_DEVICE_ERROR;
      break;
  }
  return Status;
}
