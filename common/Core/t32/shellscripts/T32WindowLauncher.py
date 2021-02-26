# -*- coding: utf-8 -*-
#!/user/bin/env python
'''
    @file T32WindowLauncher 
    @brief Manages launching of T32 windows, based on user inputs (target, window)
    #author JBILLING
    #bug

    Prerequisites:
            Expected Python version: Python 2.7 or greater. Not compatible with       
            Python3.0 and above. t32start.py, sdm845.ts2 must be in parent directory.  
            Expected that *.T32 config files are in ../../tools/cmm/config/configs/
        
    Available API's:
        Information subroutines:
            PrintAvailableWindows(target)
            GetAvailableWindows(target)
            GetAvailableModes(target)
            GetAvailableProtocols(target)
            PrintAvailableTargets(target)
            GetAvailableTargets(target)
        T32 Launcher subroutines:
            LaunchWindow(target,window,protocol='swd',mode='dongle',t32dir=None,silentoption=False)
                (Note that LaunchWindow is the default script run  with command line arguments)
            SendT32CommandAPIDict(t32command, apidict):    
            SendT32Command(t32command,target,window,protocol='swd',mode='dongle',t32dir=None,silentoption=False):
            CloseT32Window(target,window,protocol='swd',mode='dongle'):        
            CloseT32WindowByAPIDict(apidict)
        
'''
######################################################################################
##
##        @file T32WindowLauncher
##        @brief Manages launching of T32 windows, based on user inputs (target, window)
##        #author JBILLING
##        #bug
##        
##       Copyright (c) 2015 - 2017 by QUALCOMM Technologies Incorporated.
##       All Rights Reserved.
##      
##       QUALCOMM Confidential and Proprietary
##
##        Prerequisites:
##                Expected Python version: Python 2.7 or greater. Not compatible with Python3.0 and above.   
##                t32start.py, sdm845.ts2 must be in parent directory. Expected that *.T32 config files are 
##                in ..\..\tools\cmm\config\configs\
##            
##        Available API's:
##          PrintAvailableWindows(target)
##          GetAvailableWindows(target)
##          GetAvailableModes(target)
##          GetAvailableProtocols(target)
##          LaunchWindow(target,window,protocol='swd',mode='dongle',t32dir=None,silentoption=False)
##          PrintAvailableTargets(target)
##          GetAvailableTargets(target)
##
##                             EDIT HISTORY FOR FILE
##        This section contains comments describing changes made to the module.
##           when       who             what, where, why
##           --------   ---             ---------------------------------------------------------
##           11/17/2017 JBILLING        Quicklaunch and other changes for EUDPeekPoke.py functionality
##           01/11/2017 JBILLING        Changed Initialization API names
##           10/01/2016 JBILLING        Created
##
######################################################################################
import os
import sys 
import time
import ctypes
from ctypes import *
from optparse import OptionParser
import platform

        
##====---------------------------------------------------====##
##====                Global Variables                   ====##
##====---------------------------------------------------====##

FILEDIR=os.path.dirname(__file__)
global CONFIGDIRECTORIES
CONFIGDIRECTORIES=os.path.join(FILEDIR,"..","..","tools","cmm","config","configs")
APIDIRECTORIES=os.path.join(FILEDIR,"..","..","tools","automation","t32configs")
global METATOOLDIRECTORY
METATOOLDIRECTORY=os.path.abspath(os.path.join(FILEDIR,"..","..","tools","automation"))
global METAROOTDIRECTORY
METAROOTDIRECTORY=os.path.abspath(os.path.join(FILEDIR,"..","..","..",".."))

global SUCCESS, FAILURE
SUCCESS=0
FAILURE=-1

global PERSCRIPTWAITTIME
PERSCRIPTWAITTIME=2

global DEFAULT_T32DIR
#FIXME - add options for linux etc.
DEFAULT_T32DIR="C:\\t32"
global DEFAULT_PROTOCOL
DEFAULT_PROTOCOL='swd'
global DEFAULT_MODE
DEFAULT_MODE='dongle'

global T32DLLPRESENT
T32DLLPRESENT=False

global EUDMSMResetHelpString
global PBIDICT
PBIDICT={
    'EUD':'MCILIB',
    'DONGLE':'USB',
    'SIM':'SIM',
    'ETHERNET':'NET',
    }
##====---------------------------------------------------====##
##====                Import Libraries                   ====##
##====---------------------------------------------------====##

sys.path.append(CONFIGDIRECTORIES)
sys.path.append(APIDIRECTORIES)
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(__file__)),'..'))
import t32start
import TargetConfigurator
import json, subprocess
##TODO - add lib's for linux

if  os.path.isfile(os.path.join(APIDIRECTORIES,'T32Session.py')) and \
    os.path.isfile(os.path.join(APIDIRECTORIES,'t32api.dll')) and\
    os.path.isfile(os.path.join(APIDIRECTORIES,'t32api64.dll')):
    
    import T32Session
    T32DLLPRESENT=True
else:
    T32DLLPRESENT=False
            
global console
global color_red
global color_yellow
global color_green
global color_black
global color_blue
global color_intensitfy
global STD_OUTPUT_HANDLE
console = -1
color_red = 12
color_yellow = 14
color_green = 10
color_black = 7
color_blue = 9
color_intensify = 8 #subtract from color_* to remove intensity
STD_OUTPUT_HANDLE = -11

##====--------------------------------------------------====##
##====                  Other Utils                     ====##
##====--------------------------------------------------====##

def changeColor(color):
    # color change code borrowed from JBILLINGS
    stdout_handle = windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
    windll.kernel32.SetConsoleTextAttribute(stdout_handle, color)
    
def resetColor():
    # color change code borrowed from JBILLINGS
    stdout_handle = windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
    windll.kernel32.SetConsoleTextAttribute(stdout_handle,color_black)

##====--------------------------------------------------====##
##====                  Python API's                    ====##
##====--------------------------------------------------====##
def PrintAvailableWindows(target):
    '''
        #################################################################
        ##
        ##  Function: PrintAvailableWindows
        ##
        ##  Description:
        ##      Prints available windows depending on target.
        ##      Also will print available protocols and modes.
        ##      These returns can be used in LaunchWindow
        ##
        ##  Arguments:
        ##      target   - desired target (e.g. sdm845)
        ##
        ##  Returns:
        ##      None
        ##
        #################################################################
        
    '''
    windowlist=GetAvailableWindows(target)
    print "Available T32 windows:"
    for window in windowlist:
        print "\t"+window
    return
def GetAvailableWindows(target):    
    '''
        #################################################################
        ##
        ##  Function: GetAvailableWindows
        ##
        ##  Description:
        ##      Returns list of available windows for given target
        ##
        ##  Arguments:
        ##      target   - desired target (e.g. sdm845)
        ##
        ##  Returns:
        ##      None
        ##
        #################################################################
        
    '''
    windowlist=[]
    
    targetconfig=gettargetconfig(target)
    return targetconfig.CoreList
    
def GetAvailableModes(target):
    '''
        #################################################################
        ##
        ##  Function: GetAvailableModes
        ##
        ##  Description:
        ##      Returns list of available Modes (dongle/eud) for given target
        ##
        ##  Arguments:
        ##      target   - desired target (e.g. sdm845)
        ##
        ##  Returns:
        ##      modelist - list of strings of available modes ['dongle','eud']
        ##
        #################################################################
        
    '''
    
    targetconfig=gettargetconfig(target)
    return targetconfig.Modes
    
def GetAvailableProtocols(target):
    '''
        #################################################################
        ##
        ##  Function: GetAvailableProtocols
        ##
        ##  Description:
        ##      Returns list of available Protocols (SWD/JTAG) for given target
        ##
        ##  Arguments:
        ##      target   - desired target (e.g. sdm845)
        ##
        ##  Returns:
        ##      protocollist - list of strings of available protocols ['jtag','swd']
        ##
        #################################################################
        
    '''
    targetconfig=gettargetconfig(target)
    return targetconfig.Protocols
    
def getcleantarget(target):
    '''
        Helper function to get clean target name from list of aliases.
    '''
    availabletargets=TargetConfigurator.GetAvailableTargets()
    cleantarget=None
    for availabletargetkey in availabletargets.keys():
        if target.lower() in availabletargets[availabletargetkey]['aliases']:
            cleantarget=availabletargets[availabletargetkey]['instance'].TargetName
            cleantargetkey=availabletargetkey
    if not cleantarget:
        print "Error! Target specified ("+target+") not found in available targets. Available targets:"
        PrintAvailableTargets()
        kill()
        
    return cleantarget
    
def gettargetconfig(target):
    '''
        Helper function to check if given target and window are valid. Generate if so.
    '''
    availabletargets=TargetConfigurator.GetAvailableTargets()
    cleantarget=None
    for availabletargetkey in availabletargets.keys():
        if target.lower() in availabletargets[availabletargetkey]['aliases']:
            cleantarget=availabletargets[availabletargetkey]['instance'].TargetName
            cleantargetkey=availabletargetkey
    if not cleantarget:
        print "Error! Target specified ("+target+") not found in available targets. Available targets:"
        PrintAvailableTargets()
        kill()
        
    targetconfig=availabletargets[cleantargetkey]['instance']
    
    return targetconfig

def checkift32windowopen(target,window,apiport=None):
    '''
        Helper function to check if T32 window is open.
    '''
    if T32DLLPRESENT is True:
        apidict=GetAPIPortConfig(target,window,apiport=apiport)
        if apidict is -1:
            print "Error - could not retrieve API dictionary for this window ("+window+")."
            return False
        else:
            session=T32Session.T32Session(apidict)
            t32state=session.init_t32py()
            if t32state > 0:
                return False  
            else:
                return True
            
def checkandcloset32window(target,window, silentoption, closewindow=True, apiport=None):
    '''
        Helper function to check if T32 window is already open and running.
        Closes that window if silent option is asserted or if user specifies via prompt.
        Returns True if window  was  closed, False if not
    '''
    if T32DLLPRESENT is True:
        apidict=GetAPIPortConfig(target,window,apiport=apiport)
        if apidict is -1:
            print "Error - could not retrieve API dictionary for this window ("+window+")."
            return False
        else:
            session=T32Session.T32Session(apidict)
            t32state=session.init_t32py()
            if t32state > 0:
                return False
        #Used when user wants to just check  if window was open
        if closewindow is False:
            return True
        
        if silentoption:
            session.terminate()
            session.exit()
            return True
        else:
            
            input=raw_input("Found a T32 window already open with desired configuration. Close window?Y/n: ")
            if 'Y' in input:
                session.terminate()
                session.exit()
                return True
            else:
                print "Detected T32 window already running. Exiting"
                sys.exit(0)
                
    return False
    
def createconfigfile(target,window,protocol,mode,t32dir=None,filedir=None,apiport=None,intercomport=None):
    '''
        Helper function to create t32 config file
    '''
    import tempfile as tf
    
    #
    #   Find temp directory and name temp file.
    #
    #   If filedir is legit, make tempdir match filedir
    #   Else, use tempfile's defaults.
    using_tempfile_dir=False
    givenfiledirbad=True
    if not filedir is None:
        if os.path.isdir(filedir):
            if os.access(filedir,os.W_OK):
                tempdir=filedir
                tf.tempdir=tempdir
                givenfiledirbad=False
        
    if givenfiledirbad is True:
        try:
            tempdir=os.environ['TEMP']
        except:
            tempdir=tf.gettempdir()
            using_tempfile_dir=True
            
        if using_tempfile_dir is False:
            tf.tempdir=tempdir
                        
    if t32dir is None:
        t32dir=DEFAULT_T32DIR
        
    portconfig=GetAPIPortConfig(target,window,apiport=apiport)
    if apiport is None:
        apiport=GetAPIPortConfig(target,window,apiport=apiport)['PORT']
        
    if intercomport is None:
        intercomportnum=GetIntercomPort(target,window)
    else:
        intercomportnum=intercomport
    
    pbival=PBIDICT[mode.upper()]
    configfilebasename="_".join([target,window,protocol,mode])
    targetconfig=gettargetconfig(target)
    ipaddr=targetconfig.IPAddr
    f = tf.NamedTemporaryFile(mode='w',suffix='.t32',prefix=configfilebasename,delete=False)
    
    instanceval=GetInstanceVal(target,window)
    if not instanceval is None:
        coreval=instanceval+1
    
    #with open(filepath,'wr') as f:
    f.write(";This configuration file is generated with T32Start2\n")
    f.write(";===================================================\n")
    f.write("                                                    \n")
    f.write(";Environment Variables                              \n")
    f.write("OS=                                                 \n")
    f.write("ID="+configfilebasename.upper()                  +"\n")
    f.write("TMP="+tf.tempdir                                  +"\n")
    f.write("SYS="+t32dir                                      +"\n")
    f.write("HELP="+t32dir+"\\pdf                                \n")
    f.write("                                                    \n")
    f.write("; Standard License File (license.t32) used          \n")
    f.write("                                                    \n")
    f.write(";T32 API Access                                     \n")
    f.write("RCL="+str(portconfig['RCL'])                      +"\n")
    f.write("PORT="+str(apiport)                               +"\n")
    f.write("PACKLEN="+str(portconfig['PACKLEN'])              +"\n")
    f.write("                                                    \n")
    if not 'sim' in window.lower():
        f.write(";T32 Intercom                                       \n")
        f.write("IC=NETASSIST                                        \n")
        f.write("PORT="+str(intercomportnum)                       +"\n")
        f.write("PACKLEN=1024                                        \n")
    f.write("                                                    \n")
    f.write(";T32 GDB                                            \n")
    f.write("; not used                                          \n")
    f.write("                                                    \n")
    f.write(";T32 TCF                                            \n")
    f.write("; not used                                          \n")
    f.write("                                                    \n")
    f.write(";Connection to Host                                 \n")
    if pbival in 'USB' or pbival in 'NET':
        f.write("PBI="                                         +"\n")
        f.write(pbival                                         +"\n")
    else:
        f.write("PBI="+pbival                                  +"\n")
    if pbival in 'NET':
        f.write("NODE="+ipaddr                                         +"\n")
    if not instanceval is None:
        #f.write("INSTANCE="+str(instanceval)                       +"\n")
        f.write("CORE="+str(coreval)                               +"\n")
    f.write("                                                    \n")
    f.write(";Screen Settings                                    \n")
    f.write("SCREEN=                                             \n")
    f.write("FONT=SMALL                                          \n")
    f.write("HEADER="+configfilebasename.upper()               +"\n")
    f.write("                                                    \n")
    f.write(";Printer Settings:                                  \n")
    f.write("PRINTER="+platform.system().upper()               +"\n")
    f.close()

    return f.name
    
def CloseT32Window(target,window,protocol='swd',mode='dongle',apiport=None,intercomport=None):        
    '''
        #################################################################
        ##
        ##  Function: CloseT32Window
        ##
        ##  Description:
        ##      Closes desired T32 window, if it's open. 
        ##
        ##  Arguments:
        ##      target   - desired target (e.g. sdm845)
        ##      window   - desired window (e.g. APPS0)
        ##      protocol - (optional) desired protocol (e.g. 'swd' or 'jtag')
        ##                  Default is swd.
        ##      mode     - (optional) desired mode (e.g. 'dongle' or 'eud')
        ##                  Default is dongle
        ##
        ##  Returns:
        ##      Success (0) or failure(-1)
        ##
        #################################################################
    '''
    
    configfile=createconfigfile(target,window,protocol,mode,apiport=apiport,intercomport=intercomport)
    silentoption=True
    if checkandcloset32window(target,window,silentoption):
        return SUCCESS
    else:
        return FAILURE
    
def CloseT32WindowByAPIDict(apidict):
    '''
        #################################################################
        ##
        ##  Function: CloseT32WindowByAPIDict
        ##
        ##  Description:
        ##      Closes desired T32 window using apidict, which is returned 
        ##      from LaunchWindow subroutine.
        ##
        ##  Arguments:
        ##      apidict - dictionary return from successful LaunchWindow
        ##
        ##  Returns:
        ##      Success (0) or failure(-1)
        ##
        #################################################################
    '''
    
    
    if T32DLLPRESENT is True:
        
        session=T32Session.T32Session(apidict)
        t32state=session.init_t32py()    
        if t32state > 0:
            return False
            
        session.terminate()
        session.exit()
        return True
        
    else:  
        return False

def SendT32Command(t32command,window,meta=None,target=None,protocol='swd',mode='dongle',t32dir=None,silentoption=False,apiport=None,intercomport=None):
    '''
        #################################################################
        ##
        ##  Function: SendT32Command
        ##
        ##  Description:
        ##      Sends a command to desired T32 window. If window is not opened,
        ##      subroutine will open that window and bootprocessor if applicable.
        ##
        ##  Arguments:
        ##      window   - desired window (e.g. APPS0)
        ##      metabuild- (Optional) - Path to desired metabuild. if not given, 
        ##                  will attempt to retrieve  metabuild/target info from 
        ##                  containing metabuild.
        ##      target   - (Optional) - desired target (e.g. sdm845). If not given 
        ##                  will attempt to retrieve target info from containing metabuild.
        ##      protocol - (optional) desired protocol (e.g. 'swd' or 'jtag')
        ##                  Default is swd.
        ##      mode     - (optional) desired mode (e.g. 'dongle' or 'eud')
        ##                  Default is dongle
        ##      t32dir   - (optional) Location of T32 directory so that user is not prompted.
        ##      silentoption - (optional) Do not prompt user. Fail silently if not enough 
        ##                      information is provided.       
        ##  Returns:
        ##      Success (0) or failure(-1)
        ##
        #################################################################
    '''
    if T32DLLPRESENT is False:
        print "Error! Cannot pass T32 script without T32 DLL/LIB files present."
        print "Expect them to be in directory: "+str(APIDIRECTORIES)
        return FAILURE
        
    metaobject,target,generatemetainfoflag=sanitizetargetandmeta(meta,target)
    
    configfile = createconfigfile(target,window,protocol,mode,t32dir=t32dir,apiport=apiport,intercomport=intercomport)
    if not checkandcloset32window(target,window,True,closewindow=False): 
        apidict=LaunchWindow(window,meta=meta,target=target,protocol=protocol,mode=mode,t32dir=t32dir,silentoption=silentoption)
    if apidict is -1:
        print "Error - some error ocurred during window launch. Exiting."
        return FAILURE
    
    rvalue=SendT32CommandAPIDict(t32command,apidict)
    
    return rvalue
    
def getmetaobject(meta):
    '''
    Helper function to retrieve metabuild object from MetaBuildManager.
    '''
    
    if meta:
        if os.path.isfile(os.path.join(meta,'contents.xml')):
            metaobject=MetaBuildManager(meta)
    metaobject=MetaBuildManager(METAROOTDIRECTORY)
    return metaobject
    
def SendT32CommandAPIDict(t32command, apidict):    
    '''
        #################################################################
        ##
        ##  Function: SendT32CommandAPIDict
        ##
        ##  Description:
        ##      Sends specified command to desired T32 window. 
        ##      apidict parameter is used to determine which window. 
        ##      This is the value returne from a successful LaunchWindow call.
        ##
        ##  Arguments:
        ##      t32command - t32command  to send (e.g. 'do std_debug' or 'd.load.elf')
        ##      apidict -   python dictionary which is returned from 
        ##                  successful LaunchWindow call.
        ##
        ##  Returns:
        ##      Success (0) or failure(-1)
        ##
        #################################################################
    '''
    rvalue=FAILURE
    if T32DLLPRESENT is False:
        print "Error! Cannot pass T32 script without T32 DLL/LIB files present."
        print "Expect them to be in directory: "+str(APIDIRECTORIES)
        return FAILURE
    
    #init session and check communication
    session=T32Session.T32Session(apidict)
    t32state=session.init_t32py()
    if t32state > 0:
        return False
    
    #Send script
    rvalue=session.send_cmd(t32command)
    if not rvalue is 0:
        print "Some error ocurred when executing T32 command: "+t32command+". Please check T32 window."
    #close session
    session.exit()
    
    return SUCCESS

def sanitizetargetandmeta(meta,target):
    '''
    Helper function to sort out meta and target
    '''
    generatemetainfoflag=True
    if meta:
        metaobject=getmetaobject(meta)
        metatarget=getcleantarget(metaobject.gettargetname())
        if target:
            target=getcleantarget(target)
            if not metatarget in target:
                print "Error! Given target ("+target+") is different from specified metabuild ("+meta+")."
                print "Please run again with matching variables"
                sys.exit(1)
        else:
            target=metatarget
    elif target:
        target=getcleantarget(target)
        metaobject=getmetaobject(meta)
        metatarget=getcleantarget(metaobject.gettargetname())
        if not metatarget in target:
            print "Warning! Specified target is different from containing metabuild target."
            print "Will launch T32 windows from specified target without generating metabuild info."
            generatemetainfoflag=False
    else:
        metaobject=getmetaobject(METAROOTDIRECTORY)
        target=metaobject.gettargetname()
    return metaobject,target,generatemetainfoflag
    
def CheckT32APIConnection(apidict,statusprintflag=False):
    '''
        Helper function to determine if T32 window is open or not.
        
        Returns:
            False if  API could not  communicate with T32 window
            True if it could communicate (Window is open and responsive)
            None if T32 DLL could not be found.
    '''
    t32openflag = False

    if T32DLLPRESENT is True:
        session=T32Session.T32Session(apidict)
        t32state=session.init_t32py()
        session.exit()
        if t32state > 0:
            t32openflag=False
            if statusprintflag:
                print "Warning! Could not commmunicate with opened t32 window."
                
        else:
            t32openflag=True
            if statusprintflag:
                print "T32 window opened successfully"
                
    else:
        t32openflag=None
        if statusprintflag:
            print "Warning! T32 DLL/Lib API files not present. Cannot communicate with t32 window."
            
            
    return t32openflag
    
def LaunchWindow(window,meta=None,target=None,protocol='swd',mode='dongle',t32dir=None,silentoption=False, quickLaunchEUD=False, rumioption=None, deviceid = None,apiport=None,intercomport=None):
    '''
        #################################################################
        ##
        ##  Function: LaunchWindow
        ##
        ##  Description:
        ##      Launches desired T32 window. If window is not in 
        ##      available windows, returns error and prints
        ##       available windows.
        ##
        ##  Arguments:
        ##      window   - desired window (e.g. APPS0)
        ##      metabuild- (Optional) - Path to desired metabuild. if not given, 
        ##                  will attempt to retrieve  metabuild/target info from 
        ##                  containing metabuild.
        ##      target   - (Optional) - desired target (e.g. sdm845). If not given 
        ##                  will attempt to retrieve target info from containing metabuild.
        ##      protocol - (optional) desired protocol (e.g. 'swd' or 'jtag')
        ##                  Default is swd.
        ##      mode     - (optional) desired mode (e.g. 'dongle' or 'eud')
        ##                  Default is dongle
        ##      t32dir   - (optional) Location of T32 directory so that user is not prompted.
        ##      silentoption - (optional) Do not prompt user. Fail silently if not enough 
        ##                      information is provided.
        ##
        ##  Returns:
        ##      API Dictionary 
        ##
        #################################################################
    '''
    
    #Argument looks like the following:
    #c:\t32\bin\windows64\t32marm64.exe -c C:\Temp\SDM845_APPS0_SWD_DONGLE.t32, D:\dropbox\sdm845\SDM845.LA.1.0-00042-PRESIL.INT-1\common\core\t32\..\tools\cmm\config\startup.cmm DAP SDM845 APPS 0 SWD
        
    ##################### Main execution of LaunchWindow ############################
    #
    #   Get required arguments to  start T32 window
    #
    if quickLaunchEUD:
        mode = 'eud'
    metaobject,target,generatemetainfoflag=sanitizetargetandmeta(meta,target)
    
    #
    #   Check  if T32 window is already open. If it is, simply return API dictionary.
    #
    apidict=GetAPIPortConfig(target,window,apiport=apiport)
    if CheckT32APIConnection(apidict):
        return apidict
        
    
    
    targetconfig=gettargetconfig(target)
    t32binary=targetconfig.GetT32BinaryfromAlias(window)
    t32dir= DEFAULT_T32DIR
    if not quickLaunchEUD:
        t32start.set_t32_environment(t32directory=t32dir,targetname=targetconfig.TargetName)
    configfile = createconfigfile(target,window,protocol,mode,t32dir,apiport=apiport,intercomport=intercomport)
    startupfile=os.path.abspath(os.path.join(CONFIGDIRECTORIES,"..","startup.cmm"))
    windowarg=targetconfig.GetT32WindowArgFromImageAlias(window)
    if not targetconfig or not t32binary or not configfile or not startupfile or not windowarg:
        kill("Error! Could not retrieve one or  more of the following: targetconfig, t32binary, configfile, startupfile, windowarg")
        
    if mode.lower() in 'eud':
        modearg='EUD'
    else:
        modearg='DAP'
    #
    #   Generate needed binaries, check if t32 window already open and start.
    #   If it's not bootprocessor window and we can ping window via API, 
    #   Check to see if bootproc is open and open it if not.
    #
    
    
    if targetconfig.CheckIfBootProcessor(window) and generatemetainfoflag is True and quickLaunchEUD is False:
       t32start.generate_buildinfo()
    #TODO - for some reason, API port returns true with different API numbers?
    #else:   
        #If boot processor is not yet open, then open it.
        #import pdb; pdb.set_trace()
        #if not checkift32windowopen(target,targetconfig.BootProcessor):
        #if not checkandcloset32window(target,targetconfig.BootProcessor,silentoption,closewindow=False):
        #    LaunchWindow(targetconfig.BootProcessor,meta=None,target=target,protocol=protocol,mode=mode,t32dir=t32dir,silentoption=silentoption)   
    #windowwasclosed=checkandcloset32window(target,window,silentoption)
    
    if rumioption is True:
        rumioption = "TRUE"
    else:
        rumioption = "FALSE"
        
    fusionoption = "NULL"
    
    if not deviceid is None:
        deviceidcmd = deviceid.upper()
        if not '0X' in deviceidcmd[:2]:
            deviceidcmd="0x"+deviceidcmd
    else:
        deviceidcmd = ""
        
    command=" ".join([   os.path.join(t32dir,'bin','windows64',t32binary),\
                        '-c',
                        configfile+",",
                        startupfile,
                        modearg,
                        target.upper(),
                        windowarg,
                        protocol.upper(),
                        fusionoption,
                        rumioption,
                        deviceidcmd,
                        ])
    currdir=os.getcwd()
    os.chdir(os.path.join(CONFIGDIRECTORIES,"..",".."))
    subprocess.Popen(command, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    os.chdir(currdir)
    time.sleep(targetconfig.SessionStartDelay)
    if targetconfig.CheckIfBootProcessor(window):
        time.sleep(targetconfig.SessionStartDelay)
        #Don't check if window closed.
        #if windowwasclosed is True: #Needs extra delay if window was closed
        #    time.sleep(targetconfig.SessionStartDelay)
        #    time.sleep(targetconfig.SessionStartDelay)
            
    
    #Return the api port for user to use.
    
    
    if apidict is -1:
        print "Error - could not retrieve API dictionary for this window ("+window+")."
        return -1
    else:
        CheckT32APIConnection(apidict,statusprintflag=True)
        
    return apidict
    
def QuickLaunchEUD(launchwindow, script, isSingleInstruction=False, pausetime=1, deviceid=None, apiport=None, intercomport=None):
    '''
    API to launch a T32 window with minimal processing requirements possible. Used from EUDPeekPoke.py
    '''
    dictionary=LaunchWindow(launchwindow,quickLaunchEUD=True,deviceid=deviceid,apiport=apiport,intercomport=intercomport)
    
    time.sleep(pausetime)
    
    if type(script) is str:
        print "Running "+script
        if not isSingleInstruction:
            SendT32CommandAPIDict(str('cd.do '+script),dictionary)
        else: #if 'script' is a single instruction
            SendT32CommandAPIDict(script,dictionary)            
    
    elif type(script) is list:
        for subscript in script:
            if subscript.endswith('.cmm') or '.cmm ' in subscript:
                print "Running "+subscript
                SendT32CommandAPIDict(str('cd.do '+subscript),dictionary)
            else: #if 'script' is a single instruction
                print "Running "+subscript
                SendT32CommandAPIDict(subscript,dictionary)  
            time.sleep(PERSCRIPTWAITTIME)
            

    resetColor()
    return dictionary    
    #CloseT32WindowByAPIDict(dictionary)
    
def GetIntercomPort(target,window):
    '''
        #################################################################
        ##
        ##  Function: GetIntercomPort
        ##
        ##  Description:
        ##      Retrieves Intercom Port number from target config instance.
        ##
        ##  Arguments:
        ##      window - t32 window - must be a valid t32 window.
        ##
        ##  Returns:
        ##      Dictionary of port configuration or -1 for failure
        ##
        #################################################################
    '''
    targetconfig=gettargetconfig(target)
    return targetconfig.GetIntercomPortFromAlias(window)

def GetInstanceVal(target,window):
    '''
        #################################################################
        ##
        ##  Function: GetInstanceVal
        ##
        ##  Description:
        ##      Retrieves instance value, used  during generation of config file.
        ##
        ##  Arguments:
        ##      target - must be a valid target
        ##      window - t32 window - must be a valid t32 window.
        ##
        ##  Returns:
        ##      Dictionary of port configuration or -1 for failure
        ##
        #################################################################
    '''
    targetconfig=gettargetconfig(target)
    return targetconfig.GetInstanceValFromAlias(window)
    
def GetAPIPortConfig(target,window,apiport=None):
    '''
        #################################################################
        ##
        ##  Function: GetAPIPortConfig
        ##
        ##  Description:
        ##      Retrieves APIPort configuration from target config instance.
        ##
        ##  Arguments:
        ##      window - t32 window - must be a valid t32 window.
        ##
        ##  Returns:
        ##      Dictionary of port configuration or -1 for failure
        ##
        #################################################################
    '''
    targetconfig=gettargetconfig(target)
    apidict = targetconfig.GetPortConfigFromAlias(window)
    if not apiport is None:
        apidict['PORT']=apiport
    return apidict
    
def GetAPIPort(target,window):
    '''
        #################################################################
        ##
        ##  Function: GetAPIPort
        ##
        ##  Description:
        ##      Retrieves APIPort number from target config instance.
        ##
        ##  Arguments:
        ##      window - t32 window - must be a valid t32 window.
        ##
        ##  Returns:
        ##      Dictionary of port configuration or -1 for failure
        ##
        #################################################################
    '''
    
    return GetAPIPortConfig(target,window)['PORT']
    #if apiport is None
    #if not os.path.isfile(configfile):
    #    print "Error! configfile not found (file: "+str(configfile)
    #    return -1
    #
    #with open(configfile) as f:
    #    lines=f.readlines()
    #    for currentline in lines:
    #        if ";T32 API Access" in currentline:
    #            rcl     = lines[lines.index(currentline)+1]
    #            port    = lines[lines.index(currentline)+2]
    #            packlen = lines[lines.index(currentline)+3]
    #            break
    #
    #portvaluefound=False
    #
    #if 'RCL='in rcl:
    #    rclfound=True
    #    rcl=rcl[len('RCL='):-1]
    #if 'PORT=' in port:
    #    portfound=True
    #    port=port[len('PORT='):-1]
    #if 'PACKLEN=' in packlen:
    #    packlenfound=True
    #    packlen=packlen[len('PACKLEN='):-1]
    #    
    #if not (rclfound and portfound and packlenfound):
    #    print "Error! required port values not found in config file"
    #    return -1
    #
    #rvalue={'IC':rcl,
    #        'PORT':port,
    #        'PACKLEN':packlen,
    #        'NODE':'LOCALHOST',
    #        }
    # 
    #return rvalue
    
                
    
def PrintAvailableTargets():
    '''
        #################################################################
        ##
        ##  Function: PrintAvailableTargets
        ##
        ##  Description:
        ##      Prints available targets
        ##
        ##  Arguments:
        ##      None
        ##
        ##  Returns:
        ##      SUCCESS code (0)
        ##
        #################################################################
    '''
    print "Available Targets:"
    for target in GetAvailableTargets():
        print str("\t"+target)
    return SUCCESS
    
def GetAvailableTargets():
    '''
        #################################################################
        ##
        ##  Function: GetAvailableTargets
        ##
        ##  Description:
        ##      Returns list of available targets
        ##
        ##  Arguments:
        ##      None
        ##
        ##  Returns:
        ##      List of strings of available targets
        ##
        #################################################################
    '''
    #targetdict=t32start.GetTargetNameSearchList_dict()
    #targetlist=[]
    #for target in targetdict.keys():
    #    targetlist.append(target[:-3])
    sys.path.append(CONFIGDIRECTORIES)
    import TargetConfigurator
    availabletargets=TargetConfigurator.GetAvailableTargets()
    targetlist=[]
    for availabletargetkey in availabletargets.keys():
        targetlist.append(availabletargets[availabletargetkey]['instance'].TargetName.lower())
                
    return targetlist

def parseoptions():
    """
        Parse out given user options
    """
    argparser = OptionParser(version='%prog 1.0')
    
    argparser.add_option('-w','--window', dest='window',    help="t32 window",default=None)
    argparser.add_option('-m','--meta', dest='meta',        help="(Optional) metabuild path ",default=None)
    argparser.add_option('-c','--chipset', dest='target',   help="(Optional) chipset (e.g. 'sdm845')",default=None)
    
    argparser.add_option('-p','--protocol', dest='protocol', \
        help="(Optional) protocol: 'swd' or 'jtag'. Default: '"+str(DEFAULT_PROTOCOL)+"'.",\
        default=str(DEFAULT_PROTOCOL))
    argparser.add_option('-d','--mode', dest='mode', \
        help="(Optional) mode: 'dongle' or 'eud'. Default: '"+str(DEFAULT_MODE)+"'.",\
        default=str(DEFAULT_MODE))
        
    argparser.add_option('-s','--silent', dest='silentmode', help="(Optional) silentmode",default=False)
    argparser.add_option('-t','--t32dir', dest='t32dir',     help="(Optional) t32 directory",default=None)
    
    #If -h/--help specified, print out description
    for arg in sys.argv[1:]:
        if ('-h' in arg) or ('--help' in arg):
            import T32WindowLauncher
            print T32WindowLauncher.__doc__
            
            
    (options,args)=argparser.parse_args()
    print options
    
            
    if options.target is None:
        kill("Error! target must be specified")
    elif not options.target.lower() in GetAvailableTargets():
        print "Error! Given target not recognized."
        PrintAvailableTargets()
        kill()
        
    if options.window is None:
        print "Error! Window must be specified."
        PrintAvailableWindows(options.target)
        kill()
        
        
    return options

'''
    Description:
        Interacts with T32 based on inputs from metabuild object
    Variables:
        self.MetaBuildPath - path to metabuild, either the one given by user or the one found by findbuild.
        self.ImageList - list of images pertaining to metabuild
        
        self.T32PATH - default path used for T32
        self.t32_version - obtained from contents.xml
        self.SD_t32_server - obtained from contents.xml
        self.QIPL_t32_server - obtained from contents.xml
        self.TEX_t32_server - obtained from contents.xml
    Methods
        generator(metabuildpath)
            uses metabuildpath as is
        generator(metabuildCRMID)
            this will use findbuild to locate your metabuildpath
        
        def getmetaimages()
            returns a list of images that the metabuild has (self.ImageList)
        def gettargetname
            returns target name
        def getHLOS
            returns HLOS variable
        def getmetapath
            return self.MetaBuildPath
            
'''    
class MetaBuildManager:
    '''
        collect info from contents.xml, or from quartz build.
        different subsystems
        buildpaths
        T32 version
    '''
    
    def __init__(self,metabuildpath):
        import json, subprocess
        '''variables to be initialized:
        self.flavors_list
        self.HLOS
        self.CHIPSET
        self.xmldict
        self.flavors_list
        
        image list
        image path list
        '''
        self.t32_version=None
        self.SD_t32_server=None
        self.QIPL_t32_server=None
        self.TEX_t32_server=None
        #Simple list of images for this target
        self.ImageList=[]
        #Dictionary containing images and their paths
        self.ImageBuildRoots={}
        
        
        currentdir=os.getcwd()
        if not os.path.exists(os.path.abspath(metabuildpath)):
            metabuildpath=pyfindbuildpath(metabuildpath)
            
            if not os.path.exists(os.path.abspath(metabuildpath)):
                kill('could not access metabuild!')
        self.MetaBuildPath=metabuildpath
        
        
        sys.path.append(os.path.abspath(os.path.join((os.path.abspath(metabuildpath)),'common/build/app')))
        sys.path.append(os.path.abspath(os.path.join((os.path.abspath(metabuildpath)),'common/build/lib')))
        workingdir=os.path.abspath(os.path.join((os.path.abspath(metabuildpath)),'common/build/app'))
        #these are from metabuild
        import meta_lib as ml
        import meta_log as lg
        mi = ml.meta_info()
                
        ####################Get images and build paths#####################
        python_installation=sys.executable
        d_out = subprocess.Popen(python_installation+" "+workingdir+"/meta_cli.py get_var_values cmm_root_path_var",stdout=subprocess.PIPE,shell=True)
        stdout, stderr = d_out.communicate()
        try:
            xmldict = json.loads(stdout)    
        except:
            kill("Error! No JSON object could be decoded. Is contents.xml correct and are build files working properly (under <meta-root>/common/build/lib)")        
        #Create list of images from metabuild data
        for imagebuild in xmldict.keys():
            image=imagebuild.split("_BUILDROOT")[0]
            self.ImageList.append(image)
            #It'll give us the linux path. convert it to windows path
            buildroot = os.path.abspath("/"+xmldict[imagebuild].split("/prj/qct/asw/crmbuilds") [1])
            self.ImageBuildRoots.update({image:buildroot})    
            if not os.path.exists(buildroot):
                buildroot=pyfindbuildpath(buildroot)
                if buildroot is None:
                    continue
                if not os.path.exists(buildroot):
                    kill("Error! Could not access build root listed in contents.xml: "+buildroot)
            
                
        ####################Get Target name and T32 variables#####################        
        #Get CHIPID and HLOS
        self.flavors_list = mi.get_product_flavors()
        if (len(self.flavors_list) == 0):
            print "No flavors found, taking 'None' as default flavor."
            self.flavors_list.append("None")
        # Look for cmm_var attributes in current flavor:
        self.var_values = mi.get_var_values('cmm_var', flavor=self.flavors_list[0])
        #self.CHIPSET=TargetConfigDict[var_values['CHIPID'].upper()]
        self.HLOS=self.var_values['HLOS_TYPE']
        self.TargetName=self.var_values['CHIPID']
        self.CHIPSET=self.TargetName
        
        #Get T32 version. if not given, get it from metabuild
        d_out = subprocess.Popen(sys.executable+" "+workingdir+"/meta_cli.py get_var_values cmm_var",stdout=subprocess.PIPE,shell=True)
        stdout, stderr = d_out.communicate()
        try:
            xmldict = json.loads(stdout)   
        except:
            kill("Error! No JSON object could be decoded. Is contents.xml correct and are build files working properly (under <meta-root>/common/build/lib)")
            
        if 'T32_VER' in xmldict.keys():
                self.t32_version=str(xmldict['T32_VER'])
                self.SD_t32_server=str(xmldict['T32_VER_PATH_SD'])
                self.QIPL_t32_server=str(xmldict['T32_VER_PATH_QIPL'])
                self.TEX_t32_server=str(xmldict['T32_VER_PATH_TEX'])
                self.T32PATH=self.SD_t32_server
                if not os.path.exists(self.T32PATH):
                    print "T32 version specified but could not access directory: "+default_t32_server+". Please correct contents.xml entry or specify T32 version with -t option. Defaulting to C:\\T32"
                    self.T32PATH="C:\\T32"
        else:
                print "Warning! t32 version not found within metabuild"    
                self.T32PATH='C:\\T32'
                
            
    
    def getmetaimages(self):
        return self.ImageList
        
    def gettargetname(self):
        return self.TargetName
        
    def getHLOS(self):
        return self.HLOS
        
    def getmetapath(self):
        return self.MetaBuildPath

#Can take either a path or just a CRM ID        
def pyfindbuildpath(build):
    builddict=pyfindbuildinfo(build)
    if type(builddict) is str:
        return None
    buildpath=builddict['Location']
    return buildpath

def pyfindbuildinfo(build):
    build=str(build)
    buildinfo={}
    buildpath=''
    defaultfindbuildpath='//stone/aswcrm/smart/nt/bin/findbuild'
    
    
    if ('\\' in build) or ('/' in build):
        buildroot=os.path.abspath(build)
        buildroot=buildroot.replace('\\','/')
        buildID=os.path.basename(buildroot)
        #filelist=buildroot.split('/')
        #buildID=filelist[len(filelist)-1]
    else:
        buildID=build
        
    command='findbuild '+buildID
    
    try:
        findbuildresult=subprocess.Popen(command, stderr = subprocess.PIPE, stdout = subprocess.PIPE)
        (cmdout,cmderr)=findbuildresult.communicate()
        if len(cmderr)>0:
            kill('Error occurred when calling findbuild: '+cmderr)    
    except:
        #print "Couldn't use findbuild from systempath. Attempting absolute path: "+defaultfindbuildpath
        #Try it again with direct path
        command=defaultfindbuildpath+' '+build
        try:
            findbuildresult=subprocess.Popen(command, stderr = subprocess.PIPE, stdout = subprocess.PIPE)
            (cmdout,cmderr)=findbuildresult.communicate()
            if len(cmderr)>0:
                kill('Error occurred when calling findbuild: '+cmderr)
        except:
            kill('Could not find findbuild in system path. Tried path: '+defaultfindbuildpath+'. Please add it to your path and try again')

        
    if 'No Record Found' in cmdout:
        return cmdout
    
    builddetails=cmdout.split('\r\n')
    for detail in builddetails:
        if ':' in detail:
            detailkey   = detail.split(':')[0].strip()
            detailentry = detail.split(':')[1].strip()
            buildinfo.update({detailkey:detailentry})
    
    
    return buildinfo
 
def IsWindows64():
    command = 'C:\\windows\\system32\\wbem\\WMIC OS GET OSARCHITECTURE /Format:list'
    getosarch = subprocess.Popen(command, stderr = subprocess.PIPE, stdout = subprocess.PIPE)
    (cmdout, cmderr) = getosarch.communicate()

    if cmdout.strip().endswith('64-bit'):
        #print 'Detected 64 bit Windows'
        return True
    return False

def kill(message=None):
    if message:
        print str(message)
    sys.exit(1)
    
def test():

    rvalue=SUCCESS
    PrintAvailableTargets()
    targetlist=GetAvailableTargets()
    apiportbase=0
    intercomportbase = 0
    for target in targetlist:
        PrintAvailableWindows(target)
        windowlist=GetAvailableWindows(target)
        modelist=GetAvailableModes(target)
        protocollist=GetAvailableProtocols(target)
        
        for currmode in modelist:
            ##Skip EUD for now
            if currmode in 'EUD':
                continue
            for currprotocol in protocollist:
                openedwindowlist=[]
                apiport=apiportbase
                intercomport=intercomportbase
                for window  in windowlist:
                    apiport+=1
                    intercomport+=1
                    result=LaunchWindow(window,target=target,meta=None,protocol=currprotocol,mode=currmode,t32dir='c:\\t32',silentoption=True,apiport=1,intercomport=2)
                    if result == -1:
                        rvalue=FAILURE
                    else:
                        openedwindowlist.append(window)
                    time.sleep(3)
                for window in openedwindowlist:
                    apiport+=1
                    intercomport+=1
                    result=CloseWindow(target,window,protocol=currprotocol,mode=currmode,apiport=1,intercomport=2)
                    if result == -1:
                        rvalue=FAILURE
                if rvalue is FAILURE:
                    print "FAILURE ocurred. Could not open all windows."
                    return rvalue
                    
    
    
    if rvalue == 0x0:
        print "SUCCESS"
    else:
        print "FAILURE"
    return rvalue
    
if __name__ == '__main__':
    options=parseoptions()
    print LaunchWindow(options.window, meta=options.meta, target=options.target, protocol=options.protocol, mode=options.mode, t32dir=options.t32dir,silentoption=options.silentoption)
    
    
