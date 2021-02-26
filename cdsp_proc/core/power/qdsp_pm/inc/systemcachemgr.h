/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/
#ifndef _SYS_CACHE
#define _SYS_CACHE

#include "adsppm.h"
#include "adsppm_utils.h"
#include "systemcache/systemcache.h"

#define MAX_STREAM_ID  (1)
#define MAX_SCID_COUNT (32)

typedef enum
{
   BUSMgr_Scid_Internal      = 0,
   BUSMgr_Scid_External,
   BUSMgr_Scid_Max,
}BUSMgrScidType;


typedef struct
{
   uint32       scid;
   uint32       count;
}SysCacheMgrScidRefCount;





Adsppm_Status scpmInit(void);

Adsppm_Status syscacheMgrIncrementScidCount (uint32 scid);

Adsppm_Status syscacheMgrDecrementScidCount (uint32 scid);

Adsppm_Status syscacheMgrGetScidCount (uint32 scid, uint32 *count);

void syscacheMgrProgramInternalScid (uint32 scid, uint32 index );

Adsppm_Status sysCacheMgrResetScid (uint32 index, BUSMgrScidType scidType);

Adsppm_Status sysCacheMgrProgramScid (uint32 usecaseId,uint32 index, BUSMgrScidType scidType );

#endif
