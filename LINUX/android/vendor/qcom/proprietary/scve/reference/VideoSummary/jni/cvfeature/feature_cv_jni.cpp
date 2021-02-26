/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fastcv/fastcv.h>
#include <android/log.h>
#include "CVManagerNative.h"

#define LOG_TAG "CVfeatureJNI"

#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

static int32_t mRuningFPS = 0;
static int32_t countB = 0;
static int32_t countS = 0;
#define PI_F 3.141592653589f


JNIEXPORT jintArray JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_cvmanager_CVManagerJNILib_CVNativeProcess(
        JNIEnv *env, jobject obj, jobject buf, jint width, jint height, jint offsetY, jint strideY,
        jint offsetVU, jint strideVU) {
    DPRINTF("%s :E", __FUNCTION__);
    jbyte *jimgData = NULL;
    jintArray moduleScores = env->NewIntArray(MAX_MODULES);

    jimgData = (jbyte *) env->GetDirectBufferAddress(buf);

    CVoutput_t result = CVManagerProcess(
            (uint8_t *) jimgData + offsetY,
            (uint8_t *) jimgData + offsetY + offsetVU,
            (uint32_t) width,
            (uint32_t) height,
            (uint32_t) strideY,
            (uint32_t) strideVU,
            NULL, NULL, 0, 0);

    if (result.status == CV_ERROR)
        return NULL;
    else {
        env->SetIntArrayRegion(moduleScores, 0, MAX_MODULES, result.modScore);
        return moduleScores;
    }
}

JNIEXPORT jint JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_cvmanager_CVManagerJNILib_CVNativeInit(
        JNIEnv *env, jobject obj) {
    DPRINTF("%s :E", __FUNCTION__);

    CVStatus_t status = CVManagerInit(MODE_BASIC|MODE_MOTION);

    if (status == CV_ERROR)
        return -1;
    else
        return 0;

}

JNIEXPORT jint JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_cvmanager_CVManagerJNILib_CVNativeDeInit(
        JNIEnv *env, jobject obj) {
    DPRINTF("%s :E", __FUNCTION__);
    CVStatus_t status = CVManagerDeinit();

    if (status == CV_ERROR)
        return -1;
    else
        return 0;
}

JNIEXPORT jbyteArray JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_cvmanager_CVManagerJNILib_CVNativeGetFaceTable(
        JNIEnv *env, jobject obj) {
    DPRINTF("%s :E", __FUNCTION__);

    CVPara_t* cvPara = new CVPara_t;
    CVStatus_t status = CVManagerGet(cvPara, MODE_FACE);

    if (status == CV_ERROR)
        return NULL;
    else {
        jbyteArray paraData = env->NewByteArray(MAX_PARA_LEN);
        env->SetByteArrayRegion(paraData, 0, MAX_PARA_LEN, (const jbyte*) cvPara->data);
        return paraData;
    }
}

JNIEXPORT jint JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_cvmanager_CVManagerJNILib_CVNativeSet(
        JNIEnv *env, jobject obj, jbyteArray bArray) {
    DPRINTF("%s :E", __FUNCTION__);

    CVPara_t cvPara;

    int length = env->GetArrayLength(bArray);
    env->GetByteArrayRegion(bArray, 0, length, (jbyte *) cvPara.data);

    CVStatus_t status = CVManagerSet(cvPara, MODE_MOTION);

    if (status == CV_ERROR)
        return -1;
    else
        return 0;
}

JNIEXPORT jobject JNICALL Java_qces_qualcomm_qti_com_videosummary_videoprocessing_cvmanager_CVManagerJNILib_CVNativeSummary(
        JNIEnv *env, jobject obj, jobjectArray videosScores, jobjectArray videoFRTables, jint summaryMode, jint targetLength,
        jbyte compressRatio, jint inputFPS) {

    DPRINTF("%s :E", __FUNCTION__);

    int numVideos = env->GetArrayLength(videosScores);

    __android_log_print(ANDROID_LOG_INFO, "VSUMM", "num vids = %d", numVideos);

    CVFileInput_t input[numVideos];

    CVSuminput_t rinput;
    rinput.file_no = numVideos;

    rinput.target_length = targetLength;
    rinput.summarytype = compressRatio; //0,1,2,3 low to high compress ratio

    __android_log_print(ANDROID_LOG_INFO, "VSUMM", "target_length = %d", targetLength);
    __android_log_print(ANDROID_LOG_INFO, "VSUMM", "compressRatio = %d", compressRatio);
    __android_log_print(ANDROID_LOG_INFO, "VSUMM", "summaryMode = %d", summaryMode);

    //convert the java 2-d arrays to native 2-d arrays
    int32_t **scoresLocalArray = new int *[numVideos];

    CVPara_t cvPara[numVideos];

    for (int i = 0; i < numVideos; i++) {

        __android_log_print(ANDROID_LOG_INFO, "VSUMM", "creating input %d", i);

        jintArray currentVideoArray = (jintArray) env->GetObjectArrayElement(videosScores, i);
        jint *currentScoresArray = env->GetIntArrayElements(currentVideoArray, 0);
        int numScoresForThisVideo = env->GetArrayLength(currentVideoArray);
        scoresLocalArray[i] = new int[numScoresForThisVideo];
        for (int j = 0; j < numScoresForThisVideo; j++) {
            scoresLocalArray[i][j] = currentScoresArray[j];
        }

        jbyteArray currentFRTableArray = (jbyteArray) env->GetObjectArrayElement(videoFRTables, i);
        env->GetByteArrayRegion(currentFRTableArray, 0, MAX_PARA_LEN, (jbyte*) cvPara[i].data);

        input[i].input_length = numScoresForThisVideo / MAX_MODULES;
        input[i].inputFPS = inputFPS;
        input[i].pBuffer = scoresLocalArray[i];
        input[i].pPara = &cvPara[i];
    //input[i].MODE = summaryMode;  // the mode that you want to calculate summary for
        input[i].MODE = MODE_MOTION | MODE_BASIC; //for QIPL we will override the summarymode sent from app and hardcode to  BASIC + MOTION
        input[i].DATA_MODE = MODE_ALL; //the mode for the data that you have

        rinput.pFileinput[i] = &input[i];
    }


    //__android_log_print(ANDROID_LOG_INFO, "VSUMM", "video[0] cvPara.data = %s", input[0].pPara->data);

    CVSumouput_t* output = new CVSumouput_t;

    CVSummary(rinput, output);
    __android_log_print(ANDROID_LOG_INFO, "VSUMM", "CVSummary calculation done");

    //instantiate Java object here and populate it with the summary output data
    jclass cls = env->FindClass(
            "qces/qualcomm/qti/com/videosummary/videoprocessing/cvmanager/CVSummaryOutput");

    int num_outputs = output->ouput_num;

    jbyteArray sumOutputParaData = env->NewByteArray(MAX_PARA_LEN);
    CVPara_t pPara = output->data;
    env->SetByteArrayRegion(sumOutputParaData, 0, MAX_PARA_LEN, (const jbyte*) pPara.data);

    jmethodID constructor = env->GetMethodID(cls, "<init>", "(I[B)V");
    jobject object = env->NewObject(cls, constructor, num_outputs, sumOutputParaData);
    jmethodID addShotMethod = env->GetMethodID(cls, "addShot", "(IIII)V");

    for (int oput = 0; oput < num_outputs; oput++) {
        for (int shot = 0; shot < (int)output->output[oput].output_seg; shot++) {
            __android_log_print(ANDROID_LOG_INFO, "VSUMM",
                                "inside for-loop oput: %d shot %d start_indx: %d end_indx: %d",
                                oput, shot, output->output[oput].start_indx[shot],
                                output->output[oput].end_indx[shot]);
            env->CallVoidMethod(object, addShotMethod,
                                oput,
                                output->output[oput].start_indx[shot],
                                output->output[oput].end_indx[shot],
                                output->output[oput].seg_score[shot]);
        }
    }

    __android_log_write(ANDROID_LOG_INFO, "VSUMM",
                        "created and populated CVSummaryOutput Java object");

    delete output;

    for (int i = 0; i < numVideos; i++) {
        delete scoresLocalArray[i];
    }

    delete[] scoresLocalArray;

    return object;

}

#ifdef __cplusplus
}
#endif


