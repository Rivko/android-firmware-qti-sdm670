/**
@file AudStreamMgr_StreamOpenCmdHandler.cpp

@brief This file contains the implementation for session cmdQ Handler functions
for AudioStreamMgr.

 */

/*========================================================================
$Header $ //source/qcom/qct/multimedia2/Audio/elite/static_svcs/AudioStreamMgr/main/latest/src/AudioStreamMgr_SessionCmdHandler.cpp#18 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/29/2013 rb      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_Stream.h"
#include "AudioStreamMgr_StreamCmdHandler.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "AudioStreamMgr_EliteMsg_Custom.h"
#include "AudioStreamMgr_adsppm.h"

#include "adsp_media_fmt.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"


/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
extern uint32_t asmMemoryMapClient;
/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

static ADSPResult AudioStreamMgr_PreOpenStreamCommonHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result;

	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	//allocate new session if not present already.
	if (NULL == pSession)
	{
		result = AudioStreamMgr_AllocateSession(pMe, int_session_id);
		if (ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Allocate session failed",
					int_session_id,  stream_id);
			return result;
		}
	}
	pSession = pMe->pSessions[int_session_id];

	//initialize the session
	AudioStreamMgr_InitializeSessionState(pSession);

	result = AudioStreamMgr_InitializeStreamState(pSession, stream_id);

	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: init session failed",
				pSession->ucInternalSessionId,  stream_id);

		return result;
	}

	return result;
}

static void AudioStreamMgr_OpenStreamBailout(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream,
      elite_apr_packet_t *pPkt, ADSPResult result)
{
   if (pSession)
   {
       if (pSession->unReqResps == 0) //if no response expected
       {
           //opening of the stream never puts stream in gapless_close.
           //if a stream is in gapless_close, it must be because it was already in that state.
           //in that case we shouldn't release/destroy the stream.
           //Gapless_close is valid only for rx, for others it's dont care
           if (pStream && (pStream->state != ASM_STREAM_GAPLESS_CLOSE))
           {
               //destroy stream svc
               (void) AudioStreamMgr_DestroyStreamSvc(pMe, pSession, pStream);

               ASM_ReleaseStream(pSession, pStream->ucStreamId);
           }
           //Ack failure
           AudioStreamMgr_GenerateAck( pPkt, result, 0 , 0 , 0 );

           AudioStreamMgr_DeallocateSessionIfLastStream(pMe, pSession->ucInternalSessionId);
       }
       else
       {
           pSession->nOverallResult = ADSP_EFAILED;
       }
   }
   else
   {
       AudioStreamMgr_GenerateAck( pPkt, result, 0 , 0 , 0 );
   }

}

static ADSPResult AudioStreamMgr_OpenStreamCommonHandler(AudioStreamMgr_t *pMe,
		AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream,
		ASM_OpenStreamParams_t *pOpen)
{
	elite_apr_port_t  port   =  PORT_FROM_SESSION_STREAM(pSession, pStream);

	// every new stream assign a id for PCM/bitstream logging.
	pStream->unDataLogId = ASM_GetNextDataLogId(pMe, (uint16_t) port);

	pStream->bits_per_sample = pOpen->bits_per_sample;
	pStream->src_fmt_id = pOpen->src_fmt_id;
	pStream->sink_fmt_id = pOpen->sink_fmt_id;
	pStream->stream_perf_mode = pOpen->stream_perf_mode;

	return ADSP_EOK;
}

static ADSPResult AudioStreamMgr_PostOpenStreamCommonHandler(AudioStreamMgr_t *pMe,
		AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream)
{
	ADSPResult result = ADSP_EOK;

	pStream->state = ASM_STREAM_CONNECTING;

	return result;
}

ADSPResult AudioStreamMgr_OpenRxTxStreamCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result=ADSP_EOK;
	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

	ASM_OpenStreamParams_t openStream;

	AudioStreamMgr_StreamStateType *pStream = NULL;
	uint32_t numOpenStreams;
	ASM_TopoId asm_topo;

	numOpenStreams = ASM_GetNumStreamsOpen(pSession); //doesn't include this stream.

	if ( numOpenStreams != 0 )
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Invalid request to open an occupied Session",
				pSession->ucInternalSessionId,  stream_id );
		result = ADSP_EALREADY;
		goto __bailout;
	}

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}
	pSession = pMe->pSessions[int_session_id];

	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenReadWrite(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	//Set session properties - stream format and Rx/Tx mode
	pSession->unMode = ASM_NT_MODE;

	asm_topo = ASM_TOPO_NT_MODE;
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u",pSession->ucInternalSessionId, stream_id, asm_topo);

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}

	//create dyna services
	if (ADSP_FAILED(result = AudioStreamMgr_CreateAllStreamSvcs(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	if (ADSP_FAILED((result = AudioStreamMgr_StreamConnectGraph(pMe,pSession,pStream))))
	{
		goto __bailout;
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	return ADSP_EOK;

	__bailout:
	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);


	return result;
}

/*
This function is to deal with open_write command.

For RX path: the open sequence is :
0. Session enters from closed state to connecting state
1. create decoder/PP.  Decoder and PP in Disconnect state.
2. request matrix port handle from ADM
3. on getting matrix port handle, connect decoder with pp and pp with matrix port
4. on receiving all connect ack from pp and dec, pp and dec enters connected state.
session enters pause state

Used for
1) regular decode
2) ULL
3) combiner input
4) gapless

 */
ADSPResult AudioStreamMgr_OpenRxStreamCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result=ADSP_EOK;

	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);
	uint32_t numOpenStreams;
	AudioStreamMgr_StreamStateType *pStream = NULL;
	ASM_OpenStreamParams_t openStream;
	ASM_TopoId asm_topo;

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	pSession = pMe->pSessions[int_session_id];

	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenWrite(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	numOpenStreams = ASM_GetNumStreamsOpen(pSession); //doesn't include this stream.

	// Error out if gapless and
	// (a) number of streams is already more than 1 OR
	// (b) one of the streams is already opened in non gapless mode.
	// (c) source end point is not client
	// (d) sink end point is not matrix
	if (openStream.gapless_mode || ASM_IsGaplessSession(pSession))
	{
		if ( ((numOpenStreams > 1) ) || ((numOpenStreams == 1) && !ASM_IsGaplessSession(pSession)) ||
				((numOpenStreams == 1) && !openStream.gapless_mode) ||
				(openStream.src_endpoint_type != ASM_SOURCE_CLIENT) ||
				(openStream.sink_endpoint_type != ASM_SINK_MATRIX_STRTR))
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"AudioStreamMgr [%u,%u]: Invalid request to open a gapless session. numOpenStreams = %lu",
					pSession->ucInternalSessionId, stream_id, numOpenStreams);
			result = ADSP_EALREADY;
			goto __bailout;
		}
	}
	else
	{
		//for non-gapless a stream open is not possible is any stream is already running (unless there's combiner or MIMO decoder).
		//check will be made in ASM_CheckIntraSessionTopoErrors
	}

	// LLNP, ULL & ULLPP stream session supports only PCM data and do not support gapless playback, sample rate
	// change event notifications or more than one stream
	if ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == openStream.stream_perf_mode) ||
			(ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == openStream.stream_perf_mode) ||
			(ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == openStream.stream_perf_mode))
	{
		if ((numOpenStreams == 1) || openStream.gapless_mode || !ASM_IsPcmFormat(openStream.src_fmt_id) || openStream.sr_cm_event_needed ||
				(openStream.src_endpoint_type != ASM_SOURCE_CLIENT) || (openStream.sink_endpoint_type != ASM_SINK_MATRIX_STRTR) )
		{
			MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Invalid params in LLNP/ULL/ULLPP open write command or trying to open more than one stream in LLNP/ULL/ULLPP. numOpenStreams = %lu",
					pSession->ucInternalSessionId, stream_id, numOpenStreams);
			result = ADSP_EFAILED;
			goto __bailout;
		}
	}

	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	asm_topo = ASM_GetRxTopoId(openStream.src_endpoint_type, openStream.sink_endpoint_type);

	if ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode) ||
			(ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pStream->stream_perf_mode))
	{
		asm_topo = ASM_TOPO_RX_ULL;
	}
	else if (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pStream->stream_perf_mode)
	{
	   asm_topo = ASM_TOPO_RX_ULLPP;
	}

	if (openStream.gapless_mode)
	{
		asm_topo = ASM_TOPO_RX_GAPLESS;
	}

	if (asm_topo == ASM_INVALID_TOPO)
	{
		result = ADSP_EUNSUPPORTED;
		goto __bailout;
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u",pSession->ucInternalSessionId, stream_id, asm_topo);

	pSession->unMode = ASM_RX_MODE;

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_CheckIntraSessionTopoErrors(pSession)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo is inconsistent with already existing streams.",pSession->ucInternalSessionId, stream_id);
		goto __bailout;
	}

	//gapless is special case because each leg of gapless is not opened as a stream.
	if (openStream.gapless_mode && (numOpenStreams == 1))
	{
		result = AudioStreamMgr_OpenSecondGaplessStream(pMe, pSession, pStream, pPkt, &openStream);

		if(ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to open second gapless stream",
					pSession->ucInternalSessionId,  stream_id );
			goto __bailout;
		}
	}
	else
	{
		//if first gapless stream set the ptr on the stream (also used as a flag).
		if (openStream.gapless_mode)
		{
			pSession->pFirstGaplessStrm = pStream;
		}

		//create dyna services & request matrix handle
		if (ADSP_FAILED(result = AudioStreamMgr_CreateAllStreamSvcs(pMe, pSession, pStream, &openStream)))
		{
			goto __bailout;
		}

		//ReqResps will be nonzero if during create svc the svc exists already and a message is sent
		if (pSession->unReqResps == 0)
		{
			if (ASM_IsRxDeviceHandleNeeded(pStream)) //for combiner, dev handle is not needed.
			{
				if (ADSP_FAILED(result = AudioStreamMgr_SessionRequestRxDeviceHandle(pMe, pSession, pStream)))
				{
					goto __bailout;
				}
			}
		}
	}

	if (pSession->unReqResps == 0) //if no response expected, proceed with next step. (can happen with combiner if combiner was created in createStreamSvcs)
	{
		if (ADSP_FAILED((result = AudioStreamMgr_StreamConnectGraph(pMe,pSession,pStream))))
		{
			goto __bailout;
		}
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}


	return ADSP_EOK;

	__bailout:
	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);

	return result;
}

/*
This function is to deal with open_read command.

For TX path: the open sequence is :
0. Session enters from closed state to connecting state
1. create encoder/PP.  Encoder and PP in Disconnect state.
2. request port handle from AFE .
3. on getting AFE handle (this means AFE is connected to PP),
send afe start to afe( this is work-around only before
new interface or ADM)
4. on receving afe start ack, AFE node in connected state. Now
connect pp to encoder (or directly ack success in the absence of pp)
5. on receiving all connect ack from pp and dec, pp and dec enters connected state.
session enters run state (With Michael's ISOD interface, dynamic service should be in pause on
creation, in that case, may be better for sesion to enter pause state???)

 */
ADSPResult AudioStreamMgr_OpenTxStreamCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result=ADSP_EOK;
	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

	ASM_OpenStreamParams_t openStream;
	AudioStreamMgr_StreamStateType *pStream = NULL;
	uint32_t numOpenStreams;
	ASM_TopoId asm_topo;

	numOpenStreams = ASM_GetNumStreamsOpen(pSession); //doesn't include this stream.

	if ( numOpenStreams != 0 )
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Invalid request to open an occupied Session",
				pSession->ucInternalSessionId,  stream_id );
		result = ADSP_EALREADY;
		goto __bailout;
	}

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}
	pSession = pMe->pSessions[int_session_id];

	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenRead(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	if (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == openStream.stream_perf_mode)
	{
		if ( !ASM_IsPcmFormat(openStream.sink_fmt_id) || openStream.sr_cm_event_needed ||
				(openStream.src_endpoint_type != ASM_SOURCE_MATRIX_STRTR) || (openStream.sink_endpoint_type != ASM_SINK_CLIENT) )
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"AudioStreamMgr [%u,%u]: Invalid params in LLNP open read command ",
					int_session_id, stream_id);
			result = ADSP_EFAILED;
			goto __bailout;
		}
	}

	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

  
	//Set session properties - stream format and Rx/Tx mode
	pSession->unMode = ASM_TX_MODE;

	asm_topo = ASM_TOPO_TX_REGULAR;
	if (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == openStream.stream_perf_mode)
	{
		asm_topo = ASM_TOPO_TX_LLNP;
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u",pSession->ucInternalSessionId, stream_id, asm_topo);

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}

	//create dyna services
	if (ADSP_FAILED(result = AudioStreamMgr_CreateAllStreamSvcs(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	/* Request Tx Handle as part of creation process */
	result = AudioStreamMgr_SessionRequestTxDeviceHandle(pMe, pSession, pStream, openStream.timestamp_type);

	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudStreamMgr [%u,%u]: FAILED to request Tx Matrix handle",
				pSession->ucInternalSessionId,  stream_id);
		goto __bailout;
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	return ADSP_EOK;

	__bailout:

	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);

	return result;
}


/*
This function is to deal with open_loopback command.

For TX path: the open sequence is :
0. Session enters from closed state to connecting state
1. Create POPP.
2. Request TX matrix port handle from ADM.
3. Request RX matrix port handle from ADM.
 */

ADSPResult AudioStreamMgr_OpenLoopBackCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result=ADSP_EOK;
	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

	ASM_OpenStreamParams_t openStream;
	AudioStreamMgr_StreamStateType *pStream = NULL;
	ASM_TopoId asm_topo;
	asm_session_mode_t mode = ASM_RX_MODE;

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}
	pSession = pMe->pSessions[int_session_id];

	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenLoopback(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	asm_topo = ASM_GetLoopbackTopoIdAndMode(openStream.src_endpoint_type, openStream.sink_endpoint_type,
			openStream.src_fmt_id, openStream.sink_fmt_id,
			openStream.decoder_conv_mode, &mode);

	switch (asm_topo)
	{
	case ASM_TOPO_PCM2PCM_LOOPBACK:
		break;
	case ASM_TOPO_PCM2COMPR_LOOPBACK:
		openStream.enc_io_fmt_conv = ASM_IO_FORMAT_CONV_PCM_TO_61937;
		break;
	case ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK:
		//decoder needs to depacketize & decode
		//encoder needs to encode & packetize
		openStream.dec_io_fmt_conv = ASM_IO_FORMAT_CONV_61937_TO_PCM;
		openStream.enc_io_fmt_conv = ASM_IO_FORMAT_CONV_PCM_TO_61937;
		break;
	case ASM_TOPO_COMPR2COMPR_CONV_LOOPBACK:
		//decoder needs to depacketize & conv and packetize
		openStream.dec_io_fmt_conv = ASM_IO_FORMAT_CONV_61937_CONV_61937;
		break;
	case ASM_TOPO_COMPR2COMPR_DIRECT_LOOPBACK:
		break;
	case ASM_TOPO_COMPR2PCM_LOOPBACK:
		openStream.dec_io_fmt_conv = ASM_IO_FORMAT_CONV_61937_TO_PCM;
		break;
	default:
		result = ADSP_EUNSUPPORTED;
		goto __bailout;
	}

	pSession->unMode = mode;

	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u, mode = %u",pSession->ucInternalSessionId, stream_id, asm_topo, pSession->unMode);

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_CheckIntraSessionTopoErrors(pSession)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo is inconsistent with already exising streams.",pSession->ucInternalSessionId, stream_id);
		goto __bailout;
	}

	//create dyna services
	if (ADSP_FAILED(result = AudioStreamMgr_CreateAllStreamSvcs(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	//ReqResps will be nonzero if during create svc the svc exists already and a message is sent
	if (pSession->unReqResps == 0)
	{
		if (ASM_IsRxDeviceHandleNeeded(pStream)) //for combiner stream there's no need to req device handle.
		{
			if (ADSP_FAILED(result = AudioStreamMgr_SessionRequestRxDeviceHandle(pMe, pSession, pStream)))
			{
				goto __bailout;
			}
		}
	}

	if (pSession->unReqResps == 0) //combiner case
	{
		if (ADSP_FAILED((result = AudioStreamMgr_StreamConnectGraph(pMe,pSession,pStream))))
		{
			goto __bailout;
		}
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	return ADSP_EOK;

	__bailout:
	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);

	return result;
}

ADSPResult AudioStreamMgr_OpenWriteCompressedCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result=ADSP_EOK;
	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
	//Get command payload
	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

	ASM_OpenStreamParams_t openStream;
	AudioStreamMgr_StreamStateType *pStream = NULL;
	uint32_t numOpenStreams;
	ASM_TopoId asm_topo;

	numOpenStreams = ASM_GetNumStreamsOpen(pSession); //doesn't include this stream.

	if ( numOpenStreams != 0 )
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Invalid request to open an occupied Session",
				pSession->ucInternalSessionId,  stream_id );
		result = ADSP_EALREADY;
		goto __bailout;
	}

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}
	pSession = pMe->pSessions[int_session_id];
	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenWriteCompr(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}
	pSession->unMode = ASM_COMPRESSED_PLAYBACK_MODE;

	if (((ASM_SOURCE_CLIENT == openStream.src_endpoint_type) && (ASM_SINK_MATRIX_STRTR == openStream.sink_endpoint_type)))
	{
		asm_topo = ASM_TOPO_RX_COMPR;
	}
	else
	{
		MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Invalid ASM stream topo src,sink = %u, %u",pSession->ucInternalSessionId, stream_id,
				openStream.src_endpoint_type,openStream.sink_endpoint_type );
		goto __bailout;
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u",pSession->ucInternalSessionId, stream_id, asm_topo);

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_CheckIntraSessionTopoErrors(pSession)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo is inconsistent with already exising streams.",pSession->ucInternalSessionId, stream_id);
		goto __bailout;
	}

	//create dyna services
	if (ADSP_FAILED(result = AudioStreamMgr_CreateAllStreamSvcs(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = AudioStreamMgr_SessionRequestRxDeviceHandle(pMe, pSession, pStream)))
	{
		goto __bailout;
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	return ADSP_EOK;

	__bailout:
	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);

	return result;
}

ADSPResult AudioStreamMgr_OpenReadCompressedCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{

	ADSPResult result=ADSP_EOK;
	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
	//Get command payload
	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

	ASM_OpenStreamParams_t openStream;
	AudioStreamMgr_StreamStateType *pStream = NULL;
	uint32_t numOpenStreams;
	ASM_TopoId asm_topo;

	numOpenStreams = ASM_GetNumStreamsOpen(pSession); //doesn't include this stream.

	if ( numOpenStreams != 0 )
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Invalid request to open an occupied Session",
				pSession->ucInternalSessionId,  stream_id );
		result = ADSP_EALREADY;
		goto __bailout;
	}

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}
	pSession = pMe->pSessions[int_session_id];
	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenReadCompr(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	pSession->unMode = ASM_COMPRESSED_INPUT_MODE;

	asm_topo = ASM_TOPO_TX_COMPR;
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u",pSession->ucInternalSessionId, stream_id, asm_topo);

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}

	//create dyna services
	if (ADSP_FAILED(result = AudioStreamMgr_CreateAllStreamSvcs(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	result = AudioStreamMgr_SessionRequestTxDeviceHandle(pMe, pSession, pStream, ASM_ABSOLUTE_TIMESTAMP);
	if(ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to request Tx Stream Router handle",
				pSession->ucInternalSessionId,  stream_id);
		goto __bailout;
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	return ADSP_EOK;

	__bailout:

	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);

	return result;

}

/**
      the entire struct of the payload consists of the following

     -asm_stream_cmd_open_pull_mode_write_t
     -asm_multi_channel_pcm_fmt_blk_v3_t (if dec_fmt_id is ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
     -avs_shared_map_region_payload_t (repeated shared_pos_buf_num_regions times (usually 1))
     -avs_shared_map_region_payload_t (repeated shared_circ_buf_num_regions times)
 */
ADSPResult AudioStreamMgr_OpenPullModeRxStreamCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result=ADSP_EOK;
	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

	asm_multi_channel_pcm_fmt_blk_v3_t *pMediaFmt;
	avs_shared_map_region_payload_t *pRegion;
	asm_pull_mode_watermark_level_t *pWaterMarkLevels;

	uint32_t sharedCircBufVirtAddr, sharedPosBufVirtAddr, sharedCircBufSize;
	ASM_OpenStreamParams_t openStream;
	AudioStreamMgr_StreamStateType *pStream = NULL;
	uint32_t numOpenStreams;
	asm_stream_cmd_open_pull_mode_write_t *pOpenCmd;
	uint8_t *pOpenCmdBytePtr;

	numOpenStreams = ASM_GetNumStreamsOpen(pSession); //doesn't include this stream.

	if ( numOpenStreams != 0 )
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Invalid request to open an occupied Session",
				pSession->ucInternalSessionId,  stream_id );
		result = ADSP_EALREADY;
		goto __bailout;
	}

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}
	pSession = pMe->pSessions[int_session_id];
	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenWritePull(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	/** additional parsing & memory mapping */
	pOpenCmd = (asm_stream_cmd_open_pull_mode_write_t *)elite_apr_if_get_payload_ptr(pPkt);
	pOpenCmdBytePtr = (uint8_t*)pOpenCmd;

	//get media type info
	pOpenCmdBytePtr += sizeof(asm_stream_cmd_open_pull_mode_write_t);
	pMediaFmt = (asm_multi_channel_pcm_fmt_blk_v3_t*) pOpenCmdBytePtr;

	if(openStream.src_fmt_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
	{
	    //map memory for the position buffer
	    pOpenCmdBytePtr += sizeof(asm_multi_channel_pcm_fmt_blk_v3_t);
	}
	else if (openStream.src_fmt_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4)
	{
        //map memory for the position buffer
        pOpenCmdBytePtr += sizeof(asm_multi_channel_pcm_fmt_blk_v4_t);
	}
	else
	{
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "AudioStreamMgr [%u,%u]: pull mode: dec_fmt_id %lu is not supported",
                pSession->ucInternalSessionId, stream_id, openStream.src_fmt_id);
	    goto __bailout;
	}

	pRegion = (avs_shared_map_region_payload_t*)pOpenCmdBytePtr;

	MSG_9(MSG_SSID_QDSP6, DBG_MED_PRIO,
			"AudioStreamMgr [%u,%u]: position buf msw=0x%lx,lsw=0x%lx,prop_flag=%lx,num_reg=%ld, first reg (0x%lx,0x%lx, %lu)",
			pSession->ucInternalSessionId,  stream_id, pOpenCmd->shared_pos_buf_phy_addr_msw,
			pOpenCmd->shared_pos_buf_phy_addr_lsw, pOpenCmd->shared_pos_buf_property_flag,
			(long int)pOpenCmd->shared_pos_buf_num_regions, pRegion->shm_addr_msw, pRegion->shm_addr_lsw, pRegion->mem_size_bytes);

	result = ASM_MemMapHandler(asmMemoryMapClient, pOpenCmd->shared_pos_buf_mem_pool_id,
			pOpenCmd->shared_pos_buf_property_flag, FALSE, pOpenCmd->shared_pos_buf_num_regions,
			pRegion, &(pStream->mem_map_handle[ASM_POS_BUF_INDEX]));
	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: pull mode: Position buffer could not be mapped.",
				pSession->ucInternalSessionId,  stream_id );

		goto __bailout;
	}

	if (ADSP_FAILED(result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(asmMemoryMapClient,pStream->mem_map_handle[ASM_POS_BUF_INDEX],
			pOpenCmd->shared_pos_buf_phy_addr_lsw,
			pOpenCmd->shared_pos_buf_phy_addr_msw,
			&sharedPosBufVirtAddr)))
	{
		MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: pull mode: Phy to Virt Failed(paddr,vaddr)-->(%lx%lx,%lx)\n",
				pSession->ucInternalSessionId,  stream_id,
				pOpenCmd->shared_pos_buf_phy_addr_msw,
				pOpenCmd->shared_pos_buf_phy_addr_lsw,
				sharedPosBufVirtAddr);
		goto __bailout;
	}

	//map memory for the shared circular buffer
	pOpenCmdBytePtr += sizeof(avs_shared_map_region_payload_t) * pOpenCmd->shared_pos_buf_num_regions;
	pRegion = (avs_shared_map_region_payload_t*)pOpenCmdBytePtr;
	sharedCircBufSize = pOpenCmd->shared_circ_buf_size;
	ASM_TopoId asm_topo;

	MSG_9(MSG_SSID_QDSP6, DBG_MED_PRIO,
			"AudioStreamMgr [%u,-]: circular buf msw=0x%lx,lsw=0x%lx, size=%lu, prop_flag=%lx,num_reg=%ld, first reg (0x%lx,0x%lx, %lu)",
			pSession->ucInternalSessionId,  pOpenCmd->shared_circ_buf_start_phy_addr_msw,
			pOpenCmd->shared_circ_buf_start_phy_addr_lsw, pOpenCmd->shared_circ_buf_size, pOpenCmd->shared_circ_buf_property_flag,
			(long int)pOpenCmd->shared_circ_buf_num_regions, pRegion->shm_addr_msw, pRegion->shm_addr_lsw, pRegion->mem_size_bytes);

	// Cache is used since DSP has to just invalidate the cache and read.
	result = ASM_MemMapHandler(asmMemoryMapClient, pOpenCmd->shared_circ_buf_mem_pool_id,
			pOpenCmd->shared_circ_buf_property_flag, TRUE, pOpenCmd->shared_circ_buf_num_regions,
			pRegion, &(pStream->mem_map_handle[ASM_CIRC_BUF_INDEX]));
	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: pull mode: Shared circ buffer could not be mapped.",
				pSession->ucInternalSessionId,  stream_id );
		goto __bailout;
	}

	if (ADSP_FAILED(result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(asmMemoryMapClient,pStream->mem_map_handle[ASM_CIRC_BUF_INDEX],
			pOpenCmd->shared_circ_buf_start_phy_addr_lsw,
			pOpenCmd->shared_circ_buf_start_phy_addr_msw,
			&sharedCircBufVirtAddr)))
	{
		MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: pull mode: Phy to Virt Failed(paddr,vaddr)-->(%lx%lx,%lx)\n",
				pSession->ucInternalSessionId,  stream_id,
				pOpenCmd->shared_circ_buf_start_phy_addr_msw,
				pOpenCmd->shared_circ_buf_start_phy_addr_lsw,
				sharedCircBufVirtAddr);
		goto __bailout;
	}

	pOpenCmdBytePtr += sizeof(avs_shared_map_region_payload_t) * pOpenCmd->shared_circ_buf_num_regions;
	pWaterMarkLevels = (asm_pull_mode_watermark_level_t*) pOpenCmdBytePtr;


	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	//Set session properties - stream format and Rx/Tx mode
	pSession->unMode = ASM_RX_PULL_MODE;
	if ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode) ||
			(ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pStream->stream_perf_mode))
	{
		asm_topo = ASM_TOPO_RX_PULL_ULL;
	}
	else if (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pStream->stream_perf_mode)
	{
	   asm_topo = ASM_TOPO_RX_PULL_ULLPP;
	}
	else
	{
		asm_topo = ASM_TOPO_RX_PULL;
	}

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u",pSession->ucInternalSessionId, stream_id, asm_topo);

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}


	/** create decoder service in pull mode */
	{
		ASM_GraphNode *pDecNode = &(pStream->nodes[ASM_NODE_DEC]);

		uint32_t client_token  = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_DEC);

		AudioStreamMgr_CallBackHandleType DecCbData;
		(void) AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, client_token, &DecCbData);

		AudioDecSvcInitParams_t decInitParams;
		memset(&decInitParams, 0, sizeof(decInitParams));

		decInitParams.io_param.io_type = AUDIO_DEC_SVC_IN_OUT_STREAM;
		decInitParams.io_param.in.ulMode = DEC_SVC_PULL_MODE;
		decInitParams.io_param.format_id = openStream.src_fmt_id;
		decInitParams.io_param.stream_type = openStream.stream_type;
		decInitParams.io_param.stream_id = pStream->ucStreamId;
		decInitParams.io_param.pCbData = &DecCbData;
		decInitParams.io_param.pDelay = &pDecNode->buf_delay; // The decoder does not provide algorithmic delay.
		pDecNode->alg_delay = 0;
		decInitParams.io_param.bits_per_sample = openStream.bits_per_sample;
		decInitParams.pEventQ = pStSession->pSessionRespQ;
		decInitParams.io_param.eventToken = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_DEC);
		decInitParams.io_param.pEventQ = pStSession->pSessionRespQ;
		decInitParams.io_param.ulDataLogId = pStream->unDataLogId;
		//pass the stream perf mode to the decoder
		decInitParams.io_param.perf_mode = openStream.stream_perf_mode;
		decInitParams.io_param.in.pull_mode.shared_circ_buf_addr = sharedCircBufVirtAddr;
		decInitParams.io_param.in.pull_mode.pos_buf_ptr = (asm_pull_mode_write_position_buffer_t*)sharedPosBufVirtAddr;
		decInitParams.io_param.in.pull_mode.shared_circ_buf_size = sharedCircBufSize;
		decInitParams.io_param.in.pull_mode.num_water_mark_levels = pOpenCmd->num_watermark_levels;
		decInitParams.io_param.in.pull_mode.water_mark_levels_ptr = pWaterMarkLevels;
		decInitParams.io_param.in.no_apr_dec = openStream.no_apr_enc_dec;
		decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO;
		decInitParams.io_param.out.downstream_buf_delay_ptr = AudioStreamMgr_GetPostDecSvcVarBufDelay(pStream);

		memscpy(&decInitParams.io_param.in.pull_mode.MediaFmt.pcm, sizeof(decInitParams.io_param.in.pull_mode.MediaFmt.pcm), pMediaFmt, sizeof(asm_multi_channel_pcm_fmt_blk_v3_t));

		if(openStream.src_fmt_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
		{
		    decInitParams.io_param.in.pull_mode.MediaFmt.pcm.endianness = 0;
		}
		else
		{
		    decInitParams.io_param.in.pull_mode.MediaFmt.pcm.endianness = ((asm_multi_channel_pcm_fmt_blk_v4_t*)pMediaFmt)->endianness;
		}
		
		decInitParams.io_param.in.pull_mode.MediaFmt.pcm.mode = 0;

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Creating Decoder Service",
				pSession->ucInternalSessionId,  stream_id);

		result = AudioDecSvc_Create (&decInitParams, (void**)(&pDecNode->pSvcHandle));

		if(ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"AudioStreamMgr [%u,%u]: Failed to create decoder service in pull mode",
					pSession->ucInternalSessionId,  stream_id );
			goto __bailout;
		}

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Decoder Service Created",
				pSession->ucInternalSessionId,  stream_id);

		pDecNode->threadId = pDecNode->pSvcHandle->threadId;
	}

	/** create PP service in pull mode for non-ULL/LLNP sessions */
	if ((pStream->stream_perf_mode != ASM_ULTRA_LOW_LATENCY_STREAM_SESSION) &&
			(pStream->stream_perf_mode != ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION))
	{
		result = AudioStreamMgr_CreatePPSvc(pMe, pSession, pStream, &openStream); //TODO: need to use pMediaFmt->sample_rate. right now, issue with matrix.

		if(ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"AudioStreamMgr [%u,%u]: Failed to create Post=proc service",
					pSession->ucInternalSessionId,  stream_id );
			goto __bailout;
		}
	}

	if (ADSP_FAILED(result = AudioStreamMgr_SessionRequestRxDeviceHandle(pMe, pSession, pStream)))
	{
		goto __bailout;
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	return ADSP_EOK;

	__bailout:

	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);

	return result;
}

ADSPResult AudioStreamMgr_OpenPushModeTxStreamCmdHandler(AudioStreamMgr_t *pMe,
		uint8_t int_session_id, uint8_t stream_id)
{
	ADSPResult result=ADSP_EOK;

	asm_stream_cmd_open_push_mode_read_t *pOpenCmd ;
	AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
	AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
	elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

	pOpenCmd = (asm_stream_cmd_open_push_mode_read_t *)elite_apr_if_get_payload_ptr(pPkt);
	uint8_t *pOpenCmdBytePtr = (uint8_t*)pOpenCmd;
	asm_multi_channel_pcm_enc_cfg_v3_t *pEncCfg;
	avs_shared_map_region_payload_t *pRegion;
	asm_push_mode_watermark_level_t *pWaterMarkLevels;
	uint32_t sharedCircBufVirtAddr, sharedPosBufVirtAddr, sharedCircBufSize;

	ASM_OpenStreamParams_t openStream;
	AudioStreamMgr_StreamStateType *pStream = NULL;
	uint32_t numOpenStreams;
	ASM_TopoId asm_topo;

	numOpenStreams = ASM_GetNumStreamsOpen(pSession); //doesn't include this stream.

	if ( numOpenStreams != 0 )
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Invalid request to open an occupied Session",
				pSession->ucInternalSessionId,  stream_id );
		result = ADSP_EALREADY;
		goto __bailout;
	}

	result = AudioStreamMgr_PreOpenStreamCommonHandler(pMe, int_session_id, stream_id);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	pSession = pMe->pSessions[int_session_id];
	pStream = ASM_GetStreamInstance(pSession, stream_id);

	if (NULL == pStream)
	{
		goto __bailout;
	}

	if (ADSP_FAILED(result = ASM_ParseOpenReadPush(pPkt, &openStream, pSession->ucInternalSessionId, stream_id)))
	{
		goto __bailout;
	}

	/** additional parsing and memory mapping */
	//get media type info
	pOpenCmdBytePtr += sizeof(asm_stream_cmd_open_push_mode_read_t);
	pEncCfg = (asm_multi_channel_pcm_enc_cfg_v3_t*) pOpenCmdBytePtr;

	if (openStream.sink_fmt_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
	{
	    //map memory for the position buffer
	    pOpenCmdBytePtr += sizeof(asm_multi_channel_pcm_enc_cfg_v3_t);
	}
	else if (openStream.sink_fmt_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4)
	{
	    //map memory for the position buffer
	    pOpenCmdBytePtr += sizeof(asm_multi_channel_pcm_enc_cfg_v4_t);
	}
	else
	{
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "AudioStreamMgr [%u,%u]: push mode: enc_cfg_id %lu is not supported",
                pSession->ucInternalSessionId, stream_id, openStream.sink_fmt_id);
        goto __bailout;
	}

	pRegion = (avs_shared_map_region_payload_t*)pOpenCmdBytePtr;

	MSG_9(MSG_SSID_QDSP6, DBG_MED_PRIO,
			"AudioStreamMgr [%u,%u]: position buf msw=0x%lx,lsw=0x%lx,prop_flag=%lx,num_reg=%d, first reg (0x%lx,0x%lx, %lu)",
			pSession->ucInternalSessionId,  stream_id, pOpenCmd->shared_pos_buf_phy_addr_msw,
			pOpenCmd->shared_pos_buf_phy_addr_lsw, pOpenCmd->shared_pos_buf_property_flag,
			pOpenCmd->shared_pos_buf_num_regions, pRegion->shm_addr_msw, pRegion->shm_addr_lsw,pRegion->mem_size_bytes);

	result = ASM_MemMapHandler(asmMemoryMapClient, pOpenCmd->shared_pos_buf_mem_pool_id,
			pOpenCmd->shared_pos_buf_property_flag, FALSE, pOpenCmd->shared_pos_buf_num_regions,
			pRegion, &(pStream->mem_map_handle[ASM_POS_BUF_INDEX]));
	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: push mode: Position buffer could not be mapped.",
				pSession->ucInternalSessionId,  stream_id );

		goto __bailout;
	}

	if (ADSP_FAILED(result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(asmMemoryMapClient,pStream->mem_map_handle[ASM_POS_BUF_INDEX],
			pOpenCmd->shared_pos_buf_phy_addr_lsw,
			pOpenCmd->shared_pos_buf_phy_addr_msw,
			&sharedPosBufVirtAddr)))
	{
		MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: push mode: Phy to Virt Failed(paddr,vaddr)-->(%lx%lx,%lx)\n",
				pSession->ucInternalSessionId,  stream_id,
				pOpenCmd->shared_pos_buf_phy_addr_msw,
				pOpenCmd->shared_pos_buf_phy_addr_lsw,
				sharedPosBufVirtAddr);

		goto __bailout;
	}

	//map memory for the shared circular buffer
	pOpenCmdBytePtr += sizeof(avs_shared_map_region_payload_t) * pOpenCmd->shared_pos_buf_num_regions;
	pRegion = (avs_shared_map_region_payload_t*)pOpenCmdBytePtr;
	sharedCircBufSize = pOpenCmd->shared_circ_buf_size;

	MSG_9(MSG_SSID_QDSP6, DBG_MED_PRIO,
			"AudioStreamMgr [%u,-]: circular buf msw=0x%lx,lsw=0x%lx, size=%lu, prop_flag=%lx,num_reg=%d, first reg (0x%lx,0x%lx, %lu)",
			pSession->ucInternalSessionId,  pOpenCmd->shared_circ_buf_start_phy_addr_msw,
			pOpenCmd->shared_circ_buf_start_phy_addr_lsw, pOpenCmd->shared_circ_buf_size, pOpenCmd->shared_circ_buf_property_flag,
			pOpenCmd->shared_circ_buf_num_regions, pRegion->shm_addr_msw, pRegion->shm_addr_lsw, pRegion->mem_size_bytes);

	// Cache is not used since DSP writes
	result = ASM_MemMapHandler(asmMemoryMapClient, pOpenCmd->shared_circ_buf_mem_pool_id,
			pOpenCmd->shared_circ_buf_property_flag, FALSE, pOpenCmd->shared_circ_buf_num_regions,
			pRegion, &(pStream->mem_map_handle[ASM_CIRC_BUF_INDEX]));
	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: push mode: Shared circ buffer could not be mapped.",
				pSession->ucInternalSessionId,  stream_id );

		goto __bailout;
	}

	if (ADSP_FAILED(result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(asmMemoryMapClient,pStream->mem_map_handle[ASM_CIRC_BUF_INDEX],
			pOpenCmd->shared_circ_buf_start_phy_addr_lsw,
			pOpenCmd->shared_circ_buf_start_phy_addr_msw,
			&sharedCircBufVirtAddr)))
	{
		MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AudioStreamMgr [%u,%u]: push mode: Phy to Virt Failed(paddr,vaddr)-->(%lx%lx,%lx)\n",
				pSession->ucInternalSessionId,  stream_id,
				pOpenCmd->shared_circ_buf_start_phy_addr_msw,
				pOpenCmd->shared_circ_buf_start_phy_addr_lsw,
				sharedCircBufVirtAddr);

		goto __bailout;
	}

	pOpenCmdBytePtr += sizeof(avs_shared_map_region_payload_t) * pOpenCmd->shared_circ_buf_num_regions;
	pWaterMarkLevels = (asm_push_mode_watermark_level_t*) pOpenCmdBytePtr;

	if (ADSP_FAILED(result = AudioStreamMgr_OpenStreamCommonHandler(pMe, pSession, pStream, &openStream)))
	{
		goto __bailout;
	}

	//Set session properties - stream format and Rx/Tx mode
	pSession->unMode = ASM_TX_PUSH_MODE;

	asm_topo = ASM_TOPO_TX_PUSH;
	if (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pStream->stream_perf_mode)
	{
		asm_topo = ASM_TOPO_TX_PUSH_LLNP;
	}
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: ASM stream topo %u",pSession->ucInternalSessionId, stream_id, asm_topo);

	//Establish stream topology
	if (ADSP_FAILED(result = ASM_init_stream_topo(asm_topo, pStream)))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream topo init failed.",
				pSession->ucInternalSessionId,  pStream->ucStreamId);
		goto __bailout;
	}

	if (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pStream->stream_perf_mode)
	{
		result = AudioStreamMgr_CreatePPSvc(pMe, pSession, pStream, &openStream); //TODO: need to use pEncCfg->sample_rate. right now, issue with matrix.
		if (ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to create Pre-Proc service",
					pSession->ucInternalSessionId,  stream_id);
			goto __bailout;
		}
	}

	{
		ASM_GraphNode *pEncNode = &(pStream->nodes[ASM_NODE_ENC]);

		uint32_t client_token  = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_ENC);

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Creating Encoder Service",
				pSession->ucInternalSessionId,  stream_id);

		// compose call back data for encsvc to raise event or send back to ASM.
		AudioStreamMgr_CallBackHandleType cbData;
		result = AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, client_token, &cbData);

		if (ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to create call back for Encoder service",
					pSession->ucInternalSessionId,  stream_id);

			goto __bailout;
		}

		/// This pointer is provided to enc svc for initialization.
		AudioEncSvcInitParams_t encInitParams;
		memset(&encInitParams, 0, sizeof(AudioEncSvcInitParams_t));

		encInitParams.encMode = ENC_SVC_PUSH_MODE;
		encInitParams.ulConfigId = openStream.sink_fmt_id;
		// Send the metadata request to enc svc in InitParams structure
		encInitParams.bFrameMetaDataEnable = FALSE;
		encInitParams.pCbData = &cbData;

		encInitParams.bits_per_sample = openStream.bits_per_sample;
		encInitParams.ulDataLogId = pStream->unDataLogId;
		encInitParams.frames_per_buffer = 1;
		encInitParams.no_apr_enc = FALSE;
		encInitParams.perf_mode = pStream->stream_perf_mode;

		encInitParams.push_mode.shared_circ_buf_addr = sharedCircBufVirtAddr;
		encInitParams.push_mode.pos_buf_ptr = (asm_push_mode_read_position_buffer_t*)sharedPosBufVirtAddr;
		encInitParams.push_mode.shared_circ_buf_size = sharedCircBufSize;
		encInitParams.push_mode.num_water_mark_levels = pOpenCmd->num_watermark_levels;
		encInitParams.push_mode.water_mark_levels_ptr = pWaterMarkLevels;

		encInitParams.pEventQ = pStSession->pSessionRespQ;
		encInitParams.eventToken = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_ENC);
		encInitParams.io_fmt_conv = ENC_SVC_IO_FORMAT_CONV_TYPE_AUTO;

		memscpy(&encInitParams.push_mode.EncCfg.pcm, sizeof(encInitParams.push_mode.EncCfg.pcm), pEncCfg, sizeof(asm_multi_channel_pcm_enc_cfg_v3_t));

		if (openStream.sink_fmt_id == ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
		{
		    encInitParams.push_mode.EncCfg.pcm.endianness = 0;
		}
		else
		{
		    encInitParams.push_mode.EncCfg.pcm.endianness = ((asm_multi_channel_pcm_enc_cfg_v4_t*)pEncCfg)->endianness;
		}


		result = AudioEncSvc_Create ( (uint32_t) (&encInitParams), (void**)(&pEncNode->pSvcHandle));

		if (ADSP_FAILED(result))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Encoder Service Create failed",
					pSession->ucInternalSessionId,  stream_id);
			goto __bailout;
		}

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Encoder Service Created",
				pSession->ucInternalSessionId,  stream_id);

		pEncNode->threadId = pEncNode->pSvcHandle->threadId;
	}

	/* Request Tx Handle as part of creation process */
	result = AudioStreamMgr_SessionRequestTxDeviceHandle(pMe, pSession, pStream, ASM_RELATIVE_TIMESTAMP);

	if (ADSP_FAILED(result))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudStreamMgr [%u,%u]: FAILED to request Tx Matrix handle",
				pSession->ucInternalSessionId,  stream_id);
		goto __bailout;
	}

	result = AudioStreamMgr_PostOpenStreamCommonHandler(pMe, pSession, pStream);
	if (ADSP_FAILED(result))
	{
		goto __bailout;
	}

	return ADSP_EOK;

	__bailout:
	AudioStreamMgr_OpenStreamBailout(pMe, pSession, pStream, pPkt, result);

	return result;
}

