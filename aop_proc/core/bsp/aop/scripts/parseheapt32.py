import re, itertools, sys

class Stack:
    """A single stack frame"""
    def __init__ (self, f):
        self.f = f
        self.frames = []

    def parse (self):
        while (True):
            line = f.readline ()
            if (line == ''):
                return False
            elif re.search ('B::v.f', line):
                break
        while (True):
            line = f.readline ()
            t = line.split ('|')
            m = re.search ('^-(\d+)', t[0])
            if (m):
                self.frames.insert (int(m.group (1)), t[1])
            elif (re.search ('^ ---', line)):
                return True

    def called_from (self, caller):
        for f in self.frames:
            if caller in f:
                return True
        return False

    def __str__ (self):
        s = ''
        for r in self.frames:
            s += r
        return s


class Register:
    """A single register dump"""
    def __init__ (self, f):
        self.f = f
        self.registers = dict ()

    def parse (self):
        while (True):
            line = f.readline ()
            if (line == ''):
                return False
            elif re.search ('B::register', line):
                break
        while (True):
            line = f.readline ()
            m = re.search ('R1 +([0-9a-fA-F]+)', line)
            if (m):
                self.registers['R1'] = int (m.group(1), 16)
                return True

    def val (self, r):
        return self.registers[r]

    def __str__ (self):
        return 'R1: ' + str (self.registers['R1'])

def read_stack (f):
    s = Stack (f)
    if (s.parse ()):
        stacks.append (s)
        return True
    return False

def read_registers (f):
    r = Register (f)
    if (r.parse ()):
        registers.append (r)
        return True
    return False

stacks = []
registers = []

if len(sys.argv) > 1:
    fname = sys.argv[1]
else:
    print 'Usage:', sys.argv[0], ' filename'
    sys.exit(1)

f = open (fname)

while (f):
    if not read_stack (f):
        break
    if not read_registers (f):
        break

f.close ()

heap_usage = dict ()
heap_usage['unknown'] = 0
heap_usage['total'] = 0

unknowns = []

function_map = {
    'resource_ee_request': 'rpmfw',
    'npa_init' : 'npa',
    'rpmserver_init' : 'rpmfw',
    'rpm_test_resource_init' : 'rpmfw',
    'railway_init' : 'railway',
    'pm_init' : 'pmic',
    'Clock_Init' : 'clock',
    'cb_init_cb' : 'buses',
    'sleep_init' : 'sleep',
    'rpm_ocmem_resource_init' : 'ocmem',
    'swevent_qdss_init' : 'qdss',
    'ddr_target_init' : 'boot',
    'railway_setup_sleep_handle' : 'railway',
    'rpmserver_register_handler' : 'rpmfw',
    'kvp_create' : 'rpmfw',
    'kvp_put' : 'rpmfw',
    'railway_create_voter' : 'railway'
    }

for i, j in itertools.izip (stacks, registers):
    found = False
    for k, v in function_map.items ():
        if i.called_from (k):
            if not heap_usage.get (v):
                heap_usage[v] = 0
            heap_usage[v] += j.val ('R1')
            found = True
            break

    if not found:
        unknowns.append (i)
        heap_usage['unknown'] += j.val ('R1')

    heap_usage['total'] += j.val ('R1')

print heap_usage

for u in unknowns:
    print u
