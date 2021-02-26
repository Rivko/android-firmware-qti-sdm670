# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Apr 19, 2013

@author: hraghav
'''

from c_logging import logger
import c_misc, c_path
import os
import re
import subprocess
import threading
import time
import sys

class CoreSubprocess(object):
    """ Class to launch a process. """

    # Communication type enums
    COM_ARGS = 1
    COM_STDIN = 2

    # Stderr routing options
    STDERR_TMP = 1
    STDERR_STDOUT = 2

    class CBDefault():
        pass

    def __init__(self, launchCommand, timeout=None, callback=None,
                 workingDir=None, comm=1, commands=[], debug=False,
                 stderr=None):
        """
        Initializes internal variables.

        Parameters:
        1. launchCommand (str): Command that should be launched.
        2. timeout (int): The timeout within which the tool should finish.
        3. callback (cb): Callback to send the tool ouput on.
            Call should have the following prototype:
                def callback(buffer):
        """
        self.launchCommand = launchCommand
        self.timeout = timeout if (timeout and not debug) else None
        self.callback = callback
        self.workingDir = workingDir
        self.comm = comm
        self.commands = commands
        self.debug = debug
        self.stderr = stderr

        self._toolOutput = ''
        self._toolReturnCode = 0
        self._toolTimedout = False

    @staticmethod
    def _readEnd(process, outputCallback):
        """
        Internal method used to read the last few bits.

        Parameters:
        1. process (obj): Object returned by subprocess.
        2. outputCallback (cb): Callback to send the tool output on.
        """
        charRead = process.stdout.read()
        outputCallback(charRead)

    @staticmethod
    def _readOutput(process, outputCallback):
        """
        Internal method used to read the last few bits.

        Parameters:
        1. process (obj): Object returned by subprocess.
        2. outputCallback (cb): Callback to send the tool output on.
        """
        while True:
            if not process.poll() is None:
                break
            charRead = process.stdout.read(1)
            outputCallback(charRead)

    @staticmethod
    def waitForPrompt(process, outputCallback):
        currentLine = ''
        while True:
            charRead = process.stdout.read(1)
            outputCallback(charRead)
            currentLine += charRead
            if charRead == '\n':
                currentLine = ''
            if currentLine.lstrip() == '>':
                break

    @classmethod
    def _communicate(cls, process, communication, commands, debug, outputCallback,
                     returnCodeCallback):
        """
        Internal method used to launch sub process as a seperate thread.

        Parameters:
        1. process (obj): Object returned by subprocess.
        2. outputCallback (cb): Callback to send the tool output on.
        3. returnCodeCallback (cb): Callback to set the return code
        """

        readEndBytes = True
        if communication == cls.COM_STDIN:
            if not debug:
                commands += ['quit']
            for cmd in commands:
                cls.waitForPrompt(process, outputCallback)
                outputCallback(cmd + '\r\n')
                process.stdin.write(cmd + '\r\n')

        elif communication == cls.COM_ARGS:
            readOutput = threading.Thread(target=CoreSubprocess._readOutput,
                                          args=([process, outputCallback]))
            readOutput.daemon = True
            readOutput.start()
            while True:
                if not process.poll() is None:
                    break
                time.sleep(1)
            readOutput.join(5)
            readEndBytes = not readOutput.is_alive()

        if readEndBytes:
            readEnd = threading.Thread(target=CoreSubprocess._readEnd,
                                             args=([process, outputCallback]))
            readEnd.daemon = True
            readEnd.start()
            readEnd.join(5)

        returnCodeCallback(process.wait())

    def compressToolOutput(self, output=None, lineLimit=5000):
        """ Return compressed output limiting the number of lines """
        if output is None: output = self._toolOutput
        compressedOutput = c_misc.compressBufferContents([output])[-1 * lineLimit:]
        return '\n'.join(compressedOutput)

    def killProcess(self, process):
        if sys.platform.startswith('linux'):
            process.kill()
        else:
            #Using stdout=logger.outputStream caused error
            processToKill = subprocess.Popen('taskkill /F /T /PID {0}'.format(str(process.pid)),
                                         #stdout=logger.outputStream,
                                         stderr=subprocess.STDOUT,
                                         bufsize= -1)
            processToKill.wait()

    def launchTool(self):
        """
        Launches the tool specified by the launchCommand

        Returns:
        1. toolTimedout (bool): True if tool timedout
        2. toolReturnCode (int): Returncode from the tool
        3. toolOutput (str): Output from the tool
        """
        self._toolTimedout = False
        self._toolReturnCode = 0
        self._toolOutput = ''

        if self.stderr == self.STDERR_TMP:
            tmpfile = c_path.create_tmp_file()
            stderr = open(tmpfile, 'wb')
        elif self.stderr == self.STDERR_STDOUT:
            stderr = subprocess.STDOUT
        else:
            stderr = self.stderr

        try:
            process = subprocess.Popen(self.launchCommand, cwd=self.workingDir,
                        stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                        stderr=stderr)

            try:
                communication = threading.Thread(target=self._communicate,
                    args=([process, self.comm, self.commands, self.debug,
                           self._setOutput, self._setReturnCode]))

                communication.daemon = True
                communication.start()
                start_time = end_time = time.time()
                while True:
                    timeout = 600 if (self.timeout is None) else min(600, self.timeout - (end_time - start_time))
                    communication.join(timeout)
                    end_time = time.time()
                    if ((not communication.isAlive()) or ((self.timeout is not None) and ((end_time - start_time) > self.timeout))):
                        break
                if communication.isAlive():
                    self._toolTimedout = True
                    self.killProcess(process)
                    communication.join(2)
                    assert communication.isAlive() == False

            except KeyboardInterrupt:
                self.killProcess(process)
                raise
        finally:
            if self.stderr == self.STDERR_TMP:
                stderr.close()
                c_path.remove_tmp_file(tmpfile)

        return self._toolTimedout, self._toolReturnCode, self._toolOutput

    def _setReturnCode(self, returnCode):
        """ Internal method used to set the tool returnCode """
        self._toolReturnCode = returnCode

    def _setOutput(self, output):
        """ Internal method used to set the tool output and callback """
        self._toolOutput += output
        if self.callback: self.callback(output)

    # This is a hook for caller to format the command
    # line string for printing
    def formatLaunchCommandForPrinting(self, cmd):
        return cmd

    def printCommand(self):
        cmd = self.launchCommand
        cmd = self.formatLaunchCommandForPrinting(cmd)
        location = os.path.dirname(cmd[0])
        args = ''
        curLength = 0
        for eachArg in cmd[1:]:
            if curLength > 60:
                args += '\n'
                curLength = 0
            args += eachArg + ' '
            curLength += len(eachArg) + 1
        cmd = os.path.basename(cmd[0])
        logger.info(
"""Launching tool: {0}
    From: {1}
    Args: {2}""".format(cmd, location, '\n          '.join(args.split('\n'))))

    def printFinish(self, cmd):
        logger.info('Finished tool: {0}'.format(cmd))

    def validateOutput(self, retcode, f_retcode, f_output,
                             successRegex, failureRegex,
                             successString, failureString,
                             cmd):
        returnValue = True
        returnError = 'Tool "{0}"\n'.format(cmd)

        # Perform validations
        if retcode is not None and retcode != f_retcode:
            returnError += '    Return code does not match: Expected = "{0}", Got = "{1}"'.format(retcode, f_retcode)
            returnValue = False

        if successRegex is not None and not re.search(r'{0}'.format(successRegex), f_output):
            returnError += '    Output does not have expected success regex.\n    Regex: "{0}"'.format(successRegex)
            returnValue = False
        elif failureRegex is not None and re.search(r'{0}'.format(failureRegex), f_output):
            returnError += '    Output has expected failure regex.\n    Regex: "{0}"'.format(failureRegex)
            returnValue = False
        elif successString is not None and f_output.find(successString) == -1:
            returnError += '    Output does not have expected success string.\n    Expected String: "{0}"'.format(successString)
            returnValue = False
        elif failureString is not None and not f_output.find(failureString) == -1:
            returnError += '    Output has expected failure string.\n    Expected String: "{0}"'.format(failureString)
            returnValue = False

        return returnValue, returnError

    @classmethod
    def executeCommand(cls, launchCommand, retcode=None, callback=None,
                       timeout=3600, successRegex=None, failureRegex=None,
                       successString=None, failureString=None, workingDir=None,
                       comm=1, commands=[], debug=False, stderr=STDERR_TMP,
                       print_command=False):
        """
        Launches the tool based on the params
        Only one of 5, 6, 7, 8 must be given

        Parameters:
        1. launchCommand (str): command to be executed.
        2. retcode (int): Expected return code from tool.
        3. callback (cb): Callback to take tool output
            Callback should have the following prototype:
                def callback(buffer):
        4. timeout (int): Time in which tool should finish
        5. successRegex (str): Regex in output stream signifying success.
        6. failureRegex (str): Regex in output stream signifying failure.
        7. successString (str): String in output stream signifying success.
        8. failureString (str): String in output stream signifying failure.

        Returns:
        1. returnValue (bool): True if all validations were successful
        2. returnError (str): Failure if any
        """

        if len([value for value in [successRegex, failureRegex, successString, failureString] if value is not None]) > 1:
            raise AttributeError('Only one of successRegex, failureRegex, successString, failureString must be given')

        # Log the running process nicely
        if type(launchCommand) is not list:
            cmd = launchCommand.split()
        else:
            cmd = launchCommand

        # Setup default callback
        cb = callback
        if callback is cls.CBDefault:
            cb = lambda msg: logger.info(msg, raw=True)
            cb('\n---------------------------------------------------------------\n\n')

        # Launch the tool
        tool = cls(launchCommand, timeout=timeout,
                   workingDir=workingDir, comm=comm, commands=commands, debug=debug,
                   callback=cb, stderr=stderr)

        if print_command:
            tool.printCommand()
        f_timeout, f_retcode, f_output = tool.launchTool()

        if callback is cls.CBDefault:
            cb('\n---------------------------------------------------------------\n\n')

        if print_command:
            tool.printFinish(cmd)

        returnValue, returnError = tool.validateOutput(
                                         retcode, f_retcode, f_output,
                                         successRegex, failureRegex,
                                         successString, failureString,
                                         cmd)

        return returnValue, returnError, f_timeout, f_retcode, f_output

    @classmethod
    def simpleExecuteCommand(cls, launchCommand):
        """
        Launches the tool based on the params

        Parameters:
        1. launchCommand (str): command to be executed.

        Returns:
        1. returnValue (bool): True if all validations were successful
        2. returnError (str): Failure if any
        """
        # Log the running process nicely
        if type(launchCommand) is not list:
            launchCommand = launchCommand.split()

        # Launch the tool
        retcode, output, error = 0, '', ''
        try:
            process = subprocess.Popen(launchCommand, stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE)
            process.wait()
            retcode = process.returncode
            output = process.stdout.read()
            error = process.stderr.read()
        except Exception as e:
            retcode = -1
            error = str(e)

        return retcode, output, error
