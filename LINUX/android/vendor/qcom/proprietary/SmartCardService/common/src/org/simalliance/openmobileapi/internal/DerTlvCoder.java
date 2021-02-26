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

// TODO: implement more type encoders.

/**
 * Class that implements functionalities over DER-coded objects.
 */
public final class DerTlvCoder {

    /**
     * Tag for INTEGER-type objects.
     */
    public static final byte[] TAG_INTEGER = {(byte) 0x02};
    /**
     * Tag for OCTET STRING-type objects.
     */
    public static final byte[] TAG_OCTET_STRING = {(byte) 0x04};
    /**
     * Tag for SEQUENCE-type objects.
     */
    public static final byte[] TAG_SEQUENCE = {(byte) 0x30};

    /**
     * Override default constructor to avoid instantiations.
     */
    private DerTlvCoder() {
    }

    /**
     * Returns an array of bytes representing the specified length.
     *
     * @param lengthValue The length to be represented.
     *
     * @return An array of bytes representing the specified length.
     *
     * @throws IllegalArgumentException if the length is less than 0.
     */
    public static byte[] encodeLength(int lengthValue)
            throws IllegalArgumentException {
        if (lengthValue < 0) {
            throw new IllegalArgumentException(
                    ErrorStrings.paramInvalidValue("lengthValue"));
        }
        // Get the byte array representing the length
        byte[] rawLength = ByteArrayConverter.intToByteArray(lengthValue);

        byte[] encodedLength;
        if (lengthValue <= 0x7F) {
            // Use short form: bit 8 set to 0
            encodedLength = new byte[1];
            encodedLength[0] = (byte) (0x7F & rawLength[3]);
        } else if (lengthValue <= 0x0FF) {
            // Use long form (2 bytes)
            encodedLength = new byte[2];
            encodedLength[0] = (byte) 0x81;
            encodedLength[1] = rawLength[3];
        } else if (lengthValue <= 0xFFFF) {
            // Use long form (3 bytes)
            encodedLength = new byte[3];
            encodedLength[0] = (byte) 0x82;
            encodedLength[1] = rawLength[2];
            encodedLength[2] = rawLength[3];
        } else if (lengthValue <= 0xFFFFFF) {
            // Use long form (4 bytes)
            encodedLength = new byte[4];
            encodedLength[0] = (byte) 0x83;
            encodedLength[1] = rawLength[1];
            encodedLength[2] = rawLength[2];
            encodedLength[3] = rawLength[3];
        } else {
            // Use long form (5 bytes)
            encodedLength = new byte[5];
            encodedLength[0] = (byte) 0x84;
            encodedLength[1] = rawLength[0];
            encodedLength[2] = rawLength[1];
            encodedLength[3] = rawLength[2];
            encodedLength[4] = rawLength[3];
        }

        return encodedLength;
    }

    /**
     * Encodes an object of type "INTEGER".
     *
     * @param value The value of the integer to be encoded.
     *
     * @return A byte array representing the integer in DER format.
     */
    public static byte[] encodeInteger(int value) {
        byte[] valueByteArray = ByteArrayConverter.intToByteArray(value);

        // Delete leading bytes that are equal to 0 to
        // ensure that the minimum number of bytes is used
        while (valueByteArray[0] == 0x00) {
            byte[] tmp = new byte[valueByteArray.length - 1];
            System.arraycopy(valueByteArray, 1, tmp, 0, tmp.length);
            valueByteArray = new byte[tmp.length];
            System.arraycopy(tmp, 0, valueByteArray, 0, tmp.length);
        }

        byte[] lengthByteArray = encodeLength(valueByteArray.length);

        byte[] encodedInteger = new byte[TAG_INTEGER.length
                + lengthByteArray.length + valueByteArray.length];

        System.arraycopy(TAG_INTEGER, 0, encodedInteger, 0,
                TAG_INTEGER.length);
        System.arraycopy(lengthByteArray, 0, encodedInteger,
                TAG_INTEGER.length, lengthByteArray.length);
        System.arraycopy(valueByteArray, 0, encodedInteger,
                TAG_INTEGER.length + lengthByteArray.length,
                valueByteArray.length);

        return encodedInteger;
    }

    /**
     * Encodes an object of type "OCTET STRING".
     *
     * @param octetString the object to be encoded.
     *
     * @return A byte array representing the octet string in DER format.
     */
    public static byte[] encodeOctetString(byte[] octetString) {
        byte[] lengthByteArray = encodeLength(octetString.length);

        byte[] encodedOctetString = new byte[TAG_OCTET_STRING.length
                + lengthByteArray.length + octetString.length];

        System.arraycopy(TAG_OCTET_STRING, 0, encodedOctetString, 0,
                TAG_OCTET_STRING.length);
        System.arraycopy(lengthByteArray, 0, encodedOctetString,
                TAG_OCTET_STRING.length, lengthByteArray.length);
        System.arraycopy(octetString, 0, encodedOctetString,
                TAG_OCTET_STRING.length + lengthByteArray.length,
                octetString.length);

        return encodedOctetString;
    }

    /**
     * Encodes an object of type "SEQUENCE".
     *
     * @param sequence The sequence to be encoded.
     *
     * @return A byte array representing the sequence in DER format.
     */
    public static byte[] encodeSequence(byte[] sequence) {
        byte[] lengthByteArray = encodeLength(sequence.length);

        byte[] encodedSequence = new byte[TAG_SEQUENCE.length
                + lengthByteArray.length + sequence.length];

        System.arraycopy(TAG_SEQUENCE, 0, encodedSequence, 0,
                TAG_SEQUENCE.length);
        System.arraycopy(lengthByteArray, 0, encodedSequence,
                TAG_SEQUENCE.length, lengthByteArray.length);
        System.arraycopy(sequence, 0, encodedSequence, TAG_SEQUENCE.length
                + lengthByteArray.length, sequence.length);

        return encodedSequence;
    }
}
