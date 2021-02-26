#ifndef DALSTDDEF_H
#define DALSTDDEF_H
/*=======================================================================

FILE:         DALStdDef.h

DESCRIPTION:  definition of basic types, constants,
                 preprocessor macros


  Copyright (c) 2008, 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/api/dal/DALStdDef.h#1 $

=========================================================================*/

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

/* The following definitions are the same accross platforms.  This first
** group are the sanctioned types.
*/

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  signed char        int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

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

#ifndef _BYTE_DEFINED
typedef  unsigned char      byte;        /* byte type */
#define  _BYTE_DEFINED
#endif

typedef uint32  DALBOOL;
typedef uint32  DALDEVICEID;
typedef uint32  DalPowerCmd;
typedef uint32  DalPowerDomain;
typedef uint32  DalSysReq;
typedef uint32  DALHandle;
typedef int     DALResult;
typedef void  * DALEnvHandle;
typedef void  * DALSYSEventHandle;
typedef uint32  DALMemAddr;
typedef uint32  DALSYSMemAddr;
typedef uint64  DALSYSPhyAddr;
typedef uint32  DALInterfaceVersion;

// common macro for dalsys objects
#define INHERIT_DALSYS_BASE_OBJ()\
   uint32 dwObjInfo;\
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

#define DALSYS_MEM_DESC_LIST_OBJECT(varName,numBufs) \
struct {\
    INHERIT_DALSYS_BASE_OBJ();\
    DALSYSMemAddr PhysicalAddr;\
    DALSYSMemAddr VirtualAddr;\
    uint32        hOwnerProc; \
    uint32        dwCurBufIdx;\
    uint32        dwNumDescBufs;\
    DALSysMemDescBuf BufInfo[numBufs];\
} varName

// below is the DALSysMemDescList definition
typedef DALSYS_MEM_DESC_LIST_OBJECT(DALSysMemDescList, 1);

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

#endif /* #ifndef DALSTDDEF_H */

