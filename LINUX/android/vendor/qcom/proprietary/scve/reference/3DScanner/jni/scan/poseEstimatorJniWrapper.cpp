/**********************************************************
 * poseEstimatorJniWrapper.cpp
 *
 * Author: Michel Sarkis
 *
 * Description: Routines that provide the Interface between Java and lib3DR.
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 **********************************************************/


#include "poseEstimatorJniWrapper.hpp"
#include "3DR_APIs.h"
#include "Affinity.h"
//#include "ScanInterface/ScanInterface.h"
//#include "ScanInterface/TestScanInterface.h"
#include <android/log.h>

// General JNI Miscellaneous
#ifdef LOG_TAG
#undef LOG_TAG
#define  LOG_TAG    "Scan3D"
#endif
#define  DPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//Device Used
char device[100];
std::thread  GenMesh_thread;

// ====== JNI function implementation ======
JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_init(JNIEnv * env, jobject obj,
                                                                   jstring filesdir,
                                                                   jstring externalFilesDir,
                                                                   jobject assetManager)
{

  if(strcmp(device,"8996")==0){
   cpu_set_t  mask;
   int affinity_result;
   CPU_ZERO(&mask);
   //CPU_SET(0, &mask);
   //CPU_SET(1, &mask);
   CPU_SET(2, &mask);
   CPU_SET(3, &mask);
   setCurrentThreadAffinityMask(mask, affinity_result);
   if (affinity_result == 1){
     LOGI("setaffinity failed!!");
   }else{
     LOGI("setaffinity success!!");
   }

    }else if(strcmp(device,"8994")==0){
      cpu_set_t  mask;
      int affinity_result;
      CPU_ZERO(&mask);
       CPU_SET(4, &mask);
      CPU_SET(5, &mask);
      CPU_SET(6, &mask);
      CPU_SET(7, &mask);
      setCurrentThreadAffinityMask(mask, affinity_result);
      if (affinity_result == 1){
        LOGI("setaffinity failed!!");
      }else{
        LOGI("setaffinity success!!");
      }
    }else if(strcmp(device,"8974")==0){
   cpu_set_t  mask;
   int affinity_result;
   CPU_ZERO(&mask);
   CPU_SET(0, &mask);
   CPU_SET(1, &mask);
   CPU_SET(2, &mask);
   CPU_SET(3, &mask);
   setCurrentThreadAffinityMask(mask, affinity_result);
   if (affinity_result == 1){
     LOGI("setaffinity failed!!");
   }else{
     LOGI("setaffinity success!!");
   }
  }

  LOGI(":: GL2JNILib_init 1");

  // init modules
  char configFileName[PATH_MAX];
  jboolean isCopy = false;
  const char* externalFilesDirString = env->GetStringUTFChars(externalFilesDir, &isCopy);
  snprintf(configFileName, sizeof(configFileName), "%s/SamplesConfig.xml", env->GetStringUTFChars(filesdir, &isCopy));

  LOGI(":: GL2JNILib_init 2");
  //Pass the location into the init Module
  initModules(externalFilesDirString, configFileName, AAssetManager_fromJava(env, assetManager));

  LOGI(":: GL2JNILib_init 3");
  env->ReleaseStringUTFChars(externalFilesDir, externalFilesDirString);
  LOGI(":: GL2JNILib_init 4");


}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_initPreview(JNIEnv * env, jobject obj,
                                                                          jstring filesdir,
                                                                          jobject assetManager)
{
  char configFileName[PATH_MAX];
  snprintf(configFileName, sizeof(configFileName), "%s/SamplesConfig.xml", env->GetStringUTFChars(filesdir, NULL));
  // init modules
  initPreviewModules(configFileName, AAssetManager_fromJava(env, assetManager));

}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_initDrawer(JNIEnv * env, jobject obj,
                                                                          jint width, jint height,
                                                                          jint hrCamWidth,jint hrCamHeight,
                                                                          jint depthImgWidth, jint depthImgHeight,
                                                                          jobject assetManager)
{
  // init drawer
  initDrawer(width,height,hrCamWidth,hrCamHeight,depthImgWidth,depthImgHeight, AAssetManager_fromJava(env, assetManager));

}

JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_submitColorAndDepth(JNIEnv * env, jobject obj,
             jbyteArray jImgDataColor, jint colorImgWidth, jint colorImgHeight,
             jshortArray jImgDataDepth, jint depthImgWidth, jint depthImgHeight,
             jlong timestamp, jlong syncTimestamp, jboolean isOrbbecSensor)
{
   jshort*   imgDepth = NULL;
   jbyte*    imgColor = NULL;
    jboolean  isCopy = 0;
    int count = 0;

    imgDepth = env->GetShortArrayElements(jImgDataDepth, &isCopy);
    imgColor = env->GetByteArrayElements(jImgDataColor, &isCopy);

    for(size_t j=0;j<depthImgWidth * depthImgHeight;j++){
        //LOGI("imgDepth[%d] = %d",j,imgDepth[j]);
        if (imgDepth[j] < 0)
            imgDepth[j] = 0;
        else if (imgDepth[j] > 0 && isOrbbecSensor ) //only for orbbec
            imgDepth[j] = imgDepth[j]/10;

            if(imgDepth[j]>=400 && imgDepth[j]<1400){
                count++;
            }
    }

    float fcount = (float)count/(float)(depthImgWidth * depthImgHeight)*100;

    //Only Queue if 7% of Valid Data is available in Frame, Otherwise Return False
    if(fcount>=7){
        struct timespec tp;
        //gettimeofday(&tp,NULL);
        clock_gettime(CLOCK_MONOTONIC,&tp);
        int64_t unixTimestamp = syncTimestamp;//if syncTimestamp is non zero then its from canned data, use it as is.

        //If unixtime stamp is zero then its for live data, get the current unixtime
        if(unixTimestamp == 0){
            unixTimestamp = (int64_t)(tp.tv_sec) * 1000000 + int64_t(tp.tv_nsec)/1000 - 33333; // minus 83 ms for on frame delay
        }

        //LOGI("synctime = %lld  |  timestamp = %lld", unixTimestamp, timestamp);

        SCVEqueueRGBD((uint8_t*) imgColor, colorImgWidth, colorImgHeight, (int16_t*) imgDepth, depthImgWidth, depthImgHeight, unixTimestamp, timestamp);
        return true;
   }else{
        return false;
   }

    // release the memory
    env->ReleaseShortArrayElements(jImgDataDepth, imgDepth, JNI_ABORT);
    env->ReleaseByteArrayElements(jImgDataColor, imgColor, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_AbortScanTimeOut(JNIEnv * env, jobject obj){

    requestSensorTimeOut();

}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_step(JNIEnv *env, jobject obj,
                                                                   jboolean stopScan,
                                                                   jboolean update)
{
   RenderDisplayThroughScan(stopScan,update);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_stepPreview(JNIEnv *env, jobject obj)
{
  RenderDisplayThroughPreview();
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_destroyPreview(JNIEnv * env,
                                                              jobject obj)
{
  destroyPreviewModules();
}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_destroyModules(JNIEnv * env, jobject obj)
{
    destroyModules();
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_destroyDrawer(JNIEnv * env, jobject obj)
{
  destroyDrawer();
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_stopScan(JNIEnv * env,
                                                         jobject obj, jfloat initPitch, jfloat initRoll)
{
    stopScanner(initPitch, initRoll);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_saveModel(JNIEnv * env, jobject obj)
{
/*
 * Create session ID number
 */
char sessionName[100];
time_t tm;
time(&tm);
sprintf(sessionName,"%ld",tm);
saveModel("/sdcard/3DR", sessionName);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_tsdf(JNIEnv * env,
                                                    jobject obj)
{
  LOGI("Going to call Generate Mesh from Pose Estimator jni Wrapper\n!!");
  if (GenMesh_thread.joinable())
   {
      GenMesh_thread.join();
   }

   GenMesh_thread = std::thread(GenerateMesh);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_toggleAdjustButton(JNIEnv * env, jobject obj)
{
   ToggleAdjustButton();
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_mouseButtonFunc(JNIEnv * env, jobject obj,
                                                               jint button,jint state,
                                                               jint x, jint y)
{
  MouseButtonFunction(button,state,x,y);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_mousePosFunc(JNIEnv * env, jobject obj, jint button, jint x, jint y)
{
  MousePositionFunction(button,x,y);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_captureHRImage(JNIEnv * env, jobject obj,
                                                           jbyteArray  hrImg,
                                                           jint        hrImgWidth,
                                                          jint        hrImgHeight)
{
   jbyte*    jImgData = NULL;
    jboolean  isCopy = 0;
    DPRINTF("FanLog captureHRImage : E");
    // Get data from JNI
    jImgData = env->GetByteArrayElements(hrImg, &isCopy);

    // release the memory
    env->ReleaseByteArrayElements(hrImg, jImgData, JNI_ABORT);

}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_sendHighResKeyFrame(JNIEnv * env,
                                                                  jobject obj)
{
  // send a request to capture a HR Frame
  getHRKeyFrame();
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_mmrd_sensorfusioncv_MotionSensorManager_getSensorReading(
                                                            JNIEnv * env,
                                                            jobject obj,
                                                            jfloatArray gyro,
                                                            jfloatArray acc,
                                                            jlongArray currTimeStamp,
                                                            jlong      difUnixtime,
                                                            jint size)
{
  jfloat*   jacc       = NULL;
  jfloat*   jgyro      = NULL;
  jlong*    jtimestamp = NULL;
  jboolean  isCopy = 0;
  // acquire the data
  jacc       = env->GetFloatArrayElements( acc, &isCopy );
  jgyro      = env->GetFloatArrayElements( gyro,   &isCopy );
  jtimestamp = env->GetLongArrayElements( currTimeStamp,&isCopy);

  // do the processing here
  //LOGI("get sensor reading called with data %d", size);
  {
    ReadGyroAccelerometerMeasurement(jacc,jgyro,jtimestamp,difUnixtime,size);
  }

  // release the memory
  env->ReleaseFloatArrayElements( gyro,   jgyro, JNI_ABORT);
  env->ReleaseFloatArrayElements( acc, jacc, JNI_ABORT);
  env->ReleaseLongArrayElements(currTimeStamp,jtimestamp,JNI_ABORT);

}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MainActivity_setJava(JNIEnv * env, jobject obj)
{

  setJavaEnv(env,obj);

}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MainActivity_clearJava(JNIEnv * env, jobject obj)
{

  clearJavaEnv(env,obj);

}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setJNIConfiguration(JNIEnv * env,
                                                                jobject obj,
                                                                jbooleanArray vo_config, jstring TARGET, jint cannedDatasetType)
{
  // acquire the data
  jboolean*  jvo_config;
  const char *dev = env->GetStringUTFChars(TARGET, 0);
  jvo_config  = env->GetBooleanArrayElements( vo_config,NULL);

  bool config[8];

  for(int i = 0;i<8;i++)
   config[i] = jvo_config[i];

  strcpy(device, dev);
  ConfigApp(config, dev, cannedDatasetType);

  // release the memory
  env->ReleaseBooleanArrayElements( vo_config,jvo_config, JNI_ABORT);

  env->ReleaseStringUTFChars(TARGET, dev);
}

JNIEXPORT jintArray JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_ProcessAndroidImage(JNIEnv * env, jobject obj,
                                                             jbyteArray  src,
                                                             jint        ImgWidth,
                                                             jint        ImgHeight,
                                                             jint left,
                                                             jint right,
                                                             jint top,
                                                             jint bottom)
{
   jbyte*    jImgData = NULL;
    jboolean  isCopy = 0;
    const jint size = 10;

    // Get data from JNI
    jImgData = env->GetByteArrayElements(src, &isCopy);
    //LOGI("Entered Process Image");

    //Array to return to Java Layer
    jintArray pts;
    pts = env->NewIntArray(size);
    if(pts == NULL){
      return NULL;

    }

    jint cpts[size];

    // do some processing here, should first copy the data and start another thread.
    ProcessAndroidImage((uint8_t*)jImgData,ImgWidth,ImgHeight,(uint32_t*)cpts,left,right,top,bottom);

    // release the memory
    env->ReleaseByteArrayElements(src, jImgData, JNI_ABORT);

    //Return the result
    env->SetIntArrayRegion(pts, 0, size, cpts);
    //LOGI("Exited Buffer Allocation");
    return pts;

}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_captureHRPreviewImage(JNIEnv * env, jobject obj,
                                                           jbyteArray  hrImg,
                                                           jint        hrImgWidth,
                                                          jint        hrImgHeight)
{
   jbyte*    jImgData = NULL;
    jboolean  isCopy = 0;
    // Get data from JNI
    jImgData = env->GetByteArrayElements(hrImg, &isCopy);
    //LOGI("hte high resolution image is copy ? %d\n",isCopy);

    // do some processing here, should first copy the data and start another thread.
    captureHrPreviewImage((uint8_t*)jImgData,hrImgWidth,hrImgHeight);

    // release the memory
    env->ReleaseByteArrayElements(hrImg, jImgData, JNI_ABORT);

}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setDispFlag(JNIEnv * env, jobject obj, jboolean showImage)
{
  switchSplattingDilation(showImage);
}

JNIEXPORT jint JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_getSelectedKeyFrameID(JNIEnv * env, jobject obj) {

  long id = GetKeyFrameID();
  return id;
}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_toggleRenderMode(JNIEnv * env, jobject obj, jboolean is_splatting) {

  ToggleRendering(is_splatting);

}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setViewerTargetAngles(JNIEnv * env, jobject obj, jfloat x, jfloat y) {
    SetViewerTargetAngles(x, y);
}


JNIEXPORT jfloat JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_getCameraData(JNIEnv * env, jobject obj, jfloatArray camera_matrix)
{
    bool status;
    jfloat* jcamera_matrix = NULL;

    jboolean  isCopy = JNI_FALSE;

    jcamera_matrix = env->GetFloatArrayElements( camera_matrix, &isCopy );

    float averageDepth = GetCameraData(jcamera_matrix);

    //LOGI("camera_matrix[0  1   2  3] = %f %f %f %f",jcamera_matrix[0], jcamera_matrix[1], jcamera_matrix[2], jcamera_matrix[3]);
    //LOGI("camera_matrix[4  5   6  7] = %f %f %f %f",jcamera_matrix[4], jcamera_matrix[5], jcamera_matrix[6], jcamera_matrix[7]);
    //LOGI("camera_matrix[8  9  10 11] = %f %f %f %f",jcamera_matrix[8], jcamera_matrix[9], jcamera_matrix[10], jcamera_matrix[11]);
    //LOGI("camera_matrix[12 13 14 15] = %f %f %f %f",jcamera_matrix[12], jcamera_matrix[13], jcamera_matrix[14], jcamera_matrix[15]);

    env->SetFloatArrayRegion( camera_matrix, 0, 16, jcamera_matrix);
    env->ReleaseFloatArrayElements( camera_matrix, jcamera_matrix, JNI_ABORT);

    return averageDepth;
}

JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_getBoundingBox(JNIEnv * env, jobject obj, jfloatArray bb_info)
{
    bool status;
    jfloat* jbb_info = NULL;
    jboolean  isCopy = JNI_FALSE;

    jbb_info = env->GetFloatArrayElements( bb_info,   &isCopy );

    status = GetBoundingBox( jbb_info);

    //LOGI("camera_matrix[0  1   2  3] = %f %f %f %f",jcamera_matrix[0], jcamera_matrix[1], jcamera_matrix[2], jcamera_matrix[3]);
    //LOGI("camera_matrix[4  5   6  7] = %f %f %f %f",jcamera_matrix[4], jcamera_matrix[5], jcamera_matrix[6], jcamera_matrix[7]);
    //LOGI("camera_matrix[8  9  10 11] = %f %f %f %f",jcamera_matrix[8], jcamera_matrix[9], jcamera_matrix[10], jcamera_matrix[11]);
    //LOGI("camera_matrix[12 13 14 15] = %f %f %f %f",jcamera_matrix[12], jcamera_matrix[13], jcamera_matrix[14], jcamera_matrix[15]);

    env->SetFloatArrayRegion( bb_info, 0, 6, jbb_info);

    env->ReleaseFloatArrayElements( bb_info, jbb_info, JNI_ABORT);

    return status;
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setBBSize(JNIEnv * env, jobject obj, jfloat size)
{
    SetBBSize(size);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setBBCenter(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z)
{
    SetBBCenter(x,y,z);
}





