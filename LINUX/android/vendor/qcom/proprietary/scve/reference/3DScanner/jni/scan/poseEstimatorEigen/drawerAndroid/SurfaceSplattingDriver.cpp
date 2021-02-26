/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "SurfaceSplattingDriver.hpp"

#include <mutex>

#include <unistd.h>
#include <vector>
#include <map>
#include <string>

#include <libjpeg/jpgIO.h>

#include <pil/CG.hpp>
#include <pil/Timer.hpp>

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
#include <graphics/Viewport.hpp>

#include <shaders/display_points_vert.h>
#include <shaders/display_points_frag.h>
#include <shaders/display_lines_vert.h>
#include <shaders/display_lines_frag.h>
#include <shaders/dilate_points_vert.h>
#include <shaders/dilate_points_frag.h>
#include <shaders/screen_space_vert.h>
#include <shaders/screen_space_vflip_frag.h>
#include <shaders/surface_splatting_accumulation_vert.h>
#include <shaders/surface_splatting_accumulation_frag.h>
#include <shaders/surface_splatting_normalization_frag.h>
#include <shaders/surface_splatting_visibility_vert.h>
#include <shaders/surface_splatting_visibility_frag.h>

#include <shaders/gradient_vert.h>
#include <shaders/gradient_frag.h>

#include <android/log.h>

#include "jniUtils.h"

// General JNI Miscellaneous
#define  LOG_TAG_SCVE    "Scan3D"
#define  LOGISCVE(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG_SCVE,__VA_ARGS__)
#define  LOGESCVE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG_SCVE,__VA_ARGS__)

extern int imgWidth;
extern int imgHeight;

//Max Valid Depth Taken from RGBD Image and value taken from dvoParameters.hpp
#define MAX_VALID_DEPTH    1.3*1000
float MaxValidDepth =1.3*1000;

//we want to do 2 finger translate and 1 finger rotation, but
//since mesh generation only supports BB size and translation w.r.t. the point cloud
//for now we are only enabling single finger translation and disabling rotation

//#define NUM_FINGER_BB_TRANSLATE 2
#define NUM_FINGER_BB_TRANSLATE 1

SurfaceSplattingDriver::Impl::Impl() :
    mFrames(nullptr),
    mPrograms(nullptr),
    mSamplers(nullptr),
    mTextures(nullptr),
    mBackgroundImage(nullptr),
    mStreamedPoints(nullptr),
    mKeyFrames(nullptr),
    mPreviewImageAddress(0),
    mPreviewImageHeight(0),
    mPreviewImageWidth(0),
    mStage(Stage::BeforeScan),
    mRenderMode(RenderMode::Dilation),
    mAABB(nullptr),
    mKeyFrameSelections(nullptr),
    mKeyFrameImageID(0),
    mInputMode(1),
    m_pPositions(NULL),
    m_pNormals(NULL),
    m_pColors(NULL),
    m_pComputeNormalBuffer(NULL),
    m_destinationFrameBuffer(0)
{
    mZoom[0] = 1.0f;
    mZoom[1] = (5.0f);

    //ViewController uses absolute coordinates for
    //m_currentTrackHoriz and m_currentTrackVert
    mHorizontalOffset[0] = mHorizontalOffset[1] = 0.0f;
    mVerticalOffset[0] = mVerticalOffset[1] = 0.0f;

    mNumberFingersEnabled = 0;

    //ViewController accumulations rotations
    mAngleX = 0.0f;
    mAngleY = 0.0f;

}

SurfaceSplattingDriver::Impl::~Impl() {

  mFrames = nullptr;
  mPrograms = nullptr;
  mSamplers = nullptr;
  mTextures = nullptr;
  mBackgroundImage = nullptr;

  mStreamedPoints = nullptr;
  if(nullptr != mKeyFrames) {
    mKeyFrames->clear();
    delete mKeyFrames;
    mKeyFrames = nullptr;
  }

  mKeyFrameSelections = nullptr;

  mCamera = nullptr;

  mAABB = nullptr;

  if (m_pPositions != NULL)
  {
      free(m_pPositions);
  }

  if (m_pNormals != NULL)
  {
      free(m_pNormals);
  }

  if (m_pColors != NULL)
  {
      free(m_pColors);
  }

  if (m_pComputeNormalBuffer != NULL)
  {
      free(m_pComputeNormalBuffer);
  }

  graphics::Device::terminate();
  core::Log::terminate();
}

SurfaceSplattingDriver::SurfaceSplattingDriver() : mImpl(new Impl){
  pKFD = NULL;
}

SurfaceSplattingDriver::~SurfaceSplattingDriver() {

  if(isDraw) {
    isDraw = false;
  }

  if(pKFD) {
    free(pKFD);
    pKFD = NULL;
  }

  isData = false;
  isCamMatrix = false;

}

const core::Point &SurfaceSplattingDriver::getBoundingBoxCenter() const {
  return mImpl->mBoundingBoxCenter;
}

const core::Vector &SurfaceSplattingDriver::getBoundingBoxDimension() const {
  return mImpl->mBoundingBoxDimension;
}

const core::Quaternion &SurfaceSplattingDriver::getBoundingBoxOrientation()
    const {
  return mImpl->mBoundingBoxOrientation;
}

bool SurfaceSplattingDriver::getKeyFrameStatus(int32_t id) const {
  if(nullptr == mImpl->mKeyFrameSelections)
    return false;

  return mImpl->mKeyFrameSelections[id];
}

SurfaceSplattingDriver::RenderMode SurfaceSplattingDriver::getRenderMode()
    const{
  return mImpl->mRenderMode;
}

int32_t SurfaceSplattingDriver::getSelectedKeyFrameID() const {
  return mImpl->mKeyFrameImageID;
}

SurfaceSplattingDriver::Stage SurfaceSplattingDriver::getStage() {
  return mImpl->mStage;
}

void SurfaceSplattingDriver::set(
    SurfaceSplattingDriver::RenderMode mode) {
  mImpl->mRenderMode = mode;
}

void SurfaceSplattingDriver::set(SurfaceSplattingDriver::Stage stage) {
  mImpl->mStage = stage;
}

void SurfaceSplattingDriver::setPreviewImage(uintptr_t address) {
  mImpl->mPreviewImageAddress = address;
}

void SurfaceSplattingDriver::SetViewerTargetAngles(float x, float y)
{

  if (!mImpl->movingTowardsTargetView) {
         mImpl->newTargetAngleX = mImpl->targetAngleX + x;
       mImpl->newTargetAngleY = mImpl->targetAngleY + y;

        //LOGI("--setting newTargetAngleX,Y = %f,%f",mImpl->newTargetAngleX,mImpl->newTargetAngleY);

       mImpl->movingTowardsTargetView = true;
   }
}

void SurfaceSplattingDriver::SetBBSize(float size)
{

    mImpl->BB_size_changed = true;

    mImpl->mBoundingBoxDimension(0) = size;
    mImpl->mBoundingBoxDimension(1) = size;
    mImpl->mBoundingBoxDimension(2) = size;

}

void SurfaceSplattingDriver::SetBBCenter(float x, float y, float z){

    std::lock_guard<std::mutex> lock(mImpl->mAccessSRH);

    mImpl->mBoundingBoxCenter(0) = x;
    mImpl->mBoundingBoxCenter(1) = y;
    mImpl->mBoundingBoxCenter(2) = z;

}


void SurfaceSplattingDriver::SetInitialSensorReadings(float sensorPitchDeg, float sensorRollDeg)
{
    mImpl->initSensorPitchDeg = sensorPitchDeg;
    mImpl->initSensorRollDeg = sensorRollDeg;

    //we won't be using the init pitch/roll until the texture gen meshing post process can handle BB rotations
    //so remove the next 2 lines when we have this capability;
    mImpl->initSensorPitchDeg = 0.0f;
    mImpl->initSensorRollDeg = 0.0f;

    //LOGI("SSD setting initial sensor readings for pitch, roll = %f , %f",sensorPitchDeg, sensorRollDeg);
}



void SurfaceSplattingDriver::toggleInputMode() {
  //switch input modes
  mImpl->mInputMode = (mImpl->mInputMode + 1) % 2;
  auto i = mImpl->mInputMode;

  //ViewController only supports one mode, so swap parameters between modes
  m_currentZoom = mImpl->mZoom[i];
  m_currentAngleX = mImpl->mAngleX;
  m_currentAngleY = mImpl->mAngleY;
  m_currentTrackHoriz = mImpl->mHorizontalOffset[i];
  m_currentTrackVert = mImpl->mVerticalOffset[i];
}

void SurfaceSplattingDriver::handleCursorAction(
    int32_t button, int32_t action, int32_t x, int32_t y) {
  //required for ViewController
  MouseButtonCallback(button, action, x, y);
  if( action == 0 ) mImpl->bCancelTranslate = true;
  else mImpl->bCancelTranslate = false;
  //only support up to three fingers
  if(button >= 3)
    return;

  //required to change Kuen's depth range module via tapping on screen
  if(Stage::BeforeScan == getStage()) {
    //single finger tap released
    if(0 == button && 0 == action) {
      graphics::Device dev;
      auto unit_x = x / static_cast<float>(dev.getDefaultWindowFrameWidth());
      if(unit_x < 0.5f)
        MaxValidDepth *= 0.975f;
      else
        MaxValidDepth *= 1.025f;

        MaxValidDepth = std::max<float>(1.0f * 1000,
          std::min<float>(MaxValidDepth, MAX_VALID_DEPTH));
    }
  }

  //bounding box controls
  //if(1 == mImpl->mInputMode) {
    mImpl->mOnTouchX[button] = x;
    mImpl->mOnTouchY[button] = y;
  //}

  //artifact required to work with ViewController since it doesn't support
  //multiple interactions
  if(1 == action)
    ++mImpl->mNumberFingersEnabled;
  else if(0 == action)
    --mImpl->mNumberFingersEnabled;
}

void SurfaceSplattingDriver::handleCursorMovement(
    int32_t button, int32_t x, int32_t y) {
  //required for ViewController
  MousePositionCallback(button, x,y);
  if(button >= 2)
    return;

  if( mImpl->bCancelTranslate == true ) return;

  //converts touch location to [-1,1]
  graphics::Device dev;
  auto w = static_cast<float>(dev.getDefaultWindowFrameWidth());
  auto h = static_cast<float>(dev.getDefaultWindowFrameHeight());
  auto unit_x = (mImpl->mOnTouchX[button] - x) / w;
  auto unit_y = (mImpl->mOnTouchY[button] - y) / h;

  //two or one finger movement (translation) of bounding box
  if(1 == mImpl->mInputMode && 0 == button &&
      NUM_FINGER_BB_TRANSLATE == mImpl->mNumberFingersEnabled) {
    auto shift = unit_x * mImpl->mCamera->getAxis(0) +
      unit_y * (-mImpl->mCamera->getAxis(1));
    mImpl->mBoundingBoxCenter = mImpl->mBoundingBoxCenter + shift;

    //LOGI("finger translate BB center X,Y,Z to %f, %f, %f",mImpl->mBoundingBoxCenter(0),mImpl->mBoundingBoxCenter(1),mImpl->mBoundingBoxCenter(2));

    //finger is sliding, so reset position to finger's current location
    mImpl->mOnTouchX[button] = x;
    mImpl->mOnTouchY[button] = y;
  }

}


int32_t SurfaceSplattingDriver::load_and_link_vert_frag_shaders(char* vertShaderStr, char* fragShaderStr,
    std::unique_ptr<resource::Shader>& vert, std::unique_ptr<resource::Shader>& frag, int progIndx) {

    resource::Assets assets;
    resource::Shader *ptr_shader = nullptr;
    assets.loadShaderStr(vertShaderStr, ptr_shader, resource::Shader::Type::Vertex);
    vert.reset(ptr_shader);
    assets.loadShaderStr(fragShaderStr, ptr_shader, resource::Shader::Type::Fragment);
    frag.reset(ptr_shader);

    const resource::Shader *shaders[] {vert.get(), frag.get()};
    if(0 != mImpl->mPrograms[progIndx].link(2, shaders))
       return 1;
    else
       return 0;
}


int32_t SurfaceSplattingDriver::initialize(
    int32_t preview_width, int32_t preview_height,
    int32_t vp_width, int32_t vp_height) {
  //initialize default ViewController parameters
  m_currentZoom = mImpl->mZoom[mImpl->mInputMode];
  m_currentTrackHoriz = mImpl->mHorizontalOffset[mImpl->mInputMode];
  m_currentTrackVert = mImpl->mVerticalOffset[mImpl->mInputMode];
  m_currentAngleX = mImpl->mAngleX;
  m_currentAngleY = mImpl->mAngleY;

  std::string env_path = "/sdcard/data/resources/";
  core::Log::initialize((env_path + "logs/").c_str());

  graphics::Device::initialize(static_cast<uint32_t>(vp_width),
    static_cast<uint32_t>(vp_height));

  //load in shader programs
  resource::Assets assets;
  std::string path;

  mImpl->mPrograms.reset(new graphics::Program[8]);

  std::unique_ptr<resource::Shader> vert, frag;
  resource::Shader *ptr_shader = nullptr;

  load_and_link_vert_frag_shaders(display_points_vert, display_points_frag, vert, frag, 0);
  load_and_link_vert_frag_shaders(screen_space_vert, surface_splatting_normalization_frag, vert, frag, 1);
  load_and_link_vert_frag_shaders(dilate_points_vert, dilate_points_frag, vert, frag, 2);
  load_and_link_vert_frag_shaders(surface_splatting_visibility_vert, surface_splatting_visibility_frag, vert, frag, 3);
  load_and_link_vert_frag_shaders(surface_splatting_accumulation_vert, surface_splatting_accumulation_frag, vert, frag, 4);
  load_and_link_vert_frag_shaders(display_lines_vert, display_lines_frag, vert, frag, 5);
  load_and_link_vert_frag_shaders(screen_space_vert, screen_space_vflip_frag, vert, frag, 6);

  load_and_link_vert_frag_shaders(gradient_vert, gradient_frag, vert, frag, 7);

  //initialize shader variables
  updateShaderParameters();

  graphics::Device dev;
  //create textures for FBO
  mImpl->mTextures.reset(new graphics::Texture[5]);
  //last texture is allocated during Stage::KeyFrameSelection
  graphics::Texture::Descriptor tex_desc;
  tex_desc.width = dev.getDefaultWindowFrameWidth();
  tex_desc.height = dev.getDefaultWindowFrameHeight();
  tex_desc.format = graphics::Texture::Format::Depth32F;
  mImpl->mTextures[0].load(tex_desc);
  //color
  tex_desc.format = graphics::Texture::Format::RGBA16F;
  mImpl->mTextures[1].load(tex_desc);
  //normal
  tex_desc.format = graphics::Texture::Format::RGBA16F;
  mImpl->mTextures[2].load(tex_desc);

  //create storage for displaying device's camera
  mImpl->mPreviewImageWidth = preview_width;
  mImpl->mPreviewImageHeight= preview_height;
  tex_desc.width = mImpl->mPreviewImageWidth;
  tex_desc.height = mImpl->mPreviewImageHeight;
  tex_desc.format = graphics::Texture::Format::RGB8;
  mImpl->mTextures[3].load(tex_desc);

  //create FBO for splatting
  mImpl->mFrames.reset(new graphics::Frame[1]);
  graphics::Frame::Attachment attachments[3];
  mImpl->mFrames[0].bind();
  attachments[0].buffer = &mImpl->mTextures[0];
  attachments[1].buffer = &mImpl->mTextures[1];
  attachments[1].index = 0;
  attachments[2].buffer = &mImpl->mTextures[2];
  attachments[2].index = 1;
  mImpl->mFrames[0].set(3, attachments);
  mImpl->mFrames[0].unbind();

  //samplers are used for pixel reads and backbuffer copy
  mImpl->mSamplers.reset(new graphics::Sampler[2]);
  mImpl->mSamplers[0].set(graphics::Sampler::Filter::None);
  mImpl->mSamplers[0].set(graphics::Sampler::Mipmap::None);
  mImpl->mSamplers[1].set(graphics::Sampler::Filter::Bilinear);
  mImpl->mSamplers[1].set(graphics::Sampler::Mipmap::Smooth);


  isDraw = true;
  isData = false;
  isCamMatrix = false;

  return 0;
}

void SurfaceSplattingDriver::copyToBackBuffer() {
  graphics::Device dev;
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, (unsigned int)mImpl->m_destinationFrameBuffer);

  dev.set(mImpl->mPrograms[1]);
  dev.set({
    std::forward_as_tuple(mImpl->mSamplers[0], mImpl->mTextures[1]),
  });

  const GLfloat fullscreenVertices[] = { 0.0f, 2.0f, 0.0f, 0.0f, 2.0f, 0.0f };
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, fullscreenVertices);

  dev.dispatch(graphics::Device::Primitive::Triangle, 1);
  glDisableVertexAttribArray(0);
}


void SurfaceSplattingDriver::checkBBFaceOrientationChange(int newFaceInteger)
{
   if (mImpl->currentBBFace == -1) {
        core::Quaternion qFirstRot = core::Quaternion(core::Vector(1, 0, 0), 180 * M_PI / 180.0f);
        core::Quaternion qInitPitch = core::Quaternion(core::Vector(1.0f, 0.0f, 0.0f), (mImpl->initSensorPitchDeg) * M_PI / 180.0f);
        core::Quaternion qInitRoll = core::Quaternion(core::Vector(0.0f, 0.0f, -1.0f), (mImpl->initSensorRollDeg) * M_PI / 180.0f);

        mImpl->mOrientationPrevious = normalize(qInitRoll * qInitPitch * qFirstRot);

        mImpl->currentBBFace = 0;
   }
   else if (newFaceInteger != mImpl->currentBBFace) {
        //LOGI("FACE change from %d to %d",mImpl->currentBBFace,newFaceInteger);
        mImpl->mAngleX = 0.0f; //changed face so reset angle accumulation
        mImpl->mAngleY = 0.0f; //changed face so reset angle accumulation
        m_currentAngleX = 0.0f;
        m_currentAngleY = 0.0f;
        mImpl->mOrientationPrevious = mImpl->mOrientationAccumulation; //save the current orientation as previous
        mImpl->currentBBFace = newFaceInteger;
   }
}

void SurfaceSplattingDriver::updateShaderParameters() {

  if(nullptr == mImpl->mCamera) {
    mImpl->mCamera.reset(new graphics::Camera(core::Point(0.0f, 0.0f, 0.0f),
      {core::Vector(1, 0, 0), 180 * M_PI / 180.0f}));

    mImpl->mDiskRadius = 0.0025f;
    mImpl->mFilterRadius = sqrtf(2.0f);
  }

  //update camera from user input
  if(Stage::AfterScan == getStage()) {

      //Kuen's coordinate system requires a rotation
      core::Quaternion q0(core::Vector(1, 0, 0), 180 * M_PI / 180.0f);

      /*
      *  animate the camera rotation when button gives it a new orientation
      */
      if (abs(mImpl->targetAngleX - mImpl->newTargetAngleX) < 0.1f &&
         abs(mImpl->targetAngleY - mImpl->newTargetAngleY) < 0.1f) {
          //LOGI("reached targetAngle");
          mImpl->movingTowardsTargetView = false;
          mImpl->targetAngleX = mImpl->newTargetAngleX;
          mImpl->targetAngleY = mImpl->newTargetAngleY;
      }
      else
      {
       mImpl->targetAngleX += 0.5f * (mImpl->newTargetAngleX - mImpl->targetAngleX);
       mImpl->targetAngleY += 0.5f * (mImpl->newTargetAngleY - mImpl->targetAngleY);
      }

      core::Quaternion q1;
      core::Quaternion q2;

      if (mImpl->newTargetAngleX == 0.0f && mImpl->newTargetAngleY == 0.0f) {
          checkBBFaceOrientationChange(0);
          //LOGI("FRONT face mAngleX,Y = %f,%f",mImpl->mAngleX,mImpl->mAngleY);
          q1 = core::Quaternion(core::Vector(1.0f,  0.0f, 0.0f), (mImpl->mAngleY) * M_PI / 180.0f);
          q2 = core::Quaternion(core::Vector(0.0f, 1.0f, 0.0f), (mImpl->mAngleX) * M_PI / 180.0f);
      }
      else if (mImpl->newTargetAngleX == -90.0f && mImpl->newTargetAngleY == 0.0f) {
          checkBBFaceOrientationChange(1);
          //LOGI("RIGHT face mAngleX,Y = %f,%f",mImpl->mAngleX,mImpl->mAngleY);
          q1 = core::Quaternion(core::Vector(0.0f, 0.0f, -1.0f), (mImpl->mAngleY) * M_PI / 180.0f);
          q2 = core::Quaternion(core::Vector(0.0f, 1.0f, 0.0f), (mImpl->mAngleX) * M_PI / 180.0f);
      }
      else if (mImpl->newTargetAngleX == 90.0f && mImpl->newTargetAngleY == 0.0f) {
          //LOGI("LEFT face");
          checkBBFaceOrientationChange(2);
          q1 = core::Quaternion(core::Vector(0.0f, 0.0f, 1.0f), (mImpl->mAngleY) * M_PI / 180.0f);
          q2 = core::Quaternion(core::Vector(0.0f, 1.0f, 0.0f), (mImpl->mAngleX) * M_PI / 180.0f);
      }
      else if (mImpl->newTargetAngleX == 0.0f && mImpl->newTargetAngleY == -90.0f) {
          //LOGI("TOP face");
          checkBBFaceOrientationChange(3);
          q1 = core::Quaternion(core::Vector(1.0f, 0.0f, 0.0f), (mImpl->mAngleY) * M_PI / 180.0f);
          q2 = core::Quaternion(core::Vector(0.0f, 0.0f, -1.0f), (mImpl->mAngleX) * M_PI / 180.0f);
      }

      core::Quaternion q3(core::Vector(1.0f, 0.0f, 0.0f),  mImpl->targetAngleY * M_PI / 180.0f);
      core::Quaternion q4(core::Vector(0.0f, -1.0f, 0.0f),  mImpl->targetAngleX * M_PI / 180.0f);

      //2 finger translate, then 1 finger is for BB rotation
      if (NUM_FINGER_BB_TRANSLATE == 2)
         mImpl->mOrientationAccumulation = normalize(mImpl->mOrientationPrevious * q2 * q1);
      else //1 finger translate and not allowing BB rotation
         mImpl->mOrientationAccumulation = mImpl->mOrientationPrevious;

      mImpl->mCamera->setOrientation(normalize(mImpl->mOrientationAccumulation * q4 * q3));
      mImpl->mBoundingBoxOrientation = mImpl->mOrientationAccumulation;


      auto shift = mImpl->mCamera->getAxis(2) * mImpl->mTargetRadius +
        mImpl->mCamera->getAxis(0) * mImpl->mAxisOffset(0) +
        mImpl->mCamera->getAxis(1) * mImpl->mAxisOffset(1);
      mImpl->mCamera->setPosition(getBoundingBoxCenter() + shift);

  }

  //compute parameters for surface splatting
  graphics::Device dev;
  float np = 0.1f, fp = 10.0f,
    vp_w = dev.getDefaultWindowFrameWidth(),
    vp_h = dev.getDefaultWindowFrameHeight();
  float w_over_h = vp_w / vp_h, hfov = 90.0f * M_PI / 180.0f;

  graphics::Projection proj(hfov, w_over_h, np, fp);
  auto &cam = *mImpl->mCamera;

  float np_w = 2 * np * std::tan(hfov * 0.5f);
  float np_h = np_w / w_over_h;
  float splat_size_heuristic = np * vp_h / np_h;
  mImpl->mSplatSize = splat_size_heuristic;

  //compute AABB min, max, orientation for culling in shaders
  auto aabb_min = -getBoundingBoxDimension();
  auto aabb_max = getBoundingBoxDimension();

  core::Matrix inverse_model(getBoundingBoxOrientation());
  inverse_model.transpose();

  core::Point inv_T = inverse_model * (-getBoundingBoxCenter());
  inverse_model(0, 3) = inv_T(0);
  inverse_model(1, 3) = inv_T(1);
  inverse_model(2, 3) = inv_T(2);

  //send parameters to shaders immediately
  auto cam_view = cam.getView();
  auto proj_perspective = proj.getPerspective();
  for(uint32_t i = 0; i < 7; ++i) {
    //(1) normalization shader and (6) camera preview shader
   //does not have any parameters
    if(1 == i || 6 == i)
      continue;

    dev.set(mImpl->mPrograms[i]);
    mImpl->mPrograms[i].update("gCS_view", cam_view);
    mImpl->mPrograms[i].update("gCS_projection", proj_perspective);

    //(5) bounding box shader only has model, view, projection matrix
    if(5 == i)
      continue;

    mImpl->mPrograms[i].update("gBB_max", aabb_max);
    mImpl->mPrograms[i].update("gBB_min", aabb_min);
    mImpl->mPrograms[i].update("gBB_inverseModel", inverse_model);

    if(Stage::BeforeScan == getStage() || Stage::AfterScan == getStage())
      mImpl->mPrograms[i].update("gBB_enableCulling", 0.0f); //0 enables culling
    else
      mImpl->mPrograms[i].update("gBB_enableCulling", 1.0f);

    //(0) point visualization shader does not have any of the parameters below
    if(0 == i)
      continue;

    mImpl->mPrograms[i].update("gCS_viewDirection",
      cam.getView() * cam.getAxis(2));

    mImpl->mPrograms[i].update("gCS_diskRadius", mImpl->mDiskRadius);
    mImpl->mPrograms[i].update("gCS_filterRadius",
      mImpl->mFilterRadius * 2.0f / vp_w);
    mImpl->mPrograms[i].update("gCS_splatSize", mImpl->mSplatSize);
    mImpl->mPrograms[i].update("gCS_unprojectScale", np_w / vp_w, np_h / vp_h);
    mImpl->mPrograms[i].update("gCS_unprojectOffset", np_w / 2, np_h / 2);
    mImpl->mPrograms[i].update("gCS_nearPlane", np);
    mImpl->mPrograms[i].update("gCS_depthScale", fp * np / (fp - np));
    mImpl->mPrograms[i].update("gCS_depthOffset", fp / (fp - np));

  }



}

void SurfaceSplattingDriver::visualizeBoundingBox() {
  graphics::Device dev;

  dev.set(mImpl->mPrograms[5]);

  //compute bounding box
  core::Matrix model(getBoundingBoxOrientation());
  model(0, 3) = mImpl->mBoundingBoxCenter(0);
  model(1, 3) = mImpl->mBoundingBoxCenter(1);
  model(2, 3) = mImpl->mBoundingBoxCenter(2);

  mImpl->mPrograms[5].update("gBB_model", model);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, mImpl->mAABB.get());

  dev.dispatch(graphics::Device::Primitive::Line, 4 * 3 * 2 * 3 / 6);
  glDisableVertexAttribArray(0);
}

void SurfaceSplattingDriver::visualizeCameraInput() {
  //update camera visuals before displaying
  mImpl->mTextures[3].bind();
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
    mImpl->mPreviewImageWidth, mImpl->mPreviewImageHeight,
    GL_RGB, GL_UNSIGNED_BYTE,
    reinterpret_cast<void *>(mImpl->mPreviewImageAddress));
  mImpl->mTextures[3].unbind();

  //display camera image for exposure calibration
  graphics::Device dev;
  dev.set(mImpl->mPrograms[6]);
  dev.set({
    std::forward_as_tuple(mImpl->mSamplers[1], mImpl->mTextures[3]),
  });
  const GLfloat fullscreenVertices[] = { 0.0f, 2.0f, 0.0f, 0.0f, 2.0f, 0.0f };
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, fullscreenVertices);

  dev.dispatch(graphics::Device::Primitive::Triangle, 1);
  glDisableVertexAttribArray(0);
}


void SurfaceSplattingDriver::detach()
{
  // detach the jniEnv

  if(isJavaThreadAttached) {
    //LOGI("SSD before detach thread");
   javaVM->DetachCurrentThread();
   jniEnv = NULL;
   isJavaThreadAttached = false;
  }
}

void SurfaceSplattingDriver::setScanData(SCVE::Scan3DPreviewData *pData) {
   std::lock_guard<std::mutex> lock(mImpl->mAccessSRH);

    SCVE::Scan3DPreviewData *_previewTempPtr = pData;
   int i = -1;
   while(_previewTempPtr) {

      i++;
      int size = mKF.size(); //number of key frames

      if(mKF.size() <= (uint32_t)i) {

         KFptr kfPtr(new KeyFrameDisplay(imgWidth, imgHeight));
         memcpy(kfPtr->CameraMatrix.get(), _previewTempPtr->nCameraMatrix, 16 * sizeof(float));
         memcpy(kfPtr->RGB.get(), _previewTempPtr->pColor, imgWidth * imgHeight * 3 * sizeof(uint8_t));
         memcpy(kfPtr->Pts.get(), _previewTempPtr->pPosition, imgWidth * imgHeight * 3 * sizeof(float));
         memcpy(kfPtr->Norms.get(), _previewTempPtr->pNormal, imgWidth * imgHeight * 3 * sizeof(float));
         float *cam = kfPtr ->CameraMatrix.get();
         mKF.push_back(std::move(kfPtr));

      } else {

         memcpy(mKF[i].get()->CameraMatrix.get(), _previewTempPtr->nCameraMatrix, 16 * sizeof(float));

      }
      _previewTempPtr = _previewTempPtr->pNext;
   }

   if(isData == false) isData = true;

}

//int cnt = 0;

void SurfaceSplattingDriver::setPreviewData(SCVE::Scan3DPreviewData *pData) {

   std::lock_guard<std::mutex> lock(mImpl->mAccessSRH);

   SCVE::Scan3DPreviewData *_previewTempPtr = pData;

   if(pKFD == NULL) {
      pKFD = new KeyFrameDisplay(imgWidth, imgHeight);
   }

/*
    if (cnt == 100) {
        FILE *fp = fopen("/sdcard/3dr/testrgb_out2.bin", "wb");
        fwrite(_previewTempPtr->pColor, sizeof(uint8_t), pKFD->width * pKFD->height * 3 * sizeof(uint8_t), fp);
        LOGI("setPreviewData saved .bin file");
        fclose(fp);
        //first = false;
    } else {
        LOGI("not saving data");
    }

    cnt++;
*/
   memcpy(pKFD->CameraMatrix.get(), _previewTempPtr->nCameraMatrix, 16 * sizeof(float));
   memcpy(pKFD->RGB.get(), _previewTempPtr->pColor, imgWidth * imgHeight * 3 * sizeof(uint8_t));
   memcpy(pKFD->Pts.get(), _previewTempPtr->pPosition, imgWidth * imgHeight * 3 * sizeof(float));
   memcpy(pKFD->Norms.get(), _previewTempPtr->pNormal, imgWidth * imgHeight * 3 * sizeof(float));

/*
    if (first == true) {
        int pixelCount = 0;
        for (int i=0; i < imgWidth * imgHeight * 3; i = i + 3){
            LOGI("pixel %d = %f %f %f", pixelCount, pKFD->Pts[i],pKFD->Pts[i+1],pKFD->Pts[i+2]);
            pixelCount++;
        }
        first = false;
    }
*/

/*
    if (cnt == 100) {
        FILE *fp1 = fopen("/sdcard/3dr/testrgb_out2.bin", "wb");
        fwrite(_previewTempPtr->pColor, sizeof(uint8_t), imgWidth * imgHeight  * 3 * sizeof(uint8_t), fp1);
        LOGI("setPreviewData saved .bin file");
        fclose(fp1);

        FILE *fp2 = fopen("/sdcard/3dr/test_r_g_b.bin", "wb");//this file will have the color info in the format r g b r g b r g b ...
        FILE *fp3 = fopen("/sdcard/3dr/testx_y_z.bin", "wb");//this file will have the depth info in the format x y z x y z x y z ...

        uint8_t r, g, b;
        float x, y, z;
        int pixelCount = 0;
        for (int i=0; i < imgWidth * imgHeight * 3; i = i + 3){
             r = pKFD->RGB[i];
             g = pKFD->RGB[i+1];
             b = pKFD->RGB[i+2];
            fprintf(fp2,"%d %d %d ",r, g, b);

            x = pKFD->Pts[i];
            y = pKFD->Pts[i+1];
            z = pKFD->Pts[i+2];
            fprintf(fp3,"%f %f %f ", x, y, z);
            //LOGI("pixelRGB : %d = %d %d %d", pixelCount, r, g, b);
            //if(x>0.0 || y>0.0||z>0.0)
            //LOGI("pixelXYZ : %d = %f %f %f", pixelCount, x, y, z);
           // pixelCount++;
        }

        LOGI("setPreviewData R G B saved .bin file");
        fclose(fp2);
        LOGI("setPreviewData x y z saved .bin file");
        fclose(fp3);

        //first = false;
    }

    cnt++;
*/
}


void SurfaceSplattingDriver::setCamMatrix(float *p)
{
   ////LOGE("FanLog setCamMatrix :E");
   std::lock_guard<std::mutex> lock(mImpl->mAccessSRH);
   //memcpy(CameraMatrix,p,16 * sizeof(float));
   if(pKFD == NULL) {
      pKFD = new KeyFrameDisplay(imgWidth, imgHeight);
   }
   memcpy(pKFD->CameraMatrix.get(), p, 16 * sizeof(float));

   if(isCamMatrix==false)
      isCamMatrix = true;

   ////LOGE("FanLog setCamMatrix :X");
}

void SurfaceSplattingDriver::SCVEupdateBoundingBox(KeyFrameDisplay *&kfi)
{


   if(nullptr == mImpl->mAABB)
   {
      mImpl->mAABB.reset(new float[4 * 3 * 2 * 3]);

      //heuristic for estimating bounding box center
      if(nullptr != kfi && Stage::BeforeScan == getStage())
      {
         //pick center of image
         //Width*Height/2+Width/2

         float * cp = &(kfi->Pts[(imgWidth * (imgHeight / 2) + (imgWidth / 2)) * 3]);

            //if(cp[2] > 0.0f)
            if(cp[2] > 0.5f)
         {
            mImpl->mBoundingBoxCenter(0) = cp[0];
            mImpl->mBoundingBoxCenter(1) = cp[1];
            mImpl->mBoundingBoxCenter(2) = cp[2];
         }
      }
   }

   //heuristic for estimating bounding box center
   if(nullptr != kfi && Stage::BeforeScan == getStage())
   {
      //pick center of image
      //Width*Height/2+Width/2

      float* cp = &(kfi->Pts[(imgWidth * (imgHeight / 2) + (imgWidth / 2)) * 3]);

      //if(cp[2] > 0.0f)
      if(cp[2] > 0.5f)
      {
         mImpl->mBoundingBoxCenter(2) = cp[2];
      }
   }

   auto &aabb_dim = getBoundingBoxDimension();

   auto tpp = core::Vector(aabb_dim(0), aabb_dim(1), aabb_dim(2));
   auto tpn = core::Vector(aabb_dim(0), aabb_dim(1), -aabb_dim(2));
   auto tnp = core::Vector(-aabb_dim(0), aabb_dim(1), aabb_dim(2));
   auto tnn = core::Vector(-aabb_dim(0), aabb_dim(1), -aabb_dim(2));

   auto bpp = core::Vector(aabb_dim(0), -aabb_dim(1), aabb_dim(2));
   auto bpn = core::Vector(aabb_dim(0), -aabb_dim(1), -aabb_dim(2));
   auto bnp = core::Vector(-aabb_dim(0), -aabb_dim(1), aabb_dim(2));
   auto bnn = core::Vector(-aabb_dim(0), -aabb_dim(1), -aabb_dim(2));

   auto bb_ptr = mImpl->mAABB.get();
   //top
   *bb_ptr++ = tpp(0);  *bb_ptr++ = tpp(1);  *bb_ptr++ = tpp(2);
   *bb_ptr++ = tpn(0);  *bb_ptr++ = tpn(1);  *bb_ptr++ = tpn(2);

   *bb_ptr++ = tpp(0);  *bb_ptr++ = tpp(1);  *bb_ptr++ = tpp(2);
   *bb_ptr++ = tnp(0);  *bb_ptr++ = tnp(1);  *bb_ptr++ = tnp(2);

   *bb_ptr++ = tnn(0);  *bb_ptr++ = tnn(1);  *bb_ptr++ = tnn(2);
   *bb_ptr++ = tnp(0);  *bb_ptr++ = tnp(1);  *bb_ptr++ = tnp(2);

   *bb_ptr++ = tnn(0);  *bb_ptr++ = tnn(1);  *bb_ptr++ = tnn(2);
   *bb_ptr++ = tpn(0);  *bb_ptr++ = tpn(1);  *bb_ptr++ = tpn(2);

   //bottom
   *bb_ptr++ = bpp(0);  *bb_ptr++ = bpp(1);  *bb_ptr++ = bpp(2);
   *bb_ptr++ = bpn(0);  *bb_ptr++ = bpn(1);  *bb_ptr++ = bpn(2);

   *bb_ptr++ = bpp(0);  *bb_ptr++ = bpp(1);  *bb_ptr++ = bpp(2);
   *bb_ptr++ = bnp(0);  *bb_ptr++ = bnp(1);  *bb_ptr++ = bnp(2);

   *bb_ptr++ = bnn(0);  *bb_ptr++ = bnn(1);  *bb_ptr++ = bnn(2);
   *bb_ptr++ = bnp(0);  *bb_ptr++ = bnp(1);  *bb_ptr++ = bnp(2);

   *bb_ptr++ = bnn(0);  *bb_ptr++ = bnn(1);  *bb_ptr++ = bnn(2);
   *bb_ptr++ = bpn(0);  *bb_ptr++ = bpn(1);  *bb_ptr++ = bpn(2);

   //sides
   *bb_ptr++ = tpp(0);  *bb_ptr++ = tpp(1);  *bb_ptr++ = tpp(2);
   *bb_ptr++ = bpp(0);  *bb_ptr++ = bpp(1);  *bb_ptr++ = bpp(2);

   *bb_ptr++ = tpn(0);  *bb_ptr++ = tpn(1);  *bb_ptr++ = tpn(2);
   *bb_ptr++ = bpn(0);  *bb_ptr++ = bpn(1);  *bb_ptr++ = bpn(2);

   *bb_ptr++ = tnp(0);  *bb_ptr++ = tnp(1);  *bb_ptr++ = tnp(2);
   *bb_ptr++ = bnp(0);  *bb_ptr++ = bnp(1);  *bb_ptr++ = bnp(2);

   *bb_ptr++ = tnn(0);  *bb_ptr++ = tnn(1);  *bb_ptr++ = tnn(2);
   *bb_ptr++ = bnn(0);  *bb_ptr++ = bnn(1);  *bb_ptr++ = bnn(2);

   ////LOGE("FanLog SCVEupdateBoundingBox :X");
}

void SurfaceSplattingDriver::SCVEupdateKeyFrames(KeyFrameDisplay *& kfi, std::vector<KeyFrameDisplay *>& kfis)
{
   ////LOGE("FanLog SCVEupdateKeyFrames :E");
   //grab pointers to frame data
   auto& current_frame_info = kfi;
   auto& key_frame_infos = kfis;
   current_frame_info = nullptr;

   std::lock_guard<std::mutex> lock(mImpl->mAccessSRH);
   current_frame_info        = pKFD;
   int32_t number_key_frames = mKF.size();

   for(int32_t i = 0; i < number_key_frames; ++i)
      key_frame_infos.push_back(mKF[i].get());

   //update each keyframe's camera orientation and position
   if(nullptr == mImpl->mKeyFrames) {
      mImpl->mKeyFrames = new std::vector<std::unique_ptr<graphics::Mesh>>;
   }
   ////LOGE("FanLog SCVEupdateKeyFrames Keyframe mesh size %d", mImpl->mKeyFrames ->size());
   ////LOGE("FanLog SCVEupdateKeyFrames key frame ino size %d", key_frame_infos.size());
   for(int32_t i = 0; i < key_frame_infos.size(); ++i) {
      auto kf_ptr = key_frame_infos[i];

      //allocate GPU memory once
      if(!kf_ptr->isGenTexture) {
         kf_ptr->isGenTexture = true;
         uint64_t n_valids;
         std::unique_ptr<float[]> positions, normals;
         std::unique_ptr<uint8_t[]> colors;
         SCVEextract_valid_points(*kf_ptr, n_valids, positions, normals, colors);

         resource::Model points;
         points.load(n_valids, {
            std::forward_as_tuple(resource::Model::AttributeType::F32, 12,
                             reinterpret_cast<uintptr_t>(positions.get())),
            std::forward_as_tuple(resource::Model::AttributeType::F32, 12,
                             reinterpret_cast<uintptr_t>(normals.get())),
            std::forward_as_tuple(resource::Model::AttributeType::UI8, 3,
                             reinterpret_cast<uintptr_t>(colors.get())),
         });
         mImpl->mKeyFrames->push_back(
            std::move(std::unique_ptr<graphics::Mesh>(new graphics::Mesh(points))));
      }
   }

   //update current frame's point cloud
   resource::Model points;
   if(nullptr != current_frame_info &&
      (Stage::BeforeScan == getStage() || Stage::Scanning == getStage()))
   {


      uint32_t img_w = current_frame_info->width;
      uint32_t img_h = current_frame_info->height;
      if(Stage::BeforeScan == getStage()) {
         auto position = reinterpret_cast<uintptr_t>(
                                          current_frame_info ->Pts.get());
         auto color = reinterpret_cast<uintptr_t>(
                                        current_frame_info->RGB.get());
         ////LOGE("FanLog SCVEupdateKeyFrames width %d height %d ", img_w, img_h);
         points.load(img_w * img_h, {
            std::forward_as_tuple(resource::Model::AttributeType::F32, 12, position),
            std::forward_as_tuple(resource::Model::AttributeType::UI8, 3, color),
         });
         ////LOGE("FanLog SCVEupdateKeyFrames points getVertexSize %d ", points.getVertexSize());
         ////LOGE("FanLog SCVEupdateKeyFrames points getNumberAttributes %d ", points.getNumberAttributes());
         ////LOGE("FanLog SCVEupdateKeyFrames points getNumberPartitions %d ", points.getNumberPartitions());

         //allocate up front once
         if(nullptr == mImpl->mStreamedPoints) mImpl->mStreamedPoints.reset(new graphics::Mesh(points));
         else mImpl->mStreamedPoints->update(points);
      }

      //update orientation and position of camera during preview and scan
      float *Mat = current_frame_info->CameraMatrix.get();

      double R[9];
      double T[3];

      for(uint32_t i = 0; i < 3; ++i) {
         for(uint32_t j = 0; j < 3; ++j) {
            R[i*3 + j] = double(Mat[j * 4 + i]);
            ////LOGE("FanLog SCVEupdateKeyFrames R matrix %d %d,%f ", i,j,R(i,j));
         }
      }
      T[0] = Mat[12];
      T[1] = Mat[13];
      T[2] = Mat[14];
      ////LOGE("FanLog SCVEupdateKeyFrames T matrix %f %f,%f ", T[0],T[1],T[2]);

      //use camera pose as GL camera
      double quat[4];

      //MathUtility::R2q(R, quat);
      R2q(R, quat);

      core::Quaternion q0(core::Vector(1, 0, 0), 180 *M_PI / 180.0f);
      core::Quaternion q1({static_cast<float>(quat[0]),
         static_cast<float>(quat[1]),
         static_cast<float>(quat[2]),
         static_cast<float>(quat[3])
      });
      mImpl->mCamera->setOrientation(normalize(q1 * q0));
      mImpl->mCamera->setPosition({
         static_cast<float>(T[0]),
         static_cast<float>(T[1]),
         static_cast<float>(T[2])
      });
   }
   ////LOGE("FanLog SCVEupdateKeyFrames :X");
}

void SurfaceSplattingDriver::renderBackgroundGradient() {

    graphics::Device dev;
    dev.set(mImpl->mPrograms[7]);

    auto h = static_cast<float>(dev.getDefaultWindowFrameHeight());
    mImpl->mPrograms[7].update("gCS_Height", h);

    const GLfloat fullscreenVertices[] = { -1.0f, -1.0f,
                                            1.0f, 1.0f,
                                           -1.0f, 1.0f,
                                           -1.0f,-1.0f,
                                            1.0f,-1.0f,
                                            1.0f, 1.0f};

     glEnableVertexAttribArray(0);
    core::Vector color1 = core::Vector(0.6f, 0.6f, 0.6f); //gray
    core::Vector color0 = core::Vector(0.1f, 0.1f, 0.1f); //dark gray
    mImpl->mPrograms[7].update("gCS_GradCol0",color0);
    mImpl->mPrograms[7].update("gCS_GradCol1",color1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, fullscreenVertices);
    dev.dispatch(graphics::Device::Primitive::Triangle, 2);
    glDisableVertexAttribArray(0);

    mImpl->mFrames[0].clearDepth();
}

void SurfaceSplattingDriver::SCVEvisualizeCurrentKeyFrameImage(KeyFrameDisplay *kfi)
{
   ////LOGE("FanLog SCVEvisualizeCurrentKeyFrameImage :E");
   if(nullptr == kfi) return;

   uint32_t img_w = kfi ->width;
   uint32_t img_h = kfi ->height;
   if(nullptr == mImpl->mBackgroundImage) {
      mImpl->mBackgroundImage.reset(new graphics::Texture);
      graphics::Texture::Descriptor tex_desc;
      tex_desc.width = img_w;
      tex_desc.height = img_h;
      tex_desc.format = graphics::Texture::Format::RGB8;
      mImpl->mBackgroundImage->load(tex_desc);
   }

   mImpl->mBackgroundImage->bind();
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img_w, img_h,
               GL_RGB, GL_UNSIGNED_BYTE, kfi->RGB.get());
   mImpl->mBackgroundImage->unbind();

   //display camera image for exposure calibration
   graphics::Device dev;
   dev.set(mImpl->mPrograms[6]);
   dev.set({
      std::forward_as_tuple(mImpl->mSamplers[1], *mImpl->mBackgroundImage),
   });
   const GLfloat fullscreenVertices[] = { 0.0f, 2.0f, 0.0f, 0.0f, 2.0f, 0.0f };
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, fullscreenVertices);

   dev.dispatch(graphics::Device::Primitive::Triangle, 1);
   glDisableVertexAttribArray(0);
   ////LOGE("FanLog SCVEvisualizeCurrentKeyFrameImage :X");
}

void SurfaceSplattingDriver::SCVEvisualizeKeyFrames(const std::vector<KeyFrameDisplay *>& kfis)
{
    //LOGE("SCVEvisualizeKeyFrames :E");

   graphics::Device dev;
   const int32_t kNumberKeyFrames = (nullptr == mImpl->mKeyFrames) ? 0 :
                                                     mImpl->mKeyFrames->size();
   ////LOGE("FanLog SCVEvisualizeKeyFrames %d ", kNumberKeyFrames);
   if(RenderMode::Dilation == getRenderMode()) {
      dev.set(mImpl->mPrograms[2]);

      for(int32_t i = 0; i < kNumberKeyFrames; ++i) {
         ////LOGE("FanLog SCVEvisualizeKeyFrames Update Key Frame %d",i);
         auto kf_ptr = kfis[i];
         float *Mat = kf_ptr->CameraMatrix.get();
         ////LOGE("FanLog SCVEvisualizeKeyFrames after get");
         /*core::Matrix model_matrix(Mat[0],Mat[1],Mat[2],Mat[3],
                             Mat[4],Mat[5],Mat[6],Mat[7],
                             Mat[8],Mat[9],Mat[10],Mat[11],
                             Mat[12],Mat[13],Mat[14],Mat[15]);*/
         core::Matrix model_matrix(Mat[0],Mat[4],Mat[8],Mat[12],
                             Mat[1],Mat[5],Mat[9],Mat[13],
                             Mat[2],Mat[6],Mat[10],Mat[14],
                             Mat[3],Mat[7],Mat[11],Mat[15]);
         ////LOGE("FanLog SCVEvisualizeKeyFrames before update");
         mImpl->mPrograms[2].update("gMS_model", model_matrix);
         ////LOGE("FanLog SCVEvisualizeKeyFrames  after update");
         (*mImpl->mKeyFrames)[i]->draw();
         ////LOGE("FanLog SCVEvisualizeKeyFrames after draw");
      }
      ////LOGE("FanLog SCVEvisualizeKeyFrames :X");
      return;
   }

   //visibility
   dev.toggleColorWrite(false);
   dev.set(mImpl->mPrograms[3]);
   for(int32_t i = 0; i < kNumberKeyFrames; ++i) {
      auto kf_ptr = kfis[i];
      float *Mat = kf_ptr->CameraMatrix.get();
      core::Matrix model_matrix(Mat[0],Mat[4],Mat[8],Mat[12],
                             Mat[1],Mat[5],Mat[9],Mat[13],
                             Mat[2],Mat[6],Mat[10],Mat[14],
                             Mat[3],Mat[7],Mat[11],Mat[15]);
      mImpl->mPrograms[3].update("gMS_model", model_matrix);
      (*mImpl->mKeyFrames)[i]->draw();
   }
   dev.toggleColorWrite(true);

   //attribute accumulation
   dev.toggleBlending(true);
   dev.toggleDepthWrite(false);

   graphics::Blending blending_state;
   blending_state.alpha = graphics::Blending::Equation::Add;
   blending_state.rgb = graphics::Blending::Equation::Add;
   blending_state.sinkAlpha = graphics::Blending::Factor::One;
   blending_state.sourceAlpha = graphics::Blending::Factor::One;
   blending_state.sinkRGB = graphics::Blending::Factor::One;
   blending_state.sourceRGB = graphics::Blending::Factor::SourceAlpha;
   dev.set(blending_state);

   dev.set(mImpl->mPrograms[4]);
   for(int32_t i = 0; i < kNumberKeyFrames; ++i) {
      auto kf_ptr = kfis[i];
      float *Mat = kf_ptr->CameraMatrix.get();
      core::Matrix model_matrix(Mat[0],Mat[4],Mat[8],Mat[12],
                             Mat[1],Mat[5],Mat[9],Mat[13],
                             Mat[2],Mat[6],Mat[10],Mat[14],
                             Mat[3],Mat[7],Mat[11],Mat[15]);
      mImpl->mPrograms[4].update("gMS_model", model_matrix);
      (*mImpl->mKeyFrames)[i]->draw();
   }

   dev.toggleBlending(false);
   dev.toggleDepthWrite(true);

   //LOGE("SCVEvisualizeKeyFrames :X");
}

void SurfaceSplattingDriver::SCVEvisualizePoints(KeyFrameDisplay *kfi)
{
   ////LOGE("FanLog SCVEvisualizePoints :E");
   if(nullptr == mImpl->mStreamedPoints || nullptr == kfi) return;

   graphics::Device dev;
   dev.set(mImpl->mPrograms[0]);

   //update point cloud visualization model matrix
   float *Mat = kfi->CameraMatrix.get();
   core::Matrix mm(Mat[0],Mat[4],Mat[8],Mat[12],
                             Mat[1],Mat[5],Mat[9],Mat[13],
                             Mat[2],Mat[6],Mat[10],Mat[14],
                             Mat[3],Mat[7],Mat[11],Mat[15]);
   mImpl->mPrograms[0].update("gMS_model", mm);

   mImpl->mStreamedPoints->draw();
   ////LOGE("FanLog SCVEvisualizePoints :X");
}

void SurfaceSplattingDriver::SCVEextract_valid_points(const KeyFrameDisplay &kf,
    uint64_t &n_valids, std::unique_ptr<float[]> &positions,
    std::unique_ptr<float[]> &normals, std::unique_ptr<uint8_t[]> &colors)
{
   ////LOGE("FanLog SCVEextract_valid_points :E");
   struct PixelFloat {
      float x, y, z;
   };
   struct PixelColor {
      uint8_t r, g, b;
   };
   uint32_t img_w = kf.width;
   uint32_t img_h = kf.height;
   ////LOGE("FanLog SCVEextract_valid_points KF pointer %x",&kf);
   ////LOGE("FanLog SCVEextract_valid_points width %d height %d",img_w, img_h);

   //convert to struct to make it easier to iterate over each pixel
   const PixelFloat *position_src_ptr =
      reinterpret_cast<const PixelFloat *>(kf.Pts.get());
   const PixelFloat *normal_src_ptr =
      reinterpret_cast<const PixelFloat *>(kf.Norms.get());
   const PixelColor *color_src_ptr =
      reinterpret_cast<const PixelColor *>(kf.RGB.get());

   //allocate buffers
   positions.reset(new float[img_w * img_h * 3]);
   normals.reset(new float[img_w * img_h * 3]);
   colors.reset(new uint8_t[img_w * img_h * 3]);
   PixelFloat *position_dst_ptr =
      reinterpret_cast<PixelFloat *>(positions.get());
   PixelFloat *normal_dst_ptr =
      reinterpret_cast<PixelFloat *>(normals.get());
   PixelColor *color_dst_ptr =
      reinterpret_cast<PixelColor *>(colors.get());


   ///VALID points mask

   //extract valid points
   const float kSceneRange = 2.5f;
   n_valids = 0;
   for(uint32_t y = 0; y < img_w; ++y) {
      for(uint32_t x = 0; x < img_h; ++x) {

          if(position_src_ptr->z > 0 && position_src_ptr->z < kSceneRange) {

            position_dst_ptr->x = position_src_ptr->x;
            position_dst_ptr->y = position_src_ptr->y;
            position_dst_ptr->z = position_src_ptr->z;
            normal_dst_ptr->x = normal_src_ptr->x;
            normal_dst_ptr->y = normal_src_ptr->y;
            normal_dst_ptr->z = normal_src_ptr->z;
            color_dst_ptr->r = color_src_ptr->r;
            color_dst_ptr->g = color_src_ptr->g;
            color_dst_ptr->b = color_src_ptr->b;
            ++n_valids;
            ++position_dst_ptr;
            ++normal_dst_ptr;
            ++color_dst_ptr;
         }
         ++position_src_ptr;
         ++normal_src_ptr;
         ++color_src_ptr;
      }
   }
   ////LOGE("FanLog SCVEextract_valid_points :X");
}


void SurfaceSplattingDriver::SCVErender(bool update)
{

  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mImpl->m_destinationFrameBuffer);

  mImpl->mAngleX = m_currentAngleX;
  mImpl->mAngleY = m_currentAngleY;

  //update all parameters related to ViewController
  mImpl->mZoom[mImpl->mInputMode] = m_currentZoom;

  mImpl->mHorizontalOffset[mImpl->mInputMode] = m_currentTrackHoriz;
  mImpl->mVerticalOffset[mImpl->mInputMode] = m_currentTrackVert;

  //update GUI-related parameters
  mImpl->mAxisOffset(0) = mImpl->mHorizontalOffset[0];
  mImpl->mAxisOffset(1) = mImpl->mVerticalOffset[0];
  mImpl->mTargetRadius = std::abs(mImpl->mZoom[0]);

  //first setting
  if (!mImpl->BB_size_changed)
     mImpl->mBoundingBoxDimension = 1.0f / (std::abs(mImpl->mZoom[1]));

/*
  if(Stage::CameraPreview == getStage()) {
    LOGISCVE("visualizeCameraInput()");
    visualizeCameraInput();
    return;
  }
*/
  //update keyframes and extract poses
  KeyFrameDisplay *kfi;
  //KF kf_infos;
  std::vector<KeyFrameDisplay *> kf_infos;

  if (update) {
    SCVEupdateKeyFrames(kfi, kf_infos); //Slam info here
  }

  //Moving to SCVE

  SCVEupdateBoundingBox(kfi);
  updateShaderParameters();

  //make sure viewport is set to fullscreen
  graphics::Device dev;
  graphics::Viewport vps[1];
  vps[0].width = dev.getDefaultWindowFrameWidth();
  vps[0].height = dev.getDefaultWindowFrameHeight();
  dev.set(1, vps);

  //clear framebuffer
  mImpl->mFrames[0].bind();

  if (update)
    mImpl->mFrames[0].clearColor();

  mImpl->mFrames[0].clearDepth();
  mImpl->mFrames[0].unbind();

  dev.set(mImpl->mFrames[0]);

  if (update)
     renderBackgroundGradient();


  //superimpose background
  if(false && Stage::Scanning == getStage())
    SCVEvisualizeCurrentKeyFrameImage(kfi);

  dev.toggleDepthTest(true);

  //visualize points
  if (update)
    SCVEvisualizeKeyFrames(kf_infos);

  if(Stage::BeforeScan == getStage())
    SCVEvisualizePoints(kfi);

  if(Stage::BeforeScan == getStage() || Stage::AfterScan == getStage())
  //if(Stage::AfterScan == getStage())
    visualizeBoundingBox();

  //normalize pixels and copy to device's screen
  dev.toggleDepthTest(false);
  copyToBackBuffer();

  //artifact required for ViewController
  ViewerUpdateTablet();


}


