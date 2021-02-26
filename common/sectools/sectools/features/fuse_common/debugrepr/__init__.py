# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================


import datetime
from sectools.common.utils import c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import store_data_to_file
from sectools.features.fuse_common import defines


def dump_debug_data_model(data_model, path, debug, TOOL_NAME):
    """Dumps the data model related debug data into the output directory.

    :param obj data_model: Data model to dump debug info for
    """
    if not debug:
        return

    try:
        debug_dir = c_path.join(path, defines.DEST_DEBUG_DIR)
        c_path.create_dir(debug_dir)
        debug_file = c_path.join(debug_dir, defines.DEST_DEBUG_FILE_DATA_MODEL_REPR.format(TOOL_NAME))
        store_data_to_file(debug_file, repr(data_model))
        logger.info('Dumped debug data model repr at: ' + debug_file + ', date & time: ' + datetime.datetime.now().strftime('%c'))
    except Exception as e:
        logger.warning('Failed to store debug logs: ' + str(e))


def dump_debug_secdat(secdat_parser, path, debug):
    """Dumps the secdat related debug data into the output directory.

    :param obj secdat_parser: Parser to dump debug info for
    """
    if not debug:
        return

    try:
        debug_dir = c_path.join(path, defines.DEST_DEBUG_DIR)
        c_path.create_dir(debug_dir)
        secdat_repr_log = c_path.join(debug_dir, defines.DEST_DEBUG_FILE_SECDAT_REPR)
        store_data_to_file(secdat_repr_log, repr(secdat_parser))
        logger.info('Dumped debug secdat repr at: ' + secdat_repr_log  + ', date & time: ' + datetime.datetime.now().strftime('%c'))
    except Exception as e:
        logger.warning('Failed to store debug logs: ' + str(e))