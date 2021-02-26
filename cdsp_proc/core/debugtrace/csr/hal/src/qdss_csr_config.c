/*=============================================================================

FILE:         qdss_csr_config.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "qdss_utils.h"
#include "DALSys.h"
#include "qdss_csr_config.h"
#include "halqdss_csr.h"
#include "qdss_ssc_csr.h"

struct qdss_csr_context {
   DALSYSMemHandle chip_csr_mem_handle;
   DALSYSMemInfo   chip_csr_mem_info;
   DALSYSMemHandle ssc_csr_mem_handle;
   DALSYSMemInfo   ssc_csr_mem_info;
   boolean bInitialized;
   boolean is_timestamp_synced;
};

static struct qdss_csr_context qdss_csr;



DALResult qdss_csr_dal_memmap(uint32 physAddr,
                          uint32 size,
                          uint32 dwAttribs,
                          DALSYSMemHandle *phMem,
                          DALSYSMemInfo *pMemInfo)
{
   int nErr;

   TRY(nErr,DALSYS_MemRegionAlloc(dwAttribs,
                                  DALSYS_MEM_ADDR_NOT_SPECIFIED,
                                  physAddr,
                                  size, phMem, NULL ));
   DALSYS_MemInfo(*phMem, pMemInfo);

   CATCH(nErr) {}
   return nErr;
}



void qdss_csr_timestamp_sync(void) 
{
   qdss_csr.is_timestamp_synced=TRUE;
}


void qdss_csr_timestamp_sync_renable(void) 
{
   qdss_csr.is_timestamp_synced=FALSE;
}


#define SSC_AON_CNTR_FREQ 19200000.0
extern uint32 qdss_get_ssc_tsctr_freq(void);
extern float QDSS_TO_AO_RATIO;


int qdss_csr_init(void) 
{
   int nErr;

   if (qdss_csr.bInitialized) {
      return DAL_SUCCESS;
   }

   TRY(nErr,qdss_csr_dal_memmap(QDSS_QDSSCSR_REG_BASE_PHYS,
                            0x1000,
                            DALSYS_MEM_PROPS_HWIO,
                            &qdss_csr.chip_csr_mem_handle,
                            &qdss_csr.chip_csr_mem_info));
   HAL_qdss_csr_HalConfigInit(qdss_csr.chip_csr_mem_info.VirtualAddr);


   TRY(nErr,qdss_csr_dal_memmap(QDSS_SSC_QDSSCSR_BASE_PHYS,
                            0x10000,
                            DALSYS_MEM_PROPS_HWIO,
                            &qdss_csr.ssc_csr_mem_handle,
                            &qdss_csr.ssc_csr_mem_info));
   HAL_qdss_ssc_csr_HalConfigInit(qdss_csr.ssc_csr_mem_info.VirtualAddr);
   //Reset timestamp sync request bit
   qdss_ssc_ts_lpi_request(0);


   QDSS_TO_AO_RATIO = ((float)qdss_get_ssc_tsctr_freq())/((float) SSC_AON_CNTR_FREQ);

   qdss_csr.is_timestamp_synced = FALSE;
   qdss_csr.bInitialized = TRUE;
   

   CATCH(nErr) {}

   return nErr;
}

