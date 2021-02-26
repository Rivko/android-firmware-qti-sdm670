/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/** @addtogroup IZatSecurityException
    @{ */

/**
 * An exception indicating that a security rule was violated.
 * @cond
 * <p>Copyright (c) 2017 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * @endcond
 */
public class IZatSecurityException extends RuntimeException{
    public IZatSecurityException() {
    }
    /** Creates and IZatSecurityException object.

    @param error Error string.
    @return
    None.
    */
    public IZatSecurityException(String error) {
        super(error);
    }
    /** Creates and IZatSecurityException object with the provided parameters.

    @param error Error string.
    @param cause Cause of the thrown exception. A NULL value is permitted
                     and indicates that the cause is nonexistent or unknown.
    */
    public IZatSecurityException(String error, Throwable cause) {
        super(error, cause);
    }
}
/** @} */
