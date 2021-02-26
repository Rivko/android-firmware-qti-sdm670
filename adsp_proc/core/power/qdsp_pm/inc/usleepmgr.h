/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#ifndef USLEEPMGR_H_
#define USLEEPMGR_H_

#define QDSPPM_USLEEP_DISALLOW	0
#define QDSPPM_USLEEP_ALLOW		1

/* Client NPA node name used to support qdspPm Usleep voting */
#define QDSPPM_USLEEP_NODE_NAME "/core/qdsppm_uSleep"
#define USLEEP_NODE_NAME "/core/uSleep"

Adsppm_Status USleepMgr_Init(void);

boolean USleepMgr_GetUSleepVoteEnabled(void);

#endif /* USLEEPMGR_H_ */

