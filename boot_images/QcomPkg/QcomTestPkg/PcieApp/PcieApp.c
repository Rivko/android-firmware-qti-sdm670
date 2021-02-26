/** @file PcieApp.c
   
  PCIe driver test application

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
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 05/14/12   jc      xHCI endpoint test sequence.
 01/09/12   jc      Target reset, clock, power, and GPIO support.
 10/24/11   jc      Interface directly with PcieLib.
 10/06/11   jc      Added TestInterface support.
 10/04/11   jc      Initial revision of UEFI PCIe test application

=============================================================================*/

/*=========================================================================
      Include Files
==========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TestInterface.h>
#include <Library/PcieLib.h>

/*=========================================================================
      Macros
==========================================================================*/

#define PCIEAPP_TEST_NAME   "PcieApp"
#define PCIE_MODULE_NAME    "PcieApp"

/* Sequencer definitions */
#define SEQ_END 0
#define SEQ_RD  1
#define SEQ_WR  2

/*=========================================================================
      Types
==========================================================================*/

typedef struct
{
  UINT32 PktNum;
  UINT32 Op;
  UINT32 Size;
  UINT32 Addr;
  UINT32 DataOp;
  UINT32 uSec;
  UINT32 DataRead;
} PcieAppSeqType;

/*=========================================================================
      Data
==========================================================================*/

int PcieAppStagedInit = FALSE;
int PcieAppDoIOTest = FALSE;

PcieAppSeqType PcieAppSeqSetup[] =
{
  {0,SEQ_WR,4,0xFF542000,0x402054FF,0},
  {0,SEQ_WR,4,0xFF542004,0x00000000,0},
  {0,SEQ_WR,4,0xFF542008,0x10000000,0},
  {0,SEQ_WR,4,0xFF54200C,0x00000000,0},

  {0,SEQ_WR,4,0xFF542010,0x402154FF,0},
  {0,SEQ_WR,4,0xFF542014,0x00000000,0},
  {0,SEQ_WR,4,0xFF542018,0x10000000,0},
  {0,SEQ_WR,4,0xFF54201C,0x00000000,0},

  {0, SEQ_END}
};

PcieAppSeqType PcieAppSeqArray[] =
{
  {20614,SEQ_RD,4,0x0000000,0x20009600,0},
  {20618,SEQ_RD,4,0x0000004,0x20080004,4},
  {20622,SEQ_RD,4,0x0000008,0x11000000,3},
  {20626,SEQ_RD,4,0x000000C,0x00000000,2},
  {20630,SEQ_RD,4,0x0000010,0xCB424001,2},
  {20634,SEQ_RD,4,0x0000500,0x01040000,6},
  {20638,SEQ_RD,4,0x0000014,0x00080000,2},
  {20642,SEQ_RD,4,0x0000018,0x00060000,3},
  {20646,SEQ_RD,4,0x0000028,0x01000000,2},
  {20650,SEQ_RD,1,0x0000503,0x01040000,247},
  {20652,SEQ_WR,1,0x0000503,0x07900401,2},
  {20656,SEQ_RD,1,0x0000502,0x01040001,1},
  {20660,SEQ_RD,4,0x0000024,0x01000000,3},
  {20664,SEQ_RD,4,0x0000504,0x000000E0,2},
  {20666,SEQ_WR,4,0x0000504,0x00000000,1},
  {20670,SEQ_RD,4,0x0000024,0x01000000,2},
  {20674,SEQ_RD,4,0x0000024,0x01000000,2},
  {20676,SEQ_WR,4,0x0000020,0x02000000,2},
  {20680,SEQ_RD,4,0x0000020,0x02000000,1},
  {20684,SEQ_RD,4,0x0000020,0x00000000,15852},
  {20688,SEQ_RD,4,0x0000024,0x01000000,3},
  {20692,SEQ_RD,4,0x0000010,0xCB424001,5},
  {20694,SEQ_WR,4,0x0000628,0x00000000,2051},
  {20698,SEQ_RD,4,0x0000008,0x11000000,33},
  {20702,SEQ_RD,4,0x0000500,0x01040001,22},
  {20706,SEQ_RD,4,0x0000510,0x02040003,2},
  {20710,SEQ_RD,4,0x0000520,0x02000002,2},
  {20714,SEQ_RD,4,0x0000500,0x01040001,5},
  {20718,SEQ_RD,4,0x0000510,0x02040003,2},
  {20722,SEQ_RD,4,0x0000510,0x02040003,2},
  {20728,SEQ_RD,4,0x0000514,0x55534220,1},
  {20729,SEQ_RD,4,0x0000518,0x01020000,0},
  {20733,SEQ_RD,4,0x0000420,0xA0020000,4},
  {20737,SEQ_RD,4,0x0000430,0xA0020000,2},
  {20741,SEQ_RD,4,0x0000510,0x02040003,2},
  {20745,SEQ_RD,4,0x0000520,0x02000002,2},
  {20749,SEQ_RD,4,0x0000520,0x02000002,2},
  {20755,SEQ_RD,4,0x0000524,0x55534220,1},
  {20756,SEQ_RD,4,0x0000528,0x03020000,0},
  {20760,SEQ_RD,4,0x0000440,0xA0020000,3},
  {20764,SEQ_RD,4,0x0000450,0xA0020000,2},
  {20768,SEQ_RD,4,0x0000520,0x02000002,2},
  {20772,SEQ_RD,4,0x0000420,0xA0020000,2},
  {20776,SEQ_RD,4,0x0000430,0xA0020000,6},
  {20780,SEQ_RD,4,0x0000440,0xA0020000,5},
  {20784,SEQ_RD,4,0x0000450,0xA0020000,5},
  {20788,SEQ_RD,4,0x0000000,0x20009600,251},
  {20792,SEQ_RD,4,0x0000600,0x00000000,3},
  {20794,SEQ_WR,4,0x0000628,0x02000000,10},
  {20796,SEQ_WR,4,0x0000638,0x08706EFF,1},
  {20797,SEQ_WR,4,0x000063C,0x00000000,0},
  {20800,SEQ_WR,4,0x0000630,0x002054FF,0},
  {0, SEQ_END}
};

PcieAppSeqType PcieAppSeqReps[] =
{
  {20801,SEQ_WR,4,0x0000634,0x00000000,100},
  {0, SEQ_END}
};

/*=========================================================================
      Functions
==========================================================================*/

/******************************************************************************/

/*
  PcieApp = (
    caps = (
      0x10D38086,
      0x00100006,
      0x02000000,
      0x0E,
      0x10000000,
      0x20000000,
      0x30000001,
      0x40000000,
      0x0,
      0x0,
      0x0,
      0x8086,
      0x0,
      0xC8,
      0x0,
      0x0100),
    bars = (0xFFFE0000, 0xFFBF0000, 0xFFFFFFE1, 0xFFFFC000, 0x0, 0x0),
    bars_sizes = (0x00020000, 0x00410000, 0x20, 0x4000, 0x0, 0x0),
    dev_regs = (
      0x0,
      0x00100209,
      0x00080400,
      0x0,
      0x8310,
      0x2,
      0x00100000,
      0x0600,
      0x10000000,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x8100,
      0x0))
*/
#define PCI_CAPS_REGION_SIZE  0x40
#define PCI_CAPS_NUM          (PCI_CAPS_REGION_SIZE / 4)
#define PCI_CAPS_BARS_NUM     6
#define PCI_CAPS_BARS_OFFSET  0x10
#define PCI_CAPS_BARS_INDEX   (PCI_CAPS_BARS_OFFSET / 4)
#define PCI_CAPS_ID_INDEX     0
#define I82574_ID_VALUE       0x10D38086
#define I82574_CSR_NUM        (0x40 / 4)
#define I82574_CSR_VET_OFFSET 0x38  /* has known reset value 0x00008100 */
#define I82574_CSR_VET_VALUE  0x00008100
struct PcieApp_regs_struct
{
  UINT32 caps[PCI_CAPS_NUM];
  UINT32 bars[6];
  UINT32 bars_sizes[6];
  UINT32 dev_regs[I82574_CSR_NUM];
} PcieApp;
EFI_STATUS PcieApp_IOTest(void)
{
  EFI_STATUS status;
  UINT32 n;
  UINT32 value;
  UINT32 *pAddr;

  AsciiPrint(": reading device caps\n");
  status = PcieLibGetConfigBaseAddr(0, 0, 0, &pAddr);
  if (status != EFI_SUCCESS)
  {
    AsciiPrint(": return\n");
    return status;
  }
  n = 0;
  PcieApp.caps[n] = MmioRead32((UINTN)&pAddr[n]);
  if (PcieApp.caps[n] == 0xFFFFFFFFUL || PcieApp.caps[n] == 0)
  {
    AsciiPrint(": PCIe endpoint register read failure\n");
    status = EFI_DEVICE_ERROR;
    AsciiPrint(": return\n");
    return status;
  }
  for (n++ ; n < PCI_CAPS_NUM; n++)
  {
    PcieApp.caps[n] = MmioRead32((UINTN)&pAddr[n]);
  }

  AsciiPrint(": determining device BAR sizes\n");
  pAddr = &pAddr[PCI_CAPS_BARS_OFFSET / 4];
  for (n = 0; n < PCI_CAPS_BARS_NUM; n++)
  {
    MmioWrite32((UINTN)&pAddr[n], 0xFFFFFFFFUL);
    value = MmioRead32((UINTN)&pAddr[n]);
    PcieApp.bars[n] = value;
    if ((value & 1) == 0)
    {
      value &= ~0xf;  /* Memory space */
    }
    else
    {
      value &= ~0x3;  /* I/O space */
    }
    value = ~value + 1;
    PcieApp.bars_sizes[n] = value;
    MmioWrite32((UINTN)&pAddr[n], PcieApp.caps[PCI_CAPS_BARS_INDEX + n]);
  }

  AsciiPrint(": PCIe endpoint device ID: 0x%X\n", PcieApp.caps[PCI_CAPS_ID_INDEX]);

  AsciiPrint(": reading device regs\n");
  status = PcieLibGetIoBaseAddr(0, 0, 0, 0, &pAddr);
  if (status != EFI_SUCCESS)
  {
    AsciiPrint(": return\n");
    return status;
  }
  for (n = 0; n < I82574_CSR_NUM; n++)
  {
    PcieApp.dev_regs[n] = MmioRead32((UINTN)&pAddr[n]);
  }

  if (PcieApp.caps[PCI_CAPS_ID_INDEX] == I82574_ID_VALUE)
  {
    PcieApp.dev_regs[I82574_CSR_VET_OFFSET / 4] =
      MmioRead32((UINTN)&pAddr[I82574_CSR_VET_OFFSET / 4]);
    if (PcieApp.dev_regs[I82574_CSR_VET_OFFSET / 4] != I82574_CSR_VET_VALUE)
    {
      AsciiPrint(": I82574 register read failure\n");
      status = EFI_DEVICE_ERROR;
      AsciiPrint(": return\n");
      return status;
    }
  }

  AsciiPrint(": return %d\n", status);
  return status;
}

#define SFAB_M2VMT_REG_BASE   0x01300000
#define HWIO_SFAB_M2VMT_M2VMRv_6_ADDR(v)               (SFAB_M2VMT_REG_BASE      + 0x0000a000 + 0x4 * (v))
#define HWIO_SFAB_M2VMT_M2VMRv_6_MAXv                           3
#define PCIE_M2VMT  pcie_m2vmt
volatile UINT32 pcie_m2vmt = 2;

int pcie_seq_count = 0;
int pcie_seq_fail = -1;

EFI_STATUS PcieApp_Seq
(
  PcieAppSeqType *pSeqSetup,
  PcieAppSeqType *pSeqOp,
  PcieAppSeqType *pSeqRep,
  UINT32 reps
)
{
  EFI_STATUS status;
  UINT32 value;
  UINT32 BaseAddr;
  UINT32 ConfigAddr;
  PcieAppSeqType *pSeq;
  UINT32 val = 0;
  UINT32 endcount;
  UINT32 n;

  {
    UINT32 *pConfigAddr;
    status = PcieLibGetConfigBaseAddr(0, 0, 0, &pConfigAddr);
    ConfigAddr = (UINT32)pConfigAddr;
  }
  if (status != EFI_SUCCESS)
  {
    AsciiPrint(": return %d\n", status);
    return status;
  }

  endcount = 2;
  if (pSeqRep != NULL && reps > 0)
  {
    endcount += reps;
  }

  for (n = 0; n < endcount; n++)
  {
    if (n == 0)
    {
      AsciiPrint(": running setup sequence\n");
      pSeq = pSeqSetup;
      BaseAddr = 0;
    }
    else if (n == 1)
    {
      AsciiPrint(": running operation sequence\n");
      pSeq = pSeqOp;
      {
        UINT32 *pBase;
        status = PcieLibGetIoBaseAddr(0, 0, 0, 0, &pBase);
        BaseAddr = (UINT32)pBase;
      }
      if (status != EFI_SUCCESS)
      {
        AsciiPrint(": return %d\n", status);
        return status;
      }
    }
    else if (pSeqRep != NULL)
    {
      if (n == 2)
      {
        AsciiPrint(": running repetition sequence\n");
      }
      pSeq = pSeqRep;
    }
    else
    {
      break;
    }

    for ( ; pSeq->Op != SEQ_END; pSeq++)
    {
      if (pSeq->uSec > 0)
      {
        gBS->Stall(pSeq->uSec);
      }

      if (pSeq->Op == SEQ_RD)
      {
        if (pSeq->Size == 4)
        {
          pSeq->DataRead = MmioRead32(BaseAddr + pSeq->Addr);
        }
        else if (pSeq->Size == 1)
        {
          pSeq->DataRead = MmioRead8(BaseAddr + pSeq->Addr);
        }
      }
      else
      {
        value = ((pSeq->DataOp >> 24) & 0x000000FF) |
                ((pSeq->DataOp >>  8) & 0x0000FF00) |
                ((pSeq->DataOp <<  8) & 0x00FF0000) |
                ((pSeq->DataOp << 24) & 0xFF000000);
        if (pSeq->Size == 4)
        {
          MmioWrite32(BaseAddr + pSeq->Addr, value);
        }
        else if (pSeq->Size == 1)
        {
          value = (value >> ((pSeq->Addr & 3) * 8)) & 0xFF;
          MmioWrite8(BaseAddr + pSeq->Addr, (UINT8)value);
        }
      }

      if (pSeq == pSeqRep)
      {
        gBS->Stall(100);
        val = MmioRead32(ConfigAddr+4);
        if (val & 0x10000000)
        {
          AsciiPrint("PCIE Target Abort Bit set: %d\n", pcie_seq_count);
          if (pcie_seq_fail < 0)
          {
            pcie_seq_fail = pcie_seq_count;
          }
          /* Stop the sequencer */
          n = 0x7FFFFFF0;
          status = EFI_DEVICE_ERROR;
          break;
        }
      }

      pcie_seq_count++;
    }

  } /* n */

  AsciiPrint(": return %d\n", status);
  return status;
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
PcieTestMain
(
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 uSec;
#ifdef PCIEAPP_MORE
  UINT32 *BaseAddr;
#endif

  TEST_START(PCIEAPP_TEST_NAME);

  AsciiPrint("PcieTestMain: entry\n");

  if (PcieAppStagedInit)
  {
    Status = PcieLibEnablePower();
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: PcieTestMain: enable power failure\n");
      TestStatus(PCIEAPP_TEST_NAME, Status); 
      TEST_STOP(PCIEAPP_TEST_NAME);
      return Status;
    }

    Status = PcieLibGetSettlingTimeUsec(&uSec);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: PcieTestMain: get settling time failure\n");
      TestStatus(PCIEAPP_TEST_NAME, Status); 
      TEST_STOP(PCIEAPP_TEST_NAME);
      return Status;
    }
    gBS->Stall(uSec);
  }

  /* Start the PCIe controller */
  Status = PcieLibInit();
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: PcieTestMain: init failure\n");
    TestStatus(PCIEAPP_TEST_NAME, Status); 
    TEST_STOP(PCIEAPP_TEST_NAME);
    return Status;
  }

  if (PcieAppDoIOTest)
  {
    Status = PcieApp_IOTest();
  }
  else
  {
    Status = PcieApp_Seq(PcieAppSeqSetup, PcieAppSeqArray, PcieAppSeqReps, 10);
  }
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: PcieTestMain: test sequence failure\n");
    TestStatus(PCIEAPP_TEST_NAME, Status); 
    TEST_STOP(PCIEAPP_TEST_NAME);
    return Status;
  }

#ifdef PCIEAPP_MORE
  /* Get the I/O base address for the first function of the first PCIe device */
  Status = PcieApi->GetIoBaseAddr(0, 0, &BaseAddr);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: PcieTestMain: BAR failure\n");
    TestStatus(PCIEAPP_TEST_NAME, Status); 
    TEST_STOP(PCIEAPP_TEST_NAME);
    return Status;
  }

  /* Stop the PCIe controller */
  Status = PcieApi->Stop(PcieDevice);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: PcieTestMain: stop failure\n");
    TestStatus(PCIEAPP_TEST_NAME, Status); 
    TEST_STOP(PCIEAPP_TEST_NAME);
    return Status;
  }
#endif

  AsciiPrint("PcieTestMain: all tests passed\n");
  TestStatus(PCIEAPP_TEST_NAME, EFI_SUCCESS); 
  TEST_STOP(PCIEAPP_TEST_NAME);
  return EFI_SUCCESS;
}
