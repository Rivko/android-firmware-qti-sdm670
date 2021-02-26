/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     N V   L O G G I N G S E R V I C E S

GENERAL DESCRIPTION
  This file contains code related to the error log stored in the NV.
  Functions allow the caller to retrieve and store error records from
  NV and to obtain certain error status.  The error log is stored in NV
  but is maintained in a RAM based cache to allow direct read and status
  capabilities.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  err_init must be called to initialize the cache from the NV.

Copyright (c) 2009 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_nv_log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
09/30/13   rks     CR526960: replace memcpy with memscpy
06/30/09   tbg     FIle created (moved functions from err.c)

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "err.h"
#include "errlog.h"
#include "erri.h"
#include "err_types.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "nv.h"
#include "stringl.h"

#include "qube.h"
//#include "qmemory.h"
ERR_MUTEX_TYPE err_log_mutex;



/* Packets used for ERR NV logging */

typedef PACK(struct)
{
  byte xx_header;
  word log_cnt;
  word ignore_cnt;

  PACK(struct)
  {
    byte address;
    byte err_count;
    byte file_name[8];
    word line_num;
    boolean fatal;
  } err_logs[ 20 ];
} err_nv_read_rsp_type;

typedef PACK(struct)
{
  byte xx_header;
  byte rec;
} err_nv_clear_req_type;

typedef err_nv_clear_req_type err_nv_clear_rsp_type;

/* Error record retreival                     */
#define ERR_NV_READ_CMD   42   /* DO NOT CHANGE THIS VALUE */

/* Error record clear                         */
#define ERR_NV_CLEAR_CMD  43   /* DO NOT CHANGE THIS VALUE */

/* End packet related definitions */

/* The threshold of number of errors of a unique type which are allowed    */
/* in order to force reset.  Once this number is exceeded on any reported  */
/* error we force a powerdown.  This ensures that we don't recycle power   */
/* forever (and waste NVM write cycles) for an error which occurs with     */
/* alarming regularity.                                                    */

#define ERR_NV_WRITE_THRESHOLD  20

/* Maintain count of unique errors logged.  Note that this is NOT the      */
/* total errors occuring; it is the total number of unique errors in the   */
/* log, since the occurance of an error already in the error log only      */
/* bumps up the error count of that error.                                 */

LOCAL unsigned int err_logged_count = 0;

/* Maintain count of total errors ignored because error log was full.      */

LOCAL unsigned int err_ignored_count = 0;

/* Local error log cache.                                                  */

LOCAL err_log_type err_cache[ERR_MAX_LOG];

/* Specify if error should be logged to the filesystem.
*/
LOCAL boolean err_log_to_fs = TRUE;


PACKED void * err_read_diagpkt (PACKED void *req_pkt_ptr, uint16 pkt_len );
PACKED void * err_clear_diagpkt (PACKED void *req_pkt_ptr, uint16 pkt_len );

/* Specify if error services have been initialized.  Until they are        */
/* initialized any fatal error causes a reset without logging to NV.       */

LOCAL boolean err_online = FALSE;

#ifdef FEATURE_ERR_NV_LOG

#ifdef ERR_HAS_NV_LOG_PACKET
static const diagpkt_user_table_entry_type err_nvlog_tbl[] =
{
   {ERR_NV_READ_CMD, ERR_NV_READ_CMD, err_read_diagpkt},
   {ERR_NV_CLEAR_CMD, ERR_NV_CLEAR_CMD, err_clear_diagpkt}
};

#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, err_nvlog_tbl);
#endif

#endif /* ERR_HAS_NV_LOG_PACKET */


/*===========================================================================

  FUNCTION ERR_CONVERT_TO_NV_LOG

  DESCRIPTION
	This routine converts a err_log_type structure to a byte-aligned
	nv_err_log_type.

  RETURN VALUE
	Boolean indicating success.

===========================================================================*/
boolean err_convert_to_nv_log(
	err_log_type    *err_log_ptr,
	nv_err_log_type *nv_log_ptr
  )
  {
	boolean return_val = TRUE;

	if (err_log_ptr && nv_log_ptr) {

	  nv_log_ptr->address = err_log_ptr->address;
	  nv_log_ptr->err_count = err_log_ptr->err_count;

	  memscpy((void *) nv_log_ptr->file_name,
	          sizeof(nv_log_ptr->file_name),
			 (void *)err_log_ptr->file_name,
			 sizeof(nv_log_ptr->file_name));

	  nv_log_ptr->line_num = err_log_ptr->line_num;
	  nv_log_ptr->fatal = err_log_ptr->fatal;

	} else {
	  return_val = FALSE;
	}

	return return_val;
  }


  
  /*===========================================================================

  FUNCTION ERR_CONVERT_FROM_NV_LOG

  DESCRIPTION
	This routine converts a byte-aligned nv_err_log_type structure to
	err_log_type.

  RETURN VALUE
	Boolean indicating success.

  ===========================================================================*/
  boolean err_convert_from_nv_log(
	nv_err_log_type *nv_log_ptr,
	err_log_type    *err_log_ptr
  )
  {
	boolean return_val = TRUE;

	if (err_log_ptr && nv_log_ptr) {

	  err_log_ptr->address = nv_log_ptr->address;
	  err_log_ptr->err_count = nv_log_ptr->err_count;

	  memscpy((void *)err_log_ptr->file_name,
	          sizeof(err_log_ptr->file_name),
			 (void *) nv_log_ptr->file_name,
			 sizeof(err_log_ptr->file_name));

	  err_log_ptr->line_num = nv_log_ptr->line_num;
	  err_log_ptr->fatal = nv_log_ptr->fatal;

	} else {
	  return_val = FALSE;
	}

	return return_val;
  }
#endif /* FEATURE_ERR_NV_LOG */

boolean err_get_online_status ( void )
{
  return err_online;
}

void err_nv_log_init ( void )
{
  int i;
#ifdef FEATURE_ERR_NV_LOG
  nv_err_log_type nv_log;
  nv_stat_enum_type status;

  /* If NV is built then read in from NV all error logs. */

  if (nv_built()) {

    for (i = 0; i < ERR_MAX_LOG; i++) {
      err_cache[i].address = i;
      err_cache[i].err_count = 0;
      nv_log.address = i;
      nv_log.err_count = 0;

      status = nv_read_err_log(&nv_log);

      /* Check NV status, convert to cache format, and update logged count.
      */
      if (status == NV_DONE_S) {
        if (err_convert_from_nv_log(&nv_log, &err_cache[i])) {
          if (err_cache[i].err_count > 0) {
            err_logged_count++;
          }
        } else {
          TMS_MSG_LOW("Err cache conversion failure");
        }
      } else {
        TMS_MSG_LOW("Nv err read failure");
      }
    }
    /* We have been initailized and are online from now on. */
    err_online = TRUE;
  }
#else

  for (i = 0; i < ERR_MAX_LOG; i++) {
    err_cache[i].address = i;
    err_cache[i].err_count = 0;
  }
  /* We have been initailized and are online from now on. */
  err_online = TRUE;

#endif /* FEATURE_ERR_NV_LOG */

#ifdef FEATURE_QDSP6
  {
    ERR_MUTEX_INIT(&err_log_mutex);
  }
#endif

#if defined(FEATURE_ERR_NV_LOG) && defined(ERR_HAS_NV_LOG_PACKET)
#ifndef __cplusplus
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, err_nvlog_tbl);
#endif
#endif

}

/*===========================================================================

FUNCTION ERR_LOGGED

DESCRIPTION
  Return to the caller the number of unique errors logged.

DEPENDENCIES
  err_init must be called before this call, otherwise the
  value returned is zero.

RETURN VALUE
  The number of unique errors logged.

SIDE EFFECTS
  None.

===========================================================================*/

unsigned int err_logged (void)
{
  return (err_logged_count);
}


/*===========================================================================

FUNCTION ERR_IGNORED

DESCRIPTION
  Return to the caller the total number of errors ignored.

DEPENDENCIES
  err_init must be called before this call otherwise the value
  returned is zero.

RETURN VALUE
  The total number of errors ignored because NV error log was full.

SIDE EFFECTS
  None.

===========================================================================*/

unsigned int err_ignored (void)
{
  return (err_ignored_count);
}


/*===========================================================================

FUNCTION ERR_GET_LOG_EX

DESCRIPTION
  Copy to the caller's pointer a specific error record in the error
  log cache.  The error record is identified by its number, from 0 to
  ERR_MAX_LOG.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error log existed.
  ERR_NONE_S    - No such log number or operation could not be performed.

SIDE EFFECTS
  None.

===========================================================================*/

err_stat_type err_get_log_ex
(
  byte         log_num,  /* The error log number */
  err_log_type *log_ptr  /* Where to copy the log */
)
{

  /* If the log is within range and if we have initialized then  */
  /* copy the log to the caller's buffer and return call status. */

  if ((err_online) && (log_num < ERR_MAX_LOG)) {
    *log_ptr = err_cache[log_num];
    return (ERR_DONE_S);
  } else {
    return (ERR_NONE_S);
  }
} /* err_get_log_ex */


/*===========================================================================

FUNCTION ERR_CLR_LOG

DESCRIPTION
  This function clears a specific error log both in cache and in NV.
  An error log is cleared simply by clearing the error count field
  and fatal-flag.

  If 'log_num' == 0xFF, all logs will be cleared.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error record existed and cleared, or the
                  error did not exists (but OK status is still returned).
  ERR_NONE_S    - Error record out of range or clear to NV has failed.

SIDE EFFECTS
  The specified error log is cleared in NV and in cache and is now
  available to accept a new unique error.

===========================================================================*/

err_stat_type err_clr_log (
  byte  log_num
)
{
  unsigned int idx = 0;
  err_stat_type  status;    /* Function return status */

#ifdef FEATURE_ERR_NV_LOG
  err_log_type    err_save; /* Buffer to temporarily save error record */
  nv_err_log_type nv_log;
#endif /* FEATURE_ERR_NV_LOG */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (log_num == 0xFF) {
    /* Clear all logs */

    status = ERR_DONE_S;

    /* Recursively clear all error logs */
    for(idx = 0; idx < ERR_MAX_LOG; idx++) {
      if (err_clr_log((byte)idx) == ERR_NONE_S) {
        /* Report failure even if only one fails.
        */
        status = ERR_NONE_S;
      }
    }
  } else {

    /* If we are initialized and in range then check the error */
    /* record.  If it is not empty then clear the error record */
    /* cache and NV, decrement error count and return status.  */

    status = ERR_NONE_S;
    if (err_online && err_log_to_fs && (log_num < ERR_MAX_LOG)) {

      status = ERR_DONE_S;
      if (err_cache[log_num].err_count != 0) {

#ifdef FEATURE_ERR_NV_LOG
        err_save.err_count             = err_cache[log_num].err_count;
        (void) memscpy((void *)err_save.file_name,
                       ERR_FILE_NAME_LEN,
                      (void *)err_cache[log_num].file_name,
                      ERR_FILE_NAME_LEN);
        err_save.line_num              = err_cache[log_num].line_num;
        err_save.fatal                 = err_cache[log_num].fatal;
#endif /* FEATURE_ERR_NV_LOG */

        err_cache[log_num].err_count   = 0;
        (void) memset((void *)err_cache[log_num].file_name,
                      0, ERR_FILE_NAME_LEN);
        err_cache[log_num].line_num    = 0;
        err_cache[log_num].fatal       = FALSE;

#ifdef FEATURE_ERR_NV_LOG
        /* Convert cache to PACKED NV type.
        */
        (void) err_convert_to_nv_log(&err_cache[log_num], &nv_log);

        if (nv_write_err_log(&nv_log) == NV_DONE_S) {
          err_logged_count--;

        /* If we could not write successfully to NV then return */
        /* the cache record to its previous state and exit      */
        /* with a fail status.  This is done in the hope        */
        /* that the cache and NV will be in the same state.     */
        } else {
          err_cache[log_num].err_count = err_save.err_count;
          (void) memscpy((void *)err_cache[log_num].file_name,
                        ERR_FILE_NAME_LEN,
                        (void *)err_save.file_name,
                        ERR_FILE_NAME_LEN);
          err_cache[log_num].line_num  = err_save.line_num;
          err_cache[log_num].fatal     = err_save.fatal;
          status = ERR_NONE_S;
        }
#endif /* FEATURE_ERR_NV_LOG */
      }
    }
  }

  return status;
} /* err_clr_log */


/*===========================================================================

FUNCTION ERR_LOG_STORE

DESCRIPTION
  Store a new error record to cache and NV.  If there is room for the
  new record then it is stored and the error count incremented.  If
  there is no room then the record is discarded and the errors ignored
  count is incremented.  If an error log of exact same type (file code
  and line number) already exists then we just increment the error count
  of that log.

DEPENDENCIES
  This internal function assumes that the caller has checked that we
  have been initialized before it is called.

RETURN VALUE
  The return value allows the caller to determine what action should be
  taken after trying to log the error.  The returned value is a boolean:
    TRUE  - This error record was created or incremented and is below
            threshold.
    FALSE - This error record was not created nor incremented, or it has
            been incremented but it had now exceeded threshold.

SIDE EFFECTS
  NV and cache are updated if there was room for new record or if this
  was an existing record whose error count could be incremented.

===========================================================================*/
LOCAL boolean err_log_store (
  word        line,              /* Comes from __LINE__ */
  const char *file_ptr,          /* Comes from __FILE__ */
  boolean     fatal              /* TRUE if fatal error */
)
{
  boolean   found;              /* Error record found flag */
  boolean   wrote;              /* NV written OK flag */
  boolean   ignore;             /* Error record ignore flag */
  word      i, index;           /* Working variables */
  word      non_fatal_index;    /* First non_fatal log in buffer */
  byte      file_buff[ERR_FILE_NAME_LEN];  /* File name buffer */
  uint16    length;
  int16     start_idx;
#ifndef FEATURE_QDSP6
  dword     isave;              /* Saved interrupt data */
#endif
#ifdef FEATURE_ERR_NV_LOG
  nv_err_log_type nv_log;
#endif

  memset(file_buff,0,ERR_FILE_NAME_LEN);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy file name to local buffer, strip off the */
  /* file extension and replace it with nulls.     */

  /* NOTE on file_buff[] -- As defined in the ICD, an "Error Record Retrieval
   * Response Message" contains an 8 byte field for the file name.  This is
   * NOT described as a NULL terminated string.  It is just an eight byte
   * field, where long file names are "truncated to the first eight
   * characters".  As such, file_buff[] is not necessarrily NULL terminated.
   *
   * HOWEVER, as there is some external code (nv) that treats this as a string
   * and introduces a null characters at the end, file_buff[] will be
   * NULL terminated when the NV is being used (for now) so that the file name
   * comparisons are correct.
   */

  /* First work backwards to determine where the path starts (if it is present)
   * As there are only 8 chars to work with we want to leave out any path info.
   */
  length=strlen(file_ptr);
  for (start_idx=length-1; start_idx>=0; start_idx--)
  {
    if ((file_ptr[start_idx] == '\\') || (file_ptr[start_idx] == '/'))
    {
      start_idx++; /* Increment to previous character */
      break;
    }
  }

  if(start_idx<0)
  {
    /* No path found, start at 0 */
    start_idx=0;
  }

  /* Now copy the file name */
  for (i = 0; (i < ERR_FILE_NAME_LEN)
           && (start_idx < length)
           && (file_ptr[start_idx] != '\0')
           && (file_ptr[start_idx] != '.'); i++, start_idx++)
  {
    file_buff[i] = (byte) file_ptr[start_idx];
  }

  /* Fill remaining buffer with NULL chars */
  for (; i < ERR_FILE_NAME_LEN; i++) {
    file_buff[i] = '\0';
  }

  #ifdef FEATURE_ERR_NV_LOG
  /* See earlier comment -- NV code NULL terminates this so we want to
   * go ahead and NULL terminate ours as well to match. */
  file_buff[ERR_FILE_NAME_LEN-1]='\0';
  #endif

  /* If there is an existing error record with the same file name    */
  /* and line number then increment the error count of that record.  */
  /* Increment only up to saturation point (255).  Note also that    */
  /* we increment even if we pass ERR_NV_WRITE_THRESHOLD as we want  */
  /* to track number of times error has occured for this error type. */

  index           = 0x00FF;
  non_fatal_index = 0x00FF;
  found           = FALSE;
  ignore          = FALSE;

  #ifdef FEATURE_QDSP6
  ERR_MUTEX_LOCK(&err_log_mutex);
  #else
  INTLOCK_SAV(isave);
  #endif

  for (i = 0; i < ERR_MAX_LOG; i++) {

    /* If we find an empty log, record its position so we don't have to
       loop again later. */

    if (non_fatal_index == 0x00FF && err_cache[i].fatal == FALSE)
      non_fatal_index = i; /* In case we need to overwrite a non-fatal*/

    if (err_cache[i].err_count == 0) {
      if (index == 0x00FF)
        index = i;     /* Save first open entry for new log */

    }
    else if ((err_cache[i].line_num == line) &&
             (strncmp((const char *)err_cache[i].file_name,
                      (const char *)file_buff, ERR_FILE_NAME_LEN) == 0)) {
      if (err_cache[i].err_count == 255) {
        ignore = TRUE;
      } else {
        err_cache[i].err_count++;
      }
      found = TRUE;
      index = i;
      break;
    }
  }

  /* Otherwise find the first empty error record, update all the   */
  /* record entries, and increment the unique errors logged count. */
  /* If this is a fatal error, and there are no empty logs,        */
  /* overwrite a non-fatal log                                     */

  if (!found) {

    /* If fatal and no open logs, overwrite a non-fatal log */
    if ((index != 0x00FF) || (fatal && non_fatal_index != 0x00FF)) {

      /* If replacing a non-fatal err - set index */
      if (index == 0x00FF) index = non_fatal_index;

      /* Check to make sure that we are not accessing out of bound */
      if (index < ERR_MAX_LOG)
      {
        (void) memscpy((void *)err_cache[index].file_name,
                      ERR_FILE_NAME_LEN,
                      (const void *)file_buff, ERR_FILE_NAME_LEN);

        err_cache[index].line_num = line;
        err_cache[index].fatal = fatal;
        err_cache[index].err_count = 1;
        err_logged_count++;
        found = TRUE;
      }
      else
        found = FALSE;
    }
  }
  #ifdef FEATURE_QDSP6
  ERR_MUTEX_UNLOCK(&err_log_mutex);
  #else
  INTFREE_SAV(isave);
  #endif


  /* If we found the error in the log or if we found room  */
  /* for new error then write the record to NV.  If write  */
  /* failed then increment errors ignored error count and  */
  /* set status.  If there is no empty record then we just */
  /* throw away this error and we increment the count of   */
  /* errors ignored.                                       */

  wrote = TRUE;
  if ((found) && (!(ignore))) {
#ifdef FEATURE_ERR_NV_LOG
    if (TASKS_ARE_LOCKED())
    {
      TMS_MSG_ERROR("ERR failed to write to NV");
      wrote = FALSE;
    }
    else
    {
      /* Convert cache to PACKED NV type.
      */
      if (index < ERR_MAX_LOG) {
        (void)err_convert_to_nv_log(&err_cache[index], &nv_log);
      }
      if (nv_write_err_log(&nv_log) != NV_DONE_S) {
        INC_SAT(err_ignored_count);
        wrote = FALSE;
      }
    }
#endif /* FEATURE_ERR_NV_LOG */
  } else {
    INC_SAT(err_ignored_count);
  }

  /* If we found the error record or created a new one and if we wrote   */
  /* the error record successfully to NV and if the error count is less  */
  /* then the threshold, then we return TRUE.  If we could not find room */
  /* for the new record or if NV write failed or if the current record   */
  /* error count exceeds the threshold then we return FALSE.             */

  if ((found) && (wrote) && (index < ERR_MAX_LOG) &&
      (err_cache[index].err_count <= ERR_NV_WRITE_THRESHOLD)) {
    return (TRUE);
  } else {
    return (FALSE);
  }
} /* err_log_store */


/*===========================================================================

FUNCTION ERR_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR(...) macro.  It then calls
  err_log_store to store the error.  Since this is a non fatal error
  we do not bother to check if the error could be logged or not.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.

===========================================================================*/

void err_put_log (
  word   line,                /* from __LINE__ */
  const char   *file_ptr      /* From __FILE__ */
)
{
  if (err_online && err_log_to_fs) {
    (void) err_log_store(line, file_ptr, FALSE);
  }
}


/*===========================================================================

FUNCTION ERR_READ_DIAGPKT
DESCRIPTION
  This procedure processes a request to read accumulated error
  information.

============================================================================*/
PACKED void * err_read_diagpkt (
  PACKED void *req_pkt_ptr, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  byte i;            /* loop index */
  err_log_type err_log;
  err_nv_read_rsp_type *rsp_ptr;
  const unsigned int rsp_len = sizeof(err_nv_read_rsp_type);

  (void) req_pkt_ptr; /* suppress compiler warning */
  (void) pkt_len;     /* suppress compiler warning */

  rsp_ptr = (err_nv_read_rsp_type *) diagpkt_alloc(ERR_NV_READ_CMD, rsp_len);

  if (rsp_ptr == NULL)
    return rsp_ptr;

  /*------------------------------------------------------------------------
    Header ok, get the error info. Give the number accumulated and the
    number ignored, then the error records.
  ------------------------------------------------------------------------*/

  rsp_ptr->log_cnt = (word)err_logged();
  rsp_ptr->ignore_cnt = (word)err_ignored();

  for ( i = 0; i < ERR_MAX_LOG; i++ )
  {
    if( err_get_log_ex ( i, &err_log ) == ERR_DONE_S)
    {
      rsp_ptr->err_logs[i].address = err_log.address;
      rsp_ptr->err_logs[i].err_count = err_log.err_count;

      (void) memscpy((void *)rsp_ptr->err_logs[i].file_name,
                    ERR_FILE_NAME_LEN,
                    (void *)err_log.file_name,
                    ERR_FILE_NAME_LEN);

      rsp_ptr->err_logs[i].line_num = err_log.line_num;
      rsp_ptr->err_logs[i].fatal = err_log.fatal;
    }
    else
    {
	  /* If there was an error getting the log, the rsp_ptr fields
	   * still need to be set. */
      rsp_ptr->err_logs[i].address = i;
      rsp_ptr->err_logs[i].err_count = 0;
      (void) memset((void*)rsp_ptr->err_logs[i].file_name,0,ERR_FILE_NAME_LEN);
      rsp_ptr->err_logs[i].line_num = 0;
      rsp_ptr->err_logs[i].fatal = 0;
    }
  }

  return (rsp_ptr);

} /* err_read_diagpkt */



/*===========================================================================

FUNCTION ERR_CLEAR_DIAGPKT
DESCRIPTION
  This procedure processes a request to clear an error record (or all
  records).

============================================================================*/
PACKED void * err_clear_diagpkt (
  PACKED void *req_pkt_ptr, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  byte i;                     /* loop index */
  err_nv_clear_req_type *req_ptr = (err_nv_clear_req_type *) req_pkt_ptr;
  err_nv_clear_rsp_type *rsp_ptr;
  const unsigned int rsp_len = sizeof(err_nv_clear_rsp_type);

  if (req_ptr->rec > ERR_MAX_LOG && req_ptr->rec != 0xff )
  {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len) );
  }

  /*------------------------------------------------------------------------
    Header ok, get the error info. Clear the records, then send back a copy
    of the request packet.  If the record index is -1 ( 0xFF ), then
    clear the entire set of records, otherwise just do the index specified.
  ------------------------------------------------------------------------*/
  if (req_ptr->rec == 0xFF ) {
    for (i=0; i < ERR_MAX_LOG; i++ ) {
      (void) err_clr_log ( i );
    }
  } else {
    (void) err_clr_log ( req_ptr->rec );
  }

  rsp_ptr = (err_nv_clear_rsp_type *) diagpkt_alloc(ERR_NV_CLEAR_CMD, rsp_len);

  if (rsp_ptr != NULL)
  {
    *rsp_ptr = *req_ptr;
  }

  return (rsp_ptr);

} /* err_clear_diagpkt */


/*===========================================================================

FUNCTION ERR_ENABLE_FS_LOG
DESCRIPTION
  Enables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_enable_fs_log (void)
{
  err_log_to_fs = TRUE;
} /* err_enable_fs_log */


/*===========================================================================

FUNCTION ERR_DISABLE_FS_LOG
DESCRIPTION
  Disables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_disable_fs_log (void)
{
  err_log_to_fs = FALSE;
} /* err_disable_fs_log */

