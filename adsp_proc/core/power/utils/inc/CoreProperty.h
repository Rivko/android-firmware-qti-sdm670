/*============================================================================
@file CoreProperty.h

This file provides the public API of the abstract
configuration Property functions.

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreProperty.h#1 $
============================================================================*/
#ifndef COREPROPERTY_H
#define COREPROPERTY_H

/**
 * @brief CORE_CONFIG error codes 
 * 
 * These are the possible error codes that are returned by the 
 * Core Config subsystem. 
 */
enum core_property_status
{
  /* signifies success when a count is not returned */
  CORE_PROPERTY_SUCCESS             =      0,

  /* NO ERROR if, for the routines that return a count,
   * count returned is <= to CORE_PROPERTY_NO_ERROR.
   * Not returned but can be used to validate a counting return value.
   */ 
  CORE_PROPERTY_NO_ERROR            =      0x7fffffff,

  /* Can't find the device */
  CORE_PROPERTY_ERROR_DEV_NOT_FOUND =      -6,

  /* Can't find the key */
  CORE_PROPERTY_ERROR_KEY_NOT_FOUND =      -5,

  /* Not enough room for the sequence */
  CORE_PROPERTY_ERROR_INSUF_SIZE    =      -4,

  /* parameter is not valid */
  CORE_PROPERTY_ERROR_ILLEGAL_INPUT =      -3,

  /* Wrong type of data received */
  CORE_PROPERTY_ERROR_TYPE_MISMATCH =      -2,

  /* other unknown error */
  CORE_PROPERTY_ERROR               =      -1
};

#define cp_offsetof(type, member) ((char *) &(((type*)0)->member))

/* name/offset/size macro mapping for config info */
#define CORE_PROPERTY_STRUCTURE_ELEMENT(type, member) \
  {#member, cp_offsetof(type, member), sizeof((((type*)0)->member))}

/**
 * CoreConfigPropertyStructure 
 *  
 * @brief Element to defines the structure element name and
 *        offset in bytes from the begining of the structure.
 *        Used by CoreConfig_GetPropertyStructure() to know what
 *        string to use when looking up config data and then
 *        where to put it.
 */ 
typedef struct
{
  const char *str; /* name of the element used in the struct and DALSYS config */
  char *offset;    /* offset in words of the element */
                   /* Note that some compilers do not like a static ptr to int
                    * conversion and so offsetof() gets a warning if this is not 
                    * a pointer type.
                    * Make this a char* and convert to unsigned int when using it.
                    */
  unsigned int byte_size; /* size of item in number of bytes.  If it is an array,
                           * the size is the array size in bytes
                           */
} CorePropertyStructure;


/**
 * CoreProperty_UpdateStructure
 *
 * @brief Reads the configuration data from an DALSYS Device
 *        Configuration interface and fills in/updates the
 *        structure with any elements found.
 *  
 * Note that the element name in the info_ptr entires matches 
 * the actual C name of the element in the structure minus the 
 * initial variable name and also matches the name used in the 
 * XML file passed to DALSYS. 
 * e.g. if the structures are defined as... 
 *  
 *   struct x_struct
 *   {
 *     int y;
 *   };
 *  
 *   struct a_struct
 *   {
 *     int z;
 *     struct x_struct x;
 *   };
 *  
 *   struct a_struct m;
 *  
 *  then the device name of m could be "/dev/power/m" and the
 *  name of int y in sturct m is "x.y"
 *  
 *  e.g.
 *  CorePropertyStructure m_property_struct[]
 *  = { CORE_PROPERTY_STRUCTURE_ELEMENT(a_struct, z),
 *      CORE_PROPERTY_STRUCTURE_ELEMENT(a_struct, x.y)};
 *  
 *  in the DEVCFG xml file to set m.z to 5 and m.x.y to 6 ...
 *    <driver name="NULL">
 *      <!-- pointer struct m -->
 *      <device id="/dev/power/m">
 *        <props name="z" type=DALPROP_ATTR_TYPE_UINT32>
 *          5
 *        </props>
 *        <props name="x.y" type=DALPROP_ATTR_TYPE_UINT32>
 *           6
 *        </props>
 *      </device>
 *    </driver>
 * 
 *  NOTE: Only UINT32, pointers to structures, and pointers to
 *   arrays(uint32 and bytes) are supported.
 * 
 * @param device_str: Device Config label for the device.
 * @param format_ptr:   pointer to the element name and byte 
 *                      offset of the structure passed in
 *                      config_ptr.  Last element in the list
 *                      must be {NULL, 0}
 * @param dest_ptr: Pointer to structure to be filled in
 *  
 * @return Returns number of elements in *config_ptr that were 
 *         found and updated.  Incremented even if the value did
 *         not change.
 *         Also on critical errors, will return a value from the
 *         enum core_property_status.  No error if the returned
 *         value is <= CORE_PROPERTY_NO_ERROR
 */
int CoreProperty_UpdateStructure(const char *device_str,
                                 CorePropertyStructure *format_ptr,
                                 void *dest_ptr);

/**
 * CoreProperty_GetPtr
 *
 * @brief Searches for a key and returns the matching ptr if the
 *        types match.
 *  
 * @param device_str: Device Config label for the device.
 * @param keystr:     pointer key's name.
 * @param ret_ptr:    passback pointer for the location to be 
 *                    updated. Only updated if the key was found
 *                    and of the correct type. (return value is
 *                    CORE_CONFIG_SUCCESS)
 *  
 * @return CORE_CONFIG_SUCCESS if match, else returns reason for 
 *         failure
 */
int CoreProperty_GetPtr(const char *device_str,
                        const char *keystr,
                        const void **ret_value);

/**
 * CoreProperty_GetUINT32
 *
 * @brief Searches for a key and returns the matching set if the
 *        types match.
 *  
 * @param device_str: Device Config label for the device.
 * @param keystr:     pointer key's name.
 * @param ret_ptr:    passback pointer for the location to be 
 *                    updated. Only updated if the key was found
 *                    and of the correct type. (return value is
 *                    CORE_CONFIG_SUCCESS)
 *  
 * @return CORE_CONFIG_SUCCESS if match, else returns reason for 
 *         failure
 */
int CoreProperty_GetUINT32(const char *device_str,
                           const char *keystr,
                           unsigned int *ret_value);

#endif /* COREPROPERTY_H */
