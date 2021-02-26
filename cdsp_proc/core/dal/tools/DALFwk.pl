#!/usr/bin/perl
#structure to store config
$g_dal_cfg_rec = {
        driver           => $string,
        interface        => $string,
        interface_file   => $string,
        remote_interface => $string,
        interface_output_file =>$string,
        num_devices      => $string,
        num_clients      => $string,
        include_path     => $string,
        code_output_path => $string,
        device_ids       => $string,
        tmpl_path        => $string
};

$g_dal_proc_cfg = {
        driver_names     => $string,
        code_output_path => $string,
        process          => $string,
        tmpl_path        => $string
};

$func_args = {  name     => $string,
                type     => $string,
                rem_info => $string
             };
#structure to store function information
$func_info = {
                 ret_type => $string,
                 name     => $string,
                 prototype_idx => $string,
                 args     => [@func_args]
             };
@g_user_func;


@g_remote_fnc_idx = (
                     "U_",
                     "U_U_",
                     "U_UP_",
                     "U_U_U_",
                     "U_U_UP_",
                     "IVP_U_",
                     "U_IVP_U_",
                     "IVP_U_OVP_U_UP_",
                     "IVP_U_OVP_U_",
                     "OVP_U_",
                     "U_IVP_U_OVP_U_UP_",
                     "U_OVP_U_",
                     "U_OVP_U_UP_",
                     "IVP_U_IVP_U_OVP_U_",
                     "IVP_U_OVP_U_OVP_U_UP_",
                     "IVP_U_IVP_U_OVP_U_UP_OVP_U_"
                     );


sub ReadCfgFile
{
        my($cfg_file_path) = @_;
        open(cfg_file_handle, $cfg_file_path) || die ("Could not open $cfg_file_path\n");

        @cfg_blob=<cfg_file_handle>;
        foreach $cfg_line (@cfg_blob)
        {
                chop($cfg_line);
                ($cfg_item,$cfg_val)=split("=",$cfg_line);
                #trim leading and trailing whitespace from the cfg_item and cfg_val
                $cfg_item =~ s/^\s+//;
                $cfg_item =~ s/\s+$//;
                $cfg_val  =~ s/^\s+//;
                $cfg_val  =~ s/\s+$//;
                if($cfg_item eq "driver")
                {
                        $g_dal_cfg_rec->{driver} = $cfg_val;
                }elsif($cfg_item eq "interface_file")
                {
                        $g_dal_cfg_rec->{interface_file} = $cfg_val;
                }elsif($cfg_item eq "remote_interface")
                {
                        $g_dal_cfg_rec->{remote_interface} = $cfg_val;
                }
                elsif($cfg_item eq "num_clients")
                {
                        $g_dal_cfg_rec->{num_clients} = $cfg_val;
                }elsif($cfg_item eq "num_devices")
                {
                        $g_dal_cfg_rec->{num_devices} = $cfg_val;
                }
                elsif($cfg_item eq "template_path")
                {
                        $g_dal_cfg_rec->{tmpl_path} = $cfg_val;
                }
                elsif($cfg_item eq "include_path")
                {
                        $g_dal_cfg_rec->{include_path} = $cfg_val;
                }
                elsif($cfg_item eq "code_output_path")
                {
                        $g_dal_cfg_rec->{code_output_path} = $cfg_val;
                }
                elsif($cfg_item eq "device_ids")
                {
                        $g_dal_cfg_rec->{device_ids} = $cfg_val;
                }
                elsif($cfg_item eq "interface_name")
                {
                        $g_dal_cfg_rec->{interface} = $cfg_val;
                }
        }
        if(lc($g_dal_cfg_rec->{remote_interface}) ne "yes"
           &&lc($g_dal_cfg_rec->{remote_interface}) ne "no")
        {
           $g_dal_cfg_rec->{remote_interface} = "yes";
        }
        print "---------- DAL configuration -----------\n";
        print " Driver             = $g_dal_cfg_rec->{driver}\n";
        print " Interface file     = $g_dal_cfg_rec->{interface_file}\n";
        print " Interface name     = $g_dal_cfg_rec->{interface}\n";
        print " Remote interface   = $g_dal_cfg_rec->{remote_interface}\n";
        print " Device Ids         = $g_dal_cfg_rec->{device_ids}\n";
        print " Include path       = $g_dal_cfg_rec->{include_path}\n";
        print " Code Ouput path    = $g_dal_cfg_rec->{code_output_path}\n";
        print " Template path      = $g_dal_cfg_rec->{tmpl_path}\n";
        print "------------------------------------------------\n";
        close(cfg_file_handle);
}

sub ParseUserFunction
{
   my($func_line) = @_;
   my($ret_type) = "";
   my($func_name) = "";
   my($params) = "";
   my($temp) = "";
   my $func_info={};

   # Return_type Func_Name ( prameters...);
   # Removing trailing leading whitespace
   # Remove any extra newline chars or tabs or spaces

   $func_line =~ s/^\s+//;
   $func_line =~ s/^\s$//;
   $func_line =~ s/(\s{2,}|\t|\r|\n)/ /g;

   split('\(|\)',$func_line);
   $params = @_[1];
   $temp   = @_[0];

   # Remove leading comment and whitespace
   $temp =~ s|/\*.*\*/\s*||;

   split(' ',$temp);

   $func_info->{ret_type} = @_[0];
   $func_info->{name} = @_[1];


   #proceed only if the return type is DALResult
   if($func_info->{ret_type} eq "DALResult")
   {
      split(',',$params);
      $num_params = @_;
      #iterate over all the params
      #parameters = /* remoting type*/ datatype name
      for($idx=0;$idx<$num_params;$idx++)
      {
         #check for any remote-type comments
         #remove any trailing and leading whitespace
         my($param)  = @_[$idx];
         $param =~ s/^\s+//;
         $param =~ s/\s+$//;
         my($param_name) = "";
         my($param_type) = "";

         $space_idx = rindex($param," ");
         $asterisk_idx = rindex($param,"*");

         #take the min of the index i.e the first
         $name_type_sep = $asterisk_idx+1;
         if($space_idx>$asterisk_idx)
         {
            $name_type_sep = $space_idx+1;
         }
         $param_name = substr($param,$name_type_sep);
         $param_type = substr($param,rindex($param,"/")+1);

         $space_idx = rindex($param_type," ");
         $asterisk_idx = rindex($param_type,"*");

         #take the min of the index i.e the first
         $name_type_sep = $asterisk_idx+1;
         if($space_idx>$asterisk_idx)
         {
            $name_type_sep = $space_idx+1;
         }

         $param_type = substr($param_type,0,$name_type_sep);

         my($param_com)="";
         if(index($param,"/*") != -1)
         {
            $param_com = substr($param,index($param,"/*")+2,rindex($param,"*/")-2);
         }
         push @{$func_info->{args}},{name=>$param_name,type=>$param_type,rem_info=>$param_com};
      }
      push @g_user_func,$func_info;
   }

}
sub ExtractUserDefinedFunctions
{
   my($interface_file_name) = @_;

   open(interface_file_handle,$interface_file_name) || die ("Could not open interface file : ($interface_file_name)\n");

   @interface_file_data = <interface_file_handle>;

   my($line) ="";
   my($orig_file)="";

   foreach(@interface_file_data)
   {
      $orig_file = $orig_file.$_;
      if($_ !~ m/#|;|{|}/ )
      {
         $line = $line.$_;
      }
      else
      {
         $line = $line.$_;
         if($line =~ m/DALResult(\t|\s|\n|\r)+\w*(\t|\s|\n|\r)*\((.|\t|\s|\n|\r)*\)/)
         {
            ParseUserFunction($line);
         }
         #Reset the state
         $line = "";
      }
   }
   close(interface_file_handle);
   #exclude the last #endif
   $orig_file =~ s/\#endif$//;
   return $orig_file;
}

#first param is function signature string
#second param is function name
sub ObtainRemotePrototypeIdx
{
   my ($user_func_sign) = @_[0];
   my ($user_func_def) = @_[1];
   my ($idx) = 0;
   foreach $remote_func_sign(@g_remote_fnc_idx)
   {
      if($user_func_sign eq $remote_func_sign)
      {
         return $idx;
      }
      $idx++;
   }
   print "FATAL ERROR! Function \"$user_func_def\" doesn't match one of the 16 allowed prototypes\n";
   print "Prototype is $user_func_sign \n";
   die();
}

#first param is interface name
sub FormInterfaceVtbl
{
   my ($interface_name) = @_[0];

   my ($interface_struct) = "typedef struct ".$interface_name."Handle ".$interface_name."Handle; \n";
   $interface_struct      = $interface_struct."struct ".$interface_name."Handle \n";
   $interface_struct      = $interface_struct."{\n   uint32 dwDalHandleId;\n   const ".$interface_name." * pVtbl;\n   void * pClientCtxt;\n};\n\n";
   my ($interface_vtbl_struct) = "typedef struct ".$interface_name." ".$interface_name.";\n";
   $interface_vtbl_struct = $interface_vtbl_struct."struct ".$interface_name."\n";
   $interface_vtbl_struct = $interface_vtbl_struct."{\n   struct DalDevice DalDevice;\n";

   for($func_idx=0;$func_idx<@g_user_func;$func_idx++)
   {
      my($func_param_sign) = "";
      my($func_def)="   ".@g_user_func[$func_idx]->{ret_type}." (*".@g_user_func[$func_idx]->{name}.")(DalDeviceHandle * _h";
      for($arg_idx=1;$arg_idx<@{@g_user_func[$func_idx]->{args}};$arg_idx++)
      {
         $func_def = $func_def.", ";
         #type
         if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/ostruct|obuf/)
         {
            $func_param_sign = $func_param_sign."OVP_";
         }
         elsif(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/istruct|ibuf/)
         {

            $func_param_sign = $func_param_sign."IVP_";
         }
         elsif(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/i32/)
         {

            $func_param_sign = $func_param_sign."U_";
         }
         elsif(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/o32/)
         {

            $func_param_sign = $func_param_sign."UP_";
         }
         else
         {
            if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{type} =~ m/uint32\s*\*/)
            {
               $func_param_sign = $func_param_sign."UP_";
            }
            else
            {
               $func_param_sign = $func_param_sign."U_";
            }
         }
         #TYPE
         $func_def = $func_def.@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{type}." ";
         #name
         $func_def = $func_def.@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{name};
         #add the size field
         if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/ostruct/)
         {
            $func_def = $func_def.", uint32 olen".$arg_idx;
            $func_param_sign = $func_param_sign."U_";
         }
         if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/istruct/)
         {
            $func_def = $func_def.", uint32 ilen".$arg_idx;
            $func_param_sign = $func_param_sign."U_";
         }
      }
      #store the remote func idx
      if($g_dal_cfg_rec->{remote_interface} eq "yes")
      {
         @g_user_func[$func_idx]->{prototype_idx} = ObtainRemotePrototypeIdx($func_param_sign,
                                                                             @g_user_func[$func_idx]->{name});
      }
      $interface_vtbl_struct = $interface_vtbl_struct.$func_def.");\n";
   }
   $interface_vtbl_struct = $interface_vtbl_struct."};\n";
   return $interface_vtbl_struct."\n".$interface_struct;
}


#first param is the interface name
sub FormDeviceAttachMacro{
   my($interface_name) = @_[0];
   my($interface_ver) = uc(@_[0])."_INTERFACE_VERSION";

   $interface_name =~ s/Dal//g;

   $interface_name =~ s/^\s+//;
   $interface_name =~ s/\s+$//;

   my($dev_attach_macro) =                "#define DAL_".$interface_name."DeviceAttach(DevId,hDalDevice)\\\n";
   $dev_attach_macro = $dev_attach_macro. "        DAL_DeviceAttachEx(NULL,DevId,".$interface_ver.",hDalDevice)\n";
   $dev_attach_macro = $dev_attach_macro."\n";

   return $dev_attach_macro

}

#first param is interface name
sub FormClientStubFunctions
{
   my ($interface_name) =  @_[0];
   my ($remote_check)            = "   if(DALISREMOTEHANDLE(_h))\n";
   $remote_check  = $remote_check. "   {\n";
   $remote_check  = $remote_check. "      DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);\n";

   my($stub_functions) = "";

   for($func_idx=0;$func_idx<@g_user_func;$func_idx++)
   {
      my($func_body)     = "static __inline ".@g_user_func[$func_idx]->{ret_type}."\n".$interface_name."_".@g_user_func[$func_idx]->{name}."(DalDeviceHandle * _h";
      my($remote_invocation) = "      return hRemote->pVtbl->FCN_".@g_user_func[$func_idx]->{prototype_idx};
      my($local_invocation ) = "   return ((".$interface_name."Handle *)_h)->pVtbl->".@g_user_func[$func_idx]->{name};
      my($invocation_args) = "";
      my($remote_invocation_args) = "";

      $remote_invocation   = $remote_invocation."(DALVTBLIDX(((".$interface_name."Handle *)_h)->pVtbl, ".@g_user_func[$func_idx]->{name}." ), _h";
      $local_invocation    = $local_invocation."( _h";

      for($arg_idx=1;$arg_idx<@{@g_user_func[$func_idx]->{args}};$arg_idx++)
      {
         $func_body = $func_body.", ";
         $invocation_args = $invocation_args.", ";
         $remote_invocation_args = $remote_invocation_args.", ";

         #type
         if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/ostruct|istruct|ibuf|obuf/)
         {
            $remote_invocation_args = $remote_invocation_args."(void * )";
         }
         if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/i32/)
         {
            $remote_invocation_args = $remote_invocation_args."(uint32 )";
         }
         if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/o32/)
         {
            $remote_invocation_args = $remote_invocation_args."(uint32 *)";
         }


         $invocation_args = $invocation_args.@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{name};
         $remote_invocation_args = $remote_invocation_args.@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{name};

         #for the func definition
         $func_body = $func_body.@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{type}." ";
         $func_body = $func_body.@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{name};

         #extra arg for structs
         if(@{@g_user_func[$func_idx]->{args}}[$arg_idx]->{rem_info} =~ m/ostruct|istruct/)
         {
            my($struct_type) = @{@g_user_func[$func_idx]->{args}}[$arg_idx]->{type};
            $struct_type =~ s/\*//g;
            $struct_type =~ s/const//g;
            $invocation_args = $invocation_args.",sizeof(".$struct_type.")";
            $remote_invocation_args = $remote_invocation_args.",sizeof(".$struct_type.")";
         }
      }
      $func_body = $func_body.")\n";
      $func_body = $func_body."{\n";
      #do remote only if configured for
      if($g_dal_cfg_rec->{remote_interface} eq "yes")
      {
         $func_body = $func_body.$remote_check.$remote_invocation.$remote_invocation_args.");\n   }\n";
      }
      $func_body = $func_body.$local_invocation.$invocation_args.");\n}\n";
      $stub_functions = $stub_functions.$func_body;
   }
   return $stub_functions;
}

#first param is interface name
#second param is inputfile
sub CreateInterfaceFile {
   my($interface_name) = @_[0];
   my($input_file) = @_[1];
   my($interface_file_output_name) = $g_dal_cfg_rec->{include_path}."\\";
   $g_dal_cfg_rec->{interface_output_file} = $interface_name;
   $g_dal_cfg_rec->{interface_output_file} =~ s/Dal//;
   $g_dal_cfg_rec->{interface_output_file} = "DDI".$g_dal_cfg_rec->{interface_output_file}.".h";
   $interface_file_output_name = $interface_file_output_name.$g_dal_cfg_rec->{interface_output_file};

   open(interface_file_output_handle,">",$interface_file_output_name) || die ("Could not open output interface file : ($interface_file_output_name)\n");

   #extract user defined functions
   my($user_input) = ExtractUserDefinedFunctions($input_file);

   #Form interface inheritance macro
   my($inherit_interface_macro) = FormInterfaceVtbl($interface_name);

   #Form client stub functions
   my($client_stub_funcs) = FormClientStubFunctions($interface_name);

   #Specialized device attach macro
   my($device_attach_macro) = FormDeviceAttachMacro($interface_name);

   print interface_file_output_handle $user_input;
   print interface_file_output_handle $inherit_interface_macro;
   print interface_file_output_handle $device_attach_macro;
   print interface_file_output_handle $client_stub_funcs;
   print interface_file_output_handle "#endif\n";

   close($interface_file_output_handle);
}


##################################################################################


sub FormDALFunctionDeclaration
{
   my($ret_str) = "";
   for($i=0;$i<@g_user_func;$i++)
   {
      $func_decl="";
      $func_decl = @g_user_func[$i]->{ret_type}." ".$g_dal_cfg_rec->{driver}.
                   "_".@g_user_func[$i]->{name};
      $func_decl = $func_decl."( ";

      for($j=0;$j<@{@g_user_func[$i]->{args}};$j++)
      {
         if ($j)
         {
            $func_decl = $func_decl.", ".@{@g_user_func[$i]->{args}}[$j]->{type};
         }
         else
         {
            $func_decl = $func_decl.$g_dal_cfg_rec->{driver}."ClientCtxt *";
         }
         #add the size field
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/ostruct/)
         {
            $func_decl = $func_decl.", uint32 olen".$j;
         }
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/istruct/)
         {
            $func_decl = $func_decl.", uint32 ilen".$j;
         }
      }
      $func_decl = $func_decl.");\n";
      $ret_str = $ret_str.$func_decl;
   }
   return $ret_str;
}

sub FormCSFunctionDefinition
{
   my($ret_str) = "";
   for($i=0;$i<@g_user_func;$i++)
   {
      $func_defn="static ";
      $func_defn = $func_defn.@g_user_func[$i]->{ret_type}." \n".$g_dal_cfg_rec->{driver}."_".
                   $g_dal_cfg_rec->{interface}."_".@g_user_func[$i]->{name};
      $func_defn = $func_defn."( ";

      for($j=0;$j<@{@g_user_func[$i]->{args}};$j++)
      {
         if ($j)
         {
            $func_defn = $func_defn.", ";
            $func_defn = $func_defn.@{@g_user_func[$i]->{args}}[$j]->{type}. " ";
            $func_defn = $func_defn.@{@g_user_func[$i]->{args}}[$j]->{name};
         }
         else
         {
            $func_defn = $func_defn."DalDeviceHandle * h";
         }
         #add the size field
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/ostruct/)
         {
            $func_defn = $func_defn.", uint32 olen".$j;
         }
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/istruct/)
         {
            $func_defn = $func_defn.", uint32 ilen".$j;
         }
      }
      $func_defn = $func_defn.") \n{\n";
      $func_defn = $func_defn."\treturn ".$g_dal_cfg_rec->{driver}."_".@g_user_func[$i]->{name};
      $func_defn = $func_defn."(((".$g_dal_cfg_rec->{interface}."Handle *)h)->pClientCtxt";

      $args = "";

      for($j=1;$j<@{@g_user_func[$i]->{args}};$j++)
      {
         $args = $args.", ".@{@g_user_func[$i]->{args}}[$j]->{name};
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/ostruct/)
         {
            $args = $args.", olen".$j;
         }
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/istruct/)
         {
            $args = $args.", ilen".$j;
         }
      }
      if($args ne "")
      {
         $func_defn = $func_defn.$args;
      }
      $func_defn = $func_defn.");\n";
      $func_defn = $func_defn."}\n";
      $ret_str = $ret_str.$func_defn;
   }
   return $ret_str;
}

sub FormDALFunctionDefinition
{
   my($ret_str) = "";

   for($i=0;$i<@g_user_func;$i++)
   {
      $func_defn="";
      $func_defn = $func_defn.@g_user_func[$i]->{ret_type}." \n".$g_dal_cfg_rec->{driver}."_"
                   .@g_user_func[$i]->{name};
      $func_defn = $func_defn."( ";

      for($j=0;$j<@{@g_user_func[$i]->{args}};$j++)
      {
         if ($j)
         {
            $func_defn = $func_defn.", ";
            $func_defn = $func_defn.@{@g_user_func[$i]->{args}}[$j]->{type}. " ";
            $func_defn = $func_defn.@{@g_user_func[$i]->{args}}[$j]->{name};
         }
         else
         {
            $func_defn = $func_defn.$g_dal_cfg_rec->{driver}.ClientCtxt." * pCtxt";
         }
         #add the size field
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/ostruct/)
         {
            $func_defn = $func_defn.", uint32 olen".$j;
         }
         if(@{@g_user_func[$i]->{args}}[$j]->{rem_info} =~ m/istruct/)
         {
            $func_defn = $func_defn.", uint32 ilen".$j;
         }

      }
      $func_defn = $func_defn.") \n{\n   return DAL_ERROR;\n";
      $func_defn = $func_defn."}\n";

      $ret_str = $ret_str.$func_defn;
   }
   return $ret_str;
}

sub FormVtblFunctionEntry {
   my($ret_str) = "";

   for($i=0;$i<@g_user_func;$i++)
   {
      $ret_str = $ret_str.",\n\t\t".$g_dal_cfg_rec->{driver}."_".$g_dal_cfg_rec->{interface}."_".@g_user_func[$i]->{name};
   }
   return $ret_str;
}

sub AutoGenerateFileBySearchAndReplace
{
   ($template_file,$output_file) = @_;
   open(template_file_handle,$template_file) || die ("Could not open $template_file\n");
   open(output_file_handle,">",$output_file) ||die ("Could not open $output_file\n");
   my($interface_version_macro) = uc($g_dal_cfg_rec->{interface})."_INTERFACE_VERSION";

   while(my $line = <template_file_handle>)
   {
      $line =~ s/<driver>/$g_dal_cfg_rec->{driver}/g;
      $line =~ s/<interface>/$g_dal_cfg_rec->{interface}/g;
      $line =~ s/<driver_num_devices>/$g_dal_cfg_rec->{num_devices}/g;
      $line =~ s/<platform>/$g_dal_cfg_rec->{platform}/g;
      $line =~ s/<device_id_list>/$g_dal_cfg_rec->{device_ids}/g;
      $line =~ s/<interface_header_file>/$g_dal_cfg_rec->{interface_output_file}/g;
      $line =~ s/<dal_interface_version>/$interface_version_macro/g;

      if($line =~ m/<dal_user_interface_function_declaration>/)
      {
         $line = FormDALFunctionDeclaration();
      }
      if($line =~ m/<cs_user_interface_function_definition>/)
      {
         $line = FormCSFunctionDefinition();
      }

      if($line =~ m/<dal_user_interface_function_definition>/)
      {
         $line = FormDALFunctionDefinition();
      }

      if($line =~ m/<user_interface_function_vtbl_entry>/)
      {

         my $add_vtbl_entries = FormVtblFunctionEntry();
         if( $add_vtbl_entries ne "")
         {
            $line =~ s/<user_interface_function_vtbl_entry>/$add_vtbl_entries/g
         }
      }
      if($line =~ m/<static_client_context_declaration>/)
      {
         if($g_dal_cfg_rec->{num_clients} == 1)
         {
            $line =~ s/<static_client_context_declaration>/static/g;
         }
         else
         {
            next;
         }
      }
      if($line =~ m/<client_context_declaration>/)
      {
         if($g_dal_cfg_rec->{num_clients} == 1)
         {
            $line =~ s/<client_context_declaration>/&clientCtxt/g;
         }
         else
         {
            $line =~ s/<client_context_declaration>/ NULL/g;
         }
      }
      if($line =~ m/<non_static_client_context_req>/)
      {
         if($g_dal_cfg_rec->{num_clients} == 1)
         {
            next;
         }
         else
         {
            $line =~ s/<non_static_client_context_req>//g;
         }
      }
      print output_file_handle $line;
   }
   close(output_file_handle);
   close(template_file_handle);
}


$g_gen_dal_src = 0;

sub ParseCmdArgs
{
   foreach (@_)
   {
      if( $_ =~ m/-s/)
      {
         $g_gen_dal_src = 1;
      }
   }
}



$numArgs = $#ARGV +1;


if (($numArgs < 1)
   )
{
   print "\n";
   print "Usage: perl DALFwk.pl <config_file_name> [-s]\n";
   print " OPTIONAL FLAGS \n";
   print " -s switch auto generates the source skeleton for your device driver\n";
   print " by default this is turned OFF no source file is generated\n";
   print "\n";
   die();
}

$cfg_file_name = $ARGV[0];

   ParseCmdArgs(@ARGV);

   ReadCfgFile($cfg_file_name);
   my $output_interface_file = $g_dal_cfg_rec->{interface}.".h";

   CreateInterfaceFile($g_dal_cfg_rec->{interface},$g_dal_cfg_rec->{interface_file});

   #Generate the framework source file for wrapping CS functionality
   $dal_tmpl_src_file = $g_dal_cfg_rec->{tmpl_path}."tmpDALDriverFwk.c";
   $dal_src_file = $g_dal_cfg_rec->{code_output_path}."Dal".$g_dal_cfg_rec->{driver}."Fwk.c";
   AutoGenerateFileBySearchAndReplace($dal_tmpl_src_file,$dal_src_file);
   print "Auto Generated DAL framework source file : $dal_src_file\n";

   #Generate the idevice int hdr file
   $idevice_tmpl_hdr_file = $g_dal_cfg_rec->{tmpl_path}."tmpDALDriver.h";
   $idevice_hdr_file= $g_dal_cfg_rec->{code_output_path}."Dal".$g_dal_cfg_rec->{driver}.".h";
   AutoGenerateFileBySearchAndReplace($idevice_tmpl_hdr_file,$idevice_hdr_file);
   print "Auto Generated $g_dal_cfg_rec->{interface} header file : $idevice_hdr_file\n";

   #Generate the idevice int hdr file
   $idevice_tmpl_info_file = $g_dal_cfg_rec->{tmpl_path}."tmpDALDriverInfo.c";
   $idevice_info_file= $g_dal_cfg_rec->{code_output_path}."Dal".$g_dal_cfg_rec->{driver}."Info.c";
   AutoGenerateFileBySearchAndReplace($idevice_tmpl_info_file,$idevice_info_file);
   print "Auto Generated $g_dal_cfg_rec->{interface} driver info file : $idevice_info_file\n";

   if($g_gen_dal_src)
   {
      #Generate the idevice int src file
      $idevice_tmpl_src_file = $g_dal_cfg_rec->{tmpl_path}."tmpDALDriver.c";
      $idevice_src_file= $g_dal_cfg_rec->{code_output_path}."Dal".$g_dal_cfg_rec->{driver}.".c";
      AutoGenerateFileBySearchAndReplace($idevice_tmpl_src_file,$idevice_src_file);
      print "Auto Generated $g_dal_cfg_rec->{interface} src file : $idevice_src_file\n";
   }






