#ifndef __ACDB_FILE_MGR_H__
#define __ACDB_FILE_MGR_H__
/*===========================================================================
    @file   acdb_data_mgr.h

    This file contains the implementation of the acdb file accessor interfaces.

                    Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
                    All Rights Reserved.
                    Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/* $Header: //source/qcom/qct/multimedia2/Audio/audcal4/acdb_sw/main/latest/acdb/src/acdb_file_mgr.h#13 $ */

/*===========================================================================
    EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order. Please
    use ISO format for dates.

    when        who     what, where, why
    ----------  ---     -----------------------------------------------------
    2014-05-28  mh      Updated some structure as a packed structure to avoid alignment issues
    2010-07-23  ernanl  Complete get/set function and associated functions API
===========================================================================*/

/* ---------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------- */
#include "acdb.h"

/* ---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------- */
#define ACDB_MAX_DEVICES 500

/* ---------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
* Class Definitions
*--------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------- */

typedef struct _AcdbCmdFileNameInfo AcdbCmdFileNameInfo;
#include "acdb_begin_pack.h"
struct _AcdbCmdFileNameInfo{
   uint32_t nFileNameLen;
   char chFileName[ACDB_FILENAME_MAX_CHARS];
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbCmdFileInfo AcdbCmdFileInfo;
#include "acdb_begin_pack.h"
struct _AcdbCmdFileInfo{
	char chFileName[ACDB_FILENAME_MAX_CHARS];
	uint32_t nFileNameLen;
	uint32_t nFileSize;
	uint8_t *pFileBuf;
}
#include "acdb_end_pack.h"
;

typedef struct _AcdbDataInfo AcdbDataInfo;
#include "acdb_begin_pack.h"
struct _AcdbDataInfo
{
	uint32_t nDataLen;
	uint8_t  *pData;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbDevPropInfo AcdbDevPropInfo;
#include "acdb_begin_pack.h"
struct _AcdbDevPropInfo{
	uint32_t devId;
	uint32_t pId;
	AcdbDataInfo dataInfo;
}
#include "acdb_end_pack.h"
;

typedef struct _WdspGetSubgraphListQry WdspGetSubgraphListQry;
#include "acdb_begin_pack.h"
struct _WdspGetSubgraphListQry{
	uint32_t devId;
	uint32_t usecaseId;
	AcdbDataInfo dataInfo;
}
#include "acdb_end_pack.h"
;

typedef struct _WdspGetSubgraphDefnQry WdspGetSubgraphDefnQry;
#include "acdb_begin_pack.h"
struct _WdspGetSubgraphDefnQry{
	uint32_t devId;
	uint32_t usecaseId;
	uint32_t subgraphId;
	AcdbDataInfo dataInfo;
}
#include "acdb_end_pack.h"
;

typedef struct _AcdbGlbalPropInfo AcdbGlbalPropInfo;
#include "acdb_begin_pack.h"
struct _AcdbGlbalPropInfo{
	uint32_t pId;
	AcdbDataInfo dataInfo;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbDevices AcdbDevices;
#include "acdb_begin_pack.h"
struct _AcdbDevices
{
	uint32_t   noOfDevs;
	uint32_t   devList[ACDB_MAX_DEVICES];
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbFileInfo AcdbFileInfo;
#include "acdb_begin_pack.h"
struct _AcdbFileInfo
{
   int32_t fileType;
   uint32_t maj;
   uint32_t min;
   uint32_t rev;
   uint32_t cplInfo;
   uint32_t noOfDevs;
   uint32_t *pDevList;
}
#include "acdb_end_pack.h"
;

typedef struct _VP3MidPidStruct VP3MidPidStruct;
#include "acdb_begin_pack.h"
struct _VP3MidPidStruct
{
    uint32_t    nModuleId;
    uint32_t    nParamId;
    uint32_t    nMaxParamLen;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbTableInfo AcdbTableInfo;
#include "acdb_begin_pack.h"
struct _AcdbTableInfo{
	AcdbDataInfo tblLutChnk;
	AcdbDataInfo tblCdftChnk;
	AcdbDataInfo tblCdotChnk;
	AcdbDataInfo dataPoolChnk;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbTableInfoCVD AcdbTableInfoCVD;
#include "acdb_begin_pack.h"
struct _AcdbTableInfoCVD{
	AcdbDataInfo tblLutChnk;
	AcdbDataInfo tblLutCVDChnk;
	AcdbDataInfo tblLutOffsetChnk;
	AcdbDataInfo tblCdftChnk;
	AcdbDataInfo tblCdotChnk;
	AcdbDataInfo dataPoolChnk;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbTableCmd AcdbTableCmd;
#include "acdb_begin_pack.h"
struct _AcdbTableCmd{
	uint32_t devId;
	uint32_t tblId;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbFileMgrGetFileDataReqType AcdbFileMgrGetFileDataReq;
#include "acdb_begin_pack.h"
struct _AcdbFileMgrGetFileDataReqType
{
	uint32_t nfile_offset;
	uint32_t nfile_data_len;
	uint32_t nfileNameLen;
	uint8_t  *pFileName;
}
#include "acdb_end_pack.h"
;
typedef struct _AcdbFileMgrRespType AcdbFileMgrResp;
#include "acdb_begin_pack.h"
struct _AcdbFileMgrRespType
{
	uint32_t nresp_buff_len;
	uint32_t nresp_buff_filled;
	uint8_t  *pRespBuff;
}
#include "acdb_end_pack.h"
;

typedef struct _WdspFrameworkIds WdspFrameworkIds;
#include "acdb_begin_pack.h"
struct _WdspFrameworkIds{
	uint32_t pId;
	AcdbDataInfo dataInfo;
}
#include "acdb_end_pack.h"
;

enum AcdbDataCmds {
    ACDBDATACMD_SET_ACDB_DATA= 0,
	ACDBDATACMD_RESET,
	ACDBDATACMD_GET_DEVICE_PROP,
	ACDBDATACMD_GET_GLOBAL_PROP,
	ACDBDATACMD_GET_DEVICE_LIST,
	ACDBDATACMD_GET_TABLE_INFO,
    ACDBDATACMD_GET_AVAILABLE_FILES_SLOT_COUNT,
	ACDBDATACMD_GET_LOADED_FILES_INFO,
	ACDBDATACMD_GET_FILE_DATA,
	ACDBDATACMD_GET_HEAP_INFO,
   ACDBDATACMD_GET_FILE_TYPE_INFO,
   ACDBDATACMD_GET_FILE_NAME,
   ACDBDATACMD_GET_WDSP_SUBGRAPHLIST,
   ACDBDATACMD_GET_WDSP_SUBGRAPH_DEFINITION,
   ACDBDATACMD_GET_WDSP_FRAMEWORKIDS,
};

enum AcdbFileType{
   ACDB_AV_TYPE,
   ACDB_CODEC_TYPE,
   ACDB_GLOBAL_TYPE,
   ACDB_AMDB_TYPE,
   ACDB_UNKNOWN_TYPE
};

int32_t acdbdata_ioctl (uint32_t nCommandId,
                        uint8_t *pInput,
                        uint32_t nInputSize,
                        uint8_t *pOutput,
                        uint32_t nOutputSize);

#endif /* __ACDB_FILE_MGR_H__ */
