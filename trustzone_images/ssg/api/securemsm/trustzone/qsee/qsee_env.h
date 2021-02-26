// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

#ifndef __QSEE_ENV_H
#define __QSEE_ENV_H

#include <stdint.h>
#include "object.h"

// A UID that does not identify any service.
//
#define QSEE_OPEN_INVALID_ID 0

// Request a service from the system.
//
// On success, *objOut will hold the resulting object reference and
// Object_OK will be returned.  In this case, the caller will hold a
// reference to the object and is responsible for releasing it at some
// point.
//
// Otherwise, a non-zero value defined in IOpener will be returned. The
// value of objOut will be non-deterministic in case of an error and
// the caller should not access it.
//
int32_t qsee_open(uint32_t uid, Object *objOut);


// Request a service from the system if it has not already been obtained.
//
// If the object has already been obtained, it will be returned and
// the object reference will not be retained (the caller does not need to
// call release once for each successful call).
//
// Singleton objects are persistent for the lifetime of a TA.
//
// On failure, Object_NULL is returned.
//
Object qsee_open_singleton(uint32_t classID);

// Request the Kspace object associated with the application.
//
// Returns the Kspace object and retains it, caller is expected to release.
Object qsee_get_space(void);

#endif /* __QSEE_ENV_H */
