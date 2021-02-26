#ifndef ERR_TYPES_H
#define ERR_TYPES_H

/*===========================================================================

                    Error Handling Service Typedefs Header File

Description

Copyright (c) 2013 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_types.h#1 $

===========================================================================*/

#include "errlog.h"

typedef unsigned long long err_data_flag_type;

#define ERR_FILE_NAME_LEN 8
typedef struct
{
  byte address;         /* Storage address 0 to ERR_MAX_LOG-1 */
  byte err_count;       /* Number of occurances (0=empty,FF=full) */
  byte file_name[ERR_FILE_NAME_LEN];
  word line_num;
  boolean fatal;
} err_log_type;

typedef struct {
  err_data_flag_type flag;
  err_log_type nv_log;
  unsigned long length;
  char   data[ERR_DATA_MAX_SIZE];
} err_data_log_type;

typedef struct {
  err_data_flag_type reset_flag;
  err_data_log_type err_log;
} err_data_type;


#endif /* ERR_TYPES_H */
