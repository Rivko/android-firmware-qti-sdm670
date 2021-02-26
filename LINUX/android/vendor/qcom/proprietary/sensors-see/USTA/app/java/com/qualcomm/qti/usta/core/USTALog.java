/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file LoggingUtil.java
  @brief
   Logging wrappers to control the USTA logging from app side
============================================================================*/
package com.qualcomm.qti.usta.core;

import android.util.Log;
import com.qualcomm.qti.usta.ui.UnifiedSensorTestActivity;


public class USTALog{

  private static final String USTA_TAG_INFO = "USTA_APP_INFO";
  private static final String USTA_TAG_DEBUG = "USTA_APP_DEBUG";
  private static final String USTA_TAG_ERROR = "USTA_APP_ERROR";
  private static final String USTA_TAG_VERBOSE = "USTA_TAG_VERBOSE";
  private static final String USTA_TAG_WARN = "USTA_TAG_WARN";

  public static boolean isLoggingDisabled = false ;

  public static void i(String logMessage){
    if(isLoggingDisabled == false) {
      Log.i(USTA_TAG_INFO, logMessage);
    }
  }

  public static void d(String logMessage){
    if(isLoggingDisabled == false) {
      Log.d(USTA_TAG_DEBUG, logMessage);
    }
  }

  public static void e(String logMessage){
    if(isLoggingDisabled == false) {
      Log.e(USTA_TAG_ERROR, logMessage);
    }
  }

  public static void v(String logMessage){
    if(isLoggingDisabled == false) {
      Log.v(USTA_TAG_VERBOSE, logMessage);
    }
  }

  public static void w(String logMessage){
    if(isLoggingDisabled == false) {
      Log.w(USTA_TAG_WARN, logMessage);
    }
  }

}
