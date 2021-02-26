/*==============================================================================
            Copyright (c) 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.loadjpeg;

import java.nio.ShortBuffer;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;

/** The main activity for the FastCV FAST Corner sample app. */
public class LoadJpeg extends Activity
{
   /** Logging tag */
   private static final String         TAG               = "LoadJpeg";


   static
   {
      // Load JNI library
      System.loadLibrary( "loadjpeg" );
   };

   /**
    * Called when the activity is first created.
    */
   @Override
   public void onCreate(Bundle savedInstanceState)
   {
      super.onCreate( savedInstanceState );
   }

   /**
    * Resumes camera preview
    */
   @Override
   protected void onResume()
   {
      super.onResume();

      loadJPEG( "/sdcard/test.jpg" );

   }

   /**
    * Loads a JPEG into luminance data
    * @param fileName
    */
   private void loadJPEG( String fileName )
   {
      Bitmap bmp = BitmapFactory.decodeFile( fileName );
      if( bmp != null && bmp.getConfig() == Bitmap.Config.RGB_565 ) {

         ShortBuffer rgbBuf =
            ShortBuffer.allocate( bmp.getWidth()*bmp.getHeight() );

         bmp.copyPixelsToBuffer( rgbBuf );

         convertToLum( rgbBuf.array(), bmp.getWidth(), bmp.getHeight() );
      }
   }

   /**
    * Performs conversion from RGB565 to Luminance in native.
    * @param data ShortByte.array() data.
    * @param w Width of buffer
    * @param h Height of buffer
    */
   private native void convertToLum( short[] data, int w, int h );

}
