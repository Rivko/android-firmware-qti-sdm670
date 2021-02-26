/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __COMMON__
#define __COMMON__

#include <getopt.h>

#define IFACE_LEN 16
#define MAC_ADDR_LEN 6

enum option_type {
    /* CLI entry tags*/
    O_INVALID,
    O_HELP,
    O_COMMAND,
    O_END_CMD,    //applicable for command
    O_RESPONSE,
    O_END_RSP,    //applicable for response
    O_EVENT,
    O_END_EVENT,    //applicable for event
    O_NESTED,
    O_END_ATTR, //applicable only for nested attributes
    O_SWITCH,
    O_CASE,
    O_NESTED_AUTO,  //Nested attributes which have no id and to be auto numbered
    O_END_NESTED_AUTO,
    O_CMD_LAST
};

enum cmd_type {
    /* XML attribute types */
    INVALID_CMD = O_CMD_LAST+10,
    VEN_CMD,
    END_CMD,    //applicable for command
    RESPONSE,
    END_RSP,    //applicable for response
    VEN_EVENT,
    NL_EVENT,
    END_EVENT,    //applicable for event
    SWITCH,
    CASE,
    ATTRIBUTE,
    CMD_LAST
};

enum attr_type {
    /*data types below */
    INVALID_ATTR,
    U8,
    U16,
    U32,
    U64,
    S8,
    S16,
    S32,
    S64,
    STRING,
    MAC_ADDR,
    BLOB,
    NESTED,
};

extern const char* const short_options;

typedef enum {
    SUCCESS,
    MEM_NOT_AVAILABLE = -1,
    INVALID_ARG = -2,
} status;

typedef unsigned char byte;
typedef unsigned char u8;
typedef signed char s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
typedef byte mac_addr[6];
typedef byte oui[3];

struct resp_event_info {
    int num_responses;   //No.of responses to be monitored; only one response for now, but supports multiple responses handling incase if needed.
    u32 *response;
    int num_events;
    u32 *events;
    struct cmd_params *rsp;
};

union entry_type {
    enum cmd_type xml_entry;
    enum option_type ctrl_option;
};

struct default_list_t {
    int default_id;
    struct default_list_t *next;
};

union default_values {
    struct default_list_t *default_list;
    char *val;
};

#define MAX_OPTIONS 100
struct cmd_params {
    union entry_type entry[MAX_OPTIONS];
    struct option long_options[MAX_OPTIONS];
    int attr_id[MAX_OPTIONS];
    enum attr_type data_type[MAX_OPTIONS];  //Applicable only if entry_type is attribute
    int attr_max[MAX_OPTIONS];  //Applicable only for response, events and attributes
    union default_values default_val[MAX_OPTIONS];
    int num_entries;
};

struct cb_info;
struct nlIfaceInfo;

struct nlIfaceInfo *NCT_initialize();
struct nl_msg *prepareNLMsg(struct nlIfaceInfo *info, int cmdid, char *iface);
void freeNLMsg(struct nl_msg *nlmsg);
struct nlattr *startVendorData(struct nl_msg *nlmsg);
void endVendorData(struct nl_msg *nlmsg, struct nlattr *attr);
int sendNLMsg(struct nlIfaceInfo *mInfo, struct nl_msg *nlmsg,
              struct resp_event_info *resp_info);
int startmonitorResponse(struct resp_event_info *resp_info, int attr_id);
int waitForEvent();
void hexdump(void *buf, u16 len);
int startMonitorForEvent(struct resp_event_info *event_info, int attr_id);
int populateAttribute(int argc, char **argv, struct cmd_params *cmd,
                      int *option_index, int *c, struct nl_msg *nlmsg,
                      int nest_index);

int populateDefaultAttrs(int argc, char **argv, struct cmd_params *cmd,
                      int *option_index, int *c, struct nl_msg *nlmsg,
                      int nest_index);
status cleanupNLinfo(struct nlIfaceInfo *info);
#endif
