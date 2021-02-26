// NOWHINE ENTIRE FILE
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif // __cplusplus

#if defined(ANDROID) || defined(_ANDROID_)
#include <android/log.h> // Logcat logging functions
#endif // ANDROID || _ANDROID_



/* Return Values */
#define NC_LIB_SUCCESS                  (0)                             /**< Success return value */

#define NC_LIB_ERRORE_BASE              (0x04000000)
#define NC_LIB_GENERAL_ERROR            (NC_LIB_ERRORE_BASE + 1)        /**< General error return value */
#define NC_LIB_INVALID_INPUT            (NC_LIB_ERRORE_BASE + 2)        /**< Invalid input */

#define NC_LIB_WARN_INVALID_INPUT       (NC_LIB_ERRORE_BASE + 0x100)    /**< Warning: Invalid input, possible performance or IQ degradation*/

// PASS_TYPE is the power of 4 of the scale of the pass. 4^PASS_TYPE. example: 4^0 = 1. example: 4^3 = 64.
typedef enum
{
    PASS_TYPE_FULL = 0,
    PASS_TYPE_DC4  = 1,
    PASS_TYPE_DC16 = 2,
    PASS_TYPE_DC64 = 3,
    PASS_TYPE_NUM
} PASS_TYPE;


//typedef enum
//{
//  NCLIB_FALSE = 0,
//  NCLIB_TRUE = 1
//} NCLIB_BOOLEAN;

typedef bool NCLIB_BOOLEAN;
#define NCLIB_FALSE (false)
#define NCLIB_TRUE  (true)


// LOG_LEVEL_DEBUG -   print LOGD, LOGW and LOGE
// LOG_LEVEL_WARNING - print LOGW and LOGE
// LOG_LEVEL_ERROR -   print LOGE
#define LOG_LEVEL_DEBUG   (1)
#define LOG_LEVEL_WARNING (2)
#define LOG_LEVEL_ERROR   (3)
#define LOG_LEVEL_NONE    (4)

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_ERROR
#endif /*LOG_LEVEL */

#if defined(ANDROID) || defined(_ANDROID_)

#define NCLIB_LOG(level, levelString, fmt, ...) \
    { __android_log_print((level), "NcLib", levelString " NcLib %s(%u): " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
      printf("\n" levelString "%s(%u): ", __FUNCTION__ ,__LINE__);printf(fmt, ##__VA_ARGS__);printf("\n"); }

#else // ANDROID || _ANDROID_

#define ANDROID_LOG_DEBUG  LOG_LEVEL_DEBUG
#define ANDROID_LOG_WARN   LOG_LEVEL_WARNING
#define ANDROID_LOG_ERROR  LOG_LEVEL_ERROR
#define ANDROID_LOG_SILENT LOG_LEVEL_NONE
#define NCLIB_LOG(level, levelString, ...) \
    { printf(levelString "%s(%u): ", __FUNCTION__ ,__LINE__);printf(__VA_ARGS__);printf("\n"); }

#endif // ANDROID || _ANDROID_

// LOGS
#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define  LOGD(...)  NCLIB_LOG(ANDROID_LOG_DEBUG, "[DEBUG]   ", __VA_ARGS__)
#else
#define  LOGD(...)  {}
#endif
#if (LOG_LEVEL <= LOG_LEVEL_WARNING)
#define  LOGW(...)  NCLIB_LOG(ANDROID_LOG_WARN, "[WARNING] ", __VA_ARGS__)
#else
#define  LOGW(...)  {}
#endif
#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define  LOGE(...)  NCLIB_LOG(ANDROID_LOG_ERROR, "[SERIOUS WARNING] ", __VA_ARGS__)
#else
#define  LOGE(...)  {}
#endif


#if defined(ANDROID) || defined(_ANDROID_)
    // Right now there is special log message in case of failure
    #define NCLIB_ASSERT(c) { if (!(c)) {LOGE("Assertion failed: %s, file %s, line %d\n", #c, __FILE__, __LINE__ ); assert(false);} }
    //
    // The next line can be used instead (since assert() is removed soemtimes in opeational code).to save run time.
    //#define NCLIB_ASSERT assert

#else // ! defined(ANDROID || _ANDROID_)
    #define NCLIB_ASSERT(c) { if (!(c)) {LOGE("Assertion failed: %s, file %s, line %d\n", #c, __FILE__, __LINE__ ); __debugbreak();  } }
#endif // WIN32


#ifdef NCLIB_CRASH_ON_RESTRICTION_VIOLATION
#define NCLIB_RESTRICTION_VIOLATION(msg) {LOGE("RESTRICTION_VIOLATION: replace invalid setting");LOGE(msg);NCLIB_ASSERT(false);}
#define NCLIB_RESTRICTION_VIOLATION_CONDITIONAL(condition, msg) if(false == (condition)){LOGE("RESTRICTION_VIOLATION: replace invalid setting");LOGE(msg);NCLIB_ASSERT(false);}
#else
#define NCLIB_RESTRICTION_VIOLATION(msg) {LOGE("RESTRICTION_VIOLATION: replace invalid setting");LOGE(msg);}
#define NCLIB_RESTRICTION_VIOLATION_CONDITIONAL(condition, msg) if(false == (condition)){LOGE("RESTRICTION_VIOLATION: replace invalid setting");LOGE(msg)}
#endif

#define NCLIB_RESTRICTION_WARNING(msg)  {LOGW("RESTRICTION_WARNING: ");LOGW(msg);}

// if set to 1, check that all registers are set by NcLib logic layer.
// If set to 0, this debug infrastructure is disabled
// Please note that this debug flag change NcLib logic:
// 1. Skip setting of registers to default value.
// 2. Skip early return on Bypass mode
#define DEBUG_CHECK_ALL_REGS_SET 0
#define DEBUG_INVALID_UNSET_VALUE 0xFEDCBA98;
#define IS_DEBUG_CHECK_ALL_REGS_SET() DEBUG_CHECK_ALL_REGS_SET


typedef uint32_t FIELD_UINT;
typedef int32_t FIELD_INT;

typedef uint32_t PARAM_UINT;
typedef int32_t PARAM_INT;

typedef float PARAM_FLOAT;

#define SET_PARAM( paramName, setVal)\
    paramName = setVal;

#if ( 1 == DEBUG_CHECK_ALL_REGS_SET )


#define SET_REG_FIELD_DEFAULT_VAL( fieldName, setVal)\
    fieldName = DEBUG_INVALID_UNSET_VALUE;

// set LUT
#define SET_LUT_DEFAULT_VAL( paramName, lutSize, setVal)\
{\
    uint32_t lutIndex;\
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        paramName[lutIndex] = DEBUG_INVALID_UNSET_VALUE; \
    }\
}
#else // not DEBUG_CHECK_ALL_REGS_SET


#define SET_REG_FIELD_DEFAULT_VAL( fieldName, setVal)\
    fieldName = setVal;

// set LUT
#define SET_LUT_DEFAULT_VAL( paramName, lutSize, setVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        paramName[lutIndex] = setVal; \
    }\
}
#endif // DEBUG_CHECK_ALL_REGS_SET

#define SET_LUT_PARAM( paramName, lutSize, setVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        paramName[lutIndex] = setVal; \
    }\
}

#define SET_LUT_DEFAULT_LIST( paramName, lutSize, listVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        paramName[lutIndex] = listVal[lutIndex]; \
    }\
}


///////////////////////////////////////////////////
// Validate params
#define VALIDATE_MIN_MAX_SIGNED_PARAM(param, minVal, maxVal)\
if ((param > maxVal) || (param < minVal))\
{\
    LOGE("VALIDATE_MIN_MAX_SIGNED_PARAM failed param: %s minVal:%d, maxVal:%d actualVal:%d", #param, minVal, maxVal, param); \
    retVal += 1; \
}

#define VALIDATE_MIN_MAX_UNSIGNED_PARAM(param, minVal, maxVal)\
if ((param > maxVal) || (param < minVal))\
{\
    LOGE("VALIDATE_MIN_MAX_PARAM failed param: %s minVal:%u, maxVal:%u actualVal:%u", #param, minVal, maxVal, param); \
    retVal += 1; \
}

#define VALIDATE_MIN_MAX_FLOAT_PARAM(param, minVal, maxVal)\
if ((param > maxVal) || (param < minVal))\
{\
LOGE("VALIDATE_MIN_MAX_PARAM failed param: %s minVal:%f, maxVal:%f actualVal:%f", #param, minVal, maxVal, param); \
retVal += 1; \
}


#define VALIDATE_MAX_SIGNED_PARAM(param, maxVal)\
if (param > maxVal)\
{\
    LOGE("VALIDATE_MAX_PARAM failed param: %s maxVal:%d actualVal:%d", #param, maxVal, param); \
    retVal += 1; \
}


#define VALIDATE_MAX_UNSIGNED_PARAM(param, maxVal)\
if (param > maxVal)\
{\
    LOGE("VALIDATE_MAX_PARAM failed param: %s maxVal:%u actualVal:%u", #param, maxVal, param); \
    retVal += 1; \
}

///////////////////////////////////////////////////
// Validate LUT min and max

#define VALIDATE_LUT_MIN_MAX_SIGNED_PARAM( paramName, lutSize, minVal, maxVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        if ((paramName[lutIndex] > maxVal) || (paramName[lutIndex] < minVal))\
        {\
            LOGE("VALIDATE_LUT_MIN_MAX_PARAM failed param: %s minVal:%d, maxVal:%d actualVal:%d", #paramName, minVal, maxVal, paramName[lutIndex]); \
            retVal += 1; \
            break; \
        }\
    }\
}

#define VALIDATE_LUT_MIN_MAX_UNSIGNED_PARAM( paramName, lutSize, minVal, maxVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        if ((paramName[lutIndex] > maxVal) || (paramName[lutIndex] < minVal))\
        {\
            LOGE("VALIDATE_LUT_MIN_MAX_PARAM failed param: %s minVal:%u, maxVal:%u actualVal:%u", #paramName, minVal, maxVal, paramName[lutIndex]); \
            retVal += 1; \
            break; \
        }\
    }\
}

#define VALIDATE_LUT_MIN_MAX_FLOAT_PARAM( paramName, lutSize, minVal, maxVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        if ((paramName[lutIndex] > maxVal) || (paramName[lutIndex] < minVal))\
        {\
            LOGE("VALIDATE_LUT_MIN_MAX_PARAM failed param: %s minVal:%f, maxVal:%f actualVal:%f", #paramName, minVal, maxVal, paramName[lutIndex]); \
            retVal += 1; \
            break; \
        }\
    }\
}

#define VALIDATE_LUT_MAX_SIGNED_PARAM( paramName, lutSize, maxVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        if (paramName[lutIndex] > maxVal)\
        {\
            LOGE("VALIDATE_LUT_MAX_PARAM failed param: %s maxVal:%d actualVal:%d", #paramName, maxVal, paramName[lutIndex]); \
            retVal += 1; \
            break; \
        }\
    }\
}

#define VALIDATE_LUT_MAX_UNSIGNED_PARAM( paramName, lutSize, maxVal)\
{\
    uint32_t lutIndex; \
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        if (paramName[lutIndex] > maxVal)\
        {\
            LOGE("VALIDATE_LUT_MAX_UNSIGNED_PARAM failed param: %s maxVal:%u actualVal:%u", #paramName, maxVal, paramName[lutIndex]); \
            retVal += 1; \
            break; \
        }\
    }\
}


///////////////////////////////////////////////////
// Validate several valid values
#define VALIDATE_LUT_SEVERAL_VALID_VALUES(fieldName, lutSize, list)\
{\
    uint32_t lutIndex; \
    uint32_t iter;\
    for (lutIndex = 0; lutIndex < lutSize; lutIndex++)\
    {\
        {\
            int32_t isValid = 0; \
            for (iter = 0; iter < (sizeof(list) / sizeof (int32_t)); iter++)\
            {\
                if (list[iter] == fieldName[lutIndex])\
                {\
                    isValid = 1; \
                    break; \
                }\
            }\
            if (0 == isValid)\
            {\
                LOGE("VALIDATE_LUT_SEVERAL_VALID_VALUES failed fieldName: %s", #fieldName); \
                retVal += 1; \
            }\
        }\
    }\
}

///////////////////////////////////////////////////
// Validate several valid values
#define VALIDATE_SEVERAL_VALID_VALUES(param, list)\
{\
    int32_t isValid = 0; \
    uint32_t iter;\
    for (iter = 0; iter < (sizeof(list) / sizeof (int32_t)); iter++)\
    {\
        if (list[iter] == reg.val)\
        {\
            isValid = 1; \
            break; \
        }\
    }\
    if (0 == isValid)\
    {\
        LOGE("VALIDATE_SEVERAL_VALID_VALUES failed reg: %s", #param); \
        retVal += 1; \
    }\
}

#define VALIDATE_REG_FIELD_SEVERAL_VALID_VALUES(fieldName, list)\
{\
    int32_t isValid = 0; \
    uint32_t iter;\
    for (iter = 0; iter < (sizeof(list) / sizeof (int32_t)); iter++)\
    {\
        if (list[iter] == fieldName)\
        {\
            isValid = 1; \
            break; \
        }\
    }\
    if (0 == isValid)\
    {\
        LOGE("VALIDATE_REG_FIELD_SEVERAL_VALID_VALUES failed field: %s", #fieldName); \
        retVal += 1; \
    }\
}

#define VALIDATE_SEVERAL_VALID_VALUES_PARAM(param, list)\
{\
    int32_t isValid = 0; \
    uint32_t iter;\
    for (iter = 0; iter < (sizeof(list) / sizeof (int32_t)); iter++)\
    {\
        if (list[iter] == param)\
        {\
            isValid = 1; \
            break; \
        }\
    }\
    if (0 == isValid)\
    {\
        LOGE("VALIDATE_SEVERAL_VALID_VALUES_PARAM failed param: %s", #param); \
        retVal += 1; \
    }\
}



#define VALIDATE_REG_FIELD_SIGNED_MIN_MAX(fieldName, minVal, maxVal)\
if ((fieldName > maxVal) || (fieldName < minVal))\
{\
    LOGE("VALIDATE_REG_FIELD_SIGNED_MIN_MAX failed register: %s minVal:%d, maxVal:%d actualVal:%d", #fieldName, minVal, maxVal, fieldName); \
    retVal += 1; \
}

#define VALIDATE_REG_FIELD_UNSIGNED_MIN_MAX(fieldName, minVal, maxVal)\
if ((fieldName > maxVal) || (fieldName < minVal))\
{\
    LOGE("VALIDATE_REG_FIELD_UNSIGNED_MIN_MAX failed register: %s minVal:%u, maxVal:%u actualVal:%u", #fieldName, minVal, maxVal, fieldName); \
    retVal += 1; \
}

#define WARN_REG_FIELD_UNSIGNED_MIN_MAX(fieldName, minVal, maxVal)\
if ((fieldName > maxVal) || (fieldName < minVal))\
{\
    LOGW("WARN_REG_FIELD_UNSIGNED_MIN_MAX failed register: %s minVal:%u, maxVal:%u actualVal:%u", #fieldName, minVal, maxVal, fieldName); \
}

#define WARN_REG_FIELD_UNSIGNED_MAX(fieldName, maxVal)\
if (fieldName > maxVal)\
{\
    LOGW("WARN_REG_FIELD_UNSIGNED_MAX failed register: %s maxVal:%u actualVal:%u",#fieldName, maxVal, fieldName); \
}

#define WARN_REG_FIELD_SIGNED_MIN_MAX(fieldName, minVal, maxVal)\
if ((fieldName > maxVal) || (fieldName < minVal))\
{\
    LOGW("WARN_REG_FIELD_SIGNED_MIN_MAX failed register: %s minVal:%d, maxVal:%d actualVal:%d", #fieldName, minVal, maxVal, fieldName); \
}

#define WARN_REG_FIELD_SIGNED_MAX(fieldName, maxVal)\
if (fieldName > maxVal)\
{\
    LOGW("WARN_REG_FIELD_SIGNED_MAX failed register: %s maxVal:%d actualVal:%d",#fieldName, maxVal, fieldName); \
}

#define VALIDATE_REG_FIELD_SIGNED_MAX(fieldName, maxVal)\
if (fieldName > maxVal)\
{\
    LOGE("VALIDATE_REG_FIELD_SIGNED_MAX failed register: %s maxVal:%d actualVal:%d", #fieldName, maxVal, fieldName); \
    retVal += 1; \
}

#define VALIDATE_REG_FIELD_UNSIGNED_MAX(fieldName, maxVal)\
if (fieldName > maxVal)\
{\
    LOGE("VALIDATE_REG_FIELD_UNSIGNED_MAX failed register: %s maxVal:%u actualVal:%u", #fieldName, maxVal, fieldName); \
    retVal += 1; \
}

#define VALIDATE_REG_FIELD_SIGNED_ODD(fieldName)\
if ((0 == (fieldName & 1)))\
{\
    LOGE("VALIDATE_REG_FIELD_SIGNED_ODD failed param: %s register:%d", #fieldName, fieldName); \
    retVal += 1; \
}

#define VALIDATE_REG_FIELD_UNSIGNED_ODD(fieldName)\
if ((0 == (fieldName & 1)))\
{\
    LOGE("VALIDATE_REG_FIELD_UNSIGNED_ODD failed param: %s register:%u", #fieldName, fieldName); \
    retVal += 1; \
}

#define VALIDATE_REG_FIELD_UNSIGNED_EVEN(fieldName)\
if ((1 == (fieldName & 1)))\
{\
    LOGE("VALIDATE_REG_FIELD_UNSIGNED_EVEN failed param: %s register:%u", #fieldName, fieldName); \
    retVal += 1; \
}


#define VALIDATE_ODD_UNSIGNED_PARAM(param)\
if ((0 == (param & 1)))\
{\
    LOGE("VALIDATE_ODD_UNSIGNED_PARAM failed param: %s val:%u", #param, param); \
    retVal += 1; \
}

#define VALIDATE_EVEN_UNSIGNED_PARAM(param)\
if ((1 == (param & 1)))\
{\
    LOGE("VALIDATE_EVEN_UNSIGNED_PARAM failed param: %s val:%", #param, param); \
    retVal += 1; \
}

#define PRINT_XML_PARAM_UINT( paramName, paramVal, numOfTabsToAdd ) \
{\
    uint32_t tabId;\
    for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
        fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, "<%s>%u</%s>\n", paramName, paramVal, paramName);\
}

#define PRINT_XML_PARAM_INT( paramName, paramVal, numOfTabsToAdd ) \
{\
    uint32_t tabId;\
    for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
        fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, "<%s>%d</%s>\n", paramName, paramVal, paramName);\
}

#define PRINT_XML_PARAM_FLOAT( paramName, paramVal, numOfTabsToAdd ) \
{\
    uint32_t tabId; \
for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
    fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, "<%s>%f</%s>\n", paramName, paramVal, paramName); \
}


#define PRINT_XML_PARAM_UINT_LUT(paramName, paramVal, lutSize, numOfTabsToAdd);\
{\
    uint32_t lutIndex;\
    uint32_t tabId;\
    for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
        fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, "<%s>%d", paramName, paramVal[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, " %d", paramVal[lutIndex]); \
    }\
    fprintf(dumpRegFile, "</%s>\n", paramName);\
}

#define PRINT_XML_PARAM_INT_LUT(paramName, paramVal, lutSize, numOfTabsToAdd);\
{\
    uint32_t lutIndex;\
    uint32_t tabId;\
    for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
        fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, "<%s>%d", paramName, paramVal[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, " %d", paramVal[lutIndex]); \
    }\
    fprintf(dumpRegFile, "</%s>\n", paramName);\
}

#define PRINT_XML_PARAM_FLOAT_LUT(paramName, paramVal, lutSize, numOfTabsToAdd);\
{\
    uint32_t lutIndex; \
    uint32_t tabId; \
for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
    fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, "<%s>%f", paramName, paramVal[0]); \
for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
    fprintf(dumpRegFile, " %f", paramVal[lutIndex]); \
    }\
    fprintf(dumpRegFile, "</%s>\n", paramName); \
}

#define PRINT_HEADER(name) \
    fprintf(dumpRegFile, "%s\n", name);

#define PRINT_HEADER_TAB(name, numOfTabsToAdd) \
{\
    uint32_t tabId;\
    for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
        fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, "%s\n", name);\
}

#define PRINT_HEADER_PLUS_STRING_TAB(fprintfVar, string, numOfTabsToAdd) \
{\
    uint32_t tabId;\
    for (tabId = 0; tabId < numOfTabsToAdd; tabId++)\
        fprintf(dumpRegFile, "    "); \
    fprintf(dumpRegFile, fprintfVar, string);\
}


#define PRINT_REGISTER( name, reg ) \
    fprintf(dumpRegFile, "%s = %u\n", name, reg.packed);

#define PRINT_FIELD_UINT( name, field ) \
    fprintf(dumpRegFile, "%s = %u\n", name, field);

#define PRINT_FIELD_INT( name, field ) \
    fprintf(dumpRegFile, "%s = %d\n", name, field);

#define PRINT_REGISTER_LUT_PACK(name, regLutName, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "%s=%u", name, regLutName.packed[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%u", regLutName.packed[lutIndex]); \
    }\
    fprintf(dumpRegFile, "\n");\
}

#define PRINT_REGISTER_LUT_PACK_64(name, regLutName, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "%s=%llu", name, regLutName.packed[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%llu", regLutName.packed[lutIndex]); \
    }\
    fprintf(dumpRegFile, "\n");\
}


#define PRINT_XML_FIELD_UINT( name, val ) \
    fprintf(dumpRegFile, "            <param name=\"%s\" type=\"uint\">\n", name);\
    fprintf(dumpRegFile, "                <value>%u</value>\n", val);\
    fprintf(dumpRegFile, "            </param>\n");


#define PRINT_XML_FIELD_INT( name, val ) \
    fprintf(dumpRegFile, "            <param name=\"%s\" type=\"int\">\n", name);\
    fprintf(dumpRegFile, "                <value>%d</value>\n", val);\
    fprintf(dumpRegFile, "            </param>\n");

#define PRINT_XML_REGISTER_LUT_UNPACK_UINT(name, val, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "            <param name=\"%s\" type=\"uint\" format=\"1d_array\" size=\"%d\">\n", name, lutSize); \
    fprintf(dumpRegFile, "                <value>%u",  val[0]);\
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%u", val[lutIndex]); \
    }\
    fprintf(dumpRegFile, "</value>\n");\
    fprintf(dumpRegFile, "            </param>\n");\
}

#define PRINT_XML_REGISTER_LUT_UNPACK_INT(name, val, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "            <param name=\"%s\" type=\"int\" format=\"1d_array\" size=\"%d\">\n", name, lutSize); \
    fprintf(dumpRegFile, "                <value>%d",  val[0]);\
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%d", val[lutIndex]); \
    }\
    fprintf(dumpRegFile, "</value>\n");\
    fprintf(dumpRegFile, "            </param>\n");\
}

#define PRINT_XML_REGISTER_LUT_UNPACK_64INT(name, val, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "            <param name=\"%s\" type=\"int\" format=\"1d_array\" size=\"%d\">\n", name, lutSize); \
    fprintf(dumpRegFile, "                <value>%lld",  val[0]);\
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%lld", val[lutIndex]); \
    }\
    fprintf(dumpRegFile, "</value>\n");\
    fprintf(dumpRegFile, "            </param>\n");\
}

#define PRINT_REGISTER_LUT_UNPACK_UINT(name, regLutName, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "%s=%u", name, regLutName[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%u", regLutName[lutIndex]); \
    }\
    fprintf(dumpRegFile, "\n");\
}

#define PRINT_REGISTER_LUT_UNPACK_INT(name, regLutName, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "%s=%d", name, regLutName[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%d", regLutName[lutIndex]); \
    }\
    fprintf(dumpRegFile, "\n");\
}



#define PRINT_PARAM( name, val ) \
    fprintf(dumpRegFile, "%s = %u\n", name, val);

#define PRINT_FLOAT_PARAM( name, val ) \
    fprintf(dumpRegFile, "%s = %f\n", name, val);


#define PRINT_PARAM_LUT(name, param, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "%s=%u", name, param[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%u", param[lutIndex]); \
    }\
    fprintf(dumpRegFile, "\n");\
}


#define PRINT_FLOAT_PARAM_LUT(name, param, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "%s=%f", name, param[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%f", param[lutIndex]); \
    }\
    fprintf(dumpRegFile, "\n");\
}

#define PRINT_PARAM_WITH_PREFIX( prefix, name, val ) \
    fprintf(dumpRegFile, "%s%s = %u\n", prefix, name, val);


#define PRINT_PARAM_LUT_WITH_PREFIX(prefix, name, param, lutSize);\
{\
    uint32_t lutIndex;\
    fprintf(dumpRegFile, "%s%s=%u", prefix, name, param[0]); \
    for (lutIndex = 1; lutIndex < lutSize; lutIndex++)\
    {\
        fprintf(dumpRegFile, ",%u", param[lutIndex]); \
    }\
    fprintf(dumpRegFile, "\n");\
}

///////////////////////////////////////////////////

#define COPY_ARRAY( destArrayName, srcArrayName ) \
    { \
        static_assert((sizeof(srcArrayName) == sizeof(destArrayName)), "Array sizes do not match!"); \
        memcpy(destArrayName, srcArrayName, sizeof(srcArrayName)); \
    }

#define NCLIB_ABS(a)               ( ( a > 0 ) ? (a) : (-a) )
#define NCLIB_MIN(a, b)            ( (a) < (b) ? (a) : (b) )
#define NCLIB_MAX(a, b)            ( (a) > (b) ? (a) : (b) )
#define NCLIB_CEIL_DIV(a,b)        ( (( a ) + ( b ) - 1 ) / ( b ) )
#define NCLIB_LIMIT(retVal, checkedVal, min, max) \
{                                                 \
    if ( checkedVal < min )                       \
        retVal = min;                             \
    else if (checkedVal>max)                      \
        retVal = max;                             \
    else                                          \
        retVal = checkedVal;                      \
}


#endif
