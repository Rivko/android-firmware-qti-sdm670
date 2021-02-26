/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.model;

import com.qualcomm.qti.qmmi.utils.FileUtils;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public final class PlatformManager {

    public static final String PLATFORM_DEFAULT = "default";

    public static final String SYS_SOC_ID = "/sys/devices/soc0/soc_id";
    public static final String SYS_PLATFROM_SUBTYPE_ID = "/sys/devices/soc0/platform_subtype_id";
    public static final String SYS_PLATFROM_VERSION = "/sys/devices/soc0/platform_version";

    public static class SocInfo {
        public String name;
        public int socId;

        public SocInfo(String name, int socId) {
            this.name = name;
            this.socId = socId;
        }
    }

    /* chip information */
    static SocInfo[] mSocList = new SocInfo[]{
            new SocInfo("MSM8998", 292),
            new SocInfo("MSM8953", 293),
            new SocInfo("MSM8937", 294),
            new SocInfo("MSM8940", 313),
            new SocInfo("SDM660", 317),
            new SocInfo("SDM630", 318),
            new SocInfo("MSM8920", 320),
            new SocInfo("SDM845", 321),
    };

    static String getPlatformName() {
        String socId = FileUtils.readFile(SYS_SOC_ID);
        LogUtils.logi("soc id:" + socId);

        for (SocInfo si : mSocList) {
            if (si.socId == Integer.valueOf(socId.trim())) {
                return si.name;
            }
        }

        return null;
    }

    static String getSubtypeId() {
        return FileUtils.readFile(SYS_PLATFROM_SUBTYPE_ID);
    }
}
