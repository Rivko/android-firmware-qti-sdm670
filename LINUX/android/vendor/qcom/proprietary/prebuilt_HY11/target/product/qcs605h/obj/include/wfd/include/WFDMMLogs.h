#ifndef __WFD_MM_LOGS_H__
#define __WFD_MM_LOGS_H__
/*==============================================================================
*       WFDMMLogs.h
*
*  DESCRIPTION:
*       Provides logging mechanism to WFD MM Modules
*
*
*  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/
/*==============================================================================
                             Edit History
================================================================================
   When            Who           Why
-----------------  ------------  -----------------------------------------------
03/25/2013         SK            InitialDraft
================================================================================
*/

/*==============================================================================
**               Includes and Public Data Declarations
**==============================================================================
*/

/* =============================================================================

                     INCLUDE FILES FOR MODULE

================================================================================
*/
#include "MMDebugMsg.h"
#include <string>
#ifndef WFD_ICS
#include "common_log.h"
#endif

#define WFDMMLOGE(x) MM_MSG_PRIO(MM_GENERAL, MM_PRIO_ERROR, x)
#define WFDMMLOGH(x) MM_MSG_PRIO(MM_GENERAL, MM_PRIO_HIGH, x)
#define WFDMMLOGM(x) MM_MSG_PRIO(MM_GENERAL, MM_PRIO_MEDIUM, x)
#define WFDMMLOGL(x) MM_MSG_PRIO(MM_GENERAL, MM_PRIO_LOW, x)

#define WFDMMLOGE1(x,y) MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_ERROR, x, y)
#define WFDMMLOGH1(x,y) MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_HIGH, x, y)
#define WFDMMLOGM1(x,y) MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_MEDIUM, x, y)
#define WFDMMLOGL1(x,y) MM_MSG_PRIO1(MM_GENERAL, MM_PRIO_LOW, x, y)

#define WFDMMLOGE2(x,y,z) MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_ERROR, x, y, z)
#define WFDMMLOGH2(x,y,z) MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_HIGH, x, y, z)
#define WFDMMLOGM2(x,y,z) MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_MEDIUM, x, y, z)
#define WFDMMLOGL2(x,y,z) MM_MSG_PRIO2(MM_GENERAL, MM_PRIO_LOW, x, y, z)

#define WFDMMLOGE3(w,x,y,z) MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_ERROR,w, x, y, z)
#define WFDMMLOGH3(w,x,y,z) MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_HIGH,w, x, y, z)
#define WFDMMLOGM3(w,x,y,z) MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_MEDIUM,w, x, y, z)
#define WFDMMLOGL3(w,x,y,z) MM_MSG_PRIO3(MM_GENERAL, MM_PRIO_LOW,w, x, y, z)

#define WFDMMLOGE4(v,w,x,y,z) MM_MSG_PRIO4(MM_GENERAL, MM_PRIO_ERROR,v,w, x, y, z)
#define WFDMMLOGH4(v,w,x,y,z) MM_MSG_PRIO4(MM_GENERAL, MM_PRIO_HIGH,v,w, x, y, z)
#define WFDMMLOGM4(v,w,x,y,z) MM_MSG_PRIO4(MM_GENERAL, MM_PRIO_MEDIUM,v,w, x, y, z)
#define WFDMMLOGL4(v,w,x,y,z) MM_MSG_PRIO4(MM_GENERAL, MM_PRIO_LOW,v,w, x, y, z)

#define WFDMMLOGE5(u,v,w,x,y,z) MM_MSG_PRIO5(MM_GENERAL, MM_PRIO_ERROR, u, v, w, x, y, z)
#define WFDMMLOGH5(u,v,w,x,y,z) MM_MSG_PRIO5(MM_GENERAL, MM_PRIO_HIGH, u, v, w, x, y, z)
#define WFDMMLOGM5(u,v,w,x,y,z) MM_MSG_PRIO5(MM_GENERAL, MM_PRIO_MEDIUM, u, v, w, x, y, z)
#define WFDMMLOGL5(u,v,w,x,y,z) MM_MSG_PRIO5(MM_GENERAL, MM_PRIO_LOW, u, v, w,x, y, z)

#define WFDMMLOGE6(u,v,w,x,y,z,a) MM_MSG_PRIO6(MM_GENERAL, MM_PRIO_ERROR, u, v, w, x, y, z, a)
#define WFDMMLOGH6(u,v,w,x,y,z,a) MM_MSG_PRIO6(MM_GENERAL, MM_PRIO_HIGH, u, v, w, x, y, z, a)
#define WFDMMLOGM6(u,v,w,x,y,z,a) MM_MSG_PRIO6(MM_GENERAL, MM_PRIO_MEDIUM, u, v, w, x, y, z, a)
#define WFDMMLOGL6(u,v,w,x,y,z,a) MM_MSG_PRIO6(MM_GENERAL, MM_PRIO_LOW, u, v, w,x, y, z, a)

#define WFDMMLOGE7(u,v,w,x,y,z,a,b) MM_MSG_PRIO7(MM_GENERAL, MM_PRIO_ERROR, u, v, w, x, y, z, a, b)
#define WFDMMLOGH7(u,v,w,x,y,z,a,b) MM_MSG_PRIO7(MM_GENERAL, MM_PRIO_HIGH, u, v, w, x, y, z, a, b)
#define WFDMMLOGM7(u,v,w,x,y,z,a,b) MM_MSG_PRIO7(MM_GENERAL, MM_PRIO_MEDIUM, u, v, w, x, y, z, a, b)
#define WFDMMLOGL7(u,v,w,x,y,z,a,b) MM_MSG_PRIO7(MM_GENERAL, MM_PRIO_LOW, u, v, w,x, y, z, a, b)

#define WFDMMLOGD WFDMMLOGH
#define WFDMMLOGD1 WFDMMLOGH1
#define WFDMMLOGD2 WFDMMLOGH2
#define WFDMMLOGD3 WFDMMLOGH3

#ifdef __cplusplus
class WfdDebug
{
public:
    WfdDebug(const char *pTag, const char *pString)
    {
        mpString = (char*)pString;
        mpTag    = (char*)pTag;
        if(strcmp(pTag, "MM_OSAL"))
        {
            WFDMMLOGD2("%s Entering %s",pTag, mpString);
        }
        else
        {
            WFDMMLOGD1("Entering %s",mpString);
        }

    };
    ~WfdDebug()
    {
        if(mpTag && strcmp(mpTag, "MM_OSAL"))
        {
            WFDMMLOGD2("%s Leaving %s",mpTag, mpString);
        }
        else
        {
            WFDMMLOGD1("Leaving %s", mpString);
        }
    };
private:
    char *mpString;
    char *mpTag;

};
#define WFDDEBUGTRACE WfdDebug debugObj(LOG_TAG, __FUNCTION__);
#else
#define WFDDEBUGTRACE
#endif
#define CHECKVALID(x) if(!isValid){WFDMMLOGE("Not a valid instance");return x;}

#endif /*__WFD_MM_LOGS_H__*/
