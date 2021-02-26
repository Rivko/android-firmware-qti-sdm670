/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <unistd.h>
#include <time.h>

#include <scveImageCloning.hpp>

#define  LOG_TAG    "ImageCloningSampleJNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define CLONING_SYNC

using namespace SCVE;

jboolean runGpu = 0;
Context *ctx = NULL;
ImageCloning *instance = NULL;
bool isInited = false;
Image mSrc, mDst, mMask;
JavaVM *g_jvm;
jobject g_obj;

extern "C"
{
/*
 * Class:     com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib
 * Method:    cloningInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningInit
(JNIEnv *, jobject, jboolean);

/*
 * Class:     com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib
 * Method:    cloningDeinit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningDeinit
(JNIEnv *, jobject);

/*
 * Class:     com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib
 * Method:    cloningInitImage
 * Signature: (Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;Z)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningInitImage
(JNIEnv *, jobject, jobject, jobject, jboolean);

/*
 * Class:     com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib
 * Method:    cloningOperation
 * Signature: (Landroid/graphics/Bitmap;)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningOperation
(JNIEnv *, jobject, jobject);
};

void GetTime(uint64_t *time)
{
   struct timeval tv;
   struct timezone tz;
   gettimeofday(&tv, &tz);
   *time = tv.tv_sec * 1000000ULL + tv.tv_usec;
}

jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{

   JNIEnv *localenv;

   g_jvm = jvm;
   if (g_jvm == NULL)
   {
      LOGE("JVM NOT OBTAINED!!!");
      return JNI_ERR;
   }
   LOGD("JVM OBTAINED!!!");

   if (g_jvm->GetEnv((void **)&localenv, JNI_VERSION_1_6) == JNI_OK)
   {
      LOGD("ENV OBTAINED!!!");
      return JNI_VERSION_1_6;
   }
   else
   {
      LOGE("ENV NOT OBTAINED!!!");
      return 0;
   }
   return JNI_VERSION_1_6;
}

Status object_cloning_async_cb(SCVE::Status result, Image *pImage, void *session_user_data, void *target_user_data)
{
   LOGD("++ object_extraction_async_cb");
   if (SCVE_SUCCESS == result)
   {
      int status;
      jmethodID java_mid = 0;
      JNIEnv *env;
      jclass jcls;
      int isAttached = 0;

      status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
      if (status < 0)
      {
         LOGI("callback_handler: failed to get JNI environment, assuming native thread");
         status = g_jvm->AttachCurrentThread(&env, NULL);
         if (status < 0)
         {
            LOGE("callback_handler: failed to attach current thread");
            return SCVE_ERROR_INTERNAL;
         }
         isAttached = 1;
      }
      if (g_obj != NULL)
      {
         jcls = env->GetObjectClass(g_obj);
         if (jcls)
         {
            java_mid = env->GetMethodID(jcls, "cloningCallback", "()V");
            if (java_mid != 0)
            {
               env->CallVoidMethod(g_obj, java_mid);
            }
         }
      }
      if (isAttached)
         g_jvm->DetachCurrentThread();
   }
   return SCVE_SUCCESS;
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningInit
(JNIEnv *env, jobject obj, jboolean gpuEnabled)
{
   LOGD("++ cloningInit instance=%p, gpuEnabled=%d", instance, gpuEnabled);

   runGpu = gpuEnabled;

   if (NULL == ctx)
   {
      if (!runGpu)
         ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_DEFAULT);
      else
         ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_CPU_OFFLOAD);
   }

#ifndef CLONING_SYNC
   if (NULL == instance)
      instance = ImageCloning::newInstance(ctx, object_cloning_async_cb);
#else
   if (NULL == instance)
      instance = ImageCloning::newInstance(ctx);
#endif
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningDeinit
(JNIEnv *env, jobject obj)
{
   LOGD("++ cloningDeinit instance=%p", instance);
   if (NULL != instance)
   {
      ImageCloning::deleteInstance(instance);
      instance = NULL;
   }
   if (NULL != ctx)
   {
      SCVE::Context::deleteInstance(ctx);
      ctx = NULL;
   }
   isInited = false;
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningInitImage
(JNIEnv *env, jobject obj, jobject src, jobject mask, jboolean isFilled)
{
   LOGD("++ cloningInitImage instance=%p, src=%p, mask=%p", instance, src, mask);

   if (NULL == ctx)
   {
      if (!runGpu)
         ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_DEFAULT);
      else
         ctx = SCVE::Context::newInstance(SCVE::SCVE_MODE_CPU_OFFLOAD);
   }

#ifndef CLONING_SYNC
   if (NULL == instance)
      instance = ImageCloning::newInstance(ctx, object_cloning_async_cb);
#else
   if (NULL == instance)
      instance = ImageCloning::newInstance(ctx);
#endif

   if (false == isInited && NULL != instance)
   {
      uint8_t *_src, *_mask;
      uint32_t maskBufSz;
      AndroidBitmapInfo info;
      AndroidBitmap_getInfo(env, src, &info);
      AndroidBitmap_lockPixels(env, src, (void **)&_src);
      maskBufSz = info.stride * info.height;

      mSrc.sDim.nWidth = info.width;
      mSrc.sDim.nHeight = info.height;
      mSrc.sDim.nStride = info.stride;
      mSrc.pPtr = _src;
      mSrc.nChannels = 4;
      mSrc.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;

      mMask.sDim.nWidth = info.width;
      mMask.sDim.nHeight = info.height;
      mMask.sDim.nStride = info.stride;
      mMask.pPtr = (uint8_t *)malloc(maskBufSz);
      mMask.nChannels = 4;
      mMask.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;

      // Populate RGBA mask from input mask bitmap's alpha channel
      AndroidBitmap_lockPixels(env, mask, (void **)&_mask);
      for (int i = 0; i < info.height; i++) {
         uint32_t *srcRowBase = (uint32_t *)(_mask + info.stride * i);
         uint32_t *dstRowBase = (uint32_t *)(mMask.pPtr + info.stride * i);
         for (int j = 0; j < info.width; j++) {
            dstRowBase[j] = 0xFF000000 | ((srcRowBase[j] >> 16) & 0x0000FF00);
         }
      }
      AndroidBitmap_unlockPixels(env, mask);

      instance->Init(&mSrc, &mMask, isFilled);

      AndroidBitmap_unlockPixels(env, src);
      free(mMask.pPtr);
      isInited = true;
   }
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_objectclone_CL2JNILib_cloningOperation
(JNIEnv *env, jobject obj, jobject dst)
{
   LOGD("++ cloningOperation instance=%p, dst=%p", instance, dst);
   uint64_t t1=0, t2=0;
   int *_dst;
   AndroidBitmapInfo info;
   AndroidBitmap_getInfo(env, dst, &info);
   AndroidBitmap_lockPixels(env, dst, (void **)&_dst);

   mDst.sDim.nWidth = info.width;
   mDst.sDim.nHeight = info.height;
   mDst.sDim.nStride = info.stride;
   mDst.pPtr = (uint8_t *)_dst;
   mDst.nChannels = 4;
   mDst.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;

   if (isInited)
   {
#ifndef CLONING_SYNC
      instance->Run_Async(&mDst);
#else
      GetTime(&t1);
      Status res = instance->Run_Sync(&mDst);
      GetTime(&t2);
      LOGD("++ cloningOperation run-time=%lld", (t2-t1));
      //object_cloning_async_cb(res, &mDst, NULL, NULL);
#endif
   }
   AndroidBitmap_unlockPixels(env, dst);
}


