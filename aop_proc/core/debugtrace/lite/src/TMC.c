#include "halqdss_etfetb.c"   
#include "qdss_tmc_config.h"

#define QDSS_SWAO_ETB   1
#define QDSS_CX_ETB     2

#define QDSS_TMC_MODE_CBUF   0
#define QDSS_TMC_MODE_HWFIFO 2



boolean Is_CX_ETB_MODE_HWFIFO(void)
{
   HAL_qdss_etfetb_HalConfigInit(QDSS_ETFETB_BASE_PHYS);

   if ((HAL_qdss_etfetb_IsEnabled()) &&
       (QDSS_TMC_MODE_HWFIFO==HAL_qdss_etfetb_GetMode())) {
      return TRUE;
   }
   return FALSE;
}

int ETBSwitchMode(uint32 base_phys,int new_mode)
{
   uint32 ffcr_saved;
   int current_mode = -1;
   
   HAL_qdss_etfetb_HalConfigInit(base_phys);

   if (HAL_qdss_etfetb_IsEnabled()) {
      current_mode=HAL_qdss_etfetb_GetMode();

      if (new_mode == current_mode) {
         return current_mode;
      }
   
      //stop and flush
      HAL_qdss_etfetb_AccessUnLock();
      ffcr_saved = HAL_qdss_etfetb_GetFFCR();
      HAL_qdss_etfetb_StopOnFlush(1);
      HAL_qdss_etfetb_Flush();
      HAL_qdss_etfetb_FlushWait();
      HAL_qdss_etfetb_WaitForTMCReadyVal(1);

      //if switching from CBUF mode, drain ETB
      if (QDSS_TMC_MODE_CBUF==current_mode) {
         HAL_qdss_etfetb_Drain();
         HAL_qdss_etfetb_WaitForTMCReadyVal(1);
      }
   
      //switch mode
      HAL_qdss_etfetb_DisableTraceCapture();
      HAL_qdss_etfetb_SetFFCR(ffcr_saved);
      HAL_qdss_etfetb_SetMode(new_mode);

      //Renable
      HAL_qdss_etfetb_EnableTraceCapture();
      HAL_qdss_etfetb_WaitForTMCReadyVal(0);
      HAL_qdss_etfetb_AccessLock();

   }
   return current_mode;
}


