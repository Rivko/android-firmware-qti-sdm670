# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
'''
Created on Oct 26, 2014

@author: hraghav
'''
from sectools.common.crypto.functions.utils import UtilsBase
from sectools.common.utils import c_path
from sectools.common.crypto.functions.common import run_command


class UtilsOpenSSLImpl(UtilsBase):

    hash_algos_map = \
    {
        UtilsBase.HASH_ALGO_SHA1    : '-sha1',
        UtilsBase.HASH_ALGO_SHA256    : '-sha256',
        UtilsBase.HASH_ALGO_SHA384    : '-sha384',
    }

    def __init__(self, module):
        self.openssl = module

    def hash(self, data, algo):
        """ Function to generate hashes of input file using the standard hashing algorithm specified

        input:  hashing_algorithm: ["SHA1"|"SHA256"|"SHA384"]
                file_to_hash: The file to calculate the hash of.

        output: string representing hash of file_to_hash

        """
        algo_opt = self._get_hash_algo_opt(algo)
        data_path = c_path.create_tmp_file(data)
        try:
            cmd = [self.openssl, 'dgst', algo_opt, data_path]
            return run_command(cmd).split('='.strip())
        finally:
            c_path.remove_tmp_file(data_path)

    def _get_hash_algo_opt(self, algo):
        try:
            algo_opt = self.formats_map[algo]
        except KeyError:
            raise RuntimeError('Hashing algorithm: ' + str(algo) + ' is not supported.')
        return algo_opt
