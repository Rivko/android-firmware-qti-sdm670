# ===============================================================================
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import json
import sys
import urllib2

from sectools.common.rpc import BaseServer, BaseClient, decode_bin_strings, encode_bin_strings

try:
    import cherrypy

    #######################
    # Server Implementation
    #######################
    class HTTPServer(BaseServer):

        def __init__(self, host='localhost', port=8080, config=None, connection=None,
                     https=False, ssl_cert=None, ssl_key=None):
            if port is None:
                # Grab open port and hold it
                import socket
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.bind(("", 0))
                s.listen(1)
                port = s.getsockname()[1]
                s.close()
                if connection:
                    connection.send(str(port))
                    connection.close()

            cherrypy.server.ssl_module = "builtin"
            cherrypy.server.socket_host = host
            cherrypy.server.socket_port = port

            # Enforce Python version number when urllib2 added support for https
            if https:
                if sys.version_info < (2, 7, 9):
                    raise RuntimeError("Python version 2.7.9 or later is required to start HTTPS server.")
                cherrypy.server.ssl_certificate = ssl_cert
                cherrypy.server.ssl_private_key = ssl_key

            cherrypy.tree.mount(self, '/', config=config)

        @classmethod
        def expose(cls, func):
            def f(self, *args, **kwargs):
                json_data = getattr(cherrypy.request, 'json', kwargs)

                # Handle bin string translations
                json_data = decode_bin_strings(json_data)

                out_json = func(self, *args, **json_data)

                # Decode bin string translations
                out_json = encode_bin_strings(out_json)

                return out_json
            return cherrypy.expose(cherrypy.tools.json_in()(cherrypy.tools.json_out()(f)))

        @classmethod
        def start(cls):
            cherrypy.engine.start()
            cherrypy.engine.block()

except ImportError:
    pass


#######################
# Client Implementation
#######################
class HTTPClient(BaseClient):

    def __init__(self, host='localhost', port=8080, ssl_cert=None):
        self.host = host
        self.port = port
        self.ssl_cert = ssl_cert

    @classmethod
    def connect(cls, func):
        def f(self, *args, **kwargs):
            if args:
                raise RuntimeError('Only supply kwargs')

            # Make the url
            url = self.host + ':' + str(self.port) + '/' + func.__name__

            # Make the url request
            data = {}
            for key, val in kwargs.items():
                data[key] = val

            # Handle bin string translations
            data = encode_bin_strings(data)

            # Get the server response
            return_data = self.query_server(url, data=data, cert_file=self.ssl_cert)

            # Decode bin string translations and return data
            return decode_bin_strings(return_data)
        return f

    @staticmethod
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

