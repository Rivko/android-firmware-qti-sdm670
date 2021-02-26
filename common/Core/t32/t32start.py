#===============================================================================
#
# t32start.py
#
# GENERAL DESCRIPTION
#    T32 startup script for windows
#
# Copyright (c) 2011-2012 by QUALCOMM Technologies Incorporated.
# All Rights Reserved.
#
# QUALCOMM Proprietary
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
# 07/28/2017 SONELALG     Improve t32select path logic
# 01/30/2017 JBILLING     Updated callable portions
# 11/17/2016 JBILLING     Made subroutines callable, added aliases
# 05/13/2016 JBILLING     Import gen_cmm_data3 instead of use as popen
# 03/03/2016 JBILLING     T32 version feature addition
# 02/12/2016 JBILLING     Improve path logic
# 10/09/2015 JBILLING     Add more targets (9x50, Fusion)
# 10/08/2015 JBILLING     Fix where temp directory is
# 10/01/2015 JBILLING     Automatic tracking of target name in metabuild
# 08/03/2015 JBILLING     Update for linux, path fixes
# 08/10/2014 AJCheriyan   Created v1.0 for Elessar
#==============================================================================

import glob, itertools, os, string, sys, subprocess, re, platform, shutil, fnmatch, time, tempfile, time, re
from optparse import OptionParser
global T32Start_Directory
T32Start_Directory=os.path.abspath(os.path.dirname(__file__))
        
Alias_List_sdx20=['sdx20','sdx20m','9x65','mdm9x65','mdm9665','9665']
Alias_List_sdx24=['sdx24','sdx24m','9x75','mdm9x75','mdm9675','9675']
Alias_List_sdx50=['sdx50','sdx50m']
Alias_List_9x55=['mdm9650','mdm9655','mdm9x55','mdm9655','mdm9150']
Alias_List_9x45=['mdm9640','mdm9645','mdm9x55','mdm9645']
Alias_List_8996=['msm8996']
Alias_List_8998=['msm8998']
Alias_List_sdm845=['sdm845']
Alias_List_sdm830=['sdm830']
Alias_List_8098=['apq8098']
Alias_List_sdm670=['sdm670']
Alias_List_sdm710=['sdm710']
Alias_List_qcs605=['qcs605']
Alias_List_fus8096_sdx20=['apq8096.*sdx20.']
Alias_List_fus8096_9x55=['apq8096.*mdm965.']
Alias_List_fus8096_9x45=['apq8096.*mdm964.']
Alias_List_fus8098_9x55=['apq8098.*mdm965.']
Alias_List_fus8998_9x55=['msm8998.*mdm965.']
Alias_List_fus8998_sdx50=['msm8998.*sdx50.']
Alias_List_fus845_sdx24=['sda845.*sdx24.','sda845.*sdx24m.']


TargetNameSearchList_dict={
    'mdm9645.ts2':     Alias_List_9x45,
    'mdm9655.ts2':     Alias_List_9x55,
    'sdx20.ts2':       Alias_List_sdx20,
	'sdx24.ts2':       Alias_List_sdx24,
	'sdx50.ts2':       Alias_List_sdx50,
    'msm8996.ts2':     Alias_List_8996,
    'msm8998.ts2':     Alias_List_8998,
    'sdm845.ts2':      Alias_List_sdm845,
	'apq8098.ts2':Alias_List_8098,
	'sdm670.ts2':Alias_List_sdm670,
	'sdm710.ts2':Alias_List_sdm710,
    'qcs605.ts2':Alias_List_qcs605,
    'fus80969655.ts2': Alias_List_fus8096_9x55,
    'fus8096sdx20.ts2':Alias_List_fus8096_sdx20,
    'fus80969645.ts2': Alias_List_fus8096_9x45,
	'fus845sdx24.ts2':Alias_List_fus845_sdx24,
	'fus89989655.ts2': Alias_List_fus8998_9x55,
    'fus8998sdx50.ts2':Alias_List_fus8998_sdx50
}
def GetTargetNameSearchList_dict():
    return TargetNameSearchList_dict

# Function to terminate program with a message
def debug(msg, force=False):
    debug_enable = False
    if debug_enable or force:
        print msg

def kill(msg):
    if msg:
        print msg
    sys.exit(1)

def IsWindows64():
    
    import sys
    if '2.7' in sys.version:
        import platform
        if '64' in platform.machine():
            return True
    
    #try with WMIC
    command = 'c:\\windows\\system32\\wbem\\WMIC.exe OS GET OSARCHITECTURE /Format:list'
    getosarch = subprocess.Popen(command, stderr = subprocess.PIPE, stdout = subprocess.PIPE)
    (cmdout, cmderr) = getosarch.communicate()
    if len(cmderr) is not 0:
        kill("Error getting OS Architecture. Found error: "+cmderr+", from command"+command)
    
    print 'Detecting Windows arch : ' + cmdout.strip()
    
    if cmdout.strip().endswith('64-bit'):
        debug('Detected 64 bit Windows')
        return True

def IsLinux64():
  return True

# Function to parse input options
def parseoptions():
    global options
    argparser = OptionParser(version='%prog 1.0')
    
    argparser.add_option('-t', '--target', dest='targetname', help='Name of the target: e.g. msm8994', metavar='NAME', default=None)
    argparser.add_option('-s', '--session', dest='session', help='Trace32 window to be opened e.g. RPM', metavar='SESS', default=None)

    (options, args) = argparser.parse_args()
    
def gettargetname():
    '''
        Finds what target *.ts2 file to use based on:
            available ts2 files, 
            what metabuild directory name is, 
            and what user's input is.
    '''
    #cases
    #   only 1 file in directory
    #       just use that one
    #   multiple files
    #       run through available files 
    #           run through available patterns, see if match to metaname
    #           if no match, print list of files, ask user
    #   no files - return error.
    
    configs = glob.glob(T32Start_Directory+'/*.ts2')
    filebasenames=[]
    for file in configs:
        filebasenames.append(os.path.basename(file).split('.')[0])
    files=os.listdir(T32Start_Directory)
    metaname=os.path.basename(os.path.abspath(os.path.join(T32Start_Directory,'..','..','..',)))
    
    
    
    if len(configs) is 0:
        kill("Error! No *.ts2 files present. Cannot open T32 for target")
    elif len(configs) is 1:
        #get the filename, e.g. for mdm9655.ts2, use mdm9655
        (options.targetname, ext) = os.path.splitext(os.path.basename(configs[0]))
    else:
        matchingtargetslist=[]
        for configfile in TargetNameSearchList_dict.keys():
            matchfound=False
            for stringmatchitem in TargetNameSearchList_dict[configfile]:
                if matchfound is True:
                    break
                
                if re.match(stringmatchitem.upper(),metaname.upper()):
                    matchfound=True
                    matchingtargetslist.append(configfile)
        ##For fusion case - it will turn up two options
        ##However, we only want the fusion option.
        if 'fus' in str(matchingtargetslist) and len(matchingtargetslist) is 2:
            if 'fus' in matchingtargetslist[0]:
                del matchingtargetslist[1]
            else:
                del matchingtargetslist[0]
        
        
        if len(matchingtargetslist) is 0:
            print "Available targets:"
            for filename in configs:
                #print out all available targets. get one that user specifies.
                print os.path.basename(filename).split(".")[0]
            options.targetname = raw_input("Enter Target Name: ")
                
        elif len(matchingtargetslist) is 1:
            options.targetname=matchingtargetslist[0].split('.')[0]
        else:
            print " "
            print "Full list of potential targets:"
            for filename in configs:
                print "\t"+os.path.basename(filename).split('.')[0]
            print " "
            print "Based on metabuild directory name, found the following potential matches:"
            for filename in matchingtargetslist:
                print "\t"+os.path.basename(filename).split(".")[0]
            print " "
            print "Please enter one of the above target name options"
            options.targetname = raw_input("Enter Target Name: ")
        
    
    #If we didn't get a proper target name
    if options.targetname not in filebasenames:
        print " "
        print "*** Unknown target name specified! ***"
        print " "
        print "Exiting..."
        time.sleep(3)
        kill("Unknown target name specified!")

    # Convert our  sessions to a list
    options.session = options.session.split(',') if options.session else options.session
    debug("Selected target: " + str(options.targetname), True)
    debug("Selected session: " + str(options.session), True)
    
    return options.targetname
    
    


    
def generate_buildinfo():
            if platform.system() != 'Linux':
                
                ################## Generated files ###################
                cmmfilename = 'gen_buildinfo.cmm'
                tmpfile = os.getenv('TEMP') + os.sep + cmmfilename
                
                
                print "Generating "+cmmfilename+" at location "+os.path.dirname(tmpfile)
                sys.path.append(os.path.abspath(os.path.join(__file__,'../../../build/app')))
                try:
                        import gen_cmm_data3 as gen_cmm
                        gen_cmm.create_cmm(os.path.join('..', '..', '..'), tmpfile,'-paths_only')
                except:
                        print "\n\n    Updated gen_cmm_data3 not present. \n    Using legacy popen method (won't work for BRF excel links)\n"
                        time.sleep(5)
                        print "   Generating cmm data..."
                        cmd = ' '.join(['python.exe', os.path.join('..','..', 'build', 'app', 'gen_cmm_data3.py ', os.path.join('..', '..', '..')), tmpfile, ' -paths_only'])
                        
                        debug('Temp filename: ' + tmpfile)
                        debug('Current Working Dir: ' + T32Start_Directory)
                        debug('Generate MetaBuild Information: ' + cmd)
                        
                        # Make the call to generate the cmm script
                        genfilecall = subprocess.Popen(cmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
                        (stdout, stderr) = genfilecall.communicate()
            else:
                # File which needs to be created
                cmmfilename = 'gen_buildinfo.cmm'
                tmpfile = os.getenv('TEMP') + os.sep + cmmfilename

                print(T32Start_Directory)

                cmd = ' '.join(['python', os.path.join('..', '..', 'build', 'app', 'gen_cmm_data3.py ', os.path.join('..', '..', '..')), tmpfile, ' -paths_only'])

                print "cmd :" 
                print cmd

                debug('Temp filename: ' + tmpfile)
                debug('Current Working Dir: ' + T32Start_Directory)
                debug('Generate MetaBuild Information: ' + cmd)

                # Make the call to generate the cmm script
                #genfilecall = subprocess.Popen(cmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
                #(stdout, stderr) = genfilecall.communicate()

                #call cmd
            return
            

def set_t32_environment(t32directory=None,targetname=None):
        
        ################### T32 location ###########################
        ### Set the additional environment variables
        ### Set T32 start directory
        ###
        if targetname is None:
            targetname=options.targetname
        if platform.system() != 'Linux':
            if t32directory is None:
                t32dir=t32select(targetname=targetname)
            elif os.path.exists(t32directory):
                t32dir=t32directory
            else:
                kill("Error! Could not access given t32 directory: "+t32directory)
                
            # Set the location of the T32 installation
            os.environ['T32SYS'] = t32dir
            
        else:
            # Set the additional environment variables
            # Set T32 start directory
            t32dir = ''
            t32dir = os.environ['T32SYS']
            if os.path.isdir(t32dir):
                t32dir = t32dir
            else:
                t32dir = raw_input('Enter the location of the T32 installation: ')
            # Set the location of the T32 installation
        
        
        os.environ['SCRIPTSDIR'] = os.path.join(T32Start_Directory, '..', 'tools', 'cmm')
        
        return t32dir
        
def start32(t32dir):
    if platform.system() != 'Linux':

        
        ########################## Now start T32 #########################
        windows_ver = 'windows'
        if IsWindows64():
            windows_ver = 'windows64'

        # Find the version of the exe to execute
        t32exe = os.path.join(t32dir,'bin', windows_ver, 't32start.exe')
        if not os.path.isfile(t32exe):
            t32exe = os.path.join(t32dir,'t32start.exe')
            if not os.path.isfile(t32exe):
                kill('T32start executable not found at : ' + t32exe)

        # Put together the command
        
        t32startcmd = ' '.join([ t32exe, '-runcfgfile', os.path.join(T32Start_Directory,options.targetname) + '.ts2'])

        # Adjust now
        if (options.session) and ('all' not in options.session ):
            for session in options.session:
                t32finalcmd = t32startcmd + ' -runitem \"//Root/DAP/Podbus Device Chain/Power Trace Ethernet/' + session + '\" -QUIT'
                debug("Trace32 start command is : " + t32finalcmd, True)
                subprocess.Popen(t32finalcmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE).communicate()
                time.sleep(5)
        else:
            subprocess.Popen(t32startcmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE)


    else:
        print "Detected running on Linux."
       
        # Now start T32
        linux_ver = 'pc_linux'
        if IsLinux64():
            linux_ver = 'pc_linux64'

        # Find the version of the exe to execute
        t32exe = os.path.join(t32dir,'bin', linux_ver, 't32marm64-qt')
        if not os.path.isfile(t32exe):
            kill('T32start executable not found at : ' + t32exe)

        print "Detected running on Linux, ready to start t32."
        
        #Get absolute path of startup.cmm
        current_dir = os.path.dirname(os.path.realpath(__file__))
        start_up = os.path.join('..', 'tools', 'cmm', 'config', 'startup.cmm')
        start_up = os.path.join(current_dir, start_up)

        os.chdir(os.path.join('..', 'tools', 'cmm'))

        subprocess.call([t32exe, "-c", os.getenv('TEMP')+os.sep+options.targetname.upper()+"_APPS0.t32, ", start_up, "DAP", options.targetname.upper() , "APPS 0 SWD"])

def getpath(directory=None):
    
    
    if directory is None:
        if os.path.exists(os.path.dirname(sys.argv[0])):
            #T32Start_Directory=os.path.abspath(os.path.dirname(sys.argv[0]))
            temppath=os.path.abspath(os.path.dirname(sys.argv[0]))
        elif os.path.exists(os.path.join(os.getcwd(),os.path.dirname(sys.argv[0]))):
                #T32Start_Directory=os.path.join(os.getcwd(),os.path.dirname(sys.argv[0]))
                temppath=os.path.join(os.getcwd(),os.path.dirname(sys.argv[0]))
        else:
            configs = glob.glob('./*.ts2')
            if len(configs) is 0:
                kill("Error occurred when locating t32start.py directory - missing *.ts2 files.")
            else:
                #T32Start_Directory=os.path.abspath(os.getcwd())
                temppath=os.path.abspath(os.getcwd())
    elif os.path.exists(directory):
        #T32Start_Directory=directory
        temppath=directory
        
        
        #configs = glob.glob(os.path.join(T32Start_Directory,'*.ts2'))
        configs = glob.glob(os.path.join(temppath,'*.ts2'))
        if len(configs) is 0:
            kill("Error occurred when locating t32start.py directory - missing *.ts2 files.")
    else:
        kill("Error occurred when finding directory: "+directory)
            
            
def t32select(targetname=None):
        # Set the additional environment variables
        # Set T32 start directory
        import json, subprocess
        
        if targetname is None:
            targetname=options.targetname
            
        if 'mdm9655' in targetname:
            Preferred_Local_T32_Directory="C:\\T32\LT150831"
        else:
            Preferred_Local_T32_Directory="C:\\T32"
        
        
        workingdir=os.path.abspath(os.path.join(T32Start_Directory,'../../build/app'))
        
        #Couldn't find metabuild files. Get the best local directory and return.
        if not os.path.exists(os.path.join(workingdir,'meta_cli.py')):
            print "Warning - could not find meta_cli.py in "+workingdir
            print "Cannot determine correct version of T32 from contents.xml"
            if os.path.exists(Preferred_Local_T32_Directory):
                print "Using T32 from default directory: "+ Preferred_Local_T32_Directory
                time.sleep(4)
                return str(Preferred_Local_T32_Directory)
            else:
                t32dir = raw_input('Enter the location of the T32 installation: ')
                if os.path.isdir(str(t32dir)):
                    return str(t32dir)
                else:
                    change_console_color("red")
                    print "Error! Could not access specified directory. Exiting"
                    print "User specified: " + str(t32dir)
                    time.sleep(5)
                    kill("Error! Could not access specified directory. Exiting")
                    
                    
        #d_out = subprocess.Popen("python metaCLI.py get_var_values attr=cmm_var",stdout=subprocess.PIPE,shell=True,cwd=os.path.abspath(os.path.join(__file__,'../../../build/app')))
        #d_out = subprocess.Popen(["pushd "+currentdir,"python metaCLI.py get_var_values attr=cmm_var","popd"],stdout=subprocess.PIPE,shell=True)
        #d_out = subprocess.Popen("python "+workingdir+"/meta_cli.py get_var_values attr=cmm_var",stdout=subprocess.PIPE,shell=True,cwd=os.path.abspath(os.path.join(__file__,'../../../build/app')))
        
        d_out = subprocess.Popen("python "+workingdir+"/meta_cli.py get_var_values attr='cmm_var'",stdout=subprocess.PIPE,shell=True,cwd=os.path.abspath(os.path.join(__file__,'../../../build/app')))
        stdout, stderr = d_out.communicate()
        
        #Error condition
        if (stdout in '') or ( not (stderr is None)) or '{}\r\n' in stdout:
            d_out = subprocess.Popen("python "+workingdir+"/meta_cli.py get_var_values attr=cmm_var",stdout=subprocess.PIPE,shell=True,cwd=os.path.abspath(os.path.join(__file__,'../../../build/app')))
            stdout, stderr = d_out.communicate()
        if (stdout in '') or ( not (stderr is None)) or '{}\r\n' in stdout:
            
            change_console_color("red")

            #Error condition.
            print  " " 
            print  " " 
            print  " " 
            print  "Some error ocurred trying to obtain metabuild data from contents.xml."
            print  "Using default T32 directory at "+Preferred_Local_T32_Directory             
            print  " " 
            print  " " 
            time.sleep(3)
            if os.path.exists(Preferred_Local_T32_Directory):
                t32dir=Preferred_Local_T32_Directory
                return t32dir
            else:
                print "Could not use T32 installation at "+Preferred_Local_T32_Directory
                t32dir = raw_input('Enter the location of the T32 installation: ')
                if os.path.isdir(str(t32dir)):
                    return str(t32dir)
                else:
                    change_console_color("red")
                    print "Error! Could not access specified directory. Exiting"
                    print "User specified: " + str(t32dir)
                    time.sleep(5)
                    kill("Error! Could not access specified directory. Exiting")

                
            
        xmldict = json.loads(stdout)
		
		########################################
		### this will handel the t32 default ###
		###location and it would be SD server###
		#### and internal build and external ###
		#### build                         #####
		########################################
		
        default_t32_server=None
        SD_t32_server=xmldict.get('T32_VER_PATH_SD')
        QIPL_t32_server=xmldict.get('T32_VER_PATH_QIPL')
        TEX_t32_server=xmldict.get('T32_VER_PATH_TEX')
        if (not (SD_t32_server is None)):
            default_t32_server=SD_t32_server
        elif (not (QIPL_t32_server is None)):
            default_t32_server=QIPL_t32_server
        else :
            default_t32_server=TEX_t32_server
       
        #internalbuild=xml.internal
        #internal build will have these
        if 'T32_VER' in xmldict.keys() and ( not (default_t32_server is None)):
        
                t32_version=str(xmldict['T32_VER'])
                
                #For mdm case where users are instructed to store T32 location at (for example) C:\\T32\\LT150831, 
                #If t32_version was updated and the local directory exists, use the updated directory
                if (os.path.isdir(os.path.join('C:\\T32',t32_version))) and ('mdm9655' in options.targetname):
                    Preferred_Local_T32_Directory=os.path.join('C:\\T32',t32_version)
            
			
                if os.path.isdir(Preferred_Local_T32_Directory):
                    #Gives us "Unknown" if failed to get version
                    localT32=read_t32_version(Preferred_Local_T32_Directory)
                    
                    if os.path.isdir(default_t32_server):
                        #Gives us "Unknown" if failed to get version
                        recommendedT32=read_t32_version(default_t32_server)
                        
                        #'Preferred_Local_T32_Directory' accessible but couldn't get server version
                        if recommendedT32 in "Unknown":
                            change_console_color("yellow")
                            print "Error occurred reading server T32 version."
                            print "Unknown if local "+Preferred_Local_T32_Directory+" is correct T32 version"
                            print "Resorting to "+Preferred_Local_T32_Directory+" directory"
                            time.sleep(5)
                            t32dir=Preferred_Local_T32_Directory
                            return t32dir
                        #Local T32 and server T32 match. Best case scenario
                        if localT32 in recommendedT32:
                            change_console_color("green")
                            print "Local "+Preferred_Local_T32_Directory+" build matches recommended server T32 version"
                            time.sleep(2)
                            #If they're equivalent, just return with 'Preferred_Local_T32_Directory'
                            t32dir=Preferred_Local_T32_Directory
                            return t32dir
                        #Local T32 mismatches with server T32.
                        else:
                            ######################################################
                            ####This is the error we expect most people to get####
                            ######################################################
                            change_console_color("red")

                            print  "T32start detected that local ("+Preferred_Local_T32_Directory+") version does not"             
                            print  "match recommended version for this metabuild:"                  
                            print  "Local T32 version at "+Preferred_Local_T32_Directory+": "+localT32                           
                            print  "Recommended T32 version:      "+recommendedT32                     
                            print  " "                                                                 
                            print  "It is recommended to download T32 from above location to "+Preferred_Local_T32_Directory
                            print  " "                                                                 
                            print  " Here are several locations: "                                     
                            print  "     San Diego: "+SD_t32_server                                    
                            print  "     QIPL:      "+QIPL_t32_server                                  
                            print  "     Austin Tx: "+TEX_t32_server                                   
                            print  "Please enter preferred T32 location. "
                            print  "If you'd still like to use location at "+Preferred_Local_T32_Directory+", enter that below"
                            time.sleep(3)
                            t32dir = raw_input('Enter the location of the T32 installation: ')
                            if os.path.isdir(str(t32dir)):
                                return str(t32dir)
                            else:
                                change_console_color("red")
                                print "Error! Could not access specified directory. Exiting"
                                print "User specified: " + str(t32dir)
                                time.sleep(5)
                                kill("Error! Could not access specified directory. Exiting")
                            
                            return t32dir
                    
                    else:
                        #Case for C:\T32 is accessible but server location not accessible
                        change_console_color("yellow")
                        print "Could not access preferred server location, listed below"
                        print "Cannot determine if "+Preferred_Local_T32_Directory+" is best version for this metabuild."
                        print " "
                        print "Resorting to "+Preferred_Local_T32_Directory
                        print " "
                        print Preferred_Local_T32_Directory+" version is: " + localT32
                        print "Server locations (inaccessible) are:"
                        print "     San Diego: "+SD_t32_server     
                        print "     QIPL:      "+QIPL_t32_server       
                        print "     Austin Tx: "+TEX_t32_server   
                        print " "
                        time.sleep(5)
                        
                        t32dir=Preferred_Local_T32_Directory
                        return t32dir
                        
                
                else:
                    #Couldn't find c:\t32, but server is accessible
                    if os.path.isdir(default_t32_server):
                        #Gives us "Unknown" if failed to get version
                        recommendedT32=read_t32_version(default_t32_server)
                        change_console_color("yellow")
                        
                        print "T32start cannot find local directory - "+Preferred_Local_T32_Directory
                        print "Recommended T32 version is below"
                        print "Please paste the appropriate location from below to the prompt"
                        print " or copy T32 from one of below locations to "+Preferred_Local_T32_Directory+" locally"
                        print " "
                        print "     San Diego: "+SD_t32_server     
                        print "     QIPL:      "+QIPL_t32_server                           
                        print "     Austin Tx: "+TEX_t32_server           
                        t32dir = raw_input('Enter the location of the T32 installation: ')
                        if os.path.isdir(str(t32dir)):
                            return str(t32dir)
                        else:
                            change_console_color("red")
                            print "Error! Could not access specified directory. Exiting"
                            print "User specified: " + str(t32dir)
                            time.sleep(5)
                            kill("Error! Could not access specified directory. Exiting")
                    #can't access C:\t32 or server
                    else:
                        change_console_color("yellow")
                        print "Could not access "+Preferred_Local_T32_Directory+" or preferred server locations of T32"
                        print "Recommended T32 is " + t32_version
                        t32dir = raw_input('Enter the location of the T32 installation: ')
                        if os.path.isdir(str(t32dir)):
                            return str(t32dir)
                        else:
                            change_console_color("red")
                            print "Error! Could not access specified directory. Exiting"
                            print "User specified: " + str(t32dir)
                            time.sleep(5)
                            kill("Error! Could not access specified directory. Exiting")
                            
                            
                       
        #For external customer
        elif 'T32_BUILD' in  xmldict.keys():
            if os.path.isdir(Preferred_Local_T32_Directory):
                t32dir = Preferred_Local_T32_Directory
            else:
                print "Did not detect T32 installation at "+Preferred_Local_T32_Directory
                t32dir = raw_input('Enter the location of the T32 installation: ')
                
                
            localT32=read_t32_version(t32dir)
            #covered
            if localT32 in xmldict['T32_BUILD']:
                change_console_color("green")
                print "Local T32 at "+Preferred_Local_T32_Directory+" T32 matches recommended build number"
                sleeptime=2
            else:
                change_console_color("yellow")
                print "Warning: Local T32 build number does not match recommended T32 build number"
                print "Launching T32 from " + Preferred_Local_T32_Directory
                sleeptime=4
            print "Local T32 build number:       "+localT32
            print "Recommended T32 build number: "+str(xmldict['T32_BUILD'])
            
            time.sleep(sleeptime)
            return t32dir
        else:
            #change_console_color("yellow")
            print "Warning: Contents.xml does not have T32 Build information"
            #print "Cannot determine recommended version of T32 to use with this build"
            if os.path.isdir(Preferred_Local_T32_Directory):
                t32dir = Preferred_Local_T32_Directory
            else:
                t32dir = raw_input('Enter the location of the T32 installation: ')
            
            localT32=read_t32_version(t32dir)
            
            print "Selected T32 build number: "+localT32
            time.sleep(3)
            return t32dir

            
def choose_new_t32_unknown_directory():
    import os
    directorylist=os.listdir("C:\\")
    t32dirs=[]
    for dir in directorylist:
        if "t32" in dir.lower():
            t32dirs.append(str(dir))
            
            
def read_t32_version(t32_path):
    import re
    
    t32file=os.path.abspath(os.path.join(t32_path,'bin','windows64','t32marm64.exe'))

    f=open(t32file,"rb")
    filestring=f.read()
    f.close()
    searchresult=re.search('VERSION-INFO@',filestring)
    if searchresult:
        position=searchresult.end()
        softwarebuild=filestring[position:(position+8)]
        return softwarebuild
    else:
        print "Warning! Could not parse out version information from t32 file."
        return "Unknown"

    #pdb.set_trace()
    #if os.path.isdir(t32_path):
    #    t32file=str(t32_path+"\\version.t32")
    #    if os.path.isfile(t32file):
    #        try:
    #            lines=open(t32file).read().split('\n')
    #        except IOError:
    #            return "Unknown"
    #        for line in lines:
    #            #Expecting something like 'Software Version : R.2014.02.000053817'
    #            #Parse up to ':' and return software build
    #            if 'Software Version' in line:
    #            
    #                softwareline=line
    #                softwarebuild=line[line.index(':')+2:]
    #                return softwarebuild
    #     
    ##All unsuccessful cases return false
    return "Unknown"
            
def change_console_color(color):
    if str(color) in "red":
        color_code=12
    elif str(color) in "yellow":
        color_code=30
    elif str(color) in "green":
        color_code=26
    #default to red
    else:
        color_code=12
        
        
    from ctypes import windll
    STD_OUTPUT_HANDLE = -11
    stdout_handle = windll.kernel32.GetStdHandle(STD_OUTPUT_HANDLE)
    red=12
    windll.kernel32.SetConsoleTextAttribute(stdout_handle,color_code)
    return


    
# @Description
#
#   Main routine
#

if __name__ == '__main__':

    getpath()
    parseoptions()
    gettargetname()
    generate_buildinfo()
    t32dir = set_t32_environment()
    start32(t32dir)





