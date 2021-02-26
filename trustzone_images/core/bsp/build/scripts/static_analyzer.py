import os
import atexit
import shutil
from SCons.Script import *
#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('static_analyzer')
   
def generate(env):
    setup_static_analyzer(env)
    atexit.register(post_process_sa_data, env)
    
#-------------------------------------------------------------------------------
# Setup environment variable for static analyzer.
#-------------------------------------------------------------------------------
def setup_static_analyzer(env):
  vars = Variables(None)
  if 'tzbsp_enable_sa' not in env:
    vars.Add(BoolVariable('tzbsp_enable_sa','Build with static analyer',1)) 
  # update environment and read command line variables
  vars.Update(env)
  #set path for sa analyzer reports
  if env['tzbsp_enable_sa'] == 1:
   sa_report_path = os.path.join('$BUILD_ROOT','build','ms','sa',env['BUILD_ID'], env.get('IMAGE_ALIASES')[0])
   env.Replace(SA_REPORT_PATH = sa_report_path)
   env.Append(CFLAGS = '--compile-and-analyze ' + sa_report_path + ' ')
   if  env.GetOption('clean') :
     if os.path.exists(env.subst('${SA_REPORT_PATH}')):
        shutil.rmtree(env.subst('${SA_REPORT_PATH}'))

#-------------------------------------------------------------------------------
# Run Post Process script at end of static analyzer.
#-------------------------------------------------------------------------------
def post_process_sa_data(env):
  if env['PLATFORM'] not in ['windows', 'win32', 'cygwin']:
    if os.path.exists(env.subst('$SA_REPORT_PATH')):
        cmd="perl {0} --report-dir {1} --html-title Static_Analysis_Report ".format(os.path.join(os.environ['LLVMBIN'],'post-process')     ,env.subst('${SA_REPORT_PATH}')) 
        os.system(cmd)  
