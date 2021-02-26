'''
=============================================================================

                          Q S H R I N K 3 0. P Y

Copyright (c) 2010 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================
DESCRIPTION
    Interface for QShrinkElf.py when called from a build.  Also handles
    command-line options.
=============================================================================

=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order
$Header: //components/rel/dspcore.adsp/4.0/qshrink/src/Qshrink20.py#1 $
$Date: 2017/03/30 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
13/03/13   sh      Added QSHrink 3.0 support
11/04/11   sh      Added SCONS script support
07/21/11   sh      QShrink20 commercial version release
08/11/10   kf      now long option names for build log readability
07/21/10   jw      Initial Version

=============================================================================
'''
import getopt, sys
import re

#----------------------------------------------------------------------------
# Hooks for Scons
#----------------------------------------------------------------------------
def exists(env):
   return env.Detect('qsr20_builder')

def generate(env):
   qsr20_generate(env)
   
def  qsr20_generate(env):
   qsr20_act = env.GetBuilderAction(qsr20_builder)
   qsr20_bld = env.Builder(action = qsr20_act,
                        emitter = qsr20_emitter, 
                         suffix = '.elf')

   env.Append(BUILDERS = {'Qshrink20' : qsr20_bld})
   sys.path += [env.subst("${BUILD_SCRIPTS_ROOT}/../../../../qdsp6/qshrink/src")]

def qsr20_emitter(target, source, env):
   env.Depends(target, source)  
   return (target, source) 
   
def qsr20_builder(env, target, source, **kwargs):
   
   import sys
   sys.path += [env.subst("${BUILD_SCRIPTS_ROOT}/../../../../qdsp6/qshrink/src")]
   #from QShrinkElf import runQshrink30
   from QShrinkElf import runQShrink
   from QShrinkElf import loadMapFileForMemLog   
   inputFile = env.RealPath(str(source[0]))
   outputFile=env.RealPath(str(target[0]))
   hashFile =  env.RealPath(str(source[1]))
   #ssHashFile =  re.split('\msg_hash.txt', hashFile)[0] +"ss_hash.txt" 
   logFile = re.split('\msg_hash.txt', hashFile)[0] +"qsr2" 
   anomalyListFile = re.split('\msg_hash.txt', hashFile)[0] +"qsr2_an" 
   replace_only = 1
   memLog = 0
   sclFile = ""
   mapFile="" 
   runQShrink(inputFile,
              outputFile,
              hashFile,
              sclFile,
              mapFile,
              logFile,
              anomalyListFile,
              replace_only,
              memLog) 
   return None

#----------------------------------------------------------------------------
# Main function
#----------------------------------------------------------------------------

def main():
  from QShrinkElf import runQShrink
  from QShrinkElf import loadMapFileForMemLog

  opts, args = getopt.gnu_getopt(sys.argv[1:], "",
                                 ['output=', 'hashfile=', 'scl=','mapFile=',
                                  'log=', 'discovery','discover_only',
                                  'validate', 'memLog'])
  optDict = {
    '--output' : [],
    '--hashfile' : [],
    '--scl' : [],
    '--mapFile' : [],
    '--log' : [],
    '--discovery' : [],
    '--discover_only' : [],
    '--validate' : [],
    '--memLog' : []}

  if len(args) == 0:
    print 'Usage:\n + Qshrink20.py "inputElfFile"'
    print '\t --output="outElfFile"'
    print '\t --hashfile="msg hashfile"'
    print '\t --scl="scl_file"'
    print '\t --log="logfile"'
    print '\t --discovery   discover and replace mode flag, else just replacement mode'
    print '\t --discover_only  discover new strings, Elf file not modified'
    print '\t --memLog      generate memory size log file from Mapfile'
    print '\t --mapFile="Map file, used with --memLog option"'
    print '\t --validate    validate changes made'
    
    exit()

  for i in opts :
    optDict[i[0]].append(i[1])
  for k in optDict :
    assert len(optDict[k]) <= 1

  output = args[0] + ".shrunk"
  if len(optDict['--output']) == 1 :
    output = optDict['--output'][0]

  hashFile = "msg_hash.txt"
  if len(optDict['--hashfile']) == 1 :
    hashFile = optDict['--hashfile'][0]
  
  sclFile = re.split('\.', args[0])[0] + ".scl"
  if len(optDict['--scl']) == 1 :
    sclFile = optDict['--scl'][0]

  mapFile = re.split('\.', args[0])[0] + ".map"
  if len(optDict['--mapFile']) == 1 :
    mapFile = optDict['--mapFile'][0]

  logFile = "qsr2_" + re.split('\.', args[0])[0]
  if len(optDict['--log']) == 1 :
    logFile = optDict['--log'][0]

  anomalyListFile = logFile + "_" + "qsr2_anlist"

  replace_only = 1
  if len(optDict['--discovery']) == 1:
    replace_only = 0

  if len(optDict['--discover_only']) == 1:
    replace_only = 2

  memLog = 0
  if len(optDict['--memLog']) == 1:
    memLog = 1;
    
  validate = 0
  if len(optDict['--validate']) == 1:
    validate = 1

  runQShrink(elfFile=args[0],
              outputFile=output,
              hashFile=hashFile,
              sclFile=sclFile,
              mapFile=mapFile,
              logFile=logFile,
              anomalyListFile=anomalyListFile,
              replace_only_mode=replace_only,
              memLog = memLog)
  
  if validate == 1:
    from QElfValidation import validateStructs
    validateStructs(outputFile=output,hashFile=hashFile)

if __name__ == '__main__':
    main()
