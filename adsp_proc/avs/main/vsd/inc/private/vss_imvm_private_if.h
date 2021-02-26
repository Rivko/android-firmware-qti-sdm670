#ifndef __VSS_IMVM_PRIVATE_IF_H__
#define __VSS_IMVM_PRIVATE_IF_H__

/*
   Copyright (c) 2009-2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/vss_imvm_private_if.h#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * CONCURRENCY MONITORING APIS                                              *
 ****************************************************************************/

/**
  Command sent by the MVM session to itself to indicate that a reconfiguration 
  of the voice path is necessary.
 
  @par Payload
  None.
  
  @return 
  None.

  @dependencies
  None.
 
  @comments
  This command is generated when the MVM session is notified by one of the 
  streams or vocprocs attached to it that their configuration (i.e. media type, 
  topology, sample rates and/or KPPS requirements) has changed, which requires 
  MVM to re-calculate the timing offsets for this session, as well report the 
  configuration changes to the CCM, so that the CCM may set the clocks 
  accordingly and notify all other MVM sessions of the changes.
*/
#define VSS_IMVM_CMD_RECONFIG ( 0x00012E6D )


/**
  Command sent by the MVM session to itself to indicate that dynamic system
  configurations has been changed on the voice path.
 
  @par Payload
  None
  
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
 
  @comments
  This command is generated when the MVM session is notified by one of the
  streams or vocprocs attached to it that their dynamic configurations (e.g.
  vocoder operating mode, etc.) has changed, which requires MVM to dynamically
  reconfigure the attached streams and vocprocs.
*/
#define VSS_IMVM_CMD_DYNAMIC_RECONFIG ( 0x000130A8 )

/**
  This command queries different CVD services for object leaks using is_cvd_utils_obj_freed() 
  utility. 
 
  @par Payload
  None
  
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
 
  @comments
  None
*/
#define VSS_IMVM_CMD_QUERY_OBJECT_LEAKS (  0x0001327C )

#endif /* __VSS_IMVM_PRIVATE_IF_H__ */

