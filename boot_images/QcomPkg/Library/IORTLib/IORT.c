/**
 *
 * @file IORT.c
 * 
 * @brief Implementation of IORT API on top of ACPI stored in Device
 *        Configuration
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include "IORT.h"
#include "DALSys.h"
#include "DALStdErr.h"

#pragma pack(1)
typedef struct _IORT {
    uint32 Signature;
    uint32 Length;
    uint8 Revision;
    uint8 Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32 OEMRevision;
    uint32 CreatorID;
    uint32 CreatorRevision;
    uint32 NumIORTNodes;
    uint32 IORTNodeOffset;
    uint32 Reserved;
} IORT;

typedef struct _IORTGenericNode {
    uint8 Type;
    uint16 Length;
    uint8 Revision;
    uint32 Reserved;
    uint32 NumIDMappings;
    uint32 IDArrayRef;
} IORTGenericNode;

typedef struct _IORTMemoryAccessProps {
    uint32 CacheCoherentAttr;
    uint8 AllocationHints;
    uint16 Reserved;
    uint8 MemoryAccessFlags;
} IORTMemoryAccessProps;

typedef struct _IORTNamedComponentNode {
    IORTGenericNode GenericNode;
    uint32 Flags;
    IORTMemoryAccessProps MemoryAccessProperties;
    uint8 DeviceMemAddrSizeLimit;
    /* Device Object Name is variable size, and follows the DeviceMemAddrSizeLimit. */
} IORTNamedComponentNode;

typedef struct _IORTSMMUv1v2Node {
    IORTGenericNode GenericNode;
    uint64 BaseAddr;
    uint64 Span;
    uint32 Model;
    uint32 Flags;
    uint32 GlobalIntArrayOffset;
    uint32 NumCtxtIrpts;
    uint32 CtxtIrptArrayOffset;
    uint32 NumPMUIrpts;
    uint32 PMUIrptArrayOffset;
    uint32 NSgIrpt;
    uint32 NSgIrptFlags;
    uint32 NSgCfgIrpt;
    uint32 NSgCfgIrptFlags;
    /* Variable-length Context Interrupt Array, PMU Interrupt Array and
       ID Mpapings array follow NSCfgIrptFlags. */
} IORTSMMUv1v2Node;

typedef struct _IORTIDMapping {
    uint32 InputBase;
    uint32 NumIDs;
    uint32 OutputBase;
    uint32 OutputReference;
    uint32 Flags;
} IORTIDMapping;

typedef struct _IORTInterrupt {
    uint32 GSIV;
    uint32 Flags;
} IORTInterrupt;
#pragma pack()

/** IORT Node Types - From Table 3 of IORT Specification. */
#define IORTNODE_TYPE_ITS_GROUP       0
#define IORTNODE_TYPE_NAMED_COMPONENT 1
#define IORTNODE_TYPE ROOT_COMPLEX    2
#define IORTNODE_TYPE_SMMUv1v2        3
#define IORTNODE_TYPE_SMMUv3          4

typedef struct _IORTData {
    IORT* pIORT;
    uint32 NumIORTNodes;
    IORTGenericNode* pIORTNodes;
    boolean bValid;
} IORTData;

static IORTData gIORTData = {0};

static boolean IORT_Valid(void* pTable)
{
    IORT* pIORT = (IORT*)pTable;
    
    if (('TROI' == pIORT->Signature) &&
        (0 == pIORT->Revision)) {
        /* TODO: Implement Checksum validation. */
        return 1;
    } else {
        return 0;
    }
}

static IORTData* IORT_GetData()
{
    IORTData* pIORT = NULL;
    DALSYS_InitMod(NULL);
    /* TODO: Need to add a Mutex. */
    if (!gIORTData.bValid) {
        DALResult result;
        DALSYS_PROPERTY_HANDLE_DECLARE(PropHandle) = {0, 0};
        result = DALSYS_GetDALPropertyHandleStr("SMMU", PropHandle);
        if (DAL_SUCCESS == result) {
            DALSYSPropertyVar PropVar;
            result = DALSYS_GetPropertyValue(PropHandle,
                                             "IORT",
                                             0,
                                             &PropVar);
            if ((DAL_SUCCESS == result) &&
                (IORT_Valid((void*)PropVar.Val.pStruct))){
                gIORTData.pIORT = (IORT*)PropVar.Val.pStruct;
                gIORTData.NumIORTNodes = gIORTData.pIORT->NumIORTNodes;
                gIORTData.pIORTNodes = \
                    (IORTGenericNode*) ((uint8*)gIORTData.pIORT + \
                                        gIORTData.pIORT->IORTNodeOffset);
                gIORTData.bValid = 1;
            }
        }
    }
    if (gIORTData.bValid) {
        pIORT = &gIORTData;
    }
    return pIORT;
}

static IORTNamedComponentNode* IORT_FindNamedNode(char* DevObjName)
{
    IORTNamedComponentNode* pNode = NULL;
    
    IORTData* IORTData = IORT_GetData();
    
    if(IORTData) {
        IORTGenericNode* pIORTNode = IORTData->pIORTNodes;
        uint32 NumIORTNodes = IORTData->NumIORTNodes;
        while(NumIORTNodes) {
            if(IORTNODE_TYPE_NAMED_COMPONENT == pIORTNode->Type) {
                IORTNamedComponentNode* pNamedNode = (IORTNamedComponentNode*)pIORTNode;
                if (!strcmp(DevObjName, (char*)pNamedNode + sizeof(IORTNamedComponentNode))) {
                    /* Found the requested Node. Fill in the information structure. */
                    pNode = pNamedNode;
                    break;
                }
            }
            pIORTNode = (IORTGenericNode*) ((uint8*)pIORTNode + pIORTNode->Length);
            NumIORTNodes--;
        }
    }
    return pNode;
}

static IORTSMMUv1v2Node* IORT_GetSMMUv1v2Node(BaseAddr)
{
    IORTSMMUv1v2Node* pNode = NULL;
    
    IORTData* pIORTData = IORT_GetData();
    
    if(pIORTData) {
        IORTGenericNode* pIORTNode = pIORTData->pIORTNodes;
        uint32 NumIORTNodes = pIORTData->NumIORTNodes;
        while(NumIORTNodes) {
            if(IORTNODE_TYPE_SMMUv1v2 == pIORTNode->Type) {
                IORTSMMUv1v2Node* pSMMUNode = (IORTSMMUv1v2Node*)pIORTNode;
                if(pSMMUNode->BaseAddr == BaseAddr) {
                    pNode = pSMMUNode;
                    break;
                }
            }
            NumIORTNodes--;
            pIORTNode = (IORTGenericNode*) ((uint8*)pIORTNode + pIORTNode->Length);
        }
    }
    return pNode;
}

IxErrnoType IORT_GetNamedComponentInfo( 
    char *DevObjName,
    IORT_NamedComponentInfo *pNamedComponentInfo
)
{
    IxErrnoType ret = E_NOT_AVAILABLE;
    IORTNamedComponentNode* pNamedNode = IORT_FindNamedNode(DevObjName);
    
    if(pNamedNode) {
        /* Found the requested Node. Fill in the information structure. */
        pNamedComponentInfo->NumIDMappings = pNamedNode->GenericNode.NumIDMappings;
        pNamedComponentInfo->Flags = 0x0; /* Reserved. Set to 0. */
        pNamedComponentInfo->CacheCoherentAttr = pNamedNode->MemoryAccessProperties.CacheCoherentAttr;
        pNamedComponentInfo->AllocationHints = pNamedNode->MemoryAccessProperties.AllocationHints;
        pNamedComponentInfo->MemoryAccessFlags =  pNamedNode->MemoryAccessProperties.MemoryAccessFlags;
        pNamedComponentInfo->MemSizeLimit = pNamedNode->DeviceMemAddrSizeLimit;
        ret = E_SUCCESS;
    }
    
    return ret;
}

IxErrnoType IORT_GetDeviceSMMUIDMappings (
    char *DevObjName,
    uint32 IDMappingIdx,
    uint32 NumIDMappings,
    IORT_IDMapping *pIDMappings
)
{
    IxErrnoType ret = E_NOT_AVAILABLE;
    IORTData* pIORTData = IORT_GetData();
    IORTNamedComponentNode* pNamedNode = IORT_FindNamedNode(DevObjName);
    
    if (pIORTData == NULL)
    {
        return ret;
    }
    
    if(pNamedNode) {
        if ((IDMappingIdx < pNamedNode->GenericNode.NumIDMappings) &&
            (IDMappingIdx + NumIDMappings <= pNamedNode->GenericNode.NumIDMappings)) {
            IORTIDMapping *pNodeIDMappings = (IORTIDMapping*)((uint8*)pNamedNode + \
                                                               pNamedNode->GenericNode.IDArrayRef + \
                                                               IDMappingIdx * sizeof(IORTIDMapping));
            while(NumIDMappings) {
                pIDMappings->InputBase = pNodeIDMappings->InputBase;
                pIDMappings->NumIDs = pNodeIDMappings->NumIDs;
                pIDMappings->OutputBase = pNodeIDMappings->OutputBase;
                pIDMappings->OutputSMMUBaseAddr = ((IORTSMMUv1v2Node*)((uint8*)pIORTData->pIORT + pNodeIDMappings->OutputReference))->BaseAddr;
                pIDMappings->Flags = pNodeIDMappings->Flags;
                NumIDMappings--;
                pIDMappings++;
                pNodeIDMappings++;
            }
            ret = E_SUCCESS;
        } else {
            ret = E_OUT_OF_RANGE;
        }
    }
    return ret;
}

IxErrnoType IORT_GetSMMUv1v2Info(
    uint64 BaseAddr,
    IORT_SMMUv1v2Info *pSMMUInfo
)
{
    IxErrnoType ret = E_NOT_AVAILABLE;
    IORTSMMUv1v2Node* pSMMUNode = IORT_GetSMMUv1v2Node(BaseAddr);

    if(pSMMUNode) {
        pSMMUInfo->NumIDMappings = pSMMUNode->GenericNode.NumIDMappings;
        pSMMUInfo->BaseAddr = pSMMUNode->BaseAddr;
        pSMMUInfo->Span = pSMMUNode->Span;
        pSMMUInfo->Model = pSMMUNode->Model;
        pSMMUInfo->Flags = pSMMUNode->Flags;
        pSMMUInfo->NumCtxtIrpts = pSMMUNode->NumCtxtIrpts;
        pSMMUInfo->NumPMUIrpts = pSMMUNode->NumPMUIrpts;
        pSMMUInfo->NSgIrpt.Vector = pSMMUNode->NSgIrpt;
        pSMMUInfo->NSgIrpt.Flags = pSMMUNode->NSgIrptFlags;
        pSMMUInfo->NSgCfgIrpt.Vector = pSMMUNode->NSgCfgIrpt;
        pSMMUInfo->NSgCfgIrpt.Flags = pSMMUNode->NSgCfgIrptFlags;
        ret = E_SUCCESS;
    }

    return ret;
}

IxErrnoType IORT_GetInterruptInfo(
    uint64 BaseAddr,
    IORT_SMMUv1v2IntType IntType,
    uint32 IntIdx,
    uint32 NumInts, 
    IORT_InterruptInfo *pIntInfo 
)
{
    IxErrnoType ret = E_NOT_AVAILABLE;
    IORTSMMUv1v2Node* pSMMUNode = IORT_GetSMMUv1v2Node(BaseAddr);

    if(pSMMUNode) {
        IORTInterrupt* pInterrupt = NULL;
        if ((IORT_SMMUv1v2_INTTYPE_CONTEXT == IntType) &&
            (IntIdx < pSMMUNode->NumCtxtIrpts)){
            pInterrupt = (IORTInterrupt*)((uint8*)pSMMUNode + \
                                          pSMMUNode->CtxtIrptArrayOffset + \
                                          (sizeof(IORTInterrupt)*IntIdx));
        } else if ((IORT_SMMUv1v2_INTTYPE_PMU == IntType) && 
                   (IntIdx < pSMMUNode->NumPMUIrpts)) {
            pInterrupt = (IORTInterrupt*)((uint8*)pSMMUNode + \
                                          pSMMUNode->PMUIrptArrayOffset + \
                                          (sizeof(IORTInterrupt)*IntIdx));
        } else {
            ret = E_OUT_OF_RANGE;
        }
        
        if(pInterrupt){
            while(NumInts) {
                pIntInfo->Vector = pInterrupt->GSIV;
                pIntInfo->Flags = pInterrupt->Flags;
                NumInts--;
                pIntInfo++;
                pInterrupt++;
            }
            ret = E_SUCCESS;
        }
    }
    return ret;
}

IxErrnoType IORT_GetSMMUv1v2Instances(
    uint32* pNumInstances, 
    uint64* pBaseAddrs
)
{
    IxErrnoType ret = E_NOT_AVAILABLE;
    IORTData* pIORTData = IORT_GetData();

    if (pIORTData) {
        IORTGenericNode* pIORTNode = pIORTData->pIORTNodes;
        uint32 NumIORTNodes = pIORTData->NumIORTNodes;
        /* Save local copy of the number of BaseAddrs pointed by pBaseAddrs. */
        uint32 NumAddrs = *pNumInstances;
        /* Reset the Number if instances to 0. */
        *pNumInstances = 0;
        ret = E_SUCCESS;
        while(NumIORTNodes) {
            if(IORTNODE_TYPE_SMMUv1v2 == pIORTNode->Type) {
                *pNumInstances = *pNumInstances + 1;
                if(NumAddrs && pBaseAddrs) {
                    *pBaseAddrs = ((IORTSMMUv1v2Node*)pIORTNode)->BaseAddr;
                    pBaseAddrs++;
                    NumAddrs--;
                }
            }
            pIORTNode = (IORTGenericNode*)((uint8*)pIORTNode + pIORTNode->Length);
            NumIORTNodes--;
        }
    }
    return ret;
}
