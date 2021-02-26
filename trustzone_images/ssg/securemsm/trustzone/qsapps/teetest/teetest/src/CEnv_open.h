// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
#ifndef __CENV_OPEN_H
#define __CENV_OPEN_H

#include "object.h"

typedef enum
{
  ENV_REMOTE = 0,
  ENV_LOCAL
}CEnv_Proximity_t;

Object CEnv_create(CEnv_Proximity_t proximity);

#endif  //__CENV_OPEN_H
