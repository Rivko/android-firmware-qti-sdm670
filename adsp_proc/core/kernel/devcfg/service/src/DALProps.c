#ifndef DEVCFG_STANDALONE
    #include "DALSys.h"
    #include "DALPropDef.h"
    #include "DALSysInt.h"
    #include "stringl/stringl.h"
#else
    #include "devcfg.h"
    #include "stubs.h"
#endif
#include "DALSysInt.h"


/* Defined in DalSysInt.c, contain info about SOC & HW Version */
extern DEVCFG_PLATFORM_INFO gtcsrSOCHWVersion;

DALResult
_DALSYS_GetDALPropertyInfo(DALDEVICEID DeviceId, DALProps * DALPROP_PropsInfo, 
                           DALSYSPropertyHandle hDALProps)
{
   uint32 dwPropsIdx = 0;
	DALPropsDir *pDALPropsDir  = (DALPropsDir *) DALPROP_PropsInfo->pDALPROP_PropBin;
   if (pDALPropsDir)
   {
      for (dwPropsIdx=0;dwPropsIdx<pDALPropsDir->dwNumDevices;dwPropsIdx++)
      {
         if (DeviceId == pDALPropsDir->dwPropsDeviceOffset[dwPropsIdx][0])
         {
            //Device found, intialize offset
            //Initialize handle with BASE addr.
            hDALProps[0] = (uint32) DALPROP_PropsInfo;
            hDALProps[1] = pDALPropsDir->dwPropsDeviceOffset[dwPropsIdx][1];
            return DAL_SUCCESS;
         }
      }
   }
   hDALProps[0] = NULL;
   hDALProps[1] = NULL;
   DevCfg_log("Failed- DeviceId:0x%x", DeviceId);
   return DAL_ERROR;
}


DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
   //go through the DAL Property Directory and find out if we have this 
   //device information

   //DALPropsDir *pDALPropsDir = NULL;
    DALProps * DALPROP_PropsInfo = NULL;
   DALResult ret = DAL_ERROR;
   
   //Get the Property Info Ptr for SOC for default lookup
   DALPROP_PropsInfo = DALSYS_GetPropsInfo(DEVCFG_PROP_INFO_SOC);

   if (DALPROP_PropsInfo)
   {
      ret =  _DALSYS_GetDALPropertyInfo(DeviceId, DALPROP_PropsInfo, hDALProps);
      if(DAL_SUCCESS != ret)
		{
			//look in Platform
			DALPROP_PropsInfo = NULL;
			DALPROP_PropsInfo = DALSYS_GetPropsInfo(DEVCFG_PROP_INFO_PLATFORM);
			if (DALPROP_PropsInfo)
			{
            ret = _DALSYS_GetDALPropertyInfo(DeviceId, DALPROP_PropsInfo, hDALProps);
			}
		}

	}
   return ret;
}

DALResult
DALSYS_GetDALPropertyHandleEx(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps, 
                              DEVCFG_PROP_INFO_TYPE target_info_type)
{
   DALProps * DALPROP_PropsInfo = NULL;
   DALResult ret = DAL_ERROR;

  //Look in section directly as specified by user
  DALPROP_PropsInfo = DALSYS_GetPropsInfo(target_info_type);
  if (DALPROP_PropsInfo)
  {
      ret = _DALSYS_GetDALPropertyInfo(DeviceId, DALPROP_PropsInfo, hDALProps);
  }

   return ret;
}

static uint32
DJB2_hash(const char *pszDevName)
{
   uint32 dwHash = 5381;
   while (*pszDevName)
      dwHash = ((dwHash << 5) + dwHash) + (int)*pszDevName++; /* dwHash * 33 + c */
   return dwHash;
}
    
DALResult
DAL_StringDeviceObjectLookup(const char *pszDevName, 
      const StringDevice **ppStringDeviceObj, DALProps *pDALPtr)
{
   uint32 dwDriverHash;
   int dwIndex = 0;
   int dwSize;
   const uint32 *pCollisionsIndexes = NULL;

   if(pDALPtr && pDALPtr->pDevices && pszDevName 
     && dwIndex< pDALPtr->dwDeviceSize && ppStringDeviceObj)
   {
      dwDriverHash = DJB2_hash(pszDevName); 
      for(dwIndex=0;dwIndex< pDALPtr->dwDeviceSize ; dwIndex++)
      {
         if(dwDriverHash ==  pDALPtr->pDevices[dwIndex].dwHash)
         {
            break;
         } 
      }
      // Finish the search with right device index. 
      if(dwIndex != pDALPtr->dwDeviceSize)
      {
         // if there is no Collisions.
         if(pDALPtr->pDevices[dwIndex].dwNumCollision == 0)
         {
            *ppStringDeviceObj = &(pDALPtr->pDevices[dwIndex]);
            return DAL_SUCCESS;
         }
         // If there is a Collision, scan the list for right device object.
         pCollisionsIndexes = pDALPtr->pDevices[dwIndex].pdwCollisions;
         dwSize = pDALPtr->pDevices[dwIndex].dwNumCollision;
         for(dwIndex=0;dwIndex<dwSize;dwIndex++)
         {
            // Get the dwIndex, where the str might be
            if(strlen(pszDevName) == strlen(pDALPtr->pDevices[pCollisionsIndexes[dwIndex]].pszName))
            {
               if(strncmp(pszDevName, pDALPtr->pDevices[pCollisionsIndexes[dwIndex]].pszName, strlen(pszDevName))==0)
               {
                  *ppStringDeviceObj = &(pDALPtr->pDevices[pCollisionsIndexes[dwIndex]]);
                  return DAL_SUCCESS;
               }
            }
         }
      }   
   }

  DevCfg_log("Failed- pszDevName:%s pDALPtr:0x%x",
      pszDevName, pDALPtr);
   return DAL_ERROR;
}
 
DALResult
_DALSYS_GetDALPropertyStrInfo(const char *pszDevName, DALSYSPropertyHandle hDALProps,
							  DALProps * DALPROP_PropsInfo)
{
   const StringDevice *pStringDeviceObj = NULL;
   
   if(DAL_SUCCESS == DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj, DALPROP_PropsInfo)){
      //Get the Property Dir Info Ptr
      hDALProps[0] = (uint32) DALPROP_PropsInfo;
      hDALProps[1] = pStringDeviceObj->dwOffset;
      return DAL_SUCCESS;
   }
   
   hDALProps[0] = NULL;
   hDALProps[1] = NULL;

   DevCfg_log("Failed- pszDevName:%s DALPROP_PropsInfo:0x%x",
      pszDevName, DALPROP_PropsInfo);
   return DAL_ERROR;
}

DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
   
   DALProps * DALPROP_PropsInfo = NULL;
   DALResult ret = DAL_ERROR;
   
   //default look first in SOC
   DALPROP_PropsInfo = DALSYS_GetPropsInfo(DEVCFG_PROP_INFO_SOC);
   if(DALPROP_PropsInfo)
   {
      ret = _DALSYS_GetDALPropertyStrInfo(pszDevName, hDALProps, DALPROP_PropsInfo);
      if(DAL_SUCCESS != ret)
		{
			//Get the Property Dir Info Ptr
			DALPROP_PropsInfo = NULL;
			DALPROP_PropsInfo = DALSYS_GetPropsInfo(DEVCFG_PROP_INFO_PLATFORM);
			if(DALPROP_PropsInfo)
			{
            ret = _DALSYS_GetDALPropertyStrInfo(pszDevName, hDALProps, DALPROP_PropsInfo);
			}
		}
	}
   return ret;
}

DALResult
DALSYS_GetDALPropertyHandleStrEx(const char *pszDevName, DALSYSPropertyHandle hDALProps, 
                                 DEVCFG_PROP_INFO_TYPE target_info_type)
{  
   DALProps * DALPROP_PropsInfo = NULL;
   DALResult ret = DAL_ERROR;

   //Look in platform section directly if target info type passed by user is platform
   DALPROP_PropsInfo = DALSYS_GetPropsInfo(target_info_type);
   if (DALPROP_PropsInfo)
   {
      ret = _DALSYS_GetDALPropertyStrInfo(pszDevName, hDALProps, DALPROP_PropsInfo);

   }
 
   return ret;  
}

static 
uint32 GetNameOffsetOrIdVal(uint32 dwTypeNameOffset, byte *pbCurPropsPos,uint32 *pdwId) 
{   
    #ifdef DEVCFG__PROPS_MEMORY_OPTIMISE
        //Get the name offset or id : extract the least significant 24 bits
        *pdwId = (dwTypeNameOffset & 0x00FFFFFF);
        if(*pdwId & 0x00800000 ) // 23rd bit
        {
            // clear 23rd bit as it used for identifying an id or name offset.
            // if its set its a offset
            *pdwId =  *pdwId & 0x007FFFFF ;
        }
    #else
        //read 4 bytes to retrieve nameoffset(string id) or id(numeric id)
        memscpy(pdwId,sizeof(uint32),pbCurPropsPos,sizeof(uint32));
    #endif
    return sizeof(uint32);
}

static 
void _PopulateStringProperty(DALSYSPropertyVar *pPropVar,
                             DALPropsDir *pPropsDir,
                             byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;
   memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pszVal = (char *)( pPropBase+pPropsDir->dwPropsStringSectionOffset
                                    +dwOffset) ;
   pPropVar->dwLen = strlen(pPropVar->Val.pszVal);
   pPropVar->dwType = DALSYS_PROP_TYPE_STR_PTR;
}

static
void _PopulateUint32Property(DALSYSPropertyVar *pPropVar,
                             byte *pbPropsBuf)
{
   memscpy(&(pPropVar->Val.dwVal),sizeof(uint32),pbPropsBuf,sizeof(uint32));
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
   memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
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
   memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pdwVal = (uint32 *) (pPropBase
                                      +pPropsDir->dwPropsUint32SectionOffset
                                      +dwOffset+sizeof(uint32));
   pPropVar->dwLen = pPropVar->Val.pdwVal[-1]+1;
   pPropVar->dwType = DALSYS_PROP_TYPE_UINT32_PTR;
}

static 
void _PopulateStructPtrProperty(DALSYSPropertyVar *pPropVar,
                                   DALProps *DALPROP_PropsInfo,
                                   byte *pbPropsBuf)
{
#ifdef DEVCFG__STRUCT_SIZE 
   const DALSYSPropStructTblType *pStruct = DALSYS_GetPropStructPtrArray(DALPROP_PropsInfo);
#else
   const void **pStruct = DALSYS_GetPropStructPtrArray(DALPROP_PropsInfo);
#endif
   uint32 dwIdx = 0;

   if(pStruct)
   {
      memscpy(&dwIdx,sizeof(uint32),pbPropsBuf,sizeof(uint32));
#ifdef DEVCFG__STRUCT_SIZE 
      pPropVar->Val.pStruct = pStruct[dwIdx].pStruct;
      pPropVar->dwLen = pStruct[dwIdx].dwSize;
#else
      pPropVar->Val.pStruct = pStruct[dwIdx];
      pPropVar->dwLen = sizeof(void *);
#endif
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
   
      bLen = *pbPropsBuf[0]+1+1; // The first one is refering to the len and the secound one is zero based index
      *pbPropsBuf += bLen;
      if (bLen%4)
         *pbPropsBuf+= (4-(bLen%4));
      return DAL_SUCCESS;

   default:
      DevCfg_log("Unknown prop value- pPropVar:0x%x", pPropVar);
      return DAL_ERROR;
   }
}
static 
DALResult _PopulateProp(DALSYSPropertyVar *pPropVar, DALProps *DALPROP_PropsInfo,
                        byte *pbPropsBuf)
{
   DALPropsDir *pPropsDir = (DALPropsDir *) DALPROP_PropsInfo->pDALPROP_PropBin;
   
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
      _PopulateStructPtrProperty(pPropVar,DALPROP_PropsInfo,pbPropsBuf);
      return DAL_SUCCESS;
   
   default:
      DevCfg_log("Unknown type- pPropVar:0x%x", pPropVar);
      return DAL_ERROR;
   }
}

static 
uint32 _MatchProp( DALPropsDir *pPropsDir, 
                   const char *pszName, uint32 dwId, uint32 dwName,
                   uint32 dwOffset)
{
   uint32 bPropFound = 0;
   byte *pbPropsBaseAddr = (byte *)pPropsDir;
   
   //we have a Id name & Currently search for a id property 
   if ((NULL == pszName) && (dwName == dwId))
   {
      bPropFound = 1;
   }
   else
   {
      //we have a string name, NameId
      if ((NULL != pszName) && 
          (0 == strcmp((const char *)pbPropsBaseAddr+pPropsDir->dwPropsNameSectionOffset+dwName,
                       pszName)))
      {
         //this is the property of interest
         bPropFound = 1;
      }
   }
   return bPropFound;
}

/* Read "DevCfgVersionInfo" property injected for a device if present to find a 
   right offset for propery search in a device.
*/
DALResult
DALSYS_GetHWVersionInfo( byte  *pbDevicePropsPos, DALPropsDir *pDALPropsDir, 
                         byte  **pbDeviceVerPropsPos )
{
   uint32 dwType = 0, dwName = 0, dwOffset = 0, bPropFound = 0, dwVersionId = 0;
   uint32 dwVersionOffset = 0, dwVerCount = 0;
   uint32 dwTypeNameOffset = 0;
   DALSYSPropertyVar propValue;
   byte *pbDeviceBaseAddr = pbDevicePropsPos;
   
   //Get the Type, must be DALPROP_ATTR_TYPE_UINT32_SEQ_PTR
   memscpy(&dwTypeNameOffset,sizeof(uint32),pbDevicePropsPos,sizeof(uint32));
   
   #ifdef DEVCFG__PROPS_MEMORY_OPTIMISE
        // Get the type : 8 most significant bits
        dwType = (dwTypeNameOffset & 0xFF000000) >> 24;
   #else
        dwType = dwTypeNameOffset;
        pbDevicePropsPos += sizeof(uint32);
   #endif
      
   if(DALPROP_ATTR_TYPE_UINT32_SEQ_PTR != dwType)
      return DAL_ERROR;
   
   dwOffset = GetNameOffsetOrIdVal(dwTypeNameOffset,pbDevicePropsPos,&dwName);
   pbDevicePropsPos += dwOffset;
   
   bPropFound = _MatchProp(pDALPropsDir,"DevCfgVersionInfo", 0, dwName,dwOffset);
   if (bPropFound)
       _PopulateUint32SeqPtrProperty(&propValue,pDALPropsDir,pbDevicePropsPos);
   else 
      return DAL_ERROR;
               
   for (dwVerCount=0;dwVerCount< propValue.dwLen;dwVerCount++)
   {
        // higher 16 bit for version, lower 16 bit for offset
        dwVersionId = ((propValue.Val.pdwVal[dwVerCount] & 0xFFFF0000) >> 16);
        if(dwVersionId == gtcsrSOCHWVersion.hw_version)
        {
            dwVersionOffset = (propValue.Val.pdwVal[dwVerCount] & 0x0000FFFF);
            *pbDeviceVerPropsPos   = pbDeviceBaseAddr + dwVersionOffset ;
            return DAL_SUCCESS;
        }
   }
   
   return DAL_ERROR;
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
   uint32 dwTypeNameOffset = 0;
   uint32 bCheckedCommon = 0;
   byte  *pbDeviceVerPropsPos = NULL;
   DALSYS_PROPERTY_HANDLE_DECLARE(hDALGlobalProps);
   
   //Points to prop info to begin with 
   DALProps * DALPROP_PropsInfo = (DALProps *)hDALProps[0];
   if(NULL == DALPROP_PropsInfo)
   {
      DevCfg_log("DALPROP_PropsInfo is NULL- pszName:%s",
         pszName);
      return DAL_ERROR;
   }
   
   //Points to device props to begin with 
   DALPropsDir *pDALPropsDir = (DALPropsDir *) DALPROP_PropsInfo->pDALPROP_PropBin;
   byte  *pbDevicePropsPos   = ((byte *)pDALPropsDir)+hDALProps[1];
   if((NULL == pDALPropsDir) || (0 == pbDevicePropsPos))
   {
      DevCfg_log(
         "pDALPropsDir or pbDevicePropsPos is NULL- pszName:%s pDALPropsDir:0x%x pbDevicePropsPos:0x%x",
         pszName, pDALPropsDir, pbDevicePropsPos);
      return DAL_ERROR;
   }

   //Get the Global Props handle
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(0x00,hDALGlobalProps))
   {
      bCheckedGlobal = 1; //Flag it as though we checked global
   }
   
   if (DAL_ERROR != DALSYS_GetHWVersionInfo(pbDevicePropsPos, pDALPropsDir, 
                                            &pbDeviceVerPropsPos ))
   {
      pbDevicePropsPos = pbDeviceVerPropsPos;
   }
   else
   {
      bCheckedCommon = 1;
   }
   
   for(;;)
   {
      //Get the Type
      memscpy(&dwTypeNameOffset,sizeof(uint32),pbDevicePropsPos,sizeof(uint32));
      
      #ifdef DEVCFG__PROPS_MEMORY_OPTIMISE
          // Get the type : 8 most significant bits
          pDALPropVar->dwType = (dwTypeNameOffset & 0xFF000000) >> 24;
      #else
          pDALPropVar->dwType = dwTypeNameOffset;
          pbDevicePropsPos += sizeof(uint32);
      #endif
      
      //check for end of props
      if (DALPROP_ATTR_PROP_END == dwTypeNameOffset)
      {
         //Exhausted our search for version specfic props, try common now
         if (!bCheckedCommon)
         {
            bCheckedCommon = 1;
            pbDevicePropsPos = ((byte *)pDALPropsDir)+hDALProps[1];
            continue;
         }
         
         //Exhausted our search for local props, try global now (only once)
         if (0 == bCheckedGlobal)
         {
            bCheckedGlobal = 1;
            pbDevicePropsPos = ((byte *)pDALPropsDir)
                               +pDALPropsDir->dwPropsDeviceOffset[0][1];
            continue;
         } else
         {
            //Property not found
            DevCfg_log(
               "Property not found- pszName:%s pDALPropsDir:0x%x pbDevicePropsPos:0x%x",
               pszName, pDALPropsDir, pbDevicePropsPos);
           return DAL_ERROR;
         }
      }

      dwOffset = GetNameOffsetOrIdVal(dwTypeNameOffset,pbDevicePropsPos,&dwName);
      pbDevicePropsPos += dwOffset;
      bPropFound = _MatchProp(pDALPropsDir,pszName, dwId, dwName,dwOffset);
      if (0 == bPropFound)
      {
         if (DAL_ERROR == _SkipProp(&pbDevicePropsPos,pDALPropVar))
         {
             DevCfg_log(
               "SkipProp error- pszName:%s pDALPropsDir:0x%x pbDevicePropsPos:0x%x",
               pszName, pDALPropsDir, pbDevicePropsPos);
           return DAL_ERROR;
         }
      } else if (1 == bPropFound)
      {
         return _PopulateProp(pDALPropVar, DALPROP_PropsInfo,pbDevicePropsPos);
      }
   }
}
