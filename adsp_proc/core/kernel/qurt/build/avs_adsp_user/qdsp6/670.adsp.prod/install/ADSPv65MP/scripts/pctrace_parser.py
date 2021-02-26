#=============================================================================
#
#                                    pctrace_parser.py
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
import os
import re
import bsddb
import getopt
import os.path
from optparse import OptionParser
#import create_sym_db


unique_suffix=""

def extract_hw(trace):
    threads={} 

#    soft_threads = {}

    f = open( trace )
    
    thread = ""

    i = 0
    p_line = ""
    pcycle = 0
    for line in f:
        i+=1
        if( ((i) % 100000) == 0 ):
            print i
#            #print >> threads[thread], line,
        m = re.search( r'TNUM:(\d+) PC_VA:0x(\S+).*PCycle:(\d+)', line )
        if m:
            pcycle = int(m.group(3))

        m = re.search( r'TNUM:([0-9]+)\s+.*', line )
        if m:
            thread = m.group(1)
            if thread not in threads.keys():
                threads[thread] = open( "%s.hw%s"%(trace, thread), "w" )

            p_line = line

        m = re.search( r'ugp: 0x([0-9abcdef]+)\s+.*', line )
        if m:
            ugp = m.group(1)
            if threads.has_key(thread):
                #print "ugp:%s"%ugp
                print >> threads[thread], "%s:%s"%(ugp,p_line),
                #print >> threads[thread], line,
            else:
                print "no thrads for ugp:%s"%ugp
    
 #           if soft_threads.has_key(ugp):
 #               #print "%s:%s"%(thread,line)
 #               print >> soft_threads[ugp], "%s:%s"%(ugp,p_line),
                #print >> threads[thread], line,
 #           else:
 #               soft_threads[ugp] = open( "%s.t_%s"%(trace,ugp), "w" )
 #               print >> soft_threads[ugp], "%s:%s"%(ugp,p_line),

    for i in threads.keys():
        threads[i].close()
    
    f.close()



def create_sym_db( exe_files ):
    path_name = os.path.dirname(exe_files[0])
    cur_dir = 0

    print exe_files;

    if len(path_name) == 0 or path_name == '.':
        cur_dir = 1

    global unique_suffix

    if cur_dir == 1:
        files = [ "%s.dump.%s"%(os.path.basename(f), unique_suffix) for f in exe_files ]
    else:
        files = [ "%s/%s.dump.%s"%(path_name, os.path.basename(f), unique_suffix) for f in exe_files ]
        
    image_names = [ "%s"%(os.path.basename(f)) for f in exe_files ]

    for i in range(len(exe_files)):
        os.system( "qdsp6-objdump -d %s > %s"%(exe_files[i], files[i] ))

    if cur_dir == 1:
        db = bsddb.btopen('symbol.db.%s'%(unique_suffix), 'c')
    else:
        db = bsddb.btopen('%s/symbol.db.%s'%(path_name, unique_suffix), 'c')

    for i in range( len(files) ):
        print files[i] 

        f_sym = open( files[i] )
        
        sym_file_name = image_names[i] 

        #global fun_name
        fun_name = "unknown"
        entry_addr = "unknown"
        for line in f_sym:
            #print line
            if re.match( r'^$', line ) or re.match( r'\s+\.\.\.', line) or re.match ( r'Disassembly', line):
                #end of a function
                fun_name = "unknown"
                entry_addr = "unknown"
                 

            m = re.search( r'^([0-9abcdef]+):\s+.*', line )
            if m:
                addr = m.group(1)
                if( entry_addr == "unknown" ):
                    entry_addr = addr

                db[addr] = "%s;%s;%s"%(sym_file_name, fun_name, entry_addr) 

            m = re.search( r'<(.*)>:', line )
            if m:
                fun_name = m.group(1)
                #print fun_name

    
        f_sym.close()
        os.system( "rm -f %s"%files[i] )

#    for k,v in db.iteritems():
#        print k, v

    db.close()


def vararg_callback(option, opt_str, value, parser):
    assert value is None
    done = 0
    value = []
    rargs = parser.rargs
    while rargs:
        arg = rargs[0]

        # Stop if we hit an arg like "--foo", "-a", "-fx", "--file=f",
        # etc.  Note that this also stops on "-3" or "-3.0", so if
        # your option takes numeric values, you will need to handle
        # this.
        if ((arg[:2] == "--" and len(arg) > 2) or
            (arg[:1] == "-" and len(arg) > 1 and arg[1] != "-")):
            break
        else:
            value.append(arg)
            del rargs[0]

    setattr(parser.values, option.dest, value)

def prof( symbol_db, trace ):

#    print symbol_db
    
    f = open( trace )
    db = bsddb.btopen(symbol_db, 'r')
    fout = open ( "%s.fun_out"%trace, "w" )
    
    hw="-1"
    thread = "unkonwn"
    fun_name = "unknown;unknown"
    fun_count = 0
    start_cycle = 0
    i = 0
    printed=False
    pcycle = 0 

    for line in f:
        i += 1
        if( ((i) % 100000) == 0 ):
            print i
        m =  re.search( r'([0-9abcdef]+):TNUM:(\d+).*PC_VA:0x0*(.*) PC_PA.*PCycle:(\d+)', line )
        if m:
            thread = m.group(1)
            hw = m.group(2)
            addr = m.group(3)
            pcycle = m.group(4)
        else:
            continue
    

        #print >>fout, line,
        if( db.has_key(addr) == 1 ):
            if( fun_name == db[addr] ):
                fun_count+=1
                printed = False
            else:
                if start_cycle != 0:
                    print >>fout, "%s;hw%s;%s;%d;%d;%d"%(thread, hw, fun_name, fun_count, start_cycle, (int(pcycle)-3)) 
                fout.flush()
                printed = True

                fun_name = db[addr]
                fun_count = 1
                start_cycle = int(pcycle)
        else:
            print "%s doesn't have corresponding fun"%addr
            print >>fout, "unknown(%s);1"%addr

    if not printed:
        # Take care of the last fun_name
        if start_cycle != 0:
            print >>fout, "%s;hw%s;%s;%d;%d;%d"%(thread, hw, fun_name, fun_count, start_cycle, (int(pcycle)-3)) 

    f.close()
    fout.close()

    db.close()


def main():
    parser = OptionParser()
    parser.add_option("-t", "--trace_file",
                      dest="trace_file")
    parser.add_option("-s", "--symbold_file",
                      action="callback", dest="files", callback=vararg_callback)
    parser.add_option("-m", "--max_hthreads",
                      dest="max_hthreads")

    (options, args) = parser.parse_args()
    print options

    global unique_suffix
    unique_suffix="%s.%s"%(os.popen("hostname").read().strip(),os.getpid())

    if( options.trace_file != None ):
        extract_hw(options.trace_file)
    else:
        print "-t trace_file is missing"
        sys.exit(1)

    if( options.max_hthreads != None):
        max_hthreads = options.max_hthreads
    else:
        max_hthreads = 3

    create_sym_db( options.files )
    path_name = os.path.dirname(options.files[0])
    real_path = "unknown";
    
    if len(path_name) == 0 or path_name == '.':
        real_path = "symbol.db.%s"%(unique_suffix)
    else:
        real_path = "%s/symbol.db.%s"%(path_name, unique_suffix)
    
    
    all_traces = "" 
    for thread in range(int(max_hthreads)):
        ht_trace = "%s.hw%s"%(options.trace_file, thread)
        all_traces += "%s.fun_out "%ht_trace
        print "processing %s"%ht_trace
        prof( real_path , ht_trace)

    os.system("rm -f %s"%real_path)
    os.system("sort -n -t \";\" -k 7 %s -o trace.fun_out"%(all_traces))
    sys.exit(0)

if __name__ == "__main__":
    main()
