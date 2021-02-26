/*=============================================================================

FILE:         hal_qdss_tpdm_mcmb.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2016 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "hwio_qdss_tpdm_mcmb.h"
#include "HALhwio.h"


void HAL_tpdm_write_data(uint32 data)
{
	HWIO_OUT(QDSS_SWAO_TPDM0_TPDM_CMB_MSR50,data);
}

void HAL_tpdm_AccessUnLock(void)
{
   HWIO_OUT(QDSS_SWAO_TPDM0_TPDM_LAR,0xC5ACCE55);   
}


void HAL_tpdm_AccessLock(void)
{
   HWIO_OUT(QDSS_SWAO_TPDM0_TPDM_LAR,(~0xC5ACCE55));   
}

boolean HAL_tpdm_IsAOPTraceEnabled(void)
{
   if ( 0x680000 == ( (HWIO_IN(QDSS_SWAO_TPDM0_TPDM_CMB_MSR33)) & 0xff0000)) {
      return TRUE;
   }
   return FALSE;
}

