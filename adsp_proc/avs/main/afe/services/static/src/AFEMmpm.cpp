/*========================================================================
 This file contains AFE MMPM driver warapper functions


  Copyright (c) 2009-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
 QUALCOMM Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEMmpm.cpp#3 $
 ====================================================================== */

/*==========================================================================
 Include files
 ========================================================================== */
#include "AFEMmpm_i.h"
#include "AFEInternal.h"
#include "AFESpdifDriver.h"
#include "AFEHdmiOutputDrv.h"
#include "hwd_devcfg.h"
#include "topo_utils.h"

/*==========================================================================
 Define
 ========================================================================== */
//#define AFE_MMPM_LOW_LEVEL_DEBUG

uint32_t g_lpm_ahb_voting_scale_factor;

/*==========================================================================
 Function
 ========================================================================== */
/*
  This is for committing the votes to ADSPPM.

  @param[in]     client_id: This is the client id provided by ADSPPM when the core was registered.
  @param[in]     req_param_ptr: The resource param ptr that contains the req. resources information.
  @param[in]     status_ptr: Pointer to the status returned by ADSPPM.
  @param[in]     num_req: Number of requests bundled in this call.

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_commit_vote(uint32_t client_id, MmpmRscParamType *req_param_ptr,
		MMPM_STATUS *status_ptr, uint32_t num_req)
{

	if(!(req_param_ptr && status_ptr && client_id))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed NULL pointer req_param_ptr: 0x%lx, status_ptr: 0x%lx, invalid client_id: 0x%lx",
				req_param_ptr, status_ptr, client_id);
		return ADSP_EFAILED;
	}

	ADSPResult result = ADSP_EOK;

#ifndef SIM

	MmpmRscExtParamType afe_adsppm_request;

	afe_adsppm_request.numOfReq = num_req;
	afe_adsppm_request.apiType = MMPM_API_TYPE_SYNC; // Currently synchronous requests are planned. Asynchronous will be part of next phase of implementation.
	afe_adsppm_request.reqTag = 0; // This is used for ASYNC API type, this can be ignored for SYNC API
	afe_adsppm_request.pReqArray = req_param_ptr;
	afe_adsppm_request.pStsArray = status_ptr;


	if(MMPM_STATUS_SUCCESS != MMPM_Request_Ext(client_id, &afe_adsppm_request))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"MMPM voting for client_id: 0x%lx failed!", client_id);
		result = ADSP_EFAILED;
	}
#endif //SIM

	return result;
}

/*
  This is for committing the power and register programming for various registered cores.

  @param[in]     core_info_ptr: The pointer to the core information stored during ADSPPM registration.
  @param[in]     port_id: Port id

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_power_reg_request(afe_mmpm_core_info_t *core_info_ptr, uint16_t port_id)
{
	ADSPResult result = ADSP_EOK;
	MmpmRscParamType afe_request[AFE_MMPM_MAX_NUM_CORE];
	MMPM_STATUS afe_request_status[AFE_MMPM_MAX_NUM_CORE];
	uint32 adsppm_req_cntr = 0;

	/* Depending on the flags for power and register access voting of the port,
	 * the corresponding requests are sent to ADSPPM only once.
	 */
	if(!core_info_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed NULL pointer to core_info port_id: 0x%x!",
				port_id);
		return ADSP_EFAILED;
	}

	if(core_info_ptr->power_voting)
	{
#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE is Power voting for core id %lu from port id 0x%x",
				core_info_ptr->client_id, port_id);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG

		afe_request[adsppm_req_cntr].rscId = MMPM_RSC_ID_POWER;
		adsppm_req_cntr++;
		if(core_info_ptr->reg_access_voting)
		{
			afe_request[adsppm_req_cntr].rscId = MMPM_RSC_ID_REG_PROG;
			afe_request[adsppm_req_cntr].rscParam.regProgMatch = MMPM_REG_PROG_NORM;
			adsppm_req_cntr++;
		}

		if(ADSP_FAILED(
				afe_mmpm_commit_vote(core_info_ptr->client_id, &afe_request[0], &afe_request_status[0],
						adsppm_req_cntr)))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"AFE power request for core id %lu from port id 0x%x is failed", core_info_ptr->client_id, port_id);
			return ADSP_EFAILED;
		}
	}

	return result;
}

/*
  This is registering with ADSPPM.

  @param[in]     mmpm_info_pptr: This is the handle to the MMPM structure.
  @param[in]     port_id: Port id

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_register(void **mmpm_info_pptr, uint16_t port_id)
{
	ADSPResult result = ADSP_EOK;
	afe_mmpm_info_t *mmpm_info_ptr = NULL;
	MmpmCoreIdType device_core_id = MMPM_CORE_ID_NONE;
	bool_t is_supported;
	afe_mmpm_core_info_t *core_info_ptr = NULL;
	uint32_t core_idx;
	char client_name[QURT_ELITE_DEFAULT_NAME_LEN];

	if(!mmpm_info_pptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed NULL pointer to MMPM info port_id: 0x%x!",
				port_id);
		return ADSP_EFAILED;
	}

	/* Setting the incoming ptr to NULL */
	*mmpm_info_pptr = NULL;

	/* Allocate memory for MMPM state object */
	if( NULL
			== (mmpm_info_ptr = (afe_mmpm_info_t *)qurt_elite_memory_malloc(sizeof(afe_mmpm_info_t),
					QURT_ELITE_HEAP_DEFAULT)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed to allocate memory for MMPM state object, port_id: 0x%lx",
				port_id);
		return ADSP_ENOMEMORY;
	}

	memset(mmpm_info_ptr, 0, sizeof(afe_mmpm_info_t));

	/* Assign AFE client name.*/
	snprintf(client_name, QURT_ELITE_DEFAULT_NAME_LEN, "Afe[%2x]", port_id);

	/* Find which device interface core needs to be registered */
	if(IS_AUDIO_IF_PORT_AFE_ID(port_id) || IS_AUDIO_IF_TDM_PORT_AFE_ID(port_id))
	{
		device_core_id = MMPM_CORE_ID_LPASS_AIF;
	}
	else if(IS_SPDIF_PORT_AFE_ID(port_id))
	{
		is_supported = HwdDevCfg_HWSupport(HWD_MODULE_TYPE_SPDIF);
		if(FALSE == is_supported)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Failed MMPM HWD_MODULE_TYPE_SPDIF is notsupported, port_id: 0x%lx",
					port_id);
			result = ADSP_EUNSUPPORTED;
			goto __bailout;
		}
		device_core_id=  afe_get_spdif_mmpm_coreid();
	}
	else if(IS_HDMI_OUTPUT_PORT_AFE_ID(port_id) || IS_HDMI_OVER_DP_PORT_AFE_ID(port_id))
	{
		is_supported = HwdDevCfg_HWSupport(HWD_MODULE_TYPE_HDMI_OUTPUT);
		if(FALSE == is_supported)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Failed MMPM HWD_MODULE_TYPE_HDMI_OUTPUT is notsupported, port_id: 0x%lx",
					port_id);
			result = ADSP_EUNSUPPORTED;
			goto __bailout;
		}
		device_core_id = afe_get_hdmitx_mmpm_coreid();
	}

	/* AFE should always be registered as client for LPM, ADSP and either of AIF, SPDIF or HDMI out.*/
	for(core_idx = 0; core_idx < AFE_MMPM_MAX_NUM_CORE; core_idx++)
	{
		core_info_ptr = &(mmpm_info_ptr->core_info[core_idx]);

		// Initializing the client ID to 0.
		core_info_ptr->client_id = 0;

		core_info_ptr->core.rev = MMPM_REVISION;
		core_info_ptr->core.instanceId = MMPM_CORE_INSTANCE_0;
		core_info_ptr->core.MMPM_Callback = NULL;
		core_info_ptr->core.pClientName = client_name;
		core_info_ptr->core.pwrCtrlFlag = PWR_CTRL_NONE;
		core_info_ptr->core.callBackFlag = CALLBACK_NONE;
		core_info_ptr->core.cbFcnStackSize = 0;

		switch(core_idx)
		{
		  case LPASS_CORE:
		  {
		    core_info_ptr->core.coreId = MMPM_CORE_ID_LPASS_CORE;
		    core_info_ptr->power_voting = AFE_MMPM_VOTING_ENABLE;
		    core_info_ptr->reg_access_voting = AFE_MMPM_VOTING_DISABLE;
		    break;
		  }

		  case LPM:
		  {
		    core_info_ptr->core.coreId = MMPM_CORE_ID_LPASS_LPM;
		    core_info_ptr->power_voting = AFE_MMPM_VOTING_ENABLE;
		    core_info_ptr->reg_access_voting = AFE_MMPM_VOTING_ENABLE;
		    break;
		  }
		  case ADSP:
		  {
		    /*Power and Reg voting not needed for ADSP*/
		    core_info_ptr->core.coreId = MMPM_CORE_ID_LPASS_ADSP;
		    core_info_ptr->power_voting = AFE_MMPM_VOTING_DISABLE;
		    core_info_ptr->reg_access_voting = AFE_MMPM_VOTING_DISABLE;
		    break;
		  }
		  case DEVICE:
		  {
		    core_info_ptr->core.coreId = device_core_id;
		    core_info_ptr->power_voting = AFE_MMPM_VOTING_ENABLE;
		    core_info_ptr->reg_access_voting = AFE_MMPM_VOTING_ENABLE;
		    break;
		  }
		}

		if(MMPM_CORE_ID_NONE == core_info_ptr->core.coreId)
		{
			continue;
		}

#ifndef SIM
		core_info_ptr->client_id = MMPM_Register_Ext(&core_info_ptr->core);
#else
		core_info_ptr->client_id = core_idx + 1;
#endif //SIM

		if(0 == core_info_ptr->client_id)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"MMPM_Register_Ext for AFE with core id %lu is failed for port_id: 0x%lx",
					core_info_ptr->core.coreId, port_id);
			result = ADSP_EFAILED;
			goto __bailout;
		}

#ifndef SIM
		/*Register for client class as Audio to ADSPPM. */
		if(ADSP_FAILED(afe_mmpm_register_audio_client_class_and_dcvs_adjust(core_info_ptr->client_id)))
		{
			/*result is not printing as it has taken care in the above function definition*/
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in registering eiterh DCVS UP or Audio client class" );
		}

#endif //SIM

		/* Now do the power request */
		if(ADSP_FAILED(result = afe_mmpm_power_reg_request(core_info_ptr, port_id)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed to request Power and Reg voting for MMPM, portid 0x%x", port_id);
			goto __bailout;
		}

#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AFE is registered with MMPM for core id %lu with Client ID %lu, port_id: 0x%lx",
				core_idx, core_info_ptr->client_id, port_id);
		MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AFE is registered with MMPM for client name: %x %x %x %x %x %x %x %x %x",
				client_name[0],client_name[1],client_name[2],client_name[3],client_name[4],
				client_name[5],client_name[6], client_name[7], client_name[8]);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG
	}

	/* Assign local MMPM handle to port handle.*/
	*mmpm_info_pptr = mmpm_info_ptr;
	return result;

	__bailout:
	qurt_elite_memory_free(mmpm_info_ptr);
	return result;
}

/*
  This is de-registering with ADSPPM.

  @param[in]     mmpm_info_pptr: This is the handle to the MMPM structure.
  @param[in]     port_id: Port id

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_deregister(void **mmpm_info_pptr, uint16_t port_id)
{
	if(!(mmpm_info_pptr && *mmpm_info_pptr))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed NULL MMPM ptr: 0x%lx, port_id: 0x%lx", mmpm_info_pptr, port_id);
		return ADSP_EFAILED;
	}

	ADSPResult result = ADSP_EOK;
	int32 core_idx = 0;
	afe_mmpm_core_info_t *core_info_ptr = NULL;
	afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)*mmpm_info_pptr;

	/* Deregister the cores from LAST core to LPASS_CORE.
	 *
	 * By deregistering LPASS_CORE at last, it will make sure there is power voting
	 * present until all the cores get deregisters. This will be useful for chipsets
	 * where LCC is moved to LPASS core and avoid unclocked access*/
	for(core_idx = AFE_MMPM_MAX_NUM_CORE-1; core_idx >= 0; core_idx--)
	{
		core_info_ptr = &(mmpm_info_ptr->core_info[core_idx]);
		if(0 != core_info_ptr->client_id)
		{
#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
					"AFE is Deregistering with MMPM for core id %lu with Client ID %lu, port_id: 0x%lx",
					core_idx, core_info_ptr->client_id, port_id);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG

#ifndef SIM
			if(MMPM_STATUS_SUCCESS != MMPM_Deregister_Ext(core_info_ptr->client_id))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
						"MMPM_Deregister_Ext for AFE with core id %lu is failed, port_id: 0x%lx",
						core_idx, port_id);
				result |= ADSP_EFAILED;
			}
#else
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"MMPM_Deregister_Ext for AFE with core id %lu, port_id: 0x%lx",
					core_idx, port_id);
#endif //SIM
		}
	}

	qurt_elite_memory_free(mmpm_info_ptr);
	*mmpm_info_pptr = NULL;

	if(ADSP_EOK == result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,
				"AFE deregistered as client for ADSP, AudioIF, SLIMBUS and LPM Successfully for port_id: 0x%lx", port_id);
	}

	return result;
}

/*
  This is for populating the MPPS voting requests. We will iterate through active MMPM topologies and accumulate the
  votes.

  @param[in]     afe_dev_ptr: Port Device ptr
  @param[in]     afe_mpps_req_ptr: Ptr to the ADSPPM mpps req. We will populate it here.

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_populate_mpps_voting(afe_dev_port_t *afe_dev_ptr,
		MmpmMppsReqType *afe_mpps_req_ptr, bool_t *is_voting_populated)
{
	ADSPResult result = ADSP_EOK;
	if((NULL == afe_dev_ptr) || (NULL == afe_mpps_req_ptr) || (NULL == is_voting_populated))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Null pointer sent to AFE MMPM, port ptr: 0x%lx, afe_mpps_req_ptr: 0x%lx is_voting_populated: 0x%lx",
				afe_dev_ptr, afe_mpps_req_ptr, is_voting_populated);
		return ADSP_EFAILED;
	}

	afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)afe_dev_ptr->mmpm_info_ptr;
	uint32_t temp_kpps = 0, topo_idx, num_channels;
	afe_mmpm_topo_e topo;
	*is_voting_populated = FALSE;

	for(topo_idx = 0; topo_idx <= AFE_MMPM_LAST_TOPOLOGY_BIT_POS; topo_idx++)
	{
		/* Get the corresponding Topo and accumulate votes if it is active */
		topo = (afe_mmpm_topo_e)(1 << topo_idx);

		if(mmpm_info_ptr->mmpm_active_topos & topo)
		{
			switch(topo)
			{
			case AFE_MMPM_DTMF_TOPOLOGY:
			{
				mmpm_info_ptr->mmpm_voting.cycles_in_kpps += AFE_DTMF_KPPS;
				break;
			}

			case AFE_MMPM_CLIENT_TOPOLOGY:
			{
				mmpm_info_ptr->mmpm_voting.cycles_in_kpps += mmpm_info_ptr->dyn_vot_info.agr_client_kpps;
				break;
			}

			case AFE_MMPM_LOOPBACK_TOPOLOGY:
			{
				/** 390 kpps is for loopback gain function*/
				mmpm_info_ptr->mmpm_voting.cycles_in_kpps += AFE_LOOPBACK_GAIN_KPPS;
				break;
			}

			case AFE_MMPM_SIDETONE_IIR_TOPOLOGY:
			{
				/*500kpps is for IIR resampler function kpps*/
				mmpm_info_ptr->mmpm_voting.cycles_in_kpps += AFE_SIDETONE_IIR_KPPS;
				break;
			}

			case AFE_MMPM_CUST_TOPOLOGY:
			{
				topo_t *afe_topo_ptr = NULL;

				afe_topo_ptr = (topo_t *)afe_dev_ptr->port_topo_ptr;
				if(NULL != afe_dev_ptr->port_topo_ptr)
				{
					temp_kpps = afe_topo_ptr->kpps;
				}

				mmpm_info_ptr->mmpm_voting.cycles_in_kpps += temp_kpps;
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_MMPM_CUST_TOPOLOGY KPPS request %lu from portid 0x%x", temp_kpps, afe_dev_ptr->intf);
				break;
			}

			/*With dynamic voting change, client related voting is voted separately, so, not over voting for BTFM topology*/
			case AFE_MMPM_BT_FM_TOPOLOGY:
			case AFE_MMPM_DEFAULT_TOPOLOGY:
			{
				/*Initialize temp_kpps variable to zero*/
				temp_kpps = 0;

				/* Rx processing is divided into blocks of processes (client process, DTMF, audio voice mixing, limiter, interleaver, device process and framework).
				 * Since afe client might connect/disconnect at any time, similarly DTMF enable/disable. So, processes like DTMF, audio voice mixing and limiter will
				 * vary dynamically. Hence voting will be done dynamically.
				 * Note: For Tx processing DTMF, audio voice mixing and limiter is not applicable.
				 * Under DEFAULT TOPOLOGY, will be voting only for interleaver, device process and framework.*/

				if(IS_RT_PROXY_PORT_AFE_ID(afe_dev_ptr->intf))
				{
					//increased KPPS for data copy (5ms)
					temp_kpps += AFE_RT_PROXY_KPPS;
				}

				/*Interleaver/Deinterleaver is not needed for slimbus interfaces*/
				if(!IS_SLIMBUS_PORT_AFE_ID(afe_dev_ptr->intf))
				{
					temp_kpps += AFE_INTER_DEINTERLEAVE_KPPS(afe_dev_ptr->channels);
				}

				num_channels = afe_dev_ptr->channels;
				/*For port channels greater than 2, HDMI always performs as 8 channel. */
				if((AFE_PORT_ID_MULTICHAN_HDMI_RX == afe_dev_ptr->intf || IS_HDMI_OUTPUT_PORT_AFE_ID(afe_dev_ptr->intf) || IS_HDMI_OVER_DP_PORT_AFE_ID(afe_dev_ptr->intf)) && afe_dev_ptr->channels > 2)
				{
					num_channels = 8;
				}
				/*Device process kpps is the kpps taken to read/write data from/to DMA buffer*/
				temp_kpps += AFE_DEVICE_PROCESS_KPPS(afe_dev_ptr->sample_rate, num_channels);

				/*framework kpps is the difference between (overall kpps across render thread) and (sum of individual processes kpps)*/
				temp_kpps += AFE_FWK_KPPS;

				mmpm_info_ptr->mmpm_voting.cycles_in_kpps += temp_kpps;
				break;
			}
			case AFE_MMPM_GROUP_TOPOLOGY:
			{
				afe_group_device_state_struct_t *p_group_device = NULL;

				p_group_device = afe_dev_ptr->p_group_device_state;

				/* this voting is to be safe and it will be changed after target profiling */
				if(NULL != p_group_device)
				{
					temp_kpps = AFE_GROUP_DEVICE_KPPS(p_group_device->num_channels); //10 MCPS per channel for processing mux/demux..
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "group device, AFE port 0x%x  voting kpps %lu and temp kpps %lu", afe_dev_ptr->intf,\
							mmpm_info_ptr->mmpm_voting.cycles_in_kpps, temp_kpps);
					mmpm_info_ptr->mmpm_voting.cycles_in_kpps += temp_kpps;
				}
				else
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail:incorrect topology voting which is not group device, AFE port 0x%x", afe_dev_ptr->intf);
				}
				break;
			}

			case AFE_MMPM_ENC_FWK_TOPOLOGY:
			case AFE_MMPM_DEC_FWK_TOPOLOGY:
			{
				//enc/decoding use case
				if(NULL != afe_dev_ptr->port_enc_dec_ptr)
				{
					//initialize
					temp_kpps = 0;

					//add baseline fwk KPPS for memcpy into and out of enc/dec
					temp_kpps += AFE_ENC_DEC_FWK_KPPS;

					//add de-interleaving KPPS (for non-i2s)
					if(FALSE == afe_dev_ptr->is_interleaved)
					{
						temp_kpps += AFE_INTER_DEINTERLEAVE_KPPS(afe_dev_ptr->dev_channels);
					}

					//update vote
					mmpm_info_ptr->mmpm_voting.cycles_in_kpps += temp_kpps;

					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port 0x%x  voting enc/decoding kpps %lu, total kpps %lu",
							afe_dev_ptr->intf, temp_kpps, mmpm_info_ptr->mmpm_voting.cycles_in_kpps);
				}
				break;
			}
			case AFE_MMPM_MAX_TOPOLOGY:
			{
				break;
			}
			}
		}
	}
#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE MPPS Request before fudging:: kppsPerThread %lu",\
			mmpm_info_ptr->mmpm_voting.cycles_in_kpps);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG

	if(0 != mmpm_info_ptr->mmpm_voting.cycles_in_kpps)
	{
		/*kpps is converted into mpps*/
		afe_mpps_req_ptr->mppsTotal = ceil_div(mmpm_info_ptr->mmpm_voting.cycles_in_kpps, 1000);
		/*adspFloorclock field is not required for AFE. This will be used by voice clients for RTD. So assigning it zero*/
		afe_mpps_req_ptr->adspFloorClock = 0;
		*is_voting_populated = TRUE;

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE MPPS Request:: mppsPerThread %lu, kppsPerThread %lu",\
				afe_mpps_req_ptr->mppsTotal, mmpm_info_ptr->mmpm_voting.cycles_in_kpps);
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "AFE MPPS Request:: Invalid MPPS %lu not populating into voting structure ",\
				mmpm_info_ptr->mmpm_voting.cycles_in_kpps);
	}

	return result;
}

/*
  This is for populating the BW voting requests. We will iterate through active MMPM topologies and accumulate the
  votes.

  @param[in]     afe_dev_ptr: Port Device ptr
  @param[in]     afe_bw_req_ptr: Ptr to the ADSPPM BW req. We will populate it here.

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_populate_bw_voting(afe_dev_port_t *afe_dev_ptr,
                                              MmpmGenBwReqType *afe_bw_req_ptr, MmpmGenBwReqType *afe_bw_req_lpass_core_ptr,
                                              bool_t *is_voting_populated, bool_t *is_lpass_core_voting_populated)
{
  ADSPResult result = ADSP_EOK;
  if((NULL == afe_dev_ptr) || (NULL == afe_bw_req_ptr) || (NULL == is_voting_populated)
      || (NULL == is_lpass_core_voting_populated) || (NULL == afe_bw_req_lpass_core_ptr) )
  {
    MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Null pointer sent to AFE MMPM, port ptr: 0x%lx, afe_request_ptr: 0x%lx "
          "is_voting_populated 0x%lx, is_lpass_core_voting_populated 0x%lx, afe_bw_req_lpass_core_ptr: 0x%lx",
          afe_dev_ptr, afe_bw_req_ptr, is_voting_populated, is_lpass_core_voting_populated, afe_bw_req_lpass_core_ptr);
    return ADSP_EFAILED;
  }

  afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)afe_dev_ptr->mmpm_info_ptr;
  afe_mmpm_voting_t *voting_ptr = &mmpm_info_ptr->mmpm_voting;

  *is_voting_populated = FALSE;
  *is_lpass_core_voting_populated = FALSE;

  uint32_t common_bw_factor = 0;
  uint32_t bw_req_ctr = 0, bw_req_lpass_core_ctr = 0;
  uint32_t temp_ebi_to_qdsp6ss_bw = 0;

  afe_mmpm_topo_e topo;
  uint32_t topo_idx;

  for(topo_idx = 0; topo_idx <=AFE_MMPM_LAST_TOPOLOGY_BIT_POS; topo_idx++)
  {
    /* Get the corresponding Topo and accumulate votes if it is active */
    topo = (afe_mmpm_topo_e)(1 << topo_idx);

    if(mmpm_info_ptr->mmpm_active_topos & topo)
    {
      switch(topo)
      {
        case AFE_MMPM_LOOPBACK_TOPOLOGY:
        case AFE_MMPM_SIDETONE_IIR_TOPOLOGY:
        case AFE_MMPM_MAX_TOPOLOGY:
        case AFE_MMPM_DTMF_TOPOLOGY:
        {
          break;
        }

        case AFE_MMPM_CUST_TOPOLOGY:
        {
          topo_t *afe_topo_ptr = NULL;

          afe_topo_ptr = (topo_t*)afe_dev_ptr->port_topo_ptr;
          if(NULL != afe_dev_ptr->port_topo_ptr)
          {
            temp_ebi_to_qdsp6ss_bw = afe_topo_ptr->bps;
          }

          voting_ptr->ebi_to_qdsp6ss_bw += temp_ebi_to_qdsp6ss_bw;

          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                "AFE_MMPM_CUST_TOPOLOGY ebi_to_qdsp6ss_bw %ld from portid 0x%x",
                temp_ebi_to_qdsp6ss_bw, afe_dev_ptr->intf);
        }
        break;

        case AFE_MMPM_CLIENT_TOPOLOGY:
        {
          voting_ptr->ebi_to_qdsp6ss_bw += mmpm_info_ptr->dyn_vot_info.agr_client_bw;
        }
        break;
        case AFE_MMPM_GROUP_TOPOLOGY:
        {

          afe_group_device_state_struct_t       *p_group_device = NULL;

          p_group_device = afe_dev_ptr->p_group_device_state;

          /* this voting is to be safe and it will be changed after target profiling */
          if (NULL != p_group_device)
          {
            common_bw_factor =
                (p_group_device->num_channels
                    * p_group_device->bytes_per_channel
                    * p_group_device->sample_rate);

            voting_ptr->ebi_to_qdsp6ss_bw += ((common_bw_factor + 500) * (3 + 3*2*2 + 1))>>1;
            voting_ptr->adsp_to_lpm_bw += (common_bw_factor) * (3*2*2+1);
            voting_ptr->lpm_to_aif_bw +=  (common_bw_factor) * (1);
            voting_ptr->ddr_to_aif_bw +=   (common_bw_factor) * (1);

            MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "group device, AFE port 0x%x  ebi=%lu,adsp_lpm=%lu,lpmaif=%lu,ddraif=%lu", afe_dev_ptr->intf,
                  voting_ptr->ebi_to_qdsp6ss_bw,voting_ptr->adsp_to_lpm_bw,  voting_ptr->lpm_to_aif_bw, voting_ptr->ddr_to_aif_bw);


          }
          else
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail:incorrect topology voting which is not group device, AFE port 0x%x", afe_dev_ptr->intf);
          }
          break;
        }
        case AFE_MMPM_ENC_FWK_TOPOLOGY:
        case AFE_MMPM_DEC_FWK_TOPOLOGY:
        {
          //enc/decoding use case
          if(NULL != afe_dev_ptr->port_enc_dec_ptr)
          {
            //initialize
            temp_ebi_to_qdsp6ss_bw = 0;

            //add baseline fwk BW
            temp_ebi_to_qdsp6ss_bw += AFE_ENC_DEC_FWK_BW_BPS;

            //update vote
            voting_ptr->ebi_to_qdsp6ss_bw += temp_ebi_to_qdsp6ss_bw;

            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port 0x%x  enc/dec ebi_to_qdsp6ss_bw Bps %lu, total Bps %lu",
                  afe_dev_ptr->intf, temp_ebi_to_qdsp6ss_bw, voting_ptr->ebi_to_qdsp6ss_bw);
          }
          break;
        }

        case AFE_MMPM_BT_FM_TOPOLOGY:
        case AFE_MMPM_DEFAULT_TOPOLOGY:

          // calculate the common bw factor (in Bytes per second) per one AFE frame
          common_bw_factor    = afe_port_get_common_bw_factor(afe_dev_ptr);

          if((IS_AUDIO_IF_PORT_AFE_ID(afe_dev_ptr->intf))
              || (IS_AUDIO_IF_TDM_PORT_AFE_ID(afe_dev_ptr->intf)))
          {
            // dma buffer to DMA channel transfer
            voting_ptr->lpm_to_aif_bw += (common_bw_factor) * (1);
            voting_ptr->ddr_to_aif_bw += (common_bw_factor) * (1);
          }

          if(IS_SPDIF_PORT_AFE_ID(afe_dev_ptr->intf))
          {
            //TODO:Honest DDR BW voting changes will be updated later
            voting_ptr->ddr_to_spdif_bw += (common_bw_factor) * (1);
          }
          else if((IS_SLIMBUS_PORT_AFE_ID(afe_dev_ptr->intf)))
          {
            // dma buffer to DMA channel transfer
            voting_ptr->lpm_to_sb_bw += (common_bw_factor) * (1);
          }
          else if(IS_HDMI_OUTPUT_PORT_AFE_ID(afe_dev_ptr->intf) || IS_HDMI_OVER_DP_PORT_AFE_ID(afe_dev_ptr->intf))
          {
            //TODO: Have a way to tell where DMA buffers were allocated
            //TODO: Honest BW voting changes, will be updated later
            voting_ptr->hdmitx_to_lpm_bw += (common_bw_factor) * (1);
            voting_ptr->hdmitx_to_ddr_bw += (common_bw_factor) * (1);
          }

          /*From profiling per port BW is 1MBPS (when port at 48k, 16bit, stereo). This implies 0.5MBPS (for port at 48k, 16bit, mono).
           * Considered BW value @ 48k, 16 bit mono channel as base for deriving BW scaling factors(BW values referred, in below explanation, is for
           * port @48k, 16bit mono channel).
           * For safe side, finalised 0.7MBPS. This use case does not account  resampler BW.
           * Band width is divided into DMA read/wrtie BW, Client framework BW  and client resampler BW (if resampler is needed)
           *0.7 MBPS is divided into 0.5MBPS for client framework BW and 0.2MBPS for DMA read/write BW. In addition to these, assumed resampler BW as 0.2 MBPS
           *
           *In summary, total BW for port @48k, 16bit mono channel is 0.9 MBPS = 0.2MBPS(DMA read/write) + ( 0.5MBPS(client fwk)+0.2MBPS(client resampler) )
           *Under CLIENT_TOPOLOGY, client_fwk and client_resampler bw will be voted. Under DEFAULT_TOPOLOGY, DMA read/wrtie BW will be voted.
           *
           *       DMA_bw  = scaling_factor*common_bw_factor
           *       200000  = scaling_factor*(48000*2*1)           => scaling factor as 25/12
           * */

          // data read from EBI and handled through AFE port
          // This BW is for read\write to DMA buf from EBI0
          voting_ptr->ebi_to_qdsp6ss_bw += (25*common_bw_factor)/12;

          // determine LPM to LPM BW usage.
          // Here we are considering LPM memory read or write by QDSP6 CPU

          /*
		     Applying fudge factor to allocate 10% of AFE processing time for accessing LPM.
			 AHB voting scaling factor is for considering cache flush operation in case of Cached access to LPM
           */
          voting_ptr->adsp_to_lpm_bw +=  ((common_bw_factor) * g_lpm_ahb_voting_scale_factor * 10);

          // This is to make sure in the HDMI case, we vote for max AXI bandwidth to hide device latency issue
          if(AFE_PORT_ID_MULTICHAN_HDMI_RX == afe_dev_ptr->intf)
          {
            voting_ptr->lpm_to_aif_bw += (common_bw_factor) * (1);
            voting_ptr->ddr_to_aif_bw += (common_bw_factor) * (1);
          }

          if(IS_HDMI_OUTPUT_PORT_AFE_ID(afe_dev_ptr->intf) || IS_HDMI_OVER_DP_PORT_AFE_ID(afe_dev_ptr->intf))
          {
            voting_ptr->lpm_to_aif_bw += (common_bw_factor) * (1);
            voting_ptr->ddr_to_aif_bw += (common_bw_factor) * (1);
          }

          break;
      }
    }
  }

  // ebi_to_qdsp6ss_bw
  if(0 != voting_ptr->ebi_to_qdsp6ss_bw)
  {
    afe_bw_req_ptr->pBandWidthArray[bw_req_ctr].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    afe_bw_req_ptr->pBandWidthArray[bw_req_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
    afe_bw_req_ptr->pBandWidthArray[bw_req_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->ebi_to_qdsp6ss_bw;
    afe_bw_req_ptr->pBandWidthArray[bw_req_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_ptr->pBandWidthArray[bw_req_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
    bw_req_ctr++;
  }

  // adsp_to_lpm_bw
  if(0 != voting_ptr->adsp_to_lpm_bw)
  {
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_LPM_SLAVE;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->adsp_to_lpm_bw;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
    bw_req_lpass_core_ctr++;
  }

  // lpm_to_aif_bw
  if(0 != voting_ptr->lpm_to_aif_bw)
  {
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.masterPort = MMPM_BW_PORT_ID_AIF_MASTER;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_LPM_SLAVE;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->lpm_to_aif_bw;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DMA;
    bw_req_lpass_core_ctr++;
  }

  // lpm_to_sb_bw
  if(0 != voting_ptr->lpm_to_sb_bw)
  {
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.masterPort = MMPM_BW_PORT_ID_SLIMBUS_MASTER;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_LPM_SLAVE;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->lpm_to_sb_bw;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DMA;
    bw_req_lpass_core_ctr++;
  }

  // ddr_to_aif_bw
  if(0 != voting_ptr->ddr_to_aif_bw)
  {
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.masterPort = MMPM_BW_PORT_ID_AIF_MASTER;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->ddr_to_aif_bw;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DMA;
    bw_req_lpass_core_ctr++;
  }

  // ddr_to_sdpif_bw
  if(0 != voting_ptr->ddr_to_spdif_bw)
  {
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.masterPort = MMPM_BW_PORT_ID_SPDIF_MASTER;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->ddr_to_spdif_bw;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DMA;
    bw_req_lpass_core_ctr++;
  }

  // hdmitx to lpm
  if(0 != voting_ptr->hdmitx_to_lpm_bw)
  {
    /*function is calling to get hdmitx only, becasue these id is not added in Bear Family MMPM code*/
    /* For compiling AVS code with bear MMPM code function is exposed */
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.masterPort = afe_get_hdmitx_mmpm_bwportid();
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_LPM_SLAVE;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->hdmitx_to_lpm_bw;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DMA;
    bw_req_lpass_core_ctr++;
  }

  // hdmirx to ddr
  if(0 != voting_ptr->hdmitx_to_ddr_bw)
  {

    /*function is calling to get hdmitx only, becasue these id is not added in Bear Family MMPM code*/
    /* For compiling AVS code with bear MMPM code function is exposed */
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.masterPort = afe_get_hdmitx_mmpm_bwportid();
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.bwBytePerSec = voting_ptr->hdmitx_to_ddr_bw;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usagePercentage = 100;
    afe_bw_req_lpass_core_ptr->pBandWidthArray[bw_req_lpass_core_ctr].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DMA;
    bw_req_lpass_core_ctr++;
  }

  if(0 != bw_req_ctr || 0 != bw_req_lpass_core_ctr)
  {
    if(0 != bw_req_ctr)
    {
      afe_bw_req_ptr->numOfBw = bw_req_ctr;
      *is_voting_populated = TRUE;
    }

    if(0 != bw_req_lpass_core_ctr)
    {
      afe_bw_req_lpass_core_ptr->numOfBw = bw_req_lpass_core_ctr;
      *is_lpass_core_voting_populated = TRUE;
    }

#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE BW Request:: ebi_to_qdsp6ss_bw %lu, adsp_to_lpm_bw %lu",
          voting_ptr->ebi_to_qdsp6ss_bw,voting_ptr->adsp_to_lpm_bw);
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE BW Request:: lpm_to_aif_bw %lu, lpm_to_sb_bw %lu, ddr_to_aif_bw %lu",
          voting_ptr->lpm_to_aif_bw,voting_ptr->lpm_to_sb_bw,voting_ptr->ddr_to_aif_bw);
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE BW Request:: hdmitx_to_lpm_bw %lu, hdmitx_to_ddr_bw %lu",
          voting_ptr->hdmitx_to_lpm_bw,voting_ptr->hdmitx_to_ddr_bw);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE BW Request:: ddr_to_spdif_bw %lu ",
          voting_ptr->ddr_to_spdif_bw);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG
  }
  else
  {
    MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO,"AFE BW Request:: zero bw requests - bw_req_ctr %lu, bw_req_lpass_core_ctr %lu. Not populating to final vote",
          bw_req_ctr, bw_req_lpass_core_ctr);
  }
  return result;
}

/*
  This is for populating the Sleep latency requests.

  @param[in]     afe_dev_ptr: Port Device ptr
  @param[in]     sleep_latency_ptr: It will return the required sleep latency.

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_populate_sleep_latency_voting(afe_dev_port_t *afe_dev_ptr, uint32_t *sleep_latency_us_ptr,
		bool_t *is_voting_populated)
{
	ADSPResult result = ADSP_EOK;
	if((NULL == afe_dev_ptr) || (NULL == sleep_latency_us_ptr) || (NULL == is_voting_populated))
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Null pointer sent to AFE MMPM, port ptr: 0x%lx, sleep_latency_us_ptr: 0x%lx is_voting_populated 0x%lx",
				afe_dev_ptr, sleep_latency_us_ptr, is_voting_populated);
		return ADSP_EFAILED;
	}

	afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)afe_dev_ptr->mmpm_info_ptr;
	uint32_t sleep_latency_us = AFE_SLEEP_LATENCY_DEFAULT;
	*is_voting_populated = FALSE;

	/*HW MAD is special case where we are not opening device immediately when the port start command comes. We
	 *will open the device when HW MAD interrupt comes. So we are not voting for sleep latency now. If we vote now
	 *same for sw duty cycle case also.
	 * For HWMAD and SW Duty Cyclying, we would vote AFE_SLEEP_LATENCY_DEFAULT.
	 */
	if((NULL == afe_dev_ptr->hw_mad_ptr) && (NULL == afe_dev_ptr->sw_duty_cycle_data_ptr))
	{
		if(NULL == afe_dev_ptr->port_topo_ptr)
		{
			sleep_latency_us = AFE_SLEEP_LATENCY_APCR;
		}
	}
	else
	{
		sleep_latency_us = AFE_SLEEP_LATENCY_MAX;
	}

	// make sure values are in valid range.
	if(sleep_latency_us < AFE_SLEEP_LATENCY_DEFAULT)
	{
		sleep_latency_us = AFE_SLEEP_LATENCY_DEFAULT;
	}
	else if(sleep_latency_us > AFE_SLEEP_LATENCY_MAX)
	{
		sleep_latency_us = AFE_SLEEP_LATENCY_MAX;
	}

	mmpm_info_ptr->mmpm_voting.sleep_latency_us = sleep_latency_us;
	*sleep_latency_us_ptr = sleep_latency_us;
	*is_voting_populated = TRUE;

	return result;
}

/*
  This is a top-level common function to bundle MPPS, BW and Sleep Latency votings. We call respective functions to
  populate resource requests and vote from here.

  @param[in]     afe_dev_ptr: Port Device ptr

  @return
  ADSPResult
 */
static ADSPResult afe_mmpm_mpps_bw_sleep_voting(afe_dev_port_t *afe_dev_ptr)
{
  if(NULL == afe_dev_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Null pointer sent to AFE MMPM, port ptr: 0x%lx",
          afe_dev_ptr);
    return ADSP_EFAILED;
  }

  ADSPResult result = ADSP_EOK;
  afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)afe_dev_ptr->mmpm_info_ptr;

  uint32 adsppm_req_cntr = 0, adsppm_lpass_core_req_cntr = 0;
  uint32_t sleep_latency = 0;

  MmpmRscParamType afe_request[AFE_ADSPPM_REQUEST_MAX], afe_lpass_core_request[AFE_ADSPPM_REQUEST_MAX];
  MmpmMppsReqType afe_mpps_req;
  MmpmGenBwReqType afe_bw_req, afe_lpass_core_bw_req;
  MmpmGenBwValType afe_bw_val[AFE_ADSPPM_BW_REQUEST_MAX], afe_lpass_core_bw_val[AFE_ADSPPM_BW_REQUEST_MAX];
  MMPM_STATUS afe_request_status[AFE_ADSPPM_REQUEST_MAX], afe_lpass_core_request_status[AFE_ADSPPM_REQUEST_MAX];

  bool_t is_voting_populated, is_lpass_core_voting_populated;

  /* All previous votes will be renewed with new votes, so clear old votes */
  memset(&mmpm_info_ptr->mmpm_voting, 0, sizeof(mmpm_info_ptr->mmpm_voting));

  memset(&afe_request[0], 0, sizeof(afe_request));
  memset(&afe_lpass_core_request[0], 0, sizeof(afe_lpass_core_request));
  memset(&afe_mpps_req, 0, sizeof(afe_mpps_req));

  /* Populate MPPS*/
  is_voting_populated = FALSE;
  if(ADSP_EOK != (result = afe_mmpm_populate_mpps_voting(afe_dev_ptr, &afe_mpps_req, &is_voting_populated)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Populating mpps voting failed with result %ld for 0x%x",
          result, afe_dev_ptr->intf);
    return result;
  }
  else
  {
    if(is_voting_populated)
    {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MPPS Voting: %lu mpps for portid 0x%x",
            afe_mpps_req.mppsTotal, afe_dev_ptr->intf);

      afe_request[adsppm_req_cntr].rscId = MMPM_RSC_ID_MPPS;
      afe_request[adsppm_req_cntr].rscParam.pMppsReq = &afe_mpps_req;
      adsppm_req_cntr++;
#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE is requesting for MPPS for portid 0x%x", afe_dev_ptr->intf);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG
    }
  }

  /* Populate BW */
  memset(&afe_bw_req, 0, sizeof(afe_bw_req));
  memset(&afe_bw_val[0], 0, sizeof(afe_bw_val));
  afe_bw_req.pBandWidthArray = &afe_bw_val[0];

  memset(&afe_lpass_core_bw_req, 0, sizeof(afe_lpass_core_bw_req));
  memset(&afe_lpass_core_bw_val[0], 0, sizeof(afe_lpass_core_bw_val));
  afe_lpass_core_bw_req.pBandWidthArray = &afe_lpass_core_bw_val[0];

  is_voting_populated = FALSE;
  is_lpass_core_voting_populated = FALSE;
  if(ADSP_EOK != (result = afe_mmpm_populate_bw_voting(afe_dev_ptr, &afe_bw_req, &afe_lpass_core_bw_req, &is_voting_populated, &is_lpass_core_voting_populated)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Populating bw voting failed with result %ld for 0x%x",
          result, afe_dev_ptr->intf);
    return result;
  }
  else
  {
    if(is_voting_populated || is_lpass_core_voting_populated)
    {
      MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "BW Voting: EQ - %lu Bps, LA - %lu Bps, LS - %lu Bps, LL - %lu Bps, DA - %lu Bps, DS - %lu Bps",
            mmpm_info_ptr->mmpm_voting.ebi_to_qdsp6ss_bw, mmpm_info_ptr->mmpm_voting.lpm_to_aif_bw,
            mmpm_info_ptr->mmpm_voting.lpm_to_sb_bw,mmpm_info_ptr->mmpm_voting.adsp_to_lpm_bw,
            mmpm_info_ptr->mmpm_voting.ddr_to_aif_bw, mmpm_info_ptr->mmpm_voting.ddr_to_spdif_bw);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "BW Voting: HDMITX_MULTI_STREAMS ->LPM BW: %lu, HDMITX_MULTI_STREAMS ->DDR BW: %lu",
            mmpm_info_ptr->mmpm_voting.hdmitx_to_lpm_bw, mmpm_info_ptr->mmpm_voting.hdmitx_to_ddr_bw);

      if(is_voting_populated)
      {
        afe_request[adsppm_req_cntr].rscId = MMPM_RSC_ID_GENERIC_BW_EXT;
        afe_request[adsppm_req_cntr].rscParam.pGenBwReq = &afe_bw_req;
        adsppm_req_cntr++;
      }

      if(is_lpass_core_voting_populated)
      {
        afe_lpass_core_request[adsppm_lpass_core_req_cntr].rscId = MMPM_RSC_ID_GENERIC_BW_EXT;
        afe_lpass_core_request[adsppm_lpass_core_req_cntr].rscParam.pGenBwReq = &afe_lpass_core_bw_req;
        adsppm_lpass_core_req_cntr++;
      }

#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE is requesting for BW for portid 0x%x", afe_dev_ptr->intf);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG
    }
  }

  /* Populate Sleep Latency */
  is_voting_populated = FALSE;
  if(ADSP_EOK != (result = afe_mmpm_populate_sleep_latency_voting(afe_dev_ptr, &sleep_latency, &is_voting_populated)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Populating sleep latency voting failed with result %ld for 0x%x",
          result, afe_dev_ptr->intf);
    return result;
  }
  else
  {
    if(is_voting_populated)
    {
      afe_request[adsppm_req_cntr].rscId = MMPM_RSC_ID_SLEEP_LATENCY;
      afe_request[adsppm_req_cntr].rscParam.sleepMicroSec = sleep_latency;
#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE is making %d sleep latency request ",
            afe_request[adsppm_req_cntr].rscParam.sleepMicroSec);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG
      adsppm_req_cntr++;
    }
  }

  /* Now request MIPS, BW and Sleep Latency bundled together as they are on same core, if req_cnt > 0 */
  if(0 != adsppm_req_cntr )
  {
    if(ADSP_FAILED(
        afe_mmpm_commit_vote(mmpm_info_ptr->core_info[ADSP].client_id, &afe_request[0],
                             &afe_request_status[0], adsppm_req_cntr)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AFE voting for MIPS, BW, sleep latency from portid 0x%x failed", afe_dev_ptr->intf);
      result = ADSP_EFAILED;
    }
  }

  /* All LPASS CORE related BW module votes were voted through LPASS_CORE id  */
  if(0 != adsppm_lpass_core_req_cntr)
  {
    if(ADSP_FAILED(
        afe_mmpm_commit_vote(mmpm_info_ptr->core_info[LPASS_CORE].client_id, &afe_lpass_core_request[0],
                             &afe_lpass_core_request_status[0], adsppm_lpass_core_req_cntr)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AFE voting for MIPS, BW, sleep latency from portid 0x%x, core type:%d failed", afe_dev_ptr->intf, LPASS_CORE);
      result = ADSP_EFAILED;
    }
  }

  return result;
}

/*
 * From AFE MMPM driver, we only expose this function. These are the following functionalities supported:
 * 1) If a Vote comes for the first time, we will create the MMPM info object.
 * 2) If a Vote/Devote comes with vote_now: False, we would just accumulate the votes.
 * 3) If a Vote/Devote comes with vote_now: True, we would accumulate and then vote the final values.
 * 4) We would also vote for Sleep latency as AFE_SLEEP_LATENCY_DEFAULT by default for all the vote_now and bundle the
 *    request to MMPM, in case the value needs to be overridden (for, ex HWMAD, SW Duty Cycle
 *    it can be done by exculsive call to afe_mmpm_vote_sleep_latency()) in order to avoid multiple MMPM calls.
 * 5) If no active topos are there we would deregister the MMPM client.

  @param[in]     ptr: This is the port pointer
  @param[in]     topo: the type of Topology the voting is for
  @param[in]     afe_mmpm_vote_update_ptr: this is the container for old and new votes.
  @param[in]     vote_now: this is for bundling the request to MMPM together. If it is set to true, the updated votings
                           will be committed to MMPM, if false we would just mark "topo" as active.

  @return
  ADSPResult
 */
ADSPResult afe_mmpm_voting(void *ptr, afe_mmpm_topo_e topo, afe_mmpm_vote_e mmpm_vote,
		bool_t vote_now)
{
	afe_dev_port_t *afe_dev_ptr = (afe_dev_port_t *)ptr;

	if(NULL == afe_dev_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Null Dev Port pointer sent to AFE MMPM");
		return ADSP_EFAILED;
	}

	ADSPResult result = ADSP_EOK;
	afe_mmpm_info_t *mmpm_info_ptr = NULL;

#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"afe_mmpm_port_voting with topo %d, mmpm_vote %d, vote_now %d for portid 0x%x",topo, mmpm_vote, vote_now, afe_dev_ptr->intf);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG

	/* Register the MMPM cores if this is first AFE_MMPM_VOTE */
	if(NULL == afe_dev_ptr->mmpm_info_ptr)
	{
		if(AFE_MMPM_VOTE == mmpm_vote)
		{
			if(ADSP_FAILED(result = afe_mmpm_register(&afe_dev_ptr->mmpm_info_ptr, afe_dev_ptr->intf)))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed to register port_id:0x%lx for MMPM", afe_dev_ptr->intf);
				return ADSP_EFAILED;
			}
		}
		else
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed NULL MMPM ptr for devote for port_id:0x%lx", afe_dev_ptr->intf);
			return ADSP_EOK;
		}
	}

	mmpm_info_ptr = (afe_mmpm_info_t *)afe_dev_ptr->mmpm_info_ptr;

	/* We update the list of active topologies,
	 * if VOTE: Then we set that TOPO as active
	 * if DEVOTE: We set that TOPO as inactive */

	if(AFE_MMPM_VOTE == mmpm_vote)
	{
		mmpm_info_ptr->mmpm_active_topos |= topo;
	}
	else
	{
		mmpm_info_ptr->mmpm_active_topos &= (~topo);
	}

	/* If vote_now is TRUE, we would accumulate the votes for active topologies and make the votings */
	if(TRUE == vote_now)
	{
		/* If we reach zero topologies then deregister from MMPM and return */
		if(0 == mmpm_info_ptr->mmpm_active_topos)
		{
			result = afe_mmpm_deregister(&afe_dev_ptr->mmpm_info_ptr, afe_dev_ptr->intf);
#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Deregistered MMPM for portid 0x%x", afe_dev_ptr->intf);
#endif
		}
		else
		{
			result = afe_mmpm_mpps_bw_sleep_voting(afe_dev_ptr);

#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"MMPM active Topos :0x%lx for portid 0x%x",
					mmpm_info_ptr->mmpm_active_topos, afe_dev_ptr->intf);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG
		}
	}

	return result;
}

/*
  This is for voting sleep latency. It will override the default votings done though afe_mmpm_voting().

  @param[in]     pDevPort: This is the port pointer
  @param[in]     sleep_latency: Sleep latency needed in us.

  @return
  ADSPResult
 */
ADSPResult afe_mmpm_vote_sleep_latency(void *pDevPort, uint32_t sleep_latency_us)
{
	ADSPResult result = ADSP_EOK;
	uint32_t     num_req = 0;

	MmpmRscParamType        afe_request;
	MMPM_STATUS             afe_request_status;
	afe_dev_port_t *afe_dev_ptr = (afe_dev_port_t *)pDevPort;

	if(!afe_dev_ptr->mmpm_info_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"FAILED NULL AFE MMPM ptr %ld", afe_dev_ptr->intf);
		return ADSP_EFAILED;
	}

	afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)afe_dev_ptr->mmpm_info_ptr;

	// make sure values are in valid range.
	if(sleep_latency_us < AFE_SLEEP_LATENCY_DEFAULT)
	{
		sleep_latency_us = AFE_SLEEP_LATENCY_DEFAULT;
	}
	else if(sleep_latency_us > AFE_SLEEP_LATENCY_MAX)
	{
		sleep_latency_us = AFE_SLEEP_LATENCY_MAX;
	}

	// Make the request for sleep latency.
	afe_request.rscId = MMPM_RSC_ID_SLEEP_LATENCY;
	afe_request.rscParam.sleepMicroSec = sleep_latency_us;
	num_req++;

	if(ADSP_FAILED(
			afe_mmpm_commit_vote(mmpm_info_ptr->core_info[ADSP].client_id, &afe_request,
					&afe_request_status, num_req)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AFE MMPM failed to make sleep latency vote: %lx for port id 0x%x", sleep_latency_us, afe_dev_ptr->intf);
		return ADSP_EFAILED;
	}

	mmpm_info_ptr->mmpm_voting.sleep_latency_us = sleep_latency_us;

#ifdef AFE_MMPM_LOW_LEVEL_DEBUG
	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AFE is making %d sleep latency request ", afe_request.rscParam.sleepMicroSec);
#endif //AFE_MMPM_LOW_LEVEL_DEBUG

	return result;
}

ADSPResult afe_mmpm_register_audio_client_class_and_dcvs_adjust(uint32_t client_id)
{
	ADSPResult int_res_1 = ADSP_EOK;
	ADSPResult int_res_2 = ADSP_EOK;
	//Register for client class as Audio to ADSPPM.
	MmpmClientClassType class_type = MMPM_AUDIO_CLIENT_CLASS;
	MmpmParameterConfigType param_cfg;
	param_cfg.pParamConfig = (void*)&class_type;
	param_cfg.paramId = MMPM_PARAM_ID_CLIENT_CLASS;
	MMPM_STATUS status = MMPM_SetParameter(client_id, &param_cfg);
	if(status != MMPM_STATUS_SUCCESS)
	{
		int_res_1 = ADSP_EFAILED;
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter class type failed for client id %lu with status %lu "
				, client_id,(uint32_t)status);
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter class type success for client id %lu"
				,client_id);
	}

	//Register for DCVS up only
	//LPM's core/bus votes are subject to automatic up adjustment by DCVS
	MmpmDcvsParticipationType dcvs_participation;
	dcvs_participation.enable = TRUE;
	dcvs_participation.enableOpt = MMPM_DCVS_ADJUST_ONLY_UP;
	param_cfg.pParamConfig = (void*)&dcvs_participation;
	param_cfg.paramId = MMPM_PARAM_ID_DCVS_PARTICIPATION;
	status = MMPM_SetParameter(client_id, &param_cfg);
	if(status != MMPM_STATUS_SUCCESS)
	{
		int_res_2 = ADSP_EFAILED;
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter DCVS participation for Only UP failed for client id %lu"
				" with status %lu ", client_id, (uint32_t)status);
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MMPM_SetParameter DCVS participation for Only UP success for client id %lu ",
				client_id);
	}

	if (ADSP_FAILED(int_res_1 |int_res_2))
	{
		return ADSP_EFAILED;
	}

	return ADSP_EOK;
}



