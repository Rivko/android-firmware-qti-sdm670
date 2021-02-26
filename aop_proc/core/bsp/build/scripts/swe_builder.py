#===============================================================================
#
# Builder to generate a tracer header files containing a list of sw events
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/bsp/build/scripts/swe_builder.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 5/18/12    ts      Generate a tracer_events.h containting a list of sw events
#===============================================================================

import os
import subprocess
import string
from SCons.Script import *

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
SWE_ENV_DESCRIPTOR = 'SWE_INFO_COLLECTOR'

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    #Generate function for devcfg builder.
    #Sets up the action, scanner and builder function to be called by clients to
    #pass sw event details to the builder.

    rootenv = env.get('IMAGE_ENV')
    rootenv[SWE_ENV_DESCRIPTOR] = []

    # Add method AddSWEInfo to the enviroment. 
    # AddSWEInfo is the function that needs to be called by the clients to pass
    # list of SW Event names-value pairs they plan to use for generating SW Events
    rootenv.AddMethod(swe_append_list, "AddSWEInfo")

    # Load builder into enviroment
    swe_actn = env.GetBuilderAction(swe_builder, action_source=None)
    swe_bldr = env.Builder(action = swe_actn, emitter = swe_emitter)
    rootenv.Append(BUILDERS = {'SWEBuilder' : swe_bldr})

    return

def swe_emitter(target, source, env):
    # So that tracer_event_ids.h is generated when any clients have updated their
    # SW Event names-value pair info

    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag('USES_QDSS_SWE') is False:
        return

    source.append(env.Value(rootenv[SWE_ENV_DESCRIPTOR]))
    return (target, source)

def swe_builder(target, source, env):
    # Create the event header files (tracer_event_ids.h and tracer_event_tbl.h)
    
    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag('USES_QDSS_SWE') is False:
        return


    try:
        tracer_event_ids_h = open(env.RealPath(str(target[0])), 'w')
        tracer_event_tbl_h = open(env.RealPath(str(target[1])), 'w')
    except IOError:
        env.PrintError("Could not open tracer files for write")

    # Generate the tracer event id enumeration file
    tracer_event_ids_h.write('#ifndef _TRACER_EVENT_IDS_H\n')
    tracer_event_ids_h.write('#define _TRACER_EVENT_IDS_H\n\n')
    
    tracer_event_ids_h.write('typedef enum {\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_RESERVE_0=0,\n')

    for item in rootenv[SWE_ENV_DESCRIPTOR]:
        tracer_event_ids_h.write('\t')
        tracer_event_ids_h.write(item[0])
        tracer_event_ids_h.write(',\n')

    tracer_event_ids_h.write('\tTRACER_EVENT_ID_MAX,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_RESERVE_LAST=0x400,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_ALL=0x7FFFFFFF,\n')
    tracer_event_ids_h.write('} tracer_event_id_t;\n\n')
              
    tracer_event_ids_h.write('#endif // _TRACER_EVENT_IDS_H\n')
    
    tracer_event_ids_h.close()

    # Generate the tracer event table file
    tracer_event_tbl_h.write('#ifndef _TRACER_EVENT_TBL_H\n')
    tracer_event_tbl_h.write('#define _TRACER_EVENT_TBL_H\n\n')
    
    tracer_event_tbl_h.write('#include \"tracer_event_ids.h\"\n\n')

    tracer_event_tbl_h.write('#define EVENT_SUBSYSTEM ')
    tracer_event_tbl_h.write(env.get('PROC_CONFIG'))
    tracer_event_tbl_h.write("\n\n")
    
    tracer_event_tbl_h.write('typedef struct _tracer_event_item {\n')
    tracer_event_tbl_h.write('\ttracer_event_id_t evt_id;\n')
    tracer_event_tbl_h.write('\tchar * evt_str;\n')
    tracer_event_tbl_h.write('}tracer_event_item;\n\n')

    tracer_event_tbl_h.write('tracer_event_item tracer_event_tbl[] = {\n')
    tracer_event_tbl_h.write('\t{TRACER_EVENT_RESERVE_0,""},\n')

    for item in rootenv[SWE_ENV_DESCRIPTOR]:
        tracer_event_tbl_h.write('\t{')
        tracer_event_tbl_h.write(item[0])
        tracer_event_tbl_h.write(',"')
        tracer_event_tbl_h.write(item[1])
        tracer_event_tbl_h.write('"},\n')

    tracer_event_tbl_h.write('\t{TRACER_EVENT_ID_MAX,""}\n')
    tracer_event_tbl_h.write('};\n\n')
          
    tracer_event_tbl_h.write('#endif // _TRACER_EVENT_TBL_H\n')
    
    tracer_event_tbl_h.close()

    return

def swe_append_list(env, target, source):
    #When AddSWEInfo is called by the client, the list of SW Event name-value pairs
    #passed by the clients are stored in a list here.
    
    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag('USES_QDSS_SWE') is False:
        return

    for item in source:
        rootenv[SWE_ENV_DESCRIPTOR].append(item)

    return
