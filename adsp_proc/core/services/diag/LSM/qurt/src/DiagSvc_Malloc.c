
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Malloc Module

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.

Initialization and Sequencing Requirements

Copyright (c) 2007-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                          Edit History 
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/08   JV      Pool to allocate from or de-allocate is now based on
                   a parameter
02/29/08   JV      added support for exclusive pre-malloced buffers for
                   packet req/res
11/29/07   mad     modified to conform to coding standards
11/01/07   vs      Created

===========================================================================*/

#include "DiagSvc_Malloc.h"

diagmem gaDiagSvcMalloc[DIAGSVC_MALLOC_MAX_ITEMS];
diagmem gaDiagSvcPktMalloc[DIAGSVC_MALLOC_MAX_ITEMS];
int gnDiagSvcMalloc_Initialized = 0;

/*===========================================================================

FUNCTION DiagSvc_Malloc

DESCRIPTION
  Wrapper function for malloc. Returns a pre-malloced memory region. Does a simple
  malloc if the pre-malloced buffers have been exhausted. Uses different memory 
  pools depending on the Svc_ID. Events, logs, F3s and delayed responses are 
  allocated from one pool and responses are allocated from nother pool.

DEPENDENCIES:
  DiagSvcMalloc_Init should be called ONCE before this function is called

RETURN VALUE
  Same as malloc

===========================================================================*/

void *DiagSvc_Malloc(size_t size, int Svc_ID)
{
	int i = 0;
	switch(Svc_ID)
	{
	case(GEN_SVC_ID):
		if (gnDiagSvcMalloc_Initialized) 
		{
			for (i=0; i < DIAGSVC_MALLOC_MAX_ITEMS ; i++) 
			{
				if ((gaDiagSvcMalloc[i].in_use == 0) &&
					(size <= DIAGSVC_MALLOC_ITEM_SIZE))
				{
					gaDiagSvcMalloc[i].in_use = 1;
				return gaDiagSvcMalloc[i].ptr;
				}
			}      
			/*We did not find any space in pre-allocated memory
			   call malloc directly */
			return malloc(size); 
		}
		else 
		{
			return (void *)0;
		}
		break;
	case(PKT_SVC_ID):
		if (gnDiagSvcMalloc_Initialized) 
		{
			for (i=0; i < DIAGSVC_MALLOC_MAX_ITEMS ; i++) 
			{
				if ((gaDiagSvcPktMalloc[i].in_use == 0) &&
					(size <= DIAGSVC_MALLOC_ITEM_SIZE))
				{
					gaDiagSvcPktMalloc[i].in_use = 1;
				return gaDiagSvcPktMalloc[i].ptr;
				}
			}      
			/*We did not find any space in pre-allocated memory
			   call malloc directly */
			return malloc(size); 
		}
		else 
		{
			return (void *)0;
		}
		break;
	default:
		return (void *)0;

	}
}


/*===========================================================================

FUNCTION DiagSvc_Free

DESCRIPTION
  Wrapper function for free. Frees a pre-malloced memory region, by returning
  it to the available list of buffers. This function does not invoke "free"
  for buffers that were pre-alloacted. The pool to return to is based on a
  paarmeter.

DEPENDENCIES:
  DiagSvcMalloc_Init should be called ONCE before this function is called

RETURN VALUE
  None

===========================================================================*/
void DiagSvc_Free(void *ptr, int Svc_ID) 
{
   int i=0;
   switch(Svc_ID)
	{
	case(GEN_SVC_ID):
		if (gnDiagSvcMalloc_Initialized) 
	    {
		    for (i=0; i < DIAGSVC_MALLOC_MAX_ITEMS ; i++) 
		    {
				if (gaDiagSvcMalloc[i].ptr == ptr)
				{
					gaDiagSvcMalloc[i].in_use = 0;
				return;
				}
			}      
			/* We did not find the buffer in pre-allocated memory
			   call free directly */
			free(ptr); 
	    }
		break;
   case(PKT_SVC_ID):
	   if (gnDiagSvcMalloc_Initialized) 
	   {
		   for (i=0; i < DIAGSVC_MALLOC_MAX_ITEMS ; i++) 
		   {
				if (gaDiagSvcPktMalloc[i].ptr == ptr)
				{
					gaDiagSvcPktMalloc[i].in_use = 0;
				return;
				}
			}      
			/* We did not find the buffer in pre-allocated memory
			   call free directly */
			free(ptr); 
	   }
	   break;
   }
}
/*===========================================================================

FUNCTION DiagSvc_Malloc_Init

DESCRIPTION
  Funciton to initialize the buffers used for DiagSvc_Malloc.

DEPENDENCIES:

RETURN VALUE
  TRUE if initialization is successful, FALSE otherwise

===========================================================================*/
boolean DiagSvc_Malloc_Init (void)
{
  int i=0;

  if (!gnDiagSvcMalloc_Initialized) 
  {
	  for (i=0 ; i < DIAGSVC_MALLOC_MAX_ITEMS; i++) 
      {
		  gaDiagSvcMalloc[i].ptr = malloc(DIAGSVC_MALLOC_ITEM_SIZE);
		  gaDiagSvcPktMalloc[i].ptr = malloc(DIAGSVC_MALLOC_ITEM_SIZE);
          if(!(gaDiagSvcMalloc[i].ptr) || !(gaDiagSvcPktMalloc[i].ptr))
          {
			  DiagSvc_Malloc_Exit();
              return FALSE;
          }
	      gaDiagSvcMalloc[i].in_use = 0;
		  gaDiagSvcPktMalloc[i].in_use = 0;
      }
	  gnDiagSvcMalloc_Initialized = 1;
  }
  return TRUE;

}

/*===========================================================================

FUNCTION DiagSvc_Malloc_Exit

DESCRIPTION
  Funciton to free the buffers used for DiagSvc_Malloc.

DEPENDENCIES:

RETURN VALUE
  None

===========================================================================*/
void DiagSvc_Malloc_Exit (void)
{
   int i=0;

   for (i=0 ; i < DIAGSVC_MALLOC_MAX_ITEMS; i++) 
   {
      if(gaDiagSvcMalloc[i].ptr) {
		  free(gaDiagSvcMalloc[i].ptr);
		  gaDiagSvcMalloc[i].ptr = 0;
	  }
          gaDiagSvcMalloc[i].in_use = 0;
	  if(gaDiagSvcPktMalloc[i].ptr) {
		  free(gaDiagSvcPktMalloc[i].ptr);
		  gaDiagSvcPktMalloc[i].ptr = 0;
	  }
          gaDiagSvcPktMalloc[i].in_use = 0;

   }
   gnDiagSvcMalloc_Initialized = 0;
}

