#
# This script parses a Keys directory and writes out a file named
# keys.inp in the current directory. This keys.inp file can be used to
# provision the keys & Key ID specified in the given keys
# directory over the DRM diag interface.
#
# Usage perl parse_keys.pl
#
# Requirement:
#     The script must be run from the directory that contains the keys
#     directory.
#
use strict;
use Cwd;

my %dev_priv_keys;
my %dev_priv_key_data;

my %dev_pub_key_ids;
my %dev_pub_key_id_data;

my %oem_pub_keys;
my %oem_pub_key_data;

my %oem_pub_key_ids;
my %oem_pub_key_id_data;

my $keys_dir  = "keys";
my $keys_conf_file = "keys.conf";

my $dev_priv_key_tag = "DEV_PRIV_KEY";
my $dev_pub_key_id_tag  = "DEV_PUB_KEY_ID";
my $oem_pub_key_tag = "OEM_PUB_KEY";
my $oem_pub_key_id_tag  = "OEM_PUB_KEY_ID";

# add more tags here if we add more data types

my $qxdm_inp_file = "keys.inp";

my $cwd;

$cwd = cwd ();

chdir $keys_dir or
  die "Unable to cd to $keys_dir";

open KEYS, "<$keys_conf_file" or
  die "Unable to open Keys config file: $!";

my $i = 1;
while (<KEYS>)
{
   if (/\<$dev_priv_key_tag\>(.+)\<\/$dev_priv_key_tag\>/)
   {
     $dev_priv_keys{$1} = $i++;
   }
   if (/\<$dev_pub_key_id_tag\>(.+)\<\/$dev_pub_key_id_tag\>/)
   {
     $dev_pub_key_ids{$1} = $i++;
   }
   if (/\<$oem_pub_key_tag\>(.+)\<\/$oem_pub_key_tag\>/)
   {
     $oem_pub_keys{$1} = $i++;
   }
   if (/\<$oem_pub_key_id_tag\>(.+)\<\/$oem_pub_key_id_tag\>/)
   {
     $oem_pub_key_ids{$1} = $i++;
   }

	# add more tags here if we add more data types
}

close KEYS;

my $dev_priv_key;
foreach $dev_priv_key (keys %dev_priv_keys)
{
  if (!-e $dev_priv_key)
  {
    die "Unable to find file $dev_priv_key";
  }

  open DEV_PRIV_KEY_FILE, "<$dev_priv_key" or
    die "Unable to open Device Private Key file: $!";
  binmode DEV_PRIV_KEY_FILE;

  $dev_priv_key_data{$dev_priv_key} = '';
  while (<DEV_PRIV_KEY_FILE>)
  {
    $dev_priv_key_data{$dev_priv_key} .= $_;
  }
  close DEV_PRIV_KEY_FILE;
}

my $dev_pub_key_id;
foreach $dev_pub_key_id (keys %dev_pub_key_ids)
{
  if (!-e $dev_pub_key_id)
  {
    die "Unable to find file $dev_pub_key_id";
  }

  open DEV_PUB_KEY_ID_FILE, "<$dev_pub_key_id" or
    die "Unable to open Device Public Key ID file: $!";
  binmode DEV_PUB_KEY_ID_FILE;

  $dev_pub_key_id_data{$dev_pub_key_id} = '';
  while (<DEV_PUB_KEY_ID_FILE>)
  {
    $dev_pub_key_id_data{$dev_pub_key_id} .= $_;
  }
  close DEV_PUB_KEY_ID_FILE;
}

my $oem_pub_key;
foreach $oem_pub_key (keys %oem_pub_keys)
{
  if (!-e $oem_pub_key)
  {
    die "Unable to find file $oem_pub_key";
  }

  open OEM_PUB_KEY_FILE, "<$oem_pub_key" or
    die "Unable to open OEM Public Key file: $!";
  binmode OEM_PUB_KEY_FILE;

  $oem_pub_key_data{$oem_pub_key} = '';
  while (<OEM_PUB_KEY_FILE>)
  {
    $oem_pub_key_data{$oem_pub_key} .= $_;
  }
  close OEM_PUB_KEY_FILE;
}

my $oem_pub_key_id;
foreach $oem_pub_key_id (keys %oem_pub_key_ids)
{
  if (!-e $oem_pub_key_id)
  {
    die "Unable to find file $oem_pub_key_id";
  }

  open OEM_PUB_KEY_ID_FILE, "<$oem_pub_key_id" or
    die "Unable to open Device Public Key ID file: $!";
  binmode OEM_PUB_KEY_ID_FILE;

  $oem_pub_key_id_data{$oem_pub_key_id} = '';
  while (<OEM_PUB_KEY_ID_FILE>)
  {
    $oem_pub_key_id_data{$oem_pub_key_id} .= $_;
  }
  close OEM_PUB_KEY_ID_FILE;
}

chdir $cwd;

# Format of data provisioning packet
#
# Byte 0          -> Subsystem command code: 75
# Byte 1          -> Subsystem ID:           72
# Byte 2          -> Subsystem command ID:   01
# Byte 3          -> Subsystem command ID:   04		Provision (1025 0x0401)
# Bytes  4- 7     -> Protocol Version:        1
# Bytes  8-11     -> Data Type:
#                      0: SSD_DIAG_DEV_PRIV_KEY
#                      1: SSD_DIAG_DEV_PUB_KEY_ID
#                      2: SSD_DIAG_OEM_PUB_KEY
#                      3: SSD_DIAG_OEM_PUB_KEY_ID
# Bytes 12-15     -> Format Type:
#                      0: SSD_DIAG_PKCS8_FORMAT
#                      1: SSD_DIAG_PKCS12_FORMAT
#                      2: SSD_DIAG_RAW_FORMAT
# Bytes 16-19     -> logicalNameLen:
#                      length of key or key id name in bytes
# Bytes 20-23     -> dataLen
#                      length of key or key id data in bytes
# Remaining bytes -> key or key id name, followed by key key id data
#

open INPFILE, ">$qxdm_inp_file" or
  die "Unable to open $qxdm_inp_file for writing";

my $dev_priv_key_len_bytes;
my $dev_priv_key_name;
my $dev_priv_key_name_len;
my $dev_priv_key_name_len_bytes;
my $dev_priv_key_data;
foreach $dev_priv_key (sort {$dev_priv_keys{$a} <=> $dev_priv_keys{$b}} keys %dev_priv_keys)
{
  $dev_priv_key_name           = join (',', map ord, split (//, $dev_priv_key));
  $dev_priv_key_name_len       = length $dev_priv_key;
  $dev_priv_key_name_len_bytes = convert_len_to_bytes ($dev_priv_key_name_len);
  $dev_priv_key_data           = $dev_priv_key_data{$dev_priv_key};
  $dev_priv_key_len_bytes      = convert_len_to_bytes (length $dev_priv_key_data);
  $dev_priv_key_data           = join (',', map ord, split (//, $dev_priv_key_data));
  print INPFILE <<end_dev_priv_key_data;
\!Provisioning data for key $dev_priv_key
\{75,72,01,04,1,0,0,0,0,0,0,0,0,0,0,0,$dev_priv_key_name_len_bytes,$dev_priv_key_len_bytes,$dev_priv_key_name,$dev_priv_key_data\}

end_dev_priv_key_data
}

my $dev_pub_key_id_len_bytes;
my $dev_pub_key_id_name;
my $dev_pub_key_id_name_len;
my $dev_pub_key_id_name_len_bytes;
my $dev_pub_key_id_data;
foreach $dev_pub_key_id (sort {$dev_pub_key_ids{$a} <=> $dev_pub_key_ids{$b}} keys %dev_pub_key_ids)
{
  $dev_pub_key_id_name           = join (',', map ord, split (//, $dev_pub_key_id));
  $dev_pub_key_id_name_len       = length $dev_pub_key_id;
  $dev_pub_key_id_name_len_bytes = convert_len_to_bytes ($dev_pub_key_id_name_len);
  $dev_pub_key_id_data			 = $dev_pub_key_id_data{$dev_pub_key_id};
  $dev_pub_key_id_len_bytes      = convert_len_to_bytes (length $dev_pub_key_id_data);
  $dev_pub_key_id_data           = join (',', map ord, split (//, $dev_pub_key_id_data));
  print INPFILE <<end_dev_pub_key_id_data;
\!Provisioning data for keyID $dev_pub_key_id
\{75,72,01,04,1,0,0,0,1,0,0,0,2,0,0,0,$dev_pub_key_id_name_len_bytes,$dev_pub_key_id_len_bytes,$dev_pub_key_id_name,$dev_pub_key_id_data\}

end_dev_pub_key_id_data
}

my $oem_pub_key_len_bytes;
my $oem_pub_key_name;
my $oem_pub_key_name_len;
my $oem_pub_key_name_len_bytes;
my $oem_pub_key_data;
foreach $oem_pub_key (sort {$oem_pub_keys{$a} <=> $oem_pub_keys{$b}} keys %oem_pub_keys)
{
  $oem_pub_key_name           = join (',', map ord, split (//, $oem_pub_key));
  $oem_pub_key_name_len       = length $oem_pub_key;
  $oem_pub_key_name_len_bytes = convert_len_to_bytes ($oem_pub_key_name_len);
  $oem_pub_key_data           = $oem_pub_key_data{$oem_pub_key};
  $oem_pub_key_len_bytes      = convert_len_to_bytes (length $oem_pub_key_data);
  $oem_pub_key_data           = join (',', map ord, split (//, $oem_pub_key_data));
  print INPFILE <<end_oem_pub_key_data;
\!Provisioning data for key $oem_pub_key
\{75,72,01,04,1,0,0,0,2,0,0,0,0,0,0,0,$oem_pub_key_name_len_bytes,$oem_pub_key_len_bytes,$oem_pub_key_name,$oem_pub_key_data\}

end_oem_pub_key_data
}

my $oem_pub_key_id_len_bytes;
my $oem_pub_key_id_name;
my $oem_pub_key_id_name_len;
my $oem_pub_key_id_name_len_bytes;
my $oem_pub_key_id_data;
foreach $oem_pub_key_id (sort {$oem_pub_key_ids{$a} <=> $oem_pub_key_ids{$b}} keys %oem_pub_key_ids)
{
  $oem_pub_key_id_name           = join (',', map ord, split (//, $oem_pub_key_id));
  $oem_pub_key_id_name_len       = length $oem_pub_key_id;
  $oem_pub_key_id_name_len_bytes = convert_len_to_bytes ($oem_pub_key_id_name_len);
  $oem_pub_key_id_data			 = $oem_pub_key_id_data{$oem_pub_key_id};
  $oem_pub_key_id_len_bytes      = convert_len_to_bytes (length $oem_pub_key_id_data);
  $oem_pub_key_id_data           = join (',', map ord, split (//, $oem_pub_key_id_data));
  print INPFILE <<end_oem_pub_key_id_data;
\!Provisioning data for keyID $oem_pub_key_id
\{75,72,01,04,1,0,0,0,3,0,0,0,2,0,0,0,$oem_pub_key_id_name_len_bytes,$oem_pub_key_id_len_bytes,$oem_pub_key_id_name,$oem_pub_key_id_data\}

end_oem_pub_key_id_data
}

close INPFILE;

exit 0;

sub convert_len_to_bytes ()
{
  my $len = $_[0];
  my $i;
  my $len_bytes = '';

  $len_bytes = $len & 0xFF;
  for ($i = 1; $i < 4; $i++)
  {
    $len >>= 8;
    $len_bytes .= (',' . ($len & 0xFF));
  }

  return $len_bytes;
}

