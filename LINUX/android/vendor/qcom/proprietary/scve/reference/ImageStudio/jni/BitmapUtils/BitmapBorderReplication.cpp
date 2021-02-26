/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <cstring>
#include <unistd.h>

#define  LOG_TAG    "Applog"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C"
{

//border replication: outputBitmap must be larger than inputBitmap

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_BorderReplicateJNILib_BorderReplicateRGBA32
(JNIEnv * env, jobject object, jobject inputBitmap, jobject outputBitmap);

}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_BorderReplicateJNILib_BorderReplicateRGBA32
(JNIEnv *env, jobject object, jobject inputBitmap, jobject outputBitmap)
{
    //restricting to ARGB8888
    uint32_t *_inputBitmap, *_outputBitmap;

    AndroidBitmapInfo inputInfo;
    AndroidBitmapInfo outputInfo;

    AndroidBitmap_getInfo(env, inputBitmap, &inputInfo);
    AndroidBitmap_lockPixels(env, inputBitmap, (void**)&_inputBitmap);
    AndroidBitmap_getInfo(env, outputBitmap, &outputInfo);
    AndroidBitmap_lockPixels(env, outputBitmap, (void**)&_outputBitmap);

    int inner_left = (outputInfo.width - inputInfo.width) / 2;
    int inner_right = inputInfo.width + inner_left;

    int inner_top = (outputInfo.height - inputInfo.height) / 2;
    int inner_bottom = inputInfo.height + inner_top;

    int inner_x;
    int inner_y;

    //try basic copy first
    for (int y = 0; y < (int)outputInfo.height; y++) {
        for (int x = 0; x < (int)outputInfo.width; x++) {

            inner_x = x - inner_left;
            inner_y = y - inner_top;

            //top left
            if (x < inner_left && y < inner_top){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[0];
            }
            //top middle
            else if (x >= inner_left && x < inner_right && y < inner_top){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[inner_x];
            }
            //top right
            else if (x >= inner_right && y < inner_top){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[inputInfo.width-1];
            }
            //middle left
            else if (x < inner_left && y >= inner_top && y < inner_bottom){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[inputInfo.width*inner_y];
            }
            //middle right
            else if (x >= inner_right && y >= inner_top && y < inner_bottom){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[inputInfo.width*inner_y + inputInfo.width-1];
            }
            //bottom left
            else if (x < inner_left && y >= inner_bottom){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[inputInfo.width*(inputInfo.height-1)];;
            }
            //bottom middle
            else if (x >= inner_left && x < inner_right && y >= inner_bottom){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[(inputInfo.height-1)*inputInfo.width + inner_x];
            }
            //bottom right
            else if (x >= inner_right && y >= inner_bottom){
                _outputBitmap[y*outputInfo.width + x] = _inputBitmap[(inputInfo.width)*inputInfo.height-1];
            }
            //middle
            else
            {
                //we do row copies in the middle ninth of the output bitmap
                if (x == inner_left) {
                    memcpy(&_outputBitmap[y*outputInfo.width + x], &_inputBitmap[inner_y*inputInfo.width + inner_x], inputInfo.stride);
                }
            }
        }
    }

    AndroidBitmap_unlockPixels(env, inputBitmap);
    AndroidBitmap_unlockPixels(env, outputBitmap);


}
