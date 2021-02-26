/**
 * @file DwcPlatform.c
 *
 * @ingroup CommonLayer
 *
 * @brief Device mode platform specific implementation.
 *
 * This file implements utility functions that are platform specific to allow
 * the rest of the common layer to be unchanged between the UEFI and Windows
 * driver implementations.
 *
 * @copyright Copyright (C) 2013-2014 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*==============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/18/14   ck      Address software & hardware transfer cancel notification issue
 06/12/14   ck      Address pending setup pkt and End Transfer issues
 08/23/13   mliss   Initial import
 =============================================================================*/

#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>

#include "UsbfnDwc3Util.h"
#include "DwcPlatform.h"
#include "DwcCommon.h"


#define DWC_LOG_STRING_SIZE     DWC_LOG_BUF_DUMP_LENGTH * 2 + 1
#define DWC_BIT_IS_SET(Data, Bit) ((BOOLEAN)(((Data) & (Bit)) == (Bit)))
#define DWC_REG_BIT_IS_SET(Address, Bit) \
          (DWC_BIT_IS_SET(DWC_PLAT_READ_REG(Address), (Bit)))


/*
 * See header for documentation.
 */
DWC_STATUS
DwcWaitRegBit (
  IN UINT32                 Address,
  IN UINT32                 Bit,
  IN BOOLEAN                WaitToSet,
  IN UINT32                 Timeout
  )
{
  UINT32 i;

  for (i = 0; i < (Timeout / DWC_POLL_WAIT_INTERVAL) + 1; ++i) {
    if (DWC_REG_BIT_IS_SET(Address, Bit) == WaitToSet) {
      return DWC_SUCCESS;
    }
    DWC_PLAT_DELAY(DWC_POLL_WAIT_INTERVAL);
  }

  return DWC_TIMEOUT;
}


/**
 * @brief Dump binary data to a hex string.
 *
 * Dump binary buffer Buf into Str in hex format.  The Str parameter must be
 * able to hold at least Len * 2 + 1 characters (2 per byte plus null char).
 *
 * @param  Str              String to dump into
 * @param  Buf              Binary buffer to dump
 * @param  Len              Number of bytes to dump
 *
 * @return Number of characters written to Str
 */
STATIC UINTN
DwcDumpBuffer (
  IN CHAR8                  *Str,
  IN VOID                   *Buf,
  IN UINT32                 Len
  )
{
  CHAR8 *Data = (CHAR8 *)Buf;
  UINTN Count = 0;

  if ((NULL == Str) || (NULL == Buf) || (Len == 0)) {
    return 0;
  }
  while (Len > 0) {
    AsciiSPrint(Str, 3, "%02x", *Data);
    Str   += 2;
    Count += 2;
    Data++;
    Len--;
  }
  Str = (CHAR8*)'\0';

  return Count;
}


/**
 * @brief Dump an Event Buffer
 *
 * Dumps the event buffer structure members for debugging purposes. Does
 * not dump the contents of the event buffer itself.
 *
 * @param [in] Dev          DWC Device instance
 * @param [in] EvtBufIdx    Index of event buffer to dump
 */
STATIC VOID
DwcDumpEventBuffer (
  IN DWC_DEVICE             *Dev,
  IN UINT8                  EvtBufIdx
  )
{
  DWC_EVENT_BUFFER  *EvtBuf;

  if (NULL == Dev) {
    return;
  }

  EvtBuf = &Dev->EvtBuffer[EvtBufIdx];

  DebugPrint( EFI_D_INFO,
    "    Index %d:\n"
    "      Buf     = 0x%08llx\n"
    "      NextEvt = 0x%08llx\n"
    "      Size    = %d\n",
    (UINTN)EvtBuf->Buf,
    (UINTN)EvtBuf->NextEvt,
    EvtBuf->Size
  );
}


/**
 * @brief Dump a TRB Ring
 *
 * Dumps the TRB Ring structure members as well as a hex dump of the TRB
 * Ring buffer itself.
 *
 * @param [in] Ring         TRB Ring to dump
 */
STATIC VOID
DwcDumpRing (
  IN DWC_TRB_RING           *Ring
  )
{
  CHAR8   HexDump[DWC_LOG_STRING_SIZE];
  UINTN   DumpLen;

  if (NULL == Ring) {
    return;
  }
  DumpLen = DWC_LOG_BUF_DUMP_LENGTH < Ring->BufSize ? DWC_LOG_BUF_DUMP_LENGTH : Ring->BufSize;
  DwcDumpBuffer(HexDump, Ring->Buf, DumpLen);

  DebugPrint( EFI_D_INFO,
    "        Buf     = 0x%08x -> 0x",
    Ring->Buf
  );
  DebugPrint( EFI_D_INFO, "%a\n", HexDump);

  DebugPrint( EFI_D_INFO,
    "        BufSize = %d\n"
    "        Enq     = 0x%08x\n"
    "        Deq     = 0x%08x\n"
    "        NumTRBs = %d\n",
    Ring->BufSize,
    Ring->Enq,
    Ring->Deq,
    Ring->NumTRBs
  );
}


/*
 * See header for documentation.
 */
VOID
DwcDumpTrb (
  IN DWC_TRB                *Trb,
  IN UINT32                 TotalLen
  )
{
  // For ArmCompiler6,
  // array initialization will generate
  // an instruction, "bl memset" in the assembly. However, the
  // linker is not able to resolve it.
  // Temp. workaround the issue by using SetMem
  VOID          *DataBuf = NULL;
  // CHAR8         DataHexDump[DWC_LOG_STRING_SIZE] = {'\0'};
  CHAR8         DataHexDump[DWC_LOG_STRING_SIZE];
  UINT32        DataLen;

  if (NULL == Trb) {
    return;
  }

  // For ArmCompiler6,
  // DataHexDump[DWC_LOG_STRING_SIZE] = {'\0'} will generate
  // an instruction, "bl memset" in the assembly. However, the
  // linker is not able to resolve it.
  SetMem(DataHexDump, DWC_LOG_STRING_SIZE, '\0');

  //Dump start of data buffer
  DataBuf = (VOID *) FORM_UINTN_FROM_2_UINT32(Trb->BufPtrHi, Trb->BufPtrLo);
  DataLen = DWC_LOG_BUF_DUMP_LENGTH < TotalLen ? DWC_LOG_BUF_DUMP_LENGTH : TotalLen;


  DwcDumpBuffer(DataHexDump, DataBuf, DataLen);

  DebugPrint( EFI_D_INFO,
    "          BufPtrLo  = 0x%08x\n"
    "          BufPtrHi  = 0x%08x\n"
    "          BufSize   = %d\n"
    "            BufDump = 0x",
    Trb->BufPtrLo,
    Trb->BufPtrHi,
    Trb->BufSize
  );
  DebugPrint( EFI_D_INFO, "%a\n", DataHexDump);
  DebugPrint( EFI_D_INFO,
    "          PCM1      = %d\n"
    "          TRBSTS    = 0x%x\n"
    "          HWO       = %d\n"
    "          LST       = %d\n"
    "          CHN       = %d\n"
    "          CSP       = %d\n"
    "          TRBCTL    = %d\n"
    "          ISP_IMI   = %d\n"
    "          IOC       = %d\n"
    "          SID_SOFN  = %d\n",
    Trb->PCM1,
    Trb->TRBSTS,
    Trb->HWO,
    Trb->LST,
    Trb->CHN,
    Trb->CSP,
    Trb->TRBCTL,
    Trb->ISP_IMI,
    Trb->IOC,
    Trb->SID_SOFN
  );
}


/**
 * @brief Dump an URB
 *
 * Dump an URB structure members and the first TRB in the transfer
 * descriptor, if any.
 *
 * @param [in] Urb          URB to dump
 */
STATIC VOID
DwcDumpUrb (
  IN DWC_URB                *Urb
  )
{
  if (NULL == Urb) {
    return;
  }

  DebugPrint( EFI_D_INFO,
    "        Ep               = %d\n"
    "        EpDir            = %d\n"
    "        TrbCtl           = %d\n"
    "        StreamID         = %d\n"
    "        TotalTransactLen = %d\n"
    "        UseZlt           = %a\n"
    "        TotalBytesXfered = %d\n",
    Urb->Ep,
    Urb->EpDir,
    Urb->TrbCtl,
    Urb->StreamID,
    Urb->TotalTransactLen,
    Urb->UseZlt ? "True" : "False",
    Urb->TotalBytesXfered
  );
  DebugPrint( EFI_D_INFO,
    "        NumTRBs          = %d\n"
    "        SGLen            = %d\n"
    "        SGBytesXfered    = %d\n"
    "        XferTRBSTS       = %d\n"
    "        SGList           = 0x%08x\n"
    "        XferDesc         = 0x%08x -> {\n",
    Urb->NumTRBs,
    Urb->SGLen,
    Urb->SGBytesXfered,
    Urb->XferTRBSTS,
    Urb->SGList,
    Urb->XferDesc
  );

  DwcDumpTrb(Urb->XferDesc, Urb->TotalTransactLen);

  DebugPrint( EFI_D_INFO,
    "        }\n"
  );
}


/**
 * @brief Dump an endpoint
 *
 * Dump the endpoint structure members, the endpoints TRB Ring, and the
 * in progress URB for the endpoint, if any.
 *
 * @param [in] Dev          DWC Device instance
 * @param [in] ExIdx        Index of endpoint to dump
 */
STATIC VOID
DwcDumpEndpoint (
  IN DWC_DEVICE             *Dev,
  IN UINT8                  EpIdx
  )
{
  DWC_EP  *Ep;
  CHAR8   StateStr[32];

  if (NULL == Dev) {
    return;
  }

  Ep = &Dev->Ep[EpIdx];

  // populate the state string
  switch (Ep->State) {
    case DWC_EP_STATE_UNINITIALIZED:
      AsciiSPrint(StateStr, 32, "uninitialized");
      break;
    case DWC_EP_STATE_INACTIVE:
      AsciiSPrint(StateStr, 32, "inactive");
      break;
    case DWC_EP_STATE_START_XFER:
      AsciiSPrint(StateStr, 32, "starting transfer");
      break;
    case DWC_EP_STATE_XFER_IN_PROGRESS:
      AsciiSPrint(StateStr, 32, "transfer in progress");
      break;
    case DWC_EP_STATE_STALLED:
    case DWC_EP_STATE_STALLED_XFER_PENDING:
      AsciiSPrint(StateStr, 32, "stalled");
      break;
    default:
      AsciiSPrint(StateStr, 32, "unknown endpoint state (%d)", Ep->State);
  }

  DebugPrint( EFI_D_INFO,
    "    Physical Endpoint %d:\n"
    "      State       = %a\n"
    "      ResourceID  = %d\n"
    "      EvtBufferID = %d\n"
    "      Ring        = 0x%08x -> {\n",
    EpIdx,
    StateStr,
    Ep->ResourceID,
    Ep->EvtBufferID,
    Ep->Ring
  );
  DwcDumpRing(Ep->Ring);

  DebugPrint( EFI_D_INFO,
    "      }\n"
    "      Urb         = 0x%08x -> {\n",
    Ep->Urb
  );
  DwcDumpUrb(Ep->Urb);

  DebugPrint( EFI_D_INFO,
    "      }\n"
  );
}


/*
 * See header for documentation.
 */
VOID
DwcDumpDevice (
  IN DWC_DEVICE             *Dev
  )
{
  UINTN   i;

  if ((NULL == Dev) ||
      ((PcdGet32(PcdDebugPrintErrorLevel) & EFI_D_INFO) == 0)) {
    return;
  }

  // print the start of the device structure
  DebugPrint( EFI_D_INFO,
    "DWC_DEVICE:0x%08llx = {\n"
    "  CoreBaseAddr        = 0x%08x\n"
    "  DwcUsb3DeviceNumInt = %d\n"
    "  EvtBufferCount      = %d\n"
    "  EvtBuffer           = 0x%08x -> [\n",
    (UINTN)Dev,
    Dev->CoreBaseAddr,
    Dev->DwcUsb3DeviceNumInt,
    Dev->EvtBufferCount,
    Dev->EvtBuffer
  );

  // print out the event buffers
  for (i=0; i < Dev->EvtBufferCount; i++) {
    DwcDumpEventBuffer(Dev, i);
    gBS->Stall(50000);  // 50 ms
  }
  DebugPrint( EFI_D_INFO,
    "  ]\n"
    "  Ep                  = [\n"
  );

  // print out the endpoints (stall to prevent output from getting garbled up)
  for (i=0; i < DWC_USB_MAX_ENDPOINTS; i++) {
    DwcDumpEndpoint(Dev, i);
    gBS->Stall(100000); // 100 ms
  }
  DebugPrint( EFI_D_INFO,
    "  ]\n"
    "}\n"
  );
}


/*
 * See header for documentation.
 */
VOID
DwcLogUrbComplete (
  IN DWC_URB                *Urb
  )
{
  if ((NULL == Urb) ||
      ((PcdGet32(PcdDebugPrintErrorLevel) & EFI_D_VERBOSE) == 0)) {
    return;
  }

  DebugPrint( EFI_D_VERBOSE,
    "XferComplete: Urb = 0x%08llx -> {\n",
    (UINTN)Urb
  );
  DwcDumpUrb(Urb);

  DebugPrint( EFI_D_INFO,
    "}\n"
  );
}


