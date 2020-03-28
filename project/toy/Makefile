# makfile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include/ 

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c


all:
	(cd ../timerLib; make install)

toy.elf:
	${COMMON_OBJECTS} toy.o switches.o led.o buzzer.o ../lib/lTimer.a
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ 

led.o: led.h led.c

switches.o: switches.h switches.c

buzzer.o: buzzer.h buzzer.c

all:
	toy.elf

load: toy.elf
	mspdebug rf2500 "prog $^"

# doc:
# 	rm -rf doxygen_docs
# 	doxygen Doxyfile

clean:
	(cd ../timerLib; make clean)
	rm -rf lib h
	rm -rf doxygen_docs/*
	rm -rf doxygen_docs/*rm -f *.o *.elf
