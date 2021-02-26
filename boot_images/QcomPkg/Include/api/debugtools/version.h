#ifndef _VERSION_H
#define _VERSION_H

/*===========================================================================

GENERAL DESCRIPTION
  This file contains declaration of functions and other defines used for
  image version reporting feature.

Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

#define VERSION_DEPRECATED_ATTRIBUTE  __attribute__((deprecated))

/////////////////////////////////////////////////////////////////////
// Exported Type Declarations
/////////////////////////////////////////////////////////////////////

typedef const char* VERSION_NAME;                                                // Name
typedef void* VERSION_HANDLE;                                                    // Opaque Handle
typedef enum { VERSION_FALSE, VERSION_TRUE } VERSION_BOOL;                       // Query Return

typedef void* VERSION_BUF;
typedef unsigned long VERSION_BUFLEN;

typedef struct
{
   unsigned short major;
   unsigned short minor;
   unsigned long build;
   unsigned long variant;

} version_rev_t;

#define VERSION_BUF_MINSIZ    sizeof(version_rev_t)   // bytes
#define VERSION_BUF_MAXSIZ    24                      // bytes

typedef union
{
   version_rev_t revision;

   unsigned char revision_uc[VERSION_BUF_MAXSIZ / sizeof(unsigned char)];

   unsigned short revision_us[VERSION_BUF_MAXSIZ / sizeof(unsigned short)];

   unsigned long revision_ul[VERSION_BUF_MAXSIZ / sizeof(unsigned long)];

} VERSION_T, *VERSION_P;

#define VERSION_T_SIZ         sizeof(version_t)
#define VERSION_NULL          ((void*)0)

// Convenience Macros

#define VERSION_DECLARE(x, major, minor) const version_t version_##x = { .revision = { major, minor, 0, 0 } };
#define VERSION_REGISTER(x) version_register_name(#x, &version_##x, VERSION_BUF_MINSIZ)

/////////////////////////////////////////////////////////////////////
// Exported Function Prototypes
/////////////////////////////////////////////////////////////////////

/*===========================================================================

FUNCTION init/term

DESCRIPTION
initialize all internal data structures

DEPENDENCIES
none

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/

void version_init(void);
void version_term(void);

/*===========================================================================

FUNCTION map

DESCRIPTION
map name to handle/handle to name

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/

VERSION_HANDLE version_map_name(VERSION_NAME name);
VERSION_NAME version_map_handle(VERSION_HANDLE handle);

/*===========================================================================

FUNCTION create

DESCRIPTION
create an entry

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/

VERSION_HANDLE version_create_name(VERSION_NAME name);

/*===========================================================================

FUNCTION search

DESCRIPTION
searches for a presense of name

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/

VERSION_BOOL version_search_name(VERSION_NAME name);

/*===========================================================================

FUNCTION register

DESCRIPTION
caller registered

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
caller provides the signal value for the hlos implementation; this
allows the caller to manage what signal is utilized for notification

===========================================================================*/

VERSION_HANDLE version_register_name(VERSION_NAME name, VERSION_P ver);
VERSION_HANDLE version_register_name_ex(VERSION_NAME name, VERSION_BUF buf, VERSION_BUFLEN len);

#if defined(__cplusplus)
}
#endif

#endif
