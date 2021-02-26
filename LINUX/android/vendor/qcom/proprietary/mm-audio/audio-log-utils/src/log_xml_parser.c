/*
 *
 * This library contains the API to parse dynamic logging xml
 * for audio modules.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "log_xml_parser"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include "log_xml_parser.h"
#define MAX_MODULE_NAME_LEN  64
#define MAX_FILE_LEN 64
#define MAX_MODULE_BUF_SIZE 128

const struct StringToEnum halModFiles[] = {
    {"audio_hw.c", HAL_MOD_FILE_AUDIO_HW},
    {"audio_hw_extn_api.c", HAL_MOD_FILE_AUDIO_HW_EXTN_API},
    {"edid.c", HAL_MOD_FILE_EDID},
    {"platform_info.c", HAL_MOD_FILE_PLATFORM_INFO},
    {"voice.c", HAL_MOD_FILE_VOICE},
    {"a2dp.c", HAL_MOD_FILE_A2DP},
    {"adsp_hdlr.c", HAL_MOD_FILE_ADSP_HDLR},
    {"audio_extn.c", HAL_MOD_FILE_AUDIO_EXTN},
    {"bt_hal.c", HAL_MOD_FILE_BT_HAL},
    {"compress_capture.c", HAL_MOD_FILE_COMPR_CAP},
    {"compress_in.c", HAL_MOD_FILE_COMPR_IN},
    {"dev_arbi.c", HAL_MOD_FILE_DEV_ARBI},
    {"dolby.c", HAL_MOD_FILE_DOLBY},
    {"dts_eagle.c", HAL_MOD_FILE_DTS_EAGLE},
    {"fm.c", HAL_MOD_FILE_FM},
    {"gef.c", HAL_MOD_FILE_GEF},
    {"hfp.c", HAL_MOD_FILE_HFP},
    {"keep_alive.c", HAL_MOD_FILE_KEEP_ALIVE},
    {"listen.c", HAL_MOD_FILE_LISTEN},
    {"passthru.c", HAL_MOD_FILE_PASSTH},
    {"pm.c", HAL_MOD_FILE_PM},
    {"qaf.c", HAL_MOD_FILE_QAF},
    {"sndmonitor.c", HAL_MOD_FILE_SND_MONITOR},
    {"soundtrigger.c", HAL_MOD_FILE_SND_TRIGGER},
    {"source_track.c", HAL_MOD_FILE_SRC_TRACK},
    {"spkr_protection.c", HAL_MOD_FILE_SPKR_PROT},
    {"ssr.c", HAL_MOD_FILE_SSR},
    {"usb.c", HAL_MOD_FILE_USB},
    {"utils.c", HAL_MOD_FILE_UTILS},
    {"hw_info.c", HAL_MOD_FILE_HW_INFO},
    {"platform.c", HAL_MOD_FILE_PLATFORM},
    {"compress_voip.c", HAL_MOD_FILE_COMPR_VOIP},
    {"voice_extn.c", HAL_MOD_FILE_VOICE_EXTN}
};

int stringToValue(const struct StringToEnum *table, size_t size, const char *name)
{
    int length = strnlen(name, MAX_FILE_LEN);
    for (size_t i = 0; i < size; i++) {
        if (strncmp(table[i].name, name, length) == 0) {
           ALOGV("File found %s", table[i].name);
           return table[i].value;
        }
    }
    return -EINVAL;
}

static void set_log_value(struct log_xml_data *data, const char *tag_name,
                          const XML_Char **attr)
{
    static unsigned int i = 0;
    unsigned int size = data->size;
    bool new_module = !data->parse_module;
    uint32_t index = 0;
    if (new_module) {
        if (strncmp(attr[0], "name", sizeof("name"))) {
            ALOGE("%s: 'name' not found for module tag", __func__);
            goto done;
        }

        if (strncmp(attr[1], data->mod_name, sizeof(data->mod_name))) {
            ALOGD("%s: module name does not match attr %s, mod name %s",
                   __func__, attr[1], data->mod_name);
            goto done;
        }

        if (attr[2] && !strncmp(attr[2], "log_mode", sizeof("log_mode"))) {
            ALOGD("%s: log mode set to %s", __func__, attr[3]);
            if (!strncmp(attr[3], "MOD", sizeof("MOD"))) {
                ALOGD("%s: logging module level,do not parse module", __func__);
                for (i=0; i < size; i++) {
                     data->log_value[i] = 0x1;
                }
            } else {
                ALOGD("%s: logging file level, parse module further", __func__);
                data->parse_module = true;
                /* reset i here as files under a module shall be parsed further */
                i = 0;
            }
        }
        goto done;
    }

    /* process the module elements i.e. file names under each module */
    if (strncmp(tag_name, "file", sizeof("file"))) {
            ALOGE("%s: 'file' not found under module tag", __func__);
            goto done;
    }

    if (attr[0] && !strncmp(attr[0], "name", sizeof("name"))) {
        if (strncmp(data->mod_name, "hal", sizeof("hal")) == 0) {
            index = stringToValue(halModFiles, ARR_SIZE(halModFiles), attr[1]);
            if (index < 0 || index >= HAL_MOD_FILE_MAX) {
                ALOGE("error in value of file returned");
                goto done;
            }
        }
        else {
            ALOGE("Module name does not match");
            goto done;
        }

        if (attr[2] && !strncmp(attr[2], "log_enable", sizeof("log_enable"))) {
            ALOGD("%s: file %s, log enable %s", __func__, attr[1], attr[3]);
            if (i >= size) {
                ALOGE("%s: ERROR: SHOULD NOT BE HERE", __func__);
                goto done;
            }
            if (!strncmp(attr[3], "true", sizeof("true"))) {
                ALOGD("%s: set logging to true for value %d", __func__, i);
                ALOGD("value of index is %d", index);
                data->log_value[index] = 0x1;
            } else {
                ALOGD("%s: set logging to false for value %d", __func__, i);
                data->log_value[index] = 0x0;
            }
            i = i + 1;
        }
    }

done:
    return;
}

static void start_tag(void *userdata, const XML_Char *tag_name,
                      const XML_Char **attr)
{
    const XML_Char *attr_name = NULL;
    const XML_Char *attr_value = NULL;
    struct log_xml_data *data = (struct log_xml_data *)userdata;

    if (!strncmp(tag_name, "module", sizeof("module")) || data->parse_module) {
        set_log_value(data, tag_name, attr);
    }

    return;
}

static void end_tag(void *userdata, const XML_Char *tag_name)
{
    struct log_xml_data *data = (struct log_xml_data *)userdata;

    if (!strncmp(tag_name, "module", sizeof("module"))) {
         data->parse_module = false;
    }
}

int parse_log_xml(const char *module_name, int *log_value, unsigned int size)
{
    int ret = 0;
    FILE *file;
    XML_Parser      parser;
    enum XML_Status status;
    struct log_xml_data *data;

    if (!module_name || !log_value) {
        ALOGD("%s: Invalid module name %p or log value %p", __func__,
             module_name, log_value);
      ret = -ENODEV;
      goto done;
    }

    file = fopen(LOG_XML_PATH, "r");
    if (!file) {
        ALOGD("%s: Failed to open file %s", __func__, LOG_XML_PATH);
        ret = -ENODEV;
        goto done;
    }

    parser = XML_ParserCreate(NULL);
    if (!parser) {
        ALOGD("%s: Failed to create XML parser!", __func__);
        ret = -ENODEV;
        goto err_close_file;
    }

    data = calloc(1, sizeof(struct log_xml_data));
    if (!data) {
        ALOGD("%s: Insufficient memory to allocate for data", __func__);
        ret = -ENOMEM;
        goto err_free_parser;
    }

    data->parse_module = false;
    data->mod_name = module_name;
    data->log_value = log_value;
    data->size = size;

    XML_SetUserData(parser, data);

    XML_SetElementHandler(parser, start_tag, end_tag);
    for (;;) {
        int bytes_read;

        void *buf = XML_GetBuffer(parser, BUF_SIZE);
        if (buf == NULL) {
            ALOGD("%s: XML_GetBuffer failed", __func__);
            ret = -ENOMEM;
            goto err_free_parser;
        }

        bytes_read = fread(buf, 1, BUF_SIZE, file);
        if (bytes_read < 0) {
            ALOGD("%s: fread failed, bytes read = %d", __func__, bytes_read);
            ret = bytes_read;
            goto err_free_parser;
        }

        status = XML_ParseBuffer(parser, bytes_read,
                                 bytes_read == 0);
        if (status == XML_STATUS_ERROR) {
            ALOGD("%s: XML_ParseBuffer failed, err %d", __func__, XML_GetErrorCode(parser));
            ret = -EINVAL;
            goto err_free_parser;
        }

        if (bytes_read == 0)
            break;
    }

err_free_parser:
    if (data)
        free(data);
    XML_ParserFree(parser);
err_close_file:
    if (file)
        fclose(file);
done:
    return ret;
}
