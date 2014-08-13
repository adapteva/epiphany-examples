#!/usr/bin/perl
#$run="run";
#system("screen -S demo -X stuff '$run\n'");
#system("screen -S demo -X stuff 'ls -ltr\n'");
#system("screen -S demo -X stuff 'date\n'");
#system("screen -S demo -X stuff 'ls -ltr\n'");

###Get Hostname###
$Host=`hostname`;
chomp($Host);
###SCAN BOARD###
print "1.) Scan the board bar code:";
$a = <STDIN>;
if($a=~ /(.*).*SN(.*).*SKU(.*)/){
    $MACID=$1;
    $SN=$2;
    $SKU=$3;
    $DIR=substr($MACID,9,8);
    $DIR=~ s/\://g;
    if(-e "Logs/$DIR"){
	system("rm -r Logs/$DIR");
    }
}

###POWER ON BOARD###
print "2.) Boot the board WITHOUT the SD card inserted, press enter when done: <enter>";
$a = <STDIN>;

###FLASH THE BOARD###
system("screen -s para -X stuff 'printenv\n'");
system("screen -s para -X stuff 'setenv ethaddr $MACID\n'");
system("screen -s para -X stuff 'setenv AdaptevaSKU $SKU\n'");
system("screen -s para -X stuff 'saveenv\n'");
print "3.) Flashing the board, wait for \"done\" & prompt in 2nd window:<enter>";
$a = <STDIN>;

###REBOOT###
print "4.) Reboot the board WITH the SD card inserted and press enter:<enter>";
$a = <STDIN>;
print "5.) Press enter when you see \"root\@linaro-nano:~#\" prompt in 2nd window:<enter>";
$a = <STDIN>;

###RUN TEST###
system("screen -S para -X stuff 'su linaro\n cd\n run\n'");
system("screen -S para -X stuff 'rcp -r $DIR $Host:Logs\n'");

###CHECK FOR PASS###
print "5.) Waiting for test to complete...\n";

###DISPLAY PASS/FAIL###
while(1){  
    if(-e "Logs/$DIR/PASSED"){
	system("feh epiphany-examples/scripts/pass.png");
	exit;
    }
    elsif(-e "Logs/$DIR/FAILED"){
	system("feh epiphany-examples/scripts/fail.png");
	exit;
    }
}
    





