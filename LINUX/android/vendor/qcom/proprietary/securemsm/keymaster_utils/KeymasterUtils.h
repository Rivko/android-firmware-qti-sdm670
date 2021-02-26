/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef KEYMASTER_UTILS_KEYMASTERUTILS_H_
#define KEYMASTER_UTILS_KEYMASTERUTILS_H_

#include <hardware/keymaster_defs.h>
#include <QSEEComAPI.h>
#include <cstdlib>
#include <UniquePtr.h>
#include <linux/msm_ion.h>

#define KEYMASTER_MODULE_API_VERSION_3_0 (3)
#define KEYMASTER_MODULE_HAL_MINOR_VERSION (1)

namespace keymasterutils {

#define GUARD(code)                \
    do {                           \
       ret = (code);               \
       if (ret != 0) {             \
          return ret;              \
       };                          \
    }while(0)

#define GUARD_BAIL(code, status)                   \
    do {                                           \
        if ((code) || (status != KM_ERROR_OK)) {   \
            ALOGE("%s", __func__);                 \
            ALOGE("ret: %d", code);                \
            ALOGE("resp->status: %d", status);     \
            if (ret)                               \
                return ret;                        \
            else                                   \
                ret = (keymaster_error_t) status;  \
            return ret;                            \
        }                                          \
    }while(0)

typedef struct {
   int32_t ion_fd;
   int32_t ifd_data_fd;
   struct ion_handle_data ion_alloc_handle;
   uint32_t sbuf_len;
} sp_km_ion_info;

class KeymasterUtils {

public:
    KeymasterUtils();
    virtual ~KeymasterUtils();

    size_t km_memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

    keymaster_error_t initialize(size_t size);
    keymaster_error_t gk_initialize(size_t size);
    uint32_t getKeymasterVersion(void);
    uint32_t getKeymasterTaMinorVersion(void);
    uint32_t getKeymasterHalMinorVersion(void);
    void* qseecom_dev_init(void);
    keymaster_error_t send_cmd(void *send_buf,
            uint32_t sbuf_len, void *resp_buf, uint32_t rbuf_len);
    keymaster_error_t spu_init(void);
private:
    uint32_t major_version = 0;
    uint32_t minor_version = 0;
    uint32_t ta_major_version = 0;
    uint32_t ta_minor_version = 0;
    struct QSEECom_handle *qseecom = NULL;
    sp_km_ion_info ion_info;
    uint32_t spu_initialized = 0;
    uint32_t shared_buf_allocated = 0;
};

}
#endif /* KEYMASTER_UTILS_KEYMASTERUTILS_H_ */
