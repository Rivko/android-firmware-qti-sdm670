#ifndef DALSTDDEF_H
#define DALSTDDEF_H
/*=======================================================================

FILE:         DALStdDef.h

DESCRIPTION:  definition of basic types, constants,
                 preprocessor macros


  Copyright (c) 2008, 2010, 2013-2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  All Rights Reserved.
  Qualcomm Confidential and Proprietary


=========================================================================*/

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

/* The following definitions are the same accross platforms.  This first
** group are the sanctioned types.
*/

#include "com_dtypes.h"

#if defined(__GNUC__)
#define __int64 long long
#endif

#ifndef _UINT64_DEFINED
typedef  unsigned __int64   uint64;      /* Unsigned 64 bit value */
#define _UINT64_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef  __int64            int64;       /* Signed 64 bit value */
#define _INT64_DEFINED
#endif

typedef uint32  DALBOOL;
typedef uint32  DALDEVICEID;
typedef uint32  DalPowerCmd;
typedef uint32  DalPowerDomain;
typedef uint32  DalSysReq;
typedef UINTN   DALHandle;
typedef int     DALResult;
typedef void  * DALEnvHandle;
typedef void  * DALSYSEventHandle;
typedef UINTN  DALSYSMemAddr;
typedef UINTN  DALMemAddr;

typedef uint32  DALInterfaceVersion;

// common macro for dalsys objects
#define INHERIT_DALSYS_BASE_OBJ()\
   uint32 dwObjInfo;\
   uint32 hOwnerProc;\
   DALSYSMemAddr thisVirtualAddr

// DAL System Objects which may be communicated as DDI params
typedef unsigned char * DALDDIParamPtr;

typedef struct DALEventObject DALEventObject;
struct DALEventObject
{
    uint32 obj[8];
};
typedef DALEventObject * DALEventHandle;

typedef struct _DALMemObject
{
   uint32        memAttributes;
   uint32        sysObjInfo[2];
   uint32        dwLen;
   uint32        ownerVirtAddr;
   uint32        virtAddr;
   uint32        physAddr;
}
DALMemObject;

typedef struct _DALDDIMemBufDesc
{
   uint32       dwOffset;
   uint32       dwLen;
   uint32       dwUser;
}
DALDDIMemBufDesc;

// lower 8-bits of dwFlags reserved
typedef struct _DALDDIMemDescList
{
   uint32          dwFlags;
   uint32          dwNumBufs;
   DALDDIMemBufDesc   bufList[1];  // variable length
}
DALDDIMemDescList;


// The following is memory descriptor syntax which can be used
// within an environment. (i.e. shared dalsys).

typedef struct DALSysMemDescBuf DALSysMemDescBuf;
struct DALSysMemDescBuf
{
   DALSYSMemAddr VirtualAddr;
   DALSYSMemAddr PhysicalAddr;
   uint32 size;
   uint32 user;
};

typedef struct DALSysMemDescList DALSysMemDescList;
struct DALSysMemDescList
{
   INHERIT_DALSYS_BASE_OBJ();
   DALSYSMemAddr PhysicalAddr;
   DALSYSMemAddr VirtualAddr;
   uint32        dwCurBufIdx;
   uint32        dwNumDescBufs;
   DALSysMemDescBuf BufInfo[1];
};

/* -----------------------------------------------------------------------
** Some Constants
** ----------------------------------------------------------------------- */
#ifndef TRUE
#define TRUE   1   /* Boolean true value. */
#endif

#ifndef FALSE
#define FALSE  0   /* Boolean false value. */
#endif

#ifndef NULL
#define NULL  0
#endif

#define DALSYS_MEM_INVALID_ADDR  (DALSYSMemAddr)(~0)

#endif /* #ifndef DALSTDDEF_H */
