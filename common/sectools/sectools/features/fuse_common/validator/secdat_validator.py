# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Feb 26, 2014

@author: francisg
'''


from . import BaseValidator
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.features.fuse_common.parsegen.secdat import SecdatParseGen
from sectools.features.fuse_common.debugrepr import dump_debug_data_model


class SecDatValidator(BaseValidator):
    '''
    secDatValidator
    '''
    def __init__(self, oem_config_path, qti_config_path,
                 user_config_path, ui_config_path,sec_dat_path, config, feature, output_dir, debug, TOOL_NAME):
        '''
        Constructor
        '''
        self.oem_config_path = oem_config_path
        self.qti_config_path = qti_config_path
        self.user_config_path = user_config_path
        self.ui_config_path = ui_config_path
        self.sec_dat_path = sec_dat_path
        self.output_dir = output_dir
        self.debug = debug
        self.TOOL_NAME = TOOL_NAME

        self.config_parser = config.ConfigParser()
        self.secdat_parser = SecdatParseGen(feature)

    def val_type(self):
        return 'secDatValidator'

    def get_version_dir(self, version):
        return 'v' + str(version)

    def validate(self):

        logger.info( 'Validating... ' )

        retVal = True

        self.generateConfigModel()
        self.generateSecDatModel()

        file_version = self.config_parser.get_secdat_file_version()
        version_dir = self.get_version_dir(file_version)
        debugrepr_validate = c_path.join(self.output_dir, version_dir, 'validate')
        dump_debug_data_model(self.secdat_parser.data_model, debugrepr_validate, self.debug, self.TOOL_NAME)

        try:
            if self.config_parser.data_model.compare(self.secdat_parser.data_model, 'config files', 'sec.dat'):
                logger.info( '\nValidate Successful!' )
        except Exception as e:
            append_str = ('Validation of the secdat file against the config files failed.' + '\n'
                          '\n'
                          'Differences Details: ' )
            raise RuntimeError(append_str + str(e))

        return retVal

    def generateConfigModel(self):
        '''
        generates model from both oem and qti config xml's and combines them into single data model
        '''
        self.config_parser.read(self.oem_config_path, self.qti_config_path, self.ui_config_path, self.user_config_path)

    def generateSecDatModel(self):
        '''
        generates model from sec.dat binary
        '''
        self.secdat_parser.read(self.sec_dat_path)
