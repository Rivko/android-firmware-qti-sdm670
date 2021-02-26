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
#  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/bsp/build/scripts/cmm_builder.py#1 $
#  $DateTime: 2018/02/16 04:48:18 $
#  $Author: pwbldsvc $
#  $Change: 15485400 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
#==============================================================================

import os
import subprocess 
import string
import re
import glob
import fnmatch
import stat

# Define any globals out here
# Valid images on which the builder will be used
valid_images = {'aop_proc': 'AOP', 'aop' : 'AOP', 'adsp_proc':'ADSP', 'mba':'MPSS','modem_proc':'MPSS', 'wcnss_proc':'WCNSS','core':'MPSS','apps_proc':'APPS'}
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
CMMSCRIPTSCNT="ScriptsCount"
FILEPATTERNS=['*.cmm','*.per','*.men','*.t32']
EXTENSION_FILEPATTERNS=['.men','.t32']
# T32 icons to be used for the different extensions
FILEPATTERNS_ICONS={'.cmm':':FLASH','.per':':CHIP', '.men':':ACONFIG','.t32':':ACONFIG'}
FILEPATTERNS_CMDS={'.cmm':'cd.do','.per':'per.view', '.men':'menu.reprogram', '.t32':'task.config'}
RELPATH=''


# Scons functions
def exists(env):
	return True

def generate(env):
	# Add methods to the scons environment
	mainenv = env.get('IMAGE_ENV')
	mainenv.AddMethod(add_cmm_script, "AddCMMScripts")
	
	# Add the builder into the environment
	cmmbuilder_act = env.GetBuilderAction(cmm_builder)
	cmmbuilder_scanref = env.Scanner(cmmbuilder_scan, "cmmbuilder_scanner")
	cmmbuilder = env.Builder(action = cmmbuilder_act, target_scanner=cmmbuilder_scanref, suffix='.men', src_suffix='.something')
	
	mainenv.Append(BUILDERS = {'CMMBuilder': cmm_builder})
	# Now add the global structures to the main env
	mainenv[CMMBUILDER_LIST]=[]
	mainenv[CMMSCRIPTSCNT]=0
	
def cmmbuilder_debug_output(env, type, msg):
	# Create a file in the core/products/build folder
	filename = env['BUILD_ROOT']+'/core/products/build/'+debug_filename
	# Check if the file exists and change to writable
	if os.path.isfile(filename):
		fileprops = os.stat(filename)[0]
		if (not fileprops & stat.S_IWRITE):
			os.chmod(filename, stat.S_IWRITE)
			
	fptr = open(filename, 'w')
	
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
		cmmbuilder_error(env, "Unrecognized debug message type")
		
	
def cmmbuilder_error(env, msg):
	env.PrintError(msg)

def cmmbuilder_scan(node, env, path):
	AlwaysBuild(node)
	return node
	
def add_cmm_script(env, image, sources, scriptdict, area):
	mainenv = env.get('IMAGE_ENV')
	# Easier to just assign the value this way 
	RELPATH=env['BUILD_ROOT']+'/core/products'
	
	# Create a new dictionary
	dict = {}
	dict[IMAGE]=[]
	dict[AREA]=[]
	dict[CMMSCRIPTS]=[]
	dict[ARGS]=[]
	
	# Check the image argument and add it to the dictionary
	if image is False:
		cmmbuilder_error(env, "Image entry cannot be empty")
		return
	elif image.upper() not in valid_images[str(env['IMAGE_NAME']).lower()]:
		cmmbuilder_error(env, "Image: "+ image + " is not valid")
		print "Error: CMM Scripts from " + str(sources) + " not added. \n"
		return
	else:
		dict[IMAGE] = image.upper()
	
	# Check the area argument and add it to the dictionary
	if area is False:
		cmmbuilder_error(env, "Area cannot be empty")
		return
	elif area.lower() not in valid_areas:
		cmmbuilder_error(env, "Area: "+ area + " is not valid.")
		return
	else:
		dict[AREA] = area[0].upper() + area[1:].lower()
	
	
	# Now check the cmm scripts provided in the locations
	cmmscripts = {}

	for source in sources:
		for root, dirs, files in os.walk(source):
			for pattern in FILEPATTERNS:
				for file in fnmatch.filter(files, pattern):
					script = os.path.relpath(os.path.join(root, file), RELPATH).replace('\\','/')
					if ((script not in cmmscripts.keys()) and (file in scriptdict.keys())):
						cmmscripts[script] = scriptdict[file] 
	
	
	# cmmscripts should have the list of all the scripts
	dict[CMMSCRIPTS] = cmmscripts
	
	# Now add this to the main map only if the area does not exist
	for i in range(len(mainenv[CMMBUILDER_LIST])):
		if (mainenv[CMMBUILDER_LIST][i][AREA] == (area[0].upper() + area[1:].lower())):
			for scriptname, displayname in cmmscripts.iteritems():
				mainenv[CMMBUILDER_LIST][i][CMMSCRIPTS][scriptname] = cmmscripts[scriptname]
			return
	
	#It will reach this section only if the area has not been used before. 
	# This is a new area. Create an entry for this
	mainenv[CMMBUILDER_LIST].append(dict)
	# Increment the count
	mainenv[CMMSCRIPTSCNT]+=1
	
def cmm_builder(env, source, target):
	# This is a temporary test to see if the scripts are being added correctly.
	mainenv = env.get('IMAGE_ENV')
	
	for i in range(len(mainenv[CMMBUILDER_LIST])):
		cmmbuilder_debug_output(env, 'table' , mainenv[CMMBUILDER_LIST][i])
	
	cmm_menubuilder(env)
	cmm_configbuilder(env)
	cmm_extensionbuilder(env)
	
def macro_incr_indent(string):
	return (string +"\t")
	
def macro_decr_indent(string):
	return string[:-1]

def get_display_name(script):
	properties = script[1]
	if (len(properties[0]) > 1):
		return properties[0]
	else:
		return properties

def cmm_menubuilder(env):
	# Create a file in the core/products/scripts folder
	filename = env['BUILD_ROOT']+'/core/products/scripts/'+t32menu_filename
	# This is the location of the scripts dir for this image 
	scriptsdir_var = valid_images[str(env['IMAGE_NAME']).lower()] + "_SCRIPTSDIR"
	
	mainenv = env.get('IMAGE_ENV')
	
	# Check if the file exists and change to writable
        if not os.path.exists( env['BUILD_ROOT']+'/core/products/scripts/' ):
                os.makedirs( env['BUILD_ROOT']+'/core/products/scripts/' )
	if os.path.isfile(filename):
		fileprops = os.stat(filename)[0]
		if (not fileprops & stat.S_IWRITE):
			os.chmod(filename, stat.S_IWRITE)
		
	fptr = open(filename, 'w')
	indent = default_indent
	fptr.write("ADD \n"+ "MENU \n"+ "( \n")
	fptr.write(indent + "POPUP \"[:CORRELATE]&CoreBSP Scripts\" \n")
	fptr.write(indent + "( \n")
	indent = macro_incr_indent(indent)
	# Now add the entries for every script
	if mainenv[CMMBUILDER_LIST]:
		for i in range(len(mainenv[CMMBUILDER_LIST])):
			area = mainenv[CMMBUILDER_LIST][i][AREA]
			indent = "\t"
			fptr.write(indent + "POPUP \"[:PSTEP]&" + area + "\" \n")
			fptr.write(indent + "( \n")
			scripts = mainenv[CMMBUILDER_LIST][i][CMMSCRIPTS]
			
			scripts_sorted = sorted(scripts.iteritems(), key=get_display_name)
			for scriptname, properties in scripts_sorted:
				# Properties has the following format
				# [0] - Pretty Name of the script
				# [1:] - Arguments to be passed to the script
				# People might have zero arguments to pass along in which case,
				# we identify that by checking the length
				if (len(properties[0]) > 1):
					# We have args
					displayname = properties[0]
					args = properties[1:]
				else:
					# We don't have any args
					displayname = properties
					args = []
				indent = macro_incr_indent(indent)
				(title, ext) = os.path.splitext(os.path.basename(scriptname))
				fptr.write(indent + "MENUITEM \"[" + FILEPATTERNS_ICONS[ext] + "]&" + displayname[0].upper() + displayname[1:] + "\" \n")
				fptr.write(indent + "( \n")
				indent = macro_incr_indent(indent)
				fptr.write(indent + "CD &"+ scriptsdir_var + "\n")
				fptr.write(indent + FILEPATTERNS_CMDS[ext] + " " + scriptname + " "  + " ".join(args) + "\n")
				indent = macro_decr_indent(indent)
				fptr.write(indent + ")" + "\n")
				indent = macro_decr_indent(indent)
			
			fptr.write(indent + ")" + "\n")
	
	# Now close the menu 
	indent = default_indent
	fptr.write(indent + ") \n")
	indent = macro_decr_indent(indent)
	fptr.write(")")

def cmm_extensionbuilder(env):
	# Create a file in the core/products/scripts folder
	filename = env['BUILD_ROOT']+'/core/products/scripts/'+t32extn_filename
	# This is the location of the scripts dir for this image 
	scriptsdir_var = valid_images[str(env['IMAGE_NAME']).lower()] + "_SCRIPTSDIR"
	
	# Get the main env handle
	mainenv = env.get('IMAGE_ENV')
	
	# Check if the file exists and change to writable
	if os.path.isfile(filename):
		fileprops = os.stat(filename)[0]
		if (not fileprops & stat.S_IWRITE):
			os.chmod(filename, stat.S_IWRITE)
		
	fptr = open(filename, 'w')
	fptr.write((env.CreateFileBanner(os.path.basename(str(filename)), style="CMM")).replace('#','//'))
	fptr.write("\n")
	indent = ""
	# Now add the entries for every script
	if mainenv[CMMBUILDER_LIST]:
		for i in range(len(mainenv[CMMBUILDER_LIST])):
			scripts = mainenv[CMMBUILDER_LIST][i][CMMSCRIPTS]
			for scriptname, displayname in scripts.iteritems():
				(title, ext) = os.path.splitext(os.path.basename(scriptname))
				# We are going to create extensions only for .t32 and .men files
				if ext in EXTENSION_FILEPATTERNS:
					fptr.write(indent + "CD &"+ scriptsdir_var + "\n")
					fptr.write(indent + FILEPATTERNS_CMDS[ext] + " " + scriptname + "\n")
					fptr.write("\n")
			
	# Now close the menu 
	indent = default_indent
	fptr.write("ENDDO")


def cmm_configbuilder(env):
	# Create a configuration file
	filename = env['BUILD_ROOT']+'/core/products/scripts/'+t32config_filename
	RELPATH = env['BUILD_ROOT']+'/core/products/scripts'
	newline = "\n"
	
	# Get the values 
	build_root = str(os.path.relpath(env['BUILD_ROOT'], RELPATH)).replace('\\','/')
	build_root_core = str(os.path.relpath(env['COREBSP_ROOT'], RELPATH)).replace('\\','/')
	image_name = str(env.Dump('IMAGE_NAME')).replace('\'','')
	
	#Python variables for the CMM variables
	abs_build_root_var = valid_images[str(env['IMAGE_NAME']).lower()] + "_ABS_BUILD_ROOT";
	build_root_var = valid_images[str(env['IMAGE_NAME']).lower()] + "_BUILD_ROOT"
	build_root_core_var = valid_images[str(env['IMAGE_NAME']).lower()] + "_CBSP_ROOT"
	scriptsdir_var = valid_images[str(env['IMAGE_NAME']).lower()] + "_SCRIPTSDIR"
	buildmsdir_var = valid_images[str(env['IMAGE_NAME']).lower()] + "_BUILDMSDIR"

	
	# Check if the file exists and change to writable
        if ( not os.path.exists(RELPATH) ):
                os.makedirs(RELPATH)
	if os.path.isfile(filename):
		fileprops = os.stat(filename)[0]
		if (not fileprops & stat.S_IWRITE):
			os.chmod(filename, stat.S_IWRITE)
	fptr = open(filename, 'w')
	fptr.write((env.CreateFileBanner(os.path.basename(str(filename)), style="CMM")).replace('#','//'))
	fptr.write(newline + newline)

	fptr.write("GLOBAL &" + abs_build_root_var + newline)
	fptr.write("GLOBAL &" + build_root_var + newline)
	fptr.write("GLOBAL &" + build_root_core_var + newline)
	fptr.write("GLOBAL &" + scriptsdir_var + newline)
	fptr.write("GLOBAL &" + buildmsdir_var + newline)
	
	# Now assign the value
	# Now clean some space 
	fptr.write(newline * 3)
	fptr.write("&" + abs_build_root_var + "=\"" + str(env['BUILD_ROOT']).replace('\'','') + "\"" + newline)
	fptr.write("&" + build_root_var + "=os.ppd()" + "+\"/" +build_root + "\"" + newline)
	fptr.write("&" + build_root_core_var + "=os.ppd()" + "+\"/" + build_root_core + "\"" + newline)
	fptr.write("&" + scriptsdir_var + "=os.ppd()" + "+\"/" + build_root_core + "/products\"" + newline)
	fptr.write("&" + buildmsdir_var + "=os.ppd()" + "+\"/" + build_root + "/" + image_name.lower() + "/build/ms\"" + newline)
	fptr.write(newline * 2)
	fptr.write("ENDDO")
	
	
	
	
	
	
	
	
	
