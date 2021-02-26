#=============================================================================
#
#                                    trace_parser.py
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
#!/usr/bin/env python
import sys
import struct

class Data(object):
    def __init__(self, file):
        self.file = file
    def __getitem__(self, key):
        offset = key
        try:
            self.file.seek(offset*4)
        except:
            raise "Could not seek to offset %x (address %x)" % (offset, key)
        d = struct.unpack("I", self.file.read(4))
        return d[0]


# For using a binary dump of just the tracebuffer
data = Data(file(sys.argv[1], "rb"))
tlen = int(sys.argv[2])
hwt = int(sys.argv[3])

def readtag(tag):
    id =     0x000000ff &  (tag >> 16)
    len  =   0x0000007f &  (tag >> 25)
    timehi = 0x0000ffff &  (tag >> 0 )

    return (id, len, timehi)

def tp_context_switch(loc, data):
    fromt = data[loc]
    to=data[loc+1]
    to_str = "%08x"%to
    from_str = "%08x"%fromt
    print "From %s -> To: %s"%( from_str, to_str)

def tp_interrupt(loc, data):
    int_number = data[loc]
    print "Interrupt %d"%( int_number)

def tp_futex_wait(loc, data):
    addr = data[loc]
    exp_value = data[loc+1]
    print "Futex_wait addr 0x%x expexted value 0x%x"%( addr, exp_value )

def tp_futex_resume(loc, data):
    addr = data[loc]
    no_to_wakeup = data[loc+1]
    print "Futex_resume addr 0x%x no. to wakeup 0x%x"%( addr, no_to_wakeup)

def tp_reschedule_bitmask(loc, data):
    tcb = data[loc]
    print "QURTK_reschedule_bitmask TCB 0x%x "%( tcb )

def tp_schedule_new_fromsleep(loc, data):
    tcb = data[loc]
    print "QURTK_schedule_new_fromsleep TCB 0x%x "%( tcb )

def tp_schedule_new(loc, data):
    tcb = data[loc]
    print "QURTK_schedule_new TCB 0x%x "%( tcb )

def tp_schedule_highest(loc, data):
    print "QURTK_schedule_highest"

def tp_reschedule_from_lowprio(loc, data):
    print "QURTK_reschedule_from_lowprio"

def tp_reschedule_from_wait(loc, data):
    print "QURTK_reschedule_from_wait"

def tp_prio_set(loc, data):
    tcb = data[loc]
    prio = data[loc+1]
    print "QURTK_prio_set dest %x new_prio %x"%(tcb,prio)

def tp_change_prio_no_resched(loc, data):
    print "change_prio NO resched"

def tp_change_prio_resched(loc, data):
    print "change_prio WITH resched"

traceids = {
	"0" : ("CONTEXT_SWITCH", tp_context_switch),
	"1" : ("INTERRUPT", tp_interrupt),
	"2" : ("FUTEX_WAIT", tp_futex_wait),
	"3" : ("FUTEX_RESUME", tp_futex_resume),
	"31" : ("QURTK_RESCHEDULE_BITMASK", tp_reschedule_bitmask),
	"30" : ("QURTK_SCHEDULE_NEW_FROMSLEEP", tp_schedule_new_fromsleep),
	"29" : ("QURTK_SCHEDULE_NEW", tp_schedule_new),
	"28" : ("QURTK_SCHEDULE_HIGHEST", tp_schedule_highest),
	"27" : ("QURTK_RESCHEDULE_FROM_LOWPRIO", tp_reschedule_from_lowprio),
	"26" : ("QURTK_RESCHEDULE_FROM_WAIT", tp_reschedule_from_wait),
	"25" : ("QURTK_PRIO_SET", tp_prio_set),
	"24" : ("QURTK_CHANGE_PRIO_NO_RESCHEDULE", tp_change_prio_no_resched),
	"23" : ("QURTK_CHANGE_PRIO_RESCHEDULE", tp_change_prio_resched),
}

def printrec(loc, data):
        start = loc
        tag = data[loc+1] 
	(id, len, timehi) = readtag(tag)
        if( len == 0 ):
            return 0
	timelo = data[loc]
	#timehi = data[loc+3]
	loc += 2

	time = timelo + (timehi<<32)

#	print "	(id, user, len, args, major, minor) = ", (id, user, len, args, major, minor)

        trace = traceids["%s"%id]
	func = trace[1]
	name = trace[0]

        print "%010x"% time,
        print "   TNUM: %d  " % hwt,
	print "%-15ls" % trace[0]," ", 
	if func == None:
	    for a in range(0, len-4):
		print "%08x "%( data[loc]),
		loc += 1
	    print
	else:
	    func(loc, data)

	return len


offset = 0
#while offset < len:
#    print "%x"%data[offset]
#    offset += 4

while(True):
    #print "offset:%d"%offset
    len = printrec(offset, data)
    if( len == 0 ):
        break
    else:
        offset += len
    if( tlen <= offset):
        break

