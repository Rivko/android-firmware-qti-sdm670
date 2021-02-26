# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os

from sectools.common.utils.c_logging import logger

# Import the discovery factory
from sectools.common.crypto.discovery import discovery_factory
from sectools.common.crypto.discovery import defines as modules

# Import all the discovery implementations
from sectools.common.crypto.discovery import openssl
from sectools.common.crypto.discovery import m2crypto
from sectools.common.crypto.discovery import crypto_cbc
from sectools.common.crypto.discovery import crypto_ccm
from sectools.common.crypto.discovery import ecies

# Import the functions factory
from sectools.common.crypto import functions
from sectools.common.crypto.functions import create_func_factory

# Initialize and get the function factory
func_factory = create_func_factory(discovery_factory)


def import_function(function_name):
    __import__(".".join([__name__, "functions", function_name]))

    if function_name in globals():
        return

    try:
        globals()[function_name] = func_factory.get_impl(function_name)
    except Exception as e:
        globals()[function_name] = None
        logger.debug(e)


FUNCTIONS_PATH = os.path.join(
    os.path.realpath(os.path.dirname(__file__)), "functions")

# Set utils if supported first, because others depend on it.
if os.path.isdir(os.path.join(FUNCTIONS_PATH, "utils")):
    import_function("utils")

for package in os.listdir(FUNCTIONS_PATH):
    if not os.path.isdir(os.path.join(FUNCTIONS_PATH, package)):
        continue
    if package in ["__init__", "utils", "__pycache__"]:
        continue
    import_function(package)

# The following assignments allow IDE to resolve imports of them.
# They are not functionally required.
utils = globals()["utils"]
ecdsa = globals()["ecdsa"]
cert = globals()["cert"]
rsa = globals()["rsa"]


def are_available(in_modules):
    # Check if the modules are available
    missing_modules = [m for m in in_modules if not discovery_factory.supports(m)]
    if missing_modules:
        if modules.MOD_OPENSSL in missing_modules:
            missing_modules.remove(modules.MOD_OPENSSL)
            missing_modules.insert(
                0, modules.MOD_OPENSSL + ' v' +
                   ".".join([str(x) for x in openssl.OPENSSL_VERSION_MIN]))
        raise RuntimeError(', '.join(missing_modules) + ' ' +
                           ('is' if len(missing_modules) == 1 else 'are') +
                           ' unavailable.')
