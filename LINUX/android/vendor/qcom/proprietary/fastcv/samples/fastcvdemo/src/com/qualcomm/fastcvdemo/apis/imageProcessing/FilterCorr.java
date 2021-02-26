package com.qualcomm.fastcvdemo.apis.imageProcessing;

import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import com.qualcomm.fastcvdemo.R;
import com.qualcomm.fastcvdemo.base.GraphicalActivity;

public class FilterCorr extends GraphicalActivity {

    /** Values for kernel options */
    public static final int RESET       = 0;
    public static final int THREExTHREE = 1;
    public static final int THIRTEENxTHIRTEEN = 2;


    /** Type of kernel selected, set to 3x3 by default */
    private static int mKernelType = THREExTHREE;

    static
    {
        Log.v( TAG, "FilterCorr: load fastcvimgproc library");
        System.loadLibrary( "fastcvImgProc" );
    }

    /** Function which retrieves title based on module used. */
    protected void initTitle()
    {
        title = "FastCV Filter Corr";
    }

    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate( R.menu.filtercorrmenu, menu );
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
            case R.id.THREExTHREE:
                mKernelType = THREExTHREE;
                mText[2].setText (String.format( "3x3 is selected" ));
                setKernelType(mKernelType);
              return true;
            case R.id.THIRTEENxTHIRTEEN:
                mKernelType = THIRTEENxTHIRTEEN;
                mText[2].setText (String.format( "13x13 is selected" ));
                setKernelType(mKernelType);
              return true;
            case R.id.RESET:
                mKernelType = RESET;
                mText[2].setText (String.format( "Select an Image Processing  option from the Menu" ));
                setKernelType(mKernelType);
              return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    protected void setupCamera()
    {
        super.setupCamera();
        Log.v( TAG, "FilterCorr.setupCamera()" );
        super.setupCamera();
        /** Perform FastCV example configurations as retrieved from application's preferences */
        setKernelType(mKernelType);
    }

    //Native functions

    /**
     * Sets the desired kernel type.
     * @param kernelType Desired kernel type.
     */
    public native void setKernelType(int kernelType);

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
