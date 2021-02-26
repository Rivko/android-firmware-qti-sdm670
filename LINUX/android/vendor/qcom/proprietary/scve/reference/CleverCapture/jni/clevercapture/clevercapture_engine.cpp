/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>

#include "scve/scveTypes.hpp"
#include "scve/scveContext.hpp"
#include "scve/scveCleverCapture.hpp"

using namespace SCVE;

#define LOG_TAG    "clevercapturejni"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG ,__VA_ARGS__)

//#define DUMPY
#define DUMPNUM 300
#define DUMPFILENAME "/sdcard/cc_frames.yuv"

#ifdef DUMPY
int g_dumpFrameNum = 0;
unsigned char *g_dumpbuf = 0;
#endif

typedef struct _clevercapture_s {
    Context *pContext;
    CleverCapture *pCleverCapture;

    int previewWidth;
    int previewHeight;
    int burstNum;

    Image image;
} clevercapture_s;

clevercapture_s *gInstance = 0;

extern "C" {
    JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nInit (
            JNIEnv *env, jobject obj, jint width, jint height, jint burst_num, jint mode );
    JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nRegROI (
                JNIEnv *env, jobject obj, jint x, jint y, jint w, jint h );
    JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nUnregROI (
                JNIEnv *env, jobject obj, jint RoiIdx );
    JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nProcess (
                JNIEnv *env, jobject obj, jbyteArray data, jint w, jint h, jobject socre );
    JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nSubmitForEval (
                JNIEnv *env, jobject obj, jint frameId);
    JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nGetPostProcessResult (
                JNIEnv *env, jobject obj, jintArray scores, jintArray frameIds, jint RoiIdx );
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nRelease (
                JNIEnv *env, jobject obj );
}

void releaseInstance(clevercapture_s *pinstance);
int initInstance(clevercapture_s **ppinstance, int width, int height, int burst_num);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int initInstance(clevercapture_s **ppinstance, int width, int height, int burst_num, int mode)
{
    clevercapture_s *tmpInstance = 0;
    tmpInstance = (clevercapture_s *)malloc(sizeof(clevercapture_s));

    OperationMode omode = SCVE_MODE_DEFAULT;
    if(tmpInstance) {
        switch(mode) {
        case 1:
            omode = SCVE_MODE_PERFORMANCE;
            break;
        case 2:
            omode = SCVE_MODE_CPU_OFFLOAD;
            break;
        case 3:
            omode = SCVE_MODE_LOW_POWER;
            break;
        }
        tmpInstance->pContext = Context::newInstance(omode);
        if(tmpInstance->pContext) {
            tmpInstance->image.sDim.nWidth = width;
            tmpInstance->image.sDim.nHeight = height;
            tmpInstance->image.sDim.nStride = width;
            tmpInstance->image.sDim.nStrides[0] = width;
            tmpInstance->image.sDim.nStrides[1] = width;
            tmpInstance->image.sDim.nStrides[2] = width;

            tmpInstance->image.sFormat = SCVE_COLORFORMAT_GREY_8BIT;
            tmpInstance->image.nChannels = 1;
            tmpInstance->image.pPtr = 0;
            tmpInstance->burstNum = burst_num;
            tmpInstance->pCleverCapture = CleverCapture::newInstance(tmpInstance->pContext,
                    tmpInstance->image.sDim, tmpInstance->image.sFormat, tmpInstance->burstNum);

            if(tmpInstance->pCleverCapture) {
                *ppinstance = tmpInstance;
                return 0;
            }
        }
    }

    releaseInstance(tmpInstance);
    return -1;
}

void releaseInstance(clevercapture_s *pinstance)
{
    if(pinstance) {
        if(pinstance->pCleverCapture) {
            LOGD("before call CleverCapture::deleteInstance");
            CleverCapture::deleteInstance(pinstance->pCleverCapture);
            LOGD("after call CleverCapture::deleteInstance");
        }
        if(pinstance->pContext) {
            Context::deleteInstance(pinstance->pContext);
        }

        free(pinstance);
    }
}

JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nInit (
        JNIEnv *env, jobject obj, jint width, jint height, jint burst_num, jint mode )
{
    int rc = 0;
    LOGD("nInit ++ width=%d height=%d burst_num=%d mode=%d gInstance=%p", width, height, burst_num, mode, gInstance);

    if(!gInstance)
        initInstance(&gInstance, width, height, burst_num, mode);
    LOGD("gInstance=%p", gInstance);

#ifdef DUMPY
    if(g_dumpbuf) {
        LOGD("g_dumpbuf is not null, free it first");
        free(g_dumpbuf);
        g_dumpbuf = 0;
    }

    g_dumpbuf = (unsigned char *)malloc(width * height * DUMPNUM * 3 / 2);
    if(g_dumpbuf == 0) {
        LOGD("malloc for g_dumpbuf fail");
    }
    g_dumpFrameNum = 0;
#endif

    LOGD("nInit -- %d", rc);
    return rc;
}

JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nRegROI (
            JNIEnv *env, jobject obj, jint x, jint y, jint w, jint h )
{
    int rc = -1;
    unsigned int idx = 0;
    LOGD("nRegROI ++ x=%d y=%d w=%d h=%d gInstance=%p", x, y, w, h, gInstance);

    if(gInstance) {
        Rect rect;
        rect.nLocX = x;
        rect.nLocY = y;
        rect.nWidth = w;
        rect.nHeight = h;
        if(!gInstance->pCleverCapture->RegisterROI_Sync(rect, &idx)) {
            LOGD("RegisterROI_Sync success, index=%d", idx);
            rc = (int)idx;
        }
    }

    LOGD("nRegROI -- %d", rc);
    return rc;
}

JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nUnregROI (
            JNIEnv *env, jobject obj, jint RoiIdx )
{
    int rc = -1;
    LOGD("nUnregROI ++ RoiIdx=%d gInstance=%p", RoiIdx, gInstance);

    if(gInstance && !gInstance->pCleverCapture->UnregisterROI_Sync(RoiIdx)) {
        rc = 0;
    }
    LOGD("nUnregROI-- %d", rc);
    return rc;
}

JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nProcess (
            JNIEnv *env, jobject obj, jbyteArray data, jint w, jint h, jobject socre )
{
    int rc = -1;
    LOGD("nProcess ++ data=%p w=%d h=%d score=%p gInstance=%p", data, w, h, socre, gInstance);

    if(gInstance) {
        cc_score_t score;
        gInstance->image.pPtr = (uint8_t *)env->GetDirectBufferAddress(data);

#ifdef DUMPY
        if(g_dumpbuf && g_dumpFrameNum < DUMPNUM) {
            memcpy(g_dumpbuf + g_dumpFrameNum * w * h * 3 / 2, gInstance->image.pPtr, w * h * 3 / 2);
            g_dumpFrameNum++;
            LOGD("g_dumpFrameNum=%d", g_dumpFrameNum);
        }
#endif

        rc = gInstance->pCleverCapture->isInterestingFrame_Sync(&gInstance->image, &score);
        LOGD("isInterestingFrame_Sync rc=%x isInteresting=%d frameId=%d reserved=%d",
                rc, score.is_interesting, score.frame_id, score.reserved);
        if(rc) {
            LOGE("isInterestingFrame_Sync fail error code %x", rc);
        } else {
            jclass clsScore = env->FindClass("com/qualcomm/qti/clevercapture/hardware/CleverCaptureScore");
            jfieldID is_interesting = env->GetFieldID(clsScore, "isInteresting", "Z");
            jfieldID frame_id = env->GetFieldID(clsScore, "frameId", "I");
            jfieldID reserved = env->GetFieldID(clsScore, "reserved", "I");

            env->SetBooleanField(socre, is_interesting, score.is_interesting);
            env->SetIntField(socre, frame_id, score.frame_id);
            env->SetIntField(socre, reserved, score.reserved);
        }
    }

    LOGD("nProcess -- %d", rc);
    return rc;
}
JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nSubmitForEval (
            JNIEnv *env, jobject obj, jint frameId)
{
    int rc = -1;
    LOGD("nSubmitForEval ++ frameId=%d gInstance=%p", frameId, gInstance);

    if(gInstance && !gInstance->pCleverCapture->submitForEval((uint32_t)frameId)) {
        rc = 0;
    }

    LOGD("nSubmitForEval -- %d", rc);
    return rc;
}
JNIEXPORT int JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nGetPostProcessResult (
            JNIEnv *env, jobject obj, jintArray scores,jintArray frameIds, jint RoiIdx )
{
    int rc = -1;
    LOGD("nGetPostProcessResult ++ scores=%p RoiIdx=%d gInstance=%p", scores, RoiIdx, gInstance);

    if(gInstance) {
        jboolean isCopy;
        jsize jFrameIdsSize = env->GetArrayLength(frameIds);
        jint *jFrameIds = env->GetIntArrayElements(frameIds, &isCopy);

        jsize jScoresSize = env->GetArrayLength(scores);
        jint *jScores = env->GetIntArrayElements(scores, &isCopy);

        ppm_pkt_t *frameList = new ppm_pkt_t[jFrameIdsSize];
        if(frameList) {
            for(int i = 0; i < jFrameIdsSize; i++) {
                frameList[i].frame_id = jFrameIds[i];
                frameList[i].rank = (uint32_t *)jScores + i * gInstance->burstNum;
            }

            //for(int i = 0; i < jScoresSize; i++) {
            //    LOGD("before getCleverCaptures i = %d rank = %d", i, jScores[i]);
            //}
            Status result = gInstance->pCleverCapture->getCleverCaptures(frameList, jFrameIdsSize, RoiIdx);
            //for(int i = 0; i < jScoresSize; i++) {
            //    LOGD("after getCleverCaptures i = %d rank = %d", i, jScores[i]);
            //}
            if(result != SCVE_SUCCESS) {
                LOGE("getCleverCaptures fail %d", result);
            } else {
                rc = 0;
            }
            delete frameList;
        } else {
            LOGE("Memory allocation for framelist fail");
        }

        if(isCopy) {
            env->SetIntArrayRegion(scores, 0, jScoresSize, jScores);
        }

        env->ReleaseIntArrayElements(frameIds, jFrameIds, JNI_ABORT);
        env->ReleaseIntArrayElements(scores, jScores, JNI_ABORT);
    }

    LOGD("nGetPostProcessResult -- %d", rc);
    return rc;
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_clevercapture_hardware_CleverCaptureEngine_nRelease (
            JNIEnv *env, jobject obj )
{
    LOGD("nRelease ++ gInstance=%p", gInstance);

#ifdef DUMPY
    int w = gInstance->image.sDim.nWidth;
    int h = gInstance->image.sDim.nHeight;

    if(g_dumpFrameNum > 0) {
        FILE *fp = fopen(DUMPFILENAME, "wb");
        if(fp) {
            fwrite(g_dumpbuf, sizeof(char), g_dumpFrameNum * w * h * 3 / 2, fp);
            fflush(fp);
            fclose(fp);

            LOGD("dump %d yuv frame into %s done!", g_dumpFrameNum, DUMPFILENAME);
        }
    }
#endif

    if(gInstance) {
        releaseInstance(gInstance);
        gInstance = 0;
    }
    LOGD("nRelease -- gInstance=%p", gInstance);
}
