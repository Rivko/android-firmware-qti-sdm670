/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef SURFACESPLATTINGDRIVER_HPP
#define SURFACESPLATTINGDRIVER_HPP

#include <memory>
#include <vector>

#include <scveScan3D.hpp>
#include <scveScan3DTypes.hpp>
#include <scveTypes.hpp>

#include <thread>
#include <mutex>
#include <cmath>
#include <math.h>
#include <core/Point.hpp>
#include <core/Quaternion.hpp>

//Moved from CPP
#include <pil/CG.hpp>
#include <pil/Timer.hpp>
#include <graphics/Viewport.hpp>

#include <core/Log.hpp>
#include <core/Matrix.hpp>
#include <core/Quaternion.hpp>

#include <resource/Assets.hpp>
#include <resource/Model.hpp>
#include <resource/Shader.hpp>

#include <graphics/Camera.hpp>
#include <graphics/Blending.hpp>
#include <graphics/Device.hpp>
#include <graphics/Frame.hpp>
#include <graphics/Mesh.hpp>
#include <graphics/Program.hpp>
#include <graphics/Projection.hpp>
#include <graphics/Sampler.hpp>
#include <graphics/Texture.hpp>
#include <graphics/UniformBuffer.hpp>

#include <jni.h>
#include "androidUtility.hpp"
#include "jniUtils.h"
#include <ViewController.h>

//class SlamResultHolder;
class PointCloud;

typedef struct _KeyFrameDisplay {
	bool isGenTexture;
	uint32_t glPts3fHandle;
	uint32_t glValidIndexHandle;
	uint32_t glTextureId;
	uint32_t glValidIndexCnt;
	std::unique_ptr<float[]>CameraMatrix;
	std::unique_ptr<uint8_t[]>RGB;
	std::unique_ptr<float[]>Pts;
	std::unique_ptr<float[]>Norms;
	uint32_t width;
	uint32_t height;
	INLINE _KeyFrameDisplay(uint32_t w, uint32_t h) : isGenTexture(false), glPts3fHandle(0), glValidIndexHandle(0), glTextureId(0), glValidIndexCnt(0) {
		width  = w;
		height = h;
		CameraMatrix.reset(new float[16]);
		Pts.reset(new float[w * h * 3]);
		Norms.reset(new float[w * h * 3]);
		RGB.reset(new uint8_t[w * h * 3]);
	}
}KeyFrameDisplay;

typedef std::unique_ptr<KeyFrameDisplay> KFptr;
typedef std::vector<KFptr> KF;


class SurfaceSplattingDriver : public ViewController {
public:
  enum class RenderMode : uint8_t {
    Dilation,
    Splatting
  };
  enum class Stage : uint8_t {
    CameraPreview,
    BeforeScan,
    Scanning,
    AfterScan,
    KeyFrameSelection,
  };
  SurfaceSplattingDriver();
  ~SurfaceSplattingDriver();

  const core::Point &getBoundingBoxCenter() const;
  const core::Vector &getBoundingBoxDimension() const;
  const core::Quaternion &getBoundingBoxOrientation() const;
  bool getKeyFrameStatus(int32_t id) const;
  RenderMode getRenderMode() const;
  int32_t getSelectedKeyFrameID() const;
  Stage getStage();

  void set(RenderMode mode);
  void set(Stage stage);
  void setPreviewImage(uintptr_t address);
  void toggleInputMode();
  void SetViewerTargetAngles(float x, float y);
  void SetBBSize(float size);
  void SetBBCenter(float x, float y, float z);
  void SetInitialSensorReadings(float sensorPitchDeg, float sensorRollDeg);


  void handleCursorAction(int32_t button, int32_t action, int32_t x, int32_t y);
  void handleCursorMovement(int32_t button, int32_t x, int32_t y);
  int32_t initialize(int32_t preview_width,
  int32_t preview_height, int32_t vp_width, int32_t vp_height);

  int32_t load_and_link_vert_frag_shaders(char* vertShaderStr, char* fragShaderStr,
      std::unique_ptr<resource::Shader>& vertShaderPtr, std::unique_ptr<resource::Shader>& fragShaderPtr,
      int progIndx);

  void render();
  void GetNewPointCloud(PointCloud **ppPointCloud);
  void GetLivePointCloud(PointCloud **ppPointCloud);

  void setJavaEnv(JNIEnv* (&env), jobject& obj);


  void detach();

private:
	SurfaceSplattingDriver(const SurfaceSplattingDriver&) = delete;
	SurfaceSplattingDriver &operator=(const SurfaceSplattingDriver&) = delete;

	struct Impl;
	std::unique_ptr<Impl> mImpl;
	void SCVEextract_valid_points(const KeyFrameDisplay &kf,
        uint64_t &n_valids, std::unique_ptr<float[]> &positions,
        std::unique_ptr<float[]> &normals, std::unique_ptr<uint8_t[]> &colors);

	void copyToBackBuffer();
	void updateShaderParameters();
	void checkBBFaceOrientationChange(int newFaceInteger);

	void visualizeBoundingBox();
	void visualizeCameraInput();

    void renderBackgroundGradient();

	//SCVE
	float CameraMatrix[16];

    int32_t m_winWidth, m_winHeight;
    int32_t m_imgWidth, m_imgHeight;
    int32_t selectKeyFrameNum;
    bool rotateTranslateSwitch;
    bool isDraw;
	bool isData;
	bool isCamMatrix;
    bool isRotate;
    bool isScale;
    bool isTranslate;
    bool isLightOn;
    bool isShowCam;
    bool isDepthRangeChange;

	mutable KF  mKF;
	KeyFrameDisplay* pKFD;
    std::mutex  io_mutex_std;
    std::thread drawingThread_std;

	//jclass      stopHandlerClass;
    //jobject     stopHandlerObj;

    JavaVM*     javaVM;
    JNIEnv*     jniEnv;
    jclass      dataClass;
    jclass      javaClassRef;
    bool	    isJavaThreadAttached;


public:
	void setPreviewData(SCVE::Scan3DPreviewData *pData);
	void setScanData(SCVE::Scan3DPreviewData *pData);
	void setCamMatrix(float *p);

	void SCVEupdateBoundingBox(KeyFrameDisplay *&kfi);
	void SCVEupdateKeyFrames(KeyFrameDisplay *& kfi, std::vector<KeyFrameDisplay *> & kfis);
	void SCVEvisualizeCurrentKeyFrameImage(KeyFrameDisplay *kfi);
	void SCVEvisualizeKeyFrames(const std::vector<KeyFrameDisplay *> & kfis);
	void SCVEvisualizePoints(KeyFrameDisplay *kfi);
	void SCVErender(bool update);

};


struct SurfaceSplattingDriver::Impl {
  std::unique_ptr<graphics::Frame[]> mFrames;
  std::unique_ptr<graphics::Program[]> mPrograms;
  std::unique_ptr<graphics::Sampler[]> mSamplers;
  std::unique_ptr<graphics::Texture[]> mTextures;
  std::unique_ptr<graphics::Texture> mBackgroundImage;

  std::unique_ptr<graphics::Mesh> mStreamedPoints;
  std::vector<std::unique_ptr<graphics::Mesh>> *mKeyFrames;

  uintptr_t mPreviewImageAddress;
  int32_t mPreviewImageHeight;
  int32_t mPreviewImageWidth;

  Stage mStage;
  RenderMode mRenderMode;
  std::unique_ptr<graphics::Camera> mCamera;

  std::mutex mAccessSRH;
  KF *pKF;

  core::Point mBoundingBoxCenter;
  core::Vector mBoundingBoxDimension;
  core::Quaternion mBoundingBoxOrientation;
  std::unique_ptr<float[]> mAABB;

  core::Vector mAxisOffset;
  float mTargetRadius;
  float mDiskRadius, mFilterRadius, mSplatSize;

  std::unique_ptr<bool[]> mKeyFrameSelections;
  int32_t mKeyFrameImageID;

  //ViewController-related adjustments
  uint32_t mInputMode;
  int32_t mNumberFingersEnabled;
  float mAngleX, mAngleY;
  float mHorizontalOffset[2];
  float mOnTouchX[3], mOnTouchY[3];
  float mVerticalOffset[2];
  float mZoom[2];

  float targetAngleX = 0.0f;
  float targetAngleY = 0.0f;

  float newTargetAngleX = 0.0f;
  float newTargetAngleY = 0.0f;
  core::Quaternion mOrientationAccumulation;
  core::Quaternion mOrientationPrevious;
  int currentBBFace = -1;
  bool movingTowardsTargetView = false;
  bool BB_size_changed = false;
  float initial_BB_size = 0.0f;
  float initSensorPitchDeg = 0.0f;
  float initSensorRollDeg = 0.0f;

  bool  bCancelTranslate = false;

  float * m_pPositions;
  float * m_pNormals;
  unsigned char * m_pColors;
  float * m_pComputeNormalBuffer;
  int m_destinationFrameBuffer;

  Impl();
  ~Impl();
};

template<typename T>
void R2q(T* R,T* q)
{
  const T& r11 = R[0]; const T& r12 = R[1]; const T& r13 = R[2];
  const T& r21 = R[3]; const T& r22 = R[4]; const T& r23 = R[5];
  const T& r31 = R[6]; const T& r32 = R[7]; const T& r33 = R[8];

  T& q0 = q[0];
  T& q1 = q[1];
  T& q2 = q[2];
  T& q3 = q[3];
  q0 = ( r11 + r22 + r33 + 1.0f)*0.25f;
  q1 = ( r11 - r22 - r33 + 1.0f)*0.25f;
  q2 = (-r11 + r22 - r33 + 1.0f)*0.25f;
  q3 = (-r11 - r22 + r33 + 1.0f)*0.25f;
  if(q0 < 0.0f) q0 = 0.0f;
  if(q1 < 0.0f) q1 = 0.0f;
  if(q2 < 0.0f) q2 = 0.0f;
  if(q3 < 0.0f) q3 = 0.0f;
  q0 = sqrt(q0);
  q1 = sqrt(q1);
  q2 = sqrt(q2);
  q3 = sqrt(q3);
  if(q0 >= q1 && q0 >= q2 && q0 >= q3) {
      q1 = (r32 - r23)>=0 ? q1:-q1 ;
      q2 = (r13 - r31)>=0 ? q2:-q2 ;
      q3 = (r21 - r12)>=0 ? q3:-q3 ;
  } else if(q1 >= q0 && q1 >= q2 && q1 >= q3) {
      q0 = (r32 - r23)>=0 ? q0:-q0;
      q2 = (r21 + r12)>=0 ? q2:-q2;
      q3 = (r13 + r31)>=0 ? q3:-q3;
  } else if(q2 >= q0 && q2 >= q1 && q2 >= q3) {
      q0 = (r13 - r31)>=0 ? q0:-q0;
      q1 = (r21 + r12)>=0 ? q1:-q1;
      q3 = (r32 + r23)>=0 ? q3:-q3;
  } else if(q3 >= q0 && q3 >= q1 && q3 >= q2) {
      q0 = (r21 - r12)>=0 ? q0:-q0;
      q1 = (r31 + r13)>=0 ? q1:-q1;
      q2 = (r32 + r23)>=0 ? q2:-q2;
  } else {
      printf("coding error\n");
  }

  T invR = T(1)/(sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]));

  q0 *= invR;
  q1 *= invR;
  q2 *= invR;
  q3 *= invR;
}


#endif
