/*==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcvdemo.apis.imageProcessing;

import android.util.Log;
import android.view.View;
import android.widget.ToggleButton;

import com.qualcomm.fastcvdemo.R;
import com.qualcomm.fastcvdemo.base.GraphicalActivity;

public class ImgDiff extends GraphicalActivity {
    /** Android UI button to get the reference frame */
    protected ToggleButton                tBtnDiffReset;

    static
    {
        Log.v( TAG, "Filter: load fastcvimgproc library");
        System.loadLibrary( "fastcvImgProc" );
    }

    /** Function which retrieves title based on module used. */
    protected void initTitle()
    {
        title = "FastCV Image Diff";
    }

    protected void onResume()
    {
        Log.v( TAG, "ImgDiff: onResume()");
        super.mMainLayout = R.layout.imgdiff;
        super.onResume();
        mText[2].setText (String.format( "Take a reference frame" ));
        tBtnDiffReset = (ToggleButton) findViewById(R.id.diffResetSwitch);
        //Handles reference frame selection
        tBtnDiffReset.setOnClickListener
        (
           new View.OnClickListener()
           {
              public void onClick( View view )
              {
                  if (tBtnDiffReset.isChecked())
                  {
                      mText[2].setText (String.format( "Reference Frame selected" ));
                      takeReferenceFrame();
                  }
                  else
                  {
                      mText[2].setText (String.format( "Take a new reference frame" ));
                      resetReferenceFrame();
                  }
              }
           }
        );
        tBtnDiffReset.bringToFront();
    }

    // Native function declarations

    /**
     * Function to pass camera frame for native, FastCV processing.
     * @param data Byte buffer for data.
     * @param w Width of data
     * @param h Height of data
     */
    public native void update( byte[] data, int w, int h );

    /** Performs native cleanup routines for application exit. */
    public native void cleanup();

    /** Stores current frame as reference frame. */
    public native void takeReferenceFrame();

    /** Resets reference frame. */
    public native void resetReferenceFrame();
}
