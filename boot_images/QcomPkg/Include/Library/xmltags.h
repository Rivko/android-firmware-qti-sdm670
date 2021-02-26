#ifndef _XML_TAGS_H_
#define _XML_TAGS_H_
/*=============================================================================
 
  File: xmltags.h
  
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/Library/xmltags.h#1 $$
  
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 
=============================================================================*/ 

#include "com_dtypes.h"


#ifdef __cplusplus
extern "C" {
#endif


#define XML_MAX_STRING_LENGTH  28

#define XML_STRLEN(_str_) ((sizeof(_str_)/sizeof(_str_[0])) - sizeof(_str_[0])) 

#define XML_CREATE_TAG_ENTRY(_name_, _type_, _datafield_) \
    { (_name_), XML_STRLEN(_name_), _type_, (void*)&(_datafield_), sizeof(_datafield_), 0x0, 0x0 }

#define XML_CREATE_TAG_ENTRY_EX(_name_, _type_, _datafield_, _defaultfill_) \
    { (_name_), XML_STRLEN(_name_), _type_, (void*)&(_datafield_), sizeof(_datafield_), XML_TAG_FLAGS_FILL, (_defaultfill_) }

#define XML_CREATE_TAG_NULL \
    { "\0", 0, XML_TAG_DATATYPE_NONE, (void*)NULL, 0 }


typedef enum {
   XML_TAG_DATATYPE_NONE = 0,
   XML_TAG_DATATYPE_STRING,
   XML_TAG_DATATYPE_INTEGER,
   XML_TAG_DATATYPE_FLOAT,
   XML_TAG_DATATYPE_BOOLEAN,
   XML_TAG_DATATYPE_BINARY,
   XML_TAG_DATATYPE_GUID,
   XML_TAG_DATATYPE_INTEGER_LIST,
   XML_TAG_DATATYPE_HEX_LIST,
   XML_TAG_DATATYPE_MAX
} XML_TagDataType;

typedef int8 xmlchar;

/* XML Field Flags */

/* Fill the tag with a default value before processing tags */
#define XML_TAG_FLAGS_FILL        0x01  


/* XML Tag Structure */
#pragma pack(push)
#pragma pack(1)
typedef struct
{
    xmlchar          uXMLTag[XML_MAX_STRING_LENGTH];
    uint16           uXMLTagLength;
    XML_TagDataType  eXMLTagDataType;
    void            *pXMLDataField;
    uint16           uXMLDataFieldLength;
    uint8            uTagFlags;
    uint8            uDefaultFillValue;
} XML_TagDefType;
#pragma pack(pop)



#ifdef __cplusplus
  }
#endif

#endif // _XML_TAGS_H_
