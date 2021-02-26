/*==============================================================================
            Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcorner;

import java.util.List;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.TextView;
import android.view.Surface;
import android.widget.ToggleButton;

/** The main activity for the FastCV FAST Corner sample app. */
public class FastCVSample extends Activity
{
   /** Logging tag */
   private static final String         TAG               = "FastCVSample";

   /** Android camera object */
   private Camera                      mCamera;

   /** GL Surface view to render camera and corners via GPU */
   private GLSurfaceView               mGLSurface = null;

   /** Surface Holder for camera, Camera API requirement */
   private CameraSurface               mCamSurface = null;

   /** Root view group for all views associated with this
    *  activity */
   private ViewGroup                   mRootViewGroup = null;

   /** Java class associated with the camera and corner
    *  renderering */
   private FastCVSampleRenderer        mRenderer;

   /** Text boxes for on screen display of information */
   private TextView[]                  mText = new TextView[2];

   /** Android UI button to switch between start and stop processing */
   private ToggleButton                tbtnStartStop;

   /** Flag indicates whether or not preview is running. */
   private boolean                     mPreviewRunning   = false;

   /** Buffer for camera driver to store preview data, created
    *  statically so that it doesn't need to be re-allocated
    *  each frame. */
   private byte[]                      mPreviewBuffer    = null;

   /** Background color for text boxes. */
   private final int                   mBackgroundColor  = 0xFF728FCE; // ARGB
                                                                       //
   /** Text color for text boxes */
   private final int                   mTextColor        = 0xFFFFFFFF; // ARGB

   /** Filtered processing time   */
   private float                       mProcessTime      = 10;

   /** Width to try and configure camera preview */
   private static int                  mDesiredWidth     = 800;

   /** Height to try and configure camera preview */
   private static int                  mDesiredHeight    = 480;

   /** Actual width of camera preview image, can be different
    *  than desired if camera doesn't support desired width. */
   private int                         mPreviewWidth;

   /** Actual height of camera preview image, can be different
    *  than desired if camera doesn't support desired width. */
   private int                         mPreviewHeight;

   /** Shared preference value for FAST corner threashold */
   private static int                  mPrefThreshold = 5;

   /** Shared preference value for scaling prior to corner
    *  detection */
   private static int                  mPrefScale = 1;

   /** Shared preference value to enable/disable viewfinder */
   private static int                  mPrefViewFinderEnable = 1;

   /** Shared preference value to enable/disable pixel overlay */
   private static int                  mPrefEnablePixelOverlay = 0;

   /** Shared preference value to enable/disable gaussian
    *  blurring */
   private static int                  mPrefEnableGauss = 0;

   /** Preference options for threshold for Fast Corner Detector */
   private static final int[]          mPrefOptsFastThreshold = {5,10,20,40,80};

   /** Preference options for scaling for Fast Corner Detector */
   private static final int[]          mPrefOptsDownScale = {1,2,4,8,16};

   /**  Preference options for enabling/disabling viewfinder */
   private static final int[]          mPrefOptsViewFinder = {0, 1};

   // TODO: Why aren't these checks for booleans?
   /** Preference options for enabling/disabling corner overlay */
   private static final int[]          mPrefOptsEnableCornerOverlay = {0, 1};

   /** Preference options for enabling/disabling Gaussian Filter */
   private static final int[]          mPrefOptsEnableGaussianFilter = {0, 1};

   /** Handler for when to update profiling information. */
   private Handler                     mProfileHandler = new Handler();

   /** How long to wait between profiling updates. */
   private long                        mProfilePeriod = 300;

   static
   {
      // Load JNI library
      System.loadLibrary( "fastcvsample" );
   };

   /**
    * Camera callback to retrieve camera frames.
    *
    */
   private final Camera.PreviewCallback mCameraCallback =
      new Camera.PreviewCallback()
   {
      /**
       * Actual callback function for camera frames. Does per frame
       * processing.
       *
       * @param data buffer for preview data, in YUV420sp format.
       * @param c Camera object.
       */
      public void onPreviewFrame( byte[] data, Camera c )
      {

         // Increment FPS counter for camera.
         cameraFrameTick();

         // Perform processing on the camera preview data.
         update( data, mDesiredWidth, mDesiredHeight );

         // Simple IIR filter on time.
         mProcessTime = getFastCVProcessTime();

         if( c != null )
         {
            // with buffer requires addbuffer each callback frame.
            c.addCallbackBuffer( mPreviewBuffer );
            c.setPreviewCallbackWithBuffer( this );
         }

         // Mark dirty for render.
         requestRender();
      }
   };

   /**
    * Handler for FPS display. Want to do this periodically to not
    * waste CPU.
    */
   private Runnable mUpdateTimeTask = new Runnable()
   {
      /** Overridden run function to handle callback This updates
       *  the fps and number of corners detected for the user */
      public void run()
      {
         float camFPS = getCameraFPS();
         int numCorners =  getNumCorners();
         String message;

         // Setup next profiling run.
         mProfileHandler.postDelayed( this, mProfilePeriod );

         message = String.format( "%.3fCamFPS, ", camFPS );
         mText[0].setText( message + "Corners detected: " + numCorners );

         mText[ 1 ].setText
         (
            String.format( "Preview process time: %.2fms", mProcessTime )
         );

         // Only add bg to utilized text boxes for now.
         for( int i=0; i<mText.length; i++ )
         {
            if( mText[i].getText().length() > 0 )
            {
               mText[i].setBackgroundColor( mBackgroundColor );
            }
            else
            {
               mText[i].setBackgroundColor( 0x00 );
            }
         }


      }
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

      int         screenw;
      int         width;

      // Initialize UI
      setContentView( R.layout.main );

      setTitle( "Fast Corner Detection Sample Application" );
      setTitleColor( 0xFFFF5555 );

      init();

      screenw = ((WindowManager)this.getSystemService(
                     Context.WINDOW_SERVICE)).getDefaultDisplay().getWidth();

      mText[0] = (TextView) findViewById(R.id.text1);
      mText[1] = (TextView) findViewById(R.id.text2);

      mRootViewGroup = (ViewGroup)mText[0].getParent();


      width = (screenw * 1 / 2 );

      for( int i=0; i<mText.length; i++ )
      {
         mText[i].setTextColor( mTextColor );
         mText[i].setWidth( width );
         mText[i].setText( "" );
      }
      mText[0].setText( "" );

      updatePreferences();

      tbtnStartStop = (ToggleButton) findViewById(R.id.startStopSwitch);
      tbtnStartStop.setOnClickListener
      (
         new View.OnClickListener()
         {
            public void onClick( View view )
            {
                if (tbtnStartStop.isChecked())
                {
                    startPreview();
                }
                else
                {
                    stopPreview();
                }
            }
         }
      );

      mCamera = Camera.open();

      // Camera surface is required for Camera to send
      // preview frame callback on some devices.
      // The order this is added to the rootViewGroup matters to ensure it is
      // below GLSurface. GLSurface is actually used for rendering.

      // Further note, when splash screen is not used this should be added
      // after GLSurfaceView. Really tricky to get ordering correct.
      mCamSurface = new CameraSurface( this, mCamera );
      mRootViewGroup.addView( mCamSurface );

      // Initialize the renderer
      mRenderer = new FastCVSampleRenderer( this );
      mGLSurface = mRenderer.getGLSurfaceView();
      mGLSurface.setZOrderMediaOverlay(true);
      mGLSurface.setVisibility( View.VISIBLE );
      mGLSurface.setRenderMode( GLSurfaceView.RENDERMODE_WHEN_DIRTY );
      mRootViewGroup.addView( mGLSurface );

      bringControlsToFront();

      startPreview();

      System.gc();
   }

   /**
    * Retrieves shared preferences and places them in member
    * variables for use.
    */
   protected void updatePreferences()
   {
      // Retrieve Preferences
      SharedPreferences prefs =
         PreferenceManager.getDefaultSharedPreferences( getBaseContext() );

      // set the default camera resolution to WVGA


      // retrieves user preference to set the threshold for Fast
      // Corner Detector algorithm
      int resIndex =
         Integer.decode( prefs.getString( "camThreshold", "3" ) );
      if( resIndex < mPrefOptsFastThreshold.length )
      {
         Log.e( TAG, "Configured FastThreshold: " + resIndex );
         mPrefThreshold = mPrefOptsFastThreshold[ resIndex ];
         Log.e( TAG, "Configured Threshold: " +
                mPrefThreshold );

      }

      // retrieves user preference for scaling factor to be
      // applied to image before applying corner detection on it
      resIndex =
         Integer.decode( prefs.getString( "camScaler", "0" ) );
      if( resIndex < mPrefOptsDownScale.length )
      {
         Log.e( TAG, "Configured Scaling: " + resIndex );
         mPrefScale = mPrefOptsDownScale[ resIndex ];
         Log.e( TAG, "Configured Scale: " +
                mPrefScale );

      }

      // retrieves user's preference to enable or disable the
      // green pixel overlay to be applied where corners are
      // detected
      resIndex =
         Integer.decode( prefs.getString( "cornerOverlay", "1" ) );
      if( resIndex < mPrefOptsEnableCornerOverlay.length )
      {
         Log.e( TAG, "Configured Pixel Overlay: " + resIndex );
         mPrefEnablePixelOverlay = mPrefOptsEnableCornerOverlay[ resIndex ];
         Log.e( TAG, "Configured Scale: " + mPrefScale );

      }

      // retrieves user's preference to enable or disable the
      // camera preview, or display a blank canvas
      resIndex =
         Integer.decode( prefs.getString( "viewfinder", "0" ) );
      if( resIndex < mPrefOptsViewFinder.length )
      {
         Log.e( TAG, "Configured Viewfinder: " + resIndex );
         mPrefViewFinderEnable = mPrefOptsViewFinder[ resIndex ];
         Log.e( TAG, "Configured viewfinder: " + mPrefViewFinderEnable);
      }

      // retrieves user preference for gaussian filter to be
      // applied to image before applying corner detection on it
      resIndex =
         Integer.decode( prefs.getString( "gaussianEnable", "1" ) );
      if( resIndex < mPrefOptsEnableGaussianFilter.length )
      {
         Log.e( TAG, "Configured Scaling: " + resIndex );
         mPrefEnableGauss = mPrefOptsEnableGaussianFilter[ resIndex ];
         Log.e( TAG, "Configured Scale: " + mPrefScale );
      }

   }

   /**
    * Brings the controls to the foreground
    */
   protected void bringControlsToFront()
   {
      for( int i=0; i<mText.length; i++ )
      {
         mText[i].bringToFront();
      }

      tbtnStartStop.bringToFront();
   }

   /**
    * Suspends camera preview, cleans up the renderer
    */
   @Override
   protected void onPause()
   {
      super.onPause();
      stopPreview();
       if( mCamera != null )
      {
         mCamera.release();
         mCamera = null;
      }

      if( mRenderer != null )
      {
         mRenderer.cleanup();
      }

      if( mRootViewGroup != null )
      {
         mRootViewGroup.removeAllViews();
      }

      cleanup();

      // Do a full exit here. On some devices the views won't return
      // in proper order. Since there is no state, just exit the activity
      // and start over on next launch.
      // System.exit(0);
   }
   /**
    * Creates the options menu
    * @param menu
    */
   @Override
   public boolean onCreateOptionsMenu( Menu menu )
   {
      MenuInflater inflater = getMenuInflater();
      inflater.inflate( R.menu.menu, menu );
      return true;
   }

   /**
    * User Option selection menu
    * @param item The menu item
    */
   @Override
   public boolean onOptionsItemSelected( MenuItem item )
   {
      // Handle item selection
      switch( item.getItemId() )
      {
      case R.id.settings:
         Intent settingsActivity =
            new Intent( getBaseContext(), Preferences.class );
         startActivity( settingsActivity );

         return true;

      case R.id.about:
         Intent aboutActivity =
            new Intent( getBaseContext(), About.class );
         startActivity( aboutActivity );

         return true;

      default:
         return super.onOptionsItemSelected(item);
      }
   }

   /**
    * Sets up  camera configuration parameters
    */
   private void setupCamera()
   {
     /* By default the camera preview size will be the highest supported resolution.
     * In case one of the supported resolutions has a height of 480, this will always
     * be used in conjunction with the corresponding width i.e. a WVGA or size closest
     * to this (upto 480 height) will be supported.
     * To support higher resolutions e.g. on tablets a user configurable setting can
     * be provided at a later stage.
     */
      Camera.Parameters parameters = mCamera.getParameters();
      List<Camera.Size> sizes = mCamera.getParameters().getSupportedPreviewSizes();
      Camera.Size list = sizes.get(0);
      mDesiredWidth = list.width;
      mDesiredHeight = list.height;
      for (Camera.Size s: sizes)
      {   if (s.height == 480)
           {
             mDesiredWidth = s.width;
             mDesiredHeight = s.height;
             break;
           }
      }

      // Now that the size is known, set up the camera parameters and begin
      // the preview.
      parameters.setPreviewFrameRate( 30 );
      parameters.setPreviewSize( mDesiredWidth, mDesiredHeight );
      //
      //  Set Focus mode depending on what is supported. MODE_INFINITY is
      //  preferred mode.
      //
      List<String> supportedFocusModes = parameters.getSupportedFocusModes();
      if( supportedFocusModes!= null )
      {
         if( supportedFocusModes.contains
             (
                Camera.Parameters.FOCUS_MODE_INFINITY
             ) )
         {
            parameters.setFocusMode( Camera.Parameters.FOCUS_MODE_INFINITY );
            Log.v( TAG, "Set focus mode INFINITY" );
         }
         else if( supportedFocusModes.contains
                  (
                     Camera.Parameters.FOCUS_MODE_FIXED
                  ) )
         {
            parameters.setFocusMode( Camera.Parameters.FOCUS_MODE_FIXED );
            Log.v( TAG, "Set focus mode FIXED" );
         }
         else if( supportedFocusModes.contains
                  (
                     Camera.Parameters.FOCUS_MODE_AUTO
                  ) )
         {
            parameters.setFocusMode( Camera.Parameters.FOCUS_MODE_AUTO);
            Log.v( TAG, "Set focus mode AUTO" );
         }
      }
      //
      // Set White Balance to Auto if supported.
      //
      List<String> supportedWhiteBalance =
         parameters.getSupportedWhiteBalance();
      if( supportedWhiteBalance != null &&
          supportedWhiteBalance.contains
          (
             Camera.Parameters.WHITE_BALANCE_AUTO
          ) )
      {
         parameters.setWhiteBalance( Camera.Parameters.WHITE_BALANCE_AUTO );
         Log.v( TAG, "Set white balance AUTO" );
      }

      try
      {
         mCamera.setParameters( parameters );
      }
      catch( RuntimeException re )
      {
         //
         // NOTE that we shouldn't be here as we check where our specified
         // parameters are supported or not.
         //
         re.printStackTrace();
         Log.e( TAG,"Unable to set Camera Parameters" );
         Log.i( TAG,"Falling back to setting just the camera preview" );
         parameters = mCamera.getParameters();
         parameters.setPreviewSize( mDesiredWidth, mDesiredHeight );
         try
         {
            mCamera.setParameters( parameters );
         }
         catch( RuntimeException re2 )
         {
            re2.printStackTrace();
            Log.e( TAG, "Problem with camera configuration, unable to set "+
                   "Camera Parameters. Camera not available." );

         }
      }

      mPreviewWidth = mCamera.getParameters().getPreviewSize().width;
      mPreviewHeight = mCamera.getParameters().getPreviewSize().height;

      //
      // Perform FastCV example configurations as retrieved from application's
      // preferences
      //
      setScaling(mPrefScale);
      setFastCornerThreshold(mPrefThreshold);

      if(mPrefViewFinderEnable == 1)
         setViewFinderEnable(true);
      else
         setViewFinderEnable(false);

      if(mPrefEnablePixelOverlay == 1)
         setOverlayPixelsEnable(true);
      else
         setOverlayPixelsEnable(false);

      if(mPrefEnableGauss == 1)
         setGaussianEnable(true);
      else
         setGaussianEnable(false);
   }

   /**
    * Starts camera preview
    */
   private void startPreview()
   {
      if( mCamera != null )
      {
         setupCamera();
         setCallback();
         mCamera.startPreview();
         mPreviewRunning = true;

      }
      else
      {
         mPreviewRunning = false;
      }

      mProfileHandler.removeCallbacks( mUpdateTimeTask );
      mProfileHandler.postDelayed( mUpdateTimeTask, mProfilePeriod );
    }

   /**
    * Stops camera preview
    */
   private void stopPreview()
   {
      if ((mCamera != null) && (mPreviewRunning == true))
      {
         mCamera.stopPreview();
         mCamera.setPreviewCallback( null );
      }

      mProfileHandler.removeCallbacks( mUpdateTimeTask );

      mPreviewRunning = false;
      System.gc();
   }

   /**
    * Sets the Camera preview callback
    *
    */
   public void setCallback()
   {
      int bufferSize = 0;
      int pformat;
      int bitsPerPixel;

      pformat = mCamera.getParameters().getPreviewFormat();

      // Get pixel format information to compute buffer size.
      PixelFormat info = new PixelFormat();
      PixelFormat.getPixelFormatInfo( pformat, info );
      bitsPerPixel = info.bitsPerPixel;

      bufferSize = mPreviewWidth * mPreviewHeight * bitsPerPixel / 8;

      // Make sure buffer is deleted before creating a new one.
      mPreviewBuffer = null;

      // New preview buffer.
      mPreviewBuffer = new byte[ bufferSize + 4096 ];

      // with buffer requires addbuffer.
      mCamera.addCallbackBuffer( mPreviewBuffer );
      mCamera.setPreviewCallbackWithBuffer( mCameraCallback );
   }

   /** Native function declarations*/
   public native float getCameraFPS();

   /**
    * Initializes native processing
    */
   public native void init();

   /**
    * Sets the native FAST corner threshold.
    *
    * @param threshold Barrier threshold to FAST Corner algorithm.
    */
   private native void setFastCornerThreshold(int threshold);

   /**
    * Sets the desired scaling factor.
    *
    *
    * @param scaleFactor Desired scaling factor, 1 is no scaling.
    */
   private native void setScaling(int scaleFactor);

   /**
    * Enables caller to disable camera backdrop.
    *
    * @param enableViewFinder Flag to enable/disable camera
    *                         backdrop.
    */
   private native void setViewFinderEnable(boolean enableViewFinder);

   /**
    * Enables/disables overlaying of pixels.
    *
    * @param enableOverlayPixels Flag to enable/disable pixel
    *                            overlay.
    */
   private native void setOverlayPixelsEnable(boolean enableOverlayPixels);

   /**
    * Enables/disables blurring camera image before processing
    * corners.
    *
    * @param enableGaussian Flag enables/disables Gaussian
    *                       blurring.
    */
   private native void setGaussianEnable(boolean enableGaussian);

   /**
    * Retrieves the latest number of corners for debug purposes.
    *
    * @return int Number of corners.
    */
   private native int getNumCorners();

   /**
    * Increments camera frame counter to track FPS.
    *
    */
   private native void cameraFrameTick();

   /**
    * Function to pass camera frame for native, FastCV processing.
    * @param data Byte buffer for data.
    * @param w Width of data
    * @param h Height of data
    */
   public native void update( byte[] data, int w, int h );


   /** Performs native cleanup routines for application exit. */
   private native void cleanup();

   /**
    * Retrieves FastCV processing timing from native layer.
    *
    * @return FastCV processing timing, filtered, in ms.
    */
   private native float getFastCVProcessTime();

   /**
    * Request render for on demand rendering.
    */
   public void requestRender()
   {
      if( mGLSurface != null )
      {
         mGLSurface.requestRender();
      }
   }

}
