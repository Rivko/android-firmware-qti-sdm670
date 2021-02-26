# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Feb 25, 2014

@author: francisg
'''

import __secfile__
__secfile__.init()

import operator
import defines

from sectools.features.fuse_common.parsegen import BaseDataParseGen
from sectools.features.fuse_common.datamodel.auto_gen_model import Cfg_Fuseblower, Cfg_Module, Cfg_Fuse_Region, Cfg_Fuse, Cfg_Field
from sectools.common.utils.c_base import CoreError, CoreErrorCode
from sectools.common.utils.c_logging import logger
from sectools.common.utils import c_path, c_config


class _CoreConfig(c_config.CoreConfig):
    def __init__(self):
        pass


class ConfigParser(BaseDataParseGen):
    '''
    ConfigParser for Fuse Master xmi
    '''

    def __init__(self):
        '''
        Constructor
        '''
        self.path = None
        #self.data_model = Cfg_Fuseblower()
        self.core_config = _CoreConfig()
        self.core_fmconfig = _CoreConfig()


    def read(self, xml_path, config_xml_path=None):

        import auto_gen_fm_config
        import auto_gen_fmconf_config

        try:
            self.path = c_path.normalize(xml_path)
            self.config_path =  c_path.normalize(config_xml_path)

            # load the FM config data
            if(self.config_path is not None):
                c_config.CoreConfig.__init__(self.core_fmconfig, auto_gen_fmconf_config, self.config_path)
                self.filtermethod = self.core_fmconfig.root.filterType.method
            else:
                # TODO: change default to None or error out, check with Maria
                self.filtermethod = 'all'

            # populate the FM data model
            self._populate(self.path, auto_gen_fm_config, self.core_config)

        except Exception:
            logger.debug('fuseMasterData: Exception raised')
            raise


    def write(self, file_path):
        print 'WARNING: No write intended, please check usage'
        pass


    def _populate(self, path, core, config):
        '''
        transfers the data from CorConfig root node from Fuse Master xmi and adds
        into data model fuse objects
        '''


        c_config.CoreConfig.__init__(config, core, path)

        # create main SECURITY_CONTROL module
        self.data_model = Cfg_Fuseblower()
        fuse_module = Cfg_Module()
        fuse_module.id = 'SECURITY_CONTROL'
        fuse_module.base_address = config.root.securityControlBaseAddr
        self.data_model.module.append(fuse_module)

        # create region and fuse to be populated
        fuse_region = Cfg_Fuse_Region()
        fuse = Cfg_Fuse()

        # used to keep track if FM regions should be inserted in same region based on FM rows
        previous_region = None
        previous_row = None

        for region in config.root.regions:

            # check if the current region has rolled over to the next row
            # if so, insert the last fuse into the current fuse_region and create a new one
            if(previous_region is not None and
               previous_region.sections[0].otpRowID != region.sections[0].otpRowID):
                if self._checkFilter(fuse_region.id) == True:
                    fuse_region.fuse.append(fuse)
                    self.data_model.module[0].fuse_region.append(fuse_region)
                fuse_region = Cfg_Fuse_Region()
                fuse = Cfg_Fuse()
                previous_row = None

            fuse_region.id = region.name

            # grab all the fuse/fields within the region
            for section in region.sections:

                # FM puts None in cases where it should be 0
                if section.startColumnID is None: section.startColumnID = 0
                if section.otpRowID is None: section.otpRowID = 0

                # check if the new fuse field is in same row as previous field
                # if not in same row, insert the last fuse object into Fuse list and create new one
                if (previous_row is not None and
                    previous_row != section.otpRowID):
                    fuse_region.fuse.append(fuse)
                    fuse = Cfg_Fuse()

                field = Cfg_Field()
                field.id = section.name
                # per FM, only OEM is set, anything other than that is QTI
                field.owner = section.fuses[0].owner if section.fuses[0].owner is not None else 'OwnerType_QTI'
                # TODO: raise Exception   , put function
                assert(section.startColumnID >= 0)
                assert(section.endColumnID >= 0)
                field.bits = str(section.endColumnID)+':'+str(section.startColumnID)
                field.value = 0

                assert(section.otpRowID >= 0)
                fuse.n = section.otpRowID
                fuse.address = format((int(fuse_module.base_address, 16) +  (fuse.n * 0x8)), 'x').upper()
                assert(int(fuse.address,16) >= int(fuse_module.base_address,16))

                fuse.field.append(field)
                previous_row = section.otpRowID

                # ---- end of section loop ----

            previous_region = region

            # ---- end of region loop ----

        else:
            if self._checkFilter(fuse_region.id) == True:
                fuse_region.fuse.append(fuse)
                self.data_model.module[0].fuse_region.append(fuse_region)

        # now that we got the data from Fuse Master core config, update the rows and sort
        for module in self.data_model.module:
            module.fuse_region.reverse()
            for region in module.fuse_region:
                region.fuse = sorted(region.fuse, key=operator.attrgetter('n'))
                for i in range(len(region.fuse)):
                    region.fuse[i].n = i



    def _checkFilter(self, fuse):
        retval = None

        if(self.filtermethod == 'inclusion'):
            if fuse in self.core_fmconfig.root.filter.fuse:
                retval = True
        elif(self.filtermethod == 'exclusion'):
            if fuse not in self.core_fmconfig.root.filter.fuse:
                retval = True
        elif(self.filtermethod == 'all'):
            retval = True
        elif(self.filtermethod == 'none'):
            retval = False
        else:
            raise Exception('invalid filter method: ' + self.filtermethod + '\n' + 'file: ' + self.config_path)

        return retval

