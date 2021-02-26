# ===============================================================================
#
#  Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import re
import shutil
import stat
import sys
import tempfile
import time
import traceback

from six import ensure_binary

from .c_logging import logger


def normalize(path):
    """ Returns the normalized path """
    if path:
        path = path.strip()
        if path != '':
            if not path.startswith('\\\\'):
                path = os.path.abspath(path)
            path = os.path.normpath(path)
    return path


def join(dirname, *args):
    """ Returns the concatenation of directory and its subdir/file. """
    return normalize(os.path.join(normalize(dirname), *args))


def flatten(path):
    """
    Flattens the path provided, so that a directory can be created with
    that path.
    Flattening involves replacing all backslashes(\) and periods(.) with
    underscore(_).
    """
    return normalize(path).replace('\\', '_').replace(':', '').replace('.', '_')


def basename(path):
    """ Returns basename for linux (can work with windows paths too)
    Useful for network drives such as \\hraghav\Dropbox\foo.zip
    """
    if path:
        path = os.path.basename(path.replace('\\', '/'))

    return path


def _handler_readonly(func, path, exc):
    """ Internal helper function for clean_dir """
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise Exception


def clean_file(filename):
    if filename is not None and os.path.isfile(filename):
        os.remove(filename)


def clean_dir(path, timer=30, recreate=True, suppress_warning=False):
    """
    Removes all the files from the given directory.
    IMPORTANT: Use this method with caution.

    Parameters:
    1. Path (str): The directory that needs to be cleared of files.

    Returns:
    1. returnValue (bool): True
    2. returnError (str): Error if the cleaning of the directory failed
    """

    path = normalize(path)
    if not path:
        return False, 'Invalid path given'

    # Notify user if directory contains files
    if validate_dir(path):
        if not len(os.listdir(path)):
            return True, ''

        if not (timer == 0 or suppress_warning):
            if not logger.warningTimerContinue('Cleaning directory with existing files: "{0}"'.format(path), timer=timer):
                return False, 'User canceled cleanup'

    # Try three times to clean the directory
    returnValue = False
    returnError = ''
    for i in range(0, 3):
        if i:
            time.sleep(i)

        # Try deleting the directory
        try:
            shutil.rmtree(path, ignore_errors=False, onerror=_handler_readonly)
        except OSError:
            pass
        except Exception:
            logger.debug(traceback.format_exc())
            returnError += '{0}\n'.format(sys.exc_info()[1])

        # Verify directory deletion
        if validate_dir(path):
            returnError += 'Directory deletion failed\n'
            continue

        # Create directory
        if recreate:
            try:
                os.makedirs(path)
            except OSError as e:
                returnError += '{0}\n'.format(str(e))
                returnError += 'Directory creation failed\n'
                continue
            except Exception:
                logger.debug(traceback.format_exc())
                returnError += '{0}\n'.format(sys.exc_info()[1])

            # Verify directory creation
            if not validate_dir_write(path):
                returnError += 'Directory creation failed\n'
                continue

        returnValue = True
        break

    return returnValue, returnError.strip()


def validate_file(path):
    """ Returns True if path is an existing file and read access is available """
    return os.path.isfile(path) and os.access(path, os.R_OK)


def validate_file_write(path):
    """
    Returns True if read/write access is available to path and path is not
    a directory, so that a file can be created if not existing
    """
    returnValue = False
    path = normalize(path)
    if validate_dir_write(os.path.dirname(path)):
        if validate_file(path) or (not validate_dir(path)):
            returnValue = True
    return returnValue


def validate_dir(path):
    """ Returns True if path is an existing directory and read access is available """
    return os.path.isdir(path) and os.access(path, os.R_OK)


def validate_dir_write(path):
    """ Returns True if path is an existing directory and read/write access is available """
    returnValue = True

    if validate_dir(path) and os.access(path, os.W_OK):
        testfile = join(path, '__delete_me__')
        # Cleanup

        try:
            os.remove(testfile)
        except Exception:
            pass

        # Create file
        try:
            open(testfile, 'w').close()
        except Exception:
            returnValue = False

        # Cleanup
        try:
            os.remove(testfile)
        except Exception:
            pass
    else:
        returnValue = False

    return returnValue


def create_dir(directory):
    """Creates the directory if doesn't exist already"""
    if validate_dir_write(directory):
        return

    try:
        os.makedirs(directory)
    except OSError:
        # The directory already exists due to a race condition so continue without alarm
        pass

    if not validate_dir_write(directory):
        raise RuntimeError('No write access to directory: ' + directory)


def create_debug_dir(directory=None):
    """Creates the directory if doesn't exist already.
    If directory is None, returns without doing anything.
    """
    if directory is not None:
        return create_dir(directory)


tmp_file_params = {}


def create_tmp_file(data=None):
    """ Create a temporary file (tmp_file) using tempfile.mkstemp.

    If data is not None, it is written to the newly created tmp_file.

    Return full file path of the tmp_file.
    """
    tmp_file_prefix = tmp_file_params.get("prefix", tempfile.template)
    tmp_file_suffix = "-" + tmp_file_params.get("suffix", "")
    tmp_file_dir = tmp_file_params.get("dir", None)

    fd, tmp = tempfile.mkstemp(
        prefix=tmp_file_prefix, suffix=tmp_file_suffix, dir=tmp_file_dir)
    os.close(fd)
    logger.debug("tmp_file_name: " + tmp)
    if data is not None:
        with open(tmp, 'wb') as fd:
            fd.write(ensure_binary(data, encoding='latin-1'))
    return tmp


def remove_tmp_file(path):
    if path is not None and os.path.exists(path):
        clean_file(path)


def copy_file(src=None, dest=None, src_contents=None, force=True):
    """
    Copies the contents of src to the dest.
    IMP: Only one of src and src_contents should be provided.

    Parameters:
    1. src: Source file to copy
    2. dest: Destination file to create.
    3. src_contents: Contents of the source file to be used to create the
        destination file.
    4. force: Remove file if it already exists.

    Return:
    1. returnValue: True if destination file is created
    2. returnError: Error if file copy failed
    """

    if not ((src is None) ^ (src_contents is None)):
        raise AttributeError('One of src or src_contents must be given')

    if not validate_dir_write(os.path.dirname(dest)):
        return False, 'No write access to directory'

    if validate_file(dest):
        if not force:
            return False, 'Dest file already exists'
        else:
            try:
                os.chmod(dest, stat.S_IRWXU or stat.S_IRWXG or stat.S_IRWXO)
                os.remove(dest)
            except Exception:
                return False, 'Removing file: {0}'.format(sys.exc_info()[1])

    if validate_file(dest):
        return False, 'Could not remove dest file'

    if src:
        try:
            shutil.copy2(src, dest)
        except Exception:
            return False, 'Copying file: {0}'.format(sys.exc_info()[1])
    elif src_contents:
        try:
            fd = open(dest, 'w')
        except Exception:
            return False, 'Writing file: {0}'.format(sys.exc_info()[1])
        else:
            fd.write(src_contents)
            fd.flush()
            fd.close()

    return True, ''


def copy_dir(src, dest, force=True, clean_timer=30):
    """
    Copies the files from src to dest.

    Parameters:
    1. src: Source directory.
    2. dest: Destination directory
    4. force: Clear directory if it already exists

    Return:
    1. returnValue: True if folder is copied
    2. returnError: Error if folder copy failed
    """

    if not validate_dir_write(os.path.dirname(dest)):
        return False, 'No write access to directory'

    if validate_dir(dest):
        if not force:
            return False, 'Dest folder already exists'
        else:
            f_returnValue, f_returnError = clean_dir(dest, clean_timer)
            if not f_returnValue:
                return False, 'Clearing dest dir. {0}'.format(f_returnError)
            try:
                os.rmdir(dest)
            except Exception:
                return False, 'Removing dir. {0}'.format(sys.exc_info()[1])

    if validate_dir(dest):
        return False, 'Could not remove dest dir'

    try:
        shutil.copytree(src, dest)
    except Exception:
        return False, 'Copying folder: {0}'.format(sys.exc_info()[1])

    return True, ''


def find_files(folder, file_name_pattern, recurse=True, combine=True, root=''):
    files = []

    if not validate_dir(folder):
        raise RuntimeError('No read access to directory: ' + folder)

    for node in os.listdir(folder):
        full_path = join(folder, node)
        if validate_file(full_path):
            if re.match(file_name_pattern, node):
                files.append(full_path if combine else (root + ('/' if root else '') + node))
        elif recurse and validate_dir(full_path):
            files += find_files(full_path, file_name_pattern, recurse, combine, root + ('/' if root else '') + node)

    return files


def which(filename, paths=[], cur_path=True, sys_path=True, find_one=True):
    return [f for f in which_generator(filename, paths, cur_path, sys_path, find_one)]


def which_generator(filename, paths=[], cur_path=True, sys_path=True, find_one=True):
    path_dirs = paths
    if cur_path:
        path_dirs += ['.']
    if sys_path:
        path_dirs += os.environ.get('PATH').split(os.pathsep)

    for directory in path_dirs:
        fullpath = os.path.join(directory, filename)
        if os.path.isfile(fullpath):
            yield fullpath
            if find_one:
                break


def load_data_from_file(file_path):
    with open(file_path, "rb") as fd:
        data = fd.read()
    return data


def store_data_to_file(file_path, data):
    if isinstance(data, (str, bytes)):
        data = ensure_binary(data, encoding='latin-1')
        fd = open(file_path, 'wb')
        fd.write(data)
        fd.close()
    else:
        raise RuntimeError('Data to write must type str or bytes')
