 /*! @file pm_fg_sram.c
 *
 * PMIC Battery Profile SRAM/OTP access functionalities 
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*=============================================================================
                              EDIT HISTORY


 when          who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/22/17   czq     Detect the bad conditions of FG SRAM (trim registers) and issue VDD hard reset on PM/PMi to recover
 07/10/17   pxm     Add function PmicFgSram_ImaWriteSingle
 11/01/16   va      Updated new IMA clear sequence and clear IMA by default 
 09/13/16   aab     Updated logging text
 08/24/16   aab     Updated PmicFgSram_Init()
 07/11/16   aab     Created to support PMI8998
=============================================================================*/
#include "pm_fg_sram.h"
#include "pm_fg_mem_if.h"
#include "pm_target_information.h"
#include "pm_app_chgr.h"
#include "pm_utils.h"
#include "CoreVerify.h"
#include "pm_log_utils.h"
#include "smem.h"
#include "pm_app_pon.h"


/*===========================================================================
                               MACROS
===========================================================================*/
#define FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME 30    //in us
#define FG_MEM_AVAILABLE_RT_STS_POLL_MAX_TIME 249990  // in us (~250 ms)
#define FG_SRAM_PROFILE_RANGE                 0x7F
#define FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR    0x4F  //refer MDOS for address, data values
#define FG_SRAM_PROFILE_INTEGRITY_OFFSET      0x3
#define FG_SRAM_PROFILE_INTEGRITY_ENABLE      0x01
#define PM_FG_NO_OF_IMA_RETRIES               3
#define PM_FG_NUM_OF_DMA_ERR_STS_POLL         3
#define FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT 5

#define FG_SRAM_SP_UN_USED_ADDR   20 /* Un used by SW and HW address of SRAM */ 
#define FG_SRAM_SP_UN_USED_ZERO   0x0  /* If reg is zeroed */

#define FG_SRAM_PAUSE_DELAY                   30
#define FG_SRAM_PAUSE_DELAY_MAX_TIME          9990 // in us (~10 ms)

/*=========================================================================
                            GLOBAL VARIABLES
===========================================================================*/
static FgSramState_type SramState = FG_SRAM_STATUS_INVALID;

/*===========================================================================
                               TYPE DEFINITIONS
===========================================================================*/



/*==========================================================================
                        LOCAL API PROTOTYPES
===========================================================================*/

pm_err_flag_type PmicFgSram_GetState(FgSramState_type *FgSramSt);
pm_err_flag_type PmicFgSram_ImaReadSingleOffset(uint32 PmicDeviceIndex, uint16 ReadAddress, uint8 *Data, uint8 Offset);
pm_err_flag_type PmicFgSram_ImaReadSingle(uint32 PmicDeviceIndex, uint16 ReadAddress, uint32 *fg_memif_data, boolean *bBeatCountMatch);
pm_err_flag_type PmicFgSram_RequestFgSramAccess(uint32  PmicDeviceIndex, pm_fg_mem_if_mem_intf_cfg  mem_intf_cfg);
pm_err_flag_type PmicFgSram_ReleaseFgSramImaAccess (uint32  PmicDeviceIndex, boolean *bExceptionOccured);
pm_err_flag_type PmicFgSram_HandleIfImaException(uint32  PmicDeviceIndex, boolean *bExceptionOccured);
pm_err_flag_type PmicFgSram_SetState(FgSramState_type FgSramSt);
pm_err_flag_type PmicFgSram_ResetIma(uint32 PmicDeviceIndex);
pm_err_flag_type PmicFgSram_PollImaReady(uint32 PmicDeviceIndex, boolean *bImaAccessAvailable);
pm_err_flag_type PmicFgSram_ClearDmaLog(uint32 PmicDeviceIndex);
pm_err_flag_type pm_fg_sram_content_verify(uint32 pmic_index, boolean *unexpected_match);


/*==========================================================================
                        GLOBAL API DEFINITION
===========================================================================*/


/**
PmicFgSram_Init()

@brief
Initializes Sram State
*/
pm_err_flag_type 
PmicFgSram_Init(uint32 PmicDeviceIndex)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   boolean bExceptionOccured   = FALSE;

   err_flag = PmicFgSram_ClearDmaLog(PmicDeviceIndex);
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      return err_flag;
   }

   //pm_log_message("PmicFgSram_Init explicit reset IMA ");
   err_flag = PmicFgSram_ResetIma(PmicDeviceIndex);
   //pm_log_message("PmicFgSram_Init explicit reset IMA completed ");

   SramState = FG_SRAM_STATUS_INIT;
   err_flag |= PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);
   if(bExceptionOccured == TRUE)
   {
      pm_log_message("IMAException -IMA seq cleared");
   }
   return err_flag;
}



/**
PmicFgSram_ClearDmaLog()

@brief
Clears DMA Log. Required as a work around routine to fix issue 
on PMI8998 v2.0 
*/
pm_err_flag_type 
PmicFgSram_ClearDmaLog(uint32 PmicDeviceIndex)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG_SUCCESS;
   boolean dma_err_sts = FALSE;
   uint8 poll_count    = 0;

   err_flag |= pm_fg_memif_get_dma_err_sts(PmicDeviceIndex, &dma_err_sts); //Check for DMA error
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
      return err_flag;
   }

   if (dma_err_sts)
   {
      err_flag |= pm_fg_memif_dma_clear(PmicDeviceIndex, TRUE); //Clear DMA
      for (poll_count=0; poll_count < PM_FG_NUM_OF_DMA_ERR_STS_POLL; poll_count++) //Poll for DMA err status
      {
         err_flag |= pm_fg_memif_get_dma_err_sts(PmicDeviceIndex, &dma_err_sts);
         if ((dma_err_sts == FALSE) || (err_flag != PM_ERR_FLAG_SUCCESS))
         {
            break;
         }
      }

      if ((dma_err_sts == TRUE)|| (err_flag != PM_ERR_FLAG_SUCCESS))
      {
         pm_log_message("ERROR, DMA Clear Log failed"); 
         return err_flag = PM_ERR_FLAG_PERIPHERAL_ERR;
      }
      err_flag |= pm_fg_memif_dma_clear(PmicDeviceIndex, FALSE);//reset
   }
   return err_flag;
}






/**
PmicFgSram_ReadIntegrityBit()

@brief
Read Integrity Bit Satus from Sram Address
*/
pm_err_flag_type 
PmicFgSram_ReadIntegrityBit(uint32 PmicDeviceIndex, boolean *bIntergrityBit)
{
   pm_err_flag_type  Status  = PM_ERR_FLAG_SUCCESS;
   uint8 Data = 0;

   if(bIntergrityBit == NULL)
   {
     return PM_ERR_FLAG_INVALID_PARAMETER;
   }

   Status = PmicFgSram_ImaReadSingleOffset( PmicDeviceIndex, 
                                            (uint16)FG_SRAM_PROFILE_INTEGRITY_CHK_ADDR,
                                            &Data,
                                            (uint8)FG_SRAM_PROFILE_INTEGRITY_OFFSET );

   if ((Data == 1))
   {
     *bIntergrityBit = TRUE;
   }
   else
   {
      *bIntergrityBit = FALSE;
   }

   return Status;
}



/**
PmicFgSram_GetState()

@brief
Returns Current Sram State
*/
pm_err_flag_type 
PmicFgSram_GetState(FgSramState_type *FgSramSt)
{
  pm_err_flag_type  Status  = PM_ERR_FLAG_SUCCESS;

  if ( FG_SRAM_STATUS_INVALID == SramState)
  {
    Status = PM_ERR_FLAG_PERIPHERAL_ERR;
  }
  else
  {
    *FgSramSt = SramState;
  }

  return Status;
}



/**
PmicFgSram_ImaReadSingleOffset()

@brief
Read 1 Offset byte from given Sram Address
*/
pm_err_flag_type 
PmicFgSram_ImaReadSingleOffset(uint32 PmicDeviceIndex, uint16 ReadAddress, uint8 *Data, uint8 Offset)
{
  pm_err_flag_type  Status  = PM_ERR_FLAG_SUCCESS;
  uint32     ReadData = 0;

  if(!Data || ReadAddress > FG_SRAM_END_ADDR || Offset > FG_SRAM_RD_WR_OFFSET_WIDTH)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  Status = PmicFgSram_ImaRead(PmicDeviceIndex, ReadAddress, &ReadData);
  if(Status != PM_ERR_FLAG_SUCCESS)
  {
    //pm_log_message("Single Ima Read Failed = %r \n\r", __FUNCTION__, Status));
    pm_log_message("Single IMA Read Failed");
  }
  else
  {
    /* Read is good and Assign Read Data */
    *Data = (uint8) (ReadData & (0xFF << Offset));
    //pm_log_message("Single Ima ReadData = 0x%x Offset Data = 0x%x Offset = %d \n\r", __FUNCTION__, ReadData, *Data, Offset));
  }

  return Status;
}




pm_err_flag_type 
PmicFgSram_ImaRead(uint32 PmicDeviceIndex, uint16 ReadAddress, uint32 *fg_memif_data)
{
  pm_err_flag_type  Status  = PM_ERR_FLAG_SUCCESS;
  uint32     ReadData        = 0;
  boolean    bBeatCountMatch = FALSE;
  uint8      NoOfImaRetries  = 0;

  if(!fg_memif_data || ReadAddress > FG_SRAM_END_ADDR )
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  do{
    /* Read Word*/
    Status = PmicFgSram_ImaReadSingle(PmicDeviceIndex, ReadAddress, &ReadData, &bBeatCountMatch);
    if(Status != PM_ERR_FLAG_SUCCESS)
    {
      //pm_log_message("Single Ima Read Failed = %r \n\r", __FUNCTION__, Status));
      break;
    }

    if (TRUE == bBeatCountMatch)
    {
      /* Read is good and in single FG cycle */
      break;
    }
    else
    {
      NoOfImaRetries++; /* Upkeep counter */
      ReadData = 0; /* Reset Input */
      bBeatCountMatch = FALSE; /* Reset Input */
      //pm_log_message("Single Ima Read FG Beat Count did not match Retry IMA read = %d \n\r", __FUNCTION__, NoOfImaRetries));
      /* continue; not needed but intend to continue */
     }
  }while(NoOfImaRetries < PM_FG_NO_OF_IMA_RETRIES);

  if((NoOfImaRetries > PM_FG_NO_OF_IMA_RETRIES ) || (bBeatCountMatch == FALSE))
  {
    //pm_log_message("Single Ima Read FG Beat Count did not match Read Failure = %d \n\r", __FUNCTION__));
    //Status = EFI_DEVICE_ERROR;
     Status = PM_ERR_FLAG_PERIPHERAL_ERR;
  }
  else
  {
    /* Read is good and Assign Read Data */
    *fg_memif_data = ReadData;
    //pm_log_message("Single Ima ReadData = %d NoOfImaRetries = %d \n\r", __FUNCTION__, ReadData, NoOfImaRetries));
  }

  return Status;

}



/**
PmicFgSram_ImaWriteSingle()

@brief
Writes 4 bytes (uint32) in given Sram address using IMA handshake
*/
pm_err_flag_type PmicFgSram_ImaWriteSingle(uint32 PmicDeviceIndex, uint16 WriteAddress, uint32 fg_memif_data)
{
  pm_err_flag_type       Status        = PM_ERR_FLAG_SUCCESS;
  pm_err_flag_type err_flg       = PM_ERR_FLAG_SUCCESS;
  boolean          bMemAvailable = FALSE;
  boolean          bExceptionOccured = FALSE;
  uint32           nExcpCnt      = 0;

  do{
    /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
    Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

    /* Select IMA access type  */
    err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

    /* Poll for IMA readiness  */
    Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
    if(Status != PM_ERR_FLAG_SUCCESS)
    {
      pm_log_message("IMA Polling Failed for initiating write xaction = %r \r\n", Status);
    }

    if(TRUE == bMemAvailable)
    {
      /* Request Single access */
      err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);

      /* Requet read access */
      err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FG_MEMORY_ACCESS_INTF_CTL_WRITE_ACCESS);

      /* Write Location Address */
      err_flg |=  pm_fg_mem_if_write_addr(PmicDeviceIndex, WriteAddress);

      err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

      /* Write Data */
      err_flg |=  pm_fg_mem_if_write_data(PmicDeviceIndex, fg_memif_data);

      bMemAvailable = FALSE; /* Reset Var */

      /* Poll for IMA readiness  */
      Status = PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
      if(Status != PM_ERR_FLAG_SUCCESS)
      {
        pm_log_message("IMA Polling Failed = %r \r\n", Status);
      }

      if(TRUE != bMemAvailable)
      {
        pm_log_message("IMA Polling Failed after write \r\n");
        /*Check Integrity of data by exception status */
        Status = PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
        if(TRUE == bExceptionOccured)
        {
          if(nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT)
          {
            pm_log_message("IMA Exception Retry to single Write Again here \r\n");
            /* 3 trials before giving up burst read  use continue with 3 static count and update write addr */
            pm_log_message("IMA Exception Retry CurrAddr = %d, nExcpCnt = %d \r\n", __FUNCTION__, WriteAddress, nExcpCnt);
            /* Write Location Address again to make sure we are re writing to same address where IMA exception occurred */
            err_flg =  pm_fg_mem_if_write_addr(PmicDeviceIndex, WriteAddress);
            continue;
          }
          else
          {
            pm_log_message("IMA Exception Retry Exceeded nExcpCnt = %d \r\n", nExcpCnt);
            Status = PM_ERR_FLAG_PERIPHERAL_ERR;/* Failure report */
            break;
          }
        }
        else
        {
          /* Good use case */
        }
      }
    }
    else
    {
      pm_log_message("IMA Polling Failed before write \r\n");
    }
    /* Make sure FG_MEM_IF is available i.e. RIF_MEM_ACCESS_REQ = 0 */
    PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);
  }while((bExceptionOccured == TRUE ) && (++nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT));

  return (err_flg | Status);
}




/**
PmicFgSram_ImaReadSingle()

@brief
Reads 4 bytes (uint32) in given Sram address using IMA handshake
*/
pm_err_flag_type 
PmicFgSram_ImaReadSingle(uint32 PmicDeviceIndex, uint16 ReadAddress, uint32 *fg_memif_data, boolean *bBeatCountMatch)
{
  pm_err_flag_type  Status            = PM_ERR_FLAG_SUCCESS;
  pm_err_flag_type  err_flg           = PM_ERR_FLAG_SUCCESS;
  boolean           bMemAvailable     = FALSE;
  boolean           bExceptionOccured = FALSE;
  uint8             FgBeatCount1      = 0, FgBeatCount2 = 0;
  uint32            nExcpCnt          = 0;

  if(!bBeatCountMatch || !fg_memif_data || ReadAddress > FG_SRAM_END_ADDR)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  *bBeatCountMatch = FALSE;

  /* Request FG_MEM_IF access i.e. RIF_MEM_ACCESS_REQ = 1 */
  Status = PmicFgSram_RequestFgSramAccess(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ);

  /* Select IMA access type  */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

  /* Request Burst access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);

  /* Requet read access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_EN, FG_MEMORY_ACCESS_INTF_CTL_READ_ACCESS);
  do
  {
    bMemAvailable = FALSE;

    /* Poll for IMA readiness  */
    Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
    if(Status != PM_ERR_FLAG_SUCCESS)
    {
      pm_log_message("ERROR: ImaRead Polling Failed = %d", Status);
    }

    if(TRUE == bMemAvailable)
    {
      /* Enable all byte read */
      err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

      err_flg |= pm_fg_mem_if_get_fg_beat_count(PmicDeviceIndex, &FgBeatCount1);

      /* Write Read Location Address */
      err_flg |=  pm_fg_mem_if_write_addr(PmicDeviceIndex, ReadAddress);

      bMemAvailable = FALSE; /* Reset Var */

      /* Poll for IMA readiness  */
      Status |= PmicFgSram_PollImaReady(PmicDeviceIndex, &bMemAvailable);
      if(Status != PM_ERR_FLAG_SUCCESS)
      {
        pm_log_message("ImaRead Polling Failed = %d ", Status);
      }

      Status |= PmicFgSram_HandleIfImaException(PmicDeviceIndex, &bExceptionOccured);
      if(TRUE == bExceptionOccured)
      {
        pm_log_message("ImaException Occured \n\r");
        if(nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT)
        {
          pm_log_message("IMAException Retry to Read Again");
          /* 3 trials before giving up burst read  use continue with 3 static count and update write addr */
          pm_log_message("IMAException Retry ReadAddr = %d, nExcpCnt = %d", ReadAddress, nExcpCnt);
          continue;
        }
        else
        {
          pm_log_message("IMAException Retry Exceeded nExcpCnt = %d ", nExcpCnt);
          err_flg = PM_ERR_FLAG_PERIPHERAL_ERR;
          break;
        }
      }

      if(TRUE == bMemAvailable)
      {
        /* Read Data */
        err_flg |=  pm_fg_mem_if_read_data_reg(PmicDeviceIndex, fg_memif_data);
        err_flg |= pm_fg_mem_if_get_fg_beat_count(PmicDeviceIndex, &FgBeatCount2);
        /*Check Integrity of data by exception status */
      }
      else
      {
         pm_log_message("IMA Polling Failed updating read address");
      }
    }
    else
    {
      pm_log_message("IMA Polling Failed for read action");
    }
  }while((bExceptionOccured == TRUE ) && (++nExcpCnt < FG_SRAM_IMA_EXCP_ERR_HANDLING_MAX_CNT));

  Status |= PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex,  &bExceptionOccured);

  /* Update FG beat count for data read inegrity typically if beat count reads are not same then Redo the read again */
  *bBeatCountMatch = (FgBeatCount1 == FgBeatCount2) ? TRUE : FALSE;

  return (err_flg | Status);
}


/**
PmicFgSram_RequestFgSramAccess()

@brief
Request Sram access, Sets memory access bit 
*/
pm_err_flag_type 
PmicFgSram_RequestFgSramAccess(uint32  PmicDeviceIndex, pm_fg_mem_if_mem_intf_cfg  mem_intf_cfg)
{
  pm_err_flag_type  Status  = PM_ERR_FLAG_SUCCESS;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;

  /*Enables RIF memory interface and the RIF Memory Access Mode.  1 */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, mem_intf_cfg, FG_MEMORY_ACCESS_REQ_TRUE);

  //Set Sram module internal state 
  Status = PmicFgSram_SetState(FG_SRAM_STATUS_IN_USE);

  return (Status |= err_flg);
}




/**
PmicFgSram_ReleaseFgSramImaAccess()

@brief
Release Sram access, Clears memory access bit 
*/
pm_err_flag_type 
PmicFgSram_ReleaseFgSramImaAccess (uint32  PmicDeviceIndex, boolean *bExceptionOccured)
{
  pm_err_flag_type  Status  = PM_ERR_FLAG_SUCCESS;  
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;

  if(!bExceptionOccured)
    return PM_ERR_FLAG_INVALID_PARAMETER;

  /* After write completion clear access bit request i.e. RIF_MEM_ACCESS_REQ = 0 */
  err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_MEM_ACCESS_REQ, FG_MEMORY_ACCESS_REQ_FALSE);
  //err_flg |= pm_fg_mem_if_set_mem_intf_cfg(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CFG_IACS_SLCT, FG_MEMORY_ACCESS_SLCT_IMA);

  /* Handle if IMA exception  */
  Status |= PmicFgSram_HandleIfImaException(PmicDeviceIndex, bExceptionOccured);

  /* Make sure to enable All byte write again in case previosuly APIs changed */
  err_flg |= pm_fg_mem_if_set_ima_byte_en_cfg(PmicDeviceIndex, PM_FG_MEMIF_IMA_BYTE_EN_ALL, TRUE);

  /* Request Single access */
  err_flg |=  pm_fg_mem_if_set_mem_intf_ctl(PmicDeviceIndex, PM_FG_MEMIF_MEM_INTF_CTL_WR_BURST, FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE);

  /* Set Sram module internal state */
  Status = PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE);

  return (Status | err_flg );
}





/**
PmicFgSram_HandleIfImaException()

@brief
handle IMA access exception 
*/
pm_err_flag_type 
PmicFgSram_HandleIfImaException(uint32  PmicDeviceIndex, boolean *bExceptionOccured)
{
  pm_err_flag_type       Status   = PM_ERR_FLAG_SUCCESS;
  pm_err_flag_type err_flg  = PM_ERR_FLAG_SUCCESS;
  pm_fg_memif_ima_sts fg_ima_sts ;

  boolean      bMemExceptionRtSts = FALSE;
  boolean      bExceptionSts = FALSE;

  if(!bExceptionOccured)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  DALSYS_memset(&fg_ima_sts, 0, sizeof(pm_fg_memif_ima_sts));
  

  *bExceptionOccured = FALSE;

  /* Handle if IMA exception  */
  err_flg |= pm_fg_mem_if_get_ima_sts(PmicDeviceIndex, &fg_ima_sts);
  err_flg |= pm_fg_mem_if_irq_status(PmicDeviceIndex, PM_FG_MEM_IF_MEM_XCP, PM_IRQ_STATUS_RT, &bMemExceptionRtSts);
  if(err_flg != PM_ERR_FLAG_SUCCESS)
  {
    pm_log_message("Error = %d bMemExceptRtSts = %d ", err_flg, bMemExceptionRtSts );
  }

  bExceptionSts = fg_ima_sts.excep_sts.iacs_err | fg_ima_sts.excep_sts.xct_type_err | fg_ima_sts.excep_sts.be_burstwr_warn |
                  fg_ima_sts.excep_sts.data_rd_err | fg_ima_sts.excep_sts.data_wr_err | fg_ima_sts.excep_sts.addr_burst_wrap | fg_ima_sts.excep_sts.addr_stable_err;

  if((TRUE == bExceptionSts) || (TRUE == bMemExceptionRtSts))
  {
    /*
    pm_log_message("IMA Exception Occurred bMemExceptionRtSts = %d bExceptionSts = %d Resetting IMA \n\r",__FUNCTION__, 
                bMemExceptionRtSts, bExceptionSts));
    pm_log_message("iacs_err = %d , xct_type_err = %d , be_burstwr_warn = %d , data_rd_err = %d , data_wr_err = %d , addr_burst_wrap = %d , addr_stable_err = %d \n\r",
               __FUNCTION__,fg_ima_sts.excep_sts.iacs_err, fg_ima_sts.excep_sts.xct_type_err,fg_ima_sts.excep_sts.be_burstwr_warn, fg_ima_sts.excep_sts.data_rd_err,
               fg_ima_sts->excep_sts.data_wr_err, fg_ima_sts->excep_sts.addr_burst_wrap, fg_ima_sts.excep_sts.addr_stable_err));
    */    
    *bExceptionOccured = TRUE;
    Status |= PmicFgSram_ResetIma(PmicDeviceIndex);
  }
  else
  {
    
  }

  return Status;

}


/**
PmicFgSram_SetState()

@brief
Set Sram State
*/
pm_err_flag_type 
PmicFgSram_SetState(FgSramState_type FgSramSt)
{
  pm_err_flag_type  Status  = PM_ERR_FLAG_SUCCESS;
  if ( FG_SRAM_STATUS_INVALID == SramState)
  {
    Status = PM_ERR_FLAG_PERIPHERAL_ERR; 
    //pm_log_message("PmicDxe: PmicFgSram_SetState: Error SramState = (%d) \n\r", SramState));
  }
  else{
    SramState = FgSramSt;
    //PmicFgSram_PrintState(FgSramSt);
    //PmicFgSram_PrintState(SramState);
  }

  return Status;
}


/**
PmicFgSram_ResetIma()

@brief
Reset IMA sequence for clearing IMA exception status
*/
pm_err_flag_type PmicFgSram_ResetIma(uint32 PmicDeviceIndex)
{
  pm_err_flag_type err_flg = PM_ERR_FLAG_SUCCESS;
  pm_fg_memif_ima_sts fg_ima_sts;
  uint32              nPauseDelay = 0;
  boolean             bExceptionSts = FALSE;
  UINT8               fg_memif_data = 0;

  pm_log_message("Trigger FG IMA Reset");

  /* Set AICS Clear */
  err_flg  = pm_fg_mem_if_set_ima_cfg_value(PmicDeviceIndex, 0x14);

  err_flg |= pm_fg_mem_if_set_mem_intf_ctl_value(PmicDeviceIndex, 0x00);

  err_flg |= pm_fg_mem_if_set_mem_intf_cfg_value(PmicDeviceIndex, 0xA0);

  /* Pause for 40 us*/
  pm_busy_wait(FG_SRAM_PAUSE_DELAY);

  do
  {

    fg_memif_data = 0;
    err_flg |= pm_fg_mem_if_write_addr_msb(PmicDeviceIndex, 0x00);
    err_flg |= pm_fg_mem_if_write_data3(PmicDeviceIndex, fg_memif_data);
    err_flg |= pm_fg_mem_if_read_data_reg3(PmicDeviceIndex, &fg_memif_data);

    /* Pause for 40 us*/
    pm_busy_wait(FG_SRAM_PAUSE_DELAY);
    err_flg |= pm_fg_mem_if_get_ima_sts(PmicDeviceIndex, &fg_ima_sts);
    nPauseDelay += FG_SRAM_PAUSE_DELAY;
    if(nPauseDelay > FG_SRAM_PAUSE_DELAY_MAX_TIME)
    {
      pm_log_message("IMA Reset.Timeout nPauseDelay ");
      err_flg = PM_ERR_FLAG_PERIPHERAL_ERR;
      break;
    }

    bExceptionSts = fg_ima_sts.excep_sts.iacs_err | fg_ima_sts.excep_sts.xct_type_err | fg_ima_sts.excep_sts.be_burstwr_warn |
                    fg_ima_sts.excep_sts.data_rd_err | fg_ima_sts.excep_sts.data_wr_err | fg_ima_sts.excep_sts.addr_burst_wrap | fg_ima_sts.excep_sts.addr_stable_err;

    fg_memif_data = 0;

    err_flg |= pm_fg_mem_if_get_ima_hw_sts(PmicDeviceIndex, &fg_memif_data);

  }while((err_flg == PM_ERR_FLAG_SUCCESS ) && ((bExceptionSts == TRUE ) || (fg_memif_data != 0x00)));

  /* Make sure to have Read access */
  err_flg  = pm_fg_mem_if_set_ima_cfg_value(PmicDeviceIndex, 0x10);

  /* Make sure to have Single access */
  err_flg |= pm_fg_mem_if_set_mem_intf_ctl_value(PmicDeviceIndex, 0x00);

  /* Pause for 40 us*/
  pm_busy_wait(FG_SRAM_PAUSE_DELAY);

  pm_log_message("Trigger FG IMA Reset.Completed");

  return err_flg;
}


pm_err_flag_type 
PmicFgSram_PollImaReady(uint32 PmicDeviceIndex, boolean *bImaAccessAvailable)
{
  pm_err_flag_type    Status   = PM_ERR_FLAG_SUCCESS;
  pm_err_flag_type    err_flg  = PM_ERR_FLAG_SUCCESS;
  pm_fg_memif_ima_sts fg_ima_sts;
  uint32              Ttl_spent_time_in_polling = 0;
  boolean             Mem_Available_status = FALSE;
  //boolean             bExceptionOccured = FALSE;

  if(NULL == bImaAccessAvailable)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  *bImaAccessAvailable = FALSE;

  //Set Sram module internal state 
  PmicFgSram_SetState(FG_SRAM_STATUS_POLLING);

  //Poll FG_MEM_AVAIL_RT_STS = 1 
  do
  {
      /* mem available best time is 30us */
      err_flg  = pm_fg_mem_if_get_ima_sts(PmicDeviceIndex, &fg_ima_sts);
      err_flg |= pm_fg_mem_if_irq_status(PmicDeviceIndex, PM_FG_MEM_IF_IMA_RDY, PM_IRQ_STATUS_RT, &Mem_Available_status);
      if(err_flg != PM_ERR_FLAG_SUCCESS)
      {
        //pm_log_message("PmicDxe: %a pm_fg_mem_if_get_ima_sts Error= %d \n\r", __FUNCTION__, err_flg));
        break;
      }
      if ((TRUE == fg_ima_sts.opr_sts.iacs_rdy) || (TRUE == Mem_Available_status))
      {
        *bImaAccessAvailable = TRUE;
        //PMIC_FILE_DEBUG("PmicDxe: %a PM_FG_MEM_IF_IMA_RDY \n\r", __FUNCTION__));
        break;
      }//check for error condition as we do not want to loop forever
      else if (Ttl_spent_time_in_polling >= FG_MEM_AVAILABLE_RT_STS_POLL_MAX_TIME)
      {
        Status = PM_ERR_FLAG_PERIPHERAL_ERR;
        pm_log_message("TimeOut: Ttl_spent_time_in_polling = %d ms ", Ttl_spent_time_in_polling);
        break;
      }

      /*wait for 30 us before querying mem available status again */
      pm_busy_wait(FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME); 

      Ttl_spent_time_in_polling += FG_MEM_AVAILABLE_RT_STS_POLL_MIN_TIME;
  }while(TRUE);

  //Set Sram module internal state 
  if (FALSE == *bImaAccessAvailable)
  {
    //Clear memory access bit request i.e. RIF_MEM_ACCESS_REQ = 0
    //PmicFgSram_ReleaseFgSramImaAccess(PmicDeviceIndex, &bExceptionOccured);

    //might need to read cycle streach bit and clear it here
    PmicFgSram_SetState(FG_SRAM_STATUS_AVAILABLE); //when time out for polling access request 
  }
  else 
  {
    PmicFgSram_SetState(FG_SRAM_STATUS_IN_USE);
  }

  return (Status |= err_flg);
}
 
 
 /** 
 pm_fg_sram_verify_content() 
   
 @brief 
 Verify Given SRAM content 
 */ 
 pm_err_flag_type pm_fg_sram_verify_content(uint32 pmic_index, boolean *unexpected_match) 
 { 
   pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS; 
   uint32     fg_unused_data = 0; 
   
   if(!unexpected_match)
   {	   
     return PM_ERR_FLAG_INVALID_PARAMETER; 
   }
   
   *unexpected_match = FALSE; 
   
   /* Read given address */ 
   err_flag |= PmicFgSram_ImaRead(pmic_index, FG_SRAM_SP_UN_USED_ADDR, &fg_unused_data); 
   if(err_flag != PM_ERR_FLAG__SUCCESS) 
   { 
     return err_flag; 
   }
   
   /* If read and unexpected value match then device needs to do dvdd reset to re cover SRAM trim and other contents  
      Ideally this content need to be compared with OTM trim values to be sure and current OTP read API is not available 
      verifying address 0 is all 0s’ and verifying 20 is non zero */ 
   if(fg_unused_data != FG_SRAM_SP_UN_USED_ZERO)
   { 
     *unexpected_match = TRUE;   
   } 
   
   return err_flag; 
 } 
