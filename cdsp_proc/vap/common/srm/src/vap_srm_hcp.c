/**
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "vap_srm_hcp.h"
#include "vap_srm.h"
#include "hcp_hal.h"
#include "qurt_srm_driver.h"

/*===========================================================================
 Constants, macros
===========================================================================*/
#define VMA_SEC_SID_BASE (TURING_BASE_PHYS+0x002e0000+VMA_SEC_PID0_CFG)

#define GET_ADDR_H(_addr) (((_addr) >> 32) & 0xFFFFFFFF)
#define GET_ADDR_L(_addr) ((_addr) & 0xFFFFFFFF)
#define COMBINE_ADDR(_h32, _l32) ((((unsigned long long)(_l32)) & 0xFFFFFFFF) | ((((unsigned long long)(_h32)) << 32) & 0xFFFFFFFF00000000))

/*===========================================================================
 Type definitions
===========================================================================*/


/*===========================================================================
 Local Variables
===========================================================================*/
/* HCP resources
*/
enum{
    HCP_MAP_ENTRY_SEC_SID,
////////////////////////////
    HCP_MAP_ENTRY_NUM
};

static t_StMapNode stHcpMapEntries[HCP_MAP_ENTRY_NUM] =
{
{0,0xFFFFFFFF, VMA_SEC_SID_BASE>>12, 0, 0, 4096}//Secure SID registers
};


/*===========================================================================
 Local function prototypes
===========================================================================*/


/*===========================================================================
 Function definitions
===========================================================================*/
int vap_srmHcpResourcesInit(int client_handle)
{
    int status = SRM_ERR_SUCCESS;

    stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].start_vaddr_l = 0;
    stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].start_vaddr_h = 0;
    stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].pageno_virt = 0xFFFFFFFF;
    stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].pageno_phys = (unsigned int)(VMA_SEC_SID_BASE >> 12);
    stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].u32Size = 4096;

    if(0 > qurt_srm_mapping_create(client_handle,
            &stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].pageno_virt,
            stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].pageno_phys,
            stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].u32Size>>12,
            QURT_MEM_CACHE_DEVICE,
            QURT_PERM_READ | QURT_PERM_WRITE))
    {
        stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].pageno_virt = 0;
        status = SRM_ERR_FAIL;
    }
    else
    {
        stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].start_vaddr_l =
                (unsigned long)( (((unsigned long long)stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].pageno_virt)<<12) & (unsigned long long)0x00000000FFFFF000 );
        stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].start_vaddr_h =
                (unsigned long)( (((unsigned long long)stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID].pageno_virt)>>20) & (unsigned long long)0x00000000FFFFFFFF );
    }

    return status;
}

int vap_srmHandleHcpSidRegGet(
        unsigned int u32Pid,
        unsigned int *pu32Val)
{
    int status = SRM_ERR_SUCCESS;

    t_StMapNode *pHcpSidRegMap = &stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID];

    if(!pu32Val) return SRM_ERR_NULLPTR;
    if(u32Pid > 7) return SRM_ERR_INVALID_PARAM;

    if(pHcpSidRegMap->pageno_virt)
    {
        *pu32Val = READ_REG(COMBINE_ADDR(pHcpSidRegMap->start_vaddr_h, pHcpSidRegMap->start_vaddr_l+sizeof(unsigned int)*u32Pid));
    }

    return status;
}

//The field MASK and SHIFT are the same for all VMA_SEC_PIDx_CFG
#define GET_PIX_INT_SID(val) ((val & VMA_SEC_PID0_CFG__PID0_PIX_INT_SID___M) >> VMA_SEC_PID0_CFG__PID0_PIX_INT_SID___S)
#define GET_PIX_IN_SID(val) ((val & VMA_SEC_PID0_CFG__PID0_PIX_IN_SID___M) >> VMA_SEC_PID0_CFG__PID0_PIX_IN_SID___S)
#define GET_PIX_OUT_SID(val) ((val & VMA_SEC_PID0_CFG__PID0_PIX_OUT_SID___M) >> VMA_SEC_PID0_CFG__PID0_PIX_OUT_SID___S)
#define GET_CPZ_VALID(val) ((val & VMA_SEC_PID0_CFG__PID0_CPZ_VALID___M) >> VMA_SEC_PID0_CFG__PID0_CPZ_VALID___S)

int vap_srmHandleHcpSidRegSet(
        unsigned int u32Pid,
        unsigned int u32Val,
        unsigned int *pu32Err,
        unsigned int *pu32ValBefore,
        unsigned int *pu32ValAfter)
{
    int status = SRM_ERR_SUCCESS;
    unsigned int nPixIntSid = -1;
    unsigned int nPixInSid = -1;
    unsigned int nPixOutSid = -1;
    unsigned int nCpZValid = 1;

    t_StMapNode *pHcpSidRegMap = &stHcpMapEntries[HCP_MAP_ENTRY_SEC_SID];

    if(u32Pid > 7)//Up to VMA_SEC_PID7_CFG
    {
        *pu32Err |= (1<<ERR_FLAGSHIFT_INVALID_PID);
        return SRM_ERR_INVALID_PARAM;
    }

    if(!pHcpSidRegMap->pageno_virt)
    {
        *pu32Err |= (1<<ERR_FLAGSHIFT_INVALID_VA);
        return SRM_ERR_FAIL;
    }

    //Validate!!
    nPixIntSid = GET_PIX_INT_SID(u32Val);
    nPixInSid = GET_PIX_IN_SID(u32Val);
    nPixOutSid = GET_PIX_OUT_SID(u32Val);
    nCpZValid = GET_CPZ_VALID(u32Val);
    if(nCpZValid &&
    (!nPixIntSid || !nPixInSid || !nPixOutSid || (nPixIntSid != nPixInSid) || (nPixIntSid != nPixOutSid)))
    {
        *pu32Err |= (1<<ERR_FLAGSHIFT_HCP_SID_VERIFICATION_FAIL);
        return SRM_ERR_MEM_FAIL;
    }

#ifdef EN_STATS
    *pu32ValBefore =
            READ_REG(COMBINE_ADDR(pHcpSidRegMap->start_vaddr_h, pHcpSidRegMap->start_vaddr_l+sizeof(unsigned int)*u32Pid));
#endif//EN_STATS
    //Write
    WRITE_REG(COMBINE_ADDR(pHcpSidRegMap->start_vaddr_h, pHcpSidRegMap->start_vaddr_l+sizeof(unsigned int)*u32Pid), u32Val);
#ifdef EN_STATS
    *pu32ValAfter =
            READ_REG(COMBINE_ADDR(pHcpSidRegMap->start_vaddr_h, pHcpSidRegMap->start_vaddr_l+sizeof(unsigned int)*u32Pid));
#endif//EN_STATS

    return status;
}

t_StMapNode* vap_srm_hcpGetMapEntries()
{
    t_StMapNode *p = 0;
#ifdef EN_STATS
    p = &stHcpMapEntries[0];
#endif//EN_STATS
    return p;
}
