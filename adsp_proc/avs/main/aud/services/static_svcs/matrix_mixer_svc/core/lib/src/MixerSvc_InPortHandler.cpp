/**
@file MixerSvc_InPortHandler.cpp
@brief This file define functions that handle the arrival of a
       buffer at an input port of the audio matrix mixer.

 */

/*========================================================================
Copyright (c) 2010-2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/matrix_mixer_svc/core/lib/src/MixerSvc_InPortHandler.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/04/2010 AAA     Created file.
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "MixerSvc.h"
#include "adsp_media_fmt.h"
#include "adsp_adm_api.h"
#include "MixerSvc_InPortHandler.h"
#include "MixerSvc_MsgHandlers.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "MixerSvc_Util.h"
#include "avsync_lib.h"
#include "MixerSvc_AvSync.h"

void MtMx_ProcessDataQ(This_t* me, uint32_t unInPortID)
{
	elite_msg_any_t        myDataQMsg;
	ADSPResult              result;
	MatrixInPortInfoType    *pCurrentInPort;

	MtMx_ClearInputPortChannelStatus(me, unInPortID);
	pCurrentInPort = me->inPortParams[unInPortID];

	switch (pCurrentInPort->inPortState)
	{
	case INPUT_PORT_STATE_READY:
	{
		result = qurt_elite_queue_pop_front(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&myDataQMsg );
		pCurrentInPort->myDataQMsg = myDataQMsg;

		switch (pCurrentInPort->myDataQMsg.unOpCode)
		{
		case ELITE_DATA_MEDIA_TYPE:
		{
			result = MtMx_MsgMediaType(me, unInPortID, pCurrentInPort->myDataQMsg);

			if (ADSP_EOK == result)
			{
				//Update number of samples per channel required to fill the local buffer
				pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf = pCurrentInPort->unInPortPerChBufSize;

				//There is a remote timing related issue, where an already connected i/p port was moved to ready state
				//because of a port state change, however the media type with the new sampling rate arrived before the
				//new maprouting command, thus leading to a mismatch in the sampling rate. This can be an issue because
				//opening the i/p port to accept more data at this point (active transition) can lead to a potential crash.
				//In such a case, the i/p port should remain in ready state, until the maprouting command actually arrives.
				if (TRUE == MtMx_CheckIfInputPortCanTransitionAwayFromReady(me, unInPortID))
				{
					if ((INPUT_PORT_STATE_PAUSED == pCurrentInPort->inPortStatePrToStateChange) ||
							(INPUT_PORT_STATE_ACTIVE_HOLDING_INPUT_BUFFER == pCurrentInPort->inPortStatePrToStateChange))
					{
						pCurrentInPort->inPortState = pCurrentInPort->inPortStatePrToStateChange;
					}
					else
					{
						pCurrentInPort->inPortState = INPUT_PORT_STATE_ACTIVE;
						pCurrentInPort->inPortStatePrToStateChange = INPUT_PORT_STATE_ACTIVE;
					}
					MtMx_InPortToUpdateWaitMask(me, unInPortID);
				}
			}
			else
			{
				elite_msg_finish_msg(&myDataQMsg, result);
				break;
			}

			elite_msg_data_media_type_apr_t* pMediaTypePayload = (elite_msg_data_media_type_apr_t*)myDataQMsg.pPayload;
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: returning media_type buffer", me->mtMxID, unInPortID);
			elite_msg_push_payload_to_returnq(pMediaTypePayload->pBufferReturnQ, (elite_msg_any_payload_t*)pMediaTypePayload);

			break;
		}
		case ELITE_DATA_BUFFER:
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: rcvd data buffer in ready State, returning it", me->mtMxID, unInPortID);
			if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: failed to return i/p data buffer", me->mtMxID, unInPortID);
			}
			break;
		}
		case ELITE_DATA_EOS:
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd EOS in READY state", me->mtMxID, unInPortID);
			MtMx_MsgEos(me, unInPortID, pCurrentInPort->myDataQMsg);
			break;
		}
		case ELITE_DATA_MARK_BUFFER:
		{
			MtMx_ProcessMarkBuffer(me, unInPortID, pCurrentInPort->myDataQMsg, ASM_DATA_EVENT_MARK_BUFFER_PROCESSED);
			break;
		}
		case ELITEMSG_DATA_RESYNC:
		{
		    MtMx_MsgResync(me, unInPortID, pCurrentInPort->myDataQMsg);
		    break;
		}
		default:
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd unexpected buffer in ready state, returning it", me->mtMxID, unInPortID);
			if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to return unexpected i/p buffer", me->mtMxID, unInPortID);
			}
			break;
		}
		}
		break;
	}
	case INPUT_PORT_STATE_ACTIVE:
	case INPUT_PORT_STATE_PAUSED:
	{
		if (ADM_MATRIX_ID_AUDIO_RX == me->mtMxID)
		{
			result = qurt_elite_queue_pop_front(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&myDataQMsg );
			pCurrentInPort->myDataQMsg = myDataQMsg;
			pCurrentInPort->bIsInBufShort = FALSE;

			switch (pCurrentInPort->myDataQMsg.unOpCode)
			{
			case ELITE_DATA_BUFFER:
			{
				if (NULL == pCurrentInPort->pStartLoc)
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu rcvd data buffer, pStartLoc = NULL, Media type message not recd yet, returning buffer",
							me->mtMxID, unInPortID);
					if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu Error: MtMx failed to return i/p buffer", me->mtMxID, unInPortID);
					}
					break;
				}

				pCurrentInPort->numBufRcvd++;

				//AV-Sync
				(void)MtMx_ResetAvSyncStatsDrift(me, unInPortID);

#ifdef MT_MX_EXTRA_DEBUG
				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: [num buf rcvd, num buf ret] = [%lu, %lu], strmask %lu",
						me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd, pCurrentInPort->numBufReturned, pCurrentInPort->strMask);
#endif

				if (pCurrentInPort->numBufReturned != pCurrentInPort->numBufRcvd - 1)
				{
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: Mismatch: [num buf rcvd, num buf ret] = [%lu, %lu]",
							me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd, pCurrentInPort->numBufReturned);
				}

				//Debug message
				if (100 >= pCurrentInPort->numBufRcvd)
				{
					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu received %lu sample", me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd);

					if (1 == pCurrentInPort->numBufRcvd)
					{
						// After receiving the first sample rescan the output ports for connected inputs with valid timestamps.
						// It is only after receiving the first sample (really buffer) that we know whether or not an input has
						// valid timestamps.
						MtMx_ScanOutputPortsForInputsWithValidTimestamps(me);
					}
				}

				pCurrentInPort->bInBufStatus = INPUT_BUFFER_HELD;

				//AV-Sync
				(void)MtMx_HandleSampleAddDrops_2(me, unInPortID);

				// MtMx_InPortToHonorTimestamp returns ADSP_EFAILED, if the i/p buffer has to be held or dropped (not rendered).
				if(TRUE == pCurrentInPort->bForceCheckTimeStampValidity)
				{
					result = MtMx_InPortToHonorTimestamp(me, unInPortID);
					if (ADSP_EFAILED == result)
					{
						break;
					}
				}

				if (TRUE == pCurrentInPort->bIsLastMsgEOS)
				{
					pCurrentInPort->bIsLastMsgEOS = FALSE;
					if (TRUE == pCurrentInPort->bIsSessionUnderEOS)
					{
						pCurrentInPort->bIsSessionUnderEOS = FALSE;
						pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
						(void)MtMx_SetupDefaultRenderWindow(me, unInPortID); //AV-Sync
					}
				}

				// Do not block on the i/p port till it runs out of samples
				MtMx_RemoveInputPortFromWaitMask(me,unInPortID);

				//Process appropriate MsgDataBuffer
				(void)MtMx_ProcessMsgDataBuffer(me,unInPortID);

				// If local buffer is full OR input port has accumulation pending AND top priority output port has a
				// pending accumulation request, then trigger the accumulation
				if ((pCurrentInPort->bIsLocalBufFull || pCurrentInPort->bIsAccumulationPending) && ((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask)))
				{
					// Common processing routine for this i/p port
					MxAr_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);

					// Reset the accumulation pending flag for this input port
					pCurrentInPort->bIsAccumulationPending = FALSE;
				}

				//Check if any pull ports are waiting to send data downstream
				MxAr_ProcessPendingPullOutPorts(me);

				break;
			} // End of data buffer processing

			case ELITE_DATA_EOS:
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd EOS in ACTIVE/PAUSED state", me->mtMxID, unInPortID);

				MtMx_MsgEos(me, unInPortID, pCurrentInPort->myDataQMsg);
				break;
			} // End of data EOS processing
			case ELITE_DATA_MARK_BUFFER:
			{
				MtMx_ProcessMarkBuffer(me, unInPortID, pCurrentInPort->myDataQMsg, ASM_DATA_EVENT_MARK_BUFFER_PROCESSED);
				break;
			}
			case ELITE_DATA_MEDIA_TYPE:
			{
				if (TRUE == pCurrentInPort->bIsLastMsgEOS)
				{
					pCurrentInPort->bIsLastMsgEOS = FALSE;
					if (TRUE == pCurrentInPort->bIsSessionUnderEOS)
					{
						pCurrentInPort->bIsSessionUnderEOS = FALSE;
						pCurrentInPort->bShouldSessionTimeBeDerivedFromNextTS = TRUE;
						(void)MtMx_SetupDefaultRenderWindow(me, unInPortID); //AV-Sync
					}
				}
				result = MtMx_MsgMediaType(me, unInPortID, pCurrentInPort->myDataQMsg);

				if (ADSP_EOK != result)
				{
					elite_msg_finish_msg(&myDataQMsg, result);
					break;
				}

				elite_msg_data_media_type_apr_t* pMediaTypePayload = (elite_msg_data_media_type_apr_t*)myDataQMsg.pPayload;
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu: returning media_type buffer", me->mtMxID, unInPortID);
				elite_msg_push_payload_to_returnq(pMediaTypePayload->pBufferReturnQ, (elite_msg_any_payload_t*)pMediaTypePayload);

				break;
			}// End of Data Media Type case handling
			case ELITEMSG_DATA_RESYNC:
			{
			        MtMx_MsgResync(me, unInPortID, pCurrentInPort->myDataQMsg);
			        break;
                        }
			}
		}
		else
		{
			result = qurt_elite_queue_pop_front(pCurrentInPort->inPortHandle.portHandle.dataQ, (uint64_t*)&myDataQMsg );
			pCurrentInPort->myDataQMsg = myDataQMsg;
			pCurrentInPort->bIsInBufShort = FALSE;

			switch (pCurrentInPort->bIsPortLive)
			{
			case TRUE:
			{
				switch (pCurrentInPort->myDataQMsg.unOpCode)
				{
				case ELITE_DATA_BUFFER:
				{
					if (NULL == pCurrentInPort->pStartLoc)
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu rcvd data buffer, pStartLoc = NULL, Media type message not recd yet, returning buffer",
								me->mtMxID, unInPortID);
						if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
						{
							MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu Error: MtMx failed to return i/p buffer", me->mtMxID, unInPortID);
						}
						break;
					}

					//Check to see if this i/p port is connected in the first place. If not, DROP!!
					if (((1 << unInPortID) & (me->steadyStateInPortsMask)) && (pCurrentInPort->strMask))
					{
						pCurrentInPort->numBufRcvd++;

#ifdef MT_MX_EXTRA_DEBUG
						MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu live i/p port %lu: [num buf rcvd, num buf ret] = [%lu, %lu], strmask %lu",
								me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd, pCurrentInPort->numBufReturned, pCurrentInPort->strMask);
#endif

						if (pCurrentInPort->numBufReturned != pCurrentInPort->numBufRcvd - 1)
						{
							MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: [num buf rcvd, num buf ret] = [%lu, %lu]",
									me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd, pCurrentInPort->numBufReturned);
						}

						//Default this buffer to "Held" status
						pCurrentInPort->bInBufStatus = INPUT_BUFFER_HELD;
						MtMx_RemoveInputPortFromWaitMask(me, unInPortID);

						//Debug message
						if (100 >= pCurrentInPort->numBufRcvd)
						{
							MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu live i/p port %lu received %lu sample", me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd);
						}

						//Update the incoming buffer timestamps
						{
							elite_msg_data_buffer_t*   pInputBuf;
							pInputBuf = (elite_msg_data_buffer_t*)((pCurrentInPort->myDataQMsg).pPayload);
							pCurrentInPort->ullIncomingBufferTS = pInputBuf->ullTimeStamp;
						}

						//Process the input buffer
						MtMx_MsgDataBuffer(me, unInPortID, pCurrentInPort);

						// Since the MXAT is i/p driven, the data will try to accumulate immediately (live port) if:
						// The current i/p port's local buf. is FULL.
						if (pCurrentInPort->bIsLocalBufFull)
						{
							MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
						}

					}
					else
					{
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu dropping buffer as port is not connected to anything yet. InpQ: 0x%p", me->mtMxID, unInPortID, pCurrentInPort->inPortHandle.portHandle.dataQ);
						if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
						{
							MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu Error: MtMx failed to return i/p buffer", me->mtMxID, unInPortID);
						}

						pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
						MtMx_AddInputPortToWaitMask(me, unInPortID);

#ifdef MT_MX_EXTRA_DEBUG
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu, #Buf Released: %lu, i/p port added to wait mask",
								me->mtMxID, unInPortID, pCurrentInPort->numBufReturned);
#endif

					}

					//Check if any output port should be moved to ACTIVE state
					MtMx_CheckIfInportMovesAnyOutportToActiveState(me, unInPortID);
					break;
				}
				case ELITE_DATA_MEDIA_TYPE:
				{
					if(ADSP_FAILED(result = MtMx_MsgMediaType(me, unInPortID, pCurrentInPort->myDataQMsg)))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu Error: MtMx failed to process media type", me->mtMxID, unInPortID);
					}
					if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu Error: MtMx failed to return media type buffer", me->mtMxID, unInPortID);
					}
					break;
				}
				case ELITE_DATA_EOS:
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd EOS in ACTIVE/PAUSED state", me->mtMxID, unInPortID);
					MtMx_MsgEos(me, unInPortID, pCurrentInPort->myDataQMsg);
					break;
				}
				}
				break;
			}
			case FALSE:
			{
				switch (pCurrentInPort->myDataQMsg.unOpCode)
				{
				case ELITE_DATA_BUFFER:
				{
					if (NULL == pCurrentInPort->pStartLoc)
					{
						MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu rcvd data buffer, pStartLoc = NULL, Media type message not recd yet, returning buffer. InpQ: 0x%p", me->mtMxID, unInPortID, pCurrentInPort->inPortHandle.portHandle.dataQ);
						if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
						{
							MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu Error: MtMx failed to return i/p buffer", me->mtMxID, unInPortID);
						}
						break;
					}

					//Check to see if this i/p port is connected in the first place. If not, DROP!!
					if (((1 << unInPortID) & (me->steadyStateInPortsMask)) && (pCurrentInPort->strMask))
					{
						pCurrentInPort->numBufRcvd++;

#ifdef MT_MX_EXTRA_DEBUG
						MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu non-live i/p port %lu: [num buf rcvd, num buf ret] = [%lu, %lu], strmask %lu",
								me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd, pCurrentInPort->numBufReturned, pCurrentInPort->strMask);
#endif

						if (pCurrentInPort->numBufReturned != pCurrentInPort->numBufRcvd - 1)
						{
							MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: Mismatch: [num buf rcvd, num buf ret] = [%lu, %lu]",
									me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd, pCurrentInPort->numBufReturned);
						}

						//Default this buffer to "Held" status
						pCurrentInPort->bInBufStatus = INPUT_BUFFER_HELD;
						MtMx_RemoveInputPortFromWaitMask(me, unInPortID);

						//Debug message
						if (100 >= pCurrentInPort->numBufRcvd)
						{
							MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu non-live i/p port %lu received %lu sample", me->mtMxID, unInPortID, pCurrentInPort->numBufRcvd);
						}

						//Update the incoming buffer timestamps
						{
							elite_msg_data_buffer_t*   pInputBuf;
							pInputBuf = (elite_msg_data_buffer_t*)((pCurrentInPort->myDataQMsg).pPayload);
							pCurrentInPort->ullIncomingBufferTS = pInputBuf->ullTimeStamp;
						}

						//Process the input buffer
						MtMx_MsgDataBuffer(me, unInPortID, pCurrentInPort);

						if(pCurrentInPort->strMask)
						{
							//Since the MXAT is i/p driven, the data will try to accumulate (non-live port) if:
							//1) The current i/p port's local buf. is FULL. AND
							//2) The top prio o/p port is seeking new data.
							if (pCurrentInPort->bIsLocalBufFull)
							{
								if((1 << pCurrentInPort->unTopPrioOutPort) & (pCurrentInPort->outputReqPendingMask))
								{
									MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
								}
								else
								{
									uint32_t strMask = pCurrentInPort->strMask;

									while (strMask)
									{
										MatrixOutPortInfoType *pCurrentOutPort;
										uint32_t unOutPortID = Q6_R_ct0_R(strMask);
										strMask ^= (1 << unOutPortID);
										pCurrentOutPort = me->outPortParams[unOutPortID];

										//Check if this non-live i/p port was the top priority i/p port for any o/p port.
										if((FALSE == pCurrentOutPort->bIsTopPriorityInputPortLive) &&
												((pCurrentOutPort->inPortsTopPriorityMask) & (1<<unInPortID)))
										{
											MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
											break;
										}
										else
										{
											//Else, inform the o/p port about this non-live i/p port waiting to acc.
											pCurrentOutPort->inPortsWaitingToAccMask |= (1 << unInPortID);
											pCurrentInPort->bIsNonLiveInputPortWaitingToAcc = TRUE;

											if((pCurrentOutPort->inPortsMask) && (pCurrentOutPort->inPortsWaitingToAccMask == pCurrentOutPort->inPortsMask))
											{
												MtMx_AddOutputPortToWaitMask(me, unOutPortID);
											}
										}
									}
								}
							}
						}
						else
						{
							MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu Non-live i/p port %lu not connected to any o/p port. Removing i/p port from wait mask and mark pending acc.", me->mtMxID, unInPortID);
							MtMx_RemoveInputPortFromWaitMask(me,unInPortID);
							pCurrentInPort->bIsNonLiveInputPortWaitingToAcc = TRUE;
						}
					}
					else
					{
						MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p port %lu dropping buffer as port is not connected to anything yet. InpQ: 0x%p", me->mtMxID, unInPortID, pCurrentInPort->inPortHandle.portHandle.dataQ);
						if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
						{
							MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: failed to return i/p data buffer", me->mtMxID, unInPortID);
						}

						pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
						MtMx_AddInputPortToWaitMask(me, unInPortID);
						/*MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu, #Buf Released: %lu, i/p port added to wait mask",
								me->mtMxID, unInPortID, pCurrentInPort->numBufReturned);*/
					}
					break;
				}
				case ELITE_DATA_EOS:
				{
					MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu i/p port %lu rcvd EOS in ACTIVE/PAUSED state", me->mtMxID, unInPortID);
					MtMx_MsgEos(me, unInPortID, pCurrentInPort->myDataQMsg);
					break;
				}
				case ELITE_DATA_MEDIA_TYPE:
				{
					if(ADSP_FAILED(result = MtMx_MsgMediaType(me, unInPortID, pCurrentInPort->myDataQMsg)))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx %lu i/p port %lu Error: MtMx failed to process media type", me->mtMxID, unInPortID);
					}
					if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
					{
						MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: failed to return media type buffer", me->mtMxID, unInPortID);
					}
					break;
				}
				}
				break;
			}
			}
		}
		break;
	}
	default:
	{
		MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: rcvd buf in unexpected state [%ld]", me->mtMxID, unInPortID, pCurrentInPort->inPortState);
		if(ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu failed to return unexpected i/p buffer", me->mtMxID, unInPortID);
		}
		break;
	}
	}
}

void MtMx_MsgDataBuffer(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	uint32_t                     numSamplesReqPerCh;
	elite_msg_data_buffer_t*     pInputBuf;
	pCurrentInPort->pInputBuf = (elite_msg_data_buffer_t*)((pCurrentInPort->myDataQMsg).pPayload);
	pInputBuf = pCurrentInPort->pInputBuf;

	pCurrentInPort->nNumRemainingSamplesPerCh = (pInputBuf->nActualSize /
			(pCurrentInPort->unNumChannels * pCurrentInPort->unBytesPerSample));

	pCurrentInPort->nNumSamplesUsedPerCh = 0;

	pCurrentInPort->pCurrentSample = (int8_t *)(&(pInputBuf->nDataBuf));

	// Logging the input PCM data to Matrix mixer service
	MtMx_LogPcmData(me, unInPortID, pCurrentInPort->pCurrentSample);

	//Add/Drop samples if needed
	if (pCurrentInPort->nNumRemainingSamplesPerCh >= (int)pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
	{
		(void)MtMx_HandleSampleAddDrops_3(me, unInPortID); //AV-Sync
		numSamplesReqPerCh = pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf;
	}
	else
	{
		//i/p buffer contains lesser samples than the size of local buffer (short buffer)
		numSamplesReqPerCh = pCurrentInPort->nNumRemainingSamplesPerCh;
		pCurrentInPort->bIsInBufShort = TRUE;
	}

	/** Update timestamps only if there is a possibility of picking up data 
         *   from the current svc buffer. This could cause overwriting the TS
         *   for a buffer that hasn't been accumulated yet.
         */
  
	if(numSamplesReqPerCh > 0)
	{
		//Update timestamps only if there is a possibility of picking up data from the current svc buffer
		if(pCurrentInPort->bIsTimeStampValid )
		{
			pCurrentInPort->ullTimeStampAtCopy.int_part = pCurrentInPort->ullIncomingBufferTS;
			pCurrentInPort->ullTimeStampAtCopy.frac_part = 0;
		}
		//Copy samples from i/p buffer to local buffer and update nNumRemainingSamplesPerCh
		MtMx_FillInPortLocalBuf(me, unInPortID, numSamplesReqPerCh);
	}

	if (pCurrentInPort->nNumRemainingSamplesPerCh == 0)
	{
		// The incoming buffer did not contain sufficient samples to fill local buffer
		// return the buffer and add the input port to the wait mask
		ADSPResult result;

		if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: failed to return i/p data buffer", me->mtMxID, unInPortID);
		}

		pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
		pCurrentInPort->numBufReturned++;
		MtMx_AddInputPortToWaitMask(me, unInPortID);

#ifdef MT_MX_EXTRA_DEBUG
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu, #Buf Released: %lu, i/p port added to wait mask",
				me->mtMxID, unInPortID, pCurrentInPort->numBufReturned);
#endif

	}
}

void MxAr_CommonInPortProcessingRoutine(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	//Accumulate samples to accBufs of all o/p ports that have request pending for data from this i/p port
	MxAr_ApplyGainAndAccumulateSamples(me, unInPortID);

	//Post-steps (includes AV-Sync if applicable)
	(void)MxAr_CommonInPortProcessingRoutinePostSteps(me, unInPortID);
}


void MxAt_CommonInPortProcessingRoutine(This_t *me, uint32_t unInPortID, MatrixInPortInfoType *pCurrentInPort)
{
	ADSPResult  result;

	//Try to Accumulate samples to accBufs of all o/p ports connected to this i/p port
	MxAt_ApplyGainAndAccumulateSamples(me, unInPortID);

	//If a non-live i/p port could not accumulate to any o/p port, that i/p port needs to wait, holding the data until acc. can be achieved
	if((FALSE == pCurrentInPort->bIsPortLive) && (TRUE == pCurrentInPort->bIsNonLiveInputPortWaitingToAcc))
	{
		return;
	}

	//Try refilling the i/p port's local buffer with new samples from upstream peer's o/p buffer
	if (pCurrentInPort->nNumRemainingSamplesPerCh <= (int)pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf)
	{
		//I/p buffer has insufficient samples to fill local buffer
		//i. if nNumRemainingSamples is non-zero, copy them to local buffer
		if (pCurrentInPort->nNumRemainingSamplesPerCh)
		{
			MtMx_FillInPortLocalBuf(me, unInPortID, pCurrentInPort->nNumRemainingSamplesPerCh);
		}

		//ii. if held, return that buffer to the upstream peer's bufQ
		if (INPUT_BUFFER_HELD == pCurrentInPort->bInBufStatus)
		{
			if (ADSP_FAILED(result = elite_msg_return_payload_buffer(&(pCurrentInPort->myDataQMsg))))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MtMx #%lu i/p port %lu: failed to return i/p data buffer", me->mtMxID, unInPortID);
			}
			pCurrentInPort->bInBufStatus = INPUT_BUFFER_RELEASED;
			pCurrentInPort->numBufReturned++;
		}
		//iii. add dataQ to the wait mask
		MtMx_AddInputPortToWaitMask(me, unInPortID);

#ifdef MT_MX_EXTRA_DEBUG
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx#%lu i/p port %lu, #Buf Released: %lu, i/p port added to wait mask",
				me->mtMxID, unInPortID, pCurrentInPort->numBufReturned);
#endif

	}
	else
	{
		//I/p buffer has sufficient samples to fill local buffer
		//i. copy samples from i/p buffer to local buffer and update nNumRemainingSamplesPerCh
		MtMx_FillInPortLocalBuf(me, unInPortID, pCurrentInPort->unNumSamplesPerChReqToFillLocalBuf);
		//As a result of above op, if the local buffer did get filled up, then re-trigger accumulation.
		if (pCurrentInPort->bIsLocalBufFull)
		{
			//Common processing routine for this i/p port
			MxAt_CommonInPortProcessingRoutine(me, unInPortID, pCurrentInPort);
		}
	}
}

void MtMx_InPortUpdateMediaFmtParams(This_t *me, uint32_t unInPortID, uint32_t unSampleRate, uint32_t nNumChannels, uint32_t unBytesPerSample, uint32_t unBitwidth)
{
	MatrixInPortInfoType *pCurrentInPort = me->inPortParams[unInPortID];

	if ((0 != pCurrentInPort->unSampleRate) && (unSampleRate != pCurrentInPort->unSampleRate))
	{
		MtMx_AdjustBaseValueStcUpdate(me, unInPortID, unSampleRate); //AV-Sync
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MtMx #%lu: i/p #%lu sample rate change, reset bHasFirstSampleAccumulated to FALSE", me->mtMxID, unInPortID);
		pCurrentInPort->bHasFirstSampleAccumulated = FALSE;
	}

	//Update number of channels, bps, sample rate and frame duration
	pCurrentInPort->unNumChannels = nNumChannels;
	pCurrentInPort->unBytesPerSample = unBytesPerSample;
	pCurrentInPort->unBitwidth = (int)unBitwidth;
	pCurrentInPort->unSampleRate = unSampleRate;

	MtMx_SetInPortFrameDuration(me, unInPortID);
	(void)MtMx_SetDefaultRenderWindow(me, unInPortID); //AV-Sync
	return;
}
