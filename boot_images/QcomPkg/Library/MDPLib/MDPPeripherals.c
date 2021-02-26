/*=============================================================================
 
  File: MDPPeripherals.c
 
  Source file for MDP peripheral functions such as I2C
  
     Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/EFII2C.h>
#include "MDPLib.h"
#include "MDPLib_i.h"
#include "MDPSystem.h"
#include "MDPPlatformLib.h"
#include "xmlparser.h"
#include "i2c_api.h"        

/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */
/* MDP Peripheral handle type */
typedef void      *MDPPeripheral_HandleType;

/* -----------------------------------------------------------------------
** Structures
** ----------------------------------------------------------------------- */
//-------------------------------------------------------------------------------------------------
//� I2C_DriverCtx
//
//� @brief
//����� Structure that hold I2C context information. 
//-------------------------------------------------------------------------------------------------
//

//I2C Slave Context
typedef struct
{
  MDPPeripheral_HandleType  hI2CHandle;                         /**< I2C handle for slave addresses */
  i2c_instance              eI2CCoreInstance;                   /**< I2C core Number, can be found the mapping between GPIO# and I2C Core# from IPCAT-->GPIO Maps */
  i2c_slave_config          sI2CCfg;                            /**<I2C configuration */
  uint32                    timeout_ms;                         
  uint32                    transfer_direction;
} slave_context;

//I2C Driver Context
typedef struct _I2C_DriverCtx
{
  bool32                    bInited;                               /**< Is I2C initialized */
  slave_context             sSlaveCtx[MDP_I2C_SLAVE_ID_NUM];       /**< I2C Slave Context  */
}I2C_DriverCtx;



/* -----------------------------------------------------------------------
** Global variables
** ----------------------------------------------------------------------- */
I2C_DriverCtx  gI2CDriverCtx = {0};

#define I2C_CMD_CUSTOM_DELAY                        (0xFF)
#define I2C_BUS_WAIT_TIME_IN_MS                     (2500)

#define I2C_GET_DRIVER_CTX()                        &gI2CDriverCtx


/*------------------------------------------------------------------------
Local Prototypes
-------------------------------------------------------------------------- */
static MDP_Status SendPanelXMLSequence(I2C_DriverCtx *pI2CDriverCtx, void *pCommandBuffer, MDP_Panel_AttrType *pPanelConfig);

/*------------------------------------------------------------------------
Function definitions
-------------------------------------------------------------------------- */

//-------------------------------------------------------------------------------------------------
//  I2CDriver_Init
//
//  @brief
//      Initialize the I2C handles to write to I2C bus.
//
//  @params
//      [IN] pPanelConfig
//          Panel information.
//
//  @return
//      MDP_STATUS_OK if I2C is initialized successfully.
//-------------------------------------------------------------------------------------------------
//
MDP_Status I2CDriver_Init(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status             eStatus       = MDP_STATUS_OK;
  I2C_DriverCtx         *pI2CDriverCtx = I2C_GET_DRIVER_CTX();
  EFI_QCOM_I2C_PROTOCOL *pI2CProtocol  = NULL;
  

  if ((NULL == pI2CDriverCtx) || 
      (NULL == pPanelConfig))
  {
    eStatus = MDP_STATUS_NO_RESOURCES;
    DEBUG ((EFI_D_ERROR, "DisplayDxe: I2CDriver_Init() no resources \n"));
  }
  else
  {
    if ((0 == pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CGSBICore) ||
        (pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CNumOfSlaves > MDP_I2C_SLAVE_ID_NUM))
    {
      eStatus = MDP_STATUS_BAD_PARAM;
      DEBUG ((EFI_D_ERROR, "DisplayDxe: I2CDriver_Init() Invalid I2C configuration \n"));
    }
    else if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomI2CProtocolGuid, NULL, (void**)&pI2CProtocol))
    {
      eStatus = MDP_STATUS_FAILED;
      DEBUG ((EFI_D_ERROR, "DisplayDxe: I2CDriver_Init() failed to locate gQcomI2CProtocolGuid\n"));
    }
    else
    {
      uint32                    uI;

      // Iterate through number of slaves and initialize the slave handles
      for (uI=0; uI<pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CNumOfSlaves; uI++)
      {
        
        MDP_OSAL_MEMZERO(&pI2CDriverCtx->sSlaveCtx[uI], sizeof(slave_context));
        
        
        // Core specified in the panelcfg XML starts from 1. i2c_instance starts from 0
        pI2CDriverCtx->sSlaveCtx[uI].eI2CCoreInstance = (i2c_instance)(pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CGSBICore - 1);

        // If non-zero frequency is specified in panel cfg XML, then use that
        // Else use default value
        pI2CDriverCtx->sSlaveCtx[uI].timeout_ms = I2C_BUS_WAIT_TIME_IN_MS;

        // Initialize handle
        if (I2C_SUCCESS != i2c_open(pI2CDriverCtx->sSlaveCtx[uI].eI2CCoreInstance, &pI2CDriverCtx->sSlaveCtx[uI].hI2CHandle))
        {
          eStatus = MDP_STATUS_FAILED;
          DEBUG ((EFI_D_ERROR, "DisplayDxe: I2CDriver_Init() I2C Init(SlaveAddress=0x%x) failed \n", pPanelConfig->uAttrs.sDsi.sI2CConfig.aI2CSlaveAddressList[uI]));
          break;
        }
        else
        {
          pI2CDriverCtx->sSlaveCtx[uI].sI2CCfg.slave_address = pPanelConfig->uAttrs.sDsi.sI2CConfig.aI2CSlaveAddressList[uI];
          pI2CDriverCtx->sSlaveCtx[uI].sI2CCfg.bus_frequency_khz = (pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CBusFrequency) ? pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CBusFrequency : MDP_I2C_DEFAULT_BUS_FREQUENCY_KHZ;
          pI2CDriverCtx->sSlaveCtx[uI].sI2CCfg.mode = I2C;
          pI2CDriverCtx->sSlaveCtx[uI].sI2CCfg.slave_max_clock_stretch_us = 500;
          pI2CDriverCtx->sSlaveCtx[uI].sI2CCfg.core_configuration1 = 0;
          pI2CDriverCtx->sSlaveCtx[uI].sI2CCfg.core_configuration2 = 0;
        }
      }

      if (MDP_STATUS_OK == eStatus)
      {
        pI2CDriverCtx->bInited = TRUE;
      }
    }    
  }

  return eStatus;
}

//-------------------------------------------------------------------------------------------------
//  I2CDriver_Deinit
//
//  @brief
//      Deinitialize the I2C Core.
//
//  @params
//      [IN] pPanelConfig
//          Panel information.
//
//  @return
//      VOID.
//-------------------------------------------------------------------------------------------------
//
void I2CDriver_Deinit(MDP_Panel_AttrType *pPanelConfig)
{
  I2C_DriverCtx         *pI2CDriverCtx = I2C_GET_DRIVER_CTX();
  uint32 uI;

  // Iterate through number of slaves and deinit
  for (uI=0; uI<pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CNumOfSlaves; uI++)
  {
    if (NULL != pI2CDriverCtx->sSlaveCtx[uI].hI2CHandle)
    {
      i2c_close(pI2CDriverCtx->sSlaveCtx[uI].hI2CHandle);
    }
  }

  pI2CDriverCtx->bInited = FALSE;
}

//-------------------------------------------------------------------------------------------------
//  I2CPanelSendCommandSequence
//
//  @brief
//      Send I2C command sequence.
//
//  @params
//      [IN] pPanelConfig
//          Panel information.
//      [IN] eCommand
//          Init/Term commands.
//      [IN] pCustomCommand
//          Custom command.
//      [IN] uFlags
//          Reserved.
//
//  @return
//      MDP_STATUS_OK if I2C command sequence is successfully sent.
//-------------------------------------------------------------------------------------------------
//
MDP_Status I2CPanelSendCommandSequence(MDP_Panel_AttrType  *pPanelInfo, 
                                       MdpPanelCommandType  eCommand, 
                                       void                *pCustomCommand, 
                                       uint32               uFlags)
{
  MDP_Status            eStatus        =  MDP_STATUS_OK;
  I2C_DriverCtx        *pI2CDriverCtx  =  I2C_GET_DRIVER_CTX();

  switch (eCommand)
  {
    case MDP_PANEL_COMMAND_INIT:
    {
      if (NULL != pPanelInfo->uAttrs.sDsi.sI2CConfig.pI2CInitSequence)
      {
        eStatus = SendPanelXMLSequence(pI2CDriverCtx, pPanelInfo->uAttrs.sDsi.sI2CConfig.pI2CInitSequence, pPanelInfo);

        if (EFI_SUCCESS != eStatus)
        {
          DEBUG ((EFI_D_ERROR, "DisplayDxe: I2CPanelSendCommandSequence() failed to send init sequence \n"));
        }        
      }      
    }
    break;

    case MDP_PANEL_COMMAND_TERM:
    {
      // Do nothing
    }
    break;

    default:
      eStatus = MDP_STATUS_NOT_SUPPORTED;     
  }
  return eStatus;
}

/*------------------------------------------------------------------------
Local Functions
-------------------------------------------------------------------------- */

//-------------------------------------------------------------------------------------------------
//  SendPanelXMLSequence
//
//  @brief
//      Local helper function to send a I2C command sequence to the panel.
//
//  @params
//      [IN] pI2CDriverCtx
//          I2C driver context info.
//      [IN] pCommandBuffer
//          Command buffer.
//      [IN] pPanelConfig
//          Panel information.
//
//  @return
//      MDP_STATUS_OK if I2C command sequence is successfully sent.
//-------------------------------------------------------------------------------------------------
//
static MDP_Status SendPanelXMLSequence(I2C_DriverCtx      *pI2CDriverCtx, 
                                       void               *pCommandBuffer, 
                                       MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status             eStatus       = MDP_STATUS_OK;
  EFI_QCOM_I2C_PROTOCOL *pI2CProtocol  = NULL;
  

  if ((NULL == pI2CDriverCtx  ) ||
      (NULL == pPanelConfig))
  {
      eStatus = MDP_STATUS_BAD_PARAM;
  }
  else if (EFI_SUCCESS != gBS->LocateProtocol(&gQcomI2CProtocolGuid, NULL, (void**)&pI2CProtocol))
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else if (NULL == pCommandBuffer)
  {
    // No commands, return successful
  }
  else if (FALSE == pI2CDriverCtx->bInited)
  {
    eStatus = MDP_STATUS_NO_RESOURCES;
  }
  else
  {
    uint8                *pSrc              = (uint8 *)pCommandBuffer;
    XML_BinaryPacketType  sXMLBinaryData;
    uint32                uHeaderSize       = (uint32)((UINTN)&(((XML_BinaryPacketType*)pSrc)->pData) - (UINTN)&(((XML_BinaryPacketType*)pSrc)->uPacketLength));

    MDP_OSAL_MEMZERO(&sXMLBinaryData, sizeof(XML_BinaryPacketType));

    // Use Unaligned Memory read 
    MDP_ReadUnaligned_U32(&sXMLBinaryData.uPacketLength, pSrc);
    sXMLBinaryData.pData = (uint8* )&(((XML_BinaryPacketType*)pSrc)->pData);

    /* Parse init command packets */
    while (0 != sXMLBinaryData.uPacketLength)
    {
      uint8 *pI2CCmd = (uint8*)sXMLBinaryData.pData;

      if (0 == sXMLBinaryData.uPacketLength)
      {
        // A null packet signifies end of data
        break;
      }
      else if (I2C_CMD_CUSTOM_DELAY == *pI2CCmd)
      {
        uint8 uDelayLength = *(++pI2CCmd);

        // Milliseconds delay
        MDP_OSAL_DELAYMS(uDelayLength);
      }
      else
      {
        uint32 uI = 0;

        // Find the slave handle corresponding to the slave address
        for (uI=0; uI<pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2CNumOfSlaves; uI++)
        {
          if ((uint8)pPanelConfig->uAttrs.sDsi.sI2CConfig.aI2CSlaveAddressList[uI] == *pI2CCmd)
          {
            if (NULL == pI2CDriverCtx->sSlaveCtx[uI].hI2CHandle)
            {
              eStatus = MDP_STATUS_FAILED;
              break;
            }
            else
            {       

              i2c_status  eI2cStatus          = I2C_SUCCESS;
              uint16      uOffsetLen          = (uint16)(pPanelConfig->uAttrs.sDsi.sI2CConfig.uI2COffsetLengthInByte);
              uint32      uActualWrittenBytes = 0; 
              uint16      uWriteAddress;
              uint8      *pWriteBuffer;

              if (uOffsetLen == 2)
              {
                //I2C register address offset is 16 bits
                uWriteAddress = (pI2CCmd[2] << 8) + (pI2CCmd[1]);
                pWriteBuffer  = pI2CCmd + 3;
              }
              else
              {
                //I2C register address offset is 8 bits
                uWriteAddress = pI2CCmd[1];
                pWriteBuffer  = pI2CCmd + 2;
              }

              //I2C Write
              eI2cStatus = i2c_write(pI2CDriverCtx->sSlaveCtx[uI].hI2CHandle,
                                    &pI2CDriverCtx->sSlaveCtx[uI].sI2CCfg,
                                     uWriteAddress,
                                     uOffsetLen,
                                     pWriteBuffer,
                                     sXMLBinaryData.uPacketLength - 1 - uOffsetLen,
                                    &uActualWrittenBytes,
                                     pI2CDriverCtx->sSlaveCtx[uI].timeout_ms);
              
              if (I2C_SUCCESS != eI2cStatus)
              {
                DEBUG((EFI_D_ERROR, "DisplayDxe: i2c_write failed on SlaveAddress=0x%x eI2cStatus=0x%x \n", pPanelConfig->uAttrs.sDsi.sI2CConfig.aI2CSlaveAddressList[uI], eI2cStatus));
                eStatus = MDP_STATUS_FAILED;
                break;
              }
              
            }
          }          
        }
      }

      // Go to next packet (header + payload size)
      pSrc += (uint8)(sXMLBinaryData.uPacketLength + uHeaderSize);
      // Unaligned Memory read 
      MDP_ReadUnaligned_U32(&sXMLBinaryData.uPacketLength, pSrc);
      sXMLBinaryData.pData = pSrc + uHeaderSize;
    }   
  }

  return eStatus;
}



#ifdef __cplusplus
}
#endif


