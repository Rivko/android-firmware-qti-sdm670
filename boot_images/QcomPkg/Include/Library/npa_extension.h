/*==============================================================================
@file npa_extension.h

NPA extension API 

An optional convention for defining user user_data field for an
arbitrary number of extensions. If you only have a single piece of user data,
or no user data at all, this extension mechanism may be ignored.

Copyright (c) 2009-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef NPA_EXTENSION_H
#define NPA_EXTENSION_H

#ifdef __cplusplus
extern "C" {
#endif

/* NPA Extension Enumerations */
enum
{
  NPA_EXTENSION_USER_DATA,      /* Arbitrary user data */
  NPA_EXTENSION_REMOTE_RESOURCE,
  NPA_EXTENSION_REMOTE_PROXY_RESOURCE,
  NPA_EXTENSION_RPM_TRANSACTION,
  NPA_EXTENSION_QRS_PROXY_RESOURCE,
  NPA_EXTENSION_FRAMEWORK_END = 0x3fffffff

/* Custom user extension ids should be numbered > NPA_EXTENSION_FRAMEWORK_END.
 * They do not need to be added to this enum.
 */
};

/* Header for an extension */
typedef struct npa_extension_header
{
   void   *next_extension;
   uint32  extension_id;
} npa_extension_header;

/* An extension is an arbitrary data structure that begins with an
 * npa_extension_header.
 */

/** 
  @brief npa_find_extension - Given a pointer to an extension
  structure, walk the list and return the first extension matching
  the given extension id. Returns NULL if no match is found

  It is the responsibility of the caller to only invoke this function
  on a well formed list of extensions.  Behavior is undefined if the
  list is not well formed. The list is not locked during search.
*/
npa_user_data npa_find_extension( npa_extension_header *extension, 
                                  unsigned int          extension_id );

#ifdef __cplusplus
}
#endif

#endif /* NPA_EXTENSION_H */