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
 * Copyright 2012 Giesecke & Devrient GmbH.
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

package org.simalliance.openmobileapi.service.security;

import org.simalliance.openmobileapi.internal.Util;



public class AccessFilterUtil {

    public static ApduFilter[] parseAccessConditions(byte[] accessConditions) {
        if (accessConditions.length == 0) {
            return new ApduFilter[0];
        }

        if ((accessConditions.length % 8) != 0) {
            throw new IllegalArgumentException("Access Conditions must have a length of 8 bytes");
        }

        int numOfACs = accessConditions.length / 8;
        ApduFilter[] acs = new ApduFilter[numOfACs];
        int offset = 0;
        int length = 8;
        int index = 0;
        while ((offset + length) <= accessConditions.length && length != 0) {
            acs[index] = new ApduFilter(Util.getMid(accessConditions, offset, length));
            offset += length;
            index++;
        }
        return acs;
    }
}
