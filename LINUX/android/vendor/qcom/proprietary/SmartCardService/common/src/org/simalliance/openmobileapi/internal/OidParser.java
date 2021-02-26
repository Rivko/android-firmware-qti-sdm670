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

import java.util.ArrayList;

/**
 * Class for encoding OID objects.
 */
public final class OidParser {

    /**
     * Override default constructor.
     */
    private OidParser() {
    }

    /**
     * Converts a dot-splitted OID string into the corresponding byte array.
     *
     * @param oid The String OID to be encoded.
     *
     * @return The OID as byte array.
     *
     * @throws IllegalArgumentException If oid has a bad coding.
     */
    public static byte[] encodeOid(String oid) throws IllegalArgumentException {
        ArrayList<Byte> byteList = new ArrayList<Byte>();
        String[] oidNumbers = oid.split("\\.");

        if (oidNumbers.length < 3) {
            throw new IllegalArgumentException();
        }

        int firstValue;
        int secondValue;
        try {
            firstValue = Integer.parseInt(oidNumbers[0]);
            secondValue = Integer.parseInt(oidNumbers[1]);
        } catch (Exception e) {
            throw new IllegalArgumentException();
        }
        byteList.add(encondeFirstDigits(firstValue, secondValue));

        for (int j = 2; j < oidNumbers.length; j++) {
            int number;
            try {
                number = Integer.parseInt(oidNumbers[j]);
            } catch (Exception e) {
                throw new IllegalArgumentException();
            }
            byte[] codification = encodeInteger(number);
            for (byte b : codification) {
                byteList.add(b);
            }
        }

        byte[] result = new byte[byteList.size()];
        for (int i = 0; i < byteList.size(); i++) {
            result[i] = byteList.get(i);
        }
        return result;
    }

    /**
     * Encodes the first two numbers of an oid.
     *
     * @param first The first number.
     * @param second The second number
     *
     * @return A byte representing the specified two digits
     *         according to the encoding rules.
     *
     * @throws IllegalArgumentException if any parameter is lower than 0.
     */
    public static byte encondeFirstDigits(int first, int second)
            throws IllegalArgumentException {
        if (first < 0 || second < 0) {
            throw new IllegalArgumentException();
        }
        return (byte) ((40 * first) + second);
    }

    /**
     * Encodes a number according to OID codification rules.
     *
     * @param number The number to be encoded.
     *
     * @return A byte array containing the encoding.
     *
     * @throws IllegalArgumentException If number is lower than 0.
     */
    public static byte[] encodeInteger(int number)
            throws IllegalArgumentException {
        if (number < 0) {
            throw new IllegalArgumentException();
        }
        if (number == 0) {
            return new byte[] {(byte) 0};
        }

        boolean isFirstIteration = true;
        byte[] result = new byte[0];
        while (number != 0) {
            // Increase the size of the result array
            byte[] aux = new byte[result.length];
            System.arraycopy(result, 0, aux, 0, result.length);
            result = new byte[result.length + 1];
            System.arraycopy(aux, 0, result, 1, aux.length);

            // Get the first 7 bits
            byte value = (byte) (number & 0x7F);
            if (isFirstIteration) {
                // On the first iteration we are getting the last byte,
                // so bit 8 shall be set to 0.
                // Since we already applied the mask, it is 0.
                isFirstIteration = false;
            } else {
                // On subsequent iterations, bit 8 shall be set to 1
                value = (byte) (value | 0x80);
            }
            // Add the value to the array
            result[0] = value;

            // Remove the bits that has been already encoded
            number = number >> 7;
        }

        return result;
    }
}
