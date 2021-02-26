/*
 * Copyright (C) 2015, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * Contributed by: Giesecke & Devrient GmbH.
 */

package org.simalliance.openmobileapi.util;

/**
 * This object represents ISO/IEC 7816.
 *
 * @author Giesecke & Devrient
 *
 */
public class ISO7816 {
    /**
     * Maximum command APDU data length.
     */
    public static final int MAX_COMMAND_DATA_LENGTH = 65535;

    /**
     * Maximum response APDU data  length.
     */
    public static final int MAX_RESPONSE_DATA_LENGTH = 65536;

    /**
     * Maximum command APDU data length.
     */
    public static final int MAX_COMMAND_DATA_LENGTH_NO_EXTENDED = 255;

    /**
     * Maximum response APDU data  length.
     */
    public static final int MAX_RESPONSE_DATA_LENGTH_NO_EXTENDED = 256;

    /**
     * Length of an APDU Case 1.
     */
    public static final int CMD_APDU_LENGTH_CASE1 = 4;

    /**
     * Length of an APDU Case 2.
     */
    public static final int CMD_APDU_LENGTH_CASE2 = 5;

    /**
     * Length of an APDU Case 2 extended case.
     */
    public static final int CMD_APDU_LENGTH_CASE2_EXTENDED = 7;

    /**
     * Length of an APDU Case 3 without taking data into account.
     */
    public static final int CMD_APDU_LENGTH_CASE3_WITHOUT_DATA = 5;

    /**
     * Length of an APDU Case 3 extended without taking data into account.
     */
    public static final int CMD_APDU_LENGTH_CASE3_WITHOUT_DATA_EXTENDED = 7;

    /**
     * Length of an APDU Case 4 without taking data into account.
     */
    public static final int CMD_APDU_LENGTH_CASE4_WITHOUT_DATA = 6;

    /**
     * Length of an APDU Case 4 extended without taking data into account.
     */
    public static final int CMD_APDU_LENGTH_CASE4_WITHOUT_DATA_EXTENDED = 9;

    /**
     * Length of an status word.
     */
    public static final int RESP_APDU_LENGTH_SW = 2;

    // Offsets in Command APDU.

    /**
     * CLA offset in Command APDU.
     */
    public static final int OFFSET_CLA = 0;
    /**
     * INS offset in Command APDU.
     */
    public static final int OFFSET_INS = 1;
    /**
     * P1 offset in Command APDU.
     */
    public static final int OFFSET_P1 = 2;
    /**
     * P2 offset in Command APDU.
     */
    public static final int OFFSET_P2 = 3;
    /**
     * P3 offset in Command APDU.
     */
    public static final int OFFSET_P3 = 4;
    /**
     * Data offset in Command APDU.
     */
    public static final int OFFSET_DATA = 5;

    /**
     * Data offset in Command APDU.
     */
    public static final int OFFSET_DATA_EXTENDED = 7;

    // CLA values.

    /**
     * Interindustry class value.
     */
    public static final byte CLA_INTERINDUSTRY  = (byte) 0x00;
    /**
     * Proprietary class value.
     */
    public static final byte CLA_PROPRIETARY    = (byte) 0x80;

    // Ins values.

    /**
     * ISO EXTERNAL AUTHENTICATE DATA instruction value.
     */
    public static final byte INS_EXTERNAL_AUTHENTICATE     = (byte) 0x82;
    /**
     * ISO APPEND RECORD instruction value.
     */
    public static final byte INS_APPEND_RECORD      = (byte) 0xE2;
    /**
     * ISO CHANGE REFERENCE DATA instruction value.
     */
    public static final byte INS_CHANGE_REF_DATA    = (byte) 0x24;
    /**
     * ISO ENABLE VERIFICAITION REQUIREMENT instruction value.
     */
    public static final byte INS_ENABLE_VERIF_REQ   = (byte) 0x28;
    /**
     * ISO ENVELOPE instruction value (0xC3).
     */
    public static final byte INS_ENVELOPE_C3   = (byte) 0xC3;
    /**
     * ISO ENVELOPE instruction value (0xC2).
     */
    public static final byte INS_ENVELOPE_C2   = (byte) 0xC2;
    /**
     * ISO ERASE BINARY instruction value (0x0E).
     */
    public static final byte INS_ERASE_BINARY_0E   = (byte) 0x0E;
    /**
     * ISO ERASE BINARY instruction value (0x0F).
     */
    public static final byte INS_ERASE_BINARY_0F   = (byte) 0x0F;
    /**
     * ISO ERASE RECORD instruction value.
     */
    public static final byte INS_ERASE_RECORD   = (byte) 0x0C;
    /**
     * ISO GENERAL AUTHENTICATE instruction value (0x86).
     */
    public static final byte INS_GENERAL_AUTHENTICATE_86   = (byte) 0x86;
    /**
     * ISO GENERAL AUTHENTICATE instruction value (0x87).
     */
    public static final byte INS_GENERAL_AUTHENTICATE_87   = (byte) 0x87;
    /**
     * ISO GENERATE ASYMMETRIC KEY PAIR instruction value.
     */
    public static final byte INS_GENERATE_ASYMMETRIC_KEY_PAIR   = (byte) 0x46;
    /**
     * ISO GET CHALLENGE instruction value.
     */
    public static final byte INS_GET_CHALLENGE   = (byte) 0x84;
    /**
     * ISO GET DATA instruction value (0xCA).
     */
    public static final byte INS_GET_DATA_CA   = (byte) 0xCA;
    /**
     * ISO GET DATA instruction value (0xCB).
     */
    public static final byte INS_GET_DATA_CB   = (byte) 0xCB;
    /**
     * ISO GET RESPONSE instruction value.
     */
    public static final byte INS_GET_RESPONSE   = (byte) 0xC0;
    /**
     * ISO INTERNAL AUTHENTICATE instruction value.
     */
    public static final byte INS_INTERNAL_AUTHENTICATE   = (byte) 0x88;
    /**
     * ISO MANAGE CHANNEL instruction value.
     */
    public static final byte INS_MANAGE_CHANNEL   = (byte) 0x70;
    /**
     * ISO MANAGE SECURITY ENVIRONMENT instruction value.
     */
    public static final byte INS_MANAGE_SECURITY_ENVIRONMENT   = (byte) 0x22;
    /**
     * ISO PUT DATA instruction value (0xDA).
     */
    public static final byte INS_PUT_DATA_DA   = (byte) 0xDA;
    /**
     * ISO PUT DATA instruction value (0xDB).
     */
    public static final byte INS_PUT_DATA_DB   = (byte) 0xDB;
    /**
     * ISO DISABLE VERIFICATION REQUIREMENT instruction value.
     */
    public static final byte INS_DISABLE_VERIF_REQ  = (byte) 0x26;
    /**
     * ISO READ BINARY instruction value (0xB0).
     */
    public static final byte INS_READ_BINARY_B0     = (byte) 0xB0;
    /**
     * ISO READ BINARY instruction value (0xB1).
     */
    public static final byte INS_READ_BINARY_B1     = (byte) 0xB1;
    /**
     * ISO READ RECORD instruction value (0xB2).
     */
    public static final byte INS_READ_RECORD_B2     = (byte) 0xB2;
    /**
     * ISO READ RECORD instruction value (0xB3).
     */
    public static final byte INS_READ_RECORD_B3     = (byte) 0xB3;
    /**
     * ISO RESET RETRY COUNTER instruction value.
     */
    public static final byte INS_RESET_RETRY_CTR    = (byte) 0x2C;
    /**
     * ISO SEARCH BINARY instruction value (0xA0).
     */
    public static final byte INS_SEARCH_BINARY_A0      = (byte) 0xA0;
    /**
     * ISO SEARCH BINARY instruction value (0xA1).
     */
    public static final byte INS_SEARCH_BINARY_A1      = (byte) 0xA1;
    /**
     * ISO SEARCH RECORD instruction value.
     */
    public static final byte INS_SEARCH_RECORD      = (byte) 0xA2;
    /**
     * ISO SELECT instruction value.
     */
    public static final byte INS_SELECT             = (byte) 0xA4;
    /**
     * ISO UPDATE BINARY instruction value (0xD6).
     */
    public static final byte INS_UPDATE_BINARY_D6   = (byte) 0xD6;
    /**
     * ISO UPDATE BINARY instruction value (0xD7).
     */
    public static final byte INS_UPDATE_BINARY_D7   = (byte) 0xD7;
    /**
     * ISO UPDATE RECORD instruction value (0xDC).
     */
    public static final byte INS_UPDATE_RECORD_DC   = (byte) 0xDC;
    /**
     * ISO UPDATE RECORD instruction value (0xDD).
     */
    public static final byte INS_UPDATE_RECORD_DD   = (byte) 0xDD;
    /**
     * ISO VERIFY instruction value (0x20).
     */
    public static final byte INS_VERIFY_20          = (byte) 0x20;
    /**
     * ISO VERIFY instruction value (0x21).
     */
    public static final byte INS_VERIFY_21          = (byte) 0x21;
    /**
     * ISO WRITE BINARY instruction value (0xD0).
     */
    public static final byte INS_WRITE_BINARY_D0    = (byte) 0xD0;
    /**
     * ISO WRITE BINARY instruction value (0xD1).
     */
    public static final byte INS_WRITE_BINARY_D1    = (byte) 0xD1;
    /**
     * ISO WRITE RECORD instruction value.
     */
    public static final byte INS_WRITE_RECORD       = (byte) 0xD2;

    // SW Values

    // 9000 - Normal processing - No further qualification.
    /**
     * No further qualification.
     */
    public static final int SW_NO_FURTHER_QUALIFICATION = 0x9000;

    // 61XX - Normal processing - SW2 encodes the number of data bytes still
    // available.
    /**
     * SW2 encodes the number of data bytes still available (minimum value).
     */
    public static final int SW_NORMAL_PROCESSING_MIN = 0x6100;
    /**
     * SW2 encodes the number of data bytes still available (maximum value).
     */
    public static final int SW_NORMAL_PROCESSING_MAX = 0x61FF;

    // 62XX - Warning processing - State of non-volatile memory is unchanged
    // (further qualification in SW2).
    /**
     * Warning given.
     */
    public static final byte SW1_62 = (byte) 0x62;
    /**
     * No information given.
     */
    public static final int SW_62_NO_INFO = 0x6200;
    /**
     * Triggering by the card (minimum value).
     */
    public static final int SW_62_TRIGGERING_CARD_MIN = 0x6202;
    /**
     * Triggering by the card (maximum value).
     */
    public static final int SW_62_TRIGGERING_CARD_MAX = 0x6280;
    /**
     * Part of returned data may be corrupted.
     */
    public static final int SW_DATA_CORRUPTED = 0x6281;
    /**
     * End of file or record reached before reading Ne bytes.
     */
    public static final int SW_UNEXPECTED_EOF = 0x6282;
    /**
     * Selected file deactivated.
     */
    public static final int SW_FILE_DEACTIVATED = 0x6283;
    /**
     * File control information not formatted according to 5.3.3.
     */
    public static final int SW_WRONG_FILE_CONTROL_FORMAT = 0x6284;
    /**
     * Selected file in termination state.
     */
    public static final int SW_FILE_STATE_TERMINATION = 0x6285;
    /**
     * No input data available from a sensor on the card.
     */
    public static final int SW_NO_INPUT_DATA_AVAILABLE = 0x6286;

    // 63XX - Warning processing - State of non-volatile memory has
    // changed (further qualification in SW2).
    /**
     * Warning given.
     */
    public static final byte SW1_63 = (byte) 0x63;
    /**
     * No information given.
     */
    public static final int SW_63_NO_INFO = 0x6300;
    /**
     * File filled up by the last write.
     */
    public static final int SW_FILE_FILLED_UP = 0x6381;
    /**
     * Counter from 0 to 15 encoded by X (exact meaning depending on the
     * command). Minimum value.
     */
    public static final int SW_CTR_MIN = 0x63C0;
    /**
     * Counter from 0 to 15 encoded by X (exact meaning depending on the
     * command). Maximum value.
     */
    public static final int SW_CTR_MAX = 0x63CF;

    // 64XX - Execution error - State of non-volatile memory is
    // unchanged (further qualification in SW2)
    /**
     * Execution error.
     */
    public static final int SW_EXEC_ERROR = 0x6400;
    /**
     * Immediate response required by the card.
     */
    public static final int SW_IMMEDIATE_RESPONSE_REQUIRED = 0x6401;
    /**
     * Triggering by the card (minimum value).
     */
    public static final int SW_64_TRIGGERING_CARD_MIN = 0x6402;
    /**
     * Triggering by the card (maximum value).
     */
    public static final int SW_64_TRIGGERING_CARD_MAX = 0x6480;

    // 65XX - Execution error - State of non-volatile memory has changed
    // (further qualification in SW2)
    /**
     * No information given.
     */
    public static final int SW_65_NO_INFO = 0x6500;
    /**
     * Memory failure.
     */
    public static final int SW_MEMORY_FAILURE = 0x6581;

    // 66XX - Execution error - Security-related issues
    /**
     * Security issue (minimum value).
     */
    public static final int SW_SECURITY_ISSUE_MIN = 0x6600;
    /**
     * Security issue (maximum value).
     */
    public static final int SW_SECURITY_ISSUE_MAX = 0x66FF;

    // 6700 - Checking error - Wrong length, no further indication
    /**
     * Wrong length, no further indication.
     */
    public static final int SW_WRONG_LENGTH = 0x6700;

    // 68XX - Checking error - Functions in CLA not supported
    // (further qualification in SW2)
    /**
     * No information given.
     */
    public static final int SW_68_NO_INFO = 0x6800;
    /**
     * Logical channel not supported.
     */
    public static final int SW_LOGICAL_CHANNEL_NOT_SUPPORTED = 0x6881;
    /**
     * Secure messaging not supported.
     */
    public static final int SW_SECURE_MESSAGING_NOT_SUPPORTED = 0x6882;
    /**
     * Last command of the chain expected.
     */
    public static final int SW_LAST_COMMAND_EXPECTED = 0x6883;
    /**
     * Command chaining not supported.
     */
    public static final int SW_COMMAND_CHAINING_NOT_SUPPORTED = 0x6884;

    // 69XX - Checking error - Command not allowed (further
    // qualification in SW2).
    /**
     * No information given.
     */
    public static final int SW_69_NO_INFO = 0x6900;
    /**
     * Command incompatible with file structure.
     */
    public static final int SW_COMMAND_INCOMPATIBLE = 0x6981;
    /**
     * Security status not satisfied.
     */
    public static final int SW_SECURITY_STATUS_NOT_SATISFIED = 0x6982;
    /**
     * Authentication method blocked.
     */
    public static final int SW_AUTH_METHOD_BLOCKED = 0x6983;
    /**
     * Reference data not usable.
     */
    public static final int SW_REF_DATA_NOT_USABLE = 0x6984;
    /**
     * Conditions of use not satisfied.
     */
    public static final int SW_CONDITIONS_NOT_SATISFIED = 0x6985;
    /**
     * Command not allowed (no current EF).
     */
    public static final int SW_COMMAND_NOT_ALLOWED = 0x6986;
    /**
     * Expected secure messaging data objects missing.
     */
    public static final int SW_SM_OBJECT_MISSING = 0x6987;
    /**
     * Incorrect secure messaging data objects.
     */
    public static final int SW_SM_INCORRECT_OBJECT = 0x6988;

    // 6AXX - Checking error - Wrong parameters P1-P2 (further
    // qualification in SW2)
    /**
     * No information given.
     */
    public static final int SW_6A_NO_INFO = 0x6A00;
    /**
     * Incorrect parameters in the command data field.
     */
    public static final int SW_WRONG_DATA = 0x6A80;
    /**
     * Function not supported.
     */
    public static final int SW_FUNC_NOT_SUPPORTED = 0x6A81;
    /**
     * File or application not found.
     */
    public static final int SW_FILE_OR_APP_NOT_FOUND = 0x6A82;
    /**
     * Record not found.
     */
    public static final int SW_RECORD_NOT_FOUND = 0x6A83;
    /**
     * Not enough memory space in the file.
     */
    public static final int SW_NOT_ENOUGH_MEMORY = 0x6A84;
    /**
     * Nc inconsistent with TLV structure.
     */
    public static final int SW_WRONG_NC_TLV = 0x6A85;
    /**
     * Incorrect parameters P1-P2.
     */
    public static final int SW_INCORRECT_P1P2 = 0x6A86;
    /**
     * Nc inconsistent with parameters P1-P2.
     */
    public static final int SW_WRONG_NC_P1P2 = 0x6A87;
    /**
     * Referenced data or reference data not found (exact meaning depending on
     * the command).
     */
    public static final int SW_REF_NOT_FOUND = 0x6A88;
    /**
     * File already exists.
     */
    public static final int SW_FILE_ALREADY_EXISTS = 0x6A89;
    /**
     * DF name already exists.
     */
    public static final int SW_DF_NAME_ALREADY_EXISTS = 0x6A8A;

    // 6B00 - Checking error - Wrong parameters P1-P2
    /**
     * Wrong parameters P1-P2.
     */
    public static final int SW_WRONG_PARAMETERS_P1P2 = 0x6B00;

    // 6CXX - Checking error - Wrong Le field; SW2 encodes the exact
    // number of available data bytes.
    /**
     * Wrong Le field; SW2 encodes the exact number of available data bytes
     * (minimum value).
     */
    public static final int SW_WRONG_LE_MIN = 0x6C00;
    /**
     * Wrong Le field; SW2 encodes the exact number of available data bytes
     * (maximum value).
     */
    public static final int SW_WRONG_LE_MAX = 0x6CFF;

    // 6D00 - Checking error - Instruction code not supported or
    // invalid.
    /**
     * Instruction code not supported or invalid.
     */
    public static final int SW_INS_NOT_SUPPORTED = 0x6D00;

    // 6E00 - Checking error - Class not supported.
    /**
     * Class not supported.
     */
    public static final int SW_CLA_NOT_SUPPORTED = 0x6E00;

    // 6F00 - Checking error - No precise diagnostic
    /**
     * No precise diagnostic.
     */
    public static final int SW_NO_PRECISE_DIAGNOSTIC = 0x6F00;

    /**
     * This class cannot be instantiated.
     */
    private ISO7816() {
    }
}
