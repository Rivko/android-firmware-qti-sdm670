/*===========================================================================

  Copyright (c) 2012-2015 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifdef _MSC_VER
    // enable memory leak detection under msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

#ifdef TARGET_UEFI
    #include <Library/BaseMemoryLib.h>
    #include <Library/DebugLib.h>
    #define abort() ASSERT(FALSE)
    //CopyMem is overlap-safe in UEFI:
    #define memmove(dst, src, size) CopyMem(dst, src, size);
#endif
#include "DALSys.h"
#include "CoreVerify.h"
#include "CoreHeap.h"

#include "inmap.h"
#include "bytearray.h"
#include "rpm_resource.h"
#include "ULogFront.h"
#include "kvp.h"

extern ULogHandle rpm_log;

void rpm_resource_init(rpm_resource_t *self, rpm_resource_type type, unsigned id)
{
    memset(self, 0, sizeof(*self));
    self->resource_identifier = (((uint64_t)type) << 32) | id;
}

static rpm_key_data_t *rpm_resource_find_key(rpm_resource_t *self, unsigned key)
{
    unsigned low = 0, high = self->num_keys - 1;

    if(!self->num_keys)
        return NULL;

    while(high < self->num_keys && high >= low)
    {
        unsigned mid = (low + high) / 2;
        unsigned current_key = self->keys[mid].key;

        if(current_key < key)
            low = mid + 1;
        else if(current_key > key)
            high = mid - 1;
        else
            return &self->keys[mid];
    }

    return NULL;
}

static rpm_key_data_t *rpm_resource_add_key(rpm_resource_t *self, unsigned key)
{
    unsigned i, old_num_keys;

    old_num_keys = self->num_keys;
    self->num_keys++;
    self->keys = Core_Realloc(self->keys, self->num_keys * sizeof(rpm_key_data_t));
    CORE_VERIFY_PTR(self->keys);

    for(i = 0; i < old_num_keys; ++i)
    {
        rpm_key_data_t *key_data = &self->keys[i];

        CORE_VERIFY_PTR(key_data);
        if(key_data->key > key)
        {
            // Found entries that belong after us.  Move them and insert here.
            memmove(&self->keys[i+1], key_data, (old_num_keys - i) * sizeof(rpm_key_data_t));
            memset(key_data, 0, sizeof(rpm_key_data_t));
            key_data->key = key;
            return key_data;
        }
    }

    // Must be largest in the list; insert at the end.
    memset(&self->keys[i], 0, sizeof(rpm_key_data_t));
    self->keys[i].key = key;
    return &self->keys[i];
}
                
static rpm_request_t *rpm_key_get_local_request(rpm_key_data_t *key_data, rpm_set_type set)
{
    switch(set)
    {
        case RPM_ACTIVE_SET:
            return &key_data->active_local;

        case RPM_SLEEP_SET:
            return &key_data->sleep_local;

        case RPM_NEXT_ACTIVE_SET:
            return &key_data->nas_local;

        default:
            abort();
    }
    return NULL;
}

static rpm_request_t *rpm_key_get_cached_request(rpm_key_data_t *key_data, rpm_set_type set)
{
    switch(set)
    {
        case RPM_ACTIVE_SET:
            return &key_data->active_at_rpm;

        case RPM_SLEEP_SET:
            return &key_data->sleep_at_rpm;

        case RPM_NEXT_ACTIVE_SET:
            return &key_data->active_at_rpm;

        default:
            abort();
    }
    return NULL;
}

static void rpm_full_invalidate(rpm_set_type set, rpm_resource_t *self)
{
    unsigned i;
    //set known keys to not dirty and not valid. We are invalidating everything about this set
    for(i = 0; i < self->num_keys; ++i)
    {
        rpm_key_data_t *key_data = &self->keys[i];
        rpm_request_t *local = rpm_key_get_local_request(key_data, set);
        rpm_request_t *rpm   = rpm_key_get_cached_request(key_data, set);

        if ( local && rpm ) //KW fix
        {
            if(!local->data)
                local->data = bytearray_create(0);

            rpm->valid = false;
            local->valid = false;
            key_data->dirty[set] = 0;

            bytearray_clear(local->data);
        }
    }

    //set the dirty for this set to 1, so we know we have to send
    self->dirty[set] = 1;
}


static void rpm_key_invalidate(rpm_key_data_t *key_data, rpm_set_type set)
{
    rpm_request_t *local = rpm_key_get_local_request(key_data, set);
    rpm_request_t *rpm   = rpm_key_get_cached_request(key_data, set);

    if ( local && rpm ) //KW fix
    {
        if(!local->data)
            local->data = bytearray_create(0);

        // Here we flag the resource as invalid, but dirty bit true.  This means
        // that this key has been invalidated since the last time we synchronized
        // with the RPM.
        //
        // Four states possible here:
        //   - valid-clean   -> invalid-dirty
        //   - valid-dirty   -> invalid-dirty
        //   - invalid-dirty -> invalid-dirty
        //
        //   - invalid-clean -> invalid-clean
        if(rpm->valid)
        key_data->dirty[set] = 1;

        local->valid = false;
        bytearray_clear(local->data);
    }
}

static void rpm_key_update(rpm_key_data_t *key_data, rpm_set_type set, unsigned length, void *data)
{
    rpm_request_t *local_value = rpm_key_get_local_request(key_data, set);
    rpm_request_t *rpm_value = rpm_key_get_cached_request(key_data, set);

    if ( local_value && rpm_value ) //KW fix
    {
        if(!local_value->data)
            local_value->data = bytearray_create(0);
        if(!rpm_value->data)
            rpm_value->data = bytearray_create(0);

        bytearray_clear(local_value->data);
        bytearray_put(local_value->data, data, length);

        // Four states possible here:
        //   - valid-clean   -> valid, compare for clean
        //   - valid-dirty   -> valid, compare for clean
        //   - invalid-dirty -> valid, compare for clean
        //   - invalid-clean -> valid, always dirty [RPM had no request prior]

        // Handle the invalid-clean case.
        if(!rpm_value->valid)
        {
            local_value->valid = 1;
            key_data->dirty[set] = 1;
        }
        else
        {
        void *local_data, *rpm_data;
        unsigned local_size;

            local_value->valid = 1;

            // Compare local and rpm values to determine dirty status.
            local_data = bytearray_at(local_value->data, 0);
            local_size = bytearray_size(local_value->data);
            rpm_data   = bytearray_at(rpm_value->data, 0);
            if(local_data && rpm_data) //KW fix
            {
                if((local_size != bytearray_size(rpm_value->data)) ||
                   memcmp(local_data, rpm_data, local_size))
                {
                   key_data->dirty[set] = 1;
                }
                else
                {
                   key_data->dirty[set] = 0;
                }
            } //KW fix
        }
    } //KW fix
}

unsigned rpm_resource_get_request_size(rpm_set_type set, rpm_resource_t *self)
{
    unsigned num_bytes = 0, i;
    for (i = 0; i < self->num_keys; i++)
    {
        if(self->keys[i].dirty[set])
        {
            rpm_key_data_t *key_data = &self->keys[i];
            switch(set)
            {
                case RPM_ACTIVE_SET:
                    num_bytes += (8 + bytearray_size(key_data->active_local.data));
                    break;

                case RPM_SLEEP_SET:
                    num_bytes += (8 + bytearray_size(key_data->sleep_local.data));
                    break;

                case RPM_NEXT_ACTIVE_SET:
                    num_bytes += (8 + bytearray_size(key_data->nas_local.data));
                    break;

                default:
                    abort();

            }
        }
    }

    if(num_bytes > 0)
    {
        num_bytes += 28;
    }

    return num_bytes; //add the size of the RPM message Header
}

void rpm_resource_update(rpm_resource_t *self, rpm_set_type set, kvp_t *request_data)
{
        // Check for the invalidate case.
        if(!request_data)
        {
            rpm_full_invalidate(set, self); 
        }
    else
    {
        void *data;
        rpm_key_data_t *key_data;
        unsigned key, length;
        unsigned dirty = 0;

        // Handle each key one at a time.
        while(kvp_get(request_data, &key, &length, (const char **)&data))
        {
            /* Don't allow individual KVP's that are too long. */
            CORE_LOG_VERIFY( 
                length < MAX_KVP_LENGTH,
                ULOG_RT_PRINTF_3(rpm_log, "rpm_resource_update (resource: 0x%08x) (data:0x%08x) (length: 0x%08x) KVP too large", self, request_data, length)
            );
                        
            key_data = rpm_resource_find_key(self, key);

            if(!key_data)
            {
                key_data = rpm_resource_preallocate(self, key, length);
            }

            if(!length)
                rpm_key_invalidate(key_data, set);
            else
                rpm_key_update(key_data, set, length, data);

            dirty |= key_data->dirty[set];
        }

        self->dirty[set] = dirty;
}
}

void rpm_key_compose_data(rpm_key_data_t *key_data, rpm_set_type set, kvp_t *message_data)
{
    rpm_request_t *local_value = rpm_key_get_local_request(key_data, set);
    rpm_request_t *rpm_value   = rpm_key_get_cached_request(key_data, set);

    if ( local_value && rpm_value ) //KW fix
    {
        if(!local_value->data)
            local_value->data = bytearray_create(0);
        if(!rpm_value->data)
            rpm_value->data = bytearray_create(0);

    if(!key_data->dirty[set])
            return;

        if(!local_value->valid)
        {
            kvp_put(message_data, key_data->key, 0, 0);

            ULOG_RT_PRINTF_1(rpm_log, "\trpm_invalidated_key (key: 0x%08x)", key_data->key);
        }
        else
        {
            unsigned length = bytearray_size(local_value->data);
            void *local_data = bytearray_at(local_value->data, 0);

            CORE_VERIFY_PTR(local_data);
            kvp_put(message_data, key_data->key, length, local_data);

            ULOG_RT_PRINTF_2(rpm_log, "\trpm_dirty_key (key: 0x%08x) (length: 0x%08x)", key_data->key, length);
        }

        rpm_value->valid = local_value->valid;
        bytearray_clear(rpm_value->data);
        if(local_value->valid)
        {
          unsigned length = bytearray_size(local_value->data);
          void  *local_data = bytearray_at(local_value->data, 0);

          CORE_VERIFY_PTR(local_data); 
          bytearray_put(rpm_value->data, local_data, length);
        }

        key_data->dirty[set] = 0;
     }//KW Fix end
}

bool rpm_resource_compose_message(rpm_resource_t *self, rpm_set_type set, kvp_t *message_data)
{
    kvp_clear(message_data);

    // If we're dirty, compose the message from each key's data
    if(self->dirty[set])
        {
        unsigned valid_keys = 0, i;
        rpm_key_data_t *key_data = self->keys;

        for(i = 0; i < self->num_keys; ++key_data, ++i)
            {
            if((!key_data->dirty[set] && rpm_key_get_cached_request(key_data, set)->valid) ||
                                         rpm_key_get_local_request(key_data, set)->valid)
                {
                    valid_keys++;
                }

            // If this key is dirty, grab its data.
            if(key_data->dirty[set])
                rpm_key_compose_data(key_data, set, message_data);
        }

        // Wrote out all the dirty keys, so "clean up."
        self->dirty[set] = 0;

        if(!valid_keys)
        {
            ULOG_RT_PRINTF_0(rpm_log, "\trpm_fully_invalidate");
            return true;
        }
    }

    return false;
}

#define RESERVE_BUFFER(x) \
    do { \
        if(!key_data->x.data) \
        { \
            key_data->x.data = bytearray_create(expected_length); \
        } \
        else \
        { \
            bytearray_reserve(key_data->x.data, expected_length); \
        } \
    } while(0);

rpm_key_data_t *rpm_resource_preallocate(rpm_resource_t *self, unsigned key, unsigned expected_length)
{
    rpm_key_data_t *key_data = rpm_resource_find_key(self, key);
    if(!key_data)
        key_data = rpm_resource_add_key(self, key);

    // Preallocate all of the possible buffers for this key.
    RESERVE_BUFFER(active_at_rpm);
    RESERVE_BUFFER(active_local);
    RESERVE_BUFFER(sleep_at_rpm);
    RESERVE_BUFFER(sleep_local);
    RESERVE_BUFFER(nas_local);

    return key_data;
}

