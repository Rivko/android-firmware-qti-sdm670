/*==========================================================================


This file implements the event registration utility functions for the Audio Dec Dynamic Service.

Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who        what, where, why
--------   ---        ---------------------------------------------------------
2/15/17   pparamat   Initial creation
=========================================================================== */

#include "AudioDecSvc_EventUtils.h"

static aud_dec_svc_event_node_t *aud_dec_svc_create_event_node(aud_dec_svc_event_query_payload_t *pPayload);

/*==========================================================
 * Function name : aud_dec_svc_create_event_node
 *  This function creates a new node and returns the pointer.
 *==========================================================*/
aud_dec_svc_event_node_t *aud_dec_svc_create_event_node(aud_dec_svc_event_query_payload_t *pPayload)
{

	aud_dec_svc_event_node_t *node = (aud_dec_svc_event_node_t *) NODE_MALLOC(sizeof(aud_dec_svc_event_node_t));
	
	if(node == NULL)
	{
		return node;
	}
	
	node->payload = *pPayload;
	node->next_node       = NULL;

	return node;
}


/*=====================================================================================================
 * Function name : aud_dec_svc_event_insert_node
 * This function inserts a node for a combination of (event ID, module ID, instance ID) if it is
 * not present earlier
 *======================================================================================================*/
nodeError aud_dec_svc_event_insert_node(aud_dec_svc_event_query_payload_t *pPayload,
								aud_dec_svc_event_node_t       **rootNode)
{

	aud_dec_svc_event_node_t *find_node = aud_dec_svc_find_node(pPayload,rootNode);

	if((NULL != find_node))
	{
		return NODE_SUCCESS;
	}

	aud_dec_svc_event_node_t *node = aud_dec_svc_create_event_node(pPayload);
	aud_dec_svc_event_node_t *nextNode;
	
	if(node == NULL)
	{
		return NODE_CREATION_FAIL;
	}
	
	
	if((*rootNode) == NULL)
	{	
		(*rootNode) = node;	
	}
	else 
	{
		nextNode = (*rootNode);	
		
		while(nextNode->next_node != NULL)
		{
			nextNode = nextNode->next_node;
		}
		
		nextNode->next_node = node;	
	}
	MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Inserted node at 0x%lX for (event) = (0x%lX) ", node, pPayload->event_id );
	return NODE_SUCCESS;
}


/*=====================================================================================================
 * Function name : aud_dec_svc_event_delete_node
 *  This function is called when an event is deregistered with a (module ID, instance ID) pair
 *  which has been registered earlier.
 *  This deletes the node that has been created earlier for a given (event ID, module ID, instance ID)
 *======================================================================================================*/
nodeError aud_dec_svc_event_delete_node(aud_dec_svc_event_query_payload_t *pPayload,
                                    aud_dec_svc_event_node_t  **rootNode)

{

	aud_dec_svc_event_node_t *prevNode = NULL;
	aud_dec_svc_event_node_t *currNode = (*rootNode);
	
	if((currNode == NULL) || (pPayload == NULL))
	{
		return NODE_NOT_FOUND;
	}

	while( (pPayload->event_id != currNode->payload.event_id) )
	{
		prevNode = currNode;
		currNode = currNode->next_node; 
		
		if(currNode == NULL)
		{
			return NODE_NOT_FOUND;
		}
	}
	
	if(currNode == (*rootNode))
	{
		(*rootNode) = currNode->next_node; 
	}
	else 
	{
	// reach here when you find the node with desired eventID
	  prevNode->next_node = currNode->next_node;
	}
	currNode->next_node = NULL;
	NODE_FREE(currNode); // remove the node from linked list
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Removed node at 0x%lX for (event) = (0x%lX) ", currNode,pPayload->event_id);
	return NODE_SUCCESS;
}

/*============================================================================================
 * Function name : aud_dec_svc_delete_all_nodes
 *  This function deletes all the nodes created earlier and is called from aud_dec_svc_destroy_svc()
 *============================================================================================*/
void aud_dec_svc_delete_all_nodes(aud_dec_svc_event_node_t **rootNode)
{
	aud_dec_svc_event_node_t *curr_node = *rootNode;
	aud_dec_svc_event_node_t *temp_node = NULL;
	
	if(curr_node == NULL)
	{
		return;
	}

	temp_node = curr_node->next_node;

	while ((curr_node)->next_node != NULL )
	{
		curr_node->next_node = temp_node->next_node; 
		temp_node->next_node = NULL;
		NODE_FREE(temp_node);
		temp_node  = curr_node->next_node;
		
	}
	(*rootNode)->next_node = NULL;
	NODE_FREE(*rootNode);
	*rootNode = NULL;

}

/*===============================================================================
 * Function name : aud_dec_svc_find_node
 *  This function returns the node ID which matches with the
 *  event ID, module ID and instance ID of the payload provided as input.
 *  If no node is found with the provided given combination then NULL is returned.
 *================================================================================*/

aud_dec_svc_event_node_t *aud_dec_svc_find_node(aud_dec_svc_event_query_payload_t *pPayload,
                                    aud_dec_svc_event_node_t  **rootNode)
{
	aud_dec_svc_event_node_t *currNode = (*rootNode);
	if(currNode == NULL)
	{
		return currNode;
	}

	while( (pPayload->event_id != currNode->payload.event_id) )
	{
		currNode = currNode->next_node;
		
		if(currNode == NULL)			break;
	}
	return currNode;
}									
