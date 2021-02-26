/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.sdk;

/*
 * The Class OnResultBase.
 */
public interface OnResultBase {

  /**
   * The Constant DATA_USER_ID.
   */
  public static final String DATA_USER_ID = "DATA_USER_ID";

  /**
   * The Constant DATA_KEYPHRASE_ID.
   */
  public static final String DATA_KEYPHRASE_ID = "DATA_KEYPHRASE_ID";

  /**
   * The Constant FAILURE_CODE.
   */
  public static final String FAILURE_CODE = "FAILURE_CODE";

  public static final String DEBUG = "DEBUG";

  public static String BYTE_INDEX = "BYTE_INDEX";


  /**
   * @param errorCode The code received by the voice print framework.
   * @brief An Error has occurred.
   * @details An error may occur anytime during the enrollment process.
   */
  public void onError(int errorCode);
}
