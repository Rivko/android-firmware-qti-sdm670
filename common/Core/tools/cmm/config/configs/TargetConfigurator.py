#===============================================================================
#
# msm8996.py
#
# GENERAL DESCRIPTION
#    T32 startup script for Windows
#
# Copyright (c) 2015 - 2017 by QUALCOMM Technologies Incorporated.
# All Rights Reserved.
#
# QUALCOMM Confidential and Proprietary
#
#-------------------------------------------------------------------------------
#

#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who         what, where, why
# --------   ---         ---------------------------------------------------------
# 05/24/2016 JBILLING    Created
#==============================================================================


import glob, itertools, os, string, sys, subprocess, re, platform, shutil, fnmatch, time, tempfile
    
    
    
    
##====---------------------------------------------------====##
##====                Global Variables                   ====##
##====---------------------------------------------------====##
global FILEDIR
FILEDIR=os.path.dirname(__file__)

global DebugEnabled
DebugEnabled=True
global COREDIR
COREDIR=os.path.abspath(os.path.join(FILEDIR,"..","..","..",".."))

'''
    contains:
        subsystems
        
        tests
            tests for each subsystem
            test dependency on other usecases
        
        usecases
            similar to tests but are used for test dependencies
        
        various variables for that target
            port numbers
            alias names
            metabuild build command
'''
    #    def __init__(self,target):

def testTargetConfigurator():
    targetdict=GetAvailableTargets()
    import pprint
    pp = pprint.PrettyPrinter(indent=4)
    for target in targetdict.keys():
        pp.pprint(vars(targetdict[target]['instance']))
    PrintAvailableTargets()
        

def PrintAvailableTargets():
    targetdict=GetAvailableTargets()
    print "\n\n\n\nAvailable targets:"
    for target in targetdict.keys():
        print target
    
def GetAvailableTargets():
        AvailableTargets={}
        
        #Initialize a target object from each available configuration.
        #Then get the alias names for each of those targets so that we can match what was requested
        #for file in availabletargets:
        currentdir=os.path.dirname(__file__)
        configfiles=[]
        configdirs=[]
        
        for root,dirs,files in os.walk(currentdir):
            for name in files:
                if name.endswith('.py'):
                    configfiles.append(os.path.join(root,name))
        
        
        for configfile in configfiles:
            
            if configfile.endswith('.py') and (('targetconfig_' in configfile.lower())):
                try:
                    sys.path.append(os.path.dirname(configfile))
                    filename=os.path.basename(configfile)[:-3]
                    targetconfig=__import__(filename)
                    targetobject=targetconfig.GenerateTargetConfig()
                except:
                    kill("TargetConfigurator: Could not call GenerateTargetConfig from file. Bad target file? Filename: "+filename)
                aliases=targetobject.GetTargetAliasList()
                AvailableTargets.update({filename:{'aliases':aliases,'instance':targetobject}})
        
        return AvailableTargets
        
'''
    Updated:
        Function which takes a target and returns instance of that target's configuration
        
'''
def GetTargetConfig(target):        
        
        AvailableTargets=GetAvailableTargets()
        #Now match given target to an alias in AvailableTargets. Delete the others
        SelectedTarget=None
        for targetkey in AvailableTargets.keys():
            if target.lower() in AvailableTargets[targetkey]['aliases']:
                SelectedTarget=AvailableTargets[targetkey]['instance']
        
        #If we couldn't find the target asked for, print out the available targets
        if SelectedTarget is None:
            print "##Error! Couldn't find requested target: "+target+" ##"
            print "##Here are available targets and their aliases:  ##"
            for targetkey in AvailableTargets.keys():
                print "##    "+targetkey+": "+str(AvailableTargets[targetkey]['aliases'])
                
        #Return the instance
        return SelectedTarget
    
            
            
class TargetConfig():

    def __init__(self):
        ##############################Defaults########################
        #This is a template from which we'll give all the subsystem port numbers to caller
        self.CoreIntercomConfig_ElementTemplate={ 'IC' : 'NETASSIST', 'PORT' : '25400', 'PACKLEN' : '1024', 'NODE' : 'LOCALHOST' }

        #################Target specific config variables#######################

        
        self.TargetAliasList=[] #e.g. ['8996','msm8996','apq8096','apq8996','istari']
        self.TargetName=None    #e.g. 'msm8996'
        self.CHIPSET=None       #e.g. 'msm8996'
        self.T32StartFile=None
        self.BootProcessor='APPS0'
        #These are listed in 8996.ts2 file
        
        self.RCLValue='NETASSIST'
        self.PACKLENValue='1024'
        self.APINodeValue='LOCALHOST'
        self.IPAddr='192.0.2.103'
        self.APIPortDict={}
        #e.g.:
        #'RPM'     : '25400',
        #'Apps0'   : '25370',
        #'Apps1'   : '25372',
        #etc.
        
        self.T32TempFolder='C:\\temp\\t32work'
        self.T32Container    = '\"//Root/DAP/Podbus Device Chain/Power Trace Ethernet/'
        self.T32SimContainer = '\"//Root/Simulator/'
        self.EmulationTarget = '10.46.163.10'
        self.SessionStartDelay = 2
        
        self.Protocols = []
        self.Modes = []
        self.PrimaryMode = []
        self.PrimaryProtocol = []
        self.CoreList=[]
        
        #each image has an alias list
        #each hwsubsystem has an image list
        #each t32 window has a hwsubsystem match
        
        self.SessionCoreName_AliasMap= { }
                        #e.g.:
                        #'RPM' : 'RPM',
                        #'APPS0' : 'Apps0',
                        #'APPS1' : 'Apps1',
                        #etc.
                        
        self.T32BinaryDict={ }
            #e.g.:
            #'APPS':'t32marm64.exe',
            #'ADSP':'t32mqdsp6.exe',
            #'MPSS':'t32mqdsp6.exe',

        self.hwsubsystem_to_t32arg_map={}
            #e.g.:
            #'APPS 0':'APPS0',
            #'MPSS 0':'MPSS',
        
        #This is to be assembled by .format() command in the GetT32SessionPathName
        #The keys are from SessionCoreName_AliasMap entries, so they're clean.
        #Should be all caps
        self.SessionCoreNameDict={}
                        #e.g.:
                        #'RPM':'{T32Container}RPM'
                        #'RPMSIM':'{T32SimContainer}RPMSim'
        
        #HWCoreNames = ['RPM-CORTEXM3','MSS-Q6','LPASS-Q6','SSC-Q6','APPS-HYDRA']
        self.HWCoreNames = []
        
        self.T32MasterWindow = 'Apps0'
        self.T32MasterSession=str(self.T32Container+'APPS0')

        #T32ConfigFilePath='\\common\\Core\\t32\\msm8996.ts2'
        self.T32ConfigFilePath=''

        #SoftwareImagesList=['RPM','MPSS','LA','WP','SLPI','ADSP','TZ','XBL','VSS']
        self.SoftwareImagesList=[]
            
        self.hwsubsystem_to_apiport_map={}
                        #e.g.:
                        #'APPS0-{HLOS}'   : '25370',
                        #'APPS1-{HLOS}'   : '25371',
                        #etc.
        self.hwsubsystem_to_instancenum_map={}
                        #e.g.:
                        #'APPS0'   : '0',
                        #'APPS1'   : '1',
                        #etc.
        self.hwsubsystem_to_intercomport_map={}
                        #e.g.:
                        #'APPS0-{HLOS}'   : '15370',
                        #'APPS1-{HLOS}'   : '15371',
                        #etc.
        self.hwsubsystem_to_t32sessioncfg_map={}
                        #e.g.:
                        #'RPM-CORTEXM3':'{T32Container}RPM'
                        #'MSS-Q6':'{T32Container}MPSS'
                        #'RPMSIM':'{T32SimContainer}RPMSim'
                        #etc.
        self.alias_to_t32simimage_map={}
                        #e.g.:
                        #'RPM':'ARM32Sim'
                        #'MPSS':'QDSP6Sim'
                        #'ADSP':'QDSP6Sim'
                        #'SLPI':'QDSP6Sim'
                        #'APPS':'ARM64Sim'
        self.swimage_to_t32sessioncfg_map={}                
                        #e.g.:
                        #'RPM':'{T32Container}RPM'
                        #'MPSS':'{T32Container}MPSS'
                        #'Linux':'{T32Container}APPS0'
                        #'RPMSIM':'{T32SimContainer}RPMSim'
                        #etc.
        ###################################################################
        #Alias maps. Find correct subsystem from aliases that user may give
        ###################################################################
                        
        self.alias_to_image_map={}
                        #e.g.:
                        #'RPM':'RPM',
                        #'RPM-CORTEXM3':'RPM',
                        #'LA':'LA',
                        #'LINUX':'LA',
                        #'WP':'WP',
                        #'MODEM':'MPSS"
        
        self.image_to_hwsubsystem_map={}
                        #e.g:
                        #'AOSS':'AOSS',
                        #'MPSS':'MPSS'
                        

        #Maps images from alias_to_image_map to debug images recognized by T32 metascripts
        #This is what gets passed to T32 metascripts, and matches what's in std_debug_mdm9655.cmm
        #Note that keys are caps, but entries are caps or lowercase - entries match
        self.image_to_debugoption_map={}
                        #e.g.:
                        #'RPM':'rpm',
                        #'MPSS':'mpss',
                        #'LE':'appsboot',
                        #etc.
        ########################Other variables#######################
        self.entrypointbyimage={}
                        
    ######################################Methods##############################################
    def GetImagefromAlias(self,alias):
        if alias is None:
            print "GetImagefromAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        
        alias=alias.upper()
        cleanalias=None
        for imagekey in self.alias_to_image_map.keys():
            if alias in self.alias_to_image_map[imagekey]:
                cleanalias=imagekey
        if cleanalias is None:
            print "TargetConfig error: image "+alias+" not recognized for this target ("+self.TargetName+")"
            return None
        return cleanalias
        
    def GetHWSubsystemfromAlias(self,image):
        if image is None:
            print "GetHWSubsystemfromAlias Error! Image given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        
        image=image.upper()
        cleanimage=self.GetImagefromAlias(image)
        if not cleanimage:
            print "GetHWSubsystemfromAlias error: image "+image+" not recognized for this target ("+self.TargetName+")"
            if DebugEnabled:
                import pdb; pdb.set_trace()
            return None
        cleanhwsubsystem=None
        for hwsubsystemkey in self.image_to_hwsubsystem_map.keys():
            if cleanimage in self.image_to_hwsubsystem_map[hwsubsystemkey]:
                cleanhwsubsystem=hwsubsystemkey
        if cleanhwsubsystem is None:
            print "GetHWSubsystemfromAlias error: image "+image+" not recognized for this target ("+self.TargetName+")"
            return None
        return cleanhwsubsystem
        
        
    def GetT32BinaryfromAlias(self,alias):
        if alias is None:
            print "GetImagefromAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        hwsubsystem=self.GetHWSubsystemfromAlias(alias)
        
        try:
            return self.T32BinaryDict[hwsubsystem]
        except:
            print "GetT32BinaryfromAlias error! alias given not recognized. Alias: "+alias
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            return None
            
    def GetT32WindowArgFromImageAlias(self,alias):
        if alias is None:
            print "GetT32WindowArgFromImageAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        hwsubsystem=self.GetHWSubsystemfromAlias(alias)
        try:
            return self.hwsubsystem_to_t32arg_map[hwsubsystem]
        except:
            print "Error! GetT32WindowArgFromImageAlias Failure. "
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            return None
        
    def CheckIfBootProcessor(self,bootprocalias):
        if bootprocalias is None:
            print "CheckIfBootProcessor Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        
        hwsubsystem=self.GetHWSubsystemfromAlias(bootprocalias)
        if hwsubsystem in self.BootProcessor.upper():
            return True
        else:
            return False
        
    #def GetT32WindowfromAlias(self,alias):
    #    if alias is None:
    #        print "GetImagefromAlias Error! alias given is None Type"
    #        if DebugEnabled is True:
    #            import pdb; pdb.set_trace()
    #    cleanalias=self.GetImagefromAlias(alias)
    #    if cleanalias in self.alias_to_t32window_map.keys():
    #        return self.alias_to_t32window_map[cleanalias]
    #    else:
    #        print "TargetConfig error: alias given: "+alias+" not recognized for this target ("+self.TargetName+")"
    #        if DebugEnabled is True:
    #            import pdb; pdb.set_trace()
    #            return None
    #        else:
    #            return None
        
    def GetSimContainerFromAlias(self,alias):
        if alias is None:
            print "GetImagefromAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        alias=alias.upper()
        if not (alias in self.alias_to_t32simimage_map.keys()):
            print "TargetConfig error: image "+alias+" not recognized for this target ("+self.TargetName+")"
            return None
        return self.alias_to_t32simimage_map[alias]        
        
    
    def GetDebugOptionFromImage(self,image):
        if image is None:
            print "GetDebugOptionFromImage Error! image given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            return None
            
        image=image.upper()
        
        cleanimage=self.GetImagefromAlias(image)
        if cleanimage is None:
            import pdb; pdb.set_trace()
            kill("Error - didn't get alias from image:  "+image)
        return self.image_to_debugoption_map[cleanimage]
    
    def GetT32SessionConfigByImage(self,image):
        if image is None:
            print "GetT32SessionConfigByImage Error! image given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            return None
            
        image=image.upper()
        if not (image in self.image_to_hwsubsystem_map.keys()):
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            print "TargetConfig error: image "+image+" not recognized for this target ("+self.TargetName+")"
            return None
        else:
            cleanhwsubsystemvalue=self.image_to_hwsubsystem_map[image]
            cleansessionvalue=self.hwsubsystem_to_t32sessioncfg_map[cleanhwsubsystemvalue]
            cleansessionvalue.format(T32Container=self.T32Container)
            return cleansessionvalue
    
    def GetEntryPointByImage(self,image):
        if image is None:
            print "GetEntryPointByImage Error! image given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            return None
        
        image=image.upper()
        cleanimage=self.GetImagefromAlias(image)
        if cleanimage:
            return self.image_to_entrypoint_map[cleanimage]
        else:
            print "TargetConfig error: image "+image+" not a valid image for this target ("+self.TargetName+")"
            return None
    
    def GetDebugOption(self,givendebugoption):
        if givendebugoption is None:
            print "GetDebugOption Error! givendebugoption given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            return None
        
        givendebugoption=givendebugoption.upper()
        if givendebugoption in self.image_to_debugoption_map.keys():
            return self.image_to_debugoption_map[givendebugoption]
        else:
            print "Error - could not find "+givendebugoption+" in available image to debugoption map"
            return None
    
    def GetAPIPortFromAlias(self,alias):
    
        if alias is None:
            print "GetAPIPortFromAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        hwsubsystem=self.GetHWSubsystemfromAlias(alias)
        
        try:
            return self.hwsubsystem_to_apiport_map[hwsubsystem]
        except:
            print "GetAPIPortFromAlias error! unknown hwsubsystem returned for alias: "+str(alias)
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            else:
                return None
    def GetInstanceValFromAlias(self,alias):
        if alias is None:
            print "GetInstanceValFromAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        hwsubsystem=self.GetHWSubsystemfromAlias(alias)
        
        try:
            return self.hwsubsystem_to_instancenum_map[hwsubsystem]
        except:
            print "Error! GetInstanceValFromAlias with alias: "+alias
            if DebugEnabled is True:
                import pdb; pdb.set_trace()        
            return None
            
        
    
    #FIXME - Would be good to arrange aliases so that we're flexible on what is returned
    def GetPortConfigFromAlias(self,alias):
        if alias is None:
            print "GetPortConfigFromAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        
        try:
            rvalue={'RCL':self.RCLValue,
                    'PORT':self.GetAPIPortFromAlias(alias),
                    'PACKLEN':self.PACKLENValue,
                    'NODE':self.APINodeValue,
                    }
        except:
            print "Error! GetPortConfigFromAlias with alias: "+alias
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
                
            return None
            
        return rvalue
        
        
        #cleancorename=self.SessionCoreName_AliasMap[t32window]
        #import pdb;pdb.set_trace()
        #
        ##If session given (e.g. //root/dap/podbus.../apps0)
        #if alias in self.SessionCoreNameDict.values():
        #    for key in self.SessionCoreNameDict.keys():
        #        if alias in self.SessionCoreNameDict[key] and self.SessionCoreNameDict[key] in alias:
        #            if key in self.APIPortDict.keys():
        #                return self.APIPortDict[key]
        #                
        #    kill("Error! GetPortConfigFromAlias - Expect there to be a match between SessionCoreNameDict values and APIPortDict keys")
        #else:
        #    import pdb; pdb.set_trace()
        #    print "Warning! alias "+str(alias)+" not found in Target config for "+self.CHIPSET
        #    return None
            
    def GetIntercomPortFromAlias(self,alias):
       
        if alias is None:
            print "GetIntercomPortFromAlias Error! alias given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
        hwsubsystem=self.GetHWSubsystemfromAlias(alias)
        if hwsubsystem:
            return self.hwsubsystem_to_intercomport_map[hwsubsystem]
        else:
            print "GetIntercomPortFromAlias error! unknown hwsubsystem returned for alias:"+str(alias)
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
            else:
                return None
    
    def GetT32SessionPathName(self,t32window):
        if t32window is None:
            print "GetT32SessionPathName Error! t32window given is None Type"
            if DebugEnabled is True:
                import pdb; pdb.set_trace()
                
        t32window=t32window.upper()
        cleancorename=self.GetImagefromAlias(t32window)
        if not cleancorename:
            print "Error: GetT32SessionPathName- unknown corename given. Available core names:"
            import pdb; pdb.set_trace()
            for key in self.SessionCoreName_AliasMap.keys():
                print key
            return None
        else:            
            T32sessionpathname=self.SessionCoreNameDict[cleancorename].format(T32Container=self.T32Container, T32SimContainer=self.T32SimContainer)
            return T32sessionpathname
            
        
    ###This function usually called from top level TargetConfigContainer to map target aliases to a proper target name
    def GetTargetAliasList(self):
        return self.TargetAliasList

    def GetT32ConfigFilePath(self):
        return self.T32ConfigFilePath

    def GetT32ConfigContainer(self):
        return self.T32Container
        
    def T32SimContainer(self):
        return self.T32SimContainer
        

    def GetSubsystemAPIPortConfig(self):
        #These should reflect what is in msm8996.ts2
        #Loops through the APIPortDict to give a full dictionary of intercom configs
        CoreIntercomConfig={}
        for apikey in self.APIPortDict.keys():
            CoreIntercomConfig.update({apikey:dict(self.CoreIntercomConfig_ElementTemplate, **{'PORT': self.APIPortDict[apikey]})})
        return CoreIntercomConfig
            
    def GetSubsysName_to_SimCoreName_map(self):
        return self.SubsysName_to_SimCoreName_map
        
    def GetHWCoreNamesList(self):
        return self.HWCoreNames
        
    def GetSoftwareImagesList(self):
        return self.SoftwareImagesList
    def GetDebugImagesList(self):
        return self.DebugImagesList
        
    def GetStartDelay(self):
        return self.StartDelay
        
    def GetT32MasterWindow(self):
        return self.T32MasterWindow
        
def kill(msg):
    if msg:
        print "Error: " + str(msg)
    sys.exit(1)
