/******************************************************************************
 * @file    Proximity.java
 * @brief   Provides Proximity options.
 *
 *
 * ---------------------------------------------------------------------------
 *  Copyright (C) 2011-2012,2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *  ---------------------------------------------------------------------------
 *******************************************************************************/

package com.android.ultrasound;

import com.android.ultrasound.UltrasoundCfg;
import android.util.Log;
import android.os.Bundle;
import android.os.SystemProperties;
import android.widget.CheckBox;
import android.view.View;


public class Proximity extends UltrasoundCfg {
  private final String xmlFileName = "proximity";
  private final String thisDaemon = "usf_proximity";
  private final String thisFileName = "Proximity.java";
  private final String tag = "ProximitySettings";
  private final String cfgFileLocation =
            "/data/usf/proximity/cfg/usf_proximity.cfg";  // Default cfg file
  private final String cfgLinkFile = "/data/usf/proximity/usf_proximity.cfg";
  private final String cfgFileDir = "/data/usf/proximity/cfg/";
  private final String cfgFileExpressionDir = "/data/usf/proximity/.*/";
  private final String recFileDir = "/data/usf/proximity/rec/";
  private final String forceUsingCfgProprty = "persist.sys.usf.force_using_cfg";
  private CheckBox forceUsingCfgCheckbox;

  /*===========================================================================
  FUNCTION:  onCreate
  ===========================================================================*/
  /**
    onCreate() function create the UI and set the private params
    (params that apear only in the Proximity UI).
  */
  @Override
  protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);

      // Find and set the private params

      forceUsingCfgCheckbox = (CheckBox)findViewById(R.id.force_using_cfg);
      if (null != forceUsingCfgCheckbox) {
          forceUsingCfgCheckbox.setEnabled(true);
          if ((SystemProperties.get(forceUsingCfgProprty, "0")).equals("1")) {
              forceUsingCfgCheckbox.setChecked(true);
          }

          forceUsingCfgCheckbox.setOnClickListener(new View.OnClickListener() {
              @Override
              public void onClick(View view) {
                 if (((CheckBox)view).isChecked()) {
                     SystemProperties.set(forceUsingCfgProprty, "1");
                 } else {
                     SystemProperties.set(forceUsingCfgProprty, "0");
                 }
              }
          });
      }

      Log.d(getTag(), "Finished to set the proximity private params");

      readCfgFileAndSetDisp(true);
  }

  /*===========================================================================
  FUNCTION:  onResume
  ===========================================================================*/
  /**
    onResume() function update the UI.
  */
  @Override
  protected void onResume () {
      super.onResume();
      mDaemonCfgFilePicker.setCfgFileSpinner();
      readCfgFileAndSetDisp(true);
  }

  /*===========================================================================
  FUNCTION:  getXmlFileName
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getXmlFileName() {
    return xmlFileName;
  }

  /*===========================================================================
  FUNCTION:  getThisFileName
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getThisFileName() {
    return thisFileName;
  }

  /*===========================================================================
  FUNCTION:  getTag
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getTag() {
    return tag;
  }

  /*===========================================================================
  FUNCTION:  getDaemonName
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getDaemonName() {
    return thisDaemon;
  }

  /*===========================================================================
  FUNCTION:  getCfgLinkFileLocation
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getCfgLinkFileLocation() {
    return cfgLinkFile;
  }

  /*===========================================================================
  FUNCTION:  getCfgFilesDir
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getCfgFilesDir() {
    return cfgFileDir;
  }

  /*===========================================================================
  FUNCTION:  getCfgExpressionDir
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getCfgExpressionDir() {
    return cfgFileExpressionDir;
  }

  /*===========================================================================
  FUNCTION:  getRecFilesDir
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getRecFilesDir() {
    return recFileDir;
  }

  /*===========================================================================
  FUNCTION:  getDefaultCfgFileName
  ===========================================================================*/
  /**
    See description at UltrasoundCfg.java.
  */
  @Override
  protected String getDefaultCfgFileName() {
    return cfgFileLocation;
  }

}

