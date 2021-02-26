/*============================================================================
  @file direct_channel.h

  @brief
  direct_channel class definition.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#pragma once


#include <cstdlib>
#include <cutils/native_handle.h>
#include <hardware/gralloc1.h>

/*============================================================================
 * Class direct_channel
 *=============================================================================*/
class direct_channel {
public:
    direct_channel(const struct native_handle *mem_handle,
        const size_t mem_size);
    direct_channel(const direct_channel &copy);
    ~direct_channel();
    int get_low_lat_handle();
    int get_client_channel_handle();
    int get_buffer_fd();
    bool is_same_memory(const struct native_handle *mem_handle);
private:
    struct native_handle* mem_native_handle;
    void* buffer_ptr;
    size_t buffer_size;
    int sns_low_lat_handle;

    /* Assigned by the constructor. If this field is 0, then the object is invalid */
    int client_channel_handle;

    /* Note: valid range for Android channel handles is (>0, <INT32_MAX) */
    static int client_channel_handle_counter;

    /* Gralloc1 device singleton */
    static gralloc1_device_t* gralloc1_device;
    static GRALLOC1_PFN_RETAIN gralloc1_retain;
    static GRALLOC1_PFN_RELEASE gralloc1_release;
    static GRALLOC1_PFN_LOCK gralloc1_lock;
    static GRALLOC1_PFN_UNLOCK gralloc1_unlock;
    static GRALLOC1_PFN_GET_BACKING_STORE gralloc1_GetBackingStore;

    int init_gralloc1_dev(void);
};
