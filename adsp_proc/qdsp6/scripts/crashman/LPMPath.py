import sys
import re
inputfileName = sys.argv[1]
OutputPath    = sys.argv[2]

lpm = OutputPath+"\\"+"LPMPath.txt"
input = inputfileName.split("\\")
skip =  '\\'+ input[len(input)-1]
string = inputfileName.split(skip)
f_lpm = open(lpm,'w')
f_lpm.write(string[0])
f_lpm.close()
