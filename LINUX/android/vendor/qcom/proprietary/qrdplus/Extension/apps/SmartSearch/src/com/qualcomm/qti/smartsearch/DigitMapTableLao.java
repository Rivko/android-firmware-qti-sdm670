/*
 *  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.smartsearch;

/**
 * <p>
 * Implements the Lao-characters to digits map.
 * </p>
 */
public class DigitMapTableLao extends DigitMapTableLatin {

    private static DigitMapTableLao sInstance = null;

    private static final char[][] EXTEND_LAO_TABLE = {
        // number 0
        {
            0x0ED0, 0x0EAF, 0x0EB0, 0x0EB1, 0x0EB2, 0x0EB3,
            0x0EB4, 0x0EB5, 0x0EB6, 0x0EB7, 0x0EB8, 0x0EB9,
            0x0EBB, 0x0EBC, 0x0EBD, 0x0EC0, 0x0EC1, 0x0EC2,
            0x0EC3, 0x0EC4, 0x0EC6, 0x0EC8, 0x0EC9, 0x0ECA,
            0x0ECB, 0x0ECC, 0x0ECD, 0x0EDC, 0x0EDD
        },
        // number 1
        {
            0x0ED1, 0x0E81, 0x0E82, 0x0E84
        },
        // number 2
        {
            0x0ED2, 0x0E87, 0x0E88, 0x0E8A
        },
        // number 3
        {
            0x0ED3, 0x0E8D, 0x0E94, 0x0E95
        },
        // number 4
        {
            0x0ED4, 0x0E96, 0x0E97, 0x0E99
        },
        // number 5
        {
            0x0ED5, 0x0E9A, 0x0E9B, 0x0E9C
        },
        // number 6
        {
            0x0ED6, 0x0E9D, 0x0E9E, 0x0E9F
        },
        // number 7
        {
            0x0ED7, 0x0EA1, 0x0EA2, 0x0EA3
        },
        // number 8
        {
            0x0ED8, 0x0EA5, 0x0EA7, 0x0EAA
        },
        // number 9
        {
            0x0ED9, 0x0EAB, 0x0EAD, 0x0EAE
        }
    };

    public static DigitMapTableLao getInstance() {
        if (sInstance == null) {
            sInstance = new DigitMapTableLao();
        }
            return sInstance;
    }

    private DigitMapTableLao() {

    }

    @Override
    public String toDigits(char c) {
        int idx = -1;
        for(int i=0;i<KEY_NUM;i++) {
            if (contains(EXTEND_LAO_TABLE[i],c)) {
            idx = i;
            break;
            }
        }
            if (idx == -1) {
                return super.toDigits(c);
            } else {
                return Integer.toString(idx);
            }
    }
}
