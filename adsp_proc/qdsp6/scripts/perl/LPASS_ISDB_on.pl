$inFile = $ARGV[0];
$OutFile = $ARGV[1];

open OFILE, ">$OutFile" or die $!;

$IFILE=$inFile;
open IFILE or die $!; 

binmode IFILE; 
binmode OFILE; 

my ($buf, $data, $n); 

$m=0;

while (($n = read IFILE, $data, 1) != 0) 
{ 
   if($m == 4144 | $m == 4148 | $m == 4152){
      $data =pack('C','1');       
   }
   print OFILE $data;
   $m++;
}
print OFILE $buf;

close(IFILE); 
close(OFILe);

