/*
 * Copyright (c) 2014,2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef MMI_MODULE_INCLUDE_H
#define MMI_MODULE_INCLUDE_H

#include "common.h"
#include "utils.h"
#include "xmlparse.h"

#define KEY_MODULE_NAME  "module"
/**
 * Name of the mmi_module_info
 */
#define MMI_MODULE_INFO_SYM         MMI
/**
 * Name of the mmi_module_info as a string
 */
#define MMI_MODULE_INFO_SYM_AS_STR  "MMI"

struct mmi_module_t;
typedef int32_t(*cb_print_t) (const char *module_name, const char *subcmd, const char *str, uint32_t size,
                              print_type_t type);

typedef struct mmi_module_methods_t {

    /** Open a specific device */
    int32_t(*module_init) (const mmi_module_t * module, unordered_map < string, string > &params);

    /** Close this device */
    int32_t(*module_deinit) (const mmi_module_t * module);

    /** Run one cmd with param */
    int32_t(*module_run) (const mmi_module_t * module, const char *cmd, unordered_map < string, string > &params);

    /** Stop the current run command */
    int32_t(*module_stop) (const mmi_module_t * module);

} mmi_module_methods_t;

/**
 * Every mmi module must have a data structure named MMI_MODULE_INFO_SYM
 * and the fields of this data structure must begin with mmi_module_t
 * followed by module specific information.
 */
typedef struct mmi_module_t {
    /**  Module version */
    uint16_t version_major;
    uint16_t version_minor;

    /** Name of module */
    const char *name;

    /** Author/owner/implementor of the module */
    const char *author;

    /** Module's methods */
    mmi_module_methods_t *methods;

    /** Module's handle */
    void *module_handle;

    /** Module's supported command list and it's size */
    const char **supported_cmd_list;
    uint32_t supported_cmd_list_size;

    /** Module's Callback function for send message to server
     *   The message will show in screen
     */
    cb_print_t cb_print;

    /** Module created Thread pid for Run command */
    pthread_t run_pid;
} mmi_module_t;

#endif                          /* MMI_MODULE_INCLUDE_H */
