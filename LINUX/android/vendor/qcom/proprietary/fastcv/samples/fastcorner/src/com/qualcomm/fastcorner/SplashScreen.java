/*==============================================================================
            Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcorner;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Display;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.WindowManager;

/** The splash screen activity for FastCV sample application */
public class SplashScreen extends Activity
{
    private WindowManager               mWindowManager;
    private SplashScreenView            mHomeView;
    public static Display               sDisplay;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mWindowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        sDisplay = mWindowManager.getDefaultDisplay();

        // Initialize UI
        mHomeView = new SplashScreenView(this);
        setContentView( mHomeView );
    }

    @Override
    protected void onPause()
    {
       super.onPause();
    }
    /** Called when the option menu is created. */
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
       MenuInflater inflater = getMenuInflater();
       inflater.inflate( R.menu.splashmenu, menu );
       return true;
    }

     /** User Option selection menu
     */
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
       // Handle item selection
       switch( item.getItemId() )
       {
       case R.id.main_start:
           Intent startActivity = new Intent(getBaseContext(), FastCVSample.class);
           startActivity( startActivity );

           return true;

       case R.id.settings:
           Intent settingsActivity = new Intent( getBaseContext(), Preferences.class );
           startActivity( settingsActivity );

           return true;

       case R.id.about:
          Intent aboutActivity = new Intent( getBaseContext(), About.class );
          startActivity( aboutActivity );

          return true;

       default:
          return super.onOptionsItemSelected(item);
       }
    }
}