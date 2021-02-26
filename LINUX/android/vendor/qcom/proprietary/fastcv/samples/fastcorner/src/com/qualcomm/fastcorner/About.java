/*==============================================================================
            Copyright (c) 2010-2011, 2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcorner;

import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.util.Log;

/**
 * About class.
 * Some configurations for About menu is done in the xml file.
 */
public class About extends PreferenceActivity
{
   /** Logging tag */
   protected static final String         TAG               = "FastCVSample";

   static
   {
      // Load JNI library
      Log.v( TAG, "About: load fastcvsample library");
      System.loadLibrary( "fastcvsample" );
   };

   private PreferenceScreen aboutScreen;

   /** Called when the activity is first created. */
   @Override
   protected void onCreate( Bundle savedInstanceState )
   {
      super.onCreate(savedInstanceState);
      addPreferencesFromResource( R.layout.about );
      if (aboutScreen == null)
      {
         String fastcvVersion = getFastCVVersion();

         aboutScreen = getPreferenceScreen();

         PreferenceScreen versionNumberScreen = getPreferenceManager().createPreferenceScreen(this);
         versionNumberScreen.setTitle(getString(R.string.versionNumber_text));
         versionNumberScreen.setSummary(fastcvVersion);
         aboutScreen.addPreference(versionNumberScreen);
      }
      setPreferenceScreen(aboutScreen);
   }

   //Native Function Declarations
   public native String getFastCVVersion();
}
