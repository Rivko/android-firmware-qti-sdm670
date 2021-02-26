/******************************************************************************
  @file    ActiveRequestList.h
  @brief   Implementation of active request lists

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __ACTIVEREQUESTLIST__H_
#define __ACTIVEREQUESTLIST__H_

class Request;

typedef struct RequestListNode {
    int mRetHandle;
    Request *mHandle;
    RequestListNode *mNext;
}RequestListNode;

class ActiveRequestList {
    private:
        RequestListNode *mActiveListHead;
        RequestListNode *mIter;
        int mActiveReqs;
        pthread_mutex_t mMutex;

    public:
        static const int ACTIVE_REQS_MAX = 30;

        ActiveRequestList() {
            mActiveListHead = NULL;
            mIter = NULL;
            mActiveReqs = 0;
            pthread_mutex_init(&mMutex, NULL);
        };
        ~ActiveRequestList() {
            pthread_mutex_destroy(&mMutex);
        };

        int Add(Request *req, int req_handle);
        void Remove(Request *req);
        void Reset();
        Request * DoesExists(int handle);
        Request * IsActive( int handle, Request *req);
        int GetNumActiveRequests();
        int GetActiveReqsInfo(int *handles, int *pids);
        void Dump();
};

#endif
