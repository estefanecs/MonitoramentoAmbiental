all: sbc

sbc: sbc.c
	gcc sbc.c ads1115_rpi.c sensores.c -o sbc -l mosquitto -l wiringPi -l wiringPiDev -l pthread

clean:
	rm sbc