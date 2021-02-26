/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/** @addtogroup IZatFeatureNotSupportedException
    @{ */

/**
 * An exception indicating that the feature is not supported.
 * @cond
 * <p>Copyright (c) 2017 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * @endcond
 */
public class IZatFeatureNotSupportedException extends RuntimeException{
    public IZatFeatureNotSupportedException() {
    }
    /** Creates and IZatFeatureNotSupportedException object.

        @param error Error string.
        @return
        None.
    */
    public IZatFeatureNotSupportedException(String error) {
        super(error);
    }
    /** Creates an IZatFeatureNotSupported object with the provided parameters.

        @param error Error string.
        @param cause Cause of the thrown exception. A NULL value is permitted
                         and indicates that the cause is nonexistent or unknown.
        @return
        None.
    */
    public IZatFeatureNotSupportedException(String error, Throwable cause) {
        super(error, cause);
    }
}
/** @} */
