/*  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.flp;

import java.com.qti.flp.ILocationCallback;
import java.com.qti.flp.ISessionStatusCallback;

interface IFlpService {

    void registerCallback(in int sessionType,
                          in int id,
                          in ILocationCallback cb,
                          in long sessionStartTime);

    void unregisterCallback(in int sessionType,
                            in ILocationCallback cb);

    int getAllSupportedFeatures();

    int startFlpSession(in int id,
                        in int flags,
                        in long period_ms,
                        in int distance_interval,
                        in long trip_distance);

    int updateFlpSession(in int id,
                         in int flags,
                         in long period_ms,
                         in int distance_interval,
                         in long trip_distance);

    int stopFlpSession(in int id);

    int pullLocations(in ILocationCallback cb,
                      in long sessionStartTime,
                      in int id);

    void registerForSessionStatus(in int id, in ISessionStatusCallback cb );
    void unregisterForSessionStatus(in ISessionStatusCallback cb);
}
