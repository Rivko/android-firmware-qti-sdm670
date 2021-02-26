#ifndef AOP_QDSS_H
#define AOP_QDSS_H
/*===========================================================================
                             aop_qdss.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <string.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
/**
 * <!-- aop_qdss_init -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_init (void);

/**
 * <!-- aop_qdss_enable_ddr -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_enable_ddr (void);

/**
 * <!-- aop_qdss_disable_ddr -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_disable_ddr (void);

/**
 * <!-- aop_qdss_main -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
uint32_t aop_qdss_main (uint32 sigs);

/**
 * <!-- qdss_test -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_test (void);

/**
 * <!-- aop_qdss_add_client -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_add_client (uint32 client);

/**
 * <!-- aop_qdss_remove_client -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void aop_qdss_remove_client (uint32 client);

/**
 * <!-- aop_qdss_is_trace_enabled -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
bool aop_qdss_is_trace_enabled (void);

//===========================================================================
//                             Function Definitions
//===========================================================================


#endif // AOP_QDSS_H
