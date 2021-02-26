/*******************************************************************************

 Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ---------------------------------------------------------------------------

 @file    qti-activity_trigger.cpp
 @brief   Android activity trigger library
 DESCRIPTION

 ---------------------------------------------------------------------------
*******************************************************************************/

#define LOG_TAG "ANDR-PERF-ACT-TRIG"
#include <cutils/log.h>
#include <cutils/properties.h>

#include "mp-ctl.h"
#include "client.h"
#include "Whitelists.h"

#define DEBUG_LOGS_PROPERTY "persist.vendor.qti.debug.atlogs_enable"
#define APP_HW_ACCEL_TRIGGER_PROPERTY "persist.vendor.qti.apps.hwaccel.enable"
#define APP_RES_OVERRIDE_TRIGGER_PROPERTY "persist.vendor.qti.res.override.enable"
#define APP_NET_TRIGGER_PROPERTY "persist.vendor.qti.netopts.enable"
#define GAME_TRIGGER_PROPERTY "persist.vendor.qti.games.gt.prof"
#define SCHED_COLOCATE_PROPERTY "persist.vendor.qti.sched.colocate.enable"

#define WHITELISTED_APPS_XML "/system/etc/perf/whitelistedapps.xml"

/*define misc. activty trigger function*/
#define START_PROCESS 1
#define NETWORK_OPTS 2
#define ANIMATION_SCALE 3

/* define default value for sched_group_upmigrate and sched_group_downmigrate. */
#define DEF_SCHED_GROUP_DOWNMIGRATE_TO 90
#define DEF_SCHED_GROUP_UPMIGRATE_TO 95

/* Should match with the values defined at ActivityTrigger.java */
#define FLAG_OVERRIDE_RESOLUTION 1
#define FLAG_HARDWARE_ACCELERATED 0x0200

using namespace std;
using namespace WhiteLists;

////////////////////////////////////////////////////////////////////////////////////
///activity trigger db
////////////////////////////////////////////////////////////////////////////////////
typedef struct SchedColocate {
    int mColocteHandle;
    bool mLockAcq;
    char mPackageName[MAX_APPINFO_LEN];
} SchedColocate;

typedef struct ActivityTrigger {
    bool mHwAccelTrigger;
    bool mResOverrideTrigger;
    bool mNetTrigger;
    bool mColocateTrigger;
    Whitelists mWhitelistsDB;
    struct profile_handle mProfile;
    struct SchedColocate mSchedColoc;
}ActivityTrigger;

static ActivityTrigger sActTrigger;
static bool sEnableGTLogs = false;
////////////////////////////////////////////////////////////////////////////////////
///prototypes
////////////////////////////////////////////////////////////////////////////////////
void activity_trigger_process_start(const char *, int);
void activity_trigger_animationScalesCheck(const char *, int, float *);
void activity_trigger_networkOptsCheck(int, int, const char *);
int activity_trigger_apply_opts(const char *, int *);

int getPackageName(const char *name, char *pName);
void sched_coloc_trigger(const char *name);

///////////////////////////////////////////////////////////////////////////////////////////
///whitelists callbacks/customization support
///////////////////////////////////////////////////////////////////////////////////////////
#define XML_ELEM_ATTR_TAG "AppAttributes"
#define XML_ATTR_PACKAGENAME_TAG "PackageName"
#define XML_ATTR_ACTIVITYNAME_TAG "ActivityName"
#define XML_ATTR_VERSIONCODE_TAG "VersionCode"
#define XML_ATTR_OVERRIDE_RES_TAG "ResolutionOverrideApps"
#define XML_ATTR_GAME_TRIGGER_TAG "GameTriggerApps"
#define XML_ATTR_GAME_COLOCATE_TAG "GameColocateApps"
#define XML_ATTR_ANIM_TRIGGER_TAG "AnimTriggerApps"
#define XML_ATTR_NET_TRIGGER_TAG "NetTriggerApps"
#define XML_ATTR_HW_ACCEL_TAG "HWAccelApps"

void DefaultWhiteListCB(xmlNodePtr node, void *data) {
    char *packagename, *activityname, *versioncode, *ptr;
    char *wvalue, *dvalue, *tscale;
    packagename = activityname = versioncode = NULL;
    bool ret = false;

    Whitelist *appsList = (Whitelist *) data;
    if (NULL == appsList) {
        ALOGI_IF(sEnableGTLogs, "Cb data ptr null, can not add data");
        return;
    }

    if(!xmlStrcmp(node->name, BAD_CAST "AppAttributes")) {
        if(xmlHasProp(node, BAD_CAST "PackageName")) {
            packagename = (char *) xmlGetProp(node, BAD_CAST "PackageName");
        }
        if (NULL == packagename) {
            ALOGI_IF(sEnableGTLogs, "do not have package name, abort");
            return;
        }

        if(xmlHasProp(node, BAD_CAST "ActivityName")) {
            activityname = (char *) xmlGetProp(node, BAD_CAST "ActivityName");
        }
        if(xmlHasProp(node, BAD_CAST "VersionCode")) {
            versioncode = (char *) xmlGetProp(node, BAD_CAST "VersionCode");
        }

        AppInfo *appInfo = new AppInfo(packagename,activityname,versioncode);
        if (appInfo) {
            appsList->mAppInfoList.push_back(appInfo);
            ret = true;
        }

        ALOGI_IF(sEnableGTLogs,
                "Identified pname %s aname %s vcode %s in whitelist",
                packagename ? packagename : "NULL",
                activityname ? activityname : "NULL",
                versioncode ? versioncode : "NULL");

        if (GAME_COLOCATE_WHITELIST == appsList->mType) {
            int downmigrate = -1, upmigrate = -1;
            if(xmlHasProp(node, BAD_CAST "DownMigrate")) {
                ptr = (char *) xmlGetProp(node, BAD_CAST "DownMigrate");
                if(ptr){
                    downmigrate = atoi(ptr);
                    if (downmigrate < 0) {
                        downmigrate = DEF_SCHED_GROUP_DOWNMIGRATE_TO;
                    }
                    xmlFree((void *)ptr);
                    ptr = NULL;
                }
            }
            if(xmlHasProp(node, BAD_CAST "UpMigrate")) {
                ptr = (char *) xmlGetProp(node, BAD_CAST "UpMigrate");
                if(ptr){
                    upmigrate = atoi(ptr);
                    if (upmigrate < 0) {
                        upmigrate = DEF_SCHED_GROUP_UPMIGRATE_TO;
                    }
                    xmlFree((void *)ptr);
                    ptr = NULL;
                }
            }
            appsList->mColocateInfoList.push_back(new ColocateInfo(downmigrate, upmigrate));
        } else if (ANIM_TRIGGER_WHITELIST == appsList->mType) {
            float wanimscale = -1.0f;
            float tanimscale = -1.0f;
            float aanimscale = -1.0f;

            if(xmlHasProp(node, BAD_CAST "WindowAnimationScale")) {
                wvalue = (char *) xmlGetProp(node, BAD_CAST "WindowAnimationScale");
                if (NULL != wvalue) {
                    wanimscale = strtof(wvalue, NULL);
                    xmlFree(wvalue);
                    wvalue = NULL;
                }
            }
            if(xmlHasProp(node, BAD_CAST "TransitionAnimationScale")) {
                tscale = (char *) xmlGetProp(node, BAD_CAST "TransitionAnimationScale");
                if (NULL != tscale) {
                    tanimscale = strtof(tscale, NULL);
                    xmlFree(tscale);
                    tscale = NULL;
                }
            }
            if(xmlHasProp(node, BAD_CAST "AnimatorDurationScale")) {
                dvalue = (char *) xmlGetProp(node, BAD_CAST "AnimatorDurationScale");
                if (NULL != dvalue) {
                    aanimscale = strtof(dvalue, NULL);
                    xmlFree(dvalue);
                    dvalue = NULL;
                }
            }

            appsList->mAnimInfoList.push_back(new AnimInfo(wanimscale, tanimscale, aanimscale));
        }

        if (!ret) {
            //we could not add the record, now need to freeup all the xmlGetProp returns
            if (packagename) {
                xmlFree(packagename);
            }
            if (activityname) {
                xmlFree(activityname);
            }
            if (versioncode) {
                xmlFree(versioncode);
            }
        }
        packagename = activityname = versioncode = NULL;
    }
    return;
}

void WhitelistsInit() {
    Whitelists &wl = sActTrigger.mWhitelistsDB;
    wl.Add(WHITELISTED_APPS_XML);
    wl.Add(OVERRIDE_RES_WHITELIST, XML_ATTR_OVERRIDE_RES_TAG, DefaultWhiteListCB);
    wl.Add(GAME_TRIGGER_WHITELIST, XML_ATTR_GAME_TRIGGER_TAG, DefaultWhiteListCB);
    wl.Add(GAME_COLOCATE_WHITELIST, XML_ATTR_GAME_COLOCATE_TAG, DefaultWhiteListCB);
    wl.Add(ANIM_TRIGGER_WHITELIST, XML_ATTR_ANIM_TRIGGER_TAG, DefaultWhiteListCB);
    wl.Add(NET_TRIGGER_WHITELIST, XML_ATTR_NET_TRIGGER_TAG, DefaultWhiteListCB);
    wl.Add(HW_ACCEL_WHITELIST, XML_ATTR_HW_ACCEL_TAG, DefaultWhiteListCB);
    wl.Parse();
    return;
}

void WhitelistsDeInit() {
    return;
}

///////////////////////////////////////////////////////////////////////////////////
///activity trigger apis
///////////////////////////////////////////////////////////////////////////////////
extern "C" void activity_trigger_init(void) {
    char buf[PROPERTY_VALUE_MAX] = {0};

    sEnableGTLogs = false;
    if((property_get(DEBUG_LOGS_PROPERTY, buf, NULL) > 0) &&
       (!strncmp(buf, "1", PROPERTY_VALUE_MAX ) ||
        !strncasecmp(buf,"true", PROPERTY_VALUE_MAX))) {
        sEnableGTLogs = true;
    }

    WhitelistsInit();

    sActTrigger.mHwAccelTrigger = false;
    if ((property_get(APP_HW_ACCEL_TRIGGER_PROPERTY, buf, "true") > 0) &&
        (!strncasecmp(buf, "true", PROPERTY_VALUE_MAX) ||
         !strncmp(buf,"1", PROPERTY_VALUE_MAX))) {
            ALOGD_IF(sEnableGTLogs, "Setting layout params setting to vendor specific");
            sActTrigger.mHwAccelTrigger = true;
    }

    sActTrigger.mResOverrideTrigger = false;
    if ((property_get(APP_RES_OVERRIDE_TRIGGER_PROPERTY, buf, "true") > 0) &&
        (!strncasecmp(buf, "true", PROPERTY_VALUE_MAX) ||
         !strncmp(buf,"1", PROPERTY_VALUE_MAX))) {
            ALOGD_IF(sEnableGTLogs, "Enabling resolution override opts");
            sActTrigger.mResOverrideTrigger = true;
    }

    sActTrigger.mNetTrigger = false;
    if ((property_get(APP_NET_TRIGGER_PROPERTY, buf, "false") > 0) &&
        (!strncasecmp(buf, "true", PROPERTY_VALUE_MAX) ||
         !strncmp(buf,"1", PROPERTY_VALUE_MAX))) {
            ALOGD_IF(sEnableGTLogs, "enabling net params opts");
            sActTrigger.mNetTrigger = true;
    }

    sActTrigger.mColocateTrigger = false;
    if ((property_get(SCHED_COLOCATE_PROPERTY, buf, "true") > 0) &&
        (!strncasecmp(buf, "true", PROPERTY_VALUE_MAX) ||
         !strncmp(buf,"1", PROPERTY_VALUE_MAX))) {
            ALOGD_IF(sEnableGTLogs, "Enabling colocate trigger");
            sActTrigger.mColocateTrigger = true;
    }

    sActTrigger.mProfile.profile = -1;
    sActTrigger.mProfile.handle = 0;
    if (property_get(GAME_TRIGGER_PROPERTY, buf, NULL) > 0) {
        sActTrigger.mProfile.profile = atoi(buf);
        ALOGD_IF(sEnableGTLogs, "Profile Property set for Profile Number to %d",
                 sActTrigger.mProfile.profile);
    }

    sActTrigger.mSchedColoc.mColocteHandle = -1;
    sActTrigger.mSchedColoc.mLockAcq = false;

    ALOGD_IF(sEnableGTLogs, "Activity trigger library initialized successfully");
}

int getPackageName(const char *name, char *pName) {
    char *tmp_token = NULL;
    char *temp_ptr;
    int len = 0;

    if (name == NULL || pName == NULL) {
        return len;
    }

    tmp_token = strtok_r((char*)name, "/" , &temp_ptr);
    if (tmp_token != NULL) {
        len = strlen(tmp_token);
        strlcpy(pName, tmp_token, len+1);
    }
    return len;
}

extern "C" void activity_trigger_start(const char *name, int *flags) {
    int ret = -1;
    ALOGD_IF(sEnableGTLogs, "App trigger starting '%s'", (name) ? name : "<null>");

    if (!name)
        return;

    ret = activity_trigger_apply_opts(name, flags);

    ALOGD_IF(sEnableGTLogs,"perf:calling NotifyFgAppChange");
    perf_hint(VENDOR_HINT_ACTIVITY_START, name, -1, ret);
}

extern "C" void activity_trigger_resume(const char *name) {
    int flags = 0, ret = -1;
    char pName[MAX_APPINFO_LEN];
    int pkgLen = 0;

    /* Do the same as activity start */
    if (!name)
        return;

    ret = activity_trigger_apply_opts(name, &flags);

    //Check for sched_colocation whitelisting
    if (!sActTrigger.mSchedColoc.mLockAcq) {
        pkgLen = getPackageName(name, pName);
        sched_coloc_trigger(pName);
    }

    perf_hint(VENDOR_HINT_ACTIVITY_RESUME, name, -1, ret);
}

extern "C" void activity_trigger_pause(const char *name) {
    /* Do the same as activity start */
    char pName[MAX_APPINFO_LEN];
    int pkgLen = 0;
    if (!name)
        return;

    //Release the sched_col perflock if already acq for this app
    if (sActTrigger.mSchedColoc.mLockAcq) {
        pkgLen = getPackageName(name, pName);
        if ((pkgLen > 0) && (!strncmp(pName, sActTrigger.mSchedColoc.mPackageName, pkgLen))) {
            perf_lock_rel(sActTrigger.mSchedColoc.mColocteHandle);
            sActTrigger.mSchedColoc.mLockAcq = false;
            sActTrigger.mSchedColoc.mColocteHandle = -1;
        }
    }

    perf_hint(VENDOR_HINT_ACTIVITY_PAUSE, name, -1, -1);
}

extern "C" void activity_trigger_stop(const char *name) {
    /* Do the same as activity start */
    char pName[MAX_APPINFO_LEN];
    int pkgLen = 0;
    if (!name)
        return;

    //Release the sched_col perflock if already acq for this app
    if (sActTrigger.mSchedColoc.mLockAcq) {
        pkgLen = getPackageName(name, pName);
        if ((pkgLen > 0) && (!strncmp(pName, sActTrigger.mSchedColoc.mPackageName, pkgLen))) {
            perf_lock_rel(sActTrigger.mSchedColoc.mColocteHandle);
            sActTrigger.mSchedColoc.mLockAcq = false;
            sActTrigger.mSchedColoc.mColocteHandle = -1;
        }
    }

    perf_hint(VENDOR_HINT_ACTIVITY_STOP, name, -1, -1);
}

extern "C" void activity_trigger_misc(int function, const char *name, int type, int flag, float *scale) {
    /*place for all other miscellaneous function */
    if (!name)
        return;

    switch (function) {
        case START_PROCESS:/*calling activity process start*/
            ALOGD_IF(sEnableGTLogs,"%s: call for activity_trigger_process_start\n", name);
            activity_trigger_process_start(name, type);
            break;
        case NETWORK_OPTS:/*calling network opts*/
            ALOGD_IF(sEnableGTLogs,"%s: call for activity_trigger_networkOptsCheck\n", name);
            activity_trigger_networkOptsCheck(flag, type, name);
            break;
        case ANIMATION_SCALE:/*calling animation scale*/
            ALOGD_IF(sEnableGTLogs,"%s: call for activity_trigger_animationScalesCheck\n", name);
            activity_trigger_animationScalesCheck(name, type, scale);
            break;
        default:
            ALOGD_IF(sEnableGTLogs,"%s: not a valid function call\n", name);
    }
    // Return type is always float
}

extern "C" void activity_trigger_deinit(void) {
    WhitelistsDeInit();
}

//////////////////////////////////////////////////////////////////////////////////////
///support routines
//////////////////////////////////////////////////////////////////////////////////////
void activity_trigger_process_start(const char *name, int pid) {
    //No Op
    return;
}

void sched_coloc_trigger(const char *name) {
    int pos = -1, handle = -1;

    if (!sActTrigger.mColocateTrigger)
        return;

    if (NULL == name) {
        ALOGD_IF(sEnableGTLogs,"App flattened string is NULL");
        return;
    }

    pos = sActTrigger.mWhitelistsDB.IsWhitelisted(GAME_COLOCATE_WHITELIST, name);
    if(pos >= 0) {
        ColocateInfo *cinfo = sActTrigger.mWhitelistsDB.GetColocateWhitelistItem(pos);
        if (cinfo) {
            // sched colocation is implemented via MPCTLV3_SCHED_ENABLE_THREAD_GROUPING and
            // MPCTLV3_SCHED_FREQ_AGGR_GROUP in perflock currently. On Kernel-4.4, group
            // up/down migrate thresholds have been implemented. Reduce to appropriate values
            // for perf kernel 3-18, the opcode will simply fail with no side-effects
            int buf[] = {MPCTLV3_SCHED_GROUP_DOWNMIGRATE, cinfo->mDownMigrate,
                          MPCTLV3_SCHED_GROUP_UPMIGRATE, cinfo->mUpMigrate};
            int len = sizeof(buf)/sizeof(buf[0]);
            if ((!sActTrigger.mSchedColoc.mLockAcq) &&
               ((cinfo->mDownMigrate > 0) || (cinfo->mUpMigrate > 0))) {
                sActTrigger.mSchedColoc.mColocteHandle = perf_lock_acq(handle, 0, buf, len);
                sActTrigger.mSchedColoc.mLockAcq = true;
                strlcpy(sActTrigger.mSchedColoc.mPackageName, name, MAX_APPINFO_LEN);
            }
        }
    } /*if(pos >= 0)*/
    return;
}

void activity_trigger_animationScalesCheck(const char *name, int animType, float *scaleValue) {
    int ret = -1;
    if (NULL == scaleValue) {
        return;
    }

    *scaleValue = -1.0f;

    ret = sActTrigger.mWhitelistsDB.IsWhitelisted(ANIM_TRIGGER_WHITELIST, name);

    if (ret >= 0) {
        AnimInfo *ainfo = sActTrigger.mWhitelistsDB.GetAnimWhitelistItem(ret);
        if (ainfo) {
            *scaleValue = ainfo->mWindowAnimationScale;
            if (animType == 1) {
                *scaleValue = ainfo->mTransitionAnimationScale;
            }
            else if (animType == 2) {
                *scaleValue = ainfo->mAnimatorScale;
            }
        } /*if (ainfo)*/
    }
    return;
}

void activity_trigger_networkOptsCheck(int flag, int netType, const char *name) {
    bool isMobile = (netType == 0) ? true : false;
    bool isWhitelisted = false;
    int buf[2], handle = -1;

    if (sActTrigger.mNetTrigger) {
        buf[0] = MPCTLV3_KEEP_ALIVE;
        isWhitelisted = (sActTrigger.mWhitelistsDB.IsWhitelisted(NET_TRIGGER_WHITELIST, name)>=0);
        (!flag && isMobile && isWhitelisted) ? buf[1] = 0 : buf[1] = 1;
        perf_lock_acq(handle, 100, buf, 2);
    }
    return;
}

int activity_trigger_apply_opts(const char *name, int *flags) {
    int ret = -1;
    bool found = false;

    if (!name) {
        ALOGD_IF(sEnableGTLogs,"App flattened string is NULL");
        return ret;
    }

    if (flags) {
        /* Check for perf applications and apply hw accel */
        if (sActTrigger.mHwAccelTrigger &&
            (sActTrigger.mWhitelistsDB.IsWhitelisted(HW_ACCEL_WHITELIST, name) >= 0)) {
                ALOGD_IF(sEnableGTLogs, "Setting layout params setting to vendor specific");
                *flags |= FLAG_HARDWARE_ACCELERATED;
        }

        /* Check for perf applications and apply resolution override */
        if (sActTrigger.mResOverrideTrigger &&
            (sActTrigger.mWhitelistsDB.IsWhitelisted(OVERRIDE_RES_WHITELIST, name) >= 0)) {
            ALOGD_IF(sEnableGTLogs, "Setting params for resoultion override");
            *flags |= FLAG_OVERRIDE_RESOLUTION;
        }
    }

    //does game trigger enabled?
    if (sActTrigger.mProfile.profile > 0) {
        // Find a game match
        if (sActTrigger.mWhitelistsDB.IsWhitelisted(GAME_TRIGGER_WHITELIST, name) >= 0) {
            found = true;
            ret = 0;
        }

        if ((sActTrigger.mProfile.handle > 0) && !found) {
            //game not whitelisted, profile in use
            ALOGD_IF(sEnableGTLogs,
                    "Game trigger NOT found and profile is in use for '%s'", name);
            sActTrigger.mProfile.handle = perf_lock_use_profile(sActTrigger.mProfile.handle, -1);
            sActTrigger.mProfile.handle = 0;
            ALOGD_IF(sEnableGTLogs, "Profile Removed\n");
        } else if ((sActTrigger.mProfile.handle <= 0) && found) {
            //game whitelisted, profile not in use
            sActTrigger.mProfile.handle = perf_lock_use_profile(sActTrigger.mProfile.handle,
                                                                sActTrigger.mProfile.profile);
            ALOGD_IF(sEnableGTLogs, "profile not in use. Profile Applied for %s\n", name);
        }
    }

    return ret;
}
