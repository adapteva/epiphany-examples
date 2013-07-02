#INPUT ARGUMENTS

#Default is epiphany
LINUX = 
#Name 
TEST= hello_epiphany
#Test library
LIB=e_test
#Assembly file (usually note needed)
AS = 

######NO NEED TO CHANGE ANYTHING BELOW THIS################
ifdef LINUX
CC=gcc 
EXE:=$(TEST)_linux.elf
else
CC=e-gcc -T $(EPIPHANY_HOME)/bsps/$(EPIPHANY_PLATFORM)/fast.ldf 
#CC=e-gcc -Os -T fast.ldf 

EXE:=$(TEST).elf
endif 
CFLAGS=-Wall  -I$(LIB)/include/ 
#CFLAGS=-Wall -Os -I$(LIB)/include/ 

# -Wall
LDFLAGS=
SRC=$(wildcard $(LIB)/src/*.c  $(TEST)/src/*.c)
OBJ=$(SRC:.c=.o)

all: $(SRC) $(EXE)

$(EXE): $(OBJ) 
	$(CC) $(INC) $(LDFLAGS) $(AS) $(OBJ) -o $@

clean:
	rm -rf $(LIB)/src/*.o $(TEST)/src/*.o $(EXE)

