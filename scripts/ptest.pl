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
    system("rm -r Logs/$DIR");
}

###POWER ON BOARD###
print "2.) Connect cables and power on board, press enter when done: <enter>\n";
$a = <STDIN>;

###FLASH THE BOARD###
system("screen -s para -X stuff 'printenv\n'");
system("screen -s para -X stuff 'setenv ethaddr $MACID\n'");
system("screen -s para -X stuff 'setenv AdaptevaSKU $SKU\n'");
system("screen -s para -X stuff 'saveenv\n'");
print "3.) The board is being programmed, wait for \"done\" and the \"zynq-uboot\" prompt in monitor window:<enter>\n";
$a = <STDIN>;

###REBOOT###
print "4.) Turn off power-->insert SD card,-->reapply power, press enter when you see \"root@linaro-nano:~#\" prompt in monitor window:<enter>";
$a = <STDIN>;

###RUN TEST###
system("screen -S para -X stuff 'su linaro\n cd\n run\n'");
system("screen -S para -X stuff 'rcp -r $DIR $Host:Logs\n'");

###CHECK FOR PASS###
print "7.) Wait until test completes in monitor window:<enter>";

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
    





