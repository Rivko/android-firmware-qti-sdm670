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
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/tcs/src/os/qurt/pdcTcs.c#1 $
$DateTime: 2018/07/30 01:21:36 $
============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "cmd_db.h"
#include "pdc_hwio.h"
#include "pdcTcs.h"
#include "pdcTcsCfg.h"
#include "pdcLog.h"
#include "pwr_utils_lvl.h"

#ifdef PDC_IMAGE_SLPI
#include "DDIChipInfo.h"
#endif

/*===========================================================================
 *                        EXTERNAL GLOBAL VARIABLES
 *===========================================================================*/
extern pdc_tcs_config   g_pdcTCSConfig[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS];
extern pdc_tcs_config   g_pdcTCSConfigV2[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS];

extern pdc_tcs_resource g_pdcResourceList[TCS_TOTAL_RESOURCE_NUM];

/*===========================================================================
 *                        INTERNAL GLOBAL VARIABLES
 *===========================================================================*/
struct
{
  pdc_tcs_config    (*config)[TCS_NUM_TOTAL][NUM_COMMANDS_PER_TCS];
  pdc_tcs_resource  (*resource)[TCS_TOTAL_RESOURCE_NUM];
}g_pdcTCS = {&g_pdcTCSConfig, &g_pdcResourceList};

/*===========================================================================
 *                            INTERNAL FUNCTIONS
 *===========================================================================*/
/**
 * pdcTcs_getConfig
 *
 * @brief Gets the TCS config based on the target
 */
static void pdcTcs_getConfig(void)
{
#ifdef PDC_IMAGE_SLPI
  DalChipInfoVersionType version = DalChipInfo_ChipVersion();
  if(version >= CHIPINFO_VERSION(2, 0))
  {
    /* SLPI V2+ requires different configuration */
    g_pdcTCS.config = &g_pdcTCSConfigV2;
  }
#endif
  
  return;
}

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

  /* Update main data structure with base address */
  config->cmd.resourceP = &((*g_pdcTCS.resource)[config->cmd.index]);

  if(config->cmd.resourceP->base_addr == 0)
  {
    pdcLog_printf(3,
                "TCS write skipped: Invalid resource (Resource: %s) (TCS.Cmd: %u.%u)",
                config->cmd.resourceP->name,
                tcsNum, cmdNum);

    /* Set the 'no enable' bit since this is a bad resource */
    config->data.options |= TCS_CFG_OPT_NO_ENABLE;
    config->cmd.resourceP = NULL;
    return;
  }

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

  /* Compute real address for command */
  address = config->cmd.resourceP->base_addr + config->data.addr_offset;

  /* Convert the VLVL to HLVL that the ARC expects from the TCS's */
  config->data.hlvl = pwr_utils_hlvl_named_resource(config->cmd.resourceP->name, 
                                                    config->data.vlvl, NULL);
  /* If error case, use original data given */
  if(config->data.hlvl < 0)
  {
    config->data.hlvl = config->data.vlvl;
  }

  pdcLog_printf(5, 
                "TCS write (Resource: %s + 0x%x) (TCS.Cmd: %u.%u) (hlvl: %d)",
                config->cmd.resourceP->name,
                config->data.addr_offset,
                tcsNum, cmdNum,
                config->data.hlvl);

  /* Write command/data pair */
  PDC_HWIO_OUTI2(TCSt_CMDn_ADDR, tcsNum, cmdNum, address);
  PDC_HWIO_OUTI2(TCSt_CMDn_DATA, tcsNum, cmdNum, config->data.hlvl);

  return;
}

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * pdcTcs_initialize
 *
 * @brief Initializes the TCS registers
 * 
 * @note called from RCINIT
 */
void pdcTcs_initialize(void)
{
  uint32                nTCS;
  uint32                nRes;
  uint32                nCMD;
  pdc_tcs_config        *cmdData;
  uint32                enableMask[TCS_NUM_TOTAL] = {0};

  pdcTcs_getConfig();

  /* Get all the command addresses for each resource */
  for(nRes = 0; nRes < TCS_TOTAL_RESOURCE_NUM; nRes++)
  {                                                     
    (*g_pdcTCS.resource)[nRes].base_addr = cmd_db_query_addr((*g_pdcTCS.resource)[nRes].name);

    pdcLog_printf(2, "TCS Resource lookup: (Name: %s) (Base address: 0x%x)", 
                  (*g_pdcTCS.resource)[nRes].name,
                  (*g_pdcTCS.resource)[nRes].base_addr);

    /* Verify we get a valid address for each resource */
    if((*g_pdcTCS.resource)[nRes].base_addr == 0)
    {
      pdcLog_printf(0, "WARNING: Bad TCS config: Unable to retrieve resource address");
    }
  }
  
  /* Verify code config matches hardware - SW is DRV0 */
  CORE_VERIFY(NUM_COMMANDS_PER_TCS == PDC_HWIO_INFI(PDC_PARAM_RESOURCE_DRVd, 0, BLK_NUM_TCS_CMDS));
  CORE_VERIFY(TCS_NUM_TOTAL <= PDC_HWIO_INFI(PDC_PARAM_RESOURCE_DRVd, 0, BLK_NUM_TCS));

  /* Write each TCS with the command/data sets */
  for(nTCS = 0; nTCS < TCS_NUM_TOTAL; nTCS++)
  {
    for(nCMD = 0; nCMD < NUM_COMMANDS_PER_TCS; nCMD++)
    {
      cmdData = &((*g_pdcTCS.config)[nTCS][nCMD]);

      /* Check if command is used */
      if(0 == (cmdData->data.options & TCS_CFG_OPT_NOT_USED))
      {
        pdcTcs_writeCmd(nTCS, nCMD, cmdData);

        /* Command is used, but not currently enabled */
        if(0 == (cmdData->data.options & TCS_CFG_OPT_NO_ENABLE))
        {
          enableMask[nTCS] |= (0x01 << nCMD);
        }
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
  return ((*g_pdcTCS.config)[tcsNum]);
}

/*
 * pdcTcs_getAllResourceList
 */
const pdc_tcs_resource* pdcTcs_getAllResourceList(void)
{
  return *g_pdcTCS.resource;
}

/*
 * pdcTcs_enableCommand
 */
void pdcTcs_enableCommand(char *resource, tcs_usage tcsNum, boolean enable)
{
  uint32          baseAddr;
  uint32          temp;
  uint32          nCmd;
  pdc_tcs_config  *cmdData;
  uint32          enableMask = 0;

  CORE_VERIFY_PTR(resource);
  CORE_VERIFY(tcsNum < TCS_NUM_TOTAL);

  baseAddr = cmd_db_query_addr(resource);

  if(0 != baseAddr)
  {
    for(nCmd=0; nCmd < NUM_COMMANDS_PER_TCS; nCmd++)
    {
      cmdData = &((*g_pdcTCS.config)[tcsNum][nCmd]);

      if(0 == (cmdData->data.options & TCS_CFG_OPT_NOT_USED))
      {
        if(baseAddr == cmdData->cmd.resourceP->base_addr)
        {
          enableMask |= (1 << nCmd);
        }
      }
    }

    if(enableMask)
    {
      temp = PDC_HWIO_INI(TCSt_CMD_ENABLE_BANK, tcsNum);

      if(TRUE == enable)
      {
        temp |= enableMask;
      }
      else
      {
        temp &= ~enableMask;
      }

      PDC_HWIO_OUTI(TCSt_CMD_ENABLE_BANK, tcsNum, temp);
    }
  }

  return;
}

