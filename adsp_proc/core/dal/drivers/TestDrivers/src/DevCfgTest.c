/*==============================================================================

FILE:      DevCfgTest.c

DESCRIPTION: This file implements a test framework to test Device Configuration.

PUBLIC CLASSES:  NA

INITIALIZATION AND SEQUENCING REQUIREMENTS:  NA

        Copyright © 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
            
$Header: //components/rel/core.qdsp6/2.1/dal/drivers/TestDrivers/src/DevCfgTest.c#1 $
==============================================================================*/

#ifdef DEVCFG_BOOT_TEST
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DalDevice.h"
#include "DALSysTypes.h"
#include "DALPropDef.h"
#include "DevCfgTest.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define MULTIASSERT(a,b,c,d) ASSERT(a); ASSERT(b); ASSERT(c); ASSERT(d)

// Property Types and Values 

// DALPROP_DATA_TYPE_STRING              0x00000001
// DALPROP_DATA_TYPE_BYTE_SEQ            0x00000002
// DALPROP_DATA_TYPE_UINT32_SEQ          0x00000003

// DALPROP_ATTR_TYPE_STRING_PTR          0x00000011
// DALPROP_ATTR_TYPE_UINT32              0x00000002
// DALPROP_ATTR_TYPE_BYTE_SEQ            0x00000008
// DALPROP_ATTR_TYPE_BYTE_SEQ_PTR        0x00000018
// DALPROP_ATTR_TYPE_UINT32_SEQ_PTR      0x00000014
// DALPROP_ATTR_TYPE_STRUCT_PTR          0x00000012
// DALPROP_ATTR_TYPE_BIT                 0x000F0000

// #define DALSYS_PROP_TYPE_STR_PTR           0x10
// #define DALSYS_PROP_TYPE_BYTE_PTR          0x11
// #define DALSYS_PROP_TYPE_UINT32            0x02
// #define DALSYS_PROP_TYPE_UINT32_PTR        0x12



  
void
DALSYS_PropertyTest(uint32 dwDevice)
{
   DALSYSPropertyVar propValue;
   DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);
   //Property values matched with Test driver XML files
   // The first element of an array, it's length 
   const char *pszString="TestDriverOne";
   const char *pszStringDevice[]= {"Device_1", "Device_2"};
   uint32 dwUint32 = 100;
   uint32 dwCounter = 0;
   uint32 pbByteSeq[] = {5,16,17,18,19,20};
   uint32 pbByteSeqPtr[] = {3,0x10,0x11,0x12};
   uint32 puint32Seq[] = {2,0,1};

   // dwdevice = 0 NULL Driver
   // dwdevice = 1 String Device
   // dwdevice = 0xDB2DB2DB Uint32 Device
   
   // // Check if it is a uint32 and call the right APIS
   if (0!= dwDevice && 1!=dwDevice)
   {
      ASSERT(DAL_SUCCESS == DALSYS_GetDALPropertyHandle(dwDevice, pHandle));
      dwDevice = 0;
   }else
   {
      ASSERT(DAL_SUCCESS == DALSYS_GetDALPropertyHandleStr(pszStringDevice[dwDevice], pHandle));
   }

   // Checking GetProperty return value, Property type, len, and value  for type DALSYS_PROP_TYPE_STR_PTR
   MULTIASSERT(DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"TestPropStringPtr", 0, &propValue),
               (DALSYS_PROP_TYPE_STR_PTR == propValue.dwType),
               (strlen(pszString)==propValue.dwLen),
               (0==strncmp(propValue.Val.pszVal, pszString,propValue.dwLen)));

   // Checking GetProperty return value, Property type, len, and value  for type DALSYS_PROP_TYPE_UINT32
   MULTIASSERT(DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"TestPropUint32", 0, &propValue),
              (DALSYS_PROP_TYPE_UINT32== propValue.dwType),
              (sizeof(uint32)== propValue.dwLen),
              (dwUint32== propValue.Val.dwVal));
   
   // Checking GetProperty return value, Property type, len, and value  for type DALSYS_PROP_TYPE_BYTE_PTR
   MULTIASSERT((DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"TestPropByteSeq", 0, &propValue)), 
              (DALSYS_PROP_TYPE_BYTE_PTR== propValue.dwType),
              (pbByteSeq[0]== propValue.dwLen),1);
   
   for (dwCounter=1;dwCounter<=propValue.dwLen;dwCounter++)
      ASSERT(pbByteSeq[dwCounter]== propValue.Val.pbVal[dwCounter-1]);
   
   // Checking GetProperty return value, Property type, len, and value  for type DALSYS_PROP_TYPE_UINT32_PTR
   MULTIASSERT((DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"TestPropUint32SeqPtr", 0, &propValue)),
               (DALSYS_PROP_TYPE_UINT32_PTR== propValue.dwType),
               (puint32Seq[0] == propValue.dwLen),1);
               
   for (dwCounter=1;dwCounter<= propValue.dwLen;dwCounter++)
      ASSERT(puint32Seq[dwCounter]== propValue.Val.pdwVal[dwCounter-1]);

   // Checking GetProperty return value, Property type, len, and value  for type  DALSYS_PROP_TYPE_BYTE_PTR 
   MULTIASSERT((DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"TestPropByteSeqPtr", 0, &propValue)),
               (DALSYS_PROP_TYPE_BYTE_PTR== propValue.dwType),
               (pbByteSeqPtr[0] == propValue.dwLen),1);
               
   for (dwCounter=1;dwCounter<= propValue.dwLen;dwCounter++)
      ASSERT(pbByteSeqPtr[dwCounter]== propValue.Val.pbVal[dwCounter-1]);

   // Checking GetProperty return value, Property type, len, and value  for type DALPROP_ATTR_TYPE_STRUCT_PTR
   MULTIASSERT((DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"TestPropStructPtr", 0, &propValue)),
               (DALPROP_ATTR_TYPE_STRUCT_PTR== propValue.dwType),
               (sizeof(uint32) == propValue.dwLen),
               (((StringDevice*)propValue.Val.pStruct)->dwHash == 123456u));
}

#endif //DEVCFG_BOOT_TEST
