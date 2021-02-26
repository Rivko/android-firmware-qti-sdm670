The GenSecImage tool is used to generate secured images.

ReadMe.txt           - This file.
gensecimage.py       - Main GenSecImage python script.
mbn_tools.py         - Library for image processing.
gensecimageconfig.py - Script to use meta build to generate GenSecImage configuration files.
qpsa                 - Directory containing Qualcomm Platform Signing Application (QPSA)
                       scripts and resources.
ssd                  - Directory containing Qualcomm Secured Software Download (SSD)
                       tool and resources.
resources            - Directory containing GenSecImage resources

------------------------------------------------------------------------------
 Running gensecimage
------------------------------------------------------------------------------
1. Once configuration files are ready, run the following command to sign images:

   python gensecimage.py -stage=<desired stage> --section=<desired section>
                         --config=resources/<target>_gensecimage.cfg

or

   python gensecimage.py -stage=<desired stage> --section=<desired section>
                         --config=resources/<target>_gensecimage.cfg
                         --signconfig=resources/<target>_signingattr_qpsa.cfg
                         --postsignconfig=resources/<target>_postsigncmd.cfg

2. Run the following to see usage for the tool:
   python gensecimage.py

--------------------------------------------------------------------------------
Disclaimer
--------------------------------------------------------------------------------
The licensee takes responsibility for the use of the information and tools in
the gensecimage and QPSA. The licensee takes responsibility for what code they authorize.
Should the licensee sign malware, poorly behaving or poorly developed code,
QC takes no responsibility for such code.

All the non-commercial root certificates packaged in QPSA are intended only for pre-commercial
device development and is NOT suitable for inclusion in commercial devices.
Qualcomm makes no effort to safeguard the security of the private key of
the non-commmercial root certificates or the private key of
the non-commercial CA certificates or the non-commercial CA certificates
issued under the non-commerical root certficiates each of which are intended for distribution
to multiple unknown parties allowing such parties to generate signatures that will allow applications
to execute on devices which embed the non-commercial root certificate.
