/* ======================================================================
*  Copyright (c) 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/** @addtogroup IZatStaleDataException
@{ */

/**
 * An exception indicating access of stale or invalid data.
 * @cond
 * <p>Copyright (c) 2017 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * @endcond
 */
public class IZatStaleDataException extends RuntimeException{
    public IZatStaleDataException() {
    }
    /** Creates an IZatStaleDataException object.

    @param error Error string.
    @return
    None.
    */
    public IZatStaleDataException(String error) {
        super(error);
    }
    /** Creates an IZatStaleDataException object with the provided parameters.

    @param error Error string.
    @param cause Cause of the thrown exception. A NULL value is permitted
                     and indicates that the cause is nonexistent or unknown.
    @return
    None.
    */
    public IZatStaleDataException(String error, Throwable cause) {
        super(error, cause);
    }
}
/** @} */
