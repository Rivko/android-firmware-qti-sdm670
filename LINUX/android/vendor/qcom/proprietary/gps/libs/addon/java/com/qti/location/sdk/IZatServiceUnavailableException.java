/* ======================================================================
*  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/** @addtogroup IZatServiceUnavailableException
    @{ */

/**
 * An exception indicating that the location service is unavailable.
 * @cond
 * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * @endcond
 */
public class IZatServiceUnavailableException extends RuntimeException{
    public IZatServiceUnavailableException() {
    }
    /** Creates an IZatServiceUnavailableException object.

        @param error Error string.
        @return
        None.
    */
    public IZatServiceUnavailableException(String error) {
        super(error);
    }
    /** Creates an IZatServiceUnavailableException object with the provided parameters.

        @param error Error string.
        @param cause Cause of the thrown exception. A NULL value is permitted
                         and indicates that the cause is nonexistent or unknown.
        @return
        None.
    */
    public IZatServiceUnavailableException(String error, Throwable cause) {
        super(error, cause);
    }
}
/** @} */
