/******************************************************************************
  @file    ResourceQueues.cpp
  @brief   Implementation of resource queue

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>

#define LOG_TAG "ANDR-PERF-RESOURCEQS"
#include <cutils/log.h>

#include "Request.h"

#include "ResourceQueues.h"
#include "MpctlUtils.h"

ResourceQueue::ResourceQueue() {
}

ResourceQueue::~ResourceQueue() {
}

int ResourceQueue::Init() {
    q_node *nodes = NULL;
    Target &target = Target::getCurTarget();
    int i = 0;

    memset(resource_qs, 0x00, sizeof(resource_qs));
    /*Pre-creation of right node is needed to handle display off case,
      where we may directly add the node in pend and current remians
      empty*/
    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
         nodes = (q_node *)calloc(target.getTotalNumCores(), sizeof(q_node));
         if (NULL == nodes) {
             QLOGW("Failed to pend new request for optimization");
         } else {
             memset(nodes, 0x00, sizeof(q_node)*target.getTotalNumCores());
             resource_qs[i].right = nodes;
         }
    }

    return 0;
}

void ResourceQueue::CopyQnode(q_node *node1, q_node *node2) {
    if (node1 == NULL || node2 == NULL) {
        return;
    }
    node1->handle = node2->handle;
    node1->next = node2->next;
    node1->resource = node2->resource;
}

q_node *ResourceQueue::GetNode(Resource &resObj) {
    q_node *tmp = NULL;
    q_node *nodes = NULL;
    Target &target = Target::getCurTarget();
    unsigned short idx = resObj.qindex;

    if ((idx >= MAX_MINOR_RESOURCES) ||
        (resObj.core < 0) || (resObj.core > target.getTotalNumCores())) {
        return NULL;
    }

    if (0 == resObj.core) {
        QLOGI("Returning from core 0 index");
        tmp = &resource_qs[idx];
        return tmp;
    }

    if (NULL == resource_qs[idx].right) {
        if (resObj.core) {
            QLOGI("Before calloc of qnode for inserting node in ResourceQ");
            nodes = (q_node *)calloc(target.getTotalNumCores(), sizeof(q_node));
            if (NULL == nodes) {
                QLOGW("Failed to pend new request for optimization [0x%X,0x%X]",
                      resObj.major, resObj.minor);
                tmp = NULL;
            } else {
                memset(nodes, 0x00, sizeof(q_node)*target.getTotalNumCores());
                resource_qs[idx].right = nodes;
                tmp = &resource_qs[idx].right[resObj.core];
            }
        }
    } else {
        tmp = &resource_qs[idx].right[resObj.core];
    }
    return tmp;
}

bool ResourceQueue::AddAndApply(Request *req) {
    q_node *current = NULL;
    q_node *pended = NULL, *recent = NULL, *iter = NULL;
    unsigned int level = 0;
    int rc = 0, i = 0;
    bool ret = false;
    OptsHandler &oh = OptsHandler::getInstance();
    Target &target = Target::getCurTarget();

    if (NULL == req) {
        return false;
    }

    QLOGI("no. of locks %d", req->GetNumLocks());

    for (i = 0; i < req->GetNumLocks(); i++) {
        int needAction = FAILED, action = FAILED;
        ResourceInfo *res = req->GetResource(i);
        if (NULL == res) {
            continue;
        }
        Resource &resObj = res->GetResourceObject();
        unsigned short qindex = resObj.qindex;
        res->Dump();

        /* If resource requested is not supported, ignore it */
        if (!target.isResourceSupported(resObj.major, resObj.minor)) {
            QLOGE("Resource with major=%u, minor=%u not supported", resObj.major, resObj.minor);
            continue;
        }

        level = resObj.value;
        if (level == MAX_LVL) {
            QLOGE("Resource value %x is not supported", level);
            continue;
        }

        current = GetNode(resObj);
        if (NULL == current) {
            continue;
        }

        QLOGI("Calling compareopt with level %u, currnt->level %u", level, current->resource.level);
        needAction = oh.CompareOpt(resObj.qindex, level, current->resource.level);
        if (needAction == FAILED) {
            QLOGE("Failed to apply correct action 0x%x", resObj.qindex);
            continue;
        }
        QLOGI("Need action returned as %d", needAction);

        /* Display off lock is active, pend all the coming requests*/
        if (display_off == true) {
            needAction = PEND_REQUEST;
        }

        if (needAction == ADD_NEW_REQUEST) {
            //current->level == 0
            //resource is not being used by perflock
            QLOGI("Resource not being used");
            rc = oh.ApplyOpt(resObj);
            if (rc < 0) {
                QLOGE("Failed to apply optimization [%d, %d]", resObj.major, resObj.minor);
                continue;
            }
            current->handle = req;
            current->resource = resObj;
        } else if (needAction == ADD_AND_UPDATE_REQUEST) {
            // (level > current->level) {
            /* new request is higher lvl than current */
            QLOGI("New request is higher level than current so pend");

            rc = oh.ApplyOpt(resObj);
            if (rc < 0) {
                QLOGE("Failed to apply optimization [%d, %d]", resObj.major, resObj.minor);
                continue;
            }

            pended = (struct q_node *)calloc(1, sizeof(struct q_node));
            if (pended == NULL) {
                QLOGW("Failed to pend existing request");
                continue;
            }
            CopyQnode(pended, current);

            current->handle = req;
            current->resource = resObj;
            current->next = pended;
        } else {
            /* new is equal or lower lvl than current */
            QLOGI("New request is equal or lower lvl than current");
            recent = (struct q_node *)calloc(1, sizeof(struct q_node));
            if (recent == NULL) {
                QLOGW("Failed to pend new request for optimization [%d, %d]",
                      resObj.major, resObj.minor);
                continue;
            }
            recent->handle = req;
            recent->resource = resObj;

            iter = GetNode(resObj);

            if (NULL == iter) {
                //something terribly wrong, we should not be here
                free(recent);
                continue;
            }

            if (NULL != iter->next) {
                action = oh.CompareOpt(resObj.qindex, level, iter->next->resource.level);
            }

            while ((NULL != iter->next) && (action == PEND_REQUEST)) {
                iter = iter->next;
                if (NULL == iter->next) {
                    break;
                }
                action = oh.CompareOpt(resObj.qindex, level, iter->next->resource.level);
            }

            recent->next = iter->next;
            iter->next = recent;
        }
        ret = true;
    }
    return ret;
}

bool ResourceQueue::RemoveAndApply(Request *req) {
    q_node *current = NULL, *pending = NULL, *del = NULL;
    int rc = 0, reset_opt_data = 0, i = 0;
    OptsHandler &oh = OptsHandler::getInstance();
    int needAction = FAILED;
    bool ret = false;

    if (NULL == req) {
        return false;
    }

    i = req->GetNumLocks() - 1;

    for ( ; i > -1; i--) {
        ResourceInfo *res = req->GetResource(i);

        if (NULL == res) {
            continue;
        }

        Resource &resObj = res->GetResourceObject();
        unsigned short qindex = resObj.qindex;
        res->Dump();

        current = GetNode(resObj);
        if (NULL == current) {
            //potentail applyopt failure while acquiring lock, so ignore and proceed
            //when cores present
            continue;
        }

        //is the node contains any request? if no, just ignore and continue
        if (NULL == current->handle) {
            continue;
        }

        if (req->IsResAlwaysResetOnRelease(resObj.major, resObj.minor)) {
            QLOGI("Always Reset On Remove");
            reset_opt_data = GetResetOptData(qindex, resObj.value);
            resObj.value = reset_opt_data;
            rc = oh.ResetOpt(resObj);
            if (rc < 0) {
                QLOGE("Failed to reset optimization [%d, %d]", resObj.major, resObj.minor);
            }
        }

        pending = current->next;

        if (current->handle == req) {
            if (pending != NULL) {
                needAction = oh.CompareOpt(qindex, current->resource.level, pending->resource.level);
                if (needAction == FAILED) {
                    QLOGE("Failed to find correct action for 0x%x", qindex);
                }
                if (needAction == ADD_AND_UPDATE_REQUEST) {
                    QLOGI("pending level < current->level, so apply next pending optimization");
                    //Just assign the level as it will have correct value information.
                    resObj.level = pending->resource.level;
                    rc = oh.ApplyOpt(resObj);
                    if (rc < 0) {
                        QLOGE("Failed to apply next pending optimization [%d, %d]",
                              resObj.major, resObj.minor);
                    }
                }
                del = pending;
                //TODO replace by a copy function for q_node.
                if (pending) {
                    CopyQnode(current, pending);
                }
                /*Free q_node */
                if (del) {
                    free(del);
                }
            }
            else {
                QLOGI("Reset optimization");
                reset_opt_data = GetResetOptData(qindex, resObj.value);
                resObj.value = reset_opt_data;
                rc = oh.ResetOpt(resObj);
                if (rc < 0) {
                    QLOGE("Failed to reset optimization [%d, %d]", resObj.major, resObj.minor);
                }
                current->handle = NULL;
                current->resource.level = 0;
                current->resource.qindex = UNSUPPORTED_Q_INDEX;
            }
        } else if (pending != NULL) {
            QLOGI("Removing pending requested optimization [%d, %d]",
                  resObj.major, resObj.minor);
            RemovePendingRequest(resObj, req);
        } else {
            if (qindex == UNSUPPORTED_Q_INDEX) {
                QLOGE("Release warning, resource optimization (%d, %d) not supported",
                      resObj.major, resObj.minor);
            }
        }
        ret = true;
    }
    return ret;
}

/* Remove request from pending queue */
void ResourceQueue::RemovePendingRequest(Resource &resObj, Request *req)
{
    struct q_node *del = NULL;
    struct q_node *iter = NULL;

    iter = GetNode(resObj);

    if (NULL == iter) {
        return;
    }

    while ((iter->next != NULL) && (iter->next->handle != req)) {
        iter = iter->next;
    }
    del = iter->next;
    if (del != NULL)
        iter->next = del->next;
    /* Free q_node */
    if (del) {
        free(del);
    }
}

/* This is needed because when we are given the opt code
 * we only know what optimization value to apply,
 * but not what value to reset to.
 */
int ResourceQueue::GetResetOptData(int idx, int level)
{
    int opt_data = MAX_LVL; // = reset_values[idx];

#if 0
    if ((resource >= CPU0_MIN_FREQ && resource <= CPU3_MIN_FREQ) ||
        (resource >= CPU4_MIN_FREQ && resource <= CPU7_MIN_FREQ) ||
        (resource >= CPU0_MAX_FREQ && resource <= CPU3_MAX_FREQ) ||
        (resource >= CPU4_MAX_FREQ && resource <= CPU7_MAX_FREQ)) {
        opt_data = ((resource & 0xff) << 8) | (0 & 0xff);
    } else if (resource == POWER_COLLAPSE || resource == SCREEN_PWR_CLPS) {
        opt_data = (!level) & 0xff;
    } else if (resource >= CPU0_INTERACTIVE_ABOVE_HISPEED_DELAY && resource <= CPU4_INTERACTIVE_TIMER_SLACK) {
        opt_data = ((resource & 0xff) << 8) | (opt_data & 0xff);
    } else if (resource >= CPU0_INTERACTIVE_MAX_FREQ_HYSTERESIS && resource <= CPU4_INTERACTIVE_MAX_FREQ_HYSTERESIS) {
        opt_data = ((resource & 0xff) << 8) | (opt_data & 0xff);
    } else if (resource == CLUSTR_0_MAX_CORES || resource == CLUSTR_1_MAX_CORES) {
        opt_data = ((resource & 0xff) << 8) | (opt_data & 0xff);
    } else if (resource == CLUSTR_0_CPUS_ON || resource == CLUSTR_1_CPUS_ON) {
        opt_data = ((resource & 0xff) << 8) | (opt_data & 0xff);
    }
#endif

    /* For SCHED_GROUP_OPCODE & SCHED_FREQ_AGGR_GROUP_OPCODE,*/
    /* level remains reset data*/
    if ((idx == (MAX_DISPLAY_MINOR_OPCODE + MAX_PC_MINOR_OPCODE
                       + MAX_CPUFREQ_MINOR_OPCODE + SCHED_GROUP_OPCODE)) ||
        (idx == (MAX_DISPLAY_MINOR_OPCODE + MAX_PC_MINOR_OPCODE
                       + MAX_CPUFREQ_MINOR_OPCODE + SCHED_FREQ_AGGR_GROUP_OPCODE)))
        return level;

    return opt_data;
}


void ResourceQueue::PendCurrentRequestsOp(int idx, q_node *current) {
    struct q_node *pended = NULL;
    int reset_opt_data = 0;
    int rc = 0;
    OptsHandler &oh = OptsHandler::getInstance();

    if ((NULL != current) && (NULL != current->handle)) {
        /* resource is locked, pend the current req */
        pended = (struct q_node *)calloc(1, sizeof(struct q_node));
        if (NULL == pended) {
            QLOGW("Pend all current requests, failed to pend existing request for resource=%d", idx);
            return;
        }
        CopyQnode(pended, current);
        current->next = pended;

        reset_opt_data = GetResetOptData(idx, current->resource.level);

        current->resource.value = reset_opt_data;
        //todo: better reset optdata
        rc = oh.ResetOpt(current->resource);
        if (rc == NOT_SUPPORTED) {
            QLOGW("Pend all current requests, resource=0x%X not supported", idx);
        }
        if (rc == FAILED) {
            QLOGW("Pend all current requests, failed to apply optimization for resource=0x%X", idx);
        }

        current->resource.level = 0;
        //TODO why not set handle and opcode?
    }
    return;
}

/* Iterate through all the resources, pend the current lock
 * and reset their levels but retain the current locked
 * request struct.
 */
void ResourceQueue::PendCurrentRequests()
{
    int i = 0, j = 0;
    q_node *right = NULL, *current = NULL;

    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        current = &resource_qs[i];
        //apply op on root node
        PendCurrentRequestsOp(i, current);

        //traverse nodes based on core numbers
        right = resource_qs[i].right;
        if (NULL != right) {
            for (j = 1; j < Target::getCurTarget().getTotalNumCores(); j++) {
                current = &right[j];
                PendCurrentRequestsOp(i, current);
            }
        }
    } /*for (i = 0; i < MAX_MINOR_RESOURCES; i++)*/
    return;
}

void ResourceQueue::LockCurrentStateOp(q_node *current, Request *req) {
    if (!current) {
        return;
    }
    current->handle = req;
    current->resource.level = MAX_LVL;
    //TODO should we set opcode here?
    return;
}

/* After all the requests for display off case
 * have been honored, lock the current resuorce
 * ensure display off request stays at top priority
 */
void ResourceQueue::LockCurrentState(Request *req)
{
    int i = 0, j = 0;
    q_node *right = NULL, *current = NULL;

    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        current = &resource_qs[i];
        //apply op on root node
        LockCurrentStateOp(current, req);

        //traverse nodes based on core numbers
        right = resource_qs[i].right;
        if (NULL != right) {
            for (j = 1; j < Target::getCurTarget().getTotalNumCores(); j++) {
                current = &right[j];
                LockCurrentStateOp(current, req);
            }
        }
    } /*for (i = 0; i < MAX_MINOR_RESOURCES; i++)*/
    display_off = true;
    return;
}


void ResourceQueue::UnlockCurrentStateOp(int index, q_node *current, q_node *pending, Request *req) {
    q_node *del = NULL;
    int i = 0;
    OptsHandler &oh = OptsHandler::getInstance();

    if (!current) {
        return;
    }

    if ((NULL != current->handle) && (current->handle == req)) {
        if (NULL != pending) {
            if (pending->resource.level < current->resource.level) {
                /*Current resource can be empty, as resource directly added to
                  pend list in display off case, copy the complete resource*/
                current->resource = pending->resource;
                oh.ApplyOpt(current->resource);
            }
            del = pending;
            //*current_resource = *(current_resource->next);
            if (pending) {
                CopyQnode(current, pending);
            }
            /* Free q_node */
            if (del) {
                if (del->right) {
                    free(del->right);
                    del->right = NULL;
                }
                free(del);
            }
        } else {
            current->handle = NULL;
            current->resource.level = 0;
            current->resource.qindex = UNSUPPORTED_Q_INDEX;
        }
    }
    return;
}

/* After we have released the lock on the resources
 * requested by the display off scenario, we can go
 * ahead and release all the other resources that were
 * occupied by the display off case.
 */
void ResourceQueue::UnlockCurrentState(Request *req)
{
    int i = 0, j = 0;
    q_node *current = NULL, *pending = NULL;
    q_node *right = NULL;
    display_off = false;
    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        current = &resource_qs[i];
        pending = resource_qs[i].next;

        UnlockCurrentStateOp(i, current, pending, req);

        //traverse nodes based on core numbers
        right = resource_qs[i].right;
        if (NULL != right) {
            for (j = 1; j < Target::getCurTarget().getTotalNumCores(); j++) {
                current = &right[j];
                pending = right[j].next;
                UnlockCurrentStateOp(i, current, pending, req);
            }
        }
    } /*for (i = 0; i < MAX_MINOR_RESOURCES; i++)*/
    return;
}

void ResourceQueue::Reset()
{
    int i = 0, j = 0;
    q_node *current = NULL, *next = NULL, *right = NULL;

    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        right = resource_qs[i].right;

        //traverse nodes based on core numbers
        if (NULL != right) {
            for (j = 0; j < Target::getCurTarget().getTotalNumCores(); j++) {
                current = right[j].next;
                while (NULL != current) {
                    next = current->next;
                    free(current);
                    current = next;
                }
            }
            free(right);
        }

        //traverse pended nodes
        current = resource_qs[i].next;
        while (NULL != current) {
            next = current->next;
            free(current);
            current = next;
        }
    }
}

