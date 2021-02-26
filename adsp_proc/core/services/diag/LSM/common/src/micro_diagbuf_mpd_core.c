/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               Diagnostics Micro DiagBuffer Core functionality

Description
  Functions used by Root PD to initialize and maintain User PD micro diagbuffers.

Copyright (c) 2017-2018 by QUALCOMM Technologies, Incorporated. 
All Rights Reserved. Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/18   sa      Do not return error if handle is already present.
01/08/18   is      Add support for pd threshold buffering mode in island mode
10/31/17   nk      Qshrink 2 support for uimage diag messages
06/08/17   gn      Created file
===========================================================================*/
#ifdef DIAG_UIMAGE_MODE

#include "diag_cfg.h"
#include "diag_qdi.h"
#include "diag_mode_v.h" /* For DIAG_TX_MODE_INVALID */
#include "diag_micro_i.h"  /* For DIAG_UIMAGE_BUFFERED_HIGH_PER_WM */

#include "micro_diagbuffer.h"
#include "micro_diagbuf_mpd.h"
#include "micro_diagbuf_mpd_type.h"

#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt_types.h"
#include "qurt_memory.h"

#include "ULogFront.h"
#include "stringl.h"

extern ULogHandle diag_err_log_handle;

qurt_mem_pool_t ipool;

/* Structure holding island_region and its virtual address which are needed during cleanup */
diag_island_mem_type diag_island_mem_region[DIAG_MAX_ISLAND_REGIONS];

/* Current index in diag_island_mem_region */
uint8 diag_island_mem_region_idx = 0;

/* This variable will be set to TRUE when all DIAG_MAX_ISLAND_REGIONS are allocated */
static boolean diag_island_regions_exhaust = FALSE;

/*===========================================================================
FUNCTION DIAG_ISLAND_POOL_ALLOC

DESCRIPTION
  Allocates memory area in QURTOS_ISLAND_POOL which is read/write accessible 
  to both the driver and the client.

PARAMETERS
 client_handle - Obtained from the current invocation function
 size          - Memory size to allocate

RETURN VALUE
  Non-zero value pointing to the allocated memory area

===========================================================================*/
void *diag_island_pool_alloc(int client_handle, unsigned size)
{
	unsigned vaddr;
	void* ret_ptr = NULL;
	static boolean pool_attached = FALSE;
	qurt_mem_region_attr_t attr;
	diag_island_mem_type* mem_region;

    if (!pool_attached) {
		/* Get the pool handle for the island pool (only needs to be done once) */
        if(0 != qurt_mem_pool_attach("QURTOS_ISLAND_POOL", &ipool))
		{
			ULogFront_RealTimePrintf(diag_err_log_handle,0,"diag_island_pool_alloc : qurt_mem_pool_attach failed");
			pool_attached = FALSE;
			return ret_ptr;
		}
        else
		{
			ULogFront_RealTimePrintf(diag_err_log_handle,0,"diag_island_pool_alloc : qurt_mem_pool_attach success");
			pool_attached = TRUE;
		}
    }
	
	if(diag_island_regions_exhaust == TRUE)
	{
		ULogFront_RealTimePrintf(diag_err_log_handle,0,"diag_island_pool_alloc : Ran out of memory regions");
		return ret_ptr;
	}
	
	mem_region = &diag_island_mem_region[diag_island_mem_region_idx];

    qurt_mem_region_attr_init(&attr);
	/* Set the client handle as owner (gives R/W access to the client) */
    qurt_mem_region_attr_set_owner(&attr, client_handle);
    
	/* Create memory region of size requested from the island pool  */
	if(0 != qurt_mem_region_create(&mem_region->island_region, size, ipool, &attr))
	{
		ULogFront_RealTimePrintf(diag_err_log_handle,1,"diag_island_pool_alloc : qurt_mem_region_create failed for size %d", size);
		return ret_ptr;
	}
	
	/* Query the actual attributes of the region */
    if(0 != qurt_mem_region_attr_get(mem_region->island_region, &attr))
	{
		ULogFront_RealTimePrintf(diag_err_log_handle,1,"diag_island_pool_alloc : qurt_mem_region_attr_get failed for size %d", size);
		return ret_ptr;
	}
    
	/* Extract the virtual address of the allocated region */
	qurt_mem_region_attr_get_virtaddr(&attr, &vaddr);
    mem_region->island_shared_mem = (void *)vaddr;
	
	/* Save the client handle and allocated size in the structure */
	mem_region->client_handle = client_handle;
	mem_region->size = size;
	
	/* Increment the index for diag_island_mem_region array */
	diag_island_mem_region_idx++;
	
	if(diag_island_mem_region_idx >= DIAG_MAX_ISLAND_REGIONS)
	{
		diag_island_regions_exhaust = TRUE; 
	}
	
	/* Return the virtual address of the memory allocated */
	ret_ptr = mem_region->island_shared_mem;
	ULogFront_RealTimePrintf(diag_err_log_handle,3,"diag_island_pool_alloc : Memory of size %d allocated for client_handle %d with address %p", size, client_handle, ret_ptr);
	
	return(ret_ptr);
}

/*===========================================================================
FUNCTION DIAG_ISLAND_POOL_FREE

DESCRIPTION
  Free memory area allocated to the client from QURTOS_ISLAND_POOL

PARAMETERS
 client_handle - Obtained from the current invocation function
 ptr           - pointer to the memory

===========================================================================*/
void diag_island_pool_free(int client_handle, void* ptr)
{
	uint8 idx = 0;
	
	for(idx = 0; idx < diag_island_mem_region_idx; idx++)
	{
		if(diag_island_mem_region[idx].client_handle == client_handle && 
		   diag_island_mem_region[idx].island_shared_mem == ptr)
		   {
			   /* Free the memory region allocated for the client */
			   qurt_mem_region_delete(diag_island_mem_region[idx].island_region);
			   
			   ULogFront_RealTimePrintf(diag_err_log_handle,3,"diag_island_pool_free : Freed memory of size %d allocated for client_handle %d with address %p", diag_island_mem_region[idx].size, client_handle, ptr);
			   
			   /* Zero out diag_island_mem_region structure for the freed region */
			   diag_island_mem_region[idx].island_region     = 0;
			   diag_island_mem_region[idx].island_shared_mem = NULL;
			   diag_island_mem_region[idx].client_handle     = 0;
			   diag_island_mem_region[idx].size              = 0;
			   
			   /* Decrement the index */
			   diag_island_mem_region_idx--;
			   
			   if (idx < diag_island_mem_region_idx)
			   {
					/* Move the trailing structure elements up */
					memscpy(&diag_island_mem_region[idx],   sizeof(diag_island_mem_type)*(diag_island_mem_region_idx-idx), 
					        &diag_island_mem_region[idx+1], sizeof(diag_island_mem_type)*(diag_island_mem_region_idx-idx));				   
			   }
		   }
	}
	
	if((diag_island_regions_exhaust == TRUE) && (diag_island_mem_region_idx < DIAG_MAX_ISLAND_REGIONS))
	{
		diag_island_regions_exhaust = FALSE;
	}
	return;
}

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
void micro_diagbuf_mpd_add_pd( micro_diagbuf_mpd_type *mpd_buf_to_add )
{
  if ( mpd_buf_to_add )
  {
    q_put(&micro_diagbuf_mpd_q, q_link(mpd_buf_to_add, &(mpd_buf_to_add->link)));
  }
}

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
  FALSE - it could not find the item in queue Or the PD is not being deleted because its not killed.
===========================================================================*/
boolean micro_diagbuf_mpd_remove_pd( micro_diagbuf_mpd_type *mpd_buf_to_remove )
{

  boolean return_val = FALSE;
  /*
     check if this PD is going down (PD restart case: when PD
     node is not the current that is used for draining)
  */
  if ( MICRO_DIAGBUF_IS_PD_KILLED(mpd_buf_to_remove) )
  {
    return_val = MICRO_DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST(mpd_buf_to_remove);
  }
  return (return_val);
}

/*===========================================================================

FUNCTION DIAGBUF_MPD_INIT

DESCRIPTION
   This function allocates memory of type micro_diagbuf_mpd_type, 
   initializes it and returns pointer to this location to the
   UserPD. UserPD uses this pointer to access it's micro_diagbuf which
   is allocated in micro_diagbuf_mpd_buf_init() and pointer to it, is
   saved in micro_diagbuf_mpd_type->handle->buf. It is assumed
   that this function is operating at the privileged level to
   make the diag_island_pool_alloc call.

PARAMETERS 
   int client_handle - that is passed to diag_qdi_invoke
   
RETURN VALUE
   micro_diagbuf_mpd_type if diag_island_pool_alloc is successful
   NULL otherwise
   
===========================================================================*/
micro_diagbuf_mpd_type* micro_diagbuf_mpd_init( int client_handle )
{
  micro_diagbuf_mpd_type *rv = NULL;
  int return_val = 0;

  /* Initialize the shared micro_diagbuf_mpd_type structure */
  rv = (micro_diagbuf_mpd_type *)diag_island_pool_alloc(client_handle, sizeof(micro_diagbuf_mpd_type));

  if ( rv == NULL )
  {
  	ULogFront_RealTimePrintf(diag_err_log_handle,0,"micro_diagbuf_mpd_init : Malloc for micro_diagbuf_mpd_type structure failed ");
    return (rv);
  }

  ULogFront_RealTimePrintf(diag_err_log_handle,1,"micro_diagbuf_mpd_init : Alloc successful for micro_diagbuf structure : %p",rv);
  
  //initialize all counters to zero
  rv->buffer_location   = DIAG_USERPD;
  rv->buffer_type       = DIAG_DIAGBUFFER;
  rv->handle            = NULL;
  rv->is_in_use         = FALSE;
  rv->is_pd_killed      = FALSE;
   
  /* Initialize default tx mode setings */
  rv->tx_mode.prev_mode = (uint8)DIAG_TX_MODE_INVALID;
  rv->tx_mode.mode = (uint8)DIAG_TX_MODE_BUFFERED_CIR;
  rv->tx_mode.drain = FALSE;
  rv->tx_mode.buffered_many_per_mark = DIAG_UIMAGE_BUFFERED_HIGH_PER_WM;
  
  /* Calculate actual bytes wm after handle and buffer created successfully */
  rv->tx_mode.buffered_many_bytes_mark = 0;
        
  //Initialize micro_diagbuf mutex
  rv->micro_diagbuf_mpd_buf_cs.name = "MUTEX_MICRO_DIAGBUF_MPD_CS";
  return_val = osal_init_mutex(&rv->micro_diagbuf_mpd_buf_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  return (rv);
} /* diagbuf_mpd_init */

/*===========================================================================

FUNCTION MICRO_DIAGBUF_MPD_BUF_INIT

DESCRIPTION
   This function allocates and initializes a micro_diagbuf to be used
   by UserPD. It copies the allocated pointer to the UserPD's object 
   passed in param *ptr. This allows UserPD to access this pointer for 
   writing diag traffic to this buffer which is "it's diagbuf" with 
   shared access to Guest OS. This function returns allocated pointer 
   to the caller. It is assumed that this function is operating at the
   privileged level to make the diag_island_pool_alloc call.

PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   micro_diagbuf_mpd_type *ptr - pointer to UserPD object
   int buf_len           - length of the diagbuf requested by
                           a UserPD
   
RETURN VALUE
   uint8* pointer to buffer allocated
   NULL otherwise
   
===========================================================================*/
char* micro_diagbuf_mpd_buf_init( int client_handle, micro_diagbuf_mpd_type *ptr, int buf_len )
{
  char *rv = NULL;

  if ( ptr )
  {
    /* Allocate the buffer for the User PD */
    rv = (char *)diag_island_pool_alloc(client_handle, buf_len);

    if ( rv == NULL )
    {
      ULogFront_RealTimePrintf(diag_err_log_handle,0,"micro_diagbuf_mpd_buf_init : malloc failed for the buffer size ");
      return (rv);
    }
  }
  else
  {
    ULogFront_RealTimePrintf(diag_err_log_handle,0,"micro_diagbuf_mpd_buf_init : ptr is NULL ");
  }
  return (rv);
} /* diagbuf_mpd_buf_init */

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
micro_diagbuffer_result micro_diagbuf_mpd_handle_init( int client_handle, micro_diagbuf_mpd_type *ptr, char * handle )
{
  micro_diagbuffer_result ret_val = MICRO_DIAGBUFFER_ERROR;

  if ( handle )
    {
		/* Initialized the micro_diagbuffer with the shared buffer obtained */
     ret_val = micro_diagbuffer_create( (micro_ULogHandle *)(&ptr->handle), "diag_upd_uimag_buffer", handle, NULL, UIMAGE_UPD_DIAGBUFFER_SIZE_FINAL, MICRO_DIAGBUFFER_LOCKABLE);
     if((ret_val == MICRO_DIAGBUFFER_SUCCESS) || (ret_val == MICRO_DIAGBUFFER_ERR_ALREADYCREATED))
	 {
	   ULogFront_RealTimePrintf(diag_err_log_handle,1,"micro_diagbuf_mpd_handle_init : Buffer allocated and Handle created for USER PD, ret_val=%d", ret_val);

       /* Initialize default bytes wm after handle is created successfully */
       ptr->tx_mode.buffered_many_bytes_mark = ((float)ptr->tx_mode.buffered_many_per_mark / 100) * (ptr->handle->bufSize); 
    }
	else
	{
	   ULogFront_RealTimePrintf(diag_err_log_handle,1,"micro_diagbuf_mpd_handle_init : Handle creation Failed for USER PD, return_value = %d", ret_val);
	}
  }
  else
  {
    ULogFront_RealTimePrintf(diag_err_log_handle,0,"micro_diagbuf_mpd_handle_init : ptr is NULL ");
  }
  return (ret_val);
} /* diagbuf_mpd_buf_init */

/*===========================================================================

FUNCTION MICRO_DIAGBUF_MPD_BUF_DEINIT

DESCRIPTION
   This function cleans up the diag allocation buffer at
   startup.
 
PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   micro_diagbuf_mpd_type *ptr - pointer to UserPD object
   
RETURN VALUE
   None
 
===========================================================================*/
boolean micro_diagbuf_mpd_deinit( int client_handle, micro_diagbuf_mpd_type *buf )
{
  int return_val;
  boolean status;

  ULogFront_RealTimePrintf(diag_err_log_handle,0,"micro_diagbuf_mpd_deinit : ENTRY ");

  //Delete the mutex that was previously used
  if ( buf )
  {
    return_val = osal_delete_mutex(&buf->micro_diagbuf_mpd_buf_cs);
    ASSERT(OSAL_SUCCESS == return_val);

    if ( buf->handle)
    {
      diag_island_pool_free(client_handle, buf->handle);
	  buf->handle = NULL;
    }
	diag_island_pool_free(client_handle, buf);
	buf = NULL;
	status = TRUE;
  }
  else
  {
	status = FALSE;
  }
  return status;
} /* micro_diagbuf_mpd_deinit */

extern osal_mutex_arg_t micro_diagmpd_buf_list_state_mutex;
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
void micro_diagbuf_mpd_cleanup(diag_qdi_opener *diag_qdi_obj_ptr)
{
  if ( diag_qdi_obj_ptr )
  {
    micro_diagbuf_mpd_type *mpd_buf = diag_qdi_obj_ptr->user_pd_micro_node_ptr;
    if ( mpd_buf ) 
    {
      boolean buf_in_use;
	  boolean status;
      /* Mark that PD is going down */
      mpd_buf->is_pd_killed = TRUE; 

      /* Now we need to wait until the PD's diagbuf is no longer in use,
         in case the diag task is in middle of draining a packet.
      */ 
      do
      {
        osal_lock_mutex(&micro_diagmpd_buf_list_state_mutex);
        buf_in_use = mpd_buf->is_in_use;
        osal_unlock_mutex(&micro_diagmpd_buf_list_state_mutex);
	  } while ( buf_in_use ); 

      status = micro_diagbuf_mpd_remove_pd(mpd_buf);
	  
	  ULogFront_RealTimePrintf(diag_err_log_handle,1,"micro_diagbuf_mpd_cleanup : micro_diagbuf_mpd_remove_pd returned %d", status);

      /* if we have come here, drain task has drain the PD diagbuf
         to a packet boundary, and have removed it from its linked
         list. Now it is safe to free the memory. 
      */
      status = micro_diagbuf_mpd_deinit(diag_qdi_obj_ptr->qdi_handle, diag_qdi_obj_ptr->user_pd_micro_node_ptr); 
	  ULogFront_RealTimePrintf(diag_err_log_handle,1,"micro_diagbuf_mpd_cleanup : micro_diagbuf_mpd_deinit returned %d", status);
 
      diag_qdi_obj_ptr->user_pd_micro_node_ptr = NULL;
    }
  }
} /* micro_diagbuf_mpd_cleanup */

#endif /* DIAG_UIMAGE_MODE */
