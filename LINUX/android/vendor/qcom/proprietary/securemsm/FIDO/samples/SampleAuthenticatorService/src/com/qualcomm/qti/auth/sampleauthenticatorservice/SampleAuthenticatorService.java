/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.sampleauthenticatorservice;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.security.SecureRandom;

public class SampleAuthenticatorService extends Service {
  private ISampleAuthenticator.Stub mSampleAuthenticatorBinder;
  private static final String LOG_TAG = "SampleAuthenticator";
  private static final String authIdNumFile = "auth_id_num";
  private static Context sContext;

  public SampleAuthenticatorService() {
    this.mSampleAuthenticatorBinder = new SampleAuthenticatorImpl();
    this.sContext = this;
  }

  @Override
  public IBinder onBind(Intent intent) {
    Log.d(LOG_TAG, "onBind()");
    File file = this.getFileStreamPath(authIdNumFile);
    if(file == null || !file.exists()) {
      // File doesn't yet exist, create now
      Log.d(LOG_TAG, "File doesn't yet exist, create now");
      SecureRandom sr = new SecureRandom();
      byte[] output = new byte[64];
      if(sr != null && output != null) {
        sr.nextBytes(output);
        FileOutputStream fos = null;
        try {
          fos = openFileOutput(authIdNumFile, Context.MODE_PRIVATE);
          fos.write(output);
          fos.close();
        } catch(FileNotFoundException fe){
          Log.d(LOG_TAG, "File could not be created");
        } catch(IOException ioe){
          Log.d(LOG_TAG, "File could not be written");
          if(fos != null) {
            try {
              fos.close();
            } catch (IOException ie){
            }
          }
        }
      }
    } else {
      Log.d(LOG_TAG, "File already exists");
    }
    return this.mSampleAuthenticatorBinder;
  }

  @Override
  public void onDestroy() {
  }

  public static byte[] getAppRandomAuth(){
    byte[] randAuthNum = {0};
    FileInputStream fis = null;
    try {
      fis = sContext.openFileInput(authIdNumFile);
      fis.read(randAuthNum);
      fis.close();
    } catch(FileNotFoundException e) {
      Log.d(LOG_TAG, "Error opening file for hash");
      return null;
    } catch(IOException e) {
      Log.d(LOG_TAG, "Error reading file for hash");
      if(fis != null) {
        try {
          fis.close();
        } catch (IOException ie){
        }
      }
    }
    return randAuthNum;
  }

}
