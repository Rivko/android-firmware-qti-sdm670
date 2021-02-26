/*===========================================================================
                Unified Image Encryption (UIE) Implementation

GENERAL DESCRIPTION
  Module for decrypting images that conform to the UIE specification
  (document 80-NP914-1 Rev A).

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/

//#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UIE.h"
#include "UIEPriv.h"
#include "UIEEnv.h"

/*===========================================================================
                      DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

#define BREAKIF(cond) if (cond) break

static uie_version_t version =
{
  .major = UIE_LIBRARY_VERSION_MAJOR,
  .minor = UIE_LIBRARY_VERSION_MINOR
};

/*===========================================================================
                         INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Note: internal static functions assume that all inputs are sanitized.
 * The burden is on the public caller functions to perform null checks,
 * range checks, etc.
 */

/**
 * Determines if a buffer is comprised of all zero bytes.
 */
static bool is_zero(uint8 *buffer, uint32 length)
{
  bool retval = true;
  int i;
  do {
    for (i = 0; i < length; i++)
    {
      if (buffer[i] != 0)
      {
        retval = false;
        break;
      }
    }
  } while(0);
  return retval;
}

/**
 * Calculates the hamming weight of a number.
 */
static uint8 hamming_weight(uint32 x)
{
  uint8 hw;
  for (hw = 0; x; x >>= 1)
  {
    hw += x & 1;
  }
  return hw;
}

/**
 * Allocates and initializes a new uie_ctx.
 */
static uie_ctx* create_ctx(void *header_address)
{
  uie_ctx* ctx = (uie_ctx*)UIE_MALLOC(sizeof(struct uie_ctx_priv));
  do {
    BREAKIF(ctx == NULL);
    UIE_MEMSCPY(&ctx->library_version, sizeof(ctx->library_version),
                &version, sizeof(uie_version_t));
    memset(&ctx->l3_key_eps1, 0, sizeof(ctx->l3_key_eps1));
    memset(&ctx->l3_key_eps2, 0, sizeof(ctx->l3_key_eps2));
    UIE_MEMSCPY(&ctx->hdr, sizeof(ctx->hdr),
                header_address, sizeof(uie_header_t));
  } while(0);
  return ctx;
}

/**
 * Validates a uie_enc_param_info_hdr_t struct.
 */
static bool is_valid_enc_param_info_hdr(uie_enc_param_info_hdr_t *info_hdr)
{
  bool retval = false;
  do {
    BREAKIF(info_hdr->magic_number != UIE_ENC_PARAM_INFO_HDR_MAGIC_NUMBER);
    BREAKIF(info_hdr->num_enc_param_sec == 0 ||
            info_hdr->num_enc_param_sec > 2);
    BREAKIF(!is_zero(info_hdr->_reserved, sizeof(info_hdr->_reserved)));
    BREAKIF(info_hdr->eps1_offset == 0);
    BREAKIF(memcmp(&info_hdr->eps1_ver, &version, sizeof(version)) != 0);
    if (info_hdr->num_enc_param_sec == 2)
    {
      BREAKIF(info_hdr->eps2_offset == 0);
      BREAKIF(memcmp(&info_hdr->eps2_ver, &version, sizeof(version)) != 0);
    }
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a uie_ccm_b0_t struct.
 */
static bool is_valid_b0(uie_ccm_b0_t *b0)
{
  bool retval = false;
  do {
    BREAKIF(b0->flags != UIE_CCM_B0_FLAGS);
    BREAKIF(b0->payload_len[0] != UIE_CCM_PAYLOAD_LEN_BYTE_0);
    BREAKIF(b0->payload_len[1] != UIE_CCM_PAYLOAD_LEN_BYTE_1);
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a uie_enc_param_sec_hdr_t struct.
 */
static bool is_valid_enc_param_sec_hdr(uie_enc_param_sec_hdr_t *hdr)
{
  bool retval = false;
  do {
    BREAKIF(hdr->magic_number != UIE_ENC_PARAM_SEC_HDR_MAGIC_NUMBER);
    BREAKIF(hdr->enc_param_sec_size != UIE_ENC_PARAM_SEC_SIZE);
    BREAKIF(hdr->key_ladder_algo != UIE_ENC_KEY_LADDER_ALGO_AES);
    BREAKIF(hdr->key_ladder_mode != UIE_ENC_KEY_LADDER_MODE_CCM);
    BREAKIF(hdr->_reserved != 0);
    BREAKIF(hdr->root_key_type != UIE_ROOT_KEY_TYPE_RLT_QC &&
            hdr->root_key_type != UIE_ROOT_KEY_TYPE_OTP_OEM &&
            hdr->root_key_type != UIE_ROOT_KEY_TYPE_OTP_QC);
    BREAKIF(hdr->root_key_info_id != 0);
    BREAKIF(hdr->_reserved2 != 0);
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a uie_l2_adata_t struct.
 */
static bool is_valid_l2_adata(uie_l2_adata_t *adata)
{
  bool retval = false;
  do {
    BREAKIF(adata->adata_len[0] != UIE_L2_ADATA_LEN_BYTE_0);
    BREAKIF(adata->adata_len[1] != UIE_L2_ADATA_LEN_BYTE_1);
    BREAKIF(!is_zero(adata->_reserved, sizeof(adata->_reserved)));
    BREAKIF(memcmp(&adata->ver, &version, sizeof(version)) != 0);
    BREAKIF(adata->key_ladder_len != UIE_KEY_LADDER_LEN); 
    BREAKIF(adata->_reserved2 != 0);
    BREAKIF((adata->image_id_bitmap & UIE_RESERVED_IMAGE_ID_BITMAP_MASK) != 0);
    BREAKIF(!is_zero(adata->_reserved3, sizeof(adata->_reserved3)));
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a uie_wrapped_l2_t struct.
 */
static bool is_valid_wrapped_l2(uie_wrapped_l2_t *wrapped_l2)
{
  bool retval = false;
  do {
    BREAKIF(!is_valid_b0(&wrapped_l2->b0));
    BREAKIF(!is_valid_l2_adata(&wrapped_l2->adata));
    retval = true;
  } while(0);
  return retval;
}

/**
 * Counts the number of encrypted segments specified in the
 * segment bitmap of a uie_l3_adata_t struct.
 */
static uint8 bitmap_count(uie_l3_adata_t *adata)
{
  int i;
  uint8 num_seg = 0;
  for (i = 0; i < sizeof(adata->segment_bitmap)/sizeof(uint32); i++)
  {
    num_seg += hamming_weight(adata->segment_bitmap[i]);
  }
  return num_seg;
}

/**
 * Validates a uie_l3_adata_t struct.
 */
static bool is_valid_l3_adata(uie_l3_adata_t *adata)
{
  bool retval = false;
  do {
    BREAKIF(adata->adata_len[0] != UIE_L3_ADATA_LEN_BYTE_0);
    BREAKIF(adata->adata_len[1] != UIE_L3_ADATA_LEN_BYTE_1);
    BREAKIF(!is_zero(adata->_reserved, sizeof(adata->_reserved)));
    BREAKIF(adata->image_enc_algo != UIE_IMAGE_ENC_ALGO_AES);
    BREAKIF(adata->image_enc_mode != UIE_IMAGE_ENC_MODE_CBC_CS2);
    BREAKIF(!is_zero(adata->_reserved2, sizeof(adata->_reserved2)));
    BREAKIF(adata->all_seg_encrypted != 0 &&
            adata->all_seg_encrypted != 1);
    if (adata->all_seg_encrypted == 0)
    {
      BREAKIF(adata->num_seg != bitmap_count(adata));
    }
    BREAKIF(adata->_reserved3 != 0);
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a uie_wrapped_l3_t struct.
 */
static bool is_valid_wrapped_l3(uie_wrapped_l3_t *wrapped_l3)
{
  bool retval = false;
  do {
    BREAKIF(!is_valid_b0(&wrapped_l3->b0));
    BREAKIF(!is_valid_l3_adata(&wrapped_l3->adata));
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a uie_enc_param_sec_t struct.
 */
static bool is_valid_enc_param_sec(uie_enc_param_sec_t *eps)
{
  bool retval = false;
  do {
    BREAKIF(!is_valid_enc_param_sec_hdr(&eps->hdr));
    BREAKIF(!is_valid_wrapped_l2(&eps->wrapped_l2));
    BREAKIF(!is_valid_wrapped_l3(&eps->wrapped_l3));
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a uie_header_t struct.
 */
static bool is_valid_header(uie_header_t *hdr)
{
  bool retval = false;
  do {
    BREAKIF(!is_valid_enc_param_info_hdr(&hdr->info_hdr));
    BREAKIF(!is_valid_enc_param_sec(&hdr->eps1));
    BREAKIF(hdr->info_hdr.num_enc_param_sec == 2 &&
            !is_valid_enc_param_sec(&hdr->eps2));
    retval = true;
  } while(0);
  return retval;
}

/**
 * Validates a key and length passed into uie_init.
 */
static bool is_valid_key(uint8 *key, uint32 keylen)
{
  bool retval = false;
  do {
    BREAKIF(keylen != UIE_KEY_LEN);
    BREAKIF(key == NULL);
    retval = true;
  } while(0);
  return retval;
}

/**
 * Performs the UIE key unwrap procedure.
 */
static int unwrap_keys(uie_ctx *ctx, uie_eps_t eps, uint8 *l1_key)
{
  uint32 retval = UIE_STATUS_KEYWRAP_FAIL;
  uie_key_src_t key_src = UIE_KEY_SRC_PROVIDED;
  uie_wrapped_l2_t *l2 = NULL;
  uie_wrapped_l3_t *l3 = NULL;
  uint8 *unwrapped_l3_key = NULL;

  /* The local L2 key buffer needs to be volatile so that
   * it may be wiped at the end of the function scope
   * without the wipe being optimized out
   */
  volatile uint8 unwrapped_l2_key[UIE_KEY_LEN];

  if (eps == EPS1)
  {
    key_src = ctx->hdr.eps1.hdr.root_key_type;
    l2 = &ctx->hdr.eps1.wrapped_l2;
    l3 = &ctx->hdr.eps1.wrapped_l3;
    unwrapped_l3_key = ctx->l3_key_eps1;
  }
  else if (eps == EPS2)
  {
    key_src = ctx->hdr.eps2.hdr.root_key_type;
    l2 = &ctx->hdr.eps2.wrapped_l2;
    l3 = &ctx->hdr.eps2.wrapped_l3;
    unwrapped_l3_key = ctx->l3_key_eps2;
  }

  do {
    // Unwrap L2 Key
    retval = uie_decrypt_ccm(
            UIE_KEY_SRC_PROVIDED, 
            l1_key,               UIE_KEY_LEN,
            l2->b0.nonce,         UIE_ENC_KEY_LADDER_CCM_n,
            l2->encrypted_l2_key, UIE_ENC_KEY_LADDER_CCM_Q,
            (uint8*)&l2->adata,   sizeof(uie_l2_adata_t),
            l2->mac,              UIE_ENC_KEY_LADDER_CCM_t,
            (uint8*)unwrapped_l2_key, UIE_KEY_LEN);
    BREAKIF(retval != 0);

    // Unwrap L3 Key
    retval = uie_decrypt_ccm(
            UIE_KEY_SRC_PROVIDED,
            (uint8*)unwrapped_l2_key, UIE_KEY_LEN,
            l3->b0.nonce,         UIE_ENC_KEY_LADDER_CCM_n,
            l3->encrypted_l3_key, UIE_ENC_KEY_LADDER_CCM_Q,
            (uint8*)&l3->adata,   sizeof(uie_l3_adata_t),
            l3->mac,              UIE_ENC_KEY_LADDER_CCM_t,
            unwrapped_l3_key,     UIE_KEY_LEN);
    BREAKIF(retval != 0);
    
    retval = UIE_STATUS_SUCCESS;
  } while(0);

  // Wipe the unwrapped L2 key
  memset((void*)unwrapped_l2_key, 0xff, UIE_KEY_LEN);

  return retval;
}

/**
 * Determines the IV for a segment given its segment ID.
 */
static bool derive_iv(uie_ctx *ctx, uie_eps_t eps, uint8 segment_id, uint8 *iv)
{
  bool retval = false;

  // Treat the segment ID as a big endian uint32
  const uint32 msg_len = UIE_BASE_IV_LEN + sizeof(uint32);

  uint8 msg[msg_len];
  uint8 digest[UIE_SHA256_DIGEST_LEN];
  uint8 *base_iv = NULL;

  if (eps == EPS1)
  {
    base_iv = ctx->hdr.eps1.base_iv;
  }
  else if (eps == EPS2)
  {
    base_iv = ctx->hdr.eps2.base_iv;
  }

  do {
    UIE_MEMSCPY(msg, msg_len, base_iv, UIE_BASE_IV_LEN);
    msg[msg_len-4] = 0;
    msg[msg_len-3] = 0;
    msg[msg_len-2] = 0;
    msg[msg_len-1] = segment_id;
    BREAKIF(uie_hash_sha256(msg, msg_len, digest) != UIE_STATUS_SUCCESS);

    // Use only the UIE_DERIVED_IV_LEN least significant bytes
    UIE_MEMSCPY(iv, UIE_DERIVED_IV_LEN,
                digest + UIE_DERIVED_IV_OFFSET, UIE_DERIVED_IV_LEN);

    retval = true;
  } while(0);

  return retval;
}

/**
 * Decrypts and verifies a segment relative to a specified EPS.
 */
static int decrypt_segment(uie_ctx *ctx, uie_eps_t eps, uint8 segment_id,
                           void *segment_address, uint32 segment_length,
                           uint8 *digest)
{
  int retval = UIE_STATUS_SUCCESS;
  uint8 iv[UIE_DERIVED_IV_LEN];
  uint8 *unwrapped_l3_key = NULL;

  if (eps == EPS1)
  {
    unwrapped_l3_key = ctx->l3_key_eps1;
  }
  else if (eps == EPS2)
  {
    unwrapped_l3_key = ctx->l3_key_eps2;
  }

  do {
    if (!derive_iv(ctx, eps, segment_id, iv))
    {
      retval = UIE_STATUS_HASH_FAIL;
      break;
    }

    if (uie_decrypt_cts(unwrapped_l3_key, UIE_KEY_LEN,
                        iv,               UIE_DERIVED_IV_LEN,
                        segment_address,  segment_length,
                        segment_address,  segment_length,
                        digest) != UIE_STATUS_SUCCESS)
    {
      retval = UIE_STATUS_DECRYPT_FAIL;
      break;
    }
  } while(0);
  return retval;
}

/**
 * Scrubs the memory of a uie_ctx struct.
 */
static void scribble_ctx(uie_ctx *ctx)
{
  memset(ctx, 0xff, sizeof(struct uie_ctx_priv));
}

/**
 * Determines if EPS2 is defined.
 */
static bool eps2_is_defined(uie_ctx *ctx)
{
  return (ctx->hdr.info_hdr.num_enc_param_sec == 2);
}

/**
 * Determines if segment is encrypted relative to a specified EPS adata.
 */
static bool is_segment_encrypted(uie_l3_adata_t *adata, uint8 segment_id)
{
  bool retval = true;
  do {
    BREAKIF(adata->all_seg_encrypted);
    const int index = segment_id / UINT32_BITS;
    const uint32 segment = adata->segment_bitmap[index];
    const int shift_right_dist = segment_id % UINT32_BITS;
    retval = (segment >> shift_right_dist) & 0x1;
  } while(0);
  return retval;
}

/*===========================================================================
                         PUBLIC FUNCTION DEFINITIONS
===========================================================================*/
bool uie_is_image_encrypted(void *header_address)
{
  bool retval = false;
  do {
    BREAKIF(!uie_is_image_encryption_fuse_enabled());
    BREAKIF(header_address == NULL);
    const uie_header_t* hdr = (uie_header_t*)header_address;
    retval = hdr->info_hdr.magic_number == UIE_ENC_PARAM_INFO_HDR_MAGIC_NUMBER;
  } while(0);
  return retval;
}

int uie_get_l1_key(uint8 *key, uint32 keylen)
{
  return uie_get_l1_fuse_key(UIE_KEY_SRC_OTP_OEM, key, keylen);
}

uie_ctx* uie_init(void *header_address, uint8 *key, uint32 keylen)
{
  bool success = false;
  uie_ctx* ctx = NULL;
  do {
    BREAKIF(header_address == NULL);

    BREAKIF(!is_valid_key(key, keylen));

    ctx = create_ctx(header_address);
    BREAKIF(ctx == NULL);

    BREAKIF(!is_valid_header(&ctx->hdr));
    BREAKIF(unwrap_keys(ctx, EPS1, key) != 0);
    BREAKIF(eps2_is_defined(ctx) && unwrap_keys(ctx, EPS2, key) != 0);

    UIE_LOG_MSG("Begin UIE logging\n");
    success = true;
  } while(0);

  if (!success)
  {
    uie_deinit(ctx);
    ctx = NULL;
  }

  return ctx;
}

bool uie_is_image_permitted(uie_ctx *ctx, uie_image_id_t image_id)
{
  bool retval = false;
  do {
    BREAKIF(ctx == NULL || image_id > UIE_IMAGE_ID_MAX  ); //image_id < 0
    const uint32 selector = 0x1 << image_id;
    // An image is permitted if it is contained in all image bitmaps
    retval =
      (ctx->hdr.eps1.wrapped_l2.adata.image_id_bitmap & selector) != 0;
    if (eps2_is_defined(ctx))
    {
      retval &=
        (ctx->hdr.eps2.wrapped_l2.adata.image_id_bitmap & selector) != 0;
    }
  } while(0);
  return retval;
}

bool uie_is_segment_encrypted(uie_ctx *ctx, uint8 segment_id)
{
  bool retval = false;
  do {
    BREAKIF(ctx == NULL || segment_id > UIE_MAX_SEGMENT_ID);
    // A segment is encrypted if any image's segment bitmap includes it
    retval = is_segment_encrypted(&ctx->hdr.eps1.wrapped_l3.adata,
                                  segment_id);
    BREAKIF(retval);

    if (eps2_is_defined(ctx))
    {
      retval = is_segment_encrypted(&ctx->hdr.eps2.wrapped_l3.adata,
                                    segment_id);
      BREAKIF(retval);
    }
  } while(0);
  return retval;
}

int uie_decrypt_segment(uie_ctx *ctx, uint8 segment_id,
                        void *segment_address, uint32 segment_length,
                        uint8 *digest)
{
  int retval = UIE_STATUS_INVALID_PARAM;
  do {
    BREAKIF(ctx == NULL || segment_id > UIE_MAX_SEGMENT_ID ||
            segment_address == NULL || segment_length == 0);
    BREAKIF(!uie_is_segment_encrypted(ctx, segment_id));

    retval = UIE_STATUS_SUCCESS;
    bool eps2_encrypted = eps2_is_defined(ctx) &&
      is_segment_encrypted(&ctx->hdr.eps2.wrapped_l3.adata, segment_id);
    bool eps1_encrypted = 
      is_segment_encrypted(&ctx->hdr.eps1.wrapped_l3.adata, segment_id);

    uint8 hashsum[UIE_SHA256_DIGEST_LEN];
    memset(hashsum, 0, sizeof(hashsum));
    uint8 *eps2_hashsum = NULL;
    uint8 *eps1_hashsum = hashsum;

    // Hash relative to EPS2 params if the segment is not EPS1 encrypted
    if (eps2_encrypted && !eps1_encrypted)
    {
      eps2_hashsum = hashsum;
    }

    // Decrypt EPS2 in-place if needed
    if(eps2_encrypted)
    {
      retval = decrypt_segment(ctx, EPS2, segment_id,
                               segment_address, segment_length, eps2_hashsum);
      BREAKIF(retval != UIE_STATUS_SUCCESS);
    }

    // Decrypt EPS1 in-place if needed
    if(eps1_encrypted)
    {
      retval = decrypt_segment(ctx, EPS1, segment_id,
                               segment_address, segment_length, eps1_hashsum);
      BREAKIF(retval != UIE_STATUS_SUCCESS);
    }

    // Output the hash digest if requested
    if (digest != NULL)
    {
      UIE_MEMSCPY(digest, UIE_SHA256_DIGEST_LEN, hashsum, sizeof(hashsum));
    }
  } while(0);
  return retval;
}

void uie_deinit(uie_ctx *ctx)
{
  do {
    BREAKIF(ctx == NULL);
    UIE_LOG_MSG("End UIE logging\n");
    scribble_ctx(ctx);
    UIE_FREE(ctx);
  } while(0);
}
