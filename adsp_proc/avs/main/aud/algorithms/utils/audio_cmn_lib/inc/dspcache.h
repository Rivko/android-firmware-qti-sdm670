/***************************************************************************
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/

#ifndef __DSP_CACHE_H_
#define __DSP_CACHE_H_

#ifdef __cplusplus
extern "C"
{
#endif
/*! \file dspcache.h
    \cache operation utilities.
 */

//---------------------------------------------------------------------------
/// @brief
///   Request a linear L2FETCH operation
///
/// @detailed
///   Issues L2FETCH for up to 64KB. Divides bytes by 256, uses 256 for stride and 255 for width
///   (effectively resulting in width = 256), and bytes/256 + 1 for height. The start address
///   must be a validly mapped virtual address, else an exception will be raised. However, it can
///   be of any alignment, and the entire buffer size requested can span across page boundaries
///   and even into unmapped space without exception. The L2 fetch will simply abort itself whenever
///   it reaches a page boundary.
///
/// @param addr
///   Input L2FETCH start address.
///
/// @param bytes
///   number of desired bytes to be fetched.
///
/// @return
///   none
//---------------------------------------------------------------------------
void dspcache_linear_l2fetch(unsigned char* addr, unsigned int bytes);

//---------------------------------------------------------------------------
/// @brief
///   Request a box L2FETCH operation
///
/// @detailed
///   Issues L2FETCH for up to 255 bytes high, and up to 255 bytes wide, box type L2 prefetch.
///   Width or height above 255 will be clamped. Stride (difference between starting pixels
///   in consecutive rows) must be < 2^16.
///   The starting address must be a validly mapped virtual address, else an exception will be raised.
///   However, it can be of any alignment, and the entire buffer size requested can span across page
///   boundaries and even into unmapped space without exception. The L2 fetch will simply abort itself
///   whenever it reaches a page boundary.
///
/// @param addr
///   Input L2FETCH box start address.
///
/// @param width
///   width of desired box to be fetched in bytes.
///
/// @param height
///   height of desired box to be fetched in bytes.
///
/// @param stride
///   difference between starting pixels in consecutive rows, in bytes.
///
/// @return
///   none
//---------------------------------------------------------------------------
void dspcache_box_l2fetch(unsigned char* addr, unsigned int width,
        unsigned int height, unsigned int stride);

//---------------------------------------------------------------------------
/// @brief
///   Request a linear DCZEROA operation
///
/// @detailed
///   Finds the whole cache lines fully contained within the requested buffer
///   and issues a DCZEROA on those cache lines. They will have cache lines
///   allocated for them (without fetching from memory) and cleared to all-zero.
///   Input address/len need not be 32-byte aligned; the implementation will find the
///   best-fit 32-byte boundaries within the buffer. Hence the unaligned portions
///   will NOT have cache lines allocated and set to 0, but there will be no
///   exceptions.
///
/// @param addr
///   Input DCZEROA start address.
///
/// @param bytes
///   number of desired bytes to be cleared.
///
/// @return
///   none
//---------------------------------------------------------------------------
void dspcache_linear_dczeroa(unsigned char* addr, unsigned int bytes);

//---------------------------------------------------------------------------
/// @brief
///   Request a box DCZEROA operation
///
/// @detailed
///   Finds the whole cache lines fully contained within the requested box
///   and issues a DCZEROA on those cache lines. They will have cache lines
///   allocated for them (without fetching from memory) and cleared to all-zero.
///   Input address/len need not be 32-byte aligned; the implementation will find the
///   best-fit 32-byte boundaries within the buffer. Hence the unaligned portions
///   will NOT have cache lines allocated and set to 0, but there will be no
///   exceptions.
///
/// @param addr
///   Input DCZEROA start address.
///
/// @param width
///   width of desired box to be fetched in bytes.
///
/// @param height
///   height of desired box to be fetched in bytes.
///
/// @param stride
///   difference between starting pixels in consecutive rows, in bytes.
///
/// @return
///   none
//---------------------------------------------------------------------------
void dspcache_box_dczeroa(unsigned char* addr, unsigned int width,
        unsigned int height, unsigned int stride);
#ifdef __cplusplus
}
#endif

#endif /*__DSPCACHE_H */

