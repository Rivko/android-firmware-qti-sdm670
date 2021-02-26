# ===============================================================================
#
#  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import abc

from sectools.features.isc.api import func_container as fc


class SecImageAPIError(RuntimeError):
    pass


class SecImageAPIIntf(object):
    """All class methods support the following two options:

    Args:
        verbose (bool): Increase verbosity.
        debug   (bool): Store debug files.
        quiet   (bool): Reduce verbosity. (Takes precedence over "verbose")
    """

    def __init__(self, send_signattrs=False, verbose=False, debug=False,
                 quiet=False):
        """Initializes the object.

        NOTE: This method can be overridden to store server signing details.

        Args:
            send_signattrs (bool): See [cls.generatehash] description
            verbose (bool): See class description.
            debug   (bool): See class description.
            quiet   (bool): See class description
        """
        self.send_signattrs = send_signattrs
        self.verbose = verbose
        self.debug = debug
        self.quiet = quiet

    #--------------------------------------------------------------------------
    # CLIENT METHODS [SERVER SIGNING USECASE]
    #--------------------------------------------------------------------------
    def sign(self, chipset, output_dir, sign_id=None, imagefile=None,
             metabuild=None):
        """Returns the signed image.

        NOTE: This method requires "signwithserver" to be implemented in a
              derived class.

        Args:
            chipset    (str): Chipset id corresponding to the image file. This
                              is the id specified in the secimage config file.
            output_dir (str): Directory to store output files in. Write access
                              must be available to the path.
            sign_id    (str): [Optional] Sign id corresponding to the image to
                              be signed.
            imagefile  (str): [Optional] Path to the image file to be signed.
                              Read access must be available to the path.
            metabuild  (str): [Optional] Path to the metabuild directory. Read
                              access must be available to the path.

            NOTE: One of imagefile, metabuild must be given. sign_id must be
                  given for metabuild signing and may be needed for imagefile
                  signing if the image file name doesnt conform to the sectools
                  preconfigured name.

        Returns:
            A tuple containing the return code, error string (if any error),
            the path to the signed image and the path that the signed image
            is expected to be in the build system.

            retcode       (int): 0 - if successful in signing the image file.
                                 non-zero - otherwise.
            errstr        (str): error message - if retcode is non-zero.
                                 empty - if retcode is 0.
            signed_image  (str): path to the signed image file
            expected_path (str): path that the signed image file is expected
                                 to be placed in.
        """
        # Initialize the values
        signed_image = ''
        expected_path = ''

        # Get the hash package
        retcode, errstr, hash_package = \
            self.generatehash(chipset, output_dir, sign_id, imagefile,
                              metabuild, self.send_signattrs,
                              **self._classmethod_kwargs)

        # Sign the hash package with the server
        if retcode == 0:
            retcode, errstr, sig_package = \
                self.signwithserver(chipset, output_dir, sign_id, hash_package,
                                    **self._classmethod_kwargs)

        # Generate the signed image
        if retcode == 0:
            retcode, errstr, signed_image, expected_path = \
                self.generatesigned(chipset, output_dir, sig_package, sign_id,
                                    imagefile, metabuild,
                                    **self._classmethod_kwargs)

        # Return
        return retcode, errstr, signed_image, expected_path

    @classmethod
    def generatehash(cls, chipset, output_dir, sign_id=None, imagefile=None,
                     metabuild=None, send_signattrs=False,
                     verbose=False, debug=False, quiet=False):
        """Returns the hash for the image file that should be signed.

        Args:
            chipset    (str): Chipset id corresponding to the image file. This
                              is the id specified in the secimage config file.
            output_dir (str): Directory to store output files in. Write access
                              must be available to the path.
            sign_id    (str): [Optional] Sign id corresponding to the image to
                              be signed.
            imagefile  (str): [Optional] Path to the image file to be signed.
                              Read access must be available to the path.
            metabuild  (str): [Optional] Path to the metabuild directory. Read
                              access must be available to the path.
            send_signattrs (bool): Packages the signing attributes from the
                              config file along with the hash so that the client
                              can request the server to use client's signing
                              attributes instead of server's.

            NOTE: One of imagefile, metabuild must be given. sign_id must be
                  given for metabuild signing and may be needed for imagefile
                  signing if the image file name doesnt conform to the sectools
                  preconfigured name.

            NOTE: [send_signattrs] works in conjuction with
                  [cls.generatesigpack:accept_signattrs]. Setting
                  [send_signattrs] flag to true will only work if the server
                  allows signing attributes to be overridden. If the flag is set
                  to true and server doesnt allow overriding attributes, an
                  error will be returned from the server.

        Returns:
            A tuple containing the return code, error string (if any error) and
            the hash corresponding to the image file to be signed.

            retcode (int): 0 - if successful in generating hash for the image.
                           non-zero - otherwise.
            errstr  (str): error message - if retcode is non-zero.
                           empty - if retcode is 0.
            hash_package (str): path to the hash package to be signed on the
                                remote server
        """
        return cls._run_method(fc.generatehash, chipset, output_dir, sign_id,
                               imagefile, metabuild, send_signattrs, verbose,
                               debug, quiet)

    @abc.abstractmethod
    def signwithserver(self, chipset, output_dir, sign_id, hash_package,
                       verbose=False, debug=False, quiet=False):
        """Returns the signature package corresponding to the hash package
        signed by the server.

        NOTE: This method is responsible for:
              1. Uploading the hash_package to the server
              2. Triggering the server to sign the hash_package with the
                 server keys
              3. Downloading the sig_package generated by the server.

        Args:
            chipset      (str): Chipset id corresponding to the image file. This
                                is the id specified in the secimage config file.
            output_dir   (str): Directory to store output files in. Write access
                                must be available to the path.
            sign_id      (str): Sign id corresponding to the image to be signed.
            hash_package (str): Hash package to be signed

        Returns:
            A tuple containing the return code, error string (if any error) and
            the path to the signature/certs zip package.

            retcode     (int): 0 - if successful in signing the hash.
                               non-zero - otherwise.
            errstr      (str): error message - if retcode is non-zero.
                               empty - if retcode is 0.
            sig_package (str): path to the signature/certs zip package.
        """
        raise NotImplementedError('Signing with server is not implemented.')

    @classmethod
    def generatesigned(cls, chipset, output_dir, sig_package,
                       sign_id=None, imagefile=None, metabuild=None,
                       verbose=False, debug=False, quiet=False):
        """Returns the signed image file.

        Args:
            chipset     (str): Chipset id corresponding to the image file. This
                               is the id specified in the secimage config file.
            output_dir  (str): Directory to store output files in. Write access
                               must be available to the path.
            sig_package (str): Path to the signature/certs zip package.
            sign_id     (str): [Optional] Sign id corresponding to the image to
                               be signed.
            imagefile   (str): [Optional] Path to the image file to be signed.
                               Read access must be available to the path.
            metabuild   (str): [Optional] Path to the metabuild directory. Read
                               access must be available to the path.

            NOTE: One of imagefile, metabuild must be given. sign_id must be
                  given for metabuild signing and may be needed for imagefile
                  signing if the image file name doesnt conform to the sectools
                  preconfigured name.

            NOTE: The parameters for this API should be the same as the
                  parameters used for generatehash API.

        Returns:
            A tuple containing the return code, error string (if any error),
            the path to the signed image and the path that the signed image
            is expected to be in the build system.

            retcode       (int): 0 - if successful in signing the image file.
                                 non-zero - otherwise.
            errstr        (str): error message - if retcode is non-zero.
                                 empty - if retcode is 0.
            signed_image  (str): path to the signed image file
            expected_path (str): path that the signed image file is expected
                                 to be placed in.
        """
        return cls._run_method(fc.generatesigned, chipset, output_dir,
                               sig_package, sign_id, imagefile, metabuild,
                               verbose, debug, quiet)

    #--------------------------------------------------------------------------
    # SERVER METHODS [SERVER SIGNING USECASE]
    #--------------------------------------------------------------------------
    @classmethod
    def generatesigpack(cls, output_dir, hash_package,
                        accept_signattrs=False, verbose=False, debug=False,
                        quiet=False):
        """Returns the signature/certs package for the hash provided.

        Args:
            output_dir   (str): Directory to store output files in. Write access
                                must be available to the path.
            hash_package (str): Hash package to be signed
            accept_signattrs (bool): Setting this flag to True allows the client
                                to send signing attributes which are then used
                                as overrides over the server's signing
                                attributes.

            NOTE: [accept_signattrs] works in conjuction with
                  [cls.generatehash:send_signattrs].

        Returns:
            A tuple containing the return code, error string (if any error) and
            the path to the signature/certs zip package.

            retcode     (int): 0 - if successful in signing the hash.
                               non-zero - otherwise.
            errstr      (str): error message - if retcode is non-zero.
                               empty - if retcode is 0.
            sig_package (str): path to the signature/certs zip package

            NOTE: Naming convention for the sig_package files:
                  <chipset>_<sign_id>.zip
                  -> <chipset>_<sign_id>-attestation_ca_cert.cer
                  -> <chipset>_<sign_id>-attestation_cert.cer
                  -> <chipset>_<sign_id>-root_cert.cer
                  -> <chipset>_<sign_id>-signature.bin

            NOTE: Certificate files will be in DER format.
        """
        return cls._run_method(fc.generatesigpack, output_dir, hash_package,
                               accept_signattrs, verbose, debug, quiet)

    #--------------------------------------------------------------------------
    # OTHER PUBLIC METHODS
    #--------------------------------------------------------------------------
    @classmethod
    def getmetabuildimages(cls, chipset, metabuild, verbose=False, debug=False,
                           quiet=False):
        """Returns the images that need to be signed from the given metabuild.

        Args:
            chipset   (str): Chipset id corresponding to the metabuild. This is
                             the id specified in the secimage config file.
            metabuild (str): Path to the metabuild directory. Read access must
                             be available to the path.

        Returns:
            A tuple containing the return code, error string (if any error) and
            a dict mapping sign id to the corresponding path/status from the
            metabuild info.

            retcode (int): 0 - if successful in retrieving image info from
                               metabuild.
                           non-zero - otherwise.
            errstr  (str): error message - if retcode is non-zero.
                           empty - if retcode is 0.
            images (dict): information on retrieving the image for a sign id
                           {
                               <sign_id (str)> :
                               {
                                   'retcode' : 0,
                                   'errstr' : '',
                                   'path' : '/path/to/image',
                               }
                           }
                           where 'retcode' - 0 if image path was found and can
                               be accessed.
        """
        return cls._run_method(fc.getmetabuildimages, chipset, metabuild,
                               verbose, debug, quiet)

    @classmethod
    def validateimage(cls, chipset, sign_id=None, root_cert_hash=None,
                      imagefile=None, metabuild=None, verbose=False,
                      debug=False, quiet=False):
        """Checks the following to validate image file signing:
            1. Check if image file is signed.
            2. Check integrity of cert chain.
            3. Check decrypted signature matches image hash.
            4. Check the signing attributes match the ones from config file.
            5. Check the image is signed with the root cert corresponding to the
               given root_cert_hash

        Args:
            chipset   (str): Chipset id corresponding to the image file. This
                             is the id specified in the secimage config file.
            sign_id   (str): [Optional] Sign id corresponding to the image to
                             be signed.
            root_cert_hash (str): [Optional] Hash of the root cert used for
                                  signing.
            imagefile (str): [Optional] Path to the image file to be signed.
                             Read access must be available to the path.
            metabuild (str): [Optional] Path to the metabuild directory. Read
                             access must be available to the path.

            NOTE: One of imagefile, metabuild must be given. sign_id must be
                  given for metabuild signing and may be needed for imagefile
                  signing if the image file name doesnt conform to the sectools
                  preconfigured name.

            NOTE: If root_cert_hash is not given, #5 validation is not done.

        Returns:
            A tuple containing the return code, error string (if any error),
            the path to the signed image and the path that the signed image
            is expected to be in the build system.

            retcode       (int): 0 - if successful in validating the image file.
                                 non-zero - otherwise.
            errstr        (str): error message - if retcode is non-zero.
                                 empty - if retcode is 0.
        """
        return cls._run_method(fc.validateimage, chipset, sign_id,
                               root_cert_hash, imagefile, metabuild, verbose,
                               debug, quiet)

    #--------------------------------------------------------------------------
    # PRIVATE METHODS
    #--------------------------------------------------------------------------
    @classmethod
    def _run_method(cls, func, *args, **kwargs):
        """Private method to launch a function as a subprocess
        """
        import multiprocessing
        pool = multiprocessing.Pool(processes=1)
        return_value = pool.apply(func, args, kwargs)
        pool.close()
        pool.join()
        return return_value

    @property
    def _classmethod_kwargs(self):
        return  {
                    'verbose' : self.verbose,
                    'debug' : self.debug,
                    'quiet' : self.quiet
                }

