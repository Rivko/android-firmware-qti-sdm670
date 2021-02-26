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

public class Rotate extends GraphicalActivity {
    public static final int ZERO             = 0;
    public static final int NINETY           = 1;
    public static final int ONE_EIGHTY       = 2;
    private static final int TWO_SEVENTY     = 3;
    public static final int RESET            = 4;

    private static int mRotateDegree = ZERO;

    static
    {
        Log.v( TAG, "Rotate: load fastcvImgTransform library");
        System.loadLibrary( "fastcvImgTransform" );
    }

    protected void initTitle()
    {
        title = "FastCV Rotation";
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate( R.menu.rotatemenu, menu );
        mText[1].setText (String.format( "Select a Rotation option from the Menu" ));
        return true;
    }

    /** User Option selection menu **/
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        // Handle menu item selection
        switch( item.getItemId() )
        {
            case R.id.zero:
                mRotateDegree = ZERO;
                mText[2].setText (String.format( "The image is rotated 0 degrees" ));
                break;
            case R.id.ninety:
                mRotateDegree = NINETY;
                mText[2].setText (String.format( "The image is rotated 90 degrees" ));
                break;
            case R.id.one_eighty:
                mRotateDegree = ONE_EIGHTY;
                mText[2].setText (String.format( "The image is rotated 180 degrees" ));
                break;
            case R.id.two_seventy:
                mRotateDegree = TWO_SEVENTY;
                mText[2].setText (String.format( "The image is rotated 270 degrees" ));
                break;
            case R.id.reset:
            default:
                mRotateDegree = RESET;
                mText[2].setText (String.format( "Select a Rotation option from the Menu" ));
                break;
        }
        setRotation(mRotateDegree);
        return true;
    }

    protected void setupCamera(){
        Log.v( TAG, "Rotate.setupCamera()" );
        super.setupCamera();
        setRotation(mRotateDegree);
    }


    // Native function declarations

    protected native void setRotation(int rotateAngle);

    public native void update( byte[] data, int w, int h );

    public native void cleanup();
}

