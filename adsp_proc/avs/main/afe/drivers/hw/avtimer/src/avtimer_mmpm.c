/*========================================================================
  ELite

  This file contains an example service belonging to the ELite framework.

  Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //source/qcom/qct/multimedia2/Audio/drivers/hw/avtimer/avtimer_driver/rel/avs2.0/src/avtimer_mmpm.c#1

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  05/02/12     fz      Created file.

  ========================================================================== */

#include "qurt_elite.h"
#include "adsp_error_codes.h"
#include "avtimer_mmpm.h"
#include "avtimer.h"
#include "aud_sync_hal.h"
#include "avtimer_internal.h"
#include "hwd_mmpm.h"

//1. AVTIMER  -  clock branch: LPASS_AUDIO_CORE_AVSYNC_AVTIMER_XO_CBCR,   ADSPPM ID: MMPM_CLK_ID_LPASS_AVTIMER_CORE
//2. ATIME1    -  clock branch: LPASS_AUDIO_CORE_AVSYNC_ATIME_XO_CBCR,    ADSPPM ID: MMPM_CLK_ID_LPASS_ATIME_CORE
//3. ATIME2    -  clock branch: LPASS_AUDIO_CORE_AVSYNC_ATIME2_XO_CBCR,   ADSPPM ID: MMPM_CLK_ID_LPASS_ATIME2_CORE   -- 8092 only
//4. BT       -  clock branch: LPASS_AUDIO_CORE_AVSYNC_XO_CBCR,           ADSPPM ID: MMPM_CLK_ID_LPASS_AVSYNC_XO
//               clock branch: LPASS_AUDIO_CORE_AVSYNC_BT_XO_CBCR,        ADSPPM ID: MMPM_CLK_ID_LPASS_AVSYNC_BT
//5. FM       -  clock branch: LPASS_AUDIO_CORE_AVSYNC_XO_CBCR,           ADSPPM ID: MMPM_CLK_ID_LPASS_AVSYNC_XO
//               clock branch: LPASS_AUDIO_CORE_AVSYNC_FM_XO_CBCR,        ADSPPM ID: MMPM_CLK_ID_LPASS_AVSYNC_FM
//
//Note: LPASS_AUDIO_CORE_AVSYNC_XO_CBCR is shared between BT and FM so the max clk required is 6
#define AVSYNC_MAX_REQ   8

//static HwdCoreClkInfoType mmpmAvsyncXoClkValType = {MMPM_CLK_ID_LPASS_AVSYNC_XO,19200000,MMPM_FREQ_AT_LEAST};
//static HwdCoreClkInfoType mmpmAvsyncBtClkValType = {MMPM_CLK_ID_LPASS_AVSYNC_BT,19200000,MMPM_FREQ_AT_LEAST};
//static HwdCoreClkInfoType mmpmAvsyncFmClkValType = {MMPM_CLK_ID_LPASS_AVSYNC_FM,19200000,MMPM_FREQ_AT_LEAST};
//static HwdCoreClkInfoType mmpmAvtimeCoreClkValType = {MMPM_CLK_ID_LPASS_AVTIMER_CORE,19200000,MMPM_FREQ_AT_LEAST};
//static HwdCoreClkInfoType mmpmAtime1CoreClkValType = {MMPM_CLK_ID_LPASS_ATIME_CORE,28224000,MMPM_FREQ_AT_LEAST};  //The clk source is LPAQ6_PLL as default for ATIME 
//static HwdCoreClkInfoType mmpmAtime2CoreClkValType = {MMPM_CLK_ID_LPASS_ATIME2_CORE,28224000,MMPM_FREQ_AT_LEAST};  

typedef struct
{
   uint32 clientId;
}avtimer_mmpm_info_t;

avtimer_mmpm_info_t avtimer_mmpm_info;

extern avtimer_drv_ctx_t avtimer_driver_global_ctx;


ADSPResult avtimer_mmpm_register(void)
{
   ADSPResult       status = ADSP_EOK;

   if(0 != (avtimer_mmpm_info.clientId = HwdMmpm_Register(MMPM_CORE_ID_LPASS_AVSYNC, MMPM_CORE_INSTANCE_0)))
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM Registration done for AVTIMER\n");
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MMPM_Register for AVTIMER failed\n");
      status = ADSP_EFAILED;
   }

   return status;
}

ADSPResult avtimer_mmpm_deregister(void)
{
   ADSPResult       status = ADSP_EFAILED;

   if(ADSP_EOK == (status = HwdMmpm_Deregister(avtimer_mmpm_info.clientId)))
   {
      avtimer_mmpm_info.clientId = 0;

      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM Deregistration for AVTIMER done\n");
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MMPM_Deregister for AVTIMER failed\n");
   }

   return status;
}

ADSPResult avtimer_mmpm_voting(uint32 vote_type)
{
   ADSPResult       status = ADSP_EFAILED;
   MmpmClkIdLpassType clkId[AVSYNC_MAX_REQ];
   uint32 clkFreq[AVSYNC_MAX_REQ];           
   int32_t numClk=0;
   int32_t voteSum = 0;
   int32_t i;
   mmpm_info_t *pMmpmInfo = &avtimer_driver_global_ctx.mmpm_info;

   for(i = AVTIMER_HW_INSTANCE_INDEX_0; i < AVTIMER_HW_INSTANCE_INDEX_MAX; i++)
   {
      voteSum += pMmpmInfo->avtimer_job_cnt[i];
   }

   voteSum = (voteSum + pMmpmInfo->avsync_bt_job_cnt + pMmpmInfo->avsync_fm_job_cnt);

   if(1 == voteSum)
   {
      if(ADSP_EOK != (status = HwdMmpm_RequestPwr(avtimer_mmpm_info.clientId)))
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM request power for AVTIMER failed\n");
      }
      else if(ADSP_EOK != (status = HwdMmpm_RequestRegProg(avtimer_mmpm_info.clientId, MMPM_REG_PROG_NORM)))
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM request reg prog for AVTIMER failed\n");
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM request reg power and reg prog for AVTIMER successful \n");
      }
      //do we need to vote for Q6 sleep latency here?
   }
   else
   {
      status = ADSP_EOK;
   }

   if(ADSP_EOK == status)
   {
      switch(avtimer_driver_global_ctx.avt_prop.hw_revision)
      {
         default:
         {
            //for AVTIMER v1
            //This clock request is required even for register programming.
            //so that it is added all the time without any condition checking.
            clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_XO;
            clkFreq[numClk++] = 19200000;

            if(0 != pMmpmInfo->avsync_bt_job_cnt) 
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_BT;
               clkFreq[numClk++] = 19200000;
            }

            if(0 != pMmpmInfo->avsync_fm_job_cnt) 
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_FM;
               clkFreq[numClk++] = 19200000;
            }
         }
         break;
         case AVTIMER_HW_VERSION_2:
         case AVTIMER_HW_VERSION_2_1:
         {
            //for AVTIMER v2
            for(i = AVTIMER_HW_INSTANCE_INDEX_0; i < AVTIMER_HW_INSTANCE_INDEX_MAX; i++)
            {
               if(0 != pMmpmInfo->avtimer_job_cnt[i])
               {
                  if(AVTIMER_HW_INSTANCE_INDEX_0 == i)
                  {
                     //AVTIMER
                     clkId[numClk] = MMPM_CLK_ID_LPASS_AVTIMER_CORE;
                     clkFreq[numClk++] = 19200000;
                  }

                  if(AVTIMER_HW_INSTANCE_INDEX_1 == i)
                  {
                     //ATIME1
                     clkId[numClk] = MMPM_CLK_ID_LPASS_ATIME_CORE;
                     clkFreq[numClk++] = 28224000;
                  }

                  if(AVTIMER_HW_INSTANCE_INDEX_2 == i)
                  {
                     //ATIME2
                     clkId[numClk] = MMPM_CLK_ID_LPASS_ATIME2_CORE;
                     clkFreq[numClk++] = 28224000;
                  }
               }
            }
           
            if(0 != pMmpmInfo->avsync_bt_job_cnt) 
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_XO;
               clkFreq[numClk++] = 19200000;
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_BT;
               clkFreq[numClk++] = 19200000;
            }

            if(0 != pMmpmInfo->avsync_fm_job_cnt) 
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_XO;
               clkFreq[numClk++] = 19200000;
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_FM;
               clkFreq[numClk++] = 19200000;
            }

         }
         break;
      }

      //vote for AVTimer core clks
      if(ADSP_EOK != (status = HwdMmpm_RequestClk(avtimer_mmpm_info.clientId, clkId, clkFreq, numClk)))
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM request core clocks for AVTIMER failed\n");
      }
      else
      {
         int i;

         for(i = 0; i < numClk; i++)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM request core clocks for AVTIMER clkId = %d!\n",clkId[i]);
         }
      }
   }

   return status;
}


ADSPResult avtimer_mmpm_devoting(uint32 vote_type)
{
   ADSPResult       status = ADSP_EFAILED;

   MmpmClkIdLpassType clkId[AVSYNC_MAX_REQ];
   uint32 clkFreq[AVSYNC_MAX_REQ]; 
   int32_t numClk=0;
   int32_t voteSum = 0;
   mmpm_info_t *pMmpmInfo = &avtimer_driver_global_ctx.mmpm_info ;
   int32_t i;

   for(i = AVTIMER_HW_INSTANCE_INDEX_0; i < AVTIMER_HW_INSTANCE_INDEX_MAX; i++)
   {
      voteSum += pMmpmInfo->avtimer_job_cnt[i];
   }

   voteSum = (voteSum + pMmpmInfo->avsync_bt_job_cnt + pMmpmInfo->avsync_fm_job_cnt);

   if(0 == voteSum)
   {
      clkId[numClk++] = MMPM_CLK_ID_LPASS_AVSYNC_XO;
      clkId[numClk++] = MMPM_CLK_ID_LPASS_AVSYNC_BT;
      clkId[numClk++] = MMPM_CLK_ID_LPASS_AVSYNC_FM;

      switch(avtimer_driver_global_ctx.avt_prop.hw_revision)
      {
         default:
         break;
         case AVTIMER_HW_VERSION_2:
         {
            //for AVTIMER v2
            clkId[numClk++] = MMPM_CLK_ID_LPASS_AVTIMER_CORE;
            clkId[numClk++] = MMPM_CLK_ID_LPASS_ATIME_CORE;
         }
         break;
         case AVTIMER_HW_VERSION_2_1:
         {
            clkId[numClk++] = MMPM_CLK_ID_LPASS_AVTIMER_CORE;
            clkId[numClk++] = MMPM_CLK_ID_LPASS_ATIME_CORE;
            clkId[numClk++] = MMPM_CLK_ID_LPASS_ATIME2_CORE;
         }
         break;
      }

      if(ADSP_EOK != (status = HwdMmpm_ReleaseClk(avtimer_mmpm_info.clientId, clkId,numClk)))
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM release all core clock fors AVTIMER failed\n");
      }
      else if(ADSP_EOK != (status = HwdMmpm_ReleaseRegProg(avtimer_mmpm_info.clientId)))
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM release reg prog for AVTIMER failed\n");
      }
      else if(ADSP_EOK != (status = HwdMmpm_ReleasePwr(avtimer_mmpm_info.clientId)))
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM release power for AVTIMER failed\n");
      }

      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM release all clks, reg power and reg prog for AVTIMER successful \n");
   }
   else
   {
      switch(avtimer_driver_global_ctx.avt_prop.hw_revision)
      {
         default:
         {
            //This clock request is required even for register programming.
            //so that it is added all the time without any condition checking.
            clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_XO;
            clkFreq[numClk++] = 19200000;

            if(0 != pMmpmInfo->avsync_bt_job_cnt)
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_BT;
               clkFreq[numClk++] = 19200000;
            }

            if(0 != pMmpmInfo->avsync_fm_job_cnt)
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_FM;
               clkFreq[numClk++] = 19200000;
            }
         }
         break;
         case AVTIMER_HW_VERSION_2:
         case AVTIMER_HW_VERSION_2_1:
         {
            //for AVTIMER v2
            for(i = AVTIMER_HW_INSTANCE_INDEX_0; i < AVTIMER_HW_INSTANCE_INDEX_MAX; i++)
            {
               if(0 != pMmpmInfo->avtimer_job_cnt[i])
               {
                  if(AVTIMER_HW_INSTANCE_INDEX_0 == i)
                  {
                     //AVTIMER
                     clkId[numClk] = MMPM_CLK_ID_LPASS_AVTIMER_CORE;
                     clkFreq[numClk++] = 19200000;
                  }

                  if(AVTIMER_HW_INSTANCE_INDEX_1 == i)
                  {
                     //ATIME1
                     clkId[numClk] = MMPM_CLK_ID_LPASS_ATIME_CORE;
                     clkFreq[numClk++] = 28224000;
                  }

                  if(AVTIMER_HW_INSTANCE_INDEX_2 == i)
                  {
                     //ATIME2
                     clkId[numClk] = MMPM_CLK_ID_LPASS_ATIME2_CORE;
                     clkFreq[numClk++] = 28224000;
                  }
               }
            }
           
            if(0 != pMmpmInfo->avsync_bt_job_cnt) 
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_XO;
               clkFreq[numClk++] = 19200000;
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_BT;
               clkFreq[numClk++] = 19200000;
            }

            if(0 != pMmpmInfo->avsync_fm_job_cnt) 
            {
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_XO;
               clkFreq[numClk++] = 19200000;
               clkId[numClk] = MMPM_CLK_ID_LPASS_AVSYNC_FM;
               clkFreq[numClk++] = 19200000;
            }
         }
         break;
      }

      //vote for AVTimer core clks 
      if(ADSP_EOK != (status = HwdMmpm_RequestClk(avtimer_mmpm_info.clientId, clkId, clkFreq, numClk)))
      {
         MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM release core clocks for AVTIMER failed : rerequest\n");
      }
      else
      {
         int i;
         for(i = 0; i < numClk; i++)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM request core clocks for AVTIMER clkId = %d!\n",clkId[i]);
         }
      }
   }

   return status;
}

