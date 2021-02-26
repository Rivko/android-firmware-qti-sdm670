/** @file DALProps.c

  DAL property

  Copyright (c) 2010-2011,2015 Qualcomm Technologies Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 
 07/13/15   sg      Property type have MSB turned on for visibility attr
 01/13/15   aa      Property names have MSB turned on, GetNameOffsetOrIdVal api 
                    updated
 08/15/14   aa      Moving _DAL_StringDeviceObjectLookup method to Dxe and Loader
                    specific file.
 08/14/14   aa      Adding DALSYS_GetDAL_Prop api.
 08/13/14   aa      Adding DALProp API for querying properties for string based 
                    devices
 08/11/14   sho     creation

=============================================================================*/
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSys.h"
#include "DALPropDef.h"
#include "DALSysCmn.h"
#include "DALFramework.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

/*-------------------------------------------------------------------
 Variable Definitions 
 ------------------------------------------------------------------*/
// pointer to DAL property
// DALPROP_PropsInfo is binary data automatically generated from XML files
extern DALProps DALPROP_PropsInfo;
static DALProps *pDALProps = &DALPROP_PropsInfo;

DALProps *
DALSYS_GetDAL_Prop(void)
{
   return pDALProps;
}

void
DALSYS_GetPropDir(DALPropsDir ** pDALPropsDir)
{
   *pDALPropsDir = (DALPropsDir *)pDALProps->pDALPROP_PropBin;
}

const void **
DALSYS_GetPropStructPtrArray()
{
   return pDALProps->pDALPROP_StructPtrs;
}

DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
   //go through the DAL Property Directory and find out if we have this 
   //device information
   uint32 dwPropsIdx = 0;
   DALPropsDir *pDALPropsDir = NULL;

   //Get the Property Dir Info Ptr
   DALSYS_GetPropDir(&pDALPropsDir);

   if (pDALPropsDir)
   {
      for (dwPropsIdx=0;dwPropsIdx<pDALPropsDir->dwNumDevices;dwPropsIdx++)
      {
         if (DeviceId == pDALPropsDir->dwPropsDeviceOffset[dwPropsIdx][0])
         {
            //Device found, intialize offset
            //Initialize handle with BASE addr.
            hDALProps[0] = (UINTN) pDALPropsDir;
            hDALProps[1] = pDALPropsDir->dwPropsDeviceOffset[dwPropsIdx][1];
            return DAL_SUCCESS;
         }
      }
   }
   hDALProps[0] = 0;
   hDALProps[1] = 0;
   return DAL_ERROR;
}
   
DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
   const StringDevice *pStringDeviceObj = NULL;
   DALPropsDir *pDALPropsDir = NULL;
   DALResult ret;
   
   ret = _DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj,
         DALSYS_GetDAL_Prop());
   if (DAL_NO_ERROR_RETURNED(ret))
   {
      //Get the Property Dir Info Ptr
      DALSYS_GetPropDir(&pDALPropsDir);
      hDALProps[0] = (UINTN)pDALPropsDir;
      hDALProps[1] = (UINTN)pStringDeviceObj->dwOffset;
      return DAL_SUCCESS;
   }
   else
   {
      hDALProps[0] = (UINTN)NULL;
      hDALProps[1] = (UINTN)NULL;
      return ret;
   }
}

static 
uint32 GetNameOffsetOrIdVal(uint32 dwAttr, byte *pbCurPropsPos,uint32 *pdwId) 
{
   //read 4 bytes to retrieve name-offset(string id) or id(numeric id)
   DALSYS_memscpy(pdwId,sizeof(uint32),pbCurPropsPos,sizeof(uint32));
   if(*pdwId & 0x80000000 ) // 31st bit
   {
      // clear 31st bit as it used for identifying an id or name offset.
      // if its set its a offset
      *pdwId =  *pdwId & 0x7FFFFFFF ;
   }      
   return sizeof(uint32);
}

static 
void _PopulateStringProperty(DALSYSPropertyVar *pPropVar,
                             DALPropsDir *pPropsDir,
                             byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;
   DALSYS_memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pszVal = (char *)( pPropBase+pPropsDir->dwPropsStringSectionOffset
                                    +dwOffset) ;
   pPropVar->dwLen = AsciiStrLen(pPropVar->Val.pszVal);
   pPropVar->dwType = DALSYS_PROP_TYPE_STR_PTR;
}

static
void _PopulateUint32Property(DALSYSPropertyVar *pPropVar,
                             byte *pbPropsBuf)
{
   DALSYS_memscpy(&(pPropVar->Val.dwVal),sizeof(uint32),
         pbPropsBuf,sizeof(uint32));
   pPropVar->dwLen = sizeof(uint32);
   pPropVar->dwType = DALSYS_PROP_TYPE_UINT32;
}

static 
void _PopulateByteSeqProperty(DALSYSPropertyVar *pPropVar,
                              byte *pbPropsBuf)
{
   byte bLen = pbPropsBuf[0];
   pPropVar->Val.pbVal = pbPropsBuf+1;
   pPropVar->dwLen = bLen+1;
   pPropVar->dwType = DALSYS_PROP_TYPE_BYTE_PTR;
}

static 
void _PopulateByteSeqPtrProperty(DALSYSPropertyVar *pPropVar,
                                 DALPropsDir *pPropsDir,
                                 byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;
   //Get the byte offset
   DALSYS_memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pbVal = (pPropBase+pPropsDir->dwPropsByteSectionOffset
                          +dwOffset+1);
   pPropVar->dwLen = pPropVar->Val.pbVal[-1]+1;
   pPropVar->dwType = DALSYS_PROP_TYPE_BYTE_PTR;
}

static 
void _PopulateUint32SeqPtrProperty(DALSYSPropertyVar *pPropVar,
                                   DALPropsDir *pPropsDir,
                                   byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;

   //Get the byte offset
   DALSYS_memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pdwVal = (uint32 *) (pPropBase
                                      +pPropsDir->dwPropsUint32SectionOffset
                                      +dwOffset+sizeof(uint32));
   pPropVar->dwLen = pPropVar->Val.pdwVal[-1]+1;
   pPropVar->dwType = DALSYS_PROP_TYPE_UINT32_PTR;
}

static 
void _PopulateStructPtrProperty(DALSYSPropertyVar *pPropVar,
                                   DALPropsDir *pPropsDir,
                                   byte *pbPropsBuf)
{
   const void **pStruct = DALSYS_GetPropStructPtrArray();
   uint32 dwIdx = 0;

   if(pStruct)
   {
      DALSYS_memscpy(&dwIdx,sizeof(uint32),pbPropsBuf,sizeof(uint32));
      pPropVar->Val.pStruct = pStruct[dwIdx];
      pPropVar->dwLen = sizeof(void *);
      pPropVar->dwType = DALPROP_ATTR_TYPE_STRUCT_PTR;
   }
}

static 
DALResult _SkipProp(byte **pbPropsBuf, DALSYSPropertyVar *pPropVar)
{
   byte bLen = 0;
   switch (pPropVar->dwType&DALPROP_ATTR_TYPE_ID_ATTR_MASK)
   {
   case DALPROP_ATTR_TYPE_STRING_PTR:
   case DALPROP_ATTR_TYPE_UINT32:
   case DALPROP_ATTR_TYPE_BYTE_SEQ_PTR:
   case DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:
   case DALPROP_ATTR_TYPE_STRUCT_PTR:
      *pbPropsBuf += sizeof(uint32);      
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_BYTE_SEQ:
      bLen = *pbPropsBuf[0]+1+1;
      *pbPropsBuf += bLen;
      if (bLen%4)
         *pbPropsBuf+= (4-(bLen%4));
      return DAL_SUCCESS;

   default:
      return DAL_ERROR_INVALID_TYPE;
   }
}
static 
DALResult _PopulateProp(DALSYSPropertyVar *pPropVar, DALPropsDir *pPropsDir,
                        byte *pbPropsBuf)
{
   switch (pPropVar->dwType&DALPROP_ATTR_TYPE_ID_ATTR_MASK)
   {
   case DALPROP_ATTR_TYPE_STRING_PTR:
      _PopulateStringProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_UINT32:
      _PopulateUint32Property(pPropVar,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_BYTE_SEQ:
      _PopulateByteSeqProperty(pPropVar,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_BYTE_SEQ_PTR:
      _PopulateByteSeqPtrProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:
      _PopulateUint32SeqPtrProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_STRUCT_PTR:
      _PopulateStructPtrProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;
   
   default:
      return DAL_ERROR_INVALID_TYPE;
   }
}

static 
DALResult _MatchProp( DALPropsDir *pPropsDir, 
                   const char *pszName, uint32 dwId, uint32 dwName,
                   uint32 dwOffset)
{
   DALResult bPropFound = DAL_ERROR_PROP_NOT_FOUND;
   byte *pbPropsBaseAddr = (byte *)pPropsDir;
   
      //we have a Id name & Currently search for a id property 
      if ((NULL == pszName) && (dwName == dwId))
      {
         bPropFound = DAL_SUCCESS;
      }
   else
   {
      //we have a string name, NameId
      if ((NULL != pszName) && 
          (0 == AsciiStrCmp((const char *)pbPropsBaseAddr+pPropsDir->dwPropsNameSectionOffset+dwName, pszName)))
      {
         //this is the property of interest
         bPropFound = DAL_SUCCESS;
      }
   }
   return bPropFound;
}

DALResult
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                         uint32 dwId,
                         DALSYSPropertyVar *pDALPropVar)
{
   //go through all the DAL Properties for this device and determine if we
   //have such a property
   uint32 dwName = 0;
   uint32 bPropFound  = 0;
   uint32 bCheckedGlobal = 0;
   uint32 dwOffset = 0;
   DALSYS_PROPERTY_HANDLE_DECLARE(hDALGlobalProps);
   //Points to device props to begin with 
   DALPropsDir *pDALPropsDir = (DALPropsDir *)hDALProps[0];
   DALResult ret = DAL_ERROR ;
   byte  *pbDevicePropsPos   = ((byte *)hDALProps[0])+hDALProps[1];

   if((NULL == pDALPropsDir) || (0 == pbDevicePropsPos))
   {
      return DAL_ERROR_INVALID_POINTER;
   }

   DALSYS_memset(pDALPropVar, 0, sizeof(DALSYSPropertyVar));

   //Get the Global Props handle
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(0x00,hDALGlobalProps))
   {
      bCheckedGlobal = 1; //Flag it as though we checked global
   }

   for(;;)
   {
      //Get the Type
      DALSYS_memscpy(&pDALPropVar->dwType,sizeof(uint32),
            pbDevicePropsPos,sizeof(uint32));
      pbDevicePropsPos += sizeof(uint32);

      //check for end of props
      if (DALPROP_ATTR_PROP_END == pDALPropVar->dwType)
      {
         //Exhausted our search for local props, try global now (only once)
         if (0 == bCheckedGlobal)
         {
            bCheckedGlobal = 1;
            pbDevicePropsPos = ((byte *)hDALProps[0])
                               +pDALPropsDir->dwPropsDeviceOffset[0][1];
            continue;
         } else
         {
            //Property not found
            return DAL_ERROR_PROP_NOT_FOUND;
         }
      }

      dwOffset = GetNameOffsetOrIdVal((pDALPropVar->dwType & DALPROP_ATTR_TYPE_ID_ATTR_MASK),pbDevicePropsPos,&dwName);
      pbDevicePropsPos += dwOffset;
      bPropFound = _MatchProp(pDALPropsDir,pszName, dwId, dwName,dwOffset);
      if (DAL_SUCCESS != bPropFound)
      {
         ret = _SkipProp(&pbDevicePropsPos,pDALPropVar);
         if (DAL_SUCCESS != ret ) return ret;
         }
      else if (DAL_SUCCESS == bPropFound)
      {
		 if(pDALPropVar->dwType & DALPROP_ATTR_TYPE_ID_VISIBILITY_MASK)
			return DAL_ERROR_PROP_ACCESS_DENIED;
         return _PopulateProp(pDALPropVar, pDALPropsDir,pbDevicePropsPos);
      }
   }
}
