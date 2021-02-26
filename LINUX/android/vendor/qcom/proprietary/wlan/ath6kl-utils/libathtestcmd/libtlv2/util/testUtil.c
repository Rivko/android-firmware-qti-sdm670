/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#include "commonInc.h"
#include "testUtil.h"

#if 0
//commented for ld failure in myftm because of double declartion in tlv2 and tlvutil
A_BOOL verifyChecksum(A_UINT16* stream, A_UINT32 len)
{
    A_UINT16 i,sum=0;
    for (i=0;i<len;i++) { sum ^= *stream++; }
    if (0xffff != sum) return(FALSE);
    return(TRUE);
}


A_UINT16 computeChecksumOnly(A_UINT16 *pHalf, A_UINT16 length)
{
    A_UINT16 sum = 0, i;
    for (i = 0; i < length; i++) { sum ^= *pHalf++; }
    return(sum);
}
//commented ends
#endif

A_BOOL halphyToolsMem(HALPHY_TOOLS_MEM *pMem, HALPHY_TOOLS_MEM_ENUM act, A_UINT32 size)
{
    if (HALPHY_TOOLS_MEM_ALLOC == act)
    {
        if (TRUE == pMem->allocated)
        {
            if (size <= pMem->size)
            {
                return TRUE;        //already allocated
            }
#if defined(TEST_FRAMEWORK) || defined(TLV2P0_EXPORTS)
            A_FREE(pMem->pBuf);
#endif
        }
        pMem->pBuf = (A_UINT8 *)A_ALLOCRAM(size);
        if (NULL == pMem->pBuf)
        {
            A_PRINTF_ALWAYS("Could not allocate memory of %d bytes\n", size);
            pMem->size = 0;
            pMem->allocated = FALSE;
            return FALSE;
        }
        pMem->allocated = TRUE;
        pMem->size = size;
    }
#if defined(TEST_FRAMEWORK) || defined(TLV2P0_EXPORTS)
    else if (HALPHY_TOOLS_MEM_FREE == act)
    {
        if (TRUE == pMem->allocated)
        {
            A_FREE(pMem->pBuf);
            pMem->pBuf = NULL;
            pMem->size = 0;
            pMem->allocated = FALSE;
        }
    }
#endif //TEST_FRAMEWORK
    return TRUE;
}
