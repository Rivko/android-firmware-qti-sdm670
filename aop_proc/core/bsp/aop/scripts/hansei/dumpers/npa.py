#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.  
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import logging
logger = logging.getLogger(__name__)

import os
import itertools
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object
from hansei_utils import *

dump_error = 0 # should only be used for error that indicate dump corruption

#
# Similar to the ULog dump scripts, this is copied directly from the rpm_npadump.cmm.
#
def dump(debug_data): #dump_path, memory, debug_info):
    global dump_error
    dump_error = 0
    memory = debug_data['rpm_memory']
    debug_info = debug_data['debug_info']
    dump_path = debug_data['dump_path']
    reclaim = debug_data['reclaim_pool']
    
    debug_data['printer'].p("Dumping NPA state...")

    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['aop_core_dump'].vartype
    aop_core_dump = decode_object('aop_core_dump', address, dump_type, memory, debug_info)

    unpa_type = debug_info.variables['unpa'].die
    unpa_address = debug_info.variables['unpa'].address
    unpa = decode_object('unpa', unpa_address, None, memory, debug_info, die=unpa_type)

    dump_file = open(os.path.join(dump_path, 'npa-dump.txt'), 'w')

    link = unpa.resources
    while link.address != 0:
        dump_link(dump_file, link, reclaim)
        link = link.next
    
    if dump_error != 0:
      debug_data['printer'].p("\t...Non-critical errors occured in processing dump, continuing")
    else:
      debug_data['printer'].p("\t...DONE")

def get_name(item, mem):
  if item.address > mem['start_addr'] and item.address < mem['start_addr']+mem['size']:
    name = pointer_to_str(mem['memory'].read(item.address), 100) #assume there are no names with more than 100 characters
    #name += "== recovered =="
  else:
    name  = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in item)))
  return name
        
def dump_link(dump_file, link, reclaim):
    #import pdb; pdb.set_trace()
    #link_name  = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in link.name)))
    #link_name = get_name(link.name, reclaim)
    resource   = link
    definition = resource.definition
    resource_name = get_name(definition.name, reclaim)

    if definition.address == 0:
        dump_placeholder(dump_file, resource_name)
    else:
        #resource_name = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in definition.name)))
        resource_name = get_name(definition.name, reclaim)
        dump_resource(dump_file, resource, resource_name, reclaim)


def dump_placeholder(dump_file, link_name):
    print('npa_placeholder (name: "%s")' % link_name, file=dump_file)

def dump_resource(dump_file, resource, resource_name, reclaim):
    dump_resource_state(dump_file, resource, resource_name, reclaim)
    dump_resource_clients(dump_file, resource, reclaim)

    print('        end npa_resource (handle: 0x%x)\n\n' % resource.address, file=dump_file)

def dump_resource_state(dump_file, resource, resource_name, reclaim):
    global dump_error
    try:
      #units = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in resource.definition.units)))
      #units           = get_name(resource.definition.units, reclaim)
      resource_max    = resource.definition.max
      active_state    = resource.active_state
      active_max      = resource.active_max
      #active_headroom = resource.active_headroom
      sleep_state   = resource.sleep_state
      error           = ""
    except:
      error = "\n\tAn error occured processing this resource"
      dump_error += 1
      #import pdb; pdb.set_trace()

    dumpstr = 'npa_resource (name: "{0}") (handle: {1}) (resource max: {2}) (active max: {3}) (active state: {4}) (sleep state: {5}) {6}'
    dumpdat = [resource_name, hex(resource.address), resource_max, active_max, active_state, sleep_state, error]
    print(dumpstr.format(*dumpdat), file=dump_file)

def dump_resource_clients(dump_file, resource, reclaim):
    global dump_error
    client = resource.clients

    while client.address != 0:
        try:
          #client_name = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in client.name)))
          client_name = get_name(client.name, reclaim)
        except:
          #import pdb; pdb.set_trace()
          client_name = "[ERROR: INVALID POINTER]"
          dump_error += 1
        try:
          client_type = client.type
          active_request = client.active_request.val
          pending_request = client.pending_request.val
          
          string = '        npa_client (name: {0[0]}) (handle: {0[1]}) (resource: {0[2]}) (type: {0[3]}) (active_request: {0[4]}) (pending_request: {0[5]})'
          dat = (client_name, hex(client.address), hex(resource.address), client_type, active_request,pending_request )
          print(string.format(dat), file=dump_file)
          client = client.next
        except:
          print("        ERROR: CLIENT IS CORRUPT, SKIPPING", file=dump_file)
          dump_error += 1
          break
