////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  BufferManager.h
/// @brief Buffer Manager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestLog.h"
#include <stdio.h>
#define LOG_TAG "CameraTest"
#include <log/log.h>
#define LOG_LENGHT 256

TestLog::TestLog()
{
}

TestLog::~TestLog()
{
    if (mType == LFILE){
        fclose(mFile);
    }
    mIsNewPath = false;
}

TestLog::TestLog(string logpath)
{
    mFile = stderr;
    mType = LALOGE;
    if (logpath == ""){
        mIsNewPath = false;
    } else {
        mIsNewPath = true;
    }
    mPath = logpath;
    mTag = "[META] ";
}

void TestLog::setPath(string logfile)
{
    if (logfile == "std"){
        mType = LSTDIO;
    } else if (logfile == "ALOGE"){
        mType = LALOGE;
    } else {
        mType = LFILE;
        string logpath = CAMERA_STORAGE_DIR+logfile;
        if (logpath != mPath){
            mIsNewPath = true;
            mPath = logpath;
        }
    }
}

int TestLog::print(const char *format, ...)
{
    int res = 0;
    char m[LOG_LENGHT];
    va_list ap;
    va_start(ap,format);
    vsnprintf(m,LOG_LENGHT,format,ap);
    va_end(ap);
    switch (mType){
    case LSTDIO:{
        printf("%s %s",mTag.c_str(),m);
        break;
    }
    case LALOGE:{
        __android_log_write(ANDROID_LOG_INFO, mTag.c_str(), m);
        break;
    }
    case LFILE:{
        if (mIsNewPath){
            FILE* newfile = fopen(mPath.c_str(),"w+");
            fclose(mFile);
            mFile = newfile;
            mIsNewPath = false;
            printf("DUMP LOG BEGAIN\n");
        }
        res = fprintf(mFile,"%s %s",mTag.c_str(),m);
        break;
    }
    default:
        break;
    }
    return res;
}
