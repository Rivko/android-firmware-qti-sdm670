/*===========================================================================
                    Unified Image Encryption (UIE) API

GENERAL DESCRIPTION
  Module for decrypting and verifying images that conform to the
  UIE specification (document 80-NP914-1 Rev A).

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/
#ifndef UIE_H
#define UIE_H

#include <stdbool.h>
#include <comdef.h>

/*===========================================================================
                      DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

#define UIE_LIBRARY_VERSION_MAJOR 1
#define UIE_LIBRARY_VERSION_MINOR 0

/* Error codes */
#define UIE_STATUS_SUCCESS       0
#define UIE_STATUS_FAILURE       1 
#define UIE_STATUS_INVALID_PARAM 2 
#define UIE_STATUS_DECRYPT_FAIL  3 
#define UIE_STATUS_KEYWRAP_FAIL  4 
#define UIE_STATUS_HASH_FAIL     5 
#define UIE_STATUS_KEY_READ_FAIL 6 
#define UIE_STATUS_CONFIG_ERROR  7
#define UIE_STATUS_HW_GET_ROT_INFO_FAILURE 0x1D
#define UIE_ENCRYPTION_HEADER_LEN 208

/* @brief Enum for root encryption key type IDs */
typedef enum
{
  UIE_KEY_SRC_PROVIDED = -1,
  UIE_KEY_SRC_RTL_QC   =  0,
  UIE_KEY_SRC_OTP_OEM  =  1,
  UIE_KEY_SRC_OTP_QC   =  2
} uie_key_src_t;

/* @brief Enum for image type IDs */
typedef enum
{
  UIE_IMAGE_ID_SBL1 = 0,
  UIE_IMAGE_ID_MBA,
  UIE_IMAGE_ID_AMSS,
  UIE_IMAGE_ID_EHOSTDL,
  UIE_IMAGE_ID_DSP,
  UIE_IMAGE_ID_RFU1,
  UIE_IMAGE_ID_RFU2,
  UIE_IMAGE_ID_QSEE,
  UIE_IMAGE_ID_HOSTDL,
  UIE_IMAGE_ID_APPSBL,
  UIE_IMAGE_ID_RPM_FW,
  UIE_IMAGE_ID_SPS,
  UIE_IMAGE_ID_TZ,
  UIE_IMAGE_ID_RIVA,
  UIE_IMAGE_ID_VIDEO,
  UIE_IMAGE_ID_GSS,
  UIE_IMAGE_ID_RFU3,
  UIE_IMAGE_ID_RFU4,
  UIE_IMAGE_ID_WDT,
  UIE_IMAGE_ID_VPU,
  UIE_IMAGE_ID_BCSS,
  UIE_IMAGE_ID_QHEE,
  UIE_IMAGE_ID_PMIC,
  UIE_IMAGE_ID_MAX
} uie_image_id_t;

/**
 * @brief Encapsulated structure serving as a handle for UIE operations.
 */
typedef struct uie_ctx_priv uie_ctx;

/*===========================================================================
                             FUNCTION PROTOTYPES
===========================================================================*/

/**
 * @brief Verifies if an address points to an encrypted image header.
 *
 * @param[in] header_address The address to verify
 *
 * @return - true if the address points to an encrypted image.
 */
bool uie_is_image_encrypted(void *header_address);

/**
 * @brief Obtains the layer 1 key for the key ladder
 *
 * @param[out] key    A buffer to hold the resulting key
 * @param[in]  keylen The byte length of the key
 *
 * @return - 0 on success, non-zero on failure.
 */
int uie_get_l1_key(uint8 *key, uint32 keylen);

/**
 * @brief Sets up UIE operation. For proper memory management,
 *        uie_deinit() should be called at end of life.
 *
 * @param[in] header_address The address of the encrypted image
 * @param[in] key            A buffer containing the L1 key
 * @param[in] keylen         The length of the L1 key
 *
 * @return - a uie_ctx on success or NULL on failure.
 */
uie_ctx* uie_init(void *header_address, uint8 *key, uint32 keylen);

/**
 * @brief Determines if a given image type is permitted to be decrypted.
 *
 * @param[in] ctx      The current context
 * @param[in] image_id The image type to check
 *
 * @return - true if the requested image type is permitted to be decrypted
 */
bool uie_is_image_permitted(uie_ctx *ctx, uie_image_id_t image_id);

/**
 * @brief Determines if a given segment is encrypted.
 *
 * @param[in] ctx        The current context
 * @param[in] segment_id The segment to check
 *
 * @return - true if the requested segment is encrypted
 */
bool uie_is_segment_encrypted(uie_ctx *ctx, uint8 segment_id);

/**
 * @brief Decrypts a segment in-place and verifies its validity.
 *
 * @param[in]  ctx             The current context
 * @param[in]  segment_id      The segment to check
 * @param[in]  segment_address The address of the segment to decrypt
 * @param[in]  segment_length  The byte length of the segment
 * @param[out] digest          The optional SHA256 hashsum; a non-NULL
 *                             input address will result in a digest output
 *
 * @return - 0 on success, non-zero on failure.
 */
int uie_decrypt_segment(uie_ctx *ctx, uint8 segment_id,
                        void *segment_address, uint32 segment_length,
                        uint8 *digest);

/**
 * @brief Tears down UIE operation.
 *
 * @param[in] ctx_ptr The current context
 */
void uie_deinit(uie_ctx *ctx);

#endif /* UIE_H */
