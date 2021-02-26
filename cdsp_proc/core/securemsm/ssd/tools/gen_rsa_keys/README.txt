Usage: 
gen_key_pair [n]		n could be any value

Output:
.\Key_Pair_n\
rsa_pr_key_n.pem		Private Key in PEM encoding format
rsa_pr_key_n.der		Private Key in DER encoding format
rsa_pkcs8_pr_key_n.der		PKCS8 Private Key in DER encoding format
rsa_pub_key_n.pem		Public key in PEM encofing format
rsa_pub_key_n.der		Public key in DER encofing format
rsa_pub_key_id_n.dat		Public Key ID (SHA256 hash of public key in DER format)


Notes:
The tool assumes that there exists a large random number that can be used to seed the OpenSSL random number
generator.
You could use any tool that can generate secure random numbers & store it in the same directory as "rand_seed.dat"
I have used an ELF file from one of our builds as the large random number
