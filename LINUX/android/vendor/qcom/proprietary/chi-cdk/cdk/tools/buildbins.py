#!/usr/bin/python2.7
"""
ModuleName:        BuildAndGenerateBins

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

DESCRIPTION:

    Build and genarate the binaries required for tuning binaries

"""

# ==============================================================================
# Import python libraries
import imp
import argparse
import os
import platform
import shutil
import subprocess
import sys
import time
import yaml


# ==========================================================================
# Import local modules


# ==========================================================================
# Static Variables
fileDescription = 'Runs all auto-generate steps for bin file generation'
version         = '1.1'


# =================================================================================
# Todo: Make a base class with logging etc?
class CBuildAndGenerateBins(object):
    """

        Makes bins

        TODO:example

        """

    # --------------------------------------------------------------------------
    def __init__(self, chicdkroot, args):
        """
        Initializes the CBuildAndGenerateBins object.

        CBuildAndGenerateBins(r'/my/path/to/camx')

        Possible Exceptions Thrown:

        """
        self._ChiCdkRoot = chicdkroot

        # Extract command line arguments related to this class
        self._Force          = args.force;
        self._CheckDepsOnly  = args.check_deps_only
        self._Verbose        = args.verbose
        self._OutputPaths    = args.bin_path
        self._Debug          = args.debug
        self._DoOnce         = 1

        self._NeedsUpdate = False

        if self._ChiCdkRoot is None or not isinstance(self._ChiCdkRoot, basestring):
            raise ValueError("Chi-CDK directory must not be None and must be of type \ "
                                                  "basestring. type found (%s)" % (type(self._ChiCdkRoot)))

        if not os.path.isdir(self._ChiCdkRoot):
            raise ValueError("Chi-CDK directory must be a valid directory accessible by the host system.")

        if 'Linux' == platform.system():
            progDir = "linux64"
            exeName = "ParameterParser"
        else:
            progDir = "win32"
            exeName = "ParameterParser.exe"

        self._ProgramPath = os.path.join(self._ChiCdkRoot, 'cdk', 'tools', progDir)

        self._Program = os.path.join(self._ProgramPath, exeName)
        if not self._OutputPaths:
            outputPathList = [os.path.join(self._ChiCdkRoot, 'vendor', 'bin')]
        else:
            outputPathList = [outputPath.strip() for outputPath in self._OutputPaths.split(',')]

        yamlPath = os.path.join(self._ChiCdkRoot, 'cdk', 'tools', 'buildbins.yaml')
        with open(yamlPath, 'r') as stream:
            self._Configuration = yaml.load(stream)

        for outputPath in outputPathList:
            if not os.path.exists(outputPath) and not os.path.isdir(outputPath):
                os.makedirs(outputPath)
            # We'll change directories soon, so record OutputPath as absolute
            outputPath = os.path.abspath(outputPath)

        self._OutputPathList = outputPathList;

        # For legacy reasons, lets make all paths relative to chi-cdk's vendor path
        os.chdir(os.path.join(chiCdkRoot, 'vendor'))

        self._GENERATORS = {
            'ChromatixXSD': {
                'args': [self._ChiCdkRoot + '/cdk/generated/g_chromatix',
                         's',
                         '-lts',
                         self._ChiCdkRoot + '/cdk/chromatix'],
                'inputs': [self._ChiCdkRoot + '/cdk/chromatix'],
                'outputs': [self._ChiCdkRoot + '/cdk/generated/g_chromatix']
            },
            'ParserUtils': {
                'args': [self._ChiCdkRoot + '/cdk/generated/g_parser',
                         'u',
                         '-l',
                         self._ChiCdkRoot + '/cdk/chromatix/XSD/ParameterTuningTypes.xsd'],
                'inputs': [self._ChiCdkRoot + '/cdk/chromatix/XSD/ParameterTuningTypes.xsd'],
                'outputs': [self._ChiCdkRoot + '/cdk/generated/g_parser']
            },
            'SensorXSD': {
                'args': [self._ChiCdkRoot + '/cdk/generated/g_sensor',
                         's',
                         '-pImageSensorModule',
                         '-nCamX',
                         '-l',
                         self._ChiCdkRoot + '/cdk/sensor'],
                'inputs': [self._ChiCdkRoot + '/cdk/sensor'],
                'outputs': [self._ChiCdkRoot + '/cdk/generated/g_sensor']
            },
            'FaceDetection': {
                'args': [self._ChiCdkRoot + '/cdk/generated/g_facedetection',
                         's',
                         '-pfd',
                         '-l',
                         self._ChiCdkRoot + '/cdk/fd'],
                'inputs': [self._ChiCdkRoot + '/cdk/fd'],
                'outputs': [self._ChiCdkRoot + '/cdk/generated/g_facedetection']
            }
        }

    # --------------------------------------------------------------------------
    def needs_update(self):
       """
       Called after invoking build_bins and generate_bins to see if anything needed update.

       When the software is invoked with --check-deps-only, binaries aren't build and
       source isn't generated, but this value indicates if they would have been without
       the --check-deps-only flag.
       """

       return self._NeedsUpdate

    # --------------------------------------------------------------------------
    def build_bins(self):
        """
        Builds the binaries

        CBuildAndGenerateBins()

        Possible Exceptions Thrown:

        """

        for binType, bins in self._Configuration.items():
            if self._Verbose:
                 print('Processing ' + binType)
            outputFile = "*NONE*"
            inputFile  = "*NONE*"
            for items in bins:
                latestInput = 0
                outputTime = 0
                for name, files in items.items():
                    output = os.path.realpath(os.path.join(self._OutputPathList[0], name + ".bin"))
                    if self._Verbose:
                         print('Processing ' + name)

                    if not self._Force:
                        # Determine the latest modification time of the input files
                        for currentfile in files:
                            currentfile = os.path.normpath(currentfile)
                            if os.path.isfile(currentfile):
                                mtime = os.stat(currentfile).st_mtime
                                if mtime > latestInput:
                                    latestInput = mtime
                                    inputFile = currentfile
                            elif os.path.isdir(currentfile):
                                for root, dirs, dirfiles in os.walk(currentfile):
                                    for filename in dirfiles:
                                        mtime = os.stat(os.path.join(root, filename)).st_mtime
                                        if mtime > latestInput:
                                            latestInput = mtime
                                            inputFile = filename
                            else:
                                raise ValueError("File %s not found" % currentfile)

                        # Determine the modification of the .bin file
                        if os.path.isfile(output):
                            mtime = os.stat(output).st_mtime
                            if mtime > outputTime:
                                outputTime = mtime
                                outputFile = output
                    else:
                        # Fake a requirement to update
                        outputTime = 0
                        latestInput = 1

                    # If we just want to know if things are out of date, stop here
                    if latestInput > outputTime:
                        self._NeedsUpdate = True
                        if self._Debug:
                            print('Newest input: {} ({})\n'.format(inputFile, latestInput))
                            print('Oldest output: {} ({})\n'.format(outputFile, outputTime))

                    # Invoke the chromatix tool to build the .bin
                    if not self._CheckDepsOnly and (self._Force or (latestInput > outputTime)):
                        if self._Verbose:
                            print('Building ' + binType + ' ' + output)
                        if 'chromatix_projects' == binType:
                            args = [output] + ['p'] + files
                        else:
                            args = [output] + ['b'] + files
                        if not self._Verbose:
                            args += ['-q']
                        procArgs = [self._Program] + args
                        proc = subprocess.Popen(procArgs)
                        proc.communicate()
                        if proc.returncode != 0:
                            print "***** PROCESS EXITED WITH CODE: %d *****" % proc.returncode
                            print "Arguments: %s" % procArgs
                    elif not self._CheckDepsOnly and self._Verbose:
                        print('\t... up to date')

                    # Copy to each other output path
                    if not self._CheckDepsOnly:
                        for outputPath in self._OutputPathList[1:]:
                            copy = os.path.realpath(os.path.join(outputPath, name + ".bin"))
                            shutil.copy2(output, copy)

    def generate_bins(self):
        """
        Generates the binaries

        generate_bins()

        Possible Exceptions Thrown:

        """
        for name, generator in self._GENERATORS.iteritems():
            if self._Verbose:
                print 'Processing ' + name

            earliestOutput = 0
            earliestName = "*NONE*"
            latestInput = 0
            latestName = "*NONE*"

            # Find the latest input
            if 'inputs' in generator:
                for file in generator['inputs']:
                    if os.path.isfile(file):
                        mtime = os.stat(file).st_mtime
                        if mtime > latestInput:
                            latestInput = mtime
                            latestName = file
                    elif os.path.isdir(file):
                        foundAtLeastOne = False
                        for root, dirs, files in os.walk(file):
                            for filename in files:
                                mtime = os.stat(os.path.join(root, filename)).st_mtime
                                if mtime > latestInput:
                                    latestInput = mtime
                                    latestName  = filename

            # Find the earliest output
            if 'outputs' in generator:
                for dirName in generator['outputs']:
                    if os.path.isdir(dirName):
                        for root, dirs, files in os.walk(dirName):
                            for filename in files:
                                if not earliestOutput:
                                    earliestOutput = os.stat(os.path.join(root, filename)).st_mtime
                                    earliestName = filename
                                else:
                                    mtime = os.stat(os.path.join(root, filename)).st_mtime
                                    if mtime < earliestOutput:
                                        earliestOutput = mtime
                                        earliestName = filename
                    else:
                        os.makedirs(dirName)
                        earliestOutput = 0
                        earliestName = "*NONE*"
                        break

            # Update the needsUpdate flag
            if self._Debug:
                print('Latest input: {} - {}'.format(latestInput, latestName))
                print('Earliest output: {} - {}'.format(earliestOutput, earliestName))

            if latestInput > earliestOutput:
                self._NeedsUpdate = True

            # Regen if necessary
            if self._Force or (not self._CheckDepsOnly and (latestInput > earliestOutput)):
                procArgs = [self._Program] + generator['args']
                if not self._Verbose:
                    procArgs += ['-q']
                sys.stdout.flush() # don't intermingle ouroutput with program's!
                if self._DoOnce:
                    self._DoOnce = 0
                    self._Force  = 1
                    buildBins.build_bins()
                    self._Force  = 0
                proc = subprocess.Popen(procArgs)
                proc.communicate()
            elif self._Verbose:
                print '\t... up to date'

if __name__ == '__main__':
    chiCdkRoot, filename = os.path.split(os.path.realpath(__file__))
    for i in range (2):
        chiCdkRoot = os.path.join(chiCdkRoot, '..')

    parser = argparse.ArgumentParser(description='Generate tuning binaries and their C++ parsers')

    parser.add_argument('--debug', help='Show debugging output (you want --verbose?)',
                        action='store_const', const=True)

    parser.add_argument('--verbose', help='Show verbose output',
                        action='store_const', const=True)

    parser.add_argument('--force', help='Ignore timestamps & always force regeneration',
                        action='store_const', const=1)

    parser.add_argument('--check-deps-only', help='Do not generate anyting. For makefile dep gen',
                         action='store_const', const=1)

    parser.add_argument('--bin-path', help='Comma separated list containing paths where compiled .bin files go',
                        action='store')

    args = parser.parse_args()

    buildBins = CBuildAndGenerateBins(chiCdkRoot, args)

    buildBins.build_bins()
    buildBins.generate_bins()

    # For special makefile mode
    if args.check_deps_only:
        if buildBins.needs_update():
            print('CAMX_DO_GEN_BUILD := true\n\n')
        else:
            print('CAMX_DO_GEN_BUILD := false\n\n')
    exit(0)
