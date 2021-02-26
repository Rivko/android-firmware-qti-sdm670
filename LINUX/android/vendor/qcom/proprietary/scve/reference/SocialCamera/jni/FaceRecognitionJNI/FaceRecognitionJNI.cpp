/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <jni.h>
#include <string.h>
#include <android/bitmap.h>

#include <sys/time.h>
#include "scve/scveTypes.hpp"
#include "scve/scveContext.hpp"
#include "scve/scveFaceRecognition.hpp"
#include "logger.h"

#define PACKAGE                     "com/qualcomm/qti/facerecognition/"
#define FR_CLASS PACKAGE            "FaceRecognition"
#define FR_SCORE_THRESHOLD          SCVE_FACE_RECOGNITION_DEFAULT_GOOD_SCORE

using namespace SCVE;

typedef struct {
    Context*                context;
    FaceRecognition*        faceRecoHandler;
    Image                   imageBuffer;
    FR_FaceInfo*            pFaceInfo;
    FR_Result*              pFrResult;
    uint32_t                maxUsers;
    uint32_t                maxFacesPerImage;
} NativeContext;

static jfieldID field_FaceRecoWrapper_nativeContext_J;

static void printFR_FaceInfo(FR_FaceInfo *pFaceInfo)
{
    #if 0
    LOGI("(%d,%d),(%d,%d),(%d,%d),(%d,%d)",
         pFaceInfo->sLeftTop.nLocX    ,
         pFaceInfo->sLeftTop.nLocY    ,
         pFaceInfo->sRightTop.nLocX   ,
         pFaceInfo->sRightTop.nLocY   ,
         pFaceInfo->sLeftBottom.nLocX ,
         pFaceInfo->sLeftBottom.nLocY ,
         pFaceInfo->sRightBottom.nLocX,
         pFaceInfo->sRightBottom.nLocY);
    #else
    LOGI("ID: %d, Box: (%d,%d),(%d,%d),(%d,%d),(%d,%d),  Landmark: (%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d),(%d,%d)",
         pFaceInfo->nID                                         ,
         pFaceInfo->sLeftTop.nLocX                              ,
         pFaceInfo->sLeftTop.nLocY                              ,
         pFaceInfo->sRightTop.nLocX                             ,
         pFaceInfo->sRightTop.nLocY                             ,
         pFaceInfo->sLeftBottom.nLocX                           ,
         pFaceInfo->sLeftBottom.nLocY                           ,
         pFaceInfo->sRightBottom.nLocX                          ,
         pFaceInfo->sRightBottom.nLocY                          ,
         pFaceInfo->sLandmarkLocation.sLeftEye.nLocX            ,
         pFaceInfo->sLandmarkLocation.sLeftEye.nLocY            ,
         pFaceInfo->sLandmarkLocation.sRightEye.nLocX           ,
         pFaceInfo->sLandmarkLocation.sRightEye.nLocY           ,
         pFaceInfo->sLandmarkLocation.sLeftEyeInnerCorner.nLocX ,
         pFaceInfo->sLandmarkLocation.sLeftEyeInnerCorner.nLocY ,
         pFaceInfo->sLandmarkLocation.sLeftEyeOuterCorner.nLocX ,
         pFaceInfo->sLandmarkLocation.sLeftEyeOuterCorner.nLocY ,
         pFaceInfo->sLandmarkLocation.sRightEyeInnerCorner.nLocX,
         pFaceInfo->sLandmarkLocation.sRightEyeInnerCorner.nLocY,
         pFaceInfo->sLandmarkLocation.sRightEyeOuterCorner.nLocX,
         pFaceInfo->sLandmarkLocation.sRightEyeOuterCorner.nLocY,
         pFaceInfo->sLandmarkLocation.sMouthLeft.nLocX          ,
         pFaceInfo->sLandmarkLocation.sMouthLeft.nLocY          ,
         pFaceInfo->sLandmarkLocation.sMouthRight.nLocX         ,
         pFaceInfo->sLandmarkLocation.sMouthRight.nLocY         ,
         pFaceInfo->sLandmarkLocation.sNoseLeft.nLocX           ,
         pFaceInfo->sLandmarkLocation.sNoseLeft.nLocY           ,
         pFaceInfo->sLandmarkLocation.sNoseRight.nLocX          ,
         pFaceInfo->sLandmarkLocation.sNoseRight.nLocY          ,
         pFaceInfo->sLandmarkLocation.sMouthUp.nLocX            ,
         pFaceInfo->sLandmarkLocation.sMouthUp.nLocY            );
    #endif
}


/*
 * Method: nClassInit
 * Signature: ()V
 */
static void JNICALL nClassInit(JNIEnv *jenv, jclass cls) {
    field_FaceRecoWrapper_nativeContext_J = jenv->GetFieldID(cls, "nativeContext", "J");
    if (jenv->ExceptionCheck()) {
        return;
    }
}

/*
 * Method:    nCreateScveContext
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL nCreateScveContext(JNIEnv* jenv, jobject obj, jint operationMode) {
    if (jenv->GetLongField(obj, field_FaceRecoWrapper_nativeContext_J) != 0L) {
        LOGE("Fail at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    NativeContext *nativeContext = new NativeContext();
    if (nativeContext == NULL)
    {
        LOGE("Fail at %s out of memory", __FUNCTION__);
        return SCVE_ERROR_OUT_OF_MEMORY;
    }

    if (operationMode == 0)
        nativeContext->context = Context::newInstance(SCVE_MODE_DEFAULT);
    else if (operationMode == 1)
        nativeContext->context = Context::newInstance(SCVE_MODE_CPU_OFFLOAD);
    else if (operationMode == 2)
        nativeContext->context = Context::newInstance(SCVE_MODE_PERFORMANCE);
    else if (operationMode == 3)
        nativeContext->context = Context::newInstance(SCVE_MODE_LOW_POWER);
    else {
        delete nativeContext;
        return SCVE_ERROR_INVALID_PARAM;
    }
    jenv->SetLongField(obj, field_FaceRecoWrapper_nativeContext_J, (jlong) nativeContext);
    return SCVE_SUCCESS;
}

/*
 * Method:     nFreeScveContext
 * Signature:  ()I
 */
JNIEXPORT jint JNICALL nFreeScveContext(JNIEnv* jenv, jobject obj) {
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
            field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL) {
        LOGE("Fail at %s", __FUNCTION__);
        return SCVE_ERROR_INTERNAL;
    }
    if (nativeContext->context) {
        Context::deleteInstance(nativeContext->context);
        nativeContext->context = NULL;
    }
    delete nativeContext;
    jenv->SetLongField(obj, field_FaceRecoWrapper_nativeContext_J, 0L);
    return SCVE_SUCCESS;
}

/*
 * Method:    nCreateFaceRecognition
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL nCreateFaceRecognition(JNIEnv* jenv, jobject obj, jstring databaseName, jint frMode) {
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj, field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL) {
        LOGE("Fail at %s", __FUNCTION__);
        return SCVE_ERROR_INTERNAL;
    }

    jint eRet = SCVE_SUCCESS;
    const char *nativeString = NULL;

    if(databaseName != NULL)
    {
        nativeString = jenv->GetStringUTFChars(databaseName, 0);
        LOGD("Trying to create FR Handle using database - '%s'", nativeString);
    }
    else
    {
        LOGD("Database string is NULL");
    }

    FR_ImageMode eMode = SCVE_FACERECOGNITION_STILL_IMAGE;
    if (frMode == 1)
    {
        eMode = SCVE_FACERECOGNITION_VIDEO;
    }
    nativeContext->faceRecoHandler = FaceRecognition::newInstance(nativeContext->context,
                                                                  nativeString, eMode);

    if(nativeContext->faceRecoHandler == NULL)
    {
       LOGE("Fail to create FaceRecognition instance!");
       eRet = SCVE_ERROR_INTERNAL;
    }

    if(databaseName != NULL)
    {
    jenv->ReleaseStringUTFChars(databaseName, nativeString);
    }

    nativeContext->pFaceInfo = NULL;
    nativeContext->pFrResult = NULL;
    nativeContext->maxUsers = 0;
    return eRet;
}

/*
 * Method:     nFreeFaceRecognition
 * Signature:  ()I
 */
JNIEXPORT jint JNICALL nFreeFaceRecognition(JNIEnv* jenv, jobject obj) {
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
            field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL) {
        LOGE("Fail at %s", __FUNCTION__);
        return SCVE_ERROR_INTERNAL;
    }
    if (nativeContext->faceRecoHandler) {
        FaceRecognition::deleteInstance(nativeContext->faceRecoHandler);
        nativeContext->faceRecoHandler = NULL;
    }

    if (nativeContext->pFaceInfo)
    {
        free(nativeContext->pFaceInfo);
        nativeContext->pFaceInfo = NULL;
    }
    if (nativeContext->pFrResult)
    {
        free(nativeContext->pFrResult);
        nativeContext->pFrResult = NULL;
    }

    nativeContext->maxUsers = 0;

    return SCVE_SUCCESS;
}

/*
 * Method:    nInit
 * Signature: (IIIIIII)I
 */
JNIEXPORT jint JNICALL nInit(JNIEnv* jenv, jobject obj, jint colorFormat, jint width, jint height, jint lumaStride, jint chromaStride,
                             jint maxFacesPerImage, jint maxNumUsers)
{
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
                                    field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL || nativeContext->faceRecoHandler == NULL) {
        LOGE("Fail at %s", __FUNCTION__);
        return SCVE_ERROR_INTERNAL;
    }

    switch (colorFormat)
    {
    case SCVE_COLORFORMAT_GREY_8BIT:
    case SCVE_COLORFORMAT_YUV_NV21:
    case SCVE_COLORFORMAT_YUV_NV12:
       nativeContext->imageBuffer.sDim.nWidth = width;
       nativeContext->imageBuffer.sDim.nHeight = height;
       nativeContext->imageBuffer.sDim.nStride = lumaStride;
       nativeContext->imageBuffer.sDim.nStrides[0] = lumaStride;
       break;

    default:
       LOGE("Unsupported color format %d", colorFormat);
       return SCVE_ERROR_UNSUPPORTED_FORMAT;
    }

    nativeContext->imageBuffer.sDim.nStrides[1] = chromaStride;
    nativeContext->imageBuffer.sFormat = (ColorFormat) colorFormat;
    nativeContext->imageBuffer.nChannels = (colorFormat == SCVE_COLORFORMAT_GREY_8BIT) ? 1 : 2;

    nativeContext->maxUsers = maxNumUsers;
    nativeContext->maxFacesPerImage = maxFacesPerImage;
    if (nativeContext->maxUsers == 0)
    {
        nativeContext->maxUsers = SCVE_FACE_RECOGNITION_DEFAULT_MAX_REGISTERED_FACES;
    }

    if (nativeContext->pFaceInfo)
    {
        free(nativeContext->pFaceInfo);
        nativeContext->pFaceInfo = NULL;
    }
    nativeContext->pFaceInfo = (FR_FaceInfo *) malloc(nativeContext->maxUsers * sizeof(FR_FaceInfo));
    if (nativeContext->pFaceInfo == NULL)
    {
        LOGE("Fail at %s out of memory for face info", __FUNCTION__);
        return SCVE_ERROR_OUT_OF_MEMORY;
    }

    if (nativeContext->pFrResult)
    {
        free(nativeContext->pFrResult);
        nativeContext->pFrResult = NULL;
    }
    nativeContext->pFrResult = (FR_Result *) malloc(nativeContext->maxUsers * sizeof(FR_Result));
    if (nativeContext->pFrResult == NULL)
    {
        free(nativeContext->pFaceInfo);
        nativeContext->pFaceInfo = NULL;

        LOGE("Fail at %s out of memory for results", __FUNCTION__);
        return SCVE_ERROR_OUT_OF_MEMORY;
    }

    ImageDimension dim;
    dim.nWidth = (uint32_t) width;
    dim.nHeight = (uint32_t) height;
    dim.nStride = (uint32_t) lumaStride;
    dim.nStrides[0] = (uint32_t) lumaStride;
    dim.nStrides[1] = (uint32_t) chromaStride;

    Status status = nativeContext->faceRecoHandler->Init(dim, maxFacesPerImage, maxNumUsers,
                                                         SCVE_FACE_RECOGNITION_DEFAULT_MIN_FACE_SIZE_TO_DETECT,
                                                         NULL, "/sdcard/scve/facereco/gModel.dat");

    return status;
}

static jint detectFaces(JNIEnv *jenv, jobject obj, uint8_t *pImageData, jobjectArray faceInfo)
{
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
                                    field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL || nativeContext->faceRecoHandler == NULL)
    {
       LOGE("Fail at %s", __FUNCTION__);
       return SCVE_ERROR_INTERNAL;
    }

    nativeContext->imageBuffer.pPtr = pImageData;

    int32_t nDetectedFaces = 0;
    Status ret = nativeContext->faceRecoHandler->DetectFaces(&nativeContext->imageBuffer, nativeContext->pFaceInfo, (uint32_t *) &nDetectedFaces);
    if (ret != SCVE_SUCCESS)
    {
        nDetectedFaces = ret;      // return the negative error code if not success
    }
    else
    {
       FR_FaceInfo frFaceInfo;

       jclass clsFaceInfo = jenv->FindClass("com/qualcomm/qti/facerecognition/FaceInfo");
       jfieldID faceLandmark = jenv->GetFieldID(clsFaceInfo, "faceLandmark", "Lcom/qualcomm/qti/facerecognition/FaceLandmark;");
       jfieldID id           = jenv->GetFieldID(clsFaceInfo, "id",           "I");
       jfieldID leftTopX     = jenv->GetFieldID(clsFaceInfo, "leftTopX",     "I");
       jfieldID leftTopY     = jenv->GetFieldID(clsFaceInfo, "leftTopY",     "I");
       jfieldID rightTopX    = jenv->GetFieldID(clsFaceInfo, "rightTopX",    "I");
       jfieldID rightTopY    = jenv->GetFieldID(clsFaceInfo, "rightTopY",    "I");
       jfieldID leftBottomX  = jenv->GetFieldID(clsFaceInfo, "leftBottomX",  "I");
       jfieldID leftBottomY  = jenv->GetFieldID(clsFaceInfo, "leftBottomY",  "I");
       jfieldID rightBottomX = jenv->GetFieldID(clsFaceInfo, "rightBottomX", "I");
       jfieldID rightBottomY = jenv->GetFieldID(clsFaceInfo, "rightBottomY", "I");

       jclass clsFaceLandmark = jenv->FindClass("com/qualcomm/qti/facerecognition/FaceLandmark");
       jfieldID leftEyeX             = jenv->GetFieldID(clsFaceLandmark, "leftEyeX",             "I");
       jfieldID leftEyeY             = jenv->GetFieldID(clsFaceLandmark, "leftEyeY",             "I");
       jfieldID rightEyeX            = jenv->GetFieldID(clsFaceLandmark, "rightEyeX",            "I");
       jfieldID rightEyeY            = jenv->GetFieldID(clsFaceLandmark, "rightEyeY",            "I");
       jfieldID leftEyeInnerCornerX  = jenv->GetFieldID(clsFaceLandmark, "leftEyeInnerCornerX",  "I");
       jfieldID leftEyeInnerCornerY  = jenv->GetFieldID(clsFaceLandmark, "leftEyeInnerCornerY",  "I");
       jfieldID leftEyeOuterCornerX  = jenv->GetFieldID(clsFaceLandmark, "leftEyeOuterCornerX",  "I");
       jfieldID leftEyeOuterCornerY  = jenv->GetFieldID(clsFaceLandmark, "leftEyeOuterCornerY",  "I");
       jfieldID rightEyeInnerCornerX = jenv->GetFieldID(clsFaceLandmark, "rightEyeInnerCornerX", "I");
       jfieldID rightEyeInnerCornerY = jenv->GetFieldID(clsFaceLandmark, "rightEyeInnerCornerY", "I");
       jfieldID rightEyeOuterCornerX = jenv->GetFieldID(clsFaceLandmark, "rightEyeOuterCornerX", "I");
       jfieldID rightEyeOuterCornerY = jenv->GetFieldID(clsFaceLandmark, "rightEyeOuterCornerY", "I");
       jfieldID mouthLeftX           = jenv->GetFieldID(clsFaceLandmark, "mouthLeftX",           "I");
       jfieldID mouthLeftY           = jenv->GetFieldID(clsFaceLandmark, "mouthLeftY",           "I");
       jfieldID mouthRightX          = jenv->GetFieldID(clsFaceLandmark, "mouthRightX",          "I");
       jfieldID mouthRightY          = jenv->GetFieldID(clsFaceLandmark, "mouthRightY",          "I");
       jfieldID noseLeftX            = jenv->GetFieldID(clsFaceLandmark, "noseLeftX",            "I");
       jfieldID noseLeftY            = jenv->GetFieldID(clsFaceLandmark, "noseLeftY",            "I");
       jfieldID noseRightX           = jenv->GetFieldID(clsFaceLandmark, "noseRightX",           "I");
       jfieldID noseRightY           = jenv->GetFieldID(clsFaceLandmark, "noseRightY",           "I");
       jfieldID mouthUpX             = jenv->GetFieldID(clsFaceLandmark, "mouthUpX",             "I");
       jfieldID mouthUpY             = jenv->GetFieldID(clsFaceLandmark, "mouthUpY",             "I");

       //LOGI("nDetectFaces %d was detected", nDetectedFaces);
       for (int32_t i=0; i<nDetectedFaces; i++)
       {
          //LOGI("pFaceInfo[%d]", i);
          //printFR_FaceInfo(nativeContext->pFaceInfo + i);
          jobject frFaceInfo = jenv->GetObjectArrayElement(faceInfo, i);
          jenv->SetIntField(frFaceInfo, id,           nativeContext->pFaceInfo[i].nID               );
          jenv->SetIntField(frFaceInfo, leftTopX,     nativeContext->pFaceInfo[i].sLeftTop.nLocX    );
          jenv->SetIntField(frFaceInfo, leftTopY,     nativeContext->pFaceInfo[i].sLeftTop.nLocY    );
          jenv->SetIntField(frFaceInfo, rightTopX,    nativeContext->pFaceInfo[i].sRightTop.nLocX   );
          jenv->SetIntField(frFaceInfo, rightTopY,    nativeContext->pFaceInfo[i].sRightTop.nLocY   );
          jenv->SetIntField(frFaceInfo, leftBottomX,  nativeContext->pFaceInfo[i].sLeftBottom.nLocX );
          jenv->SetIntField(frFaceInfo, leftBottomY,  nativeContext->pFaceInfo[i].sLeftBottom.nLocY );
          jenv->SetIntField(frFaceInfo, rightBottomX, nativeContext->pFaceInfo[i].sRightBottom.nLocX);
          jenv->SetIntField(frFaceInfo, rightBottomY, nativeContext->pFaceInfo[i].sRightBottom.nLocY);

          jobject landmark = jenv->GetObjectField(frFaceInfo, faceLandmark);
          jenv->SetIntField(landmark, leftEyeX,             nativeContext->pFaceInfo[i].sLandmarkLocation.sLeftEye.nLocX            );
          jenv->SetIntField(landmark, leftEyeY,             nativeContext->pFaceInfo[i].sLandmarkLocation.sLeftEye.nLocY            );
          jenv->SetIntField(landmark, rightEyeX,            nativeContext->pFaceInfo[i].sLandmarkLocation.sRightEye.nLocX           );
          jenv->SetIntField(landmark, rightEyeY,            nativeContext->pFaceInfo[i].sLandmarkLocation.sRightEye.nLocY           );
          jenv->SetIntField(landmark, leftEyeInnerCornerX,  nativeContext->pFaceInfo[i].sLandmarkLocation.sLeftEyeInnerCorner.nLocX );
          jenv->SetIntField(landmark, leftEyeInnerCornerY,  nativeContext->pFaceInfo[i].sLandmarkLocation.sLeftEyeInnerCorner.nLocY );
          jenv->SetIntField(landmark, leftEyeOuterCornerX,  nativeContext->pFaceInfo[i].sLandmarkLocation.sLeftEyeOuterCorner.nLocX );
          jenv->SetIntField(landmark, leftEyeOuterCornerY,  nativeContext->pFaceInfo[i].sLandmarkLocation.sLeftEyeOuterCorner.nLocY );
          jenv->SetIntField(landmark, rightEyeInnerCornerX, nativeContext->pFaceInfo[i].sLandmarkLocation.sRightEyeInnerCorner.nLocX);
          jenv->SetIntField(landmark, rightEyeInnerCornerY, nativeContext->pFaceInfo[i].sLandmarkLocation.sRightEyeInnerCorner.nLocY);
          jenv->SetIntField(landmark, rightEyeOuterCornerX, nativeContext->pFaceInfo[i].sLandmarkLocation.sRightEyeOuterCorner.nLocX);
          jenv->SetIntField(landmark, rightEyeOuterCornerY, nativeContext->pFaceInfo[i].sLandmarkLocation.sRightEyeOuterCorner.nLocY);
          jenv->SetIntField(landmark, mouthLeftX,           nativeContext->pFaceInfo[i].sLandmarkLocation.sMouthLeft.nLocX          );
          jenv->SetIntField(landmark, mouthLeftY,           nativeContext->pFaceInfo[i].sLandmarkLocation.sMouthLeft.nLocY          );
          jenv->SetIntField(landmark, mouthRightX,          nativeContext->pFaceInfo[i].sLandmarkLocation.sMouthRight.nLocX         );
          jenv->SetIntField(landmark, mouthRightY,          nativeContext->pFaceInfo[i].sLandmarkLocation.sMouthRight.nLocY         );
          jenv->SetIntField(landmark, noseLeftX,            nativeContext->pFaceInfo[i].sLandmarkLocation.sNoseLeft.nLocX           );
          jenv->SetIntField(landmark, noseLeftY,            nativeContext->pFaceInfo[i].sLandmarkLocation.sNoseLeft.nLocY           );
          jenv->SetIntField(landmark, noseRightX,           nativeContext->pFaceInfo[i].sLandmarkLocation.sNoseRight.nLocX          );
          jenv->SetIntField(landmark, noseRightY,           nativeContext->pFaceInfo[i].sLandmarkLocation.sNoseRight.nLocY          );
          jenv->SetIntField(landmark, mouthUpX,             nativeContext->pFaceInfo[i].sLandmarkLocation.sMouthUp.nLocX            );
          jenv->SetIntField(landmark, mouthUpY,             nativeContext->pFaceInfo[i].sLandmarkLocation.sMouthUp.nLocY            );
       }
    }

    return nDetectedFaces;
}

/*
 * Method:    nDetectFaces
 * Signature: ([B[Lcom/qualcomm/qti/facerecognition/FaceInfo;)I
 */
JNIEXPORT jint JNICALL nDetectFaces(JNIEnv* jenv, jobject obj, jbyteArray imageData, jobjectArray faceInfo)
{
    if (imageData == NULL)
    {
        LOGE("Fail invalid param at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    uint8_t *pImageData = (uint8_t*)jenv->GetByteArrayElements(imageData, NULL);
    if (pImageData == NULL)
    {
        LOGE("Failed to retrieve imageData");
        return SCVE_ERROR_INVALID_PARAM;
    }

    jint result = detectFaces(jenv, obj, pImageData, faceInfo);

    jenv->ReleaseByteArrayElements(imageData, (jbyte*)pImageData, JNI_ABORT);

    return result;
}

/*
 * Method:    nDetectFaces
 * Signature: (Ljava/nio/ByteBuffer;[Lcom/qualcomm/qti/facerecognition/FaceInfo;)I
 */
JNIEXPORT jint JNICALL nDetectFacesDirect(JNIEnv* jenv, jobject obj, jobject imageData, jobjectArray faceInfo)
{
    if (imageData == NULL)
    {
        LOGE("Fail invalid param at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    uint8_t *pImageData = (uint8_t*)jenv->GetDirectBufferAddress(imageData);
    if (pImageData == NULL)
    {
        LOGE("Failed to get direct buffer address");
        return SCVE_ERROR_INVALID_PARAM;
    }

    return detectFaces(jenv, obj, pImageData, faceInfo);
}

static jint recognizeFace(JNIEnv *jenv, jobject obj, uint8_t *pImageData, jobject faceInfo, jint nMaxResults, jobjectArray results)
{
    NativeContext *nativeContext = (NativeContext *)jenv->GetLongField(obj,
                                                                       field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL || nativeContext->faceRecoHandler == NULL)
    {
        LOGE("Fail at %s", __FUNCTION__);
        return SCVE_ERROR_INTERNAL;
    }

    nativeContext->imageBuffer.pPtr = pImageData;

    FR_FaceInfo frFaceInfo;

    jclass clsFaceInfo = jenv->FindClass("com/qualcomm/qti/facerecognition/FaceInfo");

    jfieldID id           = jenv->GetFieldID(clsFaceInfo, "id",           "I");
    jfieldID faceLandmark = jenv->GetFieldID(clsFaceInfo, "faceLandmark", "Lcom/qualcomm/qti/facerecognition/FaceLandmark;");
    jfieldID leftTopX     = jenv->GetFieldID(clsFaceInfo, "leftTopX",     "I");
    jfieldID leftTopY     = jenv->GetFieldID(clsFaceInfo, "leftTopY",     "I");
    jfieldID rightTopX    = jenv->GetFieldID(clsFaceInfo, "rightTopX",    "I");
    jfieldID rightTopY    = jenv->GetFieldID(clsFaceInfo, "rightTopY",    "I");
    jfieldID leftBottomX  = jenv->GetFieldID(clsFaceInfo, "leftBottomX",  "I");
    jfieldID leftBottomY  = jenv->GetFieldID(clsFaceInfo, "leftBottomY",  "I");
    jfieldID rightBottomX = jenv->GetFieldID(clsFaceInfo, "rightBottomX", "I");
    jfieldID rightBottomY = jenv->GetFieldID(clsFaceInfo, "rightBottomY", "I");

    frFaceInfo.nID                = jenv->GetIntField(faceInfo, id);
    frFaceInfo.sLeftTop.nLocX     = jenv->GetIntField(faceInfo, leftTopX);
    frFaceInfo.sLeftTop.nLocY     = jenv->GetIntField(faceInfo, leftTopY);
    frFaceInfo.sRightTop.nLocX    = jenv->GetIntField(faceInfo, rightTopX);
    frFaceInfo.sRightTop.nLocY    = jenv->GetIntField(faceInfo, rightTopY);
    frFaceInfo.sLeftBottom.nLocX  = jenv->GetIntField(faceInfo, leftBottomX);
    frFaceInfo.sLeftBottom.nLocY  = jenv->GetIntField(faceInfo, leftBottomY);
    frFaceInfo.sRightBottom.nLocX = jenv->GetIntField(faceInfo, rightBottomX);
    frFaceInfo.sRightBottom.nLocY = jenv->GetIntField(faceInfo, rightBottomY);

    jclass clsFaceLandmark = jenv->FindClass("com/qualcomm/qti/facerecognition/FaceLandmark");
    jfieldID leftEyeX             = jenv->GetFieldID(clsFaceLandmark, "leftEyeX",             "I");
    jfieldID leftEyeY             = jenv->GetFieldID(clsFaceLandmark, "leftEyeY",             "I");
    jfieldID rightEyeX            = jenv->GetFieldID(clsFaceLandmark, "rightEyeX",            "I");
    jfieldID rightEyeY            = jenv->GetFieldID(clsFaceLandmark, "rightEyeY",            "I");
    jfieldID leftEyeInnerCornerX  = jenv->GetFieldID(clsFaceLandmark, "leftEyeInnerCornerX",  "I");
    jfieldID leftEyeInnerCornerY  = jenv->GetFieldID(clsFaceLandmark, "leftEyeInnerCornerY",  "I");
    jfieldID leftEyeOuterCornerX  = jenv->GetFieldID(clsFaceLandmark, "leftEyeOuterCornerX",  "I");
    jfieldID leftEyeOuterCornerY  = jenv->GetFieldID(clsFaceLandmark, "leftEyeOuterCornerY",  "I");
    jfieldID rightEyeInnerCornerX = jenv->GetFieldID(clsFaceLandmark, "rightEyeInnerCornerX", "I");
    jfieldID rightEyeInnerCornerY = jenv->GetFieldID(clsFaceLandmark, "rightEyeInnerCornerY", "I");
    jfieldID rightEyeOuterCornerX = jenv->GetFieldID(clsFaceLandmark, "rightEyeOuterCornerX", "I");
    jfieldID rightEyeOuterCornerY = jenv->GetFieldID(clsFaceLandmark, "rightEyeOuterCornerY", "I");
    jfieldID mouthLeftX           = jenv->GetFieldID(clsFaceLandmark, "mouthLeftX",           "I");
    jfieldID mouthLeftY           = jenv->GetFieldID(clsFaceLandmark, "mouthLeftY",           "I");
    jfieldID mouthRightX          = jenv->GetFieldID(clsFaceLandmark, "mouthRightX",          "I");
    jfieldID mouthRightY          = jenv->GetFieldID(clsFaceLandmark, "mouthRightY",          "I");
    jfieldID noseLeftX            = jenv->GetFieldID(clsFaceLandmark, "noseLeftX",            "I");
    jfieldID noseLeftY            = jenv->GetFieldID(clsFaceLandmark, "noseLeftY",            "I");
    jfieldID noseRightX           = jenv->GetFieldID(clsFaceLandmark, "noseRightX",           "I");
    jfieldID noseRightY           = jenv->GetFieldID(clsFaceLandmark, "noseRightY",           "I");
    jfieldID mouthUpX             = jenv->GetFieldID(clsFaceLandmark, "mouthUpX",             "I");
    jfieldID mouthUpY             = jenv->GetFieldID(clsFaceLandmark, "mouthUpY",             "I");

    jobject landmark              = jenv->GetObjectField(faceInfo, faceLandmark);
    frFaceInfo.sLandmarkLocation.sLeftEye.nLocX             = jenv->GetIntField(landmark, leftEyeX);
    frFaceInfo.sLandmarkLocation.sLeftEye.nLocY             = jenv->GetIntField(landmark, leftEyeY);
    frFaceInfo.sLandmarkLocation.sRightEye.nLocX            = jenv->GetIntField(landmark, rightEyeX);
    frFaceInfo.sLandmarkLocation.sRightEye.nLocY            = jenv->GetIntField(landmark, rightEyeY);
    frFaceInfo.sLandmarkLocation.sLeftEyeInnerCorner.nLocX  = jenv->GetIntField(landmark, leftEyeInnerCornerX);
    frFaceInfo.sLandmarkLocation.sLeftEyeInnerCorner.nLocY  = jenv->GetIntField(landmark, leftEyeInnerCornerY);
    frFaceInfo.sLandmarkLocation.sLeftEyeOuterCorner.nLocX  = jenv->GetIntField(landmark, leftEyeOuterCornerX);
    frFaceInfo.sLandmarkLocation.sLeftEyeOuterCorner.nLocY  = jenv->GetIntField(landmark, leftEyeOuterCornerY);
    frFaceInfo.sLandmarkLocation.sRightEyeInnerCorner.nLocX = jenv->GetIntField(landmark, rightEyeInnerCornerX);
    frFaceInfo.sLandmarkLocation.sRightEyeInnerCorner.nLocY = jenv->GetIntField(landmark, rightEyeInnerCornerY);
    frFaceInfo.sLandmarkLocation.sRightEyeOuterCorner.nLocX = jenv->GetIntField(landmark, rightEyeOuterCornerX);
    frFaceInfo.sLandmarkLocation.sRightEyeOuterCorner.nLocY = jenv->GetIntField(landmark, rightEyeOuterCornerY);
    frFaceInfo.sLandmarkLocation.sMouthLeft.nLocX           = jenv->GetIntField(landmark, mouthLeftX);
    frFaceInfo.sLandmarkLocation.sMouthLeft.nLocY           = jenv->GetIntField(landmark, mouthLeftY);
    frFaceInfo.sLandmarkLocation.sMouthRight.nLocX          = jenv->GetIntField(landmark, mouthRightX);
    frFaceInfo.sLandmarkLocation.sMouthRight.nLocY          = jenv->GetIntField(landmark, mouthRightY);
    frFaceInfo.sLandmarkLocation.sNoseLeft.nLocX            = jenv->GetIntField(landmark, noseLeftX);
    frFaceInfo.sLandmarkLocation.sNoseLeft.nLocY            = jenv->GetIntField(landmark, noseLeftY);
    frFaceInfo.sLandmarkLocation.sNoseRight.nLocX           = jenv->GetIntField(landmark, noseRightX);
    frFaceInfo.sLandmarkLocation.sNoseRight.nLocY           = jenv->GetIntField(landmark, noseRightY);
    frFaceInfo.sLandmarkLocation.sMouthUp.nLocX             = jenv->GetIntField(landmark, mouthUpX);
    frFaceInfo.sLandmarkLocation.sMouthUp.nLocY             = jenv->GetIntField(landmark, mouthUpY);

    int32_t nResult = 0;

    uint32_t maximumResult = (nMaxResults > (int32_t)nativeContext->maxUsers) ? nativeContext->maxUsers : nMaxResults;
    Status ret = nativeContext->faceRecoHandler->RecognizeFace(&nativeContext->imageBuffer, &frFaceInfo, maximumResult, nativeContext->pFrResult,  (uint32_t *)&nResult);
    if (ret != SCVE_SUCCESS)
    {
        if (ret != (Status)SCVE_FACERECOGNITION_ERROR_SKIP_RECOGNITION)
        {
            LOGE("Fail at %s ret %x", __FUNCTION__, ret);
            nResult = ret;      // return the negative error code if not success
        }
    }
    else
    {
        jclass clsFRresult = jenv->FindClass("com/qualcomm/qti/facerecognition/FaceRecognitionResult");

        jfieldID matchScore = jenv->GetFieldID(clsFRresult, "matchScore", "F");
        jfieldID pid        = jenv->GetFieldID(clsFRresult, "pid",        "[B");

        jbyteArray *pidByteArray = (jbyteArray *)malloc(nResult * sizeof(jbyteArray));
        if (pidByteArray == NULL)
        {
            LOGE("Fail at %s ret %d", __FUNCTION__, ret);
            return SCVE_ERROR_OUT_OF_MEMORY;
        }

        int32_t nResultAfterThreshold = 0;
        //LOGI("Find %d results", nResult);
        for (int32_t i = 0; i < nResult; i++)
        {
            if (nativeContext->pFrResult[i].nMatchScore >= FR_SCORE_THRESHOLD)
            {
                jobject frResult = jenv->GetObjectArrayElement(results, i);

                jenv->SetFloatField(frResult, matchScore, nativeContext->pFrResult[i].nMatchScore);

                //LOGI("PID[%d] %s %f", i, nativeContext->frResult[i].sPID, nativeContext->frResult[i].nMatchScore);
                pidByteArray[i] = jenv->NewByteArray(SCVE_FACE_RECOGNITION_FEATURE_ID_LEN);
                jenv->SetByteArrayRegion(pidByteArray[i], 0, SCVE_FACE_RECOGNITION_FEATURE_ID_LEN, (const jbyte *)nativeContext->pFrResult[i].sPID);
                jenv->SetObjectField(frResult, pid, pidByteArray[i]);

                jenv->SetObjectArrayElement(results, i, frResult);

                jenv->DeleteLocalRef(pidByteArray[i]);
                nResultAfterThreshold++;
            }
        }
        nResult = nResultAfterThreshold;
        if (nResult > 0)
        {
            LOGI("recognizeFace best matching result ID %.20s, result %f",
                 nativeContext->pFrResult[0].sPID, nativeContext->pFrResult[0].nMatchScore);
        }

        free(pidByteArray);
    }

    return nResult;
}

/*
 * Method:    nRecognizeFace
 * Signature: ([BLcom/qualcomm/qti/facerecognition/FaceInfo;I[Lcom/qualcomm/qti/facerecognition/FaceRecognitionResult;)I
 */
JNIEXPORT jint JNICALL nRecognizeFace(JNIEnv *jenv, jobject obj, jbyteArray imageData, jobject faceInfo, jint nMaxResults, jobjectArray results)
{
    if (imageData == NULL)
    {
        LOGE("Fail invalid param at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    uint8_t *pImageData = (uint8_t *)jenv->GetByteArrayElements(imageData, NULL);
    if (pImageData == NULL)
    {
        LOGE("Failed to retrieve imageData");
        return SCVE_ERROR_INVALID_PARAM;
    }

    jint result = recognizeFace(jenv, obj, pImageData, faceInfo, nMaxResults, results);

    jenv->ReleaseByteArrayElements(imageData, (jbyte*)pImageData, 0);

    return result;
}

/*
 * Method:    nRecognizeFace
 * Signature: (Ljava/nio/ByteBuffer;Lcom/qualcomm/qti/facerecognition/FaceInfo;I[Lcom/qualcomm/qti/facerecognition/FaceRecognitionResult;)I
 */
JNIEXPORT jint JNICALL nRecognizeFaceDirect(JNIEnv* jenv, jobject obj, jobject imageData, jobject faceInfo, jint nMaxResults, jobjectArray results)
{
    if (imageData == NULL)
    {
        LOGE("Fail invalid param at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    uint8_t *pImageData = (uint8_t*)jenv->GetDirectBufferAddress(imageData);
    if (pImageData == NULL)
    {
        LOGE("Failed to get direct buffer address");
        return SCVE_ERROR_INVALID_PARAM;
    }

    return recognizeFace(jenv, obj, pImageData, faceInfo, nMaxResults, results);
}

static jint enrollFace(JNIEnv *jenv, jobject obj, uint8_t *pImageData, jobject faceInfo, jbyteArray featureID)
{
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
                                   field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL || nativeContext->faceRecoHandler == NULL)
    {
       LOGE("Fail at %s", __FUNCTION__);
       return SCVE_ERROR_INTERNAL;
    }

    nativeContext->imageBuffer.pPtr = pImageData;

    const char *pFeatureID = (const char*)jenv->GetByteArrayElements(featureID, NULL);

    FR_FaceInfo frFaceInfo;

    jclass clsFaceInfo = jenv->FindClass("com/qualcomm/qti/facerecognition/FaceInfo");
    jfieldID faceLandmark = jenv->GetFieldID(clsFaceInfo, "faceLandmark", "Lcom/qualcomm/qti/facerecognition/FaceLandmark;");
    jfieldID id           = jenv->GetFieldID(clsFaceInfo, "id",           "I");
    jfieldID leftTopX     = jenv->GetFieldID(clsFaceInfo, "leftTopX",     "I");
    jfieldID leftTopY     = jenv->GetFieldID(clsFaceInfo, "leftTopY",     "I");
    jfieldID rightTopX    = jenv->GetFieldID(clsFaceInfo, "rightTopX",    "I");
    jfieldID rightTopY    = jenv->GetFieldID(clsFaceInfo, "rightTopY",    "I");
    jfieldID leftBottomX  = jenv->GetFieldID(clsFaceInfo, "leftBottomX",  "I");
    jfieldID leftBottomY  = jenv->GetFieldID(clsFaceInfo, "leftBottomY",  "I");
    jfieldID rightBottomX = jenv->GetFieldID(clsFaceInfo, "rightBottomX", "I");
    jfieldID rightBottomY = jenv->GetFieldID(clsFaceInfo, "rightBottomY", "I");

    frFaceInfo.nID                = jenv->GetIntField(faceInfo, id);
    frFaceInfo.sLeftTop.nLocX     = jenv->GetIntField(faceInfo, leftTopX);
    frFaceInfo.sLeftTop.nLocY     = jenv->GetIntField(faceInfo, leftTopY);
    frFaceInfo.sRightTop.nLocX    = jenv->GetIntField(faceInfo, rightTopX);
    frFaceInfo.sRightTop.nLocY    = jenv->GetIntField(faceInfo, rightTopY);
    frFaceInfo.sLeftBottom.nLocX  = jenv->GetIntField(faceInfo, leftBottomX);
    frFaceInfo.sLeftBottom.nLocY  = jenv->GetIntField(faceInfo, leftBottomY);
    frFaceInfo.sRightBottom.nLocX = jenv->GetIntField(faceInfo, rightBottomX);
    frFaceInfo.sRightBottom.nLocY = jenv->GetIntField(faceInfo, rightBottomY);

    jclass clsFaceLandmark        = jenv->FindClass("com/qualcomm/qti/facerecognition/FaceLandmark");
    jfieldID leftEyeX             = jenv->GetFieldID(clsFaceLandmark, "leftEyeX",             "I");
    jfieldID leftEyeY             = jenv->GetFieldID(clsFaceLandmark, "leftEyeY",             "I");
    jfieldID rightEyeX            = jenv->GetFieldID(clsFaceLandmark, "rightEyeX",            "I");
    jfieldID rightEyeY            = jenv->GetFieldID(clsFaceLandmark, "rightEyeY",            "I");
    jfieldID leftEyeInnerCornerX  = jenv->GetFieldID(clsFaceLandmark, "leftEyeInnerCornerX",  "I");
    jfieldID leftEyeInnerCornerY  = jenv->GetFieldID(clsFaceLandmark, "leftEyeInnerCornerY",  "I");
    jfieldID leftEyeOuterCornerX  = jenv->GetFieldID(clsFaceLandmark, "leftEyeOuterCornerX",  "I");
    jfieldID leftEyeOuterCornerY  = jenv->GetFieldID(clsFaceLandmark, "leftEyeOuterCornerY",  "I");
    jfieldID rightEyeInnerCornerX = jenv->GetFieldID(clsFaceLandmark, "rightEyeInnerCornerX", "I");
    jfieldID rightEyeInnerCornerY = jenv->GetFieldID(clsFaceLandmark, "rightEyeInnerCornerY", "I");
    jfieldID rightEyeOuterCornerX = jenv->GetFieldID(clsFaceLandmark, "rightEyeOuterCornerX", "I");
    jfieldID rightEyeOuterCornerY = jenv->GetFieldID(clsFaceLandmark, "rightEyeOuterCornerY", "I");
    jfieldID mouthLeftX           = jenv->GetFieldID(clsFaceLandmark, "mouthLeftX",           "I");
    jfieldID mouthLeftY           = jenv->GetFieldID(clsFaceLandmark, "mouthLeftY",           "I");
    jfieldID mouthRightX          = jenv->GetFieldID(clsFaceLandmark, "mouthRightX",          "I");
    jfieldID mouthRightY          = jenv->GetFieldID(clsFaceLandmark, "mouthRightY",          "I");
    jfieldID noseLeftX            = jenv->GetFieldID(clsFaceLandmark, "noseLeftX",            "I");
    jfieldID noseLeftY            = jenv->GetFieldID(clsFaceLandmark, "noseLeftY",            "I");
    jfieldID noseRightX           = jenv->GetFieldID(clsFaceLandmark, "noseRightX",           "I");
    jfieldID noseRightY           = jenv->GetFieldID(clsFaceLandmark, "noseRightY",           "I");
    jfieldID mouthUpX             = jenv->GetFieldID(clsFaceLandmark, "mouthUpX",             "I");
    jfieldID mouthUpY             = jenv->GetFieldID(clsFaceLandmark, "mouthUpY",             "I");

    jobject landmark              = jenv->GetObjectField(faceInfo, faceLandmark);
    frFaceInfo.sLandmarkLocation.sLeftEye.nLocX             = jenv->GetIntField(landmark, leftEyeX);
    frFaceInfo.sLandmarkLocation.sLeftEye.nLocY             = jenv->GetIntField(landmark, leftEyeY);
    frFaceInfo.sLandmarkLocation.sRightEye.nLocX            = jenv->GetIntField(landmark, rightEyeX);
    frFaceInfo.sLandmarkLocation.sRightEye.nLocY            = jenv->GetIntField(landmark, rightEyeY);
    frFaceInfo.sLandmarkLocation.sLeftEyeInnerCorner.nLocX  = jenv->GetIntField(landmark, leftEyeInnerCornerX);
    frFaceInfo.sLandmarkLocation.sLeftEyeInnerCorner.nLocY  = jenv->GetIntField(landmark, leftEyeInnerCornerY);
    frFaceInfo.sLandmarkLocation.sLeftEyeOuterCorner.nLocX  = jenv->GetIntField(landmark, leftEyeOuterCornerX);
    frFaceInfo.sLandmarkLocation.sLeftEyeOuterCorner.nLocY  = jenv->GetIntField(landmark, leftEyeOuterCornerY);
    frFaceInfo.sLandmarkLocation.sRightEyeInnerCorner.nLocX = jenv->GetIntField(landmark, rightEyeInnerCornerX);
    frFaceInfo.sLandmarkLocation.sRightEyeInnerCorner.nLocY = jenv->GetIntField(landmark, rightEyeInnerCornerY);
    frFaceInfo.sLandmarkLocation.sRightEyeOuterCorner.nLocX = jenv->GetIntField(landmark, rightEyeOuterCornerX);
    frFaceInfo.sLandmarkLocation.sRightEyeOuterCorner.nLocY = jenv->GetIntField(landmark, rightEyeOuterCornerY);
    frFaceInfo.sLandmarkLocation.sMouthLeft.nLocX           = jenv->GetIntField(landmark, mouthLeftX);
    frFaceInfo.sLandmarkLocation.sMouthLeft.nLocY           = jenv->GetIntField(landmark, mouthLeftY);
    frFaceInfo.sLandmarkLocation.sMouthRight.nLocX          = jenv->GetIntField(landmark, mouthRightX);
    frFaceInfo.sLandmarkLocation.sMouthRight.nLocY          = jenv->GetIntField(landmark, mouthRightY);
    frFaceInfo.sLandmarkLocation.sNoseLeft.nLocX            = jenv->GetIntField(landmark, noseLeftX);
    frFaceInfo.sLandmarkLocation.sNoseLeft.nLocY            = jenv->GetIntField(landmark, noseLeftY);
    frFaceInfo.sLandmarkLocation.sNoseRight.nLocX           = jenv->GetIntField(landmark, noseRightX);
    frFaceInfo.sLandmarkLocation.sNoseRight.nLocY           = jenv->GetIntField(landmark, noseRightY);
    frFaceInfo.sLandmarkLocation.sMouthUp.nLocX             = jenv->GetIntField(landmark, mouthUpX);
    frFaceInfo.sLandmarkLocation.sMouthUp.nLocY             = jenv->GetIntField(landmark, mouthUpY);

    Status ret = nativeContext->faceRecoHandler->EnrollFace(&nativeContext->imageBuffer, &frFaceInfo, pFeatureID);

    jenv->ReleaseByteArrayElements(featureID, (jbyte*)pFeatureID, 0);

    return ret;
}

/*
 * Method:    nEnrollFace
 * Signature: "([BLcom/qualcomm/qti/facerecognition/FaceInfo;[B)I"
 */
JNIEXPORT jint JNICALL nEnrollFace(JNIEnv* jenv, jobject obj, jbyteArray imageData, jobject faceInfo, jbyteArray featureID)
{
    if (imageData == NULL || featureID == NULL)
    {
        LOGE("Fail invalid param at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    uint8_t *pImageData = (uint8_t*)jenv->GetByteArrayElements(imageData, NULL);
    if (pImageData == NULL)
    {
        LOGE("Failed to retrieve imageData");
        return SCVE_ERROR_INVALID_PARAM;
    }

    jint result = enrollFace(jenv, obj, pImageData, faceInfo, featureID);

    jenv->ReleaseByteArrayElements(imageData, (jbyte*)pImageData, 0);

    return result;
}

/*
 * Method:    nEnrollFace
 * Signature: "(Ljava/nio/ByteBuffer;Lcom/qualcomm/qti/facerecognition/FaceInfo;[B)I"
 */
JNIEXPORT jint JNICALL nEnrollFaceDirect(JNIEnv* jenv, jobject obj, jbyteArray imageData, jobject faceInfo, jbyteArray featureID)
{
    if (imageData == NULL || featureID == NULL)
    {
        LOGE("Fail invalid param at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    uint8_t *pImageData = (uint8_t*)jenv->GetDirectBufferAddress(imageData);
    if (pImageData == NULL)
    {
        LOGE("Failed to get direct buffer address");
        return SCVE_ERROR_INVALID_PARAM;
    }

    return enrollFace(jenv, obj, pImageData, faceInfo, featureID);
}

/*
 * Method:    nRemoveFromDatabase
 * Signature: ([B)I
 */
JNIEXPORT jint JNICALL nRemoveFromDatabase(JNIEnv* jenv, jobject obj, jbyteArray featureID)
{
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
      field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL || nativeContext->faceRecoHandler == NULL)
    {
       LOGE("Fail at %s", __FUNCTION__);
       return SCVE_ERROR_INTERNAL;
    }

    if (featureID == NULL)
    {
        LOGE("Fail invalid param at %s", __FUNCTION__);
        return SCVE_ERROR_INVALID_PARAM;
    }

    const char *pFeatureID = (const char*)jenv->GetByteArrayElements(featureID, NULL);

    jint ret = nativeContext->faceRecoHandler->RemoveFromDatabase(pFeatureID);

    jenv->ReleaseByteArrayElements(featureID, (jbyte*)pFeatureID, 0);

    return ret;
}

/*
 * Method:    nSaveDatabase
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL nSaveDatabase(JNIEnv* jenv, jobject obj, jstring databaseName)
{
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
      field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL || nativeContext->faceRecoHandler == NULL)
    {
       LOGE("Fail at %s", __FUNCTION__);
       return SCVE_ERROR_INTERNAL;
    }

    jint eRet = SCVE_SUCCESS;
    const char *nativeString = jenv->GetStringUTFChars(databaseName, 0);

    eRet = nativeContext->faceRecoHandler->SaveDatabase(nativeString);

    jenv->ReleaseStringUTFChars(databaseName, nativeString);
    return eRet;
}

/*
 * Method:    nGetNumDatabaseUsers
 * Signature: ()I
 */
JNIEXPORT jint JNICALL nGetNumDatabaseUsers(JNIEnv* jenv, jobject obj)
{
    NativeContext *nativeContext = (NativeContext *) jenv->GetLongField(obj,
      field_FaceRecoWrapper_nativeContext_J);
    if (nativeContext == NULL || nativeContext->faceRecoHandler == NULL)
    {
       LOGE("Fail at %s", __FUNCTION__);
       return SCVE_ERROR_INTERNAL;
    }

    uint32_t nUsers = 0;
    nativeContext->faceRecoHandler->GetNumDatabaseUsers(&nUsers);

    return (jint) nUsers;
}


///////////////////////////////////////// JNI INTERFACE ////////////////////////////////////////////

static const JNINativeMethod faceRecoMethods[] = {
        { "nClassInit"             , "()V"                    , (void *) nClassInit              },
        { "nCreateScveContext"     , "(I)I"                   , (void *) nCreateScveContext      },
        { "nFreeScveContext"       , "()I"                    , (void *) nFreeScveContext        },
        { "nCreateFaceRecognition" , "(Ljava/lang/String;I)I" , (void *) nCreateFaceRecognition  },
        { "nFreeFaceRecognition"   , "()I"                    , (void *) nFreeFaceRecognition    },
        { "nInit"                  , "(IIIIIII)I"             , (void *) nInit                   },
        { "nDetectFaces"           , "([B[Lcom/qualcomm/qti/facerecognition/FaceInfo;)I", (void *) nDetectFaces    },
        { "nDetectFaces"           , "(Ljava/nio/ByteBuffer;[Lcom/qualcomm/qti/facerecognition/FaceInfo;)I", (void *) nDetectFacesDirect    },
        { "nRecognizeFace"         , "([BLcom/qualcomm/qti/facerecognition/FaceInfo;I[Lcom/qualcomm/qti/facerecognition/FaceRecognitionResult;)I", (void *) nRecognizeFace },
        { "nRecognizeFace"         , "(Ljava/nio/ByteBuffer;Lcom/qualcomm/qti/facerecognition/FaceInfo;I[Lcom/qualcomm/qti/facerecognition/FaceRecognitionResult;)I", (void *) nRecognizeFaceDirect },
        { "nEnrollFace"            , "([BLcom/qualcomm/qti/facerecognition/FaceInfo;[B)I", (void *) nEnrollFace },
        { "nEnrollFace"            , "(Ljava/nio/ByteBuffer;Lcom/qualcomm/qti/facerecognition/FaceInfo;[B)I", (void *) nEnrollFaceDirect },
        { "nRemoveFromDatabase"    , "([B)I"                  , (void *) nRemoveFromDatabase     },
        { "nSaveDatabase"          , "(Ljava/lang/String;)I"  , (void *) nSaveDatabase           },
        { "nGetNumDatabaseUsers"   , "()I"                    , (void *) nGetNumDatabaseUsers    },
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    jclass cls = env->FindClass(FR_CLASS);
    if (env->ExceptionCheck()) {
        return -1;
    }
    env->RegisterNatives(cls, faceRecoMethods, sizeof(faceRecoMethods) / sizeof(faceRecoMethods[0]));
    if (env->ExceptionCheck()) {
        return -1;
    }

    return JNI_VERSION_1_4;
}

