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
#############################################################################
use Getopt::Long;
use File::Spec;
use File::Basename;
my $Usage =<<EOF;
##########################################################################
# USAGE     : run_test.pl -elf   <ELF>
#                         -row   <CoreRow>
#                         -col   <CoreCol>
#                         -rows  <RowsToRun>
#                         -cols  <ColsToRun>
#                         -v     <Verbose>
#                         [-h]   <DetailedHelp>
# 
##########################################################################
EOF

#Defining options to parse
my $result =  GetOptions('row:s','col:s','rows:s','cols:s','elf:s','b:s','d:s','h:s','v:s');

if(defined $opt_h){
print "$Usage";
print "#
# Elf      :Elf file to run
# Row      :Row number of first core to run elf on  
# Col      :Column number of first core to run elf on
# Rows     :Total number of rows to run elf on
# Cols     :Total number of columns to run elf on
#########################################################################
#
#EXAMPLE: run_example.pl -elf hello.elf -row 0 -col 0 -rows 1 -cols 1
#EXAMPLE: run_example.pl -elf hello.elf -row 0 -col 0 -rows 4 -cols 4
##########################################################################
";
  exit;
}
elsif(!defined $opt_elf || !defined $opt_row || !defined $opt_col || !defined $opt_rows || !defined $opt_cols ){
    print "$Usage";		
    exit;
}	

$GDB_CMD_FILE="gdb_tmp.cmd";

#################
#Parse test file
#################
$Elf=(File::Spec->rel2abs($opt_elf));
$Test=basename($Elf);
$Test=~ s/\.elf//;
$Row=$opt_row;
$Col=$opt_col;
$Rows=$opt_rows;
$Cols=$opt_cols;
$PortStart =51000;
$N=4;

#Remove Old Logfile!!
system("rm -f $Test.$Test.*.*.Log");
for $i ($Row..($Row+$Rows-1)){
    for $j ($Col..($Col+$Cols-1)){
	$Port=$PortStart+$j+$i*$N;
	print "***Running test \"$Test\" on core($i,$j)***\n";
	open(FILECMD,">$GDB_CMD_FILE");
	print FILECMD "target remote:$Port\n";
	print FILECMD "load\n";
	print FILECMD "c\n";
	print FILECMD "x 0x0\n";#check for pass
	print FILECMD "detach\n";
	print FILECMD "quit\n";	
	close(FILECMD);
	system("e-gdb $Elf -x gdb_tmp.cmd >& $Test.$i.$j.log");
	$Fail=system("grep -q -P \"0x0 <start>:\t0x12345678\" $Test.$i.$j.log");
	if(!$Fail){
	    print "PASSED\n";
	}
	else{
	    print "FAILED\n";
	}
    }
}



