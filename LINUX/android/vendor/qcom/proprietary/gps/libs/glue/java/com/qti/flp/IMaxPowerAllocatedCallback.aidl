/*  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.flp;

oneway interface IMaxPowerAllocatedCallback
{
    void onMaxPowerAllocatedChanged(in double power_mW);
}
