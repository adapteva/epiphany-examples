#!/usr/bin/perl
#$run="run";
#system("screen -S demo -X stuff '$run\n'");
#system("screen -S demo -X stuff 'ls -ltr\n'");
#system("screen -S demo -X stuff 'date\n'");
#system("screen -S demo -X stuff 'ls -ltr\n'");

###Get Hostname/IP address###
$Host=`hostname`;
chomp($Host);
#$IP   = Net::Address::IP::Local->public;
$IP=`ip addr show eth0 | grep "inet \"`;
$IP=~ s/(.*?)inet\s+(192.168.3.\d+)\/(.*)/$2/;
chomp($IP);
###SCAN BOARD###
print "1.) Scan the board bar code:";
$a = <STDIN>;
if($a=~ /(.*).*SN(.*).*SKU(.*)/){
    $MACID=lc($1);
    $SN=$2;
    $SKU=$3;
    $DIR=substr($MACID,9,8);
    $DIR=~ s/\://g;
    if(-e "Logs/$DIR"){
	system("rm -r Logs/$DIR");
    }
}
#print "$MACID $DIR $SN $SKU\n";
#exit

###POWER ON BOARD###
print "2.) Boot the board WITHOUT the SD card inserted.\n";
system("sleep 5");
###FLASH THE BOARD###
system("screen -s para -X stuff 'printenv\n'");
system("screen -s para -X stuff 'setenv ethaddr $MACID\n'");
system("screen -s para -X stuff 'setenv AdaptevaSKU $SKU\n'");
system("screen -s para -X stuff 'saveenv\n'");
print "3.) Flashing the board, wait for \"done\" & prompt in 2nd window.\n";
print "4.) Reboot the board WITH the SD card inserted.\n";
###REBOOT###
print "5.) Press enter when you see \"root\@linaro-nano:~#\" prompt in 2nd window:<enter>";
$a = <STDIN>;

###RUN TEST###
system("screen -S para -X stuff 'su linaro\n cd\n run\n'");

###CHECK FOR PASS###
print "5.) Did the test complete?(y/n):";
$a = <STDIN>;
chomp($a);
if($a eq "y"){   
system("screen -S para -X stuff 'rcp -r $DIR $IP:Logs\n'");
###DISPLAY PASS/FAIL###
    while(1){  
	if(-e "Logs/$DIR/PASSED"){
	    system("feh epiphany-examples/scripts/pass.png");
	    exit;
	}
	elsif(-e "Logs/$DIR/FAILED"){
	system("feh epiphany-examples/scripts/fail.png");
	print "What test failed?";
	$failure = <STDIN>;
	chomp($failure);
	exit;
	}
    }
}
else{
    system("screen -S para -X stuff 'rcp -r $DIR $IP:Logs\n'");
    system("touch Logs/$DIR/FAILED\n");
    system("feh epiphany-examples/scripts/fail.png");
    print "What test failed?";
    $failure = <STDIN>;
    chomp($failure);
    exit;
}
###COMPILING FILE###
#system("echo $SN,$MACID $FA    




