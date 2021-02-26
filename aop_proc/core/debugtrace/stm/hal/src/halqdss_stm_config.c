/*=========================================================================

                       STM HAL Configuration

GENERAL DESCRIPTION
   This file implements HAL functionalities for the STM hardware configuration
block.




EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


      Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*==========================================================================
 $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/debugtrace/stm/hal/src/halqdss_stm_config.c#1 $
==========================================================================*/

/*============================================================================
                          INCLUDE FILES
============================================================================*/
#include "halqdss_stm_config.h"
#include "hwioqdss_stm_config.h"


static volatile unsigned int QDSS_STM_BASE_PTR;


/*FUNCTION HAL_qdss_stm_HalConfigInit*/
void HAL_qdss_stm_HalConfigInit(uint32 nBaseAddr)
{
   QDSS_STM_BASE_PTR = nBaseAddr;
}


/*FUNCTION HAL_qdss_stm_SetTimeStampFreq*/
void  HAL_qdss_stm_SetTimeStampFreq(uint32 nFreq)
{
   HWIO_OUTF(QDSS_STMTSFREQR,FREQ,nFreq);
}


/*FUNCTION HAL_qdss_stm_AccessUnLock*/
void  HAL_qdss_stm_AccessUnLock(void)
{
   HWIO_OUT(QDSS_STMLAR,(0xC5ACCE55<<HWIO_SHFT(QDSS_STMLAR,ACCESS_W)) &
               (HWIO_FMSK(QDSS_STMLAR,ACCESS_W)));
}

/*FUNCTION HAL_qdss_stm_AccessLock*/
void  HAL_qdss_stm_AccessLock(void)
{
   HWIO_OUT(QDSS_STMLAR,((~0xC5ACCE55)<<HWIO_SHFT(QDSS_STMLAR,ACCESS_W))&
               (HWIO_FMSK(QDSS_STMLAR,ACCESS_W)));
}

boolean HAL_qdss_stm_AuthStatusNSNID(void)
{
   uint32 nsnid_status;
   #define ENABLED 0x3
   #define DISABLED 0x2
   
   nsnid_status = HWIO_INF(QDSS_STMAUTHSTATUS,NSNID);
   return ((nsnid_status==ENABLED)?TRUE:FALSE);
}
