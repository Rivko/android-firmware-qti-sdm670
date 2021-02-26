/**********************************************************
 * 3DR_APIs.h
 *
 * Author: Michel Sarkis
 *
 * Description: API Calls Definitions of 3DR App.
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 **********************************************************/
#include <sys/types.h>
#include <android/asset_manager_jni.h>
#include "androidUtility.hpp"
#include <stdio.h>


#include <stdlib.h>
#include <math.h>
#include <thread>
#include <memory>
#include <queue>

#include <iostream>
#include <fstream>
 #include <time.h>
 #include <pthread.h>
 #include <unistd.h>

#include "fastcv.h"
#include "scveScan3D.hpp"
#include "scveScan3DTypes.hpp"
#include "scveTypes.hpp"

#include <pthread.h>



#define N_COLORDEPTH_IMG      4

//this constants should match with ordering of CannedDataSetType enum in CannedDataIO.java
#define CANNED_ORBBEC_DATASET 0
#define CANNED_SHANDON_DATASET 1
#define CANNED_RABBIT_DATASET 2
#define CANNED_SET3_DATASET 3

// for test information only
typedef struct _Scan3DGlobalData
{
   // Used by more than one thread, main test and callback.
   pthread_mutex_t          hImgCBMutex;
    pthread_cond_t           hImgCBSignal;
    pthread_mutex_t          hScanMutex;
    pthread_cond_t           hScanSignal;
   bool                     bLastPreviewData;
   bool                     bUnrecoverableError;
   std::queue<uint32_t>     colorDepthIndexQ;
   uint32_t                 previewDataCBCounter;
   int                      flgAll;
   SCVE::Scan3DBoundingBox  box;

   // Used only by one thread. These variables are thread safe.
   uint32_t imuCounter;
   uint32_t colorDepthCounter;
   int32_t *pFrameTS;

   SCVE::Scan3D       *pScan3D;
   uint32_t           colorImageSize;
   uint32_t           depthImageSize;

   SCVE::Scan3DTexturedMesh mtexturedMesh;

   //IMU info
   uint32_t           nIMU;
   FILE               *fp_imu;
   uint32_t           imgBufIndex;

   //Test info
   char               filename[255];
   uint32_t           modeNum;
   bool               previewMode;     // for test purposes only
   bool               disableDrawer;
   uint32_t           nPreview;
   SCVE::Scan3DColorDepth *pcolorDepthImg;
   uint32_t pbufferindx[N_COLORDEPTH_IMG];

   INLINE _Scan3DGlobalData() : hImgCBMutex(PTHREAD_MUTEX_INITIALIZER),
      hImgCBSignal(PTHREAD_COND_INITIALIZER), hScanMutex(PTHREAD_MUTEX_INITIALIZER),
      hScanSignal(PTHREAD_COND_INITIALIZER),
      bLastPreviewData(false), bUnrecoverableError(false), previewDataCBCounter(0),
      flgAll(0), imuCounter(0), colorDepthCounter(0), pFrameTS(NULL),
      pScan3D(NULL),
      colorImageSize(0), depthImageSize(0), nIMU(0), fp_imu(NULL), imgBufIndex(0),
      modeNum(0), previewMode(true), disableDrawer(true), nPreview(0) { };
} Scan3DGlobalData;


//API Definitions
void RenderDisplayThroughScan(bool,bool);
void RenderDisplayThroughPreview();

void GenerateMesh();
void ToggleAdjustButton();
void MouseButtonFunction(long,long,long,long);
void MousePositionFunction(long,long,long);
void ReadGyroAccelerometerMeasurement(float*,float*,int64_t*,int64_t,long);
void ProcessAndroidImage(const uint8_t*,const uint32_t,  const uint32_t,uint32_t [],   int, int,int,int);
void captureHrPreviewImage(const uint8_t*,const uint32_t,const uint32_t);
void switchSplattingDilation(bool);
long GetKeyFrameID();
void ToggleRendering(bool);
void SetViewerTargetAngles(float x, float y);
void stopScanner(float initSensorPitchDeg, float initSensorRollDeg);
void saveModel(const char*, const char*);
void destroyDrawer();
void destroyPreviewModules();
void destroyModules();
void initModules(const char* externalFilesDir, char * configFile,AAssetManager *am = NULL);
void initPreviewModules(char * configFile,AAssetManager *am = NULL);
void initDrawer(int32_t winWidth,int32_t winHeight,int32_t hdCamWidth,int32_t depthImgWidth,int32_t depthImgHeight,int32_t hdCamHeight,AAssetManager *am = NULL);
bool captureHrImage(const uint8_t*, const uint32_t,const uint32_t);
void getHRKeyFrame();
void requestSensorTimeOut();

void ConfigApp(bool [], const char*, int cannedDatasetType);


void CallMeshSavedCallback( );

void setJavaEnv(JNIEnv* (&env), jobject& obj);
void clearJavaEnv(JNIEnv* (&env), jobject& obj);



/*
 * Callbacks for notifying mesh is written to file
 */
typedef int (*MeshSavedCallback)( );
void SetMeshSavedCallback(MeshSavedCallback ptr);

//SCVE queue rgbd
void SCVEqueueRGBD(uint8_t *ColorImg, uint32_t colorImgWidth, uint32_t colorImgHeight, int16_t *DepthImg, uint32_t depthImgWidth, uint32_t depthImgHeight, int64_t synctime, int64_t timestamp);

float GetCameraData(float * pCameraMatrix);
bool GetBoundingBox(float *bb);
void SetBBSize(float size);
void SetBBCenter(float x, float y, float z);


