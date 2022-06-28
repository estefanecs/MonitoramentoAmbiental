all: sbc

sbc: sbc.c
	gcc -Wall sbc.c ads1115_rpi.c sensores.c mqtt.c -o sbc -lmosquitto -l wiringPi -l wiringPiDev -l pthread

clean:
	rm sbc