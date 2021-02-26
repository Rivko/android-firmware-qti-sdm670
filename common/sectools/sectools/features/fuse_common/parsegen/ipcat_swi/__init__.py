# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Mar 14, 2014

@author: francisg
'''

from .. import BaseDataParseGen
from ...datamodel.auto_gen_model import Cfg_Fuseblower, Cfg_Module, Cfg_Fuse_Region, Cfg_Fuse, Cfg_Field
from .....common.utils import c_path
from .....common.utils.c_logging import logger

class StateMachine(object):
    '''
    helper class that maintains the state machine
    and handles the input based on state
    '''

    def __init__(self, dm):
        self.data_model = dm
        self.current_module_id = None
        self.current_module_base = None
        self.current_region_name = None
        self.current_region_type = None
        self.current_region = None
        self.current_fuse_row = None
        self.current_fuse_row_type = None
        self.state = None
        pass

    def get_state(self):
        return self.state

    def next_state(self, line):
        line = line.strip('\n')
        level = line.count('\t')

        state = self.state
        if state is None:

            if level == 0 and 'SECURITY_CONTROL':
                state = 'MODULE'
            else:
                state = 'INVALID'
                raise RuntimeError('Invalid state: from state=%s, SECURITY_MODULE not found' %str(state))

        elif state is 'MODULE':

            if level == 0 and ' ARRAY ' in line:
                state = 'ARRAY_MULTIROW_REGION'
            elif level == 0 and '_ROW' in line:
                state = 'MULTIROW_REGION'
            elif level == 0 and ' ADDRESS ' in line:
                state = 'SINGLEROW_REGION'
            elif level == 0 and 'OFFSET' in line:
                pass
            else:
                state = 'INVALID'
                raise RuntimeError('Invalid state: from state=%s, line:%s\n' %(str(state), line))

        elif state is 'ARRAY_MULTIROW_REGION':

            if level == 0 and (' ADDRESS ' in line or ' RESET_VALUE ' in line):
                # stay in this state
                pass
            elif level == 1 and ' BIT[' in line:
                state = 'FIELD'
            else:
                state = 'INVALID'
                raise RuntimeError('Invalid state: from state=%s, line:%s\n' %(str(state), line))

        elif state is 'MULTIROW_REGION':

            if level == 0 and 'RESET_VALUE' in line:
                # stay in this state
                pass
            elif level == 0 and '_MSB' in line:
                # stay in this state
                pass
            elif level == 1 and ' BIT[' in line:
                state = 'FIELD'
            else:
                state = 'INVALID'
                raise RuntimeError('Invalid state: from state=%s, line:%s\n' %(str(state), line))

        elif state is 'SINGLEROW_REGION':

            if level == 0 and 'RESET_VALUE' in line:
                # stay in this state
                pass
            elif level == 1 and ' BIT[' in line:
                state = 'FIELD'
            else:
                state = 'INVALID'
                raise RuntimeError('Invalid state: from state=%s, line:%s\n' %(str(state), line))

        elif state is 'FIELD':

            if level == 0 and ' ARRAY ' in line:
                state = 'ARRAY_MULTIROW_REGION'
            elif level == 0 and '_ROW' in line:
                state = 'MULTIROW_REGION'
            elif level == 0 and ' ADDRESS ' in line:
                state = 'SINGLEROW_REGION'
            elif level == 1 and ' BIT[' in line:
                pass  # do nothing
            elif level == 2:
                pass
            else:
                state = 'INVALID'
                raise RuntimeError('Invalid state: from state=%s, line:%s\n' %(str(state), line))

        self.state = state
        return self.state

    def handle_input(self, line):
        line = line.strip('\n')

        if self.state is 'MODULE':
            if ' BASE ' in line:
                module = Cfg_Module()
                splitted = line.split(' BASE ')
                module.id = splitted[0]+'_CORE'
                splitted = splitted[1].split(' ')
                module.base_address = splitted[0]

                self.current_module_id = module.id
                self.current_module_base = module.base_address
                self.data_model.add_module(module)

        elif self.state is 'SINGLEROW_REGION':
            if 'ADDRESS' in line:
                self.current_region_type = 'SINGLEROW_REGION'
                if '_MSB' in line:
                    # if this is a MSB line, no need to add new fuse as it was added for LSB
                    self.current_fuse_row_type = 'MSB'
                    return
                else:
                    self.current_fuse_row_type = 'SINGLE'

                splitted = line.split(' ADDRESS ')
                self.current_region_name = splitted[0].strip('_LSB').strip('_MSB')
                splitted = splitted[1].split(' ')
                address = splitted[0]
                self.current_fuse_row = '0'

                try:
                    # add the single fuse
                    fuse = Cfg_Fuse()
                    fuse.n = 0
                    fuse.operation = 'BLOW'
                    fuse.address = hex(int(self.current_module_base,16) + int(address,16))

                    self.current_region = Cfg_Fuse_Region()
                    self.current_region.id = self.current_region_name
                    self.current_region.add_fuse(fuse)

                    # add the new region into the module
                    self.data_model.get_module(self.current_module_id).add_fuse_region(self.current_region)

                except Exception, e:
                    raise RuntimeError('SINGLEROW_REGION ERROR: %s' %e)

        elif self.state is 'MULTIROW_REGION':
            if 'ADDRESS' in line:
                self.current_region_type = 'MULTIROW_REGION'
                # get the region name
                splitted = line.split('_ROW')

                region_name = splitted[0]
                splitted = splitted[1].split(' ADDRESS ')
                row, row_type = splitted[0].split('_')
                addr_splitted = splitted[1].split(' ')
                address = addr_splitted[0]

                logger.debug2('name:%s, type: %s, row:%s, address:%s' %(region_name, row_type, row, address))

                # check if the current line's region name is the current name
                if self.current_region_name != region_name:
                    # new multirow region
                    # create new region and fuse
                    fuse = Cfg_Fuse()
                    fuse.n = int(row)
                    fuse.operation = 'BLOW'
                    fuse.address = hex(int(self.current_module_base,16) + int(address,16))

                    self.current_region = Cfg_Fuse_Region()
                    self.current_region.id = region_name
                    self.current_region.add_fuse(fuse)

                    # add the new region into the module
                    self.data_model.get_module(self.current_module_id).add_fuse_region(self.current_region)

                elif self.current_fuse_row != row:
                    # same region, but row number change, should be LSB
                    if row_type != 'LSB':
                        raise RuntimeError('Unexpected type for new row: name=%s, type=%s, row=%s, address:%s' %(region_name, row_type, row, address))

                    # create new fuse row
                    fuse = Cfg_Fuse()
                    fuse.n = int(row)
                    fuse.operation = 'BLOW'
                    fuse.address = hex(int(self.current_module_base,16) + int(address,16))
                    self.current_region.add_fuse(fuse)

                else:
                    # same region, same row, should be MSB
                    if row_type != 'MSB':
                        raise RuntimeError('Unexpected type for existing row: name=%s, type=%s, row=%s, address:%s' %(region_name, row_type, row, address))

                self.current_region_name = region_name
                self.current_fuse_row_type = row_type
                self.current_fuse_row = row

        elif self.state is 'ARRAY_MULTIROW_REGION':
            if ' ARRAY ' in line:
                self.current_region_type = 'ARRAY_MULTIROW_REGION'

                splitted = line.split(':')

                # get the region name
                region_name = splitted[0].split('n',1)[0]
                region_name = region_name.strip('_ROW')

                splitted = splitted[1].split(' ARRAY ')
                # should now have something like this
                # splitted = [ '(0)-(3)' , '0x00000058+0x8*n' ]

                # check how many rows are needed
                row_from, row_to = splitted[0].split('-')
                row_from = row_from.strip('(').strip(')')
                row_to = row_to.strip('(').strip(')')
                #total_rows = int(row_to) - int(row_from) + 1

                # get the address and bit field width
                addr_splitted = splitted[1].split('+')
                address = addr_splitted[0]
                offset = addr_splitted[1].strip('*n')

                # check if the current line's region name is the current name
                if self.current_region_name != region_name:
                    # new array multirow region
                    # create new region and fuses
                    self.current_region = Cfg_Fuse_Region()
                    self.current_region.id = region_name

                    for i in range(int(row_from),int(row_to)+1):
                        fuse = Cfg_Fuse()
                        fuse.n = i
                        fuse.operation = 'BLOW'
                        fuse.address = hex(int(self.current_module_base,16) + int(address,16) + int(offset,16)*i)
                        self.current_region.add_fuse(fuse)

                    # add the new region into the module
                    self.data_model.get_module(self.current_module_id).add_fuse_region(self.current_region)

                if '_MSB' in line:
                    row_type = 'MSB'
                elif '_LSB' in line:
                    row_type = 'LSB'
                else:
                    row_type = '32BIT'

                self.current_region_name = region_name
                self.current_fuse_row_type = row_type
                self.current_fuse_row = None

        elif self.state is 'FIELD':
            if ' BIT[' in line:
                try:
                    # new field
                    offset = 0

                    if self.current_fuse_row_type == 'MSB':
                        # add offset
                        offset = 32

                    splitted = line.split(' BIT',1)
                    field = Cfg_Field()
                    field.id = splitted[0].strip('\t')
                    if not splitted[1].endswith(']'):
                        splitMore = splitted[1].split()
                        splitted[1] = splitMore[0]
                    bits = splitted[1].strip('[').strip(']')

                    if ':' in bits:
                        bits_to, bits_from = bits.split(':')
                        bits_to = int(bits_to) + offset
                        bits_from = int(bits_from) + offset
                        bits = str(bits_to) + ':' + str(bits_from)
                    else:
                        bits = str(int(bits) + offset)

                    field.bits = bits
                    field.owner = 'QTI'

                    if self.current_region_type == 'ARRAY_MULTIROW_REGION':
                        # add to every fuse row for ARRAY_MULTIROW_REGION
                        for fuse in self.current_region.fuse:
                            fuse.add_field(field)
                    else:
                        # add the new field into current fuse
                        self.current_region.get_fuse(int(self.current_fuse_row)).add_field(field)

                except Exception, e:
                    raise RuntimeError('FIELD ERROR: %s' %e)




class IpcatSwiParser(BaseDataParseGen):
    '''
    IpcatSwiParser
    '''
    def __init__(self):
        '''
        Constructor
        '''
        self.file_path = None
        self.state_machine = None
        self._data_model = Cfg_Fuseblower()
        pass

    @property
    def data_model(self):
        return self._data_model

    @data_model.setter
    def data_model(self, dm):
        self._data_model = dm

    def read(self, file_path):
        """
        reads a file an swi file from ipcat and fills in data model
        """
        self.file_path = c_path.normalize(file_path)

        swi = open(self.file_path, 'r')
        lines = swi.readlines()
        swi.close()

        # create state machine
        self.state_machine = StateMachine(self.data_model)

        for line in lines:
            logger.debug2( 'Input: %s' %line.strip('\n'))
            logger.debug2('Before: %s' %str(self.state_machine.get_state()))

            self.state_machine.next_state(line)
            self.state_machine.handle_input(line)

            logger.debug2('After: %s' %self.state_machine.get_state())

        self.data_model.sort()


    def write(self, file_path):
        pass


#------------------------------------------------------------------------------
# Restrict all import
#------------------------------------------------------------------------------
__all__ = ['IpcatSwiParser',
          ]
