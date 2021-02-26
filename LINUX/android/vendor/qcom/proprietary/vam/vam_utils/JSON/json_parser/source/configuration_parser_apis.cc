/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

extern "C"{
#include "configuration_parser_apis.h"
}
#include "configuration_parser.h"

extern "C" enum JSONVAStatus configuration_parser_init(void ** handle)
{
    JSONConfigurationParser *configuration_parser = new JSONConfigurationParser();
    *handle = configuration_parser;
    return JSON_VA_OK;
}

extern "C" enum JSONVAStatus configuration_parser_deinit(void * handle)
{
    JSONConfigurationParser *configuration_parser = (JSONConfigurationParser *)
        handle;
    delete configuration_parser;
    return JSON_VA_OK;
}

extern "C" enum JSONVAStatus configuration_parser_clear(void * handle)
{
    JSONConfigurationParser *configuration_parser = (JSONConfigurationParser *)handle;
    configuration_parser->configuration_content_.atomic_rules.clear();
    configuration_parser->configuration_content_.rules.clear();
    configuration_parser->configuration_content_.zones.clear();
#if (VA_CONFIG_VERSION==0)
    configuration_parser->configuration_content_.lines.clear();
    for (int i = 0;
        i < configuration_parser->configuration_content_.extension.size();
        i++){
        delete []configuration_parser->configuration_content_.extension[i].data;
    }
    configuration_parser->configuration_content_.extension.clear();
#endif
    return JSON_VA_OK;
}

extern "C" enum JSONVAStatus configuration_parser_load_file(void * handle, const char *filename,
    char **doc_str)
{
    JSONConfigurationParser *configuration_parser = (JSONConfigurationParser *)handle;
    return configuration_parser->LoadFileToString(filename, doc_str);
}

extern "C" enum JSONVAStatus configuration_parser_parse_doc(void * handle, const char* doc,
    struct JSONConfiguration ** configuration)
{
    JSONConfigurationParser *configuration_parser = (JSONConfigurationParser *)handle;
    JSONVAStatus res = configuration_parser->ParseDoc(doc, configuration);
    return res;
}

extern "C" enum JSONVAStatus configuration_parser_parse_file(void * handle, const char* file_name,
    struct JSONConfiguration ** configuration)
{
    JSONVAStatus res;
    char * doc;
    JSONConfigurationParser *configuration_parser = (JSONConfigurationParser *)handle;
    res = configuration_parser_load_file(handle, file_name, &doc);
    if (res == JSON_VA_OK)
        res = configuration_parser->ParseDoc(doc, configuration);
    else{
        doc = 0;
        res = configuration_parser->ParseDoc(doc, configuration);
    }
    return res;
}
