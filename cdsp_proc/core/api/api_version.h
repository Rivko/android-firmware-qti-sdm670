/*=============================================================================

DO NOT MODFIY
THIS FILE IS AUTO-GENETRATED. 

=============================================================================*/

#ifndef API_VERSION_H
#define API_VERSION_H
/*=============================================================================

                 api_version.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   API Version file 
   [Major revision #] – Major # change in api_version.h will be incremented, 
   if user pd must upgrade or non-backwards compatible change made through API definition.  
   This could include, but not limited to, any # of  existing function parameter change, type of function parameter change or return type of function change.  
   [Minor revision #] - Minor # change in api_version.h. 
   Transparent change in published APIs, i.e. user PDs don’t have to be upgraded, 
   and changes remain backwards compatible, in terms of API definition.
   This could include, but not limited to, any bug fix for existing APIs or  newly added APIs. 
   This is the default tag by PW tool. No action to be made by developer. 


EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

=============================================================================*/

#ifndef __ASSEMBLY__
/*=============================================================================
                    CONSTANTS AND DEFINITIONS
=============================================================================*/
typedef struct api_version {
    char subsystem[256]; /* this string contains componant name, major revision # and minor revision # */
    unsigned short api_ver_major;/* store the major revision # for comparsion */
    unsigned short api_ver_minor;/* store the minor revision # for comparison */
} api_version; 
#else
#define COMPONENT_NAME "core.qdsp6.2.0"
#define API_VER_MAJOR_NAME "API_VER_MAJOR"
#define API_VER_MAJOR "1"
#define API_VER_MINOR_NAME "API_VER_MINOR" 
#define API_VER_MINOR "0" 
#endif /* __ASSEMBLY__ */

#endif /* API_VERSION_H */

