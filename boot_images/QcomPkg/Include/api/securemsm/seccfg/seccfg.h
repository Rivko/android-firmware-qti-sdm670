/*===========================================================================
**  Function :  qsee_is_memory_dump_allowed
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine ram dump allowed or not
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
* @note   This function is owned by SecureMSM TrustZone team; please contact for
*         questions and modifications.
*/
boot_boolean qsee_is_memory_dump_allowed(secboot_verified_info_type *secboot_info);


/*===========================================================================
**  Function :  qsee_retail_dump_allowed
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine retail crash dump allowed or not
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
* @note   This function is owned by SecureMSM TrustZone team; please contact for
*         questions and modifications.
*/
boot_boolean qsee_is_retail_unlocked (secboot_verified_info_type *secboot_info);

/*===========================================================================
**  Function :  qsee_zero_peripheral_memory
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will determine retail crash dump allowed or not
*
* @param[in] shared_data Pointer to shared data
*        
* @par Dependencies
*   None
* 
* @note   This function is owned by SecureMSM TrustZone team; please contact for
*         questions and modifications.
*/
void qsee_zero_peripheral_memory(void);
