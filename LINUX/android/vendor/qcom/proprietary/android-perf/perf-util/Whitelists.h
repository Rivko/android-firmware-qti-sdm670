/******************************************************************************
  @file  Whitelists.h
  @brief  whitelists framework

  whitelists framework

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifndef _WHITE_LIST__H___
#define _WHITE_LIST__H___
#include <string.h>
#include <vector>
#include "XmlParser.h"

namespace WhiteLists {

#define MAX_WHITELISTS 10

#define MAX_APPLIST_SIZE 100
#define MAX_ROLIST_SIZE 20
#define MAX_APPINFO_LEN 1024

/* Whitelists for the following
   features are currently in place */
enum {
    OVERRIDE_RES_WHITELIST = 0,
    GAME_TRIGGER_WHITELIST,
    GAME_COLOCATE_WHITELIST,
    ANIM_TRIGGER_WHITELIST,
    NET_TRIGGER_WHITELIST,
    HW_ACCEL_WHITELIST
};


class AppInfo {
    public:
        const char* mPackageName;
        const char* mActivityName;
        const char* mVersionCode;

    public:
        explicit AppInfo(char *pname, char *aname, char *vcode) {
            mPackageName = pname;
            mActivityName = aname;
            mVersionCode = vcode;
        }

        ~AppInfo() {
            //freeup all xmlGetProp returns
            if (mPackageName) {
                xmlFree((void *)mPackageName);
                mPackageName = NULL;
            }
            if (mActivityName) {
                xmlFree((void *)mActivityName);
                mActivityName = NULL;
            }
            if (mVersionCode) {
                xmlFree((void *)mVersionCode);
                mVersionCode = NULL;
            }
        }
};

class AnimInfo {
    public:
        float mWindowAnimationScale;
        float mTransitionAnimationScale;
        float mAnimatorScale;

    public:
        explicit AnimInfo(float wanimscale, float tanimscale,
                float aanimscale) {
            mWindowAnimationScale = wanimscale;
            mTransitionAnimationScale = tanimscale;
            mAnimatorScale = aanimscale;
        }
        ~AnimInfo() {
        }
};

class ColocateInfo {
    public:
        int mPid;
        int mDownMigrate;
        int mUpMigrate;

    public:
        explicit ColocateInfo(int dmigrate, int umigrate) {
            mDownMigrate = dmigrate;
            mUpMigrate = umigrate;
            mPid = -1;
        }

        ~ColocateInfo() {
        }
};

typedef void (*WhitelistCBType)(xmlNodePtr, void *);

typedef struct Applist {
    int mType;
    string mTag;
    std::vector<AppInfo*> mAppInfoList;
    std::vector<AnimInfo*> mAnimInfoList;
    std::vector<ColocateInfo*> mColocateInfoList;
    WhitelistCBType mWLDataParserCb;
    int mRegistrationId;
} Whitelist;

//one or more whitelists per xml
class Whitelists {
    protected:
        Applist mApplist[MAX_WHITELISTS];
        int mTotal;
        AppsListXmlParser *mXmlParser;
        string mFilePath;

    public:
        Whitelists();
        ~Whitelists();

        bool Add(const char *filepath);
        bool Add(int type, const char *xmlChild, WhitelistCBType cb);
        void Parse();
        void Print();
        int IsWhitelisted(int whiteListType, const char *name);
        AnimInfo *GetAnimWhitelistItem(int pos);
        ColocateInfo *GetColocateWhitelistItem(int pos);
};

}; //for namespace
#endif
