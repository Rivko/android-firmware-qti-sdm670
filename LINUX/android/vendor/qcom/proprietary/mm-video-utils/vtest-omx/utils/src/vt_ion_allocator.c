/*-------------------------------------------------------------------
Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
--------------------------------------------------------------------------*/

#include "vt_debug.h"
#include "vt_ion_allocator.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#undef LOG_TAG
#define LOG_TAG "VTEST_ION_ALLOCATOR"

#define SZ_4K 0x1000
#define SZ_1M 0x100000

#ifdef MASTER_SIDE_CP
#define CP_HEAP_ID ION_SECURE_HEAP_ID
#define SECURE_ALIGN SZ_4K
#else
#define CP_HEAP_ID ION_CP_MM_HEAP_ID
#define SECURE_ALIGN SZ_1M
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int vt_ion_allocate(int secure,
        void **client_ion_buffer, size_t size, int flags) {

    int rc = 0;
    struct ion_buffer_data *ion_buffer;
    *client_ion_buffer = NULL;
    int nIonFd = -1;

    if (size == 0) {
        VTEST_MSG_ERROR("buffer size passed : %zu invalid", size);
        return -EINVAL;
    }

    nIonFd = open("/dev/ion", O_RDONLY);
    if (nIonFd < 0) {
        VTEST_MSG_ERROR("opening ion device failed");
        return nIonFd;
    }

    ion_buffer = calloc(sizeof(struct ion_buffer_data), 1);
    if (!ion_buffer) {
        VTEST_MSG_ERROR("ion_buffer structure allocation failed");
        rc = -ENOMEM;
        goto close_fd;
    }

    ion_buffer->alloc_data.len = size;
    ion_buffer->alloc_data.flags = flags;
    if (secure) {
        ion_buffer->alloc_data.align = SECURE_ALIGN;
        ion_buffer->alloc_data.heap_id_mask = ION_HEAP(CP_HEAP_ID);
    } else {
        ion_buffer->alloc_data.align = SZ_4K;
        ion_buffer->alloc_data.heap_id_mask = ION_HEAP(ION_IOMMU_HEAP_ID);
    }

    rc = ioctl(nIonFd, ION_IOC_ALLOC, &ion_buffer->alloc_data);
    if (rc) {
        VTEST_MSG_ERROR("Failed to allocate ion memory");
        goto allocation_failed;
    }

    ion_buffer->fd_data.handle = ion_buffer->alloc_data.handle;
    ion_buffer->fd_data.fd = -1;
    rc = ioctl(nIonFd, ION_IOC_MAP, &ion_buffer->fd_data);
    if (rc) {
        VTEST_MSG_ERROR("Failed to MAP ion memory");
        goto map_failed;
    }

    if (!secure) {
        ion_buffer->data = mmap(NULL, size,
                PROT_READ | PROT_WRITE, MAP_SHARED, ion_buffer->fd_data.fd, 0);
        if (ion_buffer->data == MAP_FAILED) {
            VTEST_MSG_ERROR("Failed to mmap ion memory, errno is %s", strerror(errno));
            goto map_failed;
        }
    }
    goto close_fd;

map_failed:
    ion_buffer->data = NULL;
    vt_ion_free((void *)ion_buffer);
    ion_buffer = NULL;

allocation_failed:
    rc = -ENOMEM;
    if (ion_buffer) {
        free(ion_buffer);
    }

close_fd:
    close(nIonFd);
    *client_ion_buffer = (void *)ion_buffer;
    return rc;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int vt_ion_free(void* client_ion_buffer) {

    int rc = 0;
    struct ion_buffer_data *ion_buffer = (struct ion_buffer_data *)client_ion_buffer;
    int nIonFd = -1;

    if (!ion_buffer) {
        VTEST_MSG_ERROR("client_ion_buffer is NULL");
        return -EINVAL;
    }

    nIonFd = open("/dev/ion", O_RDONLY);
    if (nIonFd < 0) {
        VTEST_MSG_ERROR("opening ion device failed");
        return nIonFd;
    }

    if (ion_buffer->data) {
        rc = munmap(ion_buffer->data, ion_buffer->alloc_data.len);
        if (rc) {
            VTEST_MSG_ERROR("munmap failed for fd = %d", ion_buffer->fd_data.fd);
        }
    }
    rc |= ioctl(nIonFd, ION_IOC_FREE, &ion_buffer->fd_data.handle);

    if (ion_buffer->fd_data.fd >= 0) {
        close(ion_buffer->fd_data.fd);
    }
    close(nIonFd);
    free(ion_buffer);
    return rc;
}
