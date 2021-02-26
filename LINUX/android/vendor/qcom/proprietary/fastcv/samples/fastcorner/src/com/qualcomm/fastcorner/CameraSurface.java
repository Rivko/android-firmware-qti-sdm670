/*============================================================================
            Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
  ============================================================================*/

package com.qualcomm.fastcorner;

import android.content.Context;
import android.hardware.Camera;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.util.Log;

/**
 * CameraSurface class is implemented to adhere to the Android
 * SDK Camera API requirements.
 * http://developer.android.com/reference/android/hardware/Camera.html
 * "Important: Pass a fully initialized SurfaceHolder to
 * setPreviewDisplay(SurfaceHolder). Without a surface, the
 * camera will be unable to start the preview. "
 *
 */
class CameraSurface extends SurfaceView implements SurfaceHolder.Callback
{
   /** Reference to application's camera instance */
   Camera                        camera;

   /** Module's logging tag */
   private static final String   TAG = "CameraSurface";

   /**
    * CameraSurface constructor, perform basic initialization
    *
    * @param context Application's context
    * @param c Camera reference
    */
   public CameraSurface( Context context, Camera c )
   {
      super(context);

      camera = c;

      SurfaceHolder surfaceHolder = getHolder();
      surfaceHolder.addCallback(this);
      surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
   }

   /**
    * SurfaceCreated callback. Installs this as the surface holder
    * for camera.
    *
    * @param holder Reference to the holder
    */
   public void surfaceCreated( SurfaceHolder holder )
   {
      Log.d( TAG, "SurfaceCreated" );
      try
      {
         if( camera != null )
         {
            camera.setPreviewDisplay( holder );
         }
      }
      catch( Exception exception )
      {

      }
   }

   /**
    * SurfaceChanged callback, does nothing
    *
    * @param holder
    * @param format
    * @param width
    * @param height
    */
   public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
   {

   }

   /**
    * SurfaceDestroyed callback, does nothing
    *
    * @param holder
    */
   public void surfaceDestroyed(SurfaceHolder holder) {}
}
