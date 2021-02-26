/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <unistd.h>

#include "scveObjectSegmentation.hpp"
#include "scveObjectMatting.hpp"

#define  LOG_TAG    "ObjectSegmentationSampleJNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using namespace SCVE;

#define ILLEGAL_ARGUMENT_EXCEPTION  "java/lang/IllegalArgumentException"
#define RUNTIME_EXCEPTION        "java/lang/RuntimeException"
#define NO_SUCH_METHOD_EXCEPTION    "java/lang/NoSuchMethodException"
#define NO_SUCH_FIELD_EXCEPTION    "java/lang/NoSuchFieldException"

typedef struct {
   Context *context;
   ObjectMatting *matteContext;
   ObjectSegmentation *segmentationContext;
   Image sourceImage;
   jobject sourceBitmapReference;
} InstanceData;

static jfieldID softSegmentationJNILib_context;

static void throwException(JNIEnv *env, const char *exceptionClassName, const char *msg)
{
   // Don't squelch an existing exception
   if (env->ExceptionOccurred() == NULL) {
      jclass exceptionClass = env->FindClass(exceptionClassName);
      if (exceptionClass != NULL) {
         env->ThrowNew(exceptionClass, msg);
      }
   }
}

static InstanceData *getInstanceData(JNIEnv *env, jobject object)
{
   return (InstanceData *)env->GetLongField(object, softSegmentationJNILib_context);
}

static void setInstanceData(JNIEnv *env, jobject object, InstanceData *instanceData)
{
   env->SetLongField(object, softSegmentationJNILib_context, (jlong)instanceData);
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib
 * Method:    nClassInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib_nClassInit
  (JNIEnv *env, jclass javaClass)
{
   softSegmentationJNILib_context = env->GetFieldID(javaClass, "nativeContext", "J");
   if(softSegmentationJNILib_context == NULL)
   {
      throwException(env, NO_SUCH_FIELD_EXCEPTION, "nativeContext:FieldNotFound");
      return;
   }
}

/*
 * Class:     com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib
 * Method:    nInit
 * Signature: (Landroid/graphics/Bitmap;)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib_nInit
  (JNIEnv *env, jobject object, jobject sourceBitmap, jint segmentationMethod)
{
   InstanceData *instanceData = getInstanceData(env, object);
   if (instanceData != NULL ) {
      throwException(env, RUNTIME_EXCEPTION, "Context already initialized");
      return;
   }

   Context *context = SCVE::Context::newInstance();
   if (context == NULL)
   {
      throwException(env, RUNTIME_EXCEPTION, "Failed to create SCVE context");
      return;
   }

   ObjectMatting *matteContext = SCVE::ObjectMatting::newInstance(context);
   if (matteContext == NULL) {
      throwException(env, RUNTIME_EXCEPTION, "Failed to create SCVE Matting context");
      SCVE::Context::deleteInstance(context);
      return;
   }

   ObjectSegmentation *segmentationContext = SCVE::ObjectSegmentation::newInstance(context);
   if(segmentationContext == NULL) {
      throwException(env, RUNTIME_EXCEPTION, "Failed to create SCVE Segmentation context");
      SCVE::Context::deleteInstance(context);
      SCVE::ObjectMatting::deleteInstance(matteContext);
   }

   instanceData = new InstanceData();
   instanceData->context = context;
   instanceData->matteContext = matteContext;
   instanceData->segmentationContext = segmentationContext;
   instanceData->sourceBitmapReference = env->NewGlobalRef(sourceBitmap);

   int *_sourceBitmap;
   AndroidBitmapInfo info;
   AndroidBitmap_getInfo(env, sourceBitmap, &info);
   AndroidBitmap_lockPixels(env, sourceBitmap, (void**)&_sourceBitmap);

   instanceData->sourceImage.sDim.nWidth = info.width;
   instanceData->sourceImage.sDim.nHeight = info.height;
   instanceData->sourceImage.sDim.nStride = info.stride;
   instanceData->sourceImage.pPtr = (uint8_t*)_sourceBitmap;
   instanceData->sourceImage.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;
   instanceData->sourceImage.nChannels = 4;

   if (segmentationMethod == 0)
      segmentationContext->init(&instanceData->sourceImage, SCVE::SCVE_SCRIBBLE_SELECTION);
   else if (segmentationMethod == 1)
      segmentationContext->init(&instanceData->sourceImage, SCVE::SCVE_QUICK_SELECTION);

   matteContext->init(&instanceData->sourceImage);
   setInstanceData(env, object, instanceData);
}

/*
 * Class:     com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib
 * Method:    nProcessSegmentation
 * Signature: (Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib_nProcessSegmentation
  (JNIEnv *env, jobject object, jobject inputMaskBitmap, jobject outputMaskBitmap, jobject previousMaskBitmap)
{
   InstanceData *instanceData = getInstanceData(env, object);
   int *_inputMaskBitmap, *_outputMaskBitmap, *_previousMaskBitmap;

   AndroidBitmapInfo inputMaskInfo;
   AndroidBitmapInfo outputMaskInfo;
   AndroidBitmapInfo previousMaskInfo;

   AndroidBitmap_getInfo(env, inputMaskBitmap, &inputMaskInfo);
   AndroidBitmap_lockPixels(env, inputMaskBitmap, (void**)&_inputMaskBitmap);
   AndroidBitmap_getInfo(env, outputMaskBitmap, &outputMaskInfo);
   AndroidBitmap_lockPixels(env, outputMaskBitmap, (void**)&_outputMaskBitmap);

   if (previousMaskBitmap != NULL)
   {
      AndroidBitmap_getInfo(env, previousMaskBitmap, &previousMaskInfo);
      AndroidBitmap_lockPixels(env, previousMaskBitmap, (void**)&_previousMaskBitmap);
   }

   Image inputMask;
   inputMask.sDim.nWidth = inputMaskInfo.width;
   inputMask.sDim.nHeight = inputMaskInfo.height;
   inputMask.sDim.nStride = inputMaskInfo.stride;
   inputMask.pPtr = (uint8_t*)_inputMaskBitmap;
   inputMask.sFormat = SCVE::SCVE_COLORFORMAT_GREY_8BIT;
   inputMask.nChannels = 1;

   Image outputMask;
   outputMask.sDim.nWidth = outputMaskInfo.width;
   outputMask.sDim.nHeight = outputMaskInfo.height;
   outputMask.sDim.nStride = outputMaskInfo.stride;
   outputMask.pPtr = (uint8_t*)_outputMaskBitmap;
   outputMask.sFormat = SCVE::SCVE_COLORFORMAT_GREY_8BIT;
   outputMask.nChannels = 1;

   Image previousMask;
   if (previousMaskBitmap != NULL)
    {
      previousMask.sDim.nWidth = previousMaskInfo.width;
      previousMask.sDim.nHeight = previousMaskInfo.height;
         previousMask.sDim.nStride = previousMaskInfo.stride;
         previousMask.pPtr = (uint8_t*)_previousMaskBitmap;
         previousMask.sFormat = SCVE::SCVE_COLORFORMAT_GREY_8BIT;
         previousMask.nChannels = 1;
   }

   LOGD("before segmentationContext run_sync");

   SCVE::Status processPassed;

   if (previousMaskBitmap != NULL)
      processPassed = instanceData->segmentationContext->Run_Sync(&inputMask, &outputMask, &previousMask);
   else
      processPassed = instanceData->segmentationContext->Run_Sync(&inputMask, &outputMask);

   LOGD("after segmentationContext run_sync with result = 0x%02X", processPassed);

   AndroidBitmap_unlockPixels(env, inputMaskBitmap);
   AndroidBitmap_unlockPixels(env, outputMaskBitmap);
   AndroidBitmap_unlockPixels(env, previousMaskBitmap);

   return processPassed;
}

/*
 * Class:     com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib
 * Method:    nProcessMatting
 * Signature: (Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib_nProcessMatting
  (JNIEnv *env, jobject object, jobject trimapBitmap, jobject outputMatteBitmap, jobject outputForegroundBitmap, jobject outputBackgroundBitmap)
{
   InstanceData *instanceData = getInstanceData(env, object);
   int *_trimap, *_ouputMask, *_outputForeground, *_outputBackground;

   AndroidBitmapInfo trimapInfo;
   AndroidBitmapInfo matteInfo;
   AndroidBitmapInfo foregroundInfo;
   AndroidBitmapInfo backgroundInfo;
   AndroidBitmap_getInfo(env, trimapBitmap, &trimapInfo);
   AndroidBitmap_lockPixels(env, trimapBitmap, (void**)&_trimap);
   AndroidBitmap_getInfo(env, outputMatteBitmap, &matteInfo);
   AndroidBitmap_lockPixels(env, outputMatteBitmap, (void**)&_ouputMask);
   AndroidBitmap_getInfo(env, outputForegroundBitmap, &foregroundInfo);
   AndroidBitmap_lockPixels(env, outputForegroundBitmap, (void**)&_outputForeground);
   AndroidBitmap_getInfo(env, outputBackgroundBitmap, &backgroundInfo);
   AndroidBitmap_lockPixels(env, outputBackgroundBitmap, (void**)&_outputBackground);

   Image trimap;
   trimap.sDim.nWidth = trimapInfo.width;
   trimap.sDim.nHeight = trimapInfo.height;
   trimap.sDim.nStride = trimapInfo.stride;
   trimap.pPtr = (uint8_t*)_trimap;
   trimap.sFormat = SCVE::SCVE_COLORFORMAT_GREY_8BIT;
   trimap.nChannels = 1;

   Image matte;
   matte.sDim.nWidth = matteInfo.width;
   matte.sDim.nHeight = matteInfo.height;
   matte.sDim.nStride = matteInfo.stride;
   matte.pPtr = (uint8_t*)_ouputMask;
   matte.sFormat = SCVE::SCVE_COLORFORMAT_GREY_8BIT;
   matte.nChannels = 1;

   Image outputForeground;
   outputForeground.sDim.nWidth = foregroundInfo.width;
   outputForeground.sDim.nHeight = foregroundInfo.height;
   outputForeground.sDim.nStride = foregroundInfo.stride;
   outputForeground.pPtr = (uint8_t*)_outputForeground;
   outputForeground.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;
   outputForeground.nChannels = 4;

   Image outputBackground;
   outputBackground.sDim.nWidth = backgroundInfo.width;
   outputBackground.sDim.nHeight = backgroundInfo.height;
   outputBackground.sDim.nStride = backgroundInfo.stride;
   outputBackground.pPtr = (uint8_t*)_outputBackground;
   outputBackground.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;
   outputBackground.nChannels = 4;

   LOGD("before matteContext run_sync");
   SCVE::Status processPassed = instanceData->matteContext->Run_Sync(&trimap, &matte, &outputForeground, &outputBackground);
   LOGD("after matteContext run_sync with result = 0x%02X", processPassed);

   AndroidBitmap_unlockPixels(env, trimapBitmap);
   AndroidBitmap_unlockPixels(env, outputMatteBitmap);
   AndroidBitmap_unlockPixels(env, outputForegroundBitmap);
   AndroidBitmap_unlockPixels(env, outputBackgroundBitmap);

   return processPassed;
}

/*
 * Class:     com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib
 * Method:    nDestroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_jnilib_SoftSegmentationJNILib_nDestroy
  (JNIEnv *env, jobject object)
{
   InstanceData *instanceData = getInstanceData(env, object);

   if(instanceData != NULL)
   {
      if(instanceData->sourceBitmapReference != NULL)
      {
         AndroidBitmap_unlockPixels(env, instanceData->sourceBitmapReference);
         env->DeleteGlobalRef(instanceData->sourceBitmapReference);
      }
      if(instanceData->matteContext != NULL)
      {
         SCVE::ObjectMatting::deleteInstance(instanceData->matteContext);
      }

      if(instanceData->context != NULL)
      {
         SCVE::Context::deleteInstance(instanceData->context);
      }

      delete instanceData;
      setInstanceData(env, object, NULL);
   }
}

#ifdef __cplusplus
}
#endif
