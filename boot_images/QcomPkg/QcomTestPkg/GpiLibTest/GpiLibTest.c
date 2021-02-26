/**
 * @file  GpiLibTest.c
 * @brief Implements the GPI Test code 
 */
/*
===============================================================================

                               Edit History

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
04/11/17   ts      Updated with working TRE
01/31/17   ts      File created.

===============================================================================
              Copyright (c) 2017 QUALCOMM Technologies, Inc.  
                         All Rights Reserved.
            Qualcomm Technologies Proprietary and Confidential.
===============================================================================
*/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/CacheMaintenanceLib.h>
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include "DALReg.h"
#include <gpi.h>
#include <string.h>

#define QUPV3_1_SE2_UART_LOOPBACK_CFG 0x00A8822C
#define QUPV3_0_SE6_UART_LOOPBACK_CFG 0x0089822C

#define UART_LOOPBACK_CONFIG_ADDR     QUPV3_1_SE2_UART_LOOPBACK_CFG

uint32 src_buf[2] = { 0xABCDEF89, 0xABCDEF89 };
uint32 dst_buf[2] = { 0x01234567, 0x01234567 };

gpi_iface_params gpii_params;
gpi_tre_req out_tre_req;
gpi_tre_req in_tre_req;

gpi_ring_elem out_transf_ring_elem[MAX_NUM_GPI_RING_ELEM]__attribute__((aligned(MAX_NUM_GPI_RING_ELEM * sizeof(gpi_ring_elem))));
gpi_ring_elem in_transf_ring_elem[MAX_NUM_GPI_RING_ELEM]__attribute__((aligned(MAX_NUM_GPI_RING_ELEM * sizeof(gpi_ring_elem))));

#define NUM_CMDS      8

typedef enum
{
   GPII_REG = 0,
   OUTBOUND_RING_ALLOC = 1,
   EVT_RING_ALLOC = 2,
   INBOUND_RING_ALLOC = 3,
   OUTBOUND_RING_START = 4,
   INBOUND_RING_START = 5,
   UART_IN_XFER = 6,
   UART_OUT_XFER = 7
}TEST_EVT_TYPE;

typedef enum
{
   N_A = 0,
   SUCCESS = 1,
   FAIL = 2
}CMD_STATUS;

// Test State
typedef enum
{
   INITIAL = 0,
   REG_TEST = 1,
   OUTBOUND_RING_ALLOC_TEST = 2,
   EVT_RING_ALLOC_TEST = 3,
   INBOUND_RING_ALLOC_TEST = 4,
   OUTBOUND_RING_START_TEST = 5,
   INBOUND_RING_START_TEST = 6,
   UART_IN_XFER_TEST = 7,
   UART_OUT_XFER_TEST = 8
}TEST_STATE;

TEST_STATE test = INITIAL;

// Results
gpi_result_type test_result[NUM_CMDS];

// User data
TEST_EVT_TYPE user_data[NUM_CMDS] = 
{
   GPII_REG, 
   OUTBOUND_RING_ALLOC, 
   EVT_RING_ALLOC, 
   INBOUND_RING_ALLOC, 
   OUTBOUND_RING_START, 
   INBOUND_RING_START, 
   UART_IN_XFER, 
   UART_OUT_XFER 
};

// Errors
CMD_STATUS cmd_status[NUM_CMDS] = { N_A, N_A, N_A, N_A, N_A, N_A, N_A, N_A };

volatile BOOLEAN test_case_compl_sig = TRUE;

// Callback
void gpi_test_uart_cb(gpi_result_type *result)
{
   if (result != NULL && result->user_data != NULL)
   {
      TEST_EVT_TYPE evt = *((TEST_EVT_TYPE *)result->user_data);

      // copy result & return
      memmove(&test_result[evt], result, sizeof(gpi_result_type));
   }

   test_case_compl_sig = TRUE;
}

void gpi_test_reg_gpii(void)
{
   gpii_params.gpii_id = 2;
   gpii_params.qup_type = QUP_1;
   gpii_params.protocol = GPI_UART_PROTOCOL;
   gpii_params.chan[GPI_INBOUND_CHAN].tre_ring_size = MAX_NUM_GPI_RING_ELEM;
   gpii_params.chan[GPI_INBOUND_CHAN].ring_base_va = (ptr_type)(&in_transf_ring_elem[0]);
   gpii_params.chan[GPI_INBOUND_CHAN].ring_base_pa = (ptr_type)(&in_transf_ring_elem[0]);
   gpii_params.chan[GPI_OUTBOUND_CHAN].tre_ring_size = MAX_NUM_GPI_RING_ELEM;
   gpii_params.chan[GPI_OUTBOUND_CHAN].ring_base_va = (ptr_type)&out_transf_ring_elem[0];
   gpii_params.chan[GPI_OUTBOUND_CHAN].ring_base_pa = (ptr_type)(&out_transf_ring_elem[0]);
   gpii_params.irq_mode = TRUE;
   gpii_params.cb = gpi_test_uart_cb;

   if (GPI_STATUS_SUCCESS != gpi_iface_reg(&gpii_params))
   {
      cmd_status[GPII_REG] = FAIL;
      return;
   }
   cmd_status[GPII_REG] = SUCCESS;
}

void gpi_test_evt_ring_alloc_cmd(void)
{
   // Send cmd to allocate evt ring
   if (GPI_STATUS_SUCCESS != gpi_issue_cmd(gpii_params.gpi_handle, 
                                           GPI_EVT_RING, 
                                           GPI_CHAN_CMD_ALLOCATE, 
                                           &user_data[EVT_RING_ALLOC]))
   {
      cmd_status[EVT_RING_ALLOC] = FAIL;
      return;
   }

   cmd_status[EVT_RING_ALLOC] = SUCCESS;
}

void gpi_test_in_chan_alloc_cmd(void)
{
   // send cmd to allocate chan 0
   if (GPI_STATUS_SUCCESS != gpi_issue_cmd(gpii_params.gpi_handle, 
                                           GPI_INBOUND_CHAN, 
                                           GPI_CHAN_CMD_ALLOCATE, 
                                           &user_data[INBOUND_RING_ALLOC]))
   {
      cmd_status[INBOUND_RING_ALLOC] = FAIL;
      return;
   }

   cmd_status[INBOUND_RING_ALLOC] = SUCCESS;
}

void gpi_test_out_chan_alloc_cmd(void)
{
   // send cmd to allocate chan 1
   if (GPI_STATUS_SUCCESS != gpi_issue_cmd(gpii_params.gpi_handle, 
                                           GPI_OUTBOUND_CHAN, 
                                           GPI_CHAN_CMD_ALLOCATE, 
                                           &user_data[OUTBOUND_RING_ALLOC]))
   {
      cmd_status[OUTBOUND_RING_ALLOC] = FAIL;
      return;
   }
   cmd_status[OUTBOUND_RING_ALLOC] = SUCCESS;
}

void gpi_test_start_in_chan_cmd(void)
{
   // send cmd to start chan 0
   if (GPI_STATUS_SUCCESS != gpi_issue_cmd(gpii_params.gpi_handle, 
                                           GPI_INBOUND_CHAN, 
                                           GPI_CHAN_CMD_START, 
                                           &user_data[INBOUND_RING_START]))
   {
      cmd_status[INBOUND_RING_START] = FAIL;
      return;
   }
   cmd_status[INBOUND_RING_START] = SUCCESS;
}

void gpi_test_start_out_chan_cmd(void)
{
   // send cmd to start chan 0
   if (GPI_STATUS_SUCCESS != gpi_issue_cmd(gpii_params.gpi_handle, 
                                           GPI_OUTBOUND_CHAN, 
                                           GPI_CHAN_CMD_START, 
                                           &user_data[OUTBOUND_RING_START]))
   {
      cmd_status[OUTBOUND_RING_START] = FAIL;
      return;
   }
   cmd_status[OUTBOUND_RING_START] = SUCCESS;
}

void gpi_test_uart_in_transf(void)
{
   // In UART_CONFIG_0_TRE
   in_transf_ring_elem[0].dword_0 = 0x01000183;
   in_transf_ring_elem[0].dword_1 = 0xE00000A;
   in_transf_ring_elem[0].dword_2 = 0x00010008;
   in_transf_ring_elem[0].ctrl = 0x00220001;

   // In UART_GO_TRE
   in_transf_ring_elem[1].dword_0 = 0x1;
   in_transf_ring_elem[1].dword_1 = 0x0;
   in_transf_ring_elem[1].dword_2 = 0x0;
   in_transf_ring_elem[1].ctrl = 0x00200001;

   // In DMA_TRE
#ifdef GPI_SDC
   // get the SDC addr
   in_transf_ring_elem[2].dword_0 = get_system_address((uint32)&dst_buf[0]); // DMA w. Buffer
   in_transf_ring_elem[2].dword_1 = 0x0;
#else
   // get the phys addr
   in_transf_ring_elem[2].dword_0 = (uint32)( ((uint64)&dst_buf[0]) & (0x00000000FFFFFFFF) );
   if (sizeof(ptr_type) > 4)
   {
     in_transf_ring_elem[2].dword_1 = (uint32)( ( ((uint64)&dst_buf[0]) & (0xFFFFFFFF00000000) ) >> 32);
   }
   else
   {
     in_transf_ring_elem[2].dword_1 = 0x0;
   }
#endif

   in_transf_ring_elem[2].dword_2 = 0x9;
   in_transf_ring_elem[2].ctrl = 0x00100000;

   //in_transf_ring_elem[2].ctrl = GPI_BUILD_TRE_CTRL(1, 0, 0, 1, 0, 0);

   WriteBackInvalidateDataCacheRange((VOID *)&in_transf_ring_elem[0], sizeof(in_transf_ring_elem));

   // Set the in tre_req
   in_tre_req.handle = gpii_params.gpi_handle;
   in_tre_req.chan = GPI_INBOUND_CHAN;
   in_tre_req.tre_list = &in_transf_ring_elem[0];
   in_tre_req.num_tre = 3;
   in_tre_req.user_data = (void *)&user_data[UART_IN_XFER];

   WriteBackInvalidateDataCacheRange((VOID *)&in_tre_req, sizeof(in_tre_req));

   // send the in TRE's
   if (GPI_STATUS_SUCCESS != gpi_process_tre(&in_tre_req))
   {
      cmd_status[UART_IN_XFER] = FAIL;
      return;
   }
   cmd_status[UART_IN_XFER] = SUCCESS;
}

void gpi_test_uart_out_transf(void)
{
   // Out UART_CONFIG_0_TRE
   out_transf_ring_elem[0].dword_0 = 0x01000183;
   out_transf_ring_elem[0].dword_1 = 0x0;
   out_transf_ring_elem[0].dword_2 = 0x00010008;
   out_transf_ring_elem[0].ctrl = 0x00220001;

   // Out UART_GO_TRE
   out_transf_ring_elem[1].dword_0 = 0x1;
   out_transf_ring_elem[1].dword_1 = 0x0;
   out_transf_ring_elem[1].dword_2 = 0x0;
   out_transf_ring_elem[1].ctrl = 0x00200001;


   // Out DMA_TRE w. Buffer
#ifdef GPI_SDC
   out_transf_ring_elem[2].dword_0 = get_system_address((uint32)&src_buf[0]);
   out_transf_ring_elem[2].dword_1 = 0x0;
#else
   // get the phys addr
   out_transf_ring_elem[2].dword_0 = (uint32)( ((uint64)&src_buf[0]) & (0x00000000FFFFFFFF) );
   if (sizeof(ptr_type) > 4)
   {
     out_transf_ring_elem[2].dword_1 = (uint32)( ( ((uint64)&src_buf[0]) & (0xFFFFFFFF00000000) ) >> 32);
   }
   else
   {
     out_transf_ring_elem[2].dword_1 = 0x0;
   }
#endif

   out_transf_ring_elem[2].dword_2 = 8;
   out_transf_ring_elem[2].ctrl = 0x00100200;

   WriteBackInvalidateDataCacheRange((VOID *)&out_transf_ring_elem[0], sizeof(out_transf_ring_elem));

   // Set the out tre_req
   out_tre_req.handle = gpii_params.gpi_handle;
   out_tre_req.chan = GPI_OUTBOUND_CHAN;
   out_tre_req.tre_list = &out_transf_ring_elem[0];
   out_tre_req.num_tre = 3;
   out_tre_req.user_data = (void *)&user_data[UART_OUT_XFER];

   WriteBackInvalidateDataCacheRange((VOID *)&out_tre_req, sizeof(out_tre_req));

   // send the out TRE's
   if (GPI_STATUS_SUCCESS != gpi_process_tre(&out_tre_req))
   {
      cmd_status[UART_OUT_XFER] = FAIL;
      return;
   }

   cmd_status[UART_OUT_XFER] = SUCCESS;
   /*
    * Invalidate the destination buffers after we completed the transfers
    * so that when APPS read the next time, the value will be read from DDR
    * to cache memory
    */
   InvalidateDataCacheRange((void *)dst_buf, sizeof (dst_buf));
}

/**
 * GPI test task handler.
 * Executes in single threaded environments
 * 
 * @param[in]    None.
 *
 * @return       None.
 */
EFI_STATUS
EFIAPI
GpiLibTest (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  GPI_RETURN_STATUS   Status = GPI_STATUS_ERROR;
  UINT32 * loopback_cfg_addr = (UINT32 *)UART_LOOPBACK_CONFIG_ADDR;

  /* 1:0 - LOOPBACK_MODE
   *  Loopback mode for IO MACRO,
   *  ____________________________________________________
   *  | 0 | (default) Normal Mode - no loopback          |
   *  |___|______________________________________________|
   *  | 1 | Loopback between RX & TX lines               |
   *  |___|______________________________________________|
   *  | 2 | Loopback between CTS & RFR lines             |
   *  |___|______________________________________________|
   *  | 3 | Loopback between RX & TX and CTS & RFR lines |
   *  |___|______________________________________________|
   */
  *loopback_cfg_addr = 0x3;

  TEST_START("GpiLibTest");

  while(test <= UART_OUT_XFER_TEST)
  {
    if (test_case_compl_sig == FALSE)
    {
      Status = gpi_iface_poll(gpii_params.gpi_handle);
      // Wait for 5 ms
      gBS->Stall(5000);
      continue;
    }

    test_case_compl_sig = FALSE;

    test = (TEST_STATE)((uint32)test + 1);

    if (test > UART_OUT_XFER_TEST)
    {
      break;
    }

    /* Process next test */
    switch (test)
    {
      case REG_TEST:
        gpi_test_reg_gpii();
        test_case_compl_sig = TRUE;
        break;
      case OUTBOUND_RING_ALLOC_TEST:
        gpi_test_out_chan_alloc_cmd();
        break;
      case EVT_RING_ALLOC_TEST:
        gpi_test_evt_ring_alloc_cmd();
        break;
      case INBOUND_RING_ALLOC_TEST:
        gpi_test_in_chan_alloc_cmd();
        break;
      case OUTBOUND_RING_START_TEST:
        gpi_test_start_out_chan_cmd();
        break;
      case INBOUND_RING_START_TEST:
        gpi_test_start_in_chan_cmd();
        break;
      case UART_IN_XFER_TEST:
        /* perform in and out transfer together */
        gpi_test_uart_in_transf();
        test = (TEST_STATE)((uint32)test + 1);
        gpi_test_uart_out_transf();
        break;
      default:
        break;
    }
  }

  gpi_iface_dereg(gpii_params.gpi_handle);
  
  return Status;
}
