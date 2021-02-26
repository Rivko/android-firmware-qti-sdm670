#pragma once
/*============================================================================
  @file sns_log.h

  @brief
  Debug service wrapper for sensors framework

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

#if defined __GNUC__
  #define SNS_PACK(x)       x __attribute__((__packed__))
#elif defined __GNUG__
  #define SNS_PACK(x)       x __attribute__((__packed__))
#elif defined __arm
  #define SNS_PACK(x)       __packed x
#elif defined _WIN32
  #define SNS_PACK(x)       x /* Microsoft uses #pragma pack() prologue/epilogue */
#else
  #define SNS_PACK(x)       x __attribute__((__packed__))
#endif

typedef SNS_PACK(struct)
{
  uint16_t len;     /* Specifies the length, in bytes of the
           entry, including this header. */

  uint16_t code;      /* Specifies the log code for the entry as
           enumerated above. Note: This is
           specified as word to guarantee size. */

  uint32_t ts[2];     /* The system timestamp for the log entry. The
           upper 48 bits represent elapsed time since
           6 Jan 1980 00:00:00 in 1.25 ms units. The
           low order 16 bits represent elapsed time
           since the last 1.25 ms tick in 1/32 chip
           units (this 16 bit counter wraps at the
           value 49152). */
}
sns_log_hdr_type;

typedef uint16_t sns_log_code_type;

// See log_status in log.h
bool sns_log_status(sns_log_code_type code);

// See log_set_length in log.h
void sns_log_set_length(void *ptr, unsigned int length);

// See log_set_code in log.h
void sns_log_set_code(void *ptr, sns_log_code_type code);

// See log_set_timestamp in log.h
void sns_log_set_timestamp(void *ptr);

// log_submit in log.h
bool sns_log_submit(void *ptr);


