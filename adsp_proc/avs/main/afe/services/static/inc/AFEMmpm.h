/*========================================================================
 This file contains AFE MMPM related apis


  Copyright (c) 2009-2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
 QUALCOMM Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEMmpm.h#1 $
 ====================================================================== */
#ifndef _AFE_MMPM_H_
#define _AFE_MMPM_H_

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/*==========================================================================
 Include
 ========================================================================== */

/*==========================================================================
 Define
 ========================================================================== */

/*Default sleep latency 10us*/
#define AFE_SLEEP_LATENCY_DEFAULT 10
/*Sleep latency for APCR 30us*/
#define AFE_SLEEP_LATENCY_APCR    30
/*Max sleep latency 1s*/
#define AFE_SLEEP_LATENCY_MAX     1000000

/* These enums are for identifying the type of vote request */
typedef enum
{
	AFE_MMPM_DEVOTE = 0,
	AFE_MMPM_VOTE,
} afe_mmpm_vote_e;

/* These enums are for identifying the type of Topology that is requesting the resources, if adding a new
 * topology please update AFE_MMPM_LAST_TOPOLOGY_BIT_POS as well*/

/* The bit position of last topology */
#define AFE_MMPM_LAST_TOPOLOGY_BIT_POS 9

typedef enum
{
	AFE_MMPM_DEFAULT_TOPOLOGY = (1 << 0),
	AFE_MMPM_LOOPBACK_TOPOLOGY = (1 << 1),
	AFE_MMPM_SIDETONE_IIR_TOPOLOGY = (1 << 2),
	AFE_MMPM_BT_FM_TOPOLOGY = (1 << 3),
	AFE_MMPM_CLIENT_TOPOLOGY = (1 << 4),
	AFE_MMPM_DTMF_TOPOLOGY=(1 << 5),
	AFE_MMPM_GROUP_TOPOLOGY=(1 << 6),
	AFE_MMPM_CUST_TOPOLOGY = (1 << 7),
	AFE_MMPM_ENC_FWK_TOPOLOGY = (1 << 8),
	AFE_MMPM_DEC_FWK_TOPOLOGY = (1 << AFE_MMPM_LAST_TOPOLOGY_BIT_POS),
	AFE_MMPM_MAX_TOPOLOGY = (1 << 31), //To make this enum 4 bytes
} afe_mmpm_topo_e;

/*
  This is for voting for MMPM resources.

  @param[in]     afe_dev_ptr: This is the port pointer
  @param[in]     topo: the type of Topology the voting is for
  @param[in]     mmpm_vote: this specifies whether it is a devote, vote or update request.
  @param[in]     vote_now: this is for bundling the request to MMPM together. If it is set to true, the current votings
                           will be committed to MMPM, if false we would just update our internal votings.

  @return
  ADSPResult
 */
ADSPResult afe_mmpm_voting(void *afe_dev_ptr, afe_mmpm_topo_e topo, afe_mmpm_vote_e mmpm_vote,
		bool_t vote_now);

/*
  This is for voting sleep latency. It will override the default votings done though afe_mmpm_voting().

  @param[in]     pDevPort: This is the port pointer
  @param[in]     sleep_latency: Sleep latency needed in us.

  @return
  ADSPResult
 */
ADSPResult afe_mmpm_vote_sleep_latency(void *pDevPort, uint32_t sleep_latency_us);


/*This function can be called by either connecting client/ disconnected client.
 *For connecting client, this function adds kpps/bw related to particular client to aggregated client kpps/bw.
 *For disconnected client, removes kpps/bw contribution due to that particular client from aggregated client kpps/bw.
 *
 *pDevPort          : Used pDevPort to access mmpm_info_ptr, from which aggregated client kpps/bw is updated/stored
 *psNewClient       : Used psNewClient to access afe_client, to store individual client kpps/bw
 *is_client_connect : This flag indicates, whether it is connecting client or disconnected client *
 **/
void afe_port_update_aggregated_client_kpps_and_bw(void *pDevPort, void *pClientInfo, bool_t is_client_connect);


/*afe_port_update_dynamic_kpps() is called (before adding new client to linked list) or (after removing client from linked list) or during DTMF initialization.
 *Considered Limiter and audio voice mixer kpps as afe port's dynamic kpps, since these operation's kpps might vary due to client connect/disconnect
 *or DTMF presence/absence.
 *
 * pDevPort          : pDevPort is used to get the information related to previous client nodes and current DTMF state
 * client_info_ptr   : client_info_ptr is pointer to updating client (that is going to be added to linked list or removed from linked list)
 *                     When DTMF is calling this function, it will send client_info_ptr pointer as NULL
 * is_client_connect : This flag is used to check whether the updating client is the  connecting client (client to be added to list) or
 *                     disconnected client (removed from list)
 *                     When DTMF is calling this function, it will send is_client_connect as FALSE
 */
void afe_port_update_dynamic_kpps(void *pDevPort, void *client_info_ptr, bool_t is_client_connect);


/*This function calculates common bandwidth factor
 * pDevPort       : Uses port device pointer to calculate common bandwidth factor
 *returns  common bandwidth factor
 *
 * Introduced this function, to provide common bandwidth factor for both CLIENT_TOPOLOGY and DEFAULT_TOPOLOGY
 * */
uint32_t afe_port_get_common_bw_factor(void *pDevPort);

/*This function registers audio client class and DCVS up only
 * client_id -> client which is registering 
 * returns none 
 * */
ADSPResult afe_mmpm_register_audio_client_class_and_dcvs_adjust(uint32_t client_id);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_MMPM_H_ */
