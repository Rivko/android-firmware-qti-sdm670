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
 * Class that wraps some functionalities for the HistoricalBytes.
 */
public final class HistoricalBytesUtilities {

    /**
     * Override default constructor.
     */
    private HistoricalBytesUtilities() {
    }

    /**
     * Parses an ATR and returns its historical bytes.
     *
     * @param atr The ATR where to get the historical bytes from.
     *
     * @return The historical bytes of the ATR.
     */
    public static byte[] getHistBytes(byte[] atr) {
        // Set the next byte to be parsed. First byte is TS, so it will be
        // ignored.
        int position = 1;

        // Get byte T0 of ATR.
        byte tdi = atr[position];

        // Set the boolean that indicates whether any of TAi+1, TBi+1, TCi+1,
        // and TDi+1 are present
        boolean areNextInterfaceBytesPresent = (tdi & 0xF0) != 0;

        // Initialize a List of all "T" values present in T0, Y1, Y2...
        ArrayList<Integer> tValues = new ArrayList<Integer>();
        // and get the first value of T.
        tValues.add(tdi & 0x0F);

        // Set the next position to be parsed
        position++;

        while (areNextInterfaceBytesPresent) {
            // Start parsing the next Interface bytes.

            // If TAi+1 is present increase position
            if ((tdi & 0x10) != 0) {
                position++;
            }

            // If TBi+1 is present increase position
            if ((tdi & 0x20) != 0) {
                position++;
            }

            // If TCi+1 is present increase position
            if ((tdi & 0x40) != 0) {
                position++;
            }

            // If TDi+1 is present...
            if ((tdi & 0x80) != 0) {
                // Update TDi
                tdi = atr[position];
                // Check if next Interface bytes are present
                areNextInterfaceBytesPresent = (tdi & 0xF0) != 0;
                // Get the next value of T
                tValues.add(tdi & 0x0F);
                // Increase the position
                position++;
            } else {
                // If TD is not present, there are no more interface bytes.
                areNextInterfaceBytesPresent = false;
            }
        }

        // Decide the length of historical bytes
        int length = atr.length - position;
        // If TCK byte is present, historical bytes are one byte shorter.
        if (isTckPresent(tValues)) {
            length--;
        }
        byte[] historicalBytes = new byte[length];
        System.arraycopy(atr, position, historicalBytes, 0, length);
        return historicalBytes;
    }

    /**
     * Decides whether TCK should be present based in the ATR on the "T" values
     * found when parsing the ATR.
     *
     * From ISO/IEC 7816-3 8.2.5: "If only T=0 is indicated, possibly by
     * default, then TCK shall be absent. If T=0 and T=15 are present and in all
     * the other cases, TCK shall be present."
     *
     * @param tValues The values of "T" found when parsing the ATR.
     *
     * @return true if TCK should be present, false otherwise.
     */
    private static boolean isTckPresent(ArrayList<Integer> tValues) {
        return !((tValues.size() == 1) && tValues.contains(0));
    }
}
