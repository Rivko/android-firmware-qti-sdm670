/*=============================================================================

FILE:       dynconfig.h

SERVICES:   API for oem configuration through a dynamic shared object.

==============================================================================
        Copyright (c) 2013 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
            QUALCOMM Proprietary and Confidential
==============================================================================*/
#ifndef DYNCONFIG_H
#define DYNCONFIG_H

#define DYN_FNAME       "oemconfig.so"

int dynconfig_ParseOEMConfig(void *h);
int* dynconfig_getCodeTCGs(void);
int* dynconfig_getTestTCGs(void);
void dynconfig_dtor(void);

#endif   // DYNCONFIG_H
