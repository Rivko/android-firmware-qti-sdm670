# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================
"""
Contains functions for secimage
"""

import json
import os

from sectools.common.utils import c_path
from sectools.common.utils.c_path import store_data_to_file, copyFile
from sectools.features.isc import cfgparser
from sectools.features.isc import secimage
from sectools.features.isc.cfgparser import auto_gen_xml_config
from sectools.features.isc.imageinfo import StatusInfo
from sectools.features.isc.signer.secimage_remote_client import SecimageRemoteClientSigner
from sectools.features.isc.signer.secimage_remote_server import SecimageRemoteServerSigner
from sectools.features.isc.signer.signerutils.package import Package
from sectools.features.isc.stager.meta_build_stager import MetaError


SIGNER_REMOTE = SecimageRemoteClientSigner.signer_id()
SIGNER_LOCAL = SecimageRemoteServerSigner.signer_id()

CUR_DIR = os.path.dirname(os.path.abspath(__file__))
TOP_DIR = os.path.join(CUR_DIR, '..', '..', '..', '..')


def launch_secimage(chipset=None, output_dir=None, sign_id=None,
                    imagefile=None, metabuild=None, signer=None,
                    sign=False, validate=False, verify_input=False,
                    return_isc=False, root_cert_hash=None, verbose=False,
                    debug=False, quiet=False, cfg_params=None, config=None):
    """Launches secimage with the appropriate parameters.
    """
    # Creates the args to launch secimage
    args = ['secimage']
    if chipset is not None: args += ['-p', chipset]
    if output_dir is not None: args += ['-o', output_dir]
    if sign_id is not None: args += ['-g', sign_id]
    if imagefile is not None: args += ['-i', imagefile]
    if metabuild is not None: args += ['-m', metabuild]
    if signer is not None: args += ['--cfg_selected_signer', signer]
    if root_cert_hash is not None: args += ['--rch', root_cert_hash]
    if config is not None: args += ['-c', config]
    if sign: args += ['-s']
    if validate: args += ['-a']
    if verify_input: args += ['-l']
    if verbose: args += ['-v']
    if debug: args += ['-d']
    if quiet: args += ['-zz']
    if cfg_params is not None:
        for cfg_tag, cfg_value in cfg_params.items():
            args += ['--cfg_' + cfg_tag, '' if cfg_value is None else str(cfg_value)]

    # Launch secimage
    parsed_args = secimage.parse_args(args)
    return secimage.main(parsed_args, return_isc=return_isc)

def update_config(chipset, update_cb, file_path):
    """Helps update the secimage config file
    """
    cfg_dir = cfgparser.ConfigDir(TOP_DIR)
    cfg_path = cfg_dir.get_chipset_config_path(chipset)
    cfg = cfgparser.ConfigParser(cfg_path)
    update_cb(cfg)
    cfg.generate(file_path)

def getmetabuildimages(chipset, metabuild, verbose=False, debug=False,
                       quiet=False):
    """Returns the images that need to be signed from the given metabuild.
    """
    retcode = 0
    errstr = ''
    images = {}

    try:
        # Launch secimage once to get the image info list
        isc = launch_secimage(chipset=chipset, metabuild=metabuild,
                              signer=SIGNER_LOCAL, verify_input=True,
                              return_isc=True, verbose=verbose, debug=debug,
                              quiet=quiet)

        # Check the status of the image info
        for sign_id, status in isc._stager.images_status.items():
            # Check if the path exists
            if status['errcode'] == MetaError.SUCCESS:
                path = status['path']
                if not c_path.validate_file(path):
                    status['errcode'] = MetaError.NO_ACCESS

            # Update the return value
            images[sign_id] = \
            {
                'retcode' : status['errcode'],
                'errstr' : MetaError.ERR_STR_MAP.get(status['errcode'], 'Unknown error'),
                'path' : status['path'],
            }

    except Exception as e:
        retcode = 1
        errstr = 'Exception occurred while running secimage. Exception - ' + str(e)

    return retcode, errstr, images

def generatehash(chipset, output_dir, sign_id=None, imagefile=None,
                 metabuild=None, send_signattrs=False, verbose=False,
                 debug=False, quiet=False):
    """Returns the hash for the image file that should be signed.
    """
    retcode = 0
    errstr = ''
    hash_package = ''


    try:
        # Launch secimage once to get the image info list
        il = launch_secimage(chipset=chipset, output_dir=output_dir,
                             sign_id=sign_id, imagefile=imagefile,
                             metabuild=metabuild, signer=SIGNER_LOCAL,
                             verify_input=True, verbose=verbose, debug=debug,
                             quiet=(False if verbose else True))

        # Check that the signature package isnt already present
        signature_package = SecimageRemoteClientSigner.get_signature_package_path(il[0])
        if c_path.validate_file(signature_package):
            retcode = 1
            errstr = ('Signature package ' + signature_package + ' is present. '
                      'Please remove to generate hash.')

        # Continue if no error
        if retcode == 0:

            # Check that the hash package isnt already present
            hash_package = SecimageRemoteClientSigner.get_to_sign_package_path(il[0])
            if c_path.validate_file(hash_package):
                retcode = 1
                errstr = ('Hash package ' + hash_package + ' is already present. '
                          'Please remove to re-generate hash.')

        # Continue if no error
        if retcode == 0:

            # Create new config file to update the send signing attribute
            temp_config_file = None
            if send_signattrs:
                def update_cfg_cb(config):
                    remote_config = auto_gen_xml_config.complex_remote_client_signer_attributes()
                    remote_config.send_signing_overrides = send_signattrs
                    config.root.signing.signer_attributes.remote_client_signer_attributes = remote_config
                temp_config_file = c_path.join(output_dir, 'hash_package_config.xml')
                update_config(chipset, update_cfg_cb, temp_config_file)
                chipset = None

            try:
                # Launch secimage to generate the hash package
                il = launch_secimage(chipset=chipset, output_dir=output_dir,
                                     sign_id=sign_id, imagefile=imagefile,
                                     metabuild=metabuild, signer=SIGNER_REMOTE,
                                     sign=True, verbose=verbose, debug=debug,
                                     quiet=quiet, config=temp_config_file)
            finally:
                if not debug:
                    try: os.remove(temp_config_file)
                    except Exception as e: pass

            # Verify the hash package was generated
            hash_package = SecimageRemoteClientSigner.get_to_sign_package_path(il[0])
            if not c_path.validate_file(hash_package):
                retcode = 1
                errstr = 'Failed to generate the hash package. ' + str(il[0].status.sign.error)

    except Exception as e:
        retcode = 1
        errstr = 'Exception occurred while running secimage. Exception - ' + str(e)

    return retcode, errstr, hash_package

def generatesigpack(output_dir, hash_package, accept_signattrs=False,
                    verbose=False, debug=False, quiet=False):
    """Returns the signature/certs package for the hash provided.
    """
    retcode = 0
    errstr = ''
    sig_package = ''

    # Objects to be cleaned at the end
    to_sign_file = None
    temp_config_file = None
    copied_sig_file = None

    try:
        # Unzip the hash package
        def get_hash_package_data(package):
            to_sign_package = Package(None, SecimageRemoteClientSigner.get_class_ToSignPackageFiles(),
                                      package=package)
            to_sign_package.update_data()
            return SecimageRemoteClientSigner.use_tosign_data(to_sign_package.pf)
        to_sign, signing_config = get_hash_package_data(hash_package)

        # Extract the signing info
        signing_config = json.loads(signing_config)
        chipset = str(signing_config['chipset'])
        sign_id = str(signing_config['sign_id'])

        # Save to_sign to a temp file
        to_sign_file_path = c_path.join(output_dir, 'hash_to_sign.bin')
        store_data_to_file(to_sign_file_path, to_sign)
        to_sign_file = to_sign_file_path

        if retcode == 0:

            # Create new config file to update the allow signing attribute &
            # the file type
            def update_cfg_cb(config):
                remote_config = auto_gen_xml_config.complex_remote_signer_attributes()
                remote_config.allow_signing_overrides = accept_signattrs
                config.root.signing.signer_attributes.remote_signer_attributes = remote_config
                sign_id_config = config.get_config_for_sign_id(sign_id)
                sign_id_config.pil_split = False
                sign_id_config.image_type = 'hash_to_sign'
            temp_config_file = c_path.join(output_dir, 'signature_package_config.xml')
            update_config(chipset, update_cfg_cb, temp_config_file)

            # Launch secimage once to get the image info list
            il = launch_secimage(config=temp_config_file, output_dir=output_dir,
                                 sign_id=sign_id, imagefile=to_sign_file,
                                 signer=SIGNER_LOCAL, verify_input=True,
                                 verbose=verbose, debug=debug,
                                 quiet=(False if verbose else True))

            # Copy the zip to where its expected in the output directory
            hash_package_exp = SecimageRemoteClientSigner.get_to_sign_package_path(il[0])
            if (hash_package != hash_package_exp):
                c_path.create_dir(os.path.dirname(hash_package_exp))
                ret, err = copyFile(hash_package, hash_package_exp)
                copied_sig_file = hash_package_exp
                if not ret:
                    raise RuntimeError(err)

            # Launch secimage
            il = launch_secimage(config=temp_config_file, output_dir=output_dir,
                                 sign_id=sign_id, imagefile=to_sign_file,
                                 signer=SIGNER_LOCAL, sign=True,
                                 verbose=verbose, debug=debug, quiet=quiet)

            # Verify the signature package was generated
            sig_package = SecimageRemoteClientSigner.get_signature_package_path(il[0])
            if not c_path.validate_file(sig_package):
                retcode = 1
                errstr = 'Failed to generate the signature package. ' + str(il[0].status.sign.error)

    except Exception as e:
        retcode = 1
        errstr = 'Exception occurred while running secimage. Exception - ' + str(e)

    finally:
        if not debug:
            if to_sign_file is not None:
                try: os.remove(to_sign_file)
                except Exception: pass
            if temp_config_file is not None:
                try: os.remove(temp_config_file)
                except Exception: pass
            if copied_sig_file is not None:
                try: os.remove(copied_sig_file)
                except Exception: pass

    return retcode, errstr, sig_package

def generatesigned(chipset, output_dir, sig_package, sign_id=None,
                   imagefile=None, metabuild=None, verbose=False, debug=False,
                   quiet=False):
    """Returns the signed image file.
    """
    retcode = 0
    errstr = ''
    signed_image = ''
    expected_path = ''

    # Launch secimage once to get the image info list
    il = launch_secimage(chipset=chipset, output_dir=output_dir,
                         sign_id=sign_id, imagefile=imagefile,
                         metabuild=metabuild, signer=SIGNER_REMOTE,
                         sign=True, verbose=verbose, debug=debug,
                         quiet=(False if verbose else True))

    # Copy the zip to where its expected in the output directory
    sig_package_exp = SecimageRemoteClientSigner.get_signature_package_path(il[0])
    if (sig_package != sig_package_exp):
        c_path.create_dir(os.path.dirname(sig_package_exp))
        ret, err = copyFile(sig_package, sig_package_exp)
        if not ret:
            raise RuntimeError(err)

    try:
        # Launch secimage
        il = launch_secimage(chipset=chipset, output_dir=output_dir,
                             sign_id=sign_id, imagefile=imagefile,
                             metabuild=metabuild, signer=SIGNER_REMOTE,
                             sign=True, verbose=verbose, debug=debug, quiet=quiet)

        # Verify the signed image was generated
        signed_image = il[0].dest_image.image_path
        if not c_path.validate_file(signed_image):
            retcode = 1
            errstr = 'Failed to generate the signed image. ' + str(il[0].status.sign.error)
        else:
            expected_path = il[0].src_image.image_path

    except Exception as e:
        retcode = 1
        errstr = 'Exception occurred while running secimage. Exception - ' + str(e)

    return retcode, errstr, signed_image, expected_path

def validateimage(chipset, sign_id=None, root_cert_hash=None, imagefile=None,
                  metabuild=None, verbose=False, debug=False, quiet=False):
    """Validates image file signing.
    """
    retcode = 0
    errstr = ''

    try:
        # Launch secimage once to get the image info list
        il = launch_secimage(chipset=chipset, sign_id=sign_id,
                             imagefile=imagefile, metabuild=metabuild,
                             signer=SIGNER_LOCAL, validate=True,
                             root_cert_hash=root_cert_hash, verbose=verbose,
                             debug=debug, quiet=quiet)

        # Check the status of the
        if il[0].status.validate_sign.state != StatusInfo.SUCCESS:
            retcode = 1
            errstr = 'Imgae signing validation failed. ' + il[0].status.validate_sign.error

    except Exception as e:
        retcode = 1
        errstr = 'Exception occurred while running secimage. Exception - ' + str(e)

    return retcode, errstr

