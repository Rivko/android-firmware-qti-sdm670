/*==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcvdemo.apis.featureDetection;

import com.qualcomm.fastcvdemo.R;
import com.qualcomm.fastcvdemo.base.GraphicalActivity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.MenuItem;

public class Corner extends GraphicalActivity{
    /** Shared preference value to enable/disable viewfinder */
    private static int                  mPrefViewFinderEnable = 1;

    /**  Preference options for enabling/disabling viewfinder */
    private static final int[]          mPrefOptsViewFinder = {0, 1};

    /** Shared preference value for FAST corner threshold */
    private static int                  mPrefThreshold = 5;

    /** Shared preference value for scaling prior to corner
     *  detection */
    private static int                  mPrefScale = 1;

    /** Shared preference value to enable/disable gaussian
     *  blurring */
    private static int                  mPrefEnableGauss = 0;

    /** Shared preference value to change detection function */
    private static int                  mPrefDetectionSwitch = 1;

    /** Preference options for threshold for Fast Corner Detector */
    private static final int[]          mPrefOptsFastThreshold = {5,10,20,40,80};

    /** Preference options for scaling for Fast Corner Detector */
    private static final int[]          mPrefOptsDownScale = {1,2,4,8};

    // TODO: Why aren't these checks for booleans?
    /** Preference options for enabling/disabling corner overlay */
    private static final int[]          mPrefOptsEnableCornerOverlay = {0, 1};

    /** Preference options for enabling/disabling Gaussian Filter */
    private static final int[]          mPrefOptsEnableGaussianFilter = {0, 1};

    /** Shared preference value to enable/disable pixel overlay */
    private static int                  mPrefEnablePixelOverlay = 0;

    /** Preference options for detection function for Fast Corner */
    private static final int[]          mPrefOptsDetectionSwitch = {1,2,3,4,5,6,7,8};

    /** Shared preference value to display Detection Function */
    private static String               detectionFunction = "Corner Detection Function";

    static
    {
        Log.v( TAG, "Corner: load fastcvFeatDetect library");
        System.loadLibrary( "fastcvFeatDetect" );
    }

    /** Function which retrieves title based on module used. */
    protected void initTitle(){
        title = "FastCV CornerDetection";
    }

    /**
     * Resumes camera preview
     */
    @Override
    protected void onResume()
    {
        Log.v( TAG, "Corner: onResume()");
        updatePreferences();
        super.onResume();
    }

    public boolean onOptionsItemSelected( MenuItem item )
    {
        // Handle item selection
        switch( item.getItemId() )
        {
        case R.id.settings:
            Intent settingsActivity =
                new Intent( getBaseContext(), com.qualcomm.fastcvdemo.apis.featureDetection.CornerPrefs.class );
            startActivity( settingsActivity );

            return true;

        case R.id.about:
            Intent aboutActivity =
                new Intent( getBaseContext(), com.qualcomm.fastcvdemo.apis.About.class );
            startActivity( aboutActivity );

            return true;

        default:
            return super.onOptionsItemSelected(item);
        }
    }

    protected Runnable mUpdateTimeTask = new Runnable()
    {
        /** Overridden run function to handle callback This updates
         *  the fps and number of corners detected for the user */
        public void run()
        {
            float camFPS = util.getCameraFPS();
            int numCorners =  getNumCorners();
            String message;

            // Setup next profiling run.
            mProfileHandler.postDelayed( this, mProfilePeriod );

            message = String.format( "%.3f CamFPS, ", camFPS );
            mText[0].setText( message + "Corners detected: " + numCorners );

            mText[ 1 ].setText
            (
                String.format( "Preview process time: %.2fms", mProcessTime )
            );

            mText[ 2 ].setText(detectionFunction);

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

        // retrieves user preferences for which fastcv
        // detection to use and applies the appropriate detection
        resIndex =
            Integer.decode( prefs.getString( "detectionSwitch", "1" ) );
        if( resIndex < mPrefOptsDetectionSwitch.length )
        {
            Log.e( TAG, "Configured Corner Detection: " + resIndex );
            mPrefDetectionSwitch = mPrefOptsDetectionSwitch[ resIndex-1 ];
            Log.e( TAG, "Configured Corner Detection Function: " +
                     mPrefDetectionSwitch );

        }

    }

    protected void startPreview()
    {
       Log.v( TAG, "GraphicalActivity: startPreview()");
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
    protected void stopPreview()
    {
       Log.v( TAG, "GraphicalActivity: stopPreview()");
       if ((mCamera != null) && (mPreviewRunning == true))
       {
          mCamera.stopPreview();
          mCamera.setPreviewCallback( null );
       }

       mProfileHandler.removeCallbacks( mUpdateTimeTask );

       mPreviewRunning = false;
       System.gc();
    }

    protected void setupCamera(){
        super.setupCamera();

        //
        // Perform FastCV example configurations as retrieved from application's
        // preferences
        //
        setScaling(mPrefScale);
        setFastCornerThreshold(mPrefThreshold);
        setDetection(mPrefDetectionSwitch);
        detectionFunction = detectionMethod();

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
     * Sets the native FAST corner threshold.
     *
     * @param threshold Barrier threshold to FAST Corner algorithm.
     */
    protected native void setFastCornerThreshold(int threshold);

    /**
     * Sets the desired scaling factor.
     *
     *
     * @param scaleFactor Desired scaling factor, 1 is no scaling.
     */
    protected native void setScaling(int scaleFactor);

    /**
     * Enables caller to disable camera backdrop.
     *
     * @param enableViewFinder Flag to enable/disable camera
     *                         backdrop.
     */
    protected native void setViewFinderEnable(boolean enableViewFinder);

    /**
     * Enables/disables overlaying of pixels.
     *
     * @param enableOverlayPixels Flag to enable/disable pixel
     *                            overlay.
     */
    protected native void setOverlayPixelsEnable(boolean enableOverlayPixels);

    /**
     * Enables/disables blurring camera image before processing
     * corners.
     *
     * @param enableGaussian Flag enables/disables Gaussian
     *                       blurring.
     */
    protected native void setGaussianEnable(boolean enableGaussian);

    /** Sets the detection function to be used. */
    protected native void setDetection(int detectionSwitch);

    /**
     * Retrieves the latest number of corners for debug purposes.
     *
     * @return int Number of corners.
     */
    protected native int getNumCorners();

    /**
     * Function to pass camera frame for native, FastCV processing.
     * @param data Byte buffer for data.
     * @param w Width of data
     * @param h Height of data
     */
    public native void update( byte[] data, int w, int h );

    /** Performs native cleanup routines for application exit. */
    public native void cleanup();

    /** Retrieves the name of the detection method being used. */
    public native String detectionMethod();

}
