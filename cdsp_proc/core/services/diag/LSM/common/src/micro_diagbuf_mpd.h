#ifdef DIAG_UIMAGE_MODE
#ifndef MICRO_DIAGBUF_MPD_H
#define MICRO_DIAGBUF_MPD_H
/*==========================================================================

               Diagnostics Micro DiagBuffer Macros

Description
  Macros associated with micro_diagbuffer

Copyright (c) 2017-2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved. Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                          Edit History

      $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/common/src/micro_diagbuf_mpd.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/19/18   sa      Created new function for micro diagbuffer handle.
06/08/17   gn      Created file 
===========================================================================*/
#include "micro_diagbuf_mpd_type.h"
#include "diagbuf.h"
#include "diag_qdi.h"
#include "queue.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the diag output allocation manager.

===========================================================================*/

extern q_type micro_diagbuf_mpd_q;

/*===========================================================================
MACRO MICRO_DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST

DESCRIPTION
  This macro removes a PD from PD LIST that drain goes through

PARAMETERS
 micro_diagbuf_mpd_type *buf - the UserPD micro_diagbuf pointer.

RETURN VALUE
  TRUE  - If the PD is removed from the list
  FALSE - it could not find the item in queue.
===========================================================================*/
#define MICRO_DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST(current)({\
  boolean rv = TRUE; \
  \
   if (current == NULL)\
   {\
      rv = FALSE;\
   }\
   else\
   {\
      rv = q_delete_ext(&micro_diagbuf_mpd_q, &current->link);\
   }\
   (rv);\
})

/*===========================================================================
MACRO MICRO_DIAGBUF_FIND_NEXT_BUFFER

DESCRIPTION
  This macro iterate through the micro_diagbuf_mpd_type queue 
  and return the next buffer to check

PARAMETERS
 micro_diagbuf_mpd_type *buf - the UserPD micro_diagbuf pointer.

RETURN VALUE
  Pointer to next micro_diagbuf (guest OS/ user space)

===========================================================================*/
#define MICRO_DIAGBUF_FIND_NEXT_BUFFER(current) ({\
\
   micro_diagbuf_mpd_type *rv = NULL;\
\
   if (current == NULL)\
   {\
      rv = q_check(&micro_diagbuf_mpd_q);\
   }\
   else\
   {\
      rv = q_next(&micro_diagbuf_mpd_q, &current->link);\
   }\
   (rv);\
})

   
/*===========================================================================
MACRO MICRO_DIAGBUF_IS_PD_KILLED

DESCRIPTION
 This macro checks if a PD is killed.

PARAMETERS
micro_diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN VALUE
 TRUE  - If the PD is being killed
 FALSE - Otherwise
===========================================================================*/
#define MICRO_DIAGBUF_IS_PD_KILLED(current)({\
	 boolean rv = FALSE; \
	 \
	 if(current != NULL)\
	 {\
	   if (current->is_pd_killed == TRUE)\
	   {\
		rv = TRUE; \
	   }\
	 }\
	 (rv);\
   })

   
/*===========================================================================

FUNCTION DIAGBUF_MPD_INIT

DESCRIPTION
   This function allocates memory of type micro_diagbuf_mpd_type, 
   initializes it and returns pointer to this location to the
   UserPD. UserPD uses this pointer to access it's micro_diagbuf which
   is allocated in micro_diagbuf_mpd_buf_init() and pointer to it, is
   saved in micro_diagbuf_mpd_type->diagbuf_mpd_buf. It is assumed
   that this function is operating at the privileged level to
   make the Qurt_qdi_user_malloc call.

PARAMETERS 
   int client_handle - that is passed to diag_qdi_invoke
   
RETURN VALUE
   micro_diagbuf_mpd_type if qurt_qdi_user_malloc is successful
   NULL otherwise
   
===========================================================================*/
micro_diagbuf_mpd_type* micro_diagbuf_mpd_init( int client_handle );

/*===========================================================================

FUNCTION MICRO_DIAGBUF_MPD_DEINIT

DESCRIPTION
   This function cleans up the diag allocation buffer at
   startup.
 
PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   micro_diagbuf_mpd_type *ptr - pointer to UserPD object
   
RETURN VALUE
   None
 
===========================================================================*/
boolean micro_diagbuf_mpd_deinit( int client_handle, micro_diagbuf_mpd_type *buf );

/*===========================================================================

FUNCTION MICRO_DIAGBUF_MPD_BUF_INIT

DESCRIPTION
   This function allocates and initializes a diagbuf to be used
   by UserPD. It copies the allocated pointer to the UserPD's
   object passed in param *ptr. This allows UserPD to access
   this pointer for writing diag traffic to this buffer which is
   "it's micro_diagbuf" with shared access to Guest OS. This function
   returns allocated pointer to the caller.
   It is assumed that this function is operating at the
   privileged level to make the Qurt_qdi_user_malloc call.

PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   micro_diagbuf_mpd_type *ptr - pointer to UserPD object
   int buf_len           - length of the diagbuf requested by
                           a UserPD
   
RETURN VALUE
   uint8* pointer to buffer allocated
   NULL otherwise
   
===========================================================================*/
char* micro_diagbuf_mpd_buf_init( int client_handle, micro_diagbuf_mpd_type *ptr, int buf_len );

/*===========================================================================

FUNCTION MICRO_DIAGBUF_MPD_HANDLE_INIT

DESCRIPTION
   This function creates a locakable diagbuffer.

PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   micro_diagbuf_mpd_type *ptr - pointer to UserPD object
   char *handle           - ptr to the buffer ptr of size requested by user.
   
RETURN VALUE
   micro_diagbuffer_result type.

   
===========================================================================*/
micro_diagbuffer_result micro_diagbuf_mpd_handle_init( int client_handle, micro_diagbuf_mpd_type *ptr,char* handle  );

/*===========================================================================
FUNCTION MICRO_DIAGBUF_MPD_ADD_PD

DESCRIPTION
  Adds the micro_diagbuf_mpd_type pointer to the linked list that keeps track of
the user space micro_diagbufs.

PARAMETERS
 micro_diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN VALUE
  None

===========================================================================*/
void micro_diagbuf_mpd_add_pd( micro_diagbuf_mpd_type *buf );

/*=========================================================================== 
FUNCTION MICRO_DIAGBUF_MPD_REMOVE_PD

DESCRIPTION
  This function deletes a PD from PD linked list. It calls
  MICRO_DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST()

PARAMETERS
 diagbuf_mpd_type *mpd_buf_to_remove - the UserPD diagbuf
                                       pointer.

RETURN VALUE
  TRUE  - If the PD has  been successfully deleted the PD node
  FALSE - it could not find the item in queue Or the PD is not being deleted 
		  because its not killed.
===========================================================================*/
boolean micro_diagbuf_mpd_remove_pd( micro_diagbuf_mpd_type *mpd_buf_to_remove );

/*===========================================================================
FUNCTION MICRO_DIAGBUF_MPD_CLEANUP

DESCRIPTION
  Updates in_use field in the client's diagbuf, indicating that
  the buffer is not in use. 
 
PARAMETERS 
   None
   
RETURN VALUE 
   None
   
===========================================================================*/

void micro_diagbuf_mpd_cleanup(diag_qdi_opener *diag_qdi_obj_ptr);

#endif /*DIAGBUF_MPD_H*/
#endif