# makfile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include/ 

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c

toy.elf: ${COMMON_OBJECTS} toyMain.o led.o switches.o buzzer.o ../lib/libTimer.a
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ 

led.o: led.h led.c

switches.o: switches.h switches.c

buzzer.o: buzzer.h buzzer.c

all:
	(cd ../timerLib; make install)
	toy.elf

load: toy.elf
	mspdebug rf2500 "prog $^"

# doc:
# 	rm -rf doxygen_docs
# 	doxygen Doxyfile

clean:
	(cd ../timerLib; make clean)
	rm -rf ../lib ../h lib h
	rm -rf doxygen_docs/*
	rm -rf doxygen_docs/*rm -f *.o *.elf
