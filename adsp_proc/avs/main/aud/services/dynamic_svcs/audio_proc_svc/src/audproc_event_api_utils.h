/*==========================================================================


This file implements the event registration function related declarations for the Audio Post
Processing Dynamic Service.

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

#ifndef AUDPROC_EVENTS_UTILS_H
#define AUDPROC_EVENTS_UTILS_H


#define NODE_MALLOC(size) qurt_elite_memory_malloc(size,QURT_ELITE_HEAP_DEFAULT)
#define NODE_FREE(ptr)    qurt_elite_memory_free(ptr)

enum nodeError{
	NODE_CREATION_FAIL,
	NODE_NOT_FOUND,
	NODE_FOUND,
	NODE_SUCCESS
};




typedef struct audproc_event_query_payload_t {

	uint32_t eventID;
	uint32_t moduleID;
	uint16_t instanceID;
	uint16_t src_addr;
	uint16_t src_port;
	uint16_t dest_port;
	uint16_t dest_addr;
	uint32_t apr_token;
	uint32_t eventbitfield;
	bool_t   is_registered;
	
} audproc_event_query_payload;

typedef struct audproc_event_node_t {
	
	audproc_event_query_payload   receivedPayload;
	audproc_event_node_t  		  *next_node;
	
}audproc_event_node_t;



nodeError AudPP_event_insert_node(audproc_event_query_payload *pPayload,
								audproc_event_node_t       **rootNode);

nodeError AudPP_event_delete_node(audproc_event_query_payload *pPayload,
                                    audproc_event_node_t  **rootNode);


void AudPP_delete_all_nodes(audproc_event_node_t **rootNode);



audproc_event_node_t *AudPP_find_node(audproc_event_query_payload *pPayload,
                                    audproc_event_node_t  **rootNode);

#endif // AUDPROC_EVENTS_UTILS_H
