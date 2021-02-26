/*===========================================================================

  rpm_events.h - runtime-replacable wrapper API for events

  Copyright (c) 2016-2107 Qualcomm Technologies, Inc.  
  All Rights Reserved.
  Proprietary and Confidential - Qualcomm Technologies, Inc.

  ===========================================================================*/

#ifndef RPMH_EVENTS_H
#define RPMH_EVENTS_H

typedef struct rpmh_event_s rpmh_event_t;

extern rpmh_event_t *(*rpmh_event_create)(void);
extern void (*rpmh_event_destroy)(rpmh_event_t *self);
extern void (*rpmh_event_reset)(rpmh_event_t *self);
extern void (*rpmh_event_set)(rpmh_event_t *self);
extern void (*rpmh_event_wait)(rpmh_event_t *self);

#endif // RPMH_EVENTS_H

