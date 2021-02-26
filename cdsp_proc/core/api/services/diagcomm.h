#ifndef DIAGCOMM_H 
#define DIAGCOMM_H

/*!
        @ingroup diag_misc
        @file diagcomm.h
        @brief This file contains APIs for changing port and bitrates.
        @details
        This file contains API declarations for changing port between Diag and
        Data Services. Also includes prototypes for changing Diag bitrate and
        baudrate.
         
*/
 /*
  Copyright (C) 2000-2010 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
 */

/*===========================================================================

                           Edit History
                           
 $Header: //components/rel/core.qdsp6/2.1.c4/api/services/diagcomm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/17/10   mad     Doxygen comment modifications
05/18/10   as      Added Doyygen headers.
10/01/09   JV      Moved prototypes for diag_change_default_sio_bitrate and
                   diag_record_dm_baudrate here
07/31/09    JV     Merged Q6 diag code back to mainline
07/17/09    mad    Featured out for API header commonality
07/06/09    as     Removed depedency from FEATURE_DATA_SERIALIZER
06/11/09    JV     Featurized inclusion of customer.h to ease merging of the 
                   Q6 branch
05/07/09    vk      removed dependencies from diagbuf_v.h
12/12/08    vg     CMI splip into diagcomm_v.h and diagcomm.h
12/16/07    pc     Added support for diag_dtr_enable and diag_dtr_changed_cb.
12/15/06    as     Fixed compiler warnings.
05/15/05    as     Added support for FEATURE_DATA_SERIALIZER
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag
12/14/04    sl     Added support for DIAG to hold multiple SIO ports
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
10/24/01   jal     Added ability to flush transmit channel
04/06/01   lad     Introduces types for callback function pointers.
                   Moved ASYNC char definitions from diagi.h.
02/23/01   lad     Created file.

===========================================================================*/
#include "comdef.h"
#include "sio.h"
#include "rdevmap.h"

/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
    extern "C" {
#endif

/*===========================================================================
FUNCTION    DIAGCOMM_SIO_QCDMG */

/*! 
@ingroup diag_misc
@brief
Execute AT$QCDMG command 

Diag will close its open port and grab Data Services port.
       
@dependencies 
None 
 
@sideeffects 
None  

@return 
One of the following
  <ul>
  <li> TRUE - if RDM accepts the port mapping
  <li> FALSE - if RDM can't change the port mapping
  </ul>

@sa
None
*/
/*===========================================================================*/
boolean diagcomm_sio_qcdmg (void); 


/*===========================================================================
FUNCTION    DIAGCOMM_SIO_QCDMG_PORT */

 
/*! 
@ingroup diag_misc
@brief
Execute AT$QCDMG command. Diag will close its open port and grab 
Data Services port.
       
@param[in] port_id - Data's current device
       
@dependencies 
None 
 
@sideeffects 
None  

@return 
One of the following
  <ul>
  <li> TRUE - if RDM accepts the port mapping
  <li> FALSE - if RDM can't change the port mapping
  </ul>

@sa
None
*/
/*===========================================================================*/
boolean diagcomm_sio_qcdmg_port(rdm_device_enum_type port_id); 


/*===========================================================================
FUNCTION    DIAG_RECORD_DM_BAUDRATE */

/*! 
@ingroup diag_misc
@brief
This function is used by caller to set baudrate for diag which will take effect 
at the next occurence of DIAG_SIO_OPEN_SIG.
       
@param[in] bitrate      Diag's bitrate
       
@dependencies 
None 
 
@sideeffects 
Setting diag's bitrate will be effective next port open cycle.

@return 
None

@sa
None
*/
/*===========================================================================*/
  void diag_record_dm_baudrate (sio_bitrate_type bitrate);


/*===========================================================================
FUNCTION    DIAG_CHANGE_DEFAULT_SIO_BITRATE */

/*! 
@ingroup diag_misc
@brief
This function changed the default diag bitrate as well as change the
current operating bitrate.
       
@param[in] new_baud      Diag's port bitrate
       
@dependencies 
None 
 
@sideeffects 
Setting diag's bitrate will be effective in the next port open cycle.

@return 
None

@sa
None
*/ 
/*===========================================================================*/
  void diag_change_default_sio_bitrate (sio_bitrate_type new_baud);


#ifdef __cplusplus
    }
#endif
#endif /* DIAGCOMM_H  */
