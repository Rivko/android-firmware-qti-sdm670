/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_limits.h

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/inc/public/adsppm_limits.h#1 $
*/

#ifndef ADSPPM_LIMIT_H_
#define ADSPPM_LIMIT_H_

#include "DALStdDef.h"

/**
 * @enum ADSPPM_LIMIT_STATUS
 * @brief return value
 */
typedef enum
{
    ADSPPM_LIMIT_STATUS_SUCCESS,
    /**< Success. */

    ADSPPM_LIMIT_STATUS_FAILED,
    /**< General failure. */

    ADSPPM_LIMIT_STATUS_BADPARM,
    /**< Invalid parameter. */

    ADSPPM_LIMIT_STATUS_MAX,
    /**< Maximum count. */

    ADSPPM_LIMIT_STATUS_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */

} ADSPPM_LIMIT_STATUS;

/**
 * @enum AdsppmLimitResourceId
 * @brief Resource id's for LIMIT requests
 */
typedef enum
{
    ADSPPM_LIMIT_RSC_ID_NONE,
	
	ADSPPM_LIMIT_RSC_ID_ADSP_CLOCK_VOTE,
	/**< Resource ID for ADSP Max Clock. */

    ADSPPM_LIMIT_RSC_ID_MAX,
    /**< Maximum count. */

    ADSPPM_LIMIT_RSC_ID_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */

} AdsppmLimitResourceId;


/**
 * @union AdsppmLimitResourceVoteType
 * @brief structure for Clients to update resource vote to ADSPPM
 */
typedef union
{
    uint32 adspClockInHz;
    /**< ADSP clock frequency request in Hertz */

} AdsppmLimitResourceVoteType;

/**
 * @brief API for Clients to update its resource votes
 * @details
 *     Clients uses this API to update its votes on ADSP clock
 *     and DDR bandwidth.
 *     Based on resourceId provided, ADSPPM aggregates a final vote
 *     for corresponding resource owner.
 * @param resourceId Resource Id for this request.
 * @parblock
 *     ADSPPM_LIMIT_RSC_ID_ADSP_CLOCK - for ADSP clock vote
 *
 * @endparblock
 * @param resourceVote Pointer to a structure with LIMIT vote for this resource.
 * @parblock
 *     ADSPPM_LIMIT_RSC_ID_ADSP_CLOCK:
 *         resourceVote.adspClockInHz LIMIT ADSP clock request in Hz
 *
 * @endparblock
 * @return ADSPPM_LIMIT_STATUS
 * @retval ADSPPM_LIMIT_STATUS_SUCCES for successful vote.
 * @retval ADSPPM_LIMIT_STATUS_FAILED vote is dropped / cannot be served.
 * @retval ADSPPM_LIMIT_STATUS_BADPARM vote has invalid parameter values.
 */
ADSPPM_LIMIT_STATUS adsppm_limit_request(
    AdsppmLimitResourceId resourceId,
    AdsppmLimitResourceVoteType *resourceVote);


#endif /* ADSPPM_LIMIT_H_ */

