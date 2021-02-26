#----------------------------------------------------------------------------
#
#
# GENERAL DESCRIPTION
#    Contains builder(s) to deploy files 
#
# Copyright 2016-2017 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
import os
import sys
import re
import shutil
import stat
import subprocess
import shlex
import threading
import traceback
import inspect
import SCons.Script
from SCons.Defaults import Delete
from SCons.Script import COMMAND_LINE_TARGETS
from SCons.Script import ARGUMENTS

#This file is created in the deploy_verify_tree to mark
# the tree as such
DEPLOY_VERIFY_TREE_FILE = '_mark_deploy_verify.txt'
#We should not see more than this number of c files in
#deploy tree
C_FILE_THRESHOLD = 200

#Maps each created filename to its file-specific lock at runtime
FILE_LOCKS = {}

#Used to avoid race conditions when adding to the 'FILE_LOCKS' lock map
LOCK_MAP_LOCK = threading.Lock()

def exists(env):
  ''' scons functions called by SCons tools'''
  return env.Detect('deploy_builder')

def generate(env):
  ''' scons functions called by SCons tools'''
  deploy_generate(env)

def deploy_generate(env):
  ''' the place we add builders or pseudo-builders we want to add
  to the environment so everyone can see these'''
  env.DeclareFeature("DEPLOY_BUILDER", uses_prefix=True)
  env.AddMethod(deploy, "Deploy")
  env.AddMethod(_deploy_dir, "_DeployDir")
  env.AddMethod(deploy_internal, "DeployInternal")
  env.AddMethod(deploy_feature, "DeployFeature")
  env.AddMethod(deploy_sdk, "DeploySdk")
  env.AddMethod(nuke_deploy_tree_builder, "NukeDeployTreeBuilder")
  env.AddMethod(create_tbc_pack, "Create_TBCPack_In_DeployTree")
  env.AddMethod(clean_deploy_list_log_files, "CleanDeployListLogFiles")
  env.AddMethod(get_default_public_variants, "GetDefaultPublicVariants")
  env.AddMethod(get_default_internal_variants, "GetDefaultInternalVariants")
  env.AddMethod(get_default_feature_variants, "GetDefaultFeatureVariants")

  verify_bld = env.Builder( action = verify_deploy_build)
  env.Append(BUILDERS = {"VerifyDeployBuild" : verify_bld})
  

def deploy(env, deploy_list, **kwargs):
  return _deploy_common(env, deploy_list, env.GetDefaultPublicVariants(), **kwargs)

def deploy_internal(env, deploy_list, **kwargs):
  kwargs['skip_check'] = False
  return _deploy_common(env, deploy_list, env.GetDefaultInternalVariants(), **kwargs)

def deploy_feature(env, deploy_list, **kwargs):
  kwargs['skip_check'] = False
  return _deploy_common(env, deploy_list, env.GetDefaultFeatureVariants(), **kwargs)

def deploy_sdk(env, deploy_list, **kwargs):
  kwargs['skip_check'] = False
  
  #version is the currently just the TZ branch version
  # TODO: work out a real versioning strategy
  version = re.sub('TZ\...\.', '' , ARGUMENTS['SOFTWARE_IMAGE'])
  
  subdirs = {
    'root'       : '',
    'qtee'       : 'qtee{}'.format(version),
    'qtee_build' : 'qtee{}/build'.format(version),
  } 
  
  subdir = subdirs.get(kwargs.get('deploy_root', 'qtee'))
  dst_path = kwargs.get('dst_path')
  
  if dst_path:
    #put the file in this destination (relative to the chose SDK root)
    kwargs['path_map'] = {'dst' : os.path.join(subdir, dst_path) }
  else:
    # maintain the source directory structure underneath the chosen SDK subdirectory
    src_path = 'trustzone_images'
    kwargs['path_map'] = {'src' : src_path, 'dst' : subdir }
 
  return _deploy_common(env, deploy_list, ['TZ_SDK'], **kwargs)
   
def _deploy_common(env, deploy_list, default_variants, **kwargs):
  deploy_variants =  env.Flatten(kwargs.get('deploy_variants', default_variants))
  skip_check = kwargs.get('skip_check', False)
  if not skip_check:
    extra_variants = list(set(deploy_variants).difference(default_variants))
    assert len(extra_variants) == 0, "Unexpected variants: {}".format(", ".join(extra_variants))
    if set(deploy_variants) != set(default_variants):
      print("WARNING: {}, ... will only be deploy to {}".format(env.RealPath(inspect.stack()[2][1], posix=True), ", ".join(deploy_variants)))

  kwargs['deploy_variants'] = deploy_variants
  deploy_units = deploy_builder(env, deploy_list, **kwargs)
  return deploy_units

def get_default_public_variants(env):
   return DeployVariant.get_default_public_variants()

def get_default_internal_variants(env):
   return DeployVariant.get_default_internal_variants()

def get_default_feature_variants(env):
   return DeployVariant.get_default_feature_variants()

def ammend_file_path(file_path):
  if '/' in file_path and '/' != os.path.sep:
      file_path = file_path.replace('/', os.path.sep)
  elif '\\' in file_path and '\\' != os.path.sep:
      file_path = file_path.replace('\\', os.path.sep)
  return file_path

def remove_tree(tree_root):
  if sys.platform.startswith('win'):
    os.chmod(tree_root, stat.S_IWRITE)
    for (path, dname_list, fname_list) in os.walk(tree_root, followlinks=True):
      for fname in fname_list:
        os.chmod(os.path.join(path, fname), stat.S_IWRITE)
      for dname in dname_list:
        os.chmod(os.path.join(path, dname), stat.S_IWRITE)	
  shutil.rmtree(tree_root)
  
def clean_deploy_list_log_files(env):
  #ideally we would want to delete only files for the BUILD_ID that deploy
  #is processing. But there is a chicken and egg problem of BUILD_ID not being
  #available when this method is called. So, we just go ahead and delete 
  #all the deploy logs
  pattern = 'deploy_.*\.log'
  print('Pattern to find files to delete {}'.format(pattern))
  d = [f for f in os.listdir(os.getcwd()) if re.search(pattern, f)]
  for logf in d:
    print('Deleting the log file {}'.format(logf))
    os.remove(os.path.join(os.getcwd(), logf))

def get_variants_from_log_files(build_id):
  pattern = 'deploy_' + re.escape(build_id) + '_.*\.log'
  print('Pattern to find files for getting variants {}'.format(pattern))
  d = [f for f in os.listdir(os.getcwd()) if re.search(pattern, f)]
  variants = []
  for logf in d:
    name, ext = os.path.splitext(logf)
    variants.append(name.split('deploy_' + build_id + '_')[-1])
  print('Returning variants {}'.format(variants))
  return variants

def create_tbc_pack(env, **kwargs):
  '''This method creates the tbc_cleanpack file that adds the 
  USES_CLEAN_PACK to the environment and allows to compile
  TAs that link against pre-built deployed artifacts
  '''
  deploy_root = get_deploy_root(env)
  requested_variants = kwargs.get('deploy_variants')
  if not requested_variants:
    requested_variants = DeployVariant.get_default_public_variants()
  requested_variants = [rv.upper() for rv in env.Flatten(requested_variants)]
  dv = get_common_variants(env, requested_variants)
  build_dir = os.path.dirname(os.path.realpath(__file__))
  tbc_template = os.path.join(build_dir, 'cp_template.py')
  if not os.path.exists(tbc_template):
    print('...ERROR: {} does not exist. Cannot proceed with deploy'.format(tbc_template))
    env.Exit(1)
  tbc_files = []
  for v in dv:
    tbc_file = os.path.join(deploy_root, v, 'trustzone_images', 'build', 'ms', 'tbc_cleanpack.py')
    env.InstallAs(tbc_file, tbc_template)
    tbc_files.append(env.File(tbc_file))
    env.Clean('deploy', tbc_file)
  return tbc_files

def verify_deploy_build(target, source, env):
  ''' a scons builder that takes in the source, target. 
  This builder walks over the different deploy variants. 
  The list of deploy variants to walk is hard-coded. In future
  we could have the source file populate the list of 
  variants the deployment should walk over and thus remove
  this need for hard-coding the list.
  '''
  target_str = str(target[0])
  source_str = str(source[0])
  print("Verify Deploy Build target {} source {}".format(target_str, source_str))
  deploy_root = get_deploy_root(env)
  deploy_verify = os.path.basename(deploy_root) + '_verify'
  deploy_verify_root = os.path.join(os.path.dirname(deploy_root), deploy_verify)
  if os.path.exists(deploy_verify_root):
    print('Deleting the Deploy Tree for Verification {}'.format(deploy_verify_root))
    remove_tree(deploy_verify_root)
  print('Copying the Deploy Tree for Verification from {} to {}'.\
         format(deploy_root, deploy_verify_root))
  #Create a copy of the deploy tree to run verification checks in that tree
  #this way we dont pollute the deploy tree
  shutil.copytree(deploy_root, deploy_verify_root)
  if source[0].exists():
    contents = source[0].get_text_contents()
  synchronized_write(str(target[0]), 'verify deploy: {}\n'.format(contents), \
      mode = 'w')
  cwd = os.getcwd()
  chipset = env['CHIPSET']
  for v in get_variants_from_log_files(env['BUILD_ID']):
    vtree = os.path.join(deploy_verify_root, v)
    synchronized_write(os.path.join(vtree, DEPLOY_VERIFY_TREE_FILE), \
        'This is a deploy verify tree:\n', mode = 'w')
    vobj = VerifyBuild(vtree,
	               ammend_file_path(env.subst('${BUILD_ROOT}')),
		       target_str,
		       chipset)
    if (vobj.verify_sources() and
        vobj.verify_license() and
        vobj.verify_compilation(v)):
      synchronized_write(target_str, \
          "Success: Verified Variant {}\n".format(v))
    else:
      #verification failed. We want to keep the verify tree to help debug
      synchronized_write(target_str, "Fail: Verified Variant {}".format(v))
      print('   ...ERROR: Failed Verifying Deployment for Variant {}'.format(v))
      env.Exit(1)

  #switch back to the original directory where scons looks for its dblite 
  #files. Not doing so results in errors
  os.chdir(cwd)
  return None

def deploy_builder(env, deploy_list, **kwargs):
  '''
  The builder takes in the list of deploy objects and key word arguments.
  The optional kwargs are:
  1.deploy_variants -> Allows user the specify the variant the deploy list
                       should be applicable for. If it is not specified
                       then the default variant list is assumed. The 
                       deploy_variants are passed in as a list.
                       eg: deploy_variants = ['HK11', 'HY11_1']
  '''
  if 'deploy' in COMMAND_LINE_TARGETS:
    return process_deploy_command(env['ROOT_GLOBAL_ENV'], deploy_list, **kwargs)

def nuke_deploy_tree_builder(env, **kwargs):
  ''' Nuke the deploy tree if it is present'''
  deploy_tree = get_deploy_root(env)
  #Somehow the deploy target is not waiting until the action
  #from the Command Builder (Delete) is executed. So, in case
  #where we are intending to deploy, simply nuke the deploy_tree
  if os.path.exists(deploy_tree):
    remove_tree(deploy_tree)
    print("DeployBuilder: NukeDeployTree {}".format(deploy_tree))
  else:
    print("DeployBuilder: NukeDeployTree {} does not exist".format(deploy_tree))
  return deploy_tree

def process_deploy_command(env, deploy_list, **kwargs):
  ''' this function finds the variants where the deploy_list
  needs to be deployed to and the destination root deploy
  directory
  ''' 
  deploy_root = get_deploy_root(env)
  workspace_root = os.path.dirname(ammend_file_path(env.subst('${BUILD_ROOT}'))) + os.sep
  requested_variants = kwargs.get('deploy_variants')
  if not requested_variants:
    requested_variants = DeployVariant.get_default_public_variants()
  requested_variants = [rv for rv in env.Flatten(requested_variants)]
  dv = get_common_variants(env, requested_variants)

  dlist = env.Flatten(deploy_list)
  node_list = []
  for item in dlist:
    node_list.append(env.SConsAbsPath(item))

  obj = deploy_command(env, dv, workspace_root, deploy_root, node_list, kwargs.get('path_map'))
  return obj

def get_common_variants(env, requested_variants):
  '''
  get the variants that are common between what user requested and 
  what is being processed. Return an empty list if there is no match
  '''
  cmdline_variants = get_command_line_variant(env)
  dv = set(cmdline_variants).intersection(requested_variants)
  if not dv:
    return []
  return dv
  
def get_command_line_variant(env):
  ''' 
  return the list of variants
  '''
  cv = []
  if ARGUMENTS.get('deploy_variant', None):
    cv.append(ARGUMENTS.get('deploy_variant').upper())
  else:
    cv= DeployVariant.get_supported_variants()
  return cv

def get_deploy_root(env):
  '''
  this function constructs the deploy root from the environment
  variables if it is not passed as a command line parameter
  '''
  if not ARGUMENTS.get('deploy_root', None):
    #construct the path
    return os.path.join(os.path.dirname(ammend_file_path(env.subst('${BUILD_ROOT}'))),
                        'deploy_' + env.subst('${BUILD_ID}'))

  return ARGUMENTS.get('deploy_root')

def deploy_command(env, variants, workspace_root, deploy_root, node_list, path_map):
  '''
  this function iterates over the variants and deploys the
  node list in the variant directory.
  '''
  target_list = []
  for v in variants:
    dv = DeployVariant(v, workspace_root, deploy_root, node_list, path_map)
    target_source_dict = dv.get_target_source_dict()
    target = []
    for k, v in target_source_dict.items():
      target.append(v)
      r = env.InstallAs(v,k)
      env.Alias('deploy', r)
    target_list.append(target)
  return target_list

def _deploy_dir(env, dir_root):
  '''helper function that walks a subtree and Deploys every file in that tree'''
  for (path, dname, fname) in os.walk(dir_root):
    for j in fname:
      if '.git' not in os.path.join(path,j):
        env.Deploy(os.path.join(path, j))

def get_file_lock(filename):
  '''
  This function returns a file-specific lock for the given filename. This lock
  should be used to ensure safe multi-threaded writes to the file.
  '''
  lock = FILE_LOCKS.get(filename)
  if lock == None:
    lock = _add_file_lock(filename)
  return lock

def _add_file_lock(filename):
  '''
  This function adds a new file-specific lock to the global lock map
  '''
  global FILE_LOCKS
  LOCK_MAP_LOCK.acquire()
  try:
    lock = FILE_LOCKS.get(filename)
    if lock == None:
      lock = threading.Lock()
      FILE_LOCKS[filename] = lock
  finally:
    LOCK_MAP_LOCK.release()
  return lock

def synchronized_write(filename, text, mode = 'a'):
  '''
  This function performs a thread-synchronized write of 'text' to 'filename'
  with the given 'mode' (default = 'a')
  '''
  lock = get_file_lock(filename)
  lock.acquire()
  try:
    with open(filename, mode) as outfile:
      outfile.write(text)
  finally:
    lock.release()

class DeployVariant(object):
  '''
  A utility class the helps generate the list of scons node objects
  for the deployed node corresponding to the source nodes.
  '''
  def __init__ (self, variant, workspace_root, deploy_root, node_list, path_map):
    '''
    workspace_root => parent workspace that needs to be deployed
    deploy_root => the root of the deploy tree
    variant => the desired variant to be deployed under the deploy_root
    node_list => the list of full src path of each node in the node_list
                 it is a list of type(str)
    '''
    self.variant = variant
    self.workspace_root = workspace_root
    self.node_list = node_list
    self.deploy_path = os.path.join(deploy_root, variant)
    deploy_dir_name = deploy_root.split(os.sep)[-1]
    self.path_map = path_map
    self.deploy_log = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                   deploy_dir_name + '_' + variant + '.log'
                                  )
    self.is_deploy_verify_tree = os.path.exists(os.path.join(workspace_root,
                                                      DEPLOY_VERIFY_TREE_FILE)
                                                )

  # Folder name listed below must be identical to 'Deliverable Folder' for SIR
  # in PCM data base else pack validation on CRM build will fail. 
  @staticmethod
  def get_supported_variants():
    return ['HK11',
            'HY11_1',
            'FEAT-BIN-PlayReady20',
            'FEAT-BIN-PlayReady30',
            'FEAT-BIN-TZ-HDCP2P2',
            'FEAT-BIN-HYPVM',
            'FEAT-BIN-TZ-HDCP1',
            'DO_NOT_SHIP'
           ]

  @staticmethod
  def get_default_public_variants():
    return ['HK11', 'HY11_1']

  @staticmethod
  def get_default_internal_variants():
    return ['DO_NOT_SHIP']

  @staticmethod
  def get_default_feature_variants():
    return ['FEAT-BIN-PlayReady20', 'FEAT-BIN-PlayReady30', 'FEAT-BIN-TZ-HDCP2P2', 'FEAT-BIN-HYPVM','FEAT-BIN-TZ-HDCP1']

  def get_scons_file_patterns(self):
    return re.compile('([a-zA-Z0-9_]+\.(api|sconscript|scons))|SConscript')

  def get_path_file(self, abspath):
    '''takes in abspath, strips the leading workspace root and extracts
    the path from trustzone_images upto the leaf directory and the filename 
    '''
    abspath = abspath.split(os.sep)
    wsroot = self.workspace_root.split(os.sep)
    assert(len(wsroot) <= len(abspath))
    fname = abspath[-1]
    tzpath = os.sep.join(abspath[len(wsroot)-1:-1])
    return (tzpath, fname)

  def target_source_dict_add_scons(self, ts_dict, tzpath):
    ''' this function iterates over all build directories along the
    tzpath and if there are any sconscript files that match the sconscript
    regex, adds them to the target_source dictionary object that is
    passed into this function - ts_dict
    '''
    build_par = self.workspace_root
    pattern = self.get_scons_file_patterns()
    tzpath_dirs_list = tzpath.split(os.sep)
    for d in tzpath_dirs_list:
      build_par = os.path.join(build_par, d)
      build_dir = os.path.join(build_par, 'build')
      if not os.path.exists(build_dir):
        continue
      ls = os.listdir(build_dir)
      for fnode in ls:
        if not pattern.match(fnode):
          continue
        build_tz_path, build = self.get_path_file(build_dir)
        target_node = os.path.join(self.deploy_path, build_tz_path, build, fnode)
        src = os.path.join(self.workspace_root, build_dir, fnode)
        ts_dict[src] = target_node
        if not self.is_deploy_verify_tree:
          synchronized_write(self.deploy_log, '{}\n'.format(target_node))

  def get_target_source_dict(self):
    ''' this function iterates over the node list and creates a 
    mapping aka dictionary of source to destination.
    '''
    target_source_dict = {}
    pattern = self.get_scons_file_patterns()
    for src_node in self.node_list:
      tzpath, fname = self.get_path_file(src_node)
      
      if isinstance(self.path_map, dict) and 'dst' in self.path_map:
        if 'src' in self.path_map:
          # a dictionary with src and dest so replace 'src' with 'dst'
          tzpath = tzpath.replace(self.path_map['src'], self.path_map['dst']).strip(os.sep)
        else:
          # no src specified, so deploy directly to dst
          tzpath = self.path_map['dst']

      target_node = os.path.join(self.deploy_path, tzpath, fname)
      target_source_dict[src_node] = target_node
      if not self.is_deploy_verify_tree:
        synchronized_write(self.deploy_log, '{}\n'.format(target_node))
      if pattern.match(fname):
        self.target_source_dict_add_scons(target_source_dict, tzpath)
    return target_source_dict

class VerifyBuild(object):
  ''' This class is the helper to the the verify deploy builder. It works
  on the build root and provides helper methods to check licenses, 
  completenesss of deployment and compilation of sources.
  '''
  LICENSE_CHECK_FILES_TO_IGNORE = ('scons_mod.py', 
                                   'stringl.h', 
                                   'atan2.c',
                                   'abs.c',
                                   'atan.c',
                                   '__cos.c',
                                   'cos.c',
                                   'floor.c',
                                   '__fpclassify.c',
                                   '__fpclassifyl.c',
                                   'io_file.h',
                                   'isdigit.c',
                                   'labs.c',
                                   'lib_math.h',
                                   '__rem_pio2.c',
                                   '__rem_pio2_large.c',
                                   'scalbn.c',
                                   '__sin.c',
                                   'sin.c',
                                   'sqrt.c',
                                   'stderr.c'
                                  )

  def __init__(self, root_dir, workspace_root, file_name, chipset):
    ''' 
    root_dir => the root of the directory that needs to be verified
    workspace_root => the workspace that was used to create the deployment.
                      the build directory of this workspace has log files
                      that this class uses
    file_name => this is the log file name passed in from the verify deploy
                 builder to populate the progress of deploy verification and
                 some record keeping
    chipset => this is the chipset that the deployment is being verified for.
               This value is needed to verify compilation.
    '''
    self.root_dir = root_dir
    self.workspace_root = workspace_root
    self.file_name = file_name
    self.chipset = chipset
    synchronized_write(file_name, \
        'VerifyBuild: init {} for chipset {}\n'.format(root_dir, chipset))

  def find_offending_license_in_files(self, sub_tree):
    ''' function that walks over the apps and core component trees and reads the
    first ~75 lines of the file to check for copyright markings. If there is a
    copyright and that copyright is not a Qualcomm copyright, then this function
    flags it and returns a True value.
    return True => if non qualcomm copyright is found
           False => if non qualcomm copyright is not found
    '''
    print('verify license for tree {}'.format(sub_tree))
    for (path, dname_list, fname_list) in os.walk(sub_tree, followlinks=True):
      for j in fname_list:
        if j in self.LICENSE_CHECK_FILES_TO_IGNORE:
          continue
        with open(os.path.join(path,j), 'r') as fr:
          count = 0
          curr_line = ''
          check_two_lines = False
          for line in fr:
            prev_line = curr_line
            curr_line = line.strip().lower()
            count = count + 1
            if not check_two_lines:
              if 'copyright' in curr_line and not ('qualcomm' in curr_line or 'scons' in curr_line):
                check_two_lines = True
              else:
                break #found the Qualcomm copyright in the file. Move onto next file
            else:
              if not ('qualcomm' in curr_line):
                print('...ERROR {} contains non QCOM copyright "{}"\n'.\
                           format(os.path.join(path,j), curr_line + '\n' + prev_line))
                return True
              else:
                check_two_lines = False
            if count == 75:
              break
    return False

  def verify_license(self):
    ''' wrapper function that calls the subfunction to check license in
    sub-dir'''
    return_value = True
    apps_dir = os.path.join(self.root_dir, 'trustzone_images', 'apps')
    core_dir = os.path.join(self.root_dir, 'trustzone_images', 'core')
    if self.find_offending_license_in_files(apps_dir) or \
       self.find_offending_license_in_files(core_dir):
      return_value =  False
    log = 'VerifyBuild: verify_license {}\n'.format(return_value)
    synchronized_write(self.file_name, log)
    print(log)
    return return_value

  def verify_sources(self):
    ''' a log file of files to be deployed is generated by the deploy builder.
    this function goes over the list in that file and verifies presence of 
    the file(s) in the deploy tree. If any file listed in the log file is not
    present in the tree, it is flagged. Also if the number of source files
    exceeds a threshold a failure is reported and verify sources will return
    False.
    '''
    variant = os.path.basename(self.root_dir)
    verify_dir = os.path.basename(os.path.dirname(self.root_dir))
    log_file_name = verify_dir.replace('verify', variant) + '.log'
    log_file_path = os.path.join(self.workspace_root, 'build', 'ms', log_file_name)
    log = 'Success {}'.format(log_file_path)
    return_value = True
    with open(log_file_path, 'r') as fr:
      for line in fr:
        line = line.strip()
        if not os.path.exists(line):
          log = '...ERROR: Fail to find {} in deploy tree'.format(line)
          return_value = False
          break

    synchronized_write(self.file_name, \
        'VerifyBuild: verify_sources :presence {}\n'.format(log))
    print('VerifyBuild: verify_sources {}\n'.format(log))

    ''' now check for number of source files aka .c files'''
    cfiles = 0
    for (path, dname_list, fname_list) in os.walk(self.root_dir, followlinks=True):
      for j in fname_list:
        path, ext = os.path.splitext(j)
        if ext in ('.c', '.cc', '.cpp', '.cxx'):
          cfiles = cfiles + 1

    log = 'Success C files = {} <= threshold {}'.format(cfiles, \
                                                       C_FILE_THRESHOLD)
    if cfiles > C_FILE_THRESHOLD:
      log = '... ERROR: {} > {} exceeded threshold of number of source files'.\
                  format(cfiles, C_FILE_THRESHOLD)
      return_value = False

    synchronized_write(self.file_name, \
        'VerifyBuild: verify_sources number of c files {}\n'.format(log))
    print('VerifyBuild: verify_sources {}\n'.format(log))
    synchronized_write(self.file_name, \
        'VerifyBuild: verify_sources {}\n'.format(return_value))
    return return_value

  def verify_compilation(self, variant):
    '''verify compilation in the tree for everything expected to be compiled
    in a deploy tree. Only default variants will compile. Other variants are
    binary only variants - no sources
    '''
    prefix = 'VerifyBuild: verify_compilation:'
    if not variant in DeployVariant.get_default_public_variants():
      return True
    build_dir = os.path.join(self.root_dir, 'trustzone_images', 'build', 'ms')
    cwd = os.getcwd()
    if not os.path.exists(build_dir):
      log = '{} ....ERROR {} does not exist\n'.format(prefix, build_dir)
      synchronized_write(self.file_name, log)
      print(log)
      return False

    os.chdir(build_dir)
    return_val = True

    try:
      branch = ARGUMENTS["SOFTWARE_IMAGE"]
      build_cmd = 'python build_all.py CHIPSET={} -b {} --cfg {} {}'.\
                                               format(
                                                 self.chipset, \
                                                 branch, \
                                                 'build_config_deploy.xml', \
                                                 '--recompile' \
                                               )
      build_args = shlex.split(build_cmd)
      print('build_cmd = {}  build_args = {}\n'.format(build_cmd, build_args))
      op = subprocess.check_call(build_args)
      if op:
        log = '{} ...ERROR Compilation failed in deploy verify step!!\n'.format(prefix)
        return_val = False
      else:
        log = '{} ...Success Compilation for deploy verify step!!\n'.format(prefix)
    except KeyError:
      log = "{} ...ERROR Branch required to verify compilation".format(prefix)
      return_val = False
    except subprocess.CalledProcessError as e:
      log = '{} ...ERROR {} {}\n'.format(prefix, e.returncode, e.cmd)
      return_val =  False
    except:
      log = '{} {}'.format(prefix, traceback.format_exc())
      return_val = False

    synchronized_write(self.file_name, log)
    print(log)
    os.chdir(cwd)
    return return_val
