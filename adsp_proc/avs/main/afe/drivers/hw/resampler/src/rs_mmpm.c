/*========================================================================
  ELite

  This file contains an example service belonging to the ELite framework.

  Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //source/qcom/qct/multimedia2/Audio/drivers/hw/resampler/rs_driver/src/rs_mmpm.c#1

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  05/02/12     fz      Created file.

  ========================================================================== */

#include "qurt_elite.h"
#include "adsp_error_codes.h"
#include "rs_driver_internal.h"

extern struct_driver_ctx rs_driver_global_ctx;

#include "hwd_mmpm.h"
#include "HAL_rs.h"
#include "rs_mmpm.h"

#define VOTE_MIPS_FOR_TEST
#undef VOTE_MIPS_FOR_TEST

#define RS_SLEEP_LATENCY_IN_MICRO_SEC            2
#define RS_CORE_CLK_MAX_REQ                      1


ADSPResult rs_mmpm_register(void)
{
   ADSPResult       status = ADSP_EOK;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

   if(0 != (ptr_driver_ctx->mmpm_info.client_id = HwdMmpm_Register(MMPM_CORE_ID_LPASS_HWRSMP, MMPM_CORE_INSTANCE_0)))
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM Registration done for HW Resampler\n");
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MMPM_Register for HW Resampler failed\n");
      status = ADSP_EFAILED;
   }

   return status;
}

ADSPResult rs_mmpm_deregister(void)
{
   ADSPResult       status = ADSP_EFAILED;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

   if(ADSP_EOK == (status = HwdMmpm_Deregister(ptr_driver_ctx->mmpm_info.client_id)))
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM Deregistration for HW Resampler done\n");
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MMPM_Deregister for HW Resampler failed\n");
   }

   ptr_driver_ctx->mmpm_info.client_id = 0;

   return status;
}

ADSPResult rs_mmpm_misc_static_vote(void)  //one time only vote
{
   ADSPResult       status = ADSP_EFAILED;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

   //1. request for power - one time only
   if(ADSP_EOK != (status = HwdMmpm_RequestPwr(ptr_driver_ctx->mmpm_info.client_id)))
   {
   }
   //2. request register programming - one time only
   else if(ADSP_EOK != (status = HwdMmpm_RequestRegProg(ptr_driver_ctx->mmpm_info.client_id, MMPM_REG_PROG_NORM)))
   {
   }
   //3. request latency - one time only
   else if(ADSP_EOK != (status = HwdMmpm_RequestSleepLatency(ptr_driver_ctx->mmpm_info.client_id, RS_SLEEP_LATENCY_IN_MICRO_SEC)))
   {
   }

#if defined(VOTE_MIPS_FOR_TEST)

   //for test only
   HwdMmpm_RequestMips(ptr_driver_ctx->mmpm_info.client_id, 600, 600);

#endif //#if defined(VOTE_MIPS_FOR_TEST)

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM static vote done for HW Resampler\n");

   return status;
}

ADSPResult rs_mmpm_misc_static_devote(void)  //one time only
{
   ADSPResult       status = ADSP_EFAILED;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

#if defined(VOTE_MIPS_FOR_TEST)
   HwdMmpm_ReleaseMips(ptr_driver_ctx->mmpm_info.client_id);  //for test only
#endif //#if defined(VOTE_MIPS_FOR_TEST)

   //1. derequest latency - one time only 
   if(ADSP_EOK != (status = HwdMmpm_ReleaseSleepLatency(ptr_driver_ctx->mmpm_info.client_id)))
   {
   }
   //2. derequest register programming - one time only
   else if(ADSP_EOK != (status = HwdMmpm_ReleaseRegProg(ptr_driver_ctx->mmpm_info.client_id)))
   {
   }
   //3. derequest for power - one time only
   else if(ADSP_EOK != (status = HwdMmpm_ReleasePwr(ptr_driver_ctx->mmpm_info.client_id)))
   {
   }

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM static devote done for HW Resampler\n");

   return status;
}

ADSPResult rs_mmpm_misc_dynamic_vote_clks(int16 job_id)
{
   ADSPResult       status = ADSP_EOK;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   MmpmClkIdLpassType clk_id[RS_CORE_CLK_MAX_REQ];
   uint32 clk_freq[RS_CORE_CLK_MAX_REQ];
   int32_t num_Clk = 0;

   //Vote for bw
   //Set the AHB clock basing on "bw". "bw_percent" will allow others to share the bus under this AHB clk.
   HwdBwReqType bw[] = 
   { 
      {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE, ptr_driver_ctx->mmpm_info.bw, ptr_driver_ctx->mmpm_info.bw_percent, MMPM_BW_USAGE_LPASS_DMA},
   };

   //Vote for core clks
   clk_id[num_Clk] = MMPM_CLK_ID_LPASS_HWRSP_CORE;
   clk_freq[num_Clk++] = ptr_driver_ctx->mmpm_info.core_clk;

   if(ADSP_EOK != (status = HwdMmpm_RequestBw(ptr_driver_ctx->mmpm_info.client_id, &bw[0], 1)))
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM request AHB clocks for HW Resampler failed\n");
   }
   else if(ADSP_EOK != (status = HwdMmpm_RequestClk(ptr_driver_ctx->mmpm_info.client_id, clk_id, clk_freq, num_Clk)))
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM request core clocks for HW Resampler failed\n");
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HW Resampler Vote for BW  job_id [%d], bw [MSB %lu  LSB %lu] percent [%lu]",job_id, (uint32_t)(ptr_driver_ctx->mmpm_info.bw >> 32), (uint32_t)ptr_driver_ctx->mmpm_info.bw, ptr_driver_ctx->mmpm_info.bw_percent);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HW Resampler Vote for CORE CLOCK  job_id %d, core_clk %d",job_id, ptr_driver_ctx->mmpm_info.core_clk);


   return status;
}

ADSPResult rs_mmpm_misc_dynamic_devote_clks(int16 job_id)
{
   ADSPResult       status = ADSP_EOK;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   MmpmClkIdLpassType clk_id[RS_CORE_CLK_MAX_REQ];
   int32 num_Clk = 0;

   clk_id[num_Clk++] = MMPM_CLK_ID_LPASS_HWRSP_CORE;
   //Fully release BW, MIPS, and CORE CLK for HW resampler
   if(ADSP_EOK != (status = HwdMmpm_ReleaseClk(ptr_driver_ctx->mmpm_info.client_id, clk_id, num_Clk)))
   {
   }
   else if(ADSP_EOK != (status = HwdMmpm_ReleaseBw(ptr_driver_ctx->mmpm_info.client_id)))
   {
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HW Resampler DeVote for BW  job_id [%d], bw [MSB %lu  LSB %lu] percent [%lu]",job_id, (uint32_t)(ptr_driver_ctx->mmpm_info.bw >> 32), (uint32_t)ptr_driver_ctx->mmpm_info.bw, ptr_driver_ctx->mmpm_info.bw_percent);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HW Resampler DeVote for CORE CLOCK  job_id %d, core_clk %d",job_id, ptr_driver_ctx->mmpm_info.core_clk);

   return status;
}


ADSPResult rs_mmpm_enable_core_clks(uint32 core_clk)
{
   ADSPResult       status = ADSP_EOK;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   MmpmClkIdLpassType clk_id[RS_CORE_CLK_MAX_REQ];
   uint32 clk_freq[RS_CORE_CLK_MAX_REQ];
   int32_t num_Clk = 0;

   //Vote for core clks
   clk_id[num_Clk] = MMPM_CLK_ID_LPASS_HWRSP_CORE;
   clk_freq[num_Clk++] = core_clk;

   if(ADSP_EOK != (status = HwdMmpm_RequestClk(ptr_driver_ctx->mmpm_info.client_id, clk_id, clk_freq, num_Clk)))
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "MMPM request core clocks for HW Resampler failed\n");
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HW Resampler Enable CORE CLOCK for regsiter cfg core_clk %d", core_clk);

   return status;
}

ADSPResult rs_mmpm_disable_core_clks(void)
{
   ADSPResult       status = ADSP_EOK;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   MmpmClkIdLpassType clk_id[RS_CORE_CLK_MAX_REQ];
   int32 num_Clk = 0;

   clk_id[num_Clk++] = MMPM_CLK_ID_LPASS_HWRSP_CORE;
   //Fully release BW, MIPS, and CORE CLK for HW resampler
   if(ADSP_EOK != (status = HwdMmpm_ReleaseClk(ptr_driver_ctx->mmpm_info.client_id, clk_id, num_Clk)))
   {
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HW Resampler Dsiable CORE CLOCK for regsiter cfg");

   return status;
}

