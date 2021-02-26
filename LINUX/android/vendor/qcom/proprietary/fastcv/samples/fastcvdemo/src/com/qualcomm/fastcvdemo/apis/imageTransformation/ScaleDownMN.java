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
import android.widget.ToggleButton;
import android.view.View;

import com.qualcomm.fastcvdemo.R;
import com.qualcomm.fastcvdemo.base.GraphicalActivity;

public class ScaleDownMN extends GraphicalActivity {

    protected ToggleButton                tBtnPreviewSwitch;

    private static int mScale;

    static
    {
        Log.v( TAG, "ScaleDownMN: load fastcvImgTransform library");
        System.loadLibrary( "fastcvImgTransform" );
    }

    protected void initTitle()
    {
        title = "FastCV Transform ScaleDownMN";
    }

    protected void onResume()
    {
        Log.v( TAG, "ScaleDownMN: onResume()");
        mScale = 0;
        super.mMainLayout = R.layout.scaledown_mn;
        super.onResume();

        tBtnPreviewSwitch = (ToggleButton) findViewById(R.id.previewSwitch);
        //Handles the preview selection i.e. when to stop preview and when to start preview
        tBtnPreviewSwitch.setOnClickListener
        (
           new View.OnClickListener()
           {
              public void onClick( View view )
              {
                  if (tBtnPreviewSwitch.isChecked())
                  {
                    setPreview(false);
                    Log.v(TAG , "Preview Button set to false");
                    startPreview();
                  }
                  else
                  {
                    setPreview(true);
                    Log.v(TAG , "Preview Button set to true");
                    startPreview();
                  }
              }
           }
        );
        tBtnPreviewSwitch.bringToFront();
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
       MenuInflater inflater = getMenuInflater();
       inflater.inflate( R.menu.scaledown_mn_menu, menu );
       mText[2].setText (String.format( "Select a Scaling Factor option from the Menu" ));
       return true;
    }

    /** User Option selection menu **/
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        // Handle menu item selection
        switch( item.getItemId() )
        {
            case R.id.mn_one:
                mScale = 0;
                setScalePref (true);
                mText[2].setText (String.format( "MN : 1 is selected" ));
                break;

            case R.id.mn_onefive:
                mScale = 1;
                setScalePref (true);
                mText[2].setText (String.format( "MN : 1.5 is selected" ));
                break;

            case R.id.mn_two:
                mScale = 2;
                setScalePref (true);
                mText[2].setText (String.format( "MN : 2 is selected" ));
                break;

            case R.id.mn_twofive:
                mScale = 3;
                setScalePref (true);
                mText[2].setText (String.format( "MN : 2.5 is selected" ));
                break;

            case R.id.nn_one:
                mScale = 0;
                setScalePref (false);
                mText[2].setText (String.format( "NN : 1 is selected" ));
                break;

            case R.id.nn_onefive:
                mScale = 1;
                setScalePref (false);
                mText[2].setText (String.format( "NN : 1.5 is selected" ));
                break;

            case R.id.nn_two:
                mScale = 2;
                setScalePref (false);
                mText[2].setText (String.format( "NN : 2 is selected" ));
                break;

            case R.id.nn_twofive:
                mScale = 3;
                setScalePref (false);
                mText[2].setText (String.format( "NN : 2.5 is selected" ));
                break;

            default:
                mScale = 0;
                setScalePref (true);
                mText[2].setText (String.format( "MN : 1 is selected" ));
                break;
        }
        setScaling(mScale);
        return true;
    }

    protected void setupCamera()  {
        Log.v( TAG, "ScaleDownMN.setupCamera()" );
        super.setupCamera();
        setScaling(mScale);
    }


    // Native function declarations

    protected native void setScaling (int scale);

    protected native void setPreview (boolean pre);

    protected native void setScalePref (boolean scaleP);

    public native void update( byte[] data, int w, int h );

    public native void cleanup();
}

