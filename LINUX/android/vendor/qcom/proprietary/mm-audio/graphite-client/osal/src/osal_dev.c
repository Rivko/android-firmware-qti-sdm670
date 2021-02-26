/*
 * osal_dev.c
 *
 * This file has implementation of device related operation helper functions.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <error.h>
#include <fcntl.h>
#include <errno.h>
#include "osal_dev.h"
#include "osal_log.h"

typedef struct osal_int_dev {
    int32_t fd;
} osal_int_dev_t;

int32_t osal_dev_open(osal_dev_t *dev, char *dev_name, int32_t flags)
{
    int32_t rc = 0;
    osal_int_dev_t *the_dev;

    if (dev == NULL || dev_name == NULL) {
        OSAL_LOGE("%s: dev or dev_name is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_dev = (osal_int_dev_t *)malloc(sizeof(osal_int_dev_t));
    if (the_dev == NULL) {
        OSAL_LOGE("%s: failed to allocate dev memory\n", __func__);
        rc = -ENOMEM;
        goto done;
    }

    the_dev->fd = open(dev_name, flags);
    if (the_dev->fd == -1) {
        OSAL_LOGE("%s: open failed: %d", __func__, errno);
        rc = -errno;
        goto err_open;
    }

    *dev = the_dev;
    goto done;

err_open:
   free(the_dev);

done:
   return rc;
}

int32_t osal_dev_close(osal_dev_t dev)
{
    int32_t rc;
    osal_int_dev_t *the_dev;

    if (dev == NULL) {
        OSAL_LOGE("%s: dev is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_dev = (osal_int_dev_t *)dev;
    rc = close(the_dev->fd);
    if (rc == -1) {
        OSAL_LOGE("%s: close failed: %d", __func__, errno);
        rc = -errno;
    }
    free(the_dev);

done:
   return rc;
}

int32_t osal_dev_read(osal_dev_t dev, void *buf, size_t size)
{
    int32_t rc;
    osal_int_dev_t *the_dev;

    if (dev == NULL) {
        OSAL_LOGE("%s: dev is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_dev = (osal_int_dev_t *)dev;
    rc =  read(the_dev->fd, buf, size);
    if (rc == -1) {
        OSAL_LOGE("%s: read failed: %d", __func__, errno);
        rc = -errno;
    }

done:
    return rc;
}

int32_t osal_dev_write(osal_dev_t dev, void *buf, size_t size)
{
    int32_t rc;
    osal_int_dev_t *the_dev;

    if (dev == NULL) {
        OSAL_LOGE("%s: dev is NULL\n", __func__);
        rc = -EINVAL;
        goto done;
    }

    the_dev = (osal_int_dev_t *)dev;
    rc =  write(the_dev->fd, buf, size);
    if (rc == -1) {
        OSAL_LOGE("%s: write failed: %d", __func__, errno);
        rc = -errno;
    }

done:
    return rc;
}
