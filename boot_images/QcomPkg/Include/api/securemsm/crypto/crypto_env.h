#ifndef SECRSA_ENV_H
#define SECRSA_ENV_H

/**
@file secrsa_env.h
@brief This file is used to define the Hash functions, random
       number genrator function & Memory management functions
       that are provided by the environment
*/

/*===========================================================================
   Copyright (c) 2011-2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/securemsm/crypto/crypto_env.h#2 $
  $DateTime: 2018/07/04 01:19:53 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/22/18    jp      added timesafe_memcmp for timing side channel vulnerability
01/14/15    cz      Added "#define SUPPORT_RSA_CRT"
08/20/10    hv      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "crypto_env_comdef.h"

#define crypto_env_mutex_enter()

#define crypto_env_mutex_exit() 

/* TODO: If some other algorithm is supported, this line of code shall be updated. */
#define MAX_SIZE 872 //right now this is = sizeof(SW_AES_Ctx)

typedef struct {
	uint8 IovecTypeBuff[MAX_SIZE]; 
	boolean_t used;
} crypto_env_mem;

/**
 * @brief Initializes the memory for malloc
 *        
 *
 * @return None
 *
 * @see 
 *
 */
int crypto_env_init(void* ptr, uint32 Len);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
int crypto_env_malloc(void** ptr, uint32 ptrLen);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
int crypto_env_free(void* ptr);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
int crypto_env_memcpy(void* dst, const void* src, uint32 len);

/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
int crypto_env_memset(void* src, uint32 val, uint32 len);



/**
 * @brief 
 *        
 *
 * @return None
 *
 * @see 
 *
 */
int crypto_env_memcmp(const void* dst, const void* src, uint32 len);

int timesafe_memcmp(const void *ptr1, const void *ptr2, size_t len);

#endif /* SECRSA_ENV_H */
