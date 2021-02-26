# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

'''
Created on Dec 18, 2015

@author: coryf
'''

import json
import sys
import urllib2


def query_server(url, data=None, cert_file=None):
    """
    @param str url: URL of server to connect to
    @param dict data: optional data to include in the request
    @param str cert_file: optional path to PEM certificate file which will be used to validate the identity of the server

    Allows for connecting to a server with a provided URL and returns a response dictionary.
    """

    # Check if data is not given
    if data is None:
        data = {}

    # Make the url request
    headers = {'Content-Type': 'application/json'}

    # If no data is provided urllib2 will use GET method. If data is provided urllib2 will use POST method.
    if data is not None and len(data) > 0:
        request_data = json.dumps(data)
    else:
        request_data = None
    req = urllib2.Request(url, request_data, headers)

    try:
        # Enforce Python version number when urllib2 added support for cafile parameter
        if sys.version_info < (2, 7, 9):
            if cert_file:
                raise RuntimeError("Python version 2.7.9 or later is required to establish HTTPS connection with server.")
            response = urllib2.urlopen(req)
        else:
            response = urllib2.urlopen(req, cafile=cert_file)
    except urllib2.HTTPError, e:
        raise RuntimeError('The server could not fulfill the request.' '\n'
                           'Error code: ' + str(e.code) + '.' '\n'
                           'Please check the followings: \n \
                            - URL is correct \n \
                            - server is running \n \
                            - there is no firewall preventing connections to server')
    except urllib2.URLError, e:
        raise RuntimeError('Failed to reach the server with URL: ' + "\"" + url + "\"" '\n'
                           'Reason: ' + str(e.reason) + '.' '\n'
                           'Please check the followings: \n \
                            - URL is correct \n \
                            - server is running \n \
                            - there is no firewall preventing connections to server')

    return_data = json.loads(response.read())
    return return_data
