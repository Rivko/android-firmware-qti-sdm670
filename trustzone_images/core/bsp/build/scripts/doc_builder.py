#===============================================================================
#
# Document file builders
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 2010-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.tz/2.0.2/bsp/build/scripts/doc_builder.py#1 $
#  $DateTime: 2018/02/06 02:57:17 $
#  $Author: pwbldsvc $
#  $Change: 15399902 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 01-19-12   MK      Added MikTeX detection to force user to add it to the path
#                    Replaced RelPath implementation with env.RelPath
# 10-11-10   MK      Added support for 1. modem; 2. Doxygen 1.7.0 & 1.5.9 
#                    coexistence; 3. LDD override; 4. Latex Cleaner from TP;
#                    5. Local executables for Doxygen and MikTex(Windows only)
# 07-08-10   MK      Fixed read-only attribute being set to files in workarea
#                    Added support for MM Docs
# 05-19-10   MK      Fixed Doxygen "File not found" warnings and added 
#                    IMAGE_PATH support
# 04-05-10   MK      First Cut
#===============================================================================

import SCons.Action
import os
import glob
import subprocess
import string
import re
import shutil
from distutils.dir_util import copy_tree
from distutils.file_util import copy_file
from distutils.file_util import move_file

#------------------------------------------------------------------------------
# Some constants
#------------------------------------------------------------------------------
DEFAULT_DOC_REVISION="80-VXXXX-XX Rev X"
DEFAULT_VERSION="170"
template_dir = "${BUILD_ROOT}/core/bsp/docs/template"
doxygen_exe_paths = {'159':"${BUILD_ROOT}/core/bsp/tools/doxygen1.5.9/doxygen",
                     '170':"${BUILD_ROOT}/core/bsp/tools/doxygen1.7.0/doxygen"}
tempdir_suffix = ""
doxyfilename   = "Doxy_Config.ldd"        # Doxygen Configuration File
doxygen_op_dir = "doxygen_output"         # Doxygen Output Folder
latex_dir="latex"                         # latex folder created by doxygen  
core_doc_repo = "${BUILD_ROOT}/core/api/doc"   # Final Document Repository for core
mm_doc_repo = "${BUILD_ROOT}/multimedia/api/doc"   # Final Document Repository for MM
modem_doc_repo = "${BUILD_ROOT}/modem/api/doc"   # Final Document Repository for Modem
mandatory_chapters = {"introduction.tex":"",
                      "overview.tex":"",
                      "configuration.tex":"",
                      "callflows.tex":"",
                      "porting_guide.tex":""
                      }


#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   #check for the right miktex path
   if not env.has_key('CORE_DOCS') and not env.has_key('MM_DOCS') and not env.has_key('MODEM_DOCS'):
      return
   path = env.WhereIs('pdflatex')
   if path is None:
       raise_error(env, "PDFLatex was not detected. Please install MikTex 2.8 and add the "\
           "path to the environment");
   if "cygwin" in path.lower():
       raise_error(env, "Cygwin version detected. Please install MikTex 2.8 and add the "\
           "path to the environment");
   elif "miktex" not in path.lower():
       raise_error(env, "Non Miktex version detected. Please install MikTex 2.8 and add the "\
           "path to the environment");
   return env.Detect(['doc_builder','pdf','doxygen','pdflatex'])

def generate(env):
   doc_generate(env)

def raise_error (env, msg):
   env.PrintError(msg)
   raise RuntimeError, msg

#===============================================================================
# Document builder rules
#  - This is the main entry point.
#===============================================================================
def doc_builder(env, target, docsrcfiles):

   if not env.has_key('CORE_DOCS') and not env.has_key('MM_DOCS') and not env.has_key('MODEM_DOCS'):
      return

   doxyfile_abspath = target + tempdir_suffix + "/" + doxyfilename
   latex_abspath    = target + tempdir_suffix + "/" + doxygen_op_dir +  "/" + latex_dir +  "/" 

   filename = os.path.basename(target)
   ext = os.path.splitext(target)[1]
   if (env.has_key('CORE_DOCS')):
       doc_abspath = core_doc_repo + "/"+ filename
   if (env.has_key('MM_DOCS')):
       doc_abspath = mm_doc_repo + "/"+ filename
   if (env.has_key('MODEM_DOCS')):
       doc_abspath = modem_doc_repo + "/"+ filename


   if ext != "pdf" :
       doc_abspath = doc_abspath + ".pdf"

   # traceback.print_stack()
   if type(docsrcfiles) is not list:
      docsrcfiles=[docsrcfiles]
   
   headerfiles = []
   texfiles = []
   ignoreheaderfiles = False;

   for docsrcfile in docsrcfiles:
       #import pdb; pdb.set_trace()
       #print "DocsrcFile = "+env.subst(docsrcfile)
       if os.path.splitext(docsrcfile)[1] == ".h" and ignoreheaderfiles == False:
           headerfiles.append(docsrcfile)
       elif os.path.splitext(docsrcfile)[1] == ".dox" and ignoreheaderfiles == False:
               headerfiles.append(docsrcfile)
       elif os.path.splitext(docsrcfile)[1] == ".ldd":
           headerfiles=[]
           headerfiles.append(docsrcfile)
           ignoreheaderfiles=True;
       elif os.path.splitext(docsrcfile)[1] == ".ldd":
           headerfiles=[]
           headerfiles.append(docsrcfile)
           ignoreheaderfiles=True;
       elif os.path.isdir(env.subst(docsrcfile)):
           texfiles.append(env.Dir(docsrcfile))
       else:
           texfiles.append(docsrcfile)

   target_doxyfile = env.DoxyfileBuilder(doxyfile_abspath,headerfiles)   
   #Always build is needed since doxygen on error wont clean its output
   env.AlwaysBuild(target_doxyfile) 
   # We will choose a known file generated by doxygen as a targets for the 
   # following build step
   target_dox = env.DoxygenBuilder(latex_abspath + "refman.tex", target_doxyfile)
   env.AlwaysBuild(target_dox)
   if texfiles:
       textargetname = os.path.basename(str(texfiles[0]))
   
   # Workaround to have unique targets for scons - pass in a dummy (but real) target
   target_tex = env.PDFTemplateBuilder(latex_abspath + "docmain.tex",target_dox+texfiles)
   env.AlwaysBuild(target_tex)
   # Generate the PDF
   target_pdf = env.PDF(latex_abspath + "refman.pdf",target_tex)
   env.AlwaysBuild(target_pdf)
   target_installdoc = env.InstallAs(doc_abspath,target_pdf)
   env.AlwaysBuild(target_installdoc)
   build_units = [
      target_doxyfile, target_dox, target_tex, target_pdf, target_installdoc
      ]
   
   return build_units

   
def doc_generate(env):
   env.AddMethod(doc_builder, "PdfDocument")
   
   #-------------------------------------------------------------------------------
   # Doxyfile Builder 
   #
   doxyfile_act = SCons.Action.Action(doxyfile_builder,'=== Generating ${DOCTITLE}/$TARGET.name') 
   doxyfile_bld = env.Builder(action = doxyfile_act, emitter = doxyfile_emitter)

   env.Append(BUILDERS = {'DoxyfileBuilder' : doxyfile_bld})

   #-------------------------------------------------------------------------------
   # Doxygen Builder 
   #
   doxygen_act =  SCons.Action.Action(doxygen_builder,'=== Generating ${DOCTITLE}/$TARGET.name')
   doxygen_bld = env.Builder(action = doxygen_act)
   env.Append(BUILDERS = {'DoxygenBuilder' : doxygen_bld})

   #-------------------------------------------------------------------------------
   # PDFTemplate Builder 
   #
   pdftemplate_act = SCons.Action.Action(pdftemplate_builder,'=== Generating ${DOCTITLE}/$TARGET.name')
   pdftemplate_bld = env.Builder(action = pdftemplate_act)
   env.Append(BUILDERS = {'PDFTemplateBuilder' : pdftemplate_bld})


   #-------------------------------------------------------------------------------
   # Overriding default configuration for PDFLaTeX 
   #
   env['PDFLATEXCOMSTR'] = '=== Generating ${DOCTITLE}/$TARGET.name'
   # Need to override the default value for EPSTOPDFCOM until we move to a 
   # later Scons rev (1.3.0 at least).
   # This is needed because epstopdf that comes with MikTeX doesnt support -o
   env['EPSTOPDFCOM']   = '$EPSTOPDF $EPSTOPDFFLAGS ${SOURCE} --outfile=${TARGET}'
   env.Append(PDFLATEXFLAGS = ' -quiet -max-print-line=200 -c-style-errors ') 
   env.Append(MAKEINDEXFLAGS = ' -q ')

#-------------------------------------------------------------------------------
# Emitter builder for DoxyfileBuilder 
#  -- Added the emitter to doxyfile, since we will clean the entire directory 
# itself when clean is issued. Rest of the builders generate data only within
# this target so no separate emitters are needed
#
def doxyfile_emitter(target, source, env):
   # here one can also add dependencies for say aexe.exe (if it needs to be build)
   # env.Depends(target, propgen_tool)

   #We get the doxyfile..now process it to get the targets
   # thedoc_tmp/doxygen_output/latex
   tgtdir = os.path.dirname(str(target[0]))
   dox_output_dir = env.Dir(tgtdir+"/doxygen_output")
   latex_output_dir = env.Dir(tgtdir+"/doxygen_output/latex")
   env.Clean(target,dox_output_dir)
   env.Clean(target,latex_output_dir)
   env.Clean(target,tgtdir)
   
   return (target, source)


#-------------------------------------------------------------------------------
# def builder for DoxyfileBuilder
#  --> Takes in the thedoc_tmp\doxyfile and the c source files
#
def doxyfile_builder(target, source, env):

   template_doxyfile = os.path.abspath(env.subst(template_dir+"/"+doxyfilename.replace(".","_"+env.get("DOXYGEN_VERSION",DEFAULT_VERSION)+".")))
   template_header = os.path.abspath(env.subst(template_dir+"/"+"header.tex"))

   doxyfile = str(target[0])
   doxyfile_dir = os.path.dirname(doxyfile)

   '''
   Check if we are given an ldd file. If so, everything in the doxygen config
   is overridden
   '''
   if os.path.splitext(str(source[0]))[1] == ".ldd":
       copy_file(str(source[0]), doxyfile_dir+"/"+doxyfilename,preserve_mode=0)
       return

   ''' 
   Uses the standard template
   '''

   '''
   First prepare the doxyfile. It is OK to overwrite everytime. Much better than to replace.
    1. Copy the doxyfile from the template
    2. Prepare the document names with this...
        env.DOCTITLE
        env.REVISION
   '''
   # Check if doxyfile is already present, if so delete it.
   if os.path.exists(doxyfile):
       os.remove(doxyfile)

   # Check if doxyfile is  present in the template
   if os.path.exists(template_doxyfile):
       #Replace the markers with the values we got and copy it to the temp location
       f = open(template_doxyfile, "r")
       template_doxycfg = f.read()
       f.close()
       f = open(doxyfile, "w")
       template_doxycfg = template_doxycfg.replace("@NAME",env['DOCTITLE'])
       template_doxycfg = template_doxycfg.replace("@REVISION",env.get('PUBLIC_API_DOC_DCN',env.get('DOCREVISION',DEFAULT_DOC_REVISION)))

       if env.has_key('DOCIMAGEPATH'):
           template_doxycfg = template_doxycfg.replace("@IMAGE_PATH",env['DOCIMAGEPATH'])
       else:
           template_doxycfg = template_doxycfg.replace("@IMAGE_PATH",'')


       #Replace absolute paths with relative paths -- Doxygen hates absolute paths.
       sourcepathlist = " "

       for sourcepath in source:
          relativepath = env.RelPath(str(env.subst(sourcepath)),doxyfile_dir)
          # Doxygen doesnt like a mix of unix and dos separators
          # So force everything to be unix separators
          relativepath = relativepath.replace(os.sep,"/")
          sourcepathlist = sourcepathlist +" \ \n" + relativepath
       
       template_doxycfg = template_doxycfg.replace("@INPUT", sourcepathlist) #" ".join(map( str, source)))
       template_doxycfg = template_doxycfg.replace("@OUTPUT",doxygen_op_dir)
       f.write(template_doxycfg)
       f.close()
   else:
       raise_error(env,"Template Doxyfile is not found : "+template_doxyfile)

   #Copy the template header file and we are good to go for now
   copy_file(template_header, doxyfile_dir,preserve_mode=0)
   return None

#-------------------------------------------------------------------------------
# def builder for DoxygenBuilder
#   Runs Doxygen tool and generates latex output from the header file doxytags
#      ---> takes only one source -- a doxyfile
#
def doxygen_builder(target, source, env):
    
    doxygen_exe_path= doxygen_exe_paths.get(env.get("DOXYGEN_VERSION",DEFAULT_VERSION),doxygen_exe_paths[DEFAULT_VERSION])+"${EXE_EXT}"
    cmd = "cd "+os.path.dirname(str(source[0]))+" && "+doxygen_exe_path+" "+doxyfilename 
    data, err, rv = env.ExecCmds(cmd, silent=1)
    return None



#-------------------------------------------------------------------------------
# def builder for PDFTemplateBuilder
#   Copyies templates and documentation sources to the doxygen generated latex
# folder
#
def pdftemplate_builder(target, source, env):

    latex_abs_dir = os.path.dirname(str(target[0]))
    # Clean the Doxygen generated Latex Files first

    LaTexCleaner(latex_abs_dir)

    # Copy the template files next
    copy_tree(env.subst(template_dir),latex_abs_dir,preserve_mode=0)
    f = open(latex_abs_dir+"/"+"project_info.tex", "r")
    project_info_data = f.read()
    f.close()  
    f = open(latex_abs_dir+"/"+"project_info.tex", "w")
    project_info_data = project_info_data.replace("@NAME",env['DOCTITLE'])        
    project_info_data = project_info_data.replace("@REVISION",env.get('PUBLIC_API_DOC_DCN',env.get('DOCREVISION',DEFAULT_DOC_REVISION)))
    project_info_data = project_info_data.replace("@TYPE","Interface Specification") 
    f.write(project_info_data)
    f.close()  

    # Now have to copy the appropriate sty file to the target dir
    copy_file(latex_abs_dir + "/doxygen_"+env.get("DOXYGEN_VERSION",DEFAULT_VERSION) +".sty", latex_abs_dir + "/doxygen.sty")

    # Prepare the docmain tex with input tex files while the input files
    # are moved to the workarea
    f = open(latex_abs_dir + "/refman.tex", "r")
    refman_tex_data = f.read()
    f.close()  
    f = open(str(target[0]), "w")


    # Copy the user supplied files next
    # Ignore the first source --> That is used to enforce dependency
    source_include_string = "";
    for sourcepath in source[1:]:
       if os.path.isfile(str(sourcepath)):
          copy_file(str(sourcepath),latex_abs_dir,preserve_mode=0)
       if os.path.isdir(str(sourcepath)):
          # Get the directory name to create
          dirname = os.path.basename(str(sourcepath))
          #print "Sourcepath  = "+str(sourcepath) +" ===> "+ latex_abs_dir+os.sep+dirname
          copy_tree(str(sourcepath),latex_abs_dir+os.sep+dirname,preserve_mode=0 )

       ext = os.path.splitext(str(sourcepath))[1]
       # print "Extension found = "+ext
       if ext  == ".tex":
           texfilename = os.path.basename(str(sourcepath))
           if mandatory_chapters.has_key(texfilename):
               if texfilename != "introduction.tex" and texfilename != "overview.tex":
                   refman_tex_data = refman_tex_data.replace("%\\input{"+texfilename+"}","\\input{"+texfilename+"}")        
           else:
               source_include_string += "\\input{"+texfilename+"}\n";



    #  Now modify the refman.tex to include the user supplied tex files
   # print "Length of the teh source is : "+str(len(source_include_string))

    if len(source_include_string)>1:
        refman_tex_data = refman_tex_data.replace("%\\input{optional_chapter.tex}",source_include_string)        

    f.write(refman_tex_data)
    f.close()  

    # Now have to rename the refman.tex as a target .tex
#    move_file(latex_abs_dir + "/refman.tex", str(target[0]))
    return None





#-------------------------------------------------------------------------------
# def for LaTexCleaner
#   Cleans and formats doxygen generated latex code for certain segments
# Current list of items processed:
# a. replaces all occurrences of double dashes (--) in the tex file with with an en dash.
# b. removes the following Doxygen tags from the tex files: [static], 
#    [inherited], [read], [write], and [virtual].
# c. removes the unwanted autogenerated Doxygen index chapters from the 
#    refman.tex file, such as File Index and Data Structure Index, and only 
#    leaves in the Module Index chapter.
# d. removes the autogenerated LaTeX hyperlink commands in the tex files that point to the header files.
#


def ReplaceDashWithenDash (sourceline):

    endash       = " -\\\/-\\\/ "       #en dash search
    emdash       = " -\\\/-\\\/-\\\/ "  #em dash search
    # search each line for the stat search string
    sourceline=re.sub(endash," -- ",sourceline)
    sourceline=re.sub(emdash," --- ",sourceline)
    return sourceline;


def RemoveRedundantTags (sourceline):
    searchstring = "\{.tt.*\s*.mbox\{\[\}static.mbox\{\]\}\}|" + \
                   "\{.tt.*\s*.mbox\{\[\}inherited.mbox\{\]\}\}|" + \
                   "\{.tt.*\s*.mbox\{\[\}read.mbox\{\]\}\}|"+ \
                   "\{.tt.*\s*.mbox\{\[\}write.mbox\{\]\}\}"
    #              "\{.tt.*\s*.mbox\{\[\}virtual.mbox\{\]\}\}|" + \
    #              "\{.tt.*\s*.mbox\{\[\}private.mbox\{\]\}\}|" + \
    sourceline = re.sub(searchstring,"",sourceline)
    return sourceline;


def RemoveHyperLinks (sourceline):
    if re.search("file .hyperlink\{.*\}(\{.*\})", sourceline, re.IGNORECASE):        
        sourceline = re.sub(".hyperlink\{.*?\}","",sourceline)
        
    return sourceline;


def RemoveIndex (sourceline):
    if re.search("^[^%]\\chapter\{.*?Index\}", sourceline) and not re.search("\\chapter\{Module Index\}", sourceline, re.IGNORECASE):
        sourceline = "%" + sourceline

    elif re.search("^[^%]\\input\{.*?\}", sourceline, re.IGNORECASE) and (re.search("\\chapter\{.*?Index\}", sourceline, re.IGNORECASE) and not re.search("\\chapter\{Module Index\}", sourceline, re.IGNORECASE)):
        sourceline = "%" + sourceline

    return sourceline;


def LaTexCleaner (output_dir):

    searchPattern = output_dir+"/*.tex"
    lineList=[]
    dirList=glob.glob(searchPattern)
    if len(dirList) < 1:
        return

    for file in dirList:
        if not os.access(file, os.W_OK): 
            print "\n     " + file + " is not accessible.  You do not have permission to modify this file."
            continue

        # FILE IN Open and store file in an array (list)
        fin = open(file, "r")
        lineList = fin.readlines()
        fin.close()

        isdocmainfile = 0
        if re.search("refman.tex",file,re.IGNORECASE) :
            isdocmainfile=1 
        for i in range(0,len(lineList)):
            
            lineList[i] = ReplaceDashWithenDash(lineList[i]); 
            lineList[i] = RemoveRedundantTags(lineList[i]); 
            lineList[i] = RemoveHyperLinks(lineList[i]); 
            if isdocmainfile==1:
                lineList[i] = RemoveIndex(lineList[i]); 

        # FILE OUT
        # Write the file
        fou = open(file, "w")
        for i in range(0,len(lineList)):
            fou.write(lineList[i])
        fou.close

