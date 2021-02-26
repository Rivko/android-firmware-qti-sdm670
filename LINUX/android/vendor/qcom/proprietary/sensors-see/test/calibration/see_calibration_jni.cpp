/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file see_calibration_jni.cpp
  @brief
  JNI implementation for see_calibration .
============================================================================*/
#include <jni.h>
#include "see_calibration_accel.h"
#include <vector>
#include <string>
#include <mutex>
using namespace std;
#define UNUSED_VAR(var) var

vector<string> samplesBufferPool;
std::mutex g_samples_mutex;
bool g_locked_samples_mutex;
accel_cal* g_accel_cal_var = NULL;
void streamEventsDisplayCallback(string displayCalSamples)
{
  samplesBufferPool.insert(samplesBufferPool.begin(), displayCalSamples);
  if(g_locked_samples_mutex == true)
      g_samples_mutex.unlock();
}

void startCalibration(JNIEnv *env, jobject obj )
{
  if(g_accel_cal_var == NULL)
  {
    g_accel_cal_var = new accel_cal();
    display_accel_cal_cb func_ptr = streamEventsDisplayCallback;
    g_accel_cal_var->register_calibrated_values_cb_func(func_ptr);
    sns_logi("calling cal_init from JNI");
    g_accel_cal_var->cal_init();
  }

}

void stopCalibration(JNIEnv *env, jobject obj)
{
  if(g_accel_cal_var != NULL)
  {
    delete g_accel_cal_var;
    g_accel_cal_var = NULL;
  }
}

jstring getAccelCalFromNative(JNIEnv *env, jobject obj)
{

  UNUSED_VAR(env);
  UNUSED_VAR(obj);

  jstring current_sample;
  if(!samplesBufferPool.empty())
  {
    string currentSample = samplesBufferPool.back();
    samplesBufferPool.pop_back();
    current_sample = env->NewStringUTF(currentSample.c_str());
    return current_sample;
  }else {
    g_samples_mutex.lock();
    g_locked_samples_mutex = true;
  }

  return NULL;
}

/*
 * Class and package name
 * */
static const char *classPathName = "com/qualcomm/qti/usta/core/AccelCalibrationJNI";

/*
 * List of native methods
 *
 * */
static JNINativeMethod methods[] = {
    {"startCalibration" , "()V", (void *)startCalibration},
    {"getAccelCalFromNative" , "()Ljava/lang/String;", (void *)getAccelCalFromNative},
    {"stopCalibration" , "()V", (void *)stopCalibration},
};

/*
 * Register several native methods for one class.
 *
 *
 * */
static int registerUSTANativeMethods(JNIEnv* envVar, const char* inClassName, JNINativeMethod* inMethodsList, int inNumMethods)
{
  jclass javaClazz = envVar->FindClass(inClassName);
  if (javaClazz == NULL)
  {
    return JNI_FALSE;
  }
  if (envVar->RegisterNatives(javaClazz, inMethodsList, inNumMethods) < 0)
  {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * Returns JNI_TRUE on success
 *
 * */
static int registerNatives(JNIEnv* env)
{
  if (!registerUSTANativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0])))
  {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

/*
 * This is called by the VM when the shared library is first loaded.
 * */
typedef union
{
  JNIEnv* env;
  void* venv;
}UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
  UNUSED_VAR(reserved);

  UnionJNIEnvToVoid uenv;
  uenv.venv = NULL;
  JNIEnv* env = NULL;
  //log_instance = usta_logging_util::create_instance();
  //INFO_LOG(log_instance , "JNI_OnLoad Start");

  if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
  {
    //ERROR_LOG(log_instance , "ERROR: GetEnv failed");
    return -1;
  }
  env = uenv.env;

  if (registerNatives(env) != JNI_TRUE)
  {
    //ERROR_LOG(log_instance , "ERROR: registerNatives failed");
    return -1;
  }
  //INFO_LOG(log_instance , "JNI_OnLoad End");

  return JNI_VERSION_1_4;
}
