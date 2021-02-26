#ifndef SECBOOT_TYPES_H
#define SECBOOT_TYPES_H

/** @file SecBoot_types.h

  This header file contains declarations and definitions for the 
  APPSBL interface to the software that authenticates the HLOS.

  Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
**/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$

when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                   Defines from secboot.h --- start
===========================================================================*/
#define SECBOOT_COMMON_NAME_SIZE     128   /* 128 character name */
#define SECBOOT_SIGNATURE_SIZE       20    /* SHA-1 digest size  */

#define SECBOOT_RSA_EXPONENT         3
#define SECBOOT_VERIFY_ROOT_CERT     0     /* should be verify the root certificate ? */

/*===========================================================================
                   Defines from secboot_i.h --- end
===========================================================================*/


/*===========================================================================
                   Defines from secboot_i.h --- start
===========================================================================*/
/* PBL ROOT KEY size in bits, used by RSA */
#define MAX_KEY_SIZE 4096
#define MIN_KEY_SIZE 2048
#define MAX_PUB_EXP_KEY_SIZE 32 /* in bits, but we expect only exp=3 (2 bits) or worst 65537 (17 bits)
                                    ** the generic case would be = PBL_MAX_KEY_SIZE but for memory reasons
                                    ** we stick to a small size to save memory whereever possible */

#define SECX509_MAX_VRFY_SIG_LEN  (MAX_KEY_SIZE/8)

/*===========================================================================
                   Defines from secboot_i.h --- end
===========================================================================*/

/*===========================================================================
                   Defines from secboot_hash.h --- start
===========================================================================*/
#define SHA1_HASH_LEN 20
#define SHA256_HASH_LEN 32
#define GENERIC_HASH_LEN SHA256_HASH_LEN
#define MAX_OEM_ROOT_CERTS 4

/*---------------------------------------------------------------------------
  Software Type identifiying image being authenticated. These values
  correspond to the code signing tools (CSMS) Software ID field which has
  lower 32 bits for Software type and upper 32 bits for Software version.
---------------------------------------------------------------------------*/
typedef enum
{
  SECBOOT_SBL_SW_TYPE                = 0,
  SECBOOT_SBL1_SW_TYPE               = 0,
  SECBOOT_AMSS_SW_TYPE               = 1, 
  SECBOOT_DMSS_SW_TYPE               = 1,
  SECBOOT_AMSS_HASH_TABLE_SW_TYPE    = 2,
  SECBOOT_FLASH_PRG_SW_TYPE          = 3,
  SECBOOT_EHOSTDL_SW_TYPE             = 3,
  SECBOOT_DSP_HASH_TABLE_SW_TYPE     = 4,
  SECBOOT_SBL2_SW_TYPE               = 5,
  SECBOOT_SBL3_SW_TYPE               = 6,
  SECBOOT_TZ_KERNEL_SW_TYPE          = 7,
  SECBOOT_HOSTDL_SW_TYPE             = 8,
  SECBOOT_APPSBL_SW_TYPE             = 9,
  SECBOOT_RPM_FW_SW_TYPE             = 10,
  SECBOOT_SPS_HASH_TABLE_TYPE        = 11,
  SECBOOT_MAX_SW_TYPE                = 0x7FFFFFFF /* force to 32 bits*/
} sw_type;

typedef enum 
{
  UNDEFINED_HASH_CODE = 0, 
  SHA1, 
  SHA256,
  UNSUPPORTED_HASH_CODE,
  HASH_RESERVED = 0x7fffffff /* force to 32 bits */
} hash_alg_t;


/*===========================================================================
                   Defines from secboot_hash.h --- end
===========================================================================*/


/*===========================================================================
                   Defines from secboot_asn1.h --- start
===========================================================================*/
/* ASN.1 data holder */
typedef struct secasn1_data_type
{
  const uint8 *data;
  const uint8 *data_bound;
  uint32       len;
} secasn1_data_type;

/* ASN.1 bit string data holder */
typedef struct secasn1_bit_string_type
{
  const uint8 *data;
  uint32       len;
  uint32       unused;
} secasn1_bit_string_type;

/* Extra structures put here since other files are removed */
typedef struct clk_julian_type
{
  uint32  year;
  uint32  month;
  uint32  day;
  uint32  hour;
  uint32  minute;
  uint32  second;
} clk_julian_type;
/*===========================================================================
                   Defines from secboot_asn1.h --- end
===========================================================================*/

typedef struct secboot_ou_field_info_type
{
  /* Enable JTag before running DMSS */
  uint32   enable_jtag;
  uint32   oem_id;
  uint64   sw_id;  
  uint64   hw_id;
  uint8    code_hash_algo[6];
} secboot_ou_field_info_type;

/*===========================================================================
                   Defines from secboot_x509.h --- start
===========================================================================*/

/* Public Key algorithms in the certificate */
typedef enum
{
  SECX509_PUBKEY_RSA = 0,
  SECX509_PUBKEY_DSA = 1,
  SECX509_PUBKEY_DH  = 2,
  SECX509_PUBKEY_MAX,               /* Last one, for error checking */
  SECX509_RESERVED_1 = 0x7FFFFFFF
} secx509_pubkey_algo_type;

/* Certificate signature algorithm type */
typedef enum
{
  SECX509_md5WithRSAEncryption    = 0,
  SECX509_md2WithRSAEncryption    = 1,
  SECX509_sha1WithRSAEncryption   = 2,
  SECX509_sha256WithRSAEncryption = 3,  
  SECX509_SIG_ALGO_MAX,                 /* Last one, for error checking */
  SECX509_RESERVED_2            = 0x7FFFFFFF
} secx509_sig_algo_type;

/* RSA public key parameters */
typedef struct secx509_rsa_pubkey_type
{
  uint32  mod_len;
  uint8   mod_data[MAX_KEY_SIZE/8]; /* from bits -> bytes */
  uint32  exp_e_len;
  uint8   exp_e_data[MAX_PUB_EXP_KEY_SIZE/8]; /* from bits -> bytes */

} secx509_rsa_pubkey_type;

/* DSA public key parameters */
typedef struct secx509_dsa_pubkey_type
{
  uint32  dummy;

} secx509_dsa_pubkey_type;

/* DH public key parameters */
typedef struct secx509_dh_pubkey_type
{
  uint32 mod_len;
  uint8  mod_data[MAX_KEY_SIZE];
  uint32 exp_len;
  uint8  exp_data[MAX_KEY_SIZE];

} secx509_dh_pubkey_type;


/* Union of all the public key types */
typedef struct secx509_pubkey_type
{
  secx509_pubkey_algo_type  algo;
  union
  {
    secx509_rsa_pubkey_type  rsa;

  }key;

} secx509_pubkey_type;

/* Signature Structure */
typedef struct secx509_signature_type
{
  secx509_sig_algo_type   algo_id;
  secasn1_data_type           val;

} secx509_signature_type;

/* Distinguished name structure */
typedef struct secx509_dn_type
{
  uint32             num_attrib;
  secasn1_data_type  data;

} secx509_dn_type;

/* Version structure */
typedef struct secx509_version_type
{
  uint32             ver;
  secasn1_data_type  val;

} secx509_version_type;

/* Time structure */
typedef struct secx509_time_type
{
  uint32             time;
  secasn1_data_type  data;

} secx509_time_type;

/* Authority Key Identifier structure */
typedef struct secx509_auth_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;
  secx509_dn_type    name;
  secasn1_data_type  serial_number;

} secx509_auth_key_id_type;

/* Subject Key Identifier structure */
typedef struct secx509_subject_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;

} secx509_subject_key_id_type;

/* Key Usage structure */
typedef struct secx509_key_usage_type
{
  uint32   val;
  boolean  set;

} secx509_key_usage_type;

/* CA structure */
typedef struct secx509_ca_type
{
  boolean  set;
  boolean  val;

} secx509_ca_type;

/* Extension structure type */
typedef struct secx509_ext_type
{
  boolean                          set;
  secx509_auth_key_id_type     auth_key_id;
  secx509_subject_key_id_type  subject_key_id;
  secx509_key_usage_type       key_usage;
  secx509_key_usage_type       ex_key_usage;
  int32                            path_len;
  secx509_ca_type              ca;

} secx509_ext_type;

/* Certificate information structure */
typedef struct secx509_cert_info_type
{
  secx509_version_type     version;
  secasn1_data_type            serial_number;
  secx509_signature_type   algorithm;
  secx509_dn_type          issuer;
  secx509_time_type        not_before;
  secx509_time_type        not_after;
  secx509_dn_type          subject;
  secx509_pubkey_type      pub_key;
  secasn1_bit_string_type      issuer_unique_id;
  secasn1_bit_string_type      subject_unique_id;
  secx509_ext_type         extension;

} secx509_cert_info_type;

/* Certificate structure */
typedef struct secx509_cert_type
{
  /* The cert_info needs to be the first member */
  secx509_cert_info_type  cert_info;

  /* Signature info on the cert */
  secx509_pubkey_type     pkey;
  secx509_sig_algo_type   sig_algo;
  uint32                      cinf_offset;
  uint32                      cinf_byte_len;
  hash_alg_t              code_hash_alg;
  uint8                       sig[SECX509_MAX_VRFY_SIG_LEN];
  uint32                      sig_len;
  
  /* For bookkeeping */
  boolean                     __used;
  uint32                      asn1_size_in_bytes;

  uint64                      hw_id;
} secx509_cert_type;
/*===========================================================================
                   Defines from secboot_x509.h --- end
===========================================================================*/

typedef struct secboot_qfprom_info_type
{
  uint32  is_oem_table;
  uint32   oem_root_cert_sel;    /* valid only if is_oem_table=TRUE */
  uint32   oem_num_root_certs;   /* valid only if is_oem_table=TRUE */
} secboot_qfprom_info_type;

#endif /* SECBOOT_TYPES_H */
