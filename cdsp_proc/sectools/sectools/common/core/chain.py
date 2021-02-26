# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

"""
Creates the chain of objects in the correct sequence.
#. Imports all modules in the correct order to create all the singletons.
#. Discovers the plugins.
"""

import traceback

from sectools.common.utils.c_logging import logger


def initialize():
    """Imports all the modules in the chain in the correct order for the
    generation of the singletons and discovery of plugins.
    """
    # Initialize the feature manager
    logger.debug('Initializing the feature manager')
    import sectools.common.core.feature

    # Initialize the interfaces
    logger.debug('Initializing all interfaces')
    try:
        import sectools.intf.ui.manager
    except ImportError:
        logger.debug('UI interface is unavailable.')
    except Exception:
        logger.note('UI interface implementation crashed:' + '\n'
                    '' + traceback.format_exc())
    else:
        logger.debug('UI interface is available.')

    # Initialize the features
    logger.debug('Initializing all features')
    try:
        import sectools.features.fbc.fbc
    except ImportError:
        logger.debug('FuseBlower is unavailable.')
    except Exception:
        logger.note('FuseBlower implementation crashed:' + '\n'
                    '' + traceback.format_exc())
    else:
        logger.debug('FuseBlower is available.')

    try:
        import sectools.features.isc
    except ImportError:
        logger.debug('SecImage is unavailable.')
    except Exception:
        logger.note('SecImage implementation crashed:' + '\n'
                    '' + traceback.format_exc())
    else:
        logger.debug('SecImage is available.')

    try:
        import sectools.features.ltf
    except ImportError:
        logger.debug('LocalTF is unavailable.')
    except Exception:
        logger.note('LocalTF implementation crashed:' + '\n'
                    '' + traceback.format_exc())
    else:
        logger.debug('LocalTF is available.')
