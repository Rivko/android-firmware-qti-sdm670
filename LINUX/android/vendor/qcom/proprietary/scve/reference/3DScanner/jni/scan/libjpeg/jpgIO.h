/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __JPG_IO_H__
#define __JPG_IO_H__
unsigned char * jpgLoad (char * filename, int * width, int * height, bool flip = true);
void jpgWrite (char * filename, unsigned char* pPixels, int imageWidth, int imageHeight, bool flip = true);
#endif
