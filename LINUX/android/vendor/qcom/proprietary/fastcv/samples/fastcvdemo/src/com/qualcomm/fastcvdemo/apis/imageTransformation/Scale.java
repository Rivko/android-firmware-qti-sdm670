/*==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcvdemo.apis.imageTransformation;

import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import com.qualcomm.fastcvdemo.R;
import com.qualcomm.fastcvdemo.base.GraphicalActivity;

public class Scale extends GraphicalActivity {
    public static final int NO_SCALE              = 0;
    public static final int SCALE_DOWN_BY2        = 1;
    public static final int SCALE_DOWN_BY4        = 2;
    public static final int SCALE_DOWN_BY2_GAUS   = 3;
    public static final int SCALE_UP_BY2_GAUS     = 4;

    private static int mScaleType;

    static
    {
        Log.v( TAG, "Scale: load fastcvImgTransform library");
        System.loadLibrary( "fastcvImgTransform" );
    }

    protected void initTitle()
    {
        title = "FastCV Scale";
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
       MenuInflater inflater = getMenuInflater();
       inflater.inflate( R.menu.scalemenu, menu );
       mText[1].setText (String.format( "Select an Image Transformation option from the Menu" ));
       return true;
    }

     /** User Option selection menu **/
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        // Handle menu item selection
        switch( item.getItemId() )
        {
            case R.id.sd_by_2u8:
                mScaleType = SCALE_DOWN_BY2;
                mText[2].setText (String.format( "fcvScaleDownBy2u8 is selected" ));
                break;
            case R.id.sd_by_4u8:
                mScaleType = SCALE_DOWN_BY4;
                mText[2].setText (String.format( "fcvScaleDownBy4u8 is selected" ));
                break;
            case R.id.sdG_by_2:
                mScaleType = SCALE_DOWN_BY2_GAUS;
                mText[2].setText (String.format( "fcvScaleDownBy2Gaussian5x5u8 is selected" ));
                break;
            case R.id.suG_by_2:
                mScaleType = SCALE_UP_BY2_GAUS;
                mText[2].setText (String.format( "fcvScaleUpBy2Gaussian5x5u8 is selected" ));
                break;
            case R.id.reset:
            default:
                mScaleType = NO_SCALE;
                mText[2].setText (String.format( "Select an Image Transformation option from the Menu" ));
                break;
        }
        setScaling(mScaleType);
        return true;
    }

    protected void setupCamera()
    {
        Log.v( TAG, "Scale.setupCamera()" );
        super.setupCamera();

        /* Perform FastCV example configurations as retrieved from application's preferences
        */
        setScaling(mScaleType);
    }

    // Native function declarations

    public native void setScaling(int scaleType);

    public native void update( byte[] data, int w, int h );

    public native void cleanup();
}
