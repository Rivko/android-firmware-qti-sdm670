/******************************************************************************
  @file  Whitelists.cpp
  @brief  whitelists framework

  whitelists framework

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "Whitelists.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ANDR-PERFUTIL-WHITELISTS"
#include <cutils/log.h>

using namespace WhiteLists;

#if QTI_DEBUG
#  define QLOGE(...)    ALOGE(__VA_ARGS__)
#  define QLOGD(...)    ALOGD(__VA_ARGS__)
#  define QLOGW(...)    ALOGW(__VA_ARGS__)
#  define QLOGI(...)    ALOGI(__VA_ARGS__)
#  define QLOGV(...)    ALOGV(__VA_ARGS__)
#else
#  define QLOGE(...)
#  define QLOGD(...)
#  define QLOGW(...)
#  define QLOGI(...)
#  define QLOGV(...)
#endif

#define XML_ROOT_TAG "AppCatalog"

Whitelists::Whitelists() {
    mXmlParser = new AppsListXmlParser();
    if (NULL == mXmlParser) {
        QLOGE("xmlParser failed to create %s", __func__);
    }
}

Whitelists::~Whitelists() {
    int i =0;
    unsigned int j = 0;
    //delete all nodes that were allocated when passed in to the vectors
    for (i=0; i<mTotal; i++) {
        for (j=0; j<mApplist[i].mAppInfoList.size(); j++) {
            delete mApplist[i].mAppInfoList[j];
        }
        mApplist[i].mAppInfoList.clear();

        if (GAME_COLOCATE_WHITELIST == mApplist[i].mType) {
            for (j=0; j<mApplist[i].mColocateInfoList.size(); j++) {
                delete mApplist[i].mColocateInfoList[j];
            }
            mApplist[i].mColocateInfoList.clear();
        }
        else if (ANIM_TRIGGER_WHITELIST == mApplist[i].mType) {
            for (j=0; j<mApplist[i].mAnimInfoList.size(); j++) {
                delete mApplist[i].mAnimInfoList[j];
            }
            mApplist[i].mAnimInfoList.clear();
        }
    }

    if (mXmlParser) {
        delete mXmlParser;
    }
}

bool Whitelists::Add(int type, const char *xmlChild, WhitelistCBType cb) {
    const string xmlRoot(XML_ROOT_TAG);

    if (mTotal < MAX_WHITELISTS) {
        mApplist[mTotal].mType = type;
        mApplist[mTotal].mTag = xmlChild;
        mApplist[mTotal].mWLDataParserCb = cb;

        if (NULL != mXmlParser) {
            mApplist[mTotal].mRegistrationId = mXmlParser->Register(xmlRoot, xmlChild, cb, &mApplist[mTotal]);
        }
        mTotal++;
        return true;
    } else
        return false;
}

bool Whitelists::Add(const char *filepath) {
    mFilePath = filepath;
    return true;
}

void Whitelists::Parse() {
    if (NULL != mXmlParser) {
        mXmlParser->Parse(mFilePath);
        for (int i=0; i<mTotal; i++) {
            mXmlParser->DeRegister(mApplist[i].mRegistrationId);
        }
    }
    return;
}

void Whitelists::Print() {
    int i = 0;

    for (i=0; i<mTotal; i++) {
        QLOGI("Whitelist: %s", mApplist[i].mTag.c_str());

        for (vector<AppInfo*>::iterator it = mApplist[i].mAppInfoList.begin();
                it != mApplist[i].mAppInfoList.end(); ++it) {
            QLOGI("PackageName %s ActivityName %s",
                  (*it)->mPackageName ? (*it)->mPackageName : "NULL",
                  (*it)->mActivityName ? (*it)->mActivityName : "NULL");
        }
    }
    return;
}

int Whitelists::IsWhitelisted(int whiteListType, const char *name) {
    /* flattened app string "name" will be as follows:
     * packageName/activityName/versionCode
     */
    int ret = -1;
    char fs[MAX_APPINFO_LEN];
    strlcpy(fs,name,MAX_APPINFO_LEN);
    char *tmp_token = NULL;
    char *temp_ptr;
    char *packagename, *activityname, *versioncode;
    packagename = activityname = versioncode = NULL;

    tmp_token = strtok_r((char*)fs, "/" , &temp_ptr);
    int i = 0;
    while(tmp_token != NULL) {
        switch(i) {
            case 0:
                packagename = tmp_token;
                break;
            case 1:
                activityname = tmp_token;
                break;
            case 2:
                versioncode = tmp_token;
                break;
        }
        i++;
        tmp_token = strtok_r(NULL, "/",  &temp_ptr);
    }

    QLOGI("flattenedString: PackageName %s ActivityName %s VersionCode %s",
          packagename ? packagename : "NULL",
          activityname ? activityname : "NULL",
          versioncode ? versioncode : "NULL");

    //if no package name, do not proceed
    if(!packagename) {
        QLOGI("packagename is NULL. Not whitelisted");
        return ret;
    }

    //anim trigger is particular about activity name also
    if (ANIM_TRIGGER_WHITELIST == whiteListType) {
        if(!activityname) {
            QLOGI("activity name is NULL. Not whitelisted");
            return ret;
        }
    }

    vector<AppInfo*>::iterator itbegin;
    vector<AppInfo*>::iterator itend;
    const char* wlname;
    for (i=0; i<mTotal; i++) {
        if (whiteListType == mApplist[i].mType) {
            itbegin = mApplist[i].mAppInfoList.begin();
            itend = mApplist[i].mAppInfoList.end();
            wlname = mApplist[i].mTag.c_str();
            break;
        }
    }

    if (i >= mTotal) {
        //no whitelist found
        QLOGI("No whitelist found");
        return ret;
    }

    for (vector<AppInfo*>::iterator it = itbegin; it != itend; ++it) {
        if(packagename && !strcmp((*it)->mPackageName, packagename)) {
            if(!((*it)->mActivityName) || (!activityname) ||
                    !strcmp((*it)->mActivityName, activityname)) {
                if(!((*it)->mVersionCode) || !versioncode ||
                        !strcmp((*it)->mVersionCode, versioncode)) {
                    QLOGI("Package %s whitelisted for %s",
                          packagename ? packagename : "NULL", wlname);
                    ret = distance(itbegin, it);
                    break;
                }
            }
        }
    }

    if (ret < 0) {
        QLOGI("Activity %s not whitelisted for %s",
              activityname ? activityname : "NULL",
              wlname);
    }
    return ret;
}

AnimInfo *Whitelists::GetAnimWhitelistItem(int pos) {
    int i = 0;
    AnimInfo *ainfo = NULL;

    for (i=0; i<mTotal && i<MAX_WHITELISTS; i++) {
        if (ANIM_TRIGGER_WHITELIST == mApplist[i].mType) {
            if ((pos >=0) && (pos < (int)mApplist[i].mAnimInfoList.size())) {
                ainfo = mApplist[i].mAnimInfoList[pos];
            }
            break;
        }
    }
    return ainfo;
}

ColocateInfo *Whitelists::GetColocateWhitelistItem(int pos) {
    int i = 0;
    ColocateInfo *cinfo = NULL;

    for (i=0; i<mTotal && i<MAX_WHITELISTS; i++) {
        if (GAME_COLOCATE_WHITELIST == mApplist[i].mType) {
            if ((pos >=0) && (pos < (int)mApplist[i].mColocateInfoList.size())) {
                cinfo = mApplist[i].mColocateInfoList[pos];
            }
            break;
        }
    }
    return cinfo;
}

