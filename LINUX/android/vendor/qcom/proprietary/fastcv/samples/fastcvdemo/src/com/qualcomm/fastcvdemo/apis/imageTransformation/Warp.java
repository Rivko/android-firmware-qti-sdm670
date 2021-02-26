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

public class Warp extends GraphicalActivity {
    public static final int NO_WARP               = 0;
    public static final int WARP_P_U8             = 1;
    public static final int WARP_P_3CHANNEL       = 2;

    private static int mWarpType = 2;

    static
    {
        Log.v( TAG, "Warp: load fastcvImgTransform library");
        System.loadLibrary( "fastcvImgTransform" );
    }

    protected void initTitle()
    {
        title = "FastCV Warp";
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
       MenuInflater inflater = getMenuInflater();
       inflater.inflate( R.menu.warpmenu, menu );
       mText[1].setText (String.format( "Select an Image Warp option from the Menu" ));
       return true;
    }

    /** User Option selection menu **/
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        // Handle menu item selection
        switch( item.getItemId() )
        {
            case R.id.warp_p_u8:
                mWarpType = WARP_P_U8;
                mText[2].setText (String.format( "fcvWarpPerspectiveu8 is selected" ));
                setWarp(mWarpType);
                return true;
            case R.id.warp_p_3channel:
                mWarpType = WARP_P_3CHANNEL;
                mText[2].setText (String.format( "fcv3ChannelWarpPerspectiveu8 is selected" ));
                setWarp(mWarpType);
                return true;
            case R.id.no_warp:
                mWarpType = NO_WARP;
                mText[2].setText (String.format( "Select an Image Warp Option from the Menu" ));
                setWarp(mWarpType);
                return true;
            default:
                mWarpType = WARP_P_U8;
                mText[2].setText (String.format( "Select an Image Warp Option from the Menu" ));
                setWarp(mWarpType);
                return true;
        }
    }

   protected void setupCamera()
   {
        Log.v( TAG, "Warp.setupCamera()" );
        super.setupCamera();
        setWarp(mWarpType);
   }


   // Native function declarations

    protected native void setWarp(int warpType);

    public native void update( byte[] data, int w, int h );

    public native void cleanup();
}

