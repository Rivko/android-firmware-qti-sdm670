#ifndef _HDCP_SRM_LIB_H_
#define _HDCP_SRM_LIB_H_

/*===========================================================================
   Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/15/17   aw      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <comdef.h>

#ifdef _MSC_VER
#define __attribute__(X)
#endif


#define IN
#define OUT
#define INOUT
#define HDCP_SRM_DEFAULT_SRM_PATH "hdcpsrm/hdcp.srm"

//#define SWAP_DEVICE_ENDIAN //Swap device_id endianess before compare

// The header is identical to the first 5 bytes of the SRM file format.
typedef struct
{
    uint8 id;                   // 0x80 signifies HDCP 1, 0x91 signifies HDCP 2.
    uint16 version;             // Sequentially increasing unique SRM numbers.
    uint8 generation_number;    // Indicates the generation of the SRM.
    void* signature;            // The cryptographic signature for the entire file.
                                // This will be the same as the signature for the
                                // last generation (if there is at least one 
                                // generation).
} hdcp_srm_header_t;

#define HDCP_V1_ID 0x80
#define HDCP_V2_ID 0x91

// This type stores a single device ID.
typedef struct 
{
    uint8 data[5];              // The device ID.
}  hdcp_srm_device_id_t;

// This is the DSA (Digital Signature Algorithm) signature used by the HDCP v1
// SRM file.
typedef struct
{
    uint8 r[20];
    uint8 s[20];
} hdcp_srm_dsa_signature_t;

// This is the RSASSA_PKCS1-v1_5 signature used by the HDCP v2 SRM file.
typedef struct
{
    uint8 data[384];
} hdcp_srm_pkcs1_signature_t;

// The data for a single generation in the SRM file.
typedef struct
{
    uint32 length;              // Specifies the combined length in bytes of 
                                // of the fields in this generation extension
                                // in the SRM file.
    uint16 number_of_devices;   // Specifies the number of device IDs in this 
                                // list.
    hdcp_srm_device_id_t* device_ids; // The device IDs.
    void* signature;            // The cryptographic signature for the entire
                                // SRM up to and including this generation.
                                // If this is a v1 SRM file, this is a DSA
                                // signature. If this is a v2 SRM file, this is
                                // RSASSA_PKCS1-v1_5 signature.
} hdcp_srm_generation_t;

// Common return values
#define HDCP_SRM_SUCCESS 0
#define HDCP_SRM_UNKNOWN_ERROR -1
#define HDCP_SRM_INVALID_PARAMETER -2
#define HDCP_SRM_OUT_OF_MEMORY -3
#define HDCP_SRM_SFS_ERROR -4
#define HDCP_SRM_SIG_AUTH_ERROR -5
#define HDCP_SRM_INVALID_SRM_FILE -6
#define HDCP_SRM_INVALID_DEV_ID -7
#define HDCP_SRM_UNSUPPORTED_CMD -8
#define HDCP_SRM_INSUFFICIENT_SRM_VERSION -9

/*
This function reads the SRM file from SFS. If the path parameter is NULL,
it reads the SRM file from a default path. If the file does not exist, 
the default SRM file is written to the path and returned. The returned
buffer must be freed by the caller.
*/
int hdcp_srm_read_srm(
    IN const char* path,        // The path in SFS of the SRM file. 
                                // This can be NULL.
    OUT uint32* srm_length,     // The length of the SRM file.
    OUT uint8** srm_data        // Buffer containing the SRM file.
);

/*
This function compares the SRM file that has been passed in with the SRM 
file that is currently saved in SFS. If the SRM file that has been passed in
is newer than the one currently in SFS, it writes the new SFS file to SFS.
Return:
*/
int hdcp_srm_update_srm(
    IN const char* path,        // The path in SFS of the SRM file.
                                // This can be NULL.
    IN uint32 length,           // Length of the SRM file.
    IN uint8* srm_data          // Buffer containing the SRM file.
);

/*
This function takes a buffer containing the contents of the SRM file as a 
parameter, parses the SRM file, validates the signature, and returns the 
data contains in the SRM file. Note: Some of the fields in the returned
data points to data in the srm_data parameter. srm_data must not be
freed until all the returned data (the generations parameter) is done being
used. The pointer returned in the generations parameter MUST be freed
by the caller.
Return: 
    HDCP_SRM_INVALID_SRM_FILE
*/
int hdcp_srm_parse_srm(
    IN uint32 length,           // Length of the SRM file.
    IN uint8* srm_data,         // Buffer containing the SRM file.
    OUT hdcp_srm_header_t* header, // On output, the header of the SRM file.
    OUT hdcp_srm_generation_t** generations
                                // The array to which the generation data is written.
                                // This can be NULL if generation count is 0.
);

/*
This function takes the ptr to the header/generations buffer of a SRM File,
and extracts the revoked DeviceIDs / number of devices / srm_version. The pointer
returned in the revoked_list parameter MUST be freed by the caller.
*/
int hdcp_srm_extract_generations(
    IN hdcp_srm_header_t* header,          // On output, the header of the SRM file.
    IN hdcp_srm_generation_t* generations, // The array to which the generation data is written.
    OUT uint16* srm_version,
    OUT uint32* revoked_number,         // Number of DeviceIDs.
    OUT uint8** revoked_list        // Buffer containing the SRM Revoked DeviceID.
);

/* 
This function takes two SRM headers and determines which is newer based on
the version number and the HDCP version.
Return:
    >0: left is newer than right.
    0:  left is the same version as right.
    <0: left is older than right.
*/
int hdcp_srm_compare_version(
    IN hdcp_srm_header_t* left, // An HDCP SRM header.
    IN hdcp_srm_header_t* right // An HDCP SRM header.
);

/*
This function checks the SRM for a list of device IDs.
Return:
    HDCP_SRM_INVALID_DEV_ID: At least one of the device IDs is invalid.
*/
int hdcp_srm_check_device_ids(
    IN uint32 device_id_count,  // Size of the device_ids parameter.
    IN hdcp_srm_device_id_t* device_ids, // The device IDs to look for.
    IN hdcp_srm_header_t* header,   // The header of the SRM file.
    IN hdcp_srm_generation_t* generations
                                // Array of the generations of the SRM file.
                                // Note: the size of the array is the
                                // generation_number member of the header.
);

/*
This function returns a pointer to the default SRM file which is
to be used before the SRM file has been updated. The returned pointer
points to a const array and thus must not be modified or freed.
*/
int hdcp_srm_get_default_srm(
    OUT const uint8** srm_data, 
    OUT uint32* srm_length
);

boolean is_secfuse_blown_device();

#endif /* _HDCP_SRM_LIB_H_ */
