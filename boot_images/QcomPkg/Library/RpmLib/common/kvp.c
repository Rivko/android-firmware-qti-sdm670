/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#include "DALSys.h"
#include "kvp.h"
#include "bytearray.h"
#include "CoreVerify.h"
#include "CoreHeap.h"
#include "ULogFront.h"

#define ALIGN_32(n) (((n)+3)&~3)

extern ULogHandle       rpm_log;

struct kvp_s
{
    bytearray_t *buffer;
    unsigned     read_idx;
};

kvp_t *kvp_create(unsigned buffer_size)
{
    kvp_t *self = Core_Malloc(sizeof(kvp_t));
    CORE_VERIFY_PTR(self);
    memset(self, 0, sizeof(kvp_t));
    self->buffer = bytearray_create(buffer_size);
    return self;
}

kvp_t *kvp_frombuffer(unsigned size, const char *buffer)
{
    kvp_t *self = Core_Malloc(sizeof(kvp_t));
    CORE_VERIFY_PTR(self);
    memset(self, 0, sizeof(kvp_t));
    self->buffer = bytearray_frombuffer(buffer, size);
    return self;
}

void kvp_swapbuffer(kvp_t *self, const char *bytes, unsigned buffer_size)
{
    bytearray_swapbuffer(self->buffer, bytes, buffer_size);
    self->read_idx = 0;
}

void kvp_destroy(kvp_t *self)
{
    bytearray_destroy(self->buffer);
    Core_Free(self);
}

unsigned kvp_eof(const kvp_t *self)
{
    return (self->read_idx >= bytearray_size(self->buffer));
}

void kvp_put(kvp_t *self, unsigned type, unsigned length, const void *value)
{
    const char padding[] = "\x00\x00\x00";
    unsigned required_pad = (ALIGN_32(length & 0x3)) - (length & 0x3);

    /* Don't allow individual KVP's that are too long. */
    CORE_LOG_VERIFY( 
        length < MAX_KVP_LENGTH,
        ULOG_RT_PRINTF_3(rpm_log, "kvp_put (kvp: 0x%08x) (type:0x%08x) (length: 0x%08x) KVP too large", self, type, length)
    );

    bytearray_put(self->buffer, &type,   sizeof(type));
    bytearray_put(self->buffer, &length, sizeof(length));
    bytearray_put(self->buffer,  value,  length);

    if(required_pad)
        bytearray_put(self->buffer, padding, required_pad);
}

static unsigned kvp_bytes_available(kvp_t *self)
{
    unsigned bufsize = bytearray_size(self->buffer);
    if(self->read_idx >= bufsize)
        return 0;
    else
        return bufsize - self->read_idx;
}

typedef struct
{
    unsigned   type;
    unsigned   length;
    const char value[];
} tlv_header;

unsigned kvp_get(kvp_t *self, unsigned *type, unsigned *length, const char **value)
{
    tlv_header *head;
    
    // If the buffer doesn't hold another header, we fail.
    if(kvp_bytes_available(self) < sizeof(tlv_header))
        return 0;

    head = (tlv_header *)bytearray_at(self->buffer, self->read_idx);
    CORE_VERIFY_PTR(head);
    if ( head ) //KW fix
    {
        self->read_idx  += sizeof(tlv_header);

        *type   = head->type;
        *length = head->length;

        // We can have keys with 'null' values.
        if(!head->length)
        {
            *value = NULL;
            return 1;
        }

        // Not null, but ensure that the buffer has the data it's promising.
        if(kvp_bytes_available(self) < ALIGN_32(head->length))
            return 0;

        *value  = head->value;
        self->read_idx += ALIGN_32(head->length);
    }
    return 1;
}

void kvp_reset(kvp_t *self)
{
    self->read_idx = 0;
}

void kvp_clear(kvp_t *self)
{
    bytearray_clear(self->buffer);
    kvp_reset(self);
}

unsigned kvp_get_raw_size(const kvp_t *self)
{
    return bytearray_size(self->buffer);
}

const char *kvp_get_raw_bytes(const kvp_t *self)
{
    return bytearray_at(self->buffer, 0);
}

