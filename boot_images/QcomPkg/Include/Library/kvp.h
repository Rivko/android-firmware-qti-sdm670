/*===========================================================================

  Copyright (c) 2014, Qualcomm Technologies, Inc.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef KVP_H
#define KVP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define       MAX_KVP_LENGTH    100
typedef struct kvp_s kvp_t;

kvp_t *kvp_create(unsigned buffer_size);
kvp_t *kvp_frombuffer(unsigned size, const char *buffer);
void kvp_swapbuffer(kvp_t *self, const char *bytes, unsigned buffer_size);
void kvp_destroy(kvp_t *self);

// Returns nonzero when there are no more pairs to read.
unsigned kvp_eof(const kvp_t *self);

void kvp_put(kvp_t *self, unsigned  type, unsigned  length, const void  *value);
unsigned kvp_get(kvp_t *self, unsigned *type, unsigned *length, const char **value);

void kvp_reset(kvp_t *self); // reset the read pointer, don't touch data
void kvp_clear(kvp_t *self); // clear all data

unsigned kvp_get_raw_size(const kvp_t *self);
const char *kvp_get_raw_bytes(const kvp_t *self);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // KVP_H

