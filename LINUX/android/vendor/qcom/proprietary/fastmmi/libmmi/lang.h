/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __SYSTEM_CORE_MMI_LANG__
#define __SYSTEM_CORE_MMI_LANG__

using namespace std;

typedef struct {
    char name[NAME_MAX];
    char filepath[PATH_MAX];
} lang_t;

int load_lang(const char *lang);
const char *get_string(const char *key);
const char *get_string(string key);
#endif
