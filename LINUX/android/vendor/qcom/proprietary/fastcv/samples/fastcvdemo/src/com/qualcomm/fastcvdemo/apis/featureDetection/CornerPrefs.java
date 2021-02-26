/*==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcvdemo.apis.featureDetection;

import com.qualcomm.fastcvdemo.R;
import android.os.Bundle;
import android.preference.PreferenceActivity;

/**
 * Basic empty Preferences class. All configurations for
 * Setting menu is done in the xml file.
 *
 */
public class CornerPrefs extends PreferenceActivity
{
   /** Called when the activity is first created. */
   @Override
   protected void onCreate( Bundle savedInstanceState )
   {
      super.onCreate(savedInstanceState);
      addPreferencesFromResource( R.layout.cornerpref );
   }
}

