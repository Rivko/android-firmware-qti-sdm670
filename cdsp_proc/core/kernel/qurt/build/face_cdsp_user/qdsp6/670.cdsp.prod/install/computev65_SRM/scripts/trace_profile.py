#=============================================================================
#
#                                    trace_profile.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
#!/pkg/python/2.4.3/bin/python

#!/usr/bin/env python

import sys
from operator import itemgetter
from optparse import OptionParser

#python trace_profile.py end-pcycle start-pcycle trace_file

print "Usage: python trace_profile.py end-pcycle-num start-pcycle-num trace_file"

def thread_profile(end_pcycles, start_pcycles, trace_file):
    context_dict = {}
    total_pcycles = 0

    #flags for HW thread first context switch time adjustment
    first_switch_ts = [0,0,0,0,0,0]

    #read input from trace file
    f = open(trace_file, "r");
    lines = f.readlines()
    f.close()

    if start_pcycles == None:
        for line in reversed(lines):
            list = line.split()
            start_pcycles = int(list[0], 16)
            break

    end_pcycles = int(end_pcycles, 16)

    for line in lines:
        cur_list = []
        list = line.split()
        if list[2:4] == ['Context', 'Switch']:
            for item in list:
                cur_list.append(item)
            #cur_list[0]: pcycles
            #cur_list[1]: TNUM
            #cur_list[4]: running pcycles
            #cur_list[5]: thread name
            if int(cur_list[0], 16) > end_pcycles or int(cur_list[0], 16) < start_pcycles:
                continue
                
            if first_switch_ts[int(cur_list[1])] == 0:
                #last context switch, the run time is not correct, update it from the end-pcycle-num
                first_switch_ts[int(cur_list[1])] = 0xfe
                val = end_pcycles - int(cur_list[0], 16)
            else:
                val = int(cur_list[4], 16)

            if not context_dict.has_key(cur_list[5]):
                context_dict[cur_list[5]] = int(0)

            context_dict[cur_list[5]] = val + context_dict[cur_list[5]]


    sort_list = sorted(context_dict.iteritems(), key=itemgetter(1), reverse=True)

    for key in context_dict.keys():
        total_pcycles = total_pcycles + context_dict[key]

    print "======================================================================================================="
    print "|","Thread name".ljust(32),"|","Pcycle number".ljust(32),"|","Percentage of total run time"
    print "-------------------------------------------------------------------------------------------------------"
    
    for item in sort_list:
        print "|", str(item[0]).ljust(32), "|", str(item[1]).ljust(32), "|", str(float(item[1]*100)/float(total_pcycles)),"%"

def main():
    parser = OptionParser()
    parser.add_option("-s", "--start_pcycles", dest="start_pcycles", help="pcycle number in hexadecimal where the profiling starts, OPTIONAL")
    parser.add_option("-e", "--end_pcycles", dest="end_pcycles", help="pcycle number in hexadecimal where profiling stops, REQUIRED")
    parser.add_option("-t", "--trace_file", dest="trace_file", help="the formatted trace dump from T32 \"Bx\" or \"MB\" window, REQUIRED")

    (options, args) = parser.parse_args()
    #print options

    if options.end_pcycles == None or options.trace_file == None:
        parser.print_help()
    else:
        thread_profile(options.end_pcycles, options.start_pcycles, options.trace_file) 

if __name__ == "__main__":
    main()
                    
