#!/usr/bin/perl

# This perl file parses the uimage diag data chunks in the uImage diagbuffer,
# applies hdlc encoding to the same and generates a QMDL file.
# The input to this perl file are as below:
# 1. uImage DiagBuffer Dump
# 2. QMDL File output Path
# 3. uImage DiagBuffer Head
# 4. uImage DiagBuffer Tail
# Output of the perl script is the QMDL file which has hdlc encoded 
# Compressed data.

my $uimage_diagbuffer_data_in;
my $uimage_diagbuffer_data_out;
my $total_size ;


#First argument represents the uImage Diagbuffer dump extracted in the CMM file
if (defined($ARGV[0])) 
{
    $uimage_diagbuffer_data_in = $ARGV[0];
} else 
{ 
    die "uImage diagbuffer data input file not specified.\n";
}

#The output QMDL file path
if (defined($ARGV[1])) 
{
    $uimage_diagbuffer_data_out = $ARGV[1];
} else 
{ 
    die "uImage diagbuffer QMDL output file not specified.\n";
}

#uImage DiagBuffer Head
if (defined($ARGV[2])) 
{
    $total_size =  $ARGV[2];
} else 
{ 
    die "uImage Diagbuffer data size not specified.\n";
}



open (INFILE,$uimage_diagbuffer_data_in) or die "Not able to open the $uimage_diagbuffer_data_in file. \n";
binmode(INFILE);

open (OUTFILE, ">", $uimage_diagbuffer_data_out) or die "Not able to open the file for writing. \n";
binmode (OUTFILE);

close (INFILE) or die "Not able to close the file: $infile \n";
close (OUTFILE) or die "Not able to close the file: $outfile \n";

open (INFILE,$uimage_diagbuffer_data_in) or die "Not able to open the $uimage_diagbuffer_data_in file. \n";
binmode(INFILE);

open (OUTFILE, ">", $uimage_diagbuffer_data_out) or die "Not able to open the file for writing. \n";
binmode (OUTFILE);


#use enum qw(DIAG_SEND_STATE_START DIAG_SEND_STATE_BUSY DIAG_SEND_STATE_CRC1 DIAG_SEND_STATE_CRC2 DIAG_SEND_STATE_TERM DIAG_SEND_STATE_COMPLETE);

@crc_16_l_table= (
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
);


use constant MICRO_ULOG_SUBTYPE_DIAG_MSG => 0x10;
use constant MICRO_ULOG_SUBTYPE_DIAG_LOG =>0x11;
use constant MICRO_ULOG_SUBTYPE_DIAG_EVENT => 0x12;
use constant MICRO_ULOG_SUBTYPE_DIAG_EVENT_PAYLOAD => 0x13;
use constant DIAGCOMPRESSED_PKT_CODE => 0x9B;
use constant DIAG_ASYNC_CONTROL_CHAR => 0x7E;
use constant DIAG_ASYNC_ESC_CHAR => 0x7D;
use constant DIAG_ASYNC_ESC_MASK => 0x20;
use constant DIAG_MSG_EXT_CMD_CODE => 0x98;
use constant DIAG_MSG_CMD_CODE => 0x79;
use constant DIAG_PROC_ID => 0x0000;
use constant DIAG_LOG_CMD_CODE => 0x10;
use constant DIAG_EVENT_CMD_CODE => 0x60;

my $crc = 0;
my $buffer = "";
my $data_type = "";

sub CRC_16_L_STEP {
   $my_crc = ((hex($_[0])) >> 8) ^ $crc_16_l_table[(hex($_[0]) ^ hex($_[1])) & 0x00ff];
   return $my_crc;  
}



while($total_size > 0)
{
   #Read the data type of the next chunk

   #Read the lower byte of the data type
   if(read(INFILE, $data_type,1)!=0)
   {
     $data_type1 = sprintf("%02x", ord($data_type));
   }
   else
   {
      die ("Unable to read the lower byte of the datatype\n");
   }
   
   #Read the upper byte of the data type   
   if(read(INFILE, $data_type,1)!=0)
   {
      $data_type2 = sprintf("%02x", ord($data_type));     
   }
   else
   {
      die ("Unable to read the Upper byte of the data type\n");
   }
   
   #Calculate the data type
   $data_type=join("",$data_type2, $data_type1);
   $data_type=sprintf("%d", hex($data_type));
      
   if (($data_type != MICRO_ULOG_SUBTYPE_DIAG_MSG) && ($data_type != MICRO_ULOG_SUBTYPE_DIAG_LOG) &&
         ($data_type != MICRO_ULOG_SUBTYPE_DIAG_EVENT) && ($data_type != MICRO_ULOG_SUBTYPE_DIAG_EVENT_PAYLOAD))
   {
       # die ("Invalid uImage diagbuffer data type for the packet: $data_type \n");
   }
  
   #Read the lower byte of the length
   if(read(INFILE, $length,1)!=0)
   {
     $length1 = sprintf("%02x", ord($length));
   }
   else
   {
      die ("Unable to read the lower byte of the Length\n");
   }
   
   #Read the upper byte of the length   
   if(read(INFILE, $length,1)!=0)
   {
      $length2 = sprintf("%02x", ord($length));     
   }
   else
   {
      die ("Unable to read the Upper byte of the Length\n");
   }
   
   #Calculate the length of the uimage diagbuffer data chunk
   $length=join("",$length2, $length1);
   $length=sprintf("%d", hex($length));


   #indicate the status of HDLC encoding
   $state=0;
   $crc = sprintf("%x",0xFFFF);
   $state++; 

   #Read the TIMESTAMP   
   if(read(INFILE, $timestamp1,4)==0)
   {
      die ("Unable to read the timestamp 1\n");
   } 
   if(read(INFILE, $timestamp2,4)==0)
   {
      die ("Unable to read the timestamp 2\n");
   } 


   #Update the file pointer to the start of the compressed data
   #seek (INFILE, "-".3, 1);   
   
   if($data_type == MICRO_ULOG_SUBTYPE_DIAG_MSG) 
   {   

     #Read the number of arguments   
     if(read(INFILE, $num_args,1)==0)
     {
       die ("Unable to read no of arguments\n");
     } 
     $num_args = sprintf("%02x", ord($num_args));
     #Read the version number   
     if(read(INFILE, $version,1)==0)
     {
       die ("Unable to read version\n");
     } 
     $version = sprintf("%02x", ord($version));
     $version=sprintf("%d", hex($version));
     #move the pointer 2 byte to skip pad
     seek(INFILE, 2,1);

     if($version == 1)
     {
       #print ext msg cmd code    
       print OUTFILE (chr hex sprintf("%x", DIAG_MSG_EXT_CMD_CODE));
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "98"));	 
       #print version   
       print OUTFILE (chr hex $version);
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "01"));	
       #print proc_id   
       print OUTFILE (chr hex "0x00");
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));	
       print OUTFILE (chr hex "0x00");
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));	
	  
       #Read the parameter   
       if(read(INFILE, $parameter,4)!=0)
       {
         foreach (split(//,$parameter)) 
         {
           $item = sprintf("%02x", ord($_));
           $item_dec =  sprintf("%d", ord($_));
           if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
           {
             $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
             print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
             print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
           }
           else
           {
             $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
             print OUTFILE (chr hex $item);
           }
         }   
       }
       else
       {
         die ("Unable to read the parameter\n");
       } 
     }  

      #print ext msg cmd code    
      print OUTFILE (chr hex sprintf("%x", DIAG_MSG_CMD_CODE));
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, "79"));	
      #print ts_type
      print OUTFILE (chr hex "0x00");
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));	
      #print num of arguments
      print OUTFILE (chr hex $num_args);
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, $num_args));		  
      #print drop count
      print OUTFILE (chr hex "0x00");	
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));
      #print timestamp
      foreach (split(//,$timestamp1)) 
      {
       $item = sprintf("%02x", ord($_));
	   $item_dec =  sprintf("%d", ord($_));
       if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
       {
         $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
         print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
         $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex $item);
       }
      }
      foreach (split(//,$timestamp2)) 
      {
       $item = sprintf("%02x", ord($_));
       $item_dec =  sprintf("%d", ord($_));
       if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
       {
         $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
         print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
         $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex $item);
       }
      }
      #Read the arguments   
	  if($num_args>0)
	  {
		  if(read(INFILE, $arguments, 4*$num_args)==0)
		  {
			die ("Unable to read Arguments \n");
		  } 	  
	  }	
	 
      #Read the ssid info
      if(read(INFILE, $info, 8)==0)
      {
        die ("Unable to read the SSID info\n");
      }
      foreach (split(//,$info)) 
      {
        $item = sprintf("%02x", ord($_));
        $item_dec =  sprintf("%d", ord($_));
        if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
        {
          $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
          print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
          $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex $item);
       }
     }  	 
     #write the argument   
 
     foreach (split(//,$arguments)) 
     {
       $item = sprintf("%02x", ord($_));
       $item_dec =  sprintf("%d", ord($_));
       if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
       {
         $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
         print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
         $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex $item);
       }
     }
     #Read the string length   
     if(read(INFILE, $str_len,1)==0)
     {
       die ("Unable to read string length\n");
     } 
     $str_len = sprintf("%02x", ord($str_len));
     $str_len=sprintf("%d", hex($str_len));   
     #Read the string
     if(read(INFILE, $str_data, $str_len)==0)
     {
       die ("Unable to read the string\n");
     }
     foreach (split(//,$str_data)) 
     {
       $item = sprintf("%02x", ord($_));
       $item_dec =  sprintf("%d", ord($_));
       if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
           ($item_dec == DIAG_ASYNC_ESC_CHAR))
       {
         $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
         print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
         $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex $item);
       }
     }
     $total_size = $total_size - $length+4-$str_len-8;	 
   }
   elsif($data_type == MICRO_ULOG_SUBTYPE_DIAG_LOG) 
   {  
     $log_length = $length-16;
     #Read the log_code  
     if(read(INFILE, $log_code,2)==0)
     {
       die ("Unable to read the log code\n");
     }

     #Read the version number   
     if(read(INFILE, $version,1)==0)
     {
       die ("Unable to read the version number\n");
     } 
     $version = sprintf("%02x", ord($version));
     $version=sprintf("%d", hex($version));
     #move the pointer 1 byte to skip pad
     seek(INFILE, 1,1);

   
     if($version == 1)
     {
      #print ext msg cmd code  	  
      print OUTFILE (chr hex sprintf("%x", DIAG_MSG_EXT_CMD_CODE));
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, "98"));
      #print version   
      print OUTFILE (chr hex $version);
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, $version));
      #print proc_id   
      print OUTFILE (chr hex "0x00");
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));
      print OUTFILE (chr hex "0x00");
      $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));
      #Read the parameter   
      if(read(INFILE, $parameter,4)!=0)
      {
        foreach (split(//,$parameter)) 
        {
          $item = sprintf("%02x", ord($_));
		  $item_dec =  sprintf("%d", ord($_));
          if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
          {
            $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
            print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
            print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
          }
          else
          {
            $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
            print OUTFILE (chr hex $item);
          }
        }   
      }
      else
      {
        die ("Unable to read the parameter\n");
      } 
	  $log_length  = $log_length - 4;
     }  

     #print log cmd code   	  
     print OUTFILE (chr hex sprintf("%x", DIAG_LOG_CMD_CODE));
     $crc = sprintf("%x", CRC_16_L_STEP ($crc, "10"));
     #print more
     print OUTFILE (chr hex "0x00");
     $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));
     #print length
     $temp = sprintf("%02x",($log_length+4)&0x00FF);
     $item_dec =  sprintf("%d", hex($temp));
     if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
     {
       $crc = sprintf("%x",CRC_16_L_STEP ($crc, $temp));
       print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
       print OUTFILE ((chr hex $temp) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
     }
     else
     {
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, $temp));
       print OUTFILE (chr hex $temp);
     }	
     $temp = sprintf("%02x",($log_length+4)>>8);
     $item_dec =  sprintf("%d", hex($temp));
     if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
         ($item_dec == DIAG_ASYNC_ESC_CHAR))
     {
       $crc = sprintf("%x",CRC_16_L_STEP ($crc, $temp));
       print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
       print OUTFILE ((chr hex $temp) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
     }
     else
     {
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, $temp));
       print OUTFILE (chr hex $temp);
     }	
  
     #Read the parameter   
     if(read(INFILE, $length_id,4)!=0)
     {
       foreach (split(//,$length_id)) 
       {
         $item = sprintf("%02x", ord($_));
         $item_dec =  sprintf("%d", ord($_));
         if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
             ($item_dec == DIAG_ASYNC_ESC_CHAR))
         {
           $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
           print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
           print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
         }
         else
         {
           $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
           print OUTFILE (chr hex $item);
         }
       }   
      }
      else
      {
        die ("Unable to read the hash value\n");
      } 

      #print timestamp
      foreach (split(//,$timestamp1)) 
      {
        $item = sprintf("%02x", ord($_));
        $item_dec =  sprintf("%d", ord($_));
        if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
           ($item_dec == DIAG_ASYNC_ESC_CHAR))
        {
          $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
          print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
        }
        else
        {
          $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex $item);
        }
      }
      #skip first 4 bytes of the timestamp field in log packet
      seek(INFILE,4,1);
	
      foreach (split(//,$timestamp2)) 
      {
        $item = sprintf("%02x", ord($_));
        $item_dec =  sprintf("%d", ord($_));
        if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
        {
          $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
          print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
        }
        else
        {
          $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex $item);
        }
      }
      #skip next 4 bytes of the timestamp field in log packet
      seek(INFILE,4,1);
  

      $read_length = $log_length-12;
	  

      if(read(INFILE, $buffer,$read_length)!=0)
      {
        foreach (split(//,$buffer)) 
        {
          $item = sprintf("%02x", ord($_));
          $item_dec =  sprintf("%d", ord($_));
          if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
          {
            $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
            print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
            print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
          }
          else
          {
            $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
            print OUTFILE (chr hex $item);
          }
        } 
      }

      $total_size = $total_size - $length;	  
   }
   else #event or event with payload
   {   
     $payload_length = $length - 16;
     #Read the event ID   
     if(read(INFILE, $event_id,2)==0)
     {
       die ("Unable to read the event ID\n");
     } 

     #Read the version number   
     if(read(INFILE, $version,1)==0)
     {
       die ("Unable to read the version number\n");
     } 
     $version = sprintf("%02x", ord($version));
     $version=sprintf("%d", hex($version));
     #move the pointer 2 byte to skip pad
     seek(INFILE, 1, 1);

     if($version == 1)
     {
       $payload_length -=4;
       #print ext msg cmd code    
       print OUTFILE (chr hex sprintf("%x", DIAG_MSG_EXT_CMD_CODE));
       $crc = sprintf("%x", CRC_16_L_STEP ($crc,"98"));
       #print version   
       print OUTFILE (chr hex $version);
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, $version));
       #print proc_id   
       print OUTFILE (chr hex "0x00");
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));
       print OUTFILE (chr hex "0x00");
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));

       #Read the parameter   
       if(read(INFILE, $parameter,4)!=0)
       {
         foreach (split(//,$parameter)) 
         {
           $item = sprintf("%02x", ord($_));
           $item_dec =  sprintf("%d", ord($_));
           if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
           {
             $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
             print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
             print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
           }
           else
           {
             $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
             print OUTFILE (chr hex $item);
           }
         }   
       }
       else
       {
         die ("Unable to read the parameter\n");
       } 
     }  

     #print event cmd code    
     print OUTFILE (chr hex sprintf("%x", DIAG_EVENT_CMD_CODE));
     $crc = sprintf("%x", CRC_16_L_STEP ($crc, "60"));
     #print length and event ID
     if($data_type == MICRO_ULOG_SUBTYPE_DIAG_EVENT)
     {
       print OUTFILE (chr hex "0x0D");
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "0D"));
       print OUTFILE (chr hex "0x00");
       $crc = sprintf("%x", CRC_16_L_STEP ($crc, "00"));
       foreach (split(//,$event_id)) 
       {
         $item = sprintf("%02x", ord($_));
         $item_dec =  sprintf("%d", ord($_));
         if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
            ($item_dec == DIAG_ASYNC_ESC_CHAR))
         {
           $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
           print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
           print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
         }
         else
         {
           $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
           print OUTFILE (chr hex $item);
         }
       } 
     }
     else
     {
       $temp = sprintf("%02x",($payload_length + 14)&0x00FF);
       $item_dec =  sprintf("%d", hex($temp));
       if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
          ($item_dec == DIAG_ASYNC_ESC_CHAR))
       {
         $crc = sprintf("%x",CRC_16_L_STEP ($crc, $temp));
         print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
         print OUTFILE ((chr hex $temp) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
         $crc = sprintf("%x", CRC_16_L_STEP ($crc, $temp));
         print OUTFILE (chr hex $temp);
       }	
       $temp = sprintf("%02x",($payload_length + 14)>>8);
       $item_dec =  sprintf("%d", hex($temp));
       if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
           ($item_dec == DIAG_ASYNC_ESC_CHAR))
       {
         $crc = sprintf("%x",CRC_16_L_STEP ($crc, $temp));
         print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
         print OUTFILE ((chr hex $temp) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
         $crc = sprintf("%x", CRC_16_L_STEP ($crc, $temp));
         print OUTFILE (chr hex $temp);
       }	

       $j = 0;
       foreach (split(//,$event_id)) 
       { 
         $j++;
         $item = sprintf("%02x", ord($_));
         $item_dec =  sprintf("%d", ord($_));
         if($j == 2)
         {
           $item_dec = $item_dec | (3<<5);
           $item = sprintf("%02x", $item_dec);
         }

         if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
             ($item_dec == DIAG_ASYNC_ESC_CHAR))
         {
           $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
           print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
           print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
         }
         else
         {
           $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
           print OUTFILE (chr hex $item);
         }
        }
     }

     #print timestamp
     foreach (split(//,$timestamp1)) 
     {
       $item = sprintf("%02x", ord($_));
       $item_dec =  sprintf("%d", ord($_));
       if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
       {
         $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
         print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
       }
       else
       {
         $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
         print OUTFILE (chr hex $item);
       }
      }
      foreach (split(//,$timestamp2)) 
      {
        $item = sprintf("%02x", ord($_));
        $item_dec =  sprintf("%d", ord($_));
        if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
        {
          $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
          print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
        }
        else
        {
          $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
          print OUTFILE (chr hex $item);
        }
      }
      if($data_type == MICRO_ULOG_SUBTYPE_DIAG_EVENT_PAYLOAD) 
      {
        #print payload length
        $temp = sprintf("%02x",$payload_length);
        print OUTFILE (chr hex $temp);	
        $crc = sprintf("%x", CRC_16_L_STEP ($crc, $temp));
  

        if(read(INFILE, $buffer,$payload_length)!=0)
        {
          foreach (split(//,$buffer)) 
          {
            $item = sprintf("%02x", ord($_));
            $item_dec =  sprintf("%d", ord($_));
            if (($item_dec == DIAG_ASYNC_CONTROL_CHAR) ||
               ($item_dec == DIAG_ASYNC_ESC_CHAR))
            {
              $crc = sprintf("%x",CRC_16_L_STEP ($crc, $item));
              print OUTFILE (chr hex sprintf("%x", DIAG_ASYNC_ESC_CHAR));
              print OUTFILE ((chr hex $item) ^ (chr hex sprintf("%x", DIAG_ASYNC_ESC_MASK)));
            }
            else
            {
              $crc = sprintf("%x", CRC_16_L_STEP ($crc, $item));
              print OUTFILE (chr hex $item);
            }
          } 
        }
      } 	   
	  

      $total_size = $total_size - $length;
   }

   if ($state == 1)
   {
      $crc = ~(hex($crc));
      $state++;
   }
 

   while ($state >= 2 && $state < 4)
   {
  
     $temp = sprintf("%x", $crc);
     $src_byte = hex($temp) & hex("0xFF");
     
     if (($src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
        ($src_byte == DIAG_ASYNC_ESC_CHAR))
     {
    
       print OUTFILE (chr hex sprintf("%x",DIAG_ASYNC_ESC_CHAR));
       print OUTFILE (chr hex sprintf("%x", ($src_byte ^ DIAG_ASYNC_CHAR_MASK)));
       $crc >>= 8;
     }
     else
     {
       $crc >>= 8;
       $src_byte = sprintf("%02x",$src_byte);
       print OUTFILE chr hex $src_byte;
      
     }
       $state++;
   }
   
   if ($state == 4)
   {
      print OUTFILE (chr hex "0x7E");
   }    

} 

 
close (INFILE) or die "Not able to close the file: $infile \n";
close (OUTFILE) or die "Not able to close the file: $outfile \n";


