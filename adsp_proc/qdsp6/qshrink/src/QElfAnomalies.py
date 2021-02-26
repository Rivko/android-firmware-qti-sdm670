'''
=============================================================================

                  A R M E L F A N O M A L I E S . P Y

Copyright (c) 2010 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Elf modification anomaly analysis.
=============================================================================

=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order
$Header: //components/rel/dspcore.adsp/4.0/qshrink/src/QElfAnomalies.py#1 $
$Date: 2017/03/30 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/21/11   sh      QShrink20 commercial version release
08/10/10   kf      improved output to better match new ArmElfShrink
08/08/10   kf      adapted to refactored ArmElfShrink
08/06/10   kf      more readable output
07/28/10   kf      Initial Version: catalogs duplicate addresses, counts

=============================================================================
'''

import re
import csv

#=========================================================================
# Function : analyzeAnomalies 
#
# Arguments
#   anomalyListFile: Anomaly file as a list
#   dupeFile: Anomaly file as a .csv
#   errors : errors to be printed
#
# Description : Anomalies are logged in .csv format.
#   Anomalies : qsr string outside QSR_STR section
#
# Return Values:
#=========================================================================

def analyzeAnomalies(anomalyListFile, dupeFile, errors):
  entries = dict() #holds VA -> [list of entry items]
  counts = dict() #message text -> number of occurences
  
  try:
    invalidListReader = open(anomalyListFile, 'r')
  except IOError:
    print "Error: Cannot open Anomaly list file ", anomalyListFile
    exit(1)
  else:  
    csv.register_dialect('ours', delimiter=',', 
                            quoting=csv.QUOTE_MINIMAL, lineterminator='\n')
    invalidList = csv.DictReader(invalidListReader, dialect='ours')  
    try:
      dupeInfo = open(dupeFile, 'w')
    except IOError:
      print "Error: Cannot open Anomaly CSV file ", dupeFile
      invalidListReader.close()
      exit(1)
    else:
      headers = invalidList.fieldnames
      #      print headers
      
      StrOutRE = re.compile(r'qsr string outside QSR_STR section')
      
      for line in invalidList:
        
        #produce summary counts
        if line['Anomaly'] not in counts:
          counts[line['Anomaly']] = 1
        else:
          counts[line['Anomaly']] = counts[line['Anomaly']] + 1
        
        #structVA
        if ( line['structAddress'] != "" and 
            (StrOutRE.search(line['Anomaly']))):
          if ( line['structAddress'] not in entries):
            entries[line['structAddress']] = [line]
          else:
            entries[line['structAddress']].append(line)
        
        #end reading
      
      dupeInfo.write("Anomalous Structs processed by Elf Shrink:\n")
      for (k,v) in counts.items():
        dupeInfo.write("".join(["    " , str(k),": ",str(v),"\n"]))
      dupeInfo.write( "\n")

  
      dupeInfo.write("Entries by Anomalous structs: \n")
      sharedStructVAs, sharedPtrVAs, sharedStrVAs = 0,0,0
      for (k,v) in entries.items():
        dupeInfo.write("".join(["VA: " , k , "\n"]) )
        for entry in v:
          for (k,v) in entry.items():
            dupeInfo.write("".join(["    " , str(k),": ",str(v),"\n"]))
            dupeInfo.write( "\n")
          
      dupeInfo.write("\n\nREPORT:\n")
          
      dupeInfo.write("".join(["Total Errors (Investigate logs if not 0) :" , str(errors),"\n"]))

      invalidListReader.close()
      
      dupeInfo.close()  


if __name__ == '__main__':
  # debugging run
  analyzeAnomalies("a.txt_2011-03-01_17.27.06.csv", "dupes.txt", 5)
