/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <string.h>
#include <osal_errorcodes.h>
#include <osal_log.h>
#include <osal_mem.h>
#include <osal_types.h>
#include <platform_info.h>
#include <ipc.h>

#if defined(_OPEN_WRT_)
    #define AUDIO_DATA_CH_INTENT1_SIZE 8192
    #define AUDIO_DATA_CH_INTENT2_SIZE 8192
#else
    #define AUDIO_DATA_CH_INTENT1_SIZE 4096
    #define AUDIO_DATA_CH_INTENT2_SIZE 4096
#endif /* _OPEN_WRT_ */

#ifdef _ANDROID_
#include <expat.h>
#define XML_READ_BUF_SIZE 1024
#define PLATFORM_XML_PATH "/vendor/etc/graphite_ipc_platform_info.xml"
#endif

struct cmd_ch_map cmd_ch_map[GRAPHITE_CMD_MAX] = {
    {GRAPHITE_CMD_OPEN, "g_glink_ctrl"},
    {GRAPHITE_CMD_CLOSE, "g_glink_ctrl"},
    {GRAPHITE_CMD_SET_CONFIG, "g_glink_ctrl"},
    {GRAPHITE_CMD_GET_CONFIG, "g_glink_ctrl"},
    {GRAPHITE_CMD_LOAD_DATA, "g_glink_persistent_data_nild"},
    {GRAPHITE_CMD_UNLOAD_DATA, "g_glink_ctrl"},
    {GRAPHITE_CMD_DATA, "g_glink_audio_data"},
};

#ifdef _ANDROID_

static void glink_ch_config(const XML_Char **attr,
                            struct platform_info *priv)
{
    struct wdsp_glink_ch_cfg *ch, *ch1;
    uint8_t i, *ch_ptr;
    char *size, *test = NULL;
    uint32_t no_of_intents = 0, cfg_size, len;

    cfg_size = sizeof(struct wdsp_glink_ch_cfg);
    ch = osal_mem_calloc(1, cfg_size);
    if (!ch) {
        OSAL_LOGE("%s: Memory allocation failed for ch\n", __func__);
        goto done;
    }

    if (!strcmp(attr[0], "name") && attr[1]) {
        len = strlcpy(ch->name, (const char *)attr[1], WDSP_CH_NAME_MAX_LEN);
        if (len >= WDSP_CH_NAME_MAX_LEN)
            OSAL_LOGE("%s: Channel name %s truncated\n", __func__, (const char *)attr[1]);
    } else {
        OSAL_LOGE("%s: Invalid channel name\n", __func__);
        goto err_free;
    }

    if (!strcmp(attr[2], "latency_in_us") && attr[3]) {
        ch->latency_in_us = atoi((const char *)attr[3]);
    } else {
        OSAL_LOGE("%s: Invalid latency\n", __func__);
        goto err_free;
    }

    if (!strcmp(attr[4], "no_of_intents") && attr[5]) {
        ch->no_of_intents = atoi((const char *)attr[5]);
    } else {
        OSAL_LOGE("%s: Invalid no_of_intents\n", __func__);
        goto err_free;
    }

    if (ch->no_of_intents) {
        cfg_size += (sizeof(uint32_t) * ch->no_of_intents);
        ch1 = osal_mem_realloc(ch, cfg_size);
        if (!ch1) {
            OSAL_LOGE("%s: Realloc failed for ch\n", __func__);
            goto err_free;
        }
        ch = ch1;

        if (!strcmp(attr[6], "intents_size") && attr[7]) {
            size = strtok_r((char *) attr[7], ", ",&test);
            if (!size) {
                OSAL_LOGE("%s: incorrect intent size\n", __func__);
                goto err_free;
            }
            ch->intents_size[0] = atoi(size);
            for (i = 1; i < ch->no_of_intents; i++) {
                size = strtok_r(NULL, ", ", &test);
                if (!size) {
                    OSAL_LOGE("%s: incorrect intent size, iter = %d\n", __func__, i);
                    goto err_free;
                }
                ch->intents_size[i] = atoi(size);
            }
        }
        priv->ch_size += (sizeof(uint32_t) * ch->no_of_intents);
        ch1 = osal_mem_realloc(priv->ch, priv->ch_size);
        if (!ch1) {
            OSAL_LOGE("%s: ch memory allocation failed\n", __func__);
            goto err_free;
        }
        priv->ch = ch1;
    }

    ch_ptr = (uint8_t *)priv->ch  + priv->ch_copied;
    osal_mem_cpy(ch_ptr, cfg_size, ch, cfg_size);
    priv->ch_copied += cfg_size;

    OSAL_LOGD("%s: ch_cfg size = %d, copied ch_size = %d\n",
              __func__, cfg_size, priv->ch_copied);

err_free:
    osal_mem_free(ch);

done:
    return;
}

static void start_tag(void *userdata, const XML_Char *tag_name,
                      const XML_Char **attr)
{
    struct platform_info *priv;
    uint32_t cfg_size;

    if (!userdata || !tag_name || !attr) {
        OSAL_LOGE("%s: Invalid arguments\n", __func__);
        return;
    }
    priv = (struct platform_info *) userdata;

    OSAL_LOGV("%s: tag %s\n", __func__, tag_name);
    if (!strcmp(tag_name, "no_of_glink_channels") && attr[0]) {
        if (!strcmp(attr[0], "value") && attr[1]) {
            priv->no_of_glink_channels = atoi((const char *)attr[1]);

            cfg_size = sizeof(struct wdsp_glink_ch_cfg);
            priv->ch = osal_mem_calloc(priv->no_of_glink_channels, cfg_size);
            if (!priv->ch) {
                OSAL_LOGE("%s: ch memory allocation failed\n", __func__);
                return;
            }
            priv->ch_size = priv->no_of_glink_channels * cfg_size;
        } else {
            OSAL_LOGE("%s: Invalid no_of_glink_channels\n", __func__);
        }
    } else if (!strcmp(tag_name, "glink_channel")) {
        if (priv->no_of_glink_channels)
            glink_ch_config(attr, priv);
    }
}

static void end_tag(void *userdata __unused, const XML_Char *tag_name __unused)
{
    return;
}

static int32_t platform_get_info_from_xml(struct platform_info *pinfo)
{
    struct wdsp_glink_ch_cfg *ch;
    int32_t i;
    XML_Parser      parser;
    FILE            *file;
    int32_t             ret = 0;
    int32_t             bytes_read;
    void            *buf;

    file = fopen(PLATFORM_XML_PATH, "r");
    if (!file) {
        OSAL_LOGD("%s: Failed to open %s\n", __func__, PLATFORM_XML_PATH);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    parser = XML_ParserCreate(NULL);
    if (!parser) {
        OSAL_LOGE("%s: Failed to create XML parser!\n", __func__);
        ret = OSAL_EBADPARAM;
        goto err_close_file;
    }
    XML_SetUserData(parser, pinfo);
    XML_SetElementHandler(parser, start_tag, end_tag);

    while (1) {
        buf = XML_GetBuffer(parser, XML_READ_BUF_SIZE);
        if (buf == NULL) {
            OSAL_LOGE("%s: XML_GetBuffer failed\n", __func__);
            ret = OSAL_ENOMEMORY;
            goto err_free_parser;
        }

        bytes_read = fread(buf, 1, XML_READ_BUF_SIZE, file);
            OSAL_LOGE("%s: bytes_read %d\n", __func__, bytes_read);
        if (bytes_read < 0) {
            OSAL_LOGE("%s: fread failed, bytes read = %d\n", __func__, bytes_read);
             ret = bytes_read;
            goto err_free_parser;
        }

        if (XML_ParseBuffer(parser, bytes_read,
                            bytes_read == 0) == XML_STATUS_ERROR) {
            OSAL_LOGE("%s: XML_ParseBuffer failed\n", __func__);
            ret = OSAL_EBADPARAM;
            goto err_free_parser;
        }

        if (bytes_read == 0)
            break;
    }

err_free_parser:
    XML_ParserFree(parser);

err_close_file:
    fclose(file);
    if (ret < 0) {
        if (pinfo->ch) {
            osal_mem_free(pinfo->ch);
            pinfo->ch = NULL;
        }
    }
done:
    return ret;
}

#else

static int32_t platform_get_info_from_xml(struct platform_info *pinfo)
{
    OSAL_LOGD("%s: XML parser is not supported, set defaults\n", __func__);

    return OSAL_EBADPARAM;
}

#endif

int32_t platform_get_ch_name(uint16_t cmd, bool use_island, struct wdsp_cmd_pkt *pkt)
{
    int i, len;

    if (!pkt) {
        OSAL_LOGD("%s: Invalid pkt\n", __func__);
        return OSAL_EBADPARAM;
    }

    for (i = 0; i < GRAPHITE_CMD_MAX; i++) {
        if (cmd_ch_map[i].cmd == cmd) {
             len = strlcpy(pkt->ch_name, cmd_ch_map[i].ch_name, WDSP_CH_NAME_MAX_LEN);
             if (len >= WDSP_CH_NAME_MAX_LEN)
                 OSAL_LOGE("%s: Channel name %s truncated\n", __func__,
                           cmd_ch_map[i].ch_name);

             if (cmd == GRAPHITE_CMD_LOAD_DATA && use_island) {
                 len = strlcpy(pkt->ch_name, "g_glink_persistent_data_ild",
                               WDSP_CH_NAME_MAX_LEN);
                 if (len >= WDSP_CH_NAME_MAX_LEN)
                     OSAL_LOGE("%s: Channel name g_glink_persistent_data_ild truncated\n",
                               __func__);
             }
             break;
        }
    }
    return 0;
}

static int32_t platform_set_default_info(struct platform_info *pinfo)
{
    int32_t ret = 0, i;
    uint8_t *ch_ptr;
    struct wdsp_glink_ch_cfg ch[GLINK_CH_MAX];
    struct wdsp_glink_ch_cfg *pch;
    uint32_t total_no_of_intents = 0, cfg_size;
    uint32_t intents_cnt = 0, len;

    len = strlcpy(ch[0].name, "g_glink_ctrl", WDSP_CH_NAME_MAX_LEN);
    if (len >= WDSP_CH_NAME_MAX_LEN)
        OSAL_LOGE("%s: Channel name g_glink_ctrl truncated\n", __func__);
    ch[0].latency_in_us = 5000;
    ch[0].no_of_intents = 1;

    len = strlcpy(ch[1].name, "g_glink_persistent_data_ild", WDSP_CH_NAME_MAX_LEN);
    if (len >= WDSP_CH_NAME_MAX_LEN)
        OSAL_LOGE("%s: Channel name g_glink_persistent_data_ild truncated\n", __func__);
    ch[1].latency_in_us = 30000;
    ch[1].no_of_intents = 0;

    len = strlcpy(ch[2].name, "g_glink_persistent_data_nild", WDSP_CH_NAME_MAX_LEN);
    if (len >= WDSP_CH_NAME_MAX_LEN)
        OSAL_LOGE("%s: Channel name g_glink_persistent_data_nild truncated\n", __func__);
    ch[2].latency_in_us = 30000;
    ch[2].no_of_intents = 0;

    len = strlcpy(ch[3].name, "g_glink_audio_data", WDSP_CH_NAME_MAX_LEN);
    if (len >= WDSP_CH_NAME_MAX_LEN)
        OSAL_LOGE("%s: Channel name g_glink_audio_data truncated\n", __func__);
    ch[3].latency_in_us = 15000;
    ch[3].no_of_intents = 2;

    for (i = 0; i < GLINK_CH_MAX; i++)
        total_no_of_intents += ch[i].no_of_intents;

    cfg_size = sizeof(struct wdsp_glink_ch_cfg);
    pinfo->ch_size = (cfg_size * GLINK_CH_MAX) + (sizeof(uint32_t) * total_no_of_intents);

    pinfo->ch = osal_mem_calloc(1, pinfo->ch_size);
    if (!pinfo->ch) {
        OSAL_LOGE("%s: Memory allocation failed for ch\n", __func__);
        ret = OSAL_ENOMEMORY;
        goto done;
    }
    pinfo->no_of_glink_channels = GLINK_CH_MAX;
    ch_ptr = (uint8_t *) pinfo->ch;

    osal_mem_cpy(ch_ptr, cfg_size, &ch[0], cfg_size);
    pch = (struct wdsp_glink_ch_cfg *)ch_ptr;
    /* Intent size is in bytes */
    pch->intents_size[0] = 1024;
    intents_cnt++;

    ch_ptr = ch_ptr + cfg_size + sizeof(uint32_t) * intents_cnt;
    osal_mem_cpy(ch_ptr, cfg_size, &ch[1], cfg_size);

    ch_ptr = ch_ptr + cfg_size;
    osal_mem_cpy(ch_ptr, cfg_size, &ch[2], cfg_size);

    ch_ptr = ch_ptr + cfg_size;
    osal_mem_cpy(ch_ptr, cfg_size, &ch[3], cfg_size);
    pch = (struct wdsp_glink_ch_cfg *)ch_ptr;

    pch->intents_size[0] = AUDIO_DATA_CH_INTENT1_SIZE;
    pch->intents_size[1] = AUDIO_DATA_CH_INTENT2_SIZE;
    intents_cnt += 2;

done:
    return ret;
}

int32_t platform_info_init(struct platform_info **info)
{
    int32_t ret = 0;
    struct wdsp_glink_ch_cfg *ch;
    uint8_t *ch_ptr;
    uint32_t i, j;
    struct platform_info *pinfo;

    if (!info) {
        OSAL_LOGE("%s: info is NULL\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    *info = NULL;
    pinfo = osal_mem_calloc(1, sizeof(struct platform_info));
    if (!pinfo) {
        OSAL_LOGE("%s: Memory allocation failed for pinfo\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }

    ret = platform_get_info_from_xml(pinfo);
    if (ret < 0) {
        OSAL_LOGE("%s: Failed to get info from xml, set default\n", __func__);
        ret = platform_set_default_info(pinfo);
        if (ret < 0)
            goto err_free;
    }

    /* Copy cmd to ch mapping */
    osal_mem_cpy(pinfo->cmd_ch_map, sizeof(cmd_ch_map), cmd_ch_map, sizeof(cmd_ch_map));

    ch_ptr = (uint8_t *) pinfo->ch;
    i = 0;
    while(i++ < pinfo->no_of_glink_channels) {
        ch = (struct wdsp_glink_ch_cfg *)ch_ptr;
        OSAL_LOGD("%s: name = %s, latency = %d, intents = %d\n",
                  __func__, ch->name, ch->latency_in_us, ch->no_of_intents);
        for (j = 0; j < ch->no_of_intents; j++)
            OSAL_LOGD("%s: intents_size = %d\n", __func__, ch->intents_size[j]);

        ch_ptr += sizeof(struct wdsp_glink_ch_cfg) + (ch->no_of_intents * sizeof(uint32_t));
    }
    *info = pinfo;
    goto done;

err_free:
    osal_mem_free(pinfo);

done:
    return ret;
}

int32_t platform_info_deinit(struct platform_info *info)
{
    int32_t ret = 0;

    if (!info) {
        OSAL_LOGE("%s: info is NULL\n", __func__);
        ret = OSAL_EBADPARAM;
        goto done;
    }
    if (info->ch) {
        osal_mem_free(info->ch);
        info->ch = NULL;
    }
    osal_mem_free(info);
    info = NULL;

done:
    return ret;
}

