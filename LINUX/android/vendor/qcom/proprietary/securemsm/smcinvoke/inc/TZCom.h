/********************************************************************
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#ifndef _TZCOM_H_
#define _TZCOM_H_

#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief daemon get root object
* This is used by Daemon to create a root IClientEnv Obj when it starts up
*
* @ param[out] rootobj: root IClientEnv Obj
* return value:  0- success; -1 - failure
*/
int TZCom_getRootEnvObject (Object *obj);

/**@brief Client get a new IClientEnv obj
*
* @param[out] obj: client's IClientEnv object
*
* return value:  0- success; -1 - failure
*/
int TZCom_getClientEnvObject (Object *obj);

#ifdef __cplusplus
}
#endif

#endif

