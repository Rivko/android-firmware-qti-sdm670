/*===========================================================================

  rpm_events.h - runtime-replacable wrapper API for events

  Copyright (c) 2012,2014 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  Qualcomm Technologies Inc. Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_EVENTS_H
#define RPM_EVENTS_H

struct rpm_event_s ;
typedef struct rpm_event_s rpm_event_t;

extern rpm_event_t *(*rpm_event_create)(void);
extern void (*rpm_event_destroy)(rpm_event_t *self);
extern void (*rpm_event_reset)(rpm_event_t *self);
extern void (*rpm_event_set)(rpm_event_t *self);
extern void (*rpm_event_wait)(rpm_event_t *self);

#endif // RPM_EVENTS_H

