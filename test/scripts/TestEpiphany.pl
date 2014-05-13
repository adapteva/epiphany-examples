#!/usr/bin/perl
#use strict
use Getopt::Long;
use File::Spec;
$Version = "0.0";
my $Usage =<<EOF;
##########################################################################
#Function  : Runs a list of tests on a chip
#Date      : Jun 25, 2013
#License   : Whatever..
##########################################################################
#USAGE     : TestEpiphany.pl -l      <TestList>
#                            -rows   <TotalRows>
#                            -cols   <TotalCols>
#                            [-d     <RunDirectory]
#                            [-row0  <RowOrigin>]
#                            [-col0  <ColOrigin>]
#                            [-skip  <SkipFile>]
#
##########################################################################
EOF

#Defining options to parse
    my $result =  GetOptions('l:s','d:s','rows:s','cols:s','row0:s','col0:s','skip:s','h:s');

if(defined $opt_h){
    print "$Usage";
    print "
#########################################################################";
    exit;
}
elsif(!defined $opt_l || !defined $opt_d || !defined $opt_rows || !defined $opt_cols){
  print "$Usage";		
  exit;
}	
$Row0=0;
$Col0=0;
$Rows=$opt_rows;
$Cols=$opt_cols;
$Fail=0;
if(defined $opt_row0){
    $Row0=$opt_row0;
}
if(defined $opt_col0){
    $Col0=$opt_col0;
}


###############################################################
#Reading in Test List
###############################################################
open(FILE,$opt_l);
while(<FILE>){
    #Use ; as comment
    s/(.*)\#.*$/$1/;
    chomp($_);
    if(/\w/){
	@List      =split(' ',$_);
	$ELF=(File::Spec->rel2abs($List[0]));
	$TestHash{$Test}{"name"} = $ELF;
	$TestHash{$Test}{"type"} = $List[1];#0,1,all
	$Test=$Test+1;
    }
}
###############################################################
#Reading in Skip List
###############################################################
open(FILE,$opt_skip);
while(<FILE>){
    s/(.*)\#.*$/$1/;
    chomp($_);
    if(/\w/){
	@List      =split(' ',$_);
	$SkipHash{$List[0]}{$List[1]} = 1;
    }
}
##############################################################
#Running All Tests
###############################################################
if(-e $opt_d){
    system("rm -r $opt_d");
}
else{
    system("mkdir -p $opt_d");    
}
chdir("$opt_d");
foreach  $Test (sort(keys %TestHash)){    
    print "Running $TestHash{$Test}{\"name\"} ";
    if($TestHash{$Test}{"type"} eq "1"){
	$Status=system("$TestHash{$Test}{\"name\"} $i $j 1 1 >& test.$Test.log");
	if($Status ne "0"){
	    print "FAILED\n";
	    $Fail=1;
	}
	else{
	    print "PASSED\n";
	}
    }
    elsif($TestHash{$Test}{"type"} eq "all"){	
	$CoreFail=0;
	for $i ($Row0..$Rows-1){
	    for $j ($Col0..$Cols-1){
		if(!$SkipHash{$i}{$j}){
		    $Status=system("$TestHash{$Test}{\"name\"} $i $j 1 1 >> test.$Test.log");	
		    if($Status ne "0"){
			$Fail=1;
			$CoreFail=1;
		    }
		}
		else{
		    system("echo Skipping $TestHash{$Test}{\"name\"} on core $i $j >> test.$Test.log");
		}
	    }
	}
	if($CoreFail){
	    print "FAILED\n";
	}
	else{
	    print "PASSED\n";
	}
    }
}    
chdir("../");
##############################################################
#Checking Results
##############################################################
print"------------------------------------------------------------------------------\n";
if($Fail){
    print "CHIP $opt_d FAILED!!!\n";
}
else{
    print "CHIP $opt_d PASSED\n";
}
