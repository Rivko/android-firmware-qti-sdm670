/*==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcvdemo.apis.imageProcessing;

import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ToggleButton;

import com.qualcomm.fastcvdemo.R;
import com.qualcomm.fastcvdemo.base.GraphicalActivity;

public class ImageGradient extends GraphicalActivity {

    /** Values for orientation options */
    public static final int VERTICAL_GRADIENT           = 0;
    public static final int HORIZONTAL_GRADIENT = 1;

    /** Values for sobel options */
    public static final int RESET          = 0;
    public static final int NO_SOBEL = 1;
    public static final int SOBEL           = 2;

    /** Type of orientation selected, set to vertical by default */
    private static int mGradientOrientationType = VERTICAL_GRADIENT;

    /** Type of gradient selected, set to non sobel by default */
    private static int mGradientType = NO_SOBEL;

    /** Button to allow user to switch orientation */
    protected ToggleButton                tBtnOrientationSwitch;

    static
    {
        Log.v( TAG, "ImageGradient: load fastcvimgproc library");
        System.loadLibrary( "fastcvImgProc" );
    }

    /** Function which retrieves title based on module used. */
    protected void initTitle()
    {
        title = "FastCV Image Gradient";
    }

    /**  Resumes camera preview */
    protected void onResume()
    {
        Log.v( TAG, "ImgDiff: onResume()");
        super.mMainLayout = R.layout.imagegradient;
        super.onResume();
        mText[2].setText (String.format( "VERTICAL_GRADIENT  is selected" ));
        tBtnOrientationSwitch = (ToggleButton) findViewById(R.id.orientationSwitch);
        //Handles orientation selection
        tBtnOrientationSwitch.setOnClickListener
        (
           new View.OnClickListener()
           {
              public void onClick( View view )
              {
                  if (tBtnOrientationSwitch.isChecked())
                  {
                    mGradientOrientationType = HORIZONTAL_GRADIENT;
                      mText[2].setText (String.format( "HORIZONTAL_GRADIENT is selected" ));
                      setGradientOrientation(mGradientOrientationType);
                  }
                  else
                  {
                    mGradientOrientationType = VERTICAL_GRADIENT  ;
                      mText[2].setText (String.format( "VERTICAL_GRADIENT  is selected" ));
                      setGradientOrientation(mGradientOrientationType);
                  }
              }
           }
        );
        tBtnOrientationSwitch.bringToFront();
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate( R.menu.imagegradientmenu, menu );
        mText[1].setText (String.format( "Select an Image Processing option from the Menu" ));
        return true;
    }

    /** User Option selection menu **/
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        mText[2].setText (String.format( "Select an Image Processing  option from the Menu" ));
        // Handle item selection
        switch( item.getItemId() )
        {
            case R.id.NO_SOBEL:
                mGradientType = NO_SOBEL;
                mText[2].setText (String.format( "NO_SOBEL is selected" ));
                setGradientType(mGradientType);
                return true;
            case R.id.SOBEL:
                mGradientType = SOBEL;
                mText[2].setText (String.format( "SOBEL is selected" ));
                setGradientType(mGradientType);
                return true;
            case R.id.RESET:
                mGradientType = RESET;
                mText[2].setText (String.format( "Select an Image Processing  option from the Menu" ));
                setGradientType(RESET);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    protected void setupCamera()
    {
        super.setupCamera();
        Log.v( TAG, "ImageGradient.setupCamera()" );
        super.setupCamera();
        /** Perform FastCV example configurations as retrieved from application's preferences */
        setGradientOrientation(mGradientOrientationType);
        setGradientType(mGradientType);
    }

    //Native functions

    /**
     * Sets the desired orientation.
     * @param gradientOrientaion Desired orientation
     */
    public native void setGradientOrientation(int gradientOrientation);

    /**
     * Sets the desired gradient type.
     * @param gradientType Desired gradient type.
     */
    public native void setGradientType(int gradientType);

    /**
     * Function to pass camera frame for native, FastCV processing.
     * @param data Byte buffer for data.
     * @param w Width of data
     * @param h Height of data
     */
    public native void update( byte[] data, int w, int h );

    /** Performs native cleanup routines for application exit. */
    public native void cleanup();
}
