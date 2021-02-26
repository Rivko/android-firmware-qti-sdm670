/**
@file AdspCoreSvc_Utils.cpp***********************

@brief This file contains the utility implementation for AdspCoreSvc.

*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/23/16   HB		Created file.

==========================================================================*/

/*------------------------------------------------------------------------
   Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.
--------------------------------------------------------------------------*/


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
/// ADSPCoreSvc header
#include "AdspCoreSvc.h"
#include "AdspCoreSvc_InstanceStruct.h"

ADSPResult AdspCoreSvc_Internal_Init()
{
	return ADSP_EOK;
}

void AdspCoreSvc_Internal_DeInit()
{
	return ;
}

int AdspCoreSvcPrivateApr(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   QURT_ELITE_ASSERT(pMsg);

   elite_apr_packet_t *pAprPacket = (elite_apr_packet_t *) pMsg->pPayload;
   AvsCoreSvc_t *pMe = (AvsCoreSvc_t*)pInstance;
   uint32_t cmd = elite_apr_if_get_opcode(pAprPacket);
   switch( cmd )
   {
       default:
       {
		   /* Handle error. */
		   if ( elite_apr_if_msg_type_is_cmd( pAprPacket) )
		   {  /* Complete unsupported commands. */
			   (void)elite_apr_if_end_cmd( pMe->aprHandle, pAprPacket, APR_EUNSUPPORTED );
		   }
		   else
		   {  /* Drop unsupported events. */
			   (void)elite_apr_if_free( pMe->aprHandle, pAprPacket );
		   }
    	   result = ADSP_EUNSUPPORTED;
    	   break;
       }
   }
   return result;
}

