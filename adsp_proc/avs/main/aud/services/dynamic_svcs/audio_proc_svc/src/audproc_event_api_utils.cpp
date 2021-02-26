/*==========================================================================


This file implements the event registration utility functions for the Audio Post
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

#include "audproc_event_api_utils.h"

static audproc_event_node_t *AudPP_create_event_node(audproc_event_query_payload *pPayload);

/*==========================================================
 * Function name : AudPP_create_event_node
 *  This function creates a new node and returns the pointer.
 *==========================================================*/
static audproc_event_node_t *AudPP_create_event_node(audproc_event_query_payload *pPayload)
{
	audproc_event_node_t *node = (audproc_event_node_t *) NODE_MALLOC(sizeof(audproc_event_node_t));
	
	if(node == NULL)
	{
		return node;
	}
	
	node->receivedPayload = *pPayload;
	node->next_node       = NULL;

	return node;
}


/*=====================================================================================================
 * Function name : AudPP_event_insert_node
 * This function inserts a node for a combination of (event ID, module ID, instance ID) if it is
 * not present earlier
 *======================================================================================================*/
nodeError AudPP_event_insert_node(audproc_event_query_payload *pPayload,
								audproc_event_node_t       **rootNode)
{

	audproc_event_node_t *find_node = AudPP_find_node(pPayload,rootNode);

	if((NULL != find_node))
	{
		return NODE_SUCCESS;
	}

	audproc_event_node_t *node = AudPP_create_event_node(pPayload);
	audproc_event_node_t *nextNode;
	
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
			nextNode = nextNode->next_node; 
		
		nextNode->next_node = node;	
	}
	MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Inserted node at 0x%lX for \
			(event,module,instance) = (0x%lX, 0x%lX, 0x%X) ", node,pPayload->eventID, pPayload->moduleID, pPayload->instanceID );
	return NODE_SUCCESS;
}


/*=====================================================================================================
 * Function name : AudPP_event_delete_node
 *  This function is called when an event is deregistered with a (module ID, instance ID) pair
 *  which has been registered earlier.
 *  This deletes the node that has been created earlier for a given (event ID, module ID, instance ID)
 *======================================================================================================*/
nodeError AudPP_event_delete_node(audproc_event_query_payload *pPayload,
                                    audproc_event_node_t  **rootNode)

{

	audproc_event_node_t *prevNode = NULL;
	audproc_event_node_t *currNode = (*rootNode);
	
	if((currNode == NULL) || (pPayload == NULL))
	{
		return NODE_NOT_FOUND;
	}

	while((pPayload->eventID != currNode->receivedPayload.eventID) ||
			(pPayload->moduleID != currNode->receivedPayload.moduleID)  ||
			(pPayload->instanceID != currNode->receivedPayload.instanceID))
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
	MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Removed node at 0x%lX for \
			(event,module,instance) = (0x%lX, 0x%lX, 0x%X) ", currNode,pPayload->eventID, pPayload->moduleID, pPayload->instanceID );
	return NODE_SUCCESS;
}

/*============================================================================================
 * Function name : AudPP_delete_all_nodes
 *  This function deletes all the nodes created earlier and is called from AudPP_DestroySvc()
 *============================================================================================*/
void AudPP_delete_all_nodes(audproc_event_node_t **rootNode)
{

	audproc_event_node_t *curr_node = *rootNode;
	audproc_event_node_t *temp_node = NULL;
	
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
 * Function name : AudPP_find_node
 *  This function returns the node ID which matches with the
 *  event ID, module ID and instance ID of the payload provided as input.
 *  If no node is found with the provided given combination then NULL is returned.
 *================================================================================*/

audproc_event_node_t *AudPP_find_node(audproc_event_query_payload *pPayload,
                                    audproc_event_node_t  **rootNode)
{
	audproc_event_node_t *currNode = (*rootNode);
	if(currNode == NULL)
	{
		return currNode;
	}

	while((pPayload->eventID != currNode->receivedPayload.eventID) ||
			(pPayload->moduleID != currNode->receivedPayload.moduleID)  ||
			(pPayload->instanceID != currNode->receivedPayload.instanceID))
	{
		currNode = currNode->next_node;
		
		if(currNode == NULL)
			break;
	}
	return currNode;
}									
