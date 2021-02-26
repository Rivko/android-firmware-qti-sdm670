/*=============================================================================

FILE:         qdss_ssc_csr.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "ssc_qdss_csr_regs.h"
#include "qurt_island.h"
#include "uTimetick.h"
#include "tracer_micro.h"
#include "tracer_event_ids.h"
#include "err.h"



struct qdss_ssc_csr_context {
   uint64 ao_ts_counter_pre;    //pre retention value of AO counter
   uint64 ao_ts_counter_post;   //post retention value of AO counter
   uint64 ts_counter_pre;  //pre retention value of QDSS counter
   uint64 ts_counter_post_computed;
};

struct qdss_ssc_csr_context qdss_ssc_csr; 
volatile uint32 qdss_ssc_csr_base = 0;

//default
float QDSS_TO_AO_RATIO = (12888746.0/19200000.0);


void HAL_qdss_ssc_csr_HalConfigInit(uint32 base_addr)
{
   qdss_ssc_csr_base = base_addr;
}


void HAL_qdss_ssc_tsgen_Enable(void)
{
   HWIO_OUTXF(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTL,TSGEN_DISABLE_CNT,0);
}

void HAL_qdss_ssc_tsgen_Disable(void)
{
   HWIO_OUTXF(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTL,TSGEN_DISABLE_CNT,1);
}


void HAL_qdss_ssc_tsgen_Preload_Enable(void)
{
   HWIO_OUTXF(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTL,TSGEN_PRELOAD_EN,1);
}

void HAL_qdss_ssc_tsgen_Preload_Disable(void)
{
   HWIO_OUTXF(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTL,TSGEN_PRELOAD_EN,0);
}

boolean HAL_qdss_ssc_tsgen_Preload_IsDone(void)
{
   return (0==HWIO_INXF(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTL,TSGEN_PRELOAD_EN));
}

void HAL_qdss_ssc_tsgen_Load(uint64 timeval) 
{
   HWIO_OUTX(qdss_ssc_csr_base, SSC_SSC_TSGEN_PRELOAD_HI,((uint32)(timeval >> 32)));
   HWIO_OUTX(qdss_ssc_csr_base,SSC_SSC_TSGEN_PRELOAD_LO,((uint32)(timeval & 0xffffffff)));
}


__inline__ void HAL_qdss_ssc_csr_TriggerTimestampCapture(void)
{
   HWIO_OUTXF(qdss_ssc_csr_base,SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL,CAPTURE,0);
   HWIO_OUTXF(qdss_ssc_csr_base,SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL,CAPTURE,1);
}


__inline__ uint64 HAL_qdss_ssc_csr_GetAOCounter(void) 
{
   return (((uint64)(HWIO_INX(qdss_ssc_csr_base,SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1)) << 32) |
           HWIO_INX(qdss_ssc_csr_base,SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0));
}


//Newly loaded timestamp takes time to take effect on trace sources
#define QDSS_TS_PROPOGATION_DELAY  50
void busywait (uint32 pause_time_us);

#ifdef QDSS_SLPI_V1
void qdss_ssc_csr_LoadTimestamp(uint64 timestamp)
{
   HAL_qdss_ssc_tsgen_Disable();
   HAL_qdss_ssc_tsgen_Preload_Enable();
   HAL_qdss_ssc_tsgen_Load(timestamp);
   HAL_qdss_ssc_tsgen_Preload_Disable();
   HAL_qdss_ssc_tsgen_Enable();
   busywait(QDSS_TS_PROPOGATION_DELAY);
}
#else

void qdss_ssc_csr_LoadTimestamp(uint64 timestamp)
{
   int wait_count = 0;
   HAL_qdss_ssc_tsgen_Disable();
   HAL_qdss_ssc_tsgen_Load(timestamp);
   HAL_qdss_ssc_tsgen_Preload_Enable();
   while (!HAL_qdss_ssc_tsgen_Preload_IsDone() &&
          (wait_count++ <QDSS_TS_PROPOGATION_DELAY) ) {
      busywait(1);
   }
   HAL_qdss_ssc_tsgen_Enable();
}
#endif


uint64 qdss_ssc_Get_AON_Timeval(void) 
{
   HAL_qdss_ssc_csr_TriggerTimestampCapture();
   return HAL_qdss_ssc_csr_GetAOCounter();
}

static uint64 qdss_gray_to_bin(uint64 num)
{
    uint64 mask;
    for (mask = num >> 1; mask != 0; mask = mask >> 1)
    {
        num = num ^ mask;
    }
    return num;
}

uint64 qdss_ssc_Get_QDSS_Timeval(void)
{
   uint32 first_hi, second_hi, lo;

   do
      {
         first_hi = HWIO_INX(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTR_HI);
         lo = HWIO_INX(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTR_LO);
         second_hi =HWIO_INX(qdss_ssc_csr_base,SSC_SSC_TSGEN_CTR_HI);
      } while(first_hi != second_hi);

   return qdss_gray_to_bin(((((uint64)second_hi) << 32) | lo));
}

void qdss_save_timestamps(void)
{
   qdss_ssc_csr.ts_counter_pre = qdss_ssc_Get_QDSS_Timeval();
   qdss_ssc_csr.ao_ts_counter_pre = uTimetick_Get();
   UTRACER_EVENT(QDSS_SAVED_TIMESTAMP);
}



void qdss_restore_timestamps(void)
{
   uint64 retention_period;

   qdss_ssc_csr.ao_ts_counter_post = uTimetick_Get();
   retention_period = qdss_ssc_csr.ao_ts_counter_post - qdss_ssc_csr.ao_ts_counter_pre;

   qdss_ssc_csr.ts_counter_post_computed =
      qdss_ssc_csr.ts_counter_pre +
      (retention_period * QDSS_TO_AO_RATIO);
   
   qdss_ssc_csr_LoadTimestamp(qdss_ssc_csr.ts_counter_post_computed);
   busywait(QDSS_TS_PROPOGATION_DELAY);

   UTRACER_EVENT(QDSS_RELOADED_TIMESTAMP,                            
                 ((uint32)qdss_ssc_csr.ao_ts_counter_pre),           
                 ((uint32)(qdss_ssc_csr.ao_ts_counter_pre >> 32)),   
                 ((uint32)qdss_ssc_csr.ao_ts_counter_post),          
                 ((uint32)(qdss_ssc_csr.ao_ts_counter_post >> 32)),  
                 ((uint32)qdss_ssc_csr.ts_counter_pre),              
                 ((uint32)(qdss_ssc_csr.ts_counter_pre >> 32)),      
                 ((uint32)qdss_ssc_csr.ts_counter_post_computed),       
                 ((uint32)(qdss_ssc_csr.ts_counter_post_computed >> 32))
                  );

}

void qdss_ssc_ts_lpi_request(uint8 request)
{
   request = request & 0x1; 

   HWIO_OUTXF(qdss_ssc_csr_base,SSC_SSC_NTS_LOW_POWER_HANDSHAKE,REQUEST,request);
}

#define SSC_LPI_REQUEST_BMSK   0x4
#define SSC_LPI_REQUEST_SHFT   0x2
#define SSC_LPI_ACK_BMSK       0x1

#define LPI_REQUEST_TIMEOUT  10000

typedef struct
{
  uint32 reg_addr;
  uint32 request;
  uint32 count;
}qdss_debug_lpi_info_type;

qdss_debug_lpi_info_type qdss_debug_lpi_info = {0,0,0};

void qdss_ssc_lpi_request(uint32 reg_addr, uint32 request, boolean timeout_flag) 
{
   int wait_count = 0;

   request = (request & 0x1);

   out_dword_masked_ns(reg_addr,
                       SSC_LPI_REQUEST_BMSK,
                       (request <<SSC_LPI_REQUEST_SHFT),
                       in_dword(reg_addr));

   //Wait for ack with a timeout
   while ((((in_dword(reg_addr)) & SSC_LPI_ACK_BMSK) != request) && (timeout_flag) &&
          (wait_count++ <LPI_REQUEST_TIMEOUT) ) {
      busywait(1);
   }
   
   if(wait_count > qdss_debug_lpi_info.count)
   {
    qdss_debug_lpi_info.reg_addr = reg_addr;
    qdss_debug_lpi_info.request = request;
    qdss_debug_lpi_info.count = wait_count;
   }
   
   if(wait_count >= LPI_REQUEST_TIMEOUT)
   {
     ERR_FATAL("Ack is not set, timeout count %d reg_addr 0x%x request %d", wait_count, reg_addr, request );
   }
}

boolean qdss_lpm_enabled = 0;
void qdss_lpm_enable(void) 
{
   qdss_lpm_enabled = 1;
}

void qdss_lpm_enter(void) 
{
   if ((qdss_lpm_enabled) && (0 != qdss_ssc_csr_base)) {
     qdss_ssc_lpi_request(SSC_ATB_LOW_POWER_HANDSHAKE_ADDR,0, TRUE);
     qdss_ssc_lpi_request(SSC_NTS_LOW_POWER_HANDSHAKE_ADDR,0, TRUE);
     qdss_ssc_lpi_request(SSC_APB_LOW_POWER_HANDSHAKE_ADDR,0, FALSE);
     qdss_ssc_lpi_request(SSC_DAP_LOW_POWER_HANDSHAKE_ADDR,0, FALSE);
   }
}


void qdss_lpm_exit(void) 
{
   if ((qdss_lpm_enabled) && (0 != qdss_ssc_csr_base)) {
      qdss_ssc_lpi_request(SSC_ATB_LOW_POWER_HANDSHAKE_ADDR,1, TRUE);
      qdss_ssc_lpi_request(SSC_NTS_LOW_POWER_HANDSHAKE_ADDR,1, TRUE);
      qdss_ssc_lpi_request(SSC_APB_LOW_POWER_HANDSHAKE_ADDR,1, FALSE);
      qdss_ssc_lpi_request(SSC_DAP_LOW_POWER_HANDSHAKE_ADDR,1, FALSE);
   }
}

void qdss_ssc_island_enter(void)
{
  if ((qdss_lpm_enabled) && (0 != qdss_ssc_csr_base))
  {  
    qdss_ssc_lpi_request(SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE,0, TRUE);
  }
  
}

void qdss_ssc_island_exit(void)
{
  if ((qdss_lpm_enabled) && (0 != qdss_ssc_csr_base))
  {  
    qdss_ssc_lpi_request(SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE,1, TRUE);
  }  
}
