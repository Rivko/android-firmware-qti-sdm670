from sectools.common.crypto import cert as cert_functions
from sectools.common.crypto import rsa as rsa_functions
from sectools.common.crypto import utils


class SignerOutput(object):

    def __init__(self):
        self.signature = None
        self.cert_chain = None
        self.root_cert = None
        self.attestation_ca_cert = None
        self.attestation_cert = None
        self.root_key = None
        self.attestation_ca_key = None
        self.attestation_key = None
        self.root_cert_list = []

    def update_certs_format(self):
        # Update basic certs
        for tag in ['root_cert', 'attestation_ca_cert', 'attestation_cert']:
            val = getattr(self, tag)
            if val:
                val = cert_functions.get_cert_in_format(val, utils.FORMAT_DER)
            setattr(self, tag, val)

        # Update root cert list
        for idx in range(len(self.root_cert_list)):
            val = self.root_cert_list[idx]
            if val:
                val = cert_functions.get_cert_in_format(val, utils.FORMAT_DER)
            self.root_cert_list[idx] = val

        # Update keys
        for tag in ['root_key', 'attestation_ca_key', 'attestation_key']:
            val = getattr(self, tag)
            if val:
                val = rsa_functions.get_key_in_format(val, utils.FORMAT_DER)
            setattr(self, tag, val)

    def generate_cert_chain(self):
        certs = ([self.attestation_cert, self.attestation_ca_cert] +
                 [c for c in self.root_cert_list])
        certs = [c for c in certs if c is not None]
        self.cert_chain = b''.join(certs)
        return self.cert_chain
