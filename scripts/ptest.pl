#!/usr/bin/perl
#$run="run";
#system("screen -S demo -X stuff '$run\n'");
#system("screen -S demo -X stuff 'ls -ltr\n'");
#system("screen -S demo -X stuff 'date\n'");
#system("screen -S demo -X stuff 'ls -ltr\n'");

###SCAN BOARD###
print "SCAN BOARD INFO:";
$a = <STDIN>;
($MACID,$SN,$SKU) = (split /,/, $a)[0,1,2];
$SN=~ s/^SN//;
$SKU=~ s/^SKU//;

###FLASH THE BOARD###
if(0){
    system("screen -s para -X stuff 'printenv\n'");
    system("screen -s para -X stuff 'setenv ethaddr $MACID\n'");
    system("screen -s para -X stuff 'setenv AdaptevaSKU $SKU\n'");
    system("screen -s para -X stuff 'saveenv\n'");
}
###POWER OFF THE BOARD###
print "1.) TURN OFF THE BOARD\n");
print "2.) PUT IN SD-CARD\n");
print "2.) TURN ON BOARD\n");
print "3.) PRESS ENTER WHEN DONE BOOTING (SEE MONITOR WINDOW):");
$a = <STDIN>;

###CHECK TEST###
my $name = substr($MACID,9,8);
$name =~ s/://g;
print "DID THE TEST PASS (y/n)?";









