/*  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.flp;

import java.com.qti.flp.IMaxPowerAllocatedCallback;

interface ITestService {

    void deleteAidingData(in long flags);

    void updateXtraThrottle(in boolean enabled);

    void registerMaxPowerChangeCallback(in IMaxPowerAllocatedCallback cb);

    void unregisterMaxPowerChangeCallback(in IMaxPowerAllocatedCallback cb);
}
