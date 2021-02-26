/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef TEST_CASE_H
#define TEST_CASE_H

#include <vector>
#include "QvopService.h"
#include "logfile.h"

#define PARAM_INDICATOR "--"
#define KEY_USERID "--user-id"
#define KEY_IDENTIFIER "--identifier"
#define KEY_KWPHRASE "--keyphrase"
#define KEY_FILES "--pcm-files"
#define DEFAULT_ENROLL_USER_ID "pacific-gem" //randomly assigned value
#define DEFAULT_ENROLL_IDENTIFIER "styrian-golding" //randomly assigned value
#define DEFAULT_ENROLL_KWPHRASE "nugget" //randomly assigned value
#define DEFAULT_ENROLL_MIC_USER_ID "west-coast-ipa" //randomly assigned value
#define DEFAULT_ENROLL_MIC_IDENTIFIER "30th-street" //randomly assigned value
#define DEFAULT_ENROLL_MIC_KWPHRASE "imperial-ipa" //randomly assigned value
#define DEFAULT_MATCH_USER_ID "passion-fruit" //randomly assigned value
#define DEFAULT_MATCH_IDENTIFIER "cosmic-ristretto" //randomly assigned value
#define DEFAULT_MATCH_FILE "tangerine-soul-style-ipa" //randomly assigned value
#define DEFAULT_DEL_USER_ID "divine-belgique" //randomly assigned value
#define DEFAULT_DEL_IDENTIFIER "le-freak" //randomly assigned value

#define KEY_INPUT_NUM_COLLECTIONS "--number_collections"
#define KEY_INPUT_SOURCE_TRACKING_ENABLED "--source-tracking-enabled"
#define KEY_INPUT_SOURCE_TRACKING_DISABLED "--source-tracking-disabled"
#define KEY_INPUT_DEVICE "--input-device"
#define KEY_INPUT_CHANNELS "--input-channels"
#define KEY_INPUT_FORMAT "--input-format"
#define KEY_INPUT_SAMPLE_RATE "--input-sample-rate"
#define KEY_INPUT_SOURCE "--input-source"
#define KEY_INPUT_LOOPTIME "--input-looptime"
#define KEY_NUM_OF_KEYWORDS "--num_keywords"
#define KEY_KW_CONF_LEVEL "--kw_conf_level"
#define KEY_NO_OF_USERS "--num_users"
#define KEY_USER_CONF_LEVEL "--usr_conf_level"
#define KEY_KB_DURATION "--kb_duration"
#define KEY_SM_FILE_PATH "--sm_file_path"

#define KEY_MATCH_EXPECTED "--match"
#define KEY_MATCH_NOT_EXPECTED "--no-match"

#define DEFAULT_ENROLL_MIC_COLLECTIONS 3
#define DEFAULT_MATCH_MIC_COLLECTIONS 1
#define DEFAULT_SRC_TRACKING 1
#define BUILT_IN_MIC 4
#define HEADSET 16
#define DEFAULT_INPUT_DEVICE BUILT_IN_MIC
#define MONO 1
#define STEREO 2
#define QUAD 4
#define DEFAULT_NUM_CHANNELS MONO
#define FORMAT_16_BIT 16
#define FORMAT_24_BIT 24
#define DEFAULT_FORMAT FORMAT_16_BIT
#define DEFAULT_SAMPLE_RATE 16000
#define DEFAUT_MIC_SOURCE 1 //header file
#define DEFAULT_LOOPTIME 2500
#define DEFAULT_KB_DURATION 2000
#define DEFAULT_NO_OF_KEYWORDS 1
#define DEFAULT_KW_CONF_LEVEL 60
#define DEFAULT_NO_OF_USERS 1
#define DEFAULT_USER_CONF_LEVEL 60
#define DEFAULT_SOUND_MODEL_FILE "/data/HeySnapdragon.uim"

enum TestCaseResult {TCSuccess, TCFailure, TCBadParams};

using namespace std;
using namespace qvop;

/*
* Base class for all test cases
*/
class TestCase {
public:
    TestCase() {};
    virtual ~TestCase() {};
    virtual TestCaseResult Run(QvopService& service, vector<string> const&) = 0;
};

#endif //TEST_CASE_H
