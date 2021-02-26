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
 * Class that wraps the values and basic functionality of a TLV entry.
 */
public class TlvEntryWrapper {
    /**
     * The Tag of the TLV entry.
     */
    private byte[] mTag;
    /**
     * The value of the TLV entry.
     */
    private byte[] mValue;
    /**
     * The total length of the entry.
     */
    private int mTotalLength;

    /**
     * Initializes a new instance of the TlvEntryWrapper class.
     *
     * @param data The data where the TLV object is.
     * @param startPosition The position where the TLV object starts.
     * @param parser The parser to use.
     *
     * @throws IllegalArgumentException If no TLV object is found.
     */
    public TlvEntryWrapper(byte[] data, int startPosition, TlvParser parser)
            throws IllegalArgumentException {
        int position = startPosition;

        mTag = parser.getTagBytes(data, startPosition);
        position += mTag.length;

        byte[] lengthBytes = parser.getLengthBytes(data, position);
        position += lengthBytes.length;

        mValue = new byte[parser.getLengthValue(lengthBytes)];
        System.arraycopy(data, position, mValue, 0, mValue.length);

        mTotalLength = mTag.length + lengthBytes.length + mValue.length;
    }

    /**
     * Gets the tag of the entry.
     *
     * @return The tag of the entry.
     */
    public byte[] getTag() {
        return mTag;
    }

    /**
     * Gets the total length of the entry.
     *
     * @return The total length of the entry.
     */
    public int getTotalLength() {
        return mTotalLength;
    }

    /**
     * Gets the value of the entry.
     *
     * @return The value of the entry.
     */
    public byte[] getValue() {
        return mValue;
    }

    /**
     * Encodes the current object.
     *
     * @return An array of bytes representing the Der-codified object.
     */
    public byte[] encode() {
        byte[] encodedObject = new byte[mTotalLength];
        System.arraycopy(mTag, 0, encodedObject, 0, mTag.length);
        byte[] lengthBytes = DerTlvCoder.encodeLength(mValue.length);
        System.arraycopy(lengthBytes, mTag.length, encodedObject, mTag.length,
                lengthBytes.length);
        System.arraycopy(mValue, 0, encodedObject, mTag.length
                + lengthBytes.length, mValue.length);
        return encodedObject;
    }
}
