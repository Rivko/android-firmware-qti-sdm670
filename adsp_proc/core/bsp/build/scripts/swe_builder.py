#===============================================================================
#
# Builder to generate a tracer header files containing a list of sw events
#
# GENERAL DESCRIPTION
#    Contains builder(s) definitions with multi-PD support.
#
# DEPENDENCIES:
#    The SWEBuilder() is hardcoded to use the filename sweevent_master_list.txt.
#
# OVERVIEW:
#    The following gives an overview for using the swe_builder.
#
#    SwTechAreaBuildScons
#       AddSWInfo|swe_append_list()
#          --> Add tech area events to the SWE_ENV_DESCRIPTOR.
#
#    LibBuildScons (or ImageBuildScons if no libs being combined)
#       SaveSWEListBuilder|swe_save_list()
#          --> Save all SW events added to SWE_ENV_DESCRIPTOR into a
#          SW event list file (for example: sweevent_list.txt).
#
#    ImageBuildScons
#       SWEMergeBuilder|swe_merge_builder()
#          --> Combine the designated SW event list files (individual libs
#          being built into an image) into one consolidated list file
#          (for example: sweevent_consolidate_list.txt).
#
#       SWEReplicateBuilder|swe_replicate_builer()
#          --> For each of the SW event list files that were designated
#          to be combined, copy the consolidated list file to a
#          sw event master list file (for example: sweevent_master_list.txt)
#          for that Lib build.
#
#    SwTechAreaScons
#       SWEBuilder|SWEFinalBuilder|swe_final_builder()
#          --> Use the SW event master list file to generate the SW tech
#          area's event ids header file (for example: tracer_event_ids.h).
#
#===============================================================================
#
# Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
# All rights reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1/bsp/build/scripts/swe_builder.py#1 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 07/07/16   sm      Add in SLPI support for sdm845 compilation support. 
# 5/18/12    ts      Generate a tracer_events.h containting a list of sw events
# 11/14/14   KKO     Update builder to support refactoring of build scons file
#===============================================================================

import os
import string
import binascii
from SCons.Script import *
import shutil

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
    env.Append(QDSS_TRACER_SWE_MULTIPD = True) # Signal to call MultiPD builder
    env.Replace(USES_QDSS_SWE = True)  # Define SW events

    # Add method AddSWEInfo to the environment.
    # AddSWEInfo is the function that needs to be called by the clients to
    # pass a list of SW event info they plan to use for generating
    # SW events
    rootenv.AddMethod(swe_append_list, "AddSWEInfo")
    rootenv.AddMethod(swe_builder, "SWEBuilder")

    # Load builders into environment
    # Please refer https://community.qualcomm.com/groups/qdss for
    # detail information on design of these builders

    swe_final_builder_actn = env.GetBuilderAction(swe_final_builder, action_source=None)
    swe_final_bldr = env.Builder(action=swe_final_builder_actn, multi=True)
    rootenv.Append(BUILDERS={'SWEFinalBuilder' : swe_final_bldr})

    swe_old_bldr = env.Builder(action=swe_final_builder_actn, emitter=swe_emitter, multi=True)
    rootenv.Append(BUILDERS={'SWEOldBuilder' : swe_old_bldr})

    swe_merge_actn = env.GetBuilderAction(swe_merge_builder, action_source=None)
    swe_merge_bldr = env.Builder(action=swe_merge_actn, multi=True)
    rootenv.Append(BUILDERS={'SWEMergeBuilder' : swe_merge_bldr})

    swe_replicate_actn = env.GetBuilderAction(swe_replicate_builder, action_source=None)
    swe_replicate_bldr = env.Builder(action=swe_replicate_actn, multi=True)
    rootenv.Append(BUILDERS={'SWEReplicateBuilder' : swe_replicate_bldr})

    swe_save_list_actn = env.GetBuilderAction(swe_save_list, action_source=None)
    swe_save_list_bldr = env.Builder(action=swe_save_list_actn, emitter=swe_emitter, multi=True)
    rootenv.Append(BUILDERS={'SaveSWEListBuilder' : swe_save_list_bldr})

    return


def swe_emitter(target, source, env):
    # So that tracer_event_ids.h is generated when any clients have
    # updated their SW Event name-desc-precfg triplet of info.

    rootenv = env.get('IMAGE_ENV')
    source.append(env.Value(rootenv[SWE_ENV_DESCRIPTOR]))

    return (target, source)


def determine_subsystem( env ):
    # Snippet from core/power/npa/build/SConscript

    # Order important! We iterate through the dict in this order
    image_names = ['MODEM', 'WCN', 'APPS','SENSOR2', 'SENSOR', 'AUDIO', 'ADSP','CDSP']
    image_tags = {
        'MODEM'        : ['CORE_MODEM', 'QDSP6_SW_IMAGE', 'MOB_IMAGE'],
        'WCN'          : ['WCN_IMAGE', 'CBSP_WCN_IMAGE', 'CORE_WCN'],
        'APPS'         : ['APPS_IMAGE', 'CBSP_APPS_IMAGE'],
        'SENSOR2'      : ['CORE_SLPI_USER', 'CORE_SSC_SLPI_USER' ],
        'SENSOR'       : ['CORE_QDSP6_SENSOR_SW','CORE_SLPI_ROOT'],
        'CDSP'         : ['CORE_CDSP_ROOT'],
        'AUDIO'        : ['CORE_QDSP6_AUDIO_SW'],
        'ADSP'         : ['ADSP_PROC', 'CORE_USER_PD'],
        }
    env_var_names = set(env.gvars().keys())
    for i in image_names:
        if env_var_names.intersection(set(image_tags[i])):
            return i

    return None


def check_folder (tracer_event_ids, env):
    # Autogenerated files must be located in BUILDPATH and not in local
    # inc folder.

    build_folder = os.path.split(env.get('BUILDPATH'))[1]
    event_path = os.path.splitdrive(env.RealPath(tracer_event_ids))[1]
    event_path = os.path.split(event_path)[0]
    good = 0;
    while 1:
        head,tail = os.path.split(event_path)
        if (tail != ''):
            if (build_folder == tail):
                good = 1
                break
        else:
            if (head != ''):
                if (build_folder == head):
                    good = 1
                    break
            break
        event_path = head
    if (good == 0):
        env.PrintWarning('SWE Builder: ' + tracer_event_ids + \
                         ' must be placed in your BUILDPATH')

    return


def swe_builder(env, target, source):
    # Invoke call to autogenerate event ids header file.

    if 'USES_QDSS_SWE_MULTIPD' in env:
        env.SWEFinalBuilder(target, env.RealPath(
            '${IMAGE_ROOT}/${SHORT_BUILDPATH}/sweevent_master_list.txt'))
    else:
        env.SWEOldBuilder(target, None)

    return


def swe_final_builder(target, source, env):
    # Create the tracer sw event files (tracer_event_ids.h,
    # tracer_event_tbl.h, and tracer_event_info.txt)

    rootenv = env.get('IMAGE_ENV')
    swe_event_descriptor = []

    # Target tracer_event_ids argument MUST be passed
    try:
        tracer_event_ids = str(target[0])
    except:
        env.PrintError("SWE Builder: Final event file argument not passed")
        exit()
    try:
        tracer_event_ids_h = open(env.RealPath(tracer_event_ids), 'w')
    except IOError:
        env.PrintError("SWE Builder: Could not open tracer swe " + \
                       "ids file for write")
        exit()

    # Check folder location (path) for autogenerated files is valid.
    check_folder(tracer_event_ids, env)

    if 'USES_QDSS_SWE_MULTIPD' in env:
        # Source event list file(s) argument MUST be passed
        try:
            swe_event_source_file_path = str(source[0])
        except:
            env.PrintError("SWE Builder: Final event source file " + \
                           "argument not passed")
            exit()

        idx = 0
        try:
            with open(env.RealPath(swe_event_source_file_path), 'r') as input:
                for line in input:
                    entry = eval(line)
                    swe_event_descriptor.insert(idx, entry)
                    idx += 1
        except IOError:
            env.PrintError("SWE Builder: Could not open tracer swe " + \
                           "ids source file for read")
            exit()
    else:
        swe_event_descriptor = rootenv[SWE_ENV_DESCRIPTOR]

    # Compute CRC over the event names
    crc = binascii.crc32("QDSS") & 0xFFFFFFFF
    for item in swe_event_descriptor:
        crc = binascii.crc32(item[0], crc) & 0xFFFFFFFF

    # Set "subsys" according to the build being placed in.
    # Valid values: 'MODEM', 'ADSP', 'WCN', 'VENUS', 'RPM','SENSOR','AUDIO'
    # Obsoleted: 'PRONTO' replace with 'WCN'
    # Determine_subsystem is a default - if it cannot determine the image,
    # subsys can be hardcoded according to the build (subsys = 'MODEM').
    subsys = determine_subsystem(env)

    # Want these tags to physically appear each file.
    header_tags = '#ifndef EVENT_TABLE_TAG\n' \
                  '#define EVENT_TABLE_TAG \"{0}' + \
                      '0000' + format(crc, '08X') + '\"\n' \
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
        for item in swe_event_descriptor:
            tracer_event_ids_h.write('\t')
            tracer_event_ids_h.write(item[0])
            tracer_event_ids_h.write(',\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_ID_MAX,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_RESERVE_LAST=0x00FFFFFF,\n')
    tracer_event_ids_h.write('\tTRACER_EVENT_ALL=0xFFFFFFFF,\n')
    tracer_event_ids_h.write('};\n\n')

    maskVal = 0
    maskCnt = 1    #Account for TRACER_EVENT_RESERVE_0 event

    tracer_event_ids_h.write('#ifndef TRACER_EVENT_PREENABLE_SCONS\n')
    tracer_event_ids_h.write('#define TRACER_EVENT_PREENABLE_SCONS {\\\n')
    if 'USES_QDSS_SWE' in env:
        for item in swe_event_descriptor:
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
            env.PrintError("SWE Builder: Could not open tracer swe " + \
                           "tbl file for write")
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
            for item in swe_event_descriptor:
                tracer_event_tbl_h.write('\t{')
                tracer_event_tbl_h.write(item[0])
                tracer_event_tbl_h.write(',"')
                tracer_event_tbl_h.write(item[1])
                tracer_event_tbl_h.write('",\'')
                tracer_event_tbl_h.write(item[2].upper())
                tracer_event_tbl_h.write('\'},\n')
        tracer_event_tbl_h.write('\t{TRACER_EVENT_ID_MAX,"",\'F\'}\n')
        tracer_event_tbl_h.write('};\n\n')

        tracer_event_tbl_h.write('#endif // _'+ \
                                 event_table_filename.upper()+'_H\n\n')

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
            env.PrintError("SWE Builder: Could not open tracer swe " + \
                           "info file for write")
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

        tracer_event_info_txt.write(info_instruction_prefix + \
            '0 TRACER_EVENT_RESERVE_0 \"\" 0\n')
        if 'USES_QDSS_SWE' in env:
            for item in swe_event_descriptor:
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


def swe_replicate_builder(target, source, env):
    # Copy the final set of events (for example,
    # sweevent_consolidate_list.txt) back to the individual builds as a
    # master event list (for example, sweevent_master_list.txt).

    try:
        qdss_source_file = str(source[0])
    except:
        env.PrintError("SWE Builder: Replicate event source file " + \
                       "argument not passed")
        exit()

    # replicate_target_file_path argument MUST be passed
    try:
        qdss_target_file = str(target[0])
    except:
        env.PrintError("SWE Builder: Replicate event target file " + \
                       "argument not passed")
        exit()

    qdsstargetlists = map(lambda x: str(x), target)
    for qdsstargetlist in qdsstargetlists:
        try:
            shutil.copyfile(env.RealPath(qdss_source_file),
                            env.RealPath(qdsstargetlist))
        except:
            env.PrintError("SWE Builder: Could not open tracer swe " + \
                           "target file for write")
            exit()

    return


def find_insert_index(env, insertname, swe_event_descriptor):
    # The SW event list is sorted using only the prefix of the event's name.
    # Order is maintained within each prefix block. The same prefix
    # subsequently encountered is added to the end of that prefix block.
    # Returns the index where insertname is to be added to SW event list.

    rootenv = env.get('IMAGE_ENV')

    for idx, item in enumerate(swe_event_descriptor):
        # Sort by prefix only; ascending order; same prefix is added
        # to the end of the prefix block.
        if (item[0].split('_')[0] > insertname):
            break
    else: # Reached end of list
        idx = len(swe_event_descriptor)

    return idx


def swe_merge_builder(target, source, env):
    # Called by a bsp builder (for example, root_pd_img) to
    # consolidate all the events from the source files (for example,
    # the various sweevent_list.txt files that are being built together)
    # into one target file (for example, sweevent_consolidate_list.txt)
    # while maintaining the sort order.

    # consolidated_target_file_path argument MUST be passed
    try:
        consolidated_target_file_path = str(target[0])
    except:
        env.PrintError("SWE Builder: Merge event target file " + \
                       "argument not passed")
        exit()

    # Create target path if it doesn't exist
    target_path = os.path.split(env.RealPath(consolidated_target_file_path))[0]
    if not os.path.exists(target_path):
        if os.makedirs(target_path):
            raise

    # Check folder location (path) for autogenerated files is valid.
    check_folder(consolidated_target_file_path, env)

    try:
        consolidated_target_file = open(env.RealPath(
            consolidated_target_file_path), 'w')
    except IOError:
        env.PrintError("SWE Builder: Could not open tracer swe " + \
                       "consolidate file for write")
        exit()

    baseprefix = "" # Start
    idx = 0
    swe_event_descriptor=[]

    qdsslists = map(lambda x: str(x), source)
    for qdsslist in qdsslists:
        try:
            with open(env.RealPath(qdsslist), 'r') as input:
                for line in input:
                    entry = eval(line)
                    currentprefix = entry[0].split('_')[0]
                    if currentprefix != baseprefix:
                        idx = find_insert_index(env, currentprefix,
                                                swe_event_descriptor)
                        baseprefix = currentprefix
                    swe_event_descriptor.insert(idx, entry) # Add item to list
                    idx += 1 # Order maintained for same prefix

        except IOError:
            env.PrintError("SWE Builder: Could not open tracer swe " + \
                           "intermediate file for read")
            exit()

    for item in swe_event_descriptor:
        consolidated_target_file.write(str(item))
        consolidated_target_file.write('\n')

    consolidated_target_file.close()

    return None


def swe_save_list(target, source, env):
    # Called by a bsp builder (for example, core_root_libs) to save the
    # current set of events (from SWE_ENV_DESCRIPTOR) into the target file
    # (for example, sweevent_list.txt)

    rootenv = env.get('IMAGE_ENV')

    # qdss_list_path argument MUST be passed
    try:
        qdss_list_path = str(target[0])
    except:
        env.PrintError("SWE Builder: Save event file argument not passed")
        exit()

    # Check folder location (path) for autogenerated files is valid.
    check_folder(qdss_list_path, env)

    # Create target path if it doesn't exist
    target_path = os.path.split(env.RealPath(qdss_list_path))[0]
    if not os.path.exists(target_path):
        if os.makedirs(target_path):
            raise

    try:
        qdss_list_file = open(env.RealPath(qdss_list_path), 'w')
    except IOError:
        env.PrintError("SWE Builder: Could not open list file for write")
        exit()

    for item in rootenv[SWE_ENV_DESCRIPTOR]:
        qdss_list_file.write(str(item))
        qdss_list_file.write('\n')

    qdss_list_file.close()

    return


def swe_append_list(env, target, source):
    # When AddSWEInfo is called by the client, the list of SW event
    # name-desc-precfg triplets passed by the clients are stored
    # in a SWE_ENV_DESCRIPTOR list.

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
                idx = find_insert_index(env, currentprefix,
                                        rootenv[SWE_ENV_DESCRIPTOR])
                baseprefix = currentprefix

            rootenv[SWE_ENV_DESCRIPTOR].insert(idx, item) # Add item to list
            idx += 1 # Order maintained for same prefix

    return


