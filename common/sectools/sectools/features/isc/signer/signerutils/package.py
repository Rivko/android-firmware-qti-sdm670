# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import glob
import os
import re
import zipfile

from sectools.common.utils import c_path
from sectools.common.utils.c_data import find_regex
from sectools.common.utils.c_path import load_data_from_file, store_data_to_file


class Package(object):

    def __init__(self, path, pf, package=None):
        self.path = path
        self.pf = pf
        if package is not None:
            if not c_path.validate_file(package):
                raise RuntimeError('Cannot access: ' + package)
            self.package = package
        else:
            self.package = self.find_package(pf.package_glob)

    def find_package(self, pattern):
        package = None
        packages = glob.glob(c_path.join(self.path, pattern))
        if len(packages) == 1:
            package = packages[0]
        elif len(packages) > 1:
            raise RuntimeError('Multiple packages exist in ' + self.path + ': ' + str(packages) +
                               ' Should contain only one package.')
        return package

    def update_data(self):
        # Set local variables
        package = self.package
        pf = self.pf

        # Check that the package exist
        if package is None:
            raise RuntimeError('Package is not set')
        elif not c_path.validate_file(package):
            raise RuntimeError('Package could not be accessed at ' + package)

        # Get the files list from the package
        files_list = []
        if pf.zip:
            fp = zipfile.ZipFile(package, 'r')
            try:
                files_list = [f.filename for f in fp.infolist()]
            finally:
                fp.close()
        else:
            files_list = os.listdir(package)

        # Find the relevant files in the package
        for file_tag, file_info in pf.file_data.items():

            # Decide optional and list tags
            file_type = file_info['type']
            is_optional = False
            is_list = False
            if isinstance(file_type, tuple):
                is_optional = True
                file_type = file_type[0]
            if isinstance(file_type, list):
                is_list = True
                file_type = file_type[0]

            # Find the files from the package
            if pf.regex_search or is_list:
                found_path = find_regex(file_info['regex'], files_list, find_first=not(is_list))
            else:
                found_path = filter(lambda x: x == file_info['name'], files_list)

            # Check the found paths
            if not is_optional and not found_path:
                raise RuntimeError(file_tag + ' is not found')

            read_func = load_data_from_file
            if pf.zip:
                fp = zipfile.ZipFile(package, 'r')
                read_func = fp.read
            try:
                # Read the data
                if is_list:
                    def get_num(key):
                        num = re.search(file_info['regex'], key).group(1)
                        return num
                    found_path.sort(key=get_num)
                    found_data = [read_func(t) for t in found_path]
                else:
                    found_path = found_path[0] if found_path else None
                    found_data = read_func(found_path) if found_path else None
            finally:
                if pf.zip:
                    fp.close()

            # Set the data
            pf.file_data[file_tag]['data'] = found_data

    def save_package(self):
        # Set local variables
        package = self.package
        pf = self.pf

        # Check that the package can be written to
        if package is None:
            raise RuntimeError('Package is not set')
        elif not c_path.validate_file_write(package):
            raise RuntimeError('Package could not be written to at ' + package)

        # Set internal functions
        write_func = lambda x, y: store_data_to_file(c_path.join(package, x), y)
        if pf.zip:
            fp = zipfile.ZipFile(package, 'w')
            write_func = fp.writestr

        # Store the relevant files in the package
        try:
            for file_tag, file_info in pf.file_data.items():
                # Decide optional and list tags
                file_type = file_info['type']
                is_optional = False
                is_list = False
                if isinstance(file_type, tuple):
                    is_optional = True
                    file_type = file_type[0]
                if isinstance(file_type, list):
                    is_list = True
                    file_type = file_type[0]

                # Check optional data
                if file_info['data'] is None:
                    if is_optional:
                        continue
                    else:
                        raise RuntimeError(file_tag + ' doesnt have any data')

                # Write the data
                if is_list:
                    if (len(file_info['data']) == 1):
                        write_func(file_info['name'].format(''), file_info['data'][0])
                    else:
                        for idx, d in enumerate(file_info['data']):
                            write_func(file_info['name'].format('_' + str(idx + 1)), d)
                else:
                    write_func(file_info['name'], file_info['data'])
        finally:
            if pf.zip:
                fp.close()


class SignaturePackageFiles(object):

    def __init__(self):
        self.zip = True
        self.regex_search = False
        self.package_glob = ''
        self.file_data = \
        {
            'attestation_cert' :
            {
                'name' : 'attestation_cert.cer',
                'type' : '',
                'data' : None,
            },
            'attestation_ca_cert' :
            {
                'name' : 'attestation_ca_cert.cer',
                'type' : ('',),
                'data' : None,
            },
            'root_cert_list' :
            {
                'name' : 'root_cert{0}.cer',
                'regex' : 'root_cert(_[\d]+)?\.cer',
                'type' : [''],
                'data' : None,
            },
            'signature' :
            {
                'name' : 'signature.bin',
                'type' : '',
                'data' : None,
            },
        }


class ToSignPackageFiles(object):

    def __init__(self):
        self.zip = True
        self.regex_search = False
        self.package_glob = ''
        self.file_data = \
        {
            'to_sign' :
            {
                'name' : 'tosign_hash.bin',
                'type' : '',
                'data' : None,
            },
            'signing_config' :
            {
                'name' : 'signing_config.json',
                'type' : '',
                'data' : None,
            },
        }


