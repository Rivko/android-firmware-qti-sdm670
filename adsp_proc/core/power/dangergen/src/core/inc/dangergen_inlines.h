/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_inlines.h
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#ifndef DANGERGEN_INLINES_H
#define DANGERGEN_INLINES_H

#include "DALSys.h"

static inline void dangergen_lock(DALSYSSyncHandle lock)
{
    DALSYS_SyncEnter(lock);
}

static inline void dangergen_unlock(DALSYSSyncHandle lock)
{
    DALSYS_SyncLeave(lock);
}

static inline bool dangergen_is_successful(dangergen_result_type result)
{
    return (result == DANGERGEN_SUCCESS);
}


#endif

