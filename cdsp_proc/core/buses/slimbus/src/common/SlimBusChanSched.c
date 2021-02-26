/*
===========================================================================

FILE:         SlimBusChanSched.c

DESCRIPTION:  This file implements the SLIMbus core driver 
              channel scheduling functionality.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/SlimBusChanSched.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/15/16   MJS     Enhanced high-concurrency scheduling and refactoring.
10/03/16   MJS     Support for full secondary line bandwidth scheduling.
12/07/15   MJS     Multi-data line channel scheduling optimization.
11/24/15   MJS     Support for high-bandwidth mixed 12k/4k use case.
04/17/15   NSD     Add log to notify if one or more channels are not scheduled
                   properly
09/09/14   mq      Multi-dataline support.
04/23/14   MJS     Support for ultrasound high-concurrency use case.
04/30/13   MJS     Support for spcifying minimum clock gear for scheduling.
07/24/12   MJS     Make sure open slots are not less than zero after scheduling.
04/09/12   MJS     Fix SAL warnings.
04/04/12   MJS     Fixes for level 4 warnings.
03/28/12   MJS     Compatibility with WPP logging.
01/07/12   MJS     Fix klocwork issues.
12/11/11   MJS     Remove references to DAL.
11/23/11   MJS     Fix issue where scheduling was improperly succeeding
                   when both slot buckets had exactly zero open slots.
11/15/11   MJS     Fix klocwork issues.
01/21/11   MJS     Initial revision.

===========================================================================
             Copyright (c) 2011-2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include <stringl/stringl.h>
#include "SlimBusTarget.h"
#include SLIMBUS_OS_H
#include "SlimBus.h"
#include "SlimBusConsts.h"
#include "SlimBusLists.h"
#include SB_TRACE_HEADER(SlimBusChanSched)

#define MAX_BUCKETS                    8
#define NUM_DEFAULT_BUCKETS            2
#define COEFF_3_TO_1_FACTOR            3
#define NUM_SECONDARY_BUCKETS          (NUM_DEFAULT_BUCKETS*COEFF_3_TO_1_FACTOR)
#define TERMINAL_SLOT_INTERVAL_COEFF_3 32
#define TERMINAL_SLOT_INTERVAL_COEFF_1 24

/* Current scheduling parameters */ 
typedef struct
{
  uint32 uCurInterval;
  uint32 uSlotsPerBucket;
  uint32 uExpShft;
  int32 iCurExp;
  int32 iFinalExp;
  uint32 uTerminalInterval;
}
SlimBusCurSchedType;

/* Slot record information for a slot bucket array */
typedef struct
{
  uint32 uNumBuckets;
  uint32 uNumCurParams;
  int32 iNextExpDiff;
  int32 iOpenSlots[MAX_BUCKETS];
}
SlimBusSlotRecordType;

/**
 * @brief Gets the Subframe Mode (SM) encoding based on the 
 *        width of the shared message channel and the length of
 *        the subframe in slots.  Also computes the number of
 *        slots per superframe dedicated to the message channel.
 *
 * This function gets the Subframe Mode (SM) based on the width 
 * of the shared message channel and the length of the subframe 
 * in slots.  Also computes the number of slots per superframe 
 * dedicated to the message channel. 
 * 
 * @param[in] uCtrlWidth  Width of the message channel in slots
 * @param[in] uSubFrmLen  Length of the subframe in slots
 * @param[in] uMsgSlots  Number of slots per superframe 
 *       dedicated to messaging
 * 
 * @return The encoded Subframe Mode 
 */
static uint32 GetSubFrameCoding(uint32 *uCtrlWidth, uint32 *uSubFrmLen, uint32 *uMsgSlots)
{
  uint32 uCode = 0;
  
  if ( *uCtrlWidth == *uSubFrmLen )
  {
    *uCtrlWidth = 8;
    *uSubFrmLen = 8;
    *uMsgSlots = 
      SB_SLOTS_PER_SUPERFRAME - 
      SB_FRAME_SLOTS_PER_SUPERFRAME -
      SB_GUIDE_SLOTS_PER_SUPERFRAME;
    return 0;
  }
  
  if ( *uSubFrmLen == 6 )
  {
    uCode = 0;
    *uMsgSlots = 256;
  }
  else if ( *uSubFrmLen == 8 )
  {
    uCode = 1;
    *uMsgSlots = 192;
  }
  else if ( *uSubFrmLen == 24 )
  {
    uCode = 2;
    *uMsgSlots = 64;
  }
  else /* 32 */
  {
    uCode = 3;
    *uMsgSlots = 48;
  }
  
  if (*uCtrlWidth < 8)
  {
    if ( *uCtrlWidth >= 6 )
    {
      *uCtrlWidth = 6;
      uCode |= 0x14;
    }
    else /* <= 5 */
    {
      if ( *uCtrlWidth == 5)
      {
        *uCtrlWidth = 4;
      }
      uCode |= (*uCtrlWidth << 2);
    }
  }
  else 
  {
    uCode -= 2;
    
    if ( *uCtrlWidth >= 24 )
    {
      *uCtrlWidth = 24;
      uCode |= 0x1e;
    }
    else if ( *uCtrlWidth >= 16 )
    {
      *uCtrlWidth = 16;
      uCode |= 0x1c;
    }
    else if ( *uCtrlWidth >= 12 )
    {
      *uCtrlWidth = 12;
      uCode |= 0x1a;
    }
    else /* >= 8 */
    {
      *uCtrlWidth = 8;
      uCode |= 0x18;
    }
  }
  
  *uMsgSlots  =
    (*uMsgSlots * *uCtrlWidth) - 
    SB_FRAME_SLOTS_PER_SUPERFRAME -
    SB_GUIDE_SLOTS_PER_SUPERFRAME;
  return uCode;
}

/**
 * @brief Is the current channel applicable to be scheduled 
 *  
 * This function checks whether the current channel is 
 * applicable to be scheduled 
 * 
 * @param[in] pParam  Pointer to the channel parameter structure 
 *       for the channel being scheduled
 * @param[in] uDataLine  The current data line being scheduled
 * 
 * @return TRUE if the channel is applicable to be scheduled or 
 *         FALSE otherwise
 */
SLIMBUS_INLINE SBBOOL ChannelIsApplicable
(
  const SlimBusChannelParamType *pParam,
  uint32 uDataLine
)
{
  return ( (pParam->eStatus != SB_CHANNEL_PENDING_REMOVAL) &&
           (pParam->uNewDataLine == uDataLine) );
}

/**
 * @brief Shift the segments offsets for all the allocated data 
 *        channels by a fixed amount
 *
 * This function shifts the segment offset for all the allocated 
 * data channels by a fixed amount. 
 * 
 * @param[in] pRoot  Pointer to the head of the linked list of 
 *       data channel parameter structures
 * @param[in] uShft  Shift amount in slots
 */
static void ShiftSegmentOffsets(SlimBusLinkNodeType *pRoot, uint32 uDataLine, uint32 uShft)
{
  SlimBusChannelParamType *pParam = NULL;

  while ( NULL != (pParam = NextParamNode(pRoot, pParam)) )
  {
    /* No need to shift channels that are pending removal */
    if ( !ChannelIsApplicable(pParam, uDataLine) )
    {
      continue;
    }
    
    pParam->uNewOffset += uShft;
    /* Segment offset must be <= the segment interval */
    if ( pParam->uNewOffset >= pParam->uNewInterval )
    {
      pParam->uNewOffset -= pParam->uNewInterval;
    }
  }
}

/**
* @brief Compute the bandwidth for a given subframe mode
*  
* This function computes the bandwidth for a given subframe mode
* 
* @param[in] iCtrlWidth  The control channel width
* @param uSubFrmLen  The length of the subframe
* 
* @return Computed bandwidth
*/
static int32 GetSubFrameBandwidth(int32 iCtrlWidth, uint32 uSubFrmLen)
{
  if (32 == uSubFrmLen)
  {
    return iCtrlWidth * 3;
  }
  else if (24 == uSubFrmLen)
  {
    return iCtrlWidth * 4;
  }
  else if (8 == uSubFrmLen)
  {
    return iCtrlWidth * 12;
  }
  else if (6 == uSubFrmLen)
  {
    return iCtrlWidth * 16;
  }
  return 0;
}

/**
* @brief Assign offset to channel parameter structure
*
* This function assigns the offset to a channel parameter
* structure based on the bucket index and updates the bucket
* open slots
* 
* @param[in,out] pParam  Pointer to channel parameter structure
* @param[in,out] pSR  Pointer to slot record info
* @param[in] iBucketIndex  Bucket index
* @param[in] pCS  Pointer to current scheduling parameters
* 
* @return SB_SUCCESS if successful, error otherwise
*/
static SBResult AssignParamOffsetAndUpdateBuckets
(
  SlimBusChannelParamType *pParam,
  SlimBusSlotRecordType *pSR,
  int32 iBucketIndex,
  const SlimBusCurSchedType *pCS    
)
{
  pSR->iOpenSlots[iBucketIndex] -= pParam->ucSegLen;
  pParam->uNewOffset = (pCS->uSlotsPerBucket*iBucketIndex) + pSR->iOpenSlots[iBucketIndex];
  if ( pSR->iOpenSlots[iBucketIndex] < 0 )
  {
    if ( iBucketIndex > 0 && pSR->iOpenSlots[iBucketIndex - 1] == (int32)pCS->uSlotsPerBucket ) 
    {
      pSR->iOpenSlots[iBucketIndex-1] += pSR->iOpenSlots[iBucketIndex];
      pSR->iOpenSlots[iBucketIndex] = 0;
      if ( pSR->iOpenSlots[iBucketIndex-1] < 0 )
      {
        return SB_ERROR;
      }
    }
    else
    {
      return SB_ERROR;
    }
  }
  pParam->uNewInterval = pCS->uCurInterval;
  return SB_SUCCESS;
}

/**
* @brief Find the bucket index with the max open slots
*
* This function finds the index of the bucket with the maximum
* open slots available.
* 
* @param[in] pSR  Pointer to slot record info
* @param[in] aSearchOrder  Pointer to array specifying the
*       search order
* 
* @return The index of the bucket with max avail open slots 
*/
static int32 GetMaxOpenSlotsIndex
(
  const SlimBusSlotRecordType *pSR,
  const int32 *aSearchOrder
)
{
  int32 i = 0, max_i = 0, maxOpenSlots = 0;
  for (i = 0; i < (int32) pSR->uNumBuckets; i++)
  {
    int32 slot = aSearchOrder[i];
    int32 curOpenSlots = pSR->iOpenSlots[slot];
    if ( curOpenSlots > maxOpenSlots )
    {
      maxOpenSlots = curOpenSlots;
      max_i = slot;
    }
  }
  
  return max_i;
}

/**
* @brief Count number of channels to schedule this round
*
* This function counts the number of channels that will be
* scheduled during the current segment interval, as well how far
* away the next group of channels are.
* 
* @param[in] ParamList  Pointer to channel parameter list
* @param[in] pParam  Pointer to current channel parameter struct
* @param[in] uDataLine  The current data line being scheduled
* @param[in] pCS  Pointer to current scheduling parameters
* @param[in,out] pSR  Pointer to slot record info
*/
static void CountNumCurrentParamsAndExpDiff
(
  const SlimBusLinkNodeType *ParamList,
  const SlimBusChannelParamType *pParam,
  uint32 uDataLine,
  const SlimBusCurSchedType *pCS,
  SlimBusSlotRecordType *pSR
)
{
  pSR->uNumCurParams = 0;
  pSR->iNextExpDiff = -1;
  while (NULL != pParam &&
         pCS->iCurExp == (int32)(pParam->ucRootExp + pCS->uExpShft))
  {
    if ( ChannelIsApplicable(pParam, uDataLine) )
    {
      (pSR->uNumCurParams)++;
    }
    pParam = NextParamNode(ParamList, pParam);
  }
  while (NULL != pParam)
  {
    if ( ChannelIsApplicable(pParam, uDataLine) )
    {
      pSR->iNextExpDiff = (int32)pParam->ucRootExp + pCS->uExpShft - pCS->iCurExp;
      break;
    }
    pParam = NextParamNode(ParamList, pParam);
  }
}

/**
* @brief Reduce the number of buckets by half by merging
*
* Reduce the number of buckets in half by merging buckets
* together.  When buckets are merged together, the number of
* open slots is equal to the minimum of the buckets being
* merged.
* 
* @param[in,out] pSR  Pointer to slot record info
*/
static void ReduceNumberOfBucketsByHalf
(
  SlimBusSlotRecordType *pSR
)
{
  int32 i;
  
  if (NULL == pSR) { return; }
  
  pSR->uNumBuckets /= 2;
  for (i = 0; i < (int32)pSR->uNumBuckets; i++)
  {
    if (pSR->iOpenSlots[i] > pSR->iOpenSlots[i + pSR->uNumBuckets])
    {
      pSR->iOpenSlots[i] = pSR->iOpenSlots[i + pSR->uNumBuckets];
    }
  }
  for (; i < (int32)pSR->uNumBuckets*2; i++)
  {
    pSR->iOpenSlots[i] = 0;
  }
}

/**
* @brief Combine the secondary buckets to primary buckets
*
* This function combines the secondary buckets to get the
* primary buckets by merging buckets together.  When buckets are
* merged, the number of open slots is equal to the minimum of
* the buckets being merged.
* 
* @param[in,out] pSRp  Pointer to the primary slot record info
* @param[in] pSRs  Pointer to the secondary slot record info
*/
static void CombineSecondaryBucketsToPrimary
(
  SlimBusSlotRecordType *pSRp,
  const SlimBusSlotRecordType *pSRs
)
{
  int32 p, s;
  
  for (s = 0; s < (int32)pSRs->uNumBuckets; )
  {
    for (p = 0; p < (int32)pSRp->uNumBuckets && p <= NUM_DEFAULT_BUCKETS; p++, s++)
    {
      if (pSRp->iOpenSlots[p] > pSRs->iOpenSlots[s])
      {
        pSRp->iOpenSlots[p] = pSRs->iOpenSlots[s];
      }
    }
  }
}

/**
* @brief Expand primary buckets into secondary buckets
*
* This function expands the primary buckets into the secondary
* buckets.  Buckets are expanded by duplicating the primary
* buckets.
* 
* @param[in] pSRp  Pointer to the primary slot record info
* @param[in,out] pSRs  Pointer to the secondary slot record info
*/
static void ExpandPrimaryBucketsToSecondary
(
  const SlimBusSlotRecordType *pSRp,
  SlimBusSlotRecordType *pSRs
)
{
  int p, s;
  
  pSRs->uNumBuckets = pSRp->uNumBuckets*COEFF_3_TO_1_FACTOR; 
  for (s = 0; s < (int32)pSRs->uNumBuckets && s < NUM_SECONDARY_BUCKETS; )
  {
    for (p = 0; p < (int32)pSRp->uNumBuckets; s++, p++)
    {
      pSRs->iOpenSlots[s] = pSRp->iOpenSlots[p];
    }
  }
  for ( ; s < NUM_SECONDARY_BUCKETS; s++) { pSRs->iOpenSlots[s] = 0; }
}

/**
* @brief Double the number of buckets by dividing them in half
*
* This function doubles the number of buckets by dividing the
* existing buckets in half.
* 
* @param[in,out] pSR  Pointer to slot record info
* @param[in,out] pCS  Pointer to current scheduling parameters
* 
* @return SB_SUCCESS on success, error code otherwise
*/
static SBResult DoubleNumberOfBucketsByDividingInHalf
(
  SlimBusSlotRecordType *pSR,
  SlimBusCurSchedType *pCS
)
{
  SBResult result = SB_SUCCESS;
  int32 i;

  pCS->uSlotsPerBucket /= 2;

  if (NULL == pSR) { return result; }
  
  for (i = pSR->uNumBuckets - 1; i >= 0; i--)
  {
    pSR->iOpenSlots[i*2+1] = pSR->iOpenSlots[i];
    pSR->iOpenSlots[i*2+1] -= pCS->uSlotsPerBucket;
    pSR->iOpenSlots[i*2] = pCS->uSlotsPerBucket;
    if (pSR->iOpenSlots[i*2+1] < 0)
    {
      pSR->iOpenSlots[i*2] += pSR->iOpenSlots[i * 2 + 1];
      pSR->iOpenSlots[i*2+1] = 0;
    }
    if (pSR->iOpenSlots[i*2] < 0)
    {
      result = SB_ERROR;
    }
  }
  pSR->uNumBuckets *= 2;

  return result;
}

/**
* @brief Skip channels that are not applicable
*
* This function skips channels that are not applicable to the
* current data line scheduling
* 
* @param[in] pParamList  Pointer to the parameter list
* @param[in,out] ppParam  Pointer to the pointer to the channel
*       paramter struct
* @param[in] uDataLine  The current data line being scheduled
*/
static void SkipNotApplicableChannels
(
  const SlimBusLinkNodeType *pParamList,
  SlimBusChannelParamType **ppParam,
  uint32 uDataLine
)
{
  while (NULL != *ppParam &&
         !ChannelIsApplicable(*ppParam, uDataLine))
  {
    *ppParam = NextParamNode(pParamList, *ppParam);
  }
}

/**
* @brief Compute the exponent of the last group of channels to
*        be scheduled
*
* This function computes the root exponent of the last group of
* channels to be scheduled.
* 
* @param[in] pParamList  Pointer to the parameter list
* @param[in] uDataLine  The data line being scheduled
* 
* @return Exponent of final channels 
*/
static uint32 ComputeFinalExp(const SlimBusLinkNodeType *pParamList, uint32 uDataLine)
{
  SlimBusChannelParamType *pParam = NULL;
  while ( NULL != (pParam = PrevParamNode(pParamList, pParam)) &&
          !ChannelIsApplicable(pParam, uDataLine) )
  {
    /* do nothing */
  }
  if ( NULL != pParam )
  {
    return pParam->ucRootExp;
  }

  return 0;
}

/**
* @brief Get the primary slot record search order
*
* This function gets the primary slot record search order
* 
* @param[in] pSR  The slot record info
* 
* @return Pointer to the bucket search order array
*/
static const int32* GetPriSrchOrder(const SlimBusSlotRecordType *pSR)
{
  static const int32 iSrchOrder8[] = { 7, 3, 5, 1, 6, 2, 4, 0 };
  static const int32 iSrchOrder4[] = { 3, 1, 2, 0 };
  static const int32 iSrchOrder2[] = { 1, 0 };
  
  if      (pSR->uNumBuckets == 8) { return iSrchOrder8; }
  else if (pSR->uNumBuckets == 4) { return iSrchOrder4; }
  else                            { return iSrchOrder2; }
}

/**
* @brief Get the secondary slot record search order
*
* This function gets the secondary slot record search order
* 
* @param[in] pSR  The slot record info
* 
* @return Pointer to the bucket search order array
*/
static const int32* GetSecSrchOrder
(
  const SlimBusSlotRecordType *pSRp, /* primary */
  const SlimBusSlotRecordType *pSRs  /* secondary */
)
{
  static const int32 iSrchOrder1[] = { 5, 2, 4, 1, 3, 0 };
  static const int32 iSrchOrder2[] = { 5, 3, 1, 4, 2, 0 };
  static const int32 iSrchOrder3[] = { 2, 1, 0, 0, 0, 0 };
  
  if      (pSRs->uNumBuckets == 3)   { return iSrchOrder3; }
  else if (pSRp->uNumCurParams == 0) { return iSrchOrder1; }
  else                               { return iSrchOrder2; } 
}

/**
* @brief Compute the base-2 log
*
* This function computes the base-2 log of an unsigned int
* 
* @param[in] val  The input value
* 
* @return The base-2 log 
*/
static int32 log_2(uint32 val)
{
  int32 e = 0;
  while (val > 1)
  {
    e++;
    val /= 2;
  }
  return e;
}

/**
* @brief Get the max of two unsigned ints
*
* This function returns the max of two unsigned ints
* 
* @param[in] a  First value
* @param[in] b  Second value
* 
* @return Max value of the two inputs 
*/
static uint32 max_uint32(uint32 a, uint32 b)
{
  return (a > b) ? a : b;
}

/**
* @brief Get the min of two unsigned ints
*
* This function returns the min of two unsigned ints
* 
* @param[in] a  First value
* @param[in] b  Second value
* 
* @return Min value of the two inputs 
*/
static uint32 min_uint32(uint32 a, uint32 b)
{
  return (a < b) ? a : b;
}

/**
 * @brief Perform data channel scheduling
 *
 * This function schedules data channels within the frame 
 * structure, based on a given root frequency and clock gear. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] uClkGear  Clock gear to use for scheduling
 * @param[out] puCtrlWidth  Location to store the scheduled 
 *       message channel width
 * @param[out] puSubFrmLen  Location to store the scheduled 
 *       subframe mode
 * 
 * @return SB_SUCCESS on success, an error code if there was a 
 *         scheduling error
 */
static SBResult SlimBus_DoScheduleChannels
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 uDataLine,
  uint32 uClkGear,
  uint32 *puCtrlWidth,
  uint32 *puSubFrmLen
)
{
  SlimBusChannelParamType *pParamP = NULL, *pParamS = NULL;
  SlimBusLinkNodeType *ParamListP = &pDevCtxt->pSched->ChanParamList3;
  SlimBusLinkNodeType *ParamListS = &pDevCtxt->pSched->ChanParamList1;
  SlimBusCurSchedType CS;
  SlimBusSlotRecordType SRp = { NUM_DEFAULT_BUCKETS };
  SlimBusSlotRecordType SRs = { NUM_SECONDARY_BUCKETS };
  SBBOOL bSecondaryOpenSlotsValid = FALSE;
  int32 iBucketIndex;
  uint32 uMaxInterval, uMaxSubframeInterval;
  uint32 uSavedInterval = 0, uSavedOffset = 0, uSavedWidth = 0;

  /* Find the first data channel for the coefficient 1 and 3 lists that are 
     not pending removal, the first data channels will be used as a starting
     point for the segment rate exponent */
  while ( NULL != (pParamP = NextParamNode(ParamListP, pParamP)) &&
          !ChannelIsApplicable(pParamP, uDataLine) )
  {
    /* do nothing */
  }
  while ( NULL != (pParamS = NextParamNode(ParamListS, pParamS)) &&
          !ChannelIsApplicable(pParamS, uDataLine) )
  {
    /* do nothing */
  }

  /* Decide if we should usea coefficient 1 or 3 bias for channel scheduling.
     If there are no channels to schedule, just set the subframe mode to 100%
     message channel and return. */
  if ( NULL == pParamP && NULL == pParamS )
  {
    *puCtrlWidth = 8;
    *puSubFrmLen = 8;
    return SB_SUCCESS;
  }
  else if ( NULL == pParamP )
  {
    /* Default is to do coefficient 3 biased scheduling, unless there are 
       no coefficient 3 channels to schedule */
    pParamP = pParamS;
    pParamS = NULL;
    ParamListP = ParamListS;
    ParamListS = NULL;
    uMaxInterval = SB_MAX_INTERVAL_COEFF_1;
    uMaxSubframeInterval = TERMINAL_SLOT_INTERVAL_COEFF_1;
  }
  else
  {
    uMaxInterval = SB_MAX_INTERVAL_COEFF_3;
    uMaxSubframeInterval = TERMINAL_SLOT_INTERVAL_COEFF_3;
  }
  
  CS.uExpShft = SB_MAX_CLK_GEAR - uClkGear;
  /* -1 to account for incrementing iCurExp during first time through loop */
  CS.iCurExp = (int32)CS.uExpShft - 1;
  /* Note: use (iCurExp+1) because can't right shift by a negative value */
  CS.uCurInterval = (uMaxInterval*2)>>(CS.iCurExp+1);
  CS.uSlotsPerBucket = CS.uCurInterval/NUM_DEFAULT_BUCKETS;
  SRp.iOpenSlots[0] = SRp.iOpenSlots[1] = CS.uSlotsPerBucket;

  CS.iFinalExp = max_uint32(ComputeFinalExp(ParamListP, uDataLine),
                            ComputeFinalExp(ParamListS, uDataLine));

  CS.uTerminalInterval = min_uint32(uMaxSubframeInterval,
                                    uMaxInterval>>(CS.iFinalExp+CS.uExpShft));

  while ( CS.uCurInterval > CS.uTerminalInterval )
  {
    CS.uCurInterval /= 2;
    CS.iCurExp++;

    /* Reduce number of buckets in half */
    ReduceNumberOfBucketsByHalf(&SRp);
    ReduceNumberOfBucketsByHalf(&SRs);

    if ( SRp.uNumBuckets < NUM_DEFAULT_BUCKETS )
    {
      if ( CS.uCurInterval > CS.uTerminalInterval )
      {
        /* Halve the bucket size and double number of buckets */
        if (DoubleNumberOfBucketsByDividingInHalf(&SRp, &CS) != SB_SUCCESS) { return SB_ERROR; }
        CS.uSlotsPerBucket *= 2; /* Need to keep absolute bucket size the same */
        if (DoubleNumberOfBucketsByDividingInHalf(&SRs, &CS) != SB_SUCCESS) { return SB_ERROR; }
      }
    }
    
    /* Check the number of channels to schedule for this value of iCurExp */
    CountNumCurrentParamsAndExpDiff(ParamListP, pParamP, uDataLine, &CS, &SRp);
    CountNumCurrentParamsAndExpDiff(ParamListS, pParamS, uDataLine, &CS, &SRs);

    /* Schedule secondary channels */
    for ( ;
          ( NULL != pParamS &&
            CS.iCurExp == (int32)(pParamS->ucRootExp + CS.uExpShft) );
          pParamS = NextParamNode(ParamListS, pParamS) )
    {
      if (!ChannelIsApplicable(pParamS, uDataLine)) { continue; }
      if ( SRp.uNumBuckets > NUM_DEFAULT_BUCKETS )  { return SB_ERROR; }

      if (!bSecondaryOpenSlotsValid)
      {
        ExpandPrimaryBucketsToSecondary(&SRp, &SRs);
        bSecondaryOpenSlotsValid = TRUE;
      }
      iBucketIndex = GetMaxOpenSlotsIndex(&SRs, GetSecSrchOrder(&SRp, &SRs)); 
      if (AssignParamOffsetAndUpdateBuckets(pParamS, &SRs, iBucketIndex, &CS) != SB_SUCCESS) { return SB_ERROR; }
      pParamS->uNewInterval *= COEFF_3_TO_1_FACTOR;
    }
    SkipNotApplicableChannels(ParamListS, &pParamS, uDataLine);

    /* Update primary open slots */
    if (bSecondaryOpenSlotsValid) { CombineSecondaryBucketsToPrimary(&SRp, &SRs); }
    
    /* Check and potentially increase the number of primary buckets */
    while ( SRp.uNumBuckets < MAX_BUCKETS &&
            SRp.uNumCurParams > SRp.uNumBuckets &&
            ( SRp.iNextExpDiff > 0 || SRs.iNextExpDiff > 0 ) &&
            ( (int32)min_uint32((uint32)SRp.iNextExpDiff,(uint32)SRs.iNextExpDiff) > log_2(SRp.uNumBuckets) ) )
    {
      /* Double the number of buckets if we have many primary channels */
      DoubleNumberOfBucketsByDividingInHalf(&SRp, &CS);
    }

    /* Schedule primary channels */
    for ( ;
          ( NULL != pParamP &&
            CS.iCurExp == (int32)(pParamP->ucRootExp + CS.uExpShft) );
          pParamP = NextParamNode(ParamListP, pParamP) )
    {
      if ( !ChannelIsApplicable(pParamP, uDataLine) ) { continue; }
      
      bSecondaryOpenSlotsValid = FALSE;
      iBucketIndex = GetMaxOpenSlotsIndex(&SRp, GetPriSrchOrder(&SRp));
      if (AssignParamOffsetAndUpdateBuckets(pParamP, &SRp, iBucketIndex, &CS) != SB_SUCCESS) { return SB_ERROR; }
    }
    SkipNotApplicableChannels(ParamListP, &pParamP, uDataLine);

    /* Check and save a potential control channel */
    if ( CS.uCurInterval <= TERMINAL_SLOT_INTERVAL_COEFF_3 &&
         CS.uCurInterval != CS.uTerminalInterval &&
         SRp.iOpenSlots[0] != SRp.iOpenSlots[1] )
    {
      uint32 uTempWidth, uTempOffset, uTempInterval = CS.uCurInterval;
      if ( uTempInterval <= TERMINAL_SLOT_INTERVAL_COEFF_3/8 || 
          (uTempInterval > TERMINAL_SLOT_INTERVAL_COEFF_3/4 && uTempInterval <= TERMINAL_SLOT_INTERVAL_COEFF_3/2))
      {
        uTempInterval *= 2;  /* Ensure the interval is valid: { 6, 8, 24, 32 } */
      }
      if (SRp.iOpenSlots[0] > SRp.iOpenSlots[1])
      {
        uTempWidth = SRp.iOpenSlots[0] - SRp.iOpenSlots[1];
        uTempOffset = SRp.iOpenSlots[1];
      }
      else
      {
        uTempWidth = SRp.iOpenSlots[1] - SRp.iOpenSlots[0];
        uTempOffset = CS.uSlotsPerBucket + SRp.iOpenSlots[0];
      }
      if ( GetSubFrameBandwidth(uTempWidth, uTempInterval) >
           GetSubFrameBandwidth(uSavedWidth, uSavedInterval) )
      {
        uSavedInterval = uTempInterval;
        uSavedOffset = uTempOffset;
        uSavedWidth = uTempWidth;
      }
    }
  }
  
  /* We need one of the buckets to have slots remaining for message channel */
  if ( SRp.iOpenSlots[0] < 0 )
  {
    return SB_ERROR;
  }
  if (!bSecondaryOpenSlotsValid)
  {
    ExpandPrimaryBucketsToSecondary(&SRp, &SRs);
    bSecondaryOpenSlotsValid = TRUE;
  }
  
  {
    int32 iMaxCtrlWidthP = SRp.iOpenSlots[0];
    int32 iMaxCtrlWidthS = SRs.iOpenSlots[0];
    int32 iMaxCtrlWidthS2 = SRs.iOpenSlots[1] - (CS.uSlotsPerBucket/2);
    if (iMaxCtrlWidthS < iMaxCtrlWidthS2)
    {
      iMaxCtrlWidthS2 = iMaxCtrlWidthS;
    }
    if ( CS.uCurInterval >= uMaxSubframeInterval )
    {
      *puSubFrmLen = uMaxSubframeInterval;
      *puCtrlWidth = iMaxCtrlWidthP;
    }
    else if ( CS.uCurInterval == uMaxSubframeInterval/2 )
    {
      *puSubFrmLen = uMaxSubframeInterval;
      *puCtrlWidth = iMaxCtrlWidthP;
      
      if ( GetSubFrameBandwidth(SRp.iOpenSlots[0] - (CS.uSlotsPerBucket/2), uMaxSubframeInterval/4) >
           GetSubFrameBandwidth(*puCtrlWidth, *puSubFrmLen) )
      {
        *puSubFrmLen = uMaxSubframeInterval/4;
        *puCtrlWidth = SRp.iOpenSlots[0] - (CS.uSlotsPerBucket/2); 
      }
      if ( GetSubFrameBandwidth(iMaxCtrlWidthS2, TERMINAL_SLOT_INTERVAL_COEFF_1) >
           GetSubFrameBandwidth(*puCtrlWidth, *puSubFrmLen) )
      {
        *puSubFrmLen = TERMINAL_SLOT_INTERVAL_COEFF_1;
        *puCtrlWidth = iMaxCtrlWidthS2;
      }
    }
    else 
    {
      if ( GetSubFrameBandwidth(iMaxCtrlWidthS, TERMINAL_SLOT_INTERVAL_COEFF_1) >=
           GetSubFrameBandwidth(iMaxCtrlWidthP, uMaxSubframeInterval/4) )
      {
        *puSubFrmLen = TERMINAL_SLOT_INTERVAL_COEFF_1;
        *puCtrlWidth = iMaxCtrlWidthS;
      }
      else
      {
        *puSubFrmLen = uMaxSubframeInterval/4;
        *puCtrlWidth = iMaxCtrlWidthP;
      }
    }
  }
  if ( GetSubFrameBandwidth(uSavedWidth, uSavedInterval) >
       GetSubFrameBandwidth(*puCtrlWidth, *puSubFrmLen) )
  {
    ShiftSegmentOffsets(ParamListS, uDataLine, uSavedInterval - uSavedOffset);
    ShiftSegmentOffsets(ParamListP, uDataLine, uSavedInterval - uSavedOffset);
    *puSubFrmLen = uSavedInterval;
    *puCtrlWidth = uSavedWidth;
  }
  if ( (uDataLine != 0) &&
       HAL_sb_IsSecondaryDataLineSlot0Supported(pDevCtxt->hSB) )
  {
    if ( (int32) *puCtrlWidth < 0 )
    {
      return SB_ERROR;
    }
  }
  else if ((int32) *puCtrlWidth <= 0)
  {
    return SB_ERROR;
  }
  
  return SB_SUCCESS;
}

/**
 * @brief Verify that the scheduled data channels do not overlap 
 *        each other or overlap the message channel
 *
 * This function verifies that the scheduled data channels do 
 * not overlap each other or overlap the message channel. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] uCtrlWidth  Width of the message channel in slots
 * @param[in] uSubFrmLen  Length of the subframe mode in slots
 * 
 * @return SB_SUCCESS if the data channel schedule is valid, or
 *         an error code if the schedule has overlaps
 */
static SBResult SlimBus_VerifyChannelSchedule
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 uDataLine,
  uint32 uCtrlWidth,
  uint32 uSubFrmLen
)
{
  SlimBusChannelParamType *pParam = NULL;
  SlimBusLinkNodeType *pList, *pNextList;
  uint32 uSlotStartNum, uSlotNum;
  
  /* First zero-out the slot array */
  memset(pDevCtxt->pSched->aSlotArr, 0, SB_SLOTS_PER_SUPERFRAME);
  
  /* Mark all the slots belonging to the message channel as used */
  for ( uSlotStartNum = 0; uSlotStartNum < SB_SLOTS_PER_SUPERFRAME; uSlotStartNum += uSubFrmLen )
  {
    for ( uSlotNum = uSlotStartNum; 
          (uSlotNum < uSlotStartNum+uCtrlWidth) &&
          (uSlotNum < SB_SLOTS_PER_SUPERFRAME);
          uSlotNum++ )
    {
      pDevCtxt->pSched->aSlotArr[uSlotNum] = 1;
    }
  }
  
  pList = &pDevCtxt->pSched->ChanParamList1;
  pNextList = &pDevCtxt->pSched->ChanParamList3;
  /* For all the data channels, mark their slots as used, and verify
     that none of the slots that would be used by the data channels
     have already been used */
  for ( ; pList != NULL; pList = pNextList, pNextList = NULL )
  {
    pParam = NULL;
    while (NULL != (pParam = NextParamNode(pList, pParam)) )
    {
      if ( ChannelIsApplicable(pParam, uDataLine) )
      {
        for ( uSlotStartNum = pParam->uNewOffset; 
              uSlotStartNum < SB_SLOTS_PER_SUPERFRAME;
              uSlotStartNum += pParam->uNewInterval )
        {
          for ( uSlotNum = uSlotStartNum;
                (uSlotNum < uSlotStartNum+pParam->ucSegLen) && 
                (uSlotNum < SB_SLOTS_PER_SUPERFRAME); 
                uSlotNum++)
          {
            if (pDevCtxt->pSched->aSlotArr[uSlotNum] != 0)
            {
              return SB_ERROR;
            }
            pDevCtxt->pSched->aSlotArr[uSlotNum] = 1;
          }
        }
      }
    }
  }

  /* If we've gotten here then we've verified there are no channel overlaps */
  return SB_SUCCESS;
}

/**
 * @brief Outer loop for performing data channel bandwidth 
 *        allocation
 *
 * This function implements the outer loop for performing data 
 * channel bandwidth allocation 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[out] puSubFrmCode  Location to store the scheduled 
 *       subframe code
 * @param[in,out] puClkGear  Location to receive the minimum
 *       clock gear and store the scheduled clock gear
 * 
 * @return SB_SUCCESS if bandwidth allocation was performed 
 *         sucessfully, or and error code if there an allocation
 *         error 
 */
SBResult SlimBus_DoBandwidthAlloc
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 *puSubFrmCode,
  uint32 *puClkGear
)
{
  uint32 dlIdx;
  SBResult rslt = SB_SUCCESS;
  
  /* Subtract frame channel and guide channel from available slots */
  const uint32 uSlotsAvail = SB_SLOTS_PER_SUPERFRAME -
                             SB_FRAME_SLOTS_PER_SUPERFRAME -
                             SB_GUIDE_SLOTS_PER_SUPERFRAME;
  
  /* Initialize the subframe code */
  *puSubFrmCode = 0;
              
  for (dlIdx = 0; dlIdx < SB_MAX_DATA_LINES; dlIdx++)
  {
    uint32 uMinClkGear = *puClkGear;
    uint32 uNewSlotsUsed = pDevCtxt->pSched->aDataLineSchedules[dlIdx].uNewSlotsUsed;
    
    *puClkGear = SB_MAX_CLK_GEAR;

    /* Keep decreasing the clock gear as long as twice the number of used slots is
       less than the number of available slots */
    while (uNewSlotsUsed * 2 <= uSlotsAvail && *puClkGear > 1 && *puClkGear > uMinClkGear)
    {
      (*puClkGear)--;
      uNewSlotsUsed *= 2;
    }
  }
  
  for (dlIdx = 0; dlIdx < SB_MAX_DATA_LINES;)
  {
    /* Only line 0 has msg slots */
    uint32 uMsgSlotReq = dlIdx == 0 ? pDevCtxt->pSched->uMsgSlotReq : 0;
    uint32 uPrevClkGear = *puClkGear;

    if (pDevCtxt->pSched->aDataLineSchedules[dlIdx].uNewSlotsUsed > uMsgSlotReq)
    {
      uint32 uMsgSlots = 0;
      uint32 uCtrlWidth = 0;
      uint32 uSubFrmLen = 0;
  
      /* Try to schedule the data channels at the current clock gear, if we
      we're able to schedule all the channels at the current clock gear, or
      if we don't satisfy the reserved messaging bandwidth after scheduling
      the data channels, increase the clock gear and try again */
      for (; *puClkGear <= SB_MAX_CLK_GEAR; (*puClkGear)++)
      {
        rslt = SlimBus_DoScheduleChannels(pDevCtxt,
                                          dlIdx,
                                          *puClkGear,
                                          &uCtrlWidth,
                                          &uSubFrmLen );
        if (SB_SUCCESS == rslt)
        {
          /* sub frame code only applies to primary line */
          if(dlIdx == 0) 
          {
            *puSubFrmCode = GetSubFrameCoding( &uCtrlWidth, &uSubFrmLen, &uMsgSlots );
          }
          
          if (uMsgSlots >> (SB_MAX_CLK_GEAR - *puClkGear) < uMsgSlotReq && *puClkGear < SB_MAX_CLK_GEAR)
          {
            continue;
          }
          else
          {
            /* If we are already at the max clock gear but haven't satisfied our
               reserved messaging bandwidth, then we've done the best effort and 
               we have to stop and go with what we have */
            break;
          }
        }
      }

      if (SB_SUCCESS == rslt)
      {
        rslt = SlimBus_VerifyChannelSchedule( pDevCtxt,
                                              dlIdx,
                                              uCtrlWidth,
                                              uSubFrmLen );
      }
    }
    
    if (SB_SUCCESS != rslt)
    {
      /* Data line is not schedulable, so don't bother with continuing */
      SB_LOG_ERR_0("One or more channels was not scheduled properly");
      break;
    }

    if (dlIdx > 0 && *puClkGear != uPrevClkGear)
    {
      /* Clock gear increased, restart from the beginning to recalculate parameters */
      dlIdx = 0;
    }
    else 
    {
      dlIdx++;
    }
  }
  
  return rslt;
}
