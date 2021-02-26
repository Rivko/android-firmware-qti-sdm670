#===============================================================================
# Copyrigha (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================

import sys
import os, errno
import string
from subprocess import Popen, PIPE, STDOUT
import SCons.Action
from SCons.Script import *

def pl_register(env, name, images=None):
   if env.GetOption('clean') or images == None or env['IMAGE_NAME'] not in images:
     return
   list_file = os.path.join(env['BUILD_ROOT'],
                            'platform',
                            'platform_libs_list',
                            'build',
                            env['BUILDPATH'],
                            'libs_list.txt')
   with FileLock(list_file):
      f = open(list_file, "a")
      f.write("%s\n" % name)
      f.close()

def pl_register_default(env, name, location, images):
   # Only platform libs can register for default libs
   if env.GetOption('clean') or env['IMAGE_NAME'] not in images:
      return
   default_list_file = os.path.join(env['BUILD_ROOT'],
                                'platform',
                                'platform_libs_list',
                                'build',
                                env['BUILDPATH'],
                                'libs_list_default.txt')
   if location < 0:
      raise ValueError('pl_register_default called with value:%d' %location )

   with FileLock(default_list_file):
      lines = []

      # Ensure that we dont have a library on that line already
      if (os.path.exists(default_list_file)):
         f = open(default_list_file)
         lines = f.readlines()
         f.close()

         if(len(lines) >= location+1):
            if(lines[location] != '\n'):
                # If this is a duplicate, then the file did not get deleted for
                # some reason. Ignore the call
                if lines[location][:-1] == name:
                    return
                else:
                    raise ValueError('Library %s already exists at location: %d' % (lines[location][:-1], location))

      f = open(default_list_file, "w")

      # Pad the list if required
      if len(lines) < int(location)+1 :
          while len(lines) < location+1:
              lines.extend('\n')

      lines[location] = name + lines[location]
      f.writelines(lines)
      f.close()

def pl_generate_builder(target, source, env):
   if env.GetOption('clean') or env['IMAGE_NAME'] == 'DEVCFG_IMG':
     return
   # Read the default list
   default_list_file = os.path.join(env['BUILD_ROOT'],
                                    'platform',
                                    'platform_libs_list',
                                    'build',
                                     env['BUILDPATH'],
                                    'libs_list_default.txt'
                                    )
   default_libs = []
   try:
     f = open(default_list_file, "r")
     default_libs = f.readlines()
     f.close()
   except:
     print "No default libs found."

   # Read the list file into a set
   list_file = os.path.join(env['BUILD_ROOT'],
                            'platform',
                            'platform_libs_list',
                            'build',
                            env['BUILDPATH'],
                            'libs_list.txt')
   f = open(list_file, "r")
   lines = f.readlines()
   f.close()

   #Remove duplicates
   libs = list(set(lines))

   # Write output source
   f = open(str(target[0]), "w")
   f.write('#include "platform_libs_list.h"\n')
   f.write('#include "platform_libs.h"\n\n')

   # Default libs first
   for name in default_libs:
      if name != '\n':
         f.write("PL_DEP(%s)\n" % name.strip())

   for name in libs:
      f.write("PL_DEP(%s)\n" % name.strip())

   f.write( '\nstruct platform_lib* (*pl_list[])(void) = {\n')
   for name in default_libs:
      if name != '\n':
         f.write("\tPL_ENTRY(%s),\n" % name.strip())

   for name in libs:
      f.write("\tPL_ENTRY(%s),\n" % name.strip())

   f.write('\t0\n')
   f.write('};\n')
   f.close()

   # Delete the list file so it gets recreated the next time
   if os.path.exists(list_file):
      os.remove(list_file)
   if os.path.exists(default_list_file):
      os.remove(default_list_file)


def pl_dl_expose_lstbuilder(target, source, env):
   #import pdb;pdb.set_trace()
   dynsymbols_file = str(target[0])
   externs_file = str(target[1])
   version_file = str(target[2])
   symbol_list = []

   # Open the base symbol file for reading, and symbol file for writing.
   try:
      dyn_symbol_handle = open(dynsymbols_file, 'w')
      extern_symbol_handle = open(externs_file, 'w')
      version_symbol_handle = open(version_file, 'w')
   except IOError, error :
      errStr = error.strerror + ":" + error.filename
      dlinker_error(env, errStr)

   dyn_symbol_handle.write('{\n')
   extern_symbol_handle.write('EXTERN (')
   version_symbol_handle.write('FASTRPC_SHELL {\n')
   version_symbol_handle.write('global:\n')

   for sym_file in source:
      # Open the symbol file for reading
      try:
         # can be scons node or file
         sym_file = env.RealPath(str(sym_file))
         dlexpose_symbol_handle = open(sym_file, 'r')
      except IOError, error :
         errStr = error.strerror + ":" + error.filename
         dlinker_error(env, errStr)

      for line in dlexpose_symbol_handle :
         line = line.strip()
         if line and (line not in symbol_list):
            symbol_list.append(line)
            dyn_symbol_handle.write(line + ";\n")
            extern_symbol_handle.write(line + "\n")
            version_symbol_handle.write(line + ";\n")

      dlexpose_symbol_handle.close()

   # write footer
   dyn_symbol_handle.write('};\n')
   extern_symbol_handle.write(');\n')
   version_symbol_handle.write('local: *;\n')
   version_symbol_handle.write('};\n')

   dyn_symbol_handle.close()
   extern_symbol_handle.close()
   version_symbol_handle.close()

   return

def pl_check_dynamic_symbols_builder(target, source, env):

  symbols = []
  unexported_syms = []
  target_file = str(target[0])
  elf_file = str(source[0])
  sym_file = str(source[1])
  tfile = elf_file + ".symbol_check_failed"

  if os.path.isfile(target_file) and os.access(target_file, os.R_OK):
    os.remove(target_file)

  if os.path.isfile(tfile) and os.access(tfile, os.R_OK):
    os.remove(tfile)

  if env.GetOption('clean') or env['IMAGE_NAME'] == 'DEVCFG_IMG':
    return

  if not (os.path.isfile(elf_file) and os.access(elf_file, os.R_OK)):
    print "\nERROR: file not found " + str(elf_file)
    return
  if not (os.path.isfile(sym_file) and os.access(sym_file, os.R_OK)):
    print "\nERROR: symbol file not found " + str(sym_file)
    return

  # Get dynamic symbols from elf file
  cmd = ''.join([env.subst('$NM'), ' -D ', str(elf_file)])
  proc = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT)
  while True:
    line = proc.stdout.readline()
    if not line:
      break
    line = line.rstrip()
    line = line.split(' ')
    symbols.append(line[2])

  # Remove duplicate symbols
  final_syms = list(set(symbols))

  #Its time to verify symbols present or not
  sym_handle = open(sym_file, 'r')
  for line in sym_handle:
    line = line.strip()
    line = line.translate(None, " {;}")
    if line and (line not in final_syms):
      unexported_syms.append(line)

  if len(unexported_syms) > 0:
    print "\n ERROR, Symbols which are not exported found " + str(len(unexported_syms)) + "\n"
    print unexported_syms
    os.rename(elf_file, tfile)
    print "\n renamed elf file"
  else:
    print "\n\n\nAll dynamic symbols are exported!"
    open(target_file, 'w+').close()

  return

def pl_shell_builder(target, source, env):
   if env.GetOption('clean') or env['IMAGE_NAME'] == 'DEVCFG_IMG':
      return

   target_debug = str(target[0])+".unstripped"
   source_file = str(source[0])
   symbol_file = str(target_debug)+ ".undefined_symbols"
   if os.path.isfile(symbol_file) and os.access(symbol_file, os.R_OK):
      os.remove(symbol_file);
   if os.path.isfile(target_debug) and os.access(target_debug, os.R_OK):
      os.remove(target_debug);

   if os.path.isfile(source_file) and os.access(source_file, os.R_OK):
      fp = open(source_file, "rb")
      line = fp.read().replace("DEFAULT_PHYSPOOL","HLOS_PHYSPOOL\x00\x00\x00")
      fp.close()
      fpt = open(str(target_debug), "wb+")
      fpt.write(line)
      fpt.close()
   else:
      print str("ERROR: " + source_file + " file not found or no access permission")
      return

   symbols_cmd = ''.join([ '${NM} -D --undefined-only ', str(target_debug), ' > ' + symbol_file])
   data, err, rv = env.ExecCmds (symbols_cmd, silent=True, target='fastrpc shell undefined symbols')
   undefined_symbols = []
   if os.path.isfile(symbol_file) and os.access(symbol_file, os.R_OK):
      fp = open(symbol_file)
      undefined_symbols = fp.read().splitlines()
      fp.close()

   if len(undefined_symbols) == 0 or 'USES_FASTRPC_SHELL_SYMBOLS_NOCHECK' in env:
      strip_cmd = ''.join([ '${STRIP} -x ', str(target_debug), ' -o ', str(target[0])])
      data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='FastRPC Stripped Shell File')
   else:
      print "ERROR: Failed to generated "+str(target[0])+". Found undefined symbols ("+str(len(undefined_symbols))+")"
      print undefined_symbols
      return

def pl_shell_patch_builder(env,target, source):
   if env.GetOption('clean') or env['IMAGE_NAME'] == 'DEVCFG_IMG':
     return

   src_file = str(source[0])
   if os.path.isfile(src_file) and os.access(src_file, os.R_OK):
       # qurt_fastint.o made PIE compatible, keep the file.
       #ar_cmd = ''.join([ '${AR} -d ', src_file, ' qurt_fastint.o'])
       #data, err, rv = env.ExecCmds (ar_cmd, silent=True, target='FastRPC shell patches')
       fp = open(str(target[0]), 'w+')
       fp.close()
   else:
       print "fastrpc shell patches failed. QuRT library not found or no access "+src_file

def pl_dl_expose_file(env, source, exclude=None):
   if env.GetOption('clean') or env['IMAGE_NAME'] != 'FASTRPC_SHELL_IMG':
     return

   build_folder = str(env.RealPath('${BUILDPATH}'))
   filename = os.path.basename(source)
   target = os.path.join(build_folder, filename)

   if os.path.isfile(source) and os.access(source, os.R_OK):
         if not os.path.exists(os.path.dirname(target)):
            os.makedirs(os.path.dirname(target))

         exclude_symbols = []
         if os.path.isfile(exclude) and os.access(exclude, os.R_OK):
            fpe = open(str(exclude));
            exclude_symbols = fpe.read().splitlines()
            fpe.close()

         fp = open(str(source))
         base_symbols = fp.read().splitlines()
         fp.close()
         fpt = open(str(target),'w+');
         for symbol in base_symbols:
            if symbol not in exclude_symbols:
               fpt.write(symbol+'\n')

         fpt.close()
         env.Depends(source, target)
         env.DLExposeFile(target)
   else:
         print "ERROR: failed to create symbol file, check file existence or no access "+source

   return target

def pl_create_extern_list(env, source):
    if env.GetOption('clean') or env['IMAGE_NAME'] != 'FASTRPC_SHELL_IMG':
     return source

    if type(source) != list:
        source = [source]

    build_folder = os.path.dirname(source[0])
    if not os.path.exists(build_folder):
        os.makedirs(build_folder)

    dfilename = os.path.join(build_folder, "dynsymbols.c")
    hfilename = os.path.join(build_folder, "dynsymbols.h")
    dhandle = open(dfilename, 'w+')
    hhandle = open(hfilename, 'w+')
    dhandle.write('#include "dynsymbols.h"\n')
    dhandle.write('typedef unsigned int uintptr_t;\n')
    dhandle.write('uintptr_t dynsymbols(void) {\n')
    dhandle.write('return 0 |\n')
    hhandle.write('#pragma GCC system_header\n')
    for sfile in source:
        sf = env.RealPath(str(sfile))
        if os.path.isfile(sf) and os.access(sf, os.R_OK):
            try:
               shandle = open(sf, 'r')
            except IOError, error :
               errStr = error.strerror + ":" + error.filename
               dlinker_error(env, errStr)

            for line in shandle:
                line = line.strip()
                dhandle.write('(uintptr_t)')
                dhandle.write(line)
                dhandle.write(' |\n')
                hhandle.write('extern void '+line+'(void);\n')
            shandle.close()

    hhandle.close()
    dhandle.write('0;\n')
    dhandle.write('}\n')
    dhandle.close()

    env.Depends(source, dfilename)
    env.Depends(source, hfilename)
    return dfilename

def pl_create_debug_file(env, target, source):
   images_debug_file =   {'FASTRPC_SHELL_IMG','AVS_ADSP_USER', 'SSC_ADSP_USER'} 
   if env.GetOption('clean') or env['IMAGE_NAME'] not in images_debug_file:
     return target
   source_file = str(source)
   target_file = str(target)
   build_folder = os.path.dirname(target_file)
   if not os.path.exists(build_folder):
      os.makedirs(build_folder)

   if os.path.isfile(source_file) and os.access(source_file, os.R_OK):
      fp = open(source_file, "r")
      line = fp.read()
      fp.close()
      fpt = open(target_file, "w+")
      fpt.write("#define FASTRPC_EXCEPTION_HANDLER_DISABLE \n\n")
      fpt.write(line)
      fpt.close()
   else:
      print str("WARNING: " + source_file + " file not found or no access permission")
      return target

   return target

def pl_generate(env, name):
   env.RequirePublicApi("PLATFORM_LIBS")
   return env.AlwaysBuild(env.PLGenerateBuilder(target = name, source = None))

def exists(env):
   return env.Detect('platform_libs')

def generate(env):
   if env.GetOption('clean'):
     print 'Platform builders CLEANING'
     list_file = os.path.join(env['BUILD_ROOT'],
                              'platform',
                              'platform_libs_list',
                              'build',
                               env['BUILDPATH'],
                               'libs_list.txt')
     if os.path.exists(list_file):
       print '\t removing %s', list_file
       os.remove(list_file)

     default_list_file = os.path.join(env['BUILD_ROOT'],
                                      'platform',
                                      'platform_libs_list',
                                      'build',
                                       env['BUILDPATH'],
                                      'libs_list_default.txt')
     if os.path.exists(default_list_file):
       print '\t removing %s', default_list_file
       os.remove(default_list_file)

   env.AddMethod(pl_register, "PLRegister")
   env.AddMethod(pl_register_default, "PLRegisterDefault")
   env['BUILDERS']['PLGenerateBuilder'] = Builder(name = "PLGenerateBuilder", action = pl_generate_builder)
   env['BUILDERS']['PLShellBuilder'] = Builder(name = "PLShellBuilder", action = pl_shell_builder)
   env['BUILDERS']['PLShellPatchBuilder'] = Builder(name = "PLShellPatchBuilder", action = pl_shell_patch_builder)
   env['BUILDERS']['PLDLExposeLSTBuilder'] = Builder(name = "PLDLExposeLSTBuilder", action = pl_dl_expose_lstbuilder)
   env['BUILDERS']['PLCheckDynamicSymbols'] = Builder(name = "PLCheckDynamicSymbols", action = pl_check_dynamic_symbols_builder)
   env.AddMethod(pl_dl_expose_file,"PLDLExposeFile")
   env.AddMethod(pl_create_extern_list,"PLCreateExternList")
   env.AddMethod(pl_create_debug_file,"PLCreateDebugFile")
   env.AddMethod(pl_generate, "PLGenerate")

# BSD Licensed Cross-platform file locking support in Python
# See http://www.evanfosmark.com/2009/01/cross-platform-file-locking-support-in-python/
# License information - http://www.evanfosmark.com/2009/01/cross-platform-file-locking-support-in-python/#comment-1054

class FileLockException(Exception):
    pass

class FileLock(object):
    """ A file locking mechanism that has context-manager support so
        you can use it in a with statement. This should be relatively cross
        compatible as it doesn't rely on msvcrt or fcntl for the locking.
    """

    def __init__(self, file_name, timeout=10, delay=.05):
        """ Prepare the file locker. Specify the file to lock and optionally
            the maximum timeout and the delay between each attempt to lock.
        """
        self.is_locked = False
        self.lockfile = os.path.join(os.getcwd(), "%s.lock" % file_name)
        self.file_name = file_name
        self.timeout = timeout
        self.delay = delay


    def acquire(self):
        """ Acquire the lock, if possible. If the lock is in use, it check again
            every `wait` seconds. It does this until it either gets the lock or
            exceeds `timeout` number of seconds, in which case it throws
            an exception.
        """
        start_time = time.time()
        while True:
            try:
                #Create enclosing folder (if necessary)
                if not os.path.exists(os.path.dirname(self.lockfile)):
                    os.makedirs(os.path.dirname(self.lockfile))
                self.fd = os.open(self.lockfile, os.O_CREAT|os.O_EXCL|os.O_RDWR)
                break;
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise
                if (time.time() - start_time) >= self.timeout:
                    raise FileLockException("Timeout occured.")
                time.sleep(self.delay)
        self.is_locked = True


    def release(self):
        """ Get rid of the lock by deleting the lockfile.
            When working in a `with` statement, this gets automatically
            called at the end.
        """
        if self.is_locked:
            os.close(self.fd)
            os.unlink(self.lockfile)
            self.is_locked = False


    def __enter__(self):
        """ Activated when used in the with statement.
            Should automatically acquire a lock to be used in the with block.
        """
        if not self.is_locked:
            self.acquire()
        return self


    def __exit__(self, type, value, traceback):
        """ Activated at the end of the with statement.
            It automatically releases the lock if it isn't locked.
        """
        if self.is_locked:
            self.release()


    def __del__(self):
        """ Make sure that the FileLock instance doesn't leave a lockfile
            lying around.
        """
        self.release()

def dlinker_error(env, Info, Info1=None):
    """
    Error handler for linker symbol generate script. Info can be a string that
    describes the error and Info1 can be any data that needs to be printed along
    with the error message.
    """
    env.PrintError("Dlinker error found: " + Info)
    env.PrintError(Info1)
    raise Exception(Info)
