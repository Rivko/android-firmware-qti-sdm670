/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Legacy Service Mapping layer implementation for Packet request/response

GENERAL DESCRIPTION
  Contains main implementation of Legacy Service Mapping layer for Diagnostic Packet Req/Res Services.

EXTERNALIZED FUNCTIONS
  diagpkt_alloc
  diagpkt_subsys_alloc
  diagpkt_shorten
  diagpkt_commit
  diagpkt_get_cmd_code
  diagpkt_set_cmd_code
  diagpkt_subsys_get_id
  diagpkt_subsys_get_cmd_code
  diagpkt_err_rsp
  diagpkt_subsys_alloc_v2
  diagpkt_subsys_alloc_v2_delay 
  diagpkt_delay_commit 
  diagpkt_subsys_get_status
  diagpkt_subsys_set_status
  diagpkt_subsys_get_delayed_rsp_id
  diagpkt_subsys_reset_delayed_rsp_id
  diagpkt_subsys_set_rsp_cnt
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2007-2017 by QUALCOMM Technologies, Incorporated. All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/Diag_LSM_Pkt.c#1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
11/23/17   bs     Fix for crash during modem SSR.
05/30/17   gn     Changes to support command deregistration
10/03/16   as     Migrated mutex lock calls to qurt_pimutex_lock()
07/10/15   rh     Moved command header structs to diagpkti.h
05/09/14   rs     Moving delayed response out of diag buffer. 
11/27/13   ph     Updated MSG 1.0 API references to MSG.2.0.
07/05/12   sg     Changes to bringup Diag MultiPD
09/01/10   mad    Packet-processing synch. event is now set for all packets
06/24/10   mad    Fixes in Diag_LSM_Pkt_DeInit(): Setting gbCritSectInitialized 
                  to FALSE AND Checking for validity of ghWinDiag before IOCTL.
06/08/10   mad    Klockwork fix in diagpkt_tbl_reg() 
10/01/09   mad    Merged changes to move proc_id initialization from 
                  header to source file, to remove IMAGE_APPS_PROC feature 
                  from diagpkt.h.
07/15/09   mad    Changed struct name: diagpkt_user_table_type to 
                  diagpkt_master_table_type, to merge API header diagpkt.h
07/07/09   mad    Removed inclusion of AEEStdDef.h,queue.h and diagcmd.h
07/07/09   JV     Included err.h
04/24/09   mad    Changes to make delayed response mechanism functional.
03/23/09   mad    De-featurized inclusion of windows header files
01/15/09   mad    Synch. Event signals diag thread reg. processing
                  packet from common table(e.g. mask change)
12/03/08   mad    Changes to take out Component services, use WM native 
                  Critical section and to use filesystem calls to access 
                  windiag driver.
10/31/08   mad   Fix for crash when Diag_LSM_Init()-DeInit() is called multiple 
                 times. Reset global handles etc explicitly to NULL, in 
                 Diag_LSM_Pkt_DeInit(), so they get initialized correctly the 
                 next  time.
09/04/08   mad   Addded call to release Critical section before return due to 
                 malloc failure, in function diagpkt_tbl_reg
04/14/08   JV    Added support to pass a pointer to a locally created object as
                 an argument to IDiagPkt_BindPkt()
04/14/08   JV    Replaced KxMutex lock and unlock with ICritSect enter and leave
02/27/08   JV    Created      

===========================================================================*/
                     
                      
/* ==========================================================================
   Include Files
========================================================================== */

#include "msg.h" /* For MSG_* macros */
#include "comdef.h"
#include "stringl.h"
#include "diagpkt.h"
#include "Diag_LSMi.h"
#include "DiagSvc_Malloc.h"
#include "diagpkti.h" /* header-file re-factoring needs to be done after interim release */
#include "diagdiag.h" /* For DIAG_BAD_CMD_F_rsp_type */
#include "Diag_LSM_Pkt_i.h"
#include "diagi.h"
#include "diag_shared_i.h" /* data structures for registration */
#include "err.h"
#include "qurt_qdi.h"
#include "diagstub.h"
#include "qurt.h"
#include "Diag_LSM_Qdi_wrapper.h"
/*Local Function declarations*/
static void diagpkt_user_tbl_init (void);
uint32 diagpkt_with_delay (diagpkt_subsys_id_type id,
                           diagpkt_subsys_cmd_code_type code);

PACKED void * diagpkt_delay_alloc(
  diagpkt_cmd_code_type code,
  unsigned int length);

void diagpkt_request_response_handler(const unsigned char* packet,
        uint16 packetLen, unsigned char* response, int responseLen, int* responseLenReq);

/*this keeps track of number of failures to WriteFile().
This will currently be used only internally.*/
static unsigned int gPkt_commit_fail = 0;

#define DIAGPKT_HDR_PATTERN (0xDEADD00DU)
#define DIAGPKT_OVERRUN_PATTERN (0xDEADU)
#define DIAGPKT_USER_TBL_SIZE 128


/* This is the user dispatch table. */
static diagpkt_master_table_type *diagpkt_user_table[DIAGPKT_USER_TBL_SIZE];

#define DIAGPKT_PKT2LSMITEM(p) \
  ((diagpkt_lsm_rsp_type *) (((byte *) p) - FPOS (diagpkt_lsm_rsp_type, rsp.pkt)))

#define DIAGPKT_PKT2DELAYLSMITEM(p) \
  ((diagpkt_lsm_delay_rsp_type *) (((byte *) p) - FPOS (diagpkt_lsm_delay_rsp_type, pkt)))

qurt_mutex_t diagpkt_user_table_mutex;

static unsigned int diagpkt_user_tbl_init_begin = FALSE;
static unsigned int diagpkt_user_tbl_initialized = FALSE;

/* HANDLE to Event that needs to be set when 
Common packets(e.g. mask change request) are processed */
//extern HANDLE ghDiag_Pkt_Process_Event;

/*=============================================================================
FUNCTION DIAGPKT_GET_DELAYED_RSP_ID_LSM

DESCRIPTION
Internal function.
IOCTL into the diag driver, and get the next delayed response id in the system

DEPENDENCIES
Windiag driver should be initialized.
=============================================================================*/

boolean diagpkt_get_delayed_rsp_id_lsm(uint16* pRsp_ID)
{
   boolean success = FALSE;
   if((diag_qdi_handle >= DIAG_FD_MIN_VALUE) && pRsp_ID)
   {
      dword NumberOfBytes = 0;
     if(
         (diag_lsm_ioctl(DIAG_IOCTL_GET_DELAYED_RSP_ID,NULL,0,(byte*)pRsp_ID, FSIZ(diagpkt_subsys_hdr_type_v2,delayed_rsp_id), &NumberOfBytes))
         && (NumberOfBytes == FSIZ(diagpkt_subsys_hdr_type_v2,delayed_rsp_id))
         )
	   {
         success = TRUE;
      }
     
   }
   return success;
}
/*===========================================================================

FUNCTION DIAGPKT_WITH_DELAY

DESCRIPTION
  This procedure checks if the diagnostic packet has been registered with or
  without delay.

DEPENDENCIES
  None.

RETURN VALUE
  Return value is 0 if Diag packet has no delayed response and 1 if Diag 
      packet has delayed response

SIDE EFFECTS
  None



===========================================================================*/
uint32 diagpkt_with_delay (diagpkt_subsys_id_type id,
                           diagpkt_subsys_cmd_code_type code)
{
	uint16 packet_id = code;      /* Command code for std or subsystem */
    uint8 subsys_id = id;
    const diagpkt_master_table_type *user_tbl_entry = NULL;
    const diagpkt_user_table_entry_type *tbl_entry = NULL;
    int tbl_entry_count = 0;
    int i, j;
    boolean found = FALSE;
    uint32 delay_flag = 0;

  /* Search the dispatch table for a matching subsystem ID.  If the
     subsystem ID matches, search that table for an entry for the given
     command code. */
    for (i = 0; !found && i < DIAGPKT_USER_TBL_SIZE; i++)
    {
		user_tbl_entry = diagpkt_user_table[i];
		if (user_tbl_entry != NULL && user_tbl_entry->subsysid == subsys_id)
        {
			tbl_entry = user_tbl_entry->user_table;
			delay_flag = user_tbl_entry->delay_flag;
			tbl_entry_count = (tbl_entry) ? user_tbl_entry->count : 0;

            for (j = 0; (tbl_entry!=NULL) && !found && j < tbl_entry_count; j++)
            {
				if (packet_id >= tbl_entry->cmd_code_lo && 
                       packet_id <= tbl_entry->cmd_code_hi)
                {
                /* If the entry has no func, ignore it. */
                    found = TRUE;
                }
                tbl_entry++;
            }
        } /* endif if (user_tbl_entry != NULL && user_tbl_entry->subsysid == subsys_id) */
    }
	return delay_flag;
}               /* diagpkt_with_delay */


/*===========================================================================

FUNCTION DIAGPKT_DELAY_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in a pre-malloced buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
PACKED void * diagpkt_delay_alloc(
  diagpkt_cmd_code_type code,
  unsigned int length)
{
  unsigned int size = 0;
  diagpkt_hdr_type *pkt = NULL;
  diagpkt_lsm_delay_rsp_type *item = NULL;
  PACKED uint16 *pattern = NULL;    /* Overrun pattern. */
  unsigned char *p = NULL;
  diag_data* pdiag_data = NULL;

  size =     DIAG_REST_OF_DATA_POS + FPOS(diagpkt_lsm_delay_rsp_type,pkt) +  /* Diagpkt header    */
             (uint32)length     +  /* Number of bytes requested buffer */
             sizeof(uint16) ;   /* buffer overrun pattern */

  /* We allocate from the memory pool used for events, logs and messages
  because it is a delayed response. */
  pdiag_data = (diag_data*)DiagSvc_Malloc (size, GEN_SVC_ID);

  if(pdiag_data)
  {
      pdiag_data->diag_data_type = DIAG_DATA_TYPE_DELAYED_RESPONSE;
      item = (diagpkt_lsm_delay_rsp_type *) ((byte*)(pdiag_data)+DIAG_REST_OF_DATA_POS);

      item->pattern = DIAGPKT_HDR_PATTERN;    /* Sanity check pattern */
      item->length = length;

      pattern = (PACKED uint16 *) &item->pkt[length] ;

      p = (unsigned char *) pattern;
      p[0] = (DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff;
      p[1] = (DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff;

      pkt = (diagpkt_hdr_type *)&item->pkt;
      if(pkt)
      {
          pkt->command_code = code; 
      }
  }
  return (PACKED void *)pkt;
    
} /* diagpkt_delay_alloc */


/*===========================================================================

FUNCTION DIAGPKT_LSM_PROCESS_REQUEST

DESCRIPTION
  This procedure formats a response packet in response to a request
  packet received from the Diagnostic Monitor.  Calls packet processing
  functions through a table, counts input and output types

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void
diagpkt_LSM_process_request (void *req_pkt, uint16 pkt_len,
							 diag_cmd_rsp rsp_func, void *rsp_func_param)
{
	uint16 packet_id;     /* Command code for std or subsystem */
    uint8 subsys_id = DIAGPKT_NO_SUBSYS_ID;
    const diagpkt_master_table_type *user_tbl_entry = NULL;
    const diagpkt_user_table_entry_type *tbl_entry = NULL;
    int tbl_entry_count = 0;
    int i,j;
    void *rsp_pkt = NULL;
    boolean found = FALSE;
    uint16 cmd_code = 0xFF;

    packet_id = diagpkt_get_cmd_code (req_pkt);

    if ( packet_id == DIAG_SUBSYS_CMD_VER_2_F )
    {
		cmd_code = packet_id;
    }

	if ((packet_id == DIAG_SUBSYS_CMD_F) || ( packet_id == DIAG_SUBSYS_CMD_VER_2_F ))
    {
		subsys_id = diagpkt_subsys_get_id (req_pkt);
              packet_id = diagpkt_subsys_get_cmd_code (req_pkt);
    }

  /* Search the dispatch table for a matching subsystem ID.  If the
     subsystem ID matches, search that table for an entry for the given
     command code. */

    for (i = 0; !found && i < DIAGPKT_USER_TBL_SIZE; i++)
    {
        user_tbl_entry = diagpkt_user_table[i];
        if (user_tbl_entry != NULL && user_tbl_entry->subsysid == subsys_id
             && user_tbl_entry->cmd_code == cmd_code)
        {
		  tbl_entry = user_tbl_entry->user_table;
            tbl_entry_count = (tbl_entry) ? user_tbl_entry->count : 0;

            for (j = 0; (tbl_entry!=NULL) && !found && j < tbl_entry_count; j++)
            {
		      if (packet_id >= tbl_entry->cmd_code_lo && packet_id <= tbl_entry->cmd_code_hi)
                {
                 /* If the entry has no func, ignore it. */
                     if (tbl_entry->func_ptr)
                     {
			           found = TRUE;
                       rsp_pkt = (void *) (*tbl_entry->func_ptr) (req_pkt, pkt_len);
		       diag_lsm_send_pkt_read_complete_notify();
                    
                       if (rsp_pkt)
                         {
				   /* The most common case: response is returned.  Go ahead
                             and commit it here. */
                           diagpkt_commit (rsp_pkt);
                         } /* endif if (rsp_pkt) */
                     } /* endif if (tbl_entry->func_ptr) */
                } /* endif if (packet_id >= tbl_entry->cmd_code_lo && packet_id <= tbl_entry->cmd_code_hi)*/
		      tbl_entry++;
            } /* for (j = 0; (tbl_entry!=NULL) && !found && j < tbl_entry_count; j++) */
        } /* endif if (user_tbl_entry != NULL && user_tbl_entry->subsysid == subsys_id
             && user_tbl_entry->cmd_code == cmd_code)*/
     } /*  for (i = 0; !found && i < DIAGPKT_USER_TBL_SIZE; i++) */

  /* Assume that rsp and rsp_pkt are NULL if !found */

  if (!found)
  {
   // ERR_FATAL("Diag_LSM: diagpkt_LSM_process_request: Did not find match in user table",0,0,0);
  }
  return;
}               /* diagpkt_LSM_process_request */

/* ==========================================================================
FUNCTION
DIAGPKT_USER_TBL_INIT

DESCRIPTION
  Registers the table given to the diagpkt user table

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================= */
static void
diagpkt_user_tbl_init (void)
{
    int i = 0;

    if(!qurt_atomic_compare_and_set(&diagpkt_user_tbl_init_begin, FALSE, TRUE))
       return;

    qurt_pimutex_init(&diagpkt_user_table_mutex);
    for (i = 0; i < DIAGPKT_USER_TBL_SIZE; i++)
    {
       diagpkt_user_table[i] = NULL;
    }
    diagpkt_user_tbl_initialized = TRUE;
}
/*===========================================================================
FUNCTION   Diag_LSM_Pkt_Init

DESCRIPTION
  Initializes the Packet Req/Res service.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE = failure, else TRUE

SIDE EFFECTS
  None

===========================================================================*/

boolean Diag_LSM_Pkt_Init(void)
{
   return TRUE;
} /* Diag_LSM_Pkt_Init */

/*===========================================================================

FUNCTION    Diag_LSM_Pkt_DeInit

DESCRIPTION
  De-Initialize the Diag Packet Req/Res service.

DEPENDENCIES
  None.

RETURN VALUE
  boolean: returns TRUE; currently does nothing.

SIDE EFFECTS
  None

===========================================================================*/
boolean Diag_LSM_Pkt_DeInit(void)
{
  int i = 0;
  boolean ret = TRUE;

   /* De-register from DCM. */
  if((diag_qdi_handle < DIAG_FD_MIN_VALUE)){
     ret = FALSE;
  }
  diag_lsm_ioctl(DIAG_IOCTL_COMMAND_DEREG, NULL, 0, NULL, 0, NULL);
  /*
   * If the Diag_LSM_Pkt_DeInit gets invoked during initialization
   * failure we may end up in accessing the uiinitialized mutex.
   * check for diagpkt_user_tbl_initialized flag and return if it is
   * not set.
   */

  if(diagpkt_user_tbl_initialized == FALSE)
  {
     return ret;
  }

  qurt_pimutex_lock(&diagpkt_user_table_mutex);
   /* free the entries in user table */
  for (i = 0; i < DIAGPKT_USER_TBL_SIZE; i++)
  {
    if (diagpkt_user_table[i] != NULL)
    {
      free(diagpkt_user_table[i]);
      diagpkt_user_table[i] = NULL;
    }
    else
      break;
   }
   qurt_pimutex_unlock(&diagpkt_user_table_mutex);
   return ret;
} /* Diag_LSM_Pkt_DeInit */


/* ==========================================================================*/
/* Externalized functions */

/* Do not call this function directly. Use the macros defined in diagpkt.h. */
/* ==========================================================================
FUNCTION DIAGPKT_TBL_REG

DESCRIPTION 
   Registers the table given to the diagpkt user table

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void
diagpkt_tbl_reg (const diagpkt_master_table_type * tbl_ptr)
{
   if((diag_qdi_handle >= DIAG_FD_MIN_VALUE) &&  tbl_ptr)
   {
     int i = 0;
     word num_entries = tbl_ptr->count;
     bindpkt_params_per_process *pbind_req_send = (bindpkt_params_per_process *)
                              malloc(sizeof(bindpkt_params_per_process) * num_entries);
     if(NULL != pbind_req_send)
     {
        while(diagpkt_user_tbl_initialized == FALSE)
        {
          diagpkt_user_tbl_init();
        }

        qurt_pimutex_lock(&diagpkt_user_table_mutex);

        for (i = 0; i < DIAGPKT_USER_TBL_SIZE; i++)
        {
    	  if (diagpkt_user_table[i] == NULL)
    	  {
            diagpkt_user_table[i] = (diagpkt_master_table_type *)
    			                        malloc(sizeof(diagpkt_master_table_type));
            if (NULL == diagpkt_user_table[i])
            {
       	      free (pbind_req_send);
			  qurt_pimutex_unlock(&diagpkt_user_table_mutex);
              return;
            }
            memcpy(diagpkt_user_table[i], tbl_ptr, sizeof(diagpkt_master_table_type));
            break;
           }
         }
         qurt_pimutex_unlock(&diagpkt_user_table_mutex);
         for (i = 0; i < num_entries; i++)
	 {
            pbind_req_send[i].count = num_entries;
            pbind_req_send[i].cmd_code = tbl_ptr->cmd_code;
            pbind_req_send[i].subsys_id = tbl_ptr->subsysid;
            pbind_req_send[i].cmd_code_lo = tbl_ptr->user_table[i].cmd_code_lo;
	        pbind_req_send[i].cmd_code_hi = tbl_ptr->user_table[i].cmd_code_hi;
            pbind_req_send[i].proc_id = tbl_ptr->proc_id;
	        pbind_req_send[i].event_id = 0;
            pbind_req_send[i].log_code = 0;
            pbind_req_send[i].client_id = diag_pid;
         }
          
        if(!diag_lsm_ioctl(DIAG_IOCTL_COMMAND_REG, (byte*)pbind_req_send, (sizeof(bindpkt_params_per_process)*num_entries),NULL,0,NULL))
        {
         // ASSERT(0);
        }
        free (pbind_req_send);
    } /* if(NULL != pbind_req_send) */
   } /* if(NULL != ghWinDiag && NULL != tbl_ptr) */
 
}

/* ==========================================================================*/
/* Externalized functions */

/* Do not call this function directly. Use the macros defined in diagpkt.h. */
/* ==========================================================================
FUNCTION DIAGPKT_TBL_DEREG

DESCRIPTION 
   Deregisters the table given to the diagpkt user table  
   1) Frees this entry if this is the last entry
   2) Copies last valid registration table after this entry to this location

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void
diagpkt_tbl_dereg (const diagpkt_master_table_type * tbl_ptr)
{
   if((diag_qdi_handle >= DIAG_FD_MIN_VALUE) &&  tbl_ptr)
   {
     int i = 0;
	 int k = 0;
	 boolean found = FALSE;
     bindpkt_params_per_process *pbind_req_send = (bindpkt_params_per_process *)
                              malloc(sizeof(bindpkt_params_per_process));
     if(NULL != pbind_req_send)
     {
       qurt_pimutex_lock(&diagpkt_user_table_mutex);

       for (i = 0; i < DIAGPKT_USER_TBL_SIZE; i++)
       {
         if(diagpkt_user_table[i] == NULL)
		    break;
		  
		 if((diagpkt_user_table[i]->cmd_code == tbl_ptr->cmd_code) && (diagpkt_user_table[i]->subsysid == tbl_ptr->subsysid)
           && (diagpkt_user_table[i]->user_table[0].cmd_code_lo == tbl_ptr->user_table[0].cmd_code_lo)
           && (diagpkt_user_table[i]->user_table[0].cmd_code_hi == tbl_ptr->user_table[0].cmd_code_hi))
           {
          	 found = TRUE;		
			 break;
           }
       }

	   if(found == FALSE)
	   {
		 free (pbind_req_send);
		 qurt_pimutex_unlock(&diagpkt_user_table_mutex);
         return;
	   }
			
	   for (k = (DIAGPKT_USER_TBL_SIZE-1); i < k ; k--)
       {
		 if(diagpkt_user_table[k] != NULL) 
		 {
		   memscpy(diagpkt_user_table[i], sizeof(diagpkt_master_table_type), diagpkt_user_table[k], sizeof(diagpkt_master_table_type));
		   break;
		 }
       }
		
	   free(diagpkt_user_table[k]);
	   diagpkt_user_table[k] = NULL;
		
       qurt_pimutex_unlock(&diagpkt_user_table_mutex);

	   pbind_req_send->cmd_code = tbl_ptr->cmd_code;
	   pbind_req_send->subsys_id = tbl_ptr->subsysid;
	   pbind_req_send->cmd_code_lo = tbl_ptr->user_table[0].cmd_code_lo;
	   pbind_req_send->cmd_code_hi = tbl_ptr->user_table[0].cmd_code_hi;
         
       if(!diag_lsm_ioctl(DIAG_IOCTL_COMMAND_DEREG, (byte*)pbind_req_send, sizeof(bindpkt_params_per_process),NULL,0,NULL))
       {
         // ASSERT(0);
       }
       free (pbind_req_send);
     } /* if(NULL != pbind_req_send) */
    } /* if(NULL != ghWinDiag && NULL != tbl_ptr) */
}


/* ==========================================================================
FUNCTION diagpkt_tbl_reg_append_proc

DESCRIPTION Registers the table given by the client.
============================================================================= */
void
diagpkt_tbl_reg_append_proc (diagpkt_master_table_type * tbl_ptr)
{


   if ((tbl_ptr) && (tbl_ptr->proc_id == DIAG_NO_PROC)) tbl_ptr->proc_id = DIAG_QDSP6_PROC;


   diagpkt_tbl_reg(tbl_ptr); 
}

/*===========================================================================

FUNCTION DIAGPKT_GET_CMD_CODE

DESCRIPTION
  This function returns the command code in the specified diag packet.

DEPENDENCIES
  None.

RETURN VALUE
  cmd_code

SIDE EFFECTS
  None

===========================================================================*/
diagpkt_cmd_code_type
diagpkt_get_cmd_code (PACKED void *ptr)
{
	diagpkt_cmd_code_type cmd_code = 0;
	if(ptr)
	{
		/* Diag command codes are the first byte */
        return *((diagpkt_cmd_code_type *) ptr);
	}
	return cmd_code;
}               /* diag_get_cmd_code */


/*===========================================================================

FUNCTION DIAGPKT_SET_CMD_CODE

DESCRIPTION
  This function sets the command code in the specified diag packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void
diagpkt_set_cmd_code (PACKED void *ptr, diagpkt_cmd_code_type cmd_code)
{
	if(ptr)
	{
		*((diagpkt_cmd_code_type *) ptr) = cmd_code;
	}
}               /* diagpkt_set_cmd_code */



/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_ID

DESCRIPTION
  This function returns the subsystem ID in the specified diag packet.


DEPENDENCIES
  None.

RETURN VALUE
  subsys_id. If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
  0xFF is returned.

SIDE EFFECTS
  None

===========================================================================*/
diagpkt_subsys_id_type
diagpkt_subsys_get_id (PACKED void *ptr)
{
	diagpkt_subsys_id_type id = 0;
	if (ptr)
	{
		diagpkt_subsys_hdr_type *pkt_ptr = (void *) ptr;

        if ((pkt_ptr->command_code == DIAG_SUBSYS_CMD_F) || (pkt_ptr->command_code
                      == DIAG_SUBSYS_CMD_VER_2_F)) 
        {
		    id = (diagpkt_subsys_id_type) pkt_ptr->subsys_id;
        } 
        else 
        {
		    id = 0xFF;
        }
	}
    return id;
}               /* diagpkt_subsys_get_id */

/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_CMD_CODE

DESCRIPTION
  This function returns the subsystem command code in the specified
  diag packet.

DEPENDENCIES
  None.

RETURN VALUE
  subsys_cmd_code. If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
  0xFFFF is returned.

SIDE EFFECTS
  None
===========================================================================*/
diagpkt_subsys_cmd_code_type
diagpkt_subsys_get_cmd_code (PACKED void *ptr)
{
	diagpkt_subsys_cmd_code_type code = 0;
	if(ptr)
	{
		diagpkt_subsys_hdr_type *pkt_ptr = (void *) ptr;

        if ((pkt_ptr->command_code == DIAG_SUBSYS_CMD_F) || (pkt_ptr->command_code
            == DIAG_SUBSYS_CMD_VER_2_F)) 
        {
		    code = pkt_ptr->subsys_cmd_code;
        } 
        else 
        {
            code = 0xFFFF;
		}
	}
	return code;
}               /* diagpkt_subsys_get_cmd_code */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_STATUS 

DESCRIPTION
  This function gets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet

DEPENDENCIES
  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

RETURN VALUE
  status

SIDE EFFECTS
  None

===========================================================================*/
diagpkt_subsys_status_type 
diagpkt_subsys_get_status (PACKED void *ptr) 
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  //TODO: add these back in when I have an assert header
//  ASSERT (pkt_ptr != NULL);
//  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);
  
  return pkt_ptr->status;
}               /* diagpkt_subsys_get_status */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_SET_STATUS 

DESCRIPTION
  This function sets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet.

DEPENDENCIES
  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void
diagpkt_subsys_set_status (PACKED void *ptr, diagpkt_subsys_status_type status)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

//  ASSERT (pkt_ptr != NULL);      TODO: find out where ASSERT is mapped to something useful
//  ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);      TODO: find out where ASSERT is mapped to something useful
  
  pkt_ptr->status = status;
}               /* diagpkt_subsys_set_status */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_GET_DELAYED_RSP_ID

DESCRIPTION
  This function gets the delayed response ID field in the 
  DIAG_SUBSYS_CMD_VER_2_F packet.

DEPENDENCIES
  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

RETURN VALUE
  delayed response ID

SIDE EFFECTS
  None
===========================================================================*/
diagpkt_subsys_delayed_rsp_id_type 
diagpkt_subsys_get_delayed_rsp_id (PACKED void *ptr)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  //ASSERT (pkt_ptr != NULL);
  //ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);
  
  return pkt_ptr->delayed_rsp_id;
}               /* diagpkt_subsys_get_delayed_rsp_id */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_RESET_DELAYED_RSP_ID

DESCRIPTION
  This function sets the delayed response ID to zero in the 
  DIAG_SUBSYS_CMD_VER_2_F packet. 

DEPENDENCIES
  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void 
diagpkt_subsys_reset_delayed_rsp_id (PACKED void *ptr)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;
  
  //ASSERT (pkt_ptr != NULL);
  //ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);

  pkt_ptr->delayed_rsp_id = 0;
}               /* diagpkt_subsys_reset_delayed_rsp_id */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_SET_RSP_CNT

DESCRIPTION
  This function sets the response count in the DIAG_SUBSYS_CMD_VER_2_F packet.

DEPENDENCIES
  This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
  packet.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void 
diagpkt_subsys_set_rsp_cnt (PACKED void *ptr, diagpkt_subsys_rsp_cnt rsp_cnt)
{
  diagpkt_subsys_hdr_type_v2 *pkt_ptr = (void *) ptr;

  //ASSERT (pkt_ptr != NULL);
  //ASSERT (pkt_ptr->command_code == DIAG_SUBSYS_CMD_VER_2_F);
  
  pkt_ptr->rsp_cnt = rsp_cnt;
}               /* diagpkt_subsys_set_rsp_cnt */


/*============================================================================
FUNCTION DIAGPKT_ALLOC

DESCRIPTION
  This function allocates the specified amount of space from a pre-malloced buffer.
  If space is unavailable in the pre-malloced buffer, then a malloc is done.

DEPENDENCIES
  diagpkt_commit() must be called to commit the response packet to be sent.
  Not calling diagpkt_commit() will result in a memory leak.

RETURN VALUE
  pointer to the allocated memory

SIDE EFFECTS
  None

============================================================================*/
PACKED void *
diagpkt_alloc (diagpkt_cmd_code_type code, unsigned int length)
{
    diagpkt_lsm_rsp_type *item = NULL;
    diagpkt_hdr_type *pkt = NULL;
    PACKED uint16 *pattern = NULL;    /* Overrun pattern. */
    unsigned char *p;
    diag_data* pdiag_data = NULL;
     unsigned int size = 0;

     if( diag_qdi_handle < DIAG_FD_MIN_VALUE)
      {
         return NULL;
      }
     size = DIAG_REST_OF_DATA_POS + FPOS (diagpkt_lsm_rsp_type, rsp.pkt) + length + sizeof (uint16);
   
    /*-----------------------------------------------
      Try to allocate a buffer.  Size of buffer must
      include space for overhead and CRC at the end.
    -----------------------------------------------*/
      pdiag_data = (diag_data*)DiagSvc_Malloc (size, PKT_SVC_ID);
      if(NULL == pdiag_data)
      {
         /* Alloc not successful.  Return NULL. DiagSvc_Malloc() allocates memory
	  from client's heap using a malloc call if the pre-malloced buffers are not available.
	  So if this fails, it means that the client is out of heap. */
         return NULL;
      }
      /* Fill in the fact that this is a response */
      pdiag_data->diag_data_type = DIAG_DATA_TYPE_RESPONSE;
      item = (diagpkt_lsm_rsp_type*)((byte*)(pdiag_data)+DIAG_REST_OF_DATA_POS);

    /* This pattern is written to verify pointers elsewhere in this
       service  are valid. */
    item->rsp.pattern = DIAGPKT_HDR_PATTERN;    /* Sanity check pattern */
    
    /* length ==  size unless packet is resized later */
    item->rsp.size = length;
    item->rsp.length = length;

    pattern = (PACKED uint16 *) & item->rsp.pkt[length];

    /* We need this to meet alignment requirements - MATS */
    p = (unsigned char *) pattern;
    p[0] = (DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff;
    p[1] = (DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff;

    pkt = (diagpkt_hdr_type *) & item->rsp.pkt;

    if (pkt)
    {
        pkt->command_code = code;
    }
    return (PACKED void *) pkt;

}               /* diagpkt_alloc */

/*===========================================================================

FUNCTION DIAGPKT_DELAY_COMMIT

DESCRIPTION
  This function commits the response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void diagpkt_delay_commit (PACKED void *pkt)
{
    if (pkt)
    {
       unsigned int pkt_len = 0;
       diag_data* pdiag_data = NULL;
       diagpkt_lsm_delay_rsp_type *item = DIAGPKT_PKT2DELAYLSMITEM (pkt);
        
       pdiag_data = (diag_data*)((byte*)(item) - DIAG_REST_OF_DATA_POS);

       pkt_len = DIAG_REST_OF_DATA_POS + FPOS(diagpkt_lsm_delay_rsp_type,pkt) +  /* Diagpkt delayed rsp header    */
             item->length     +  /* Number of bytes requested buffer */
             sizeof(uint16) ;   /* buffer overrun pattern */

       if (item->length > 0 && (diag_qdi_handle >= DIAG_FD_MIN_VALUE))
       {
           if(diag_lsm_write_data((byte*)pdiag_data, pkt_len) < pkt_len ) 
           {
		      gPkt_commit_fail++;
           }
       }
       DiagSvc_Free(pdiag_data, GEN_SVC_ID);
   } /* end if (pkt)*/
    return;
}               /* diagpkt_delay_commit */

/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_ALLOC

DESCRIPTION
  This function returns the command code in the specified diag packet.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to allocated memory

SIDE EFFECTS
  None

===========================================================================*/
PACKED void *
diagpkt_subsys_alloc (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code, unsigned int length)
{
  diagpkt_subsys_hdr_type *hdr = NULL;
  if( diag_qdi_handle < DIAG_FD_MIN_VALUE)
  {
     return NULL;
  }

  hdr = (diagpkt_subsys_hdr_type *) diagpkt_alloc (DIAG_SUBSYS_CMD_F, length);

  if( hdr != NULL )
  {
      hdr->subsys_id = id;
      hdr->subsys_cmd_code = code;
  }

  return (PACKED void *) hdr;

}               /* diagpkt_subsys_alloc */


/*===========================================================================
FUNCTION DIAGPKT_SUBSYS_ALLOC_V2

DESCRIPTION
  This function allocates the specified amount of space from a pre-malloced buffer.
  If space is unavailable in the pre-malloced buffer, then a malloc is done.

DEPENDENCIES
  diagpkt_commit() must be called to commit the response packet to be sent.
  Not calling diagpkt_commit() will result in a memory leak.

RETURN VALUE
  pointer to the allocated memory

SIDE EFFECTS
  None
  
============================================================================*/

PACKED void *
diagpkt_subsys_alloc_v2 (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code, unsigned int length)
{
  diagpkt_subsys_hdr_type_v2 *hdr = NULL;
   if(diag_qdi_handle < DIAG_FD_MIN_VALUE )
   {
       return NULL;
   }

  hdr = (diagpkt_subsys_hdr_type_v2 *) diagpkt_alloc (DIAG_SUBSYS_CMD_VER_2_F, length);

  if (hdr != NULL)
  {
      hdr->subsys_id = id;
      hdr->subsys_cmd_code = code;
      hdr->status = 0;

      if( diagpkt_with_delay (id,code) )
      {
         /* IOCTL into the diag driver, to get the next delayed response id in the system */
         if(!diagpkt_get_delayed_rsp_id_lsm(&(hdr->delayed_rsp_id)))
         {
            /* IOCTL failed, free the allocated memory and return. */
            diagpkt_lsm_rsp_type *item = DIAGPKT_PKT2LSMITEM (hdr);
            diag_data* pdiag_data = (diag_data*)((byte*)(item) - DIAG_REST_OF_DATA_POS);
            DiagSvc_Free(pdiag_data,PKT_SVC_ID);
            return NULL;
         }
      }
	  else
      {
          hdr->delayed_rsp_id = 0;
      }
      hdr->rsp_cnt = 0;
  }
  return (PACKED void *) hdr;

}               /* diagpkt_subsys_alloc_v2 */


/*===========================================================================

FUNCTION DIAGPKT_SUBSYS_ALLOC_V2_DELAY

DESCRIPTION
  This function allocates the specified amount of space from a pre-malloced buffer.
  If space is unavailable in the pre-malloced buffer, then a malloc is done.This
  function is used to send a delayed response.This response has same priority as
  F3 messages and logs.

DEPENDENCIES
  diagpkt_delay_commit() must be called to commit the response packet to be 
  sent. Not calling diagpkt_delay_commit() will result in a memory leak and 
  response packet will not be sent.

  Note:User is required to provide delayed response id as an argument.
       This helps tools to match the delayed response with the original 
       request response pair.

RETURN VALUE
  pointer to the allocated memory

SIDE EFFECTS
  None

===========================================================================*/
PACKED void *
diagpkt_subsys_alloc_v2_delay (diagpkt_subsys_id_type id,
              diagpkt_subsys_cmd_code_type code,
              diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id_arg,              
              unsigned int length)
{
  diagpkt_subsys_hdr_type_v2 *hdr = NULL;
   if(diag_qdi_handle < DIAG_FD_MIN_VALUE)
   {
       return NULL;
   }

  hdr = (diagpkt_subsys_hdr_type_v2 *) diagpkt_delay_alloc(
                 DIAG_SUBSYS_CMD_VER_2_F, 
                 length);

  if(hdr != NULL)
  {
      hdr->subsys_id = id;
      hdr->subsys_cmd_code = code;
      hdr->status = 0;
      hdr->delayed_rsp_id = delayed_rsp_id_arg;
      hdr->rsp_cnt = 1;
  }
  return (PACKED void *) hdr;
}               /* diagpkt_subsys_alloc_v2_delay */


/*===========================================================================

FUNCTION DIAGPKT_SHORTEN

DESCRIPTION
  In legacy diag, this function was used to shorten a previously allocated
  response buffer. Now, since we use pre-malloced buffers, this function will
  not serve the purpose of freeing any memory. It just updates the length
  field with the new length.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
diagpkt_shorten (PACKED void *pkt, unsigned int new_length)
{

  diagpkt_lsm_rsp_type *item = NULL;
  PACKED uint16 *pattern = NULL;

  if (pkt)
  {
    /* Do pointer arithmetic in bytes, then case to q_type; */
      item = DIAGPKT_PKT2LSMITEM (pkt);

      if (new_length < item->rsp.size)
      {
		  unsigned char *p;
          item->rsp.length = new_length;

      /* Write the new buffer overrun detection pattern */
          pattern = (PACKED uint16 *) & item->rsp.pkt[new_length];

      /* We need this to meet alignment requirements - MATS */
          p = (unsigned char *) pattern;
          p[0] = (DIAGPKT_OVERRUN_PATTERN >> 8) & 0xff;
          p[1] = (DIAGPKT_OVERRUN_PATTERN >> 0) & 0xff;
      }
      else
      {
	return;
      }
  }
  return;
}               /* diagpkt_shorten */
/*===========================================================================

FUNCTION DIAGPKT_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diagnostics output
  buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
diagpkt_commit (PACKED void *pkt)
{
    if (pkt)
    {
       unsigned int length = 0;
       diag_data* pdiag_data = NULL;
       diagpkt_lsm_rsp_type *item = DIAGPKT_PKT2LSMITEM (pkt);
        pdiag_data = (diag_data*)((byte*)(item) - DIAG_REST_OF_DATA_POS);
       
		/* set the mobile-view stuff to NULL, for Phase B */
       item->rsp_func = NULL;
       item->rsp_func_param = NULL;
       /* end mobile-view */
       length = DIAG_REST_OF_DATA_POS + FPOS(diagpkt_lsm_rsp_type, rsp.pkt) + item->rsp.length + sizeof(uint16);
       if (item->rsp.length > 0)
       {
         if(diag_qdi_handle>=0)
         {
           if(diag_lsm_write_data( (byte*)pdiag_data, length) < length) /*TODO: Check the Numberofbyteswritten against number of bytes we wanted to write?*/
           {
	      gPkt_commit_fail++;
           }
          }
        }
        DiagSvc_Free(pdiag_data, PKT_SVC_ID);
    } /* end if (pkt)*/
    return;
}               /* diagpkt_commit */


/*===========================================================================

FUNCTION DIAGPKT_ERR_RSP

DESCRIPTION
  This function generates an error response packet.

DEPENDENCIES
  None

RETURN VALUE
  pointer to the error response

SIDE EFFECTS
  None

===========================================================================*/

PACKED void *
diagpkt_err_rsp (diagpkt_cmd_code_type code,
         PACKED void *req_pkt, uint16 req_len)
{
  DIAG_BAD_CMD_F_rsp_type *rsp;
  const unsigned int rsp_len = MIN (sizeof (DIAG_BAD_CMD_F_rsp_type),
               req_len + FPOS (DIAG_BAD_CMD_F_rsp_type, pkt));
  rsp = (DIAG_BAD_CMD_F_rsp_type *) diagpkt_alloc (code, rsp_len);

  if(req_pkt) 
  {
    memcpy ((void *) rsp->pkt,
            (void *) req_pkt, 
            rsp_len - FPOS (DIAG_BAD_CMD_F_rsp_type, pkt));
  }
  else if (req_len != 0)
  {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Diag_LSM:diagpkt_err_rsp:Non-0 request length (%d) and NULL request pointer!",req_len);
  }

  return ((void *) rsp);
}               /* diagkt_err_rsp */

/*=========================================================================
FUNCTION DIAGPKT_FREE

DESCRIPTION

  This function free the packet allocated by diagpkt_alloc(), which doesn't

  need to 'commit' for sending as a response if it is merely a temporary

  processing packet.

===========================================================================*/

void

diagpkt_free(PACKED void *pkt)
{
  if (pkt)
  {
    byte *item = (byte*)DIAGPKT_PKT2LSMITEM(pkt);
    item -= DIAG_REST_OF_DATA_POS;
    DiagSvc_Free ((void *)item,PKT_SVC_ID);
  }
 return;
}
























