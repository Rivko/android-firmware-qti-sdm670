/******************************************************************************
  @file    ResourceQueues.h
  @brief   Implementation of resource queue

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __RESOURCE_QUEUES__H_
#define __RESOURCE_QUEUES__H_

#define NOT_SUPPORTED -2
#define MAX_LVL 0xFFFFFFFF


/*************************************************************************************************
** Visual representation of resource Qs                                                         **
**  1------1                                                                                    **
**  |  []  |  (right)             3-------3                                                     **
**  |  []--|----------------------|-->[]  |   4------------4                                    **
**  |  []  |   2----------2       |   []--|---|-->[]-->[]  |                                    **
**  |  []--|---|->[]-->[] |       |    .  |   4------------4                                    **
**  |  .   |   | (next)   |       |   []  |                                                     **
**  |  []  |   2----------2       3-------3                                                     **
**  |  []  |                                                                                    **
**  |  []  |                                                                                    **
**  1------1                                                                                    **
**                                                                                              **
**  table 1 represents all statically created resource q nodes of total MAX_MINOR_RESOURCES     **
**  table 2 represents when we pend requests based level to a resource                          **
**  table 3 represents when a minor type has conflicts in resource, then MAX_CORES              **
**            additional resource q nodes created (dynamically) to accomodate resources         **
**            which are extra based on core/cluster numbers                                     **
**  table 4 same as 2nd                                                                         **
*************************************************************************************************/
#include "OptsHandler.h"
#include "RestoreHandler.h"
#include "ResourceInfo.h"

typedef struct q_node {
    Request *handle;
    Resource resource;
    q_node *next;
    q_node *right;
}q_node;

class ResourceQueue {
    private:
        q_node resource_qs[MAX_MINOR_RESOURCES];
        bool display_off = false;

    private:
        void CopyQnode(q_node *n1, q_node *n2);
        q_node *GetNode(Resource &resObj);
        int GetResetOptData(int idx, int level);
        void PendCurrentRequestsOp(int idx, q_node *current);
        void LockCurrentStateOp(q_node *current, Request *req);
        void UnlockCurrentStateOp(int idx, q_node *current, q_node *pending, Request *req);

    public:
        ResourceQueue();
        ~ResourceQueue();

        int Init();
        bool AddAndApply(Request *req);
        bool RemoveAndApply(Request *req);
        void RemovePendingRequest(Resource &resObj, Request *req);
        void PendCurrentRequests();
        void LockCurrentState(Request *req);
        void UnlockCurrentState(Request *req);
        void Reset();
};


#endif /*__RESOURCE_QUEUES__H_*/
