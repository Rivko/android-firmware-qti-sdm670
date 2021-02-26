#include "halqdss_tpda.h"
#include "qdss_tpda_regs.h"



volatile uint32 QDSS_TPDA_TPDA_TPDA_S8_W64_D8_M64_CSF9D1CB23_REG_BASE;

void HAL_qdss_tpda_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_TPDA_TPDA_TPDA_S8_W64_D8_M64_CSF9D1CB23_REG_BASE = nBaseAddr;
}

void  HAL_qdss_tpda_AccessUnLock(void)
{
   HWIO_OUT(QDSS_TPDA_TPDA_LOCKACCESS,0xC5ACCE55);   
}

void  HAL_qdss_tpda_AccessLock(void)
{ 
   HWIO_OUT(QDSS_TPDA_TPDA_LOCKACCESS,(~0xC5ACCE55));   
}

void  HAL_qdss_tpda_SetTimeStampFreq(uint32 nFreq)
{
   HWIO_OUT(QDSS_TPDA_TPDA_FREQREQ_VAL,nFreq);
}


void HAL_qdss_tpda_freq_pkt_req(void)
{
   HWIO_OUTF(QDSS_TPDA_TPDA_CR,FREQREQ,1);
   return;
}
