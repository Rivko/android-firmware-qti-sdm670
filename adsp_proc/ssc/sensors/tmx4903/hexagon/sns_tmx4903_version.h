/*
 * Copyright (c) 2017, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef AMS_TMX4903_VERSION_H
#define AMS_TMX4903_VERSION_H

/*******************************************************************************
 * Version History
 * tmd2772 for OpenSSC
 *
 *   Date
 * YYYYMMDD  Who  Version  Changes
 * --------  ---  -------  -----------------------------------------------------
 * 20170428  jcr  1.0.0    Initial SEE release
 * 20170714  jcr  1.6.0    Many fixes and other changes
 * 20170716  jcr  1.6.2    Fixed: registry fail for RGB; 1st OC report if STRM
 *                         already going
 * 20170721  jcr  1.7.0    ALS factory selftest now working
 * 20170727  dd   1.7.4    factory cal fixes, clean up
 * 20170727  dd   1.7.5    First proximity report fixed, sample rate fixes, clean up
 * 20170802  dd   1.7.6    First report for next OC clients fixed, clean up
 * 20170808  dd   1.7.8    Add RGB factory test functionality.
 * 20170810  dd   1.7.9    Publish calibration data events.
 * 20170810  dd   1.7.10   Island mode refactoring optimization.
 * 20170902  dd   1.7.12   Take the SEE5.05 and make changes addressing modified framework API.
 * 20170907  dd   1.7.13   Fix an issue with ALS/RGB and autogain resulting in inaccurate inital reports.
 * 20170911  dd   1.7.14   Optimize factory test procedure. Fix calibration accuracy for prox. Implement
 *                         SNS_STD_MSGID_SNS_STD_FLUSH_EVENT reply to SNS_STD_MSGID_SNS_STD_FLUSH_REQ.
 * 20171006  jcr  1.7.15   Some i2c bus usage optimizations
 * 20171017  jcr  1.7.16   Fix interrupt registering/enabling race. Fix an i2c optimization.
 * 20171018  jcr  1.7.17   Fixes (hopefully) for Klocwork issues raised by QTI
 * 20171020  jcr  1.7.18   Fix copy-paste errors in the Klocwork fixes
 * 20171023  jcr  1.7.19   Fixed another Klocwork issue
 * 20171030  jcr  1.7.20   Enable island mode, move amsLibrary_xxx to _hal_island.c
 * 20171101  jcr  1.7.21   Max 10Hz ODR; improved saturation handling; no longer an
 *                         unstable state upon ALS gain chg; don't force prox state
 *                         to non-near after cal; restore ~CalibrateRgb calls whacked
 *                         in copy-paste mod
 * 20171103  jcr  1.7.22   Chg RGB to 10Hz max ODR too
 * 20171205  jcr  1.7.23   Prox mods to cope with cover glass
 * 20171205  jcr  1.7.24   Corrected prox current consumption attribute; revamped
 *                         RGB calibration procedure; src cleanup
 * 20171220  jcr  1.7.25   Changed PTIME to 44.48 ms (22.5 Hz); elim. more ifdefs
 *                         that don't apply to the 4903
 * 20180117  jcr  1.7.26   Roll back RGB cal revamp and prox glass mods. Retain current
 *                         consump. mods incl. longer PTIME. Fix prox state machine to
 *                         not reenter _FIRST_MEASURE state.
 * 20180127  jcr  1.7.27   SELECTED_RANGE attribute for prox and the 2 error code changes.
 *                         ALS config events that wrongly claimed to be DRI
 *                         Fix for a race condition in the factory_cal selftests. Occurred for prox
 *                         when the reflectance is too high (cover glass or target too close/too big);
 *                         in theory it applied to lux and RGB too, so all 3 were fixed.
 *                         Added support for generating an sns_cal_event, 
 *                         whenever and only when, a new unique client is added.
 * 20180206  qcom 1.7.28   Reverted use of encoder functions, added in v1.7.21 to
 *                         write_calibration_to_registry, since it was corrupting 
 *                         data and format in registry files.
 * 20180214  qcom 1.7.29   Added support for SELECTED_RANGE attribute, for ALS sensor.
 *                         Restricted handling of flush request such that the only driver
 *                         activity is to send a flush complete event
 * 20180220  qcom 1.7.30   Fixed sample rate in _PHYSICAL_CONFIG_EVENT, generated due to
 *                         gain change, for ALS. Fixed the series for sns_cal_event events
 *                         to occur every time a new or change Request comes in.
 * 20180307  qcom 1.7.31   Added support to get "alsCalibrationLuxTarget" value from registry 
 *                         for ALS sensor.
 *                          
 ******************************************************************************/

#define AMS_DRIVER_VERSION   0x01731

#endif /* AMS_TMX4903_VERSION_H */
