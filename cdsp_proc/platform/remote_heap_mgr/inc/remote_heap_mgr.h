/*==========================================================================

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

===========================================================================*/

#ifndef REMOTE_HEAP_MGR_H
#define REMOTE_HEAP_MGR_H

#define MAX_RETRIES 10

enum remote_heap_mgr_cmd
{
    INIT_REMOTEHEAP = 0,
    GROW_HEAP,
    SHRINK_HEAP
};

struct remote_heap_mgr_work_item
{
    enum remote_heap_mgr_cmd	cmd;
    union {
    	void *heap_cb_ptr;
        void *heap_ptr;
        void *vaddr;
    } ptr;

    int	size;           /*  Unused for growth. (To be used when we
                            allow custom growth sizes)
                            Set to heap size while shrinking */
  int sleep_timer;    /*  Timer set in case of retry. In milliseconds */
};

int remote_heap_mgr_enqueue_cmd(struct remote_heap_mgr_work_item*);
int remote_heap_mgr_handle_failure_cb_handler(void* heap_ptr,
        size_t request_size, size_t default_grow_size);

#endif /*REMOTE_HEAP_MGR_H*/
