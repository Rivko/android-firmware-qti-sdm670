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

import com.qualcomm.fastcvdemo.R;
import com.qualcomm.fastcvdemo.base.GraphicalActivity;

public class Filter extends GraphicalActivity {

    /** Values for filter options */
    public static final int RESET             = 0;
    public static final int ENABLE_GAUSS_3X3        = 1;
    public static final int ENABLE_GAUSS_5X5      = 2;
    public static final int ENABLE_GAUSS_11X11   = 3;
    public static final int ENABLE_CANNY     = 4;
    public static final int ENABLE_ERODE     = 5;
    public static final int ENABLE_DILATE   = 6;
    public static final int ENABLE_MEDIAN    = 7;
    public static final int ENABLE_SOBEL     = 8;
    public static final int ENABLE_THRESHOLD    = 9;

    /** Type of filter selected, set to no filter by default */
    private static int mFilterType = RESET;

    static
    {
        Log.v( TAG, "Filter: load fastcvimgproc library");
        System.loadLibrary( "fastcvImgProc" );
    }

    /** Function which retrieves title based on module used. */
    protected void initTitle()
    {
        title = "FastCV Filter";
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate( R.menu.filtermenu, menu );
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
            case R.id.Gaussian3x3Filter:
                mFilterType = ENABLE_GAUSS_3X3 ;
                mText[2].setText (String.format( "Gaussian3x3Filter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.Gaussian5x5Filter:
                mFilterType = ENABLE_GAUSS_5X5 ;
                mText[2].setText (String.format( "Gaussian5x5Filter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.Gaussian11x11Filter:
                mFilterType = ENABLE_GAUSS_11X11 ;
                mText[2].setText (String.format( "Gaussian11x11Filter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.CannyFilter:
                mFilterType = ENABLE_CANNY;
                mText[2].setText (String.format( "CannyFilter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.ErodeFilter:
                mFilterType = ENABLE_ERODE;
                mText[2].setText (String.format( "ErodeFilter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.DilateFilter:
                mFilterType = ENABLE_DILATE;
                mText[2].setText (String.format( "DilateFilterr is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.MedianFilter:
                mFilterType = ENABLE_MEDIAN;
                mText[2].setText (String.format( "MedianFilter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.SobelFilter:
                mFilterType = ENABLE_SOBEL;
                mText[2].setText (String.format( "SobelFilter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.ThresholdFilter:
                mFilterType = ENABLE_THRESHOLD;
                mText[2].setText (String.format( "ThresholdFilter is selected" ));
                setFilter(mFilterType);
                return true;
            case R.id.Reset:
                mFilterType = RESET;
                mText[2].setText (String.format( "Select an Image Processing  option from the Menu" ));
                setFilter(mFilterType);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    protected void setupCamera()
    {
        super.setupCamera();
        Log.v( TAG, "Scale.setupCamera()" );
        super.setupCamera();
        /** Perform FastCV example configurations as retrieved from application's preferences */
        setFilter(mFilterType);
    }

    //Native functions

    /**
     * Sets the desired filter type.
     * @param filterType Desired filter type.
     */
    public native void setFilter(int filterType);

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
