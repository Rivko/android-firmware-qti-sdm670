import struct
import logging
logger = logging.getLogger(__name__)

DATA_RAM_BASE = 0xE0000
DATA_RAM_SIZE = 32 * 1024

def locate_by_elf(memory, debug_info):
    if 'aop_core_dump' not in debug_info.variables:
        logger.debug('Symbol "aop_core_dump" not found in ELF!')
        return None

    address = debug_info.variables['aop_core_dump'].address
    logger.debug('Found variable in ELF listed at 0x%0.8x' % address)

    raw_cookie = memory.read(address, 4)
    cookie, = struct.unpack('<L', raw_cookie)
    if cookie != 0xbaff1ed:
        logger.info('Magic core dump cookie did not match (%x != %x)!' % (cookie, 0xbaff1ed))
        return None

    return address

def locate_by_scan(memory):
    for address in xrange(DATA_RAM_BASE, DATA_RAM_BASE + DATA_RAM_SIZE, 4):
        raw_cookie = memory.read(address, 4)
        cookie, = struct.unpack('<L', raw_cookie)
        if cookie == 0xbaff1ed:
            logger.warning('rpm_core_dump located by magic cookie; not all symbols may match')
            return address

    logger.error('Could not find rpm_core_dump magic cookie!')
    return None

def locate(memory, debug_info):
    logger.info('Searching for the rpm_core_dump symbol...')

    logger.info('Checking ELF symbol first')
    address = locate_by_elf(memory, debug_info)
    if address:
        return address

    logger.info('Falling back to magic cookie scan')
    return locate_by_scan(memory)


