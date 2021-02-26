#ifndef MICRO_DIAG_BUFFER_PARSER_H
#define MICRO_DIAG_BUFFER_PARSER_H
/*============================================================================
  micro_diagbuffer_parser.h

  The parser functions translates any pointer dereferences to local
  address using mapping function set by the client before parsing.

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/micro_diagbuffer_parser.h#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ---------------------------------------------------------------- 
06/06/17   sp     Created new file to support microULog parser functionality.
==================================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "micro_ULog.h"

/** For sprintfs of 32 bit numbers, what is a safe minimum number of available bytes needed */
#define SNPRINTF_MIN_SIZE 14

/**
 * Macros for adding characters to strings. These operations are done a lot,
 * so these help make the code a little more compact and hopefully easier to read. 
 * To avoid lots of strcats and copies we mostly do snprintf at the end of the
 * existing strings. 
 */
//no argument "...." string only type sprintf string
#define SNPRINTF_AND_MOVEPTRS1(x) {unsigned int bytesOut; bytesOut = core_snprintf(&(parse_data->outputsz[parse_data->outOffset]), parse_data->outputsz_size, x); parse_data->outOffset += bytesOut;  parse_data->outputsz_size -= bytesOut;}
//one argument  "..%x.",value type string
#define SNPRINTF_AND_MOVEPTRS2(x, y) {unsigned int bytesOut; bytesOut = core_snprintf(&(parse_data->outputsz[parse_data->outOffset]), parse_data->outputsz_size, x, y); parse_data->outOffset += bytesOut; parse_data->outputsz_size -= bytesOut;}
//add a single char to the string. 
#define ADDCHAR_AND_MOVEPTRS(x) {parse_data->outputsz[parse_data->outOffset]=x; parse_data->outOffset+=1; parse_data->outputsz[parse_data->outOffset]=0; parse_data->outputsz_size--;}

/* Virtual address mapping function pointer if set by client */
typedef uint32 (*vaddr_fptr)( uint32 addr );

// Multipart messages introduce challenges for streaming environments. 
// In this environment the %m data won't be available so we won't try to read it.
typedef enum{
   MICRO_DIAGBUFFER_PARSE_DATA_STREAM = 0,
   MICRO_DIAGBUFFER_PARSE_READ_DATA
} MICRO_DIAGBUFFER_PARSE_ENVIRONMENT;


// MICRO_DIAGBUFFER_READ_TO_ASCII_DATA makes passing around the needed pointers and data easier.
// During a normal log read, only msgpart1 will be used. 
typedef struct
{
  char* msgpart1;
  char* msgpart2;
  uint32 part1_size;
  uint32 part2_size;
  char* outputsz;              //This pointer is frequently moved forward  
  unsigned int outputsz_size;  //and the size updated accordingly.
  unsigned int outOffset;      //ouputsz[outOffset] points to the end of the string. 
  MICRO_DIAGBUFFER_PARSE_ENVIRONMENT environment;
} MICRO_DIAGBUFFER_READ_TO_ASCII_DATA;

typedef struct 
{
  union
  {
    float fData;
    unsigned int uiData[2];
  } u;
} FloatConverter;

/**
 * <!-- micro_diagbuffer_FormatMsg_vaddr -->
 * 
 * @brief Prepare the message into our convenient parse_data structure.
 *
 * @param log : Pointer to the log the data came from
 * @param message : The binary log data to parse/convert to ascii. 
 * @param outputsz : Pointer to memory to place the ascii decoded log. 
 * @param outputSizeBytes : How much memory is available at outputsz. 
 * @param wordsConsumed : How many bytes of the log message were decoded.
 * @param mapping_fptr : mapping function set by client for address translation
 *
 * @return MICRO_ULOG_SUCCESS if parsing is successful
 */

micro_ULogResult micro_diagbuffer_FormatMsg_vaddr( micro_ULogHandle log,
                                  char *  message,
                                  char *  outputsz,
                                  uint32 outputSizeBytes,
                                  uint32 * wordsConsumed,
                                  vaddr_fptr mapping_fptr);

#ifdef __cplusplus
}
#endif
#endif              /* MICRO_DIAG_BUFFER_PARSER_H  */
