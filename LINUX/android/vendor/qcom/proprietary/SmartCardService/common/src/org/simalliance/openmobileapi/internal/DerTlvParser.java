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
 * Class that parses DER-Coded byte arrays.
 */
public class DerTlvParser extends TlvParser {

    @Override
    public byte[] getTagBytes(byte[] array, int position)
            throws IllegalArgumentException {
        byte[] tagBytes;

        if ((array[position] & 0x1F) != 0x1F) {
            // Tag is 1-byte long
            tagBytes = new byte[1];
        } else {
            int tmpPosition = position;
            tmpPosition++;
            // ITU X.690: bits 7 to 1 of the first subsequent
            // octet shall not all be zero.
            if ((array[tmpPosition] & 0x7F) == 0x00) {
                throw new IllegalArgumentException(
                        ErrorStrings.TLV_INVALID_TAG);
            }

            // The length of the tag
            int length = 2;
            // Loop until a zero is found in the bit 8 of the array
            while ((array[tmpPosition] & 0x80) == 0x80) {
                length++;
                tmpPosition++;
            }

            tagBytes = new byte[length];
        }

        System.arraycopy(array, position, tagBytes, 0, tagBytes.length);
        return tagBytes;
    }

    @Override
    public byte[] getLengthBytes(byte[] array, int position)
            throws IllegalArgumentException {
        byte[] lengthBytes;

        if ((array[position] & 0x80) != 0x80) {
            // Short form
            lengthBytes = new byte[1];
        } else {
            // ITU X.690: in the long form, [...] the value
            // 0xFF shall not be used [for the first byte].
            if ((array[position] & 0xFF) == 0xFF) {
                throw new IllegalArgumentException(
                        ErrorStrings.TLV_INVALID_LENGTH);
            }

            // Bits 7 to 1 indicate the number of subsequent octets
            lengthBytes = new byte[1 + (array[position] & 0x7F)];
        }

        System.arraycopy(array, position, lengthBytes, 0, lengthBytes.length);
        return lengthBytes;
    }

    @Override
    public int getLengthValue(byte[] lengthBytes) {
        int valueStartPosition;
        byte[] lengthValue;

        if (lengthBytes.length == 1) {
            valueStartPosition = 0;
            lengthValue = new byte[1];
        } else {
            valueStartPosition = 1;
            lengthValue = new byte[lengthBytes.length - 1];
        }

        System.arraycopy(lengthBytes, valueStartPosition, lengthValue, 0,
                lengthValue.length);
        return ByteArrayConverter.byteArrayToInt(lengthValue);
    }
}
