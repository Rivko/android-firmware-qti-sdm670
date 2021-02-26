/* ======================================================================
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

/** @addtogroup IZatIllegalArgumentException
    @{ */

/**
 * An exception indicating illegal argument(s) in calling the FLP service.
 * @cond
 * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * @endcond
 */
public class IZatIllegalArgumentException extends RuntimeException{
    public IZatIllegalArgumentException() {
    }
    /** Creates an IZatIllegalArgumentException object.

        @param error Error string.
        @return
        None.
    */
    public IZatIllegalArgumentException(String error) {
        super(error);
    }
    /** Creates an IZatIllegalArgumentException object with the provided parameters.

        @param error Error string.
        @param cause Cause of the thrown exception. A NULL value is permitted
                         and indicates that the cause in nonexistent or unknown.
        @ return
        None.
    */
    public IZatIllegalArgumentException(String error, Throwable cause) {
        super(error, cause);
    }
}
/** @} */