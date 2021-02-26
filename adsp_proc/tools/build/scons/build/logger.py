#===============================================================================
#
# logger
#
# GENERAL DESCRIPTION
#    Library to provide logging services to build scripts.
#
# Copyright (c) 2011-2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/scons.qc/1.1/build/logger.py#5 $
#  $DateTime: 2014/12/10 15:44:17 $
#  $Change: 7115037 $
#
#===============================================================================

'''
Description:
This is a library and all logging functions would be a part of this script. 
This module would be imported from all modules 
needing to log the necessary information
'''

import sys
import os
import subprocess
import time
import traceback
from threading import Timer

VERBOSE_LEVEL = 2
   
class Logger:
    '''Class Logger''' 
    def __init__(self, lg_file_name, prefix=None, log_to_stdout=True, \
        resume=False):
        '''init'''
        self.write_exception_count = 0
        self.flush_exception_count = 0
        if resume:
            access_mode = 'a'
        else:
            access_mode = 'w'
            if os.path.exists(lg_file_name + '.log'):
                i = 1
                while os.path.exists(lg_file_name + '_%.2d.log' % (i)):
                    i += 1
                os.rename(lg_file_name + '.log', 
                    lg_file_name + '_%.2d.log' % (i))
        self.log_file = open(lg_file_name + '.log', access_mode, 0)
        self.prefix_stack = [prefix]
        self.log_to_stdout = log_to_stdout
        self.timer = None
        self.start_time = {}

    def push_prefix (self, new_prefix):
        '''push prefix'''
        self.prefix_stack.append(new_prefix)
	  
    def pop_prefix (self):
        '''pop prefix'''
        self.prefix_stack.pop()

    def __flush (self, cb_kargs):
        '''flush'''
        try:
            if self.log_file:
                self.log_file.flush()
        except IOError:
            # Ignore this error.  This is disappointing, but shouldn't
            # crash the caller.
            self.flush_exception_count += 1

    def log(self, string, verbose=1, log_to_stdout=True):
        '''log'''
        try:
            if VERBOSE_LEVEL >= verbose:
                string = string + '\n'
                if self.prefix_stack[0]:
                    string = self.prefix_stack[-1] + string
                if self.log_file:
                    self.log_file.write(string)
                if self.log_to_stdout and log_to_stdout:
                    sys.stdout.write(string)
                if self.timer:
                    self.timer.cancel()
                self.timer = Timer(5.0, self.__flush, args=[self])
                self.timer.start()
        except IOError:
            # Ignore this error.  This is disappointing, but shouldn't
            # crash the caller.
            self.write_exception_count += 1


    def log_exec (self, cmd, shell=False, stdin = sys.stdin, env=None, \
        verbose=1, capture_output = False):
        '''log exec'''

        if type(cmd) == list:
            cmd_str = ' '.join(cmd)
        else:
            cmd_str = cmd

        self.log('Executing: ' + cmd_str)

        task = subprocess.Popen(cmd, shell=shell, stderr=subprocess.STDOUT, \
            stdout=subprocess.PIPE, stdin=stdin, env=env)
        captured_output = ''
        char = task.stdout.read(1)
        while char:
            if capture_output:
                captured_output += char
            if VERBOSE_LEVEL >= verbose:
                if self.log_file:
                    self.log_file.write(char)
                if self.log_to_stdout:
                    sys.stdout.write(char)
                char = task.stdout.read(1)
        task.wait()  # Wait for the task to really complete
        if task.returncode != 0:
            cmd_name = cmd_str.split()[0]
            self.log(cmd_name + " command returned error: " +\
                str(task.returncode) + '\n' + \
                "  Full command: " + cmd_str, verbose)
        if capture_output:
            return [task.returncode, captured_output]
        else:
            return task.returncode
   
    def __delta_time (self, start, end):
        '''delta time'''
        delta = end - start
        minutes = delta / 60
        seconds = delta % 60
        delta_str = "%d minutes, %d seconds" % (minutes, seconds)
        return delta_str
    # End of __delta_time()

    def mark_time_start (self, description):
        '''mark time start'''
        self.start_time[description] = time.time()
        self.log('Starting: "' + description + '"')

    def mark_time_end (self, description):
        '''mark time end'''
        end_time = time.time()
        self.log('Elapsed time for "' + description + '" was ' +
            self.__delta_time(self.start_time[description], end_time))

    def log_error (self, msg):
        '''log error'''
        self.log("Error: " + msg, verbose = 0)
        if self.log_file:
            self.log_file.close()
        exit(1)

    def log_crash (self):
        '''log crash'''
        if self.log_file:
            traceback.print_exc(None, self.log_file)

    def close (self):
        '''close'''
        if self.log_file:
            if self.flush_exception_count > 0:
                self.log_file.write(str(self.flush_exception_count) + \
                    " flush exceptions occurred")
            if self.write_exception_count > 0:
                self.log_file.write(str(self.write_exception_count) + \
                    " write exceptions occurred")
            self.log_file.close()
            self.log_file = None
      
    def __del__ (self):
        '''del'''
        self.close()
