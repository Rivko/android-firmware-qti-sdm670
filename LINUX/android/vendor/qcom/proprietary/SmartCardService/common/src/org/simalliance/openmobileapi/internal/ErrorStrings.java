/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright 2013 Giesecke & Devrient GmbH.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package org.simalliance.openmobileapi.internal;

/**
 * Class that contains the error strings that will be used throughout the APIs.
 */
public final class ErrorStrings {

    /**
     * Override the default constructor to not allow the instantiation
     * of the class.
     */
    private ErrorStrings() {
    }

    // Errors related to parameters

    /**
     * Parameter <parameterName> must not be null.
     *
     * @param parameterName The name of the parameter that can't be null.
     *
     * @return Parameter <parameterName> must not be null.
     */
    public static String paramNull(String parameterName) {
        return "Parameter " + parameterName + " must not be null.";
    }

    /**
     * Parameter <parameterName> has an invalid length.
     *
     * @param parameterName The name of the parameter that has an
     * invalid length.
     *
     * @return Parameter <parameterName> has an invalid length.
     */
    public static String paramInvalidArrayLength(String parameterName) {
        return "Parameter " + parameterName + " has an invalid length.";
    }

    /**
     * Parameter <parameterName> has an invalid value.
     *
     * @param parameterName The name of the parameter that has an invalid value.
     *
     * @return Parameter <parameterName> has an invalid value.
     */
    public static String paramInvalidValue(String parameterName) {
        return "Parameter " + parameterName + " has an invalid value.";
    }

    // Errors related to APDU formation and exchange

    /**
     * APDU has an invalid format.
     */
    public static final String APDU_WRONG_FORMAT
    = "APDU has an invalid format.";

    /**
     * Response APDU has a bad coding.
     */
    public static final String APDU_BAD_RESPONSE
    = "Response APDU has a bad coding.";

    /**
     * Channel is closed.
     */
    public static final String CHANNEL_CLOSED = "Channel is closed.";

    // Errors related to PIN operations

    /**
     * Wrong PIN.
     */
    public static final String PIN_WRONG = "Wrong PIN.";

    /**
     * Referenced PIN could not be found.
     */
    public static final String PIN_REF_NOT_FOUND
        = "Referenced PIN could not be found.";

    /**
     * Referenced PIN is blocked.
     */
    public static final String PIN_BLOCKED
        = "Referenced PIN is blocked.";


    // Errors related to files

    /**
     * File not found.
     */
    public static final String FILE_NOT_FOUND
        = "File not found.";

    /**
     * Invalid File ID.
     * File ID must be between FID_MIN_VALUE and FID_MAX_VALUE.
     */
    public static final String INVALID_FID
        = "Invalid File ID. File ID must be between ";
                //+ FileViewProvider.FID_MIN_VALUE + " and "
                //+ FileViewProvider.FID_MAX_VALUE + ".";

    /**
     * Invalid Short File ID. SFI must be between
     * FileViewProvider.SFI_MIN_VALUE and FileViewProvider.SFI_MAX_VALUE,
     * or be FileViewProvider.CURRENT_FILE.
     */
    public static final String INVALID_SFI
        = "Invalid Short File ID. SFI must be between ";
                //+ FileViewProvider.SFI_MIN_VALUE + " and "
                //+ FileViewProvider.SFI_MAX_VALUE + ", or "
                //+ "be FileViewProvider.CURRENT_FILE.";

    /**
     * The selected file is not transparent.
     */
    public static final String NO_TRANSPARENT_FILE = "The selected file is not"
            + " transparent.";

    /**
     * The selected file is not record-based.
     */
    public static final String NO_RECORD_BASED_FILE = "The selected"
            + " file is not record-based.";

    /**
     * Offset outside EF.
     */
    public static final String OFFSET_OUTSIDE_EF = "Offset outside EF.";

    /**
     * Record not found.
     */
    public static final String RECORD_NOT_FOUND = "Record not found.";

    // Errors related to TLV parsing

    /**
     * Tag not found.
     */
    public static final String TLV_TAG_NOT_FOUND = "Tag not found.";

    /**
     * Unexpected tag.
     */
    public static final String TLV_TAG_UNEXPECTED = "Unexpected tag.";

    /**
     * Invalid tag.
     */
    public static final String TLV_INVALID_TAG = "Invalid tag.";

    /**
     * Invalid length field.
     */
    public static final String TLV_INVALID_LENGTH = "Invalid length field.";

    // Other messages

    /**
     * Referenced data not found.
     */
    public static final String REF_NOT_FOUND
        = "Referenced data not found.";

    /**
     * This operation is not supported by the selected Applet.
     */
    public static final String OPERATION_NOT_SUPORTED
        = "This operation is not supported by the selected Applet.";

    /**
     * Invalid CLA.
     */
    public static final String INVALID_CLA
        = "Invalid class";

    /**
     * Wrong length.
     */
    public static final String WRONG_LENGTH = "Wrong length.";

    /**
     * Security status not satisfied.
     */
    public static final String SECURITY_STATUS_NOT_SATISFIED
        = "Security status not satisfied.";

    /**
     * Memory failure.
     */
    public static final String MEMORY_FAILURE = "Memory failure.";

    /**
     * No file is currently selected.
     */
    public static final String NO_CURRENT_FILE =
            "No file is currently selected.";

    /**
     * Not enough memory space in the file.
     */
    public static final String NOT_ENOUGH_MEMORY =
            "Not enough memory space in the file.";

    /**
     * Authentication method blocked.
     */
    public static final String AUTH_METHOD_BLOCKED
        = "PIN is blocked.";

    /**
     * Unexpected Status Word: 0x<swValue>.
     *
     * @param swValue The value of the SW.
     *
     * @return Unexpected Status Word: 0x<swValue>.
     */
    public static String unexpectedStatusWord(int swValue) {
        return "Unexpected Status Word: 0x"
                + String.format("%02x", swValue) + ".";
    }

    // PKCS#15 error strings

    /**
     * No PKCS#15 file structure found.
     */
    public static final String PKCS15_NO_FS =
            "No PKCS#15 file structure found.";

    // Secure Storage error strings.

    /**
     * Channel is not connected to a Secure Storage applet.
     */
    public static final String SES_APP_NOT_PRESENT
        = "Channel is not connected to a Secure Storage applet.";

    /**
     * File creation failed due to memory issues.
     */
    public static final String SES_CREATE_FAILED_MEMORY
        = "File creation failed due to memory issues.";

    /**
     * Security exception.
     */
    public static final String SES_SECURITY_EXCEPTION =
            "The PIN to access the Secure Storage Applet has not been verified";
    /**
     * Channel is close.
     */
    public static final String SES_CHANNEL_CLOSE =
            "The channel is close";
    /**
     * There is an incorrect title of the file.
     */
    public static final String SES_INCORRECT_TITLE =
            "The title is incorrect: bad encoding or wrong length.";

    /**
     * IOError due to an incomplete read procedure.
     */
    public static final String SES_IOERROR_READ =
            "The entry could not be read because an incomplete read procedure.";

    /**
     * IOError due to an incomplete write procedure.
     */
    public static final String SES_IOERROR_WRITE =
            "The entry could'b be read because an incomplete write procedure.";

    /**
     * The title is repeated.
     */
    public static final String SES_TITLE_NOT_EXISTS =
            "The written title does not exist.";

    /**
     * Incorrect values in the command data.
     */
    public static final String SES_TITLE_EXISTS =
            "The specified title already exists.";

    /**
     * Not enough memory space.
     */
    public static final String SES_NOT_ENOUGH_MEMORY =
            "Not enough memory space";

    /**
     * Referenced data not found (if no SeS entry is currently selected.
     */
    public static final String SES_NO_ENTRY_SELECTED =
            "Referenced data not found (if no SeS entry is currently selected";
    /**
     * Referenced data not found (if no SeS entry not exists.
     */
    public static final String SES_ENTRY_NOT_EXISTS =
            "Referenced data not found (if no SeS entry not exists";
    /**
     * If title is empty.
     */
    public static final String SES_EMPTY_TITLE =
            "The title is empty";

    /**
     * If title is too long (max value 60 chars).
     */
    public static final String SES_LONG_TITLE =
            "The title is too long (max value 60 chars).";

}
