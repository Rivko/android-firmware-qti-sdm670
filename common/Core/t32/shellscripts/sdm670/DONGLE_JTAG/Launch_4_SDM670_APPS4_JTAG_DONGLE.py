#!/user/bin/env python
'''
    @file (General T32 launcher file)
    @brief Based on filename, calls T32WindowLauncher with arguments from filename.
    #author JBILLING
    #bug

    Prerequisites:
            Expected Python version: Python 2.7 or greater. Not compatible with Python3.0 and above.      
            T32WindowLauncher must be in parent directory, along with all  of its dependencies.
        
    Available API's:
        LaunchWindow
    
'''
######################################################################################
##
##       @file (General T32 launcher file)
##       @brief Based on filename, calls T32WindowLauncher with arguments from filename.
##       #author JBILLING
##       #bug
##
##       Copyright (c) 2015 - 2017 by QUALCOMM Technologies Incorporated.
##       All Rights Reserved.
##      
##       QUALCOMM Confidential and Proprietary
##
##       Prerequisites:
##               Expected Python version: Python 2.7 or greater. Not compatible with Python3.0 and above.  
##               T32WindowLauncher must be in parent directory, along with all  of its dependencies.
##           
##       Available API's:
##           LaunchWindow
##       
##                             EDIT HISTORY FOR FILE
##        This section contains comments describing changes made to the module.
##           when       who             what, where, why
##           --------   ---             ---------------------------------------------------------
##           01/24/2017 JBILLING        Created
##
######################################################################################
import os
import sys 
from optparse import OptionParser

##====---------------------------------------------------====##
##====                Global Variables                   ====##
##====---------------------------------------------------====##
global FILEDIR
global LAUNCHSCRIPTDIRECTORY
global LAUNCHSCRIPTFILE
global LAUNCHSCRIPTFILENAME

FILEDIR=os.path.dirname(os.path.abspath(__file__))

LAUNCHSCRIPTFILENAME='T32WindowLauncher.py'
if LAUNCHSCRIPTFILENAME in os.listdir(os.path.join(FILEDIR,"..")):
    LAUNCHSCRIPTFILE=os.path.join(FILEDIR,"..",LAUNCHSCRIPTFILENAME)
    LAUNCHSCRIPTDIRECTORY=os.path.join(FILEDIR,"..")
elif LAUNCHSCRIPTFILENAME in os.listdir(os.path.join(FILEDIR,"..","..")):
    LAUNCHSCRIPTFILE=os.path.join(FILEDIR,"..","..",LAUNCHSCRIPTFILENAME)
    LAUNCHSCRIPTDIRECTORY=os.path.join(FILEDIR,"..","..")
else:
    print "Error! Could not fine T32WindowLauncher.py"
    sys.exit(1)


global SUCCESS
global FAILURE
SUCCESS=0
FAILURE=-1

##====--------------------------------------------------====##
##====                  Python API's                    ====##
##====--------------------------------------------------====##

def LaunchWindow(t32dir,silentoption):
    '''
        #################################################################
        ##
        ##  Function: LaunchWindow
        ##
        ##  Description:
        ##      Launches T32 window based on this file's name
        ##
        ##  Arguments:
        ##      None - only launches window in title of script
        ##
        ##  Returns:
        ##      API port number of launched window. This can be used for automation.
        ##
        #################################################################
    '''
    sys.path.append(LAUNCHSCRIPTDIRECTORY)
    import T32WindowLauncher
    filename=os.path.basename(__file__)[:-3]
    
    attributelist=filename.split('_')
    target=attributelist[2].lower()
    window=attributelist[3].lower()
    protocol=attributelist[4].lower()
    mode=attributelist[5].lower()
    apiport=T32WindowLauncher.LaunchWindow(window,target=target,protocol=protocol,mode=mode,t32dir=t32dir,silentoption=silentoption)
    
    return apiport
    
def kill(message=None):
    '''
    Print message and exit(1)
    '''
    if message:
        print str(message)
    sys.exit(1)
    
def parseoptions():
    """
        Parse out given user options
    """
    argparser = OptionParser(version='%prog 1.0')
    argparser.add_option('-t','--t32', dest='t32dir',           help="T32 installation directory to use",default=None)
    argparser.add_option('-s','--silent', dest='silentoption',  help="Silent option - launch T32 without user interaction if possible",default=False)
    
    #If -h/--help specified, print out description
    for arg in sys.argv[1:]:
        if ('-h' in arg) or ('--help' in arg):
            filename=os.path.basename(__file__)[:-3]
            thisfile=__import__(filename)
            print thisfile.__doc__
    
    (options,args)=argparser.parse_args()
    
    if not options.silentoption is False:
        options.silentoption=True
            
    return options
    
def test():
    '''
    Test routine. Runs script, prints SUCCESS or FAILURE depending on output.
    '''
    rvalue=SUCCESS
    rvalue=LaunchWindow()
    if rvalue == -1:
        print "FAILURE"
    else:
        print "SUCCESS"
    return rvalue
    
if __name__ == '__main__':

    options=parseoptions()
    #Run primary subroutine
    LaunchWindow(options.t32dir,options.silentoption)
    
    
    
