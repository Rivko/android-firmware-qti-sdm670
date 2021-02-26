/*============================================================================
@file CoreProperty.c
 
This file provides an abstraction layer for reading a
key/value pair from DALSYS Device Config API.
 
Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/os/dal/CorePropertyImpl.c#1 $
============================================================================*/
#include "CoreVerify.h"
#include "CoreProperty.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include "DalDevice.h"
#include "DALPropDef.h"

/* ==================================================================
              EXTERNAL FUNCTION DECLARATIONS
   ================================================================== */              

/* Get a whole structure's fields that exist */
int CoreProperty_UpdateStructure(const char *device_str,
                                 CorePropertyStructure *format_ptr,
                                 void *dest_ptr)
{
  uint32 num_processed = 0;
  uint32 item_processed;
  DALSYSPropertyVar PropVar;
  DALSYS_PROPERTY_HANDLE_DECLARE( hProps);
  char *insert_ptr = (char *)dest_ptr; /* byte indexed */

  if (!dest_ptr)
  { /* nothing to process */
    return(CORE_PROPERTY_ERROR_ILLEGAL_INPUT);
  }

  if (DALSYS_GetDALPropertyHandleStr(device_str, hProps)
      != DAL_SUCCESS)
  { /* No types are defined */
    return(CORE_PROPERTY_ERROR_DEV_NOT_FOUND);
  }

  while (format_ptr->str)
  { /* step through the info and populate any hits */
    item_processed = 0;
    if (DALSYS_GetPropertyValue(hProps, format_ptr->str, 0, &PropVar)
        == DAL_SUCCESS)
    { /* got an updated value.  force it in as a 32 bit value */
      switch (PropVar.dwType)
      {
        case DALPROP_ATTR_TYPE_UINT32 :
        { /* create pointer for a uint32 write */
          uint32 *valptr = (uint32 *)(insert_ptr + (unsigned int) format_ptr->offset);
          *valptr = PropVar.Val.dwVal;
          item_processed = 1;
        }
        break;

        case DALPROP_ATTR_TYPE_BYTE_SEQ_PTR :
        case DALPROP_ATTR_TYPE_STRING_PTR :
        case DALPROP_ATTR_TYPE_STRUCT_PTR :
        case DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:
        {
          void **valptr = (void *)(insert_ptr + (unsigned int)format_ptr->offset);
          *valptr = PropVar.Val.pdwVal;
          item_processed = 1;
        }
        break;


        default:
          CORE_VERIFY(0); /* Unknown format */
          break;
      }
      num_processed += item_processed;
    }
    ++format_ptr;
  }

  return(num_processed);
}

/* Get a structure pointer */
int CoreProperty_GetPtr(const char *device_str,
                              const char *keystr,
                              const void **ret_value)
{
  DALSYSPropertyVar PropVar;
  DALSYS_PROPERTY_HANDLE_DECLARE( hProps);

  if (!device_str
      || DALSYS_GetDALPropertyHandleStr(device_str, hProps)
      != DAL_SUCCESS)
  { /* invalid string or property does not exist */
      return(CORE_PROPERTY_ERROR_DEV_NOT_FOUND);
  }

  if (DALSYS_GetPropertyValue(hProps, keystr, 0, &PropVar)
        == DAL_SUCCESS)
  {

    switch (PropVar.dwType)
    {
      case DALPROP_ATTR_TYPE_BYTE_SEQ_PTR :
      case DALPROP_ATTR_TYPE_STRING_PTR :
      case DALPROP_ATTR_TYPE_STRUCT_PTR :
      case DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:
      { /* right type */
        *ret_value = PropVar.Val.pdwVal;
        return (CORE_PROPERTY_SUCCESS);
      }

      default:
      /* else attribute type is wrong */
        return(CORE_PROPERTY_ERROR_TYPE_MISMATCH);
    }
  }

  /* Not found */
  return(CORE_PROPERTY_ERROR_KEY_NOT_FOUND);
}

/* Get a structure pointer */
int CoreProperty_GetUINT32(const char *device_str,
                           const char *keystr,
                           unsigned int *ret_value)
{
  DALSYSPropertyVar PropVar;
  DALSYS_PROPERTY_HANDLE_DECLARE( hProps);

  if (!device_str
      || DALSYS_GetDALPropertyHandleStr(device_str, hProps)
      != DAL_SUCCESS)
  { /* invalid string or property does not exist */
      return(CORE_PROPERTY_ERROR_DEV_NOT_FOUND);
  }

  if (DALSYS_GetPropertyValue(hProps, keystr, 0, &PropVar)
        == DAL_SUCCESS)
  {
    if (DALPROP_ATTR_TYPE_UINT32 == PropVar.dwType)
    { /* it is the right type */
      *ret_value = PropVar.Val.dwVal;
      return (CORE_PROPERTY_SUCCESS);
    }
  }

  /* Not found */
  return(CORE_PROPERTY_ERROR_KEY_NOT_FOUND);
}
