/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include "common.h"
#include "xmlparse.h"
#include "utils.h"
#include "board.h"
#include "mmi_cfg.h"

// module item name
static const char *module_item_name[] = {
    XML_NODE_ATTR_LIB_NAME,
    XML_NODE_ATTR_ENABLE,
    XML_NODE_ATTR_AUTOMATION,
    XML_NODE_ATTR_DISPLAY,
    XML_NODE_ATTR_LAYOUT,
    XML_NODE_ATTR_PARAMETER,
    XML_NODE_ATTR_LIMIT,
    NULL
};

// mmi module item name
static const char *mmi_module_item_name[] = {
    XML_NODE_ATTR_DB_ENABLE,
    XML_NODE_ATTR_SYS_BL,
    XML_NODE_ATTR_MISC_DEV,
    XML_NODE_ATTR_LANGUAGE,
    XML_NODE_ATTR_FONT_SIZE,
    XML_NODE_ATTR_TEST_MODE,
    XML_NODE_ATTR_LAYOUT,
    XML_NODE_ATTR_PER_PAGE_ITEMS,
    XML_NODE_ATTR_HOME_SCREENS,
    XML_NODE_ATTR_AUTORUN_ENABLE,
    XML_NODE_ATTR_SKIP_AUTORUN_PASSED,
    NULL
};

static char mmi_cfg_file[64];
static char mmi_pcba_cfg_file[64];

static bool find_attr_by_name(xmlNodePtr node, node_arrt_t * attr_info) {
    xmlAttrPtr attr;

    if(NULL == node || NULL == attr_info) {
        ALOGE("Invalid parameter");
        return false;
    }

    attr = node->properties;
    while(attr != NULL) {
        char *value = (char *) xmlGetProp(node, (const xmlChar *) attr->name);

        if(value != NULL) {
            if(!xmlStrcmp(attr->name, (xmlChar *) attr_info->arrt_name) && !strcasecmp(value, attr_info->arrt_val)) {
                xmlFree(value);
                return true;
            } else {
                xmlFree(value);
            }
        }
        attr = attr->next;
    }

    return false;
}

static xmlNodePtr find_node(xmlDocPtr doc, find_node_key_t * key) {
    bool is_find = false;
    xmlNodePtr cur_node = NULL;

    if(NULL == doc || NULL == key || NULL == key->node_name) {
        ALOGE("Invalid parameter");
        return NULL;
    }

    cur_node = xmlDocGetRootElement(doc);
    if(cur_node == NULL) {
        ALOGE("Not find root node");
        return NULL;
    }

    cur_node = cur_node->xmlChildrenNode;
    while(NULL != cur_node) {
        if(!xmlStrcmp(cur_node->name, (xmlChar *) key->node_name)) {
            is_find = true;

            if(NULL != key->node_arrt1.arrt_name && !find_attr_by_name(cur_node, &(key->node_arrt1))) {
                is_find = false;
            }

            if(NULL != key->node_arrt2.arrt_name && !find_attr_by_name(cur_node, &(key->node_arrt2))) {
                is_find = false;
            }

            if(NULL != key->node_arrt3.arrt_name && !find_attr_by_name(cur_node, &(key->node_arrt3))) {
                is_find = false;
            }

            if(is_find) {
                break;
            }
        }
        cur_node = cur_node->next;
    }
    return cur_node;
}

static xmlNodePtr find_node(xmlNodePtr node, find_node_key_t * key) {
    bool is_find = false;
    xmlNodePtr cur_node = NULL;

    if(NULL == node || NULL == key || NULL == key->node_name) {
        ALOGE("Invalid parameter");
        return NULL;
    }

    cur_node = node->xmlChildrenNode;
    while(NULL != cur_node) {
        if(!xmlStrcmp(cur_node->name, (xmlChar *) key->node_name)) {
            is_find = true;

            if(NULL != key->node_arrt1.arrt_name && !find_attr_by_name(cur_node, &(key->node_arrt1))) {
                is_find = false;
            }

            if(NULL != key->node_arrt2.arrt_name && !find_attr_by_name(cur_node, &(key->node_arrt2))) {
                is_find = false;
            }

            if(NULL != key->node_arrt3.arrt_name && !find_attr_by_name(cur_node, &(key->node_arrt3))) {
                is_find = false;
            }

            if(is_find) {
                break;
            }
        }
        cur_node = cur_node->next;
    }
    return cur_node;
}

static bool is_valid_module_name(const char *module_name, const char *attr_name) {
    const char **item_name = NULL;
    string str_module_name = string(module_name);
    string str_arrt_name = string(attr_name);

    if(str_module_name.empty() || str_arrt_name.empty()) {
        ALOGE("module name is empty");
        return false;
    }
    // not [MMI] module
    if(str_module_name.compare(MODULE_NAME_MMI))
        item_name = module_item_name;
    // [MMI] module
    else
        item_name = mmi_module_item_name;

    while(NULL != *item_name) {
        if(!str_arrt_name.compare(*item_name))
            return true;
        item_name++;
    }

    return false;
}

static cfg_module_info *alloc_memory_for_module(const char *module_name) {
    cfg_module_info *addr = NULL;

    addr = new cfg_module_info((char *) module_name);
    if(NULL == addr) {
        ALOGE("new object fail");
        return NULL;
    }
    return addr;
}

static int parse_module_info(xmlNodePtr first_node, list < cfg_module_info * >*ordered_module,
                             unordered_map < string, cfg_module_info * >*module_map) {
    xmlNodePtr cur_node = NULL;
    xmlAttrPtr attr = NULL;
    cfg_module_info *module_node = NULL;
    char module_name[64];

    if(NULL == first_node || NULL == ordered_module || NULL == module_map) {
        ALOGE("Invalid parameter");
        return FAILED;
    }

    cur_node = first_node;
    while(cur_node != NULL) {
        if(xmlStrcmp(cur_node->name, (const xmlChar *) XML_NODE_NAME_MODULE)) {
            ALOGE("Invalid node name: %s", cur_node->name);
            cur_node = cur_node->next;
            continue;
        }
        memset(module_name, 0x00, sizeof(module_name));

        attr = cur_node->properties;
        while(attr != NULL) {
            char *name = (char *) attr->name;
            char *value = (char *) xmlGetProp(cur_node, (const xmlChar *) attr->name);

            if(value != NULL) {
                if(!xmlStrcmp(attr->name, (const xmlChar *) XML_NODE_ATTR_NAME)) {  // module name
                    snprintf(module_name, sizeof(module_name), "%s", value);

                    if(NULL == (*module_map)[(string) value]) {
                        module_node = alloc_memory_for_module(module_name);
                        if(NULL == module_node) {
                            xmlFree(value);
                            return FAILED;
                        }
                        (*module_map)[(string) value] = module_node;
                        ordered_module->push_back(module_node);
                    }
                } else {
                    if(is_valid_module_name(module_name, name)) {
                        if(NULL != module_node) {
                            module_node->config_list[(string) name] = (string) value;
                        } else {
                            ALOGE("Not found valid module name(<module name=\"example\">) in node");
                        }
                    } else {
                        ALOGE("Invalid module name(%s) in module:[%s]", (char *) attr->name, module_name);
                    }
                }
                xmlFree(value);
            }
            attr = attr->next;
        }
        cur_node = cur_node->next;
    }
    return SUCCESS;
}

static int update_module_info(xmlNodePtr first_node, unordered_map < string, cfg_module_info * >*module) {
    xmlNodePtr cur_node = NULL;
    xmlAttrPtr attr = NULL;
    char module_name[64];

    if(NULL == first_node || NULL == module) {
        ALOGE("Invalid parameter");
        return FAILED;
    }

    cur_node = first_node;
    while(cur_node != NULL) {
        if(xmlStrcmp(cur_node->name, (const xmlChar *) XML_NODE_NAME_MODULE)) {
            ALOGE("Invalid node name: %s", cur_node->name);
            cur_node = cur_node->next;
            continue;
        }
        memset(module_name, 0x00, sizeof(module_name));

        attr = cur_node->properties;
        while(attr != NULL) {
            char *name = (char *) attr->name;
            char *value = (char *) xmlGetProp(cur_node, (const xmlChar *) attr->name);

            if(value != NULL) {
                if(!xmlStrcmp(attr->name, (const xmlChar *) XML_NODE_ATTR_NAME)) {  // module name
                    snprintf(module_name, sizeof(module_name), "%s", value);
                } else {
                    if(is_valid_module_name(module_name, name)) {
                        if(NULL != (*module)[(string) module_name]) {
                            (*module)[(string) module_name]->config_list[(string) name] = (string) value;
                        } else {
                            ALOGE("Update module fail, error=Not found [%s] in default module", module_name);
                        }
                    } else {
                        ALOGE("Invalid module name: %s", (char *) attr->name);
                    }
                }
                xmlFree(value);
            }
            attr = attr->next;
        }
        cur_node = cur_node->next;
    }
    return SUCCESS;
}

static bool is_xml_variable(char *var) {
    return ('$' == *var) ? true : false;
}

static char *get_xml_variable_name(char *var, char *ret, int ret_buf_size) {
    char *start = NULL;
    char *end = NULL;

    memset(ret, 0x00, ret_buf_size);

    if(NULL == (start = strstr(var, "{"))) {
        ALOGE("Invalid xml variable reference");
        return NULL;
    }
    start++;
    if(NULL == (end = strstr(start, "}"))) {
        ALOGE("Invalid xml variable reference");
        return NULL;
    }
    end--;

    while(start != end && (0x20 == *start || 0x09 == *start)) {
        start++;
    }
    while(start != end && (0x20 == *end || 0x09 == *end)) {
        end--;
    }
    snprintf(ret, (end - start + 2) > ret_buf_size ? ret_buf_size : (end - start + 2), "%s", start);
    ALOGI("Get xml variable success, variable name=%s", ret);

    return ret;
}

static char *get_xml_variable_addr(xmlDocPtr doc, char *node_name, char *var_name) {
    find_node_key_t key;
    char *value = NULL;
    xmlNodePtr node = NULL;

    if(NULL == doc || NULL == node_name || NULL == var_name) {
        ALOGE("Invalid parameter");
        return NULL;
    }

    SET_NODE_NAME(key, node_name);
    SET_ATTR_INFO(key.node_arrt1, XML_NODE_ATTR_NAME, var_name);
    node = find_node(doc, &key);
    if(NULL == node) {
        ALOGE("Not found xml variable:%s", var_name);
        return NULL;
    }
    value = (char *) xmlNodeGetContent(node);
    if(NULL == value) {
        ALOGE("Get xml variable(%s) value failed", var_name);
        return NULL;
    }
    return value;
}

static void set_disable_flag(xmlDocPtr doc, char *value, bool is_disable, const char *separator, unordered_map < string,
                             cfg_module_info * >*module) {
    char *next = NULL;
    char *last = NULL;
    char *trim_str = NULL;
    char var_name[XML_VARIABLE_NAME_LEN_MAX] = { 0 };

    if(NULL == value) {
        ALOGE("Invalid parameter");
        return;
    }

    next = strtok_r(value, separator, &last);
    while(NULL != next) {
        trim_str = trim(next);

        if(0 != strlen(trim_str)) {
            if(true == is_xml_variable(trim_str)) {
                char *addr = NULL;

                addr =
                    get_xml_variable_addr(doc, XML_NODE_NAME_VARIABLE,
                                          get_xml_variable_name(trim_str, var_name, sizeof(var_name)));
                if(NULL != addr) {
                    set_disable_flag(doc, addr, is_disable, separator, module);
                    xmlFree(addr);
                }
            } else {
                if(NULL != (*module)[(string) trim_str]) {
                    (*module)[(string) trim_str]->is_disable = is_disable;
                } else {
                    ALOGE("module:[%s] not found in default module info", trim_str);
                }
            }
        }
        next = strtok_r(NULL, separator, &last);
    }
}

static void reset_disable_flag(bool is_disable, list < cfg_module_info * >*ordered_module) {
    list < cfg_module_info * >::iterator iter;

    // reset 'is_disable' of all modules to false(except [MMI] module)
    for(iter = ordered_module->begin(); iter != ordered_module->end(); iter++) {
        cfg_module_info *node = (cfg_module_info *) (*iter);

        if(node != NULL && strcmp(node->module, MODULE_NAME_MMI)) {
            node->is_disable = is_disable;
        }
    }
}

static int get_board_info(board_info_t * board_info) {
    if(NULL == board_info) {
        ALOGE("Invalid parameter");
        return FAILED;
    }
    memset(board_info, 0x00, sizeof(board_info_t));

    if(0 != get_chip_name(board_info->soc_name, BOARD_LEN)) {
        ALOGE("Get soc chip name failed");
        return FAILED;
    }
    ALOGI("Get chip name: %s", board_info->soc_name);

    if(0 != get_subtype_id(board_info->subtype_id, BOARD_LEN)) {
        ALOGE("Get subtype name failed");
        return FAILED;
    }
    ALOGI("Get subtype id: %s", board_info->subtype_id);

    if(get_platform_version(board_info->platform_ver, BOARD_LEN)) {
        ALOGE("Get platform version failed");
        return FAILED;
    }
    ALOGI("Get platform version: %s", board_info->platform_ver);

    return SUCCESS;
}

static int parse_cfg_xml(const char *cfg_path,
                         unordered_map < string, cfg_module_info * >*module_ui_map,
                         list < cfg_module_info * >*ordered_ui_module,
                         unordered_map < string, cfg_module_info * >*module_pcba_map,
                         list < cfg_module_info * >*ordered_pcba_module) {
    int ret = 0;
    char *value = NULL;
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    xmlNodePtr sub_node = NULL;
    find_node_key_t key;
    board_info_t board_info;
    char os_type[32];

    xmlKeepBlanksDefault(0);

    doc = xmlReadFile(cfg_path, "UTF-8", XML_PARSE_RECOVER);
    if(doc == NULL) {
        ALOGE("file(%s) read failed\n", cfg_path);
        return FAILED;
    }

    node = xmlDocGetRootElement(doc);
    if(node == NULL) {
        ALOGE("Not found root node in the file: '%s'\n", cfg_path);
        xmlFreeDoc(doc);
        return FAILED;
    }

    if(xmlStrcmp(node->name, (const xmlChar *) "cfg")) {
        ALOGE("'%s' is invalid cfg xml, reason=not found root node 'cfg'\n", cfg_path);
        xmlFreeDoc(doc);
        return FAILED;
    }

    get_os_type(os_type, sizeof(os_type));
    // parse MMI mode
    SET_NODE_NAME(key, XML_NODE_NAME_OS);
    SET_ATTR_INFO(key.node_arrt1, XML_NODE_ATTR_TYPE, os_type);
    node = find_node(doc, &key);
    if(NULL == node) {
        ALOGE("Not found os config, os type=%s", os_type);
        xmlFreeDoc(doc);
        return FAILED;
    }
    if(SUCCESS != parse_module_info(node->xmlChildrenNode, ordered_ui_module, module_ui_map) ||
       SUCCESS != parse_module_info(node->xmlChildrenNode, ordered_pcba_module, module_pcba_map)) {
        ALOGE("Parse MMI module failed\n");
        xmlFreeDoc(doc);
        return FAILED;
    }
    // parse default config for ui mode
    SET_NODE_NAME(key, XML_NODE_NAME_MODULES);
    node = find_node(doc, &key);
    if(NULL == node) {
        ALOGE("Not found default config for ui: [%s]", XML_NODE_NAME_MODULES);
        xmlFreeDoc(doc);
        return FAILED;
    }
    ret = parse_module_info(node->xmlChildrenNode, ordered_ui_module, module_ui_map);
    if(SUCCESS != ret) {
        ALOGE("Parse default ui config failed\n");
        xmlFreeDoc(doc);
        return FAILED;
    }
    // parse default config for pcba mode
    SET_NODE_NAME(key, XML_NODE_NAME_MODULES_PCBA);
    node = find_node(doc, &key);
    if(NULL == node) {
        ALOGE("Not found default config for pcba: [%s]", XML_NODE_NAME_MODULES_PCBA);
        xmlFreeDoc(doc);
        return FAILED;
    }
    ret = parse_module_info(node->xmlChildrenNode, ordered_pcba_module, module_pcba_map);
    if(SUCCESS != ret) {
        ALOGE("Parse default pcba config failed\n");
        xmlFreeDoc(doc);
        return FAILED;
    }
    // get board info(soc name, subtype id, platform version)
    ret = get_board_info(&board_info);
    if(SUCCESS != ret) {
        ALOGW("Get board info failed, use default config for ui and pcba mode");
    }
    // parse config by board info(soc name, subtype id, platform version)
    SET_NODE_NAME(key, XML_NODE_NAME_SOC);
    SET_ATTR_INFO(key.node_arrt1, XML_NODE_ATTR_OS_TYPE, os_type);
    SET_ATTR_INFO(key.node_arrt2, XML_NODE_ATTR_NAME, board_info.soc_name);
    SET_ATTR_INFO(key.node_arrt3, XML_NODE_ATTR_SUB_TYPE_ID, board_info.subtype_id);
    if(NULL == (node = find_node(doc, &key))) {
        // If not found by board(soc name, subtype id), use board(soc name, subtype id="*") continue find
        SET_ATTR_INFO(key.node_arrt3, XML_NODE_ATTR_SUB_TYPE_ID, "*");
        node = find_node(doc, &key);
    }
    if(NULL != node) {
        // parse default config for ui mode of specified platform
        SET_NODE_NAME(key, XML_NODE_NAME_MODULES);
        sub_node = find_node(node, &key);
        if(NULL != sub_node) {
            ret = update_module_info(sub_node->xmlChildrenNode, module_ui_map);
            if(SUCCESS != ret) {
                ALOGE("update default ui config failed for board: soc name=%s, subtype id=%s\n", board_info.soc_name,
                      board_info.subtype_id);
                xmlFreeDoc(doc);
                return FAILED;
            }
        } else {
            ALOGW("Not found config:%s in board(soc name=%s, subtype id=%s)\n", XML_NODE_NAME_MODULES,
                  board_info.soc_name, board_info.subtype_id);
        }

        // parse default config for pcba mode of specified platform
        SET_NODE_NAME(key, XML_NODE_NAME_MODULES_PCBA);
        sub_node = find_node(node, &key);
        if(NULL != sub_node) {
            ret = update_module_info(sub_node->xmlChildrenNode, module_pcba_map);
            if(SUCCESS != ret) {
                ALOGE("update default pcba config failed for board: soc name=%s, subtype id=%s\n", board_info.soc_name,
                      board_info.subtype_id);
                xmlFreeDoc(doc);
                return FAILED;
            }
        } else {
            ALOGW("Not found config:%s in board(soc name=%s, subtype id=%s)\n", XML_NODE_NAME_MODULES_PCBA,
                  board_info.soc_name, board_info.subtype_id);
        }

        // parse enable modules for ui mode of specified platform
        SET_NODE_NAME(key, XML_NODE_NAME_ENABLE_MODULES);
        SET_ATTR_INFO(key.node_arrt1, XML_NODE_ATTR_MODE, XML_NODE_ATTR_VAL_UI);
        sub_node = find_node(node, &key);
        if(NULL != sub_node) {
            value = (char *) xmlNodeGetContent(sub_node);
            if(NULL == value) {
                ALOGE("Get ui enable modules failed in board(soc name=%s, subtype id=%s)\n",
                      board_info.soc_name, board_info.subtype_id);
                xmlFreeDoc(doc);
                return FAILED;
            }
            reset_disable_flag(true, ordered_ui_module);
            set_disable_flag(doc, value, false, MODULE_SEPARATOR, module_ui_map);
            xmlFree(value);
        } else {
            ALOGW("Not found ui enable modules config in board(soc name=%s, subtype id=%s)\n", board_info.soc_name,
                  board_info.subtype_id);
        }

        // parse enable modules for pcba mode of specified platform
        SET_NODE_NAME(key, XML_NODE_NAME_ENABLE_MODULES);
        SET_ATTR_INFO(key.node_arrt1, XML_NODE_ATTR_MODE, XML_NODE_ATTR_VAL_PCBA);
        sub_node = find_node(node, &key);
        if(NULL != sub_node) {
            value = (char *) xmlNodeGetContent(sub_node);
            if(NULL == value) {
                ALOGE("Get pcba enable modules val failed in board(soc name=%s, subtype id=%s)\n",
                      board_info.soc_name, board_info.subtype_id);
                xmlFreeDoc(doc);
                return FAILED;
            }
            reset_disable_flag(true, ordered_pcba_module);
            set_disable_flag(doc, value, false, MODULE_SEPARATOR, module_pcba_map);
            xmlFree(value);
        } else {
            ALOGW("Not found pcba enable modules config in board(soc name=%s, subtype id=%s)\n", board_info.soc_name,
                  board_info.subtype_id);
        }

        // parse disable config for ui mode of specified platform
        SET_NODE_NAME(key, XML_NODE_NAME_DISABLE_MODULES);
        SET_ATTR_INFO(key.node_arrt1, XML_NODE_ATTR_MODE, XML_NODE_ATTR_VAL_UI);
        SET_ATTR_INFO(key.node_arrt2, XML_NODE_ATTR_PLATFORM_VER, board_info.platform_ver);
        if(NULL == (sub_node = find_node(node, &key))) {
            SET_ATTR_INFO(key.node_arrt2, XML_NODE_ATTR_PLATFORM_VER, "*");
            sub_node = find_node(node, &key);
        }
        if(NULL != sub_node) {
            value = (char *) xmlNodeGetContent(sub_node);
            if(NULL == value) {
                ALOGE("Get ui disable config val failed in board(soc name=%s, subtype id=%s, platform ver=%s)\n",
                      board_info.soc_name, board_info.subtype_id, board_info.platform_ver);
                xmlFreeDoc(doc);
                return FAILED;
            }
            set_disable_flag(doc, value, true, MODULE_SEPARATOR, module_ui_map);
            xmlFree(value);
        } else {
            ALOGW("Not found ui disable config in board(soc name=%s, subtype id=%s, platform ver=%s)\n",
                  board_info.soc_name, board_info.subtype_id, board_info.platform_ver);
        }

        // parse disable config for pcba mode of specified platform
        SET_NODE_NAME(key, XML_NODE_NAME_DISABLE_MODULES);
        SET_ATTR_INFO(key.node_arrt1, XML_NODE_ATTR_MODE, XML_NODE_ATTR_VAL_PCBA);
        SET_ATTR_INFO(key.node_arrt2, XML_NODE_ATTR_PLATFORM_VER, board_info.platform_ver);
        if(NULL == (sub_node = find_node(node, &key))) {
            SET_ATTR_INFO(key.node_arrt2, XML_NODE_ATTR_PLATFORM_VER, "*");
            sub_node = find_node(node, &key);
        }
        if(NULL != sub_node) {
            value = (char *) xmlNodeGetContent(sub_node);
            if(NULL == value) {
                ALOGE("Get pcba disable config val failed in board(soc name=%s, subtype id=%s, platform ver=%s)\n",
                      board_info.soc_name, board_info.subtype_id, board_info.platform_ver);
                xmlFreeDoc(doc);
                return FAILED;
            }
            set_disable_flag(doc, value, true, MODULE_SEPARATOR, module_pcba_map);
            xmlFree(value);
        } else {
            ALOGW("Not found pcba disable config in board(soc name=%s, subtype id=%s, platform ver=%s)\n",
                  board_info.soc_name, board_info.subtype_id, board_info.platform_ver);
        }
    } else {
        ALOGW("Not found config by board(soc name=%s, subtype id=%s, platform ver=%s)\n", board_info.soc_name,
              board_info.subtype_id, board_info.platform_ver);
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return SUCCESS;
}

static void format_output_buf(char *buf, int buf_size, cfg_module_info * module_info) {
    const char **item_name = NULL;
    string module_name = string(module_info->module);

    unordered_map < string, string > *config_list = &(module_info->config_list);

    if(module_name.empty()) {
        ALOGE("Invalid module name: %s", "null");
        return;
    }
    memset(buf, 0x00, buf_size);

    // not [MMI] module
    if(module_name.compare(MODULE_NAME_MMI))
        item_name = module_item_name;
    // [MMI] module
    else
        item_name = mmi_module_item_name;

    // module name
    snprintf(buf, buf_size, "[%s]\n", module_info->module);

    // module items
    while(NULL != *item_name) {
        if(!(*config_list)[(string) (*item_name)].empty())
            snprintf(buf + strlen(buf), buf_size - strlen(buf), "%s=%s\n",
                     *item_name, (*config_list)[(string) (*item_name)].c_str());
        item_name++;
    }
}

static int create_mmi_cfg(const char *cfg_path, list < cfg_module_info * >*ordered_module) {
    FILE *fd = NULL;
    char output_buf[1024] = { 0 };
    char os_type[32] = { 0 };
    board_info_t board_info;

    list < cfg_module_info * >::iterator iter;

    if(NULL == cfg_path || NULL == ordered_module) {
        ALOGE("Invalid parameter");
        return FAILED;
    }

    fd = fopen(cfg_path, "w");
    if(NULL == fd) {
        ALOGE("file(%s) open fail, error=%s", cfg_path, strerror(errno));
        return FAILED;
    }

    get_board_info(&board_info);
    get_os_type(os_type, sizeof(os_type));

    // output config file comments
    snprintf(output_buf, sizeof(output_buf), "#os type=%s, soc name=%s, subtype id=%s, platform version=%s",
             '\0' == os_type[0] ? "null" : os_type,
             '\0' == board_info.soc_name[0] ? "null" : board_info.soc_name,
             '\0' == board_info.subtype_id[0] ? "null" : board_info.subtype_id,
             '\0' == board_info.platform_ver[0] ? "null" : board_info.platform_ver);
    fprintf(fd, "%s\n\n", output_buf);
    fflush(fd);

    // output modules
    for(iter = ordered_module->begin(); iter != ordered_module->end(); iter++) {
        cfg_module_info *module = (cfg_module_info *) (*iter);

        if(module != NULL && true != module->is_disable) {
            format_output_buf(output_buf, sizeof(output_buf), module);
            fprintf(fd, "%s\n", output_buf);
            fflush(fd);
        }
    }

    fclose(fd);
    return SUCCESS;
}

char *get_mmi_cfg_file(int file_type) {
    char *cgf_file = NULL;
    char ftm_ap_dir[128];

    snprintf(ftm_ap_dir, sizeof(ftm_ap_dir), "%s", get_value(KEY_FTM_AP_DIR));
    if(0 != strlen(ftm_ap_dir) && '/' == ftm_ap_dir[strlen(ftm_ap_dir) - 1]) {
        ftm_ap_dir[strlen(ftm_ap_dir) - 1] = '\0';
    }

    if(CFG_TYPE_MMI_CFG == file_type) {
        cgf_file = mmi_cfg_file;
        snprintf(mmi_cfg_file, sizeof(mmi_cfg_file), "%s/%s", ftm_ap_dir, MMI_CFG_FILE_NAME);
    } else {
        cgf_file = mmi_pcba_cfg_file;
        snprintf(mmi_pcba_cfg_file, sizeof(mmi_pcba_cfg_file), "%s/%s", ftm_ap_dir, MMI_PCBA_CFG_FILE_NAME);
    }
    ALOGI("mmi config file path: %s", cgf_file);

    return cgf_file;
}

static void release_memory(list < cfg_module_info * >*ordered_module) {
    list < cfg_module_info * >::iterator iter;

    for(iter = ordered_module->begin(); iter != ordered_module->end(); iter++) {
        cfg_module_info *node = (cfg_module_info *) (*iter);

        if(node != NULL) {
            delete node;
        }
    }
}

static void release_memory(list < cfg_module_info * >*ordered_module_ui, list < cfg_module_info * >*ordered_module_pcba) {
    release_memory(ordered_module_ui);
    release_memory(ordered_module_pcba);
}

int create_mmi_cfg(void) {
    int ret = FAILED;

    list < cfg_module_info * >ordered_ui_module;
    list < cfg_module_info * >ordered_pcba_module;
    unordered_map < string, cfg_module_info * >module_ui_map;
    unordered_map < string, cfg_module_info * >module_pcba_map;

    ordered_ui_module.clear();
    ordered_pcba_module.clear();
    module_ui_map.clear();
    module_pcba_map.clear();

    // parse file: "/etc/mmi/mmi.xml"
    ret = parse_cfg_xml(MMI_XML_FILE, &module_ui_map, &ordered_ui_module, &module_pcba_map, &ordered_pcba_module);
    if(SUCCESS != ret) {
        ALOGE("file(%s) parse failed", MMI_XML_FILE);
        release_memory(&ordered_ui_module, &ordered_pcba_module);
        return FAILED;
    }
    // create file: "/cache/FTM_AP/mmi.cfg"
    ret = create_mmi_cfg(MMI_CONFIG, &ordered_ui_module);
    if(SUCCESS != ret) {
        ALOGE("file(%s) create failed", MMI_CONFIG);
        release_memory(&ordered_ui_module, &ordered_pcba_module);
        return FAILED;
    }
    // create file: "/cache/FTM_AP/mmi-pcba.cfg"
    ret = create_mmi_cfg(MMI_PCBA_CONFIG, &ordered_pcba_module);
    if(SUCCESS != ret) {
        ALOGE("file(%s) create failed", MMI_PCBA_CONFIG);
        release_memory(&ordered_ui_module, &ordered_pcba_module);
        return FAILED;
    }

    release_memory(&ordered_ui_module, &ordered_pcba_module);
    return SUCCESS;
}

static unsigned long calcu_checksum(const char *path) {
    struct stat file_stat;

    if(-1 == stat(path, &file_stat)) {
        return 0;
    }
    return ((unsigned long) (file_stat.st_mtime) + (unsigned long) (file_stat.st_size));
}

static int save_checksum(const char *path, unsigned long chk_sum) {
    FILE *fp = NULL;
    char *checksum_addr = NULL;
    char contents[2048] = { 0 };

    if(0 == chk_sum) {
        return FAILED;
    }

    fp = fopen(path, "r+");
    if(NULL == fp) {
        return FAILED;
    }

    if(1 != fread(contents, sizeof(contents), 1, fp) && !feof(fp)) {
        fclose(fp);
        return FAILED;
    }

    checksum_addr = strstr(contents, "checksum");
    if(NULL == checksum_addr) {
        snprintf(contents + strlen(contents), sizeof(contents) - strlen(contents), "checksum=%012ul\n", chk_sum);
    } else {
        snprintf(checksum_addr, sizeof(contents) - (checksum_addr - contents), "checksum=%012ul\n", chk_sum);
    }

    fseek(fp, 0, SEEK_SET);
    if(1 != fwrite(contents, strlen(contents), 1, fp)) {
        fclose(fp);
        return FAILED;
    }

    fclose(fp);
    return SUCCESS;
}

static unsigned long get_checksum(const char *path) {
    FILE *fp = NULL;
    char name[9 + 1] = { 0 };
    char value[64 + 1] = { 0 };
    char contents[2048] = { 0 };
    char *temp = NULL;
    char *checksum_addr = NULL;

    fp = fopen(path, "r");
    if(NULL == fp) {
        return 0;
    }

    if(1 != fread(contents, sizeof(contents), 1, fp) && !feof(fp)) {
        fclose(fp);
        return 0;
    }

    checksum_addr = strstr(contents, "checksum");
    if(NULL == checksum_addr) {
        fclose(fp);
        return 0;
    }
    temp = checksum_addr;
    while('\0' != *temp) {
        if('\n' == *temp) {
            *temp = '\0';
            break;
        }
        temp++;
    }
    parse_value((const char *) checksum_addr, '=', name, sizeof(name), value, sizeof(value));

    fclose(fp);
    return (unsigned long) strtol(value, NULL, 10);
}

bool is_create_mmi_cfg(void) {
    FILE *fp = NULL;
    char check_sum_file[64];
    unsigned long cal_checksum = 0;
    unsigned long bak_checksum = 0;

    snprintf(check_sum_file, sizeof(check_sum_file), "%s/%s", get_value(KEY_FTM_AP_DIR), MMI_SETTING_FILE);

    // Check "/etc/mmi/mmi.xml" whether exist, cannot config files if not exist.
    if(!check_file_exist(MMI_XML_FILE)) {
        ALOGE("Not found file(%s)", MMI_XML_FILE);
        return false;
    }
    // Create "/cache/FTM_AP/.setting" file to save the checksum
    if(!check_file_exist(check_sum_file) && NULL == (fp = fopen(check_sum_file, "w"))) {
        return true;
    }
    if(NULL != fp) {
        fclose(fp);
    }
    // Calculate checksum use the modification time and size of "/etc/mmi/mmi.xml"
    cal_checksum = calcu_checksum(MMI_XML_FILE);
    if(0 == cal_checksum) {
        return true;
    }
    // if "/cache/FTM_AP/mmi.cfg" or "/cache/FTM_AP/mmi-pcba.cfg" not exist, create the files
    if(!check_file_exist(MMI_CONFIG) || !check_file_exist(MMI_PCBA_CONFIG)) {
        save_checksum(check_sum_file, cal_checksum);
        return true;
    }
    // Get the checksum from "/cache/FTM_AP/.setting"
    bak_checksum = get_checksum(check_sum_file);
    if(0 == bak_checksum) {
        save_checksum(check_sum_file, cal_checksum);
        return true;
    }
    // Check checksum whether change
    if(cal_checksum != bak_checksum) {
        save_checksum(check_sum_file, cal_checksum);
        return true;
    }

    return false;
}
