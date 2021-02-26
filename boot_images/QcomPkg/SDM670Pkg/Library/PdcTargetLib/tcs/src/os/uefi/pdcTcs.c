/*============================================================================
  FILE:         pdcTcs.c
  
  OVERVIEW:     This file implements the functions for initial programming of the TCS
                registers
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PdcTargetLib/tcs/src/os/uefi/pdcTcs.c#1 $
$DateTime: 2017/09/18 09:26:13 $
============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "cmd_db.h"
#include "pdc_hwio.h"
#include "pdcHal_hwio.h"
#include "pdcTcs.h"
#include "pdcTcsCfg.h"
#include "pdcLog.h"

/*===========================================================================
 *                        EXTERNAL GLOBAL VARIABLES
 *===========================================================================*/
extern pdc_tcs_config g_pdcTCSConfig[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS];
extern pdc_tcs_resource g_pdcResourceList[TCS_TOTAL_RESOURCE_NUM];

/*===========================================================================
 *                            INTERNAL FUNCTIONS
 *===========================================================================*/
/**
 * pdcTcs_writeCmd
 *
 * @brief Internal function helper to write the command/data pairs
 *
 * @param tcsNum:   TCS number to write commands to
 * @param cmdNum:   Command index
 * @param resAddr:  Command address
 * @param data:     Command data
 */
static void pdcTcs_writeCmd(uint16         tcsNum,
                            uint16         cmdNum,
                            pdc_tcs_config *config)
{
  uint32 cmdWrite;
  uint32 cmdWait;
  uint32 cmdResp;
  uint32 address;
  uint32 tmp;
  
  cmdWrite  = (0 == (config->data.options & TCS_CFG_OPT_READ)) ? 1 : 0;
  cmdWait   = (0 != (config->data.options & TCS_CFG_OPT_CMD_WAIT)) ? 1 : 0;
  cmdResp   = (0 != (config->data.options & TCS_CFG_OPT_CMD_RESP_REQ)) ? 1 : 0;

  /* Write configuration */
  PDC_HWIO_OUTI2(TCSt_CMDn_MSGID, tcsNum, cmdNum,
                 PDC_HWIO_FVAL(TCSt_CMDn_MSGID, READ_OR_WRITE,  cmdWrite)  |
                 PDC_HWIO_FVAL(TCSt_CMDn_MSGID, RES_REQ,        cmdResp));
  
  /* Set the wait for completion bit, register default is no-wait, so no need to clear */
  if(1 == cmdWait)
  {
    tmp = PDC_HWIO_INI(TCSt_CMD_WAIT_FOR_CMPL_BANK, tcsNum);
    tmp |= (0x01 << cmdNum);
    PDC_HWIO_OUTI(TCSt_CMD_WAIT_FOR_CMPL_BANK, tcsNum, tmp);
  }

  /* Update main data structure with base address */
  config->cmd.resourceP = &g_pdcResourceList[config->cmd.index];

  /* Compute real address for command */
  address = config->cmd.resourceP->base_addr + config->data.addr_offset;

  /* Write command/data pair */
  PDC_HWIO_OUTI2(TCSt_CMDn_ADDR, tcsNum, cmdNum, address);
  PDC_HWIO_OUTI2(TCSt_CMDn_DATA, tcsNum, cmdNum, config->data.res_val);

  return;
}

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * pdcTcs_initialize
 *
 * @brief Initializes the PDC TCS registers
 * 
 * @note called from RCINIT
 */
void pdcTcs_initialize(void)
{
  uint32                nTCS;
  uint32                nRes;
  uint32                nCMD;
  pdc_tcs_config        *cmdData;
  uint32                nullCnt                   = 0;
  uint32                enableMask[TCS_NUM_TOTAL] = {0};
  uint32                base_addr;

  /* Get all the command addresses for each resource */
  for(nRes = 0; nRes < TCS_TOTAL_RESOURCE_NUM; nRes++)
  {
    base_addr = cmd_db_query_addr(g_pdcResourceList[nRes].name);

    pdcLog_printf(2, "TCS Resource lookup: (Name: %s) (Base address: 0x%x)", 
                  g_pdcResourceList[nRes].name,
                  g_pdcResourceList[nRes].base_addr);

    /* Verify we get a valid address for each resource */
    if(base_addr == 0)
    {
      nullCnt++;
    }
    else
    {
      /* Found entry in cmd_db. Some resources need offset on the top of 
       * cmd_db addr like vrm.soc so use += instead of simple assignment */
      g_pdcResourceList[nRes].base_addr += base_addr;
    }
  }
  
  if(0 != nullCnt)
  {
    pdcLog_printf(0, "WARNING: Bad TCS config; unable to retreive one or more resrouce values");
    return;
  }

  /* Verify code config matches hardware - SW is DRV0 */
  CORE_VERIFY(NUM_COMMANDS_PER_TCS == PDC_HWIO_INF1(PDC_PARAM_RESOURCE_DRVd, 0, BLK_NUM_TCS_CMDS));
  CORE_VERIFY(TCS_NUM_TOTAL <= PDC_HWIO_INF1(PDC_PARAM_RESOURCE_DRVd, 0, BLK_NUM_TCS));

  /* Write each TCS with the command/data sets */
  for(nTCS = 0; nTCS < TCS_NUM_TOTAL; nTCS++)
  {
    for(nCMD = 0; nCMD < NUM_COMMANDS_PER_TCS; nCMD++)
    {
      cmdData = &g_pdcTCSConfig[nTCS][nCMD];

      /* Check if command is used */
      if(0 == (cmdData->data.options & TCS_CFG_OPT_NOT_USED))
      {
        /* Command is used, but not currently enabled */
        if(0 == (cmdData->data.options & TCS_CFG_OPT_NO_ENABLE))
        {
          enableMask[nTCS] |= (0x01 << nCMD);
        }

        pdcTcs_writeCmd(nTCS, nCMD, cmdData);
      }
    }
  }

  /* Enable TCS commands */
  for(nTCS = 0; nTCS < TCS_NUM_TOTAL; nTCS++)
  {
    PDC_HWIO_OUTI(TCSt_CMD_ENABLE_BANK, nTCS, enableMask[nTCS]);
  }

  pdcLog_printf(0, "TCS registers programmed successfully");
  return;
}

/*
 * pdcTcs_getResources
 */
const pdc_tcs_config* pdcTcs_getResources(tcs_usage tcsNum)
{
  CORE_VERIFY(tcsNum < TCS_NUM_TOTAL);
  return g_pdcTCSConfig[tcsNum];
}

/*
 * pdcTcs_getAllResourceList
 */
const pdc_tcs_resource* pdcTcs_getAllResourceList(void)
{
  return g_pdcResourceList;
}

