/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef READ_EXIF_DATA_H
#define READ_EXIF_DATA_H

#include <stdint.h>

extern int32_t get_exif_orientation(const unsigned char *frame_data, unsigned int frame_data_size);

#endif //READ_EXIF_DATA_H
