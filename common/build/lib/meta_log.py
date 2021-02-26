'''
=======================================================================================================
meta_log.py:

Usage:
   This is a library and all logging functions would be a part of this script.This module would be 
   imported from all modules needing to log the necessary information

 Copyright (c) 2011-2018 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR 
-----------------------------------------------------------------------------------------------
 $Header: //components/rel/build.glue/1.0/lib/meta_log.py#7 $
 $DateTime: 2018/04/10 14:00:50 $
======================================================================================================
'''
import sys
import os
import subprocess
import shlex
import string
import time
import traceback
from threading import Timer


   
class Logger:
   
   def __init__(self, lg_file_name=None, prefix=None, log_to_stdout=True, save_old_log_file=True, verbose_level=2):
      self.write_exception_count = 0
      self.flush_exception_count = 0
      if save_old_log_file:
         if os.path.exists(lg_file_name + '.log'):
            i = 1
            while os.path.exists(lg_file_name + '_%.2d.log' % (i)):
               i += 1
            os.rename(lg_file_name + '.log', lg_file_name + '_%.2d.log' % (i))
      if lg_file_name:
         self.log_file = open(lg_file_name + '.log', 'a+' , 0)
      else:
         self.log_file = None
      self.prefix = prefix
      self.log_to_stdout = log_to_stdout
      self.timer = None
      self.verbose_level = verbose_level
      self.start_time = {}
      # Override the default sys.excepthook to log uncaught exceptions
      sys.excepthook = self.log_uncaught_exceptions

   def __flush (self, cb_kargs):
      try:
         if self.log_file:
            self.log_file.flush()
      except IOError:
         # Ignore this error.  This is disappointing, but shouldn't
         # crash the caller.
         self.flush_exception_count += 1

   def log(self, logstr, verbose=1, log_to_stdout=True):
      try:
         if self.verbose_level >= verbose:
            logstr = logstr + '\n'
            if self.prefix:
               logstr = self.prefix + logstr
            logstr = "[" + time.strftime("%H:%M:%S") + "] - " + logstr            
            if self.log_file:
               self.log_file.write(logstr)
               self.timer = Timer(5.0, self.__flush, args=[self])
               self.timer.start()
            if self.log_to_stdout and log_to_stdout:
               sys.stdout.write(logstr)
            if self.timer:
               self.timer.cancel()
      except IOError:
         # Ignore this error.  This is disappointing, but shouldn't
         # crash the caller.
         self.write_exception_count += 1


   def log_exec (self, cmd, shell=False, verbose=1, capture_output=False):
      if type(cmd) is not list:
         self.log('Executing: ' + cmd)
         cmd = shlex.split(cmd)
      else:
         self.log('Executing: ' + ' '.join(cmd))
      
      task = subprocess.Popen(cmd, shell=shell, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
      captured_output = ''
      c = task.stdout.read(1)
      while c:
         if capture_output:
            captured_output += c
         if self.verbose_level >= verbose:
            if self.log_file:
               self.log_file.write(c)
            if self.log_to_stdout:
               sys.stdout.write(c)
            c = task.stdout.read(1)
      task.wait()  # Wait for the task to really complete
      if task.returncode != 0:
         self.log(cmd[0] + " command returned error: " + str(task.returncode), verbose)
      if capture_output:
         return [task.returncode, captured_output]
      else:
         return task.returncode

   def __delta_time (self, start, end):
      delta = end - start
      minutes = delta / 60
      seconds = delta % 60
      delta_str = "%d minutes, %d seconds" % (minutes, seconds)
      return delta_str
   # End of __delta_time()

   def mark_time_start (self, description):
      self.start_time[description] = time.time()

   def mark_time_end (self, description):
      end_time = time.time()
      self.log('Elapsed time for ' + description + ' was ' + 
               self.__delta_time(self.start_time[description], end_time))

   def log_highlight (self, str):
      self.log("\n")
      self.log("################################################################################")
      self.log("# " + str)
      self.log("################################################################################")

   def log_underline (self, str):
      self.log("\n")
      self.log(str)
      line = ""
      for i in str:
         line += "="
      self.log(line)

   def log_error (self, msg):
      self.log("\n\nError:\n" + msg, verbose=0)
      if self.log_file:
         self.log_file.close()
      exit(1)
   
   def log_exception (self):
      traceback.print_exc (None, self.log_file)

   def log_uncaught_exceptions(self, ex_cls, ex, tb):
      self.log("\nERROR: Uncaught Exception occurred!")
      self.log(''.join(traceback.format_tb(tb)))
      self.log('{0}: {1}'.format(ex_cls, ex))

   def __del__ (self):
      if self.log_file:
         if self.flush_exception_count > 0:
            self.log_file.write(str(self.flush_exception_count) + " flush exceptions occurred")
         if self.write_exception_count > 0:
            self.log_file.write(str(self.write_exception_count) + " write exceptions occurred")
         self.log_file.close()
