#!/usr/bin/perl -w
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
#USAGE     : TestEpiphany.pl -t    <TestPath>
#                            -r    <TotalRows>
#                            -c    <TotalCols>
#                            [-r0  <RowOrigin>]
#                            [-c0  <ColOrigin>]
#                            [-p   <RunParallel>]
#
##########################################################################
EOF

#Defining options to parse
    my $result =  GetOptions('t:s','r:s','c:s','r0:s','c0:s','p:s','h:s');

if(defined $opt_h){
    print "$Usage";
    print "
#########################################################################";
    exit;
}
elsif(!defined $opt_t || !defined $opt_r || !defined $opt_c){
	print "$Usage";
	exit;
}
$Row0=0;
$Col0=0;
$Rows=$opt_r;
$Cols=$opt_c;
$Fail=0;
if(defined $opt_r0){
    $Row0=$opt_r0;
}
else{
    $Row0=0;
}
if(defined $opt_c0){
    $Col0=$opt_c0;
    $Col0=0;
}
$ELF=$opt_t;

##############################################################
#Running All Tests
###############################################################

for $i ($Row0..$Rows-1){
    for $j ($Col0..$Cols-1){
	$ENV{EROW0}=$i;
	$ENV{ECOL0}=$j;
	$ENV{EROWS}=1;
	$ENV{ECOLS}=1;
	if(!$SkipHash{$i}{$j}){
	    $Status=system("$TestHash{$Test}{\"name\"} >> test.$Test.log");
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
  
