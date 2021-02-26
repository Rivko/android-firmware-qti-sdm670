/*==========================================================================


This file implements the event registration function related declarations for the Audio Dec Dynamic Service.

Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who        what, where, why
--------   ---        ---------------------------------------------------------
2/15/17   pparamat   Initial creation
=========================================================================== */

#include "adsp_media_fmt.h"
#include "Elite.h"
#include "AudioStreamMgr.h"

#ifndef AUDIODECSVC_EVENTS_UTILS_H_
#define AUDIODECSVC_EVENTS_UTILS_H_

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define NODE_MALLOC(size) qurt_elite_memory_malloc(size,QURT_ELITE_HEAP_DEFAULT)
#define NODE_FREE(ptr)    qurt_elite_memory_free(ptr)

enum nodeError{
	NODE_CREATION_FAIL,
	NODE_NOT_FOUND,
	NODE_FOUND,
	NODE_SUCCESS
};

typedef struct aud_dec_svc_event_query_payload_t {
   AudioStreamMgr_CallBackHandleType cb;
	uint32_t event_id;
	uint32_t event_bit_field;
} aud_dec_svc_event_query_payload_t;

typedef struct aud_dec_svc_event_node_t {
	
	aud_dec_svc_event_query_payload_t   payload;
	aud_dec_svc_event_node_t  		      *next_node;
	
} aud_dec_svc_event_node_t;

nodeError aud_dec_svc_event_insert_node(aud_dec_svc_event_query_payload_t *pPayload, aud_dec_svc_event_node_t **rootNode);

nodeError aud_dec_svc_event_delete_node(aud_dec_svc_event_query_payload_t *pPayload, aud_dec_svc_event_node_t  **rootNode);

void aud_dec_svc_delete_all_nodes(aud_dec_svc_event_node_t **rootNode);

aud_dec_svc_event_node_t *aud_dec_svc_find_node(aud_dec_svc_event_query_payload_t *pPayload, aud_dec_svc_event_node_t  **rootNode);

#if defined(__cplusplus)
}
#endif // __cplusplus


#endif // AUDIODECSVC_EVENTS_UTILS_H_
