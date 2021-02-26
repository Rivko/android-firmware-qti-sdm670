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
import java.util.Arrays;
import java.util.List;

/**
 * Abstract class that must be extended by any TLV parser.
 */
public abstract class TlvParser {

    /**
     * Extracts the type field that starts at the specified position.
     *
     * @param array The array that contains the type field to be extracted.
     *
     * @param position The start position of the type field.
     *
     * @return A byte array containing the type field found at the specified
     * position.
     *
     * @throws IllegalArgumentException if no valid tag is found at the
     * specified position.
     */
    abstract byte[] getTagBytes(byte[] array, int position)
            throws IllegalArgumentException;

    /**
     * Extracts the length field that starts at the specified position.
     *
     * @param array The array that contains the length field to be extracted.
     *
     * @param position The position where the length field starts.
     *
     * @return A byte array containing the length field.
     *
     * @throws IllegalArgumentException if no valid length field is found at the
     * specified position.
     */
    abstract byte[] getLengthBytes(byte[] array, int position)
            throws IllegalArgumentException;

    /**
     * Converts a length field to an integer value.
     *
     * @param lengthBytes The length field.
     *
     * @return The integer value of the length field.
     */
    abstract int getLengthValue(byte[] lengthBytes);

    /**
     * Parses the full TLV array.
     *
     * @param array The array to be parsed.
     *
     * @return The list of TLV objects found in the array.
     */
    public List<TlvEntryWrapper> parseArray(byte[] array) {
        byte[] data = getValidTlvData(array);

        int position = 0;
        ArrayList<TlvEntryWrapper> list = new ArrayList<TlvEntryWrapper>();

        while (position < data.length) {
            TlvEntryWrapper nextEntry
                    = new TlvEntryWrapper(data, position, this);
            list.add(nextEntry);
            position += nextEntry.getTotalLength();
        }

        return (List<TlvEntryWrapper>) list;
    }

    /**
     * Searches for the first occurrence of the specified tag after the
     * specified position.
     *
     * @param data The data where to find the tag.
     * @param tag The tag to search for.
     * @param startPosition The position where to start the search.
     *
     * @return The position where the tag is found.
     *
     * @throws IllegalArgumentException if startPosition is lower than 0.
     * @throws IllegalArgumentException if the tag could not be found.
     * @throws IllegalArgumentException if data is not correctly coded.
     */
    public int searchTag(byte[] data, byte[] tag, int startPosition)
            throws IllegalArgumentException {
        if (startPosition < 0) {
            throw new IllegalArgumentException(
                    ErrorStrings.paramInvalidValue("startPosition"));
        }

        int position = startPosition;
        while (position < data.length) {
            TlvEntryWrapper derEntry
                    = new TlvEntryWrapper(data, position, this);
            if (Arrays.equals(tag, derEntry.getTag())) {
                return position;
            } else {
                position += derEntry.getTotalLength();
            }
        }

        // If data array has been parsed and tag has not been found...
        throw new IllegalArgumentException(ErrorStrings.TLV_TAG_NOT_FOUND);
    }

    /**
     * Extracts a valid TLV array at the beginning of the specified array.
     *
     * @param rawData The data to extract the valid TLF from.
     *
     * @return A byte array containing a valid TLV structure found at the
     *         beginning of rawData.
     */
    public byte[] getValidTlvData(byte[] rawData) {
        if (isValidTlvStructure(rawData)) {
            return rawData;
        } else {
            int position = 0;
            while (true) {
                try {
                    int tmpPosition = position;
                    byte[] tag = getTagBytes(rawData, tmpPosition);
                    tmpPosition += tag.length;

                    byte[] length = getLengthBytes(rawData, tmpPosition);
                    tmpPosition += length.length;

                    tmpPosition += getLengthValue(length);

                    if (tmpPosition > rawData.length) {
                        break;
                    } else {
                        position = tmpPosition;
                    }
                } catch (Exception e) {
                    // If parsing fails...
                    break;
                }
            }

            byte[] validData = new byte[position];
            System.arraycopy(rawData, 0, validData, 0, position);
            return validData;
        }
    }

    /**
     * Checks that the length of the array is consistent with the TLV structure.
     *
     * @param data The data to be checked.
     *
     * @return true if the data contains a valid TLV structure, false otherwise.
     */
    public boolean isValidTlvStructure(byte[] data) {
        int position = 0;
        while (position < data.length) {
            try {
                // Parse tag
                byte[] tag = getTagBytes(data, position);
                position += tag.length;

                // Parse length
                byte[] length = getLengthBytes(data, position);
                position += length.length;

                // Increase position
                position += getLengthValue(length);
            } catch (Exception e) {
                // If parsing fails...
                return false;
            }
        }

        return position == data.length;
    }
}
