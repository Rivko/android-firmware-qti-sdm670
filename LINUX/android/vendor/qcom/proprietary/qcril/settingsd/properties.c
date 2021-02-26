/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdio.h>
#include "properties.h"

int property_set(const char *key, const char *value) {
    printf("setting property %s to %s\n", key, value);
    return 0;
}
