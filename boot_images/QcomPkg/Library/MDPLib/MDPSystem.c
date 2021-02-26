/*=============================================================================
 
  File: MDPSystem.c
 
  Source file for MDP functions
  
 
  Copyright (c) 2011-2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#include <Uefi.h>
#include <libfdt.h>
#include <Library/UefiLib.h>
#include <Library/ArmLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/RamPartitionTableLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/QcomBaseLib.h>
#include "MMUSIDhwio_4xx.h"
#include "MDPSystem.h"
#include "HALhwio.h"
#include "MDPLib_i.h"
#include "MDPSystem.h"
#include "MDPPlatformLib.h"
#include "DisplayUtils.h"

/*=========================================================================
     Default Defines
==========================================================================*/
//Maximum debug output string length
#define DISP_MAX_DEBUG_MESSAGE_LENGTH   100

//Time out value for polling resource status - as recommended by hardware team
#define MMU_SID_WAIT_TIMEOUT_US         100

//Modify field value using mask
#define SET_FIELD_VALUE(_curr_,_mask_,_val_) (((_curr_) & (uint32)(~(_mask_))) | \
                                              ((uint32)((_val_) & (_mask_))))

//Device tree related constants
#define DTB_PAD_SIZE                    1024                              // Device tree buffer padding size
#define SPLASH_MEM_DT_NODE_NAME         "/reserved-memory/splash_region"  // Name of the splash buffer device tree node
#define SPLASH_MEM_DT_REG               "reg"                             // Splash Node's sub node name
#define SPLASH_MEM_PROP_SIZE            (4 * sizeof(UINT32))              // Size of splash node in bytes

// MMU related
#define SMR_VALID                       0x80000000
#define TOTAL_SMR                       160
#define EMPTY_CB                        0
#define TOTAL_CB                        64
#define CBAR_POR_NOAC                   0x20000

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* HW Register field information from HWIO defs
*/
typedef struct {
  uint32 uReg;                    // Register Address
  uint32 uMask;                   // Field mask, 0 to write to entire register
  uint32 uValue;                  // Field value. shifted to its bit-offset
} HWIOFieldType;

/* HWIO resource field information for control & status
*
*  NOTE: All values should be pre-shifted to appropriate field offset.
*/
typedef struct {
  uint32 uCmdReg;                 // Register address for command field
  uint32 uCmdMask;                // field mask for command
  uint32 uSetValue;               // command value for enabling resource
  uint32 uUnsetValue;             // command value for disabling resource
  uint32 uStatusReg;              // Register address for status field
  uint32 uStatusMask;             // field mask for status
  uint32 uSetStatus;              // status value to check for enable
  uint32 uUnsetStatus;            // status value to check for disable
} HWIOResourceFieldType;

/*=========================================================================
     Local Static Variables
==========================================================================*/

/* MMU control data, terminated by all zeros
*/
HWIOFieldType gMMU_regs[] = {
  { HWIO_APPS_SMMU_SMR0_ADDR,       HWIO_APPS_SMMU_SMR0_RMSK,        0x80080880 }, // Add stream id 0 to stream mapping table search 
  { HWIO_APPS_SMMU_S2CR0_ADDR,      HWIO_APPS_SMMU_S2CR0_RMSK,       0x0        }, // Use all attributes as presented with transaction
  { HWIO_APPS_SMMU_SMR1_ADDR,       HWIO_APPS_SMMU_SMR1_RMSK,        0x80080C80 }, 
  { HWIO_APPS_SMMU_S2CR1_ADDR,      HWIO_APPS_SMMU_S2CR1_RMSK,       0x0        },
  { HWIO_APPS_SMMU_CBAR0_ADDR,      HWIO_APPS_SMMU_CBAR0_RMSK,       0x10003    }, // Virtual Machine Identifier to be associated with the translation context bank
  { HWIO_APPS_SMMU_CBA2R0_ADDR,     HWIO_APPS_SMMU_CBA2R0_VA64_BMSK, 0x1        }, // enable access to addresses > 4GB
  { HWIO_APPS_SMMU_CB0_SCTLR_ADDR,  HWIO_APPS_SMMU_CB0_SCTLR_M_BMSK, 0x0        }  // Value 0 indicates that MMU for this context bank is disabled. ie. bypass mode.
};

/*=========================================================================
     Local Static Functions
==========================================================================*/
/*
Reserved memory allocation
*/
static MDP_Status MDPAllocateReservedMemory(EFIChipInfoFamilyType eChipId, uint64 *pReservedMemoryAddress, uint64 *pReservedMemorySize);

/* Locks used to prevent re-entrance */
static MDP_LockInfo sLockInfo;


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: PollRegisterField()
**
** DESCRIPTION:
**   Poll the register field for a specific value or time out
**
** ---------------------------------------------------------------------- */
static bool32 PollRegisterField(uint32 uReg, uint32 uMask, uint32 uExpected, uint32 uTimeoutUs)
{
  bool32 bCond      = FALSE;
  int32  iTimeout   = uTimeoutUs;

  while (0 <= iTimeout)
  {
    uint32 uValue = in_dword_masked(uReg, uMask);
    
    //Check field value
    if (uValue == uExpected )
    {
      bCond = TRUE;
      break;
    }
    
    //resource not ready yet, wait & retry if not timed out
    iTimeout--;
  }

  return bCond;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: WriteRegisterField()
**
** DESCRIPTION:
**   Helper to write to a field in a HW register
**
** ---------------------------------------------------------------------- */
static void WriteField(uint32 uReg, uint32 uMask, uint32 uValue)
{
  //if mask is non-zero then write to field, if not, then the entire register
  if (0 < uMask)
  {
    //Read
    uint32 uData = in_dword(uReg);

    //Modify
    uValue = SET_FIELD_VALUE(uData,uMask,uValue);
  }
  
  //write
  out_dword(uReg, uValue);
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: EnableResources()
**
** DESCRIPTION:
**   Enable/Disable a list of clock/power resources
**
** ---------------------------------------------------------------------- */
static bool32 EnableResources(bool32                       bEnable,
                              const HWIOResourceFieldType *pResources,
                              uint32                       uCount,
                              uint32                       uTimeoutUs)
{
  bool32 bRet   = TRUE;
  int32  i      = 0;

  for (i = 0;i < uCount; i++)
  {
    uint32 uValue = 0;
    uint32 uCheck = 0;
    
    if (bEnable)
    {
      uValue = pResources[i].uSetValue;
      uCheck = pResources[i].uSetStatus;
    }
    else
    {
      uValue = pResources[i].uUnsetValue;
      uCheck = pResources[i].uUnsetStatus;
    }
    
    //set resource state
    WriteField(pResources[i].uCmdReg, pResources[i].uCmdMask, uValue);

    //poll the status bit only when enabling the resource
    if (bEnable && 
       (FALSE == (bRet = PollRegisterField(pResources[i].uStatusReg, pResources[i].uStatusMask, uCheck, uTimeoutUs))))
    {
      DEBUG((EFI_D_ERROR, "DisplayDxe: Failed to enable MMU resources - Timed out\n"));
    }
  }

  return bRet;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: GetUpdatedCmdLine()
**
** DESCRIPTION:
**   Append the display command line to the existing command line
**
** ---------------------------------------------------------------------- */
static bool32 GetUpdatedCmdLine(CHAR8* pOrig, uint32 uOrigSize, CHAR8 **ppUpdatedStr, int *piUpdatedSize)
{

  CHAR8 *pCmdLine = NULL;
  UINT32 uLen     = 0;
  UINT32 uSize    = 0;
  bool32 bUpdate  = FALSE;
  CHAR8* pStr     = NULL;

  /* Get the display command line from Display ABL interface */
  Display_Utils_GetDisplayCommandLine(&pCmdLine, &uLen);

  /* calculate size including termination*/
  uSize = sizeof(CHAR8) * (uLen + 1);

  if ((NULL == pCmdLine) ||
      (0 == uLen))
  {
    /* No panel configuration available */
  }
  else if (NULL == (pStr = MDP_OSAL_CALLOC(uOrigSize + uSize)))
  {
    DEBUG ((EFI_D_ERROR, "Display: Failed to allocate memory for display command line\n"));
  }
  /* Replace null termination with space */
  else
  {
    /* uOrigSize is checked for zero in the calling function */
    UINT32 uOrigLen = uOrigSize - sizeof(CHAR8);

    /* Copy original string - without termination character*/
    MDP_OSAL_MEMCPY(pStr, pOrig, uOrigLen * sizeof(CHAR8));

    /* space to separate */
    pStr[uOrigLen] = ' ';

    /* copy display command line - has termination at end*/
    MDP_OSAL_MEMCPY(&pStr[uOrigLen + 1], pCmdLine, uSize);

    /* Return updated data & size */
    *ppUpdatedStr = pStr;
    *piUpdatedSize = (int) uOrigSize + uSize;

    /* Flag property update */
    bUpdate = TRUE;
  }
  
  return bUpdate;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: UpdateDisplayCmdLine()
**
** DESCRIPTION:
**   Update the display command line in the device tree node
**
** ---------------------------------------------------------------------- */
static void UpdateDisplayCmdLine(void *pFdt)
{
  int           iRet     = 0;
  const CHAR8  *pOrigStr = NULL;
  CHAR8        *pNewStr  = NULL;
  int           iOrigSize= 0;
  int           iNewSize = 0;
  int           iOffset;

  /* Get offset of the chosen node */
  if (0 > (iOffset = fdt_path_offset(pFdt, "/chosen")))
  {
    DEBUG ((EFI_D_ERROR, "ERROR: Could not find chosen node ...\n"));
  }
  else if ((NULL == (pOrigStr = (const CHAR8*) fdt_getprop(pFdt, iOffset, (CONST char*)"bootargs", &iOrigSize))) ||
           (iOrigSize <= 0 ))
  {
    DEBUG ((EFI_D_ERROR, "Display: Could not get bootargs from device tree\n"));
  }
  else if (FALSE == GetUpdatedCmdLine((CHAR8*) pOrigStr, iOrigSize, &pNewStr, &iNewSize))
  {
    /* Nothing to update - Ignore and quit */
  }
  /* Set property */
  else if (0 != (iRet = fdt_setprop(pFdt, iOffset, (const char*)"bootargs", (const void*)pNewStr, iNewSize)))
  {
    DEBUG ((EFI_D_ERROR, "Display: Failed to update display command line on device tree. Return code: %d\n", iRet));
  }

  /* Free memory if allocated */
  if (NULL != pNewStr)
  {
    MDP_OSAL_FREE(pNewStr);
  }
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: UpdateSplashNode()
**
** DESCRIPTION:
**   Update the continuous splash device tree node
**
** ---------------------------------------------------------------------- */
static void UpdateSplashNode(void *pFdt)
{
  CONST struct fdt_property  *pProperty     = NULL;
  int                         iPropertySize = 0;
  int                         iRet          = 0;
  int                         iOffset;
  
  /* Get offset of the splash memory reservation node */
  if (0 > (iOffset = fdt_path_offset(pFdt, SPLASH_MEM_DT_NODE_NAME)))
  {
    DEBUG ((EFI_D_ERROR, "Display: Could not get splash memory region node\n"));
  }
  /* Get the property that specifies the splash memory details */
  else if (0 == (pProperty = fdt_get_property(pFdt, iOffset, SPLASH_MEM_DT_REG, &iPropertySize)))
  {
    DEBUG ((EFI_D_ERROR, "Display: Could not find the splash reg property\n"));
  }
  else if (iPropertySize != SPLASH_MEM_PROP_SIZE)
  {
    DEBUG ((EFI_D_ERROR, "Display: Splash memory node size mismatch in device tree. Expected: %d, Actual: %d\n",
            SPLASH_MEM_PROP_SIZE, iPropertySize));
  }
  else 
  {
    UINT32* pData = (UINT32*) pProperty->data;
    UINT32  uAddr = 0;
    UINT32  uSize = 0;

    /* Get splash buffer info from Display ABL */
    Display_Utils_GetContinuousSplashInfo(&uAddr, &uSize);
    
    pData[0] = 0;
    pData[1] = cpu_to_fdt32(uAddr);
    pData[2] = 0;
    pData[3] = cpu_to_fdt32(uSize);
    
    /* Update the property value in place */
    if (0 > (iRet = fdt_setprop_inplace(pFdt, iOffset, SPLASH_MEM_DT_REG, pProperty->data, iPropertySize)))
    {
      DEBUG ((EFI_D_ERROR, "Display: Could not update splash mem device tree node\n"));
    } 
  }
}

/*=========================================================================
      Public APIs
==========================================================================*/
extern EFI_GUID gOSAVendorGuid;
extern EFI_GUID gQcomTokenSpaceGuid;


/****************************************************************************
*
** FUNCTION: MDP_Log_Message()
*/
/*!
* \brief
*   This function will perform the logging of Debug messages
*
* \param [in] uLogType   - Log level
*        [in] pLogString - Log String
*
* \retval void
*
****************************************************************************/
void MDP_Log_Message(MDP_LogLevel uLogType, char *pLogString, ...)
{
  CHAR8    buffer[DISP_MAX_DEBUG_MESSAGE_LENGTH];
  VA_LIST  marker;
  UINT32   uLogLevel;
  switch (uLogType)
  {
  case MDP_LOGLEVEL_INFO:
    uLogLevel = EFI_D_INFO;
    break;
  case MDP_LOGLEVEL_WARN:
    uLogLevel = EFI_D_WARN;
    break;
  case MDP_LOGLEVEL_ERROR:
  default:
    uLogLevel = EFI_D_ERROR;
    break;
  }
  //
  // Convert the DEBUG() message to a Unicode String
  //
  VA_START (marker, pLogString);
  AsciiVSPrint(buffer, DISP_MAX_DEBUG_MESSAGE_LENGTH, pLogString, marker);
  VA_END (marker);

  DEBUG ((uLogLevel, buffer));
}

/****************************************************************************
*
** FUNCTION: MDP_ReadUnaligned_U32()
*/
/*!
* \brief
* This function reads the uint16 values from aligned boundary and packs them into uint32
*
* if pSrc is at offset of 1 or 3 from the uint32 aligned address
*            reads uint16 from aligned (pSrc -1) address and extracts the LSB 
*            reads next uint16 addresses and 
*            reads the next +1 uint16 address and extracts the MSB
*            Packs these into uint32 pDest address
*
*if pSrc is at offset 2 from the the uint32 aligned address
*            reads uint16 from aligned (pSrc -1) address 
*            reads next uint16 addresses 
*            Packs these into uint32 pDest addr
*
* Otherwise pSrc is on aligned address boundary

*
* \param [in] pDest   - pointer to Uint32
*        [in] pSrc    - pointer to Uint8 
*
* \retval void
*
****************************************************************************/
void MDP_ReadUnaligned_U32(uint32 *pDest, uint8 * pSrc)
{
    // if pSrc is at offset of 1 or 3 from the uint32 aligned address
    if (0 != ((UINTN)pSrc & 0x1) ) 
    {
        uint16 *pTemp = (uint16*)((uint8 *)(pSrc) - 1); 

         // Pack these into uint32 pDest address
        *pDest = (uint8)(*(pTemp + 2) &  0xff) << 24 | (uint16)(*(pTemp + 1) << 16) | (uint8)(*pTemp >> 8); 
    } 
    // if pSrc is at offset 2 from the the uint32 aligned address
    else if (0 != ((UINTN)(pSrc) & 0x2) )
    { 
       uint16 *pTemp = (uint16*)pSrc; 

       // Packs these into uint32 pDest addr
       *pDest =((uint16)*(pTemp + 1) << 16) | (uint16)*pTemp ;
    }
    // Otherwise pSrc is on aligned address boundary
    else 
    {
        *pDest = *(uint32 *)pSrc; 
    }
}


/****************************************************************************
*
** FUNCTION: MDP_OSAL_MB()
*/
/*!
* \brief
*   This function creates a hardware memory barrier (fence) that prevents the CPU from 
*  re-ordering read and write operations.
*
* \param void
*        
*
* \retval void
*
****************************************************************************/
void MDP_OSAL_MB(void)
{
    ArmDataMemoryBarrier();
}


/****************************************************************************
*
** FUNCTION: MDP_SetFirmwareEnvironmentVariable()
*/
/*!
* \brief
*   Program the platform environment variable
*
* \param [in] pVariableNpFirmwareEnvInfoame - Environment variable 
*
* \retval MDP_STATUS_OK on success
*
****************************************************************************/
MDP_Status MDP_SetFirmwareEnvironmentVariable(void *pValue, UINT32 uSize)
{
  MDP_Status    eStatus            = MDP_STATUS_OK;
  EFI_GUID      sOutputGUID        = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  const CHAR16  sPlatformEnvName[] = L"UEFIDisplayInfo";

  
  if (EFI_SUCCESS != gRT->SetVariable((CHAR16*)&sPlatformEnvName, &sOutputGUID,
                                      EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                      uSize,
                                      (void*)pValue))
  {
    DEBUG ((EFI_D_ERROR, "MDPSystem: Failed to update UEFI Environment variable (UEFIDisplayInfo)!\n"));  
    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDP_GetEnvironmentVariable()
*/
/*!
* \brief
*   Helper to retrieve a system environment variable.
*
* \param [in]  pVariableName - Environment variable name
*        [out] pValue        - Variable storage
*        [in]  uSize         - Size of the environment variable
*
* \retval MDP_STATUS_OK on success
*
****************************************************************************/
MDP_Status MDP_GetEnvironmentVariable(CHAR16 *pVariableName, void *pValue, UINT32 uSize)
{
  MDP_Status eStatus      = MDP_STATUS_OK;
  UINT32     uAttributes  = 0;
  UINT8      uStorage[MDP_OSAL_MAX_ENVVAR_SIZE];
  UINTN      uStorageSize = MDP_OSAL_MAX_ENVVAR_SIZE;  

  if (uSize > MDP_OSAL_MAX_ENVVAR_SIZE)
  {
   eStatus = MDP_STATUS_FAILED;  
  }
  else if (EFI_SUCCESS == gRT->GetVariable((CHAR16*)pVariableName, 
                                      &gOSAVendorGuid,
                                      &uAttributes,
                                      (UINTN*)&uStorageSize,
                                      (void*)uStorage))
  {
    MDP_OSAL_MEMCPY(pValue, &uStorage, uSize);
  }
  else
  {
    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_GetBootServiceVariableSize()
*/
/*!
* \brief
*   Helper to retrieve data size for system variable from the QcomTokenSpace GUID.
*
* \param [in]  pVariableName - Environment variable name
*        [out] puSize        - Size of the environment variable
*
* \retval EFI_STATUS
*
****************************************************************************/
EFI_STATUS MDP_GetBootServiceVariableSize(CHAR16 *pVariableName, UINTN *puSize)
{
  void       *pValue       = NULL;
  UINTN       uSize        = 0;
  EFI_STATUS  eStatus;

  if ((NULL == pVariableName) ||
      (NULL == puSize))
  {
    eStatus = EFI_INVALID_PARAMETER;
  }
  else
  {
    /* 
    * GetVariable call with NULL pointer for data will return the actual size
    * and EFI_BUFFER_TOO_SMALL error, if the variable exists
    */
    eStatus = gRT->GetVariable(pVariableName, 
                              &gQcomTokenSpaceGuid,
                               0,
                              &uSize,
                               pValue);

    if (EFI_BUFFER_TOO_SMALL == eStatus)
    {
      /* This is the expected result. Set the size and update return status */
      *puSize = uSize;
      eStatus = EFI_SUCCESS;
    }
    else
    {
      DEBUG((EFI_D_INFO, "MDPSystem: Failed to get size for variable <%s>. Error: %r\n", pVariableName, eStatus));
    }
    
  }
  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_GetBootServiceVariable()
*/
/*!
* \brief
*   Helper to retrieve a system environment variable from the QcomTokenSpace GUID.
*
* \param 
*        [in]   pVariableName - Environment variable name
*        [out]  pValue        - Variable storage
*        [out]  uSize         - Size of the environment variable
*
* \retval EFI_STATUS
*
****************************************************************************/
EFI_STATUS MDP_GetBootServiceVariable(CHAR16 *pVariableName, void *pValue, UINTN *puSize)
{
  UINT32     uAttributes  = 0;
  EFI_STATUS eStatus;

  if ((NULL == pVariableName) ||
      (NULL == pValue)        ||
      (NULL == puSize))
  {
    DEBUG((EFI_D_ERROR, "MDPSystem: Null parameter\n"));
    eStatus = EFI_INVALID_PARAMETER;
  }
  else if (EFI_SUCCESS != (eStatus = gRT->GetVariable(pVariableName,
                                                     &gQcomTokenSpaceGuid,
                                                     &uAttributes,
                                                      puSize,
                                                      pValue)))
  {
    /* No error message if variable not found */
    if (EFI_NOT_FOUND != eStatus)
    {
      DEBUG((EFI_D_WARN, "MDPSystem: Failed to get variable <%s>. Error: %r\n", pVariableName, eStatus));
    }
  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDP_SetBootServiceVariable()
*/
/*!
* \brief
*   Helper to set a system environment variable using the QcomTokenSpace GUID.
*
* \param [in]  pVariableName - Environment variable name
*        [in]  pValue        - Variable storage
*        [in]  uSize         - Size of the environment variable
*        [in]  uFlags        - the MDP variable flags
*
* \retval EFI_STATUS
*
****************************************************************************/
EFI_STATUS MDP_SetBootServiceVariable(CHAR16 *pVariableName, void *pValue, UINTN uSize, UINT32 uFlags)
{
  EFI_STATUS eStatus     = EFI_SUCCESS;
  UINT32     uAttributes = 0;

  if ((NULL == pVariableName) ||
      (NULL == pValue))
  {
    DEBUG((EFI_D_WARN, "MDPSystem: Null parameter\n"));
    eStatus = EFI_INVALID_PARAMETER;
  }
  else
  {
    uAttributes = EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS;

    if (uFlags & MDP_VARIABLE_FLAG_NON_VOLATILE)
    {
      uAttributes |= EFI_VARIABLE_NON_VOLATILE;
    }

    if (EFI_SUCCESS != (eStatus = gRT->SetVariable(pVariableName,
                                                  &gQcomTokenSpaceGuid,
                                                   uAttributes,
                                                   uSize,
                                                   pValue)))
    {
      DEBUG((EFI_D_WARN, "MDPSystem: Failed to set variable <%s>. Error: %r\n", pVariableName, eStatus));
    }
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_SetupMMUSIDs()
*/
/*!
* \brief
*   Helper to program Display MMU SID to bypass
*
* \param [in] uMajorVersion - MDSS  major version
*        [in] uMinorVersion - MDSS  minor version
*
* \retval MDP_STATUS_OK on success
*
****************************************************************************/
MDP_Status MDP_SetupMMUSIDs(uint32  uMajorVersion, uint32 uMinorVersion)
{
    MDP_Status                   eStatus    = MDP_STATUS_OK;
    HWIOFieldType                *pCmds     = gMMU_regs;  
    uint32                       uCmdCount  = sizeof(gMMU_regs)/sizeof(HWIOFieldType);
    uint32 i                                = 0;
    uint32 smrValue                         = SMR_VALID; // Valid SMR
    uint32 cbarValue                        = 0xFF; // non-zero value

    // find the empty SMR
    while(((smrValue&SMR_VALID) == SMR_VALID) && (i < TOTAL_SMR))
    {
      smrValue = in_dword(((gMMU_regs[0].uReg+i*4)));
      i++;
    }  
    // i-1 index is empty SMR starting index from lower index
    if((i == 0) || (i == TOTAL_SMR))
    {
      return MDP_STATUS_FAILED;
    }
    // Update SMR and S2CR addr
    gMMU_regs[0].uReg = gMMU_regs[0].uReg + (i-1)*4;
    gMMU_regs[1].uReg = gMMU_regs[1].uReg + (i-1)*4;
    gMMU_regs[2].uReg = gMMU_regs[0].uReg + 4;
    gMMU_regs[3].uReg = gMMU_regs[1].uReg + 4;

    // find the unused CB, starting from lower index
    i = 0;
    while(((cbarValue != EMPTY_CB) && (cbarValue != CBAR_POR_NOAC)) && (i < TOTAL_CB))
    {
      cbarValue = in_dword(((gMMU_regs[4].uReg+i*4)));
      i++;
    }
    // i-1 is empty CB
    if(i == TOTAL_CB)
    {
      return MDP_STATUS_FAILED;
    }
    
    if(i != 0)
    {
      //Update the CBAR CBA2R and SCTLR addr
      gMMU_regs[4].uReg = gMMU_regs[4].uReg + (i-1)*4;
      gMMU_regs[5].uReg = gMMU_regs[5].uReg + (i-1)*4;
      gMMU_regs[6].uReg = gMMU_regs[6].uReg + (i-1)*0x1000;

      //Update the S2CR with new CB value
      gMMU_regs[1].uValue = (i-1);
      gMMU_regs[3].uValue = (i-1);
    }

    //Setup display MMU SID to bypass
    for (i = 0;i < uCmdCount; i++)
    {
      WriteField(pCmds[i].uReg, pCmds[i].uMask, pCmds[i].uValue);
    }

    //Barrier
    MDP_OSAL_MB();

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_OSAL_MEMZERO()
*/
/*!
* \brief
*   Fills a target buffer with 0 value
*
* \param [in] pBuffer   - The memory to set
*        [in] uLength   - The number of bytes to set
*
* \retval void
*
****************************************************************************/
void MDP_OSAL_MEMZERO(void *pBuffer, UINTN uLength)
{
  gBS->SetMem(pBuffer, uLength, 0x0);
}


/****************************************************************************
*
** FUNCTION: MDP_OSAL_MEMCPY()
*/
/*!
* \brief
*   Copies a source buffer to a destination buffer
*
* \param [in] pDest   - The pointer to the destination buffer of the memory copy
*        [in] pSrc    - The pointer to the source buffer of the memory copy.
*        [in] uLength - The number of bytes to copy from SourceBuffer to DestinationBuffer.

*
* \retval void
*
****************************************************************************/
void MDP_OSAL_MEMCPY(void *pDest, void *pSrc, UINTN uLength)
{
  CopyMem(pDest, pSrc, uLength);
}

/****************************************************************************
*
** FUNCTION: MDP_OSAL_MEMSET()
*/
/*!
* \brief
*   Fills a target buffer with a byte value
*
* \param [in] pBuffer - The memory to set
*        [in] uLength - The number of bytes to set
*        [in] uValue  - The value with which to fill Length bytes of Buffer
*
* \retval void
*
****************************************************************************/
void MDP_OSAL_MEMSET(void  *pBuffer,  UINT8  uValue, UINTN  uLength)
{
  gBS->SetMem(pBuffer, uLength, uValue);
}


/****************************************************************************
*
** FUNCTION: MDP_OSAL_CALLOC()
*/
/*!
* \brief
*   Allocate memory from local heap
*
* \param [in] uLength   - Requested size of memory to be allocated
*
* \retval point to the block of allocated memory
*
****************************************************************************/
void* MDP_OSAL_CALLOC(UINTN uLength)
{
  return UncachedAllocateZeroPool(uLength);
}

/****************************************************************************
*
** FUNCTION: MDP_OSAL_FREE()
*/
/*!
* \brief
*   Free allocated memory from local heap
*
* \param [in] pBuffer   - Pointer to block of memory allocated
*
* \retval void
*
****************************************************************************/
void MDP_OSAL_FREE(void* pBuffer)
{
  UncachedSafeFreePool(pBuffer);
}


/****************************************************************************
*
** FUNCTION: MDP_OSAL_DELAYMS()
*/
/*!
* \brief
*   This function will sleep for a given time in milliseconds
*
* \param [in] uDelayMs   - Sleep time in milliseconds
*
* \retval void
*
****************************************************************************/
void MDP_OSAL_DELAYMS(UINTN uDelayMs)
{
  gBS->Stall((uint32)(uDelayMs)* 1000);
}


/****************************************************************************
*
** FUNCTION: MDP_OSAL_DELAYUS()
*/
/*!
* \brief
*   This function will sleep for a given time in microsecond
*
* \param [in] uDelayUs   - Sleep time in microsecond
*
* \retval void
*
****************************************************************************/
void MDP_OSAL_DELAYUS(UINTN uDelayUs)
{
  gBS->Stall((uint32)(uDelayUs));
}


/****************************************************************************
*
** FUNCTION: MDP_SaveFirmwareEnvironmentVariable()
*/
/*!
* \brief
*   This function will save display info into UEFI environment variable
*
* \param [in] eDisplayId        - Display ID
*
* \retval void
*
****************************************************************************/
MDP_Status MDP_SaveFirmwareEnvironmentVariable(MDP_Display_IDType eDisplayId)
{
  MDP_Status          eStatus          = MDP_STATUS_OK;
  MDP_Panel_AttrType *pDisplayInfo     = MDP_GET_DISPLAYINFO(eDisplayId);
  MDPSurfaceInfo     *pFrameBufferInfo = &pDisplayInfo->sFrameBuffer;
  uint32              uBacklightLevel  = pDisplayInfo->uBacklightLevel;
  MDPPlatformParams   sPlatformParams;

  if (MDP_STATUS_OK == (eStatus = MDPPlatformConfigure(eDisplayId, MDPPLATFORM_CONFIG_GETPLATFORMINFO, &sPlatformParams)))
  {
    UINT32                      FrameBufferSize       = 0;
    UINT8                       uHighestBankBit       = 0;           //0 is defined to be error value with GFX
    CHAR8*                      MemLabel              = "Display Reserved";
    MemRegionInfo               DisplayMemRegionInfo;
    MDPFirmwareEnvType          sFirmwareEnvInfo;
    DisplaySplashBufferInfoType sSplashInfo;
    EFI_STATUS Status;
    
    MDP_OSAL_MEMZERO(&sFirmwareEnvInfo, sizeof(MDPFirmwareEnvType));
    MDP_OSAL_MEMZERO(&sSplashInfo, sizeof(DisplaySplashBufferInfoType));

    // Get the Display region.
    Status = GetMemRegionInfoByName(MemLabel, &DisplayMemRegionInfo);
    if (Status != EFI_SUCCESS)
    {
      DEBUG((EFI_D_WARN, "MDPLib: Could not find the Display memory region. Defaulting to 8MB.\n"));
      FrameBufferSize = 0x00800000; /* Default to 8MB */
    }
    else
    {
      FrameBufferSize = DisplayMemRegionInfo.MemSize;
    }

    if (EFI_SUCCESS != (Status = GetHighestBankBit(&uHighestBankBit)))
    {
      DEBUG((EFI_D_WARN, "MDPLib: GetHighestBankBit failed.\n"));
    }

    // Populate firmware environment information
    sFirmwareEnvInfo.uVersionInfo = (MDP_FIRMWARE_ENV_VERSION_MAGIC << 16) |
                                    (MDP_FIRMWARE_ENV_VERSION_MAJOR << 8) |
                                    (MDP_FIRMWARE_ENV_VERSION_MINOR);
    sFirmwareEnvInfo.uPlatformId      = sPlatformParams.sPlatformInfo.sEFIPlatformType.platform;
    sFirmwareEnvInfo.uChipsetId       = sPlatformParams.sPlatformInfo.sEFIChipSetId;
    sFirmwareEnvInfo.uPlatformVersion = 0; // Reserved
                                           // Frame buffer information comes from the currently cached frame buffer
    sFirmwareEnvInfo.uFrameBufferAddress      = (UINTN)pFrameBufferInfo->pPlane0Offset;
    // We report the frame buffer size as the carve out memory size which is sum of PcdFrameBufferSize and PcdHDMISize
    sFirmwareEnvInfo.uFrameBufferSize         = FrameBufferSize;
    sFirmwareEnvInfo.uFrameBufferPixelDepth   = (8 * pFrameBufferInfo->uPlane0Stride) / pFrameBufferInfo->uWidth;
    sFirmwareEnvInfo.uFrameBufferImagePxWidth = pFrameBufferInfo->uWidth;
    sFirmwareEnvInfo.uFrameBufferImagePxHeight= pFrameBufferInfo->uHeight;
    sFirmwareEnvInfo.uFrameBufferImageStride  = pFrameBufferInfo->uPlane0Stride;
    sFirmwareEnvInfo.uHighestBankBit          = uHighestBankBit;

    // Populate primary panel information
    if (MDP_DISPLAY_PRIMARY == eDisplayId)
    {
      // Allocate any required reserved regions, if not allocated already.
      if (0 == pDisplayInfo->uReservedAllocationSize)
      {
        if (MDP_STATUS_OK == MDPAllocateReservedMemory(sPlatformParams.sPlatformInfo.sEFIChipSetFamily, &sFirmwareEnvInfo.uReservedMemoryAddress, &sFirmwareEnvInfo.uReservedMemorySize))
        {
          // Keep track of the allocation size so we don't allocate it again
          pDisplayInfo->uReservedAllocationSize = sFirmwareEnvInfo.uReservedMemorySize;
        }
        else
        {
          // Reset values on failure.
          sFirmwareEnvInfo.uReservedMemoryAddress = 0x0;
          sFirmwareEnvInfo.uReservedMemorySize    = 0x0;
        }
      }

      // If panel configuration has provided the panel id, then override panel id with this value
      // This is required when panel supports multiple modes with same panel id, by overriding 
      // the panel id for a particular configuration HLOS can identify that specifc mode of panel
      sFirmwareEnvInfo.uPrimaryPanelId             = (0 != pDisplayInfo->uPanelId) ? pDisplayInfo->uPanelId : sPlatformParams.sPlatformInfo.uPrimaryPanelId;
      sFirmwareEnvInfo.uPrimaryPanelFlags          = MDP_FIRMWARE_ENV_FLAG_DISPLAY_INITIALIZED;
      sFirmwareEnvInfo.uPrimaryPanelBacklightLevel = uBacklightLevel;
    }

    eStatus = MDP_SetFirmwareEnvironmentVariable(&sFirmwareEnvInfo, sizeof(sFirmwareEnvInfo));

    //Set frame buffer variable for ABL
    sSplashInfo.uVersion   = DISPLAY_SPLASH_BUFFER_INFO_TYPE_VERSION;
    sSplashInfo.uFrameAddr = (UINTN)pFrameBufferInfo->pPlane0Offset;
    sSplashInfo.uFrameSize = FrameBufferSize - MDP_MAX_PLLCODES_SIZE;

    //Save the info in Display ABL
    Display_Utils_SetContinuousSplashInfo(sSplashInfo.uFrameAddr, sSplashInfo.uFrameSize);

    MDP_SetBootServiceVariable(DISPVAR_SPLASH_BUFFER_INFO, &sSplashInfo, sizeof(sSplashInfo), 0);
    
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPAcquireLockOrFail()
*/
/*!
* \brief
*   This function will try to get the specified lock.
*
* The function will acquire the lock if not already acquired
* It will return FAIL if lock is already acquired
*
* \param  eLock              - Lock to acquire
*
* \retval MDP_STATUS_OK      - Lock acquired successfully
* \retval MDP_STATUS_FAILED  - Failed to acquire Lock
*
****************************************************************************/
MDP_Status  MDPAcquireLockOrFail(MDP_Lock_Type eLock)
{
  MDP_Status eStatus = MDP_STATUS_OK;

  if (MDP_LOCK_TYPE_MAX <= eLock)
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    // uLock = 0 -> free, uLock = 1 -> acquired
    if (InterlockedCompareExchange32(&sLockInfo.uLock[eLock], 0, 1) != 0)
    {
      // failed to acquire lock
      eStatus = MDP_STATUS_FAILED;
    }
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPReleaseLock()
*/
/*!
* \brief
*   This function will release the specified lock.
*
* \param  eLock          - Lock to release
*
* \retval MDP_STATUS_OK  - Lock released successfully
*
****************************************************************************/
MDP_Status  MDPReleaseLock(MDP_Lock_Type eLock)
{
  MDP_Status eStatus = MDP_STATUS_OK;

  if (MDP_LOCK_TYPE_MAX <= eLock)
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    // uLock = 0 -> free, uLock = 1 -> acquired
    InterlockedCompareExchange32(&sLockInfo.uLock[eLock], 1, 0);
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_GetConfigValue()
*/
/*!
* \brief
*   Get config value created by parsing the platform config file
*
* \param [in] Key         - Pointer to ASCII parameter to search for in config file
*        [in] Value       - On input pointer to buffer used to return value
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status MDP_GetConfigValue(IN CHAR8* Key, IN OUT UINT32* Value)
{
  MDP_Status eStatus = MDP_STATUS_OK;

  if (EFI_SUCCESS != GetConfigValue(Key, Value))
  {
    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_GetTimerCountMS()
*/
/*!
* \brief
*    Running time in mS based on the Arch Timer Counter.
*    This could get wrapped around based on the resolution and Speed.So
*    use with caution.
*    If the Timer is 32 bit running at 19.2 MHz, it would wrap around, to 0
*    after 223 Sec
*
* \param NONE  - Log level
*
* \retval Timer count
*
****************************************************************************/
UINT32  MDP_GetTimerCountMS(void)
{
  return GetTimerCountms();
}

/****************************************************************************
*
** FUNCTION: MDP_GetTimerCountUS()
*/
/*!
* \brief
*    Running time in uS based on the Arch Timer Counter.
*    This could get wrapped around based on the resolution and Speed.So
*    use with caution.
*    If the Timer is 32 bit running at 19.2 MHz, it would wrap around, to 0
*    after 223 Sec
*
* \param NONE  - Log level
*
* \retval Timer count
*
****************************************************************************/
UINT32     MDP_GetTimerCountUS(void)
{
  return GetTimerCountus();
}

/****************************************************************************
*
** FUNCTION: MDP_Display_SetVariable()
*/
/*!
* \brief
*   Function to set the display variable.
*
  @param[in]  pVariableName              The variable name that need to set.
  @param[in]  pVariableValue             The variable value.
  @param[in]  uDataLength                The length of the variable value.
  @param[in]  uFlags                     The flag to set variable.
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status MDP_Display_SetVariable (CHAR16 *pVariableName, UINT8 *pVariableValue, UINTN uDataLength, UINTN uFlags)
{
  MDP_Status  eStatus = MDP_STATUS_OK;

  if (EFI_SUCCESS != Display_Utils_SetVariable(pVariableName, pVariableValue, uDataLength, uFlags))
  {
    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_Display_GetVariable()
*/
/*!
* \brief
*   Function to get the display variable.
*
  @param[in]   pVariableName              The variable name that need to get.
  @param[out]  pVariableValue             The variable value.
  @param[out]  pDataLength                The length of the variable value.
  @param[in]   uFlags                     The flag to get variable.
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status MDP_Display_GetVariable (CHAR16 *pVariableName, UINT8 *pVariableValue, UINTN *pDataLength, UINTN uFlags)
{
  MDP_Status  eStatus = MDP_STATUS_OK;

  if (EFI_SUCCESS != Display_Utils_GetVariable(pVariableName, pVariableValue, pDataLength, uFlags))
  {
    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_Display_SetVariable_Integer()
*/
/*!
* \brief
*   Function to set the display variable.
*
  @param[in]  pVariableName              The variable name that need to set.
  @param[in]  uVariableValue             The variable value.
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status MDP_Display_SetVariable_Integer (CHAR16 *pVariableName, UINT32 uVariableValue)
{
  MDP_Status  eStatus       = MDP_STATUS_OK;
  CHAR8       VarValue[64]  =  "";

  AsciiSPrint (VarValue, sizeof(VarValue), "%x", uVariableValue);

  if (EFI_SUCCESS  != Display_Utils_SetVariable(pVariableName, (UINT8 *)VarValue, AsciiStrLen (VarValue), 0))
  {
    eStatus = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDP_Display_GetVariable_Integer()
*/
/*!
* \brief
*   Function to get the display variable.
*
  @param[in]   pVariableName              The variable name that need to get.
  @param[out]  pVariableValue             The variable value.
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status MDP_Display_GetVariable_Integer (CHAR16 *pVariableName, UINT32 *pVariableValue)
{
  MDP_Status  eStatus       = MDP_STATUS_OK;
  CHAR8       VarValue[64]  =  "";
  UINTN       uVarLength    =  sizeof(VarValue) / sizeof(CHAR8);

  if ((EFI_SUCCESS  == Display_Utils_GetVariable(pVariableName, (UINT8 *)VarValue, &uVarLength, 0)) &&
      (uVarLength  > 0))
  {
    *pVariableValue = AsciiStrHexToUintn(VarValue);
  }
  else
  {
    *pVariableValue = 0;
    eStatus         = MDP_STATUS_FAILED;
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPAllocateReservedMemory()
*/
/*!
* \brief
*   This function will allocate a reserved memory region. Target specific
*
* \param [in]  eChipId                 - Chipset family
*        [out] pReservedMemoryAddress  - pointer to the allocated memory address
*        [out] pReservedMemorySize     - pointer to the allocated memory size
*
* \retval MDP_Status PcdReservedMemory alignment
*
****************************************************************************/
static MDP_Status MDPAllocateReservedMemory(EFIChipInfoFamilyType eChipId, uint64 *pReservedMemoryAddress, uint64 *pReservedMemorySize)
{
  MDP_Status       eStatus        = MDP_STATUS_OK;
  uint32           uMemSize       = PcdGet32(PcdReservedMemorySize);
  uint32           uMemAlignment  = PcdGet32(PcdReservedMemoryAlignment);

  if ((NULL == pReservedMemoryAddress) ||
      (NULL == pReservedMemorySize))
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else if ((0 == uMemSize)    ||
           (0 == uMemAlignment))
  {
    //No need to allocate memory
    eStatus = MDP_STATUS_OK;
  }
  else
  {
    UINTN   uReservedMemorySize       = 0;
    UINTN   uReservedMemoryAlignment  = 0;
    void   *pMemoryAddress            = NULL;

    switch (eChipId)
    {
    case EFICHIPINFO_FAMILY_MSM8998:
      uReservedMemorySize       = MDP_MEGA(uMemSize);
      uReservedMemoryAlignment  = MDP_KILOBYTES(uMemAlignment);
      break;
    default:
      uReservedMemorySize       = 0;
      uReservedMemoryAlignment  = 0;
      break;
    }

    if (uReservedMemorySize > 0)
    {
      UINTN uPages = EFI_SIZE_TO_PAGES(uReservedMemorySize);

      /* Allocate reserved memory */
      if (NULL == (pMemoryAddress = AllocateAlignedReservedPages(uPages, uReservedMemoryAlignment)))
      {
        DEBUG((EFI_D_ERROR, "MDPAllocateReservedMemory() failed to allocate %d bytes (%d pages).\n", uReservedMemorySize, uPages));
        eStatus = MDP_STATUS_BAD_PARAM;
      }
      else
      {
        /* Update the memory size after a successful allocation */
        uReservedMemorySize = EFI_PAGES_TO_SIZE(uPages);
        DEBUG((EFI_D_INFO, "MDPAllocateReservedMemory() allocated %d bytes.\n", uReservedMemorySize));
      }
    }

    *pReservedMemoryAddress = (uint64)pMemoryAddress;
    *pReservedMemorySize    = uReservedMemorySize;
  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDP_UpdateDeviceTree()
*/
/*!
* \brief
*   Update display specific devicee tree nodes
*
* \param [in] pFdt         - Device tree address
*
* \retval EFI_STATUS
*
****************************************************************************/
EFI_STATUS MDP_UpdateDeviceTree(void *pFdt)
{
  EFI_STATUS                  eStatus       = EFI_NOT_FOUND;
  int                         iRet           = 0;

  if (0 != fdt_check_header(pFdt))
  {
      DEBUG ((EFI_D_ERROR, "Display: Failed to validate device tree header\n"));
  }
  /* Add padding to make space for new nodes and properties. */
  else if (0 != (iRet = fdt_open_into(pFdt, pFdt, fdt_totalsize(pFdt) + DTB_PAD_SIZE)))
  {
    DEBUG ((EFI_D_ERROR, "Display: Failed to move/resize device tree buffer\n"));
  }
  else 
  {
    /* Update display command line*/
    UpdateDisplayCmdLine(pFdt);

    /* Update continuous splash node */
    UpdateSplashNode(pFdt);
    
    if (0 != (iRet = fdt_pack(pFdt)))
    {
      DEBUG((EFI_D_ERROR, "Display: Device tree write failed: %d\n", iRet));
    }
    eStatus = EFI_SUCCESS;
  }
  
  return eStatus;
}

