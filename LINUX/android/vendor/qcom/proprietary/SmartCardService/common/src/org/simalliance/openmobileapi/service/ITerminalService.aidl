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

package org.simalliance.openmobileapi.service;

import org.simalliance.openmobileapi.service.SmartcardError;
import org.simalliance.openmobileapi.service.OpenLogicalChannelResponse;

/**
 * Smartcard service interface.
 */
interface ITerminalService {

    /**
     * Implementation of the MANAGE CHANNEL open and SELECT commands.
     *
     * @param aid The aid of the applet to be selected.
     *
     * @return the number of the logical channel according to ISO 7816-4.
     *
     * @throws Exception If the channel could not be opened.
     */
    OpenLogicalChannelResponse internalOpenLogicalChannel(in byte[] aid, in byte p2, out SmartcardError error);

    /**
     * Implementation of the MANAGE CHANNEL close command.
     *
     * @param channelNumber The channel to be closed.
     *
     * @throws CardException If the channel could not be closed.
     */
    void internalCloseLogicalChannel(int channelNumber, out SmartcardError error);

    /**
     * Implements the terminal specific transmit operation.
     *
     * @param command the command APDU to be transmitted.
     * @return the response APDU received.
     * @throws CardException if the transmit operation failed.
     */
    byte[] internalTransmit(in byte[] command, out SmartcardError error);

    /**
     * Returns the ATR of the connected card or null if the ATR is not
     * available.
     *
     * @return the ATR of the connected card or null if the ATR is not
     *         available.
     */
    byte[] getAtr();

    /**
     * Returns <code>true</code> if a card is present; <code>false</code>
     * otherwise.
     *
     * @return <code>true</code> if a card is present; <code>false</code>
     *         otherwise.
     * @throws CardException if card presence information is not available.
     */
    boolean isCardPresent();

    /**
     * Exchanges APDU (SELECT, READ/WRITE) to the  given EF by File ID and file
     * path via iccIO.
     *
     * The given command is checked and might be rejected.
     *
     * @param filePath
     * @param cmd
     * @return
     */
    byte[] simIOExchange(in int fileID, in String filePath, in byte[] cmd, out SmartcardError error);

    /**
     * Gets the Intent Action that is broadcasted when the SE state changes.
     *
     * @return the Intent Action that is broadcasted when the SE state changes.
     */
    String getSeStateChangedAction();
}
