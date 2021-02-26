/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __MMI_CFG__
#define __MMI_CFG__

#define MMI_XML_FILE                        MMI_RES_BASE_DIR"mmi.xml"
#define MMI_CFG_FILE_NAME                   "mmi.cfg"
#define MMI_PCBA_CFG_FILE_NAME              "mmi-pcba.cfg"
#define MMI_SETTING_FILE                    ".setting"
#define MODULE_NAME_MMI                     "MMI"
#define MODULE_SEPARATOR                    ",\n\r" // , \n \r
#define BOARD_LEN                           32
#define CFG_TYPE_MMI_CFG                    1
#define CFG_TYPE_MMI_PCBA_CFG               2
#define XML_VARIABLE_NAME_LEN_MAX           128

#define XML_NODE_NAME_OS                    "os"
#define XML_NODE_NAME_MODULES               "modules"
#define XML_NODE_NAME_MODULES_PCBA          "modules_pcba"
#define XML_NODE_NAME_SOC                   "soc"
#define XML_NODE_NAME_MODULE                "module"
#define XML_NODE_NAME_DEF_DISABLE_MODULES   "default_disable_modules"
#define XML_NODE_NAME_DISABLE_MODULES       "disable_modules"
#define XML_NODE_NAME_VARIABLE              "variable"
#define XML_NODE_NAME_ENABLE_MODULES        "enable_modules"

#define XML_NODE_ATTR_NAME                  "name"
#define XML_NODE_ATTR_LIB_NAME              "lib_name"
#define XML_NODE_ATTR_ENABLE                "enable"
#define XML_NODE_ATTR_AUTOMATION            "automation"
#define XML_NODE_ATTR_DISPLAY               "display_name"
#define XML_NODE_ATTR_PARAMETER             "parameter"
#define XML_NODE_ATTR_LAYOUT                "layout"
#define XML_NODE_ATTR_LIMIT                 "limit"
#define XML_NODE_ATTR_MODE                  "mode"
#define XML_NODE_ATTR_DB_ENABLE             "debug_enable"
#define XML_NODE_ATTR_AUTORUN_ENABLE        "autorun_enable"
#define XML_NODE_ATTR_SKIP_AUTORUN_PASSED   "skip_if_autorun_passed"
#define XML_NODE_ATTR_SYS_BL                "sys_backlight"
#define XML_NODE_ATTR_MISC_DEV              "misc_dev"
#define XML_NODE_ATTR_LANGUAGE              "language"
#define XML_NODE_ATTR_FONT_SIZE             "font_size"
#define XML_NODE_ATTR_TEST_MODE             "test_mode"
#define XML_NODE_ATTR_PER_PAGE_ITEMS        "item_display_per_page"
#define XML_NODE_ATTR_SUB_TYPE_ID           "subtype_id"
#define XML_NODE_ATTR_PLATFORM_VER          "platform_ver"
#define XML_NODE_ATTR_HOME_SCREENS          "home_screens"
#define XML_NODE_ATTR_TYPE                  "type"
#define XML_NODE_ATTR_OS_TYPE                "os_type"
#define XML_NODE_ATTR_VAL_UI                "ui"
#define XML_NODE_ATTR_VAL_PCBA              "pcba"

#define SET_NODE_NAME(key, name) { \
                      memset(&key, 0x00, sizeof(find_node_key_t)); \
                      key.node_name = name; \
}
#define SET_ATTR_INFO(attr_info, name, val) { \
                      attr_info.arrt_name = name; \
                      attr_info.arrt_val = val; \
}

typedef struct board_info {
    char soc_name[BOARD_LEN];
    char platform_ver[BOARD_LEN];
    char subtype_id[BOARD_LEN];
} board_info_t;

typedef struct node_arrt_info {
    char *arrt_name;
    char *arrt_val;
} node_arrt_t;

typedef struct find_key {
    char *node_name;            // xml node name
    node_arrt_t node_arrt1;     // xml node properties
    node_arrt_t node_arrt2;     // xml node properties
    node_arrt_t node_arrt3;     // xml node properties
} find_node_key_t;

class cfg_module_info {
  public:
    char module[64];
    bool is_disable;            // false:not output; true:output
      unordered_map < string, string > config_list;

      cfg_module_info(char *mod_name) {
        if(mod_name != NULL)
            snprintf(module, sizeof(module), "%s", mod_name);

        // default all module enable
        is_disable = false;
        config_list.clear();
    }

     ~cfg_module_info() {
        config_list.clear();
    }
};

int create_mmi_cfg(void);
char *get_mmi_cfg_file(int file_type);
bool is_create_mmi_cfg(void);

#endif
