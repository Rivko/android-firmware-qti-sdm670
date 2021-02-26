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
 * Class that wraps the functionality to parse BER-TLV data.
 */
public final class BerTlvParser extends TlvParser {

    @Override
    public byte[] getTagBytes(byte[] array, int position)
            throws IllegalArgumentException {
        byte[] tagBytes;

        if ((array[position] & 0x1F) != 0x1F) {
            // Tag is 1-byte long
            tagBytes = new byte[1];
        } else {
            // ISO-IEC 7816-4 5.2.2.1: In tag fields of two or more bytes,
            // the values '00' to '1E' and '80' are invalid for the second byte.
            if ((array[position + 1] >= 0x00 && array[position + 1] <= 0x1E)
                    || ((array[position + 1] & 0xFF) == 0x80)) {
                throw new IllegalArgumentException(
                        "Invalid \"tag\" field at position " + position + ".");
            }

            if ((array[position + 1] & 0x80) == 0) {
                // Tag is 2-byte long
                tagBytes = new byte[2];
            } else if ((array[position + 1] & 0x80) == 0x80
                    && (array[position + 2] & 0x80) == 0x00) {
                // Tag is 3-byte long
                tagBytes = new byte[3];
            } else {
                throw new IllegalArgumentException(
                        "Invalid \"tag\" field at position " + position + ".");
            }
        }

        System.arraycopy(array, position, tagBytes, 0, tagBytes.length);
        return tagBytes;
    }

    @Override
    public byte[] getLengthBytes(byte[] array, int position)
            throws IllegalArgumentException {
        byte[] length;
        if ((array[position] & 0xFF) < 0x80) {
            // Length is 1-byte long
            length = new byte[1];
        } else if ((array[position] & 0xFF) == 0x81) {
            // Length is 2-byte long
            length = new byte[2];
        } else if ((array[position] & 0xFF) == 0x82) {
            // Length is 3-byte long
            length = new byte[3];
        } else if ((array[position] & 0xFF) == 0x83) {
            // Length is 4-byte long
            length = new byte[4];
        } else if ((array[position] & 0xFF) == 0x84) {
            // Length is 5-byte long
            length = new byte[5];
        } else {
            throw new IllegalArgumentException(
                    "Invalid length field at position " + position + ".");
        }

        System.arraycopy(array, position, length, 0, length.length);
        return length;
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
