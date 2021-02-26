/**
  @file rextls.h
  @brief Provides TLS support APIs
  
  This file provides the necessary APIs required for TLS support
  on native rex.  Using these functions once can store thread 
  specific data in the TLS region of the REX TCB. 
 */
#ifndef REX_TLS_H
#define REX_TLS_H /* #ifndef REX_TLS_H */

/** @addtogroup dxy_rextls
  @{ */

//#define FEATURE_REX_TLS
/** Feature to use for feature guarding.  */

#ifdef FEATURE_REX_TLS

#define REX_MAX_TLS_OBJECTS 16
/** Defines the maximum number of TLS objects available.  */

typedef void (*rex_tls_destructor_t)(void*);
/** TLS object destructor function pointer type.  */

/** 
  @brief A TLS entry.
 
  This structure contains the pointer to the user created 
  TLS object and a pointer to the destructor to be invoked 
  upon thread deletion. 
*/
typedef struct rex_tls_entry {
    void *value;
}rex_tls_entry_t;

typedef rex_tls_entry_t rex_tls_table_t[REX_MAX_TLS_OBJECTS];
/* TLS table type; array that stores the TLS entries */

/* Error Codes */
#define REX_TLS_SUCCESS     0
/** Generic Success  */
#define REX_TLS_ERROR_FULL  1
/** Reported when there are no more TLS keys available. */
#define REX_TLS_INVALID_KEY 2
/** This error results from out of bounds key. */
#define REX_TLS_FAILURE     3
/** Unexpected TLS failure. */

typedef uint32 rex_tls_key_bitmask_t;
/** Bitmask type. */
typedef uint32 rex_tls_key_t;
/** Key type. */
typedef void *rex_tls_value_t;
/** TLS value type. */

/**
  rex_tls_create - Creates a key to access a TLS entry.  A key 
  must be destroyed before it can be reused.  Once a key is 
  created, it is unique for its lifetime. 
 
  @param[out] key        Pointer to the key storage.
  @param[in]  func       Function pointer to destructor func.
 
  @return
  REX_TLS_SUCCESS on success, REX_TLS_ERROR_FULL if no more keys
  are available, and REX_TLS_FAILURE if an unexpected error 
  occurs. 
*/
int rex_tls_create(rex_tls_key_t *key, rex_tls_destructor_t func);

/**
  rex_tls_setspecific - Sets the value member of the TLS entry. 
 
  @param[in]  key        Key to use for TLS access.
  @param[in]  val        Value to set.
 
  @return
  REX_TLS_SUCCESS on success. REX_TLS_INVALID_KEY if the key 
  provided is bad. 
*/
int rex_tls_setspecific(rex_tls_key_t key, rex_tls_value_t val);

/**
  rex_tls_getspecific - Gets the value member of the TLS entry. 
 
  @param[in]  key        Key to use for TLS access.
  @param[out] val        Pointer to store the value.
 
  @return
  REX_TLS_SUCCESS on success.  REX_TLS_INVALID_KEY if the key 
  provided is bad. 
*/
int rex_tls_getspecific(rex_tls_key_t key, rex_tls_value_t *val);

/**
  rex_tls_destroy - Relinquishes a key, allowing it to be 
  reused.  It is up to the user to ensure that proper cleanup of 
  the value has been accomplished prior to this call. 
  rex_tls_destroy is callable from within destructor functions. 
  rex_tls_destroy will not call any destructor functions. 
 
  @param[in]  key        Key to use for TLS access.
  @param[out] val        Pointer to store the value.
 
  @return
  REX_TLS_SUCCESS on success.  REX_TLS_INVALID_KEY if the key 
  provided is bad. 
*/
int rex_tls_destroy(rex_tls_key_t key);

#endif

/** @} */ /* end_addtogroup dxy_rextls */

#endif /* #ifndef REX_TLS_H */

