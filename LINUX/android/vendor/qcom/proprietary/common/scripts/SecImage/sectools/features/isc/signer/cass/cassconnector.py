# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import os
import shutil

from __secfile__ import DIR_PATH
from sectools.common.utils import c_misc, c_path
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_process import CoreSubprocess
from sectools.features.isc.signer.base_signer import ExternalSignerError
from sectools.features.isc.signer.cass.identity_keystore import IdentityKeystoreFactory
from sectools.features.isc.signer.cass.trust_keystore import TrustKeystore


CASSCONNECTOR_DIR = DIR_PATH

class CassConnector(object):
    CASS_CLIENT_REFAPP_DIR = os.path.join(CASSCONNECTOR_DIR, 'cass-client-refapp')
    CLIENT_REFAPP_JAR = os.path.join('bin', 'java', 'cass-client-refapp.jar')
    CASS_CLIENT_REFAPP_CONFIG_DIR = os.path.join(CASS_CLIENT_REFAPP_DIR, 'config')
    CASS_CLIENT_REFAPP_INPUT_DIR = os.path.join(CASS_CLIENT_REFAPP_DIR, 'input')
    CASS_CLIENT_REFAPP_OUTPUT_DIR = os.path.join(CASS_CLIENT_REFAPP_DIR, 'output')
    SCHEMA_XSD_FILEPATH = os.path.join(CASS_CLIENT_REFAPP_CONFIG_DIR, 'signature_package.xsd')
    SIGNATUREPACKPAGE_RELPATH = os.path.join("signingpackage", "signingpackage.xml")

    DEFAULT_HOST_VALIDATION_MODE = "STAGE_PROD"
    TIMEOUT = 300

    MESG_TIMEOUT = "Connection to CASS server timeout after {0} seconds"
    MESG_WRONG_JAVA_VERSION = 'JRE 32 bit (1.6 or 1.7) is required to provide SunPKCS11 implementation.\n' \
                              'Run "java -version" and check that JRE 32 bit (1.6 or 1.7) is installed and in the path environment variable.\n' \
                              'Run "which java" to make sure the path points to the correct version.'\
                              'If it points to c:\\Windows\\system32\\java, remove the java binary from c:\\Windows\\system32 and add the explicit path to the path environment variable'

    ERR_WRONG_JAVA_VERSION = "java.lang.NoClassDefFoundError: sun/security/pkcs11/SunPKCS11"
    ERR_WRONG_JAVA_VERSION2 = "java.lang.NoClassDefFoundError: sun/security/pkcs11/wrapper/PKCS11"
    ERR_UNABLE_TO_ACCESS_KEYSTORE = "UNABLE TO ACCESS KEYSTORE"

    def __init__(self, cass_signer_attributes):
        self.cass_signer_attributes = cass_signer_attributes
        self.identity_keystore = IdentityKeystoreFactory.getKeyStore(
                                    self.cass_signer_attributes.user_identity)
        self.trust_keystore = None
        self.host_validation_mode = self.DEFAULT_HOST_VALIDATION_MODE
        if self.cass_signer_attributes.server:
            self.trust_keystore = TrustKeystore(
                    filepath=self.cass_signer_attributes.server.trust_keystore,
                    password=self.cass_signer_attributes.server.trust_keystore_password,
                    keystoreType=self.cass_signer_attributes.server.trust_keystore_type)
            self.host_validation_mode = self.cass_signer_attributes.server.host_validation_mode

    @property
    def host_validation_mode(self):
        return self._host_validation_mode

    @host_validation_mode.setter
    def host_validation_mode(self, value):
        if value:
            self._host_validation_mode = value
        else:
            self._host_validation_mode = self.DEFAULT_HOST_VALIDATION_MODE


    def sign(self, signingpackage_fname, outputdir):
        signaturepackage_binary = None
        cmds = self._getCmds(signingpackage_fname, outputdir)
        cass_server = self.cass_signer_attributes.server.host if self.cass_signer_attributes.server else "default CASS server"
        logger.info("Connecting to {0}".format(cass_server))
        logger.debug("Executing commands: \n'" + "\n".join(cmds) + "'\n")
        output = self._executeCmds(cmds)
        logger.debug(output)
        signaturepackage_filepath = os.path.join(outputdir, self.SIGNATUREPACKPAGE_RELPATH)
        if os.path.isfile(signaturepackage_filepath):
            logger.info("Signature package retrieved from server: " + c_path.normalize(signaturepackage_filepath))
            signaturepackage_binary = c_misc.load_data_from_file(signaturepackage_filepath)

            # clean up
            path, filename = os.path.split(signaturepackage_filepath)
            shutil.rmtree(path)
        return signaturepackage_binary

    def _formatpath(self, path):
        return c_path.normalize(path)

    def _getCmds(self, signingpackage_fname, outputdir):
        if os.path.exists(outputdir) is None:
            os.makedirs(outputdir)

        signingpackage_file = self._formatpath(signingpackage_fname)
        cmds = ["java"]
        if self.cass_signer_attributes.server:
            cmds.append("-Dcass_server.host={0}".format(self.cass_signer_attributes.server.host))
            cmds.append("-Dcass_server.sslport={0}".format(self.cass_signer_attributes.server.port))
            cmds.append("-Dtrust_anchor.file={0}".format(self.trust_keystore.file))
            cmds.append("-Dtrust_anchor.password={0}".format(self.trust_keystore.password))
            cmds.append("-Dtrust_anchor.keystoretype={0}".format(self.trust_keystore.type))
            cmds.append("-Dcn_verify.mode={0}".format(self.host_validation_mode))
        cmds.append("-Dsig_authority.file={0}".format(self.identity_keystore.file))
        cmds.append("-Dsig_authority.password={0}".format(self.identity_keystore.password))
        cmds.append("-Dsig_authority.keystoretype={0}".format(self.identity_keystore.type))
        if self.identity_keystore.token_driver_home:
            cmds.append("-Detoken_driver_home={0}".format(self.identity_keystore.token_driver_home))
        cmds.append("-Doutput.location={0}".format(outputdir))
        cmds.append("-Dschema.location={0}".format(self._formatpath(self.SCHEMA_XSD_FILEPATH)))

        cmds.extend(['-jar', self.CLIENT_REFAPP_JAR, signingpackage_file])

        # only enable -v if debug is enabled
        if logger.verbosity < logger.DEBUG:
            cmds.append('-v')

        return cmds

    def _process_error(self, output):
        error_mesg = output

        if (output.lower().find(self.ERR_WRONG_JAVA_VERSION.lower()) > 0 or
            output.lower().find(self.ERR_WRONG_JAVA_VERSION2.lower()) > 0):
            error_mesg = self.MESG_WRONG_JAVA_VERSION
        elif (output.lower().find(self.ERR_UNABLE_TO_ACCESS_KEYSTORE.lower()) > 0):
            self.identity_keystore.reset_password()

        return error_mesg

    def _executeCmds(self, cmds):
        # Need to run in cass-client-refapp directory
        returnValue, returnError, f_timeout, f_retcode, f_output = CassCoreSubprocess.executeCommand(
                            launchCommand=cmds,
                            retcode=0,
                            timeout=self.TIMEOUT,
                            successString="SUCCESS",
                            workingDir=self.CASS_CLIENT_REFAPP_DIR,
                            stderr=CoreSubprocess.STDERR_STDOUT)

        if returnValue is False:
            if f_timeout is True:
                raise ExternalSignerError(self.MESG_TIMEOUT.format(self.TIMEOUT))
            else:
                error_mesg = self._process_error(f_output)
                raise ExternalSignerError(error_mesg)
        return f_output

class CassCoreSubprocess(CoreSubprocess):

    def _maskPassword(self, string_to_replace):
        string_replaced = string_to_replace
        keyword_to_search = "password="

        try:
            index_start = string_to_replace.index(keyword_to_search) + len(keyword_to_search)
            index_end = len(string_to_replace)
        except ValueError:
            # It is ok if password is not found
            pass
        else:
            value = '*' * (index_end - index_start)
            string_replaced = string_to_replace[:index_start] + str(value) + string_to_replace[index_end + 1:]

        return string_replaced

    # This is a hook for caller to format the command
    # line string for printing
    def formatLaunchCommandForPrinting(self, cmd):
        formatted_cmd = []

        for eachArg in cmd:
            eachArg = self._maskPassword(eachArg)
            formatted_cmd.append(eachArg)

        return formatted_cmd

    # Suppress printing Finish command
    def printFinish(self, cmd):
        pass
