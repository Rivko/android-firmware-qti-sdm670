/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2011 The Android Open Source Project
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
package com.android.firewall;

import android.net.Uri;
import android.provider.BaseColumns;

// BlockRecord (PCI ADD)
    public class BlockRecord implements BaseColumns {

        public static final Uri CONTENT_URI = Uri
                .parse("content://com.android.firewall/blockrecorditems");

        private static final String BLOCKRECORD_TABLE = "blockrecorditems";

        public static final String DEFAULT_SORT_ORDER = "_id ASC";

        public static final int BLOCK_TYPE_CALL = 0;

        public static final int BLOCK_TYPE_RCS_MESSAGE = 1;
        public static final int BLOCK_TYPE_SMS = 2;
        public static final int BLOCK_TYPE_MMS = 3;
        // blockRecord type(0 is call block record , 1 is rcs message block
        // record)
        // 2 is sms block record ,3 is mms block record;
        public static final String BLOCK_TYPE = "block_type";

        public static final String DATE = "date";

        public static final String CONTACT = "contact";

        public static final String DATA = "data";

        public static final String BURN = "burn";

        public static final String MSG_TYPE = "msg_type";

        public static final String CALL_TYPE ="call_type";

        public static final String SUB_ID = "sub_id";

        public static final String NAME = "name";

        public static final String CREATE_TABLE_BLOCK_RECORD = "CREATE TABLE "+ BLOCKRECORD_TABLE
                    +
                    " (_id INTEGER PRIMARY KEY,"
                    +
                    "contact TEXT,"
                    +
                    "date INTEGER, "
                    +
                    "block_type INTEGER ,"
                    +
                    "data TEXT ,"
                    +
                    "msg_type INTEGER ,"
                    +
                    "sub_id TEXT ,"
                    +
                    "call_type INTEGER ,"
                    +
                    "burn INTEGER ,"
                    +
                    " name TEXT "
                    +
                    ");";
    }