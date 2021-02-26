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

#include <scveImageRemoval.hpp>

#define  LOG_TAG    "ImageRemovalJNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using namespace SCVE;

jboolean runGpu = 0;
Context *ctx = NULL;
ImageRemoval *instance = NULL;
Image mImage, mMask;
JavaVM *g_jvm;
jobject g_obj;
bool isInited = false;

extern "C"
{
/*
 * Class:     com_imageediting_objectremoval_CL2JNILib
 * Method:    removalInit
 * Signature: (Lcom/imageediting/objectremoval/ObjectRemover;)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalInit
(JNIEnv *, jobject, jobject, jboolean);

/*
 * Class:     com_imageediting_objectremoval_CL2JNILib
 * Method:    removalDeInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalDeInit
(JNIEnv *, jobject);

/*
 * Class:     com_imageediting_objectremoval_CL2JNILib
 * Method:    removalOperation
 * Signature: (Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;I)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalOperation
(JNIEnv *, jobject, jobject, jobject, jint);

/*
 * Class:     com_imageediting_objectremoval_CL2JNILib
 * Method:    removalStop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalStop
(JNIEnv *, jobject);
};

//For measuring time.
uint64_t t1=0, t2=0;

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

void object_removal_async_cb(SCVE::Status result, uint32_t finished, Image *image, SCVE::Rect rect, void *session_user_data, void *target_user_data)
{
   if (finished)
      LOGI("++ removal finished");
   if (image != &mImage)
      LOGD("wrong output image");
   if (SCVE_SUCCESS == result && finished != 0)
   {
      int status;
      jmethodID java_mid = 0;
      JNIEnv *env;
      jclass jcls;
      int isAttached = 0;

      GetTime(&t2);
      LOGD("++ removalOperation run-time=%lld", (t2-t1));

      status = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
      if (status < 0)
      {
         //LOGI("callback_handler: failed to get JNI environment, assuming native thread");
         status = g_jvm->AttachCurrentThread(&env, NULL);
         if (status < 0)
         {
            LOGE("callback_handler: failed to attach current thread");
            return;
         }
         isAttached = 1;
      }
      if (g_obj != NULL)
      {
         jcls = env->GetObjectClass(g_obj);
         if (jcls)
         {
            java_mid = env->GetMethodID(jcls, "removalCallBack", "(ZIIII)V");
            if (java_mid != 0)
            {
               env->CallVoidMethod(g_obj, java_mid, finished, rect.nLocX, rect.nLocY, rect.nLocX + rect.nWidth, rect.nLocY + rect.nHeight);
            }
         }
      }
      if (isAttached)
         g_jvm->DetachCurrentThread();
   }
   return;
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalInit
(JNIEnv *env, jobject obj, jobject jarg1, jboolean gpuEnabled)
{
   LOGD("++ removalInit instance=%p, gpuEnabled %d", instance, gpuEnabled);

   runGpu = gpuEnabled;

   if (NULL == ctx)
   {
      if (!runGpu)
         ctx = Context::newInstance(SCVE_MODE_DEFAULT);
      else
         ctx = Context::newInstance(SCVE_MODE_CPU_OFFLOAD);
   }

   if (NULL == instance)
   {
      instance = ImageRemoval::newInstance(ctx, object_removal_async_cb);
   }

   if(g_obj)
      env->DeleteGlobalRef(g_obj);

   g_obj = env->NewGlobalRef(jarg1);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalDeInit
(JNIEnv *env, jobject obj)
{
   LOGD("++ removalDeinit instance=%p", instance);
   if (NULL != instance)
   {
      ImageRemoval::deleteInstance(instance);
      instance = NULL;
   }

   if (NULL != ctx)
   {
      Context::deleteInstance(ctx);
      ctx = NULL;
   }

   if (g_obj)
   {
      env->DeleteGlobalRef(g_obj);
      g_obj = NULL;
   }
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalOperation
(JNIEnv *env, jobject obj, jobject image, jobject mask, jint patchSize)
{
   LOGD("++ removalOperation instance=%p, image=%p, mask=%p", instance, image, mask);
   int *_image, *_mask;
   AndroidBitmapInfo info;
   AndroidBitmap_getInfo(env, image, &info);
   AndroidBitmap_lockPixels(env, image, (void **)&_image);
   AndroidBitmap_getInfo(env, mask, &info);
   AndroidBitmap_lockPixels(env, mask, (void **)&_mask);

   mImage.sDim.nWidth = info.width;
   mImage.sDim.nHeight = info.height;
   mImage.sDim.nStride = info.stride;
   mImage.pPtr = (uint8_t *)_image;
   mImage.nChannels = 4;
   mImage.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;

   mMask.sDim.nWidth = info.width;
   mMask.sDim.nHeight = info.height;
   mMask.sDim.nStride = info.stride;
   mMask.pPtr = (uint8_t *)_mask;
   mMask.nChannels = 4;
   mMask.sFormat = SCVE::SCVE_COLORFORMAT_RGBA_8888;
   isInited = true;

   if (NULL != instance)
   {
      GetTime(&t1);
      instance->Run_Async(&mImage, &mMask, patchSize);
   }

   AndroidBitmap_unlockPixels(env, image);
   AndroidBitmap_unlockPixels(env, mask);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_imageedit_removal_RemovalJNILib_removalStop
(JNIEnv *env, jobject obj)
{
   if (NULL != instance)
   {
      instance->Stop_Async();
   }
}
