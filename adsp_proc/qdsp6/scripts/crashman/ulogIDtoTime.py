import sys
import re
import os
import fileinput

if len(sys.argv)!=2:
    print "Usage: ulogIDtoTime.py Input_file_name"
    sys.exit()

file_name = sys.argv[1]
def convert(hex_value):
    mili_seconds = int(hex_value,0)/19200
    seconds, mili = divmod(mili_seconds, 1000)
    m, s = divmod(seconds, 60)
    h, m = divmod(m, 60)
    str = "%02d:%02d:%02d.%03d" % (h, m, s, mili)
    return str 


hexaPattern = re.compile(r'(0[xX]([0-9a-fA-F]+)(:))')

for line in fileinput.input([file_name], inplace=True):
    m = re.search(hexaPattern,line)
    if m:
        s=convert(m.groups(0)[0][0:-1])
        line = re.sub(hexaPattern, s, line)
    sys.stdout.write(line)
