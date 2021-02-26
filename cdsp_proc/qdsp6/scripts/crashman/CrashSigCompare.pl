use File::Compare;
use File::Copy;
$sigfilepath=$ARGV[0];
$uniquenumber="$ARGV[1]";
$targetName="$ARGV[2]";
$sigtodatabase="$ARGV[3]";
$sigfilename="$targetName\_$uniquenumber\.txt";
#print"\n$sigfilename\n";
$issuetype=0;
$issuetype1=0;
$issuetype11=0;
$issuetype111=0;
$councmp=0;
#die"out";
$target_directory_qipl = "\\\\rover\\hyd\_dspfw\\ADSP\_Tools\\DO\_NOT\_DELETE\\CrashDataBase\\$targetName";
$target_directory_nonqipl = "\\\\balor\\vocoder\_appdsp4\\users\\ats\\DO\_NOT\_DELETE\\CrashDataBase\\$targetName";

system("ping rover > $sigfilepath\\QiplServer.txt");
system("ping fosters > $sigfilepath\\NonQiplServer.txt");
if (open(SERVLIST,"$sigfilepath\\QiplServer.txt")){
$serveline=<SERVLIST>;
while ( $serveline ne "" ){
if($serveline =~ /.+time\<(\d+)ms.+/)
{
$timetopingq=$1;
#my $target_directory = $target_directory_nonqipl;
}
elsif ($serveline =~ /.+time\=(\d+)ms.+/)
{
$timetopingq=$1;
}
$serveline=<SERVLIST>;
}
}
close SERVLIST;
if (open(SERVLIST,"$sigfilepath\\NonQiplServer.txt")){
$serveline=<SERVLIST>;
while ( $serveline ne "" ){
if($serveline =~ /.+time\<(\d+)ms.+/)
{
$timetopingnq=$1;
#my $target_directory = $target_directory_nonqipl;
}
elsif ($serveline =~ /.+time\=(\d+)ms.+/)
{
$timetopingnq=$1;
}
$serveline=<SERVLIST>;
}
}
close SERVLIST;
if ($timetopingnq >= $timetopingq)
{
#print"\n$timetopingnq\n";
#print"\n$timetopingq\n";
$target_directory = $target_directory_qipl;
}
else
{
$target_directory = $target_directory_nonqipl;
}

#print "\n$target_directory\n";
if ( -d "~/target_directory_qipl" ) {
    #print "found \n";
} else {
    #print "not found \n";
	mkdir $target_directory_qipl, 0777;
}

if ( -d "~/target_directory_nonqipl" ) {
    #print "found \n";
} else {
    #print "not found \n";
	mkdir $target_directory_nonqipl, 0777;
}

$cmpfilename="$sigfilepath\\$sigfilename";
open(SIGFILE1, ">$sigfilepath\\SimilarIssue\.txt") or die "not able to open file \"SimilarIssue\.txt\" in write mode";#Writing to a file "system.xls" which is output.
open(SIGFILE2, ">$sigfilepath\\SimilarCrashes\.txt") or die "not able to open file \"SimilarCrashes\.txt\" in write mode";#Writing to a file "system.xls" which is output.
$simissue=0;
opendir(DIR,$target_directory);
my @files = readdir(DIR);
closedir(DIR);
$length=@files;
for($i=2;$i<$length;$i++)
{

if($sigfilename !~ /$files[$i]/i)
{
#print"\n$files[$i]\n";
#print"\n$cmpfilename\n";
  if (compare("$cmpfilename","$target_directory\\$files[$i]") == 0) {
        $simissue = 1;
        #print"\n$cmpfilename\n";
		#print"\n$files[$i]\n";
	    #print "They're equal\n";
		if($cmpfilename =~ /.+\\(\w+)\..+/)
		{
		$name1=$1;
		}
		if($files[$i] =~ /(.+)\..+/)
		{
			$name2=$1;
			#print "\nFiles are same is same folder: $files[$i]\n";
			if(($name2 =~ /\w+\_\CR\d+/) || ($name2 =~ /\w+\_\cr\d+/))
			{
				$issuetype1=1;
				#print"\n$name22\n";
				print SIGFILE2 ("This issue $name1 looks similar to $name2\n");
			}	
			else
			{
			    $issuetype111=2;
				push(@presetissue, $name1);
				push(@pastissue, $name2);
			}
		}	
	}
}
}
if(($issuetype111 == 2) && ($issuetype1 == 0))
{
$issuelen=@presetissue;
	for($ii=0;$ii<$issuelen;$ii++)
	{
		print SIGFILE2 ("This issue $presetissue[$ii] looks similar to $pastissue[$ii]\n");
	}
}
if($simissue != 1)
{
#print"pramod\n";
my $directory = $target_directory;
opendir(DIR,$directory);
my @dir_files = readdir(DIR);
closedir(DIR);
$dir_length=@dir_files;
for($i=2;$i<$dir_length;$i++)
{
$subdirectory="$directory\\$dir_files[$i]";
if ( -d $subdirectory )
  {
    #print"pramodp1\n";
    opendir(DIR,$subdirectory);
    @c2=readdir(DIR);
	$subdir_length=@c2;
	for($k=2;$k<$subdir_length;$k++)
	{
	 #print "$c2[$k]\n";
	 if($sigfilename !~ /$c2[$k]/i)
	{
	if (compare("$cmpfilename","$subdirectory\\$c2[$k]") == 0) {
        $simissue = 1;
        #print"\n$cmpfilename\n";
		#print"\n$files[$i]\n";
	    #print "They're equal\n";
		if($cmpfilename =~ /.+\\(\w+)\..+/)
		{
			$name11=$1;
		}
		if($c2[$k] =~ /(.+)\..+/)
		{
			$name22=$1;
			if(($name22 =~ /\w+\_\CR\d+/) || ($name22 =~ /\w+\_\cr\d+/))
			{
				$issuetype=1;
				#print"\n$name22\n";
				print SIGFILE2 ("This issue $name11 looks similar to $name22\n");
			}
			else
			{
			    $issuetype11=2;
				#print"\n$issuetype\n";
				#print"\n$name11\n";
				#print"\n$name22\n";				
				push(@presetissue1, $name11);
				push(@pastissue1, $name22);
			}
			
		}		
        		
	}
	}
	}
  }
else
  {
#print "$dir_files[$i]\n";
  }
}
if(($issuetype11 == 2) && ($issuetype == 0))
{
#print"\n$issuetype\n";
$issuelen1=@presetissue1;
	for($ii=0;$ii<$issuelen1;$ii++)
	{
		print SIGFILE2 ("This issue $presetissue1[$ii] looks similar to $pastissue1[$ii]\n");
	}
}
}
#print"\n$length\n";
#print"\n$simissue\n";
if($simissue == 1)
{
print SIGFILE1 ("$simissue");

}
else
{
if (open(CMPLIST,"$cmpfilename")){
$inputline123=<CMPLIST>;
while ( $inputline123 ne "" ){
$listcmp++;
$inputline123=<CMPLIST>;
}
}
opendir(DIR,$target_directory);
my @files = readdir(DIR);
closedir(DIR);
$length=@files;
#print"$length";
for($i=2;$i<$length;$i++)
{
if($sigfilename !~ /$files[$i]/i)
{
if (open(CMPLIST,"$cmpfilename")){
$inputline=<CMPLIST>;
$inputline1=<CMPLIST>;
#print"\n$inputline\n";
#print"\n$inputline1\n";
$inputline4=<CMPLIST>;
#while ( $inputline4 ne "" ) {
#print"\n99:$inputline4\n";

if (open(CMP1LIST,"$target_directory\\$files[$i]")){
$inputline2=<CMP1LIST>;
$inputline3=<CMP1LIST>;
#print"\n2:$inputline2\n";
#print"\n3:$inputline3\n";
$inputline5=<CMP1LIST>;
while (( $inputline5 ne "" ) || ( $inputline4 ne "" )){
if(($inputline2 eq $inputline) && ($inputline3 eq $inputline1))
{
if($inputline5 eq $inputline4)
{
print"\n$inputline5 : $inputline4\n";
#print"\n7:$inputline5\n";
#print"\n7:$inputline4\n";
$councmp++;
}
}
else
{
goto nextfile;
}
$inputline5=<CMP1LIST>;
$inputline4=<CMPLIST>;
}
}
close CMP1LIST;
close CMPLIST;
#}
}
}
$eqlines=($listcmp/2);
$eqlines=int($eqlines);
#print"\n$listcmp\n";
#print"\n$eqlines\n";
$eqlines=$eqlines+1;
#print"\n$eqlines\n";
#print"\n$councmp\n";
if($councmp >= $eqlines)
{
		if($cmpfilename =~ /.+\\(\w+)\..+/)
		{
		$name1=$1;
		}
		if($files[$i] =~ /(.+)\..+/)
		{
		$name2=$1;
		}	
		$simissue=1;
		#print "\nthis is similar issue: $simissue\n";
		print SIGFILE1 ("$simissue\n");
		
        print SIGFILE2 ("This issue $name1 may be similar to $name2\n");	
}
nextfile:
close CMPLIST;
close CMP1LIST;
$councmp=0;
$listcmp=0;
$eqlines=0;
}
}
if($sigtodatabase == 2)
{
copy("$cmpfilename","$target_directory_qipl");
copy("$cmpfilename","$target_directory_nonqipl");
}
if ($simissue == 0)
{
print SIGFILE1 ("$simissue");
}