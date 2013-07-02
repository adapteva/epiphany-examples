#!/usr/bin/perl
#use strict
###############################################################################
#The MIT License (MIT)
#
#Copyright (c) 2013 Adapteva, Inc
#
#Contributor Andreas Olofsson <andreas@adapteva.com>
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.
#
##########################################################################
use Getopt::Long;
use File::Spec;
my $Usage =<<EOF;
##########################################################################
#
# USAGE: run_suite.pl -l    <TestList>
#                     -d    <RunDir>
#                     [-nb  NoBuild]
#                     [-h   GetHelp]
#
##########################################################################
EOF

#Defining options to parse
my $result =  GetOptions('l:s',,'b:s','d:s','nb:s','h:s');

if(defined $opt_h){
print "$Usage";
print "#
# TestList : Test file, in the following format:
#            <elf file> <row#> <col#> <rows to run on> <cols to run on>
#            Example file for launching simple_matmul on 16 cores 
#            one at a time (content of file):
#             simple_matmul.elf 0 0 3 3            
#
# RunDir   : Name of directory to place log files in, usually chip#            
#
# NoBuild  : Skips building 
#
# Assumes  : Make file in run directory that takes as argument:TEST=$test
#
##########################################################################
\n";
  exit;
}
elsif(!defined $opt_l || !defined $opt_d){
  print "$Usage";		
  exit;
}	
#################
#Parse test file
$Test=0;
open(FILE,$opt_l);
while(<FILE>){
    #Use ; as comment
    s/(.*)\;.*$/$1/;
    chomp($_);
    if(/\w/){
	@List      =split(' ',$_);
	$Name=$List[0];
	$ELF=$Name.".elf";
	$ELF=(File::Spec->rel2abs($ELF));
	$TestHash{$Test}{"name"} = $Name;
	$TestHash{$Test}{"file"} = $ELF;
	$TestHash{$Test}{"row"}  = $List[1];
	$TestHash{$Test}{"col"}  = $List[2];
	$TestHash{$Test}{"rows"} = $List[3];
	$TestHash{$Test}{"cols"} = $List[4];
	$Test=$Test+1;
    }
}
#####################
#Building All Tets
if(!defined $opt_nb){
    foreach  $Test (sort(keys %TestHash)){    
	system("make TEST=$TestHash{$Test}{\"name\"} clean");
	system("make TEST=$TestHash{$Test}{\"name\"}");
    }
}

####################$
#Running All Tests
system("mkdir -p $opt_d");
chdir("$opt_d");
foreach  $Test (sort(keys %TestHash)){        
    system("run_test.pl -elf $TestHash{$Test}{file} -row $TestHash{$Test}{row} -col $TestHash{$Test}{col}   -rows $TestHash{$Test}{rows} -cols $TestHash{$Test}{cols} -v");   
}

$Pass=system("grep -q -P \"FAIL\" *.log");
if(!$Pass){
    print "***FAILING TEST SUMMARY***\n";
    system("grep FAIL *.log");
}

chdir("../");



      
      






