/*
* Copyright (c) 2011 Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Technologies Confidential and Proprietary.
* Modifications to this file developed by the FastCV Engineering Team.
* Not a Contribution, Apache license notifications and license are retained
* for attribution purposes only.
*/
/*
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/


package com.qualcomm.fastcorner;

import android.content.Context;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.EGLContextFactory;

/**
 * @brief The FastCV sample application's renderer, provides
 *        interface to the native renderer
 *
 */
public class FastCVSampleRenderer
   implements GLSurfaceView.Renderer, EGLContextFactory
{
   /** GL Surface for rendering */
   private GLSurfaceView         mSurface;

   /** Logging tag */
   private static final String   TAG               = "FastCVSampleRenderer";

   /**
    * Constructor
    * @param context
    */
   public FastCVSampleRenderer( Context context )
   {
      mSurface = new GLSurfaceView(context);
      mSurface.setEGLContextFactory(this);
      mSurface.setEGLConfigChooser(new ConfigChooser(5, 6, 5, 0, 16, 0));
      mSurface.setRenderer(this);
   }

   /**
    * Returns reference to Renderer's GLSurfaceView.
    *
    * @return Reference to GLSurfaceView
    */
   public GLSurfaceView getGLSurfaceView()
   {
      return mSurface;
   }

   /**
    * Handles Renderer window resize
    */
   public native void surfaceChanged(int w, int h);

   /**
    * Render function invoked on GLSurfaceView::requestRender()
    */
   public native void render();

   /**
    * Performs Renderer cleanup when GL surface is destroyed
    */
   public native void cleanup();

   public native float getGPUFPS();

   /**
    * GL surface created
    * @param gl
    * @param config
    */
   public void onSurfaceCreated( GL10 gl, EGLConfig config )
   {
   }

   /**
    * GL surface changed
    * @param gl
    * @param w
    * @param h
    */
   public void onSurfaceChanged(GL10 gl, int w, int h)
   {
       surfaceChanged(w, h);
   }

   /**
    * On draw frame thread - invoked by requestRender()
    * @param gl
    */
   public void onDrawFrame(GL10 gl)
   {
       render();
   }

   /**
    * GL context created
    * @param egl
    * @param display
    * @param eglConfig
    *
    * @return EGLContext
    */
   public EGLContext createContext(EGL10 egl, EGLDisplay display,
          EGLConfig eglConfig)
   {

       final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
       int[] attr = {
           EGL_CONTEXT_CLIENT_VERSION, 2,
           EGL10.EGL_NONE
       };

       return egl.eglCreateContext(display, eglConfig,EGL10.EGL_NO_CONTEXT, attr);
   }

   /**
    * GL context destroyed
    * @param egl
    * @param display
    * @param context
    */
   public void destroyContext(EGL10 egl, EGLDisplay display,
          EGLContext context)
   {
       cleanup();
       egl.eglDestroyContext(display, context);
   }

   private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
   {
     /**
     * Sets the configuration parameters
     * @param r
     * @param g
     * @param b
     * @param a
     * @param depth
     * @param stencil
     */
      public ConfigChooser(int r, int g, int b, int a, int depth, int stencil)
      {
         mRedSize = r;
         mGreenSize = g;
         mBlueSize = b;
         mAlphaSize = a;
         mDepthSize = depth;
         mStencilSize = stencil;
      }

     /**
     * getMatchingConfig
     * @param egl
     * @param display
     * @param configAtribs
     * @return Returns the best config
     */
      private EGLConfig getMatchingConfig(EGL10 egl, EGLDisplay display,
            int[] configAttribs)
      {
         // get the number of minimally matching EGL configurations
         //
         int[] num_config = new int[1];
         egl.eglChooseConfig(display, configAttribs, null, 0, num_config);

         int numConfigs = num_config[0];
         if (numConfigs <= 0)
             throw new IllegalArgumentException("No matching EGL configs");

         // allocate then read the array of minimally matching EGL configs
         //
         EGLConfig[] configs = new EGLConfig[numConfigs];
         egl.eglChooseConfig(display, configAttribs, configs, numConfigs,
             num_config);

         // dump EGL configs for debugging purposes
         //
         //printConfigs(egl, display, configs);

         // now return the "best" one
         //
         return chooseConfig(egl, display, configs);
      }

     /**
     * Chooseconfig Gets the matching configuration
     * @param egl
     * @param display
     * @return Returns the matching configuration
     */
      public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
      {
         // This EGL config specification is used to specify 2.0 rendering.
         // We use a minimum size of 4 bits for red/green/blue, but will
         // perform actual matching in chooseConfig() below.
         //
         final int EGL_OPENGL_ES2_BIT = 0x0004;
         final int[] s_configAttribs_gl20 =
         {
             EGL10.EGL_RED_SIZE, 4,
             EGL10.EGL_GREEN_SIZE, 4,
             EGL10.EGL_BLUE_SIZE, 4,
             EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
             EGL10.EGL_NONE
         };

         return getMatchingConfig(egl, display, s_configAttribs_gl20);
      }

     /**
     * Chooseconfig Gets the matching configuration
     * @param egl
     * @param display
     * @return Returns the matching configuration
     */
      public EGLConfig chooseConfig(
          EGL10 egl, EGLDisplay display, EGLConfig[] configs)
      {
         for(EGLConfig config : configs)
         {
            int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
            int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);

            // We need at least mDepthSize and mStencilSize bits
            //
            if (d < mDepthSize || s < mStencilSize)
               continue;

            // We want an *exact* match for red/green/blue/alpha
            //
            int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
            int g = findConfigAttrib(egl, display, config,
                            EGL10.EGL_GREEN_SIZE, 0);
            int b = findConfigAttrib(egl, display, config,
                            EGL10.EGL_BLUE_SIZE, 0);
            int a = findConfigAttrib(egl, display, config,
                            EGL10.EGL_ALPHA_SIZE, 0);

            if (r == mRedSize &&
                g == mGreenSize &&
                b == mBlueSize &&
                a == mAlphaSize)
                return config;
         }

         return null;
     }
     /**
     * findConfigAttrib Gets the matching configuration attributes
     * @param egl
     * @param display
     * @param config
     * @param attribute
     * @param defaultValue
     * @return Returns the attribute value
     */
     private int findConfigAttrib(
         EGL10 egl, EGLDisplay display, EGLConfig config, int attribute,
         int defaultValue)
     {
        if (egl.eglGetConfigAttrib(display, config, attribute, mValue))
             return mValue[0];

        return defaultValue;
     }




     // Subclasses can adjust these values:
     //
     protected int mRedSize;
     protected int mGreenSize;
     protected int mBlueSize;
     protected int mAlphaSize;
     protected int mDepthSize;
     protected int mStencilSize;
     private int[] mValue = new int[1];
   }
}
