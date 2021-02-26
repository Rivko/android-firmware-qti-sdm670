#ifndef MFS_SERVICE_01_H
#define MFS_SERVICE_01_H
/**
  @file modem_filesystem_v01.h
  
  @brief This is the public header file which defines the mfs service Data structures.

  This header file defines the types and structures that were defined in 
  mfs. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were 
  defined in the IDL as messages contain mandatory elements, optional 
  elements, a combination of mandatory and optional elements (mandatory 
  always come before optionals in the structure), or nothing (null message)
   
  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to. 
   
  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:
   
  uint32_t test_opaque_len;
  uint8_t test_opaque[16];
   
  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set 
  by the decode routine and should be checked so that the correct number of 
  elements in the array will be accessed. 

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.adsp/2.6/mfs/api/modem_filesystem_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY 
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.2 
   It was generated on: Tue Jul 23 2013 (Spin 1)
   From IDL File: modem_filesystem_v01.idl */

/** @defgroup mfs_qmi_consts Constant values defined in the IDL */
/** @defgroup mfs_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup mfs_qmi_enums Enumerated types used in QMI messages */
/** @defgroup mfs_qmi_messages Structures sent as QMI messages */
/** @defgroup mfs_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup mfs_qmi_accessor Accessor for QMI service object */
/** @defgroup mfs_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup mfs_qmi_version 
    @{ 
  */ 
/** Major Version Number of the IDL used to generate this file */
#define MFS_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define MFS_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define MFS_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define MFS_V01_MAX_MESSAGE_ID 0x0021;
/** 
    @} 
  */


/** @addtogroup mfs_qmi_consts 
    @{ 
  */

/**  Max file path length in bytes. */
#define MFS_MAX_FILE_PATH_V01 1024

/**  Max EFS item-file size in bytes. */
#define MFS_MAX_ITEM_FILE_SIZE_V01 32768
/**
    @}
  */

/** @addtogroup mfs_qmi_enums
    @{
  */
typedef enum {
  MFS_ERRNO_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  MFS_ENOERR_V01 = 0, /**<  Success */
  MFS_EPERM_V01 = 1, /**<  Operation not permitted */
  MFS_ENOENT_V01 = 2, /**<  No such file or directory */
  MFS_EEXIST_V01 = 3, /**<  File exists */
  MFS_EBADF_V01 = 4, /**<  Bad file descriptor */
  MFS_ENOMEM_V01 = 5, /**<  Out of memory */
  MFS_EACCES_V01 = 6, /**<  Permission denied */
  MFS_EBUSY_V01 = 7, /**<  Device or resource is busy */
  MFS_EXDEV_V01 = 8, /**<  Attempt to cross device */
  MFS_ENODEV_V01 = 9, /**<  No such device */
  MFS_ENOTDIR_V01 = 10, /**<  Not a directory */
  MFS_EISDIR_V01 = 11, /**<  Is a directory */
  MFS_EINVAL_V01 = 12, /**<  Invalid argument */
  MFS_EMFILE_V01 = 13, /**<  Too many open files */
  MFS_ETXTBSY_V01 = 14, /**<  File or directory already open */
  MFS_ENOSPC_V01 = 15, /**<  No space left on device */
  MFS_ESPIPE_V01 = 16, /**<  Seek is not permitted */
  MFS_ENAMETOOLONG_V01 = 17, /**<  File name too long */
  MFS_ENOTEMPTY_V01 = 18, /**<  Directory not empty */
  MFS_ELOOP_V01 = 19, /**<  Too many symbolic links encountered */
  MFS_EILSEQ_V01 = 20, /**<  Illegal byte sequence */
  MFS_ESTALE_V01 = 21, /**<  Stale remote file handle */
  MFS_EDQUOT_V01 = 22, /**<  Attempt to write beyond quota */
  MFS_ENOCARD_V01 = 23, /**<  No Media present */
  MFS_EBADFMT_V01 = 24, /**<  Bad Formated Media */
  MFS_ENOTITM_V01 = 25, /**<  Not an EFS2 item file */
  MFS_EROLLBACK_V01 = 26, /**<  Rollback failed while backing orig file */
  MFS_FS_ERANGE_V01 = 27, /**<  Parameter out of range */
  MFS_EEOF_V01 = 28, /**<  End of file. Internal error code */
  MFS_EUNKNOWN_SFAT_V01 = 29, /**<  Untranslated SFAT error code */
  MFS_EUNKNOWN_HFAT_V01 = 30, /**<  Untranslated HFAT error code */
  MFS_ERRNO_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}mfs_errno_type_v01;
/**
    @}
  */

/**  Possible error numbers returned by Modem filesystem service
 Non-posix errnos used within EFS
 open flags to be specified for files */
typedef uint32_t mfs_oflag_type_v01;
#define MFS_O_ACCMODE_V01 ((mfs_oflag_type_v01)0x1) 
#define MFS_O_RDONLY_V01 ((mfs_oflag_type_v01)0x2) 
#define MFS_O_WRONLY_V01 ((mfs_oflag_type_v01)0x4) 
#define MFS_O_RDWR_V01 ((mfs_oflag_type_v01)0x8) 
#define MFS_O_CREAT_V01 ((mfs_oflag_type_v01)0x10) 
#define MFS_O_EXCL_V01 ((mfs_oflag_type_v01)0x20) 
#define MFS_O_NOCTTY_V01 ((mfs_oflag_type_v01)0x40) 
#define MFS_O_TRUNC_V01 ((mfs_oflag_type_v01)0x80) 
#define MFS_O_APPEND_V01 ((mfs_oflag_type_v01)0x100) 
#define MFS_O_NONBLOCK_V01 ((mfs_oflag_type_v01)0x200) 
#define MFS_O_SYNC_V01 ((mfs_oflag_type_v01)0x400) 
#define MFS_FASYNC_V01 ((mfs_oflag_type_v01)0x800) 
#define MFS_O_DIRECT_V01 ((mfs_oflag_type_v01)0x1000) 
#define MFS_O_LARGEFILE_V01 ((mfs_oflag_type_v01)0x2000) 
#define MFS_O_DIRECTORY_V01 ((mfs_oflag_type_v01)0x4000) 
#define MFS_O_NOFOLLOW_V01 ((mfs_oflag_type_v01)0x8000) 
#define MFS_O_ITEMFILE_V01 ((mfs_oflag_type_v01)0x10000) 
#define MFS_O_AUTODIR_V01 ((mfs_oflag_type_v01)0x20000) 
#define MFS_O_SFS_RSVD_V01 ((mfs_oflag_type_v01)0x40000) 
#define MFS_O_MAX_OFLAG_V01 ((mfs_oflag_type_v01)0x80000) 
/** @addtogroup mfs_qmi_messages
    @{
  */
/** Request Message; Stores a value to an EFS2 special item file. */
typedef struct {

  /* Mandatory */
  /*  Path */
  uint32_t path_len;  /**< Must be set to # of elements in path */
  char path[MFS_MAX_FILE_PATH_V01];
  /**<   Path of the file to write. */

  /* Mandatory */
  /*  Data */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[MFS_MAX_ITEM_FILE_SIZE_V01];
  /**<   Buffer containing the data. */

  /* Mandatory */
  /*  Open Flag */
  mfs_oflag_type_v01 oflag;
  /**<   Open flag is a bitwise OR of one of the following: \n
       - MFS_O_CREAT   -- Create the item if it does not exist. \n
       - MFS_O_EXCL    -- Fail if the item already exists. \n
       - MFS_O_AUTODIR -- Autocreate parent directories.
       
       Other flags (MFS_O_APPEND, MFS_O_TRUNC) have no effect.
  */

  /* Mandatory */
  /*  Mode */
  int32_t mode;
  /**<   Sets the initial permission bits of the item file. */
}mfs_put_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mfs_qmi_messages
    @{
  */
/** Response Message; Stores a value to an EFS2 special item file. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Error Number */
  uint8_t efs_err_num_valid;  /**< Must be set to true if efs_err_num is being passed */
  mfs_errno_type_v01 efs_err_num;
  /**<   Provides additional error information in the case of a failure. Values: \n
       - 0 -- MFS_ENOERR -- Success \n
       - 1 -- MFS_EPERM  -- Operation is not permitted \n
       - 2 -- MFS_ENOENT -- No such file or directory \n
       - 3 -- MFS_EEXIST -- File exists \n
       - 4 -- MFS_EBADF  -- Bad file descriptor \n
       - 5 -- MFS_ENOMEM -- Out of memory \n
       - 6 -- MFS_EACCES -- Permission is denied \n
       - 7 -- MFS_EBUSY -- Device or resource is busy \n
       - 8 -- MFS_EXDEV -- Attempt to cross the device \n
       - 9 -- MFS_ENODEV -- No such device \n
       - 10 -- MFS_ENOTDIR -- Not a directory \n
       - 11 -- MFS_EISDIR -- Is a directory \n
       - 12 -- MFS_EINVAL -- Invalid argument \n
       - 13 -- MFS_EMFILE -- Too many open files \n
       - 14 -- MFS_ETXTBSY -- File or directory is already open \n
       - 15 -- MFS_ENOSPC -- No space is left on the device \n
       - 16 -- MFS_ESPIPE -- Seek is not permitted \n
       - 17 -- MFS_ENAMETOOLONG -- Filename is too long \n
       - 18 -- MFS_ENOTEMPTY -- Directory is not empty \n
       - 19 -- MFS_ELOOP -- Too many symbolic links were encountered \n
       - 20 -- MFS_EILSEQ -- Illegal byte sequence \n
       - 21 -- MFS_ESTALE -- Stale remote file handle \n
       - 22 -- MFS_EDQUOT -- Attempt to write beyond the quota \n
       - 23 -- MFS_ENOCARD -- Non-posix error: no media is present \n
       - 24 -- MFS_EBADFMT -- Non-posix error: bad formatted media \n
       - 25 -- MFS_ENOTITM -- Non-posix error: not an EFS2 item file \n
       - 26 -- MFS_EROLLBACK -- Non-posix error: rollback failed while backing 
                                the original file \n
       - 27 -- MFS_FS_ERANGE -- Non-posix error: parameter is out of range \n
       - 28 -- MFS_EEOF -- Non-posix error: end of file (internal error code) \n
       - 29 -- MFS_EUNKNOWN_SFAT -- Non-posix error: untranslated SFAT error code \n
       - 30 -- MFS_EUNKNOWN_HFAT -- Non-posix error: untranslated HFAT error code      
  */
}mfs_put_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mfs_qmi_messages
    @{
  */
/** Request Message; Retrieves a value from an EFS2 special item file. */
typedef struct {

  /* Mandatory */
  /*  Path */
  uint32_t path_len;  /**< Must be set to # of elements in path */
  char path[MFS_MAX_FILE_PATH_V01];
  /**<   Path of the file to read. */

  /* Optional */
  /*  Data Length */
  uint8_t data_length_valid;  /**< Must be set to true if data_length is being passed */
  uint32_t data_length;
  /**<   Size in bytes of the file to be read. */
}mfs_get_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup mfs_qmi_messages
    @{
  */
/** Response Message; Retrieves a value from an EFS2 special item file. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type.*/

  /* Optional */
  /*  Error Number */
  uint8_t efs_err_num_valid;  /**< Must be set to true if efs_err_num is being passed */
  mfs_errno_type_v01 efs_err_num;
  /**<   Provides additional error information in the case of a failure. Values: \n
       - 0 -- MFS_ENOERR -- Success \n
       - 1 -- MFS_EPERM  -- Operation is not permitted \n
       - 2 -- MFS_ENOENT -- No such file or directory \n
       - 3 -- MFS_EEXIST -- File exists \n
       - 4 -- MFS_EBADF  -- Bad file descriptor \n
       - 5 -- MFS_ENOMEM -- Out of memory \n
       - 6 -- MFS_EACCES -- Permission is denied \n
       - 7 -- MFS_EBUSY -- Device or resource is busy \n
       - 8 -- MFS_EXDEV -- Attempt to cross the device \n
       - 9 -- MFS_ENODEV -- No such device \n
       - 10 -- MFS_ENOTDIR -- Not a directory \n
       - 11 -- MFS_EISDIR -- Is a directory \n
       - 12 -- MFS_EINVAL -- Invalid argument \n
       - 13 -- MFS_EMFILE -- Too many open files \n
       - 14 -- MFS_ETXTBSY -- File or directory is already open \n
       - 15 -- MFS_ENOSPC -- No space is left on the device \n
       - 16 -- MFS_ESPIPE -- Seek is not permitted \n
       - 17 -- MFS_ENAMETOOLONG -- Filename is too long \n
       - 18 -- MFS_ENOTEMPTY -- Directory is not empty \n
       - 19 -- MFS_ELOOP -- Too many symbolic links were encountered \n
       - 20 -- MFS_EILSEQ -- Illegal byte sequence \n
       - 21 -- MFS_ESTALE -- Stale remote file handle \n
       - 22 -- MFS_EDQUOT -- Attempt to write beyond the quota \n
       - 23 -- MFS_ENOCARD -- Non-posix error: no media is present \n
       - 24 -- MFS_EBADFMT -- Non-posix error: bad formatted media \n
       - 25 -- MFS_ENOTITM -- Non-posix error: not an EFS2 item file \n
       - 26 -- MFS_EROLLBACK -- Non-posix error: rollback failed while backing 
                                the original file \n
       - 27 -- MFS_FS_ERANGE -- Non-posix error: parameter is out of range \n
       - 28 -- MFS_EEOF -- Non-posix error: end of file (internal error code) \n
       - 29 -- MFS_EUNKNOWN_SFAT -- Non-posix error: untranslated SFAT error code \n
       - 30 -- MFS_EUNKNOWN_HFAT -- Non-posix error: untranslated HFAT error code   
  */

  /* Optional */
  /*  Data */
  uint8_t data_valid;  /**< Must be set to true if data is being passed */
  uint32_t data_len;  /**< Must be set to # of elements in data */
  uint8_t data[MFS_MAX_ITEM_FILE_SIZE_V01];
  /**<   Buffer containing the data. */
}mfs_get_resp_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup mfs_qmi_msg_ids
    @{
  */
#define QMI_MFS_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_MFS_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_MFS_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_MFS_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_MFS_PUT_REQ_V01 0x0020
#define QMI_MFS_PUT_RESP_V01 0x0020
#define QMI_MFS_GET_REQ_V01 0x0021
#define QMI_MFS_GET_RESP_V01 0x0021
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro mfs_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type mfs_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define mfs_get_service_object_v01( ) \
          mfs_get_service_object_internal_v01( \
            MFS_V01_IDL_MAJOR_VERS, MFS_V01_IDL_MINOR_VERS, \
            MFS_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

