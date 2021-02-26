/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include "common.h"
#include "ven_cmd_tool.h"

const char* const short_options = "hi:f:";

static enum attr_type getAttrTypeenum(char *value)
{
    enum attr_type type = INVALID_ATTR;
    if(!strcasecmp(value, "u8")) {
        type = U8;
    } else if(!strcasecmp(value, "u16")) {
        type = U16;
    } else if(!strcasecmp(value, "u32")) {
        type = U32;
    } else if(!strcasecmp(value, "u64")) {
        type = U64;
    } else if(!strcasecmp(value, "s8")) {
        type = S8;
    } else if(!strcasecmp(value, "s16")) {
        type = S16;
    } else if(!strcasecmp(value, "s32")) {
        type = S32;
    } else if(!strcasecmp(value, "s64")) {
        type = S64;
    } else if(!strcasecmp(value, "string")) {
        type = STRING;
    } else if(!strcasecmp(value, "mac_addr")) {
        type = MAC_ADDR;
    } else if(!strcasecmp(value, "blob")) {
        type = BLOB;
    } else if(!strcasecmp(value, "nested")) {
        type = NESTED;
    } else {
        printf("unknown attr type : %s\n", value);
    }

    return type;
}


static enum cmd_type getCmdType(char *value)
{
    enum cmd_type type = INVALID_CMD;

    if(!strcasecmp(value, "VendorCmd")) {
        type = VEN_CMD;
    } else if(!strcasecmp(value, "VendorRsp")) {
        type = RESPONSE;
    } else if(!strcasecmp(value, "VendorEvent")) {
        type = VEN_EVENT;
    } else if(!strcasecmp(value, "NLEvent")) {
        type = NL_EVENT;
    } else if(!strcasecmp(value, "switch")) {
        type = SWITCH;
    } else if(!strcasecmp(value, "case")) {
        type = CASE;
    } else if(!strcasecmp(value, "attribute")) {
        type = ATTRIBUTE;
    } else {
        printf("unknown cmd type : %s\n", value);
    }

    return type;
}


static void parseDefaultAttrList(union default_values *default_val, const char *data)
{
    struct default_list_t *node=NULL, *list = NULL;
    int i=0;
    char *values, *id, delim[2] = {',', 0};
    char *saveptr;

    values = (char *)malloc(strlen(data) + 1);
    if (!values) {
        printf("Failed to allocate memory for values\n");
        return;
    }
    memcpy(values, data, strlen(data)+1);

    printf("Default attributes: %s\n", values);
    id = strtok_r(values, delim, &saveptr);
    while (id) {
        node = (struct default_list_t *)malloc(sizeof(struct default_list_t));
        if (!node) {
            printf("Failed to allocate memory for node\n");
            return;
        }
        node->default_id = atoi(id);
        node->next = list;
        list = node;
        id = strtok_r(NULL, delim, &saveptr);
    }

    default_val->default_list = list;

    free(values);
}


static void parseAttribute(xmlNode *nodeptr, struct cmd_params *cmd)
{
    xmlNode *node = nodeptr;
    xmlAttrPtr attr;
    int i = cmd->num_entries;

    cmd->entry[i].xml_entry = getCmdType((char *)node->name);
    attr = node->properties;

    while(attr != NULL) {
        char *value = (char *) xmlGetProp(node, (const xmlChar *) attr->name);

        if(value != NULL) {
            if(!xmlStrcmp(attr->name, (const xmlChar *) "name")) {
                if (!value)
                    return;
                cmd->long_options[i].name = (char *)malloc(strlen(value)+1);
                if (!cmd->long_options[i].name) {
                    printf("Failed to allocate memory for name\n");
                }
                memcpy((char *)cmd->long_options[i].name, value,
                       strlen(value)+1);
                cmd->long_options[i].val = i;
            } else if(!xmlStrcmp(attr->name, (const xmlChar *) "ID")) {
                cmd->attr_id[i] = atoi(value);
            } else if(!xmlStrcmp(attr->name, (const xmlChar *) "TYPE")) {
                cmd->data_type[i] = getAttrTypeenum(value);
                if (cmd->data_type[i] >= U8 && cmd->data_type[i] <= BLOB)
                    cmd->long_options[i].has_arg = 1;
            } else if(!xmlStrcmp(attr->name, (const xmlChar *) "DEFAULTS")) {
                parseDefaultAttrList(&cmd->default_val[i], value);
            } else if(!xmlStrcmp(attr->name, (const xmlChar *) "DEFAULT")) {
                cmd->default_val[i].val = (char *)malloc(strlen(value)+1);
                if (!cmd->default_val[i].val) {
                    printf("Failed to allocate memory for default val\n");
                }
                memcpy((char *)cmd->default_val[i].val, value, strlen(value)+1);
            } else if(!xmlStrcmp(attr->name, (const xmlChar *) "ATTR_MAX")) {
                cmd->attr_max[i] = atoi(value);
            }
            xmlFree(value);
        }
        attr = attr->next;
    }

    cmd->num_entries++;
}


static xmlNode *getValidNode(xmlNode *node)
{
    xmlNode *nextNode;
    if (!node) {
        return NULL;
    }
    nextNode = node;

    while (nextNode->next &&
            !xmlStrcmp(nextNode->name, (const xmlChar *) "text")) {
        nextNode = nextNode->next;
    }
    if (!xmlStrcmp(nextNode->name, (const xmlChar *) "text"))
        nextNode = NULL;

    return nextNode;
}


static xmlNode *getNextNode(xmlNode *node)
{
    xmlNode *nextNode;
    if (!node) {
        return NULL;
    }
    nextNode = getValidNode(node);
    if (!nextNode) {
        return NULL;
    }
    nextNode = nextNode->next;
    nextNode = getValidNode(nextNode);

    return nextNode;
}


static xmlNode *getChildNode(xmlNode *node)
{
    xmlNode *nextNode;
    if (!node) {
        return NULL;
    }
    nextNode = getValidNode(node);
    nextNode = node->xmlChildrenNode;
    nextNode = getValidNode(nextNode);
    /*Value present at next node*/
    if (!nextNode) {
        nextNode = getNextNode(node);
    }
    return nextNode;
}


static xmlNode *getParentNode(xmlNode *node)
{
    xmlNode *nextNode;;
    if (!node) {
        return NULL;
    }

    nextNode = node->parent;
    nextNode = getValidNode(nextNode);
    return nextNode;
}


static xmlNode *parseRsp(xmlNode *node, struct cmd_params *response)
{
    xmlNode *nextNode, *childNode, *tempNode;
    if (!node) {
        return NULL;
    }
    nextNode = node;

    do {
        parseAttribute(nextNode, response);
        childNode = getChildNode(nextNode);
        if (childNode) {
            nextNode = childNode;
        } else {
            tempNode = getNextNode(nextNode);
            if (tempNode) {
                nextNode = tempNode;
            } else {
                do {
                    nextNode = getParentNode(nextNode);
                    if (!nextNode)
                        return NULL;
                    if (nextNode == node) {
                        return getNextNode(nextNode);
                    }
                    tempNode = getNextNode(nextNode);
                    if (tempNode) {
                        nextNode = tempNode;
                    }
                } while (!tempNode);
            }
        }
        nextNode = getValidNode(nextNode);
    } while (nextNode);
    return nextNode;
}


static xmlNode *parseCmd(xmlNode *node, struct cmd_params *cmd[], int num)
{
    xmlNode *nextNode, *childNode, *tempNode;
    if (!node || num < 2) {
        return NULL;
    }
    nextNode = node;

    do {
        parseAttribute(nextNode, cmd[0]);
        childNode = getChildNode(nextNode);
        if (childNode) {
            nextNode = childNode;
        } else {
            tempNode = getNextNode(nextNode);
            if (tempNode) {
                nextNode = tempNode;
            } else {
                do {
                    nextNode = getParentNode(nextNode);
                    if (!nextNode)
                        return NULL;
                    if (nextNode == node) {
                        return getNextNode(nextNode);
                    }
                    tempNode = getNextNode(nextNode);
                    if (tempNode) {
                        nextNode = tempNode;
                    }
                } while (!tempNode);
            }
        }
        nextNode = getValidNode(nextNode);
        if (!nextNode)
            return NULL;
        if (!xmlStrcmp(nextNode->name, (const xmlChar *) "VendorRsp")) {
            tempNode = parseRsp(nextNode, cmd[1]);
            if (!tempNode) {
                nextNode = getParentNode(nextNode);
                nextNode = getNextNode(nextNode);
            } else {
                nextNode = tempNode;
            }
        }
    } while (nextNode);
    return nextNode;
}


//tuple: Parse the XML doc for Node if specified by the caller.
//      If it is NULL, parse for VendorCmd
static void parseTree(xmlNode *node, struct cmd_params *cmds[], int num, char *tuple)
{
    xmlNode *nextNode;
    if (!node) {
        return;
    }

    nextNode = getValidNode(node);

    while (nextNode) {
        if(!xmlStrcmp(nextNode->name, (const xmlChar *) "VendorCmd")) {
            if (tuple) {
                xmlAttrPtr attr = nextNode->properties;

                while(attr != NULL) {
                    char *value = (char *) xmlGetProp(nextNode, (const xmlChar *) attr->name);

                    if(value != NULL) {
                        if(!xmlStrcmp(attr->name, (const xmlChar *) "name")) {
                            if (!strncmp(value, tuple, strlen(tuple))) {
                                nextNode = parseCmd(nextNode, cmds, num);
                                return;
                            }
                            break;
                        }
                    }
                }
            } else {
                parseAttribute(nextNode, cmds[0]);
            }
            nextNode = getNextNode(nextNode);
        } else if(!xmlStrcmp(nextNode->name, (const xmlChar *) "VendorEvent")) {
            //        parseEvent(nextNode);
        } else {
            printf("Invalid Entry %s:%d\n", __func__, __LINE__);
        }
    }
}


static char *enumToString(enum option_type type)
{
    char *cmdtype;
    switch (type)
    {
        case O_HELP:
            cmdtype = "HELP";
            break;
        case O_COMMAND:
            cmdtype = "START_CMD";
            break;
        case O_END_CMD:
            cmdtype = "END_CMD";
            break;
        case O_RESPONSE:
            cmdtype = "RESPONSE";
            break;
        case O_END_RSP:
            cmdtype = "END_RSP";
            break;
        case O_EVENT:
            cmdtype = "EVENT";
            break;
        case O_END_EVENT:
            cmdtype = "END_EVENT";
            break;
        case O_NESTED:
            cmdtype = "NESTED";
            break;
        case O_SWITCH:
            cmdtype = "SWITCH";
            break;
        case O_CASE:
            cmdtype = "CASE";
            break;
        case O_NESTED_AUTO:
            cmdtype = "NESTED_AUTO";
            break;
        case O_END_ATTR:
            cmdtype = "END_ATTR";
            break;
        case O_END_NESTED_AUTO:
            cmdtype = "END_NESTED_AUTO";
            break;
        default:
            printf("invalid enum value : %d\n", type);
            cmdtype = NULL;
    }
    return cmdtype;
}


static void populateCLIOptions(struct cmd_params *cmd)
{
    enum option_type j;
    char *cmdname;

    for (j=O_HELP; j< O_CMD_LAST; j++, cmd->num_entries++) {
        cmd->entry[cmd->num_entries].ctrl_option = j;

        cmdname = enumToString(j);
        if (!cmdname) {
            printf("command name not found option_type : %d\n", j);
            return;
        }
        cmd->long_options[cmd->num_entries].name =
                                            (char *)malloc(strlen(cmdname)+1);
        memcpy((char *)cmd->long_options[cmd->num_entries].name,
               cmdname,
               strlen(cmdname)+1);
        cmd->long_options[cmd->num_entries].val = cmd->num_entries;
        /* No need to populate other elements for now */
    }
}


static status parseXMLDoc(char *config_file, struct cmd_params *cmds[], int num, char *tuple)
{
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    struct cmd_params *cmd;

    if (num < 2)
        return INVALID_ARG;
    cmd = cmds[0];

    doc = xmlReadFile(config_file, "UTF-8", XML_PARSE_RECOVER);
    if(doc == NULL) {
        printf("%s parse failed\n", config_file);
        return INVALID_ARG;
    }
    node = xmlDocGetRootElement(doc);
    if(node == NULL) {
        printf("%s empty\n", config_file);
        xmlFreeDoc(doc);
        return INVALID_ARG;
    }

    if(xmlStrcmp(node->name, (const xmlChar *) "WCN_VenTool")) {
        printf("%s not a Ven xml\n", config_file);
        xmlFreeDoc(doc);
        return INVALID_ARG;
    }

    node = node->xmlChildrenNode;

    populateCLIOptions(cmd);
    parseTree(node, cmds, num, tuple);

    cmd->long_options[cmd->num_entries].name = NULL;
    cmd->long_options[cmd->num_entries].has_arg = 0;
    cmd->long_options[cmd->num_entries].flag = 0;
    cmd->long_options[cmd->num_entries].val = 0;

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return SUCCESS;
}


/* Parse command inputs;
 * This API expects only long options as described in the
 * xml file. Any inputs out of xml file will be discarded.
 */

static void parseCmdinputs(struct nlIfaceInfo *info, int argc, char **argv,
                    struct cmd_params *cmd, int *option_index,
                    common_data data)
{
    int c, ret;
    struct nl_msg *nlmsg;
    struct nlattr *nlVenData = NULL;
    struct cmd_params command, response, event;
    struct cmd_params *cur_cmds[NUM_NLMSG_TYPES] = {&command, &response, &event};
    struct resp_event_info resp_info;

    memset(&command, 0, sizeof(struct cmd_params));
    memset(&response, 0, sizeof(struct cmd_params));
    memset(&event, 0, sizeof(struct cmd_params));
    memset(&resp_info, 0, sizeof(struct resp_event_info));

    // Use this response attributes while parsing the response
    resp_info.rsp = &response;

    // Parse for command id; This is the first input in the command params
    c = getopt_long(argc, argv, short_options,
            cmd->long_options, option_index);
    if (c == -1 || c > MAX_OPTIONS-1)
        return;

    if (c > cmd->num_entries || cmd->entry[c].xml_entry != VEN_CMD) {
        printf("Command not present: c = %d, entry = %d\n",
               c, cmd->entry[c].xml_entry);
        return;
    }

    if (parseXMLDoc(data.config_file, cur_cmds, NUM_NLMSG_TYPES, (char *)cmd->long_options[c].name) != SUCCESS) {
        printf ("Failed to parse the file : %s\n", argv[1]);
        return;
    }

    c = O_CMD_LAST-1;

    // command.attr_id[c] carries the vendor command number
    nlmsg = prepareNLMsg(info, command.attr_id[c], data.iface);
    if (!nlmsg) {
        printf("Failed to prepare nlmsg\n");
        return;
    }

    nlVenData = startVendorData(nlmsg);
    if (!nlVenData)
        printf("failed to start vendor data\n");

    populateDefaultAttrs(argc, argv, &command, option_index, &c, nlmsg, 0);
    // Parse for each input and populate it accordingly
    while (1) {
        c = getopt_long(argc, argv, short_options,
                        command.long_options, option_index);
        if (c == -1 || c > MAX_OPTIONS-1)
            break;
        if (command.entry[c].xml_entry == ATTRIBUTE) {
            if (populateAttribute(argc, argv, &command, option_index, &c, nlmsg, 0)
                != 0) {
                printf("Failed to fill Attributes\n");
                break;
            }
        } else if (command.entry[c].ctrl_option == O_END_CMD) {
            if (nlVenData)
                endVendorData(nlmsg, nlVenData);
            ret = sendNLMsg(info, nlmsg, &resp_info);
            if (ret != 0) {
                printf("Failed to send message to driver Error:%d\n", ret);
                break;
            }
        } else if (command.entry[c].ctrl_option == O_RESPONSE) {
            c = getopt_long(argc, argv, short_options,
                    command.long_options, option_index);
            if (c == -1 || c > MAX_OPTIONS-1)
                break;
            if (command.attr_id[c]) {
                printf("Monitor for Response : %d\n", command.attr_id[c]);
                startmonitorResponse(&resp_info, command.attr_id[c]);
            }
        } else {
            printf("Unknown CLI option\n");
        }
    }
    free(resp_info.response);
}


/* General tendency is to provide common data first and then
 * command specific data. This API expects iface and file inputs in
 * any order and then command specific data
 */
static status get_common_data(common_data *data,
                       int argc,
                       char** argv,
                       const char* short_options,
                       struct option *long_options)
{
    int data_read = 0; //No.of elements to be read as common data
    while (1) {
        int option_index = 0, c;
        unsigned int arg_len;

        c = getopt_long(argc, argv, short_options,
                long_options, &option_index);
        if (c == -1)
            break;

        arg_len = optarg ? strlen(optarg) : 0;
        switch(c) {
            case 'i':
                if (!arg_len || arg_len >= IFACE_LEN) {
                    printf("iface name too long or empty\n");
                    return INVALID_ARG;
                }

                memcpy(&data->iface[0], optarg, arg_len);
                data->iface[arg_len] = '\0';
                data_read++;
            break;
            case 'f':
                if (!arg_len || arg_len >= FILE_NAME_LEN) {
                    printf("file name too long or empty\n");
                    return INVALID_ARG;
                }

                memcpy(&data->config_file[0], optarg, arg_len);
                data->config_file[arg_len] = '\0';
                data_read++;
            break;
        }
        if (data_read == NO_OF_ELEMENTS_IN_COMMON_DATA) {
            break;
        }
    }

    if (strlen(data->iface) == 0) {
        printf ("Failed to get iface\n");
        return INVALID_ARG;
    }

    if (strlen(data->config_file) == 0) {
        printf ("Failed to get input file\n");
        return INVALID_ARG;
    }

    return SUCCESS;
}


static status dissolveLongOptions(struct cmd_params *cmds[], int num)
{
    int i, j;
    struct cmd_params *cmd;

    for (i=0; i<num; i++) {
        cmd = cmds[i];
        for (j=0; j<cmd->num_entries;j++) {
            free((void *)cmd->long_options[j].name);
        }
    }

    return SUCCESS;
}


int main(int argc, char **argv)
{
    struct nlIfaceInfo *info;
    struct cmd_params command, response, event;
    struct cmd_params *cmds[NUM_NLMSG_TYPES] = {&command, &response, &event};
    common_data data;

    memset(&command, 0, sizeof(struct cmd_params));
    memset(&response, 0, sizeof(struct cmd_params));
    memset(&event, 0, sizeof(struct cmd_params));

    memset(&data, 0, sizeof(common_data));

    if (get_common_data(&data, argc, argv, short_options,
                        cmds[NLMSG_TYPE_COMMAND]->long_options) != SUCCESS) {
        printf ("Failed to get common data\n");
        return INVALID_ARG;
    }

    if (parseXMLDoc(data.config_file, cmds, NUM_NLMSG_TYPES, NULL) != SUCCESS) {
        printf ("Failed to parse the file : %s\n", argv[1]);
        return INVALID_ARG;
    }

    info = NCT_initialize();
    if (info == NULL) {
        printf ("Failed to initialize sockets\n");
        return INVALID_ARG;
    }

    while (1) {
        int option_index = 0, c;

        //Parse for command/event params from CLI
        c = getopt_long(argc, argv, short_options,
                cmds[NLMSG_TYPE_COMMAND]->long_options, &option_index);
        if (c == -1 || c > MAX_OPTIONS-1)
            break;

        if (c < cmds[NLMSG_TYPE_COMMAND]->num_entries) {
            if (cmds[NLMSG_TYPE_COMMAND]->entry[c].ctrl_option == O_COMMAND) {
                parseCmdinputs(info, argc, argv, cmds[NLMSG_TYPE_COMMAND],
                               &option_index, data);
            } else
                printf("Unknown Command : %d\n",
                       cmds[NLMSG_TYPE_COMMAND]->entry[c].ctrl_option);
        } else
            printf("getopt returned character code %d \n", c);
    }

    dissolveLongOptions(cmds, 3);
    cleanupNLinfo(info);
    printf("\n\nCleaned up; Exiting...\n");

    return 0;
}
