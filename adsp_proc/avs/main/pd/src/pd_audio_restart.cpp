/*=============================================================================
  @file pd_audio_restart.c

  This file contains audio PD restart code.

 *******************************************************************************
 * Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary.
 ********************************************************************************/


/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
 11/04/2015   YW   File Creation
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "pd_audio_restart.h"
#include "servreg_monitor.h"
#include "servreg_locator.h"

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
static SERVREG_MON_HANDLE audioServregHandle = NULL;
#define AUDIO_MAX_SERVREG_CHAR_LENGTH 16
/*----------------------------------------------------------------------------
 *  Function Implementations
 * -------------------------------------------------------------------------*/

ADSPResult audio_pd_restart_register(void)
{
	ADSPResult ret = ADSP_EOK;
	SERVREG_NAME domain;
	char svcName[AUDIO_MAX_SERVREG_CHAR_LENGTH] = "avs/audio";
	domain = servreg_get_local_domain();
	audioServregHandle = servreg_alloc_monitor_handle(domain,svcName);
	//Set register handle to high if allocation is successful.
	//In case of any previous setup failures, registry will not happen.
	if(audioServregHandle)
	{
		SERVREG_RESULT servreg_ret = SERVREG_FAILURE;
		servreg_ret = servreg_set_state(audioServregHandle, SERVREG_SERVICE_STATE_UP);
		if(SERVREG_SUCCESS != servreg_ret)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PD service Registry Failed with Result %d",servreg_ret);
			servreg_free_monitor_handle(audioServregHandle);
			audioServregHandle = SERVREG_NULL;
			ret = ADSP_EFAILED;
		}
	}
	return ret;
}

ADSPResult audio_pd_restart_deregister(void)
{
	ADSPResult ret = ADSP_EOK;
	if(NULL != audioServregHandle)
	{
		SERVREG_RESULT servreg_ret = SERVREG_FAILURE;
		servreg_ret = servreg_set_state(audioServregHandle, SERVREG_SERVICE_STATE_DOWN);
		if(SERVREG_SUCCESS != servreg_ret)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PD service pull down Failed with Result %d",servreg_ret);
			ret = ADSP_EFAILED;
		}
		//Regardless of the return value, in framework stop we have to free the handles
		servreg_ret = servreg_free_monitor_handle(audioServregHandle);
		if(SERVREG_SUCCESS != servreg_ret)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PD service DeRegistry Failed with Result %d",servreg_ret);
			ret = ADSP_EFAILED;
		}
	}
	return ret;
}
