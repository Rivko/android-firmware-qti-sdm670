#  ===========================================================================
#
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================

import os, sys, subprocess, logging, re, shutil, stat, atexit, platform
from datetime import datetime

def show_runtime(start):
  end = datetime.now()
  #seconds = (end-start).total_seconds()
  #minutes = int(seconds / 60)
  #seconds -= 60 * minutes
  #print 'Time taken: %sm %ss' % (minutes, seconds)

def describe_func(func_description):
  def decorator(func):
    func.func_description = func_description
    def wrapper(*args, **kwargs):
      print '[' + datetime.now().strftime('%H:%M:%S') + ']',
      print '%s...' % func.func_description,
      result = func(*args, **kwargs)
      if not result:
        print 'FAILED'
        sys.exit(1)
      elif type(result) == bool:
        print 'ok'
      else:
        print result
      return result
    return wrapper
  return decorator

def trace_command(command):
  log = open('packrat.log', 'a')
  log.write('\n******************************\n')
  log.write('*** Running command: %s\n' % command)
  log.write('******************************\n')
  cmd_pipe = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
  stdoutdata, stderrdata = cmd_pipe.communicate()
  log.write('\nSTDOUT:\n\n' + stdoutdata)
  log.write('\nSTDERR:\n\n' + stderrdata)
  log.close()
  return stdoutdata, stderrdata

def delete_file(file):
  # ensure the read-only flag is not set on the file before deleting, or it will fail
  os.chmod(file, stat.S_IWUSR | stat.S_IRUSR | stat.S_IXUSR)
  os.remove(file)

def move_file(src_file, dst_file):
  if os.path.isfile(src_file):
      # ensure the read-only flag is not set on the file
      os.chmod(src_file, stat.S_IWUSR | stat.S_IRUSR | stat.S_IXUSR)

  if os.path.isfile(dst_file):
      # ensure the read-only flag is not set on the file
      os.chmod(dst_file, stat.S_IWUSR | stat.S_IRUSR | stat.S_IXUSR)

  shutil.move(src_file, dst_file)

def chomp(x):
    if x[-2:]=='\r\n': return x[:-2]
    if x[-1:]=='\n': return x[:-1]
    return x

def normalize(path):
  return os.path.normcase(os.path.normpath(chomp(path)))

@describe_func('Check that we\'re running from the base of an AOP build')
def running_from_aop_build():
  print "\nCWD: " + ( re.escape(normalize(os.getcwd())) )
  return os.path.exists('build/build_common.py')

@describe_func('Identifying the MSM that this build targets')
def identify_msm_target(argv):
  if len(argv) < 2:
    return False
  if platform.system() == "Linux":
    build_script = 'build/build_%s.sh' % argv[1]
  else:
    build_script = 'build/build_%s.bat' % argv[1]
  if os.path.exists(build_script):
    return argv[1]
  return False

@describe_func('Clean the build of any known build targets')
def clean_build(msm):
  os.chdir('build')
  if platform.system() == "Linux":
    trace_command('./build_%s.sh aop -c' % msm)
  else:
    trace_command('.\\build_%s.bat aop -c' % msm)
  os.chdir('..')
  return True

@describe_func('Configure build for USES_NO_STRIP_NO_ODM + USES_NO_DEBUG to generate binary libraries, remove USES_INTERNAL_BUILD to strip features')
def configure_no_strip_no_debug(msm):
  msm_path = 'core/bsp/aop/build'
  move_file(msm_path + '/aop.scons', msm_path + '/aop.scons.orig')
  orig = open(msm_path + '/aop.scons.orig', 'r')
  new = open(msm_path + '/aop.scons', 'w')

  for line in orig.readlines():
    if 'USES_NO_STRIP_NO_ODM' in line:
      line = line.lstrip('#')
    if 'USES_NO_DEBUG' in line:
      line = line.lstrip('#')
    if 'USES_INTERNAL_BUILD' in line:
      line = ""
    new.write(line)

  orig.close()
  new.close()
  delete_file(msm_path + '/aop.scons.orig')
  
  # Update build_common to default to external variant BUILD_ID
  move_file('build/build_common.py', 'build/build_common.py.orig')
  orig = open('build/build_common.py.orig', 'r')
  new = open('build/build_common.py', 'w')

  for line in orig.readlines():
    if 'DEFAULT_BUILD_ID' in line:
      nline = line.split("=")[0]
      line = nline+"= 'AAAAANAZO'\n"
    new.write(line)

  orig.close()
  new.close()
  delete_file('build/build_common.py.orig')
  
  return True

@describe_func('Build the AOP image')
def build(msm):
  os.chdir('build')
  if platform.system() == "Linux":
    trace_command('./build_%s.sh %s' % (msm, ' '.join(sys.argv[2:])))
  else:
    trace_command('.\\build_%s.bat %s' % (msm, ' '.join(sys.argv[2:])))
  os.chdir('..')
  return True

@describe_func('Configure build to the customer release configuration')
def configure_customer_release(msm):
  # Update aop.scons for customer release options
  msm_path = 'core/bsp/aop/build'
  move_file(msm_path + '/aop.scons', msm_path + '/aop.scons.orig')
  orig = open(msm_path + '/aop.scons.orig', 'r')
  new = open(msm_path + '/aop.scons', 'w')

  for line in orig.readlines():
    if 'USES_NO_STRIP_NO_ODM' in line:
      line = '#' + line.lstrip('#')
    if 'USES_NO_DEBUG' in line:
      line = '#' + line.lstrip('#')
    if 'USES_INTERNAL_BUILD' in line:
      line = ""
    new.write(line)

  orig.close()
  new.close()
  delete_file(msm_path + '/aop.scons.orig')
  
  return True

@describe_func('Check that the build was successful')
def check_elf_exists(msm):
  msm_path = 'build/aop/bf/build'
  if msm == '670':
    return os.path.exists(msm_path + '/' + '670' + '/pm670' + '/aop.mbn')
  elif msm == '605':
	return os.path.exists(msm_path + '/' + '670' + '/pm605' + '/aop.mbn')
  else:
    return os.path.exists(msm_path + '/aop.mbn')

@describe_func('Create local scons.bat that runs the build in the python profiler')
def create_profiled_scons():
  if platform.system() == "Linux":
    move_file('tools/build/scons/SCons/scons.sh', 'tools/build/scons/SCons/scons.sh.normal')
    move_file('tools/build/scons/SCons/profiled_scons.sh', 'tools/build/scons/SCons/scons.sh')
  else:
    move_file('tools/build/scons/SCons/scons.bat', 'tools/build/scons/SCons/scons.bat.normal')
    move_file('tools/build/scons/SCons/profiled_scons.bat', 'tools/build/scons/SCons/scons.bat')
  return True

@describe_func('Restore the normal scons.bat')
def restore_regular_scons():
  if platform.system() == "Linux":
    move_file('tools/build/scons/SCons/scons.sh', 'tools/build/scons/SCons/profiled_scons.sh')
    move_file('tools/build/scons/SCons/scons.sh.normal', 'tools/build/scons/SCons/scons.sh')
  else:
    move_file('tools/build/scons/SCons/scons.bat', 'tools/build/scons/SCons/profiled_scons.bat')
    move_file('tools/build/scons/SCons/scons.bat.normal', 'tools/build/scons/SCons/scons.bat')
  return True

@describe_func('Disable RINIT Builder')
def disable_rinit_builder(msm):
  # Remove rinit_builder from the build tools so that it won't be called during build process.
  # Since the .FLAT file have to be packed out, the rinit_builder cannot be run successfully.
  # As rinit_vals.c has been generated in previous build process, we can safely disable rinit builder.
  msm_path = 'core/bsp/aop/build'
  move_file(msm_path + '/aop.scons', msm_path + '/aop.scons.orig')
  orig = open(msm_path + '/aop.scons.orig', 'r')
  new = open(msm_path + '/aop.scons', 'w')

  for line in orig.readlines():
    if 'rinit_builder.py' in line:
      line = '#' + line.lstrip('#')
    new.write(line)

  orig.close()
  new.close()
  delete_file(msm_path + '/aop.scons.orig')
  
  return True

@describe_func('Run a simulated build, collecting scons dependency information')
def build_with_nobuild_and_tree_output(msm):
  os.chdir('build')
  if platform.system() == "Linux":
    trace_command('./build_%s.sh -n --tree=all,prune' % msm)
  else:
    trace_command('.\\build_%s.bat -n --tree=all,prune' % msm)
  os.chdir('..')
  return True

flist = set()
	
@describe_func('Parse dependency/profiling information to generate a list of all files used in build')
def parse_files_used(file_list,msm_id):
  # Figure out which SConscripts are used
  os.chdir('build')
  stats, statserr = trace_command('python -c "import pstats; pstats.Stats(\'buildstats\').print_stats()"')
  os.chdir('..')
  cwd = re.escape(normalize(os.getcwd()))
  stats = normalize(stats)
  profiler_used_files = re.findall(cwd + '[^:]+', stats)
  file_list.update(set(profiler_used_files))
  
  if msm_id == '670' or msm_id == '605':
    file_location = 'build/build-log.txt'	
    if os.path.exists(file_location):
        index = 1
        while os.path.exists('build/build-log-%i.txt' % (index)):
          index += 1
        logfile = 'build-log-%i.txt' % (index-1)
        log_location = 'build/' + logfile
        build_log = open(log_location, 'r')
        scons_used_files = []
        for line in build_log.readlines():
          m = re.search('\+-(?P<file>.+)', normalize(line))
          if m:
            scons_used_files.append(m.group('file'))
        
        flist.update(set(scons_used_files))
  else:	
	  # Figure out what files scons uses in a build
	  logfile = 'build-log.txt'	
	  # Figure out what files scons uses in a build
	  log_location = 'build/' + logfile
	  build_log = open(log_location, 'r')
	  
	  scons_used_files = []
	  for line in build_log.readlines():
		m = re.search('\+-(?P<file>.+)', normalize(line))
		if m:
		  scons_used_files.append(m.group('file'))
	  file_list.update(set(scons_used_files))
  
  return 'found %s used files' % len(file_list)

@describe_func('Collect a list of all files in the build tree')
def list_all_files(file_list):
  for root, dirs, files in os.walk('.'):
    root = os.path.abspath(root)
    file_list.update(set([normalize(os.path.join(root, f)) for f in files]))
  return 'found %s files total' % len(file_list)

@describe_func('Selecting additional whitelisted files as used')
def match_whitelist(all_files, whitelist_files, msm):
  packrat_path = os.path.abspath(os.path.dirname(sys.argv[0]))
  whitelist = {'MSM_ID':msm,'VARIANT_ID':'AAAAANAZO'}
  if msm == '670':
    execfile(packrat_path + '/whitelist_670.py', whitelist)
  else:
	execfile(packrat_path + '/whitelist.py', whitelist)
  matches = [f for rule in whitelist['rules'] for f in all_files if re.search(rule, f)]
  log = open('./build/whitelist_log.txt', 'a')
  for match in matches:
    log.write(match+'\n')
  log.close()
  whitelist_files.update(matches)
  return 'matched %s' % len(matches)

@describe_func('Selecting blacklisted files as unused')
def match_blacklist(all_files, blacklist_files, msm):
  packrat_path = os.path.abspath(os.path.dirname(sys.argv[0]))
  blacklist = {'MSM_ID':msm,'VARIANT_ID':'AAAAANAZO'}
  execfile(packrat_path + '/blacklist.py', blacklist)
  matches = [f for rule in blacklist['rules'] for f in all_files if re.search(rule, f, flags=re.IGNORECASE)]
  #with open('./build/blacklist_log.txt', 'a') as f:
  #    f.write('\n'.join(matches))
  blacklist_files.update(matches)
  return 'matched %s' % len(matches)


@describe_func('Remove files that do not contribute to the build')
def remove_unused_files(file_list):
  for file in file_list:
    delete_file(file)
  return 'removed %s' % len(file_list)

@describe_func('Clean up any empty directories')
def clean_empty_directories():
  # clean up empty directories -- note, rmdir will refuse non-empty, hence
  # the lack of checking for that and the try catch block
  empty_dir_count = 0
  for root, dirs, files in os.walk('.', topdown=False):
    for name in dirs:
      try:
        dirname = os.path.join(root, name)
        os.chmod(dirname, stat.S_IWUSR | stat.S_IRUSR | stat.S_IXUSR)
        os.rmdir(dirname)
        empty_dir_count += 1
      except:
        pass
  return 'removed %s' % empty_dir_count

@describe_func('Clean up after packrat')
def final_cleanup():
  packrat_files = [
    'packrat.log',
    'build/packrat.log',
    'build/buildstats',
    'build/build.log',
    'build/buildstats',
  ]
  for f in packrat_files:
    if os.path.exists(f):
      delete_file(f)
  return True


def main(argv):
  start = datetime.now()
  atexit.register(show_runtime, (start))

  # housekeeping
  running_from_aop_build()
  msm = identify_msm_target(argv)
  
  if msm == '670': 
    supported_targets = ['670','605']
  else:
    supported_targets = [msm]

  # build the binary libraries
  for msm_id in supported_targets:
    configure_no_strip_no_debug(msm_id)
    build(msm_id)

    # ensure that the build works using the binary libraries
    configure_customer_release(msm_id)
    clean_build(msm_id)
    build(msm_id)
    check_elf_exists(msm_id)
    
    # profile and remove unused files
    clean_build(msm_id)
  create_profiled_scons()

  used_files = set()
  #with open('used_files.txt','w') as f:
  #    f.write('\n'.join(used_files))

  for msm_id in supported_targets:
    build_with_nobuild_and_tree_output(msm_id)
    parse_files_used(used_files, msm_id)
	
  all_files = set()
  list_all_files(all_files)
  #with open('all_files.txt','w') as f:
  #    f.write('\n'.join(all_files))

  whitelist_files = set()
  match_whitelist(all_files, whitelist_files, msm)
  #with open('whitelist_files.txt','w') as f:
  #    f.write('\n'.join(whitelist_files))

  blacklist_files = set()
  match_blacklist(all_files, blacklist_files, msm)
  #with open('blacklist_files.txt','w') as f:
  #    f.write('\n'.join(blacklist_files))

  unused_files  = all_files - used_files
  #with open('unused_files.txt','w') as f:
  #    f.write('\n'.join(blacklist_files))
  if msm == '670':
	packout_files = (unused_files | blacklist_files) - whitelist_files - flist
  else:
	packout_files = (unused_files | blacklist_files) - whitelist_files
  #with open('packout_files.txt','w') as f:
  #    f.write('\n'.join(packout_files))

  remove_unused_files(packout_files)
  clean_empty_directories()

  # finally, ensure the build still works
  restore_regular_scons()
  disable_rinit_builder(msm)
  for msm_id in supported_targets:
    build(msm_id)
    check_elf_exists(msm_id)

if __name__ == '__main__':
  main(sys.argv)

