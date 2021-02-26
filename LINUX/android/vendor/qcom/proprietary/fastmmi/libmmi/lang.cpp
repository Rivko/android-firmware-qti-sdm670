/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "common.h"
#include "lang.h"
#include "xmlparse.h"

static lang_t langs[] = {
    {"en", MMI_RES_LANG_EN},
    {"zh-rCN", MMI_RES_LANG_CN},
};

static unordered_map < string, string > strings_map;

static char *get_filepath(const char *name) {

    if(name == NULL)
        return NULL;

    for(int i = 0; i < sizeof(langs) / sizeof(lang_t); i++) {
        if(!strcmp(name, langs[i].name))
            return langs[i].filepath;
    }

    return NULL;
}

int load_lang(const char *lang) {
    return parse_strings(get_filepath(lang),strings_map);
}

const char *get_string(const char *key) {
    if(key == NULL)
        return "";

    return get_string((string) key);
}
const char *get_string(string key) {
    return strings_map[key].c_str();
}
