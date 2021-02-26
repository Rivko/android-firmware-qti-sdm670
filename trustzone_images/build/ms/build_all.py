
#===============================================================================
#
# SSG Build system
#
# Copyright (c) 2015-2017 Qualcomm Technologies, Inc. All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
#===============================================================================
'''
    Created on Jan. 14, 2016
    @authors: c_jacke, lenhardw

    Python Script for creating Builds

    Syntax: run with --help for syntax

'''

import logging

VERBOSE = 5
logging.basicConfig()
logger = logging.getLogger("build_all")
logger.setLevel(logging.INFO)

import glob
import optparse
import os
import platform
import re
import signal
import shutil
import socket
import sys
import time
import traceback

from subprocess import call
from xml.dom import minidom
from xml.etree import ElementTree

import BuildConfigParser

try:
    import cleanpack_tz2 as cleanpack_tz # FIXME rename file to cleanpack_tz
except ImportError:
    cleanpack_tz = None # pylint: disable=C0103

# directory containing this script
EXEC_DIR = os.path.dirname(os.path.realpath(__file__))

# Flags
FORBID_UNDOCUMENTED_ARTIFACTS = False
KEEP_GOING = False

# TODO - move these to XML config as well if possible
# root of the TZ build (two levels up from this script)
BUILD_ROOT = os.path.abspath(os.path.join(EXEC_DIR, os.pardir, os.pardir))
PACKAGE_ROOT = os.path.abspath(os.path.join(BUILD_ROOT, os.pardir))
TOOLS_SCONS_PATH = os.path.join("tools", "build", "scons")
SCONS_BUILD_PATH = "build"
BUILD_PATH = os.path.join("build", "ms")
LOGFILE_ROOT = os.path.join(BUILD_ROOT, BUILD_PATH)
TEMP_DIR = os.path.join(PACKAGE_ROOT, "temp")
CONFIG_FILES = [
    os.path.join(LOGFILE_ROOT, "build_config.xml"),
    os.path.join(LOGFILE_ROOT, "build_config_deploy.xml"),
]
REPORT_GENERATOR_SCRIPT = os.path.join(BUILD_ROOT, TOOLS_SCONS_PATH, \
        "linker_reporter", "report_generator.py")
REPORT_GENERATOR_PATH = os.path.join("linker_reporter", "report_generator.py")

MANIFEST_FILE = os.path.join(BUILD_ROOT, "build", "manifest.xml")
BUILD_LOG_PREFIX = "build-log"
UNIFIED_LOG = "LOGFILE"
COV_FILE_NAME = "core_tz_bullseye.cov"
COV_LOG_FILE = "core_tz_bullseye.log"

DEFAULT_ARGS = [
    "-f",
    "target.scons",
]

CLEANPACK_ARGS = [
    "-c",
    "--implicit-deps-unchanged",
    "--cleanpack",
]

CLEANPACK_COPY_TARGETS = [
    os.path.join("trustzone_images", "core", "securemsm", "trustzone", \
            "monitor"),
    os.path.join("trustzone_images", "core", "securemsm", "trustzone", "qsee"),
]

# TODO - clean these directory references up somehow
HK11_DIR = os.path.join(PACKAGE_ROOT, "HK11")
HY11_DIR = os.path.join(PACKAGE_ROOT, "HY11_1")

DEFAULT_MAPREPORT = False
DEFAULT_OS = "default-os"

CRM_CLIENT_LOG = "crm_client.log"
CRM_INFO_FILE = "crm_info.txt"

ENVIRONMENT_DUMP_SHELL = "envdump.sh"
ENVIRONMENT_DUMP_BATCH = "envdump.cmd"

SHORT_DELAY = 1
MEDIUM_DELAY = 2
LONG_DELAY = 5
DEQUEUE_RETRY_DELAY = 2
WORKER_WAIT_TIME = 60
FAST_BUILD = False
SEPARATOR = 40 * "-"
CLEAN_FLAG = "-c"

# worker response types
WORKER_SUCCESS = 0
WORKER_FAILURE = 1
WORKER_EXITED = 2
WORKER_EXCEPTION = 3

class ArtifactNotFoundException(IOError):
    """Custom exception thrown for missing build artifacts"""
    pass

# error codes
# TODO: for now, every line using one of these has a pylint suppression comment.
# However, this mechanism should be redesigned to allow proper checking of
# variable definitions as bugs could sneak past the checker.
ERROR_CODES = {
    "ERROR_OK" : 0,
    "ERROR_INVALID_ARG" : -1,
    "ERROR_BAD_CONFIG" : -2,
    "ERROR_INVALID_ARTIFACT" : -3,
    "ERROR_SYS_ERR" : -4,
    "ERROR_BAD_PATH" : -5,
    "ERROR_WORKER_EXCEPTION" : -6,
    "ERROR_INTERRUPTED" : -7,
    "ERROR_UNKNOWN" : -8,
}

def _setErrors(mapping):
    """Translate ERROR_CODES into usable variables"""
    _globals = globals()
    for key in mapping:
        val = mapping[key]
        _globals[key] = val
        for key2 in mapping:
            if key == key2:
                continue
            assert mapping[key2] != val, \
                    "%s & %s cannot have the same value" % (key, key2)

_setErrors(ERROR_CODES)

def strerr(err):
    """Return human-readable strings for error codes"""
    for key in ERROR_CODES:
        if ERROR_CODES[key] == err:
            return key
    logger.debug("%d is not a recognized error code" % err)
    return str(err)

def buildAll():
    """main build function"""
    # TODO - @redesign to reduce returns, branches, local vars & statements
    #pylint: disable=R0911, R0912, R0914, R0915
    startTime = time.time()
    (options, args) = parseArguments()

    if options.verbose:
        if logger.getEffectiveLevel() > logging.DEBUG:
            logger.setLevel(logging.DEBUG)
        DEFAULT_ARGS.append("--verbose=2")

    global FORBID_UNDOCUMENTED_ARTIFACTS #pylint: disable=W0603
    global KEEP_GOING # pylint: disable=W0603

    if options.keep_going:
        KEEP_GOING = True

    if options.fast_build:
        global FAST_BUILD #pylint: disable=W0603
        FAST_BUILD = True

    cleanBuild = options.clean or options.clean_build
    makeBuild = not options.clean

    # change to build/ms directory for config stage
    os.chdir(LOGFILE_ROOT)

    # load the XML config
    config = BuildConfigParser.loadXml(options.config_file)
    if len(config) == 0:
        logger.critical("failed to load %s" % options.config_file)
        return ERROR_BAD_CONFIG #pylint: disable=E0602

    if options.detect_branch:
        branch = detectBranch(config)
        if branch != None:
            print(branch)
            return ERROR_OK #pylint: disable=E0602
        else:
            logger.error("Could not auto-detect branch")
            return ERROR_INVALID_ARG #pylint: disable=E0602

    # get a mapping of branch aliases to actual branch names
    aliasMap = BuildConfigParser.loadAliasMap(config)

    autoSelectedBranch = False
    if options.branch == None:
        branch = detectAndLogBranch(config)
        if branch == None:
            # we are guaranteed to have at least one non-virtual branch in xml
            productionBranches = []
            for branchName in config:
                if config[branchName]["virtual"]:
                    continue
                # return an actual branch, not an alias
                if branchName not in aliasMap:
                    productionBranches.append(branchName)
            productionBranches.sort()
            availableBranches = "Available branches are:\n%s" \
                    % "\n".join(productionBranches)
            logger.critical( \
                    #pylint: disable=C0301
                    "Could not detect branch. Please pass it explicitly:\nEg: -b %s\n\n%s" \
                    % (productionBranches[0], availableBranches))
            return ERROR_INVALID_ARG #pylint: disable=E0602
        autoSelectedBranch = True
    else:
        branch = options.branch
        logger.debug("Branch '%s' explicitly passed via command line" % branch)

    branchConfig = config.get(branch)
    if branchConfig == None:
        logger.critical("Branch '%s' not found in %s" \
                % (branch, options.config_file))
        return ERROR_BAD_CONFIG #pylint: disable=E0602
    if branchConfig["virtual"] == True:
        logger.critical( \
                "Branch '%s' is virtual (non-production) and cannot be built" \
                % branch)
        return ERROR_BAD_CONFIG #pylint: disable=E0602

    # We have a legitimate branch; now translate any alias to its real name
    rawBranch = branch
    branch = aliasMap.get(branch, rawBranch)
    if rawBranch != branch:
        logger.debug("Translate branch alias '%s' --> '%s'" \
                % (rawBranch, branch))

    # Now that we have our config & branch, parse any leftover arguments to
    # establish additional params (eg X=Y), target files (eg tz), or pass-though
    # args (anything leftover after the others)
    buildInput = parseExtraArguments(args, branchConfig)
    cmdParams = buildInput["params"]

    # set software image for use by SConscripts
    cmdParams["SOFTWARE_IMAGE"] = branch

    if options.scons_args:
        buildInput["args"].append(options.scons_args)

    # IMPORTANT NOTE:
    # While 'chipset' & 'target' may appear overloaded, there is a distinct
    # difference: 'target' represents a buildable target name, as represented in
    # build_config.xml by <chipset> tags. A target's 'chipset' is USUALLY but NOT
    # ALWAYS its name. Differences may occur for special targets like
    # pre-silicon build targets (eg rumi/virtio) or targets with binary
    # compatibility (eg msm8976 uses the 'msm8956' chipset).
    # NOTE: <chipset> tags used to be (legacy) <target> tags, but were changed
    #       to reduce confusion
    #
    # This difference is abstracted away from the user via the CHIPSET param
    # which actually references a 'target', letting the build script perform any
    # necessary translation. Errors to the user will always reflect 'chipset'.
    targets = []
    target = cmdParams.get("CHIPSET")
    autoSelectedTarget = False
    if target == None:
        if options.use_default_target:
            defaultTarget = branchConfig["default_target"]
            if defaultTarget == None:
                logger.critical( \
                        #pylint: disable=C0301
                        "No default target found for branch '%s' - must provide a valid chipset (eg CHIPSET=%s)" \
                        % defaultTarget)
                return ERROR_INVALID_ARG #pylint: disable=E0602
            targets = [ defaultTarget ]
        else:
            # build all valid targets for this branch
            for target in branchConfig["targets"]:
                targetInfo = branchConfig["targets"][target]
                if targetInfo["buildable"] and not targetInfo["exclusive"]:
                    targets.append(target)
            #pylint: disable=C0301
            assert len(targets) > 0, \
                    "%s is invalid - no buildable targets listed for branch %s" \
                    % (options.config_file, branch)
        autoSelectedTarget = True
    else:
        targets = target.split(",")
        logger.debug( \
                "Chipset(s) explicitly passed via command line: {}".format( \
                ", ".join(targets)))
        if options.use_default_target:
            logger.critical( \
                    "Cannot both specify a chipset & request the default")
            return ERROR_INVALID_ARG #pylint: disable=E0602
        # Now, remove the 'CHIPSET' param so that the given target gets properly
        # translated to a chipset later via our build config
        del cmdParams["CHIPSET"]
    targets.sort() # sort the targets for more consistent build behavior
    del target # we no longer need this & redefine it immediately below

    # validate selected targets
    for target in targets:
        targetConfig = branchConfig["targets"].get(target)
        if targetConfig == None:
            logger.critical("Chipset '%s' not defined in %s" \
                    % (target, options.config_file))
            return ERROR_BAD_CONFIG #pylint: disable=E0602
        if targetConfig["buildable"] == False:
            logger.critical("Chipset '%s' is not a buildable target" % target)
            return ERROR_BAD_CONFIG #pylint: disable=E0602
        if targetConfig["exclusive"] == True and len(targets) > 1:
            logger.critical( \
                    "Chipset '%s' cannot be built with other targets" % target)
            return ERROR_BAD_CONFIG #pylint: disable=E0602
        if targetConfig["chipset"] != target:
            logger.debug("Target '%s' maps to chipset '%s'" \
                    % (target, targetConfig["chipset"]))

    # if files were specified via the command line, verify that they apply
    # to at least one of the selected targets
    for f in buildInput["files"]:
        valid = False
        for target in targets:
            targetConfig = branchConfig["targets"].get(target)
            fileMeta = targetConfig["files"].get(f)
            if fileMeta != None:
                valid = True
                if fileMeta["disable"]:
                    logger.warn( \
                            #pylint: disable=C0301
                            "'{}' (disabled in {} {} config) is explicitly enabled by the user".format( \
                            f, target, branch))
            else:
                logger.debug( \
                        "'{}' is not supported for {} on branch {}".format( \
                        f, target, branch))
        if not valid:
            logger.critical( \
                    # pylint: disable=C0301
                    "'{}' is not supported for any chipset ({}) on branch {}".format( \
                    f, "/".join(targets), branch))
            return ERROR_BAD_CONFIG #pylint: disable=E0602

    addCustomBuildTargets(options, branchConfig, buildInput)

    if options.list_targets:
        print(", ".join(targets))
        return ERROR_OK #pylint: disable=E0602

    osType = getOsType()
    setupEnv(targetConfig, osType, options.dump_environment)

    setStaticAnalysisEnvironment(options.enable_static_analysis, buildInput)

    if options.code_coverage:
        setCodeCoverageEnvironment()

    if checkEnv(osType) != 0:
        logger.critical( \
                #pylint: disable=C0301
                "Environment settings are incorrect - please see previous error message. Exiting ...")
        return ERROR_BAD_CONFIG #pylint: disable=E0602

    loadableParamNames = [
        "BUILD_ASIC",
        "BUILD_ID",
        "BUILD_VER",
        "CHIPSET", # handles the translation from target -> chipset
        "HAL_PLATFORM",
        "MAX_NUM_ROOT_CERTS",
        "MSM_ID",
        "PBL_VERSION",
        "TARGET_FAMILY",
        "SOC_HW_VERSION",
        "SOC_VERS",
        "TZ_EXEC_MODE",
        "SECIMAGE_CONFIG",
    ]

    targetCount = len(targets)
    if targetCount > 1:
        for paramName in loadableParamNames:
            if cmdParams.get(paramName) != None:
                logger.critical( \
                        #pylint: disable=C0301
                        "Cannot use custom {} ({}) when building multiple chipsets ({})" \
                        % (paramName, cmdParams.get(paramName), \
                        ", ".join(targets)))
                return ERROR_INVALID_ARG #pylint: disable=E0602

    os.chdir(BUILD_ROOT) # ensure that we start in the BUILD_ROOT directory
    printDiagnosticInfo(branch, targets, options, buildInput, \
            autoSelectedBranch)
    # The user is relying on some default arguments & auto-detection and may not
    # expect the config that has been chosen. Pause to give them a chance to
    # identify any errors and cancel the build.
    if (autoSelectedBranch or autoSelectedTarget) and not FAST_BUILD:
        time.sleep(LONG_DELAY)

    # create the manifest after the delay (in case the user aborts)
    if createManifestIfNeeded(branch):
        logger.debug("Generated updated manifest file")

    # TODO - add logic forbidding combinations of some flags, such as clean +
    # recompile only or cleanpack + clean-and-build
    retCode = ERROR_OK #pylint: disable=E0602

    # pylint: disable-msg=E0602
    assert not KEEP_GOING or ERROR_OK == 0, \
            "BUG: Error accumulator requires ERROR_OK == 0"
    errorAccumulator = ERROR_OK
    # pylint: enable-msg=E0602

    for target in targets:
        # Each target's config was already validated above
        targetConfig = branchConfig["targets"][target]
        targetParams = dict(cmdParams) # shallow copy to preserve the original
        targetArgs = list(DEFAULT_ARGS) # shallow copy
        targetArgs.extend(buildInput["args"])

        for paramName in loadableParamNames:
            if targetParams.get(paramName) == None:
                value = targetConfig.get(paramName.lower())
                logger.debug("{} = {}".format(paramName, value))
                if value != None:
                    targetParams[paramName] = value

        printParams(targetParams, "build")

        # An empty list evaluates to false; fileList becomes None in this case
        # TODO - need to ensure that all dependencies are also added & that they
        # are in an acceptable order, although since SCons handles most of this,
        # it is a minor problem for now
        fileList = buildInput["files"] or None

        if options.test_mode:
            if cleanBuild or options.recompile_only or options.clean_pack \
                    or not makeBuild:
                logger.error("Test mode can only be used with a normal build")
                return ERROR_INVALID_ARG #pylint: disable=E0602
            # force full artifact verification
            FORBID_UNDOCUMENTED_ARTIFACTS = True

        if not FAST_BUILD:
            time.sleep(MEDIUM_DELAY)

        if cleanBuild:
            logger.info("Cleaning %s for %s" % (branch, target))
            # must maintain our original args for later, so copy them
            cleanArgs = list(targetArgs)
            cleanArgs.append(CLEAN_FLAG)
            # Don't check artifacts since clean should remove them
            retCode = buildTargetFiles(targetConfig, targetParams, cleanArgs, \
                    fileList = fileList, checkArtifacts = False)
            if retCode != ERROR_OK: #pylint: disable=E0602
                logger.error("Error encountered when cleaning %s for %s" \
                        % (branch, target))
                if not KEEP_GOING:
                    break
                else:
                    errorAccumulator |= retCode
        if not makeBuild:
            continue
        if options.recompile_only:
            logger.info("Triggering recompilation only of %s for %s" \
                    % (branch, target))
            # TODO - does recompilation need to rename the build log too?
            retCode = recompileTargetFiles(targetConfig, \
                    targetParams, targetArgs)
            if retCode != ERROR_OK: #pylint: disable=E0602
                logger.error("Error encountered in recompilation of %s for %s" \
                        % (branch, target))
                if not KEEP_GOING:
                    break
                else:
                    errorAccumulator |= retCode
            continue
        elif options.clean_pack:
            logger.info("Triggering cleanpack of %s for %s" % (branch, target))
            # TODO - does cleanpack need to rename the build log too?
            retCode = cleanpack(branch, targetConfig, targetParams, \
                    targetArgs)
            if retCode != ERROR_OK: #pylint: disable=E0602
                logger.error("Error encountered in cleanpack of %s for %s" \
                        % (branch, target))
                if not KEEP_GOING:
                    break
                else:
                    errorAccumulator |= retCode
            continue
        # Finally, we're ready to make a normal build
        retCode = buildTargetFiles(targetConfig, targetParams, targetArgs, \
                fileList = fileList, checkArtifacts = True, \
                mapreport = options.map_report, testMode = options.test_mode)
        if retCode != ERROR_OK: #pylint: disable=E0602
            logger.error("Error encountered when building %s for %s" \
                    % (branch, target))
            if not KEEP_GOING:
                break
            else:
                errorAccumulator |= retCode
    completionTime = time.time()
    elapsedTime = completionTime - startTime
    logger.info("Build %s" % ("completed successfully" if retCode == 0 \
            else "encountered errors"))
    logger.info("Total elapsed time: %d minutes, %d seconds" % \
            (elapsedTime // 60, elapsedTime % 60))
    if KEEP_GOING and errorAccumulator != ERROR_OK: # pylint: disable=E0602
        return errorAccumulator
    return retCode

def createManifestIfNeeded(branch):
    """Some files in the build tree rely on trustzone_images/build/manifest.xml
       for the branch. Generate one if it does not exist to prevent a build
       failure. Adds a custom flag indicating that the manifest was created
       by build_all.py so that we can re-create it if the branch changes."""
    if checkManifest(MANIFEST_FILE, checkCreator = True):
        # manifest exists and is valid as-is
        return False
    # Generate actual XML
    configXml = ElementTree.Element("config")
    imageTreeXml = ElementTree.SubElement(configXml, "image_tree")
    nameXml = ElementTree.SubElement(imageTreeXml, "name")
    nameXml.text = branch
    revisionXml = ElementTree.SubElement(imageTreeXml, "revision")
    revisionXml.text = "00000"
    creatorXml = ElementTree.SubElement(configXml, "creator")
    creatorXml.set("name", os.path.realpath(__file__))
     # Write results to file
    try:
        outfile = open(MANIFEST_FILE, "w")
    except (IOError, OSError) as e:
        logger.warn("[%s] failed to write to %s" % (str(e), MANIFEST_FILE))
        assert os.path.exists(MANIFEST_FILE), \
                "Could not create manifest - build will fail if we continue"
        return False # if it already exists, just hope for the best
    declaration = minidom.Document().toxml()
    reparsedXML = minidom.parseString(ElementTree.tostring(configXml, "utf-8"))
    outfile.write(reparsedXML.toprettyxml(indent = "  ")[len(declaration) + 1:])
    outfile.close()
    assert checkManifest(MANIFEST_FILE), "BUG: created invalid manifest"
    return True

def checkManifest(filename, checkCreator = False):
    """Returns True if this is valid manifest XML file, False otherwise. If
       'checkCreator' is True, also validates whether the file was created by
       this script, returning 'False' (invalid file) if so."""
    if not os.path.exists(filename):
        return False
    try:
        tree = ElementTree.parse(filename)
    except ElementTree.ParseError as e:
        logger.warn("[%s] failed to parse %s" % (str(e), filename))
        return False
    root = tree.getroot()
    if checkCreator:
        creator = root.find("creator")
        if creator != None:
            name = creator.get("name")
            if name == os.path.realpath(__file__):
                return False
    imageTree = root.find("image_tree")
    if imageTree == None:
        logger.debug("Missing <image_tree>")
        return False
    name = imageTree.findall("name")
    if name == None:
        logger.debug("Missing <name> in <image_tree>")
        return False
    revision = imageTree.findall("revision")
    if revision == None:
        logger.debug("Missing <revision> in <image_tree>")
        # missing return is intentional - we don't really care about revision
    return True

def setupEnv(config, osType, dump = False):
    """
    Sets up the environment variables used by programs
    that are called from this python code. Sets them up
    differently for Windows, Linux and MAC.

    """
    logger.debug("Setting environment variables")
    sys.stdout.flush()

    defaultEnv = config["environment"].get(DEFAULT_OS, {})
    if len(defaultEnv) == 0:
        logger.warn("No default environment found!")
    environmentConfig = config["environment"].get(osType, defaultEnv)

    outfile = None
    if dump:
        if(isWindows()):
            outfile = open(ENVIRONMENT_DUMP_BATCH,"w")
            outfile.write("@ECHO Environment Variables \n")
        else:
            outfile = open(ENVIRONMENT_DUMP_SHELL,"w")
            outfile.write("#!/bin/bash\n")


    for varName in environmentConfig["variables"]:
        varInfo = environmentConfig["variables"][varName]
        if varInfo["export"]:
            # 'value' must always be present; don't worry about KeyError
            value = varInfo["value"]
            logger.debug("Setting %s=%s" % (varName, value))
            os.environ[varName] = value
            if outfile:
                if(isWindows()):
                    outfile.write("SET %s=%s\n" % (varName, value))
                else:
                    outfile.write("export %s=%s\n" % (varName, value))
    if outfile:
        outfile.close()

def checkEnv(osType):
    """Checks for the presense of mandatory build tools and environment
       variables. Returns an error if a needed tool or env var is missing."""
    logV("%s environment appears ok" % osType)
    return ERROR_OK #pylint: disable=E0602

def getOsType():
    """
    Determines the OS type:
    """
    return platform.system()

def isWindows():
    """Returns True if the system is a Windows variant & False otherwise"""
    return "win" in getOsType().lower()

def getCpuCount():
    """Returns the number of available CPUs in the current system"""
    cpuCount = 1
    if hasattr(os, "sysconf") and \
            os.sysconf_names.has_key("SC_NPROCESSORS_ONLN"):
        # Unix
        cpuCount = os.sysconf("SC_NPROCESSORS_ONLN")
    elif os.environ.has_key("NUMBER_OF_PROCESSORS"):
        # Windows
        cpuCount = int(os.environ["NUMBER_OF_PROCESSORS"])
    logger.debug("Detected %d CPUs" % cpuCount)
    return cpuCount

def detectAndLogBranch(config):
    """Attempts to auto-detect the branch & logs the result"""
    branch = detectBranch(config)
    if branch:
        logger.debug("Auto-detected branch '%s'" % branch)
    return branch

def detectBranch(config):
    """Attempts to auto-detect the current branch"""
    vceFolder = os.path.join(PACKAGE_ROOT, "vce")
    if os.path.exists(vceFolder):
        branch = detectVceBranch(vceFolder, config)
        if branch != None:
            return branch
    #if os.path.exists(repoFolder):
    # We could not find any metadata indicating the branch, so attempt to get it
    # directly from the root folder's name, as this is our 'best guess'
    branchOptions = list(config.keys())
    # sort our potential branches (with aliases mixed in) by descending length
    # so that we check for longest match first
    branchOptions.sort(key = len, reverse = True)
    dirName = os.path.basename(os.path.normpath(PACKAGE_ROOT))
    for branch in branchOptions:
        if branch in dirName:
            logger.debug("Guessed branch '%s' from path name '%s'" \
                    % (branch, dirName))
            return branch
    return None

def detectVceBranch(vceDir, config):
    """Attempts to auto-detect the current branch via VCE sync logs"""
    productionBranches = []
    for key in config:
        branchInfo = config[key]
        if branchInfo["virtual"]:
            continue
        elif key in branchInfo["aliases"]:
            # this is an alias; skip it
            continue
        productionBranches.append(key)
    # sort branch names by longest-first for matching purposes
    productionBranches.sort(key = len, reverse = True)
    # check the crm_client_log file first as it is usually most accurate
    crmLog = os.path.join(vceDir, CRM_CLIENT_LOG)
    branch = parseCrmClientLog(crmLog, productionBranches)
    if branch != None:
        return branch
    # we still haven't found a branch so try the crm_info file
    crmInfo = os.path.join(vceDir, CRM_INFO_FILE)
    branch = parseCrmInfoFile(crmInfo, productionBranches)
    return branch

def parseCrmClientLog(path, branches):
    """Attempts to read the branch from a CRM client log file"""
    crmLogLines = readLines(path) or []
    branch = None
    for line in crmLogLines:
        line = line.strip()
        if not line.startswith("+"):
            continue
        logger.debug("Candidate line for branch: %s" % line)
        branchCandidates = matchBranches(line, branches)
        if len(branchCandidates) == 0:
            continue
        branch = branchCandidates[-1]
        logger.debug("Detected branch %s in crm log" % branch)
        break
    return branch

def parseCrmInfoFile(path, branches):
    """Attempts to read the branch from a CRM info file"""
    crmInfoLines = readLines(path)
    if crmInfoLines == None:
        return None
    plfCandidate = None
    crmCandidate = None
    imageCandidate = None
    for line in crmInfoLines:
        line = line.strip()
        if "=" not in line:
            continue
        parts = line.split("=")
        potentialBranch = "=".join(parts[1:])
        if parts[0].startswith("PLF_USED") and "=" in line:
            branchCandidates = matchBranches(potentialBranch, branches)
            if len(branchCandidates) == 0:
                continue
            plfCandidate = branchCandidates[-1]
            logger.debug("Detected branch %s in crm info plf" % plfCandidate)
        elif parts[0].startswith("CRM_Append") and "=" in line:
            branchCandidates = matchBranches(potentialBranch, branches)
            if len(branchCandidates) == 0:
                continue
            crmCandidate = branchCandidates[-1]
            logger.debug("Detected branch %s in crm info append" % crmCandidate)
        elif parts[0].startswith("Software Image") and "=" in line:
            if potentialBranch not in branches:
                logger.debug( \
                        #pylint: disable=C0301
                        "Found SI %s in %s, but it does not match any production branch" \
                        % (potentialBranch, CRM_INFO_FILE))
                continue
            if imageCandidate != None:
                logger.warn( \
                        #pylint: disable=C0301
                        "Found multiple Software Images in %s; replacing %s with %s" \
                        % (CRM_INFO_FILE, imageCandidate, potentialBranch))
            else:
                logger.debug("Found SI %s in crm info" % potentialBranch)
            imageCandidate = potentialBranch
    return plfCandidate or crmCandidate or imageCandidate

def matchBranches(line, branchList):
    """Matches text against a list of known branches"""
    ret = []
    for branch in branchList:
        # there should not be many candidate lines, so on-the-fly regex
        # compilation should never be a notable performance issue
        branchRegex = re.compile("/%s_" % branch)
        match = branchRegex.search(line)
        if match == None:
            continue
        logger.debug("Detected branch %s in %s" % (branch, CRM_CLIENT_LOG))
        ret.append(branch)
    if len(ret) > 1:
        logger.debug("Multiple branch candidates found in line: %s" \
                % ", ".join(ret))
    return ret

def readLines(filename):
    """Reads text from a file as a list of lines"""
    if not os.path.isfile(filename):
        logger.warn("'%s' is not a valid file" % filename)
        return None
    try:
        infile = open(filename)
    except (IOError, OSError) as e:
        logger.error("[%s] could not read %s" % (str(e), filename))
        return None
    ret = infile.readlines()
    infile.close()
    return ret

def renameLog(dirPath, logfilename):
    """
    Renames the build-log.txt file to build-log-[0-9]+.txt where
    the number is one larger than the highest existing log number.
    """
    filename = logfilename + ".txt"
    buildLog = os.path.join(LOGFILE_ROOT, filename)
    if not os.path.exists(buildLog):
        logger.debug("No previous build log found @%s" % buildLog)
        return
    # glob will handle the unix-path '/' here; don't need to use os.path.join
    dirList = glob.glob(dirPath + "/%s-*[0-9].txt"%logfilename)
    if len(dirList) > 0:
        dirList.sort()
        lastLog = dirList[-1]
        logRegex = re.compile("%s-([0-9]+)\\.txt"%re.escape(logfilename))
        match = logRegex.search(lastLog)
        # this should always match, based on our glob string
        assert match != None, "BUG: %s did not handle %s correctly" % \
                (__file__, lastLog)
        logNum = int(match.group(1))
        logNum += 1
        newLogName = (("%s-" + str(logNum).zfill(3) + ".txt")%logfilename)
    else:
        newLogName = "%s-001.txt"% logfilename
    logger.info("Log: %s" % newLogName)
    try:
        fin = open(buildLog, 'rb')
        fin.close()
        os.rename(buildLog, os.path.join(LOGFILE_ROOT, newLogName))
    except (IOError, OSError) as e:
        logger.error("[%s] could not open %s" % (str(e), buildLog))

def updateLog():
    """Updates the unified build log if a build-log.txt file exists"""
    sconsLog = os.path.join(LOGFILE_ROOT, BUILD_LOG_PREFIX + ".txt")
    fullLog = os.path.join(LOGFILE_ROOT, UNIFIED_LOG + ".txt")

    if(os.path.isfile(sconsLog) and os.path.isfile(fullLog)):
        concatLogs(sconsLog, fullLog)

def generateReport(reportFiles):
    """Generates a mapping report for the given files"""
    for f in reportFiles:
        filePath = os.path.join(BUILD_ROOT, f)
        reportCmd = "python %s %s" % (REPORT_GENERATOR_SCRIPT, filePath)
        logger.debug("Mapreport command: %s" % reportCmd)
        retCode = call(reportCmd, shell = True)
        if retCode != ERROR_OK: #pylint: disable=E0602
            logger.error("failure occured while generating '%s' report" % f)
            return retCode
    return ERROR_OK #pylint: disable=E0602

# takes string(filename), dict(params), optional list(args)
def buildCmd(filename, params, args = None, mapreportFiles = None):
    """Calls the build.cmd or build.sh batch/shell file."""
    args = args or []
    mapreportFiles = mapreportFiles or []
    assert params.get("CHIPSET") != None, "params must contain CHIPSET"
    buildScript = "build" + (".cmd" if isWindows() else ".sh")
    buildScriptPath = os.path.join(getSConsDir(), SCONS_BUILD_PATH, buildScript)
    buildCommandParts = [ buildScriptPath ]
    buildCommandParts.extend(args)
    for param in params:
        buildCommandParts.append("%s=%s" % (param, params[param]))
    buildCommandParts.append(filename) # finally, add the target file
    buildCommand = " ".join(buildCommandParts)
    # the target.scons script is located within build/ms, so cd there
    buildDir = os.path.join(os.getcwd(), BUILD_PATH)
    if not os.path.exists(buildDir):
        logger.error("%s does not exist; cannot trigger build" % buildDir)
        return ERROR_BAD_PATH #pylint: disable=E0602
    curDir = os.getcwd()
    try:
        os.chdir(buildDir)
#        logger.debug("pwd = %s" % os.getcwd())
        logger.debug("build command: %s" % buildCommand)
        logger.info("\n\nStarting to build %s ...\n\n" % filename)
        sys.stdout.flush() # ensure that all messages are printed before start
        if not FAST_BUILD:
            # give time for the build command to be seen by the user
            time.sleep(SHORT_DELAY)
        retCode = call(buildCommand, shell = True)
        if retCode != ERROR_OK: #pylint: disable=E0602
            logger.error("build command encountered errors: %s" % buildCommand)
            return retCode
        if len(mapreportFiles) == 0:
            return ERROR_OK #pylint: disable=E0602
        # if mapreportFiles has elements, we have 1+ report file(s) to populate
        retCode = generateReport(mapreportFiles)
        if retCode != ERROR_OK: #pylint: disable=E0602
            logger.error("error during mapreport generation")
    finally:
        os.chdir(curDir)
    return retCode

def recompileTargetFiles(config, params, args):
    """Triggers recompilation of key binaries from binaries, not source"""
    logger.info("\n\nRecompile %s ...\n\n" % os.getcwd())
    buildId = params.get("BUILD_ID")
    assert buildId != None, "params must contain BUILD_ID"
    targetFiles = config["files"]
    filesToRebuild = selectFiles(config, recompile = True)
    for f in filesToRebuild:
        artifacts = targetFiles[f]["artifacts"]
        for artifact in artifacts:
            artifactPath = getBinDir(buildId, artifact)
            logger.info("Removing artifact: %s" % artifactPath)
            if not os.path.isfile(artifactPath):
                logger.warn("Cannot remove %s (artifact not found in bin dir)" \
                        % artifact)
                continue
            try:
                os.remove(artifactPath)
            except (IOError, OSError) as e:
                logger.error("[%s] failed to remove artifact: %s" \
                        % (str(e), artifactPath))
                return ERROR_SYS_ERR #pylint: disable=E0602
    retCode = buildTargetFiles(config, params, args, \
            fileList = filesToRebuild, checkArtifacts = True, \
            ignoreParams = True)
    return retCode

def cleanpack(branch, config, params, args):
    """Creates HY11 & HK11 builds from binaries"""
    logger.info("\n\nIn cleanpack ...\n\n")
    assert cleanpack_tz != None, "cleanpack module not available"
    chipset = params.get("CHIPSET")
    assert chipset != None, "params must contain CHIPSET"
    # TODO - consider adding logic to detect if previous build artifacts are
    # present and squash them to ensure that nothing improper gets packed into
    # the build
    retCode = 0
    for i in range(3):
        stepArgs = list(args) # copy original args so we don't overwrite them
        if i == 0 or i == 2:
            # first and last build are normal
            stepName = "build" if i == 0 else "pack"
        elif i == 1:
            # second build strips
            stepArgs.extend(CLEANPACK_ARGS)
            stepName = "strip"
        fileList = selectFiles(config, strip = True, firstBuild = (i == 0))
        # check artifacts only for first & third builds (i = 0 & i = 2)
        # 'strip' omits files that do not apply to cleanpack builds, but it is
        # entirely redundant as we have already explicitly selected our files
        retCode = buildTargetFiles(config, params, stepArgs, fileList, \
                checkArtifacts = (i != 1), ignoreParams = True)
        if retCode != ERROR_OK: #pylint: disable=E0602
            logger.error("build failure detected in %s step" % stepName)
            return retCode
        if i == 0:
            retCode = copyOutput(TEMP_DIR)
            if retCode != ERROR_OK: #pylint: disable=E0602
                return retCode
    retCode = cleanpack_tz.cleanpack(branch, chipset, PACKAGE_ROOT)
    if retCode != ERROR_OK: #pylint: disable=E0602
        logger.error("Failed in cleanpack_tz")
        return retCode
    os.chdir(os.path.join(HY11_DIR, "trustzone_images"))
    retCode = recompileTargetFiles(config, params, args)
    if retCode != ERROR_OK: #pylint: disable=E0602
        logger.error("Failed to recompile HY11")
        return retCode
    os.chdir(os.path.join(HK11_DIR, "trustzone_images"))
    retCode = recompileTargetFiles(config, params, args)
    if retCode != ERROR_OK: #pylint: disable=E0602
        logger.error("Failed to recompile HK11")
        return retCode
    return ERROR_OK #pylint: disable=E0602

def copyOutput(dest):
    """Copies some essential files for cleanpack to HY11 & HK11 folders"""
    for copyTarget in CLEANPACK_COPY_TARGETS:
        srcPath = os.path.join(PACKAGE_ROOT, copyTarget)
        destPath = os.path.join(dest, copyTarget)
        if os.path.exists(destPath):
            try:
                if os.path.isdir(destPath):
                    shutil.rmtree(destPath)
                else:
                    os.remove(destPath)
            except (IOError, OSError) as e:
                logger.error("[%s] failed to remove existing dir: %s" \
                        % (str(e), destPath))
                return ERROR_SYS_ERR #pylint: disable=E0602
        logger.info("Copy %s -> %s" % (srcPath, destPath))
        try:
            if os.path.isdir(srcPath):
                shutil.copytree(srcPath, destPath, symlinks = True)
            else:
                shutil.copy2(srcPath, destPath)
        except (IOError, OSError) as e:
            logger.error("[%s] failed to copy %s -> %s" \
                    % (str(e), srcPath, destPath))
            return ERROR_SYS_ERR #pylint: disable=E0602
    return ERROR_OK #pylint: disable=E0602

def selectFiles(config, strip = False, recompile = False, firstBuild = True):
    """Determines which files to build based on the input arguments.
       Ignores disabled files."""
    targetFiles = config["files"]
    ret = []
    # use the target-specific file list for this target
    for f in targetFiles:
        fileMeta = targetFiles[f]
        if fileMeta["disable"]:
            continue # skip disabled files
        elif strip and fileMeta["strip"]:
            continue # omit stripped files
        elif (not firstBuild) and fileMeta["build-once"]:
            continue # don't build 'build-once' files after the first time
        elif recompile and not fileMeta["recompile"]:
            continue # skip files that should not be recompiled
        ret.append(f)
    return ret

# It is the caller's responsibility what files are passed in. If no file list is
# passed, all applicable files will be automatically selected from the current
# target's config. For features like 'strip', selectFiles() should be explicitly
# called by the calling function & have its result passed as 'fileList'.
def buildTargetFiles(config, params, args, fileList = None, \
        checkArtifacts = True, ignoreParams = False, mapreport = False, \
        testMode = False):
    """Main build function"""
    # TODO - @redesign to reduce arguments & local vars
    #pylint: disable=R0913, R0914
    retCode = ERROR_OK #pylint: disable=E0602

    # Preserve the last log (if one exists) by renaming it from "build-log.txt"
    # to "build-log-[0-9]+.txt"
    renameLog(LOGFILE_ROOT, BUILD_LOG_PREFIX)

    if fileList == None:
        fileList = selectFiles(config)
    else:
        # Some of the given files may not be valid (listed) for this chipset.
        # Remove these to avoid unexpected build failures.
        fileList = [ alias for alias in fileList if alias in config["files"] ]

    fileList.sort()

    try:
        retCode = builderLoop(fileList, config["files"], params, \
                args, checkArtifacts, ignoreParams, mapreport, \
                testMode)
    except Exception: #pylint: disable=W0703
        traceback.print_exc()
        updateLog() # always capture last log, no matter what happened
        return ERROR_UNKNOWN #pylint: disable=E0602
    updateLog()
    return retCode

def concatLogs(file1, file2):
    '''Function to concatenate build logs into the Logfile'''

    try:
        with open(file2, 'a') as outfile:
            with open(file1) as infile:
                for lines in infile:
                    outfile.write(lines)
    except IOError as e:
        logger.error("I/O error({0}): {1}".format(e.errno, e.strerror))


# TODO: @redesign to reduce arguments, local vars, branches, & lines
# pylint: disable-msg=R0912,R0913,R0914,R0915
def builderLoop(fileList, fileMetadata, params, args, \
        checkArtifacts, ignoreParams, mapreport, testMode):
    """Main build worker function"""
    if params.get("CHIPSET") == None:
        logger.error("params must contain CHIPSET")
        return ERROR_INVALID_ARG # pylint: disable=E0602

    if params.get("BUILD_ID") == None:
        logger.error("params must contain BUILD_ID")
        return ERROR_INVALID_ARG # pylint: disable=E0602

    ret = ERROR_OK # pylint: disable=E0602
    if len(fileList) == 0:
        logger.warn("Nothing to build for {} ({}) - skipping".format( \
                params["CHIPSET"], params["BUILD_ID"]))
        return ret

    # Disable artifact checking if SCons command line argument "--filter" is used
    # since there is no guarantee that expected artifacts will indeed be built
    # --filter   Forces SCons to only load specified scripts
    if "--filter" in args:
       checkArtifacts = False

    artifactsDir = getBinDir(params["BUILD_ID"])

    # build up list of aliases that need to compile with/without USES_NO_CP
    # then make a single SCons invocation for each unified list
    #
    # look at fileMeta['internal-test'] to determine which list to associate the
    # image with
    #
    normal_images = []
    for fileItem in fileList:
        normal_images.append(fileItem)

    errorAccumulator = ERROR_OK # pylint: disable=E0602
    while True: # enables goto-like behavior via 'break'
        if testMode and os.path.exists(artifactsDir):
            logger.debug("Removing %s" % artifactsDir)
            try:
                shutil.rmtree(artifactsDir)
            except (IOError, OSError) as e:
                logger.error( \
                        "Failed to remove artifacts dir in test mode: %s" \
                        % artifactsDir)
                ret = ERROR_SYS_ERR #pylint: disable=E0602
                break

        fileParams = dict(params)
        fileArgs = list(args)
        if KEEP_GOING:
            fileArgs.append("-k")

        logger.debug("Params: %s" % ", ".join( \
                [ "%s=%s" % (key, fileParams[key]) for key in fileParams]))
        logger.debug("Args: %s" % ", ".join(fileArgs))

        updateLog()

        expectedArtifacts = set([])
        try:
            initialArtifactSet = getArtifacts(artifactsDir)
        except OSError:
            ret = ERROR_BAD_PATH # pylint: disable=E0602
            break

        if len(normal_images) > 0:
            logger.info("Building {}".format(" ".join(normal_images)))
            mapreportFiles = []
            if mapreport:
                for imageName in normal_images:
                    mapreportFiles.extend( \
                            list(fileMetadata[imageName]["mapreport"]))
            ret = buildCmd(" ".join(normal_images), fileParams, fileArgs, \
                    mapreportFiles = mapreportFiles)
            if ret != ERROR_OK: #pylint: disable=E0602
                logger.error("error %s while building %s" \
                        % (strerr(ret), " ".join(normal_images)))
                if not KEEP_GOING:
                    break
                else:
                    errorAccumulator |= ret

            if checkArtifacts and expectedArtifacts:
                try:
                    validateArtifacts(artifactsDir, normal_images, \
                            fileMetadata, expectedArtifacts)
                except ArtifactNotFoundException as e:
                    logger.error(str(e))
                    if not KEEP_GOING:
                        ret = ERROR_INVALID_ARTIFACT #pylint: disable=E0602
                        break
                    else:
                        errorAccumulator |= ERROR_INVALID_ARTIFACT # pylint: disable=E0602,C0301
                except (IOError, OSError) as e:
                    logger.debug(str(e))
                    logger.error( \
                            # pylint: disable=C0301
                            "Artifact dir {} does not exist or is inaccessible".format( \
                            artifactsDir))
                    ret = ERROR_BAD_PATH #pylint: disable=E0602
                    break

        try:
            artifactSet = getArtifacts(artifactsDir)
        except OSError:
            ret = ERROR_BAD_PATH # pylint: disable=E0602
            break

        # Check for extra artifacts that were just generated but not expected
        newArtifacts = artifactSet.difference(initialArtifactSet)
        unexpectedArtifacts = newArtifacts.difference(expectedArtifacts)

        if checkArtifacts and len(unexpectedArtifacts) > 0:
            extraList = list(unexpectedArtifacts)
            extraList.sort()
            msg = "Extra artifacts: {}".format(", ".join(extraList))
            if FORBID_UNDOCUMENTED_ARTIFACTS:
                logger.error(msg)
                ret = ERROR_INVALID_ARTIFACT #pylint: disable=E0602
                break
            else:
                logger.warn(msg)

        break # always break

    if KEEP_GOING and errorAccumulator != ERROR_OK: # pylint: disable=E0602
        return errorAccumulator

    return ret
# pylint: enable-msg=R0912,R0913,R0914,R0915

def getArtifacts(artifactsDir, mustExist = False):
    """Returns a set of artifacts in the given dir, if it exists. If
       'mustExist' is True (default False), raises IOError if artifactsDir is
       non-existent."""
    artifactSet = set([])

    # if these checks fails, the resulting IOError/OSError should be caught by
    # the caller
    if not os.path.exists(artifactsDir):
        if mustExist:
            raise IOError("Artifact output dir '{}' does not exist".format( \
                    artifactsDir))
        else:
            logger.debug("Artifact dir {} does not exist".format(artifactsDir))
            return artifactSet

    try:
        artifactSet = set([ uri for uri in os.listdir(artifactsDir) \
                if os.path.isfile(os.path.join(artifactsDir, uri)) ])
    except OSError:
        logger.debug(traceback.format_exc())
        logger.error( \
                "Artifact dir {} exists but is inaccessible".format( \
                artifactsDir))
        raise

    return artifactSet

def validateArtifacts(artifactsDir, imageList, imageMetadata, \
        expectedArtifacts = None):
    """Validates that all expected artifacts are present for each of the SCons
       images in the given 'imageList'. If a non-None 'expectedArtifacts' set is
       provided, also updates it with the expected artifacts for each image."""
    if expectedArtifacts == None:
        expectedArtifacts = set([])

    # Any IOError/OSError is passed on to the caller
    artifactSet = getArtifacts(artifactsDir, mustExist = True)

    # Check for missing artifacts. Note that the set
    # constructed above only contains valid files.
    for imageName in imageList:
        expected = getExpectedArtifacts(imageName, imageMetadata)
        expectedArtifacts.update(expected)
        if len(expected) == 0:
            continue

        # We diff against the full artifact set since some of the artifacts
        # might already exist from a previous build.
        missing = expected.difference(artifactSet)
        logger.info("Found {} of {} expected artifacts for {}".format( \
                len(expected) - len(missing), len(expected), imageName))
        if len(missing) > 0:
            missingList = list(missing)
            missingList.sort()
            raise ArtifactNotFoundException("Missing {} artifacts: {}".format( \
                    imageName, ", ".join(missingList)))

def getExpectedArtifacts(filename, fileMetadata, dependencyList = None):
    """Returns the expected artifacts for a file (current + dependencies)"""
    dependencyList = dependencyList or []
    if len(dependencyList) == 0:
        logger.debug("%s has no listed dependencies" % filename)
    else:
        logger.debug("%s has listed dependencies: %s" \
                % (filename, ", ".join(dependencyList)))
    artifacts = set(fileMetadata[filename]["artifacts"])
    for dep in dependencyList:
        artifacts.update(set(fileMetadata[dep]["artifacts"]))
    return artifacts

def getBinDir(buildId, artifact = None):
    """Gets the bin output folder for the current target"""
    buildDir = os.path.join(os.getcwd(), BUILD_PATH)
    if artifact == None:
        return os.path.join(buildDir, 'bin', buildId)
    return os.path.join(buildDir, 'bin', buildId, artifact)

def getDefaultConfig():
    """Returns the 'default' config file based on the first candidate in the
       CONFIG_FILES list that exists locally."""
    assert len(CONFIG_FILES) > 0, \
            "BUG: Must have at least one default config file"
    for config in CONFIG_FILES:
        if os.path.isfile(config):
            logger.debug("build config: {}".format(config))
            return config
    logger.warn("No valid config file found. Expecting {}".format( \
            CONFIG_FILES[0]))
    return CONFIG_FILES[0]

class PassthroughOptionParser(optparse.OptionParser):
    """Extends OptionParser to support pass-through of unknown args to SCons"""
    def _process_args(self, largs, rargs, values):
        originalArgs = list(rargs)
        while rargs:
            try:
                logV("Remaining args: %s" % ", ".join(rargs))
                optparse.OptionParser._process_args(self, largs, rargs, values)
            except (optparse.BadOptionError, optparse.AmbiguousOptionError) \
                    as e:
                logV("parse error on '%s'" % e.opt_str)
                # The OptionParser may not return the raw argument (eg '-j4'
                # will be declared invalid and returned as '-j'), so we have to
                # determine which original argument caused the problem & add it
                # to largs instead of e.opt_str. We are guaranteed that
                # _process_args has removed at least 1 item from rargs UNLESS
                # we had a special '=' argument where the value was added as an
                # additional rarg (eg --jobs=4). We check for this corner case
                # first, then decrement if it does not appear to be the case.
                failureIndex = len(originalArgs) - len(rargs) - 1
                paramValue = ""
                try:
                    # doubles as an implicit
                    # "if failureIndex + 1 < len(originalArgs)"
                    badArg = originalArgs[failureIndex + 1]
                    if badArg[0] == "-":
                        # doubles as an implicit "if '=' in badArg"
                        paramValue = "=".join(badArg.split("=")[1:])
                except IndexError:
                    badArg = originalArgs[failureIndex]
                logV("paramValue = '%s'; rargs = [ %s ]" \
                        % (paramValue, ", ".join(rargs)))
                # paramValue will only match rargs[0] if that arg is (a) empty
                # or (b) the value of an invalid <key>=<value> param
                if len(rargs) > 0 and rargs[0] == paramValue:
                    logV("Pop redundant arg '%s' from args list" \
                            % rargs.pop(0))
                else:
                    badArg = originalArgs[failureIndex]
                logger.debug("passthrough arg: %s (parsed as %s)" \
                        % (badArg, e.opt_str))
                largs.append(badArg)

def parseArguments():
    """Basic argument definition & parsing"""
    parser = PassthroughOptionParser()
    parser.add_option("-b", "--branch", \
            action = "store", \
            type = "string", \
            dest = "branch", \
            help = "Build this branch")
    parser.add_option("--cfg", "--config-file", \
            action = "store", \
            type = "string", \
            dest = "config_file", \
            default = getDefaultConfig(), \
            help = "Specify the build config xml file")
    parser.add_option("-c", "--clean", \
            action = "store_true", \
            dest = "clean", \
            default = False, \
            help = "Clean the build ONLY")
    parser.add_option("--cleanpack", \
            action = "store_true", \
            dest = "clean_pack", \
            default = False, \
            help="Clean and Pack")
    parser.add_option("--cnb", "--clean-and-build", \
            action = "store_true", \
            dest = "clean_build", \
            default = False, \
            help = "Clean and Build")
    parser.add_option("--cbt", "--custom-build-targets", \
            action = "store", \
            type = "string", \
            dest = "custom_build_targets", \
            #pylint: disable=C0301
            help = "Request build of the given SCons target(s) / alias(es), even if they don't appear in build_config.xml")
    parser.add_option("-d", "--default", \
            action = "store_true", \
            default = False, \
            dest = "use_default_target", \
            help = "Build only the branch's default chipset")
    parser.add_option("--detect-branch", \
            action = "store_true", \
            default = False, \
            dest = "detect_branch", \
            help = "Try to detect current branch, print, & exit")
    parser.add_option("--dumpenv", \
            action = "store_true", \
            default = False, \
            dest = "dump_environment", \
            help = "Dump the environment to a shell script")
    parser.add_option("--enable_cc", \
            action="store_true", \
            dest="code_coverage", \
            default=False, \
            #pylint: disable=C0301
            help="Build a code coverage enabled image.This is applicable for tz tz_with_test and tztestexec image only for now.")
    parser.add_option("--fast", \
            action = "store_true", \
            default = False,
            dest = "fast_build", \
            help = "Suppress scripted prompts & delays")
    parser.add_option("-k", "--keep-going", \
            action = "store_true", \
            default = False, \
            dest = "keep_going", \
            # pylint: disable=C0301
            help = "Attempt to keep going despite build errors. Still (eventually) returns a non-zero exit code on failure.""")
    parser.add_option("--list-targets", \
            action = "store_true", \
            default = False, \
            dest = "list_targets", \
            help = "List supported targets for the current branch & exit")
    parser.add_option("-m", "--map-report", \
            action = "store_true", \
            dest = "map_report", \
            default = DEFAULT_MAPREPORT, \
            help = "Mapreport")
    parser.add_option("-r", "--recompile", \
            action = "store_true", \
            dest = "recompile_only", \
            default = False, \
            help = "Recompile only")
    parser.add_option("--sa", "--static-analysis",
            action = "store_true", \
            dest = "enable_static_analysis", \
            default = False, \
            help = "Enable KLOCKWORK buildspec generation & basic SA checks")
    parser.add_option("--scons-args", "--args", \
            action = "store", \
            type = "string", \
            dest = "scons_args", \
            #pylint: disable=C0301
            help = "Explicitly specify custom arguments to be passed down to SCons.")
    parser.add_option("--test-mode", \
            action = "store_true", \
            dest = "test_mode", \
            default = False, \
            #pylint: disable=C0301
            help = "Remove the current target's entire bin dir before building each file in order to verify that dependencies & artifacts are exactly as expected.")
    parser.add_option("-v", "--verbose", \
            action = "store_true", \
            dest = "verbose", \
            default = False, \
            help = "Verbose output")
    return parser.parse_args()

def parseExtraArguments(args, branchConfig):
    """Performs additional parsing of command line args like requested files"""
    ret = {
        "args" : [],
        "files" : [],
        "params" : {},
    }
    # build a list of all <alias>es from all targets to validate what is
    # considered a 'file' vs a pass-through argument. Acceptable files will be
    # validated on each target once targets are determined, but this parsing
    # can include CHIPSET=<chipset>, so we cannot perform this validation yet
    allFiles = set([])
    for target in branchConfig["targets"]:
        targetFiles = set(branchConfig["targets"][target]["files"].keys())
        allFiles.update(targetFiles)
    for arg in args:
        if len(arg.strip()) == 0:
            logger.warn("Received empty arg '%s'" % arg)
            continue
        logV("Parse arg '%s" % arg)
        if arg in allFiles:
            logger.debug("Found supported file '%s' in command line args" % arg)
            if arg in ret["files"]:
                logger.critical("file '%s' is provided twice" % arg)
                sys.exit(ERROR_INVALID_ARG) #pylint: disable=E0602
            ret["files"].append(arg)
        elif arg.startswith("-"):
            # arguments must start with - or --
            # two-part arguments like "-j 1" are no longer supported since users
            # can too-easily pass untracked SCons images (eg skeleton) as extra
            # arguments, causing unexpected/confusing build behavior
            # Any custom command-line arguments to SCons can be passed with
            # --scons-args="<extra args>"
            logger.debug( \
                    "Passing '%s' from command line directly to build script" \
                    % arg)
            ret["args"].append(arg)
        elif "=" in arg:
            # param notation ignores any argument prefixed by '-' or '--' since
            # these are likely passthrough arguments
            paramParts = arg.split("=")
            key = paramParts[0]
            value = "=".join(paramParts[1:])
            logger.debug("Found param %s = %s in command line args" \
                    % (key, value))
            existingParam = ret["params"].get(key)
            if existingParam != None:
                logger.critical("param '%s' is provided twice" % key)
                sys.exit(ERROR_INVALID_ARG) #pylint: disable=E0602
            ret["params"][key] = value
        else:
            msg = """Invalid argument: '{0}'

The given argument is not recognized as a known SCons image or command-line
parameter. If it was intended to be a SCons image, please add it to the build
config or use:
--cbt="{0}"

If it was intended to be a whole or partial SCons command-line argument, use:
--scons-args="{0}".""".format(arg)
            logger.critical(msg)
            sys.exit(ERROR_INVALID_ARG) #pylint: disable=E0602
    return ret

def addCustomBuildTargets(options, branchConfig, buildInput):
    """Adds custom build targets to the list of command line files. Creates
       dummy references for the build targets (as if they had entries within
       build_config.xml) if they are not already present. Also enforces strict
       dependency in the order that custom build targets are given. Applies to
       all chipsets, although only the specified chipset(s) will be built."""
    if not options.custom_build_targets:
        return
    logger.debug("Custom build targets: %s" % options.custom_build_targets)
    customTargets = options.custom_build_targets.split()
    for chipset in branchConfig["targets"]:
        chipsetConfig = branchConfig["targets"][chipset]
        for i in range(len(customTargets)):
            buildTarget = customTargets[i]
            if buildTarget in chipsetConfig["files"]:
                continue # an entry is already present for this chipset
            dummyEntry = BuildConfigParser.fileTemplate()
            # make this entry explicitly dependent on all previous entries to
            # ensure that we never violate an implicit dependency
            dummyEntry["dependencies"] = customTargets[:i]
            chipsetConfig["files"][buildTarget] = dummyEntry.dict()
    buildInput["files"].extend([ buildTarget \
            for buildTarget in options.custom_build_targets.split() \
            if buildTarget not in buildInput["files"] ])

def printParams(params, title = None, exclude = None):
    """Prints the given params (KEY=VALUE)"""
    exclude = exclude or []
    if title == None:
        title = "Parameters:"
    else:
        title = "%s parameters:" % title.capitalize()
    logger.info(title)
    keys = params.keys()
    keys.sort()
    for key in keys:
        if key in exclude:
            continue
        logger.info("%s: %s" % (key, params[key]))
    sys.stdout.flush()

def getSConsDir():
    """Returns the current SCons dir"""
    return os.path.join(os.getcwd(), TOOLS_SCONS_PATH)

def printDiagnosticInfo(branch, targets, options, buildInput, \
        branchWarning = False):
    """Prints essential build info before beginning build"""
    logger.info("")
    logger.info("Diagnostic info")
    logger.info(SEPARATOR)
    logger.info("Platform: %s" % sys.platform)
    logger.info("Python version: %s" % sys.version)
    logger.info("Current directory: %s" % os.getcwd())
    logger.info("Build root: %s" % BUILD_ROOT)
    logger.info("SCons root: %s" % getSConsDir())
    logger.info("OS version : %s" % (platform.system()+" "+platform.release()))
    logger.info("Host Name : %s" % socket.gethostname())
    logger.info("Build Command Line: %s" % " ".join(sys.argv))
    logger.info(SEPARATOR)
    logger.info("Command line options:")
    logger.info("Clean: %s" % str(options.clean))
    logger.info("Clean and build: %s" % str(options.clean_build))
    logger.info("Clean and pack: %s" % str(options.clean_pack))
    logger.info("Use default target: %s" % str(options.use_default_target))
    logger.info("Keep going: %s" % str(options.keep_going))
    logger.info("Static analysis: %s" % str(options.enable_static_analysis))
    logger.info("Mapreport: %s" % str(options.map_report))
    logger.info("Verbose: %s" % str(options.verbose))
    logger.info(SEPARATOR)
    if len(buildInput["files"]) > 0:
        logger.info("Command line files:")
        logger.info(" ".join(buildInput["files"]))
        logger.info(SEPARATOR)
    if len(buildInput["params"]) > 0:
        printParams(buildInput["params"], "command line", \
                exclude = [ "CHIPSET" ])
        logger.info(SEPARATOR)
    if len(buildInput["args"]) > 0:
        logger.info("Additional command line arguments:")
        logger.info(" ".join(buildInput["args"]))
        logger.info(SEPARATOR)
    logger.info("BRANCH:\t\t%s" % branch)
    targetCount = len(targets)
    logger.info("%d %s:\t%s" % (targetCount, \
            "CHIPSET" if targetCount == 1 else "CHIPSETS", \
            ", ".join(targets)))
    logger.info(SEPARATOR)
    if branchWarning:
        logger.info("")
        logger.info("Auto-detected branch '%s'" % branch)
        logger.info("If this is not correct, please explicitly give -b <branch>") #pylint: disable=C0301
    if targetCount > 1:
        logger.info("")
        logger.info("Multiple chipsets (%d) will be built for %s" \
                % (targetCount, branch))
    logger.info("")
    logger.info("Preparing to build...")
    logger.info("")
    sys.stdout.flush()

def logV(msg):
    """verbose module-level logging"""
    logger.log(VERBOSE, msg)

def delete_BuildProductFile():
    """" Function used to delete BuildProducts.txt file if build fails"""
    os.chdir(PACKAGE_ROOT)
    if os.path.isfile('BuildProducts.txt'):
        os.remove("BuildProducts.txt")

def signal_handler(sig, frame): #pylint: disable=W0613
    """Handles <Ctrl> + C"""
    logger.warn("Received <Ctrl> + C - killing all processes")
    sys.exit(ERROR_INTERRUPTED) #pylint: disable=E0602

def setStaticAnalysisEnvironment(enable, buildInput):
    """Set-up environment for static analysis enabled build"""
    if enable:
        pass # all SA features are enabled by default
    else:
        del os.environ["BUILDSPEC"] # disable buildspec_builder (klockwork)
        if len(buildInput["files"]) == 0:
            # full build; disable individual app SA checks
            DEFAULT_ARGS.append("tzbsp_enable_sa=0") # disable SA checks

def setCodeCoverageEnvironment():
    """Set-up environment for code coverage enabled build"""
    cctBinPath = os.environ["CCBIN"]
    os.environ["BULLSEYE_PATH"] = os.environ.get("LLVMBIN")
    os.environ["LLVMBIN"] = cctBinPath
    os.environ["COVFILE"] = os.path.join(BUILD_ROOT, COV_FILE_NAME)
    os.environ["COVERR"] = os.path.join(BUILD_ROOT, COV_LOG_FILE)
    DEFAULT_ARGS.extend(["tzbsp_cc=1", "tzbsp_no_pimem=1", "tzbsp_quadmb=1"])
    enableCodeCovCmd = str(os.path.join(cctBinPath, 'cov01')) + " -1"
    call(enableCodeCovCmd, shell = True)

if os.path.exists(UNIFIED_LOG+".txt"):
    renameLog(LOGFILE_ROOT, UNIFIED_LOG)
handler = logging.FileHandler(UNIFIED_LOG+".txt")
handler.setLevel(logging.DEBUG)
logger.addHandler(handler)

BuildConfigParser.logger.addHandler(handler)

if cleanpack_tz != None:
    cleanpack_tz.logger.addHandler(handler)

if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    try:
        EXIT_CODE = buildAll()
    except:
        delete_BuildProductFile()
        raise
    if EXIT_CODE != 0:
        delete_BuildProductFile()

    sys.exit(EXIT_CODE)

