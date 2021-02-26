#ifndef QDSS_TS_MICRO_H
#define QDSS_TS_MICRO_H

/*=============================================================================

FILE:         qdss_ts_micro.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2014 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

void* qdss_ts_timer_callback_micro(void *pCtxt,
                                   uint32 unused1,void* unused2,uint32 unused3);


extern void (*pfn_qdss_ts_timer_callback)(void);

#endif //QDSS_TS_MICRO_H
