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

public class Affine extends GraphicalActivity {
    public static final int NO_AFFINE             = 0;
    public static final int AFFINE_U8              = 1;
    public static final int AFFINE_8X8            = 2;

    private static int mAffineType;

    static
    {
        Log.v( TAG, "Affine: load fastcvImgTransform library");
        System.loadLibrary( "fastcvImgTransform" );
    }

    protected void initTitle()
    {
        title = "FastCV Transform Affine";
    }

    protected void onResume()
    {
        Log.v( TAG, "Affine: onResume()");
        mAffineType = AFFINE_U8;
        super.onResume();
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
       MenuInflater inflater = getMenuInflater();
       inflater.inflate( R.menu.affinemenu, menu );
       mText[1].setText (String.format( "Select a Transform Affine option from the Menu" ));
       return true;
    }

    /** User Option selection menu **/
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        // Handle menu item selection
        switch( item.getItemId() )
        {
            case R.id.affine_u8:
                mAffineType = AFFINE_U8;
                mText[2].setText (String.format( "fcvTransformAffineu8 is selected" ));
                break;

            case R.id.affine_8x8:
                mAffineType = AFFINE_8X8;
                mText[2].setText (String.format( "fcvTransformAffine8x8u8 is selected" ));
                break;

            case R.id.no_affine:
            default:
                mAffineType = NO_AFFINE;
                mText[2].setText (String.format( "Select a Transform Affine option from the Menu" ));
                break;
        }
        setAffine(mAffineType);
        return true;
    }

    protected void setupCamera(){
        Log.v( TAG, "Affine.setupCamera()" );
        super.setupCamera();
        setAffine(mAffineType);
    }


    // Native function declarations

    protected native void setAffine(int affineType);

    public native void update( byte[] data, int w, int h );

    public native void cleanup();
}

