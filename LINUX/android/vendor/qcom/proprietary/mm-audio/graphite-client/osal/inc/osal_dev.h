/*
 * osal_dev.h
 *
 * This file defines device related operation helper functions.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_DEV_H__
#define __OSAL_DEV_H__

/* Client communicates with osal device using a handle of this type. */
typedef void *osal_dev_t;

/*
 * osal_dev_open - Open osal device structure
 *
 * @dev:       Pointer to osal device structure
 * @dev_name:  Device name to be open
 * @flags:     Flags to use for device to open
 * Returns  0 upon success and error code otherwise
 *
 * Prerequisites
 *      None
 */
int32_t osal_dev_open(osal_dev_t *dev, char *dev_name, int32_t flags);

/*
 * osal_dev_close - Close osal device structure.
 *
 * NOTE: Client must ensure device will not be used after it is closed.
 *
 * @dev:    Osal device structure.
 *
 * Returns 0 upon success and error code otherwise
 *
 * Prerequisites
 *      Device must be created.
 */
int32_t osal_dev_close(osal_dev_t dev);

/*
 * osal_dev_read - Read buffer from osal device.
 *
 * @dev:        Osal device structure.
 * @buf:        Buffer pointer
 * @size:       Buffer size in bytes
 *
 * Returns 0 upon success and error code otherwise
 *
 * Prerequisites
 *      Device must be created.
 */
int32_t osal_dev_read(osal_dev_t dev, void *buf, size_t size);

/*
 * osal_dev_write - Write on buffer to osal device structure
 *
 * @dev:         Osal device structure.
 * @buf:         Buffer pointer
 * @size:        Buffer size in bytes
 * Returns 0 upon success and error code otherwise
 *
 * Prerequisites
 *      Device must be created.
 */
int32_t osal_dev_write(osal_dev_t dev, void *buf, size_t size);

#endif /* __OSAL_DEV_H__ */
