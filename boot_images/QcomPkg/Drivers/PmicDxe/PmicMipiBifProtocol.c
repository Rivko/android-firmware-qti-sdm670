/*! @file PmicMipiBifProtocol.c 

*  PMIC- MIPI BIF MODULE RELATED DECLARATION
*  This file contains functions and variable declarations to support 
*  the PMIC MIPI BIF module.
*
*  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/18/14   al      Updated copyright  
04/04/14   al      Warning cleanup 
03/04/14   va      API to Enable Pull up, Poll Tx Go Status, Enable Rx before Slave Hard Reset
13/12/13   aa      PmicLib Dec Addition
04/22/13   sm      Added PmicMipiBifReadWord API to read data
                   Added APIs in EFI_PmicMipiBifInit to enable BSI module, 
                   TX_INT, RX_INT and ERR_INT interrupts. 
02/13/13   dy      New file.
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "pm_uefi.h"
#include "pm_bsi.h"

#include <Protocol/EFIPmicMipiBif.h>

/*===========================================================================
              LOCAL VARIABLE DECLARATIONS
===========================================================================*/
static PM_UEFI_BSI_DATA_FORMAT_TYPE DataFormatType = PM_UEFI_BSI_DATA_FORMAT__11_BIT; 

/*===========================================================================
                  LOCAL FUNCTION DECLARATIONS
===========================================================================*/
/**
  PmicMipiBifPollIrq()

  @brief
  Poll TX/RX IRQ Status 
  */
EFI_STATUS PmicMipiBifPollIrq
(
  IN  UINT32            PmicDeviceIndex,
  IN  pm_bsi_irq_type   PollIrq,
  OUT BOOLEAN           *IrqStatus
)
{
  BOOLEAN               IrqSts = FALSE;
  pm_err_flag_type      errFlag = PM_ERR_FLAG__SUCCESS;
  EFI_STATUS            Status  = EFI_SUCCESS;
  UINT8                 Timeout = 3;
  
  while (!IrqSts)
  {
    /* Wait for IRQ */
    errFlag = pm_bsi_irq_status( PmicDeviceIndex,
                                 PollIrq,
                                 PM_IRQ_STATUS_LATCHED,
                                 &IrqSts );
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to get IRQ Status- PMIC Error:  %d IrqSts = %d \n\r", errFlag, IrqSts));
      Status = EFI_DEVICE_ERROR;
      goto exit;
    }

    if (!IrqSts)
    {
       //DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to get IRQ Status- PollIrq = (%d) IrqSts = %d \n\r", PollIrq, IrqSts));
      /* Check Timeout */
      if (0 == Timeout--)
      {
          Status = EFI_TIMEOUT;
          goto exit;
      }

      /* Wait 6 ms */
      gBS->Stall(6000);
    }
  }

  /* Clear Tx IRQ */
  errFlag = pm_bsi_irq_clear( PmicDeviceIndex, PollIrq );
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to clear latched IRQ status - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
  }

exit:
  if (IrqStatus)
  {
    *IrqStatus = IrqSts;
  }

  return Status;
}

#define TX_GO_SET TRUE
#define TX_GO_CLEAR FALSE

/**
PmicMipiBifPollTxGoStatus ()

@brief
Poll Tx Go Status 
Note: Assumes that Tx Go is set before calling this API
*/
EFI_STATUS PmicMipiBifPollTxGoStatus
(
  IN  UINT32   PmicDeviceIndex,
  OUT BOOLEAN  *go_sts
)
{
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
  EFI_STATUS        Status  = EFI_SUCCESS;
  UINT8             Timeout = 5;
  BOOLEAN           TxGoSts = TRUE;

  while (TxGoSts)
  {
    /* Wait for IRQ */
    errFlag = pm_bsi_get_tx_go_status( PmicDeviceIndex, &TxGoSts );
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to get TX_GO Status - PMIC Error:  %d \n\r", errFlag));
      Status = EFI_DEVICE_ERROR;
      goto exit;
    }

    if ( TX_GO_SET == TxGoSts)
    {
       //DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to get Tx Go Status \n\r"));
      /* Check Timeout */
      if (0 == Timeout--)
      {
          Status = EFI_TIMEOUT;
          goto exit;
      }

      /* Wait 6 ms */
      gBS->Stall(6000);
    }
  }

exit:
  if ( EFI_SUCCESS == Status)
  {
    *go_sts = TxGoSts;
  }

  return Status;
}

/**
PmicMipiBifSendData ()

@brief
Sends  Data to Slave
*/
EFI_STATUS PmicMipiBifSendData
(
  IN UINT32     PmicDeviceIndex,
  IN UINT32     *SendWord,
  IN UINT32     Length
)
{
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
  BOOLEAN           TxGoSts  = TRUE;
  BOOLEAN           ErrFlagSts  = FALSE;
  UINT32            word_index;

  pm_bsi_error_code_type      BsiErr = PM_MIPI_BIF__NO_ERROR;

  for (word_index = 0; word_index < Length; word_index++)
  {
    /* Write Word */
    errFlag = pm_bsi_write_tx_data( PmicDeviceIndex,  SendWord[word_index] );
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to write TX data - PMIC Error:  %d \n\r", errFlag));
      Status = EFI_DEVICE_ERROR;
      goto exit;
    }

    /* Set Tx Go */
    errFlag = pm_bsi_tx_go(PmicDeviceIndex);
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to set TX GO - PMIC Error:  %d \n\r", errFlag));
      Status = EFI_DEVICE_ERROR;
      goto exit;
    }

    Status = PmicMipiBifPollTxGoStatus(PmicDeviceIndex, &TxGoSts);
    if (Status != PM_ERR_FLAG__SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "MIPIBIF - Failed to set TX GO - PMIC Error:  %d \n\r", Status));
      Status = EFI_DEVICE_ERROR;
      goto exit;
    }
    /* If Go Tx is not set after time out then flag error */
    if (TX_GO_CLEAR == TxGoSts)
    {
      errFlag = pm_bsi_get_err_flag_status( PmicDeviceIndex, &ErrFlagSts );
      if (ErrFlagSts)
      {

        /* Retrieve BSI Error Code */
        errFlag = pm_bsi_get_error_code( PmicDeviceIndex, &BsiErr );
        DEBUG((EFI_D_ERROR, "MIPIBIF - BSI Tx Error Received, Code: %d \n\r", BsiErr));

        /* Clear Error */
        errFlag = pm_bsi_clear_error(PmicDeviceIndex);
        goto exit;

      }
    }
  }

  /* Stall for last byte sent */
  gBS->Stall(6000); //stall 6mS

exit:
  return Status;
}

#define RX_FORMAT_FOR_11_BIT        0x00FF
#define RX_FORMAT_FOR_17_BIT        0x1EE8

/**
PmicMipiBifReadWord ()

@brief
Reads Actual Data 
*/
EFI_STATUS PmicMipiBifReadWord
(
  IN UINT32   PmicDeviceIndex,
  IN UINT32   *ReadWord
)
{
  EFI_STATUS        Status   = EFI_SUCCESS;
  pm_err_flag_type  errFlag  = PM_ERR_FLAG__SUCCESS;
  BOOLEAN           RxIrqSts = FALSE;
  UINT32            ReadData = 0;

  /* Wait for RX IRQ */
  Status = PmicMipiBifPollIrq(PmicDeviceIndex, PM_BSI_MIPI_BIF__RX_INT, &RxIrqSts);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendReceive - Failed to acknowledge RX IRQ - EFI Error: %d \n\r", Status));
    goto exit;
  }

  if (RxIrqSts)
  {
    /* Read Data from RX */
    errFlag =  pm_bsi_read_rx_data( PmicDeviceIndex, (uint32 *)&ReadData );
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "MIPIBIF ReadWord - Failed to read RX data - PMIC Error: %d \n\r", errFlag));
      Status = EFI_DEVICE_ERROR;
    }
    /* Check for Data Format and mask data respectively */
    if(DataFormatType == (PM_UEFI_BSI_DATA_FORMAT_TYPE)PM_BSI_DATA_FORMAT__11_BIT)
    {
        if(!(ReadData & 0x300))
        {
          Status = EFI_DEVICE_ERROR;
          DEBUG((EFI_D_ERROR, "MIPIBIF ReadWord - Device Error \n\r"));
        }
        else
        {
          *ReadWord = (ReadData & RX_FORMAT_FOR_11_BIT);
           //DEBUG((EFI_D_ERROR, "*** MIPIBIF ReadWord - Device SUCCESS ***\n\r"));
        }
    }
    else if(DataFormatType == (PM_UEFI_BSI_DATA_FORMAT_TYPE)PM_BSI_DATA_FORMAT__17_BIT)
    {
        if(!(ReadData & 0x6000))
        {
          Status = EFI_DEVICE_ERROR;
        }
        else
        {
          *ReadWord = (ReadData & RX_FORMAT_FOR_17_BIT);
        }
    }
  }

exit:
  return Status;
}

/*===========================================================================
EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/**
EFI_PmicMipiBifInit ()

@brief
Initializes MipiBif
*/
EFI_STATUS
EFIAPI
EFI_PmicMipiBifInit
(
  IN UINT32  PmicDeviceIndex
)
{
  EFI_STATUS          Status = EFI_SUCCESS;
  pm_err_flag_type    errFlag = PM_ERR_FLAG__SUCCESS;
  
  /* Enable and Configure IRQ for TX, RX and ERR */
  errFlag = pm_bsi_enable(PmicDeviceIndex, PM_BSI_CMD__ENABLE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to enable BSI Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }
  
  errFlag = pm_bsi_irq_enable(PmicDeviceIndex, PM_BSI_MIPI_BIF__TX_INT, TRUE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to enable TX IRQ - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }

  errFlag = pm_bsi_irq_set_trigger(PmicDeviceIndex, PM_BSI_MIPI_BIF__TX_INT, PM_IRQ_TRIGGER_RISING_EDGE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to configure TX IRQ - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }

  errFlag = pm_bsi_irq_enable(PmicDeviceIndex, PM_BSI_MIPI_BIF__RX_INT, TRUE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to enable RX IRQ - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }

  errFlag = pm_bsi_irq_set_trigger(PmicDeviceIndex, PM_BSI_MIPI_BIF__RX_INT, PM_IRQ_TRIGGER_RISING_EDGE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to configure RX IRQ - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }
  
  errFlag = pm_bsi_irq_enable(PmicDeviceIndex, PM_BSI_MIPI_BIF__ERR_INT, TRUE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to enable ERR IRQ - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }

  errFlag = pm_bsi_irq_set_trigger(PmicDeviceIndex, PM_BSI_MIPI_BIF__ERR_INT, PM_IRQ_TRIGGER_RISING_EDGE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to configure ERR IRQ - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }

  errFlag = pm_bsi_pullup_enable(PmicDeviceIndex, PM_BSI_CMD__ENABLE);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Init - Failed to enable BSI PULL UP:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
    goto exit;
  }

exit:
  return Status;
}

/**
EFI_PmicMipiBifSetMode ()

@brief
Set MIPI BIF Modes
*/
EFI_STATUS
EFIAPI
EFI_PmicMipiBifSetMode
(
  IN UINT32                          PmicDeviceIndex,
  IN PM_UEFI_BSI_TAU_CFG_TYPE        TauCfg,
  IN PM_UEFI_BSI_SAMPLING_RATE_TYPE  SamplingRate,
  IN PM_UEFI_BSI_DATA_FORMAT_TYPE    DataFormat
)
{
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_err_flag_type  errFlag = PM_ERR_FLAG__SUCCESS;
  
  /* Configure Tau Cfg */
  errFlag = pm_bsi_set_tau_cfg (PmicDeviceIndex, (pm_bsi_tau_cfg_type)TauCfg);
  
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Set Mode - Failed to configure Tau Cfg - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
  }
  
  /* Configure Sampling Rate */
  errFlag = pm_bsi_set_sampling_rate(PmicDeviceIndex, (pm_bsi_sampling_rate_type)SamplingRate);
  
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Set Mode - Failed to configure sampling rate - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
  }
  
  DataFormatType = DataFormat;

  /* Set Tx and Rx Mode */
  errFlag = pm_bsi_set_rx_data_format(PmicDeviceIndex, (pm_bsi_data_format_type)DataFormat);
  
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Set Mode - Failed to configure RX Data format - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
  }
  
  errFlag = pm_bsi_set_tx_data_format(PmicDeviceIndex, (pm_bsi_data_format_type)DataFormat);
  
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Set Mode - Failed to configure TX Data format - PMIC Error:  %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
  }
  
  return Status;
}


/**
EFI_PmicMipiBifSendWords ()

@brief
Send MIPI BIF Words on through BSI
*/
EFI_STATUS
EFIAPI
EFI_PmicMipiBifSendWords
(
  IN UINT32  PmicDeviceIndex, 
  IN UINT32  SlaveDeviceAddress,
  IN UINT32  WriteAddr,
  IN UINT32  WriteData
)
{   
  EFI_STATUS  Status  = EFI_SUCCESS;
  UINT32      Length  = 4;
  UINT32      SendData[] = {0, 0, 0, 0} ;
  
  /*Send Slave Device Address */
  SendData[0] = (0x600 | SlaveDeviceAddress);

  /*Extended Register Address */
  SendData[1] = (0x0100 | ((0xFF00 & WriteAddr)>>8));

  /*Write Register Address */
  SendData[2] = (0x0200 | (0x00FF & WriteAddr));

  /*Write Data */
  SendData[3] = WriteData;

  
  /* Clear Error */
  Status = pm_bsi_clear_error(PmicDeviceIndex);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Clear Error - Failed to Clear Error before sending words - PMIC Error: %d \n\r", Status));
    Status = EFI_DEVICE_ERROR;
    return Status;
  }

  /* Enable Tx */
  Status = pm_bsi_tx_enable(PmicDeviceIndex, PM_BSI_CMD__ENABLE);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendWords - Failed to enable TX - PMIC Error: %d \n\r", Status));
    Status = EFI_DEVICE_ERROR;
    return Status;
  }

  /* Pass data to helper function */
  Status = PmicMipiBifSendData(PmicDeviceIndex, SendData, Length);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendWords - Failed to send data - EFI Error: %d \n\r", Status));
  }

  /* After each Transaction Disabling TX = 0 */
  Status = pm_bsi_tx_enable(PmicDeviceIndex, PM_BSI_CMD__DISABLE);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendWords - Failed to Disbale TX - PMIC Error: %d \n\r", Status));
    Status = EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
EFI_PmicMipiBifSendReceive ()

@brief
Receive MIPI BIF Data from Slave
*/
EFI_STATUS
EFIAPI
EFI_PmicMipiBifSendReceive
(
  IN  UINT32  PmicDeviceIndex, 
  IN  UINT32  SlaveDeviceAddress,
  IN  UINT32  ReadAddr,
  OUT UINT32  *pReadWord
)
{   
  EFI_STATUS  Status   = EFI_SUCCESS;
  UINT32      Length   = 4;
  UINT32      WordRead = 0;
  UINT32      SendData[] = {0, 0, 0, 0};
  
  if (!pReadWord)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  //send RBL, ERA and RRA and then read
  /*Send Slave Device Address */
  SendData[0] = (0x600 | SlaveDeviceAddress);

  /*Send Read Burst Length as 1*/
  SendData[1] = 0x421;

  /*Extended Register Address */
  SendData[2] = (0x0100 | ((0xFF00 & ReadAddr)>>8));

  /*Read Register Address */
  SendData[3] = (0x300 | (0x00FF & ReadAddr));

  /* Clear Error */
  Status = pm_bsi_clear_error(PmicDeviceIndex);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Clear Error - Failed to Clear Error before sending words - PMIC Error: %d \n\r", Status));
    Status = EFI_DEVICE_ERROR;
    return Status;
  }

  /* Enable Tx */
  Status = pm_bsi_tx_enable(PmicDeviceIndex, PM_BSI_CMD__ENABLE);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendWords - Failed to enable TX - PMIC Error: %d \n\r", Status));
    Status = EFI_DEVICE_ERROR;
    return Status;
  }

  /* Enable Rx */
   Status = pm_bsi_rx_enable(PmicDeviceIndex, PM_BSI_CMD__ENABLE);
   if (EFI_ERROR(Status))
   {
     DEBUG((EFI_D_ERROR, "MIPIBIF SendWords - Failed to enable RX - PMIC Error: %d \n\r", Status));
     Status = EFI_DEVICE_ERROR;
     goto exit;
   }

  /* Clear Tx IRQ */
  Status = pm_bsi_irq_clear( PmicDeviceIndex, PM_BSI_MIPI_BIF__TX_INT);

  Status = pm_bsi_irq_clear( PmicDeviceIndex, PM_BSI_MIPI_BIF__RX_INT);

  /* Pass data to helper function */
  Status = PmicMipiBifSendData(PmicDeviceIndex, SendData, Length); 
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendReceive - Failed to send data - EFI Error: %d \n\r", Status));
    goto exit;
  }
  
  Status = PmicMipiBifReadWord(PmicDeviceIndex, &WordRead);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendReceive - Failed to read data - EFI Error: %d \n\r", Status));
  }

  /* Dsiable Tx */
  Status = pm_bsi_tx_enable(PmicDeviceIndex, PM_BSI_CMD__DISABLE);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Tx Disable - Failed - EFI Error: %d \n\r", Status));
    goto exit;
  }

  /* After each Transaction Disabling RX = 0 */
  Status = pm_bsi_rx_enable(PmicDeviceIndex, PM_BSI_CMD__DISABLE);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF SendWords - Failed to enable RX - PMIC Error: %d \n\r", Status));
    Status = EFI_DEVICE_ERROR;
  }

  *pReadWord = WordRead;

exit:
  return Status;
}


/**
EFI_PmicMipiBifSlaveHardReset()

@brief
Hard Resets Slave
*/
EFI_STATUS
EFIAPI
EFI_PmicMipiBifSlaveHardReset
(
  IN UINT32 PmicDeviceIndex
)
{
  EFI_STATUS       Status  = EFI_SUCCESS;
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  /* i)BSI_EN = 1*/
  errFlag = pm_bsi_enable(PmicDeviceIndex, PM_BSI_CMD__ENABLE);

  /* i)MIPI BIF RX Disabled  = 0*/
  errFlag = pm_bsi_rx_enable(PmicDeviceIndex, PM_BSI_CMD__DISABLE);

  /* ii)Set MIPI_BIF_FORCE_LOW = 1*/
  errFlag |= pm_bsi_set_bcl_force_low(PmicDeviceIndex, TRUE);

  /* iii)wait for more than 2ms*/
   gBS->Stall(4000); 

  /* iv)Set MIPI_BIF_FORCE_LOW = 0*/
  errFlag |= pm_bsi_set_bcl_force_low(PmicDeviceIndex, FALSE);

  /* v)Wait for more than 10ms*/
   gBS->Stall(12000);

  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "MIPIBIF Slave hard reset failed - EFI Error: %d \n\r", errFlag));
    Status = EFI_DEVICE_ERROR;
  }

  return Status;

}

/**
EFI_PmicMipiBifGetStatus()

@brief
Reads MIPI-BIF configuration status
*/
EFI_STATUS
EFIAPI
EFI_PmicMipiBifGetStatus
(
  IN  UINT32                    PmicDeviceIndex,
  OUT PM_UEFI_BSI_STATUS_TYPE   *BsiStatus
)
{
  EFI_STATUS        Status  = EFI_SUCCESS;
  pm_bsi_status_type bsi_status;
  pm_err_flag_type    errFlag = PM_ERR_FLAG__SUCCESS;

  if (!BsiStatus)
  {
    Status = EFI_DEVICE_ERROR;
  }
  else
  {
    errFlag = pm_bsi_get_status(PmicDeviceIndex, &bsi_status);
    if (errFlag != PM_ERR_FLAG__SUCCESS)
    {
      Status = EFI_DEVICE_ERROR;
    }
    else
    {
      BsiStatus->BsiEnable = (bsi_status.bsi_enable == PM_BSI_CMD__ENABLE)? TRUE : FALSE;
      BsiStatus->TauCfg = (PM_UEFI_BSI_TAU_CFG_TYPE)bsi_status.tau_cfg;
      BsiStatus->RxDataFormat = (PM_UEFI_BSI_DATA_FORMAT_TYPE)bsi_status.rx_data_format;
      BsiStatus->TxDataFormat = (PM_UEFI_BSI_DATA_FORMAT_TYPE)bsi_status.tx_data_format;
      BsiStatus->RxEnable = (bsi_status.rx_enable == PM_BSI_CMD__ENABLE) ? TRUE : FALSE;
      BsiStatus->TxEnable = (bsi_status.tx_enable == PM_BSI_CMD__ENABLE) ? TRUE : FALSE;
      BsiStatus->SamplingRate = (PM_UEFI_BSI_SAMPLING_RATE_TYPE)bsi_status.sampling_rate;
      BsiStatus->BclForceLow = (BOOLEAN)bsi_status.bcl_force_low;
    }
  }

  return Status;
}

/**
PMIC MIPI BIF UEFI Protocol implementation
*/
EFI_QCOM_PMIC_MIPIBIF_PROTOCOL PmicMipiBifProtocolImplementation = 
{
    PMIC_MIPIBIF_REVISION,
    EFI_PmicMipiBifInit,
    EFI_PmicMipiBifSetMode,
    EFI_PmicMipiBifSendWords,
    EFI_PmicMipiBifSendReceive,
    EFI_PmicMipiBifSlaveHardReset,
    EFI_PmicMipiBifGetStatus
};

