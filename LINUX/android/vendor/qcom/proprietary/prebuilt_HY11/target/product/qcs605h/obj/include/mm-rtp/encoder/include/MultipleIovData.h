/*=======================================================================
                              MultipleIovData.h
DESCRIPTION

This module implements the iMultipleIovData interface.
It is used to store and transport multiple "struct iovec" arrays.

Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */

/*========================================================================
 *                             Include Files
 *==========================================================================*/


#ifndef __MULTIPLE_IOV_DATA_H__
#define __MULTIPLE_IOV_DATA_H__

#include "sys/uio.h"
#include "DataSourcePort.h"

/**
 * IovData stores a single array of struct iovec.
 */
struct IovData
{
    struct iovec *m_Iovec;
    uint32       m_IovecLen;
    uint32       m_TotalDataSize; // excluding idx 0 which is reserved for RTP header.

    // 12 for RTP header and 2 in case TCP plus some padding
    mutable uint8 m_RtpPktHeader[12 + 2 + 16];
};

class MultipleIovData : public video::iMultipleIovData
{
public:
    MultipleIovData(bool& bOk, uint32 nMaxCount, uint32 nMaxIovEntriesPerIov);
    virtual ~MultipleIovData();

    /**
     * Reset object for reuse.
     */
    virtual void ResetMultipleIovData();

    /**
     * Add a struct iovec array.
     *  pIov: Pointer to an array of struct iovec entries
     *  nNumIovs: Number of struct iovec entries in pIov
     */
    virtual bool AddIovec(struct iovec *pIov, uint32 nNumIovs);

    /**
     * Return the number of arrays of struct iovec entries.
     */
    virtual uint32 GetSize() const;

    /**
     * Returns true if the total number of pointers to struct iovec
     * arrays has hit the max limit.
     */
    virtual bool IsFull() const;

    /**
     * Log information for debugging.
     */
    virtual void PrintMultipleIovData() const;

    /**
     * Return a reference to IovData at index 'idx'. Caller to
     * ensure idx is within bounds.
     */
    const IovData& GetIovDataAt(uint32 idx) const;

    /**
     * Populate the first iov element which is the RTP header in the
     * implementation.
     */
    bool AddFirstIov(uint32 nIovIdx, char *pData, int nDataLen);

private:
    MultipleIovData();
    MultipleIovData(const MultipleIovData &rhs);
    MultipleIovData& operator=(const MultipleIovData& rhs);

    IovData* m_IovDataArray; // MAX_RTP_PKTS_LIMIT
    uint32  m_IovDataArraySize;

    const uint32 m_IovDataArrayMaxSize;
    const uint32 m_MaxIovCountPerArrayElement;
    const uint32 m_MaxIovCountPerArrayElementAllocated;

    friend class CRTPPacketTransmit;
};

class MultipleIovDataAllocator : public video::iMultipleIovDataAllocator
{
public:
    MultipleIovDataAllocator() { }
    virtual ~MultipleIovDataAllocator() { }

    /**
     * Allocate an instance of iMultipleIovData.
     *
     * maxCount: Number of 'struct iovec*' arrays to hold
     * maxIovEntriesPerIov: Number of 'struct iovec *' elements in a
     *   single 'struct iovec *' array.
     */
    virtual video::iMultipleIovData *AllocateMultipleIovData(uint32 maxCount, uint32 maxIovEntriesPerIov);

    /**
     * Release an instance of iMultipleIovData.
     */
    virtual void ReleaseMultpleIovData(video::iMultipleIovData *ptr);
};

#endif
