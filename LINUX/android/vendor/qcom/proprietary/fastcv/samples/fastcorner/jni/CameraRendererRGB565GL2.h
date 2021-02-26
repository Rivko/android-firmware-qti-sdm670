
/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 *
 * @file Defines CameraRendererRGB565GL2 class.
 *         This class provides efficeint rendering
 *       of a camera image.
 *
 */


#ifndef __CAMERARENDERER_RGB565_GL2_HEADERFILE__
#define __CAMERARENDERER_RGB565_GL2_HEADERFILE__


#include <stdint.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "FPSCounter.h"

class CameraBuffer;

/**
 * @brief CameraRendererRGB565GL2 class provide functionality of
 *        rendering the camera image to the screen.
 *
 * CameraRendererRGB565GL2 support multiple modes for rendering.
 * If the user is using any of the GPU based rendering mode,
 * CameraRendererRGB565GL2 requires the caller to setup the
 * OpenGL ES 2.0 context and set up the OpenGL view port in
 * which to draw the camera image.
 *
 * Usage: Usage of CameraRender is very simple as shown below.
 *
 * //CameraRendererRGB565GL2 object needs to be created during
 * //initialization time of your application.
 *
 * CameraRendererRGB565GL2 camRenderer();
 *
 * //In your OpenGL Rendering loop please call following
 * //function.
 *
 * camRenderer.Render(img, w,h);
 *
 */
class CameraRendererRGB565GL2
{

public:
   /**
    * @brief Basic Constructor
    *
    * @param npotTextures Indicate if this renderer is to use
    *                     non-power-of-two-textures or not. Default
    *                     value is false.
    **/
   CameraRendererRGB565GL2(bool npotTextures = false);

   /**
    * @brief Renders provided camera image in NV21 format in
    *        current viewport.
    *
    * @param img Input image in NV21 format.
    * @param w Width of input image.
    * @param h Height of input image.
    *
    * @return true if rendering was successfull false otherwise.
    */
   bool Render
   (
      const uint8_t* img,
      uint16_t w,
      uint16_t h
   );


   /**
    * @brief Returns GPU Rendering FPS for Camera frames.
    *
    * @return Frames per Second.
    *
    */
   float GetRenderingFPS();


   /**
    * @brief Initializes the object.
    *
    * @return true if the object is initialized, false otherwise.
    *
   */
   bool Init();

   /**
    * @brief UnInitializes the object.
    *
    * Releases any graphics resources held such as shader programs
    * and textures.
    *
   */
   void UnInit();


   /**
    * @brief Default desctructor
    *
    */
   virtual ~CameraRendererRGB565GL2();

protected:

   /**
    * Flag indicating initialization status
    */
   bool           mInitialized;

   /**
    * Flag indicating non-power-of-two texture support
    */
   bool          mNPOTTextures;

   /**
    * Compiled and Linked Shader Program object.
    */
   GLuint         mProgramObject;

   /**
    * Vertex Array used to define Quad for drawing and its texture
    * mapping.
    */
   static GLfloat mVertices[];

   /**
    * Reference vertex array for landscape rendering
    */
   static const GLfloat mLandscapeVertices[];

   /**
    * Reference vertex array for reverse landscape rendering
    */
   static const GLfloat mLandscapeReverseVertices[];

   /**
    * Reference vertex array for portrait rendering
    */
   static const GLfloat mPortraitVertices[];

   /**
    * Reference vertex array for reverse portrait rendering
    */
   static const GLfloat mPortraitReverseVertices[];

   /**
    * Location for OpenGL shader attribute for quad vertices.
    */
   GLint          mGL_av4_PositionLoc;

   /**
    * Location for OpenGL shader attribute for quad texture
    * mapping.
    */
   GLint          mGL_av2_TexCoordLoc;

   /**
    * Array for texture ids. Either one or two of these array will
    * be used depending rendering mode.
    */
   GLuint         mTextureId[1];  // texture ID's

   /**
    * Location for OpenGL shader uniform for RGB Image sampler.
    */
   GLint          mGL_u_ImgRGBLoc;

   /**
    * FPS counter for Camera frame rendering.
    */
   FPSCounter        mFPSCounter;

   /**
    * Width
    */
   GLuint         mWidth;

   /**
    * Height
    */
   GLuint         mHeight;

   /**
    * @brief Creating 2D texture for an RGB565 image.
    *
    * @return returns true if successfull false otherwise.
    *
   */
   bool CreateTextures();


   /**
    * @brief Updates texture with new input image.
    *
    * @param rgbImg input image in RGB565 format
    * @param w Width of input image.
    * @param h Height of input image.
    *
    * @return returns true if successfull false otherwise.
    *
   */
   bool UpdateTextures(const uint8_t* rgbImg, uint16_t w, uint16_t h);


   /**
    * @brief Creates shader program required to render yuv texture.
    *
    * @return returns true if successfull false otherwise.
    *
   */
   bool CreateShaderProgram();


   /**
    * @brief Initializes the object.
    *
    * @return true if the object is initialized, false otherwise.
    *
   */
   bool InitGPU();

   /**
    * @brief Renders texture in current viewport by provide RGB
    *        image to GPU.
    *
    * @param rgbImg Input image in YUV420SP format.
    * @param w Width of input image.
    * @param h Height of input image.
    *
    * @return true if the rendering was successfull and false
    *         otherwise. If false see adb log for error messages
    *         from this class.
    *
    */
   bool RenderGPURGB(const uint8_t* rgbImg, uint16_t w, uint16_t h);

}; // class CameraRendererRGB565GL2

#endif //#ifndef __CAMERARENDERER_RGB565_GL2_HEADERFILE__
