#===============================================================================
#
# Builder to generate a tracer header files containing a list of sw events
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2012,2016 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/swe_builder.py#1 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 03/24/16   dks     QDSS Support for trustzone
# 05/18/12   ts      Generate a tracer_events.h containting a list of sw events
#===============================================================================

import os
import string
import binascii
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
    # Generate function for swe builder.
    # Sets up the action, scanner and builder function to be called by
    # clients to pass SW event details to the builder.

    rootenv = env.get('IMAGE_ENV')
    rootenv[SWE_ENV_DESCRIPTOR] = []
    env.Append(QDSS_TRACER_SWE = True) # Signal to call builder
    env.Replace(USES_QDSS_SWE = True)  # Define SW events

    # Add method AddSWEInfo to the environment.
    # AddSWEInfo is the function that needs to be called by the clients to
    # pass a list of SW event info they plan to use for generating
    # SW events
    rootenv.AddMethod(swe_append_list, "AddSWEInfo")

    # Load builder into enviroment
    swe_actn = env.GetBuilderAction(swe_builder, action_source=None)
    swe_bldr = env.Builder(action=swe_actn, emitter=swe_emitter, multi=True)
    rootenv.Append(BUILDERS={'SWEBuilder' : swe_bldr})

    return


def swe_emitter(target, source, env):
    # So that tracer_event_ids.h is generated when any clients have
    # updated their SW Event names-value pair info

    rootenv = env.get('IMAGE_ENV')
    source.append(env.Value(rootenv[SWE_ENV_DESCRIPTOR]))

    return (target, source)


# Snippet from core/power/npa/build/SConscript
def determine_subsystem( env ):

    # Order important! We iterate through the dict in this order
    image_names = ['MODEM', 'WCN', 'APPS', 'SENSOR', 'AUDIO', 'ADSP', 'TZ']
    image_tags = {
        'MODEM'        : ['MODEM_PROC', 'QDSP6_SW_IMAGE'],
        'WCN'          : ['WCN_IMAGE', 'CBSP_WCN_IMAGE', 'CORE_WCN'],
        'APPS'         : ['APPS_IMAGE', 'CBSP_APPS_IMAGE'],
        'SENSOR'       : ['CORE_QDSP6_SENSOR_SW'],
        'AUDIO'        : ['CORE_QDSP6_AUDIO_SW'],
        'ADSP'         : ['ADSP_PROC', 'CORE_QDSP6_SW'],
        'TZ'           : ['TZOS_IMAGE', 'MONITOR_IMAGE'],        
        }
    env_var_names = set(env.gvars().keys())
    for i in image_names:
        if env_var_names.intersection(set(image_tags[i])):
            return i

    return None


def swe_builder(target, source, env):
    
    # Create the event files (tracer_event_ids.h, tracer_event_tbl.h,
    # and tracer_event_info.txt)

    rootenv = env.get('IMAGE_ENV')

    #tracer_event_ids argument MUST be passed
    try:
        tracer_event_ids = str(target[0])
    except:
        env.PrintError("SWE Builder: Event file argument not passed")
        exit()

    try:
        tracer_event_ids_h = open(env.RealPath(tracer_event_ids), 'w')
    except IOError:
        env.PrintError("Could not open tracer swe ids file for write")
        exit()

    # Compute CRC over the event names
    crc = binascii.crc32("QDSS") & 0xFFFFFFFF
    for item in rootenv[SWE_ENV_DESCRIPTOR]:
        crc = binascii.crc32(item[0],crc) & 0xFFFFFFFF

    # Set "subsys" according to the build being placed in.
    # Valid values: 'MODEM', 'ADSP', 'WCN', 'VENUS', 'RPM','SENSOR','AUDIO'
    # Obsoleted: 'PRONTO' replace with 'WCN'
    # Determine_subsystem is a default - if it cannot determine the image,
    # subsys can be hardcoded according to the build (subsys = 'MODEM').
    subsys = determine_subsystem(env)

    # Want these tags to physically appear each file.
    header_tags = '#ifndef EVENT_TABLE_TAG\n' \
                  '#define EVENT_TABLE_TAG \"{0}' + \
                      '0000' + format(crc,'08X') + '\"\n' \
                  '#define EVENT_PROCESSOR \"' + \
                      env.get('PROC_CONFIG') + '\"\n' \
                  '#define EVENT_SUBSYSTEM ' + subsys + '\n' \
                  '#endif\n\n'

    # Generate the tracer event id enumeration file
    event_id_filename = os.path.split(tracer_event_ids)[1].split('.')[0]
    tracer_event_ids_h.write('#ifndef _'+event_id_filename.upper()+'_H\n')
    tracer_event_ids_h.write('#define _'+event_id_filename.upper()+'_H\n\n')
    tracer_event_ids_h.write('// File autogenerated by swe_builder.\n\n')

    tracer_event_ids_h.write(header_tags.format('QDSS_TAG_'))

    tracer_event_ids_h.write('enum tracer_event_id_enum {\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_RESERVE_0=0,\n')
    if 'USES_QDSS_SWE' in env:
        for item in rootenv[SWE_ENV_DESCRIPTOR]:
            tracer_event_ids_h.write('\t')
            tracer_event_ids_h.write(item[0])
            tracer_event_ids_h.write(',\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_ID_MAX,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_RESERVE_LAST=0x00FFFFFF,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_ALL=-1,\n')
    tracer_event_ids_h.write('};\n\n')

    maskVal = 0
    maskCnt = 1    #Account for TRACER_EVENT_RESERVE_0 event

    tracer_event_ids_h.write('#ifndef TRACER_EVENT_PREENABLE_SCONS\n')
    tracer_event_ids_h.write('#define TRACER_EVENT_PREENABLE_SCONS {\\\n')
    if 'USES_QDSS_SWE' in env:
        for item in rootenv[SWE_ENV_DESCRIPTOR]:
            if 'T' == item[2].upper():
                maskVal = maskVal | (1 << maskCnt)
            maskCnt += 1
            if 32 <= maskCnt:
                tracer_event_ids_h. write('\t' + hex(maskVal) + ',\\\n')
                maskVal = 0
                maskCnt = 0
        if 0 < maskCnt:
            tracer_event_ids_h. write('\t' + hex(maskVal) + '\\\n')
    else:
        tracer_event_ids_h.write('\t0\\\n')
    tracer_event_ids_h.write('}\n')
    tracer_event_ids_h.write('#endif\n\n')

    tracer_event_ids_h.write('#endif // _'+event_id_filename.upper()+'_H\n\n')
    tracer_event_ids_h.close()

    #tracer_event_tbl argument MAY be passed
    try:
        tracer_event_tbl = str(target[1])
    except:
        tracer_event_tbl = ''

    if tracer_event_tbl != '':
        try:
            tracer_event_tbl_h = open(env.RealPath(tracer_event_tbl), 'w')
            event_table_filename=os.path.split(tracer_event_tbl)[1].split('.')[0]
        except IOError:
            env.PrintError("Could not open tracer swe tbl file for write")
            exit()

        # Generate the tracer event table file
        tracer_event_tbl_h.write('#ifndef _'+event_table_filename.upper()+'_H\n')
        tracer_event_tbl_h.write('#define _'+event_table_filename.upper()+'_H\n\n')
        tracer_event_tbl_h.write('// File autogenerated by swe_builder.\n\n')
        tracer_event_tbl_h.write('#include \"'+event_id_filename+'.h\"\n\n')
        tracer_event_tbl_h.write(header_tags.format(''))

        tracer_event_tbl_h.write('typedef struct _tracer_event_item {\n')
        tracer_event_tbl_h.write('\tenum tracer_event_id_enum evt_id;\n')
        tracer_event_tbl_h.write('\tchar *evt_str;\n')
        tracer_event_tbl_h.write('\tchar pre_enable;\n')
        tracer_event_tbl_h.write('} tracer_event_item;\n\n')

        tracer_event_tbl_h.write('tracer_event_item tracer_event_tbl[] = {\n')
        tracer_event_tbl_h.write('\t{TRACER_EVENT_RESERVE_0,"",\'F\'},\n')
        if 'USES_QDSS_SWE' in env:
            for item in rootenv[SWE_ENV_DESCRIPTOR]:
                tracer_event_tbl_h.write('\t{')
                tracer_event_tbl_h.write(item[0])
                tracer_event_tbl_h.write(',"')
                tracer_event_tbl_h.write(item[1])
                tracer_event_tbl_h.write('",\'')
                tracer_event_tbl_h.write(item[2].upper())
                tracer_event_tbl_h.write('\'},\n')
        tracer_event_tbl_h.write('\t{TRACER_EVENT_ID_MAX,"",\'F\'}\n')
        tracer_event_tbl_h.write('};\n\n')

        tracer_event_tbl_h.write('#endif // _'+event_table_filename.upper()+'_H\n\n')

        tracer_event_tbl_h.close()

    #tracer_event_info argument MAY be passed
    try:
        tracer_event_info = str(target[2])
    except:
        tracer_event_info = ''

    if tracer_event_info != '':
        try:
            tracer_event_info_txt = open(env.RealPath(tracer_event_info), 'w')
            event_info_filename=os.path.split(tracer_event_info)[1]
        except IOError:
            env.PrintError("Could not open tracer swe info file for write")
            exit()

        info_file_format_version = '1'
        info_instruction_prefix = 'swt \"'+subsys+'\" info \"event\" '

        # Generate the tracer event info file
        tracer_event_info_txt.write('# File: '+event_info_filename+'\n')
        tracer_event_info_txt.write('# File autogenerated by swe_builder.\n')
        tracer_event_info_txt.write('swt info format '+info_file_format_version+'\n')
        tracer_event_info_txt.write(info_instruction_prefix+'tags \"'+ \
            env.get('PROC_CONFIG')+'\" \"0000'+format(crc,'08X')+'\"\n')

        info_instruction_prefix += 'id '
        event_count = 1;

        tracer_event_info_txt.write(info_instruction_prefix+\
            '0 TRACER_EVENT_RESERVE_0 \"\" 0\n')
        if 'USES_QDSS_SWE' in env:
            for item in rootenv[SWE_ENV_DESCRIPTOR]:
                tracer_event_info_txt.write(info_instruction_prefix)
                tracer_event_info_txt.write(repr(event_count))
                tracer_event_info_txt.write(' '+ item[0]+' \"'+item[1]+'\" ')
                if (item[2].upper() == 'T'):
                    preenab = '1'
                else:
                    preenab = '0'
                tracer_event_info_txt.write(preenab +'\n')
                event_count += 1
        tracer_event_info_txt.write(info_instruction_prefix)
        tracer_event_info_txt.write(repr(event_count))
        tracer_event_info_txt.write(' TRACER_EVENT_ID_MAX \"\" 0\n\n')

        tracer_event_info_txt.close()

    return None


def find_insert_index(env, insertname):
    # The SW event list is sorted using only the prefix of the event's name.
    # Order is maintained within each prefix block. The same prefix 
    # subsequently encountered is added to the end of that prefix block.
    # Returns the index where insertname is to be added to SW event list.

    rootenv = env.get('IMAGE_ENV')

    for idx, item in enumerate(rootenv[SWE_ENV_DESCRIPTOR]):
        # Sort by prefix only; ascending order; same prefix is added
        # to the end of the prefix block.
        if (item[0].split('_')[0] > insertname):
            break
    else: # Reached end of list
        idx = len(rootenv[SWE_ENV_DESCRIPTOR])

    return idx


def swe_append_list(env, target, source):
    # When AddSWEInfo is called by the client, the list of SW event
    # name-desc-precfg triplets passed by the clients are stored
    # in a list here.

    rootenv = env.get('IMAGE_ENV')

    if 'USES_QDSS_SWE' in env:
        baseprefix = "" # Start
        idx = 0

        for item in source:
            item[0] = item[0].split('=')[0]     # Drop assignment portion
            if len(item) == 2:                  # Convert old definition
                item.append('F')                # Add third param

            # Sorted location for items. Update when prefix changes.
            currentprefix = item[0].split('_')[0]
            if currentprefix != baseprefix:
                idx = find_insert_index(env, currentprefix)
                baseprefix = currentprefix

            rootenv[SWE_ENV_DESCRIPTOR].insert(idx, item) # Add item to list
            idx += 1 # Order maintained for same prefix

    return

