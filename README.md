apps:         Complete applications and utilities  
board:        Various board tests  
cpu:          Various "howto" examples  
dma:          DMA examples  
emesh:        Performance testing for the emesh  
errata:       Examples that demonstrates documented erratas  
labs:         Experimental feature testing  
softcache:    Software caching examples  
test:         Regression tests  

To run the TestEpiphany.pl test harness you must define an environment variable
name EPIPHANY_TESTS which defines the path to the epiphany-examples. The LIST
file should list the path to the tests run scripts relative to the value of
EPIPHANY_TESTS.

Jenkins: To configure Jenkins to run the Epiphany tests nightly change directories
to the jenkins directory and execute the setup.sh script. Note that Jenkins must
be installed on the target prior to running the setup script.

