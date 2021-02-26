#! /pkg/python/2.4.3/bin/python

#####################################################################
# Although gprof has a keyword to sum all the gmon files 
# into a summary this feature is painfully slow. 
# This script provides an ability to merge the output 
# of the parsed gmon files into a single output
#####################################################################

# Import relevant modules
import array
import os
import re
import sys
import subprocess
from optparse import OptionParser

# The marker is present in the gprof output
# This is used to identify end of the profile data section
marker = re.compile('^$')


#####################################################################
# Helper function to update dictionary key/value pairs
#####################################################################
def UpdateKeyValue(dict, key, value):
   if key in dict:
      oldval = float(dict.setdefault(key, value))
      newval = oldval + value
      dict[key] = newval 
   else:
      dict.setdefault(key, value)

def UpdateKeyValueString(dict, key, value):
   if key in dict:
      oldval = dict.setdefault(key, value)
      if oldval.find(value) == -1:
         newval = oldval + '?#' + value
         dict[key] = newval
   else:
      dict.setdefault(key, value)
      

# Initialize the dictionary
mcpsdict = {}
algmcpsdict = {}
algnamedict = {}
dcachedict = {}
icachedict = {}
l2cachedict = {}

#####################################################################
# The Main function
#####################################################################
def main():

    Usage = '''
.....................................................................

This function parses all gmon files and prints a summary in vprof.csv

Default bootimg as ./obj/qdsp6v3_ReleaseG/bootimg.pbn. 
Use -e option to specify a different executable.
Use -n option to input parsed gmon files

%prog [-h|--help] duration infile(s) [-e executable] [-n]
   duration = duration of the test in millisecs
   infile(s) = output(s) of gprof
              
a) Execute sim        : qdsp6-sim --profile "other parameters"
b) Execute Script     : {path to}/vprof.py duration gmon*
c) Output             : vprof.csv, vprofbin.csv, vprofdcache.csv, vproficache.csv, vprofl2cache.csv in the same directory

Limitation: Can be used only in linux/unix

.....................................................................'''

    parser = OptionParser(Usage)
    parser.add_option("-e", "--exe", dest="exename", help="specify executable other than default", metavar="exe")
    parser.add_option("-n", "--noprof", dest="prof", action="store_false", default=True, help="specify not to run gprof (input should be parsed gmon files)")

    (options,args) = parser.parse_args()
    print "....................................................................."

    if len(args) == 0:
       parser.print_help()
       sys.exit()
    else:
       print "Starting parsing...."
       op = open("vprof.csv","w")
       opd = open("vprofdcache.csv","w")
       opi = open("vproficache.csv","w")
       opl2 = open("vprofl2cache.csv","w")
       opa = open("vprofbin.csv","w")

    i = 0

    duration = float(args[i])
    print "Duration of profile data", duration/1000, "seconds"
    totalMcps = 0
    totalDcache = 0
    totalIcache = 0
    totalL2cache = 0

    if options.exename == None:
       exe = './obj/qdsp6v3_ReleaseG/bootimg.pbn'
    else:
       exe = options.exename

    print "Using executable", exe

    i = 1
    if options.prof == True:
      print "Parsing gmon files", len(args)-1
      while i < len(args):
         shellcommand = 'qdsp6-gprof -p ' + exe + ' ' + args[i] + ' > m_' + args[i] + '.txt 2> status.txt'
         print shellcommand
         result = subprocess.call(shellcommand, shell=True, executable='/bin/bash') 
         if result != 0:
           print "operation failed"
           sys.exit()
         i = i + 1

#      For future parallelization of the gprof
#      print "Waiting for parsing to complete..."
#      subprocess.call('export USER=`whoami`; export JOBS=`ps -u $USER | grep qdsp6-gprof | gawk \'{print $1}\'`; for job in $JOBS; do wait $job; done',shell=False, executable='/bin/bash') 
#      print "Done"

    i = 1
    # Iterate over all the input files
    while i < len(args):
       if options.prof == True:
          infile = 'm_' + args[i] + '.txt'
       else:
          infile = args[i]

       i = i + 1

       try:
          inp = open(infile,"r")

          linecount = 0
          for line in inp.readlines():
             linecount = linecount + 1

             # Skip first few lines
             if linecount <= 5: 
                if linecount == 3:
                   if re.search('\*',line):
                      scalefactor = float(((line.split(' '))[5]).split('*')[0])
                   else:
                      scalefactor = 1
                   print infile, scalefactor 
                continue

             # If marker matches we are done with the profile data
             if marker.match(line):
                break


             shortline = re.sub('^',' ',line)

             # replace the space(s) with one space
             shortline = re.sub('([\s\t]+)',' ',shortline)

             # get corresponding function name
             name = re.sub('^([\s\t.0-9]+)([\s\t]{1})','',line.strip())
             name = re.sub('(\([^0-9].*\))','',name)

             # Add to dictionary
             splitline = shortline.split(' ')

             if re.search('gmon.dcache',infile) != None:
                 dcachemiss = float(splitline[3])
                 totalDcache = totalDcache + dcachemiss
                 UpdateKeyValue(dcachedict, name, dcachemiss)
                 
             if re.search('gmon.icache',infile) != None:
                 icachemiss = float(splitline[3])
                 totalIcache = totalIcache + icachemiss
                 UpdateKeyValue(icachedict, name, icachemiss)

             if re.search('gmon.l2cache',infile) != None:
                 l2cachemiss = float(splitline[3])
                 totalL2cache = totalL2cache + l2cachemiss
                 UpdateKeyValue(l2cachedict, name, l2cachemiss)

             if re.search('gmon.t',infile) != None:
                 mcps = float(splitline[3])/(1000*duration)
                 totalMcps = totalMcps + mcps 
                 UpdateKeyValue(mcpsdict, name, mcps)

                 # Remove starting spaces 
                 morphname = re.sub('^[\s]*','',name)
                 # Remove starting :: 
                 morphname = re.sub('^[\s]*::','',name)

                 # Replace all :: with _ for splitting
                 morphname = re.sub('::','_',morphname)

                 # Replace all starting . with ._ for grouping to .
                 morphname = re.sub('^_','stdlibs_',morphname)

                 # Replace all starting . with ._ for grouping to .
                 morphname = re.sub('^\.','._',morphname)

                 alg = morphname.split('_')[0]

                 if re.search('_', morphname) == None:
                    alg = 'Misc'

                 UpdateKeyValue(algmcpsdict, alg , mcps)
                 UpdateKeyValueString(algnamedict, alg , name)
                    


       except IOError: 
          print "script failed"

#####################################################################
#  At this stage the dictionary is populated with  functions and cycles
#  Sort it and print to output file
#####################################################################

    print >>op, "-------------- Summary ---------------------------------------------"
    print >>op, "Total MCPS, %f"  % totalMcps
    print >>op, ""
    print >>op, "---------------Details : Sorted by MCPS ----------------------------"

    sortdict = sorted(mcpsdict.items(), key=lambda(k,v):(v,k), reverse=True)
    print >>op, "Function Name,   MCPS"
    print >>op, "-------------------------------------------"
    for inp in sortdict:
       print >>op, "%s,%f" % (inp)

    sortdict = sorted(mcpsdict.items())
    print >>op, ""
    print >>op, "---------------Details : Sorted by name ----------------------------"
    print >>op, "Function Name,   MCPS"
    print >>op, "-------------------------------------------"
    for inp in sortdict:
       print >>op, "%s,%f" % (inp)

    print >>opa, "-------------- Summary ---------------------------------------------"
    print >>opa, "Total MCPS, %f"  % totalMcps
    print >>opa, ""
    print >>opa, "---------------Details : Sorted by MCPS ----------------------------"

    sortdict = sorted(algmcpsdict.items(), key=lambda(k,v):(v,k), reverse=True)
    print >>opa, "Bin Name,   MCPS"
    print >>opa, "-------------------------------------------"
    for inp in sortdict:
       print >>opa, "%s,%f" % (inp)

    sortdict = sorted(algnamedict.items())
    print >>opa, ""
    print >>opa, "---------------Details : Sorted by bin ----------------------------"
    for inp in sortdict:
       print >>opa, "%s:" % (inp[0])
       names = inp[1].split('?#') 
       for func in names:
          print >>opa, "   %s" % (func)

    print >>opl2, "-------------- Summary ---------------------------------------------"
    print >>opl2, "Total L2CACHE, %f"  % totalL2cache
    print >>opl2, ""
    print >>opl2, "---------------Details : Sorted by L2CACHE ----------------------------"

    sortdict = sorted(l2cachedict.items(), key=lambda(k,v):(v,k), reverse=True)
    print >>opl2, "Function Name,   L2CACHE"
    print >>opl2, "-------------------------------------------"
    for inp in sortdict:
       print >>opl2, "%s,%f" % (inp)

    sortdict = sorted(l2cachedict.items())
    print >>opl2, ""
    print >>opl2, "---------------Details : Sorted by name ----------------------------"
    print >>opl2, "Function Name,   L2CACHE"
    print >>opl2, "-------------------------------------------"
    for inp in sortdict:
       print >>opl2, "%s,%f" % (inp)

    print >>opd, "-------------- Summary ---------------------------------------------"
    print >>opd, "Total DCACHE, %f"  % totalDcache
    print >>opd, ""
    print >>opd, "---------------Details : Sorted by DCACHE ----------------------------"

    sortdict = sorted(dcachedict.items(), key=lambda(k,v):(v,k), reverse=True)
    print >>opd, "Function Name,   DCACHE"
    print >>opd, "-------------------------------------------"
    for inp in sortdict:
       print >>opd, "%s,%f" % (inp)

    sortdict = sorted(dcachedict.items())
    print >>opd, ""
    print >>opd, "---------------Details : Sorted by name ----------------------------"
    print >>opd, "Function Name,   DCACHE"
    print >>opd, "-------------------------------------------"
    for inp in sortdict:
       print >>opd, "%s,%f" % (inp)

    print >>opi, "-------------- Summary ---------------------------------------------"
    print >>opi, "Total ICACHE, %f"  % totalIcache
    print >>opi, ""
    print >>opi, "---------------Details : Sorted by ICACHE ----------------------------"

    sortdict = sorted(icachedict.items(), key=lambda(k,v):(v,k), reverse=True)
    print >>opi, "Function Name,   ICACHE"
    print >>opi, "-------------------------------------------"
    for inp in sortdict:
       print >>opi, "%s,%f" % (inp)

    sortdict = sorted(icachedict.items())
    print >>opi, ""
    print >>opi, "---------------Details : Sorted by name ----------------------------"
    print >>opi, "Function Name,   ICACHE"
    print >>opi, "-------------------------------------------"
    for inp in sortdict:
       print >>opi, "%s,%f" % (inp)




#####################################################################
# For a standalone script following is needed
#####################################################################
if __name__ == "__main__":
   main()
