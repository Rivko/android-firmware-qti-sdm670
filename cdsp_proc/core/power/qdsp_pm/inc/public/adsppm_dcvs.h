/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_dcvs.h
@brief: Header for ADSPPM APIs for DCVS.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/public/adsppm_dcvs.h#1 $
*/

#ifndef ADSPPM_DCVS_H_
#define ADSPPM_DCVS_H_

#include "DALStdDef.h"

/**
 * @enum ADSPPM_DCVS_STATUS
 * @brief return value
 */
typedef enum
{
    ADSPPM_DCVS_STATUS_SUCCESS,
    /**< Success. */

    ADSPPM_DCVS_STATUS_FAILED,
    /**< General failure. */

    ADSPPM_DCVS_STATUS_BADPARM,
    /**< Invalid parameter. */

    ADSPPM_DCVS_STATUS_MAX,
    /**< Maximum count. */

    ADSPPM_DCVS_STATUS_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */

} ADSPPM_DCVS_STATUS;

/**
 * @enum AdsppmDcvsResourceId
 * @brief Resource id's for DCVS requests
 */
typedef enum
{
    ADSPPM_DCVS_RSC_ID_NONE,

    ADSPPM_DCVS_RSC_ID_ADSP_CLOCK,
    /**< Resource ID for ADSP clock vote. */

    ADSPPM_DCVS_RSC_ID_ADSP_DDR_BW,
    /**< Resource ID for ADSP to DDR bandwidth vote. */

    ADSPPM_DCVS_RSC_ID_HCP_DDR_BW,
    /**< Resource ID for HCP to DDR bandwidth vote. */

    ADSPPM_DCVS_RSC_ID_DMA_DDR_BW,
    /**< Resource ID for DMA to DDR bandwidth vote. */
	
    ADSPPM_DCVS_RSC_ID_MAX,
    /**< Maximum count. */

    ADSPPM_DCVS_RSC_ID_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */

} AdsppmDcvsResourceId;

/**
 * @struct AdsppmDcvsAdsp2DdrBwVoteType
 * @brief structure for DCVS to update DDR bandwidth vote to ADSPPM
 */
typedef struct
{
    uint64 iBw;
    /**< Instantaneous bandwidth vote in Bytes per second */

    uint64 aBw;
    /**< Average bandwidth vote in Bytes per second */
    
} AdsppmDcvsAdsp2DdrBwVoteType;

/**
 * @union AdsppmDcvsResourceVoteType
 * @brief structure for DCVS to update resource vote to ADSPPM
 */
typedef union
{
    uint32 adspClockInHz;
    /**< ADSP clock frequency request in Hertz */

    AdsppmDcvsAdsp2DdrBwVoteType adsp2DdrBwInIbAb;
    /**< Structure for holding bandwidth vote in IB/AB */

    AdsppmDcvsAdsp2DdrBwVoteType vapss2DdrBwInIbAb;
    /**< Structure for holding bandwidth vote in IB/AB */

} AdsppmDcvsResourceVoteType;

/**
 * @brief API for DCVS to update its resource votes
 * @details
 *     DCVS uses this API to update its votes on ADSP clock
 *     and DDR bandwidth.
 *     Based on resourceId provided, ADSPPM aggregates a final vote
 *     for corresponding resource owner.
 *     Both static and DCVS votes are considered in aggregation.
 * @param resourceId Resource Id for this request.
 * @parblock
 *     ADSPPM_DCVS_RSC_ID_ADSP_CLOCK - for ADSP clock vote
 *
 *     ADSPPM_DCVS_RSC_ID_DDR_BW - for DDR bandwidth vote
 * @endparblock
 * @param resourceVote Pointer to a structure with DCVS vote for this resource.
 * @parblock
 *     ADSPPM_DCVS_RSC_ID_ADSP_CLOCK:
 *         resourceVote.adspClockInHz DCVS ADSP clock request in Hz
 *
 *     ADSPPM_DCVS_RSC_ID_DDR_BW:
 *         resourceVote.adsp2DdrBwInIbAb.iBw DCVS Average DDR bandwidth
 *         request in Bytes per second
 *
 *         resourceVote.adsp2DdrBwInIbAb.iAw DCVS Instantaneous DDR bandwidth
 *         request in Bytes per second
 * @endparblock
 * @return ADSPPM_DCVS_STATUS
 * @retval ADSPPM_DCVS_STATUS_SUCCES for successful vote.
 * @retval ADSPPM_DCVS_STATUS_FAILED vote from DCVS is dropped / cannot be served.
 * @retval ADSPPM_DCVS_STATUS_BADPARM vote from DCVS has invalid parameter values.
 */
ADSPPM_DCVS_STATUS adsppm_dcvs_request(
    AdsppmDcvsResourceId resourceId,
    AdsppmDcvsResourceVoteType *resourceVote);


#endif /* ADSPPM_DCVS_H_ */

