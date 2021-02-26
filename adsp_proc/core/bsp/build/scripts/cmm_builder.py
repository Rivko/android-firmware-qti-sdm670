#===============================================================================
#
# CBSP Products CMM Builder
#
# GENERAL DESCRIPTION
#    Contains rules to generate CMM scripts related info from the build
#
# Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary
#
#-------------------------------------------------------------------------------
#

#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/13/2015 JB      Updated for postmortem script
# 07/02/2015 JB      Updated scons structure for simpler command line, fixed dictionary clobbering
# 01/14/2015 JB      Default to SLPI_PROC if that is in the environment
# 12/17/2014 JB      Update to accomodate multiple scons environments
# 09/12/2014 JB      Major Rev to clean up rootenv[MAINCMMDICT] structure
# 07/07/2014 JB      Major Rev for correct scons implementation
# 04/30/2013 AJC     v2 version. New checks and tags in place
# 10/27/2012 AJC     Added debugtrace and products tech areas, cleaned up scripts
# 09/07/2012 AJC     Added to RPM BF 1.0 Warehouse
# 09/07/2012 AJC     Added support for args to be passed for each script
# 05/07/2012 AJC     Added separate file for T32 extensions
# 11/11/2011 AJC     Created for Aragorn (MPSS 2.0 and ADSP 2.0)
#==============================================================================

import os
import subprocess 
import string
import re
import glob
import fnmatch
import stat
import pprint
pp = pprint.PrettyPrinter(indent=4)

# Define any globals out here
# Valid images on which the builder will be used
valid_images = {'core_rpm': 'RPM', 'core_adsp':'ADSP','core_slpi':'SLPI', 'core_modem':'MPSS','root_modem':'MODEM','core_wcn':'WCNSS','core_apps':'APPS','core_cdsp':'CDSP'}
# Tech teams
valid_areas = ['kernel','services','systemdrivers','power','debugtools','debugtrace','buses','storage','hwengines','dal', 'debug', 'mproc', 'securemsm', 'products']
# CMM Builder Globals
debug_filename='cmmbuilder_debug.txt'
t32menu_filename="std_scripts.men"
t32extn_filename="std_extensions.cmm"
t32config_filename="std_toolsconfig.cmm"
default_indent="\t"
IMAGE="image"
AREA="area"
CMMSCRIPTS="scripts"
ARGS="Arguments"
CMMBUILDER_LIST="ScriptsList"
CMMBUILDER_DICT="ScriptsDict"
MAINCMMDICT="MainCMMDict"
CMMFILES="CmmFiles"
CMMSCRIPTSCNT="ScriptsCount"
CMMBUILDER_TAG="ScriptsTag"
FILEPATTERNS=['*.cmm','*.per','*.men','*.t32']
EXTENSION_FILEPATTERNS=['.men','.t32','.opt']
OSAM_FILEPATTERNS=['.t32','.opt','.men']
# T32 icons to be used for the different extensions
FILEPATTERNS_ICONS={'.cmm':':FLASH','.per':':CHIP', '.men':':ACONFIG','.t32':':ACONFIG','.opt':'ACONFIG'}
FILEPATTERNS_CMDS={'.cmm':'cd.do','.per':'per.view', '.men':'menu.reprogram', '.t32':'task.config', '.opt':'task.option'}
RELPATH=''

ComponentScriptListDirectory='${BUILD_ROOT}/core/products/build/${SHORT_BUILDPATH}/scriptlist'


# Other variables to be initialized
current_buildtag = ''


###############################################################
# Scons functions
###############################################################
def exists(env):
    return env.Detect('cmm_builder')

def generate(env):
    # Add methods to the scons environment
    env.AddUsesFlags('USES_CMMBUILDER')
    env.AddUsesFlags('USES_CMMBUILDER_V2')
   
    rootenv = env.get('IMAGE_ENV')
    rootenv[CMMFILES]=dict()
    rootenv[MAINCMMDICT]=dict()
    
    cmmbuilder_generate(env)

    
def cmmbuilder_generate(env):
    
    ####################Register builders with Scons######################
    #These are the API's for adding scripts at tech-team-level.
    env.AddMethod(add_cmm_script2, "AddCMMScripts2")  
    env.AddMethod(add_cmm_script, "AddCMMScripts")  
    #For post-mortem API
    env.AddMethod(add_cmm_post_mortem, "AddCMM_PostMortem")   
    #env.AddMethod(add_cmm_script_pm, "AddCMMScripts_PM")  
    #API's for top level scripts - to gather scripts added at component level
    env.AddMethod(cmm_component_builder_wrapper, "CreateCMMImageFile")
    env.AddMethod(cmm_component_builder_wrapper, "CreateCMMComponentFile")
    #This invokes the top-level/image level scons builder (CMMImageBuilder)
    env.AddMethod(cmm_image_builder_wrapper, "CMMGenerateFiles")
    env.AddMethod(cmm_image_builder_wrapper, "CMMBuilderGenerateFiles")
    #This is for backwards compatibility. Shouldn't do anything.
    env.AddMethod(cmm_dummy, "CMMBuilder")
    
    cmmimagebuilder_act      = env.GetBuilderAction( cmm_image_builder,        action_source=None )
    cmmcomponentbuilder_act  = env.GetBuilderAction( cmm_component_builder,    action_source=None )
    
    
    
    
    #Called at image level environment. 
    #Creates file to be later scanned 
    #by top level scons cmmbuilder
    cmm_component_builder_bld = env.Builder(  action = cmmcomponentbuilder_act, 
                                        suffix='.txt', 
                                        emitter = cmm_component_builder_emit,
                                        src_suffix='.something')
    
    #Called at top level scons. 
    #Collects lower level files 
    #and generates cmm scripts
    cmm_image_builder_bld      = env.Builder(  action = cmmimagebuilder_act, 
                                        suffix='.men', 
                                        emitter = cmm_image_builder_emit,
                                        src_suffix='.something')
                                        
                                        
    #cmmimagefilebuilder_bld      = env.Builder(  action = cmmimagefilebuilder_act, 
    #                                    suffix='.men', 
    #                                    emitter = cmmimagefilebuilder_emit,
    #                                    src_suffix='.something')      
                                        
                                        
                                        
    env.Append(BUILDERS = {'CMMImageBuilder'     : cmm_image_builder_bld,
                           'CMMComponentBuilder': cmm_component_builder_bld,
                           })       

    


    
######################################################
#   Name: add_cmm_post_mortem
#   Action: Primary API by which tech team adds their script. 
#           Adds scripts and commands to 'scripts bucket' which 
#           is a dictionary in the env. Later this is picked up
#           by script generating methods
#   Notes:
#######################################################
def add_cmm_post_mortem(env, scriptpaths, scriptdict, area):
    rootenv = env.get('IMAGE_ENV')
    image = get_image_name(rootenv)
    # Check the image argument and add it to the dictionary
    
    set_image_name(rootenv,image,area)

    cmmdict = dict()
    cmmdict[AREA]=[]
    # Check the area argument and add it to the dictionary
    if area is False:
        cmmbuilder_error(env, "Area cannot be empty")
        return
    elif area.lower() not in valid_areas:
        cmmbuilder_error(env, "Area: "+ area + " is not valid.")
        return
    else:
        cmmdict[AREA] = area[0].upper() + area[1:].lower()

    
    source_cmmscripts=[]
    
    if area not in rootenv[MAINCMMDICT].keys():
        areadict={area:dict()}
        rootenv[MAINCMMDICT].update(areadict)
    

    for scriptname in scriptdict.keys():

        
            dropdown_arg_pair_list = scriptdict[scriptname]
            #3 cases can occur:
            #   dropdownname is a string
                #pp scriptdict: {'DALLog.cmm': 'DAL Log'}
            #   dropdownname is a list
                #pp scriptdict: {'bamsetup.cmm': ['Setup BAM Scripts', '8996']}
            #   dropdownname is a 2 by X array.
            
            if type( dropdown_arg_pair_list ) is str: #covers the first case
                    dropdown_arg_pair_list_formatted = [[scriptdict[scriptname],''],['NULL','NULL']]
            elif type( dropdown_arg_pair_list ) is list:
                
                if type(scriptdict[scriptname][0]) is str:
                    dropdown_arg_pair_list_formatted=[  scriptdict[scriptname],['NULL','NULL']  ]
                    
                else:
                    scriptdict[scriptname].append( ['NULL','NULL'] )
                    dropdown_arg_pair_list_formatted=scriptdict[scriptname]
                
                
                
                
            for single_name_argument_pair in dropdown_arg_pair_list_formatted:
                if single_name_argument_pair[0] is 'NULL': break
                
                drop_down_menu_name = single_name_argument_pair[0]
                mode = "post-mortem"
                scriptpath = rootenv.RealPath(scriptpaths[0]+'/'+scriptname)
                (title,ext)= os.path.splitext(os.path.basename(scriptname))
                arguments = str(dropdown_arg_pair_list_formatted[dropdown_arg_pair_list_formatted.index(single_name_argument_pair)][1])
                
                rootenv[MAINCMMDICT][area].update( {drop_down_menu_name :
                                                     {'scriptname'          : scriptname,
                                                      'dropdown menu name'  : single_name_argument_pair[0],
                                                      'arguments'           : arguments,
                                                      'abspath'             : rootenv.RealPath(scriptpath),
                                                      'absdirectory'        : os.path.dirname(rootenv.RealPath(scriptpath)),
                                                      'extension'           : ext,
                                                      'mode'                : mode
                                                     }
                                                } )
            
    
def add_cmm_script2(env, scriptpaths, scriptdict, area):
    
    image = get_image_name(env)
    

    add_cmm_script(env, image, scriptpaths, scriptdict, area)

######################################################
#   Name: add_cmm_script
#   Action: Primary API by which tech team adds their script. 
#           Adds scripts and commands to 'scripts bucket' which 
#           is a dictionary in the env. Later this is picked up
#           by script generating methods
#   Notes:
#######################################################
def add_cmm_script(env, image, scriptpaths, scriptdict, area):
    rootenv = env.get('IMAGE_ENV')
    
    # Check the image argument and add it to the dictionary
    
    set_image_name(rootenv,image,area)
    

    
    cmmdict = dict()
    cmmdict[AREA]=[]
    # Check the area argument and add it to the dictionary
    if area is False:
        cmmbuilder_error(env, "Area cannot be empty")
        return
    elif area.lower() not in valid_areas:
        cmmbuilder_error(env, "Area: "+ area + " is not valid.")
        return
    else:
        cmmdict[AREA] = area[0].upper() + area[1:].lower()

    
    source_cmmscripts=[]
    
    if area not in rootenv[MAINCMMDICT].keys():
        areadict={area:dict()}
        rootenv[MAINCMMDICT].update(areadict)
    

    for scriptname in scriptdict.keys():

        
            dropdown_arg_pair_list = scriptdict[scriptname]
            #3 cases can occur:
            #   dropdownname is a string
                #pp scriptdict: {'DALLog.cmm': 'DAL Log'}
            #   dropdownname is a list
                #pp scriptdict: {'bamsetup.cmm': ['Setup BAM Scripts', '8996']}
            #   dropdownname is a 2 by X array.
            
            if type( dropdown_arg_pair_list ) is str: #covers the first case
                    dropdown_arg_pair_list_formatted = [[scriptdict[scriptname],''],['NULL','NULL']]
            elif type( dropdown_arg_pair_list ) is list:
                
                if type(scriptdict[scriptname][0]) is str:
                    dropdown_arg_pair_list_formatted=[  scriptdict[scriptname],['NULL','NULL']  ]
                    
                else:
                    scriptdict[scriptname].append( ['NULL','NULL'] )
                    dropdown_arg_pair_list_formatted=scriptdict[scriptname]
                
                
                
                
            for single_name_argument_pair in dropdown_arg_pair_list_formatted:
                if single_name_argument_pair[0] is 'NULL': break
                
                drop_down_menu_name = single_name_argument_pair[0]
                mode = "live-debug"
                scriptpath = rootenv.RealPath(scriptpaths[0]+'/'+scriptname)
                (title,ext)= os.path.splitext(os.path.basename(scriptname))
                arguments = str(dropdown_arg_pair_list_formatted[dropdown_arg_pair_list_formatted.index(single_name_argument_pair)][1])
                
                rootenv[MAINCMMDICT][area].update( {drop_down_menu_name :
                                                     {'scriptname'          : scriptname,
                                                      'dropdown menu name'  : single_name_argument_pair[0],
                                                      'arguments'           : arguments,
                                                      'abspath'             : rootenv.RealPath(scriptpath),
                                                      'absdirectory'        : os.path.dirname(rootenv.RealPath(scriptpath)),
                                                      'extension'           : ext,
                                                      'mode'                : mode
                                                     }
                                                } )
            

###########################################################
#   Name: cmm_component_builder_wrapper
#   Action: Wrapper Function which calls Scons builder - cmm_component_builder, 
#           which creates the image level file
#           This file will later be searched by top image's scons builder
#           to assemble cmm scripts (cmm_image_builder)
#   Notes:
###########################################################
def cmm_component_builder_wrapper(*arg):
    env=arg[0]
    rootenv = env.get('IMAGE_ENV')
    
    #done in emitter
    #cmm_master_hash=env.Value(str(rootenv[MAINCMMDICT]))

    cmm_target_scripts_list=[]
    for area in rootenv[MAINCMMDICT].keys(): 
        for dropdownkey in rootenv[MAINCMMDICT][area]: 
            cmm_target_scripts_list.append(rootenv[MAINCMMDICT][area][dropdownkey]['abspath'])
        

    imagefile=env.RealPath(ComponentScriptListDirectory+'/'+rootenv['IMAGE_NAME']+'_cmmoutputlist.txt')
    
    #Call the builder
    cmm_image_file=env.CMMComponentBuilder(imagefile, cmm_target_scripts_list)
    
    
    return cmm_image_file
    
    
###########################################################
#   Name: cmm_component_builder_emit
#   Action: Emitter to create dependency of cmm_component_builder on dictionary (MAINCMMDICT)
#           generated by add_cmm_script calls
#           Ensures that cmm_component_builder is called after dictionary is filled out
#   Notes: None
###########################################################
def cmm_component_builder_emit(env, target, source):
    rootenv = env.get('IMAGE_ENV')
    
    #Depends on the hash value of the MAINCMMDICT dictionary.
    #In order to run the builder only after the dictionary has
    #been fully populated
    rootenv.Depends(target, rootenv.Value(str(rootenv[MAINCMMDICT])))

    return (target, source)
   
###########################################################
#   Name: cmmbuilder_generate_files
#   Action: Generates a file with all the cmm scripts added to current env
#   Notes: None
###########################################################
def cmm_component_builder(env, target, source):
    
    rootenv = env.get('IMAGE_ENV')
    filename = rootenv.RealPath(str(target[0]))
    filepath = os.path.dirname(filename)
    
    # This is the location of the scripts dir for this image
    scriptsdir_var = get_image_name(rootenv) + "_SCRIPTSDIR"

    fptr = open(filename, 'w')
    indent = default_indent
    fptr.write("Header: CMM Image Builder\n")
    
    indent = default_indent
    
    for area in rootenv[MAINCMMDICT].keys():
    
        fptr.write("Area:"+ area + "\n")
        for singledropdownname in rootenv[MAINCMMDICT][area]:        
            this_script_struct = rootenv[MAINCMMDICT][area][singledropdownname]
            
            indent = macro_incr_indent(indent)
            fptr.write(indent + "dropdownname:" + singledropdownname + "\n")
            
            indent = macro_incr_indent(indent)

            fptr.write(indent + "scriptname->" +        this_script_struct['scriptname'] + "\n")
            fptr.write(indent + "dropdown menu name->"+ this_script_struct['dropdown menu name'] + "\n")
            fptr.write(indent + "arguments->" +         this_script_struct['arguments'] + "\n")
            fptr.write(indent + "abspath->" +           this_script_struct['abspath'] + "\n")
            fptr.write(indent + "absdirectory->" +      this_script_struct['absdirectory'] + "\n")
            fptr.write(indent + "extension->" +         this_script_struct['extension'] + "\n")
            fptr.write(indent + "mode->" +              this_script_struct['mode'] + "\n")

            indent = macro_decr_indent(indent)
            indent = macro_decr_indent(indent)
            
    
    fptr.write("\n")
    fptr.close()
    indent = default_indent
    
            
    #Copy file to known location
    reference_image_file=rootenv.RealPath(ComponentScriptListDirectory+rootenv['IMAGE_NAME']+'/cmmoutputlist.txt')
    cmm_scons_image_file = rootenv.InstallAs(reference_image_file,filename)
     
            
            
            

    
###########################################################
#   Name: cmm_image_builder_wrapper
#   Action: Wrapper Function which calls SCons Builder cmm_image_builder
#   Notes: Will be called early on in build time. Sets up dependencies for Scons builder to run late in build.
###########################################################
def cmm_image_builder_wrapper(*arg):
    env=arg[0]
    rootenv = env.get('IMAGE_ENV')
    
    #First call the cmm component builder in case there's any components in this environment that need building.
    cmm_component_file=rootenv.RealPath(ComponentScriptListDirectory+rootenv['IMAGE_NAME']+'_scriptlist.txt')
    
    component=env.CreateCMMComponentFile(cmm_component_file,[])
    
    
    
    
    #Write out outputs
    cmmscripts = [rootenv.RealPath('${BUILD_ROOT}/core/products/scripts/std_scripts.men'),
                rootenv.RealPath('${BUILD_ROOT}/core/products/scripts/std_extensions.cmm'),
                rootenv.RealPath('${BUILD_ROOT}/core/products/scripts/std_toolsconfig.cmm'),
                rootenv.RealPath('${BUILD_ROOT}/core/products/scripts/std_postmortem.cmm'),
                
                ]   

    
        
        
    #Should be done by Emitter
    imagesourcefiles=[]
    cmmimagesourcefiles=[]
    filepatterns=['*_cmmoutputlist.txt']
    
    
    for root, dirs, files in os.walk(rootenv.RealPath(ComponentScriptListDirectory)):
        for cmmimagefile in files:
            #if cmmimagefile in filepatterns:
                imagesourcefiles.append( rootenv.RealPath( os.path.join(root,cmmimagefile) ) )
    

    
    rootenv.Depends(cmmscripts,rootenv.Value(imagesourcefiles))
    rootenv.Depends(cmmscripts,component)
    
    cmmbuilderout = rootenv.CMMImageBuilder(cmmscripts, imagesourcefiles)


    return cmmbuilderout

###########################################################
#   Name: cmmbuilder_generate_files
#   Action: Emitter for top image's cmm builder: (cmm_image_builder)
#           Creates SCons dependency on the hash of the list of files generated by each image cmmbuilder.
#           If the list of files changes, it will cause the builder to be re-invoked.
#   Notes: None
###########################################################
def cmm_image_builder_emit(env, target, source):
    rootenv = env.get('IMAGE_ENV')
    
    imagesourcefiles=[]
    cmmimagesourcefiles=[]
    filepatterns=['*_cmmoutputlist.txt']
    
    for root, dirs, files in os.walk(rootenv.RealPath(ComponentScriptListDirectory)):
        for cmmimagefile in files:
                imagesourcefiles.append( rootenv.RealPath( os.path.join(root,cmmimagefile) ) )

    rootenv.Depends(target,rootenv.Value(imagesourcefiles))
    
    return (target, source)   
   
###########################################################
#   Name: cmm_image_builder
#   Action: Calls sub-methods to generate cmm scripts
#   Notes: This is the main scons builder action
###########################################################
def cmm_image_builder(env, target, source):
    
    rootenv = env.get('IMAGE_ENV')
    
    imagesourcefiles=[]
    cmmimagesourcefiles=[]
    
    for root, dirs, files in os.walk(rootenv.RealPath(ComponentScriptListDirectory)):
        for cmmimagefile in files:
                imagesourcefiles.append( rootenv.RealPath( os.path.join(root,cmmimagefile) ) )

    source=imagesourcefiles
    
    finalcmmdict=dict()
    for fileobject in source: #Go through the cmmscript files
        file=str(fileobject)
        lines=open(file).read().split('\n') #List of all the lines in the file
        
        for line in lines:
            lines[lines.index(line)]=line.rstrip()
            lines[lines.index(line)]=line.lstrip()
        
        
        currentArea='NULL'
        currentDropdownName='NULL'
        
        for line in lines:      #Parse out the lines into greater dictionary
            if re.search('^Header', line): continue
            if re.search('^Area', line ):
                
                #Create new dictionary entry for this area
                areaname=re.sub('^Area:','',line)
                areaname=areaname.rstrip()
                #Don't overwrite area with blank dictionary if it already exists
                if areaname not in finalcmmdict.keys():
                    finalcmmdict.update({areaname:dict()})
                
                #This is the current area we're iterating on
                currentArea=areaname
            elif re.search('^dropdownname', line ):
                if currentArea is 'NULL':
                    raise RuntimeError, "Error parsing cmm image file! currentArea value not populated."
                    
                dropdownlist=line.split(':')
                dropdownkey=dropdownlist[1]
                finalcmmdict[currentArea].update({dropdownkey:dict()})
                
                #Current dropdownname we're iterating on
                currentDropdownName=dropdownkey
            else:
                if currentDropdownName is 'NULL' or currentArea is 'NULL':
                    #If it's the 2nd line it's just an empty file
                    if lines.index(line) is 1:
                        break
                    else: #else there's a problem
                        raise RuntimeError, "Error parsing cmm image file! currentArea or currentDropDownName value(s) not populated."
                
                
                scriptdictentry=line.split('->')
                entrykey=scriptdictentry[0]
                if len(scriptdictentry) is 2:
                    entryname=scriptdictentry[1]
                else:
                    entryname=""
                
                
                finalcmmdict[currentArea][currentDropdownName].update({entrykey:entryname})
    
    a = cmm_menubuilder         (rootenv, str(target[0]), finalcmmdict)
    c = cmm_extensionbuilder    (rootenv, str(target[1]), finalcmmdict)
    b = cmm_configbuilder       (rootenv, str(target[2]), finalcmmdict)
    d = cmm_postmortembuilder   (rootenv, str(target[3]), finalcmmdict)
    




###########################################################
#   Name: cmm_menubuilder
#   Action: Private method to generate menu T32 script
#   Notes:
###########################################################
def cmm_menubuilder(env, target, source ):
    rootenv = env.get('IMAGE_ENV')
    ScriptDictionary=source
    ###################################################
    # Create a file in the core/products/scripts folder
    ###################################################
    filename = rootenv.RealPath(target)

    # This is the location of the scripts dir for this image
    scriptsdir_var = get_image_name(rootenv) + "_SCRIPTSDIR"

    # Check if the file exists and change to writable
    if not os.path.exists( rootenv['BUILD_ROOT']+'/core/products/scripts/' ):
                os.makedirs( rootenv['BUILD_ROOT']+'/core/products/scripts/' )
    if os.path.isfile(filename):
        fileprops = os.stat(filename)[0]
        if (not fileprops & stat.S_IWRITE):
            os.chmod(filename, fileprops | stat.S_IWRITE)
        
    fptr = open(filename, 'w')
    indent = default_indent
    fptr.write("ADD \n"+ "MENU \n"+ "( \n")
    fptr.write(indent + "POPUP \"[:CORRELATE]&CoreBSP Scripts\" \n")
    fptr.write(indent + "( \n")
    indent = macro_incr_indent(indent)
    
    # Now add the entries for every script
    rootenv.PrintInfo( ScriptDictionary )
    if ScriptDictionary:
        for area in ScriptDictionary.keys():
            indent = "\t"
            fptr.write(indent + "POPUP \"[:PSTEP]&" + area + "\" \n")
            fptr.write(indent + "( \n")
            
            for singledropdownname in ScriptDictionary[area]:
                    this_script_struct = ScriptDictionary[area][singledropdownname]
                    
                    indent = macro_incr_indent(indent)
                    extension = this_script_struct['extension']
                    displayname = this_script_struct['dropdown menu name']
                    arguments = this_script_struct['arguments']
                    scriptname = this_script_struct['scriptname']
                    relpath = rootenv.RelPath(this_script_struct['abspath'])
                    mode = this_script_struct['mode']
                    
                    #don't add .opt or .t32 extensions in this menu.
                    if extension in ['.opt'] or mode == "post-mortem" : continue
                    
                    fptr.write(indent + "MENUITEM \"[" + FILEPATTERNS_ICONS[extension] + "]&" + displayname + " " + "\" \n")
                    fptr.write(indent + "( \n")
                    indent = macro_incr_indent(indent)
                    fptr.write(indent + "CD &"+ scriptsdir_var + "\n")
                    fptr.write(indent + FILEPATTERNS_CMDS[extension] + " " + relpath + " "  + str(arguments) + "\n")
                    indent = macro_decr_indent(indent)
                    fptr.write(indent + ")" + "\n")
                    indent = macro_decr_indent(indent)
                    

            fptr.write(indent + ")" + "\n")

            
    
    # Now close the menu 
    indent = default_indent
    fptr.write(indent + ") \n")
    indent = macro_decr_indent(indent)
    fptr.write(")")
    
    fptr.write("\n")

    
    
    fptr.close()

    return target
                
            
    
###########################################################
#   Name: cmm_configbuilder
#   Action: Private method to generate std_toolsconfig.cmm T32 script
#   Notes:
###########################################################
def cmm_configbuilder(env, target, source):
    rootenv = env.get('IMAGE_ENV')    
    ScriptDictionary=source
    filename = rootenv.RealPath(target)
    
    scriptsdir_var = get_image_name(rootenv) + "_SCRIPTSDIR"
    
    # This is the location of the scripts dir for this image 
    scriptsdir_var = get_image_name(rootenv) + "_SCRIPTSDIR"

    # Check if the file exists and change to writable
    if os.path.isfile(filename):
        fileprops = os.stat(filename)[0]
        if (not fileprops & stat.S_IWRITE):
            os.chmod(filename, fileprops | stat.S_IWRITE)
        
    fptr = open(filename, 'w')
    fptr.write((rootenv.CreateFileBanner(os.path.basename(str(filename)), style="CMM")).replace('#','//'))
    fptr.write("\n")
    indent = ""
    
    #pp.pprint(ScriptDictionary)
    for area in ScriptDictionary.keys():
        
        t32optlines=[]
        t32osamlines=[]
        
        for singledropdownname in ScriptDictionary[area]:
                this_script_struct = ScriptDictionary[area][singledropdownname]
                
                extension = this_script_struct['extension']
                displayname = this_script_struct['dropdown menu name']
                arguments = this_script_struct['arguments']
                scriptname = this_script_struct['scriptname']
                reldirectory = rootenv.RelPath(this_script_struct['absdirectory'])
                relpath = rootenv.RelPath(this_script_struct['abspath'])
                
                if extension in OSAM_FILEPATTERNS:
                    if extension in ['.opt']:
                            t32optlines.append(str(indent + FILEPATTERNS_CMDS[extension] + " " + str(arguments) + " " + "\"&" + scriptsdir_var + "\\" + reldirectory + "\"" + "\n"))
                    else:
                        t32osamlines.append(str(indent + FILEPATTERNS_CMDS[extension] + " " + relpath + "\n"))

        
        #osam commands must come before .opt commands
        for t32osamline in t32osamlines:
            fptr.write(indent + "CD &"+ scriptsdir_var + "\n")
            fptr.write(str(t32osamline))
            fptr.write("\n")
            
        for t32optline in t32optlines:
            fptr.write(str(t32optline))
            fptr.write("\n")
                                


    # Now close the menu 
    indent = default_indent
    fptr.write("ENDDO")
    fptr.close()

    return target
        


###########################################################
#   Name: cmm_extensionbuilder
#   Action: Private method to generate std_extensions.cmm T32 script
#   Notes:
###########################################################
def cmm_extensionbuilder(env, target, source):
    rootenv = env.get('IMAGE_ENV')   
    # Create a configuration file
    filename = rootenv.RealPath(target)
    RELPATH = os.path.dirname(filename)
    newline = "\n"

    # Get the values 
    build_root = str(os.path.relpath(rootenv['BUILD_ROOT'], RELPATH)).replace('\\','/')
    build_root_core = str(os.path.relpath(rootenv['COREBSP_ROOT'], RELPATH)).replace('\\','/')
    image_name = str(rootenv.Dump('IMAGE_NAME')).replace('\'','')

    #Python variables for the CMM variables
    abs_build_root_var = get_image_name(rootenv) + "_ABS_BUILD_ROOT";
    build_root_var = get_image_name(rootenv) + "_BUILD_ROOT"
    build_root_core_var = get_image_name(rootenv) + "_CBSP_ROOT"
    scriptsdir_var = get_image_name(rootenv) + "_SCRIPTSDIR"
    buildmsdir_var = get_image_name(rootenv) + "_BUILDMSDIR"

    # Check if the file exists and change to writable
    if ( not os.path.exists(RELPATH) ):
                os.makedirs(RELPATH)
    if os.path.isfile(filename):
        fileprops = os.stat(filename)[0]
        if (not fileprops & stat.S_IWRITE):
            os.chmod(filename,fileprops | stat.S_IWRITE)
    fptr = open(filename, 'w+')
    fptr.write((rootenv.CreateFileBanner(os.path.basename(str(filename)), style="CMM")).replace('#','//'))
    fptr.write(newline + newline)

    fptr.write("GLOBAL &" + abs_build_root_var + newline)
    fptr.write("GLOBAL &" + build_root_var + newline)
    fptr.write("GLOBAL &" + build_root_core_var + newline)
    fptr.write("GLOBAL &" + scriptsdir_var + newline)
    fptr.write("GLOBAL &" + buildmsdir_var + newline)

    # Now assign the value
    # Now clean some space 
    fptr.write(newline * 3)
    fptr.write("&" + abs_build_root_var + "=\"" + str(rootenv['BUILD_ROOT']).replace('\'','') + "\"" + newline)
    fptr.write("&" + build_root_var + "=os.ppd()" + "+\"/" +build_root + "\"" + newline)
    fptr.write("&" + build_root_core_var + "=os.ppd()" + "+\"/" + build_root_core + "\"" + newline)
    fptr.write("&" + scriptsdir_var + "=os.ppd()" + "+\"/" + build_root_core + "/products\"" + newline)
    fptr.write("&" + buildmsdir_var + "=os.ppd()" + "+\"/" + build_root + "/" + image_name.lower() + "/build/ms\"" + newline)
    fptr.write(newline * 2)
    fptr.write("ENDDO")
    fptr.close()

    return target


###########################################################
#   Name: cmm_postmortembuilder
#   Action: 
#   Notes:
###########################################################
def cmm_postmortembuilder(env, target, source):
    rootenv = env.get('IMAGE_ENV')    
    ScriptDictionary=source
    filename = rootenv.RealPath(target)
    
    scriptsdir_var = get_image_name(rootenv) + "_SCRIPTSDIR"
    
    # This is the location of the scripts dir for this image 
    scriptsdir_var = get_image_name(rootenv) + "_SCRIPTSDIR"

    # Check if the file exists and change to writable
    if os.path.isfile(filename):
        fileprops = os.stat(filename)[0]
        if (not fileprops & stat.S_IWRITE):
            os.chmod(filename, fileprops | stat.S_IWRITE)
        
    fptr = open(filename, 'w')
    fptr.write((rootenv.CreateFileBanner(os.path.basename(str(filename)), style="CMM")).replace('#','//'))
    fptr.write("\n")
    indent = ""
    
    fptr.write('&DEFAULTLOGDIR=os.env(temp)' + '\n')
    fptr.write( '&DEFAULTLOGDIR="&DEFAULTLOGDIR\std_loadsim_logs\"' + '\n')
    fptr.write( '\n' )

    scriptnumber = 1
    
    for area in ScriptDictionary.keys():
        
        t32cmmlines=[]
        t32cmmnames=[]
        for singledropdownname in ScriptDictionary[area]:
                this_script_struct = ScriptDictionary[area][singledropdownname]
                
                extension = this_script_struct['extension']
                displayname = this_script_struct['dropdown menu name']
                arguments = this_script_struct['arguments']
                
                scriptname = this_script_struct['scriptname']
                reldirectory = rootenv.RelPath(this_script_struct['absdirectory'])
                relpath = rootenv.RelPath(this_script_struct['abspath'])
                mode = this_script_struct['mode']
                
                CMM_FILEPATTERNS = ['.cmm']
                if extension in CMM_FILEPATTERNS:
                    if extension in ['.cmm']:
                        if mode == "post-mortem":
                            t32cmmlines.append(str(indent + "do std_utils EXECUTESCRIPT EXIT" + " &" + scriptsdir_var + "\\" + relpath+  " " + str(arguments) +  "\n"))
                            t32cmmnames.append(str(scriptname))
                    

            
        for t32cmmline in t32cmmlines:
            
        
            fptr.write("ON ERROR GOTO error_" + str(scriptnumber) + '\n')
            fptr.write( '\t' + str(t32cmmline))
            fptr.write( '\t' + "GOTO success_" + str (scriptnumber) + '\n')
            fptr.write( "error_" + str (scriptnumber) + ":" + '\n' )
            fptr.write( '\t' + "print " +  "\"" + "failed " + t32cmmnames[t32cmmlines.index(t32cmmline)] + "\"" + '\n')
            fptr.write( "success_" + str(scriptnumber) + ":" + '\n' )
            fptr.write( '\t' + "print " +  "\"" + "success " + t32cmmnames[t32cmmlines.index(t32cmmline)]  + "\"" + '\n')
            fptr.write("\n")
            
            scriptnumber += 1
                                


    fptr.write('print "Logs saved to &DEFAULTLOGDIR"')
    fptr.write( '\n' )
    # Now close the menu 
    indent = default_indent
    fptr.write("ENDDO")
    fptr.close()

    return target
    
def cmmbuilder_debug_output(env, type, msg):
    rootenv = env.get('IMAGE_ENV')   
    # Create a file in the core/products/build folder
    filename = rootenv['BUILD_ROOT']+'/core/products/build/'+debug_filename
    # Check if the file exists and change to writable
    if os.path.isfile(filename):
        fileprops = os.stat(filename)[0]
        if (not fileprops & stat.S_IWRITE):
            os.chmod(filename, fileprops | stat.S_IWRITE)
            
    fptr = open(filename, 'w+')
    
    # Depending on the type, you print the message accordingly. 
    if type == "string":
        fptr.write(msg)
    elif type == "list":
        fptr.write("List Debug:")
        for entry in msg:
            fptr.write(entry);
        fptr.write('\n \n')
    elif type == "table":
        fptr.write("Dictionary Debug:")
        for key,value in msg.iteritems():
            fptr.write(str(key)+':'+str(value))
            fptr.write('\n \n')
    else:
        cmmbuilder_error(rootenv, "Unrecognized debug message type")
        
    
    
    
#################################
###Other various private methods
#################################
def get_image_name(env):
    rootenv = env.get('IMAGE_ENV')   
    try:
        rootenv.PrintInfo( "Current build tag : " + str(rootenv[CMMBUILDER_TAG]) )
        return rootenv[CMMBUILDER_TAG]
    except KeyError:
       
       
       imagename=rootenv['IMAGE_NAME']
       
       keys =  valid_images.keys()
       
       for key in keys:
            current_proc=valid_images[key]+'_PROC'
            if rootenv.IsTargetEnable( current_proc ):
                rootenv[CMMBUILDER_TAG]=current_proc
                return rootenv[CMMBUILDER_TAG]
                
       
       
       
       
       
       raise RuntimeError, "cmmbuilder - CMMBUILDER_TAG is null! Unknown processor used"
       return ''


#valid_images = {'core_rpm': 'RPM', 'core_adsp':'ADSP','core_slpi':'SLPI', 'core_modem':'MPSS','core_wcn':'WCNSS','core_apps':'APPS'}

def set_image_name(env, image, area):
    rootenv = env.get('IMAGE_ENV')   
    

    
    if image is False:
        raise RuntimeError, "Image entry cannot be empty. Error caused from area:" + str(area)
        return
    else:
        cmm_keys=valid_images.keys()
        for cmmkey in cmm_keys:
            if image == valid_images[cmmkey]:
                #Check for SLPI case
                if (image=="ADSP") and (rootenv.IsTargetEnable("SLPI_PROC")):
                    image="SLPI"
                rootenv[CMMBUILDER_TAG]=image

            
    if rootenv[CMMBUILDER_TAG] == '':
        #cmmbuilder_error(rootenv, "CMM Builder tag null.")
        rootenv.PrintError("Unknown Image Specified: " + str(image_name) + ", In Area: " + str(area))
        #raise RuntimeError, "Unknown Image Specified: " + str(image_name) + ", In Area: " + str(area)
    
    return
    
    
def cmmbuilder_error(env, msg):
    env.PrintError(msg)
    
def macro_incr_indent(string):
    return (string +"\t")
    
def macro_decr_indent(string):
    return string[:-1]
    
    
def cmm_dummy(env,target,source):
    return 
