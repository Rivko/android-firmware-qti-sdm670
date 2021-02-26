/*=============================================================================

FILE:       dtslicense.h

SERVICES:   API for authenticating DTS license file and model ID
   

==============================================================================
        Copyright (c) 2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
            QUALCOMM Proprietary and Confidential
==============================================================================*/
#ifndef DTSLICENSE_H
#define DTSLICENSE_H

//////////////////////////////////////////////////////////////////////////////
// DTS license file values

/* 
   Fixed Constants:
      TSIZE    128
      MSIZE    20
      SIGSIZE  256
 
   License File layout:
      --- begin auth section ---
      Bytes 000-003: DSEC        # contains the bytes: 'D','S','E','C'
      Bytes 004-007: version     # Version # (1)
      Bytes 008-00B: TSIZE       # (128)
      Bytes 00C-00F: MSIZE       # (20)
      Bytes 010-08F: token       # permissions
      Bytes 090-0A3: model       # model ID scrambled with SHA-1
      --- end auth section ---
      Bytes 0A4-0A7: SIGSIZE     # (256)
      Bytes 0A8-1A7: sig         # Signature
      Bytes 1A8-1AB: STRSIZE     # size of following JSON string
      Bytes 1AC-385: json        # Informational JSON string

*/
#define DTSL_DSEC             0x43455344     // 'D','S','E','C' as a uint32
#define DTSL_VERSION          1
#define DTSL_TSIZE            128
#define DTSL_MSIZE            20
#define DTSL_SIGSIZE          256

#define DTSL_HEADER_SIZE      16

// Offset to model ID (0x90)
#define DTSL_MID_OFFSET       (DTSL_HEADER_SIZE + DTSL_TSIZE)

// authorization size (0xA4)
#define DTSL_AUTHSIZE         (DTSL_HEADER_SIZE + DTSL_TSIZE + DTSL_MSIZE)

// Offset to signature (0xA8)
#define DTSL_SIG_OFFSET       (DTSL_AUTHSIZE + 4)

// License file must be at least this size (0x1A8)
#define DTSL_MIN_LICENSEFILE_SIZE    (DTSL_SIG_OFFSET + DTSL_SIGSIZE)


/*
   @brief 
      This function is used to authenticate the signature associated with the
      DTS license file.
   @details
      This function will use hardcoded DTS public key to authenticate the signature. 
      It assumes the license binary structure is as defined in "DTS Binary License 
      Specification v1.0"
      (© Postsecondary Electronic Standards Council (PESC) 2005. All Rights Reserved. )

   @dependencies
      None

   @param[in] license_data    : pointer to license data in memory
   @param[in] license_length  : length of the buffer pointed to by license_data

   @return 
      Status of request: 0 - SUCCESS
                         1 - FAILURE

   @sideeffects
      None
*/
int dtslicense_authenticate_license(const void *license_data, int license_length);


/*
   @brief 
      This function will compare the model id that is passed in with the
      obfuscated (SHA-1 hashed) value stored in the license file.
 
   @dependencies
      None

   @param[in] license_data    : pointer to license data in memory
   @param[in] license_length  : length of the buffer pointed to by license_data
   @param[in] model_id        : pointer to model_id in memory
   @param[in] model_id_length : length of the model id

   @return 
      Status of request: 0 - SUCCESS
                         1 - FAILURE

   @sideeffects
      None
*/
int dtslicense_verify_model_id(const void *license_data, int license_length,
                               const void *model_id, int model_id_length);


#endif // DTSLICENSE_H




