/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#ifndef __VEN_TOOL__
#define __VEN_TOOL__

//Current NL message types are COMMAND, RESPONSE and EVENT
#define NUM_NLMSG_TYPES 3
#define NLMSG_TYPE_COMMAND 0
#define NLMSG_TYPE_RESPONSE 1
#define NLMSG_TYPE_EVENT 2

#define FILE_NAME_LEN 64
#define NO_OF_ELEMENTS_IN_COMMON_DATA 2

typedef struct {
    char iface[IFACE_LEN];
    char config_file[FILE_NAME_LEN];
} common_data;

#endif
