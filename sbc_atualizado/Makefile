all: sbc

sbc: sbc.c
	gcc sbc.c ads1115_rpi.c sensores.c -o sbc -lmosquitto -l wiringPi -l wiringPiDev -l pthread

clean:
	rm sbc
