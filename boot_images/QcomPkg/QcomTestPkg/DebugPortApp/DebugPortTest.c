/** @file DebugPortTest.c
   
  Tests for Debug Port

  Copyright (c) 2012, 2019 Qualcomm Technologies, Inc.  All Rights Reserved. 
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/30/13   vk      Fix warning
 11/01/12   plc     New File from Joel Winarske for WinDbg support

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/DebugPort.h>

#include "DbgKd.h"


KD_RECV_CODE
  KdReceivePacket (
  IN EFI_DEBUGPORT_PROTOCOL *DebugPort,
  IN UINTN ReadTimeout,
  KD_PACKET **Packet,
  UINT8 **SecondaryData
  )
{
  EFI_STATUS  Status;

  UINTN TotalBytesToRead, BytesToRead, BytesRead;
  UINT8 *Buffer;
  UINT32 Checksum;
  UINTN Index;

  Buffer = (UINT8 *)*Packet;

  TotalBytesToRead = sizeof(KD_PACKET);
  BytesToRead = TotalBytesToRead;
  BytesRead = 0;
  Index = TotalBytesToRead - BytesToRead;

  do 
  {
    Status = DebugPort->Read ( DebugPort, ReadTimeout, &BytesToRead, &Buffer[Index] );
    if (BytesToRead > 0) AsciiPrint("\r\n%d-%d %04X\r\n", BytesToRead, TotalBytesToRead, Status);
    if (EFI_ERROR (Status))
      goto HeaderErr;

    BytesRead += BytesToRead;
    Index = TotalBytesToRead - BytesRead;

    if(BytesRead == TotalBytesToRead)
      break;

  } while (1);

  AsciiPrint("KdReceivePacket\r\n");

  if ((*Packet)->PacketLeader != PACKET_LEADER_CONTROL && (*Packet)->PacketLeader != PACKET_LEADER_DATA) {
    AsciiPrint("  Unknown PacketLeader = 0x%04X\r\nFLUSHING PORT!\r\n", (*Packet)->PacketLeader);
    Status = DebugPort->Reset ( DebugPort ); /* Packet Framing Error */
    goto HeaderErr;
  }

  AsciiPrint("  PacketLeader .. ");
  switch ((*Packet)->PacketLeader) {
  case PACKET_LEADER_CONTROL:           AsciiPrint("CONTROL\r\n"); break;
  case PACKET_LEADER_DATA:              AsciiPrint("DATA\r\n"); break;
  }

  AsciiPrint("  PacketType .... ");
  switch ((*Packet)->PacketType) {
  case PACKET_TYPE_KD_STATE_CHANGE32:   AsciiPrint("STATE_CHANGE32\r\n"); break;
  case PACKET_TYPE_KD_MANIPULATE_STATE: AsciiPrint("STATE_MANIPULATE\r\n"); break;
  case PACKET_TYPE_KD_DEBUG_IO:         AsciiPrint("DEBUG_IO\r\n"); break;
  case PACKET_TYPE_KD_ACKNOWLEDGE:      AsciiPrint("ACKNOWLEDGE\r\n"); break;
  case PACKET_TYPE_KD_RESEND:           AsciiPrint("RESEND\r\n"); break;
  case PACKET_TYPE_KD_RESET:            AsciiPrint("RESET\r\n"); break;
  case PACKET_TYPE_KD_STATE_CHANGE64:   AsciiPrint("STATE_CHANGE64\r\n"); break;
  case PACKET_TYPE_KD_POLL_BREAKIN:     AsciiPrint("POLL_BREAKIN\r\n"); break;
  case PACKET_TYPE_KD_TRACE_IO:         AsciiPrint("TRACE_IO\r\n"); break;
  case PACKET_TYPE_KD_CONTROL_REQUEST:  AsciiPrint("CONTROL_REQUEST\r\n"); break;
  case PACKET_TYPE_KD_FILE_IO:          AsciiPrint("FILE_IO\r\n"); break;
  default: AsciiPrint("\r\n");
  }

  AsciiPrint("  PacketId ...... 0x%04X\r\n", (*Packet)->PacketId);

  /* Get Data */    
  if ((*Packet)->ByteCount != 0) {

    AsciiPrint("  ByteCount ..... %d\r\n", (*Packet)->ByteCount);

    TotalBytesToRead = (*Packet)->ByteCount + 1;  /* include data terminator */
    BytesToRead = TotalBytesToRead;
    BytesRead = 0;
    Index = TotalBytesToRead - BytesToRead;

    do
    {
      Status = DebugPort->Read ( DebugPort, ReadTimeout, &BytesToRead, &(*SecondaryData)[Index] );
      if (EFI_ERROR (Status))
        goto DataErr;

      BytesRead += BytesToRead;
      Index = TotalBytesToRead - BytesRead;

      if(BytesRead == TotalBytesToRead)
        break;

    } while (1);

    if ((*SecondaryData)[(*Packet)->ByteCount] != PACKET_DATA_TRAILING_BYTE) {
      AsciiPrint("Data trailing byte not found! 0x%02X\r\n", (*SecondaryData)[(*Packet)->ByteCount]);
      goto DataErr;
    }

    /* validate checksum */
    Checksum = 0;
    for (Index = 0; Index < (*Packet)->ByteCount; Index++)
      Checksum += (*SecondaryData)[Index];

    if ((*Packet)->Checksum != Checksum) {
      AsciiPrint("Invalid Checksum! 0x%04X should be 0x%04X\r\n", Checksum, (*Packet)->Checksum);
      goto DataErr;
    }

    for (Index = 1; Index <= (*Packet)->ByteCount; Index++) {
      AsciiPrint(" %02X", (*SecondaryData)[Index-1]);
      if ((Index % 16) == 0) AsciiPrint("\r\n");
    }
  }

  return KD_RECV_CODE_OK;

DataErr:
HeaderErr:

  if (Status == EFI_TIMEOUT)
    return KD_RECV_CODE_TIMEOUT;

  return KD_RECV_CODE_FAILED;
}

EFI_STATUS
  KdSendPacket (
  IN EFI_DEBUGPORT_PROTOCOL *DebugPort,
  IN UINTN WriteTimeout,
  IN KD_PACKET *Packet,  /* Pre-allocated maximum KD Packet buffer */
  IN UINTN SecondaryDataLen,
  IN VOID *SecondaryData
  )
{
  EFI_STATUS Status = EFI_NOT_READY; /*For KlockWork*/
  UINT32 BufferLen;
  UINT8 *Buffer;
  UINT8 *p;
  UINTN i, Count;

  Buffer = (UINT8 *)Packet;

  BufferLen = sizeof(KD_PACKET) + ((SecondaryDataLen) ? SecondaryDataLen + 1 : 0);

  if (SecondaryDataLen != 0) {
    Packet->ByteCount = SecondaryDataLen;

    /* terminate data packet */
    Buffer[sizeof(KD_PACKET) + SecondaryDataLen] = PACKET_DATA_TRAILING_BYTE;

    /* Calculate Checksum - need to validate the roll over case */
    Packet->Checksum = 0;
    p = (UINT8 *) SecondaryData;
    for (i = 0; i < SecondaryDataLen; i++)
      Packet->Checksum += *p++;
  }

  AsciiPrint("\r\nKdSendPacket\r\n");
  for (i = 1; i <= BufferLen; i++) {
    AsciiPrint(" %02X", Buffer[i-1]);
    if ((i % 16) == 0) AsciiPrint("\n");
  }
  AsciiPrint("\r\n");

  /* Why single byte write? because that's how bootmgr/winload do it... */
  Count = BufferLen;
  for (i = 0; i < Count; i++) {
    BufferLen = 1;
    Status = DebugPort->Write ( DebugPort, WriteTimeout, &BufferLen, &Buffer[i] );
  }

  return Status;
//  return DebugPort->Write ( DebugPort, WriteTimeout, &BufferLen, Buffer );
}

EFI_STATUS
  DbgIoPrintString (
  IN EFI_DEBUGPORT_PROTOCOL *DebugPort,
  IN UINT8 *Buffer,
  IN UINT32 PacketId,
  IN UINT16 ProcessorLevel,
  IN UINT16 Processor,
  IN UINT32 Id,
  IN UINT8 *Str,
  IN UINTN Len
  )
{
  KD_PACKET *Packet;
  UINTN SecondaryDataLen;
  UINT8 *SecondaryData;
  DBGKD_DEBUG_IO *DebugIo;

  Packet = (KD_PACKET *)Buffer;
  SecondaryData = &Buffer[sizeof(KD_PACKET)];
  SecondaryDataLen = sizeof(DBGKD_DEBUG_IO) + Len;

  Packet->PacketLeader = PACKET_LEADER_DATA;
  Packet->PacketType = PACKET_TYPE_KD_DEBUG_IO;
  Packet->PacketId = PacketId;
  Packet->ByteCount = 0;
  Packet->Checksum = 0;

  DebugIo = (DBGKD_DEBUG_IO *)SecondaryData;
  DebugIo->ApiNumber = DbgKdPrintStringApi;
  DebugIo->ProcessorLevel = ProcessorLevel;
  DebugIo->Processor = Processor;
  DebugIo->Id = Id;
  DebugIo->LengthOfString = Len;

  CopyMem (&SecondaryData[sizeof(DBGKD_DEBUG_IO)], Str, Len);

  return KdSendPacket ( DebugPort, 0, Packet, SecondaryDataLen, SecondaryData );
}

EFI_STATUS
  KdSendPacketReset (
  IN EFI_DEBUGPORT_PROTOCOL *DebugPort,
  IN UINT8 *Buffer
  )
{
  KD_PACKET *Packet;

  Packet = (KD_PACKET *)Buffer;

  Packet->PacketLeader = PACKET_LEADER_CONTROL;
  Packet->PacketType = PACKET_TYPE_KD_RESET;
  Packet->PacketId = 0;
  Packet->ByteCount = 0;
  Packet->Checksum = 0;

  return KdSendPacket ( DebugPort, 0, Packet, 0, NULL );
}

EFI_STATUS
  KdSendPacketAck (
  IN EFI_DEBUGPORT_PROTOCOL *DebugPort,
  IN UINT8 *Buffer
  )
{
  KD_PACKET *Packet;

  Packet = (KD_PACKET *)Buffer;

  Packet->PacketLeader = PACKET_LEADER_CONTROL;
  Packet->PacketType = PACKET_TYPE_KD_ACKNOWLEDGE;
  Packet->PacketId = INITIAL_PACKET_ID;
  Packet->ByteCount = 0;
  Packet->Checksum = 0;

  return KdSendPacket ( DebugPort, 0, Packet, 0, NULL );
}


EFI_STATUS
  KdSendPacketStateChange64 (
  IN EFI_DEBUGPORT_PROTOCOL *DebugPort,
  IN UINT8 *Buffer
  )
{
  KD_PACKET *Packet;
  UINTN SecondaryDataLen;
  UINT8 *SecondaryData;

  Packet = (KD_PACKET *)Buffer;
  SecondaryData = &Buffer[sizeof(KD_PACKET)];
  SecondaryDataLen = sizeof(StateChange64_Start);

  Packet->PacketLeader = PACKET_LEADER_DATA;
  Packet->PacketType = PACKET_TYPE_KD_STATE_CHANGE64;
  Packet->PacketId = INITIAL_PACKET_ID;
  Packet->ByteCount = 0;
  Packet->Checksum = 0;


  CopyMem (SecondaryData, StateChange64_Start, sizeof(StateChange64_Start));

  return KdSendPacket ( DebugPort, 0, Packet, SecondaryDataLen, SecondaryData );
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DebugPortTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  EFI_DEBUGPORT_PROTOCOL *DebugPortProtocol = NULL;

  //UINT8 *Data;
  UINTN Count;

  UINT32 BufferSize;
  UINT8 Buffer[] = { "This is a test of the emergency broadcast system, if this were an actual test...\0" };

  
  /* Locate DebugPort Protocol */
  Status = gBS->LocateProtocol(
                  &gEfiDebugPortProtocolGuid,
                  NULL,
                  (VOID**) &DebugPortProtocol );
  
  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: DebugPortTest - Failed to locate DebugPort Protocol! Error Code: 0x%08X\r\n", Status);
    return Status;
  }
  AsciiPrint("DebugPortTest - DebugPort Protocol located!\n");
  
  /* Reset DebugPort */
  Status = DebugPortProtocol->Reset( DebugPortProtocol );

  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: DebugPortTest - Failed to reset the DebugPort! Error Code: 0x%08X\r\n", Status);
    return Status;
  }
  AsciiPrint("DebugPortTest - Successfully reset DebugPort!\n");
  

  /* Write */

  BufferSize = sizeof(Buffer);
  Status = DebugPortProtocol->Write ( DebugPortProtocol, 0, &BufferSize, Buffer );

  if (Status != EFI_SUCCESS) 
  {
    AsciiPrint("ERROR: DebugPortTest - Failed to Write DebugPort Protocol! Error Code: 0x%08X\r\n", Status);
    return Status;
  }
  AsciiPrint("DebugPortTest - DebugPort Write success!\n");


  /* DebugPort Poll & Read */
  AsciiPrint("DebugPortTest - Polling for input... [ESC] to exit\n");

  Count = 0;
  for (;;) {

    Status = DebugPortProtocol->Poll( DebugPortProtocol );

    switch(Status) {
    case EFI_SUCCESS:      // At least one byte of data is available to be read.

      BufferSize = 1;
      Status = DebugPortProtocol->Read ( DebugPortProtocol, 0, &BufferSize, Buffer );

      if (Status != EFI_SUCCESS) 
      {
        AsciiPrint("ERROR: DebugPortTest - Read failed! Error Code: 0x%08X\r\n", Status);
      }
      else 
      {
        Count++;
        AsciiPrint(" %02X", Buffer[0]);
        if ((Count % 16) == 0) { Count = 0; AsciiPrint("\n"); }
        if (Buffer[0] == 0x1B) {
          AsciiPrint("\r\n");
          goto Done;
        }
      }
      break;

    case EFI_DEVICE_ERROR: // The DebugPort device is not functioning correctly.
      AsciiPrint("ERROR: DebugPortTest - Poll() - The DebugPort device is not functioning correctly.\r\n");
      return Status;
    }
  }

Done:

{
  KD_RECV_CODE PacketStatus;
  UINT8 *KdPacketBuffer;
  KD_PACKET *Packet;
  UINT8 *SecondaryData;
  volatile UINT8 DummyVar = 0; /* For KlockWork */
//  UINTN SecondaryDataLen;

  Status = DebugPortProtocol->Reset( DebugPortProtocol ); /* Flush Port */
  AsciiPrint("\r\nDebugPortTest - Connect WinDbg @ 115200 8N1\n");

  // allocate our primary packet buffer
  KdPacketBuffer = AllocatePool (KD_PACKET_MAX_SIZE);
  ASSERT (KdPacketBuffer != NULL);

  while (1)
  {
    Packet = (KD_PACKET *)KdPacketBuffer;
    SecondaryData = &KdPacketBuffer[sizeof(KD_PACKET)];

    PacketStatus = KdReceivePacket ( DebugPortProtocol, 100000, &Packet, &SecondaryData ); 

    if (PacketStatus != KD_RECV_CODE_OK)
      continue;

    // respond with reset packet
    if (Packet->PacketLeader == PACKET_LEADER_CONTROL && Packet->PacketType == PACKET_TYPE_KD_RESET) {
      KdSendPacketReset ( DebugPortProtocol, KdPacketBuffer );
      KdSendPacketStateChange64 ( DebugPortProtocol, KdPacketBuffer );
    }

    if (Packet->PacketLeader == PACKET_LEADER_DATA && Packet->PacketType == PACKET_TYPE_KD_STATE_CHANGE64) {
      KdSendPacketAck ( DebugPortProtocol, KdPacketBuffer );
    }

    if (Packet->PacketLeader == PACKET_LEADER_CONTROL && Packet->PacketType == PACKET_TYPE_KD_ACKNOWLEDGE) {
    }

    if (DummyVar == 1) /* For KlockWork */
      break;
  }

#if 0    
      Count++;

      if (Count == 1) 
      {
        DbgIoPrintString ( DebugPortProtocol, KdPacketBuffer, INITIAL_PACKET_ID | SYNC_PACKET_ID, \
                           0, 0, DEBUG_IO_ID_BOOTDEBUG_INIT, \
                           DebugIoMsg_BootDebugInitialized, sizeof(DebugIoMsg_BootDebugInitialized) );
      }
      else if (Count == 2) 
      {
        DbgIoPrintString ( DebugPortProtocol, KdPacketBuffer, INITIAL_PACKET_ID, \
                           0x6f, 0x03, 0x00000000, \
                           DebugIoMsg_SacdrvFail, sizeof(DebugIoMsg_SacdrvFail) );
      }
      else if (Count == 3) 
      {
        DbgIoPrintString ( DebugPortProtocol, KdPacketBuffer, INITIAL_PACKET_ID, \
                           0x6f, 0x03, 0x01010101, \
                           DebugIoMsg_RefreshingKdConnection, sizeof(DebugIoMsg_RefreshingKdConnection) );
      }
}
#endif
}
  AsciiPrint("DebugPortTest Done!\r\n");

  return EFI_SUCCESS;
}


