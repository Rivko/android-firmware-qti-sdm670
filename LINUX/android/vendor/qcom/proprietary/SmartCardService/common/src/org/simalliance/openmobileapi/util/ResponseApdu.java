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

package org.simalliance.openmobileapi.util;

/**
 * This object represents a response APDU as specified by ISO/IEC 7816.
 *
 * @author Giesecke & Devrient
 *
 */
public class ResponseApdu {

    /**
     * DATA field of response APDU.
     */
    private byte[] mData;

    /**
     * STATUS WORD field of response APDU.
     */
    private byte[] mSw;

    /**
     * Creates a response APDU.
     *
     * @param response The response APDU as a byte array.
     */
    public ResponseApdu(byte[] response) {
        if (response == null) {
            throw new IllegalArgumentException("Response must not be null.");
        }
        if (response.length < ISO7816.RESP_APDU_LENGTH_SW
                || response.length > ISO7816.MAX_RESPONSE_DATA_LENGTH
                + ISO7816.RESP_APDU_LENGTH_SW) {
            throw new IllegalArgumentException(
                    "Invalid response length (" + response.length + ").");
        }
        if (response.length > ISO7816.RESP_APDU_LENGTH_SW) {
            mData = new byte[response.length - ISO7816.RESP_APDU_LENGTH_SW];
            System.arraycopy(response, 0, mData, 0, mData.length);
        }
        mSw = new byte[ISO7816.RESP_APDU_LENGTH_SW];
        System.arraycopy(response, response.length
                - ISO7816.RESP_APDU_LENGTH_SW, mSw, 0,
                ISO7816.RESP_APDU_LENGTH_SW);
    }

    /**
     * Returns the DATA field of the response if present, null otherwise.
     *
     * @return The DATA field of the response if present, null otherwise.
     */
    public byte[] getData() {
        return mData;
    }

    /**
     * Returns the STATUS WORD field of the response.
     *
     * @return The STATUS WORD field of the response.
     */
    public byte[] getSw() {
        return mSw;
    }

    /**
     * Returns the STATUS WORD field as int value.
     *
     * @return The STATUS WORD field as int value.
     */
    public int getSwValue() {
        return ((mSw[0] & 0x0FF) << 8) + (mSw[1] & 0x0FF);
    }

    /**
     * Returns true if the SW = 90 00, false otherwise.
     *
     * @return true if the SW = 90 00, false otherwise..
     */
    public boolean isSuccess() {
        return getSwValue() == ISO7816.SW_NO_FURTHER_QUALIFICATION;
    }

    /**
     * Returns true if the SW = 62 XX or SW = 63 XX, false otherwise.
     *
     * @return true if the SW = 62 XX or SW = 63 XX, false otherwise.
     */
    public boolean isWarning() {
        return mSw[0] == ISO7816.SW1_62 || mSw[0] == ISO7816.SW1_63;
    }

    /**
     * Returns the first byte of STATUS WORD field as int value.
     *
     * @return The first byte of STATUS WORD field as int value.
     */
    public int getSw1Value() {
        return (int) 0x0FF & mSw[0];
    }

    /**
     * Returns the second byte of STATUS WORD field as int value.
     *
     * @return The second byte of STATUS WORD field as int value.
     */
    public int getSw2Value() {
        return (int) 0x0FF & mSw[1];
    }
}
