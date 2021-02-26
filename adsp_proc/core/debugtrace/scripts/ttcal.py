#!/usr/bin/env python
from __future__ import print_function
from collections import defaultdict
import getopt
import re
import sys 
import json


class ArgParser:
    input_file=""
    tabular_list=False
    ignore_before_sync=False
    show_drift=False
     
    @classmethod 
    def usage(cls):
        print(
            """
Usage: python ttcal.py [-t] json_file

This script corrects QDSS timestamps in the input json_file based on
uTimetick values logged in QDSS_SSC_TIMETICK event. If enabled from QTF,
QDSS_SSC_TIMETICK event is logged right before every software event.
After the correction is applied delta between any two events as reported
by QDSS timestamps would be the same as that obtained using timetick vales.

Options:  -t   This option can be used to produce a tabular listing

            """,
            file=sys.stderr)
        exit(1)
     
    @classmethod 
    def parse(cls):
        opts, args = getopt.getopt(sys.argv[1:], "std")
        for o, a in opts:
            if o == "-t":
                cls.tabular_list=True
            if o == "-s":
                cls.ignore_before_sync=True
            if o == "-d":
                cls.show_drift=True
             
        if (len(args) < 1):
            print("JSON file not specified",file=sys.stderr)
            cls.usage()
        else:
            cls.input_file=args[0]


class SyncChecker:
    ts_sync_pattern=re.compile("QDSS SSC timestamp sync at (\d+)")
    sync_tag_map=defaultdict(int)

    @classmethod
    def check_sync(cls,line,packet):
        if "QDSS_SSC_TS_SYNC" in line:
            event_data = cls.ts_sync_pattern.search(packet['SWEventA']['Data'])
            sync_tag=int(event_data.group(1))
            if (cls.sync_tag_map[sync_tag]):
                return True
            cls.sync_tag_map[sync_tag]=1
            return False
        return False

        
                               
class Calibrator:
    total_drift  = 0
    first_timetick  = 0
    first_qdss_ts = 0
    last_qdss_ts = 0
    timetick_pattern=re.compile("Event \[0x[0-9a-fA-F]+\] uTimetick = (\d+)")
  
    @classmethod
    def calibrate(cls):
        syncpoint_found = True
        if (ArgParser.ignore_before_sync):
            syncpoint_found = False
        with open(ArgParser.input_file) as json_file:
            for line in json_file:
                try:
                    packet = json.loads(unicode(line.strip()))
                except:
                    continue
                if (not syncpoint_found):
                    syncpoint_found=SyncChecker.check_sync(line,packet)
                    continue
                try:
                    this_qdss_ts = int(packet['STP']['Timestamp'])
                except:
                    this_qdss_ts = cls.last_qdss_ts
                
                if  (0==this_qdss_ts):
                    this_qdss_ts = cls.last_qdss_ts
         
                if "QDSS_SSC_TIMETICK" in line:
                    event_data = cls.timetick_pattern.search(packet['SWEventA']['Data'])
                    this_timetick=int(event_data.group(1))
                    if (cls.first_timetick!=0):
                        timetick_delta_from_first = int ((this_timetick - cls.first_timetick) * 1000/19.2)
                        qdss_ts_delta_from_first = this_qdss_ts - cls.first_qdss_ts
                        cls.total_drift =  timetick_delta_from_first - qdss_ts_delta_from_first
                    else:
                        cls.first_timetick = this_timetick
                        cls.first_qdss_ts = this_qdss_ts
       
              
                updated_qdss_ts = this_qdss_ts + cls.total_drift
                cls.last_qdss_ts = updated_qdss_ts
                packet['STP']['Timestamp'] = str(updated_qdss_ts)
       
                if (ArgParser.tabular_list):
                    try:
                        timestamp=packet['STP']['Timestamp']
                        packet_type=packet['STP']['Type']
                        source=packet['STP']['Source']
                        if 'SWEvent' in packet_type:
                            payload="%s[0x%x]: %s" % (packet[packet_type]["Name"],int(packet[packet_type]["Id"]),packet[packet_type]['Data'])
                        elif 'HWEvent' in packet_type:
                            payload="%s[0x%x]" % (packet[packet_type][0]["Name"],int(packet[packet_type][0]["Id"]))
                        else:
                            payload=''
       
                        print("%15s %15s %15s       %s" %
                              (timestamp,
                               packet_type,
                               source,
                               payload))
                    except:
                        continue
                else:
                    print(json.dumps(packet))
                if (ArgParser.show_drift):
                    print("Drift= %d" % (cls.total_drift))

if __name__ == '__main__':
    ArgParser.parse()
    Calibrator.calibrate()


