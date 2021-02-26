/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "exifhelper.h"

/*
 * Read exif orientaiton data using libexif, in case no exif info available, return -1
 * Exif orientation
 * 0th Row      0th Column
 * 1  top          left side
 * 2  top          right side
 * 3  bottom       right side
 * 4  bottom       left side
 * 5  left side    top
 * 6  right side   top
 * 7  right side   bottom
 * 8  left side    bottom
 */
int32_t get_exif_orientation(const unsigned char *frame_data, unsigned int frame_data_size) {
    int32_t exif_orientation = 0;
    int32_t frame_orientation = 0;

    #if 0
    ExifData *exif_data = NULL;
    exif_data = exif_data_new_from_data(frame_data, frame_data_size);

    if (exif_data) {
        ExifByteOrder byteOrder = exif_data_get_byte_order(exif_data);
        ExifEntry *exif_entry = exif_data_get_entry(exif_data, EXIF_TAG_ORIENTATION);

        if (exif_entry) {
           exif_orientation = exif_get_short(exif_entry->data, byteOrder);
        }

        exif_data_free(exif_data);

        if(exif_orientation != 0) {
            switch(exif_orientation) {
                case 6:
                    frame_orientation = 90;
                    break;
                case 3:
                    frame_orientation = 180;
                    break;
                case 8:
                    frame_orientation = 270;
                    break;
                default:
                    frame_orientation = 0;
                    break;
            }
        }
    }
    #endif

    return frame_orientation;
}
