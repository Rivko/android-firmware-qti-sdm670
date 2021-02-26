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
 * This object represents a command APDU as specified by ISO/IEC 7816.
 *
 * @author Giesecke & Devrient
 *
 */
public class CommandApdu {

    /**
     * CLA field of command APDU.
     */
    private byte mCla;

    /**
     * INS field of command APDU.
     */
    private byte mIns;

    /**
     * P1 field of command APDU.
     */
    private byte mP1;

    /**
     * P2 field of command APDU.
     */
    private byte mP2;

    /**
     * DATA field of command APDU.
     */
    private byte[] mData;

    /**
     * Le field of command APDU.
     */
    private Integer mLe;

    /**
     * Override default constructor.
     */
    private CommandApdu() {
    }

    /**
     * Creates a case 1 Command APDU.
     *
     * @param cla Value of CLA field.
     * @param ins Value of INS field.
     * @param p1 Value of P1 field.
     * @param p2 Value of P2 field.
     *
     * @throws IllegalArgumentException if CLA or INS are invalid.
     */
    public CommandApdu(byte cla, byte ins, byte p1, byte p2)
            throws IllegalArgumentException {
        setCla(cla);
        setIns(ins);
        setP1(p1);
        setP2(p2);
    }

    /**
     * Creates a case 2 Command APDU.
     *
     * @param cla Value of CLA field.
     * @param ins Value of INS field.
     * @param p1 Value of P1 field.
     * @param p2 Value of P2 field.
     * @param le Value of Le field.
     *
     * @throws IllegalArgumentException if CLA, INS or Le are invalid.
     */
    public CommandApdu(byte cla, byte ins, byte p1, byte p2, int le)
            throws IllegalArgumentException {
        setCla(cla);
        setIns(ins);
        setP1(p1);
        setP2(p2);
        setLe(le);
    }

    /**
     * Creates case 3 Command APDU.
     *
     * @param cla Value of CLA field.
     * @param ins Value of INS field.
     * @param p1 Value of P1 field.
     * @param p2 Value of P2 field.
     * @param data Value of DATA field.
     *
     * @throws IllegalArgumentException if CLA, INS or Data are invalid.
     */
    public CommandApdu(byte cla, byte ins, byte p1, byte p2, byte[] data)
            throws IllegalArgumentException {
        setCla(cla);
        setIns(ins);
        setP1(p1);
        setP2(p2);
        setData(data);
    }

    /**
     * Creates a case 4 Command APDU.
     *
     * @param cla Value of CLA field.
     * @param ins Value of INS field.
     * @param p1 Value of P1 field.
     * @param p2 Value of P2 field.
     * @param data Value of DATA field.
     * @param le Value of Le field.
     *
     * @throws IllegalArgumentException if CLA, INS, Data or Le are invalid.
     */
    public CommandApdu(
            byte cla, byte ins, byte p1, byte p2, byte[] data, int le)
                    throws IllegalArgumentException {
        setCla(cla);
        setIns(ins);
        setP1(p1);
        setP2(p2);
        setData(data);
        setLe(le);
    }

    /**
     * Creates a command APDU object from a byte Array.
     *
     * @param cmdApduAsByteArray Command APDU as byte array.
     *
     * @throws IllegalArgumentException If cmdApduAsByteArray does not
     * contain a valid APDU.
     */
    public CommandApdu(byte[] cmdApduAsByteArray)
            throws IllegalArgumentException {
        if (cmdApduAsByteArray.length < ISO7816.CMD_APDU_LENGTH_CASE1) {
            throw new IllegalArgumentException("Invalid length for command ("
                + cmdApduAsByteArray.length + ").");
        }

        setCla(cmdApduAsByteArray[ISO7816.OFFSET_CLA]);
        setIns(cmdApduAsByteArray[ISO7816.OFFSET_INS]);
        setP1(cmdApduAsByteArray[ISO7816.OFFSET_P1]);
        setP2(cmdApduAsByteArray[ISO7816.OFFSET_P2]);

        if (cmdApduAsByteArray.length == ISO7816.CMD_APDU_LENGTH_CASE1) {
            // Case 1 APDU -- Nothing left to be done
        } else if (cmdApduAsByteArray.length == ISO7816.CMD_APDU_LENGTH_CASE2) {
            // Case 2 APDU
            setLe((int) 0x0FF & cmdApduAsByteArray[ISO7816.OFFSET_P3]);
        } else if (cmdApduAsByteArray[ISO7816.OFFSET_P3] != (byte) 0x00) {
            // Case 3 or Case 4 APDU

            // Get Lc and check that it's not 0
            int lc = ((int) 0x0FF & cmdApduAsByteArray[ISO7816.OFFSET_P3]);
            if (lc == 0) {
                throw new IllegalArgumentException(
                        "Lc can't be 0");
            }

            if (cmdApduAsByteArray.length
                    == ISO7816.CMD_APDU_LENGTH_CASE3_WITHOUT_DATA + lc) {
                // Case 3 APDU -- nothing to be done here
            } else if (cmdApduAsByteArray.length
                    == ISO7816.CMD_APDU_LENGTH_CASE4_WITHOUT_DATA + lc) {
                // Case 4 APDU -- get Le:
                setLe((int) 0x0FF
                        & cmdApduAsByteArray[cmdApduAsByteArray.length - 1]);
            } else {
                // Lc has a wrong value!
                throw new IllegalArgumentException(
                        "Unexpected value of Lc (" + lc + ")");
            }

            // Store the data
            mData = new byte[lc];
            System.arraycopy(
                        cmdApduAsByteArray,
                        ISO7816.OFFSET_DATA,
                        mData,
                        0,
                        lc);
        } else  if (cmdApduAsByteArray.length
                == ISO7816.CMD_APDU_LENGTH_CASE2_EXTENDED) {
            // Case 2 extended APDU
            setLe((((int) 0x0FF
                    & cmdApduAsByteArray[ISO7816.OFFSET_DATA]) << 8)
                    + ((int) 0x0FF
                            & cmdApduAsByteArray[ISO7816.OFFSET_DATA + 1]));
        } else {
            // Case 3 or Case 4 APDU

            if (cmdApduAsByteArray.length <= ISO7816.OFFSET_DATA_EXTENDED) {
                throw new IllegalArgumentException(
                        "Unexpected value of Lc or Le" + cmdApduAsByteArray.length);
            }
            // Get Lc and check that it's not 0
            int lc = (((int) 0x0FF
                    & cmdApduAsByteArray[ISO7816.OFFSET_DATA]) << 8)
                    + ((int) 0x0FF
                            & cmdApduAsByteArray[ISO7816.OFFSET_DATA + 1]);
            if (lc == 0) {
                throw new IllegalArgumentException(
                        "Lc can't be 0");
            }

            if (cmdApduAsByteArray.length
                    == ISO7816.CMD_APDU_LENGTH_CASE3_WITHOUT_DATA_EXTENDED
                    + lc) {
                // Case 3 APDU -- nothing to be done here
            } else if (cmdApduAsByteArray.length
                    == ISO7816.CMD_APDU_LENGTH_CASE4_WITHOUT_DATA_EXTENDED
                    + lc) {
                // Case 4 APDU -- get Le:
                setLe((((int) 0x0FF
                        & cmdApduAsByteArray[cmdApduAsByteArray.length - 2])
                        << 8)
                        + ((int) 0x0FF
                        & cmdApduAsByteArray[cmdApduAsByteArray.length - 1]));
            } else {
                // Lc has a wrong value!
                throw new IllegalArgumentException(
                        "Unexpected value of Lc (" + lc + ")--- 9 -" + cmdApduAsByteArray.length);
            }

            // Store the data
            mData = new byte[lc];
            System.arraycopy(
                        cmdApduAsByteArray,
                        ISO7816.OFFSET_DATA_EXTENDED,
                        mData,
                        0,
                        lc);
        }
    }

    /**
     * Returns this Command APDU as a byte array.
     *
     * @return Command APDU as byte array.
     */
    public byte[] toByteArray() {
        byte[] array;
        if (!isExtendedLength()) {
            if (mData == null && mLe == null) {
                // APDU Case 1
                array = new byte[ISO7816.CMD_APDU_LENGTH_CASE1];
                array[ISO7816.OFFSET_CLA] = mCla;
                array[ISO7816.OFFSET_INS] = mIns;
                array[ISO7816.OFFSET_P1] = mP1;
                array[ISO7816.OFFSET_P2] = mP2;
            } else if (mData == null && mLe != null) {
                // APDU Case 2
                array = new byte[ISO7816.CMD_APDU_LENGTH_CASE2];
                array[ISO7816.OFFSET_CLA] = mCla;
                array[ISO7816.OFFSET_INS] = mIns;
                array[ISO7816.OFFSET_P1] = mP1;
                array[ISO7816.OFFSET_P2] = mP2;
                array[ISO7816.OFFSET_P3] = (byte) (mLe & 0x0FF);
            } else if (mData != null && mLe == null) {
                // APDU Case 3
                array = new byte[ISO7816.CMD_APDU_LENGTH_CASE3_WITHOUT_DATA
                                 + mData.length];
                array[ISO7816.OFFSET_CLA] = mCla;
                array[ISO7816.OFFSET_INS] = mIns;
                array[ISO7816.OFFSET_P1] = mP1;
                array[ISO7816.OFFSET_P2] = mP2;
                array[ISO7816.OFFSET_P3] = (byte) (mData.length & 0x0FF);
                System.arraycopy(
                        mData, 0, array, ISO7816.OFFSET_DATA, mData.length);
            } else {
                // APDU Case 4
                array = new byte[ISO7816.CMD_APDU_LENGTH_CASE4_WITHOUT_DATA
                                 + mData.length];
                array[ISO7816.OFFSET_CLA] = mCla;
                array[ISO7816.OFFSET_INS] = mIns;
                array[ISO7816.OFFSET_P1] = mP1;
                array[ISO7816.OFFSET_P2] = mP2;
                array[ISO7816.OFFSET_P3] = (byte) (mData.length & 0x0FF);
                System.arraycopy(
                        mData, 0, array, ISO7816.OFFSET_DATA, mData.length);
                array[array.length - 1] = (byte) (mLe & 0x0FF);
            }

        } else {
            if (mData == null && mLe != null) {
                // APDU Case 2
                array = new byte[ISO7816.CMD_APDU_LENGTH_CASE2_EXTENDED];
                array[ISO7816.OFFSET_CLA] = mCla;
                array[ISO7816.OFFSET_INS] = mIns;
                array[ISO7816.OFFSET_P1] = mP1;
                array[ISO7816.OFFSET_P2] = mP2;
                array[ISO7816.OFFSET_P3] = (byte) 0x00;
                array[ISO7816.OFFSET_DATA] = (byte) ((mLe >> 8) & 0x0FF);
                array[ISO7816.OFFSET_DATA + 1] = (byte) (mLe & 0x0FF);
            } else if (mData != null && mLe == null) {
                // APDU Case 3
                array = new byte[ISO7816.
                                 CMD_APDU_LENGTH_CASE3_WITHOUT_DATA_EXTENDED
                                 + mData.length];
                array[ISO7816.OFFSET_CLA] = mCla;
                array[ISO7816.OFFSET_INS] = mIns;
                array[ISO7816.OFFSET_P1] = mP1;
                array[ISO7816.OFFSET_P2] = mP2;
                array[ISO7816.OFFSET_P3] = (byte) 0x00;
                array[ISO7816.OFFSET_DATA] = (byte) ((mData.length >> 8)
                        & 0x0FF);
                array[ISO7816.OFFSET_DATA + 1] = (byte) (mData.length & 0x0FF);
                System.arraycopy(
                        mData,
                        0,
                        array,
                        ISO7816.OFFSET_DATA_EXTENDED,
                        mData.length);
            } else {
                // APDU Case 4
                array = new byte[ISO7816.
                                 CMD_APDU_LENGTH_CASE4_WITHOUT_DATA_EXTENDED
                                 + mData.length];
                array[ISO7816.OFFSET_CLA] = mCla;
                array[ISO7816.OFFSET_INS] = mIns;
                array[ISO7816.OFFSET_P1] = mP1;
                array[ISO7816.OFFSET_P2] = mP2;
                array[ISO7816.OFFSET_P3] = (byte) 0x00;
                array[ISO7816.OFFSET_DATA] = (byte) ((mData.length >> 8)
                        & 0x0FF);
                array[ISO7816.OFFSET_DATA + 1] = (byte) (mData.length & 0x0FF);
                System.arraycopy(
                        mData,
                        0,
                        array,
                        ISO7816.OFFSET_DATA_EXTENDED,
                        mData.length);
                array[array.length - 2] = (byte) ((mLe >> 8) & 0x0FF);
                array[array.length - 1] = (byte) (mLe & 0x0FF);
            }
        }
        return array;
    }

    public CommandApdu cloneWithLe(int le) {
        if (mData == null) {
            // Original APDU was Case 1 or Case 2
            return new CommandApdu(mCla, mIns, mP1, mP2, (byte) le);
        } else {
            // Original APDU was case 3 or 4
            return new CommandApdu(mCla, mIns, mP1, mP2, mData, (byte) le);
        }
    }

    /**
     * Private method - Set CLA byte and check if it is a valid value or not.
     *
     * @param cla Value of CLA field.
     *
     * @throws IllegalArgumentException if CLA is invalid.
     */
    private void setCla(byte cla) throws IllegalArgumentException {
        if (cla == (byte) 0xFF) {
            // cla has a wrong value!
            throw new IllegalArgumentException(
                    "Invalid value of CLA (" + Integer.toHexString(cla) + ")");
        }
        mCla = cla;
    }

    /**
     * Private method - Set INS byte and check if it is a valid value or not.
     *
     * @param ins Value of INS field.
     *
     * @throws IllegalArgumentException if INS is invalid.
     */
    private void setIns(byte ins) throws IllegalArgumentException {
        if ((ins & 0x0F0) == 0x60 || ((ins & 0x0F0) == 0x90)) {
            // ins has a wrong value!
            throw new IllegalArgumentException(
                    "Invalid value of INS (" + Integer.toHexString(ins) + "). "
                            + "0x6X and 0x9X are not valid values");
        }
        mIns = ins;
    }

    /**
     * Private method - Set p1 byte.
     *
     * @param p1 Value of P1 field.
     */
    private void setP1(byte p1) {
        mP1 = p1;
    }

    /**
     * Private method - Set p2 byte.
     *
     * @param p2 Value of P2 field.
     */
    private void setP2(byte p2) {
        mP2 = p2;
    }

    /**
     * Private method Set Data.
     *
     * @param data Value of APDU data.
     * @throws IllegalArgumentException if Data is null, 0 or is too long.
     */
    private void setData(byte[] data) throws IllegalArgumentException {
        if (data == null) {
            throw new IllegalArgumentException(
                    "Data must not be null.");
        }

        if (data.length > ISO7816.MAX_COMMAND_DATA_LENGTH) {
            throw new IllegalArgumentException(
                    "Data too long.");
        }

        if (data.length == 0) {
            throw new IllegalArgumentException(
                    "Data must not be empty.");
        }

        mData = new byte[data.length];
        System.arraycopy(data, 0, mData, 0, data.length);
    }

    /**
     * Private method - Set LE byte.
     *
     * @param le Value of LE field.
     *
     * @throws IllegalArgumentException if Le is invalid.
     */
    private void setLe(int le) throws IllegalArgumentException {

        if (le < 0 || le > ISO7816.MAX_RESPONSE_DATA_LENGTH) {
            throw new IllegalArgumentException(
                    "Invalid value for le parameter (" + le + ").");
        }
        mLe = le;
    }

    /**
     * Check if the Apdu is extended.
     *
     * @return true if Apdu is extended or false otherwise.
     */
    public boolean isExtendedLength() {
        if ((mLe != null && mLe > ISO7816.MAX_RESPONSE_DATA_LENGTH_NO_EXTENDED)
                || (mData != null
                && mData.length
                > ISO7816.MAX_COMMAND_DATA_LENGTH_NO_EXTENDED)) {
            return true;
        }
        return false;
    }
}
