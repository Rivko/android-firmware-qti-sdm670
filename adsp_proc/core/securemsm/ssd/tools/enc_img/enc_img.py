#============================================================================
#
# CBSP Builders build rules
#
# GENERAL DESCRIPTION
#    Contains methods to encrypt PIL segments
#
# Copyright  2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/core.qdsp6/2.1/securemsm/ssd/tools/enc_img/enc_img.py#1 $
#  $DateTime: 2017/07/21 22:10:47 $
#  $Author: pwbldsvc $
#  $Change: 13907704 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 04/10/12    hv       Initial revision
#
#============================================================================
import ConfigParser
import os
import binascii
import shutil
import sys
import getopt

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------
DEBUG = 0

E_SUCCESS = 0
E_FAIL = 1

SSD_TOOLS_DIR = "${BUILD_ROOT}/core/securemsm/ssd/tools/"
ENC_IMG = SSD_TOOLS_DIR + "enc_img/"
CONFIG_FILE = ENC_IMG + "md_config.ini"
SSD_KEYS_DIR = SSD_TOOLS_DIR + "gen_rsa_keys/"
DEV_KEY_DIR = "golden_dev/"
OEM_KEY_DIR = "golden_oem/"
PRODUCTION_KEYS = 0

SSD_SECURE_KEY_SERVER_PATH = r"\\baskin\happyfeet\users\hvimadal\aswcrm_8960_fluid\bsquare_msm8960_keys"
SSD_SECURE_KEY_SERVER_OEM_DIR = "oem"
SSD_SECURE_KEY_SERVER_DEV_DIR = "dev"

env                         = 0
file_list                   = ("sbl1", "sbl2", "sbl3", "rpm", "tz")
PAD_BYTE_1                  = 255
XML_HEADER_MAXSIZE          = 2048

#----------------------------------------------------------------------------
# Copy key files from remote location if specified
#----------------------------------------------------------------------------
def copy_ssd_keys(env):
   
   #if the server key location exists use that
   if os.path.exists(SSD_SECURE_KEY_SERVER_PATH):      
      global OEM_KEY_DIR
      global DEV_KEY_DIR
      global PRODUCTION_KEYS

      OEM_KEY_DIR = SSD_SECURE_KEY_SERVER_OEM_DIR
      DEV_KEY_DIR = SSD_SECURE_KEY_SERVER_DEV_DIR
      PRODUCTION_KEYS = 1
      
      print "Using keys from : " + SSD_SECURE_KEY_SERVER_PATH
      #remove keys if they were left behind
      if True == os.path.exists(SSD_KEYS_DIR + OEM_KEY_DIR):
         shutil.rmtree(SSD_KEYS_DIR + OEM_KEY_DIR)
      if True == os.path.exists(SSD_KEYS_DIR + DEV_KEY_DIR):
         shutil.rmtree(SSD_KEYS_DIR + DEV_KEY_DIR)
	  
      #copy keys locally
      dst = SSD_KEYS_DIR + "\\" + OEM_KEY_DIR
      src = SSD_SECURE_KEY_SERVER_PATH + "\\" + OEM_KEY_DIR
      shutil.copytree(src, dst)

      dst = SSD_KEYS_DIR + "\\" + DEV_KEY_DIR
      src = SSD_SECURE_KEY_SERVER_PATH + "\\" + DEV_KEY_DIR
      shutil.copytree(src, dst)

   else:
      print "WARNING: not using production keys;"

class SSDConfigClass:
   
   # Tag values
   IEK_ENC_ALGO = "RSA-2048"
   IEK_ENC_PADDING_TYPE = "PKCS#1-V1.5"
   IMG_ENC_PADDING_TYPE = "NO_PAD"
   IMG_ENC_OPERATION_MODE = "CBC_MODE"
   IMG_HASH_ALGO = "SHA-256"
   MD_SIG_DGST_ALGO = "SHA-256"
   MD_SIG_ALGO = "RSA-2048"
   MD_SIG_PADDING_TYPE = "PKCS#1-V1.5"
   
   CONFIG_BUFF_SIZE = 4096
   IEK_ENC_PUB_KEY_ID_BUFF_SIZE = 512
   IEK_CIPHER_VALUE_SIZE = 1536
   IMG_ENC_IV_BUFF_SIZE = 32
   MD_SIG_OEM_PUB_KEY_ID_BUFF_SIZE = 512
   MD_SIGNATURE_SIZE =1536

   # parsed from INI file
   MD_VERSION_VAL = ""
   MFG_ID_VAL = ""
   SW_VERSION_VAL = ""
   IMG_ENC_ALGO_VAL = ""
       
   IV_FILE = ""
   
   DEV_PUB_KEY_FILE_NAME = ""
   DEV_PUB_KEY_ID_FILE_NAME = ""
   OEM_PRIV_KEY_FILE_NAME = ""
   OEM_PUB_KEY_ID_FILE_NAME = ""
        
   #other local variables
   config_p = ConfigParser.ConfigParser()
   
   status = 0
      
   #open the file & read the data
   def __init__ (self, config_file_name):
      try:
         # need to have full file paths here, cleanup later maybe
         self.RAND_IV_BIN_FILE_NAME = ENC_IMG + "rand_iv.bin"
         self.RAND_IEK_BIN_FILE_NAME = ENC_IMG + "rand_iek.bin"
         self.IV_BIN_FILE_NAME = ENC_IMG + "iv.bin"
         self.IV_HEX_FILE_NAME = ENC_IMG + "iv.hex"         
         self.IEK_BIN_FILE_NAME = ENC_IMG + "iek.bin"
         self.IEK_HEX_FILE_NAME = ENC_IMG + "iek.hex"
         self.SSD_MD_INT_FILE_NAME = ENC_IMG + "temp_md.xml"
         self.SSD_MD_FINAL_FILE_NAME = ENC_IMG + "ssd_md.xml"
         self.SSD_HASH_INT_FILE_NAME = ENC_IMG + "temp_hash.dat"
         self.SSD_TEMP_SIG_DAT_FILE_NAME = ENC_IMG + "temp_sig.dat"
         self.SSD_TEMP_SIG_B64_FILE_NAME = ENC_IMG + "temp_sig.b64"
                
         self.config_p.readfp(open(config_file_name))            
      except:
         print "Failed to open file:", config_file_name
         self.status = 1

   # get the tag from the file
   def get_config_tag_value(self, section, tag):
      try:
         tag_val = self.config_p.get(section, tag)
         return tag_val
      except:
            return ""

   #parse the config file values
   def parse_config_file(self):
      self.MD_VERSION_VAL = self.get_config_tag_value("QC_DATA", "MD_VERSION")
      self.MFG_ID_VAL = self.get_config_tag_value("OEM_CONFIG", "MFG_ID")
      self.SW_VERSION_VAL = self.get_config_tag_value("OEM_CONFIG", "SW_VERSION")
      self.IMG_ENC_ALGO_VAL = self.get_config_tag_value("OEM_CONFIG", "IMG_ENC_ALGO")
      
      self.DEV_PUB_KEY_FILE_NAME = SSD_KEYS_DIR + DEV_KEY_DIR + '/' + "rsa_pub_key.pem"
      self.DEV_PUB_KEY_ID_FILE_NAME = SSD_KEYS_DIR + DEV_KEY_DIR + '/' + "rsa_pub_key_id.b64"
      self.OEM_PRIV_KEY_FILE_NAME = SSD_KEYS_DIR + OEM_KEY_DIR + '/' + "rsa_pr_key.pem"
      self.OEM_PUB_KEY_ID_FILE_NAME = SSD_KEYS_DIR + OEM_KEY_DIR + '/' + "rsa_pub_key_id.b64"

   def ssd_gen_key_iv(env):

      # remove old IEK & IV if they exist
      if True == os.path.exists(ssd_p.RAND_IEK_BIN_FILE_NAME):
         os.remove(ssd_p.RAND_IEK_BIN_FILE_NAME)
      if True == os.path.exists(ssd_p.RAND_IV_BIN_FILE_NAME):
         os.remove(ssd_p.RAND_IV_BIN_FILE_NAME)
      if True == os.path.exists(ssd_p.IEK_HEX_FILE_NAME):
         os.remove(ssd_p.IEK_HEX_FILE_NAME)
      if True == os.path.exists(ssd_p.IV_HEX_FILE_NAME):
         os.remove(ssd_p.IV_HEX_FILE_NAME)
      if True == os.path.exists(ssd_p.IEK_BIN_FILE_NAME):
         os.remove(ssd_p.IEK_BIN_FILE_NAME)
      if True == os.path.exists(ssd_p.IV_BIN_FILE_NAME):
         os.remove(ssd_p.IV_BIN_FILE_NAME)

      # generate a new IEK
      rand_iek = open(ssd_p.RAND_IEK_BIN_FILE_NAME, "wb")
      rand_iek.write(os.urandom(1024))
      rand_iek.close()
      os.system("openssl rand 16 -out " + ssd_p.IEK_BIN_FILE_NAME + " -rand " + ssd_p.RAND_IEK_BIN_FILE_NAME)
      iek_bin_fp = open(ssd_p.IEK_BIN_FILE_NAME, "rb")
      iek_bin = iek_bin_fp.read(16)
      iek_bin_fp.close()

      # convert IEK to hex & write it back to file for later
      iek_b64_fp = open(ssd_p.IEK_HEX_FILE_NAME, "w")
      iek_b64_fp.write(binascii.hexlify(iek_bin))
      iek_b64_fp.close()

      # generate a new IV
      rand_iv = open(ssd_p.RAND_IV_BIN_FILE_NAME, "wb")
      rand_iv.write(os.urandom(1024))
      rand_iv.close()
      os.system("openssl rand 16 -out " + ssd_p.IV_BIN_FILE_NAME + " -rand " + ssd_p.RAND_IV_BIN_FILE_NAME)
      iv_bin_fp = open(ssd_p.IV_BIN_FILE_NAME, "rb")
      iv_bin = iv_bin_fp.read(16)
      iv_bin_fp.close()

      # convert IV to hex & write it back to file for later
      iv_b64_fp = open(ssd_p.IV_HEX_FILE_NAME, "w")
      iv_b64_fp.write(binascii.hexlify(iv_bin))    
      iv_b64_fp.close()

      # remove temp files
      os.remove(ssd_p.RAND_IV_BIN_FILE_NAME)
      os.remove(ssd_p.RAND_IEK_BIN_FILE_NAME)

   def create_temp_xml(self):
      temp_buf = ""
      # delete the temp file if it exists
      try:        
         os.stat(self.SSD_MD_INT_FILE_NAME)
         os.remove(self.SSD_MD_INT_FILE_NAME)
      except:
         print ""
          
      fd = open(self.SSD_MD_INT_FILE_NAME, 'w')
      
      fd.write("<MD_SIGN>\r")
      
      fd.write("    <MD_VERSION>" + self.MD_VERSION_VAL + "</MD_VERSION>\r")
      
      fd.write("    <MFG_ID>" + self.MFG_ID_VAL + "</MFG_ID>\r")
      
      fd.write("    <SW_VERSION>" + self.SW_VERSION_VAL + "</SW_VERSION>\r")
      
      fd.write("    <IMG_ENC_INFO>\r")
      fd.write("        <IMG_ENC_ALGO>" + self.IMG_ENC_ALGO_VAL + "</IMG_ENC_ALGO>\r")
              
      # Skip everything the IMG is not encrypted
      if "NULL" == self.IMG_ENC_ALGO_VAL:
         fd.write("    </IMG_ENC_INFO>\r" + "</MD_SIGN>")
         fd.close()
         return

      fd.write("        <IMG_ENC_PADDING_TYPE>" + self.IMG_ENC_PADDING_TYPE + "</IMG_ENC_PADDING_TYPE>\r")
      
      fd.write("        <IMG_ENC_OPERATION_MODE>" + self.IMG_ENC_OPERATION_MODE + "</IMG_ENC_OPERATION_MODE>\r")
      
      iv_bin_fp = open(self.IV_BIN_FILE_NAME, "rb")
      fd.write("        <IMG_ENC_IV>" + (binascii.b2a_base64(iv_bin_fp.read())).rstrip() + "</IMG_ENC_IV>\r")
      iv_bin_fp.close()
      fd.write("    </IMG_ENC_INFO>\r")
      
      fd.write("    <IEK_ENC_INFO>\r")
      fd.write("        <IEK_ENC_ALGO>" + self.IEK_ENC_ALGO + "</IEK_ENC_ALGO>\r")
      fd.write("        <IEK_ENC_PADDING_TYPE>" + self.IEK_ENC_PADDING_TYPE + "</IEK_ENC_PADDING_TYPE>\r")
      temp_key_id = open(self.DEV_PUB_KEY_ID_FILE_NAME, 'rb')
      fd.write("        <IEK_ENC_PUB_KEY_ID>" + (temp_key_id.read()).rstrip() + "</IEK_ENC_PUB_KEY_ID>\r")
      temp_key_id.close()
      
      # encrypt the IEK with Device public key
      os.system("openssl rsautl -encrypt -pkcs -in " + \
                self.IEK_BIN_FILE_NAME + \
                " -inkey " +  self.DEV_PUB_KEY_FILE_NAME + \
                " -pubin -out temp_iek.dat")
      # convert the encrypted key to b64 format
      os.system("openssl enc -base64 -A -in temp_iek.dat -out temp_iek.b64")
        
      temp_iek = open("temp_iek.b64")
      fd.write("        <IEK_CIPHER_VALUE>" + (temp_iek.read()).rstrip() + "</IEK_CIPHER_VALUE>\r")
      
      fd.write("    </IEK_ENC_INFO>\r")
          
      fd.write("    <MD_SIG_INFO>\r")

      fd.write("        <MD_SIG_ALGO>" + self.MD_SIG_ALGO + "</MD_SIG_ALGO>\r")
      fd.write("        <MD_SIG_DGST_ALGO>" + self.MD_SIG_DGST_ALGO + "</MD_SIG_DGST_ALGO>\r")
      fd.write("        <MD_SIG_PADDING_TYPE>" + self.MD_SIG_PADDING_TYPE + "</MD_SIG_PADDING_TYPE>\r")
      temp_key_id = open(self.OEM_PUB_KEY_ID_FILE_NAME, 'rb')
      fd.write("        <MD_SIG_OEM_PUB_KEY_ID>" + temp_key_id.read().rstrip() + "</MD_SIG_OEM_PUB_KEY_ID>\r")
      temp_key_id.close()

      fd.write("    </MD_SIG_INFO>\r")
      fd.write("</MD_SIGN>")
      # close files & remove temp files                
      fd.close()
      temp_iek.close()
      os.remove("temp_iek.dat")
      os.remove("temp_iek.b64")
          
   def write_final_xml_to_file(self):
      try:            
         fd_final = open(self.SSD_MD_FINAL_FILE_NAME, 'w+')

         # write the XML header tag
         fd_final.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r")
         
         # Write top level SSD Metadata tag
         fd_final.write("<SSD_METADATA>\r")
         
         # read the Intermediate XML & write it to Final XML 
         fd_int = open(self.SSD_MD_INT_FILE_NAME, 'r')

         # write the intermediate file to final file
         fd_final.write((fd_int.read()).rstrip())
         
         # close the intermediate file, we need to use it for signing
         fd_int.close()
         
         fd_final.write("\r<MD_SIGNATURE>")
         # generate the signature over the intermediate XML
         # call the RSA sign function with the OEM private key
         #
         # run SHA-256 hash over the intermediate XML file
         os.system("openssl dgst -binary -sha256 -out " + self.SSD_HASH_INT_FILE_NAME + " " + self.SSD_MD_INT_FILE_NAME)
         
         # sign the Hash
         os.system("openssl rsautl -in " + self.SSD_HASH_INT_FILE_NAME + " -inkey " \
                   + self.OEM_PRIV_KEY_FILE_NAME + \
                   " -pkcs -sign -keyform PEM -out " + self.SSD_TEMP_SIG_DAT_FILE_NAME)
         
         # covert the Signature to b64 format
         os.system("openssl enc -base64 -A -in " + self.SSD_TEMP_SIG_DAT_FILE_NAME + " -out " + self.SSD_TEMP_SIG_B64_FILE_NAME)
         fd_sig = open(self.SSD_TEMP_SIG_B64_FILE_NAME)
         fd_final.write((fd_sig.read()).rstrip())
         fd_sig.close()
         fd_final.write("</MD_SIGNATURE>\r")
         fd_final.write("</SSD_METADATA>\r")
         
         # do we want to do padding?

         fd_final.seek(0,0)
         fd_final_buf = fd_final.read()
         
         #close all the files; remove temp files
         fd_int.close()
         fd_final.close()
         os.remove(self.SSD_MD_INT_FILE_NAME)
         os.remove(self.SSD_MD_FINAL_FILE_NAME)
         os.remove(self.SSD_HASH_INT_FILE_NAME)
         os.remove(self.SSD_TEMP_SIG_DAT_FILE_NAME)
         os.remove(self.SSD_TEMP_SIG_B64_FILE_NAME)

         return fd_final_buf
         
      except:            
         print"Error in write_final_xml_to_file()"
         return ""
        
   def _to_hex_string(self, int_):
        
      hex_values = { 0: '0', 1: '1', 2: '2', 3: '3', 4:'4', 5:'5', 6:'6', 7:'7', 8:'8', 9:'9', \
                     10:'a', 11:'b', 12: 'c', 13:'d', 14: 'e', 15:'f'}
      
      MSB = int_/16
      LSB = int_%16
      
      return hex_values.get(MSB) + hex_values.get(LSB)            
            
   def enc_segment(self, seg_no, ip_buf, ip_buf_len):
        
      # how much data are we going to encrypt
      data_to_enc = ip_buf_len - (ip_buf_len%16)
             
      temp_ip = open("temp_ip.dat", "wb")
      temp_ip.write(ip_buf[:data_to_enc])
      temp_ip.close()
      iek_hex_fp = open(self.IEK_HEX_FILE_NAME, "r")
      iv_hex_fp = open(self.IV_HEX_FILE_NAME, "r")
      temp_iv_buf = iv_hex_fp.read()
      
      temp_iv_hex = temp_iv_buf[14:16]
      temp_iv_int = int(temp_iv_hex, 16)
      temp_iv_int = temp_iv_int ^ seg_no
              
      temp_iv_str = self._to_hex_string(temp_iv_int)
      temp_iv_fp = open("temp_iv.hex", "w")
      temp_iv_fp.write(temp_iv_buf[:14])
      temp_iv_fp.write(temp_iv_str)
      temp_iv_fp.write(temp_iv_buf[16:])
      temp_iv_fp.close()
      
      temp_iv_fp = open("temp_iv.hex", "r")
      
      os.system("openssl enc -aes-128-cbc -in temp_ip.dat -K " + (iek_hex_fp.read()).rstrip() + \
                " -iv " + (temp_iv_fp.read()).rstrip() + \
                " -out temp_op.dat -nopad" )
      os.remove("temp_ip.dat")
      temp_iv_fp.close()
      os.remove("temp_iv.hex")
      temp_op = open("temp_op.dat", "r+b")
      temp_op.seek(0, os.SEEK_END)
      temp_op.write(ip_buf[data_to_enc:])
      temp_op.seek(0,0)
      temp_op_size = os.path.getsize("temp_op.dat")
      ip_buf = temp_op.read(temp_op_size)
      temp_op.close()
      os.remove("temp_op.dat")
      
      iek_hex_fp.close()
      iv_hex_fp.close()
 
      return ip_buf
        
        
# Exported Functions
# Initialize SSD & parse the config file
def ssd_generate(ssd_tools_dir):

   # Set up paths + assign to global vars
   global SSD_TOOLS_DIR   
   SSD_TOOLS_DIR = ssd_tools_dir

   global ENC_IMG
   ENC_IMG = SSD_TOOLS_DIR + "enc_img/"

   global CONFIG_FILE
   CONFIG_FILE = ENC_IMG + "md_config.ini"

   global SSD_KEYS_DIR
   SSD_KEYS_DIR = SSD_TOOLS_DIR + "gen_rsa_keys/"


   # Initialize SSD Config Pointer 
   global ssd_p
   ssd_p = ssd_init(0)
  
   return None

def ssd_init(env):
    
   ssd_p = SSDConfigClass(CONFIG_FILE)
   return ssd_p

def ssd_get_se_fname(env):
    return ssd_p.IV_BIN_FILE_NAME


# encrypts the incoming buffer
def ssd_enc_seg(env, ip_buf_index, ip_buf, ip_buf_len):

   if E_FAIL == ssd_p.status:
       #Something went wrong
      exit()

   #Parse the file    
   return ssd_p.enc_segment(ip_buf_index, ip_buf, ip_buf_len)


# returns a buffer that holds the XML header    
def ssd_get_md(env, magic_num = False):
   global PRODUCTION_KEYS
    
   copy_ssd_keys(env)
   ssd_p.parse_config_file()

   if E_FAIL == ssd_p.status:
      #Something went wrong
      exit()

   #generate the key & the IV
   ssd_p.ssd_gen_key_iv()

   # Parse the file   
   ssd_p.create_temp_xml()

   if True == magic_num:
      temp_xml = "tzssdimg"
   else:
      temp_xml = ""
   temp_xml += ssd_p.write_final_xml_to_file()

   #Remove all the RSA keys from local machine
   if 1 == PRODUCTION_KEYS:
      print "Removing production keys"
      shutil.rmtree(SSD_KEYS_DIR + OEM_KEY_DIR)
      shutil.rmtree(SSD_KEYS_DIR + DEV_KEY_DIR)      

   return temp_xml

def ssd_deinit(env):
   global DEBUG
   if DEBUG == 0:
      # clean up temp files
      if True == os.path.exists(ssd_p.IEK_HEX_FILE_NAME):
         os.remove(ssd_p.IEK_HEX_FILE_NAME)
      if True == os.path.exists(ssd_p.IEK_BIN_FILE_NAME):
         os.remove(ssd_p.IEK_BIN_FILE_NAME)
      if True == os.path.exists(ssd_p.IV_HEX_FILE_NAME):
         os.remove(ssd_p.IV_HEX_FILE_NAME)
      if True == os.path.exists(ssd_p.IV_BIN_FILE_NAME):
         os.remove(ssd_p.IV_BIN_FILE_NAME)
   return



def pad_file(fp, num_bytes, value):
    
   while num_bytes > 0:
      fp.write('%c' % value)
      num_bytes -= 1

   return 

def usage ():
    print "========================================================"
    print "options"

    print "-h, --help       prints this help"

    print "usage"
    print "enc_bldr.py --ip_file=<file to encrypt> --op_file=<encrypted file name> --tools_dir=<ssd tools root dir>"
    print "eg"
    print "enc_bldr.py --ip_file=c:\common\builds\NON-HLOS.bin --op_file=c:\common\builds\NON-HLOS.enc --tools_dir=c:\common\\tools\ssd\\tools\""
    print "========================================================"

def main():
    global SSD_SECURE_KEY_SERVER_PATH
    SSD_KEY_SERVER_PATH = ""
    TOOLS_DIR = ""
    ip_fn = ""
    op_fn = ""

    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help", "ip_file=", "op_file=", "tools_dir=", "keys="])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)
    if len(sys.argv) < 2:
        usage()
        sys.exit(2)
    for o, a in opts:
        if o in ("-h", "--help"):
            usage()            
            sys.exit()
        elif o in ("--ip_file"):
            ip_fn = a
        elif o in ("--op_file"):
            op_fn = a
        elif o in ("--tools_dir"):
            TOOLS_DIR = a
        elif o in ("--keys"):
            SSD_KEY_SERVER_PATH = a

    if ip_fn == "" :
       print "Missing input file name"
       usage()
       sys.exit(2) 

    if op_fn == "":
       print "Missing output file name"
       usage()
       sys.exit(2) 

    if TOOLS_DIR == "":
       TOOLS_DIR = os.getcwd() + "/../"

    print "Tools directory is  : " + TOOLS_DIR

    if SSD_KEY_SERVER_PATH != "":
       SSD_SECURE_KEY_SERVER_PATH = SSD_KEY_SERVER_PATH

    

    ssd_generate(TOOLS_DIR + "/")

    print "========================================================"
    print "Encrypting :" + ip_fn

    #remove existing files    
    if True == os.path.exists(op_fn):
        os.remove(op_fn)
    
    # Init SSD
    ssd_p = ssd_init(env)

    # Generate MD
    ssd_xml = ssd_get_md(env, True)
    ssd_xml_len = len(ssd_xml)

    # Create op file
    op_fp = open(op_fn, "wb")
    op_fp.write(ssd_xml)

    # Pad if necessary to the maximum size 
    if ssd_xml_len <= XML_HEADER_MAXSIZE:
        bytes_to_pad = XML_HEADER_MAXSIZE - ssd_xml_len
        pad_file(op_fp, bytes_to_pad, PAD_BYTE_1) 

    ip_fp = open(ip_fn, "rb")
    ip_data = ip_fp.read()

    op_seg = ssd_enc_seg(env, 0, ip_data, os.path.getsize(ip_fn))
    ip_fp.close()    

    op_fp.write(op_seg)
    op_fp.close()

    ssd_deinit(env)

    print "Output : " + op_fn
    print "========================================================"

if __name__ == "__main__":
    main()
