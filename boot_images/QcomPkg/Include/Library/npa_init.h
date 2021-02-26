/*============================================================================
@file npa_init.h

NPA framework init routines

Copyright © 2009-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
QUALCOMM Proprietary/GTDR

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