#include "DALSysInt.h"
#include "devcfgnew.h"
#include "DevCfg_ImgParams.h"
#ifndef DEVCFG_STANDALONE
    #include "stringl/stringl.h"
#endif

extern DALProps DAL_Mod_Info;
extern DEVCFG_TARGET_INFO devcfg_target_soc_info[];
#define SOC_PropInfo 0
#define PLATFORM_PropInfo 1
#define OEM_PropInfo 2
DEVCFG_PLATFORM_INFO gtcsrSOCHWVersion = {0, 0}; //family_number_device_number, major_ver_minor_ver

void DevCfg_log(const char* errMsg, ...){ return; }

void PropsSymLookUp(DEVCFG_TARGET_INFO * targetInfoTable, int propType)
{
   int dwPlatformCount = 0;
   DALProps *props;

   if(!targetInfoTable)
   {
     DevCfg_log("PropInfo lookup failed\n");
     return;
   }
   do
   {
      props = (DALProps *)targetInfoTable->plinker_soc_seg_sym;
      #ifndef DEVCFG_MULTICFG_ENABLED
      break;
      #endif
      if(targetInfoTable->tcsr_soc_info == gtcsrSOCHWVersion.soc_number)
      {
         props = (DALProps *)targetInfoTable->plinker_soc_seg_sym;
         #ifdef DEVCFG_MULTIPLATFORM_ENABLED
         if(targetInfoTable->num_platform_type > 0)
         {
            DalPlatformInfoPlatformType platform_id = DalPlatformInfo_Platform();
            for( dwPlatformCount = 0; dwPlatformCount < targetInfoTable->num_platform_type; dwPlatformCount++) 
            {
               if(targetInfoTable->platform_info->platform_id == platform_id)
               {
                  props =  (DALProps *)(targetInfoTable->platform_info->plinker_platform_seg_sym);
                  break;
               }
               targetInfoTable->platform_info++;
            }
         }
         #endif
      }
      targetInfoTable++;
   }
   while(targetInfoTable->plinker_soc_seg_sym); 
   devcfg_blob_append(props, propType);   
}

void DALSYS_SOC_PropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter)
{
   PropsSymLookUp(pTargetInfoTableIter, SOC_PropInfo);
}

void DALSYS_Platform_PropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter)
{
   #ifdef DEVCFG_MULTIPLATFORM_ENABLED
   PropsSymLookUp(pTargetInfoTableIter, PLATFORM_PropInfo);
   #endif
}           

void DALSYS_OEM_PropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter)
{
   #ifdef DEVCFG_OEM_ENABLED
   PropsSymLookUp(pTargetInfoTableIter,OEM_PropInfo);
   #endif
}
/*
  Called at DALSYS Init to detect chipset info by mapping the 
  TCSR_SOC_HW_VERSION register physical address. The register
  address is assumed to be fixed for a family and defined in ipcat
*/


#ifdef DEVCFG_MULTICFG_ENABLED

void DALSYS_ReadSocHwInfo(void)
{    
    DALResult eResult;

#ifdef TCSR_ADDR_PHS_SAME_AS_VIR
    memscpy(&gtcsrSOCHWVersion, sizeof(uint32), (uint32*)DEVCFG_TCSR_SOC_HW_ADDR, sizeof(uint32));
#else
    DALSYSMemHandle hMem;
    DALSYSMemInfo MemInfo;
    volatile uint32 val_tcsrSOCHWVersion;
    
    eResult = DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO, DALSYS_MEM_ADDR_NOT_SPECIFIED, DEVCFG_TCSR_SOC_HW_ADDR, 0x1000, &hMem, NULL);
    
    if (eResult != DAL_SUCCESS) 
    {
        DevCfg_log("Failed to map DEVCFG_TCSR_SOC_HW_ADDR");
        return; 
    }

    eResult = DALSYS_MemInfo(hMem, &MemInfo);
    if (eResult != DAL_SUCCESS) 
    {
        DevCfg_log("Failed to get memory info\n");
        DALSYS_DestroyObject(hMem);
        return; 
    }
    
    val_tcsrSOCHWVersion = *(uint32*)MemInfo.VirtualAddr;
    gtcsrSOCHWVersion.hw_version = (uint16)(val_tcsrSOCHWVersion & 0xFFFF);
    gtcsrSOCHWVersion.soc_number = (uint16)(val_tcsrSOCHWVersion>>16);

    DALSYS_DestroyObject(hMem);
   
#endif    //ifdef TCSR_ADDR_PHS_SAME_AS_VIR
}
#endif    //ifndef DEVCFG_MULTICFG_ENABLED

/*
  Called at Runtime by clients
*/
DALProps * DALSYS_GetPropsInfo(DEVCFG_PROP_INFO_TYPE dwInfoType)
{  
   return devcfgGetBlobByID((int)dwInfoType);
}
  
void DALSYS_GetPropDir(DALPropsDir ** pDALPropsDir, DEVCFG_PROP_INFO_TYPE dwInfoType)
{
   DALProps * PropsInfo = DALSYS_GetPropsInfo(dwInfoType);
   if (PropsInfo)
       *pDALPropsDir = (DALPropsDir *) PropsInfo->pDALPROP_PropBin;
}

const DALSYSPropStructTblType * DALSYS_GetPropStructPtrArray(DALProps *PropsInfo){ return PropsInfo->pDALPROP_StructPtrs; }

void DevCfg_Init(void)
{  
   #ifdef DEVCFG_MULTICFG_ENABLED
   //DALSYS_ReadSocHwInfo();
   #endif
   devcfg_init_new();
   //DALSYS_SOC_PropsSymLookUp(devcfg_target_soc_info);
   //DALSYS_Platform_PropsSymLookUp(devcfg_target_soc_info);
   #ifdef DEVCFG_OEM_ENABLED
   DALSYS_OEM_PropsSymLookUp((DEVCFG_TARGET_INFO *)DEVCFG_OEM_BASEADDR);
   #endif
}
