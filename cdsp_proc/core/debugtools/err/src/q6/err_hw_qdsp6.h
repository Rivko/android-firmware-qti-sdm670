#ifndef ERR_HW_QDSP6_H
#define ERR_HW_QDSP6_H
 
/*===========================================================================

                    Error Handling Service Internal Header File

Description
 
Copyright (c) 2012 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/q6/err_hw_qdsp6.h#1 $
 
===========================================================================*/



#ifndef ERR_HW_QDSP6
#error BAD CONFIGURATION: CHECK DEFINITIONS
#endif


// MACRO to stall execution until write has been committed to memory
#define ERR_MEMORY_BARRIER()   __asm__ __volatile__( "barrier\n" )



#endif /* ERR_HW_QDSP6_H */
