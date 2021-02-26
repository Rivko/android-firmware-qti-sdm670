#ifndef AES_CORE_H
#define AES_CORE_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/aes/aes_core.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/12   yk      initial version
===========================================================================*/


 /*
   I retain copyright in this code but I encourage its free use provided
   that I don't carry any responsibility for the results. I am especially 
   happy to see it used in free and open source software. If you do use 
   it I would appreciate an acknowledgement of its origin in the code or
   the product that results and I would also appreciate knowing a liitle
   about the use to which it is being put.

   Dr B. R. Gladman <brg@gladman.uk.net> 1st June 2001.

   This is an implementation of the AES encryption algorithm (Rijndael)
   designed by Joan Daemen and Vincent Rijmen. This version is designed
   to provide both fixed and dynamic block and key lengths and can also 
   run with either big or little endian internal byte order.
    
   NOTE: Input block and key lengths are given in terms of the lengths of
   the byte arrays involved, the legal values being 16, 24 and 32.

   A. THE CIPHER INTERFACE

   byte                    (an unsigned 8-bit type)
   word                    (an unsigned 32-bit type)
   aes_ret:                (a signed 16 bit type for function return values)
        aes_good            (value != 0, a good return)
        aes_bad             (value == 0, an error return)
   enum aes_key:           (encryption direction)
        enc                 (set key for encryption)
        dec                 (set key for decryption)
        both                (set key for both)
   class or struct aes     (structure for context)

   C subroutine calls:

   aes_ret set_blk(const word block_length, aes *cx)  (variable block size)
   aes_ret set_key(const byte key[], const word key_length,
				   const enum aes_key direction, aes *cx)
   aes_ret encrypt(const byte input_blk[], byte output_blk[], const aes *cx)
   aes_ret decrypt(const byte input_blk[], byte output_blk[], const aes *cx)

   IMPORTANT NOTE: If you are using this C interface and your compiler does 
   not set the memory used for objects to zero before use, you will need to 
   ensure that cx.mode is set to zero before using the C subroutine calls.

   C++ aes class subroutines:

   aes_ret set_blk(const word block_length)  (variable block size)
   aes_ret set_key(const byte key[], const word key_length,
				   const aes_key direction)
   aes_ret encrypt(const byte input_blk[], byte output_blk[]) const
   aes_ret decrypt(const byte input_blk[], byte output_blk[]) const

   The block length inputs to set_block and set_key are in numbers of
   BYTES, not bits.  The calls to subroutines must be made in the above 
   order but multiple calls can be made without repeating earlier calls
   if their parameters have not changed. If the cipher block length is
   variable but set_blk has not been called before cipher operations a
   value of 16 is assumed (that is, the AES block size). In contrast to 
   earlier versions the block and key length parameters are now checked
   for correctness and the encryption and decryption routines check to 
   ensure that an appropriate key has been set before they are called.
   
   B. BYTE ORDER WITHIN 32 BIT WORDS

   The fundamental data processing units in Rijndael are 8-bit bytes. The 
   input, the output and the key input are all enumerated arrays of bytes 
   in which bytes are numbered starting at zero and increasing to one less
   than the number of bytes in the array in question.  When these inputs 
   and outputs are considered as bit sequences, the n'th byte contains 
   bits 8n to 8n+7 of the sequence with the lower numbered bit mapped to 
   the most significant bit within the  byte (i.e. that having a numeric 
   value of 128).  However, Rijndael can be implemented more efficiently 
   using 32-bit words to process 4 bytes at a time provided that the order
   of bytes within words is known.  This order is called big-endian if the 
   lowest numbered bytes in words have the highest numeric significance 
   and little-endian if the opposite applies. This code can work in either 
   order irrespective of the native order of the machine on which it runs.
   The byte order used internally is set by defining INTERNAL_BYTE_ORDER
   whereas the order for all inputs and outputs is specified by defining 
   EXTERNAL_BYTE_ORDER, the only purpose of the latter being to determine
   if a byte order change is needed immediately after input and immediately
   before output to account for the use of a different internal byte order.  
   In almost all situations both of these defines will be set to the native 
   order of the processor on which the code is to run but other settings 
   may somtimes be useful in special circumstances.

#define INTERNAL_BYTE_ORDER LITTLE_ENDIAN
#define EXTERNAL_BYTE_ORDER LITTLE_ENDIAN

   C. COMPILATION 

   To compile AES (Rijndael) for use in C code
    a. Exclude the AES_DLL define in aes.h
    b. Exclude the AES_IN_CPP define in aes.h

   To compile AES (Rijndael) for use in in C++ code
    a. Exclude the AES_DLL define in aes.h
    b. Include the AES_IN_CPP define in aes.h

   To compile AES (Rijndael) in C as a Dynamic Link Library
    a. Include the AES_DLL define in aes.h
    b. Compile the DLL.  If using the test files, exclude aes.c from
       the test build project and compile it with the same defines 
       as used for the DLL (ensure that the DLL path is correct)

   D. CONFIGURATION OPTIONS (see also aes.c)

   1. define AES_BLOCK_SIZE to set the cipher block size (16, 24 or 32) or
      leave this undefined for dynamically variable block size (this will
      result in much slower code).
   2. set AES_IN_CPP to use the code from C++ rather than C
   3. set AES_DLL if AES (Rijndael) is to be compiled to a DLL
   4. set INTERNAL_BYTE_ORDER to one of the above constants to set the
      internal byte order (the order used within the algorithm code)
   5. set EXTERNAL_BYTE_ORDER to one of the above constants to set the byte
      order used at the external interfaces for the input, output and key
      byte arrays.

   IMPORTANT NOTE: AES_BLOCK_SIZE is in BYTES: 16, 24, 32 or undefined for aes.c
   and 16, 20, 24, 28, 32 or undefined for aes++.c.  If left undefined a 
   slower version providing variable block length is compiled   

#define AES_BLOCK_SIZE  16

   Define AES_IN_CPP if you intend to use the AES C++ class rather than the
   C code directly.

#define AES_IN_CPP

   Define AES_DLL if you wish to compile the code to produce a Windows DLL

#define AES_DLL

*/
#include "crypto/crypto_env_comdef.h"
#include "crypto/crypto_env.h"

#define SECCRYPT_ENC_AES_BLOCK_SIZE     16
#define SECAPI_ENC_AES_BLOCK_BYTE_LEN   16


/* Adapt the open source to our build.
*/
#define byte  uint8                       /* must be an 8-bit storage unit */
#define word uint32                       /* must be a 32-bit storage unit */

#define cf_dec int

typedef struct
{
  uint32  sub_key[4 * SECCRYPT_ENC_AES_BLOCK_SIZE]; /* the key schedule  */
  uint32  Nkey;               /* the number of words in the key input block */
  uint32  Nrnd;               /* the number of cipher rounds                */
} secenc_aes_ctx_type __attribute__ ((aligned (sizeof(uint32))));

#define AES_BLOCK_SIZE  16

#define BIG_ENDIAN      1   /* do not change */
#define LITTLE_ENDIAN   2   /* do not change */

#define INTERNAL_BYTE_ORDER LITTLE_ENDIAN
#define EXTERNAL_BYTE_ORDER LITTLE_ENDIAN

#if defined(AES_BLOCK_SIZE) && ((AES_BLOCK_SIZE & 3) || AES_BLOCK_SIZE < 16 || AES_BLOCK_SIZE > 32)
#error an illegal block size has been specified
#endif  

#if defined(AES_DLL)
#undef AES_IN_CPP
#endif

/* Adjust the C subroutine names if using the C++ interface */

#if defined(AES_IN_CPP)
#define c_name(x)   c_##x
#else
#define c_name(x)   x
#endif

 /*
   The number of key schedule words for different block and key lengths
   (allowing for the method of computation which requires the length to 
   be a multiple of the key length):

   Key Schedule    key length (bytes)
   Length          16  20  24  28  32
                ---------------------
   block     16 |  44  60  54  56  64
   length    20 |  60  60  66  70  80
   (bytes)   24 |  80  80  78  84  96
             28 | 100 100 102  98 112
             32 | 120 120 120 126 120

   Rcon Table      key length (bytes)
   Length          16  20  24  28  32
                ---------------------
   block     16 |  10   9   8   7   7
   length    20 |  14  11  10   9   9
   (bytes)   24 |  19  15  12  11  11
             28 |  24  19  16  13  13
             32 |  29  23  19  17  14
   
   The following values assume that the key length will be variable and may
   be of maximum length (32 bytes). 

   Nk = number_of_key_bytes / 4
   Nc = number_of_columns_in_state / 4
   Nr = number of encryption/decryption rounds
   Rc = number of elements in rcon table
   Ks = number of 32-bit words in key schedule
 */

#define Nr(Nk,Nc)	((Nk > Nc ? Nk : Nc) + 6)
#define Rc(Nk,Nc)	((Nb * (Nr(Nk,Nc) + 1) - 1) / Nk)	
#define Ks(Nk,Nc)	(Nk * (Rc(Nk,Nc) + 1))

#if !defined(AES_BLOCK_SIZE)
#define RC_LENGTH    29
#define KS_LENGTH   128
#else
#define RC_LENGTH   (5 * AES_BLOCK_SIZE / 4 - (AES_BLOCK_SIZE == 16 ? 10 : 11))
#define KS_LENGTH   (4 * AES_BLOCK_SIZE)
#endif

/* End of configuration options, but see also aes.c */

//typedef unsigned char   byte;           /* must be an 8-bit storage unit */
//typedef unsigned long   word;           /* must be a 32-bit storage unit */
//typedef short           aes_ret;        /* function return value         */

//#define aes_bad     0
//#define aes_good    1

 /*
   upr(x,n): rotates bytes within words by n positions, moving bytes 
   to higher index positions with wrap around into low positions
   ups(x,n): moves bytes by n positions to higher index positions in 
   words but without wrap around
   bval(x,n): extracts a byte from a word
 */

#if(INTERNAL_BYTE_ORDER == LITTLE_ENDIAN)

#define upr(x,n)        (((x) << 8 * (n)) | ((x) >> (32 - 8 * (n))))
#define ups(x,n)        ((x) << 8 * (n))
#define bval(x,n)       ((byte)((x) >> 8 * (n)))
#define byte_swap(x)    (upr(x,1) & 0x00ff00ff | upr(x,3) & 0xff00ff00)
#define bytes2word(b0, b1, b2, b3)  \
        ((word)(b3) << 24 | (word)(b2) << 16 | (word)(b1) << 8 | (b0))
#else /* INTERNAL_BYTE_ORDER == LITTLE_ENDIAN */

#define upr(x,n)        (((x) >> 8 * (n)) | ((x) << (32 - 8 * (n))))
#define ups(x,n)        ((x) >> 8 * (n)))
#define bval(x,n)       ((byte)((x) >> 24 - 8 * (n)))
#define byte_swap(x)    (upr(x,3) & 0x00ff00ff | upr(x,1) & 0xff00ff00)
#define bytes2word(b0, b1, b2, b3)  \
        ((word)(b0) << 24 | (word)(b1) << 16 | (word)(b2) << 8 | (b3))
#endif /* INTERNAL_BYTE_ORDER == LITTLE_ENDIAN */

#if(INTERNAL_BYTE_ORDER == EXTERNAL_BYTE_ORDER)

#define word_in(x)      *(word*)(x)
#define word_out(x,v)   (*(word*)(x) = (v))

#else /* INTERNAL_BYTE_ORDER == EXTERNAL_BYTE_ORDER */

#define word_in(x)      byte_swap(*(word*)(x))
#define word_out(x,v)   *(word*)(x) = byte_swap(v)

#endif /* INTERNAL_BYTE_ORDER == EXTERNAL_BYTE_ORDER */

enum aes_const  {   Nrow =  4,  /* the number of rows in the cipher state       */
                    Mcol =  8,  /* maximum number of columns in the state       */
#if defined(AES_BLOCK_SIZE)         /* set up a statically defined block size       */
                    Ncol =  AES_BLOCK_SIZE / 4,  
                    Shr0 =  0,  /* the cyclic shift values for rows 0, 1, 2 & 3 */
                    Shr1 =  1,  
                    Shr2 =  AES_BLOCK_SIZE == 32 ? 3 : 2,
                    Shr3 =  AES_BLOCK_SIZE == 32 ? 4 : 3
#endif /* AES_BLOCK_SIZE */
                };

enum aes_key    
{   
  //enc  =  1,  /* set if encryption is needed */
  //dec  =  2,  /* set if decryption is needed */
  //both =  3   /* set if both are needed      */
  AES_ENCRYPT_KEY  =  1,  /* set if encryption is needed */
  AES_DECRYPT_KEY  =  2,  /* set if decryption is needed */
  //AES_BOTH_KEYS    =  3   /* set if both are needed      */
};

#if defined(__cplusplus)
extern "C"
{
#endif

/* _stdcall is needed for Visual Basic DLLs but is not necessary for C/C++ */
 
typedef struct
{
    word    sub_key[KS_LENGTH];   /* the encryption key schedule      */
    //word    e_key[KS_LENGTH];   /* the encryption key schedule      */
    //word    d_key[KS_LENGTH];   /* the decryption key schedule           */
    word    Nkey;               /* the number of words in the key input block */
    word    Nrnd;               /* the number of cipher rounds                */
#if !defined(AES_BLOCK_SIZE)
    word    Ncol;               /* the number of columns in the cipher state */
#endif /* AES_BLOCK_SIZE */
    //byte    mode;               /* encrypt, decrypt or both               */
} aes_ctx_type;

//cf_dec c_name(set_key)(const byte key[], const word n_bytes, 
//                       const enum aes_key key_needed, 
//                       c_name(aes_ctx_type) *cx);
//cf_dec c_name(encrypt)(const byte in_blk[], byte out_blk[], 
//                       const c_name(aes_ctx_type) *cx);
//cf_dec c_name(decrypt)(const byte in_blk[], byte out_blk[], 
//                       const c_name(aes_ctx_type) *cx);
//cf_dec c_name(set_blk)(const word n_bytes, c_name(aes_ctx_type) *cx);

#if defined(__cplusplus)
}

#if defined(AES_IN_CPP) 

class aes
{
    c_name(aes) cx;
public:            
#if defined(AES_BLOCK_SIZE)
    aes()	{ cx.mode = 0; }
#else /* AES_BLOCK_SIZE */    
    aes(word n_bytes = 16)
            { cx.mode = 0; c_name(set_blk)(n_bytes, &cx); }
    aes_ret set_blk(const word n_bytes)
            { return c_name(set_blk)(n_bytes, &cx); }
#endif /* AES_BLOCK_SIZE */
    aes_ret set_key(const byte key[], const word n_bytes, const aes_key f)
            { return c_name(set_key)(key, n_bytes, f, &cx); }
    aes_ret encrypt(const byte in_blk[], byte out_blk[]) const
            { return c_name(encrypt)(in_blk, out_blk, &cx); }
    aes_ret decrypt(const byte in_blk[], byte out_blk[]) const
            { return c_name(decrypt)(in_blk, out_blk, &cx); }
};

#endif /* AES_IN_CPP */
#endif /* __cplusplus */

/*===========================================================================
FUNCTION aes_set_key

DESCRIPTION
  Generate the AES subkey values

DEPENDENCIES
  None

RETURN VALUE
  aes_good
 
SIDE EFFECTS
  None.
===========================================================================*/
/*cf_dec c_name(set_key)*/
cf_dec aes_set_key
(
const byte             in_key[], 
const word             n_bytes, 
const enum aes_key     key_needed /*f*/, 
secenc_aes_ctx_type    *cx
);

/*===========================================================================
FUNCTION aes_encrypt_block

DESCRIPTION
  AES encrypt a block of data.

DEPENDENCIES
  None

RETURN VALUE
  aes_good
 
SIDE EFFECTS
  None.
===========================================================================*/
cf_dec aes_encrypt_block
(
byte                      in_blk_1[], 
byte                      out_blk_1[], 
const secenc_aes_ctx_type *cx
);


/*===========================================================================
FUNCTION aes_decrypt_block

DESCRIPTION
  AES decrypt a block of data.

DEPENDENCIES
  None

RETURN VALUE
  aes_good
 
SIDE EFFECTS
  None.
===========================================================================*/
cf_dec aes_decrypt_block
(
byte                      in_blk_1[], 
byte                      out_blk_1[], 
const secenc_aes_ctx_type *cx
);

#endif /* AES_CORE_H */
