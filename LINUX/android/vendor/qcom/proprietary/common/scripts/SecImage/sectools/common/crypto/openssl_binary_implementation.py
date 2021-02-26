# ===============================================================================
#
#  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# ===============================================================================

import binascii
import calendar
import os
import re
import subprocess
import tempfile
import time

import utility_functions
from common_classes import HmacParams
from sectools.common.crypto.functions.ecdsa.openssl import EcdsaOpenSSLImpl
from sectools.common.utils.c_logging import logger
from sectools.common.utils.c_misc import hexdump

openssl_binary_path = None  # This field must be overridden by a service provider
ccm_crypto_path = None  # This field must be overridden by a service provider
cbc_crypto_path = None  # This field must be overridden by a service provider

SHA1_OU_STRING = '07 0000 SHA1'
SHA256_OU_STRING = '07 0001 SHA256'
SHA384_OU_STRING = '07 0002 SHA384'

PSS_SALT_LENGTH = '-1'
PSS_HASH_ALGORITHM = 'sha256'


@utility_functions.logwrap
def gen_rsa_key_pair(key_size_in_bits, key_exponent, priv_key_output_file, pub_key_output_file):
    """ Generate RSA Key pair

        input:

        key_size_in_bits: Size of the key in bits.
        key_exponent: [3, 65537]
                      Exponent used in key calculation.
        priv_key_output_file: File name for storing private key
        pub_key_output_file: File name for storing public key

        output:

        returned value: {"public_key": [Generated public key],
                         "private_key": [Generated private key] }
                         Dictionary holding the values of public and private keys
    """

    logger.debug("the openssl_binary is:{0}".format(openssl_binary_path))

    if key_exponent == 3:
        exponent_str = "3"
    elif key_exponent == 257:
        exponent_str = "257"
    elif key_exponent == 65537:
        exponent_str = "65537"
    else:
        logger.warning("Exponent value supplied is INVALID! going with default exponent of 65537.")
        exponent_str = "65537"

    key_size_str = str(key_size_in_bits)

    if priv_key_output_file is not None:
        pk_file = open(priv_key_output_file, "wb")
    else:
        pk_file = tempfile.NamedTemporaryFile(delete=False)
        logger.debug("No output file specified for private key storage, so creating temp file: " + pk_file.name)

    try:
        private_key = utility_functions.system_command_logged([openssl_binary_path, "genpkey",
                                                               "-algorithm", "RSA",
                                                               "-outform", "PEM",
                                                               "-pkeyopt", "rsa_keygen_bits:" + key_size_str,
                                                               "-pkeyopt", "rsa_keygen_pubexp:" + exponent_str], stderr_to_temp=True)
    except subprocess.CalledProcessError, e:
        os.unlink(pk_file.name)
        logger.critical("gen_rsa_key_pair: OPENSSL Errored out on generation of RSA key.")
        logger.critical("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " Command = " + str(e.cmd))
        raise RuntimeError("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " Command = " + str(e.cmd))

    logger.debug("Writing generated private key to PEM file:" + pk_file.name)
    pk_file.write(private_key)
    pk_file.close()

    try:
        public_key = utility_functions.system_command_logged([openssl_binary_path, "rsa", "-in", pk_file.name, "-pubout"], stderr_to_temp=True)
    except subprocess.CalledProcessError, e:
        os.unlink(pk_file.name)
        logger.critical("gen_rsa_key_pair: OPENSSL could not get public key")
        logger.critical("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " Command = " + str(e.cmd))
        raise RuntimeError("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " Command = " + str(e.cmd))

    if pub_key_output_file is not None:
        logger.debug("Writing public key to file: " + pub_key_output_file)
        pubk_file = open(pub_key_output_file, "wb")
        pubk_file.write(public_key)
        pubk_file.close()

    if priv_key_output_file is None:
        logger.debug("Since private key file is temporary, deleting: " + pk_file.name)
        os.unlink(pk_file.name)

    return {'private_key': private_key, 'public_key': public_key}

def generate_hash(hashing_algorithm, file_to_hash):
    """ Function to generate hashes of input file using the standard hashing algoritm specified

        input:  hashing_algorithm: ["SHA1"|"SHA256"|"SHA384]
                file_to_hash: The file to calculate the hash of.

        output: string representing hash of file_to_hash

    """

    generated_hash = ''
    if hashing_algorithm.lower() not in ["sha1", "sha256","sha384"]:
        logger.warning("The algorithm specified is invalid! Using SHA256 as default.")
        hashing_algorithm = "-sha256"
    else:
        hashing_algorithm = "-" + hashing_algorithm

    try:
        generated_hash = utility_functions.system_command_logged([openssl_binary_path, "dgst", hashing_algorithm, file_to_hash]).rstrip().split("= ")[1]
    except:
        logger.critical("generate_hash: OPENSSL Hash generation failed")
    return generated_hash

def privkey_der_to_pem(der_privkey):
    """ Convert binary DER format PRIVATE key into base64 coded ASCII PEM format
    input:
    der_privkey: String containing binary PRIVATE KEY

    output
    pem_privkey: String containing base64 PEM PRIVATE KEY
    """

    pem_privkey = ''
    der_tempfile_name = utility_functions.store_data_to_temp_file(der_privkey)
    try:
        pem_privkey = utility_functions.system_command_logged([openssl_binary_path, 'rsa', '-in', der_tempfile_name, '-inform', 'DER', '-outform', 'PEM'], stderr_to_temp=True)
        logger.debug2("PEM Format Private Key: " + pem_privkey)
    except:
        logger.critical("privkey_der_to_pem: OPENSSL Could not convert DER key to PEM key")
    finally:
        os.unlink(der_tempfile_name)
        logger.debug("Deleting temporary file: " + der_tempfile_name)
    return pem_privkey

def privkey_pem_to_der(pem_privkey):
    """ Convert PEM format PRIVATE key into DER format
    input:
    pem_privkey: String containing base64 PEM Private key

    output
    der_privkey: String containing binary Private key
    """

    der_privkey = ''
    pem_tempfile_name = utility_functions.store_data_to_temp_file(pem_privkey)
    try:
        der_privkey = utility_functions.system_command_logged([openssl_binary_path, 'rsa', '-in', pem_tempfile_name, '-inform', 'PEM', '-outform', 'DER'], stderr_to_temp=True)
        logger.debug2("PEM Format private key: " + hexdump(der_privkey))
    except:
        logger.critical("privkey_pem_to_der: OPENSSL Could not convert PEM key to DER key")
    finally:
        os.unlink(pem_tempfile_name)
        logger.debug("Deleting temporary file: " + pem_tempfile_name)
    return der_privkey

def cert_der_to_pem(der_certificate):
    """ Convert binary DER format certificate into base64 coded ASCII PEM format
    input:
    der_certificate: String containing binary certificate

    output
    pem_certificate: String containing base64 PEM certificate
    """

    pem_certificate = ''
    der_tempfile_name = utility_functions.store_data_to_temp_file(der_certificate)
    try:
        pem_certificate = utility_functions.system_command_logged([openssl_binary_path, 'x509', '-in', der_tempfile_name, '-inform', 'DER', '-outform', 'PEM'], stderr_to_temp=True)
        logger.debug2("PEM Format certificate: " + pem_certificate)
    except:
        logger.critical("cert_der_to_pem: OPENSSL could not convert DER cert to PEM")
    finally:
        os.unlink(der_tempfile_name)
        logger.debug("Deleting temporary file: " + der_tempfile_name)
    return pem_certificate

def cert_pem_to_der(pem_certificate):
    """ Convert PEM format certificate into DER format
    input:
    pem_certificate: String containing base64 PEM certificate

    output
    der_certificate: String containing binary certificate
    """

    der_certificate = ''
    pem_tempfile_name = utility_functions.store_data_to_temp_file(pem_certificate)
    try:
        der_certificate = utility_functions.system_command_logged([openssl_binary_path, 'x509', '-in', pem_tempfile_name, '-inform', 'PEM', '-outform', 'DER'], stderr_to_temp=True)
        logger.debug2("PEM Format certificate: " + hexdump(der_certificate))
    except:
        logger.critical("cert_pem_to_der: OPENSSL could not convert PEM cert to DER")
    finally:
        os.unlink(pem_tempfile_name)
        logger.debug("Deleting temporary file: " + pem_tempfile_name)
    return der_certificate

def encrypt_with_private_key(message, private_key, use_pss=False, pss_digest_algorithm=PSS_HASH_ALGORITHM):
    """ Encrypt a message with a private key
    input:
        message: String representing message
        private_key: String representing the private key
        use_pss: Use PSS instead of PKCS

    output:
        signature: String representing encrypted message
    """

    encrypted_message = ''
    private_key_tempfile_name = utility_functions.store_data_to_temp_file(private_key)
    message_tempfile_name = utility_functions.store_data_to_temp_file(message)
    command_list = [openssl_binary_path, "pkeyutl", "-sign", "-inkey", private_key_tempfile_name, '-in', message_tempfile_name]

    # use pss padding and salt instead of PKCS
    if use_pss:
        logger.debug("Signing with PSS")
        command_list += ["-pkeyopt", "rsa_padding_mode:pss", "-pkeyopt", "rsa_pss_saltlen:"+PSS_SALT_LENGTH, "-pkeyopt", "digest:"+pss_digest_algorithm]

    try:
        encrypted_message = utility_functions.system_command_logged(command_list, stderr_to_temp=True)
    except:
        logger.critical("encrypt_with_private_key: OPENSSL could not perform RSA sign operation")
    finally:
        os.unlink(private_key_tempfile_name)
        os.unlink(message_tempfile_name)
    return encrypted_message

def _decrypt_with_public_key_ecdsa(encrypted_message, public_key, image_hash):
    message = ''

    encrypted_message = EcdsaOpenSSLImpl.strip_sig_padding(encrypted_message)

    public_key_tempfile_name = utility_functions.store_data_to_temp_file(public_key)
    encrypted_message_tempfile_name = utility_functions.store_data_to_temp_file(encrypted_message)
    image_hash_tempfile_name = utility_functions.store_data_to_temp_file(image_hash)

    command_list = ([openssl_binary_path, 'pkeyutl', '-verify', '-pubin', '-inkey', public_key_tempfile_name, '-sigfile', encrypted_message_tempfile_name, '-in', image_hash_tempfile_name])

    logger.debug("Attempt to verify cert with ECDSA")

    # execute command
    try:
        process = subprocess.Popen(command_list, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        process.wait()
        output = process.stdout.read()
        if output.strip() == 'Signature Verified Successfully':
            message = image_hash
    except:
        logger.critical("decrypt_with_public_key_ecdsa: OPENSSL could not decrypt with public key")
    finally:
        os.unlink(public_key_tempfile_name)
        os.unlink(encrypted_message_tempfile_name)
        os.unlink(image_hash_tempfile_name)
    return message

def _decrypt_with_public_key_pss(encrypted_message, public_key, image_hash, pss_digest_algorithm):
    """ Decrypt an encrypted message with a public key
    input:
        encrypted_message = String representation of encrypted message
        public_key = String representation of public key
        image_hash = message to verify against

    output:
        message = String representing decrypted message
    """

    message = ''
    public_key_tempfile_name = utility_functions.store_data_to_temp_file(public_key)
    encrypted_message_tempfile_name = utility_functions.store_data_to_temp_file(encrypted_message)
    image_hash_tempfile_name = utility_functions.store_data_to_temp_file(image_hash)

    command_list = [openssl_binary_path, 'pkeyutl', '-verify', '-pubin', '-inkey', public_key_tempfile_name, '-in', image_hash_tempfile_name, "-sigfile", encrypted_message_tempfile_name,"-pkeyopt", "rsa_padding_mode:pss", "-pkeyopt", "rsa_pss_saltlen:"+PSS_SALT_LENGTH, "-pkeyopt", "digest:"+pss_digest_algorithm]

    logger.debug("Attempt to verify cert with PSS")

    # execute command
    try:
        process = subprocess.Popen(command_list, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        process.wait()
        output = process.stdout.read()
        if output.strip() == 'Signature Verified Successfully':
            message = image_hash
    except:
        logger.critical("decrypt_with_public_key_pss: OPENSSL could not decrypt with public key")
    finally:
        os.unlink(public_key_tempfile_name)
        os.unlink(encrypted_message_tempfile_name)
        os.unlink(image_hash_tempfile_name)
    return message

def _decrypt_with_public_key_pkcs(encrypted_message, public_key):
    message = ''
    public_key_tempfile_name = utility_functions.store_data_to_temp_file(public_key)
    encrypted_message_tempfile_name = utility_functions.store_data_to_temp_file(encrypted_message)

    command_list = [openssl_binary_path, 'rsautl', '-inkey', public_key_tempfile_name, '-pubin', '-in', encrypted_message_tempfile_name]
    try:
        message = utility_functions.system_command_logged(command_list, stderr_to_temp=True)
    except:
        logger.critical("decrypt_with_public_key_pkcs: OPENSSL could not decrypt with public key")
    finally:
        os.unlink(public_key_tempfile_name)
        os.unlink(encrypted_message_tempfile_name)
    return message

def decrypt_with_public_key(encrypted_message, public_key, image_hash=None, use_pss=False, use_dsa=False, pss_digest_algorithm=PSS_HASH_ALGORITHM):
    """ Decrypt an encrypted message with a public key
    input:
        encrypted_message = String representation of encrypted message
        public_key = String representation of public key
        image_hash = message to verify against
        use_pss: Use PSS instead of PKCS
        use_dsa: Use ECDSA instead of RSA

    output:
        message = String representing decrypted message
    """

    if not image_hash and use_pss:
        raise RuntimeError('Can decrypt without image_hash while using PSS')

    if use_dsa:
        return _decrypt_with_public_key_ecdsa(encrypted_message, public_key, image_hash)
    elif use_pss:
        return _decrypt_with_public_key_pss(encrypted_message, public_key, image_hash, pss_digest_algorithm)
    else:
        return _decrypt_with_public_key_pkcs(encrypted_message, public_key)

def decrypt_with_private_der_key(encrypted_message, private_key):
    """ Decrypt an encrypted message with a private key
    input:
        encrypted_message = String representation of encrypted message
        private_key = String representation of private key

    output:
        message = String representing decrypted message
    """

    message = ''
    private_key_tempfile_name = utility_functions.store_data_to_temp_file(private_key)
    encrypted_message_tempfile_name = utility_functions.store_data_to_temp_file(encrypted_message)
    pt_tempfile_name = utility_functions.store_data_to_temp_file('')
    command_list = [openssl_binary_path, 'rsautl', '-decrypt', '-inkey', private_key_tempfile_name, '-in', encrypted_message_tempfile_name, '-keyform', 'DER']
    try:
        message = utility_functions.system_command_logged(command_list, stderr_to_temp=True)
    except:
        logger.debug2("decrypt_with_private_der_key: OPENSSL could not decrypt with private key")
    finally:
        os.unlink(private_key_tempfile_name)
        os.unlink(encrypted_message_tempfile_name)
        os.unlink(pt_tempfile_name)
    return message

def create_root_certficate(root_cert_params, root_key_pair, days, configfile, serial_num, use_pss, hash_algorithm):
    """ Creates a self signed Root cert
    input:
        root_cert_params={
                              'C'              : "US",
                              'ST'             : "California",
                              'L'              : "San Diego",
                              'O'              : "ASIC",
                              'CN'             : "Qualcomm",
                              'OU'             : [r"General Use Test Key (for testing only)", r"CDMA Technologies"]
                          }
        key_pair = {"public_key": [Generated public key],
                    "private_key": [Generated private key] }
    """

    return _create_self_signed_certificate(root_cert_params, root_key_pair, days, configfile, serial_num, use_pss, hash_algorithm)

def create_certificate(certificate_params, certificate_key_pair, CACertificate, CA_key_pair, days, configfile, serial_num, extfile_name, use_pss, hash_algorithm):
    """ Generate a certificate.
    input:

        certificate_params ={
                              'C'              : "US",
                              'ST'             : "California",
                              'L'              : "San Diego",
                              'O'              : "ASIC",
                              'CN'             : "Qualcomm",
                              'OU'             : [r"General Use Test Key (for testing only)", r"CDMA Technologies"]
                          }
        Dictionary of parameters to put in the certificate. The parameters above are an example
        If the same parameter has multiple values as 'OU' above, create a list of values

        CACertificate: String representation of CA certificate used to sign the cert

        certificate_key_pair = None | key_pair = {"public_key": [Generated public key],
                                                  "private_key": [Generated private key] }

        Dictionary holding the values of public and private keys. If this is None, a key
        is generated.

        days = validity period of certificate in days

        configfile = configfile used by openssl

    output:
        certificate: String representation of PEM certificate.
        certificate_key_pair : {"public_key": certificate public key],
                                "private_key": certificate private key] }
    """

    csr, csr_key_pair = _create_signing_request(certificate_params, certificate_key_pair, days, configfile)
    logger.debug2(csr + repr(csr_key_pair))
    certificate, ca_key_pair = _sign_csr_with_CA_certificate(csr, CACertificate, CA_key_pair, days, serial_num, extfile_name, use_pss,hash_algorithm)
    return certificate, csr_key_pair

def get_public_key_from_cert_chain(certificate_chain_list):
    """ Verify certificate chain and extract public key from chain list
    """

    if verify_certificate_chain(certificate_chain_list):
        return _extract_public_key_from_certificate(certificate_chain_list[0])
    else:
        logger.critical("get_public_key_from_cert_chain: The certificate chain could not be verified!")
        raise RuntimeError("The certificate chain could not be verified!")

def get_cert_valid_from_time(cert_path):
    """ Returns the "Valid from" value of a certificate

    input:
    cert_path: str. The path to the certificate who's "Valid from" value is desired

    output: str. The certificate's "Valid from" value represented as the number of seconds since 1/1/1970
    """
    cmd = [openssl_binary_path, "x509", "-in", cert_path, "-noout", "-startdate"]
    try:
        date_string = utility_functions.system_command_logged(cmd)
        date_list = date_string.split()
        # remove "notBefore=" from openssl output string
        month = date_list[0]
        date_list[0] = month[10:]
        # remove 2 first digits of year
        year = date_list[3]
        shortened_year = year[-2:]
        date_list[3] = shortened_year
        # get seconds since epoch
        date_object = time.strptime(" ".join(date_list), "%b %d %H:%M:%S %y %Z")
        cert_seconds_since_epoch = calendar.timegm(date_object)
        machine_current_time = int(time.time())
        machine_time_lag = cert_seconds_since_epoch - machine_current_time
        if machine_time_lag > 0:
            logger.debug("Local machine's time is slow by at least {0} seconds.".format(machine_time_lag))
        return str(cert_seconds_since_epoch)
    except:
        logger.debug("Failed to extract \"Valid from\" value of certificate.")
        return None

def verify_certificate_chain(certificate_chain):
    """ Verify the certificate chain to be valid

    input:
    certificate_chain: [cert1,cert2,cert3]
    List of certificates (*in PEM FORMAT*) in the certificate chain. It assumes that the last certificate is the Root CA certificate.

    output:
    [True|False]
    Boolean value
    """

    CAfile_contents = _create_CAfile_contents_from_cert_chain(certificate_chain)

    CAfile_tempfile_name = utility_functions.store_data_to_temp_file(CAfile_contents)

    level1_cert_to_verify_contents = certificate_chain[0]

    level1_cert_to_verify_tempfile_name = utility_functions.store_data_to_temp_file(level1_cert_to_verify_contents)

    valid_from = get_cert_valid_from_time(level1_cert_to_verify_tempfile_name)
    attime_command = ["-attime", valid_from] if valid_from is not None else []

    try:
        verify_level1_cert_command_out = utility_functions.system_command_logged([openssl_binary_path, 'verify'] + attime_command + ['-CAfile', CAfile_tempfile_name, level1_cert_to_verify_tempfile_name])
    except:
        logger.critical("verify_certificate_chain: OPENSSL could not verify cert chain")
        raise RuntimeError("verify_certificate_chain: OPENSSL could not verify cert chain")
    finally:
        os.unlink(CAfile_tempfile_name)
        os.unlink(level1_cert_to_verify_tempfile_name)
    logger.debug("OpenSSL verify command output: " + verify_level1_cert_command_out)
    logger.debug("Deleting temporary files: " + CAfile_tempfile_name + ", " + level1_cert_to_verify_tempfile_name)

    if verify_level1_cert_command_out.rstrip() == level1_cert_to_verify_tempfile_name + ": OK":
        logger.debug("The certificate chain is verified")
        return True
    else:
        logger.debug("The certificate chain is not verified")
        return False

def get_der_certificate_text(der_certificate_path):
    """ Return text contents of certificate
    input:
        der_certificate_path = path to DER certficate

    output:
        certificate_text = String representation certificate contents
    """

    proc = subprocess.Popen([openssl_binary_path, 'x509', '-in', der_certificate_path, '-inform', 'DER', '-noout', '-text', '-certopt', 'ext_dump'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    certificate_text, error = proc.communicate()
    if error:
        raise RuntimeError('Openssl command failed to get cert text.\n' + error)
    return certificate_text

def get_certificate_text_from_binary(certificate_blob):
    """ Return a certificate class
    input:
    certificate_blob: PEM/DER format BLOB of certificate

    output:
        certificate_text: String representation certificate contents
    """

    if 'BEGIN CERTIFICATE' in certificate_blob:
        certificate_der = cert_pem_to_der(certificate_blob)
    else:
        certificate_der = certificate_blob

    der_certificate_path = utility_functions.store_data_to_temp_file(certificate_der)
    certificate_text = get_der_certificate_text(der_certificate_path)
    os.unlink(der_certificate_path)
    return certificate_text

def get_asn1_text(pem_certificate_path):
    """ Return text contents of certificate
    input:
        pem_certificate_path = path to PEM certficate

    output:
        certificate_text = String representation asn1 contents
    """

    # Get the following error when using DER format. So, switch to PEM
    # before calling asn1parse:
    # 110140:error:0D07207B:asn1 encoding routines:ASN1_get_object:header too long:.\crypto\asn1\asn1_lib.c:150:
    certificate_text = subprocess.check_output([openssl_binary_path, 'asn1parse', '-in', pem_certificate_path, '-inform', 'PEM'])
    return certificate_text

def get_asn1_text_from_binary(certificate_blob):
    """ Return a certificate class
    input:
    certificate_blob: DER/PEM format BLOB of certificate

    output:
        certificate_text: String representation asn1 contents
    """

    if 'BEGIN CERTIFICATE' not in certificate_blob:
        certificate_pem = cert_der_to_pem(certificate_blob)
    else:
        certificate_pem = certificate_blob

    certificate_path = utility_functions.store_data_to_temp_file(certificate_pem)
    certificate_text = get_asn1_text(certificate_path)
    os.unlink(certificate_path)
    return certificate_text

def get_hmacparams_from_certificate_chain(certificate_chain_blob):
    """ Return a dictionary of HMAC parameters
    input:
    certificate_chain_blob: DER format BLOB of certificate chain

    output:
    hmac_params: dictionary of HMAC parameters
    """

    der_certificate_path = utility_functions.store_data_to_temp_file(certificate_chain_blob)
    certificate_text = get_der_certificate_text(der_certificate_path)
    certificate_subject_dictionary = _extract_certificate_subject_params(certificate_text)
    hmac_params = _get_hmacparams_from_certificate_subject_dictionary(certificate_subject_dictionary)
    os.unlink(der_certificate_path)
    return hmac_params

def _get_hmacparams_from_certificate_subject_dictionary(certificate_subject_dictionary):
    """ Return a dictionary of the HMAC params from the certificate subject dictionary
    input:
        certificate_subject_dictionary = dictionary of subject params from certificate

    output:
        hmac_params = Dictionary of HMAC parameters from certificate subject
    """

    sw_id_re = re.compile(r'01 ([0-9A-F]{16}) SW_ID')
    hw_id_re = re.compile(r'02 ([0-9A-F]{16}) HW_ID')
    if 'OU' in certificate_subject_dictionary.keys() and type(certificate_subject_dictionary['OU']) == list:
        certificate_subject_dictionary['OU'].sort()
    sw_id_element = sw_id_re.match(certificate_subject_dictionary['OU'][0])
    hw_id_element = hw_id_re.match(certificate_subject_dictionary['OU'][1])
    if sw_id_element is None:
        logger.critical("Error in certificate subject. SW_ID field not found. Not a valid certificate. Exiting")
        raise RuntimeError("Error in certificate subject. SW_ID field not found. Not a valid certificate. Exiting")
    elif hw_id_element is None:
        logger.critical("Error in certificate subject. HW_ID field not found. Not a valid certificate. Exiting")
        raise RuntimeError("Error in certificate subject. HW_ID field not found. Not a valid certificate. Exiting")
    else:
        logger.debug("Valid certificate: Found SW_ID and HW_ID")
        sw_id_text = sw_id_element.group(1)
        hw_id_text = hw_id_element.group(1)
        logger.debug('SW_ID = ' + sw_id_text)
        logger.debug('HW_ID = ' + hw_id_text)
        hw_id_int = int(hw_id_text, 16)
        sw_id_int = int(sw_id_text, 16)
        hmac_params = HmacParams(hw_id_int, sw_id_int)
        return hmac_params

def get_hash_algorithm_from_certicate_chain(certificate_chain_blob):
    """ Return a string of hash algorithm
    input:
    certificate_chain_blob: DER format BLOB of certificate chain

    output:
    hash_algo: string of hash algorithm
    """

    der_certificate_path = utility_functions.store_data_to_temp_file(certificate_chain_blob)
    certificate_text = get_der_certificate_text(der_certificate_path)
    certificate_subject_dictionary = _extract_certificate_subject_params(certificate_text)
    hash_algo = _get_hash_algorithm_from_certificate_subject_dictionary(certificate_subject_dictionary)
    os.unlink(der_certificate_path)
    return hash_algo

def _get_hash_algorithm_from_certificate_subject_dictionary(certificate_subject_dictionary):
    """ Return a string of SHA algorithm from the certificate subject dictionary
    input:
        certificate_subject_dictionary = dictionary of subject params from certificate

    output:
        hash_algo: sha1 or sha256 or sha384
    """

    sha_algo = None
    if 'OU' in certificate_subject_dictionary.keys() and type(certificate_subject_dictionary['OU']) == list:
        certificate_subject_dictionary['OU'].sort()
    import fnmatch
    sha_algo_field = fnmatch.filter(certificate_subject_dictionary['OU'], '07*SHA*')
    if sha_algo_field is None:
        raise RuntimeError("Error in certificate subject. SHA algorithm (OU 7) field not found. Not a valid certificate. Exiting")
    else:
        sha_algo_field = sha_algo_field[0]
        logger.debug('SHA Algorithm = ' + sha_algo_field)
        if sha_algo_field == SHA1_OU_STRING:
            sha_algo = 'sha1'
        elif sha_algo_field == SHA256_OU_STRING:
            sha_algo = 'sha256'
        elif sha_algo_field == SHA384_OU_STRING:
            sha_algo = 'sha384'
        else:
            raise RuntimeError("Unknown SHA algorithm string: " + sha_algo_field)
    return sha_algo

def _extract_public_key_from_certificate(certificate):
    """ Pulls out public key stored in certificate.
    input:
        certificate = String representation of PEM certificate

    output:
        public_key = String representation of public key in certificate
    """

    certificate_tempfile_name = utility_functions.store_data_to_temp_file(certificate)

    command_list = [openssl_binary_path, 'x509', '-in', certificate_tempfile_name, '-pubkey', '-noout', '-inform', 'PEM']
    try:
        public_key = utility_functions.system_command_logged(command_list)
    except:
        os.unlink(certificate_tempfile_name)
        logger.critical("_extract_public_key_from_certificate: OPENSSL could not extract public key from cert")
    os.unlink(certificate_tempfile_name)
    return public_key

def split_certificate_blob_into_certs(certificate_blob):
    """ Split a binary certificate chain blob into single binary certificates

    input:
    certificate_blob: String containing the entire certificate blob

    output:
    [cert1, cert2, ...]
    List of individual certificates found in the blob

    This function looks for a pattern ('0\x82.{2}0\x82') that marks the beginning of all certs, and splits the
    blob on these markers.
    """

    offsets = []
    certificates_list = []
    certificate_start_pattern = r'0\x82.{2}0\x82'
    for matches in re.finditer(certificate_start_pattern, certificate_blob, re.DOTALL):
        offsets.append(matches.start())

    logger.debug("Offsets:" + repr(offsets))
    for index, offset_val in enumerate(offsets):
        start_offset = offset_val
        if index < len(offsets) - 1:
            end_offset = offsets[index + 1]
        else:
            end_offset = None
        certificate = certificate_blob[start_offset:end_offset]
        certificates_list.append(certificate)

    return certificates_list

def _create_CAfile_contents_from_cert_chain(certificate_chain):
    """ Creates a CAfile that is used as input to the openssl verify command.

    input:
    certificate_chain: [cert1,cert2,cert3,...]
    Certificate list of certs in *PEM format*.

    output:
    CAfile_contents: String representing the concatenation of cert2 to certn

    The openssl verify function needs a CAfile to verify the certchain. This file contains
    a chain of all the higher level certs that issue the lowest level cert1.
    """

    CAfile_contents = "\n".join(certificate_chain[-(len(certificate_chain) - 1):])
    return CAfile_contents

def _create_list_of_CAfile_contents_from_cert_chain(certificate_chain):
    """ Creates list of CAfile that is used as input to the openssl verify command.

    input:
    certificate_chain: [cert1,cert2,cert3,...]
    Certificate list of certs in *PEM format*.

    output:
    CAfile_contents: String representing the concatenation of cert2 and certi

    The openssl verify function needs a CAfile to verify the certchain. This file contains
    a chain of all the higher level certs that issue the lowest level cert1.
    """

    CAfile_list = []
    if len(certificate_chain) > 3:
        # mutliple root cert case
        pass

    return CAfile_list

def _get_subject_string_from_certificate_params(certificate_params):
    subject_list = []
    for key in certificate_params:
        if type(certificate_params[key]) == list:
            for item in certificate_params[key]:
                subject_list.append(key + '=' + item)
            continue
        subject_list.append(key + "=" + certificate_params[key])
    logger.debug("Subject List = " + repr(subject_list))
    subject_string = r'/' + r"/".join(subject_list)
    logger.debug("Subject String = " + subject_string)
    return subject_string

def _execute_openssl_certificate_command(command_list, key_pair, key_tempfile_name, shell=False):
    try:
        certificate_request = utility_functions.system_command_logged(command_list, stderr_to_temp=True, shell=shell)
    except subprocess.CalledProcessError, e:
        logger.critical("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " Command = " + str(e.cmd))
        raise RuntimeError("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " \nCommand = " + ' '.join(e.cmd))

    logger.debug("Generated Output of openssl certificate command: " + certificate_request)

    if key_pair is None:
        generated_key_file = open(key_tempfile_name, "rb")
        private_key = generated_key_file.read()
        generated_key_file.close()
        public_key = get_public_key_from_private_key(private_key)
        os.unlink(key_tempfile_name)
        return certificate_request, {'public_key': public_key, 'private_key': private_key}
    else:
        os.unlink(key_tempfile_name)
        return certificate_request, key_pair

def _create_self_signed_certificate(certificate_params, key_pair, days, configfile, serial_num, use_pss, hash_algorithm):
    """ Create a self signed certificate
    input:
        certificate_params ={
                              'C'              : "US",
                              'ST'             : "California",
                              'L'              : "San Diego",
                              'O'              : "ASIC",
                              'CN'             : "Qualcomm",
                          }
        Dictionary of parameters to put in the certificate. The parameters above are an example

        key_pair = None | key_pair = {"public_key": [Generated public key],
                                      "private_key": [Generated private key] }

        Dictionary holding the values of public and private keys. If this is None, a key
        is generated.

        days = validity period of certificate in days

        configfile = configfile used by openssl

        serial_num = Serial number of certificate

    output:
        certificate: String representation of PEM certificate.
        key_pair : {"public_key": [Generated public key],
                    "private_key": [Generated private key] }
    """

    hash_algorithm = hash_algorithm if hash_algorithm is not None else 'sha256'

    subject_string = _get_subject_string_from_certificate_params(certificate_params)

    if key_pair is None:
        logger.debug("No key pair provided, will generate a key RSA 2048 bits")
        key_tempfile_name = "certificate.key"
        command_list = [openssl_binary_path, "req", "-new", "-x509", "-keyout", key_tempfile_name, "-subj", subject_string, "-newkey", "rsa:2048", "-days", str(days), "-config", configfile, "-set_serial", str(serial_num), '-' + hash_algorithm]
    else:
        key_tempfile_name = utility_functions.store_data_to_temp_file(key_pair['private_key'])
        command_list = [openssl_binary_path, "req", "-new", "-key", key_tempfile_name, "-x509", "-subj", subject_string, "-days", str(days), "-config", configfile, "-set_serial", str(serial_num), '-' + hash_algorithm]

    if use_pss:
        command_list += ("-sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:"+PSS_SALT_LENGTH+" -sigopt digest:"+ hash_algorithm).split()

    return _execute_openssl_certificate_command(command_list, key_pair, key_tempfile_name)

def _sign_csr_with_CA_certificate(certificate_signing_request, CA_certificate, CA_key_pair, days, serial_num, extfile_name, use_pss, hash_algorithm):
    """ Sign a Certificate signing request with a higher level CA certificate
    input:
    certificate_signing_request: String form of CSR
    CA_certificate: String representation of a higher level CA certificate

    CA_key_pair : {"public_key": [Generated public key],
                "private_key": [Generated private key] }
    The key pair of the CA_certificate

    days = validity period of certificate in days

    serial_num = Serial number of certificate

    extfile_name = Name of the extensions file to be used by openssl

    output:
    CA_signed_certificate: String representation of CA Signed certificate (PEM)
    CA_key_pair: {"public_key": CA public key,
                  "private_key": CA private key }
    """

    hash_algorithm = hash_algorithm if hash_algorithm is not None else 'sha256'

    CA_certificate_tempfile_name = utility_functions.store_data_to_temp_file(CA_certificate)
    CA_privkey_tempfile_name = utility_functions.store_data_to_temp_file(CA_key_pair['private_key'])
    certificate_signing_request_tempfile_name = utility_functions.store_data_to_temp_file(certificate_signing_request)

    command_list = [openssl_binary_path, "x509", "-req", "-in", certificate_signing_request_tempfile_name, "-CAkey",
                    CA_privkey_tempfile_name, "-CA", CA_certificate_tempfile_name, "-days", str(days),
                    "-set_serial", str(serial_num), "-extfile", extfile_name, '-'+hash_algorithm]

    if use_pss:
        command_list += ("-sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:"+PSS_SALT_LENGTH+ " -sigopt digest:"+hash_algorithm).split()

    logger.debug("Command_list = " + repr(command_list))

    CA_signed_certificate = utility_functions.system_command_logged(command_list, stderr_to_temp=True)
    logger.debug("Generated Output of openssl certificate command: " + CA_signed_certificate)

    os.unlink(CA_certificate_tempfile_name)
    os.unlink(CA_privkey_tempfile_name)
    os.unlink(certificate_signing_request_tempfile_name)

    return CA_signed_certificate, CA_key_pair

def _create_signing_request(certificate_params, key_pair, days=7300, configfile="opensslroot.cfg"):
    """ Create a CSR (Certificate Signing Request)
    input:

        certificate_params ={
                              'C'              : "US",
                              'ST'             : "California",
                              'L'              : "San Diego",
                              'O'              : "ASIC",
                              'CN'             : "Qualcomm",
                          }
        Dictionary of parameters to put in the certificate. The parameters above are an example

        key_pair = None | key_pair = {"public_key": [Generated public key],
                                      "private_key": [Generated private key] }

        Dictionary holding the values of public and private keys. If this is None, a key
        is generated.

        days = validity period of certificate in days

        configfile = configfile used by openssl

    output:
        certificate_request: String representation of PEM certificate signing request (CSR).
        key_pair : {"public_key": [Generated public key],
                    "private_key": [Generated private key] }
    """

    subject_string = _get_subject_string_from_certificate_params(certificate_params)

    if key_pair is None:
        logger.debug("No key pair provided, will generate a key RSA 2048 bits")
        key_tempfile_name = "certificate.key"
        command_list = [openssl_binary_path, "req", "-new", "-nodes", "-keyout", key_tempfile_name, "-subj", subject_string, "-newkey", "rsa:2048", "-days", str(days), "-config", configfile]
    else:
        key_tempfile_name = utility_functions.store_data_to_temp_file(key_pair['private_key'])
        command_list = [openssl_binary_path, "req", "-new", "-key", key_tempfile_name, "-subj", subject_string, "-days", str(days), "-config", configfile]
    logger.debug("Command List: " + repr(command_list))

    return _execute_openssl_certificate_command(command_list, key_pair, key_tempfile_name)

def get_public_key_from_private_key(private_key, use_dsa=False):
    """ Extracts public key from provided private key
    input:
    private_key: String representation of private key
    use_dsa: Boolean that governs when to use ECDSA

    output:
    public key: String representation of public key
    """

    privkey_tempfile = utility_functions.store_data_to_temp_file(private_key)
    try:
        public_key = utility_functions.system_command_logged([openssl_binary_path, "rsa" if not use_dsa else "ecdsa", "-in", privkey_tempfile, "-pubout"], stderr_to_temp=True)
    except subprocess.CalledProcessError, e:
        logger.critical("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " Command = " + str(e.cmd))
        raise RuntimeError("call to OpenSSL binary returned an error!: retval = " + str(e.returncode) + " Command = " + str(e.cmd))
    os.unlink(privkey_tempfile)
    return public_key

def _extract_certificate_subject_params(certificate_text):
    """ Extracts Subject parameter from provided certificate text
    input:
    certificate_text: String representation of Certificate

    output:
    certificate_subject_dictionary: Dictionary of certificate parameters
    """

    subject_re = re.compile(r'Subject:.*\n?')
    certificate_subject = subject_re.search(certificate_text).group(0)
    certificate_subject_contents = certificate_subject.split(':')
    certificate_subject_list = certificate_subject_contents[1].split(',')
    return utility_functions.normalize_param_list_into_dict(certificate_subject_list)

def cbc_cts_encrypt_binary(binary_blob, hex_preexisting_128_bit_key, hex_preexisting_iv):
    """ Function to encrypt binary with a CBC 128 bit cipher.
    input:
    binary_blob: Binary blob to encrypt
    hex_preexisting_128_bit_key: hex representarion of 128bit key | None,
    if None, the key is generated

    hex_preexisting_iv: hex representarion of image IV | None,
    if None, the IV is generated

    output:
    (encrypted_binary, encryption_key, image_iv): Tuple with the encrypted binary, the key, and the IV
    """

    if hex_preexisting_128_bit_key is None:
        encryption_key = os.urandom(16)
        encryption_key = binascii.hexlify(encryption_key)

    else:
        encryption_key = hex_preexisting_128_bit_key

    if hex_preexisting_iv is None:
        image_iv = os.urandom(16)
        image_iv = binascii.hexlify(image_iv)
    else:
        image_iv = hex_preexisting_iv

    binary_blob_file = utility_functions.store_data_to_temp_file(binary_blob)
    encrypted_image_file_name = utility_functions.store_data_to_temp_file('')

    command_list = [cbc_crypto_path, "--input-file", binary_blob_file, '--key', encryption_key, '--iv', image_iv, '--output', encrypted_image_file_name, '--operation=encrypt']
    utility_functions.system_command_logged(command_list)
    with open(encrypted_image_file_name, 'rb') as encrypted_output_file:
        encrypted_binary = encrypted_output_file.read()
    os.unlink(binary_blob_file)
    os.unlink(encrypted_image_file_name)
    return encrypted_binary, encryption_key, image_iv

def cbc_cts_decrypt_binary(encrypted_blob, hex_preexisting_128_bit_key, hex_preexisting_iv):
    """ Function to decrypt a CBC encrypted binary.
    input:
    encrypted_blob: Encrypted Binary blob to decrypt
    hex_preexisting_128_bit_key: hex representarion of 128bit key

    hex_preexisting_iv: hex representarion of image IV

    output:
    plaintext_binary: Decrypted plaintext binary
    """

    if hex_preexisting_128_bit_key is None:
        raise RuntimeError('Key must be supplied')
    else:
        encryption_key = hex_preexisting_128_bit_key

    if hex_preexisting_iv is None:
        raise RuntimeError('IV must be supplied')
    else:
        image_iv = hex_preexisting_iv

    encrypted_blob_file = utility_functions.store_data_to_temp_file(encrypted_blob)
    plaintext_binary_file_name = utility_functions.store_data_to_temp_file('')

    command_list = [cbc_crypto_path, "--input-file", encrypted_blob_file, '--key', encryption_key, '--iv', image_iv, '--output', plaintext_binary_file_name, '--operation=decrypt']
    utility_functions.system_command_logged(command_list)
    with open(plaintext_binary_file_name, 'rb') as plaintext_output_file:
        plaintext_binary = plaintext_output_file.read()
    os.unlink(encrypted_blob_file)
    os.unlink(plaintext_binary_file_name)
    return plaintext_binary

def ccm_encrypt_binary(binary_blob, hex_preexisting_128_bit_key, hex_preexisting_104_bit_iv, hex_preexisting_aad):
    """ Function to encrypt binary with a CCM 128 bit cipher.
    input:
    binary_blob: Binary blob to encrypt
    hex_preexisting_128_bit_key: hex representarion of 128bit key | None,
    if None, the key is generated

    hex_preexisting_iv: hex representarion of image IV | None,
    if None, the IV is generated

    hex_preexisting_aad: hex representation of Additional Authentication data needed by the algorithm

    output:
    (encrypted_binary, encryption_key, image_iv, hex_preexisting_aad): Tuple with the encrypted binary, the key, the IV, and the AAD
    """

    if hex_preexisting_aad is None:
        raise RuntimeError('AAD must be supplied')

    if hex_preexisting_128_bit_key is None:
        encryption_key = os.urandom(16)
        encryption_key = binascii.hexlify(encryption_key)

    else:
        encryption_key = hex_preexisting_128_bit_key

    if hex_preexisting_104_bit_iv is None:
        image_iv = os.urandom(13)
        image_iv = binascii.hexlify(image_iv)
    else:
        image_iv = hex_preexisting_104_bit_iv

    binary_blob_file = utility_functions.store_data_to_temp_file(binary_blob)
    encrypted_image_file_name = utility_functions.store_data_to_temp_file('')

    command_list = [ccm_crypto_path, "--input-file", binary_blob_file, '--key', encryption_key, '--iv', image_iv, '--output', encrypted_image_file_name, '--operation=encrypt', '--aad', hex_preexisting_aad]
    utility_functions.system_command_logged(command_list)
    with open(encrypted_image_file_name, 'rb') as encrypted_output_file:
        encrypted_binary = encrypted_output_file.read()
    os.unlink(binary_blob_file)
    os.unlink(encrypted_image_file_name)
    return encrypted_binary, encryption_key, image_iv, hex_preexisting_aad

def ccm_decrypt_binary(encrypted_blob, hex_preexisting_128_bit_key, hex_preexisting_104_bit_iv, hex_preexisting_aad):
    """ Function to decrypt a CCM encrypted binary.
    input:
    encrypted_blob: Encrypted Binary blob to decrypt
    hex_preexisting_128_bit_key: hex representarion of 128bit key

    hex_preexisting_iv: hex representarion of image IV

    hex_preexisting_aad: hex representation of Additional Authentication data needed by the algorithm

    output:
    plaintext_binary: Decrypted plaintext binary
    """

    if hex_preexisting_aad is None:
        raise RuntimeError('AAD must be supplied')

    if hex_preexisting_128_bit_key is None:
        raise RuntimeError('Key must be supplied')
    else:
        encryption_key = hex_preexisting_128_bit_key

    if hex_preexisting_104_bit_iv is None:
        raise RuntimeError('IV must be supplied')
    else:
        image_iv = hex_preexisting_104_bit_iv

    encrypted_blob_file = utility_functions.store_data_to_temp_file(encrypted_blob)
    plaintext_binary_file_name = utility_functions.store_data_to_temp_file('')

    command_list = [ccm_crypto_path, "--input-file", encrypted_blob_file, '--key', encryption_key, '--iv', image_iv, '--output', plaintext_binary_file_name, '--operation=decrypt', '--aad', hex_preexisting_aad]
    output = utility_functions.system_command_logged(command_list)

    if "Caught HashVerificationFailed..." in output:
        raise subprocess.CalledProcessError(-1, output)

    with open(plaintext_binary_file_name, 'rb') as plaintext_output_file:
        plaintext_binary = plaintext_output_file.read()
    os.unlink(encrypted_blob_file)
    os.unlink(plaintext_binary_file_name)
    return plaintext_binary

def cbc_encrypt_binary(binary_blob, hex_preexisting_128_bit_key, hex_preexisting_iv):
    """ Function to encrypt binary with a CBC 128 bit cipher.
    input:
    binary_blob: Binary blob to encrypt
    hex_preexisting_128_bit_key: hex representarion of 128bit key | None,
    if None, the key is generated

    hex_preexisting_iv: hex representarion of image IV | None,
    if None, the IV is generated

    output:
    (encrypted_binary, encryption_key, image_iv): Tuple with the encrypted binary, the key, and the IV
    """

    if hex_preexisting_128_bit_key is None:
        encryption_key = os.urandom(16)
        encryption_key = binascii.hexlify(encryption_key)

    else:
        encryption_key = hex_preexisting_128_bit_key

    if hex_preexisting_iv is None:
        image_iv = os.urandom(16)
        image_iv = binascii.hexlify(image_iv)
    else:
        image_iv = hex_preexisting_iv

    binary_blob_file = utility_functions.store_data_to_temp_file(binary_blob)
    encrypted_image_file_name = utility_functions.store_data_to_temp_file('')

    command_list = [openssl_binary_path, "enc", "-aes-128-cbc", "-in", binary_blob_file, "-K", encryption_key, "-iv", image_iv, "-out", encrypted_image_file_name, "-nopad"]
    utility_functions.system_command_logged(command_list)
    with open(encrypted_image_file_name, 'rb') as encrypted_output_file:
        encrypted_binary = encrypted_output_file.read()
    os.unlink(binary_blob_file)
    os.unlink(encrypted_image_file_name)
    return encrypted_binary, encryption_key, image_iv

def cbc_decrypt_binary(encrypted_blob, hex_preexisting_128_bit_key, hex_preexisting_iv):
    """ Function to decrypt a CBC encrypted binary.
    input:
    encrypted_blob: Encrypted Binary blob to decrypt
    hex_preexisting_128_bit_key: hex representarion of 128bit key

    hex_preexisting_iv: hex representarion of image IV

    output:
    plaintext_binary: Decrypted plaintext binary
    """

    if hex_preexisting_128_bit_key is None:
        raise RuntimeError('Key must be supplied')
    else:
        encryption_key = hex_preexisting_128_bit_key

    if hex_preexisting_iv is None:
        raise RuntimeError('IV must be supplied')
    else:
        image_iv = hex_preexisting_iv

    encrypted_blob_file = utility_functions.store_data_to_temp_file(encrypted_blob)
    plaintext_binary_file_name = utility_functions.store_data_to_temp_file('')

    command_list = [openssl_binary_path, "enc", "-aes-128-cbc", "-d", "-in", encrypted_blob_file, "-K", encryption_key, "-iv", image_iv, "-out", plaintext_binary_file_name, "-nopad"]
    utility_functions.system_command_logged(command_list)
    with open(plaintext_binary_file_name, 'rb') as plaintext_output_file:
        plaintext_binary = plaintext_output_file.read()
    os.unlink(encrypted_blob_file)
    os.unlink(plaintext_binary_file_name)
    return plaintext_binary

def get_public_key_from_certificate(certificate):
    if 'BEGIN CERTIFICATE' not in certificate:
        certificate = cert_der_to_pem(certificate)

    pubkey = _extract_public_key_from_certificate(certificate)
    pubkey_file_name = utility_functions.store_data_to_temp_file(pubkey)
    command_list = [openssl_binary_path, 'rsa', '-pubin', '-inform', 'PEM', '-text', '-noout', '<', pubkey_file_name]
    logger.debug("Command_list = " + repr(command_list))

    pubkey_text = utility_functions.system_command_logged(command_list, stderr_to_temp=True)
    logger.debug2("Pubkey text: " + pubkey_text)
    os.unlink(pubkey_file_name)

def cert_uses_pss(certificate, form="PEM"):
    """
    Returns true if the certificate uses PSS in signature
    """

    cert_tempfile_name = utility_functions.store_data_to_temp_file(certificate)

    command_list = [openssl_binary_path, "x509", "-text", "-inform", form, "-in", cert_tempfile_name, '-text', '-noout']
    use_pss = False
    try:
        output = utility_functions.system_command_logged(command_list, stderr_to_temp=True)
        if 'Signature Algorithm: rsassaPss' in output:
            use_pss = True
    except Exception as e:
        logger.critical(e)
        use_pss = False
    finally:
        os.unlink(cert_tempfile_name)
    return use_pss

def cert_uses_dsa(certificate, form="PEM"):
    """
    Returns true if the certificate uses DSA in signature
    """

    cert_tempfile_name = utility_functions.store_data_to_temp_file(certificate)

    command_list = [openssl_binary_path, "x509", "-text", "-inform", form, "-in", cert_tempfile_name, '-text', '-noout']
    use_dsa = False
    try:
        output = utility_functions.system_command_logged(command_list, stderr_to_temp=True)
        if 'Signature Algorithm: ecdsa' in output:
            use_dsa = True
    except Exception as e:
        logger.critical(e)
        use_dsa = False
    finally:
        os.unlink(cert_tempfile_name)
    return use_dsa

def cert_version_check(certificate, version):
    """
    certificate must be in DER form
    current version is 3 (i.e. v3)

    Returns true if cert version match
    """

    der_cert_text = get_der_certificate_text(certificate)
    match = re.search('Version: (\d+) \(.*?\)\n', der_cert_text, re.IGNORECASE)
    if match is None:
        raise RuntimeError('Could not get the Version info from certificate')
    cert_version = match.group(1)
    if int(cert_version, 10) == version:
        return True
    else:
        logger.critical('Certificate version is incorrect: ' + str(cert_version))
        return False
