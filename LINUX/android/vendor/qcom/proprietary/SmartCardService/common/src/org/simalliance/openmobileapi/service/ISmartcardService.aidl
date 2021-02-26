/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2011, The Android Open Source Project
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

import org.simalliance.openmobileapi.service.ISmartcardServiceCallback;
import org.simalliance.openmobileapi.service.ISmartcardServiceReader;
import org.simalliance.openmobileapi.service.SmartcardError;

/**
 * Smartcard service interface.
 */
interface ISmartcardService {

    /**
     * Returns the friendly names of available smart card readers.
     */
    String[] getReaders();

    /**
     * Returns Smartcard Service reader object to the given name.
     */
    ISmartcardServiceReader getReader(String reader, out SmartcardError error);

     /**
     * Checks if the application defined by the package name is allowed to receive
     * NFC transaction events for the defined AID.
     */
    boolean[] isNFCEventAllowed(String reader, in byte[] aid, in String[] packageNames, ISmartcardServiceCallback callback, out SmartcardError error);

}
