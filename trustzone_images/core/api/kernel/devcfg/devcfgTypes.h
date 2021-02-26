#ifndef DEVCFGTYPES_H
#define DEVCFGTYPES_H
/*==============================================================================
@file devcfgTypes.h


        Copyright (c) 2016 Qualcomm Technologies Inc.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

==============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/


#ifndef DEVCFG_STANDALONE
#include "DALStdDef.h"
#include "DALReg.h"
#else
#include "stubs.h"
#endif

/*------------------------------------------------------------------------------
* Type declaration
*-----------------------------------------------------------------------------*/
typedef unsigned long DevCfgMemAddr;
typedef DevCfgMemAddr *DALSYSPropertyHandle;

typedef struct  DALSYSPropertyVar DALSYSPropertyVar;
struct DALSYSPropertyVar
{
    uint32 dwType;
    uint32 dwLen;
    union
    {
        byte *pbVal;
        char *pszVal;
        uint32 dwVal;
        uint32 *pdwVal;
        const void *pStruct;
    }Val;
};

// Structure Table
typedef struct  DALSYSPropStructTblType DALSYSPropStructTblType ;
struct DALSYSPropStructTblType{
   uint32 dwSize;
   const void *pStruct;
};

// Struct to hold array of all Drivers which has String names
typedef struct  StringDevice StringDevice;
 struct StringDevice{
   const char *pszName; // Name of Device
   uint32 dwHash; // Hash Value
   uint32 dwOffset; // Offset
   DALREG_DriverInfo *pFunctionName; // Function Pointer
   uint32 dwNumCollision; // Number of Collisions
   const uint32 *pdwCollisions; // List of Collisions offset in the array
};

typedef struct DALProps DALProps;
struct DALProps
{
   const byte *pDALPROP_PropBin;
#ifdef DEVCFG__STRUCT_SIZE  
   const DALSYSPropStructTblType *pDALPROP_StructPtrs;
#else
   const void **pDALPROP_StructPtrs;
#endif 
   const uint32 dwDeviceSize; //Size of Devices array
   const StringDevice *pDevices; //String Device array 
};

// DALProp Type 
typedef enum 
{
   DEVCFG_PROP_INFO_SOC,
   DEVCFG_PROP_INFO_PLATFORM,
   DEVCFG_PROP_INFO_INTERNAL,
   DEVCFG_PROP_INFO_OEM
}DEVCFG_PROP_INFO_TYPE;

#define DALSYS_PROPERTY_HANDLE_DECLARE(varName) DevCfgMemAddr varName[2]

#endif /* DEVCFGTYPES_H */
