#!/bin/bash

#	Copyright (c) 2015 Qualcomm Technologies, Inc.
#	All Rights Reserved.
#	Confidential and Proprietary - Qualcomm Technologies, Inc.

# CONFIGURATION
#===================================================================================

outdir="test_pki"
version="1.06"

# MDTP Root CA
#-----------------------------------------------------------------------------------
mdtp_root_ca_crt=${outdir}/mdtp_root_ca.crt	# public Qualcomm Technologies, Inc. MDTP root CA certificate file
						# Self-signed
mdtp_root_ca_key=${outdir}/mdtp_root_ca.key	# key file for Qualcomm Technologies, Inc. MDTP root CA certificate

mdtp_root_ca_request=${outdir}/mdtp_root_ca_request.csr
mdtp_subject="/O=Test Qualcomm Technologies, Inc./OU=Test Qualcomm CDMA Technologies/CN=Test MDTP Root CA"
root_start_date=20140101000001Z
root_end_date=20351231235959Z
root_ca_db_dir="root_ca_db"
root_ca_serial_path=$root_ca_db_dir"/serial"

# MDTP ISV A CA
#-----------------------------------------------------------------------------------
isv_a_ca_crt=${outdir}/isv_a_ca.crt	# public isv A ca certificate file
					# certificate issued by Qualcomm Technologies, Inc., signed by mdtp_root_ca_key
isv_a_ca_key=${outdir}/isv_a_ca.key	# key file for ISV A ca certificate
					# Saved at ISV A

isv_a_ca_request=${outdir}/isv_a_ca_request.csr
isv_a_ca_subject="/O=Test Qualcomm Technologies, Inc./OU=Test Qualcomm CDMA Technologies/CN=Test ISV A CA"
isv_a_ca_end_date=20251231235959Z
isv_a_ca_db_dir="isv_a_ca_db"
isv_a_ca_serial_path=$isv_a_ca_db_dir"/serial"

# MDTP ISV B CA
#-----------------------------------------------------------------------------------
isv_b_ca_crt=${outdir}/isv_b_ca.crt	# public isv B ca certificate file
					# certificate issued by Qualcomm Technologies, Inc., signed by mdtp_root_ca_key
isv_b_ca_key=${outdir}/isv_b_ca.key	# key file for ISV B ca certificate
					# Saved at ISV B

isv_b_ca_request=${outdir}/isv_b_ca_request.csr
isv_b_ca_subject="/O=Test Qualcomm Technologies, Inc./OU=Test Qualcomm CDMA Technologies/CN=Test ISV B CA"
isv_b_ca_end_date=20251231235959Z
isv_b_ca_db_dir="isv_b_ca_db"
isv_b_ca_serial_path=$isv_b_ca_db_dir"/serial"

# MDTP ISV A1
#-----------------------------------------------------------------------------------
isv_a1_crt=${outdir}/isv_a1.crt		# public isv A1 certificate file
					# certificate issued by Qualcomm Technologies, Inc., signed by isv_a_ca_key
isv_a1_key=${outdir}/isv_a1.key		# key file for ISV A1 certificate
					# Saved at ISV A1
isv_a1_request=${outdir}/isv_a1_request.csr
isv_a1_subject="/O=Test Qualcomm Technologies, Inc./OU=Test Qualcomm CDMA Technologies/CN=Test ISV A1"
isv_a1_end_date=20151231235959Z

# MDTP ISV A2
#-----------------------------------------------------------------------------------
isv_a2_crt=${outdir}/isv_a2.crt		# public isv A2 certificate file
					# certificate issued by Qualcomm Technologies, Inc., signed by isv_a_ca_key
isv_a2_key=${outdir}/isv_a2.key		# key file for ISV A2 certificate
					# Saved at ISV A2
isv_a2_request=${outdir}/isv_a2_request.csr
isv_a2_subject="/O=Test Qualcomm Technologies, Inc./OU=Test Qualcomm CDMA Technologies/CN=Test ISV A2"
isv_a2_end_date=20151231235959Z

# MDTP ISV B1
#-----------------------------------------------------------------------------------
isv_b1_crt=${outdir}/isv_b1.crt		# public isv B1 certificate file
					# certificate issued by Qualcomm Technologies, Inc., signed by isv_b_ca_key
isv_b1_key=${outdir}/isv_b1.key		# key file for ISV B1 certificate
					# Saved at ISV B1
isv_b1_request=${outdir}/isv_b1_request.csr
isv_b1_subject="/O=Test Qualcomm Technologies, Inc./OU=Test Qualcomm CDMA Technologies/CN=Test ISV B1"
isv_b1_end_date=20151231235959Z

# MDTP ISV B2
#-----------------------------------------------------------------------------------
isv_b2_crt=${outdir}/isv_b2.crt		# public isv B2 certificate file
					# certificate issued by Qualcomm Technologies, Inc., signed by isv_b_ca_key
isv_b2_key=${outdir}/isv_b2.key		# key file for ISV B2 certificate
					# Saved at ISV B2
isv_b2_request=${outdir}/isv_b2_request.csr
isv_b2_subject="/O=Test Qualcomm Technologies, Inc./OU=Test Qualcomm CDMA Technologies/CN=Test ISV B2"
isv_b2_end_date=20151231235959Z

#===================================================================================
# END OF CONFIGURATION
#===================================================================================

function gen_pki {

	rm -Rf $outdir
	rm -Rf $root_ca_db_dir
	rm -Rf $isv_a_ca_db_dir
	rm -Rf $isv_b_ca_db_dir
	mkdir -p $outdir
	mkdir -p $root_ca_db_dir
	mkdir -p $isv_a_ca_db_dir
	mkdir -p $isv_b_ca_db_dir

	#===================================================================================
	# Creating MDTP Root CA
	#===================================================================================
	echo "Creating MDTP Root CA"
	openssl req -new -out ${mdtp_root_ca_request} -nodes -newkey rsa:2048 -sha256 -keyout ${mdtp_root_ca_key} -subj "$mdtp_subject"

	touch $root_ca_db_dir/index.txt
	echo "01" > $root_ca_serial_path

	cat <<-CONFIG |
		distinguished_name=1
		x509_extensions=2
		[1]
		[2]
		keyUsage=critical, keyCertSign, cRLSign
		basicConstraints=critical, CA:TRUE
		subjectKeyIdentifier=hash
		#authorityKeyIdentifier=keyid:always

	    [ ca ]
	    default_ca      = CA_default          	   # The default ca section

	    [ CA_default ]
	    policy         = policy_any          	   # default policy
	    email_in_dn    = no                 	   # Don't add the email into cert DN
	    dir            = ./              	  	   # top dir
	    database       = $root_ca_db_dir"/index.txt"   # index file.
	    new_certs_dir  = $root_ca_db_dir       	   # new certs dir
	    certificate    = $mdtp_root_ca_request 	   # The CA cert
	    serial         = $root_ca_serial_path          # serial nocrt file
	    private_key    = $mdtp_root_ca_key   	   # CA private key

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG
	openssl ca -batch -md sha256 \
			-out $mdtp_root_ca_crt \
			-config <(cat) \
			-selfsign \
			-keyfile $mdtp_root_ca_key \
			-in $mdtp_root_ca_request \
			-startdate $root_start_date \
			-enddate $root_end_date

	rm $mdtp_root_ca_request

	# openssl x509 -text -noout -in $outdir/mdtp_root_ca.crt

	#===================================================================================
	# Creating MDTP ISV A CA and MDTP ISV B CA
	#===================================================================================
	echo "Creating MDTP ISV A CA and MDTP ISV B CA"
	openssl req -new -out ${isv_a_ca_request} -nodes -newkey rsa:2048 -sha256 -keyout ${isv_a_ca_key} -subj "$isv_a_ca_subject"
	openssl req -new -out ${isv_b_ca_request} -nodes -newkey rsa:2048 -sha256 -keyout ${isv_b_ca_key} -subj "$isv_b_ca_subject"

	touch $isv_a_ca_db_dir/index.txt
	echo "01" > $isv_a_ca_serial_path
	touch $isv_b_ca_db_dir/index.txt
	echo "01" > $isv_b_ca_serial_path

	cat <<-CONFIG |
		distinguished_name=1
		x509_extensions=2
		[1]
		[2]
		keyUsage=critical, keyCertSign, cRLSign
		basicConstraints=critical, CA:TRUE, pathlen:0
		subjectKeyIdentifier=hash
		#authorityKeyIdentifier=keyid:always

	    [ ca ]
	    default_ca      = CA_default            # The default ca section

	    [ CA_default ]
	    policy         = policy_any            	    # default policy
	    email_in_dn    = no                    	    # Don't add the email into cert DN
	    dir            = ./              	   	    # top dir
	    database       = $root_ca_db_dir"/index.txt"   # index file.
	    new_certs_dir  = $root_ca_db_dir       	   # new certs dir
	    certificate    = $mdtp_root_ca_crt    	    # The CA cert
	    serial         = $root_ca_serial_path          # serial nocrt file
	    private_key    = $mdtp_root_ca_key     	    # CA private key

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG
	openssl ca -batch -md sha256 \
			-out $isv_a_ca_crt \
			-config <(cat) \
			-in $isv_a_ca_request \
			-startdate $root_start_date \
			-enddate $isv_a_ca_end_date

	rm $isv_a_ca_request

	# openssl x509 -text -noout -in $outdir/isv_a_ca.crt

	cat <<-CONFIG |
		distinguished_name=1
		x509_extensions=2
		[1]
		[2]
		keyUsage=critical, keyCertSign, cRLSign
		basicConstraints=critical, CA:TRUE, pathlen:0
		subjectKeyIdentifier=hash
		#authorityKeyIdentifier=keyid:always

	    [ ca ]
	    default_ca      = CA_default            # The default ca section

	    [ CA_default ]
	    policy         = policy_any            	    # default policy
	    email_in_dn    = no                    	    # Don't add the email into cert DN
	    dir            = ./              	   	    # top dir
	    database       = $root_ca_db_dir"/index.txt"   # index file.
	    new_certs_dir  = $root_ca_db_dir       	   # new certs dir
	    certificate    = $mdtp_root_ca_crt    	    # The CA cert
	    serial         = $root_ca_serial_path          # serial nocrt file
	    private_key    = $mdtp_root_ca_key     	    # CA private key

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG
	openssl ca -batch -md sha256 \
			-out $isv_b_ca_crt \
			-config <(cat) \
			-in $isv_b_ca_request \
			-startdate $root_start_date \
			-enddate $isv_b_ca_end_date

	rm $isv_b_ca_request

	# openssl x509 -text -noout -in $outdir/isv_a_ca.crt

	#===================================================================================
	# Creating MDTP ISV A1 and MDTP ISV A2
	#===================================================================================
	echo "Creating MDTP ISV A1 and MDTP ISV A2"
	openssl req -new -out ${isv_a1_request} -nodes -newkey rsa:2048 -sha256 -keyout ${isv_a1_key} -subj "$isv_a1_subject"
	openssl req -new -out ${isv_a2_request} -nodes -newkey rsa:2048 -sha256 -keyout ${isv_a2_key} -subj "$isv_a2_subject"

	cat <<-CONFIG |
		distinguished_name=1
		x509_extensions=2
		[1]
		[2]
		keyUsage=critical, digitalSignature
		basicConstraints=critical, CA:FALSE
		subjectKeyIdentifier=hash
		#authorityKeyIdentifier=keyid:always

		[ ca ]
		    default_ca      = CA_default            		  # The default ca section

		    [ CA_default ]
		    policy         = policy_any            	    	  # default policy
		    email_in_dn    = no                    	    	  # Don't add the email into cert DN
		    dir            = ./              	   	    	  # top dir
		    database       = $isv_a_ca_db_dir"/index.txt" 	  # index file.
		    new_certs_dir  = $isv_a_ca_db_dir             	  # new certs dir
		    certificate    = $isv_a_ca_crt    	    	  	  # The CA cert
		    serial         = $isv_a_ca_serial_path        	  # serial no file
		    private_key    = $isv_a_ca_key	     	    	  # CA private key

		    [ policy_any ]
		    countryName            = optional
		    stateOrProvinceName    = optional
		    organizationName       = optional
		    organizationalUnitName = optional
		    commonName             = optional
		    emailAddress           = optional
		CONFIG
	openssl ca -batch -md sha256 \
			-out $isv_a1_crt \
			-config <(cat) \
			-in $isv_a1_request \
			-startdate $root_start_date \
			-enddate $isv_a1_end_date

	rm $isv_a1_request

	# openssl x509 -text -noout -in $outdir/isv_a1.crt

	cat <<-CONFIG |
		distinguished_name=1
		x509_extensions=2
		[1]
		[2]
		keyUsage=critical, digitalSignature
		basicConstraints=critical, CA:FALSE
		subjectKeyIdentifier=hash
		#authorityKeyIdentifier=keyid:always

		[ ca ]
		    default_ca      = CA_default            		  # The default ca section

		    [ CA_default ]
		    policy         = policy_any            	    	  # default policy
		    email_in_dn    = no                    	    	  # Don't add the email into cert DN
		    dir            = ./              	   	    	  # top dir
		    database       = $isv_a_ca_db_dir"/index.txt" 	  # index file.
		    new_certs_dir  = $isv_a_ca_db_dir             	  # new certs dir
		    certificate    = $isv_a_ca_crt	    	    	  # The CA cert
		    serial         = $isv_a_ca_serial_path        	  # serial no file
		    private_key    = $isv_a_ca_key	     	    	  # CA private key

		    [ policy_any ]
		    countryName            = optional
		    stateOrProvinceName    = optional
		    organizationName       = optional
		    organizationalUnitName = optional
		    commonName             = optional
		    emailAddress           = optional
		CONFIG
	openssl ca -batch -md sha256 \
			-out $isv_a2_crt \
			-config <(cat) \
			-in $isv_a2_request \
			-startdate $root_start_date \
			-enddate $isv_a2_end_date

	rm $isv_a2_request

	# openssl x509 -text -noout -in $outdir/isv_a2.crt

	#===================================================================================
	# Creating MDTP ISV B1 and MDTP ISV B2
	#===================================================================================
	echo "Creating MDTP ISV B1 and MDTP ISV B2"
	openssl req -new -out ${isv_b1_request} -nodes -newkey rsa:2048 -sha256 -keyout ${isv_b1_key} -subj "$isv_b1_subject"
	openssl req -new -out ${isv_b2_request} -nodes -newkey rsa:2048 -sha256 -keyout ${isv_b2_key} -subj "$isv_b2_subject"

	cat <<-CONFIG |
		distinguished_name=1
		x509_extensions=2
		[1]
		[2]
		keyUsage=critical, digitalSignature
		basicConstraints=critical, CA:FALSE
		subjectKeyIdentifier=hash
		#authorityKeyIdentifier=keyid:always

		[ ca ]
		    default_ca      = CA_default            		  # The default ca section

		    [ CA_default ]
		    policy         = policy_any            	    	  # default policy
		    email_in_dn    = no                    	    	  # Don't add the email into cert DN
		    dir            = ./              	   	    	  # top dir
		    database       = $isv_b_ca_db_dir"/index.txt" 	  # index file.
		    new_certs_dir  = $isv_b_ca_db_dir             	  # new certs dir
		    certificate    = $isv_b_ca_crt    	    	  	  # The CA cert
		    serial         = $isv_b_ca_serial_path       	  # serial no file
		    private_key    = $isv_b_ca_key     	    	  	  # CA private key

		    [ policy_any ]
		    countryName            = optional
		    stateOrProvinceName    = optional
		    organizationName       = optional
		    organizationalUnitName = optional
		    commonName             = optional
		    emailAddress           = optional
		CONFIG
		openssl ca -batch -md sha256 \
			-out $isv_b1_crt \
			-config <(cat) \
			-in $isv_b1_request \
			-startdate $root_start_date \
			-enddate $isv_b1_end_date

	rm $isv_b1_request

	# openssl x509 -text -noout -in $outdir/isv_b1.crt

	cat <<-CONFIG |
		distinguished_name=1
		x509_extensions=2
		[1]
		[2]
		keyUsage=critical, digitalSignature
		basicConstraints=critical, CA:FALSE
		subjectKeyIdentifier=hash
		#authorityKeyIdentifier=keyid:always

		[ ca ]
		    default_ca      = CA_default            		  # The default ca section

		    [ CA_default ]
		    policy         = policy_any            	    	  # default policy
		    email_in_dn    = no                    	    	  # Don't add the email into cert DN
		    dir            = ./              	   	    	  # top dir
		    database       = $isv_b_ca_db_dir"/index.txt" 	  # index file.
		    new_certs_dir  = $isv_b_ca_db_dir             	  # new certs dir
		    certificate    = $isv_b_ca_crt	   	    	  # The CA cert
		    serial         = $isv_b_ca_serial_path       	  # serial no file
		    private_key    = $isv_b_ca_key	    	    	  # CA private key

		    [ policy_any ]
		    countryName            = optional
		    stateOrProvinceName    = optional
		    organizationName       = optional
		    organizationalUnitName = optional
		    commonName             = optional
		    emailAddress           = optional
		CONFIG
	openssl ca -batch -md sha256 \
			-out $isv_b2_crt \
			-config <(cat) \
			-in $isv_b2_request \
			-startdate $root_start_date \
			-enddate $isv_b2_end_date

	rm $isv_b2_request

	# openssl x509 -text -noout -in $outdir/isv_b2.crt

	#===================================================================================
	# Creating CRL, Root CA revoking none
	#===================================================================================

	cat <<-CONFIG |

	    [ ca ]
	    default_ca      = CA_default           			 # The default ca section

	    [ CA_default ]
	    policy         = policy_any            			# default policy
	    email_in_dn    = no                    			# Don't add the email into cert DN
	    dir            = ./              	   			# top dir
	    database       = $root_ca_db_dir"/index.txt"   		# index file.
	    new_certs_dir  = $root_ca_db_dir       	   		# new certs dir
	    certificate    = $mdtp_root_ca_crt 				# The CA cert
	    serial         = $root_ca_serial_path          		# serial no file
	    private_key    = $mdtp_root_ca_key     			# CA private key
	    default_md     = md5
	    default_crl_days= 30                  			 # how long before next CRL

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG

	openssl ca -keyfile $outdir/mdtp_root_ca.key -cert $outdir/mdtp_root_ca.crt -gencrl -out $outdir/root_ca_revoking_none.crl -config <(cat)

	#===================================================================================
	# Creating CRL, ISV B CA revoking none
	#===================================================================================

	cat <<-CONFIG |

	    [ ca ]
	    default_ca      = CA_default           			 # The default ca section

	    [ CA_default ]
	    policy         = policy_any            			# default policy
	    email_in_dn    = no                    			# Don't add the email into cert DN
	    dir            = ./              	   			# top dir
	    database       = $isv_b_ca_db_dir"/index.txt"   		# index file.
	    new_certs_dir  = $isv_b_ca_db_dir       	   		# new certs dir
	    certificate    = $mdtp_root_ca_crt 				# The CA cert
	    serial         = $isv_b_ca_serial_path           		# serial no file
	    private_key    = $mdtp_root_ca_key     			# CA private key
	    default_md     = md5
	    default_crl_days= 30                  			# how long before next CRL

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG

	openssl ca -keyfile $outdir/isv_b_ca.key -cert $outdir/isv_b_ca.crt -gencrl -out $outdir/isv_b_ca_revoking_none.crl -config <(cat)

}

function sign_msg {

	#===================================================================================
	# Creating S/MIME
	#===================================================================================
	echo "Creating the S/MIME"
	touch $outdir/chain.crt
	cat $mdtp_root_ca_crt > $outdir/chain.crt
	cat $isv_a_ca_crt >> $outdir/chain.crt

	openssl smime -sign -md sha256 -binary -noattr -in $1 -nodetach -out $outdir/msg-signed.bin \
			-outform DER -inkey $outdir/isv_a1.key -signer $outdir/isv_a1.crt -certfile $outdir/chain.crt

	rm $outdir/chain.crt

}

function verify_signed_msg {
	echo "CMS verification"
	#verify the signed msg with the root ca crt
	openssl cms -verify -inform DER -in $1 -CAfile $outdir/mdtp_root_ca.crt

}

function print_smime {
	echo "Printing S/MIME"
	#print smime output
	openssl smime -pk7out -inform DER -in test_pki/activate-msg-signed.bin
	#print the smime in ASN1 format
	#openssl asn1parse -inform DER -in $outdir/activate-msg-signed.bin
}

function print_certs {
	echo "Printing all certificates inside the SMIME"
	#print all the certificates in pkcs7
	openssl pkcs7 -inform DER -in test_pki/activate-msg-signed.bin -print_certs
}

function crl_revoke_isv_b_ca {
	echo "Creating CRL, signed by MDTP Root CA, revoking ISV B CA"

	cat <<-CONFIG |

	    [ ca ]
	    default_ca      = CA_default           			 # The default ca section

	    [ CA_default ]
	    policy         = policy_any            			# default policy
	    email_in_dn    = no                    			# Don't add the email into cert DN
	    dir            = ./              	   			# top dir
	    database       = $root_ca_db_dir"/index.txt"   		# index file.
	    new_certs_dir  = $root_ca_db_dir       	   		# new certs dir
	    certificate    = $mdtp_root_ca_crt 				# The CA cert
	    serial         = $root_ca_serial_path          		# serial no file
	    private_key    = $mdtp_root_ca_key     			# CA private key
	    default_md     = md5
	    default_crl_days= 30                  			 # how long before next CRL

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG
	openssl ca -keyfile $outdir/mdtp_root_ca.key -cert $outdir/mdtp_root_ca.crt -revoke $outdir/isv_b_ca.crt -config <(cat)

	sleep 2

	cat <<-CONFIG |

	    [ ca ]
	    default_ca      = CA_default           			 # The default ca section

	    [ CA_default ]
	    policy         = policy_any            			# default policy
	    email_in_dn    = no                    			# Don't add the email into cert DN
	    dir            = ./              	   			# top dir
	    database       = $root_ca_db_dir"/index.txt"   		# index file.
	    new_certs_dir  = $root_ca_db_dir       	   		# new certs dir
	    certificate    = $mdtp_root_ca_crt 				# The CA cert
	    serial         = $root_ca_serial_path          		# serial no file
	    private_key    = $mdtp_root_ca_key     			# CA private key
	    default_md     = md5
	    default_crl_days= 30                  			 # how long before next CRL

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG
	openssl ca -keyfile $outdir/mdtp_root_ca.key -cert $outdir/mdtp_root_ca.crt -gencrl -out $outdir/root_ca_revoking_isv_b_ca.crl -config <(cat)
}

function crl_revoke_isv_b2 {
	echo "Creating CRL, signed by ISV CA, revoking ISV B2"

	cat <<-CONFIG |

	    [ ca ]
	    default_ca      = CA_default           			 # The default ca section

	    [ CA_default ]
	    policy         = policy_any            			# default policy
	    email_in_dn    = no                    			# Don't add the email into cert DN
	    dir            = ./              	   			# top dir
	    database       = $isv_b_ca_db_dir"/index.txt"   		# index file.
	    new_certs_dir  = $isv_b_ca_db_dir       	   		# new certs dir
	    certificate    = $mdtp_root_ca_crt 				# The CA cert
	    serial         = $isv_b_ca_serial_path           		# serial no file
	    private_key    = $mdtp_root_ca_key     			# CA private key
	    default_md     = md5
	    default_crl_days= 30                  			# how long before next CRL

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG

	openssl ca -keyfile $outdir/isv_b_ca.key -cert $outdir/isv_b_ca.crt -revoke $outdir/isv_b2.crt -config <(cat)

	sleep 2

	cat <<-CONFIG |

	    [ ca ]
	    default_ca      = CA_default           			 # The default ca section

	    [ CA_default ]
	    policy         = policy_any            			# default policy
	    email_in_dn    = no                    			# Don't add the email into cert DN
	    dir            = ./              	   			# top dir
	    database       = $isv_b_ca_db_dir"/index.txt"   		# index file.
	    new_certs_dir  = $isv_b_ca_db_dir       	   		# new certs dir
	    certificate    = $mdtp_root_ca_crt 				# The CA cert
	    serial         = $isv_b_ca_serial_path           		# serial no file
	    private_key    = $mdtp_root_ca_key     			# CA private key
	    default_md     = md5
	    default_crl_days= 30                  			# how long before next CRL

	    [ policy_any ]
	    countryName            = optional
	    stateOrProvinceName    = optional
	    organizationName       = optional
	    organizationalUnitName = optional
	    commonName             = optional
	    emailAddress           = optional
		CONFIG

	openssl ca -keyfile $outdir/isv_b_ca.key -cert $outdir/isv_b_ca.crt -gencrl -out $outdir/isv_b_ca_revoking_isv_b2.crl -config <(cat)
}

if [ "$#" != 1 -a "$#" != 2 ]; then
	echo ""
	echo "MDTP PKI - version $version"
	echo "Usage: $0 COMMAND"
	echo " "
	echo "available commands:"
	echo "    gen_pki              				generate the pki"
	echo "    sign_msg path_to_unsigned_msg_file    		sign the message at the given path"
	echo "    verify_signed_msg path_to_signed_msg_file     	verify the message"
	echo "    root_ca_revoking_isv_b_ca  				create crl, signed by mdtp root ca, revoking isv b ca"
	echo "    isv_b_ca_revoking_isv_b2    			create crl, signed by isv ca, revoking isv b"
	echo "    print_smime            				print the smime"
	echo "    print_certs          				print all the certificates inside the smime"
	echo " "
	exit
fi

if [ "$1" = "gen_pki" ]; then
	gen_pki
	exit
elif [ "$1" = "sign_msg" ]; then
	if [ -e "$2" ]
	then
		sign_msg $2
	else
		echo "please provide valid path to file"
	fi
	exit
elif [ "$1" = "root_ca_revoking_isv_b_ca" ]; then
	crl_revoke_isv_b_ca
	exit
elif [ "$1" = "isv_b_ca_revoking_isv_b2" ]; then
	crl_revoke_isv_b2
	exit
elif [ "$1" = "verify_signed_msg" ]; then
	if [ -e "$2" ]
	then
		verify_signed_msg $2
	else
		echo "please provide valid path to file"
	fi
	exit
elif [ "$1" = "print_smime" ]; then
	print_smime
	exit
elif [ "$1" = "print_certs" ]; then
	print_certs
	exit
else
	echo "Wrong command"
	exit
fi
