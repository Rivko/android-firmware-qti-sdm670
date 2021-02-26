/*===========================================================================

                    BOOT EXTERN ENCRYPTION DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external crypto drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2017,2018 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/17   aus     Updated to support minidump
25/04/17   bd      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include <cil_ramdump.h>
#include <boot_extern_crypto_interface.h>
#include <cil_loader.h>
#include "string.h"

//sizes in bytes
#define XBL_SW_AES_KEY_SIZE    32
#define XBL_SW_HMAC_KEY_SIZE    32
#define XBL_MINIDUMP_VERSION_SIZE 2
#define XBL_SW_AES_PLUS_HMAC_KEY_PLUS_VERSION_SIZE    (XBL_SW_AES_KEY_SIZE + XBL_SW_HMAC_KEY_SIZE + XBL_MINIDUMP_VERSION_SIZE)

#define XBL_SW_AES_IV_SIZE     16
#define XBL_SW_AES_MAC_SIZE     32
#define XBL_RSA_KEY_LEN         256//2048bits

static uint8             dload_aes_key[XBL_SW_AES_KEY_SIZE];
static uint8             dload_hmac_key[XBL_SW_HMAC_KEY_SIZE];
static uint8             dload_minidump_version[XBL_MINIDUMP_VERSION_SIZE] = {0x00,0x01};// format is MSB : LSB -> Major Version : Minor Version 
static uint8             dload_aes_and_hmac_key_and_version[XBL_SW_AES_PLUS_HMAC_KEY_PLUS_VERSION_SIZE+2];
static uint8             dload_rsa_encr_key[XBL_RSA_KEY_LEN];

static uint8             dload_aes_key_iv[NUM_REGIONS][XBL_SW_AES_IV_SIZE];
static uint8             dload_aes_key_mac[NUM_REGIONS][XBL_SW_AES_MAC_SIZE];


#define MD_UNENCR_MEMORY_REGION2 {(uint32)dload_rsa_encr_key, XBL_RSA_KEY_LEN, "Encrypted AES Key", "md_encr_key_aes.BIN"}
#define MD_UNENCR_MEMORY_REGION3 {(uint32)dload_aes_key_iv, (NUM_REGIONS*XBL_SW_AES_IV_SIZE), "IV key region", "md_encr_key_iv.BIN"}
#define MD_UNENCR_MEMORY_REGION4 {(uint32)dload_aes_key_mac, (NUM_REGIONS*XBL_SW_AES_MAC_SIZE), "MAC region", "md_encr_key_mac.BIN"}

#define MD_UNENCR_MEMORY_REGION_TABLE MD_UNENCR_MEMORY_REGION2,MD_UNENCR_MEMORY_REGION3,MD_UNENCR_MEMORY_REGION4,MEMORY_REGION_LAST

size_t memscpy( void *dst, size_t dst_size, const void *src, size_t src_size);
 
/*===========================================================================
                      FUNCTION DEFINITIONS
===========================================================================*/ 




static void generate_aes_iv(int i)
{
  PrngML_getdata_lite(dload_aes_key_iv[i],XBL_SW_AES_IV_SIZE);
}


static void generate_encr_aeskey(void)
{

  uint32  cipherlen = XBL_RSA_KEY_LEN;
  uint32  offset = 0;
  PrngML_getdata_lite(dload_aes_key, XBL_SW_AES_KEY_SIZE);
  PrngML_getdata_lite(dload_hmac_key, XBL_SW_HMAC_KEY_SIZE);
  
  offset+=memscpy(dload_aes_and_hmac_key_and_version,XBL_SW_AES_PLUS_HMAC_KEY_PLUS_VERSION_SIZE,dload_aes_key,(size_t)XBL_SW_AES_KEY_SIZE);
  offset+=memscpy(&dload_aes_and_hmac_key_and_version[offset],(XBL_SW_AES_PLUS_HMAC_KEY_PLUS_VERSION_SIZE-offset),dload_hmac_key,XBL_SW_HMAC_KEY_SIZE);
  offset+=memscpy(&dload_aes_and_hmac_key_and_version[offset],(XBL_SW_AES_PLUS_HMAC_KEY_PLUS_VERSION_SIZE-offset),dload_minidump_version,XBL_MINIDUMP_VERSION_SIZE);
   
  boot_encr_aes_key(dload_aes_and_hmac_key_and_version, XBL_SW_AES_PLUS_HMAC_KEY_PLUS_VERSION_SIZE, dload_rsa_encr_key, &cipherlen);
}

/*===========================================================================

**  Function :  cil_md_crypto_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Perform encryption initiliazation
*   
*
* @par Dependencies   
*   
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void cil_md_crypto_init(void)
{
  boot_sw_cipher_init();

  generate_encr_aeskey();
  boot_sw_cipher_post_init();
}

/*===========================================================================

**  Function :  cil_md_encrypt_region

** ==========================================================================
*/
/*!
* 
* @brief
*   Perform in-place encryption of the given region
*
* @par Dependencies   
*   
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void cil_md_encrypt_region(uint64 r_base, uint64 r_len, uint32 r_num)
{
  uint8 *iv, *mac;  
 
  generate_aes_iv(r_num);

  iv = dload_aes_key_iv[r_num];
  mac = dload_aes_key_mac[r_num];

  boot_sw_cipher((void*)(r_base), r_len, 
                 (void*)(r_base), r_len, 
                 (void*)(&dload_aes_key[0]), XBL_SW_AES_KEY_SIZE,
				 (void*)(&dload_hmac_key[0]), XBL_SW_HMAC_KEY_SIZE, 
                 (void*)(iv), XBL_SW_AES_IV_SIZE,
                 (void*)(mac), XBL_SW_AES_MAC_SIZE,
                 FALSE);
}

/*===========================================================================

**  Function :  cil_md_add_unencr_regions

** ==========================================================================
*/
/*!
* 
* @brief
*   Adds the encryption key regions to debug table
*
* @par Dependencies
*   This api Must be called AFTER cil_md_encrypt_region has been called for all regions
*   
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
void cil_md_add_unencr_regions(void)
{
    uint32 index;
    struct memory_region dump_regions[] = {MD_UNENCR_MEMORY_REGION_TABLE};     
    
    index = 0;

    while ( dump_regions[index].region_base != 0x0 ) 
    {
      dload_debug_add_region(OPTIONAL_DEF_SAVE, 
                             dump_regions[index].region_base,
                             dump_regions[index].region_size,
                             dump_regions[index].desc,
                             dump_regions[index].filename
                             );
      index++;
    }
}
