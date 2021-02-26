/*-------------------------------------------------------------------
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/
#include "vtest_XmlComdef.h"
#include "vtest_XmlParserHelper.h"

namespace vtest {

static inline char* find_last_of(char *str, const char *lookfor)
{
    char *match = str;
    char *prevmatch = NULL;
    while (match != NULL)
    {
        if (match != str) prevmatch = match;
        match = strstr(match, lookfor);
        if (match)
        {
            match += strlen(lookfor);
        }
    }
    return prevmatch;
}
static inline char* substr(char *dst, const char *src, int start, int len)
{
    SNPRINTF(dst, MAX_STR_LEN, "%.*s", len, &src[start]);
    return dst;
}


void FindTestID(char *filename, char *testID)
{
    char *match;
    match = find_last_of(filename, "/");
    if (!match)
    {
        match = find_last_of(filename, "\\");
    }
    testID[0] = '\0';
    if (match)
    {
        testID = substr(testID, match, 0, strlen(match) - 4);
    }
    else
    {
        testID = substr(testID, filename, 0, strlen(filename) - 4);
    }
}


}
