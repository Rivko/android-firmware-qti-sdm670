#ifndef LOG_H
#define LOG_H
/*! 
@ingroup log_service
@file log.h
@brief 
This file describes Logging Service APIs

@details
  The logging service allows clients to send information in the form of a 
  log record to the external device that is collecting logs (i.e., QXDM).

@note  
  Important usage note:
  The logging service uses a memory management system for logging outbound 
  information.  Due to limited resources, this memory management system is a
  FIFO queueing system with no garbage collection.  Queue insertion occurs at
  the time a logging buffer is allocated, not when it is commmited.  When you
  allocate a buffer, that buffer blocks the emptying of the FIFO until that 
  log record is commmited.  Therefore, if you hold onto a buffer for a long 
  time, no other log records can be sent until you commit (or free) your 
  buffer.  If you need to accumulate data for a log record, you must 
  accumulate it in your own memory space, not the memory allocated by the 
  logging service.  When ready to send, call log_submit().


*/

/*
Copyright (c) 1991-2010, 2013, 2017 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/api/services/log.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/17   sp      Add support for microULog debug capability
04/09/17   sp      Added support for extended logs in rootPD
11/22/13   sr      Removed PACK from pointer references
09/27/13   xy      Removed code under FEATURE_WINCE 
06/17/10   mad     change in doxygen document structure, fixed doxygen warnings. 
                   Removed log_time_indicator_type.
09/30/09   as      Removed inclusion of customer.h  
08/18/09   as      Decoupled code under FEATURE_DIAG_PACKET_COUPLING
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/15/09   mad     Featurized inclusion of customer.h
07/10/09   as      Decoupled FEATURE_LOG_EXPOSED_HEADER & FEATURE_ZREX_TIME 
06/11/09   JV      Featurized inclusion of customer.h to ease merging of the 
                   Q6 branch
10/03/08   vg      Updated code to use PACK() vs. PACKED
09/18/02   lad     Created file from old version.  Content has been removed
                   from this file, leaving only the logging service API.
===========================================================================*/
#include "comdef.h"

#include "log_codes.h"

/*!
@defgroup log_service Diagnostic Logging Services

The purpose of the Logging Service is to enable clients to send information to the external device
when it is available, rather than when requested from the external device. 
  
@par
  
The external device has the ability to individually configure (enable/disable) 
any log packet in DMSS. All log packets must conform to a published format, 
identified by the log code. Each log packet must begin with a member of type 
log_hdr_type. The header contains the log code, length, and timestamp. 
The Logging Service is not reliable. If resources are insufficient, the log service 
may drop a log by returning NULL from log_alloc(). 

@par

Log Record Header Type:
   Currently, all log records structure definitions must begin with
   log_hdr_type.  This place holder is needed for the internal 
   implementation of the logging service to function properly.
@note
   
   Do not reference this header directly.  In planned future
   versions of this service, this type will be typedef void, and the header
   will be transparant to the user of this service.  Any direct reference
   to this type will not compile when this enhancement is implemented. 
   Some clients, for legacy reasons, reference the log header.  Until those
   references are cleaned up, the logging service must expose the header
   to avoid compilation failure. 

*/

/* -------------------------------------------------------------------------
   Definitions and Declarations
   ------------------------------------------------------------------------- */

/*! Log code type.  Currently this is 16 bits. */
typedef uint16 log_code_type;

/*!
@cond DOXYGEN_BLOAT
*/

#include "qw.h"

/*!
@endcond
*/

/*! log_hdr_type ,includes length, code and timestamp */
typedef PACK(struct)
{
  word len;			/*!< Specifies the length, in bytes of the
				   entry, including this header. */

  word code;	   /*!<  Specifies the log code for the entry as
				   enumerated above. Note: This is
				   specified as word to guarantee size. */

  qword ts;		   /*!< The system timestamp for the log entry. The 
				   upper 48 bits represent elapsed time since
				   6 Jan 1980 00:00:00 in 1.25 ms units. The
				   low order 16 bits represent elapsed time
				   since the last 1.25 ms tick in 1/32 chip
				   units (this 16 bit counter wraps at the
				   value 49152). */
}
log_hdr_type;

#define DIAG_ULOG_MAX_PAYLOAD_SIZE  (0x1000) /*For Ulog payload for version 2 packets*/

typedef struct{
  log_hdr_type  header;  /* log code header */
  uint32 output_format;
  uint32 output_length;
  byte buf[DIAG_ULOG_MAX_PAYLOAD_SIZE];
}LOG_DIAG_ULOG_type;

/* -------------------------------------------------------------------------
     Function Declarations
   ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif


/*!
@ingroup log_service
@brief
Allocate a buffer of size 'length' for logging data



@param[in] code                 ID of the event to be reported
@param[in] length               The specified length is the length of the entire log, 
                             	including the log header. 

@dependencies
Log services must be initialized.

@sideeffects
Since this allocation is made from a shared resource pool, log_commit() 
or log_free() must be called as soon as possible and in a timely fashion.  
This allocation system has no garbage collection.  Calling this routine
places the log buffer in a FIFO queue. If the return pointer is held for a 
significant period of time, then diag task will be blocked waiting for 
you to call log_commit().

@return
<ul>
<li>A pointer to the allocated buffer is returned on success. 
<li>If the log code is disabled or there is not enough space, NULL is returned.
</ul>
@sa
log_submit, log_commit
*/


void *
log_alloc (log_code_type code, unsigned int length);

/*!
@ingroup log_service
@brief
Allocate a buffer of size 'length' for logging data. Supports version field  

@param[in] code                 ID of the log to be reported
@param[in] length               The specified length is the length of the entire log, 
                                including the log header. 
@param[in] version              Specifies the version - currently, there are two versions 
                                0 - same as log_alloc() (for backwards compatibility)
                                1 - The outgoing log packet header will have support to
                                    include ID field to differentiate the log pkts
                                    coming from different instances of the processor.
@param[in] *parameter           Based on the version passed in, this pointer is interpreted
                                as follows-
                                version       pointer
                                0             NULL
                                1             uint32 * . The uint32 value this pointer points to,
                                              is the ID of the instance generating the
                                              log packet
                                     
 @dependencies
Log services must be initialized.

@sideeffects
Since this allocation is made from a shared resource pool, log_commit() 
or log_free() must be called as soon as possible and in a timely fashion.  
This allocation system has no garbage collection.  Calling this routine
places the log buffer in a FIFO queue. If the return pointer is held for a 
significant period of time, then diag task will be blocked waiting for 
you to call log_commit().

@return
<ul>
<li>A pointer to the allocated buffer is returned on success. 
<li>If the log code is disabled or there is not enough space, NULL is returned.
</ul>
@sa
log_submit, log_commit
*/
void *
log_alloc_ext (log_code_type code, unsigned int length, unsigned int version, void * parameter);

/*!
@ingroup log_service
@brief
This function shortens the length of a previously allocated logging buffer.
This is used when the size of the record is not known at allocation time.


@param[in,out] *log_ptr          log_ptr is the pointer to the log whose length is to be reduced.
@param[in] length               The specified length is the length of the entire log, including the log header.

@dependencies
Log services must be initialized. This must be called prior to log_commit().

@sideeffects
A hole is left in the diag output buffer until transmit time, resulting
  in a short-term waste of output buffering RAM.

@return
None

@sa
None
*/

void log_shorten (void * log_ptr, unsigned int length);

/*!
@ingroup log_service
@brief
This is a wrapper for the log_shorten() API. The two APIs may be 
used interchangeably. See log_shorten() for usage details.

@dependencies
Log services must be initialized. This must be called prior to log_commit(). 
 
@sideeffects
A hole is left in the diag output buffer until transmit time, resulting
  in a short-term waste of output buffering RAM.

@return
None

@sa
None
*/
#define log_shorten_ext(log_ptr, length) log_shorten(log_ptr,length)

/*!
@ingroup log_service
@brief
This function commits a log buffer allocated by log_alloc(). Calling this
function tells the logging service that the user is finished with the allocated buffer 
and diagnostics layer can transfer this packet over the underlying transport layer.


@param[in] *ptr          ptr points to the log which is being commited


@dependencies
'ptr' must point to the address that was returned by a prior call to log_alloc().

@sideeffects
Since this allocation is made from a shared resource pool, this must be 
called as soon as possible after a log_alloc call.  This operation is not 
intended for logs that take considerable amounts of time ( > 0.01 sec ).

@return
None

@sa
None
*/
  void log_commit (void * ptr);


/*!
@ingroup log_service
@brief
This is a wrapper for the log_commit() API. The two APIs may be 
used interchangeably. See log_commit() for usage details.

@dependencies
'ptr' must point to the address that was returned by a prior call to log_alloc().

@sideeffects
Since this allocation is made from a shared resource pool, this must be 
called as soon as possible after a log_alloc call.  This operation is not 
intended for logs that take considerable amounts of time ( > 0.01 sec ).

@return
None

@sa
None
*/
#define log_commit_ext(ptr) log_commit(ptr)
/*!
@ingroup log_service
@brief
This function frees an allocated log packet without committing it to be sent.

@param[in] *ptr          ptr points to the log which is to be freed.


@dependencies
''ptr' must point to a log entry that was allocated by log_alloc().

@sideeffects

@return
None
@sa
log_alloc, log_submit
*/

void log_free (void * ptr);

/*!
@ingroup log_service
@brief
This is a wrapper for the log_free() API. The two APIs may be 
used interchangeably. See log_free() for usage details.

@dependencies
''ptr' must point to a log entry that was allocated by log_alloc().

@sideeffects

@return
None
@sa
log_alloc, log_submit
*/

#define log_free_ext(ptr) log_free(ptr)

/*!
@ingroup log_service
@brief
This function is called to log an accumlated log entry. If logging is
enabled for the entry by the external device, then the entry is copied 
into the diag allocation manager and commited immediately.

  This function essentially does the folliwng:
  log = log_alloc ();
  memcpy (log, ptr, log->len);
  log_commit (log);
  
  A tradeoff must be considered when using this function:
  
  Pros:
  The caller owns the memory.  Therefore, there is no timing consideration
  involved in filling the contents of that buffer.
  
  Cons:
  Header contents must be assigned by the caller prior to calling this 
  function.  Therefore, log_set code(), log_set_length(), and 
  log_set_timestamp() must be used prior to this call.
    
  (Resource cost) memory must be allocated by the caller.
  
  (Resource cost) A copy step is performed (added run-time cost).

@param[in] *ptr          ptr points to the log which is to be submitted


@dependencies

@sideeffects

@return
<ul>
<li> TRUE if log is submitted successfully into diag buffers 
<li> FALSE if there is no space left in the buffers.
</ul>

@sa
log_alloc

None
*/

  boolean log_submit (void * ptr);

/*!
@ingroup log_service
@brief
This function is called to log an accumlated log entry. If logging is
enabled for the entry by the external device, then the entry is copied 
into the diag allocation manager and commited immediately.

  This function essentially does the folliwng:
  log = log_alloc ();
  memcpy (log, ptr, log->len);
  log_commit (log);
  
  A tradeoff must be considered when using this function:
  
  Pros:
  The caller owns the memory.  Therefore, there is no timing consideration
  involved in filling the contents of that buffer.
  
  Cons:
  Header contents must be assigned by the caller prior to calling this 
  function.  Therefore, log_set code(), log_set_length(), and 
  log_set_timestamp() must be used prior to this call.
    
  (Resource cost) memory must be allocated by the caller.
  
  (Resource cost) A copy step is performed (added run-time cost).

@param[in] *ptr          ptr points to the log which is to be submitted
@param[in] version       Specifies the version - currently, there are two versions 
                         0 - same as log_submit() (for backwards compatibility)
                         1 - The outgoing log packet header will have support to
                             include ID field to differentiate the log pkts
                             coming from different instances of the processor.
@param[in] *parameter    Based on the version passed in, this pointer is interpreted
                         as follows-
                         version       pointer
                         0             NULL
                         1             uint32 * . The uint32 value this pointer points to
                                       is the ID of the instance generating the
                                       log packet.
 
@dependencies

@sideeffects

@return
<ul>
<li> TRUE if log is submitted successfully into diag buffers 
<li> FALSE if there is no space left in the buffers.
</ul>

@sa
log_alloc

None
*/

  boolean log_submit_ext (void * ptr, unsigned int version, void * parameter);
/*!
@ingroup log_service
@brief
 This function sets the length field in the given log record.

@param[in] *ptr          ptr points to the log whose length field is being set
@param[in] length       length to be set in the log header


@dependencies
''ptr' must not point to a log entry that was allocated by log_alloc().

@sideeffects
Use with caution.  It is possible to corrupt a log record using this
command.  It is intended for use only with accumulated log records, not
buffers returned by log_alloc().

@return
None

@sa
None
*/
  void log_set_length (void * ptr, unsigned int length);

/*!
@ingroup log_service
@brief
This is a wrapper for the log_set_length() API. The two APIs may be 
used interchangeably. See log_set_length() for usage details.

@param[in] *ptr          ptr points to the log whose length field is being set
@param[in] length       length to be set in the log header


@dependencies
''ptr' must not point to a log entry that was allocated by log_alloc().

@sideeffects
Use with caution.  It is possible to corrupt a log record using this
command.  It is intended for use only with accumulated log records, not
buffers returned by log_alloc().

@return
None

@sa
None
*/
#define log_set_length_ext(ptr, length) log_set_length(ptr, length)
/*!
@ingroup log_service
@brief
 This function sets the log code 

@param[in] *ptr         ptr points to the log whose logcode field is being set
@param[in] code       length to be set in the log header


@dependencies


@sideeffects

@return
None

@sa
None
*/


  void log_set_code (void * ptr, log_code_type code);

/*!
@ingroup log_service
@brief
This is a wrapper for the log_set_code() API. The two APIs may be 
used interchangeably. See log_set_code() for usage details.
 
@param[in] *ptr         ptr points to the log whose logcode field is being set
@param[in] code       length to be set in the log header 
 
@dependencies


@sideeffects

@return
None

@sa
None
*/


#define log_set_code_ext(ptr,code) log_set_code(ptr, code) 
/*!
@ingroup log_service
@brief
  This function captures the system time and stores it in the given log record.

@param[in] *ptr         ptr points to the log whose timestamp field is being set


@dependencies


@sideeffects

@return
None

@sa
None
*/

  void log_set_timestamp (
    void * ptr
    );

/*!
@ingroup log_service
@brief
This is a wrapper for the log_set_timestamp() API. The two APIs may be 
used interchangeably. See log_set_timestamp() for usage details.
 
@param[in] *ptr         ptr points to the log whose logcode field is being set
 
@dependencies


@sideeffects

@return
None

@sa
None
*/

#define log_set_timestamp_ext(ptr) log_set_timestamp(ptr)
/*!
@ingroup log_service
@brief
  This function returns the length field in the given log record.

@param[in] *ptr         ptr points to the log whose length field is being queried.


@dependencies


@sideeffects

@return
<ul>
<li> returns the length field in the given log record.
</ul>
@sa
None
*/

  unsigned int log_get_length (void * ptr);

/*!
@ingroup log_service
@brief
  This function returns the log code of the given log record.

@param[in] *ptr         ptr points to the log whose length field is being queried.


@dependencies


@sideeffects

@return
<ul>
<li> returns the log_code
</ul>

@sa
None
*/


  log_code_type log_get_code (void * ptr);

/*!
@ingroup log_service
@brief
  This function returns whether a particular code is enabled for logging.

@param[in] code     Specifies the code.


@dependencies


@sideeffects

@return
<ul>
<li> TRUE if log mask is enabled
<li> FALSE if log mask is disabled
</ul>
@sa
None
*/


  boolean log_status (log_code_type code);




/*===========================================================================

FUNCTION TYPE LOG_ON_DEMAND

DESCRIPTION
  This function, provided via reference by the caller, indicates a trigger
  for the specified log code issued from the external device.  This routine
  must return status, which is send to the external device.
  
DEPENDENCIES
  None.

RETURN VALUE
  'log_on_demand_status_enum_type'

SIDE EFFECTS
  None.

===========================================================================*/

/*!
enum for return type for log_on_demand callback function.
Specifies the status returned by device for Log On Demand Command (command-code 120). 
More details in 80-V1294-1. 
*/

typedef enum
{
  LOG_ON_DEMAND_SENT_S = 0, /*!< Logging request and operation successful */
  LOG_ON_DEMAND_ACKNOWLEDGE_S, /*!< Logging request acknowledged, success of logging unknown*/
  LOG_ON_DEMAND_DROPPED_S, /*!< Logging attempted, but log packet was dropped or disabled*/
  LOG_ON_DEMAND_NOT_SUPPORTED_S, /*!< Request unsuccessful, log code not supported for this service */
  LOG_ON_DEMAND_FAILED_ATTEMPT_S /*!< Unable to log this packet in the present context */
}
log_on_demand_status_enum_type;

/*!
function prototype for callback function for log_on_demand_register
*/
typedef log_on_demand_status_enum_type (*log_on_demand) (log_code_type
  log_code);


/*!
@ingroup log_service
@brief
  This function registers a function pointer to be associated with a 
  log code for logging on demand.  If the external device sends a request
  to trigger this log code, the function will be called.  The logging 
  must be performed by the client of this service.  It will not be 
  performed by the logging service itself.

@param[in] log_code            Specifies the log code.
@param[in] log_on_demand_ptr   Callback function registereted by client


@dependencies


@sideeffects

@return
<ul>
<li> TRUE if log_on_demand_register is successful
<li> FALSE if log_on_demand_register is not successful
</ul>

@sa
None
*/

  boolean log_on_demand_register (log_code_type log_code,
    log_on_demand log_on_demand_ptr);

/*!
@ingroup log_service
@brief
 This function unregisters the log function  

@param[in] log_code            Specifies the log code.

@dependencies


@sideeffects

@return

<ul>
<li> TRUE if log_on_demand_unregister is successful
<li> FALSE if log_on_demand_unregister is not successful
</ul>

@sa
None
*/

  boolean log_on_demand_unregister (log_code_type log_code);



/*!
@ingroup log_service

  MACRO LOG_RECORD_DEEFINE
 
  MACRO LOG_RECORD_END
 
  These macros were defined to provide and enforce naming
  conventions for declaring packets.  However, these macros
  make editor tags useless and add consufion.  The use of
  these macros has been deprecated, but is included here for
  compatibility with existing usage.  The naming convention
  enforced by these macros is not required for use of this
  service.

@note  
  It is not recommended to continue use of these macros.
  
  All that is required for defining a log structure is to place
  a member of 'log_hdr_type' at the top of the structure.  Do not
  access this member directly as this type is slated to be type-cast
  to 'void' when extending the logging service beyond 16-bit log codes.

  The naming convention enforced by these macros is outlined
  below:
  
  Log codes use the naming convention LOG_xxx_F.
  
  This macro expands the name of the defined structure to be:
  LOG_xxx_C_type

*/
#define LOG_RECORD_DEFINE( xx_code )           \
  typedef struct xx_code##_tag xx_code##_type; \
  PACK(struct) xx_code##_tag {                \
    log_hdr_type hdr;

/*!
Provides ending brace to go along with LOG_RECORD_DEFINE */
#define LOG_RECORD_END };

/*! maps to log_alloc*/
#define log_alloc_ex(a, b) log_alloc (a, b)




#ifdef __cplusplus
}
#endif
#endif				/* LOG_H */
