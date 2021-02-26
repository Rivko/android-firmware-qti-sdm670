#=============================================================================
#
#                                    build_xml.py
#
# GENERAL DESCRIPTION
#
# EXTERNAL FUNCTIONS
#        None.
#
# INITIALIZATION AND SEQUENCING REQUIREMENTS
#        None.
#
#             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
#=============================================================================
#
#  We actually push the parsing of <build> elements into qurt-image-build.py
#   and import that parsing functionality here.  This way we don't maintain
#   two separate locations for parsing the same elements.
#

def collect_build_element(cfg):
    import imp, os, sys

    qib = None

    # See if it's already loaded as a module

    if qib == None:
        try:
            qib = sys.modules['qib']
        except KeyError:
            pass

    # See if we can load it from qurt-image-build.py

    if qib == None:
        try:
            qib_file = os.path.join(os.path.dirname(sys.argv[0]),'qurt-image-build.py')
            qib = imp.load_source('qib', qib_file)
        except IOError:
            pass

    # If we can't find the module, ignore it and hope that's okay

    if qib != None:
        qib.parse_build_elements(cfg, cfg.build_elements)

