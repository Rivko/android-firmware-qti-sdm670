/*============================================================================
@file npa_init.h

NPA framework init routines

Copyright © 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1/api/power/npa_init.h#1 $
============================================================================*/
#ifndef NPA_INIT_H
#define NPA_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  @brief npa_init - Initializes the NPA subsystem. No other NPA functions may
  be invoked prior to npa_init.
*/
void npa_init( void );

/**
  @brief npa_timer_init - Sets up a marker to indicate to NPA that the timer
  subsystem is up. Invoked from rcinit.
*/
void npa_timer_init( void );

#ifdef __cplusplus
}
#endif

#endif /* NPA_INIT_H */
