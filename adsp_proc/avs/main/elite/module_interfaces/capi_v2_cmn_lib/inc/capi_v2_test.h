#ifndef __CAPI_V2_TEST_H
#define __CAPI_V2_TEST_H
/*===========================================================================

                  C A P I V 2  T E S T   U T I L I T I E S

  Common definitions, types and functions for CAPIv2.

/*------------------------------------------------------------------------
   Copyright (c) 2013, 2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/capi_v2_cmn_lib/inc/capi_v2_test.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2013/09/03 dg      Created file
===========================================================================*/

#include "capi_v2_util.h"
#include "Elite_CAPI_V2.h"

const uint32_t wordSize = 30;

typedef struct _args_t
{
   char *input_filename;
   char *output_filename;
   char *config_filename;
}
args_t;

static const uint32_t FORMAT_BUF_SIZE = 128;

typedef struct _module_info_t
{
   capi_v2_t*    module_ptr;
   bool_t   is_enabled;
   uint32_t   alg_delay;
   bool_t     is_in_place;
   uint32_t   in_buffer_len;
   uint32_t   out_buffer_len;
   FILE*      finp;
   FILE*      fout;
   FILE*      fCfg;
   bool_t requires_data_buffering;
   capi_v2_buf_t in_format;
   capi_v2_buf_t out_format;
   // Preallocating buffers to avoid the need for handling malloc failures.
   int8_t in_format_buf[FORMAT_BUF_SIZE];
   int8_t out_format_buf[FORMAT_BUF_SIZE];
} module_info_t;

struct testCommand
{
    char opCode[wordSize];
    capi_v2_err_t(*pFunction)(module_info_t * module);
};

capi_v2_err_t RunTest(module_info_t* module, const testCommand *pExtendedCmdSet = NULL, const uint32_t extendedCmdSetSize = 0);

capi_v2_err_t GetWord(FILE* fCfg, char word[]);
capi_v2_err_t GetUIntParameter(FILE* fCfg, const char parameterName[], uint32_t *pValues);
capi_v2_err_t GetIntParameter(FILE* fCfg, const char parameterName[], int32_t *pValue);
capi_v2_err_t ReadBufferContents(FILE *fCfg, const uint32_t payloadSize, uint8_t *pPayload);
capi_v2_err_t ReadChannelMapping(FILE *fCfg, const uint32_t num_channels, uint16_t channel_mapping[]);
uint32_t capi_v2_tst_get_num_mallocs();
uint32_t capi_v2_tst_get_num_frees();
void capi_v2_tst_set_malloc_failure(int num_mallocs_to_skip);
void capi_v2_tst_clear_malloc_failure(void);

void usage(FILE * fp, char *prog_name);

void get_eargs(
      int32_t argc,
      char * argv[],
      args_t* input_args
      );

capi_v2_event_callback_info_t capi_v2_tst_get_cb_info(module_info_t *module);

#endif // __CAPI_V2_TEST_H

