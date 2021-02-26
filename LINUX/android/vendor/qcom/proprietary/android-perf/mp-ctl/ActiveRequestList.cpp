/******************************************************************************
  @file    ActiveRequestList.cpp
  @brief   Implementation of active request lists

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015,2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>

#define LOG_TAG "ANDR-PERF-ACTIVEREQLIST"
#include <cutils/log.h>
#include <stdlib.h>

#include "Request.h"
#include "ActiveRequestList.h"
#include "MpctlUtils.h"

void ActiveRequestList::Reset() {
    RequestListNode *current = NULL;
    RequestListNode *next = NULL;

    pthread_mutex_lock(&mMutex);
    current = mActiveListHead;

    if (NULL == current) {
        QLOGI("No activity present");
        pthread_mutex_unlock(&mMutex);
        return;
    }

    while (NULL != current) {
        next = current->mNext;
        if (NULL != current->mHandle) {
            delete current->mHandle;
        }
        free(current);
        current = next;
    }
    mActiveListHead = NULL;
    mActiveReqs = 0;
    pthread_mutex_unlock(&mMutex);
}

/* Always add new request to head of active list */
int ActiveRequestList::Add(Request *req, int req_handle) {
    RequestListNode *newnode = NULL;

    newnode = (struct RequestListNode *)calloc(1, sizeof(struct RequestListNode));
    if (NULL == newnode) {
        QLOGE("Failed to create active request");
        return FAILED;
    } else {
        newnode->mHandle = req;
        newnode->mRetHandle = req_handle;

        pthread_mutex_lock(&mMutex);
        newnode->mNext = mActiveListHead;
        mActiveListHead = newnode;
        mActiveReqs++;
        pthread_mutex_unlock(&mMutex);
    }
    return SUCCESS;
}

void ActiveRequestList::Remove(Request *req) {
    struct RequestListNode *del = NULL;
    struct RequestListNode *iter = NULL;

    pthread_mutex_lock(&mMutex);

    if (mActiveListHead == NULL) {
        QLOGI("No activity to remove");
        pthread_mutex_unlock(&mMutex);
        return;
    }

    if (mActiveListHead->mHandle == req) {
        del = mActiveListHead;
        mActiveListHead = mActiveListHead->mNext;
    } else {
        iter = mActiveListHead;
        while ((iter->mNext != NULL) && (iter->mNext->mHandle != req)) {
            iter = iter->mNext;
        }
        del = iter->mNext;
        if (del != NULL)
            iter->mNext = del->mNext;
    }

    /* Free list_node */
    if (del) {
        mActiveReqs--;
        free(del);
    }
    pthread_mutex_unlock(&mMutex);
}

Request * ActiveRequestList::DoesExists(int handle) {
    RequestListNode *iter = NULL;
    Request *tmpreq = NULL;

    pthread_mutex_lock(&mMutex);
    iter = mActiveListHead;

    if (iter == NULL || handle < 1) {
        pthread_mutex_unlock(&mMutex);
        return NULL;
    }

    while ((iter->mNext != NULL) && (iter->mRetHandle != handle)) {
        iter = iter->mNext;
    }
    if (iter->mRetHandle == handle) {
        tmpreq = iter->mHandle;
    }
    pthread_mutex_unlock(&mMutex);
    return tmpreq;
}

/* Search for request in active list */
Request * ActiveRequestList::IsActive(int handle, Request *req) {
    int i = 0;
    RequestListNode *iter = NULL;
    Request *tmpreq = NULL;

    pthread_mutex_lock(&mMutex);
    iter = mActiveListHead;
    if ((iter == NULL) || (handle < 1) || (NULL == iter->mHandle) ||
        (NULL == req) || (req->GetNumLocks() < 0)) {
        pthread_mutex_unlock(&mMutex);
        return NULL;
    }

    while ((iter->mNext != NULL) && (iter->mRetHandle != handle)) {
        iter = iter->mNext;
    }
    if (iter->mRetHandle == handle) {
        if (*req != *(iter->mHandle)) {
            tmpreq = NULL;
        }
        tmpreq = iter->mHandle;
    }
    pthread_mutex_unlock(&mMutex);
    return tmpreq;
}

int ActiveRequestList::GetNumActiveRequests() {
    int tmp = 0 ;
    pthread_mutex_lock(&mMutex);
    tmp = mActiveReqs;
    pthread_mutex_unlock(&mMutex);
    return tmp;
}

int ActiveRequestList::GetActiveReqsInfo(int *handles, int *pids) {
    RequestListNode *iter = NULL;
    int i = 0;

    if ((NULL == handles) || (NULL == pids)) {
        QLOGE("Invalid data structure to ActiveRequestList");
        return 0;
    }

    pthread_mutex_lock(&mMutex);
    iter = mActiveListHead;

    QLOGI("Total no. of active requet: %d", mActiveReqs);
    while (iter != NULL) {
        if((iter->mHandle != NULL) && (i < ACTIVE_REQS_MAX)) {
            pids[i] = iter->mHandle->GetPid();
            handles[i] = iter->mRetHandle;
            i++;
        }
        iter = iter->mNext;
    }
    pthread_mutex_unlock(&mMutex);
    return i;
}

void ActiveRequestList::Dump() {
    RequestListNode *iter = NULL;

    pthread_mutex_lock(&mMutex);
    iter = mActiveListHead;

    while (iter != NULL) {
        QLOGI("request handle:%d ", iter->mRetHandle);
        iter = iter->mNext;
    }
    pthread_mutex_unlock(&mMutex);
}
