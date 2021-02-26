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

package org.simalliance.openmobileapi.internal;

import android.content.Context;
import android.content.pm.PackageManager;

import java.security.AccessControlException;

public class Util {

    public static final byte END = -1;

    public static byte[] mergeBytes(byte[] array1, byte[] array2) {
        byte[] data = new byte[array1.length + array2.length];
        System.arraycopy(array1, 0, data, 0, array1.length);
        System.arraycopy(array2, 0, data, array1.length, array2.length);
        return data;
    }

    public static byte[] getMid(byte[] array, int start, int length) {
        byte[] data = new byte[length];
        System.arraycopy(array, start, data, 0, length);
        return data;
    }

    @Deprecated // User ByteArrayConverter instead
    public static String bytesToString(byte[] bytes) {
        if(bytes == null)
            return "";
        StringBuilder sb = new StringBuilder();
        for (byte b : bytes) {
            sb.append(String.format("%02x ", b & 0xFF));
        }
        String str = sb.toString();
        if (str.length() > 0) {
            str = str.substring(0, str.length() - 1);
        }
        return str;
    }

    /**
     * Returns a concatenated response.
     *
     * @param r1 the first part of the response.
     * @param r2 the second part of the response.
     * @param length the number of bytes of the second part to be appended.
     * @return a concatenated response.
     */
    public static byte[] appendResponse(byte[] r1, byte[] r2, int length) {
        byte[] rsp = new byte[r1.length + length];
        System.arraycopy(r1, 0, rsp, 0, r1.length);
        System.arraycopy(r2, 0, rsp, r1.length, length);
        return rsp;
    }

    /**
     * Creates a formatted exception message.
     *
     * @param commandName the name of the command. <code>null</code> if not
     *            specified.
     * @param sw the response status word.
     * @return a formatted exception message.
     */
    public static String createMessage(String commandName, int sw) {
        StringBuilder message = new StringBuilder();
        if (commandName != null) {
            message.append(commandName).append(" ");
        }
        message.append("SW1/2 error: ");
        message.append(Integer.toHexString(sw | 0x10000).substring(1));
        return message.toString();
    }

    /**
     * Creates a formatted exception message.
     *
     * @param commandName the name of the command. <code>null</code> if not
     *            specified.
     * @param message the message to be formatted.
     * @return a formatted exception message.
     */
    public static String createMessage(String commandName, String message) {
        if (commandName == null) {
            return message;
        }
        return commandName + " " + message;
    }

    @Deprecated // Use ByteArrayConverter instead
    public static String bytesToString(byte[] array,int offset,int length, String prefix) {
        if (array==null) return null;
        if (length==-1) length=array.length-offset;

        StringBuilder buffer=new StringBuilder();
        for (int ind=offset;ind<offset+length;ind++)
            buffer.append(prefix).append(Integer.toHexString(0x100 + (array[ind] & 0xFF)).substring(1));
        return buffer.toString();
    }

    /**
     * Get package name from the user id.
     *
     * This shall fix the problem the issue that process name != package name
     * due to anndroid:process attribute in manifest file.
     *
     * But this call is not really secure either since a uid can be shared
     * between one and more apks
     *
     * @param context
     * @param uid
     * @return The first package name associated with this uid.
     */
    public static String getPackageNameFromCallingUid(Context context, int uid) {
        PackageManager packageManager = context.getPackageManager();
        if (packageManager != null) {
            String packageName[] = packageManager.getPackagesForUid(uid);
            if (packageName != null && packageName.length > 0) {
                return packageName[0];
            }
        }
        throw new AccessControlException(
                "Caller PackageName can not be determined");
    }

    /**
     * Returns a copy of the given CLA byte where the channel number bits are
     * set as specified by the given channel number See GlobalPlatform Card
     * Specification 2.2.0.7: 11.1.4 Class Byte Coding.
     *
     * @param cla the CLA byte. Won't be modified
     * @param channelNumber within [0..3] (for first interindustry class byte
     *            coding) or [4..19] (for further interindustry class byte
     *            coding)
     * @return the CLA byte with set channel number bits. The seventh bit
     *         indicating the used coding (first/further interindustry class
     *         byte coding) might be modified
     */
    public static byte setChannelToClassByte(byte cla, int channelNumber) {
        if (channelNumber < 4) {
            // b7 = 0 indicates the first interindustry class byte coding
            cla = (byte) ((cla & 0xBC) | channelNumber);
        } else if (channelNumber < 20) {
            // b7 = 1 indicates the further interindustry class byte coding
            boolean isSM = (cla & 0x0C) != 0;
            cla = (byte) ((cla & 0xB0) | 0x40 | (channelNumber - 4));
            if (isSM) {
                cla |= 0x20;
            }
        } else {
            throw new IllegalArgumentException(
                    "Channel number must be within [0..19]");
        }
        return cla;
    }

    /**
     * Clear the channel number.
     *
     * @param cla
     *
     * @return the cla without channel number
     */
    public static byte clearChannelNumber(byte cla) {
        // bit 7 determines which standard is used
        boolean isFirstInterindustryClassByteCoding = (cla & 0x40) == 0x00;

        if (isFirstInterindustryClassByteCoding) {
            // First Interindustry Class Byte Coding
            // see 11.1.4.1: channel number is encoded in the 2 rightmost bits
            return (byte) (cla & 0xFC);
        } else {
            // Further Interindustry Class Byte Coding
            // see 11.1.4.2: channel number is encoded in the 4 rightmost bits
            return (byte) (cla & 0xF0);
        }
    }

    /**
     * Extracts the channel number from a CLA byte. Specified in GlobalPlatform
     * Card Specification 2.2.0.7: 11.1.4 Class Byte Coding.
     *
     * @param cla
     *            the command's CLA byte
     * @return the channel number within [0x00..0x0F]
     */
    public static int parseChannelNumber(byte cla) {
        // bit 7 determines which standard is used
        boolean isFirstInterindustryClassByteCoding = (cla & 0x40) == 0x00;

        if (isFirstInterindustryClassByteCoding) {
            // First Interindustry Class Byte Coding
            // see 11.1.4.1: channel number is encoded in the 2 rightmost bits
            return cla & 0x03;
        } else {
            // Further Interindustry Class Byte Coding
            // see 11.1.4.2: channel number is encoded in the 4 rightmost bits
            return (cla & 0x0F) + 4;
        }
    }
}
