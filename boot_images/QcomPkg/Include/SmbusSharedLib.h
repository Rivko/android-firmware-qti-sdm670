/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Protocol/DriverBinding.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/I2cMaster.h>
#include <I2cApi.h>

#define SMBUS_CRC8_POLYNOMIAL       (0x1070 << 3)
#define QUPE_DEFAULT_BUS_FREQ_KHZ   400
#define I2C_TRANSFER_TIMEOUT        2000

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))


// Since all SMBUS requests are of length 2 or less
// we can reuse this struct which is castable into a
// normal EFI_I2C_REQUEST_PACKET
typedef struct {
  UINTN               OperationCount;
  EFI_I2C_OPERATION   Operation[2];
}
EFI_I2C_REQUEST_PACKET2;

UINT8
SmbusCrc8 (
  IN UINT8 Poly,
  IN UINT8 *Data,
  IN UINTN Length
);

VOID
AppendPecCrcToRequest(
  IN OUT    EFI_I2C_REQUEST_PACKET        *RequestPacket
);

EFI_STATUS
SmbusInitiateTransfer(
  IN      i2c_instance              Instance,
  IN      EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND  Command,
  IN      EFI_SMBUS_OPERATION       Operation,
  IN      BOOLEAN                   PecCheck,
  IN OUT  UINTN                     *Length,
  IN OUT  VOID                      *Buffer
);

EFI_STATUS
GetRequestPacket(
  IN              EFI_SMBUS_DEVICE_COMMAND      Command,
  IN              EFI_SMBUS_OPERATION           Operation,
  IN              BOOLEAN                       PecCheck,
  IN              EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN OUT  UINTN                                 *Length,
  IN OUT  VOID                                  *Buffer,
  OUT             EFI_I2C_REQUEST_PACKET        **RequestPacket
);

EFI_STATUS
ProcessResultsFromRequestPacket(
  IN  EFI_SMBUS_OPERATION       Operation,
  IN  EFI_I2C_REQUEST_PACKET    *RequestPacket,
  IN  BOOLEAN                   PecCheck,
  OUT UINTN                     *Length,
  OUT VOID                      *Buffer
);

EFI_STATUS
ProcessResultsFromTransferList(
  IN  EFI_SMBUS_OPERATION       Operation,
  IN i2c_transfer_list          *Transfers,
  IN  BOOLEAN                   PecCheck,
  OUT UINTN                     *Length,
  OUT VOID                      *Buffer
);


EFI_STATUS
ConvertRequestPacketToTransferPacket(
  IN  EFI_I2C_REQUEST_PACKET    *RequestPacket,
  OUT i2c_transfer_list         *Transfers
);

EFI_STATUS
ConvertTransferListToRequestPacket(
  IN   i2c_transfer_list         *Transfers,
  IN   UINTN                     TransferCount,
  OUT  EFI_I2C_REQUEST_PACKET    *RequestPacket
);

EFI_STATUS
I2cStatusToEfiStatus(
  i2c_status I2cStatus
);
