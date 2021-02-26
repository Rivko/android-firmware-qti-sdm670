#ifndef SWCRYPTO_API_H
#define SWCRYPTO_API_H
/*===========================================================================

                          SOFTWARE SECURITY API

                    
GENERAL DESCRIPTION 
This API provides HASH, HMAC and AES related function calls 
 
AES ECB and CBC expects the data to be multiple of 16 byte size 
AES CTR doesnt follow the above restriction  
 
Hash doesnt have a size restriction 
 
The functions are run in caller context and caller task will bare 
delay for the same. Thus sending in large data will lead to the operation 
taking longer. 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS 
 
Copyright (c) 2012 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 
  $Author: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/10   yk     initial version
===========================================================================*/

/**
 * AES Key and IV sizes
 */
#define SW_AES128_KEY_SIZE   16
#define SW_AES_IV_SIZE    16
#define SW_AES_BLOCK_BYTE_LEN  16
#define SW_AES192_KEY_SIZE   24
#define SW_AES256_KEY_SIZE   32
#define SW_AES_MAX_KEY_SIZE  60

/** 
 * SHA DIGEST and Block sizes 
  */
#define SW_SHA1_DIGEST_SIZE             20
#define SW_SHA256_DIGEST_SIZE           32
#define SW_SHA_BLOCK_SIZE               64


typedef enum
{
  /*** Generic outcomes of operations ***/
  E_SUCCESS            =  0,    /* Operation successful                  */
  E_FAILURE            =  1,    /* Operation failed due to unknown err   */
  E_NOT_ALLOWED        =  2,    /* Operation currently not allowed       */
  E_NOT_AVAILABLE      =  3,    /* Operation currently not available     */
  E_NOT_SUPPORTED      =  4,    /* Operation not yet implemented         */
  E_CANCELED           =  5,    /* Operation was scheduled but canceled  */
  E_ABORTED            =  6,    /* Operation was started but interrp'd   */
  E_INTERRUPTED        =  7,    /* Operation was started but interrp'd   */
  E_DEADLOCK           =  8,    /* Operation would cause a deadlock      */
  E_AGAIN              =  9,    /* Caller should retry operation         */
  E_RESET              = 10,    /* Executing module was reset            */
  E_WOULD_BLOCK        = 11,    /* nonblocking IO operation would block  */
  E_IN_PROGRESS        = 12,    /* Operation in progress                 */
  E_ALREADY_DONE       = 13,    /* Operation already completed           */

  /*** Memory management related error conditions ***/
  E_NO_DSM_ITEMS       = 14,    /* Out of DSM items                      */
  E_NO_MEMORY          = 15,    /* Allocation from a memory pool failed  */

  /*** Parameter or data parsing related error conditions ***/
  E_INVALID_ARG        = 16,    /* Arg is not recognized                 */
  E_OUT_OF_RANGE       = 17,    /* Arg value is out of range             */
  E_BAD_ADDRESS        = 18,    /* Ptr arg is bad address                */
  E_NO_DATA            = 19,    /* Expected data, received none          */
  E_BAD_DATA           = 20,    /* Data failed sanity check, e.g. CRC    */
  E_DATA_INVALID       = 21,    /* Data is correct, but contents invalid */
  E_DATA_EXPIRED       = 22,    /* Data is not yet or not any more valid */
  E_DATA_TOO_LARGE     = 23,    /* Data is too large to process          */

  /*** Hardware related error conditions ***/
  E_NO_DEV             = 24,    /* Hardware resource not available       */
  E_DEV_FAILURE        = 25,    /* Hardware failure                      */
  E_NV_READ_ERR        = 26,    /* Operation failed due to NV read err   */
  E_NV_WRITE_ERR       = 27,    /* Operation failed due to NV write err  */
  E_EFS_ERROR          = 28,    /* EFS interface error.                  */
  E_DSP_ERROR          = 29,    /* DSP returned error on cmd (-1)        */

  /*** Protocol operation related error conditions ***/
  E_TIMER_EXP          = 30,    /* Operation not completed (timer exp)   */
  E_VERSION_MISMATCH   = 31,    /* Unexpected software or protocol ver.  */
  E_TASK_SIG_ERR       = 32,    /* Signal unknown or no handler          */
  E_TASK_Q_FULL        = 33,    /* Task queue is full                    */
  E_PROT_Q_FULL        = 34,    /* Protocol queue is full                */
  E_PROT_TX_Q_FULL     = 35,    /* Protocol tx data queue is full        */
  E_PROT_RX_Q_FULL     = 36,    /* Protocol rx data queue is full        */
  E_PROT_UNKN_CMD      = 37,    /* Protocol doesn't understand cmd       */
  E_PROT_UNKN_IND      = 38,    /* Protocol doesn't understand ind       */
  E_PROT_UNKN_MSG      = 39,    /* Protocol doesn't understand msg       */
  E_PROT_UNKN_SIG      = 40,    /* Protocol doesn't understand signal    */

  /*** File related error conditions ***/
  E_NO_ENTRY           = 41,    /* No such file or directory             */
  E_FILENAME_TOO_LONG  = 42,    /* File name too long                    */
  E_DIR_NOT_EMPTY      = 43,    /* Directory not empty                   */
  E_IS_DIR             = 44,    /* Is a directory                        */
  E_EOF                = 45,    /* End Of File                           */
  E_XDEV               = 46,    /* Attempt to cross device               */
  E_BAD_F              = 47,    /* Bad file descriptor                   */
  E_MAX_OPEN_FILES     = 48,    /* Too many open files                   */
  E_BAD_FMT            = 49,    /* Bad formatted media                   */
  E_FILE_EXISTS        = 50,    /* File already exists                   */
  E_EFS_FULL           = 51,    /* No space left on device               */
  E_NOT_DIR            = 52,    /* Not a directory                       */
  E_NOT_EFS_ITEM       = 53,    /* Not an EFS2 item                      */
  E_ALREADY_OPEN       = 54,    /* File/Directory already open           */
  E_BUSY               = 55,    /* Device/Item is busy                   */
  E_OUT_OF_NODES       = 56,    /* Too many symbolic links encountered   */

  /*** SQL related error conditions ***/
  E_SQL_INTERNAL       = 57,    /* An internal logic error in SQLite     */
  E_SQL_BUSY           = 58,    /* The database file is locked           */
  E_SQL_LOCKED         = 59,    /* A table in the database is locked     */
  E_SQL_READONLY       = 60,    /* Attempt to write a readonly database  */
  E_SQL_IOERR          = 61,    /* Some kind of disk I/O error occurred  */
  E_SQL_CORRUPT        = 62,    /* The database disk image is malformed  */
  E_SQL_NOTFOUND       = 63,    /* Table or record not found             */
  E_SQL_FULL           = 64,    /* Insertion failed because DB is full   */
  E_SQL_CANTOPEN       = 65,    /* Unable to open database file          */
  E_SQL_PROTOCOL       = 66,    /* Database lock protocol error          */
  E_SQL_EMPTY          = 67,    /* Database is empty                     */
  E_SQL_SCHEMA         = 68,    /* The database schema changed           */
  E_SQL_CONSTRAINT     = 69,    /* Abort due to constraint violation     */
  E_SQL_MISMATCH       = 70,    /* Data type mismatch                    */
  E_SQL_MISUSE         = 71,    /* Library used incorrectly              */
  E_SQL_NOLFS          = 72,    /* uses OS features not supported        */
  E_SQL_FORMAT         = 73,    /* Auxiliary database format error       */
  E_SQL_NOTADB         = 74,    /* file opened that is not a db file     */
  E_SQL_ROW            = 75,    /* sqlite3_step() has another row ready  */
  E_SQL_DONE           = 76,    /* sqlite3_step() has finished executing */
  E_NO_ATTR            = 77,    /* No Attr found                         */
  E_INVALID_ITEM       = 78,    /* Invalid Item found                    */
  /*** Networking related error conditions ***/
  E_NO_NET             = 100,   /* Network is not connected (up)         */
  E_NOT_SOCKET         = 101,   /* Socket operation on non-socket        */
  E_NO_PROTO_OPT       = 102,   /* Network protocol not available        */
  E_SHUTDOWN           = 103,   /* Socket is being shut down             */
  E_CONN_REFUSED       = 104,   /* Connection refused by peer            */
  E_CONN_ABORTED       = 105,   /* Connection aborted                    */
  E_IS_CONNECTED       = 106,   /* Connection is already established     */
  E_NET_UNREACH        = 107,   /* Network destination is unreachable    */
  E_HOST_UNREACH       = 108,   /* Host destination is unreachable       */
  E_NO_ROUTE           = 109,   /* No route to destination               */
  E_ADDR_IN_USE        = 110,   /* IP address is already in use          */

  /*** BCMCS related error conditions ***/
  E_DB_INVALID         = 111,   /* Entire database is invalid            */
  E_FLOW_NOT_IN_ZONE   = 112,   /* The flow is not in the specified zone */
  E_ZONE_NOT_IN_DB     = 113,   /* The specified zone is not in database */
  E_BUF_OVERFLOW       = 114,   /* The output buffer is too small        */
  //E_EOF              = 115,   /* End of file is reached                */
  E_DB_OVERFLOW        = 116,   /* The db is full, no further updates    */
  E_DB_NO_OVERWRITE    = 117,   /* value exists and overwrite is false   */
  E_NOT_FOUND          = 118,   /* Data not found                        */
  E_NO_RIGHTS          = 119,   /* No Rights/Permission to access data   */
  E_HASH_VERIF_FAIL    = 120,   /* Hash Verification on DCF Failed       */

  /*** HTTP related error conditions ***/
  E_CONTINUE               = 200,   /* Server response status code of continue */

  /* Values to be deleted */
  IxSUCCESS           =  E_SUCCESS,
  IxFAILED,
  IxNOMEMORY, 
  IxBADPARAM,
  IxNOTSUPPORTED,

  E_RESERVED           = 0x7FFFFFFF
} sw_crypto_errno_enum_type;


typedef struct
{
  void                              *pvBase; 
  uint32                            dwLen;  
} __attribute__((__packed__)) IovecType;


typedef struct
{
  IovecType                     *iov;                 
  uint32                        size;                 
} __attribute__((__packed__)) IovecListType;


typedef void CryptoCntxHandle;

/*----------------------------------------------------------*/
/*        SHA and HMAC related declarations                 */
/*----------------------------------------------------------*/

/**
 * Enum for Hash algorithm type
 */
typedef enum
{
  SW_AUTH_ALG_SHA1               = 0x1,
  SW_AUTH_ALG_SHA256
} SW_Auth_Alg_Type;


/*----------------------------------------------------------*/
/*                 AES related declarations                 */
/*----------------------------------------------------------*/

/**
 * Enum for Cipher algorithm type
 */
typedef enum {
  SW_CIPHER_ALG_AES128            = 0x0,
  SW_CIPHER_ALG_AES256            = 0x1,
  SW_CIPHER_ALG_MAX               = 0x7FFFFFFF
} SW_Cipher_Alg_Type;

/**
 * Enum for Cipher parameters
 */
typedef enum 
{ 
  SW_CIPHER_PARAM_DIRECTION   = 0x01,
  SW_CIPHER_PARAM_KEY         = 0x02,
  SW_CIPHER_PARAM_IV          = 0x03,
  SW_CIPHER_PARAM_MODE        = 0x04,
  SW_CIPHER_PARAM_BLOCKSIZE   = 0x05,
  SW_CIPHER_PARAM_KEY_SIZE    = 0x06,
  SW_CIPHER_PARAM_IV_SIZE     = 0x07,
  SW_CIPHER_PARAM_MAX         = 0x7FFFFFFF
} SW_CipherParam;


/**
 * Enum for Cipher direction
 */
typedef enum 
{
  SW_CIPHER_ENCRYPT           = 0x00, 
  SW_CIPHER_DECRYPT           = 0x01, 
  SW_CIPHER_MAX               = 0x7FFFFFFF
} SW_CipherEncryptDir;


/**
 * Enum for Cipher key size
 */
typedef enum 
{
  SW_CIPHER_KEY_SIZE_AES128      = 0x0,
  SW_CIPHER_KEY_SIZE_AES192      = 0x1,
  SW_CIPHER_KEY_SIZE_AES256      = 0x2,
  SW_CIPHER_KEY_SIZE_MAX         = 0x7FFFFFFF
} SW_Cipher_Key_Size;


/**
 * Enum for Cipher mode
 */
typedef enum 
{
  SW_CIPHER_MODE_ECB          = 0x0,    ///< ECB Mode   
  SW_CIPHER_MODE_CBC          = 0x1,    ///< CBC Mode   
  SW_CIPHER_MODE_CTR          = 0x2     ///< CTR Mode   
} SW_CipherModeType;


/*----------------------------------------------------------*/
/*                    SHA and HMAC apis                     */
/*----------------------------------------------------------*/

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param auth_alg     [in] see SW_Auth_Alg_Type
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg); 

/**
 * @brief Update function for sha1/sha256 for intermediate data 
 *        blocks hash
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecIn      [in] Input to cipher
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn);

/**
 * @brief Sha1/Sha256 last block hash update
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecOut     [in] Output from cipher
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut);

/**
 * @brief Main function for sha1/sha256 hmac
 *
 * @param key_ptr      [in] Pointer of Key for HMAC
 * @param keylen       [in] key length (16bytes for SHA1 / 
 *                          32bytes for SHA256)
 * @param ioVecIn      [in] Input to cipher
 * @param ioVecOut     [in] Output from cipher 
 * @param pAlgo        [in] See enum SW_Auth_Alg_Type  
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hmac (uint8 * key_ptr, uint32 keylen, IovecListType ioVecIn, IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);


/*----------------------------------------------------------*/
/*                    CIPHER apis                           */
/*----------------------------------------------------------*/

/**
 * @brief Intialize a cipher context 
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param pAlgo        [in] Cipher algorithm type 
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type SW_Cipher_Init(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);

/**
 * @brief This function encrypts/decrypts the passed message
 *        using the specified algorithm.
 *
 * @param handle       [in] Pointer of pointer to cipher context
 * @param ioVecIn      [in] Input to cipher
 * @param ioVecOut     [in] Output from cipher
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type SW_CipherData (CryptoCntxHandle   *handle, 
                               IovecListType      ioVecIn,
                               IovecListType      *ioVecOut);

/**
 * @brief This functions sets the Cipher paramaters used by 
 *        SW_CRYPTO_CipherData
 *
 * @param handle    [in] Pointer of pointer to cipher context
 * @param nParamID  [in] Cipher parameter id to set
 * @param pParam    [in] Pointer to parameter data 
 * @param cParam    [in] Size of parameter data in bytes
 *
 * @return errno_enum_type
 *
 * @see SW_CRYPTO_Cipher_ProcessBlocks 
 *
 */
sw_crypto_errno_enum_type SW_Cipher_SetParam (CryptoCntxHandle     *handle,
                                    SW_CipherParam       nParamID, 
                                    const void           *pParam, 
                                    uint32               cParam );


/**
 * @brief Deintialize a cipher context 
 *
 * @param handle       [in] Pointer to pointer to cipher context
 * @param pAlgo        [in] Cipher algorithm type 
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type SW_Cipher_DeInit(CryptoCntxHandle **handle, SW_Cipher_Alg_Type pAlgo);


/*!
   @addtogroup crypto_securechannel
   @{
*   This section explains the secure channel APIs
*/
/**
  @brief
  This function is used to initialize the keys at modem side.

  @param
  None

  @dependencies
  None

  @return
  E_SUCCESS      - Successful.
  E_ALREADY_DONE - Secure channel is already established.

  @sideeffects
  None.

*/

sw_crypto_errno_enum_type secapi_secure_channel_init
(
 void
);

typedef enum
{
  SC_SSID_TZ=0,                  /* Trustzone.                                  */
  SC_SSID_MPSS,                  /* Modem subsystem.                            */
  SC_SSID_LPASS,                 /* Low power audio subsystem.                  */
  SC_SSID_WCNSS,                 /* Wireless conectivity subsystem.             */
  SC_SSID_COUNT,                 /* Number of supported SS for SC.              */
  SC_SSID_UNSPECIFIED,           /* Client dont want to speficy SSID            */
  SC_UNSUPPORTED_SS=0x7F         /* Subsystem not supported.                    */
} secapi_sc_ss_e_type;

typedef enum
{
  SC_CID_SSM=0,                  /* Trustzone.                                  */
  SC_CID_UIM,                    /* Modem subsystem.                            */
  SC_CID_COUNT,                  /* Number of supported CIDs for SC.            */
  SC_UNSUPPORTED_CID=0x7F        /* Subsystem not supported.                    */
} secapi_sc_cid_e_type;

/**
  @brief
  This is used to secure the the input message

  @details
  The output buffer should be large enough to hold the encrypted message
  and some internal headers and possible padding. Recommended output buffer size
  is atleast input_msg_len + 100 bytes. The memory has to be managed by the caller.

  @param[in] ssid                    destination Sub-system ID
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the plaintext data.
  @param[in] input_msg_len           Length of the plaintext in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the secure blob
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to
                                     the length of the secure blob on return

  @return
  E_SUCCESS - Successful.  \n
  E_FAILURE - Operation failed.

  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
sw_crypto_errno_enum_type secapi_secure_message
(
  secapi_sc_ss_e_type  ssid,
  secapi_sc_cid_e_type cid,
  const uint8*         input_msg_ptr,
  uint32               input_msg_len,
  uint8*               output_msg_ptr,
  uint32*              output_msg_len_ptr
);

/**
  @brief
  This is used to authenticate and decrypt the secure blob

  @details
  The output buffer should be large enough to hold the decrypted message
  Recommended output buffer size is atleast input_msg_len.
  The memory has to be managed by the caller.

  @param[in] ssid                    Source Sub-system ID 
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the secure blob.
  @param[in] input_msg_len           Length of the secure blob in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the decrypted data
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to
                                     the length of the decrypted data on return.

  @return
  E_SUCCESS - Successful.  \n
  E_FAILURE - Operation failed.

  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/

sw_crypto_errno_enum_type secapi_authenticate_decrypt_message
(
  secapi_sc_ss_e_type  ssid,
  secapi_sc_cid_e_type cid,
  const uint8*         input_msg_ptr,
  uint32               input_msg_len,
  uint8*               output_msg_ptr,
  uint32*              output_msg_len_ptr
);


/** @} */  /* end_addtogroup_crypto_securechannel */

#endif /* SWCRYPTO_API_H */
