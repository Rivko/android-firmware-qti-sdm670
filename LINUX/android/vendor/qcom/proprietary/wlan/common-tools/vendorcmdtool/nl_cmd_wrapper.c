/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <net/if.h>
#include "nl_cmd_wrapper.h"
#include <inttypes.h>

#ifndef __linux__
#include <netlink-private/object-api.h>
#include <netlink-private/types.h>
#endif

#define NCT_CMD_SOCK_PORT       777
#define NCT_EVENT_SOCK_PORT     778
static const unsigned NL80211_ATTR_MAX_INTERNAL = 256;
#define MAX_ATTR 100

static u8 get_u8(const struct nlattr *nla)
{
    return *(u8 *) nla_data(nla);
}


static u16 get_u16(const struct nlattr *nla)
{
    return *(u16 *) nla_data(nla);
}


static u32 get_u32(const struct nlattr *nla)
{
    return *(u32 *) nla_data(nla);
}


static u64 get_u64(const struct nlattr *nla)
{
    return *(u64 *) nla_data(nla);
}


static s8 get_s8( const struct nlattr *nla)
{
    return *(s8 *) nla_data(nla);
}


static s16 get_s16(const struct nlattr *nla)
{
    return *(s16 *) nla_data(nla);
}


static s32 get_s32(const struct nlattr *nla)
{
    return *(s32 *) nla_data(nla);
}


static s64 get_s64(const struct nlattr *nla)
{
    return *(s64 *) nla_data(nla);
}


static int chartohex(char c)
{
    int val = -1;
    if (c >= '0' && c <= '9')
        val = c - '0';
    else if (c >= 'a' && c <= 'f')
        val = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        val = c - 'A' + 10;

    return val;
}


static status extract_mac_addr(u8 *addr, const char *text)
{
    u8 i=0;
    int nibble;
    const char *temp = text;
    while (temp && i < MAC_ADDR_LEN) {
        nibble = chartohex(*temp++);
        if (nibble == -1)
            return INVALID_ARG;
        addr[i] = nibble << 4;
        nibble = chartohex(*temp++);
        if (nibble == -1)
            return INVALID_ARG;
        addr[i++] += nibble;
        if (*temp == ':')
            temp++;
    }
    return SUCCESS;
}


u8 get_byte(char msnibble, char lsnibble)
{
    u8 byte = 0;
    if (msnibble) {
        byte = chartohex(msnibble) * 16;
    }

    return (byte + chartohex(lsnibble));
}


int string_to_hex(char *dest, int max_len, char *src, int src_len)
{
    int i, j;
    if (src_len & 1)    // Don't process odd length hex data
        return -1;
    for (i=0, j=0; i<max_len, j<src_len; i++, j+=2) {
        dest[i] = get_byte(src[j], src[j+1]);
    }
    return 0;
}



/* Populates the given data as an attibute of the given type in nlmsg
 */
static status fillAttribute(struct cmd_params *cmd, int *c,
                            struct nl_msg *nlmsg, int isdefault)
{
    switch (cmd->data_type[*c])
    {
        case U8:
        {
            uint8_t value;
            if (isdefault)
                value = (uint8_t) atoi(cmd->default_val[*c].val);
            else
                value = (uint8_t) atoi(optarg);
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(uint8_t), &value)
                != SUCCESS) {
                printf("nla_put failed for attr: %d, data : %u\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case U16:
        {
            uint16_t value;
            if (isdefault)
                value = (uint16_t) atoi(cmd->default_val[*c].val);
            else
                value = (uint16_t) atoi(optarg);
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(uint16_t), &value)) {
                printf("nla_put failed for attr: %d, data : %u\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case U32:
        {
            uint32_t value;
            if (isdefault)
                value = (uint32_t) atoi(cmd->default_val[*c].val);
            else
                value = (uint32_t) atoi(optarg);
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(uint32_t), &value)) {
                printf("nla_put failed for attr: %d, data : %u\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case U64:
        {
            uint64_t value;
            if (isdefault)
                value = (uint64_t) atoi(cmd->default_val[*c].val);
            else
                value = (uint64_t) atoi(optarg);
            //TODO: atoi doesn't work for 64-bit types
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(uint64_t), &value)) {
                printf("nla_put failed for attr: %d, data : %"PRIu64"\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case S8:
        {
            int8_t value;
            if (isdefault)
                value = (int8_t) atoi(cmd->default_val[*c].val);
            else
                value = (int8_t) atoi(optarg);
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(int8_t), &value)) {
                printf("nla_put failed for attr: %d, data : %d\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case S16:
        {
            int16_t value;
            if (isdefault)
                value = (int16_t) atoi(cmd->default_val[*c].val);
            else
                value = (int16_t) atoi(optarg);
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(int16_t), &value)) {
                printf("nla_put failed for attr: %d, data : %d\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case S32:
        {
            int32_t value;
            if (isdefault)
                value = (int32_t) atoi(cmd->default_val[*c].val);
            else
                value = (int32_t) atoi(optarg);
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(int32_t), &value)) {
                printf("nla_put failed for attr: %d, data : %d\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case S64:
        {
            int64_t value;
            if (isdefault)
                value = (int64_t) atoi(cmd->default_val[*c].val);
            else
                value = (int64_t) atoi(optarg);
            //TODO: atoi doesn't work for 64-bit types
            if (nla_put(nlmsg, cmd->attr_id[*c], sizeof(int64_t), &value)) {
                printf("nla_put failed for attr: %d, data : %"PRId64"\n",
                       cmd->attr_id[*c], value);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case STRING:
        {
            char *value;
            if (isdefault)
                value = cmd->default_val[*c].val;
            else
                value = optarg;
            if (nla_put(nlmsg, cmd->attr_id[*c], strlen(value), value)) {
                printf("nla_put failed for attr: %d, data : %s\n",
                        cmd->attr_id[*c], optarg);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case BLOB:
        {
            char data[256];
            if (string_to_hex(&data[0], 256, optarg, strlen(optarg)) != 0) {
                printf("Not able to parse hex data for attr: %d, data : %s\n",
                        cmd->attr_id[*c], optarg);
                return INVALID_ARG;
            }
            if (nla_put(nlmsg, cmd->attr_id[*c], strlen(optarg)/2, data)) {
                printf("nla_put failed for attr: %d, data : %s\n",
                        cmd->attr_id[*c], optarg);
                return MEM_NOT_AVAILABLE;
            }
        }
        break;
        case MAC_ADDR:
        {
            u8 mac_addr[MAC_ADDR_LEN];
            char *value;
            if (isdefault)
                value = cmd->default_val[*c].val;
            else
                value = optarg;
            if (extract_mac_addr(mac_addr, value) == SUCCESS) {
                if (nla_put(nlmsg, cmd->attr_id[*c], 6, mac_addr)) {
                    printf("nla_put failed for attr: %d, data : %s\n",
                           cmd->attr_id[*c], value);
                    return MEM_NOT_AVAILABLE;
                }
            } else {
                printf("Failed to parse mac address\n");
            }
        }
        break;
        default:
           printf ("Uknown data type : %d\n", cmd->data_type[*c]);
    }
    return 0;
}


/* This API takes care of filling the attributes including nested attributes, by
 * populating them recursively
 */
int populateAttribute(int argc, char **argv, struct cmd_params *cmd,
                      int *option_index, int *c, struct nl_msg *nlmsg,
                      int nest_index)
{
    int attrindex = 0;
    struct nlattr *nlNestData = NULL;

    // Fill if it is simple data type or process if it is NESTED attribute type
    if (cmd->data_type[*c] >= U8 && cmd->data_type[*c] <= BLOB) {
        fillAttribute(cmd, c, nlmsg, 0);
    } else if (cmd->data_type[*c] == NESTED ||
               cmd->entry[*c].ctrl_option == O_NESTED_AUTO) {
        /* Nested input data found;
         * Start a new nested attribute start with id if specified.
         * start the attribute index from 0 if no input id is specified for
         * nested attribute -> NESTED_AUTO
         */

        if (cmd->entry[*c].ctrl_option == O_NESTED_AUTO)
            nlNestData = attr_start(nlmsg, nest_index);
        else
            nlNestData = attr_start(nlmsg, cmd->attr_id[*c]);

        if (!nlNestData) {
            printf("Failed to start Nested attribute\n");
            return -1;
        }
        while (1) {
            *c = getopt_long(argc, argv, short_options,
                    cmd->long_options, option_index);
            if (*c == -1 || *c > MAX_OPTIONS-1)
                break;
            if (cmd->entry[*c].ctrl_option == O_END_ATTR) {
                nest_index = 0;
                attr_end (nlmsg, nlNestData);
                break;
            }

            if (populateAttribute(argc, argv, cmd, option_index, c, nlmsg,
                                  nest_index + 1) != 0) {
                printf("Failed to fill Attributes\n");
                break;
            }
        }
    } else {
        printf("Unknown type\n");
    }
    return 0;
}


static const char* get_attr_name(struct cmd_params *response, int xmlindex)
{
    return response->long_options[xmlindex].name;
}


static int get_attr_type(struct cmd_params *response, int xmlindex)
{
    return response->data_type[xmlindex];
}


static status get_long_option_index(struct cmd_params *response, int cmd_id,
                                    enum cmd_type type)
{
    int i;
    for (i=0; i<response->num_entries; i++)
        if ((response->attr_id[i] == cmd_id) &&
            (response->entry[i].xml_entry == type))
            return response->long_options[i].val;
    return INVALID_ARG;
}


static void parseAttributeNL (struct cmd_params *response,
                              struct nlattr *tb_vendor,
                              int index)
{
    printf ("%s: ", get_attr_name(response, index));
    switch (get_attr_type(response, index) )
    {
        case U8:
            printf ("%u\n", get_u8(tb_vendor));
            break;
        case U16:
            printf ("%u\n", get_u16(tb_vendor));
            break;
        case U32:
            printf ("%u\n", get_u32(tb_vendor));
            break;
        case U64:
            printf ("%"PRIu64"\n", get_u64(tb_vendor));
            break;
        case S8:
            printf ("%d\n", get_s8(tb_vendor));
            break;
        case S16:
            printf ("%d\n", get_s16(tb_vendor));
            break;
        case S32:
            printf ("%d\n", get_s32(tb_vendor));
            break;
        case S64:
            printf ("%"PRId64"\n", get_s64(tb_vendor));
            break;
        case STRING:
            printf ("%s\n", nla_data(tb_vendor));
            break;
        case MAC_ADDR:
        {
            u8 *data = nla_data(tb_vendor);
            printf ("%02x:%02x:%02x:%02x:%02x:%02x\n",
                    data[0], data[1], data[2], data[3], data[4], data[5]);
            break;
        }
        case BLOB:
            hexdump(nla_data(tb_vendor), nla_len(tb_vendor));
            break;
        case NESTED:
            parseNested(response, tb_vendor, index);
            break;
        default:
            printf("Invalid attribute type: %d\n",
                   get_attr_type(response, index));
    }
}


static void parseNested(struct cmd_params *response, struct nlattr *tb_vendor,
                 int index)
{
    struct nlattr *attr;
    int rem;
    int i, xmlindex;

    for (attr = (struct nlattr *) nla_data(tb_vendor),
            rem = nla_len(tb_vendor);
            nla_ok(attr, rem); attr = nla_next(attr, &rem))
    {
        struct nlattr *tb2[MAX_ATTR];
        nla_parse(tb2, response->attr_max[index],
                (struct nlattr *) nla_data(attr),
                nla_len(attr), NULL);
        for (i = 0; i< response->attr_max[index]; i++) {
            if (tb2[i]) {
                xmlindex = get_long_option_index(response, i, ATTRIBUTE);
                parseAttributeNL (response, tb2[i], xmlindex);
            }
        }
    }
}


static int parseVendata(char *vendata, int datalen,
                        int cmd_id, struct cmd_params *response)
{
    struct nlattr *tb_vendor[MAX_ATTR];
    int index, i, xmlindex;

    index = get_long_option_index(response, cmd_id, RESPONSE);
    if (index<0) {
        printf("failed to get long option index\n");
        return -1;
    }

    nla_parse(tb_vendor, response->attr_max[index],
            (struct nlattr *)vendata,
            datalen, NULL);

    for (i = 0; i< response->attr_max[index]; i++) {
        if (tb_vendor[i]) {
            xmlindex = get_long_option_index(response, i, ATTRIBUTE);
            parseAttributeNL (response, tb_vendor[i], xmlindex);
        }
    }
    return 0;
}


static status set_iface_id(struct nl_msg *nlmsg, char *iface)
{
    uint32_t index = if_nametoindex(iface);
    if (nla_put(nlmsg, NL80211_ATTR_IFINDEX, sizeof(uint32_t), &index))
        return MEM_NOT_AVAILABLE;
    return SUCCESS;
}

#ifdef __linux__
#define socket_set_local_port /* no-op */
#else
static void socket_set_local_port(struct nl_sock *sock, uint32_t port)
{
    uint32_t pid;
    pid = getpid() & 0x3FFFFF;

    if (port == 0) {
        sock->s_flags &= ~NL_OWN_PORT;
    } else {
        sock->s_flags |= NL_OWN_PORT;
    }

    sock->s_local.nl_pid = pid + (port << 22);
}
#endif

static struct nl_sock * create_nl_socket(int port, int protocol)
{
    struct nl_sock *sock = nl_socket_alloc();
    if (sock == NULL) {
        printf("Failed to create NL socket\n");
        return NULL;
    }

    socket_set_local_port(sock, port);

    if (nl_connect(sock, protocol)) {
        printf("Could not connect handle\n");
        nl_socket_free(sock);
        return NULL;
    }

    return sock;
}


static int internal_valid_message_handler(struct nl_msg *msg, void *arg)
{
    printf("Valid handler called\n");
    return NL_OK;
}


/* Event handlers */
static int response_handler(struct nl_msg *msg, void *arg)
{
    int i, res;
    struct resp_event_info *resp_info = (struct resp_event_info *)arg;
    struct genlmsghdr *mHeader;
    struct nlattr *mAttributes[NL80211_ATTR_MAX_INTERNAL + 1];
    char *vendata = NULL;
    int datalen;

    mHeader = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
    int result = nla_parse(mAttributes, NL80211_ATTR_MAX_INTERNAL, genlmsg_attrdata(mHeader, 0),
          genlmsg_attrlen(mHeader, 0), NULL);

    if (mAttributes[NL80211_ATTR_VENDOR_DATA]) {
        vendata = ((char *)nla_data(mAttributes[NL80211_ATTR_VENDOR_DATA]));
        datalen = nla_len(mAttributes[NL80211_ATTR_VENDOR_DATA]);
        if (!vendata) {
            printf("Vendor data not found\n");
            return -1;
        }
    } else {
        printf("NL80211_ATTR_VENDOR_DATA not found\n");
        return -1;
    }

    if (resp_info->num_responses) {
        for (i=0; i<resp_info->num_responses; i++) {
            printf("Received response: %d\n", resp_info->response[i]);
            res = parseVendata (vendata, datalen, resp_info->response[i], resp_info->rsp);
            if (res < 0) {
                printf("Failed to parse reply message for response id = %d %d\n", resp_info->response[i], res);
            }
        }
    }

    return NL_OK;
}


int ack_handler(struct nl_msg *msg, void *arg)
{
    int *err = (int *)arg;
    *err = 0;
    return NL_STOP;
}


int finish_handler(struct nl_msg *msg, void *arg)
{
    int *ret = (int *)arg;
    *ret = 0;
    return NL_SKIP;
}


int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
    int *ret = (int *)arg;
    *ret = err->error;

    printf("error_handler received : %d\n", err->error);
    return NL_SKIP;
}


static int no_seq_check(struct nl_msg *msg, void *arg)
{
    return NL_OK;
}


struct nlIfaceInfo *NCT_initialize()
{
    struct nl_sock *cmd_sock = NULL;
    struct nl_sock *event_sock = NULL;
    struct nl_cb *cb = NULL;
    int err;

    struct nlIfaceInfo *info = (struct nlIfaceInfo *)malloc(sizeof(struct nlIfaceInfo));
    if (info == NULL) {
        printf("Could not allocate nlIfaceInfo\n");
        return NULL;
    }
    memset(info, 0, sizeof(struct nlIfaceInfo));

    cmd_sock = create_nl_socket(NCT_CMD_SOCK_PORT, NETLINK_GENERIC);
    if (cmd_sock == NULL) {
        printf("Failed to create command socket port\n");
        goto cleanup;
    }

    /* Set the socket buffer size */
    if (nl_socket_set_buffer_size(cmd_sock, (256*1024), 0) < 0) {
        printf("Could not set nl_socket RX buffer size for cmd_sock: %s\n",
                   strerror(errno));
        /* continue anyway with the default (smaller) buffer */
    }

    event_sock = create_nl_socket(NCT_EVENT_SOCK_PORT, NETLINK_GENERIC);
    if (event_sock == NULL) {
        printf("Failed to create event socket port\n");
        goto cleanup;
    }

    /* Set the socket buffer size */
    if (nl_socket_set_buffer_size(event_sock, (256*1024), 0) < 0) {
        printf("Could not set nl_socket RX buffer size for event_sock: %s\n",
                   strerror(errno));
        /* continue anyway with the default (smaller) buffer */
    }

    cb = nl_socket_get_cb(event_sock);
    if (cb == NULL) {
        printf("Failed to get NL control block for event socket port\n");
        goto cleanup;
    }

    err = 1;
    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
    nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
    nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, internal_valid_message_handler,
            info);
    nl_cb_put(cb);

    info->cmd_sock = cmd_sock;
    info->event_sock = event_sock;
    info->clean_up = 0;
    info->in_event_loop = 0;

    info->event_cb = (struct cb_info *)malloc(sizeof(struct cb_info) *
                                              DEFAULT_EVENT_CB_SIZE);
    if (info->event_cb == NULL) {
        printf("Could not allocate event_cb\n");
        goto cleanup;
    }
    info->alloc_event_cb = DEFAULT_EVENT_CB_SIZE;
    info->num_event_cb = 0;

    info->nl80211_family_id = genl_ctrl_resolve(cmd_sock, "nl80211");
    if (info->nl80211_family_id < 0) {
        printf("Could not resolve nl80211 familty id\n");
        goto cleanup;
    }

    pthread_mutex_init(&info->cb_lock, NULL);
    return info;
cleanup:
    if (cmd_sock)
        nl_socket_free(cmd_sock);
    if (event_sock)
        nl_socket_free(event_sock);
    if (info) {
        if (info->event_cb)
            free(info->event_cb);
        free(info);
    }
    return NULL;
}


static int internal_pollin_handler(struct nlIfaceInfo *info, struct nl_sock *sock)
{
    struct nl_cb *cb = nl_socket_get_cb(sock);

    int res = nl_recvmsgs(sock, cb);
    if(res)
        printf("Error :%d while reading nl msg\n", res);
    nl_cb_put(cb);
    return res;
}


static void internal_event_handler(struct nlIfaceInfo *info, int events,
                                   struct nl_sock *sock)
{
    if (events & POLLERR) {
        printf("Error reading from socket\n");
        internal_pollin_handler(info, sock);
    } else if (events & POLLHUP) {
        printf("Remote side hung up\n");
    } else if (events & POLLIN) {
        //printf("Found some events!!!");
        internal_pollin_handler(info, sock);
    } else {
        printf("Unknown event - %0x", events);
    }
}


static void internal_cleanup_handler(struct nlIfaceInfo *info)
{
    if (info->cmd_sock != 0) {
        nl_socket_free(info->cmd_sock);
        nl_socket_free(info->event_sock);
        info->cmd_sock = NULL;
        info->event_sock = NULL;
    }
}


struct nl_msg *prepareNLMsg(struct nlIfaceInfo *info, int cmdid, char *iface)
{
    int res, ifindex;
    struct nl_msg *nlmsg = nlmsg_alloc();
    uint32_t OUI = 0x001374;

    if (nlmsg == NULL) {
        printf("Out of memory\n");
        return NULL;
    }

    genlmsg_put(nlmsg, /* pid = */ 0, /* seq = */ 0, info->nl80211_family_id,
            0, 0, NL80211_CMD_VENDOR, /* version = */ 0);
    res = nla_put(nlmsg, NL80211_ATTR_VENDOR_ID, sizeof(uint32_t), &OUI);
    if (res < 0) {
        printf("Failed to put vendor id\n");
        goto cleanup;
    }

    printf("Vendor sub command: %d\n", cmdid);
    res = nla_put(nlmsg, NL80211_ATTR_VENDOR_SUBCMD, sizeof(uint32_t), &cmdid);
    if (res < 0) {
        printf("Failed to put vendor sub command\n");
        goto cleanup;
    }

    if (iface && (strlen(iface) > 0)) {
        if (set_iface_id(nlmsg, iface) != SUCCESS) {
            printf ("Failed to get iface index for iface: %s\n", iface);
            goto cleanup;
        }
    } else {
        printf("No interface provided\n");
    }

    return nlmsg;

cleanup:
    freeNLMsg(nlmsg);
    return NULL;
}


void freeNLMsg(struct nl_msg *nlmsg)
{
    if (nlmsg)
        nlmsg_free(nlmsg);
}


struct nlattr * attr_start(struct nl_msg *nlmsg, int attribute)
{
    return nla_nest_start(nlmsg, attribute);
}


void attr_end(struct nl_msg *nlmsg, struct nlattr *attr)
{
    nla_nest_end(nlmsg, attr);
}


struct nlattr *startVendorData(struct nl_msg *nlmsg)
{
    return attr_start(nlmsg, NL80211_ATTR_VENDOR_DATA);
}


void endVendorData(struct nl_msg *nlmsg, struct nlattr *attr)
{
    attr_end(nlmsg, attr);
}


int sendNLMsg(struct nlIfaceInfo *mInfo, struct nl_msg *nlmsg,
              struct resp_event_info *resp_info)
{
    int err = 0;

    struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (!cb)
        goto out;

    err = nl_send_auto_complete(mInfo->cmd_sock, nlmsg);    /* send message */
    if (err < 0)
        goto out;

    err = 1;

    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
    nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
    nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, response_handler, resp_info);

    while (err > 0) {                   /* wait for reply */
        int res = nl_recvmsgs(mInfo->cmd_sock, cb);
        if (res) {
            printf("nl80211: %s->nl_recvmsgs failed: %d\n", __FUNCTION__, res);
        }
    }
out:
    nl_cb_put(cb);
    freeNLMsg(nlmsg);
    return err;
}


status cleanupNLinfo(struct nlIfaceInfo *info)
{
    nl_socket_free(info->cmd_sock);
    nl_socket_free(info->event_sock);
    free(info->event_cb);
    free(info);
    return SUCCESS;
}


int startmonitorResponse(struct resp_event_info *resp_info, int attr_id)
{
    u32 *responses;
    if (!resp_info) {
        printf("resp_info is null\n");
        return -1;
    }

    responses = (u32 *)malloc((resp_info->num_responses + 1)*sizeof(u32));
    if (!responses) {
        printf("Failed to allocate memory for Response list\n");
        return -1;
    }

    if (resp_info->num_responses) {
        free (resp_info->response);
    }
    resp_info->response = responses;
    resp_info->response[resp_info->num_responses] = attr_id;
    resp_info->num_responses++;

    return 0;
}


int stopmonitorResponse(struct resp_event_info *resp_info, u32 attr_id)
{
    int *responses, i, j;
    if (!resp_info) {
        printf("resp_info is null\n");
        return -1;
    }

    if (1 == resp_info->num_responses) {
        free (resp_info->response);
        resp_info->num_responses = 0;
        return 0;
    }

    for (i=0; i<resp_info->num_responses; i++) {
        if (attr_id == resp_info->response[i]) {
            if (i<resp_info->num_responses-1) {
                memmove(&resp_info->response[i], &resp_info->response[i+1],
                        resp_info->num_responses-i-1);
            } else {
                resp_info->response[i] = 0;
            }
            resp_info->num_responses--;
        }
    }

    return 0;
}


int startMonitorForEvent(struct resp_event_info *event_info, int attr_id)
{
    u32 *events;
    if (!event_info) {
        printf("event_info is null\n");
        return -1;
    }

    events = (u32 *)malloc(event_info->num_events + 1);
    if (event_info->num_events) {
        free (event_info->events);
    }
    event_info->events = events;
    event_info->num_events++;

    return 0;
}


int stopMonitorForEvent(struct resp_event_info *event_info, u32 attr_id)
{
    int *events, i, j;
    if (!event_info) {
        printf("event_info is null\n");
        return -1;
    }

    if (1 == event_info->num_events) {
        free (event_info->events);
        event_info->num_events = 0;
        return 0;
    }

    for (i=0; i<event_info->num_events; i++) {
        if (attr_id == event_info->events[i]) {
            if (i<event_info->num_events-1) {
                memmove(&event_info->events[i], &event_info->events[i+1],
                        event_info->num_events-i-1);
            } else {
                event_info->events[i] = 0;
            }
            event_info->num_events--;
        }
    }

    return 0;
}


/* Run event handler */
void NCT_event_loop(struct nlIfaceInfo *info)
{
    if (info->in_event_loop) {
        return;
    } else {
        info->in_event_loop = 1;
    }

    struct pollfd pfd[2];
    memset(&pfd, 0, 2*sizeof(pfd[0]));

    pfd[0].fd = nl_socket_get_fd(info->event_sock);
    pfd[0].events = POLLIN;


    /* TODO: Add support for timeouts */

    do {
        int timeout = -1;                   /* Infinite timeout */
        pfd[0].revents = 0;
        pfd[1].revents = 0;
        //printf("Polling sockets");
        int result = poll(pfd, 2, -1);
        if (result < 0) {
            printf("Error polling socket\n");
        } else {
            if (pfd[0].revents & (POLLIN | POLLHUP | POLLERR)) {
                internal_event_handler(info, pfd[0].revents, info->event_sock);
            }
        }
    } while (!info->clean_up);


    printf("Cleaning up\n");
    //internal_cleaned_up_handler(info);
}


/* This API takes care of filling the default attributes
 */
int populateDefaultAttrs(int argc, char **argv, struct cmd_params *cmd,
                      int *option_index, int *c, struct nl_msg *nlmsg,
                      int nest_index)
{
    struct default_list_t *list = cmd->default_val[*c].default_list;
    int index, i, xmlindex;

    //get the default attr list
    while (list) {
        //Get each attr index and its typr from the table
        xmlindex = get_long_option_index(cmd, list->default_id, ATTRIBUTE);
        //Fill attribute
        fillAttribute(cmd, &xmlindex, nlmsg, 1);
        list = list->next;
    }
    return 0;
}


void hexdump(void *buf, u16 len)
{
    int i=0;
    char *bytes = (char *)buf;
    printf("******HexDump len:%d*********\n", len);
    for (i = 0; ((i + 7) < len); i+=8) {
        printf("%02x %02x %02x %02x   %02x %02x %02x %02x\n",
              bytes[i], bytes[i+1],
              bytes[i+2], bytes[i+3],
              bytes[i+4], bytes[i+5],
              bytes[i+6], bytes[i+7]);
    }
    if ((len - i) >= 4) {
        printf("%02x %02x %02x %02x\n",
              bytes[i], bytes[i+1],
              bytes[i+2], bytes[i+3]);
        i+=4;
    }
    for (;i < len;i++) {
        printf("%02x\n", bytes[i]);
    }
    printf("******HexDump End***********\n");
}
