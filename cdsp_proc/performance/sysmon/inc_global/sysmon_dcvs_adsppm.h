/*-----------------------------------------------------------------------------
   Copyright (c) 2015 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------------*/

#ifndef SYSMON_DCVS_ADSPPM_H_
#define SYSMON_DCVS_ADSPPM_H_

#include <stdlib.h>
/**
 *  @file sysmon_dcvs_adsppm.h
 *  @brief SYSMON APIs for ADSPPM
 */

/**
 *  @enum sd_adsppm_resource_id
 *  @brief Resource Ids for ADSPPM interaction
 */
typedef enum
{
    SYSMON_DCVS_RSC_ID_NONE,
    /**< Unused resource Id */

    SYSMON_DCVS_RSC_ID_ADSP_CLK,
    /**< Resource Id for ADSP core clock */

    SYSMON_DCVS_RSC_ID_ADSP_DDR_BW,
    /**< Resource Id for ADSP to DDR bandwidth */
    
    SYSMON_DCVS_RSC_ID_HCP_DDR_BW,
    /**< Resource ID for HCP to DDR bandwidth vote. */
    
    SYSMON_DCVS_RSC_ID_DMA_DDR_BW, 
    /**< Resource ID for DMA to DDR bandwidth vote. */

    SYSMON_DCVS_RSC_ID_MAX,
    /**< Maximum count */
} sd_adsppm_resource_id;

/**
 *  @struct sd_adsppm_bus_vote_t
 *  @brief structure definition for ADSP to DDR bandwidth vote
 */
typedef struct
{
    uint64_t uAb;
    /**< Average bandwidth in bytes per second */

    uint64_t uIb;
    /**< Instantaneous bandwidth in bytes per second */
} sd_adsppm_bus_vote_t;

/**
 *  @union sd_adsppm_vote_t
 *  @brief union definition for ADSP core clk and ADSP to DDR bandwidth votes
 */
typedef union
{
    uint32_t coreVoteinHz;
    /**< ADSP core clock vote in hertz */

    sd_adsppm_bus_vote_t busVoteinAbIb;
    /**< ADSP to DDR bandwidth vote in uAb, uIb pair */
} sd_adsppm_vote_t;

/**
 *  @enum sd_adsppm_dcvs_state
 *  @brief structure definition for DCVS state for ADSPPM interaction
 */
typedef enum
{
    SYSMON_DCVS_STATE_NONE,
    /**< Invalid DCVS state entry */

    SYSMON_DCVS_STATE_ENABLE,
    /**< DCVS state entry for enabling DCVS */

    SYSMON_DCVS_STATE_DISABLE,
    /**< DCVS state entry for disabling DCVS */

    SYSMON_DCVS_STATE_MAX,
    /**< Maximum count */
} sd_adsppm_dcvs_state;


/**************************************************************************//**
 * @fn sysmon_adsppm_static_vote_update
 * @brief To be called by ADSPPM whenever there is a change in the final static
 * vote.
 *
 * This API would basically set a signal to SysMon for processing the new
 * static vote.
 * @param resourceId Resource id of the updated resource
 * @parblock
 *        SYSMON_DCVS_RSC_ID_ADSP_CLK for core clock vote update
 *        SYSMON_DCVS_RSC_ID_ADSP_DDR_BW for ADSP to DDR bandwidth vote update
 * @endparblock
 * @param resourceVote Pointer to the structure with updated vote
 * @parblock
 *        SYSMON_DCVS_RSC_ID_ADSP_CLK:
 *          resourceVote.coreClkinkHz with the new core clock vote in
 *          kilo-hertz
 *        SYSMON_DCVS_RSC_ID_ADSP_DDR_BW:
 *          resourceVote.busVote.uAb with the new average DDR bandwidth vote
 *          resourceVote.busVote.uIb with the new instantaneous DDR bandwidth
 *              vote
 * @endparblock
 * @return none
 *****************************************************************************/
void sysmon_adsppm_static_vote_update(sd_adsppm_resource_id resourceId,
                                      sd_adsppm_vote_t *resourceVote);

/**************************************************************************//**
 * @fn sysmon_adsppm_state_change_request
 * @brief API exposed by sysmon to ADSPPM for enabling or disabling DCVS.
 *
 * By default, DCVS will be in DISABLED state. ADSPPM has to enable DCVS on
 * receiving the first client's vote (DCVS_FLOOR or DCVS_DYNAMIC client) after
 * bootup. ADSPPM is expected to DISABLE DCVS and discard DCVS vote in
 * aggregator when there are no active DCVS_FLOOR or DCVS_DYNAMIC clients in
 * the system. i.e. when all the clients have removed their votes. ENABLE DCVS
 * back when there is an active client i.e, on receiving vote from a client
 * (other than DCVS_FIXED client)
 *
 * ADSPPM should DISABLE DCVS when only DCVS_FIXED clients are active i.e,
 * only DCVS_FIXED clients have voted and ENABLE DCVS back on receiving a vote
 * from either a DCVS_FLOOR or DCVS_DYNAMIC client.
 *
 * @param state new DCVS state
 * @parblock
 *        SYSMON_DCVS_STATE_ENABLE for enabling DCVS
 *        SYSMON_DCVS_STATE_DISABLE for disabling DCVS
 * @endparblock
 *
 * @return none
 *****************************************************************************/
void sysmon_adsppm_dcvs_state_change_request(sd_adsppm_dcvs_state state);

/**************************************************************************//**
 * @fn sysmon_adsppm_client_class_update
 * @brief API exposed by sysmon to ADSPPM for receiving aggregate client class.
 * 
 * ADSPPM needs to call this API on updating the aggregated client class. 
 * sysmon will use this information to configure QoS settings appropriate to
 * the current active client class set.
 *****************************************************************************/
void sysmon_adsppm_client_class_update(uint32_t aggregateClientClass);

#endif /* SYSMON_DCVS_ADSPPM_H_ */
