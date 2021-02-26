#ifndef __ACDB_COMMAND_H__
#define __ACDB_COMMAND_H__
/*===========================================================================
    @file   acdb_command.h

    This file contains the implementation of the Acdb public interface.

    This file contains the implementation of the commands exposed by the
    Acdb public interface.

                    Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
                    All Rights Reserved.
                    Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/* $Header: //source/qcom/qct/multimedia2/Audio/audcal4/acdb_sw/main/latest/acdb/src/acdb_command.h#17 $ */

/*===========================================================================
    EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order. Please
    use ISO format for dates.

    when        who     what, where, why
    ----------  ---     -----------------------------------------------------
    2016-17-05 mahadevk Alignment fix for enabling CLANG 64bit compiler
    2015-14-09  San	   Add AcdbCmdGetAcdbSwVersionV3 API to support ACDB_CMD_GET_ACDB_SW_VERSION_V3 command.
    2014-02-14  avi     Support ACDB persistence and SET APIs for AudProc, AudStrm TABLE.
    2013-08-07  avi     Support Fluence VP3 interfaces
    2013-07-18  mh      Added new query type for data size
    2013-05-23  avi     Support VocProcVolV2 table (part of Volume boost feature)
    2010-07-23  ernanl  Complete get/set function and associated functions API
    2010-07-01  vmn     Cleaned up the api implementations.
    2010-06-10  aas     Initial revision.
	2012-05-15 kpavan   1. New interface for APQ MDM device Mapping.
						2. New interface to get TX RX Device pair for Recording
===========================================================================*/

/* ---------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------- */
#include "acdb.h"
#include "acdb_private.h"
#include "acdb_datainfo.h"
/* ---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------- */
enum GetQueryType{
   TABLE_CMD,
   TABLE_SIZE_CMD,
   DATA_CMD,
   DATA_SIZE_CMD
};

enum PidType{
	PID_FWK_MODULE_INFO_LIST,
	PID_FWK_CONNECT,
	PID_FWK_DISCONNECT
};

typedef struct _AcdbCmdGetFileDataReqType AcdbCmdGetFileDataReq;
#include "acdb_begin_pack.h"
struct _AcdbCmdGetFileDataReqType
{
	uint32_t nfile_offset;
	uint32_t nfile_data_len;
	uint32_t nfileNameLen;
	uint8_t *pFileName;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbCmdRespType AcdbCmdResp;
#include "acdb_begin_pack.h"
struct _AcdbCmdRespType
{
	uint32_t nresp_buff_len;
	uint32_t nresp_buff_filled;
	uint8_t *pRespBuff;
}
#include "acdb_end_pack.h"
;
// Variable to enable/disable persistence from test framework.
// It is not expected to be used externally.

extern int32_t g_persistenceStatus;

/* ---------------------------------------------------------------------------
* Class Definitions
*--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------- */

int32_t AcdbCmdInitializeAcdb (AcdbInitCmdType *pInput);

int32_t AcdbCmdSystemReset(void);

int32_t AcdbCmdSaveDeltaFileData(void);

int32_t AcdbCmdGetAcdbSwVersion (AcdbModuleVersionType *pOutput);

int32_t AcdbCmdGetAcdbSwVersionV2 (AcdbModuleVersionTypeV2 *pOutput);

int32_t AcdbCmdGetCmnDeviceInfo (AcdbDeviceInfoCmnCmdType* pInput,
                                 AcdbQueryResponseType* pOutput
                                );

int32_t AcdbCmdGetTSDeviceInfo (AcdbDeviceInfoTargetSpecificCmdType* pInput,
                                AcdbQueryResponseType* pOutput
                                );

int32_t AcdbCmdGetDeviceCapabilities (AcdbDeviceCapabilitiesCmdType *pInput,
                                      AcdbQueryResponseType *pOutput
                                     );

int32_t AcdbCmdGetDevicePair (AcdbDevicePairType *pInput,
                              AcdbDevicePairingResponseType *pOutput);

int32_t AcdbCmdGetANCDevicePair (AcdbAncDevicePairCmdType *pInput,
                                 AcdbAncDevicePairRspType *pOutput);

int32_t AcdbCmdGetVolTableStepSize (AcdbVolTblStepSizeRspType *pOutput);

int32_t AcdbCmdGetAudProcCmnTopId (AcdbGetAudProcTopIdCmdType *pInput,AcdbGetTopologyIdRspType *pOutput);

int32_t AcdbCmdGetVocProcCmnTopId (AcdbGetVocProcTopIdCmdType *pInput,
                                   AcdbGetTopologyIdRspType *pOutput
                                   );
int32_t AcdbCmdGetAudProcStrmTopId (AcdbGetAudProcStrmTopIdCmdType *pInput,
                                    AcdbGetTopologyIdRspType *pOutput
                                    );
int32_t AcdbCmdGetAfeTopId (AcdbGetAfeTopIdCmdType *pInput,AcdbGetTopologyIdRspType *pOutput);

int32_t AcdbCmdGetLsmTopId (AcdbGetLsmTopIdCmdType *pInput,
   AcdbGetTopologyIdRspType *pOutput
   );

int32_t AcdbCmdGetCompRemoteDevId (AcdbGetRmtCompDevIdCmdType *pInput,
                                   AcdbGetRmtCompDevIdRspType *pOutput
                                   );
int32_t AcdbCmdGetRecordRxDeviceList (AcdbAudioRecRxListCmdType *pInput,
                                    AcdbAudioRecRxListRspType *pOutput) ;

int32_t AcdbCmdGetAudProcInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdSetAudProcData(AcdbAudProcCmdType *pInput);

int32_t AcdbCmdSetAudProcInfo(AcdbAudProcTableCmdType *pInput);

int32_t AcdbCmdGetAudProcGainDepStepInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetAudProcVolStepInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetAudStreamInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdSetAudStreamInfo(AcdbAudStrmTableV2CmdType *pInput);

int32_t AcdbCmdSetAudStreamData(AcdbAudStrmV2CmdType *pInput);

int32_t AcdbCmdGetVocProcInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);
int32_t AcdbCmdGetVocProcDynInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);
int32_t AcdbCmdGetVocProcStatInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);
int32_t AcdbCmdGetVocProcVolInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetVocProcVolV2Info(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetVocStreamInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);
int32_t AcdbCmdGetVocStream2Info(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);
int32_t AcdbCmdGetAdieANCDataTable (AcdbCodecANCSettingCmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAdieProfileTable (AcdbAdiePathProfileV2CmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetGlobalTable (AcdbGblTblCmdType *pInput,
                               AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAfeData (AcdbAfeDataCmdType *pInput,
                           AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAfeCmnInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetVocprocDevCfgInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetVocColInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);
int32_t AcdbCmdGetVocColInfo_v2(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);
int32_t AcdbCmdGetFilesInfo(AcdbQueryCmdType *pReq,AcdbQueryResponseType *pRsp);

int32_t AcdbCmdGetFileData(AcdbCmdGetFileDataReq *pReq,AcdbCmdResp *pRsp);

int32_t AcdbCmdGetOnlineData(uint32_t tblId,uint8_t *pIndices,uint32_t nIdxCount,uint32_t mid,uint32_t pid,
						  uint8_t *pBuff, uint32_t nBuffLen,uint32_t *pBuffBytesFilled);

int32_t AcdbCmdSetOnlineData(uint32_t persistData, const uint32_t tblId,uint8_t *pIndices,uint32_t nIdxCount,uint32_t mid,uint32_t pid,
						  uint8_t *pInBuff, uint32_t nInBuffLen);

int32_t AcdbCmdGetTblEntriesOnHeap(uint8_t *pCmd,uint32_t nCmdSize ,uint8_t *pRsp,uint32_t nRspSizse);

int32_t AcdbCmdGetNoOfTblEntriesOnHeap(uint8_t *pCmd,uint32_t nCmdSize ,uint8_t *pRsp,uint32_t nRspSizse);

int32_t AcdbCmdGetFeatureSupportedDevList(AcdbFeatureSupportedDevListCmdType *pCmd,AcdbQueryResponseType *pResp);

int32_t AcdbCmdGetDevPairList(AcdbDevicePairListCmdType *pCmd,AcdbQueryResponseType *pResp);

int32_t AcdbCmdGetLSMTblInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetCodecFeaturesData (AcdbCodecCalDataCmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAdieSidetoneTblInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetAudioCOPPTopologyData(AcdbQueryCmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAudioCOPPTopologyDataSize(AcdbSizeResponseType *pOutput);

int32_t AcdbCmdGetAVCSCustomTopoInfo(AcdbQueryCmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAVCSCustomTopoInfoSize(AcdbSizeResponseType *pOutput);

int32_t AcdbCmdGetAudioPOPPTopologyData(AcdbQueryCmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAudioPOPPTopologyDataSize(AcdbSizeResponseType *pOutput);

int32_t AcdbCmdGetAfeTopologyData(AcdbQueryCmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetAfeTopologyDataSize(AcdbSizeResponseType *pOutput);

int32_t AcdbCmdGetLsmTopologyData(AcdbQueryCmdType *pInput,
                                    AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetLsmTopologyDataSize(AcdbSizeResponseType *pOutput);

int32_t AcdbCmdGetAANCTblCmnInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetDeviceProperty(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t GetVP3InfoFromUseCaseId(int32_t useCaseId, uint32_t *tblId, int32_t *lutIndicesCount, int32_t *cmdIndicesCount, int32_t *tblIndicesCount);

int32_t GetMaxLenPrpty(MaxLenDefPrptyType *maxLenPrpty);

int32_t AcdbCmdGetVP3MidPidList(AcdbVP3MidPidListCmdType *pInput,
                                     AcdbQueryResponseType *pOutput);

int32_t AcdbCmdGetVP3Data(AcdbVP3CmdType *pIn,AcdbQueryResponseType *pOut);

int32_t AcdbCmdSetVP3Data(AcdbVP3CmdType *pInput);

int32_t AcdbCmdIsPersistenceSupported(uint32_t *resp);

void AcdbCmdEnablePersistence(void);

void AcdbCmdDisablePersistence(void);

int32_t AcdbCmdDeleteAllDeltaFiles(int32_t *resp);

int32_t AcdbCmdGetMetaInfoSize (AcdbGetMetaInfoSizeCmdType *pInput, AcdbSizeResponseType *pOutput);

int32_t AcdbCmdGetMetaInfo (AcdbGetMetaInfoCmdType *pInput, AcdbQueryResponseType *pOutput);
int32_t GetActualTableID(int32_t voctableid);

int32_t AcdbCmdGetAcdbSwVersionV3(AcdbModuleVersionTypeV3 *pOutput);

int32_t AcdbCmdGetWdspSubgraphList(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t WdspGetModuleInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut, uint32_t MID, uint32_t IID);

int32_t WdspGetSubGraphDefinitions(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetWdspModuleConnections(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetWdspSubGraphConnections(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetWdspSubGraphModuleInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetWdspModuleCalibration(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t WdspGetCalibrationData(uint32_t queryType,uint8_t *pIn,uint8_t *pOut, uint32_t MID, uint32_t IID);

int32_t AcdbCmdGetWdspBootTimeModulesList(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

int32_t AcdbCmdGetWdspDynamicModuleMetaInfo(uint32_t queryType,uint8_t *pIn,uint8_t *pOut);

uint32_t AcdbCmdWdspIsModulePresent( uint32_t queryType,uint8_t *pIn, uint32_t moduleID);

#endif /* __ACDB_COMMAND_H__ */
