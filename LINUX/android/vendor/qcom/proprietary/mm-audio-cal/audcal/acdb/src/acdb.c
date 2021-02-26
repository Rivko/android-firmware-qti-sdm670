/*===========================================================================
    FILE:           acdb.c

    OVERVIEW:       This file contains the implementaion of the Acdb API and
                    its helper methods.

    DEPENDENCIES:   None

                    Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
                    All Rights Reserved.
                    Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */

/*===========================================================================
    EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order. Please
    use ISO format for dates.

    $ $Header: //source/qcom/qct/multimedia2/Audio/audcal4/acdb_sw/main/latest/acdb/src/acdb.c#20 $

    when        who     what, where, why
    ----------  ---     -----------------------------------------------------
	2015-06-29 mahadev  Added AVCS custom topology support
    2014-05-28  mh      Removed KW warnings and errors
    2014-02-14  avi     Support SET APIs for AudProc, AudStrm TABLE.
    2013-08-07  avi     Support Fluence VP3 interfaces
    2013-07-18  mh      Added new commands for getting device property and its size
    2013-06-07  avi     Support Voice Volume boost.
    2010-09-21  ernanl  Enable ADIE calibration support.
========================================================================== */

/* ---------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------- */
#include "acdb_command.h"
#include "acdb.h"

/* ---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Externalized Function Definitions
 *--------------------------------------------------------------------------- */
int32_t acdb_ioctl (uint32_t nCommandId,
                    const uint8_t *pCommandStruct,
                    uint32_t nCommandStructLength,
                    uint8_t *pResponseStruct,
                    uint32_t nResponseStructLength)
{
   int32_t result = ACDB_SUCCESS;

#ifdef ACDB_DEBUG_L1_LOG
   ACDB_DEBUG_LOG("[acdb]->[acdb_ioctl]->Command Id[%08lX]\n",nCommandId);
#endif

   switch (nCommandId)
   {
   case ACDB_CMD_INITIALIZE_V2:
	   if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbInitCmdType) ||
         pResponseStruct != NULL || nResponseStructLength != 0)
      {
         result = ACDB_BADPARM;
      }
      else
      {
		  AcdbInitCmdType *pInput = (AcdbInitCmdType *)pCommandStruct;
          result = AcdbCmdInitializeAcdb (pInput);
      }
      break;
   case ACDB_CMD_RESET:
      if (pCommandStruct != NULL || nCommandStructLength != 0 ||
         pResponseStruct != NULL || nResponseStructLength != 0)
      {
         result = ACDB_BADPARM;
      }
      else
      {
         result = AcdbCmdSystemReset();
      }
      break;
   case ACDB_CMD_GET_ACDB_SW_VERSION:
      if (pCommandStruct != NULL || nCommandStructLength != 0 ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbModuleVersionType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbModuleVersionType *pSwVrs = (AcdbModuleVersionType *)pResponseStruct;
         result = AcdbCmdGetAcdbSwVersion (pSwVrs);
      }
      break;
   case ACDB_CMD_GET_COMMON_DEVICE_INFO:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbDeviceInfoCmnCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbDeviceInfoCmnCmdType *pInput = (AcdbDeviceInfoCmnCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetCmnDeviceInfo (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbDeviceInfoTargetSpecificCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbDeviceInfoTargetSpecificCmdType *pInput = (AcdbDeviceInfoTargetSpecificCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetTSDeviceInfo (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_DEVICE_CAPABILITIES:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbDeviceCapabilitiesCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbDeviceCapabilitiesCmdType *pInput = (AcdbDeviceCapabilitiesCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetDeviceCapabilities (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_IS_DEVICE_PAIRED:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbDevicePairType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbDevicePairingResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbDevicePairType *pInput = (AcdbDevicePairType *)pCommandStruct;
         AcdbDevicePairingResponseType *pOutput = (AcdbDevicePairingResponseType *)pResponseStruct;
         result = AcdbCmdGetDevicePair (pInput, pOutput);
      }
      break;
   case ACDB_CMD_GET_VOL_TABLE_STEP_SIZE:
      if (pCommandStruct != NULL || nCommandStructLength != 0 ||
          pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbVolTblStepSizeRspType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVolTblStepSizeRspType *pOutput = (AcdbVolTblStepSizeRspType *)pResponseStruct;
         result = AcdbCmdGetVolTableStepSize (pOutput);
      }
      break;
   case ACDB_CMD_GET_ANC_TX_DEVICE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAncDevicePairCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbAncDevicePairRspType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAncDevicePairCmdType *pInput = (AcdbAncDevicePairCmdType *) pCommandStruct;
         AcdbAncDevicePairRspType *pOutput = (AcdbAncDevicePairRspType *) pResponseStruct;
         result = AcdbCmdGetANCDevicePair (pInput, pOutput);
      }
      break;
   case ACDB_CMD_GET_AUDPROC_COMMON_TOPOLOGY_ID:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetAudProcTopIdCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbGetTopologyIdRspType))
      {
		 result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetAudProcTopIdCmdType *pInput = (AcdbGetAudProcTopIdCmdType *)pCommandStruct;
         AcdbGetTopologyIdRspType *pOutput = (AcdbGetTopologyIdRspType *)pResponseStruct;
         result = AcdbCmdGetAudProcCmnTopId (pInput, pOutput);
      }
      break;
   case ACDB_CMD_GET_VOCPROC_COMMON_TOPOLOGY_ID:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetVocProcTopIdCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbGetTopologyIdRspType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetVocProcTopIdCmdType *pInput = (AcdbGetVocProcTopIdCmdType *)pCommandStruct;
         AcdbGetTopologyIdRspType *pOutput = (AcdbGetTopologyIdRspType *)pResponseStruct;
         result = AcdbCmdGetVocProcCmnTopId (pInput, pOutput);
      }
      break;
   case ACDB_CMD_GET_AUDPROC_STREAM_TOPOLOGY_ID:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetAudProcStrmTopIdCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbGetTopologyIdRspType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetAudProcStrmTopIdCmdType *pInput = (AcdbGetAudProcStrmTopIdCmdType *)pCommandStruct;
         AcdbGetTopologyIdRspType *pOutput = (AcdbGetTopologyIdRspType *)pResponseStruct;
         result = AcdbCmdGetAudProcStrmTopId (pInput, pOutput);
      }
      break;
	case ACDB_CMD_GET_AFE_TOPOLOGY_ID:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetAfeTopIdCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbGetTopologyIdRspType))
      {
		 result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetAfeTopIdCmdType *pInput = (AcdbGetAfeTopIdCmdType *)pCommandStruct;
         AcdbGetTopologyIdRspType *pOutput = (AcdbGetTopologyIdRspType *)pResponseStruct;
         result = AcdbCmdGetAfeTopId (pInput, pOutput);
      }
      break;
	case ACDB_CMD_GET_LSM_TOPOLOGY_ID:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetLsmTopIdCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbGetTopologyIdRspType))
      {
		 result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetLsmTopIdCmdType *pInput = (AcdbGetLsmTopIdCmdType *)pCommandStruct;
         AcdbGetTopologyIdRspType *pOutput = (AcdbGetTopologyIdRspType *)pResponseStruct;
         result = AcdbCmdGetLsmTopId (pInput, pOutput);
      }
      break;
 	case ACDB_CMD_GET_COMPATIBLE_REMOTE_DEVICE_ID:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetRmtCompDevIdCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbGetRmtCompDevIdRspType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetRmtCompDevIdCmdType *pInput = (AcdbGetRmtCompDevIdCmdType *)pCommandStruct;
         AcdbGetRmtCompDevIdRspType *pOutput = (AcdbGetRmtCompDevIdRspType *)pResponseStruct;
         result = AcdbCmdGetCompRemoteDevId (pInput, pOutput);
      }
      break;
	case ACDB_CMD_GET_AUDIO_RECORD_RX_DEVICE_LIST:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbAudioRecRxListCmdType) ||
		  pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbAudioRecRxListRspType))
		{
			result = ACDB_BADPARM;
		}
		else
		{
			AcdbAudioRecRxListCmdType *pInput = (AcdbAudioRecRxListCmdType *)pCommandStruct;
			AcdbAudioRecRxListRspType  *pOutput = (AcdbAudioRecRxListRspType *)pResponseStruct;
			result = AcdbCmdGetRecordRxDeviceList (pInput, pOutput);
		}
		break;
   case ACDB_CMD_GET_AUDPROC_COMMON_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcCmdType *pInput = (AcdbAudProcCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetAudProcInfo (DATA_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_SET_AUDPROC_COMMON_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcCmdType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcCmdType *pInput = (AcdbAudProcCmdType *)pCommandStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdSetAudProcData (pInput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDPROC_COMMON_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcTableCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcTableCmdType *pInput = (AcdbAudProcTableCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetAudProcInfo (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDPROC_COMMON_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcTableSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcTableSizeCmdType *pInput = (AcdbAudProcTableSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAudProcInfo (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_SET_AUDPROC_COMMON_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcTableCmdType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcTableCmdType *pInput = (AcdbAudProcTableCmdType *)pCommandStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdSetAudProcInfo (pInput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDPROC_GAIN_DEP_STEP_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcGainDepVolTblStepCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcGainDepVolTblStepCmdType *pInput = (AcdbAudProcGainDepVolTblStepCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetAudProcGainDepStepInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDPROC_GAIN_DEP_STEP_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcGainDepVolTblStepSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcGainDepVolTblStepSizeCmdType *pInput = (AcdbAudProcGainDepVolTblStepSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAudProcGainDepStepInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_AUDPROC_VOL_STEP_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcGainDepVolTblStepCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcGainDepVolTblStepCmdType *pInput = (AcdbAudProcGainDepVolTblStepCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAudProcVolStepInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDPROC_VOL_STEP_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudProcGainDepVolTblStepSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudProcGainDepVolTblStepSizeCmdType *pInput = (AcdbAudProcGainDepVolTblStepSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAudProcVolStepInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;

   case ACDB_CMD_GET_AUDPROC_STREAM_DATA_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudStrmV2CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudStrmV2CmdType *pInput = (AcdbAudStrmV2CmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetAudStreamInfo(DATA_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;

   case ACDB_CMD_SET_AUDPROC_STREAM_DATA_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudStrmV2CmdType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudStrmV2CmdType *pInput = (AcdbAudStrmV2CmdType *)pCommandStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdSetAudStreamData (pInput);
		 }
      }
      break;

   case ACDB_CMD_GET_AUDPROC_STREAM_TABLE_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudStrmTableV2CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudStrmTableV2CmdType *pInput = (AcdbAudStrmTableV2CmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAudStreamInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_SET_AUDPROC_STREAM_TABLE_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudStrmTableV2CmdType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudStrmTableV2CmdType *pInput = (AcdbAudStrmTableV2CmdType *)pCommandStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdSetAudStreamInfo(pInput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDPROC_STREAM_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAudStrmTblSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAudStrmTblSizeCmdType *pInput = (AcdbAudStrmTblSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAudStreamInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_COMMON_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcCmnTblCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcCmnTblCmdType *pInput = (AcdbVocProcCmnTblCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocProcInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_COMMON_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcCmnTblSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcCmnTblSizeCmdType *pInput = (AcdbVocProcCmnTblSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocProcInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;

   case ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcGainDepVolTblV2CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcGainDepVolTblV2CmdType *pInput = (AcdbVocProcGainDepVolTblV2CmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocProcDynInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
      case ACDB_CMD_GET_VOC_PROC_STATIC_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcCmnTblCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcCmnTblCmdType *pInput = (AcdbVocProcCmnTblCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocProcStatInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcGainDepVolTblSizeV2CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcGainDepVolTblSizeV2CmdType *pInput = (AcdbVocProcGainDepVolTblSizeV2CmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocProcDynInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
       case ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcCmnTblSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcCmnTblSizeCmdType *pInput = (AcdbVocProcCmnTblSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocProcStatInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcGainDepVolTblCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcGainDepVolTblCmdType *pInput = (AcdbVocProcGainDepVolTblCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
         AcdbVocProcGainDepVolTblV2CmdType pInputV2;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 pInputV2.nRxDeviceId = pInput->nRxDeviceId;
			 pInputV2.nTxDeviceId = pInput->nTxDeviceId;
			 pInputV2.nFeatureId = ACDB_VOCVOL_FID_DEFAULT;
			 pInputV2.nBufferPointer = pInput->nBufferPointer;
			 pInputV2.nBufferLength = pInput->nBufferLength;
			 result = AcdbCmdGetVocProcVolV2Info(TABLE_CMD,((uint8_t *)&pInputV2), (uint8_t *)pOutput);
			 if(result != ACDB_SUCCESS)
			 {
				result = AcdbCmdGetVocProcVolInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
			 }
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcGainDepVolTblSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcGainDepVolTblSizeCmdType *pInput = (AcdbVocProcGainDepVolTblSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;

         AcdbVocProcGainDepVolTblSizeV2CmdType pInputV2;

         pInputV2.nRxDeviceId = pInput->nRxDeviceId;
         pInputV2.nTxDeviceId = pInput->nTxDeviceId;
         pInputV2.nFeatureId = ACDB_VOCVOL_FID_DEFAULT;

         result = AcdbCmdGetVocProcVolV2Info(TABLE_SIZE_CMD,((uint8_t *)&pInputV2), (uint8_t *)pOutput);

         if(result != ACDB_SUCCESS)
         {
            result = AcdbCmdGetVocProcVolInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
         }
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcGainDepVolTblV2CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcGainDepVolTblV2CmdType *pInput = (AcdbVocProcGainDepVolTblV2CmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocProcVolV2Info(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_SIZE_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcGainDepVolTblSizeV2CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
	   {
		   result = ACDB_BADPARM;
	   }
       else
       {
            AcdbVocProcGainDepVolTblSizeV2CmdType *pInput = (AcdbVocProcGainDepVolTblSizeV2CmdType *)pCommandStruct;
			AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
			result = AcdbCmdGetVocProcVolV2Info(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
       }
      break;
   case ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbQueryCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbQueryCmdType *pInput = (AcdbQueryCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocStreamInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;

     case ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbQueryCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbQueryCmdType *pInput = (AcdbQueryCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocStream2Info(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE_SIZE:
      if (pCommandStruct != NULL || nCommandStructLength != 0 ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocStreamInfo(TABLE_SIZE_CMD,NULL, (uint8_t *)pOutput);
      }
      break;
    case ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE:
      if (pCommandStruct != NULL || nCommandStructLength != 0 ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocStream2Info(TABLE_SIZE_CMD,NULL, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_CODEC_ANC_SETTING:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbCodecANCSettingCmdType) ||
         pResponseStruct == NULL || nResponseStructLength == 0)
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbCodecANCSettingCmdType *pInput = (AcdbCodecANCSettingCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAdieANCDataTable (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_ADIE_CODEC_PATH_PROFILE_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAdiePathProfileV2CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAdiePathProfileV2CmdType *pInput = (AcdbAdiePathProfileV2CmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAdieProfileTable (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_GLBTBL_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGblTblCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbGblTblCmdType *pInput = (AcdbGblTblCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetGlobalTable (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AFE_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAfeDataCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAfeDataCmdType *pInput = (AcdbAfeDataCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAfeData (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AFE_COMMON_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAfeCmnDataCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAfeCmnDataCmdType *pInput = (AcdbAfeCmnDataCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAfeCmnInfo (DATA_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AFE_COMMON_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAfeCommonTableCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAfeCommonTableCmdType *pInput = (AcdbAfeCommonTableCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAfeCmnInfo (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AFE_COMMON_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAfeCommonTableSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAfeCommonTableSizeCmdType *pInput = (AcdbAfeCommonTableSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAfeCmnInfo (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_DEVICE_CFG:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcDevCfgCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcDevCfgCmdType *pInput = (AcdbVocProcDevCfgCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBuff == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocprocDevCfgInfo (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_PROC_DEVICE_CFG_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocProcDevCfgSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocProcDevCfgSizeCmdType *pInput = (AcdbVocProcDevCfgSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocprocDevCfgInfo (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_VOC_COLUMNS_INFO:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocColumnsInfoCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocColumnsInfoCmdType *pInput = (AcdbVocColumnsInfoCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBuff == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocColInfo (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocColumnsInfoSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocColumnsInfoSizeCmdType *pInput = (AcdbVocColumnsInfoSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocColInfo (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;

      case ACDB_CMD_GET_VOC_COLUMNS_INFO_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocColumnsInfoCmdType_v2) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocColumnsInfoCmdType_v2 *pInput = (AcdbVocColumnsInfoCmdType_v2 *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBuff == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVocColInfo_v2(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE_V2:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVocColumnsInfoSizeCmdType_v2) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVocColumnsInfoSizeCmdType_v2 *pInput = (AcdbVocColumnsInfoSizeCmdType_v2 *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetVocColInfo_v2(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_FEATURE_SUPPORTED_DEVICE_LIST:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbFeatureSupportedDevListCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbFeatureSupportedDevListCmdType *pInput = (AcdbFeatureSupportedDevListCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetFeatureSupportedDevList (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_DEVICE_PAIR_LIST:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbDevicePairListCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbDevicePairListCmdType *pInput = (AcdbDevicePairListCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetDevPairList(pInput,pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_LSM_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbLsmTableCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbLsmTableCmdType *pInput = (AcdbLsmTableCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetLSMTblInfo (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_LSM_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbLsmTableSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbLsmTableSizeCmdType *pInput = (AcdbLsmTableSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetLSMTblInfo (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_CODEC_CAL_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbCodecCalDataCmdType) ||
         pResponseStruct == NULL || nResponseStructLength == 0)
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbCodecCalDataCmdType *pInput = (AcdbCodecCalDataCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetCodecFeaturesData(pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDIO_COPP_TOPOLOGIES:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbQueryCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbQueryCmdType *pInput = (AcdbQueryCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAudioCOPPTopologyData(pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDIO_COPP_TOPOLOGIES_SIZE:
      if (pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAudioCOPPTopologyDataSize(pOutput);
      }
      break;
    case ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO:
    if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbQueryCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbQueryCmdType *pInput = (AcdbQueryCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAVCSCustomTopoInfo(pInput, pOutput);
		 }
      }
      break;
    case ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO_SIZE:
      if (pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAVCSCustomTopoInfoSize(pOutput);
      }
      break;
   case ACDB_CMD_GET_AUDIO_POPP_TOPOLOGIES:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbQueryCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbQueryCmdType *pInput = (AcdbQueryCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAudioPOPPTopologyData(pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AUDIO_POPP_TOPOLOGIES_SIZE:
      if (pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAudioPOPPTopologyDataSize(pOutput);
      }
      break;
	case ACDB_CMD_GET_AFE_TOPOLOGIES:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbQueryCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbQueryCmdType *pInput = (AcdbQueryCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAfeTopologyData(pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_AFE_TOPOLOGIES_SIZE:
      if (pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAfeTopologyDataSize(pOutput);
      }
      break;
   case ACDB_CMD_GET_LSM_TOPOLOGIES:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbQueryCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbQueryCmdType *pInput = (AcdbQueryCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetLsmTopologyData(pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_LSM_TOPOLOGIES_SIZE:
      if (pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetLsmTopologyDataSize(pOutput);
      }
      break;
   case ACDB_CMD_GET_ADIE_SIDETONE_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAdieSidetoneTableCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAdieSidetoneTableCmdType *pInput = (AcdbAdieSidetoneTableCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAdieSidetoneTblInfo(TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;

   case ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAANCConfigTableCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAANCConfigTableCmdType *pInput = (AcdbAANCConfigTableCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAANCTblCmnInfo (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAANCConfigTableSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAANCConfigTableSizeCmdType *pInput = (AcdbAANCConfigTableSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetAANCTblCmnInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbAANCConfigDataCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbAANCConfigDataCmdType *pInput = (AcdbAANCConfigDataCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetAANCTblCmnInfo(DATA_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_DEVICE_PROPERTY:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbDevPropCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbDevPropCmdType *pInput = (AcdbDevPropCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->pBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetDeviceProperty(DATA_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_DEVICE_PROPERTY_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbDevPropSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbDevPropSizeCmdType *pInput = (AcdbDevPropSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
         result = AcdbCmdGetDeviceProperty(DATA_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
      break;
   case ACDB_CMD_GET_VP3_MIDPID_LIST:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVP3MidPidListCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVP3MidPidListCmdType *pInput = (AcdbVP3MidPidListCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVP3MidPidList(pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_GET_VP3_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVP3CmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVP3CmdType *pInput = (AcdbVP3CmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetVP3Data (pInput, pOutput);
		 }
      }
      break;
   case ACDB_CMD_SET_VP3_DATA:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbVP3CmdType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbVP3CmdType *pInput = (AcdbVP3CmdType *)pCommandStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdSetVP3Data (pInput);
		 }
      }
      break;
    case ACDB_CMD_GET_ACDB_SW_VERSION_V2:
      if (pCommandStruct != NULL || nCommandStructLength != 0 ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbModuleVersionTypeV2))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbModuleVersionTypeV2 *pSwVrs = (AcdbModuleVersionTypeV2 *)pResponseStruct;
         result = AcdbCmdGetAcdbSwVersionV2 (pSwVrs);
      }
      break;

	case ACDB_CMD_GET_META_INFO_SIZE:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetMetaInfoSizeCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetMetaInfoSizeCmdType *pInput = (AcdbGetMetaInfoSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetMetaInfoSize (pInput, pOutput);
		 ACDB_DEBUG_LOG ("Output from GetMetaInfoSize command : %u\n", pOutput->nSize);
      }
      break;

	case ACDB_CMD_GET_META_INFO:
      if (pCommandStruct == NULL || nCommandStructLength != sizeof(AcdbGetMetaInfoCmdType) ||
          pResponseStruct == NULL || nResponseStructLength != sizeof(AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbGetMetaInfoCmdType *pInput = (AcdbGetMetaInfoCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			 result = AcdbCmdGetMetaInfo (pInput, pOutput);
			 ACDB_DEBUG_LOG ("Bytes copied in output buffer from GetMetaInfo command : %u\n", pOutput->nBytesUsedInBuffer);
		 }
      }
      break;
	case ACDB_CMD_GET_ACDB_SW_VERSION_V3:
		if (pCommandStruct != NULL || nCommandStructLength != 0 || nResponseStructLength != sizeof(AcdbModuleVersionTypeV3) || pResponseStruct == NULL)
		{
			result = ACDB_BADPARM;
		}
		else
		{
			AcdbModuleVersionTypeV3 *pSwVrs = (AcdbModuleVersionTypeV3 *)pResponseStruct;
			result = AcdbCmdGetAcdbSwVersionV3(pSwVrs);
		}
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_LIST_SIZE:
		 if (pCommandStruct == NULL || nCommandStructLength != sizeof (WdspSubgraphListSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         WdspSubgraphListSizeCmdType *pInput = (WdspSubgraphListSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
         result = AcdbCmdGetWdspSubgraphList(DATA_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_LIST:
		 if (pCommandStruct == NULL || nCommandStructLength != sizeof (WdspSubgraphListCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         WdspSubgraphListCmdType *pInput = (WdspSubgraphListCmdType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetWdspSubgraphList (DATA_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_INFO_LIST_SIZE:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (WdspDidUcidSubGraphIDSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         WdspDidUcidSubGraphIDSizeCmdType *pInput = (WdspDidUcidSubGraphIDSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetWdspSubGraphModuleInfo(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_INFO_LIST:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbWdspDidUcidSubGraphIDType) ||
			pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
		{
			result = ACDB_BADPARM;
		}
		else
		{
			AcdbWdspDidUcidSubGraphIDType *pInput = (AcdbWdspDidUcidSubGraphIDType *)pCommandStruct;
			AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
			if(pInput->nBufferPointer == NULL)
			{
				result = ACDB_BADPARM;
			}
			else
			{
				result = AcdbCmdGetWdspSubGraphModuleInfo (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
			}
		}
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CONNECTION_SIZE:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (WdspDidUcidSubGraphIDSizeCmdType) ||
			pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
		{
			result = ACDB_BADPARM;
		}
		else
		{
			WdspDidUcidSubGraphIDSizeCmdType *pInput = (WdspDidUcidSubGraphIDSizeCmdType *)pCommandStruct;
			AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
			result = AcdbCmdGetWdspModuleConnections (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		}
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CONNECTION:
		 if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbWdspDidUcidSubGraphIDType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbWdspDidUcidSubGraphIDType *pInput = (AcdbWdspDidUcidSubGraphIDType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetWdspModuleConnections (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_CONNECTIONS_SIZE:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (WdspSubgraphConnectionsSizeCmdType) ||
			pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
		{
			result = ACDB_BADPARM;
		}
		else
		{
			WdspSubgraphConnectionsSizeCmdType *pInput = (WdspSubgraphConnectionsSizeCmdType *)pCommandStruct;
			AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
			result = AcdbCmdGetWdspSubGraphConnections (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		}
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPHS_CONNECTION:
		 if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbWdspDidUcidSubGraphIDPairType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbWdspDidUcidSubGraphIDPairType *pInput = (AcdbWdspDidUcidSubGraphIDPairType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetWdspSubGraphConnections (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CALIBRATION_SIZE:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (WdspDidUcidSubGraphIDSizeCmdType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         WdspDidUcidSubGraphIDSizeCmdType *pInput = (WdspDidUcidSubGraphIDSizeCmdType *)pCommandStruct;
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetWdspModuleCalibration(TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
      }
		break;
	case ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CALIBRATION:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbWdspDidUcidSubGraphIDType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbWdspDidUcidSubGraphIDType *pInput = (AcdbWdspDidUcidSubGraphIDType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetWdspModuleCalibration (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
		break;
	case ACDB_WDSP_CMD_GET_BOOT_TIME_MODULE_LIST_SIZE:
		if (pCommandStruct != NULL|| pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
		 result = AcdbCmdGetWdspBootTimeModulesList (TABLE_SIZE_CMD, NULL, (uint8_t *)pOutput);
      }
		break;
	case ACDB_WDSP_CMD_GET_BOOT_TIME_MODULE_LIST:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbWdspBootTimeModuleReqType) ||
         pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbQueryResponseType))
      {
         result = ACDB_BADPARM;
      }
      else
      {
         AcdbWdspBootTimeModuleReqType *pInput = (AcdbWdspBootTimeModuleReqType *)pCommandStruct;
         AcdbQueryResponseType *pOutput = (AcdbQueryResponseType *)pResponseStruct;
		 if(pInput->nBufferPointer == NULL)
		 {
			 result = ACDB_BADPARM;
		 }
		 else
		 {
			result = AcdbCmdGetWdspBootTimeModulesList (TABLE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		 }
      }
		break;
	case ACDB_WDSP_CMD_GET_MODULE_LOADING_META_INFO_SIZE:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (WdspGetDynamicModuleInfoSizeCmdType) ||
			pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
		{
			result = ACDB_BADPARM;
		}
		else
		{
			WdspGetDynamicModuleInfoSizeCmdType *pInput = (WdspGetDynamicModuleInfoSizeCmdType *)pCommandStruct;
			AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
			result = AcdbCmdGetWdspDynamicModuleMetaInfo (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		}
		break;
	case ACDB_WDSP_CMD_GET_MODULE_LOADING_META_INFO:
		if (pCommandStruct == NULL || nCommandStructLength != sizeof (AcdbWdspGetDynamicModuleReqType) ||
			pResponseStruct == NULL || nResponseStructLength != sizeof (AcdbSizeResponseType))
		{
			result = ACDB_BADPARM;
		}
		else
		{
			AcdbWdspGetDynamicModuleReqType *pInput = (AcdbWdspGetDynamicModuleReqType *)pCommandStruct;
			AcdbSizeResponseType *pOutput = (AcdbSizeResponseType *)pResponseStruct;
			result = AcdbCmdGetWdspDynamicModuleMetaInfo (TABLE_SIZE_CMD,(uint8_t *)pInput, (uint8_t *)pOutput);
		}
		break;
   default:
      result = ACDB_NOTSUPPORTED;
	  ACDB_DEBUG_LOG("Received unsupported command request with cmd id %08X \n",nCommandId);
      break;
   }
   if(result == ACDB_BADPARM)
   {
	   ACDB_DEBUG_LOG("Received Bad Params for command request with cmd id %08X \n",nCommandId);
   }
   else if(result == ACDB_INSUFFICIENTMEMORY)
   {
	   ACDB_DEBUG_LOG("Received command request with cmd id %08X with insufficient memory\n",nCommandId);
   }
   return result;
}
