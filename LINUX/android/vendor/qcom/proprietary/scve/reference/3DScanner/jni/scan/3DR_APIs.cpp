/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
//With Pose Graph

#include "3DR_APIs.h"
#include "jniUtils.h"

#include <stdlib.h>
#include <math.h>
#include <thread>
#include<mutex>
#include <memory>

// include modules
#include "SurfaceSplattingDriver.hpp"
#include "jpgIO.h"

#include <sys/stat.h>

#ifdef USE_ANDROID_NDK_PROFILER
#include <prof.h>
#endif

const bool USE_RGB  = 1;


//Functional Flags will be configured from Java/JNI*/
bool USE_LIVE_TRACKING;
bool USE_INERTIAL_SENSOR;
bool USE_HR_CAMERA;
bool USE_AUTO_HR_KF;
bool USE_MOTION_CONSTRAINT;
bool USE_PREIVOUS_FRAME_KF;
bool IS_DUMP_IMGS;
bool USE_TSDF;
int CANNED_DATASET_TYPE;

bool startWrite = false;
//Device Used
char device[100], sessionName[100];

using namespace std;
float DispCamMatrix[16];
float AvgDepthDist;
bool DispCamReady = false;
bool isTrackerLost = false;
bool isSensorTimeOut = false;

//using namespace Eigen;


// set camera intrinsics

std::unique_ptr<SurfaceSplattingDriver> g_ssd;

PointCloud *            g_livePointCloud = NULL;

std::mutex   drawer_mutex;
std::mutex   java_mutex;
std::mutex   previewHolder_mutex;
std:: mutex camHolder_mutex;
std::thread  tsdfManager_thread;
std::thread  saveTexturedMesh_thread;
std:: thread StopBA_thread;
//SCVE Variable
int32_t g_winWidth = 0;
int32_t g_winHeight = 0;
int32_t g_hdCamWidth = 0;
int32_t g_hdCamHeight = 0;

// ==== global variables =====
//uint32_t imgWidth  = 432, imgHeight = 244;//active sensing
//uint32_t imgWidth = 640, imgHeight = 480; //orbbec
//uint32_t imgWidth  = 1280, imgHeight = 720;//canned data
uint32_t imgWidth, imgHeight;

bool NotswitchToSplat = true;
bool g_saveModelCallbackCalled = false;
std::string* externalFilesDir;
void saveTexturedMesh(std::string, std::string);
MeshSavedCallback g_meshSaved = NULL;

//Final Textured Mesh
SCVE::Scan3DTexturedMesh gMesh;
bool isTextureMeshReady = false;

SCVE::Context* pScveContext = NULL;
SCVE::Scan3D * pScan3D      = NULL;

SCVE::Scan3DColorDepth colorDepthImg[N_COLORDEPTH_IMG];

Scan3DGlobalData LData;

JavaVM*     javaVM;
JNIEnv*     jniEnv;
jclass      dataClass;
jobject     javaObjRef;
bool      isJavaThreadAttached;


#ifndef WIN32
#include <android/log.h>

// General JNI Miscellaneous
#define  LOG_TAG_SCVE    "Scan3D"
#define  LOGISCVE(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG_SCVE,__VA_ARGS__)
#define  LOGESCVE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG_SCVE,__VA_ARGS__)
#else
#define LOGISCVE printf
#endif

#define CALIBRATION_FOLDER "/sdcard/3DR/calib"

//APIs Implementations
void ConfigApp(bool config[], const char* dev, int cannedDatasetType) {

   USE_LIVE_TRACKING     = config[0];
   USE_INERTIAL_SENSOR   = config[1];
   USE_HR_CAMERA         = config[2];
   USE_AUTO_HR_KF        = config[3];
   USE_MOTION_CONSTRAINT = config[4];
   USE_PREIVOUS_FRAME_KF = config[5];
   IS_DUMP_IMGS          = config[6];
   USE_TSDF              = config[7];
//  imgWidth = 432;
//  imgHeight = 244;
  //DispCamReady = false;

  //Offline HD Data
  if(!USE_LIVE_TRACKING){
   USE_HR_CAMERA = true;
    USE_AUTO_HR_KF = true;
    imgWidth = 432;
    imgHeight = 244;
    CANNED_DATASET_TYPE = cannedDatasetType;
  }
   strcpy(device, dev);
    isSensorTimeOut = false;
    isTrackerLost = false;
    g_saveModelCallbackCalled = false;
    NotswitchToSplat = true;
    isTextureMeshReady = false;


}

void MeshDone(){

   std::lock_guard<std::mutex> lock(java_mutex);
    //if(jniEnv==NULL) {
      int32_t getEnvState = javaVM->GetEnv((void**)&jniEnv,JNI_VERSION_1_6);
      if(getEnvState == JNI_EDETACHED) {
        javaVM->AttachCurrentThread(&jniEnv, NULL);
        LOGI("MeshDone() :: attached to jvm current thread");
          isJavaThreadAttached = true;
      }
    // }

     jmethodID method = jniEnv->GetMethodID(dataClass, "requestMeshReady", "(Ljava/lang/String;)V");
     if (method == 0)
     {
        LOGE("MeshDone() :: Function requestMeshReady not found.");
        //detach();
        return;
     }

     jstring jObjectNameString = jniEnv->NewStringUTF(sessionName);
     jniEnv->CallVoidMethod(javaObjRef, method, jObjectNameString);

     //detach
     if(isJavaThreadAttached) {
        javaVM->DetachCurrentThread();
      //jniEnv = NULL;
      isJavaThreadAttached = false;
     }

}

void requestDisplayTrackerLost() {

    std::lock_guard<std::mutex> lock(java_mutex);
    if (!isTrackerLost) {

        isTrackerLost = true;

        //if(jniEnv==NULL) {
            int32_t getEnvState = javaVM->GetEnv((void**)&jniEnv,JNI_VERSION_1_6);
            if(getEnvState == JNI_EDETACHED) {
              javaVM->AttachCurrentThread(&jniEnv, NULL);
              //LOGI("requestDisplayTrackerLost() :: attached to jvm current thread");
              isJavaThreadAttached = true;
            }
        //}

        jmethodID method = jniEnv->GetMethodID(dataClass, "DisplayTrackerLost", "()V");
        if (method == 0)
        {
            LOGE("requestDisplayTrackerLost :: Function DisplayTrackerLost not found.");
            return;
        }

        jniEnv->CallVoidMethod(javaObjRef, method);

        //detach
        if(isJavaThreadAttached) {
           javaVM->DetachCurrentThread();
           //jniEnv = NULL;
           isJavaThreadAttached = false;
        }

    }
    else {
        LOGISCVE("requestDisplayTrackerLost: NOT notifiying Java that tracker is lost because already sent");
    }


}


void requestSensorTimeOut() {

    std::lock_guard<std::mutex> lock(java_mutex);
    if (!isSensorTimeOut) {

        isSensorTimeOut = true;

        //if(jniEnv==NULL) {
            int32_t getEnvState = javaVM->GetEnv((void**)&jniEnv,JNI_VERSION_1_6);
            if(getEnvState == JNI_EDETACHED) {
              javaVM->AttachCurrentThread(&jniEnv, NULL);
              //LOGI("requestStopScanAS() :: attached to jvm current thread");
              isJavaThreadAttached = true;
           // }
        }

        jmethodID method = jniEnv->GetMethodID(dataClass, "requestStopScanAS", "()V");
        if (method == 0)
        {
            LOGE("requestStopScanAS() :: Function requestStopScanAS not found.");
            return;
        }

        jniEnv->CallVoidMethod(javaObjRef, method);

        //detach
        if(isJavaThreadAttached) {
           javaVM->DetachCurrentThread();
           //jniEnv = NULL;
           isJavaThreadAttached = false;
        }

    }
    else {
        LOGISCVE("requestStopScanAS(): NOT notifiying Java that tracker is lost because already sent");
    }


}

void scveSingleFrameProcessed() {

    std::lock_guard<std::mutex> lock(java_mutex);
    //if(jniEnv==NULL) {
        int32_t getEnvState = javaVM->GetEnv((void**)&jniEnv,JNI_VERSION_1_6);
        if(getEnvState == JNI_EDETACHED) {
            javaVM->AttachCurrentThread(&jniEnv, NULL);
            //LOGI("scveSingleFrameProcessed() :: attached to jvm current thread");
            isJavaThreadAttached = true;
        }
    //}

    jmethodID method = jniEnv->GetMethodID(dataClass, "scveFrameProcessed", "()V");
    if (method == 0)
    {
        LOGE(" scveSingleFrameProcessed :: Function scveFrameProcessed() not found in Java.");
        return;
    }

    jniEnv->CallVoidMethod(javaObjRef, method);

    //detach
    if(isJavaThreadAttached) {
       javaVM->DetachCurrentThread();
       //jniEnv = NULL;
       isJavaThreadAttached = false;
    }

}





void SetMeshSavedCallback(MeshSavedCallback ptr)
{
   g_meshSaved = ptr;
   g_saveModelCallbackCalled = true;
}
/**********************************************************
 * Call the callback
 **********************************************************/
void CallMeshSavedCallback( )
{
   if (g_meshSaved != NULL)
   {
      (*g_meshSaved)( );
   }
}

void RenderDisplayThroughScan(bool stopScan, bool update) {

   static bool isInitRenderImg = false;
   static bool stoppedScan = false;

  /*
   * ScanInterface determines if we call our own renderer or
   * just get the data and pass to callback.
   */
   std::lock_guard<std::mutex> lock(drawer_mutex);
   if (stopScan == false)
   {
       // SCANNING - render point clouds.
      g_ssd->set(SurfaceSplattingDriver::Stage::Scanning);
      g_ssd->SCVErender(update);
   }
   else
   {
       // STOP SCAN, allow for manual bounding box adjustment.
      g_ssd->set(SurfaceSplattingDriver::Stage::AfterScan);
      g_ssd->SCVErender(true);

   }

}

void RenderDisplayThroughPreview()
{

   if (nullptr != g_ssd)
   {
      std::lock_guard<std::mutex> lock(drawer_mutex);

      if (NotswitchToSplat)
      {
         g_ssd->set(SurfaceSplattingDriver::Stage::CameraPreview);
      }
      else
      {
         g_ssd->set(SurfaceSplattingDriver::Stage::BeforeScan);
      }

      /*
       * ScanInterface determines if we call our own renderer or
       * just get the data and pass to callback.
       */
      g_ssd->SCVErender(true);

   }
}

void GenerateMesh() {


   LOGESCVE("SCVE Entered Generate Mesh\n");


   float cubeCenter[3] = {0.0f, -0.15f, 1.0f};
    float cubeDim[3] = {0.6f, 0.6f, 0.6f};

   if (nullptr != g_ssd)
   {
      cubeCenter[0] = g_ssd->getBoundingBoxCenter()(0);
      cubeCenter[1] = g_ssd->getBoundingBoxCenter()(1);
      cubeCenter[2] = g_ssd->getBoundingBoxCenter()(2);
      cubeDim[0] = g_ssd->getBoundingBoxDimension()(0) * 2.0f;
      cubeDim[1] = g_ssd->getBoundingBoxDimension()(1) * 2.0f;
      cubeDim[2] = g_ssd->getBoundingBoxDimension()(2) * 2.0f;

      LOGI("INTERNAL BOUNDING BOX : %f %f %f    %f %f %f \n",
         cubeCenter[0],
         cubeCenter[1],
         cubeCenter[2],
         cubeDim[0],
         cubeDim[1],
         cubeDim[2]);
       }

        SCVE::Scan3DBoundingBox boundBox;
        boundBox.nPosX = cubeCenter[0];
      boundBox.nPosY =  cubeCenter[1];
      boundBox.nPosZ =  cubeCenter[2];
      boundBox.nScale = cubeDim[0];
      boundBox.nRotY = 0;
      boundBox.nRotZ = 0;

      SCVE::Status sRet = pScan3D ->GetTexturedMesh(&boundBox, &gMesh );
        if (sRet != SCVE::SCVE_SUCCESS)
        {
            LOGESCVE("SCVE : GetTexturedMesh fail ret %x\n", sRet);
            isTextureMeshReady  = false;
        }
        else
        {
            isTextureMeshReady  = true;
            SetMeshSavedCallback((MeshSavedCallback) MeshDone);
            time_t tm;
            time(&tm);
            sprintf(sessionName,"%ld",tm);
            saveModel("/sdcard/3DR", sessionName);

            LOGISCVE("SCVE GetTexturedMesh success\n");
        }
      LOGI("Finished saving RGB Images to the SD card");

}

void setJavaEnv(JNIEnv* (&env), jobject& obj){

  std::lock_guard<std::mutex> lock(java_mutex);
  LOGI("setJavaEnv :: 1");
  env->GetJavaVM(&javaVM);
  jclass cls         = env->GetObjectClass(obj);
  dataClass = (jclass) env->NewGlobalRef(cls);
  javaObjRef   = env->NewGlobalRef(obj);
  jniEnv=NULL;
  isJavaThreadAttached = false;
  LOGI("setJavaEnv :: 2");

}

void clearJavaEnv(JNIEnv* (&env), jobject& obj){

    std::lock_guard<std::mutex> lock(java_mutex);
    //Clear Java Pointers
    env->DeleteGlobalRef(dataClass);
    env->DeleteGlobalRef(javaObjRef);

}

void ToggleAdjustButton() {

   if (nullptr != g_ssd)
   {
      g_ssd->toggleInputMode();
   }
}

void MouseButtonFunction(long button, long state, long x, long y) {

   if (nullptr != g_ssd)
   {
      g_ssd->handleCursorAction(button, state, x, y);
   }

}

void getHRKeyFrame() {

}

void MousePositionFunction(long button, long x, long y) {

   if (nullptr != g_ssd)
   {
       //
       // TODO:  The BUTTON parameter can be passed in to handleCursorMovement() if desired. We now
       // track motion for each of the fingers.
       //
      g_ssd->handleCursorMovement(button, x, y);
   }

}

void ReadGyroAccelerometerMeasurement(float *jacc, float *jgyro, int64_t* jtimestamp, int64_t difUnixtime, long size) {

   // dump data here
   SCVE::Scan3DIMU imu;
   imu.pGyro            = jgyro;
   imu.pAcc             = jacc;
   imu.nNumMeasurements = size;
   imu.pTimeStamp       = jtimestamp;
   imu.nSyncTimeStamp   = difUnixtime;
   if (pScan3D)
   {
      pScan3D ->QueueIMU(&imu);
   }

}

void switchSplattingDilation(bool showImage) {

   NotswitchToSplat = showImage;
   LOGISCVE("Switched Display to Splatting\n");

}

long GetKeyFrameID() {

   if (nullptr == g_ssd)
      return -1;

   return g_ssd->getSelectedKeyFrameID();
}

void ToggleRendering(bool is_splatting) {

   if (nullptr == g_ssd)
      return;

   if (is_splatting)
      g_ssd->set(SurfaceSplattingDriver::RenderMode::Splatting);
   else
      g_ssd->set(SurfaceSplattingDriver::RenderMode::Dilation);

}

void SetViewerTargetAngles(float x, float y)
{
    g_ssd->SetViewerTargetAngles(x, y);
}



void initModules(const char* externalFilesDirString,
   char * configFile,
   AAssetManager *am)
{
   isTextureMeshReady = false;
   externalFilesDir = new std::string(externalFilesDirString);

   bool isDumpInfo = true;

   LOGI(":: initModules 1");
   SCVE::Status ret = pScan3D ->Start();
   LOGI(":: initModules 2");
   LData.previewMode = false;
   LOGI(":: initModules 3");

   startWrite = true;
}

void loadCalibrationFileLR(const char * pathName, const char * fileName, AAssetManager *am,
                         double* K_c, double* K_d, float* R_d2c, float* T_d2c)
{

    char fullFilePath[100];
    sprintf(fullFilePath, "%s/%s", pathName, fileName);

    K_c[0] = 1.0; K_c[1] = 0.0; K_c[2] = 0.0;
    K_c[3] = 0.0; K_c[4] = 1.0; K_c[5] = 0.0;
    K_c[6] = 0.0; K_c[7] = 0.0; K_c[8] = 1.0;

    K_d[0] = 1.0; K_d[1] = 0.0; K_d[2] = 0.0;
    K_d[3] = 0.0; K_d[4] = 1.0; K_d[5] = 0.0;
    K_d[6] = 0.0; K_d[7] = 0.0; K_d[8] = 1.0;

    double K[9] = {1.0,0.0,0.0,
                   0.0,1.0,0.0,
                   0.0,0.0,1.0};

    if (access(fullFilePath, F_OK) != -1) {

        LOGISCVE("Loading %s from sdcard", fileName);
        FILE* fid = fopen(fullFilePath,"r");

        // ps camera
        fscanf(fid,"%lf %lf %lf %lf\n",&K[0],&K[4],&K[2],&K[5]);
        K_c[0] = K[0];
        K_c[4] = K[4];
        K_c[2] = K[2];
        K_c[5] = K[5];

        // depth camera
        fscanf(fid,"%lf %lf %lf %lf\n",&K[0],&K[4],&K[2],&K[5]);
        K_d[0] = K[0];
        K_d[4] = K[4];
        K_d[2] = K[2];
        K_d[5] = K[5];

        fscanf(fid,"%f %f %f ", &R_d2c[0],&R_d2c[1],&R_d2c[2]);
        fscanf(fid,"%f %f %f ", &R_d2c[3],&R_d2c[4],&R_d2c[5]);
        fscanf(fid,"%f %f %f\n",&R_d2c[6],&R_d2c[7],&R_d2c[8]);
        fscanf(fid,"%f %f %f\n", &T_d2c[0],&T_d2c[1],&T_d2c[2]);

        fclose(fid);
    }
    else {
        LOGISCVE("%s not found on sdcard so loading from app assets instead", fileName);
        AAsset* asset = AAssetManager_open(am, fileName, AASSET_MODE_UNKNOWN);
        if (NULL == asset) {
                LOGISCVE("error asset open was null");
        }
        long size = AAsset_getLength(asset);
        char* buffer = (char*) malloc (sizeof(char)*size);
        AAsset_read (asset,buffer,size);
        AAsset_close(asset);

        // ps camera
        sscanf(buffer,"%lf %lf %lf %lf\n",&K[0],&K[4],&K[2],&K[5]);
        K_c[0] = K[0];
        K_c[4] = K[4];
        K_c[2] = K[2];
        K_c[5] = K[5];

        // depth camera
        sscanf(buffer,"%lf %lf %lf %lf\n",&K[0],&K[4],&K[2],&K[5]);
        K_d[0] = K[0];
        K_d[4] = K[4];
        K_d[2] = K[2];
        K_d[5] = K[5];

        sscanf(buffer,"%f %f %f ", &R_d2c[0],&R_d2c[1],&R_d2c[2]);
        sscanf(buffer,"%f %f %f ", &R_d2c[3],&R_d2c[4],&R_d2c[5]);
        sscanf(buffer,"%f %f %f\n",&R_d2c[6],&R_d2c[7],&R_d2c[8]);
        sscanf(buffer,"%f %f %f\n", &T_d2c[0],&T_d2c[1],&T_d2c[2]);

        free(buffer);
    }

    LOGI("DPS cam %f %f %f %f\n",K_d[0],K_d[4],K_d[2],K_d[5]);
    LOGI("CPS cam %f %f %f %f\n",K_c[0],K_c[4],K_c[2],K_c[5]);
    LOGI("TPS cam T %f %f %f\n",T_d2c[0], T_d2c[1],T_d2c[2]);


}


void loadCalibrationFileHR(const char * pathName, const char * fileName, AAssetManager *am,
                         float* K, float* R_h2c, float* T_h2c)
{

    char fullFilePath[100];
    sprintf(fullFilePath, "%s/%s", pathName, fileName);

    K[0] = 1.0; K[1] = 0.0; K[2] = 0.0;
    K[3] = 0.0; K[4] = 1.0; K[5] = 0.0;
    K[6] = 0.0; K[7] = 0.0; K[8] = 1.0;

    if (access(fullFilePath, F_OK) != -1) {

        LOGISCVE("Loading %s from sdcard", fileName);
        FILE* fid = fopen(fullFilePath,"r");

        // ps camera
        fscanf(fid,"%f %f %f %f\n",&K[0],&K[4],&K[2],&K[5]);
        // R_h2c
        fscanf(fid,"%f %f %f ", &R_h2c[0],&R_h2c[1],&R_h2c[2]);
        fscanf(fid,"%f %f %f ", &R_h2c[3],&R_h2c[4],&R_h2c[5]);
        fscanf(fid,"%f %f %f\n",&R_h2c[6],&R_h2c[7],&R_h2c[8]);
        // T_h2c
        fscanf(fid,"%f %f %f\n", &T_h2c[0], &T_h2c[1], &T_h2c[2]);

        fclose(fid);
    }
    else {
        LOGISCVE("%s not found so loading from app assets instead", fileName);
        AAsset* asset = AAssetManager_open(am, fileName, AASSET_MODE_UNKNOWN);
        if (NULL == asset) {
                LOGISCVE("error asset open was null");
        }
        long size = AAsset_getLength(asset);
        char* buffer = (char*) malloc (sizeof(char)*size);
        AAsset_read (asset,buffer,size);
        AAsset_close(asset);

        // ps camera
        sscanf(buffer,"%f %f %f %f\n",&K[0],&K[4],&K[2],&K[5]);
        // R_h2c
        sscanf(buffer,"%f %f %f ", &R_h2c[0],&R_h2c[1],&R_h2c[2]);
        sscanf(buffer,"%f %f %f ", &R_h2c[3],&R_h2c[4],&R_h2c[5]);
        sscanf(buffer,"%f %f %f\n",&R_h2c[6],&R_h2c[7],&R_h2c[8]);
        // T_h2c
        sscanf(buffer,"%f %f %f\n", &T_h2c[0], &T_h2c[1], &T_h2c[2]);

        free(buffer);
    }

    LOGI("hr cam %f %f %f %f\n",K[0],K[4],K[2],K[5]);
    LOGI("hr cam T %f %f %f\n",T_h2c[0],T_h2c[1],T_h2c[2]);

}

void initPreviewModules(char * configFile, AAssetManager *am)
{

    int state     = fcvSetOperationMode(FASTCV_OP_CPU_PERFORMANCE);

   LData.previewMode = true;


}

static void SCVEcolorDepthProcessedCb(SCVE::Status                  status,
                                  const SCVE::Scan3DColorDepth *pColorDepthImg,
                                  void                         *pSessionUserData,
                                  void                         *pTaskUserData)
{
   Scan3DGlobalData *pLData = (Scan3DGlobalData *)pSessionUserData;
   uint32_t *pIndx = (uint32_t*)pTaskUserData;
   float cam[16] = { 0.0f };

   switch (status)
   {
   case (SCVE::Status)SCVE::SCVE_SCAN3D_STATUS_BUFFER_FLUSHED:
      // Should be the same as SUCCESS
      // TODO: need to revisit if this status is required

   case (SCVE::Status)SCVE::SCVE_SUCCESS:

        //LOGI("SCVEcolorDepthProcessedCb() \n");
       scveSingleFrameProcessed();
       pthread_mutex_lock(&pLData->hImgCBMutex);
      pLData->colorDepthIndexQ.push(*pIndx);
      pthread_cond_signal(&pLData->hImgCBSignal);
      pthread_mutex_unlock(&pLData->hImgCBMutex);
      if (pLData->pScan3D->GetCurrentCameraData(cam, &AvgDepthDist) == SCVE::SCVE_SUCCESS)
      {
         if (g_ssd)
         {
            g_ssd ->setCamMatrix(cam);
            std::lock_guard<std::mutex> lock(camHolder_mutex);
            memcpy(DispCamMatrix,cam,16*sizeof(float));
            DispCamReady = true;
         }
      }
      break;
   default:
      break;
   }
}

static void SCVEpreviewDataCb(SCVE::Status             status,
                          SCVE::Scan3DPreviewData *pPreviewData,
                          void                    *pSessionUserData)
{

    if (pPreviewData == NULL)
        LOGISCVE("SCVEpreviewDataCb :: pPreviewData is null \n");

   Scan3DGlobalData * pLData = (Scan3DGlobalData *)pSessionUserData;
   switch (status)
   {
   case (SCVE::Status)SCVE::SCVE_SUCCESS:

       if (pLData->previewMode)
       {
          //LOGISCVE("SCVEpreviewDataCb :: previewMode\n");
         SCVE::Status sRet = pLData->pScan3D->GetScan3DBoundingBox(pPreviewData, &(pLData->box));
         g_ssd ->setPreviewData(pPreviewData);
      }
      else
      {
          //LOGISCVE("SCVEpreviewDataCb :: setScanData(pPreviewData)\n");
          g_ssd ->setScanData(pPreviewData);
      }
      break;

   case (SCVE::Status)SCVE::SCVE_SCAN3D_STATUS_LAST_PREVIEW_DATA:
       LOGISCVE("SCVEpreviewDataCb :: SCVE_SCAN3D_STATUS_LAST_PREVIEW_DATA \n");
      // Signal that we received the last/final preview data callback
      pthread_mutex_lock(&pLData->hScanMutex);
        pLData->bLastPreviewData = true;
        pthread_cond_signal(&pLData->hScanSignal);
        pthread_mutex_unlock(&pLData->hScanMutex);
      break;
   case (SCVE::Status)SCVE::SCVE_SCAN3D_ERROR_UNRECOVERABLE:
       LOGISCVE("SCVEpreviewDataCb :: SCVE_SCAN3D_ERROR_UNRECOVERABLE \n");
       requestDisplayTrackerLost();
       break;
   case (SCVE::Status)SCVE::SCVE_SCAN3D_ERROR_LOST_TRACK:
      LOGISCVE("SCVEpreviewDataCb :: SCVE_SCAN3D_ERROR_LOST_TRACK \n");
      requestDisplayTrackerLost();
        break;
   default:
      LOGESCVE("previewDataCb - Error status was returned, ret %d", status);
      break;
   }
}

SCVE::Status SCVEAllocImage(int32_t colorImgWidth,
                            int32_t colorImgHeight,
                            int32_t depthImgWidth,
                            int32_t depthImgHeight)
{
   SCVE::Status result = SCVE::SCVE_SUCCESS;

   uint32_t colorImageSize = 0;
   SCVE::Image colorImage;

   colorImage.sDim.nWidth       = colorImgWidth;
   colorImage.sDim.nHeight      = colorImgHeight;

   //colorImage.sDim.nStride      = SCVE_SCAN3D_DEFAULT_HD_WIDTH;
   //colorImage.sDim.nStrides[0]  = SCVE_SCAN3D_DEFAULT_HD_WIDTH;
   //colorImage.sDim.nStrides[1]  = SCVE_SCAN3D_DEFAULT_HD_WIDTH;
   //colorImage.sDim.nStrides[2]  = SCVE_SCAN3D_DEFAULT_HD_WIDTH;
   colorImage.sDim.nStride      = 3*colorImgWidth;
   colorImage.sDim.nStrides[0]  = 3*colorImgWidth;

   //colorImage.sFormat   = SCVE::SCVE_COLORFORMAT_YUV_NV21;
   colorImage.sFormat   = SCVE::SCVE_COLORFORMAT_RGB_888;

   //colorImage.nChannels = 2;
   colorImage.nChannels = 1;

   //colorImageSize       = (colorImage.sDim.nStride * colorImage.sDim.nHeight) +
   // (colorImage.sDim.nStrides[1] * colorImage.sDim.nHeight / 2);
    colorImageSize       = (colorImage.sDim.nStride * colorImage.sDim.nHeight);


   //DEPTH images
   SCVE::DepthImage depthImage;
   uint32_t depthImageSize = 0;

   depthImage.sDim.nWidth  = depthImgWidth;
   depthImage.sDim.nHeight = depthImgHeight;
   depthImage.sDim.nStride = 2*depthImgWidth;

   depthImageSize = depthImage.sDim.nStride * depthImage.sDim.nHeight;

   for (uint32_t j = 0; j < N_COLORDEPTH_IMG; j++)
   {
      memcpy(&colorDepthImg[j].sColorImage, &colorImage, sizeof(SCVE::Image));
      colorDepthImg[j].sColorImage.pPtr = (uint8_t *)fcvMemAlloc(colorImageSize,16); //mode heap
      memcpy(&colorDepthImg[j].sDepthImage, &depthImage, sizeof(SCVE::DepthImage));
      colorDepthImg[j].sDepthImage.pPtr = (uint16_t *)fcvMemAlloc(depthImageSize,16); //mode heap

      if (colorDepthImg[j].sColorImage.pPtr == NULL || colorDepthImg[j].sDepthImage.pPtr == NULL)
      {
          if (colorDepthImg[j].sColorImage.pPtr)
            {
                fcvMemFree(colorDepthImg[j].sColorImage.pPtr);
                colorDepthImg[j].sColorImage.pPtr = NULL;
            }

            if (colorDepthImg[j].sDepthImage.pPtr)
            {
                fcvMemFree(colorDepthImg[j]. sDepthImage.pPtr);
                colorDepthImg[j]. sDepthImage.pPtr = NULL;
            }

         LOGE("unitTest_scveScan3DEndToEnd - fail to allocate colordept buffer %d", j);
         return SCVE::SCVE_ERROR_OUT_OF_MEMORY;
      }
   }

   pthread_mutex_lock(&LData.hImgCBMutex);
   for (uint32_t i = 0 ; i < N_COLORDEPTH_IMG; ++i)
   {
      LData.pbufferindx[i] = i;
      LData.colorDepthIndexQ.push(i);
   }
   pthread_mutex_unlock(&LData.hImgCBMutex);
   return result;
}

SCVE::Scan3DCalibration SCVECalibinit(AAssetManager *am)
{
   SCVE::Scan3DCalibration DCalibration;

   float R_d2c[9];
   float T_d2c[3];
   float hrCamK[9],R_h2c[9];
   double K2[9], depthK2[9];
   float T_h2c[3];

   if (USE_LIVE_TRACKING){
       //for orbbec sensor, live tracking or for canned data which was collected using orbbec sensor
       loadCalibrationFileLR(CALIBRATION_FOLDER,"QD_cam_calibration_LR.txt", am, K2, depthK2, R_d2c, T_d2c);
       loadCalibrationFileHR(CALIBRATION_FOLDER,"QD_cam_calibration_HR.txt", am, hrCamK, R_h2c, T_h2c);
   }else{
       //for canned data collected using device 223 i.e for rabbit and set3(atalie) dataset
       if((CANNED_DATASET_TYPE == CANNED_RABBIT_DATASET)|| (CANNED_DATASET_TYPE == CANNED_SET3_DATASET) ){
          loadCalibrationFileLR(CALIBRATION_FOLDER,"QD_cam_calibration_LR_223.txt", am, K2, depthK2, R_d2c, T_d2c);
          loadCalibrationFileHR(CALIBRATION_FOLDER,"QD_cam_calibration_HR_223.txt", am, hrCamK, R_h2c, T_h2c); // VGA
      }else if(CANNED_DATASET_TYPE == CANNED_SHANDON_DATASET){
          loadCalibrationFileLR(CALIBRATION_FOLDER,"QD_cam_calibration_LR_32.txt", am, K2, depthK2, R_d2c, T_d2c);
             loadCalibrationFileHR(CALIBRATION_FOLDER,"QD_cam_calibration_HR_32.txt", am, hrCamK, R_h2c, T_h2c); // VGA
      }else if(CANNED_DATASET_TYPE == CANNED_ORBBEC_DATASET){
         loadCalibrationFileLR(CALIBRATION_FOLDER,"QD_cam_calibration_LR_ORBBEC_SS.txt", am, K2, depthK2, R_d2c, T_d2c);
            loadCalibrationFileHR(CALIBRATION_FOLDER,"QD_cam_calibration_HR_ORBBEC_SS.txt", am, hrCamK, R_h2c, T_h2c); // VGA
      }
    }


   for (uint32_t i = 0; i < 3; ++i)
   {
      for (uint32_t j = 0; j < 3; ++j)
      {
         uint32_t idx = j + i * 3;
         //Copy Camera Parameters into Struct
         DCalibration.cameraCalibration.K_hr[idx]    = hrCamK[idx];
         DCalibration.cameraCalibration.K_depth[idx] = depthK2[idx];
         DCalibration.cameraCalibration.R_d2c[idx]   = R_d2c[idx];
      }
   }

   DCalibration.cameraCalibration.T_d2c[0]  = T_d2c[0];
   DCalibration.cameraCalibration.T_d2c[1]  = T_d2c[1];
   DCalibration.cameraCalibration.T_d2c[2]  = T_d2c[2];

   if (USE_LIVE_TRACKING)
   {
      //Read in Motion Sensor Calibration
      std::ifstream in;
      in.open("/sdcard/3DR/calib/motionSensorParam.txt");
      if (in.good())
      {
         std::string line;
         //read first line
         for (uint32_t i = 0; i < 3; i++)
         {
            for (uint32_t j = 0; j < 3; j++)
            {
               uint32_t idx = j + i * 3;
               in >> DCalibration.motionSensorCalibration.R_gyro[idx];
            }
         }

         in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         //read second line
         for (uint32_t i = 0; i < 3; i++)
         {
            for (uint32_t j = 0; j < 3; j++)
            {
               uint32_t idx = j + i * 3;
               in >> DCalibration.motionSensorCalibration.R_acc[idx];
            }
         }
         in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
         in.close();
      }

      in.open("/sdcard/3DR/calib/motionSensorParam.txt");
      if (in.good())
      {
         std::string line;
         std::string subStr;
         std::vector<std::string> strVec;
         //read first line
         std::getline(in, line);
         //read second line
         std::getline(in, line);
         //get the parameters
         in >> DCalibration.motionSensorCalibration.nNominalState[0];
         in >> DCalibration.motionSensorCalibration.nNominalState[1];
         in >> DCalibration.motionSensorCalibration.nNominalState[2];

         in.close();
      }
   }

   return DCalibration;
}

SCVE::Status SCVEALDeinit()
{
   SCVE::Status result = SCVE::SCVE_SUCCESS;

   if (pScan3D)
   {
      if (SCVE::SCVE_SUCCESS != SCVE::Scan3D::deleteInstance(pScan3D))
      {
         LOGESCVE("SCVE::Scan3D::deleteInstance failed");
      }
      else
        {
         LOGISCVE("SCVE::Scan3D::deleteInstance success");
        }
      pScan3D = NULL;
   }

   if (pScveContext)
   {
      if (SCVE::SCVE_SUCCESS != SCVE::Context::deleteInstance(pScveContext))
      {
         LOGESCVE("scveScan3DEndToEnd - %s", "SCVE::Context::deleteInstance() failed");
      }
      pScveContext = NULL;
   }

   for (uint32_t j = 0; j < N_COLORDEPTH_IMG; j++)
   {
      if (colorDepthImg[j].sColorImage.pPtr)
      {
         fcvMemFree(colorDepthImg[j].sColorImage.pPtr);
         colorDepthImg[j].sColorImage.pPtr = NULL;
      }

      if (colorDepthImg[j].sDepthImage.pPtr)
      {
         fcvMemFree(colorDepthImg[j].sDepthImage.pPtr);
         colorDepthImg[j].sDepthImage.pPtr = NULL;
      }
   }

   return result;
}

SCVE::Status SCVEALinit(
            int32_t hdcamwidth,
            int32_t hdcamheight,
            int32_t depthImgwidth,
            int32_t depthImgheight,
            AAssetManager *am)
{
   SCVE::Status result = SCVE::SCVE_SUCCESS;

   pScveContext = SCVE::Context::newInstance((SCVE::OperationMode)SCVE::SCVE_MODE_DEFAULT);     //for Venum

   //init scan3d config
   SCVE::Scan3DConfig config;
   char systemCmd[300];
   int eFileCmd = system(systemCmd);
   if (eFileCmd)
   {
      LOGESCVE("Error: %s. Error notes: %s", systemCmd, strerror(errno));
   }
   snprintf(config.sDirPath, 255, "/sdcard/3DR");
#ifndef WIN32
   eFileCmd = mkdir(config.sDirPath, S_IRWXU | S_IRWXG | S_IRWXO);
#else
   eFileCmd = _mkdir(config.sDirPath);
#endif

   config.bInertialSensor = USE_INERTIAL_SENSOR;//false; //NO IMU?

   config.sDepthImgDim.nWidth  = config.sDepthImgDim.nStride = depthImgwidth;
   config.sDepthImgDim.nHeight = depthImgheight;
   config.sHighDefDim.nWidth   = config.sHighDefDim.nStride = config.sHighDefDim.nStrides[0]
                               = config.sHighDefDim.nStrides[1] = hdcamwidth;
   config.sHighDefDim.nHeight  = hdcamheight;


   for(int j=0; j<N_COLORDEPTH_IMG; j++){
       colorDepthImg[j].sColorImage.sDim.nWidth  = hdcamwidth;
       colorDepthImg[j].sColorImage.sDim.nHeight = hdcamheight;
       colorDepthImg[j].sColorImage.sDim.nStride = 3*hdcamwidth;

       colorDepthImg[j].sDepthImage.sDim.nWidth  = depthImgwidth;
       colorDepthImg[j].sDepthImage.sDim.nHeight = depthImgheight;
       colorDepthImg[j].sDepthImage.sDim.nStride = 2*depthImgwidth;
   }

   /****************************/
   SCVEAllocImage(hdcamwidth, hdcamheight, depthImgwidth, depthImgheight);
   /******************************/
   SCVE::Scan3DCalibration DCalibration = SCVECalibinit(am);

   SCVE::ImageDimension DepthImgDim;

   pScan3D = SCVE::Scan3D::newInstance(pScveContext,
                              &config,
                              &DCalibration,
                              SCVEcolorDepthProcessedCb,
                              SCVEpreviewDataCb,
                              &LData,
                              &DepthImgDim);

   if (pScan3D == NULL)
   {
      return SCVE::SCVE_SCAN3D_ERROR;
   }

   LData.previewDataCBCounter = 0;
   LData.pScan3D = pScan3D;
   LData.pcolorDepthImg = colorDepthImg;

   imgWidth = DepthImgDim.nWidth;
   imgHeight = DepthImgDim.nHeight;
   LOGISCVE("SCVEALinit: Depth dimension returned from SCVE lib is W = %d, H = %d",imgWidth, imgHeight);
   return result;
}

void initDrawer(int32_t winWidth,
   int32_t winHeight,
   int32_t hdCamWidth,
   int32_t hdCamHeight,
   int32_t depthImgWidth,
    int32_t depthImgHeight,
   AAssetManager *am)
{

   if (nullptr == g_ssd)
      g_ssd.reset(new SurfaceSplattingDriver());

   g_ssd->initialize(hdCamWidth, hdCamHeight, winWidth, winHeight);

   g_winWidth = winWidth;
   g_winHeight = winHeight;
   g_hdCamWidth = hdCamWidth;
   g_hdCamHeight = hdCamHeight;

   SCVEALinit(hdCamWidth, hdCamHeight, depthImgWidth, depthImgHeight, am);
}

void destroyModules()
{

    LOGI("destroyModules() 1\n");

   if (saveTexturedMesh_thread.joinable())
   {
      saveTexturedMesh_thread.join();
   }

    LOGI("destroyModules() 2\n");
   if( externalFilesDir != 0 )
   {
      delete externalFilesDir;
   }

    LOGI("destroyModules() 3\n");

}

void destroyPreviewModules()
{

   //

}

void destroyDrawer()
{

    {
        std::lock_guard<std::mutex> lock(drawer_mutex);
       if (nullptr != g_ssd) {
          g_ssd = nullptr;
       }
   }

   SCVEALDeinit();

}

void stopScanner(float initSensorPitchDeg, float initSensorRollDeg)
{
    startWrite = false;
    //bool lostTrack = false;
    if(g_ssd)
        g_ssd->SetInitialSensorReadings(initSensorPitchDeg, initSensorRollDeg);

    LOGI("APP Stop Tracking : stopScanner called");
      SCVE::Status sRet = pScan3D->Stop();
      if (sRet != SCVE::SCVE_SUCCESS)
         LOGE("stopScanner - Stop fail i, ret %x", sRet);

      pthread_mutex_lock(&LData.hScanMutex);

      //if (!lostTrack){
          LOGI("stopScanner() is waiting for LAST_PREVIEW_DATA");
          if (!LData.bLastPreviewData)
          {
             // Wait for the SCVE_SCAN3D_STATUS_LAST_PREVIEW_DATA
             pthread_cond_wait(&LData.hScanSignal, &LData.hScanMutex);
          }
      /*}
      else {
          LOGI("app lost tracking so will NOT wait for LAST_PREVIEW_DATA");
      }
      */
      pthread_mutex_unlock(&LData.hScanMutex);

}


void saveModel(const char* folderAbsolutePath, const char* fileName)
{
   LOGI("save model is called\n");
   if (saveTexturedMesh_thread.joinable())
   {
      saveTexturedMesh_thread.join();
   }

   std::string* stringfolderAbsolutePath = externalFilesDir;
   bool deleteStr = false;

   if( folderAbsolutePath == 0 )
   {
      if( stringfolderAbsolutePath == 0 )
      {
         stringfolderAbsolutePath = new std::string("/sdcard/3DR");
         deleteStr = true;
      }
   }
   else
   {
      stringfolderAbsolutePath = new std::string(folderAbsolutePath);
      deleteStr = true;
   }

   std::string stringFileName(fileName);

   //saveTexturedMesh_thread = std::thread(saveTexturedMesh, *stringfolderAbsolutePath, stringFileName);
    saveTexturedMesh(*stringfolderAbsolutePath, stringFileName);
   if( deleteStr )
      delete stringfolderAbsolutePath;
}



/*
 * saveTexturedMesh -
 */

void saveTexturedMesh(std::string folderAbsolutePath, std::string fileName)
{
   bool g_savingModel = true;
   bool g_writtenToFile = false;

   LOGI("save Textured Mesh is called with %s - %s\n", folderAbsolutePath.c_str(), fileName.c_str());

   //char sessionName[100];
   //time_t tm;
   //time(&tm);
   //sprintf(sessionName, "%ld", tm);


   char dirName[256];

   //SCVE
   sprintf(dirName, "%s/%s", folderAbsolutePath.c_str(), fileName.c_str());
   mkdir(dirName, S_IRWXU | S_IRWXG);
   pScan3D ->SaveTexturedMeshToObj(&gMesh,dirName,"output");

   g_writtenToFile = true;
   g_savingModel = false;

   if( (g_writtenToFile == true) && (g_saveModelCallbackCalled == true))
   {
      CallMeshSavedCallback();
      g_saveModelCallbackCalled = false;
   }
}

//Grab Image from Java Layer
void ProcessAndroidImage(const uint8_t* src,
   const uint32_t imgWidth,
   const uint32_t imgHeight,
   uint32_t pts[],
   int left,
   int right,
   int top,
   int bottom)
{
}

void captureHrPreviewImage(const uint8_t* src,
   const uint32_t imgWidth,
   const uint32_t imgHeight)
{

}


void SCVEqueueRGBD(uint8_t *ColorImg, uint32_t colorImgWidth, uint32_t colorImgHeight, int16_t *DepthImg, uint32_t depthImgWidth, uint32_t depthImgHeight, int64_t synctime, int64_t timestamp)
{

   if (pScan3D != NULL && ColorImg != NULL && DepthImg != NULL)
   {
      pthread_mutex_lock(&LData.hImgCBMutex);
        while (LData.colorDepthIndexQ.empty())
        {
            pthread_cond_wait(&LData.hImgCBSignal, &LData.hImgCBMutex);
        }
      uint32_t colorDepthIndex = LData.colorDepthIndexQ.front();
      LData.imgBufIndex = colorDepthIndex;
      LData.colorDepthIndexQ.pop();
      pthread_mutex_unlock(&LData.hImgCBMutex);

      memcpy(LData.pcolorDepthImg[LData.imgBufIndex].sColorImage.pPtr, ColorImg, colorImgWidth * colorImgHeight * 3);
      memcpy(LData.pcolorDepthImg[LData.imgBufIndex].sDepthImage.pPtr, DepthImg, depthImgWidth * depthImgHeight * sizeof(int16_t)); //depth size = depth W x dep H * 2 bytes

      LData.pcolorDepthImg[LData.imgBufIndex].nSyncTimeStamp = synctime;
      LData.pcolorDepthImg[LData.imgBufIndex].nTimeStamp     = timestamp;

      SCVE::Status sRet = LData.pScan3D->QueueColorDepth_Async(&LData.pcolorDepthImg[LData.imgBufIndex],
                                                 &LData.pbufferindx[LData.imgBufIndex]);
      LData.colorDepthCounter++;
   }
}


#include <graphics/Camera.hpp>

float GetCameraData(float * pCameraMatrix)
{
   std::lock_guard<std::mutex> lock(camHolder_mutex);

    if (pCameraMatrix == NULL || DispCamReady == false)
       {
           return false;
       }

    /*
     * Get the camera matrix
     */
    double r[9];
    double t[3];

    for(uint32_t i = 0; i < 3; ++i) {
      for(uint32_t j = 0; j < 3; ++j) {
         r[i*3 + j] = double(DispCamMatrix[j * 4 + i]);
      }
     }
     t[0] = DispCamMatrix[12];
     t[1] = DispCamMatrix[13];
     t[2] = DispCamMatrix[14];

    auto &R = r;
    auto &T = t;

   double quat[4];

    R2q(R, quat);

    core::Quaternion q0(core::Vector(1, 0, 0), 180 * M_PI / 180.0f);
    core::Quaternion q1({static_cast<float>(quat[0]),
      static_cast<float>(quat[1]),
      static_cast<float>(quat[2]),
      static_cast<float>(quat[3])
    });


    graphics::Camera tempCamera;
    tempCamera.setOrientation(normalize(q1 * q0));
    tempCamera.setPosition( {(float)(T[0]), (float)(T[1]),(float)(T[2])} );
    const float * pMatrix = tempCamera.getView().getData();

    memcpy(pCameraMatrix, pMatrix, 16 * sizeof(float));

   return AvgDepthDist;
}

bool GetBoundingBox(float *bb){

  std::lock_guard<std::mutex> lock(camHolder_mutex);
  if(g_ssd != nullptr || DispCamReady == false) {

   bb[0] = g_ssd->getBoundingBoxCenter()(0);
   bb[1] = g_ssd->getBoundingBoxCenter()(1);
   bb[2] = g_ssd->getBoundingBoxCenter()(2);
   bb[3] = g_ssd->getBoundingBoxDimension()(0) * 2.0f;
   bb[4] = g_ssd->getBoundingBoxDimension()(1) * 2.0f;
   bb[5] = g_ssd->getBoundingBoxDimension()(2) * 2.0f;
   return true;

  }else
   return false;

}

void SetBBSize(float size)
{
    if (g_ssd != nullptr)
        g_ssd->SetBBSize(size);
}

void SetBBCenter(float x, float y, float z)
{
    if (g_ssd != nullptr)
        g_ssd->SetBBCenter(x, y, z);
}
