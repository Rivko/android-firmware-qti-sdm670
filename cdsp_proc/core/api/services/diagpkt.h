#ifndef DIAGPKT_H
#define DIAGPKT_H

/*!
@ingroup packet_service
@file diagpkt.h
@brief 
   This file describes Packet  Service APIs
@details

The purpose of the packet service is to enable clients to process inbound packets coming from
external device  and return a response packet or error response  to the external device.
All new clients are required to use the Subsystem Dispatch feature. This feature simply assigns
an 8-bit subsystem ID to a client, allowing that client to define its own 16-bit command codes.
These subsystem IDs are defined in diagcmd.h under diagpkt_subsys_cmd_enum_type 
This provides unique packet identification, eliminating the logistical issue of managing
command code values between different clients.Please refer to 80-V1294-1 document for more 
details.

@note
Diagnostic packets are sent over an external interface.  
Structure definitions must be portable to other C compilers.  The easiest
way to do this is to byte-align the packet definitions.  The ARM compiler
uses the PACK keyword to denote byte alignment.  Most other compilers 
use pragma pack(1) to enable structure packing.  The structure is not
required to be byte-aligned, but it is required to be portable to other
compilers.
  
@warning 
Each command code is part of the externalized diagnostic command
interface.  Internally within QCT, these numbers *MUST* be assigned by a
member of QCT's tools development team.  3rd party developers may use the
reserved subsystem IDs to define and grow the diagnostic packet needs.

*/

/*
Copyright (c) 1992-2011, 2013, 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
				  Qualcomm Confidential and Proprietary
*/

/* <EJECT> */

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.qdsp6/2.1.c4/api/services/diagpkt.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/30/17   gn      Changes to support command deregistration
07/23/13   rh      Increased max diag packet to 8.5k
11/22/13   sr      Removed PACK from pointer references
09/27/13   xy      Removed code under FEATURE_WINCE
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap. 
04/10/11   vs      Max RX buffer size set to 4k.
10/04/10   is      Update DIAGPKT_DISPATCH_TABLE_REGISTER_*() macros with correct parameters
09/14/10   is      New control channel and central routing features 
05/22/10   sg      Doxygenated the file 
01/20/09   vs      Increased max diag packet to 6k
09/30/09   as      Removed inclusion of customer.h 
09/28/09   vs      Removed feature IMAGE_APPS_PROC, moved to diagpkt.c
09/09/09   as      Moved DIAG packet TX and RX Max size macros from diagtune.h
08/18/09   as      Decoupled code under FEATURE_DIAG_PACKET_COUPLING
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/22/09   mad     Added #pragma pack for PACKed structs
07/15/09   mad     Featurized inclusion of customer.h
07/10/09   as      Decoupled code under FEATURE_DIAG_EXPOSED_HEADER
06/11/09   JV      Featurized inclusion of customer.h to ease merging of the 
                   Q6 branch
05/07/09   vk      moved diagpkt_err_rsp and diagbuf_pkt_alloc from diag.h
03/06/09   vg      Added function diagpkt_rsp_q_init declaration
02/06/09   ps	   Changes to support wrapping of response Id count after 
                   0xFFFF & Return wrap count.
10/03/08   vg      Updated code to use PACK() vs. PACKED
12/22/06   as      Moved proc ID macros to diag.h
10/31/05   as      Fixed lint errors.
12/23/04   ec      Added function diagpkt_free() declaration
03/15/04   as      Implemented functions to access fields in delayed response 
                   header
12/16/03   as      Added new macro to support delayed response.
08/30/02   lad     Revised file for multimode diagnostics version.
05/21/01   lad     Made diagpkt headers in DIAGPKT macros opaque.
                   Added FEATURE_DIAG_PACKET_COUPLING to facililitate migration
                   to this API.
04/17/01   lad     Removed inclusion of diagtune.h.
                   Moved subsystem dispatch IDs to diagcmd.h.
04/06/01   lad     Introduced typedefs for command codes, etc.
                   Updated DIAGPKT_SUBSYS_REQ_DEFINE macros.
                   Added diagpkt_subsys_alloc().
                   Removed diagpkt_process_request() since it is not part of the
                   externalized interface.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  This file now
                   contains an API for packet processing services.  
                   No other information or coupling remains except for
                   featurized legacy code that impacts existing targets.
                   Old history comments have been removed since most of them
                   are no longer applicable.
                   Packet definitions are no longer included in this file.
                   
===========================================================================*/

#include "comdef.h"
#include "diag.h"


/*!
@defgroup packet_service Diagnostics Packet Service
The packet service enables clients to receive diagnostic commands from the external device. The
client takes appropriate action for the command, generates a response packet using the packet
service API, and returns (or commits) the response packet. All packets must return a response
packet to the external device.
*/

/* -------------------------------------------------------------------------
   Type and Packet Definition Macros
   ------------------------------------------------------------------------- */
/*! 
@ingroup packet_service
Definition for diagpkt_cmd_code_type.

*/
typedef uint8 diagpkt_cmd_code_type;

/*! 
@ingroup packet_service
Definition for  diagpkt_subsys_id_type
*/
typedef uint8 diagpkt_subsys_id_type;

/*! 
@ingroup packet_service
Definition for  diagpkt_subsys_cmd_code_type
*/
typedef uint16 diagpkt_subsys_cmd_code_type;

/*! 
@ingroup packet_service
Definition for  diagpkt_subsys_status_type
*/
typedef uint32 diagpkt_subsys_status_type;

/*! 
@ingroup packet_service
Definition for  diagpkt_subsys_delayed_rsp_id_type
*/
typedef uint16 diagpkt_subsys_delayed_rsp_id_type;

/*! 
@ingroup packet_service
Definition for  diagpkt_subsys_rsp_cnt
*/
typedef uint16 diagpkt_subsys_rsp_cnt;



/*! @ingroup packet_service
 structure to hold the header of the diagnostics packet 
*/
typedef PACK(struct)
{
  byte opaque_header;  /*!< -- Header for the diagnostic packet */
}
diagpkt_header_type;

/*! @ingroup packet_service
structure to hold the header of the susbsytem dispatch diagnostics packet  
*/
typedef PACK(struct)
{
  byte opaque_header[4]; /*!< -- header of the subsytem dispatch diagnostics packet */
}
diagpkt_subsys_header_type;

/*! @ingroup packet_service
 structure to hold the  header of the subsytem dispatch verson 2 diagnostics packet  
 */
typedef PACK(struct)
{
  byte opaque_header[12]; /*!< -- header of the subsytem dispatch verson 2 diagnostics packet */
}
diagpkt_subsys_header_v2_type;



/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif


/*! 
@ingroup packet_service

   Macro to build error packet -- Note that the commas cause the sizeof
   to be returned when this is used in an assignment, e.g.
   ret_val = ERR_PKT ( something );
*/

#define ERR_PKT(code) diagpkt_err_rsp (code, req_ptr, pkt_len)


/*! 
@ingroup packet_service

   Macro to allocate the memory for the response packet 
   @note Users of this interface fill in the command code themselves.
*/
#define diagbuf_pkt_alloc(length) diagpkt_alloc (0xFF, (unsigned int) length)

  /* Packet Handler Types */
  
/*! 
@ingroup packet_service
   An array of diagpkt_user_table_entry_type is created by the client and registered with 
   diagnostics packet service. It must be declared 'const' (preferrably 'static const').  
   The function is called when an inbound packet matches subsystem ID and
   is within the command code range specified in the table.
*/ 
  typedef struct
  {
    word cmd_code_lo; /*!<  Minimum Command code value  */
    word cmd_code_hi; /*!< Maximum Command code value   */
    void * (*func_ptr) (void * req_pkt_ptr, uint16 pkt_len); /*!< Pointer to Function to handle the packet  
                                                                          when command code is in the range of cmd_code_lo,
                                                                          cmd_code_hi  */
  }
  diagpkt_user_table_entry_type;

/*!
@ingroup packet_service
   Structure for the diagnostics packet service master table to hold the clients table entries when clients
   registers with the diagnostics packet services

@note This is an internal structure used by the macros 
*/
  typedef struct
  { 
    uint16 delay_flag;  /*!< Delay Flag 0 means no delay and 1 means with delay */
    uint16 cmd_code;    /*!< Commad Code (defined in diagcmd.h)*/
    word subsysid;      /*!< Subsystem ID if Diagnostics Subsytem Dispatch is used by client 
                         else it would be DIAGPKT_NO_SUBSYS_ID */
    word count;         /*!< Number of entries in the table*/ 
    uint16 proc_id;     /*!< Processor ID */
    uint16 is_fwded;    /*!< 0 means has not been fwded to Master Diag; 1 means has been fwded */
    int port;           /*!< Port to remote proc to route recieved CMD */
    const diagpkt_user_table_entry_type *user_table; /**< Entry to Client's Packet Dispatch Table  */
  } diagpkt_master_table_type;

/*! 
@ingroup packet_service
   @def DIAGPKT_NO_SUBSYS_ID

   DIAGPKT_NO_SUBSYS_ID is used for diagnostics packets which donot 
   use diagnostics subsystem dispatch version 2 (cmd code 128)
*/
#define DIAGPKT_NO_SUBSYS_ID 0xFF

/*! 
@ingroup packet_service

   @brief 
   This function registers the user's dispatch table with packet service

   @details
   This function is called by DIAGPKT_DISPATCH_TABLE_REGISTER, registers the
   user's dispatch table with the packet service.This is an internal function.

   @param[in] tbl_ptr  Pointer to the diagpkt_master_table_type

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None 
*/
  void diagpkt_tbl_reg (const diagpkt_master_table_type * tbl_ptr);

    /*! 
@ingroup packet_service

   @brief 
   This function deregisters the user's dispatch table with packet service

   @details
   This function is called by DIAGPKT_DISPATCH_TABLE_DEREGISTER, deregisters the
   user's dispatch table with the packet service. Clients should call the macro
   DIAGPKT_DISPATCH_TABLE_DEREGISTER and not call this function directly.

   @param[in] tbl_ptr  Pointer to the diagpkt_master_table_type

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None 
*/
  void diagpkt_tbl_dereg (const diagpkt_master_table_type * tbl_ptr);
  

/*! 
@ingroup packet_service

   @brief
   This function is called to regsiter user's dispatch table with the packet service 
   on a particular processor 

   @details
   This function is called by DIAGPKT_DISPATCH_TABLE_REGISTER_PROC internally, registers the
   user's dispatch table with the packet service

   @param[in] tbl_ptr  Pointer to the diagpkt_master_table_type

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None 
*/
  void diagpkt_tbl_reg_append_proc(diagpkt_master_table_type * tbl_ptr);

/*!
@ingroup packet_service
   @name Diagnostics Packet Dispatch Table Registration macros 
   These macros are used to register client dispatch table with the diagnostics
   packet dispatching service. 
*/

/*@{*/  /* start grouping for the packet dispatch table registers */
  /* Single processor or modem proc*/
/*! 
   This macro registers the client's dispatch table with the diagnostics packet dispatching service 
*/
  #define DIAGPKT_DISPATCH_TABLE_REGISTER(xx_subsysid, xx_entry) \
    do { \
      static diagpkt_master_table_type xx_entry##_table = { \
       0, 0xFF, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_reg_append_proc (&xx_entry##_table); \
    } while (0)
     /*lint -restore */
/*! 
   This macro deregisters the client's dispatch table with the diagnostics packet dispatching service 
*/
  #define DIAGPKT_DISPATCH_TABLE_DEREGISTER(xx_subsysid, xx_entry) \
    do { \
      static diagpkt_master_table_type xx_entry##_table = { \
       0, 0xFF, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_dereg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */

/*! 
   This macro registers the client's dispatch table with the packet dispatching service for subsystem
   dispatch version 2 (cmd code 128) 
*/
    #define DIAGPKT_DISPATCH_TABLE_REGISTER_V2(xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static diagpkt_master_table_type xx_entry##_table = { \
        0, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_reg_append_proc (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 

	 	 
/*! 
   This macro deregisters the client's dispatch table with the packet dispatching service for subsystem
   dispatch version 2 (cmd code 128) 
*/
    #define DIAGPKT_DISPATCH_TABLE_DEREGISTER_V2(xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static diagpkt_master_table_type xx_entry##_table = { \
        0, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_dereg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 
	 
	
/*!
   This macro registers the client's dispatch table with the packet dispatching service for subsystem
   dispatch version 2 (cmd code 128) with delayed response 
*/
   
  #define DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static diagpkt_master_table_type xx_entry##_table = { \
        1, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_reg_append_proc (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 
/*@}*/ /* end of the group Diagnostics Packet Dispatch Table Registration macros  */

/*!
   This macro deregisters the client's dispatch table with the packet dispatching service for subsystem
   dispatch version 2 (cmd code 128) with delayed response 
*/
  #define DIAGPKT_DISPATCH_TABLE_DEREGISTER_V2_DELAY(xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static diagpkt_master_table_type xx_entry##_table = { \
        1, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_dereg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 
	 
/*!
@ingroup packet_service
   @name  Diagnostics Packet Dispatch Table Registration on Dual Processor Target macros  
   Use these macros to register your dispatch table with the diagnostics
   packet dispatching service on a particular processor

   USAGE: For registering diag packet tables on dual processor target, use
     the following functions:
     For diag packets which are specific to APP's processor use: 
      DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_APP_PROC, DIAG_SUBSYS_XXXX,  
        XXXX_subsys_tbl_app);
     For diag packets which are common to both processor's use:
       DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_COMMON_PROC, DIAGPKT_SUBSYS_XXXX, 
        XXXX_common_tbl);
*/
/*@{*/  /* start macros grouping for the packet dispatch table to register with a particular processor  */
/*! 
   Use this macro to register client's dispatch table with the diagnostics
   packet dispatching service on a particular processor
*/
  #define DIAGPKT_DISPATCH_TABLE_REGISTER_PROC(xx_proc_id, xx_subsysid, xx_entry) \
    do { \
      static const diagpkt_master_table_type xx_entry##_table = { \
       0, 0xFF, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), xx_proc_id, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_reg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 

/*! 
   Use this macro to deregister client's dispatch table with the diagnostics
   packet dispatching service on a particular processor
*/ 
  #define DIAGPKT_DISPATCH_TABLE_DEREGISTER_PROC(xx_proc_id, xx_subsysid, xx_entry) \
    do { \
      static const diagpkt_master_table_type xx_entry##_table = { \
       0, 0xFF, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), xx_proc_id, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_dereg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 
	 
/*! 
   Use this macro to register your dispatch table with the diagnostics
   packet dispatching service version 2 on a particular processor
*/
  #define DIAGPKT_DISPATCH_TABLE_REGISTER_V2_PROC(xx_proc_id, xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static const diagpkt_master_table_type xx_entry##_table = { \
        0, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), xx_proc_id, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_reg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 

	 /*! 
   Use this macro to deregister your dispatch table with the diagnostics
   packet dispatching service version 2 on a particular processor
*/
  #define DIAGPKT_DISPATCH_TABLE_DEREGISTER_V2_PROC(xx_proc_id, xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static const diagpkt_master_table_type xx_entry##_table = { \
        0, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), xx_proc_id, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_dereg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 
	 
/*! 
   Use this macro to register your dispatch table with the diagnostics
   packet dispatching service version 2 on a particular processor
   This macro is used if the diag packet has delayed response 
*/
  #define DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY_PROC(xx_proc_id, xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static const diagpkt_master_table_type xx_entry##_table = { \
        1, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), xx_proc_id, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_reg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */

	 
	 /*! 
   Use this macro to deregister your dispatch table with the diagnostics
   packet dispatching service version 2 on a particular processor
   This macro is used if the diag packet has delayed response 
*/
  #define DIAGPKT_DISPATCH_TABLE_DEREGISTER_V2_DELAY_PROC(xx_proc_id, xx_cmdcode, xx_subsysid, xx_entry) \
    do { \
      static const diagpkt_master_table_type xx_entry##_table = { \
        1, xx_cmdcode, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), xx_proc_id, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
      }; \
       /*lint -save -e717 */ \
      diagpkt_tbl_dereg (&xx_entry##_table); \
    } while (0)
     /*lint -restore */ 
/*@}*/  /* end of Diagnostics Packet Dispatch Table Registration on Dual Processor Target macros  */
#ifdef __cplusplus
  /* C++ class used for autoregister */

  /*!
   If C++ is supported in the build, DIAGPKT_DISPATCH_AUTOREGISTER macro,
   defined in the preprocessor, instantiates DiagDispatch class with a
   'static const' constructor
  */
  class DiagDispatch
  {
  public:
    /*! DiagDispatch Constructor 
     This registers clients dispatch table with Diagnostics Packet Service 
    */
    DiagDispatch (const diagpkt_master_table_type *tbl_ptr)
    {
      diagpkt_tbl_reg(tbl_ptr);
    }
  };

/*! 
@ingroup packet_service

   @name  Diagnostics Packet Dispatch Table Registration macros for C++
   If C++ is supported in the build, this macro, defined in the
   preprocessor, instantiates a class with a 'static const' constructor.
   This class is therefore instantiated at boot, calling the constructor
   prior to the system 'main()'.  This  eliminates the need to acquire
   scope to register, it is done automatically. 
*/
/*@{*/ /* start grouping of  DIAGPKT_DISPATCH_AUTOREGISTER macros */

/*!
   If C++ is supported in the build,this macro is used to register 
   client's dispatch table with diagnostics packet dispatch service 
*/

  #define DIAGPKT_DISPATCH_AUTOREGISTER(xx_subsysid, xx_entry) \
    static const diagpkt_master_table_type xx_entry##_table = { \
     0, 0xFF, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
    }; \
    DiagDispatch xx_entry##_instance(&xx_entry##_table)

/*!
   If C++ is supported in the build, this macro, defined in the
   preprocessor, instantiates a class with a 'static const' constructor.
   This class is therefore instantiated at boot, calling the constructor
   prior to the system 'main()'.  This  eliminates the need to acquire
   scope to register, it is done automatically. 
   This macro is used if the diag packet has delayed response 
*/

  #define DIAGPKT_DISPATCH_AUTOREGISTER_DELAY(xx_subsysid, xx_entry) \
    static const diagpkt_master_table_type xx_entry##_table = { \
     1, 0xFF, xx_subsysid, sizeof (xx_entry) / sizeof (xx_entry[0]), DIAG_NO_PROC, 0, DIAGCOMM_CTRL_NO_PORT, xx_entry \
    }; \
    DiagDispatch xx_entry##_instance(&xx_entry##_table)
    
#endif
/*@}*/ /* end  grouping of  Diagnostics Packet Dispatch Table Registration macros for C++ */

/* Some external devices in the past shared diagpkt.h with the DMSS.  
 * This is the max size they expect, so we don't go larger that 944
 * until those external devices are updated and distributed to handle
 * arbitrary packet lengths.
 *
 * Also, this number must always be <= to DIAGBUF_SIZE - 4, and would still be
 * really unlikely to alloc successfully (unless it is a guaranteed
 * response). */

/*! 
@ingroup packet_service
@name Packet Size Definitions 
*/
/*@{*/ /* start grouping of Packet Size Definitions */

/*!
   Maximum TX Packet Size 
*/
#define DIAG_MAX_TX_PKT_SIZ (0x2220) /* To support 8.5kB logs + 32B header */

/*!
   Maximum RX Packet Size 
*/
#define DIAG_MAX_RX_PKT_SIZ 0x1000

/*@}*/ /* start grouping of Packet Size Definitions */

/* ------------------------------------------------------------------------
     Functions
------------------------------------------------------------------------ */
/*!
@ingroup packet_service
   @brief
   Initializes the packet response queue. 

   @dependencies
   None

   @sideeffects
   None

   @return 
   None

   @sa
   None

*/
void diagpkt_rsp_q_init (void);

/*!
@ingroup packet_service

   @brief
   Gets the response Id count.

   @param [in] wrap_flag Wrap Flag

   @dependencies
   none    

   @sideeffects
   None

   @return 
   <ul>
   <li>  Response ID count 
   </ul>
   @sa 
   None   

*/
uint32 getWrapcount(uint16 wrap_flag);

/*!
@ingroup packet_service

   @brief 
   Allocates the specified number of bytes in the diag heap memory 
   for the response packet  

   @details
   This function allocates the specified amount of space in the diag output 
   buffer.  If space is unavailable, access to the allocation buffer will be
   blocked using a semaphore until space is available.

   @param[in] code Command code (found in diagcmd.h)
   @param[in] length  Length, in bytes, of the requested packet allocation (including cmd_code)

   @dependencies
   This may only be called by the diag task for responses to explicit request
   packets!  This function is *not* re-entrant.  If the OS ever supports
   other than INTLOCK, this function can be made re-entrant.  
  
   diagpkt_commit() must be called to commit the response packet to be sent.
   Not calling diagpkt_commit() will result in a memory leak.

   @sideeffects
   None

   @return 
   <ul>
   <li>  Pointer to the  response packet if the allocation is successful
   <li>  NULL if the allocation fails when diagpkt_alloc is not called 
         in diag task context 
   </ul>
   

   @sa 
   None       
*/
void * diagpkt_alloc (diagpkt_cmd_code_type code, unsigned int length);

/*!
@ingroup packet_service

   @brief 
   Allocates the specified number of bytes in the diag heap memory 
   to the response packet of subsytem command

   @details
   This call is the same as calling diagpkt_alloc(), but is used for
   allocating responses for subsystem commands.  It  fills in the subsystem
   header info for you.

   @param[in] id Sub System id (refer to diagcmd.h)
   @param[in] code Subsystem Command code
   @param[in] length  Length, in bytes, of the requested packet allocation (including cmd_code)

   @dependencies
   This may only be called by the diag task for responses to explicit request
   packets!  This function is *not* re-entrant.  If the OS ever supports
   semaphores other than INTLOCK, this function can be made re-entrant.  
  
   diagpkt_commit() must be called to commit the response packet to be sent.
   Not calling diagpkt_commit() will result in a memory leak.

   @sideeffects
   None

   @return 
   <ul>
   <li>  Pointer to the  response packet if the allocation is successful
   <li>  NULL if the allocation fails when the function is not called 
         in diag task context 
   </ul>
  
   @sa 
   None      
*/
void * diagpkt_subsys_alloc (diagpkt_subsys_id_type id,
    diagpkt_subsys_cmd_code_type code, unsigned int length);


/*!
@ingroup packet_service

   @brief 
   Allocates the specified number of bytes in the diag heap memory 
   to the response packet of subsytem dispatch version 2 command 

   @details 
   This function allocates the specified amount of space in the diag output 
   buffer.  If space is unavailable, access to the allocation buffer will be
   blocked using a semaphore until space is available.
   This call is same as diagpkt_subsys_alloc but used for subsystem dispatch
   version 2 commands

   @param[in] id sub system id 
   @param[in] code Subsystem Command code
   @param[in] length Length, in bytes, of the requested packet allocation (including cmd_code)

   @dependencies
   This may only be called by the diag task for responses to explicit request
   packets!  This function is *not* re-entrant.  If the OS ever supports
   semaphores other than INTLOCK, this function can be made re-entrant.  
  
   diagpkt_commit() must be called to commit the response packet to be sent.
   Not calling diagpkt_commit() will result in a memory leak.

   @sideeffects
   None

   @return 
  <ul>
   <li>  Pointer to the  response packet if the allocation is successful
   <li>  NULL if the allocation fails when the function is not called 
         in diag task context 
   </ul>

   @sa 
   None      
*/
   void * diagpkt_subsys_alloc_v2 (diagpkt_subsys_id_type id,
    diagpkt_subsys_cmd_code_type code, unsigned int length);


/*!
@ingroup packet_service

   @brief
   Allocates the specified number of bytes in the AMSS heap
   to the delayed response packet of subsytem dispatch version 2 command 

   @details
   This function allocates the specified amount of space in the AMSS heap.
   This function is used to send a delayed response.

   @param[in] id sub system id 
   @param[in] code  Subsystem Command code
   @param[in] delayed_rsp_id  Delayed Response ID 
   @param[in] length Length, in bytes, of the requested packet allocation (including cmd_code)

   @dependencies
   diagpkt_delay_commit() must be called to commit the response packet to be 
   sent. Not calling diagpkt_delay_commit() will result in a memory leak.

   @note  
   User is required to provide delayed response id as an argument.
   This helps tools to match the delayed response with the original 
   request response pair.

   @sideeffects
   None

   @return 
   <ul>
   <li>  Pointer to the memory allocated for response packet
   <li>  NULL if the allocation fails when AMSS heap is full
   </ul>
   
   @sa 
   None      
*/
  void * diagpkt_subsys_alloc_v2_delay (
    diagpkt_subsys_id_type id,
    diagpkt_subsys_cmd_code_type code, 
    diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id,
    unsigned int length);

/*!
@ingroup packet_service

   @brief
   This function reduces the length field of a previously allcated buffer. 

   @details
   'ptr' must point to the same address that was returned by a prior call to
   diagpkt_alloc() or diagpkt_subsys_alloc().
       
   Allocating too much and using this to shorten the packet is ideal for 
   situations in which the length of the packet is not known prior to 
   allocation.  Using this scheme does, however, consume resources that would
   otherwise be used to buffer outbound diagnostics data.  Please use this
   capability sparingly.

   @param[in] ptr  Pointer to the response packet
   @param[in] length New Length  in bytes for the packet

   @sideeffects
   None

   @return 
   None

   @sa 
   None      

*/
void diagpkt_shorten (void * ptr, unsigned int length);

/*!
@ingroup packet_service

   @brief 
   This function free the packet allocated by diagpkt_alloc(), which doesn't
   need to 'commit' for sending as a response because it is merely a temporary
   processing packet.

   @param[in] pkt  Pointer to the response Packet 

   @dependencies
   None

   @sideeffects
   None

   @return 
   None

   @sa 
   none   

*/
void diagpkt_free( void * pkt);

/*!
@ingroup packet_service

   @brief 
   This function commits previously allocated space in the diagnostics output
   buffer.

   @details
   'ptr' must be the same pointer that was returned from diagpkt_alloc() or
   diagpkt_subsys_alloc().

   This function signals the DIAG task and may cause a context switch.

   The packet handler type returns the response pointer.  The dispatcher
   calls diagpkt_commit() internally.  This only needs to be called
   explicitly if the packet needs to be committed before the packet handler
   returns.  In this case, the packet handler should return NULL.

   @param[in] ptr  Pointer to response packet 

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None  

*/
  void diagpkt_commit (void * ptr);

/*!
@ingroup packet_service

   @brief 
   This function commits previously allocated space in the AMSS heap.

   @details
   This function needs to be called explicitly for the delayed response to be committed. If this
   function is not called, delayed response packet will not be sent and a memory leak will occur.
   There is no restriction about which task's context this function should be called

   @param[in] pkt  Pointer to the response packet 

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None  
*/
  void diagpkt_delay_commit (void * pkt);


/*!
@ingroup packet_service

   @brief 
   This function returns the command code in the specified diagnostics packet.

   @param[in] ptr  Pointer to  the diagnostic request packet 

   @dependencies
   none 

   @sideeffects
   None

   @return 
   <ul>
   <li>  Returns the command code in the specified diagnostics packet
   </ul>
 
   @sa 
   None  
*/
  diagpkt_cmd_code_type diagpkt_get_cmd_code (void * ptr);



/*!
@ingroup packet_service

   @brief 
   This function sets the command code in the specified diagnostics packet.

   @param[in] ptr              Pointer to the diagnostic subsystem  packet 
   @param[in] cmd_code         Command Code(in diagcmd.h) 

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None  

*/
void diagpkt_set_cmd_code (void * ptr, diagpkt_cmd_code_type cmd_code);

/*!
@ingroup packet_service

   @brief 
   This function returns the subsystem ID in the specified diagnostics packet.

   @details
   If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
   0xFFFF is returned.

   @param[in] ptr             Pointer to the diagnostic subsytem packet 

   @dependencies
   None 

   @sideeffects
   None

   @return 
   <ul>
   <li> subsystem ID in the specified diagnostics packet.
   <li> 0xFFFF If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet
   </ul>

   @sa 
   None  
*/

diagpkt_subsys_id_type diagpkt_subsys_get_id (void * ptr);


/*!
@ingroup packet_service

   @brief
   This function returns the subsystem command code in the specified 
   diagnostics packet.

   @details
   If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet,
   0xFFFF is returned. 

   @param[in] ptr             Pointer to the diagnostic subsytem packet 

   @dependencies
   None 

   @sideeffects
   None

   @return 
    <ul>
   <li> subsystem command code  in the specified diagnostics packet.
   <li> 0xFFFF If the packet is not a DIAG_SUBSYS_CMD_F or DIAG_SUBSYS_CMD_VER_2_F packet
   </ul>

   @sa 
   None  

*/
  diagpkt_subsys_cmd_code_type diagpkt_subsys_get_cmd_code (void * ptr);


/*!
@ingroup packet_service

   @brief
   This function gets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet.

   @details
   This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
   packet.

   @param[in] ptr             Pointer to the diagnostic subsytem packet 

   @dependencies
   None 

   @sideeffects
   None

   @return 
   <ul>
   <li>  Returns the value in the status field of the packet 
   </ul>
  
   @sa 
   None  

*/
  diagpkt_subsys_status_type diagpkt_subsys_get_status (void * ptr);


/*!
@ingroup packet_service

   @brief
   This function sets the status field in the DIAG_SUBSYS_CMD_VER_2_F packet

   @details
   This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
   packet.

   @param[in] ptr               Pointer to the subsystem diagnostic packet
   @param[in] status            value to be assigned in the status field in ptr 

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None  

*/
  void diagpkt_subsys_set_status (void * ptr, 
      diagpkt_subsys_status_type status);


/*!
@ingroup packet_service

   @brief
   This function gets the delayed response ID field in the 
   DIAG_SUBSYS_CMD_VER_2_F packet

   @details
   This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F packet.

   @param[in] ptr            Pointer to the subsystem diagnostic packet

   @dependencies
   None 

   @sideeffects
   None

   @return 
   <ul>
   <li>  Gets the delayed response ID for subsystem dispatch
   version 2 packet (cmd code 128) 
   </ul>
   
   @sa 
   None

*/
diagpkt_subsys_delayed_rsp_id_type diagpkt_subsys_get_delayed_rsp_id 
      (void * ptr);


/*!
@ingroup packet_service

   @brief 
   This function sets the delayed response ID to zero in the 
   DIAG_SUBSYS_CMD_VER_2_F packet. 

   @details
   This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
   packet.

   @param[in] ptr              Pointer to the diagnostic subsystem packet 

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None  
*/
  void diagpkt_subsys_reset_delayed_rsp_id (void * ptr);


/*!
@ingroup packet_service

   @brief
   This function sets the response count in the DIAG_SUBSYS_CMD_VER_2_F packet.

   @details
   This function is used when multiple delayed responses have to be sent. The function
   diagpkt_subsys_alloc_v2_delay() should be called once for each response. The response count
   for each of these delayed responses should be set, using the function
   diagpkt_subsys_set_rsp_cnt().

   This function's first argument (ptr) should always be DIAG_SUBSYS_CMD_VER_2_F
   packet.

   @param[in] ptr            Pointer to the subsystem diagnostic packet
   @param[in] rsp_cnt        value to assign to respont count field in ptr

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None  

*/
  void diagpkt_subsys_set_rsp_cnt (void * ptr, 
      diagpkt_subsys_rsp_cnt rsp_cnt);


/*!
@ingroup packet_service

   @brief 
   This function builds an error packet.
   Usage: rsp_ptr = diagpkt_err_rsp ( error_code, req_pkt );

   @param[in] code              Command code (Found in diagcmd.h)
   @param[in] req_ptr           Pointer to the request packet
   @param[in] req_len           Length of the request packet

   @dependencies
   None 

   @sideeffects
   None

   @return 
   <ul>
   <li>    Pointer to the response packet 
   </ul>
 
   @sa 
   None  
*/
  void * diagpkt_err_rsp (diagpkt_cmd_code_type code,
    void * req_ptr, uint16 req_len);

/*!
@ingroup packet_service

   @brief
   This procedure blocks while we wait for the DIAG_ASYNC_BLOCK_SIG to be set
   to allow for asynchronous delays in packet handling.

   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   diagpkt_async_release  
*/
  void diagpkt_async_block (void);

/*!
@ingroup packet_service

   @brief
   This procedure sets the DIAG_ASYNC_BLOCK_SIG to end the asynchronous delay
   started with DIAGPKT_ASYNC_BLOCK
   
   @dependencies
   None 

   @sideeffects
   None

   @return 
   None

   @sa 
   None  
*/
  void diagpkt_async_release (void);



/*!
@ingroup packet_service
@name  Diagnostics Packet Type Definition Macros
            
  These macros were defined to provide and enforce naming
  conventions for declaring packets.  However, these macros
  make editor tags useless and add consufion.  The use of
  these macros has been deprecated, but is included here for
  compatibility with existing usage.  The naming convention
  enforced by these macros is not required for use of this
  service.
  
  The naming convention enforced by these macros is outlined
  below:

  Command codes use the naming convention: DIAG_xxx_F
  
  Requests types:
  DIAG_xxx_F_req_type
  
  Response types:
  DIAG_xxx_F_rsp_type
@warning  It is not recommended to continue use of these macros.
  
*/
/*@{*/ /* start grouping the Diagnostics Packet Type Definition Macros*/

/*!
 This macro is used to declare the diagnostics  request packet 
*/
#define DIAGPKT_REQ_DEFINE( xx_cmd_code ) \
  typedef struct xx_cmd_code##_req_tag \
                 xx_cmd_code##_req_type; \
  PACK(struct)  xx_cmd_code##_req_tag { \
    diagpkt_header_type xx_header;

/*!
 This macro is used to end the declarion of the diagnostics request packet 
*/
#define DIAGPKT_REQ_END };

/*!This macro is used to declare the diagnostics  response packet 
if the response is the same structure as the request...
*/

#define DIAGPKT_DEFINE_RSP_AS_REQ( xx_cmd_code ) \
  typedef xx_cmd_code##_req_type xx_cmd_code##_rsp_type;

/*!
 This macro is used to declare the diagnostics  response packet 
*/
#define DIAGPKT_RSP_DEFINE( xx_cmd_code ) \
  typedef struct xx_cmd_code##_rsp_tag \
                 xx_cmd_code##_rsp_type; \
  PACK(struct)  xx_cmd_code##_rsp_tag { \
    diagpkt_header_type xx_header;

/*!
 This macro is used to end the declarion of the diagnostics response packet 
*/
#define DIAGPKT_RSP_END };
/*@}*/ /* start grouping the Diagnostics Subsystem Packet Type Definition Macros*/

/*!

  @ingroup packet_service
  @name  Diagnostics Subsystem Packet Type Definition Macros

  These macros were defined to provide and enforce naming
  conventions for declaring packets. 
  
  The naming convention enforced by these macros is outlined
  below:

  Diag has a subsystem command that dispatches diag commands to
  various subsystems.  All subsystem packet types use the same
  naming convention throughout the DMSS.  The subsystem command
  uses a 16 bit command code per subsystem.  This results in
  user data starting on a 32 bit boundary.
  
  The naming convention is as follows:
  
  Command codes use the naming convnetion: DIAG_SUBSYS_xxx_F
  
  Requests types:
  DIAG_SUBSYS_xxx_yyy_req_type
  
  Response types:
  DIAG_SUBSYS_xxx_yyy_rsp_type
  @warning  It is not recommended to continue use of these macros.

*/
/*@{*/ /* start grouping the Diagnostics Subsystem Packet Type Definition Macros*/

/*!
 This macro is used to declare the diagnostics subsystem request packet 
*/
#define DIAGPKT_SUBSYS_REQ_DEFINE( xx_subsys, xx_subsys_cmd_code ) \
  typedef struct DIAG_SUBSYS_##xx_subsys##_##xx_subsys_cmd_code##_req_tag \
                 DIAG_SUBSYS_##xx_subsys##_##xx_subsys_cmd_code##_req_type; \
  PACK(struct)  DIAG_SUBSYS_##xx_subsys##_##xx_subsys_cmd_code##_req_tag { \
    diagpkt_subsys_header_type xx_header;
/*!
 This macro is used to end the declarion of the diagnostics subsystem request packet 
*/
#define DIAGPKT_SUBSYS_REQ_END };

/*!
 This macro is used to declare the diagnostics subsystem response packet 
*/
#define DIAGPKT_SUBSYS_RSP_DEFINE( xx_subsys, xx_subsys_cmd_code ) \
    typedef struct DIAG_SUBSYS_##xx_subsys##_##xx_subsys_cmd_code##_rsp_tag \
                   DIAG_SUBSYS_##xx_subsys##_##xx_subsys_cmd_code##_rsp_type; \
    PACK(struct)  DIAG_SUBSYS_##xx_subsys##_##xx_subsys_cmd_code##_rsp_tag { \
      diagpkt_subsys_header_type xx_header;

/*!
 This macro is used to end the declarion of the diagnostics subsystem response packet 
*/
#define DIAGPKT_SUBSYS_RSP_END };

/*@{*/ /* end grouping the Diagnostics Subsystem Packet Type Definition Macros*/

#ifdef __cplusplus
}
#endif

#endif				/* DIAGPKT_H  */
