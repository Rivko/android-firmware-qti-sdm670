# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
This module provides methods to query, copy and generate meta build.
"""
import glob
import json
import os
import sys
import re
import subprocess

# Append sectools directory in the path
from sectools.common.utils.c_platform import is_linux
from sectools.common.utils.c_process import run_command
from sectools.features.isc.signer.singleton import Singleton
from six import ensure_str, add_metaclass

from sectools.common.build.meta_lib_shim import RESULT_TOKEN

if __name__ == '__main__':
    from . import __secfile__
    __secfile__.init()
    from .__secfile__ import DIR_PATH
    sys.path.append(DIR_PATH + '/../../..')

from sectools.common.utils.c_logging import logger
from sectools.common.utils import c_path
from sectools.common.utils.c_misc import process_kill
from sectools.common.device.android import AndroidDevice
from sectools.common.utils.c_misc import load_data_from_file, store_data_to_file

FINDBUILD_EXE="//stone/aswcrm/smart/NT/Bin/FindBuild.exe"
UFC_EXE = "//stone/aswcrm/smart/NT/Bin/UFC.exe"
BUILD_SCRIPTS_DIR = "common/build/"
BUILD_PY = 'build.py'
BUILD = 'tz'
REGENERATE_CMD_TZ = BUILD_PY + ' ' + BUILD + ':'
BUILDLOAD_PY = 'fastboot_complete.py'

UFC_LOG = '_ufc.log'
UFC_COMPLETE_STR = '_ufc_completed'
REGENERATE_BUID_LOG = '_regenerate_meta_build.log'
GENERATION_COMPLETE_STR = '_metageneration_completed'
FASTBOOT_COMPLETE_LOG = '_fastboot_complete.log'
META_BUILD_LOG = 'metabuild_log'
META_LIB_PATH_REL = ['common/tools/meta',
                     'common/build/lib']

SECTION_BREAK = '================================================================'


def getCurrentTime():
    from time import strftime, localtime
    st = strftime("%Y-%m-%d %H:%M:%S", localtime())
    return st


@add_metaclass(Singleton)
class Py2Finder(object):
    """
    Singleton class to find python2 executable if it exists
    and return the full path to it.
    """
    py2_exe = None
    def __init__(self):
        if not self.py2_exe:
            print("initializing...")
            if is_linux():
                # is python2 in the path?
                cmd = ['which', 'python2']
                output = run_command(cmd, ret_binary=False)
                if 'python2' in output:
                    self.py2_exe = output.strip()
                else:
                    # is python2 in /usr/bin?
                    if os.path.exists('/usr/bin/python2'):
                        self.py2_exe = '/usr/bin/python2'
            else:
                # is python in the path?
                cmd = ['python', '--version']
                output = run_command(cmd, ret_binary=False)
                # if so does the version start with 2?
                if 'Python' in output:
                    version = output.split()[1]
                    if version.startswith('2'):
                        self.py2_exe = 'python'

                # check the usual install location for py2
                if not self.py2_exe:
                    possible = glob.glob(r'c:\Python*')
                    for p_dir in possible:
                        poss_python_exe = os.path.join(p_dir, 'python.exe')
                        if os.path.exists(poss_python_exe):
                            self.py2_exe = poss_python_exe
                            break

    def found_py2(self):
        """
        Returns True if we found a viable python2, else False
        """
        return self.py2_exe is not None

    def py2_executable(self):
        """
        returns resolved path to py2 executable if found, else
        returns None
        """
        return self.py2_exe

class MetaInfo:
    """
    Interface to meta_lib_shim.py, which imports the metabuild's
    meta_lib.py, and services specific queries of contents.xml
    """
    def __init__(self, metabuild_path):
        if metabuild_path is None:
            metabuild_path = os.getcwd()
        metabuild_path = metabuild_path.strip()

        lib_path, contents_xml = self.validate_metabuild(
            metabuild_path, META_LIB_PATH_REL, "contents.xml")

        script_dir = os.path.dirname(os.path.realpath(__file__))
        self.py2_exe = Py2Finder().py2_executable()

        self.base_cmd = [
            self.py2_exe,
            os.path.join(script_dir, 'meta_lib_shim.py'),
            lib_path, contents_xml]

    def run_cmd(self, cmd):
        if self.py2_exe is None:
            raise RuntimeError(
                r"To access metabuild info running under python3, a python2 executable "
                r"is currently required. If you need this functionality please "
                r"ensure python2 is installed")

        """ shell command to meta_lib_shim.py """
        response = ensure_str(subprocess.check_output(cmd)).splitlines()

        # look for 'Result >>>' line, the line after contains the json we want
        resp_obj = None
        try:
            token_pos = response.index(RESULT_TOKEN)
            resp_obj = json.loads(response[token_pos+1])
        except (KeyError, ValueError) as e:
            logger.warning("Bad return from meta_lib_shim: {}".format(str(e)))
            logger.warning("--> Response was: {}".format(response))

        return resp_obj

    @staticmethod
    def validate_metabuild(metabuild_path, target_paths, target_file):
        """Check that the given 'metabuild_path' is a valid, accessible metabuild.
           Returns the resolved path of 'target_path' & 'target_file' within
           'target_path' (used as a key file to determine meta validity)."""
        if not os.path.exists(metabuild_path):
            raise ValueError(
                "Given metabuild_path not found/accessible: {}".format(
                    metabuild_path))

        if not isinstance(target_paths, list):
            target_paths = [target_paths]

        for target_path in target_paths:
            child_path = os.path.join(metabuild_path, target_path)
            key_file = os.path.join(metabuild_path, target_file)

            if os.path.isdir(child_path) and os.path.isfile(key_file):
                return child_path, key_file

        raise ValueError(
            "metabuild_path does not appear to be a valid metabuild. "
            "Did you accidentally provide another image path (eg BOOT.XF?) "
            "File(s) don't exist in {}: {} {})".format(
                metabuild_path, target_paths, target_file)
        )

    def get_build_path(self, component_name):
        """ get the build path """
        cmd = list(self.base_cmd)
        cmd.extend(['get_build_path', component_name])

        return self.run_cmd(cmd)

    def get_file_vars(self, val, **kwargs):
        cmd = list(self.base_cmd)
        cmd.extend(['get_file_vars', val])
        flavor = kwargs.get('flavor')
        if flavor:
            cmd.extend(['--flavor', kwargs['flavor']])

        return self.run_cmd(cmd)

    def get_files_detailed(self, val):
        cmd = list(self.base_cmd)
        cmd.extend(['get_files_detailed', val])

        return self.run_cmd(cmd)

    def get_files(self, file_types, flavor=None):
        cmd = list(self.base_cmd)
        cmd.extend(['get_files', file_types])
        if flavor:
            cmd.extend(['--flavor', flavor])

        return self.run_cmd(cmd)

    def get_product_info(self):
        cmd = list(self.base_cmd)
        cmd.append('get_product_info')

        return self.run_cmd(cmd)

class MetaBuild(object):

    def __init__(self, meta_build_path):
        """ Initializes internal variables """
        self.meta_build_path = meta_build_path
        self.output_dir = None
        self.image_build_path = None

        self.pid = None
        self.log = None
        self.run = True
        self.state = None
        self.retcode = None

    @classmethod
    def getMetainfo(self, meta_build_path):
        """ Returns meta_info object (or in the case of python3 a MetaInfo instance)
            from meta build's meta_lib module.

            In the case of python3 we return a MetaInfo instance, which invokes
            a shim layer to deal with running the python2 code required to import meta_lib

        :param str meta_build_path: meta build path
        :returns: meta_info object
        :rtype: class instance
        """
        sys.path += [c_path.join(meta_build_path, path) for path in META_LIB_PATH_REL]

        try:
            import meta_lib
        except SyntaxError as e:
            # SyntaxError on import of meta_lib expected to occur when importing
            # the python2 meta_lib when running sectools in python3.  In this case we
            # return the shimmed version on meta_info.
            return MetaInfo(meta_build_path)

        try:
            meta_info = meta_lib.meta_info(meta_dir=meta_build_path)
        except Exception:
            meta_info = meta_lib.meta_info()
        return meta_info


    def logging_state_info(self, msg):
        logger.info(msg)
        if self.state:
            self.state += '\n' + msg
        else:
            self.state = '\n' + msg

    def get_metabuild_log(self):
        data = []
        logdir = os.path.dirname(self.log)
        ufc_log = c_path.join(logdir, UFC_LOG)
        regenerate_log = c_path.join(logdir, REGENERATE_BUID_LOG)
        buildloading_log = c_path.join(logdir, FASTBOOT_COMPLETE_LOG)
        metabuild_log = c_path.join(logdir, META_BUILD_LOG)

        # remove meta build log from previous session
        if c_path.validate_file(metabuild_log):
            os.remove(metabuild_log)

        data.append('Meta build log:\n')

        if c_path.validate_file(ufc_log):
            data.append(SECTION_BREAK)
            data.append('Step 1: UltraFastCopy meta build')
            data.append('Meta Build: ' + self.meta_build_path)
            data.append(SECTION_BREAK)
            data.append(load_data_from_file(ufc_log))
        else:
            data.append('meta build copy skipped')
            data.append(SECTION_BREAK)

        if c_path.validate_file(regenerate_log):
            data.append(SECTION_BREAK)
            data.append('Step 2: Meta build regeneration')
            data.append('Meta Build: ' + logdir)
            data.append('Image Build:' + self.image_build_path)
            data.append(SECTION_BREAK)
            data.append(load_data_from_file(regenerate_log))
        else:
            data.append('meta build regeneration skipped')
            data.append(SECTION_BREAK)

        if c_path.validate_file(buildloading_log):
            data.append(SECTION_BREAK)
            data.append('Step 3: Meta build loading')
            data.append('Meta Build: ' + logdir)
            data.append(SECTION_BREAK)
            data.append(load_data_from_file(buildloading_log))

        store_data_to_file(metabuild_log, "\n".join(data))

        return metabuild_log

    def execute(self, cmd, logfile, work_dir=None, timer=900, interval=5):
        fd = open(logfile, 'w+')
        p = subprocess.Popen(r" ".join(cmd), cwd=work_dir, shell=False, stdout=fd, stderr=fd)
        self.pid = p.pid

        # set timer and start polling process status
        # kill the test process and cleanup upon timeout
        import time
        count = int(timer / interval)
        while count > 0:
            time.sleep(interval)
            if p.poll() is not None:
                break
            count -= 1
        if p.poll() is None:
            p.terminate()
            logger.error('process is terminiated due to timeout after ' + str(timer) + ' seconds')
        fd.close()

        return p.returncode

    def stop_execute(self):
        self.logging_state_info('meta build process (pid: ' + str(self.pid) + ') is terminated')
        self.run = False
        retval = process_kill(process_id=self.pid)
        return retval

    def copyMetaBuild(self, exclusion=None, force=False, timer=3600, interval=5):
        """ copy meta build to dest directory with UFC
        :param str build_path: meta build directory, require read access
        :param str output_dir: destination directory, require write access
        :param str exclusion: file extension in semicolon separated list (e.g. .o;.lib)
        :param bool force: if enabled, will force the meta build copy even if it already exists

        :return str dest meta build directory
        """
        if self.run:
            self.retcode = ''
            # get build id from build_path as it is the directory name in dest
            build_id = getBuildIDfrompath(self.meta_build_path)

            # check if meta build is already copied to dest directory
            dest_val = c_path.normalize(self.output_dir)
            dest_dir = c_path.join(dest_val, build_id)
            UFC_complete_flag = c_path.join(dest_dir, UFC_COMPLETE_STR)
            self.log = c_path.join(dest_dir, UFC_LOG)

            if c_path.validate_file(UFC_complete_flag) and force == False:
                self.logging_state_info('meta build exists in destination directory: ' + dest_dir)
                self.logging_state_info('meta build copy is skipped')

            else:
                c_path.create_dir(dest_dir)
                cmd = [UFC_EXE, self.meta_build_path, dest_val, '-e', exclusion] if exclusion else [UFC_EXE, self.meta_build_path, dest_val]

                self.logging_state_info(getCurrentTime() + ' start meta build copy, please wait and do not interrupt ...')
                self.logging_state_info('UFC command: ' + " ".join(cmd))
                self.run = False
                self.retcode = self.execute(cmd, self.log, dest_val, timer, interval)
                self.logging_state_info(getCurrentTime() + ' UFC return code: ' + str(self.retcode))
                self.logging_state_info('UFC log can be found at: ' + self.log)
                if self.retcode == 0:
                    open(UFC_complete_flag,'a').close()
                    self.run = True
                self.logging_state_info(SECTION_BREAK)
        else:
            self.logging_state_info('skip meta build copying')

        return dest_dir

    def generateMetaBuild(self, timer=1200, interval=5):
        """ generate new meta build with given image build
        :param str meta_build_path: meta build directory, require write access
        :param str image_build_path: image build directory, require read access
        """
        if self.run:
            self.retcode = ''
            # meta build path validation
            meta_path_val = c_path.normalize(self.meta_build_path)
            if not c_path.validate_dir_write(meta_path_val):
                raise RuntimeError('Cannot write at: ' + meta_path_val)

            # image build path validation
            image_path_val = c_path.normalize(self.image_build_path)
            if not c_path.validate_dir(image_path_val):
                raise RuntimeError('Cannot access: ' + image_path_val)

            """TODO: Don't know how to check if TZ build has changed
                     and if re-generate NON-HLOS.bin is needed.
                     For now, always re-generate meta build.
            """
            # check if meta build has been generated with the required image build already
            meta_info = self.getMetainfo(self.meta_build_path)
            path = meta_info.get_build_path(BUILD)
            tz_path_in_meta_build_val = c_path.normalize(path)
            generation_complete_flag = c_path.join(meta_path_val, GENERATION_COMPLETE_STR)
            """
            if tz_path_in_meta_build_val == image_path_val and c_path.validate_file(generation_complete_flag):
                self.logging_state_info('meta build with required image build is available at: ' + meta_path_val)
                self.logging_state_info('meta build generation is skipped')
                logger.debug('meta build path: ' + meta_path_val)
                logger.debug('image build path: ' + image_path_val)
            else:
            """
            cmd = ['python', REGENERATE_CMD_TZ + image_path_val]
            cwd = c_path.join(meta_path_val, BUILD_SCRIPTS_DIR)
            self.logging_state_info(getCurrentTime() + ' start meta build generation, please wait and do not interrupt ...')
            self.logging_state_info('Set current working directory: ' + cwd)
            self.logging_state_info('Generate Meta build command: ' + " ".join(cmd))
            self.log = c_path.join(meta_path_val, REGENERATE_BUID_LOG)
            self.run = False
            self.retcode = self.execute(cmd, self.log, cwd, timer, interval)
            self.logging_state_info(getCurrentTime() +' meta build generation return code: ' + str(self.retcode))
            self.logging_state_info('meta build generation log can be found at: ' + self.log)
            if self.retcode == 0:
                open(generation_complete_flag,'a').close()
                self.run = True
            self.logging_state_info(SECTION_BREAK)
        else:
            self.logging_state_info('skip meta build regeneration')

    def loadMetaBuild(self, keyword=None, timer=1200, interval=5):
        """ load meta build with fastboot_complete.py
        :param str meta_build_path: meta build directory, require read access
        """
        if self.run:
            self.retcode = ''
            # meta build path validation
            meta_path_val = c_path.normalize(self.meta_build_path)
            if not c_path.validate_dir_write(meta_path_val):
                raise RuntimeError('Cannot write at: ' + meta_path_val)

            # check device status (put device to fastboot mode)
            if AndroidDevice.checkFastboot():
                enable_buildloading = True
            else:
                enable_buildloading = AndroidDevice.fastbootDevice()

            if enable_buildloading:
                cmd = ['python', BUILDLOAD_PY]
                cwd = c_path.join(meta_path_val, BUILD_SCRIPTS_DIR)
                self.logging_state_info(getCurrentTime() + ' start meta build loading, please wait and do not interrupt ...')
                self.logging_state_info('Set current working directory: ' + cwd)
                self.logging_state_info('Meta build loading command: ' + " ".join(cmd))
                self.log = c_path.join(meta_path_val, FASTBOOT_COMPLETE_LOG)
                self.run = False
                self.retcode = self.execute(cmd, self.log, cwd, timer, interval)
                self.logging_state_info(getCurrentTime() +' meta build loading return code: ' + str(self.retcode))
                self.logging_state_info('meta build loading log can be found at: ' + self.log)
                if self.retcode == 0:
                    self.run = True
                self.logging_state_info(SECTION_BREAK)
                if AndroidDevice.restartDevice():
                    self.logging_state_info('device reboot completed')
                else:
                    self.logging_state_info('device reboot failed, please reboot the device manually')
            else:
                self.retcode = 1
                logger.error('device cannot be set in fastboot mode, please set manually')
        else:
            self.logging_state_info('skip meta build loading')

def getBuildIDfrompath(meta_build_path):
    """ Returns build ID from meta build path
    :param str meta_build_path: meta build path
    :returns: meta build ID
    :rtype: str
    """
    # meta build path validation
    path_val = c_path.normalize(meta_build_path)
    if not c_path.validate_dir(path_val):
        raise RuntimeError('Cannot access: ' + path_val)

    # get build id from build_path as it is the directory name in dest
    build_id = os.path.basename(path_val)

    return build_id

def findBuild(build_id):
    """ Returns build path from build ID
    :param str build_id: build ID to search build path
    :returns: build directory
    :rtype: str
    """
    # '-la' is used to get the latest build
    cmd = [FINDBUILD_EXE, '-la', build_id]
    process = subprocess.Popen(" ".join(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    stdout = ensure_str(stdout)

    if stderr:
        logger.error(stderr)
    if re.findall("No Record Found!", stdout, re.M | re.S):
        raise RuntimeError('build path with build id "' + build_id + '" is not found')
    else:
        slist = re.findall("Location:(.*)\r\nComments:", stdout, re.M | re.S)
        if slist:
            path = c_path.normalize(slist[0])
            logger.info('build path: ' + path)
            return path
        else:
            raise RuntimeError('build path with build id "' + build_id + '" is not found')

if __name__ == '__main__':

    """
    metabuild_path = r'\\harv-c-qingw\Dropbox\new\M8994AAAAANLGD1100340.1'
    mb = MetaBuild(metabuild_path)
    mb.image_build_path = r'\\snowcone\builds684\INTEGRATION\TZ.BF.3.0.r1-00156-M8994AAAAANAAT-1'
    mb.output_dir = 'c:/tmp/ltf_test'
    mb.loadMetaBuild()
    findBuild('MDM9655.MN.1.0-00011-PRESIL.INT-1')
    """
